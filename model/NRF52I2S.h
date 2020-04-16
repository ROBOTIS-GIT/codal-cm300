#ifndef CODAL_NRF52_I2S_H
#define CODAL_NRF52_I2S_H

#include "Pin.h"
#include "nrf.h"
#include "CodalComponent.h"
#include "CodalConfig.h"
#include "hal/nrf_i2s.h"

typedef void (*DoublePVoidCallback)(void* fixed_arg, void* user_arg);

namespace codal
{
class NRF52I2S : public CodalComponent
{
    Pin &sck;
    Pin &ws;
    uint8_t mode;
    uint32_t sampleRate;

    uint16_t buffer_size;

    volatile bool buffers_needed;
    volatile bool buffers_reused;
    volatile bool use_tx;
    volatile bool use_rx;
    volatile bool tx_ready;
    volatile bool rx_ready;

    volatile bool is_first_buf;

    NRF_I2S_Type *p_i2s_;

    void* done_handler_arg;
    DoublePVoidCallback doneHandler;
    static void _irqHandler(void *self);

public:
    /**
     * Initialize I2S instance with given pins.
     */
    NRF52I2S(Pin &sck, Pin &ws, Pin *mck = nullptr, Pin *sdout = nullptr, Pin *sdin = nullptr);

    ~NRF52I2S();

    /** Set the frequency of the I2S interface
     *
     * @param frequency The bus frequency in hertz
     * @param bits Number of bits per sample (8, 16, 24, 32)
     */
    virtual int setSampleRateAndBits(uint32_t frequency, int bits = 16);
    uint32_t getSampleRate(); //For DEBUG

    /** Set the mode of the I2S interface
     *
     * @param mode I2S protocol mode (0 - 3)
     *
     * @code
     *  num | Mode
     * -----+--------
     *   0  | Master
     *   1  | Slave
     * @endcode
     */
    virtual int setMode(int mode);
    virtual int setFormat(int format);
    virtual int setChannel(int channel);

    virtual int setNextBuffers(const uint32_t *txBuffer, uint32_t *rxBuffer);
    virtual int start(const uint32_t *txBuffer, uint32_t *rxBuffer,
        uint16_t rxTxSize, DoublePVoidCallback done_handler, void* arg);
    virtual int stop();

    bool isFirstBufferFromStart();
};
} // namespace codal

#endif
