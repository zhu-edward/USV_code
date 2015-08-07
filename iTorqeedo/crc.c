/**
 * \file crc.c
 * Functions and types for CRC checks.
 *
 * Generated on Fri Jun 14 14:09:03 2013,
 * by pycrc v0.8.1, http://www.tty1.net/pycrc/
 * using the configuration:
 *    Width        = 8
 *    Poly         = 0x31
 *    XorIn        = 0x00
 *    ReflectIn    = True
 *    XorOut       = 0x00
 *    ReflectOut   = True
 *    Algorithm    = bit-by-bit
 *****************************************************************************/
#include "crc.h"     /* include the header file generated with pycrc */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

static crc_t crc_reflect(crc_t data, size_t data_len);

/**
 * Reflect all bits of a \a data word of \a data_len bytes.
 *
 * \param data         The data word to be reflected.
 * \param data_len     The width of \a data expressed in number of bits.
 * \return             The reflected data.
 *****************************************************************************/
crc_t crc_reflect(crc_t data, size_t data_len)
{
    unsigned int i;
    crc_t ret;

    ret = data & 0x01;
    for (i = 1; i < data_len; i++) {
        data >>= 1;
        ret = (ret << 1) | (data & 0x01);
    }
    return ret;
}


/**
 * Update the crc value with new data.
 *
 * \param crc      The current crc value.
 * \param data     Pointer to a buffer of \a data_len bytes.
 * \param data_len Number of bytes in the \a data buffer.
 * \return         The updated crc value.
 *****************************************************************************/
crc_t crc_update(crc_t crc, const unsigned char *data, size_t data_len)
{
    unsigned int i;
    bool bit;
    unsigned char c;

    while (data_len--) {
        c = crc_reflect(*data++, 8);
        for (i = 0; i < 8; i++) {
            bit = crc & 0x80;
            crc = (crc << 1) | ((c >> (7 - i)) & 0x01);
            if (bit) {
                crc ^= 0x31;
            }
        }
        crc &= 0xff;
    }
    return crc & 0xff;
}

/**
 * Calculate the final crc value.
 *
 * \param crc  The current crc value.
 * \return     The final crc value.
 *****************************************************************************/
crc_t crc_finalize(crc_t crc)
{
    unsigned int i;
    bool bit;

    for (i = 0; i < 8; i++) {
        bit = crc & 0x80;
        crc = (crc << 1) | 0x00;
        if (bit) {
            crc ^= 0x31;
        }
    }
    crc = crc_reflect(crc, 8);
    return (crc ^ 0x00) & 0xff;
}

