
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
    angle=angle-floorf(angle * INVTWOPI) * TWOPI;
    angle=angle>0.f?angle:-angle;

    if(angle<halfpi) return cos_32s(angle);
    if(angle<pi) return -cos_32s(pi-angle);
    if(angle<threehalfpi) return -cos_32s(angle-pi);
    return cos_32s(TWOPI - angle);
}

PAC5XXX_RAMFUNC float fast_sin(float angle)
{
    return fast_cos(halfpi-angle);
}

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

/* wrap x -> [0,max) */
PAC5XXX_RAMFUNC float wrapf_max(float x, float max)
{
    return fmodf(max + fmodf(x, max), max);
}
/* wrap x -> [min,max) */
PAC5XXX_RAMFUNC float wrapf_min_max(float x, float min, float max)
{
    return min + wrapf_max(x - min, max - min);
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
