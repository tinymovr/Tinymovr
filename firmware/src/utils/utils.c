
//  * This file is part of the Tinymovr-Firmware distribution
//  * (https://github.com/yconst/tinymovr-firmware).
//  * Copyright (c) 2020 Ioannis Chatzikonstantinou.
//  * 
//  * This program is free software: you can redistribute it and/or modify  
//  * it under the terms of the GNU General Public License as published by  
//  * the Free Software Foundation, version 3.
//  *
//  * This program is distributed in the hope that it will be useful, but 
//  * WITHOUT ANY WARRANTY; without even the implied warranty of 
//  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
//  * General Public License for more details.
//  *
//  * You should have received a copy of the GNU General Public License 
//  * along with this program. If not, see <http://www.gnu.org/licenses/>.

#include "src/utils/utils.h"

PAC5XXX_RAMFUNC float unwrapf(float reference_val, float wrapped_val, float half_interval)
{
    float full_interval = half_interval * 2.0f;
	float delta = fmodf((wrapped_val - reference_val) + half_interval, 
		full_interval) - half_interval;
    if (delta < 0.0f)
    {
        delta += full_interval;
    }
    delta -= half_interval;
    return reference_val + delta;
}

PAC5XXX_RAMFUNC float wrapf(float unbound_val, float half_interval)
{
    float full_interval = half_interval * 2.0f;
    float bound_val = unbound_val;
    while (bound_val > half_interval)
    {
        bound_val -= full_interval;
    }
    while (bound_val < -half_interval)
    {
        bound_val += full_interval;
    }
    return bound_val;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
PAC5XXX_RAMFUNC float fast_inv_sqrt(float n)
{
	long i;
	float x, y;

	x = n * 0.5f;
	y = n;
	i = *(long *)&y;
	i = 0x5f3759df - (i >> 1);
	y = *(float *)&i;
	y = y * (1.5f - (x * y * y));

	return y;
}
#pragma GCC diagnostic pop

// based on https://github.com/divideconcept/FastTrigo/blob/master/fasttrigo.cpp
PAC5XXX_RAMFUNC float cos_32s(float x)
{
    const float c1= 0.99940307f;
    const float c2=-0.49558072f;
    const float c3= 0.03679168f;
    float x2;      // The input argument squared
    x2=x * x;
    return (c1 + x2*(c2 + c3 * x2));
}

PAC5XXX_RAMFUNC float fast_cos(float angle)
{
    //clamp to the range 0..2pi
    angle=angle-floorf(angle*invtwopi)*twopi;
    angle=angle>0.f?angle:-angle;

    if(angle<halfpi) return cos_32s(angle);
    if(angle<pi) return -cos_32s(pi-angle);
    if(angle<threehalfpi) return -cos_32s(angle-pi);
    return cos_32s(twopi-angle);
}

PAC5XXX_RAMFUNC float fast_sin(float angle)
{
    return fast_cos(halfpi-angle);
}

PAC5XXX_RAMFUNC float fminf(float x, float y)
{
    if (isnan(x))
        return y;
    if (isnan(y))
        return x;
    /* handle signed zeros, see C99 Annex F.9.9.2 */
    if (signbit(x) != signbit(y))
        return signbit(x) ? x : y;
    return x < y ? x : y;
}

#if __ARM_FEATURE_FMA && __ARM_FP&4 && !__SOFTFP__ && !BROKEN_VFP_ASM

PAC5XXX_RAMFUNC float fabsf(float x)
{
    __asm__ ("vabs.f32 %0, %1" : "=t"(x) : "t"(x));
    return x;
}

#else

PAC5XXX_RAMFUNC float fabsf(float x)
{
	union {float f; uint32_t i;} u = {x};
	u.i &= 0x7fffffff;
	return u.f;
}

#endif

PAC5XXX_RAMFUNC float floorf(float x)
{
	if (x >= 0.0f)
	{
		return (float)((int)x);
	}
	return (float)((int)x - 1);
}

PAC5XXX_RAMFUNC float fmodf(float a, float b)
{
    return (a - b * floorf(a / b));
}

PAC5XXX_RAMFUNC bool clamp(float *d, float min, float max)
{
    bool clamped = false;
    if (*d < min)
    {
        *d = min;
        clamped = true;
    }
    else if (*d > max)
    {
        *d = max;
        clamped = true;
    }
    return clamped;
}

PAC5XXX_RAMFUNC int ltoa(int32_t value, uint8_t *sp, int radix)
{
    char tmp[64];// be careful with the length of the buffer
    char *tp = tmp;
    uint32_t v;

    const int sign = (radix == 10 && value < 0);
    if (sign)
        v = -value;
    else
        v = (uint32_t)value;

    while (v || tp == tmp)
    {
        const int i = v % radix;
        v /= radix; // v/=radix uses less CPU clocks than v=v/radix does
        if (i < 10)
          *tp++ = i+'0';
        else
          *tp++ = i + 'a' - 10;
    }

    int len = tp - tmp;

    if (sign)
    {
        *sp++ = '-';
        len++;
    }

    while (tp > tmp)
        *sp++ = *--tp;

    return len;
}

PAC5XXX_RAMFUNC char checksum(char* msg, uint8_t len)
{
    uint8_t checksum = msg[0];
    for (uint8_t i=1; i<len; i++)
    {
        checksum ^= msg[i];
    }
    return checksum;
}
