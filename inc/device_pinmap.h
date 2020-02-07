
#ifndef CODAL_PINNAMES_H
#define CODAL_PINNAMES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    p0  = 0,
    p1  = 1,
    p2  = 2,
    p3  = 3,
    p4  = 4,
    p5  = 5,
    p6  = 6,
    p7  = 7,
    p8  = 8,
    p9  = 9,
    p10 = 10,
    p11 = 11,
    p12 = 12,
    p13 = 13,
    p14 = 14,
    p15 = 15,
    p16 = 16,
    p17 = 17,
    p18 = 18,
    p19 = 19,
    p20 = 20,
    p21 = 21,
    p22 = 22,
    p23 = 23,
    p24 = 24,
    p25 = 25,
    p26 = 26,
    p27 = 27,
    p28 = 28,
    p29 = 29,
    p30 = 30,
    p31 = 31,
    p32 = 32,
    p33 = 33,
    p34 = 34,
    p35 = 35,
    p36 = 36,
    p37 = 37,
    p38 = 38,
    p39 = 39,
    p40 = 40,
    p41 = 41,
    p42 = 42,
    p43 = 43,
    p44 = 44,
    p45 = 45,
    p46 = 46,
    p47 = 47,

    P0_0  = p0,
    P0_1  = p1,
    P0_2  = p2,
    P0_3  = p3,
    P0_4  = p4,
    P0_5  = p5,
    P0_6  = p6,
    P0_7  = p7,

    P0_8  = p8,
    P0_9  = p9,
    P0_10 = p10,
    P0_11 = p11,
    P0_12 = p12,
    P0_13 = p13,
    P0_14 = p14,
    P0_15 = p15,

    P0_16 = p16,
    P0_17 = p17,
    P0_18 = p18,
    P0_19 = p19,
    P0_20 = p20,
    P0_21 = p21,
    P0_22 = p22,
    P0_23 = p23,

    P0_24 = p24,
    P0_25 = p25,
    P0_26 = p26,
    P0_27 = p27,
    P0_28 = p28,
    P0_29 = p29,
    P0_30 = p30,
    P0_31 = p31,

    P1_0  = p32,
    P1_1  = p33,
    P1_2  = p34,
    P1_3  = p35,
    P1_4  = p36,
    P1_5  = p37,
    P1_6  = p38,
    P1_7  = p39,

    P1_8  = p40,
    P1_9  = p41,
    P1_10 = p42,
    P1_11 = p43,
    P1_12 = p44,
    P1_13 = p45,
    P1_14 = p46,
    P1_15 = p47,


    // // mBed interface Pins
    // USBTX = TX_PIN_NUMBER,
    // USBRX = RX_PIN_NUMBER,

    // SPI_PSELMOSI0 = p6,
    // SPI_PSELMISO0 = p7,
    // SPI_PSELSS0   = p3,
    // SPI_PSELSCK0  = p8,

    // SPI_PSELMOSI1 = p29,
    // SPI_PSELMISO1 = p30,
    // SPI_PSELSS1   = p28,
    // SPI_PSELSCK1  = p2,

    // SPIS_PSELMOSI = p29,
    // SPIS_PSELMISO = p30,
    // SPIS_PSELSS   = p28,
    // SPIS_PSELSCK  = p2,

    // I2C_SDA0 = p28,
    // I2C_SCL0 = p2,

    // D0 = p30,
    // D1 = p29,
    // D2 = p28,
    // D3 = p2,

    // D4 = p3,
    // D5 = p6,
    // D6 = p7,
    // D7 = p8,
    // D8 = p21,

    // D9 = p4,
    // D10 = p5,

    // D13 = p11,

    // A0 = p28,
    // A1 = p29,
    // A2 = p30,
    // A3 = p2,
    // A4 = p4,
    // A5 = p5,

    // Not connected
    NC = (int)0xFFFFFFFF
} PinName;

#ifdef __cplusplus
}
#endif

#endif