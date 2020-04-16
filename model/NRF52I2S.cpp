#include "NRF52I2S.h"
#include "peripheral_alloc.h"
#include "NotifyEvents.h"
#include "codal_target_hal.h"
#include "hal/nrf_gpio.h"

#if !defined(USE_WORKAROUND_FOR_I2S_STOP_ANOMALY) &&                               \
    (defined(NRF52832_XXAA) || defined(NRF52832_XXAB) || defined(NRF52840_XXAA) || \
     defined(NRF9160_XXAA))
// Enable workaround for nRF52832 and nRF52840 anomaly 194 / nrf9160 anomaly 1
// (STOP task does not switch off all resources).
#define USE_WORKAROUND_FOR_I2S_STOP_ANOMALY 1
#endif

namespace codal
{
/**
 * Constructor.
 */
NRF52I2S::NRF52I2S(Pin &sck, Pin &ws, Pin *mck, Pin *sdout, Pin *sdin)
    : sck(sck), ws(ws)
{
    p_i2s_ = (NRF_I2S_Type*)allocate_peripheral(PERI_MODE_I2S);

    if(p_i2s_ == nullptr)
        target_panic(DEVICE_HARDWARE_CONFIGURATION_ERROR);

    // Alignment = Left
    p_i2s_->CONFIG.ALIGN = NRF_I2S_ALIGN_LEFT;

    // - MCK (optional) - always output
    if(mck) nrf_gpio_cfg_output(mck->name);
    // - SDOUT (optional) - always output
    if(sdout) nrf_gpio_cfg_output(sdout->name);
    // - SDIN (optional) - always input.
    if(sdin) nrf_gpio_cfg_input(sdin->name, NRF_GPIO_PIN_NOPULL);

    setMode(NRF_I2S_MODE_MASTER);
    setFormat(NRF_I2S_FORMAT_I2S);
    setChannel(NRF_I2S_CHANNELS_LEFT);
    nrf_i2s_pins_set(p_i2s_, sck.name, ws.name, 
        mck!=nullptr?mck->name:NRF_I2S_PIN_NOT_CONNECTED,
        sdout!=nullptr?sdout->name:NRF_I2S_PIN_NOT_CONNECTED, 
        sdin!=nullptr?sdin->name:NRF_I2S_PIN_NOT_CONNECTED);
    setSampleRateAndBits(44100, 16);

    set_alloc_peri_irq(p_i2s_, &_irqHandler, this);

    IRQn_Type IRQn = get_alloc_peri_irqn(p_i2s_);
    NVIC_SetPriority(IRQn, 0);
    NVIC_ClearPendingIRQ(IRQn);
    NVIC_EnableIRQ(IRQn);  

    nrf_i2s_enable(p_i2s_);
}


NRF52I2S::~NRF52I2S()
{
    stop();

    NVIC_DisableIRQ(get_alloc_peri_irqn(p_i2s_));
    nrf_i2s_pins_set(p_i2s_,
                     NRF_I2S_PIN_NOT_CONNECTED,
                     NRF_I2S_PIN_NOT_CONNECTED,
                     NRF_I2S_PIN_NOT_CONNECTED,
                     NRF_I2S_PIN_NOT_CONNECTED,
                     NRF_I2S_PIN_NOT_CONNECTED);
    free_alloc_peri(p_i2s_);
}


uint32_t NRF52I2S::getSampleRate()
{
    return sampleRate;
}


int NRF52I2S::setSampleRateAndBits(uint32_t frequency, int bits)
{
    uint32_t swidth;

    switch(bits){
        case 8: 
            swidth = NRF_I2S_SWIDTH_8BIT;
            break;
        case 16:
            swidth = NRF_I2S_SWIDTH_16BIT;
            break;
        case 24:
            swidth = NRF_I2S_SWIDTH_24BIT;
            break;
        default:
            return DEVICE_NOT_SUPPORTED;
    }

    if(mode == NRF_I2S_MODE_SLAVE){
        p_i2s_->CONFIG.SWIDTH = swidth;
        this->sampleRate = frequency;
        return DEVICE_OK;
    }

    if(frequency == 0){
        p_i2s_->CONFIG.MCKEN = I2S_CONFIG_MCKEN_MCKEN_Disabled;
        this->sampleRate = frequency;
        return DEVICE_OK;
    }

    uint8_t div_tbl[] = {125,63,42,32,31,30,23,21,16,15,11,10,8
#if defined(I2S_CONFIG_MCKFREQ_MCKFREQ_32MDIV6)
        ,6
#endif
#if defined(I2S_CONFIG_MCKFREQ_MCKFREQ_32MDIV5)
        ,5
#endif
#if defined(I2S_CONFIG_MCKFREQ_MCKFREQ_32MDIV4)
        ,4
#endif
#if defined(I2S_CONFIG_MCKFREQ_MCKFREQ_32MDIV3)
        ,3
#endif
#if defined(I2S_CONFIG_MCKFREQ_MCKFREQ_32MDIV2)
        ,2
#endif  
    };
    uint16_t ratio_tbl[] = {32,48,64,96,128,256,384,512};
    uint8_t div = 0;
    uint32_t ratio = 0;
    uint32_t mck_freq;
    uint32_t pre_diff = 0xFFFFFFFF, diff, cal_freq;

    for(uint8_t i=0; i<sizeof(div_tbl); i++){
        mck_freq = 32000000/div_tbl[i];
        for(uint8_t j=0; j<(sizeof(ratio_tbl)/sizeof(ratio_tbl[0])); j++){
            // The MCK/LRCK ratio shall be a multiple of 2 * CONFIG.SWIDTH
            if(ratio_tbl[j]%(2*bits) == 0){
                // LRCK(freq) = MCK/RATIO
                cal_freq = mck_freq/ratio_tbl[j];
                diff = frequency>=cal_freq ? (frequency-cal_freq):(cal_freq-frequency);
                if(diff < pre_diff){
                    pre_diff = diff;
                    div = div_tbl[i];
                    ratio = ratio_tbl[j];
                }
            }
        }
    }

    switch(div)
    {
        case 125: mck_freq = NRF_I2S_MCK_32MDIV125; break;
        case 63: mck_freq = NRF_I2S_MCK_32MDIV63; break;
        case 42: mck_freq = NRF_I2S_MCK_32MDIV42; break;
        case 32: mck_freq = NRF_I2S_MCK_32MDIV32; break;
        case 31: mck_freq = NRF_I2S_MCK_32MDIV31; break;
        case 30: mck_freq = NRF_I2S_MCK_32MDIV30; break;
        case 23: mck_freq = NRF_I2S_MCK_32MDIV23; break;
        case 21: mck_freq = NRF_I2S_MCK_32MDIV21; break;
        case 16: mck_freq = NRF_I2S_MCK_32MDIV16; break;
        case 15: mck_freq = NRF_I2S_MCK_32MDIV15; break;
        case 11: mck_freq = NRF_I2S_MCK_32MDIV11; break;
        case 10: mck_freq = NRF_I2S_MCK_32MDIV10; break;
        case 8: mck_freq = NRF_I2S_MCK_32MDIV8; break;
#if defined(I2S_CONFIG_MCKFREQ_MCKFREQ_32MDIV6)        
        case 6: mck_freq = NRF_I2S_MCK_32MDIV6; break;
#endif        
#if defined(I2S_CONFIG_MCKFREQ_MCKFREQ_32MDIV5)        
        case 5: mck_freq = NRF_I2S_MCK_32MDIV5; break;
#endif        
#if defined(I2S_CONFIG_MCKFREQ_MCKFREQ_32MDIV4)
        case 4: mck_freq = NRF_I2S_MCK_32MDIV4; break;
#endif
#if defined(I2S_CONFIG_MCKFREQ_MCKFREQ_32MDIV3)        
        case 3: mck_freq = NRF_I2S_MCK_32MDIV3; break;
#endif
#if defined(I2S_CONFIG_MCKFREQ_MCKFREQ_32MDIV2)
        case 2: mck_freq = NRF_I2S_MCK_32MDIV2; break;
#endif
        default: return DEVICE_NOT_SUPPORTED;
    }

    this->sampleRate = 32000000/div/ratio;

    switch(ratio)
    {
        case 32:
            ratio = NRF_I2S_RATIO_32X;
            break;
        case 48:
            ratio = NRF_I2S_RATIO_48X;
            break;
        case 64:
            ratio = NRF_I2S_RATIO_64X;
            break;
        case 96:
            ratio = NRF_I2S_RATIO_96X;
            break;
        case 128:
            ratio = NRF_I2S_RATIO_128X;
            break;
        case 256:
            ratio = NRF_I2S_RATIO_256X;
            break;
        case 384:
            ratio = NRF_I2S_RATIO_384X;
            break;
        case 512:
            ratio = NRF_I2S_RATIO_512X; 
            break;
    }

    p_i2s_->CONFIG.SWIDTH = swidth;
    p_i2s_->CONFIG.RATIO = ratio;
    p_i2s_->CONFIG.MCKFREQ = mck_freq;
    p_i2s_->CONFIG.MCKEN = I2S_CONFIG_MCKEN_MCKEN_Enabled;

    return DEVICE_OK;
}


int NRF52I2S::setMode(int mode)
{
    switch(mode)
    {
        case 0:
            stop(); 
            this->mode = NRF_I2S_MODE_MASTER;
            nrf_gpio_cfg_output(sck.name);
            nrf_gpio_cfg_output(ws.name);
            break;
        case 1: 
            stop();
            this->mode = NRF_I2S_MODE_SLAVE;
            p_i2s_->CONFIG.MCKEN = I2S_CONFIG_MCKEN_MCKEN_Disabled;
            nrf_gpio_cfg_input(sck.name, NRF_GPIO_PIN_NOPULL);
            nrf_gpio_cfg_input(ws.name, NRF_GPIO_PIN_NOPULL);
            break;
        default: 
            return DEVICE_NOT_SUPPORTED;
    }
    p_i2s_->CONFIG.MODE = this->mode;

    return DEVICE_OK;
}


int NRF52I2S::setFormat(int format)
{
    switch(format)
    {
        case 0: 
            p_i2s_->CONFIG.FORMAT = NRF_I2S_FORMAT_I2S; 
            break;
        case 1: 
            p_i2s_->CONFIG.FORMAT = NRF_I2S_FORMAT_ALIGNED; 
            break;
        default: 
            return DEVICE_NOT_SUPPORTED;
    }

    return DEVICE_OK;
}


int NRF52I2S::setChannel(int channel)
{
    switch(channel)
    {
        case 0: 
            p_i2s_->CONFIG.CHANNELS = NRF_I2S_CHANNELS_STEREO; 
            break;
        case 1: 
            p_i2s_->CONFIG.CHANNELS = NRF_I2S_CHANNELS_LEFT; 
            break;
        case 2: 
            p_i2s_->CONFIG.CHANNELS = NRF_I2S_CHANNELS_RIGHT; 
            break;            
        default: 
            return DEVICE_NOT_SUPPORTED;
    }

    return DEVICE_OK;
}


int NRF52I2S::setNextBuffers(const uint32_t *txBuffer, uint32_t *rxBuffer)
{
    if(!buffers_needed){
        return DEVICE_INVALID_STATE;
    }

    if ((txBuffer != nullptr && !nrfx_is_in_ram(txBuffer) && !nrfx_is_word_aligned(txBuffer))
        || (rxBuffer != nullptr && !nrfx_is_in_ram(rxBuffer) && !nrfx_is_word_aligned(rxBuffer))){
        return DEVICE_INVALID_PARAMETER;
    }

    if (use_tx && txBuffer){
        nrf_i2s_tx_buffer_set(p_i2s_, txBuffer);
    }
    if (use_rx && rxBuffer){
        nrf_i2s_rx_buffer_set(p_i2s_, rxBuffer);
    }

    buffers_needed = false;

    return DEVICE_OK;
}


int NRF52I2S::start(const uint32_t *txBuffer, uint32_t *rxBuffer,
    uint16_t rxTxSize, DoublePVoidCallback done_handler, void* arg)
{
    if ((txBuffer != nullptr && !nrfx_is_in_ram(txBuffer) && !nrfx_is_word_aligned(txBuffer))
        || (rxBuffer != nullptr && !nrfx_is_in_ram(rxBuffer) && !nrfx_is_word_aligned(rxBuffer))){
        return DEVICE_INVALID_PARAMETER;
    }

    use_rx         = (rxBuffer != nullptr);
    use_tx         = (txBuffer != nullptr);
    rx_ready       = false;
    tx_ready       = false;
    buffers_needed = false;
    buffer_size    = rxTxSize;
    doneHandler    = done_handler;
    done_handler_arg = arg;

    // Set the provided initial buffers as next, they will become the current
    // ones after the IRQ handler is called for the first time, what will occur
    // right after the START task is triggered.
    nrf_i2s_transfer_set(p_i2s_,
                         buffer_size,
                         rxBuffer,
                         txBuffer);
    is_first_buf = true;

    nrf_i2s_enable(p_i2s_);

    nrf_i2s_event_clear(p_i2s_, NRF_I2S_EVENT_RXPTRUPD);
    nrf_i2s_event_clear(p_i2s_, NRF_I2S_EVENT_TXPTRUPD);
    nrf_i2s_event_clear(p_i2s_, NRF_I2S_EVENT_STOPPED);    
    nrf_i2s_int_enable(p_i2s_, (use_rx ? NRF_I2S_INT_RXPTRUPD_MASK:0) |
                               (use_tx ? NRF_I2S_INT_TXPTRUPD_MASK:0) |
                               NRF_I2S_INT_STOPPED_MASK);
    nrf_i2s_task_trigger(p_i2s_, NRF_I2S_TASK_START);

    return DEVICE_OK;
}

int NRF52I2S::stop()
{
    // First disable interrupts, then trigger the STOP task, so no spurious
    // RXPTRUPD and TXPTRUPD events (see nRF52 anomaly 55) are processed.
    nrf_i2s_int_disable(p_i2s_, NRF_I2S_INT_TXPTRUPD_MASK |
                                NRF_I2S_INT_RXPTRUPD_MASK);
    nrf_i2s_task_trigger(p_i2s_, NRF_I2S_TASK_STOP);

#if NRFX_CHECK(USE_WORKAROUND_FOR_I2S_STOP_ANOMALY)
    *((volatile uint32_t *)(((uint32_t)p_i2s_) + 0x38)) = 1;
    *((volatile uint32_t *)(((uint32_t)p_i2s_) + 0x3C)) = 1;
#endif

    return DEVICE_OK;
}

bool NRF52I2S::isFirstBufferFromStart()
{
    return is_first_buf;
}

void NRF52I2S::_irqHandler(void *self_)
{
    NRF52I2S *self = (NRF52I2S *)self_;
    NRF_I2S_Type *p_i2s = self->p_i2s_;

    if (nrf_i2s_event_check(p_i2s, NRF_I2S_EVENT_TXPTRUPD)){
        nrf_i2s_event_clear(p_i2s, NRF_I2S_EVENT_TXPTRUPD);
        self->tx_ready = true;
        if (self->use_tx && self->buffers_needed){
            self->buffers_reused = true;
        }
    }

    if (nrf_i2s_event_check(p_i2s, NRF_I2S_EVENT_RXPTRUPD)){
        nrf_i2s_event_clear(p_i2s, NRF_I2S_EVENT_RXPTRUPD);
        self->rx_ready = true;
        if (self->use_rx && self->buffers_needed){
            self->buffers_reused = true;
        }
    }

    if (nrf_i2s_event_check(p_i2s, NRF_I2S_EVENT_STOPPED)){
        nrf_i2s_event_clear(p_i2s, NRF_I2S_EVENT_STOPPED);
        nrf_i2s_int_disable(p_i2s, NRF_I2S_INT_STOPPED_MASK);
        nrf_i2s_disable(p_i2s);
    }else{
        // Check if the requested transfer has been completed:
        // - full-duplex mode
        if ((self->use_tx && self->use_rx && self->tx_ready && self->rx_ready) ||
            // - TX only mode
            (!self->use_rx && self->tx_ready) ||
            // - RX only mode
            (!self->use_tx && self->rx_ready))
        {
            self->tx_ready = false;
            self->rx_ready = false;

            // If the application did not supply the buffers for the next
            // part of the transfer until this moment, the current buffers
            // cannot be released, since the I2S peripheral already started
            // using them. Signal this situation to the application by
            // passing nullptr instead of the structure with released buffers.
            if (self->buffers_reused){
                self->buffers_reused = false;
                // This will most likely be set at this point. However, there is
                // a small time window between TXPTRUPD and RXPTRUPD events,
                // and it is theoretically possible that next buffers will be
                // set in this window, so to be sure this flag is set to true,
                // set it explicitly.
                self->buffers_needed = true;
            }else{
                // Buffers that have been used by the I2S peripheral (current)
                // are now released and will be returned to the application,
                // and the ones scheduled to be used as next become the current
                // ones.
                self->buffers_needed = true;
            }
            if(self->doneHandler)
                self->doneHandler(self, self->done_handler_arg);
            self->is_first_buf = false;
        }
    }
}

} // namespace codal
