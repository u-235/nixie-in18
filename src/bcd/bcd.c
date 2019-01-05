/**
 * @file
 * @brief
 * @details
 *
 * @date создан 17.10.2015
 * @author Nick Egorrov
 */

#include "bcd.h"

static bcd2_t adjust_add(bcd2_t a);

static bcd2_t adjust_sub(bcd2_t s);

extern bcd2_t bcd_from_uint8(uint8_t tc)
{
        return (tc % 10) + ((tc / 10) << 4);
}

extern uint8_t bcd_to_uint8(bcd2_t tc)
{
        return (tc & 0x0f) + ((tc >> 4) * 10);
}

extern bcd2_t bcd2_inc(bcd2_t a)
{
        return adjust_add(++a);
}

extern bcd2_t bcd2_dec(bcd2_t a)
{
        return adjust_sub(--a);
}

extern bcd2_t bcd_add2(bcd2_t a, bcd2_t b)
{
        return adjust_add(a + b);
}

extern bcd2_t bcd_sub2(bcd2_t a, bcd2_t b)
{
        return adjust_sub(a - b);
}

static bcd2_t adjust_add(bcd2_t a){
        if ((a & 0x0f) > 9) {
                a += 6;
        }

        if (a >= 0xa0) {
                a += 0x60;
        }

        return a;
}

static bcd2_t adjust_sub(bcd2_t s)
{
        if ((s & 0x0f) > 9) {
                s -= 6;
        }

        if (s >= 0xa0) {
                s -= 0x60;
        }

        return s;
}
