#include "bcd.h"

bcd2_t bcd_from_uint8(uint8_t tc)
{
        return (tc % 10) + ((tc / 10) << 4);
}

uint8_t bcd_to_uint8(bcd2_t tc)
{
        return (tc & 0x0f) + ((tc >> 4) * 10);
}

bcd2_t bcd_add2(bcd2_t a, bcd2_t b)
{
        bcd2_t ret = a + b;
        if ((ret & 0x0f) > 9) {
                ret += 6;
        }

        if (ret >= 0xa0) {
                ret += 0x60;
        }

        return ret;
}

bcd2_t bcd_sub2(bcd2_t a, bcd2_t b)
{
        bcd2_t ret = a - b;
        if ((ret & 0x0f) > 9) {
                ret -= 6;
        }

        if (ret >= 0xa0) {
                ret -= 0x60;
        }

        return ret;
}
