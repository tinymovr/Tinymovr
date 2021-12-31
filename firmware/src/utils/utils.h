
//  * This file is part of the Tinymovr-Firmware distribution
//  * (https://github.com/yconst/tinymovr-firmware).
//  * 
//  *  *except* for the function "SVM()", which is Copyright (c) 2016-2018 Oskar Weigl
//  * 
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


#include "src/common.h"

#ifndef UTILS_UTILS_H_
#define UTILS_UTILS_H_

PAC5XXX_RAMFUNC float fast_sqrt(float n);
PAC5XXX_RAMFUNC float fast_inv_sqrt(float n);
PAC5XXX_RAMFUNC float fast_cos(float angle);
PAC5XXX_RAMFUNC float fast_sin(float angle);
PAC5XXX_RAMFUNC float fminf(float x, float y);
PAC5XXX_RAMFUNC float fabsf(float x);
PAC5XXX_RAMFUNC float floorf(float x);
PAC5XXX_RAMFUNC float fmodf(float a, float b);
PAC5XXX_RAMFUNC bool our_clamp(float *d, float min, float max);
PAC5XXX_RAMFUNC char checksum(char* msg, uint8_t len);
PAC5XXX_RAMFUNC float wrapf_max(float x, float max);
PAC5XXX_RAMFUNC float wrapf_min_max(float x, float min, float max);

static inline void delay_us(uint32_t us)
{
    pac_delay_asm(us * 16u);
}

// https://github.com/madcowswe/ODrive/blob/3113aedf081cf40e942d25d3b0b36c8806f11f23/Firmware/MotorControl/utils.c
// Released under teh following license:
// The MIT License (MIT)

// Copyright (c) 2016-2018 Oskar Weigl

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// Needs to be defined in header for inlining
PAC5XXX_RAMFUNC static inline int SVM(float alpha, float beta, float* tA, float* tB, float* tC)
{
    int Sextant;

    if (beta >= 0.0f)
    {
        if (alpha >= 0.0f)
        {
            //quadrant I
            if (one_by_sqrt3 * beta > alpha)
                Sextant = 2; //sextant v2-v3
            else
                Sextant = 1; //sextant v1-v2

        }
        else
        {
            //quadrant II
            if (-one_by_sqrt3 * beta > alpha)
                Sextant = 3; //sextant v3-v4
            else
                Sextant = 2; //sextant v2-v3
        }
    }
    else
    {
        if (alpha >= 0.0f)
        {
            //quadrant IV
            if (-one_by_sqrt3 * beta > alpha)
                Sextant = 5; //sextant v5-v6
            else
                Sextant = 6; //sextant v6-v1
        }
        else
        {
            //quadrant III
            if (one_by_sqrt3 * beta > alpha)
                Sextant = 4; //sextant v4-v5
            else
                Sextant = 5; //sextant v5-v6
        }
    }

    switch (Sextant)
    {
        // sextant v1-v2
        case 1:
        {
            // Vector on-times
            float t1 = alpha - one_by_sqrt3 * beta;
            float t2 = two_by_sqrt3 * beta;

            // PWM timings
            *tA = (1.0f - t1 - t2) * 0.5f;
            *tB = *tA + t1;
            *tC = *tB + t2;
        } break;

        // sextant v2-v3
        case 2:
        {
            // Vector on-times
            float t2 = alpha + one_by_sqrt3 * beta;
            float t3 = -alpha + one_by_sqrt3 * beta;

            // PWM timings
            *tB = (1.0f - t2 - t3) * 0.5f;
            *tA = *tB + t3;
            *tC = *tA + t2;
        } break;

        // sextant v3-v4
        case 3:
        {
            // Vector on-times
            float t3 = two_by_sqrt3 * beta;
            float t4 = -alpha - one_by_sqrt3 * beta;

            // PWM timings
            *tB = (1.0f - t3 - t4) * 0.5f;
            *tC = *tB + t3;
            *tA = *tC + t4;
        } break;

        // sextant v4-v5
        case 4:
        {
            // Vector on-times
            float t4 = -alpha + one_by_sqrt3 * beta;
            float t5 = -two_by_sqrt3 * beta;

            // PWM timings
            *tC = (1.0f - t4 - t5) * 0.5f;
            *tB = *tC + t5;
            *tA = *tB + t4;
        } break;

        // sextant v5-v6
        case 5:
        {
            // Vector on-times
            float t5 = -alpha - one_by_sqrt3 * beta;
            float t6 = alpha - one_by_sqrt3 * beta;

            // PWM timings
            *tC = (1.0f - t5 - t6) * 0.5f;
            *tA = *tC + t5;
            *tB = *tA + t6;
        } break;

        // sextant v6-v1
        case 6:
        {
            // Vector on-times
            float t6 = -two_by_sqrt3 * beta;
            float t1 = alpha + one_by_sqrt3 * beta;

            // PWM timings
            *tA = (1.0f - t6 - t1) * 0.5f;
            *tC = *tA + t1;
            *tB = *tC + t6;
        } break;
    }

    // if any of the results becomes NaN, result_valid will evaluate to false
    int result_valid =
            *tA >= 0.0f && *tA <= 1.0f
         && *tB >= 0.0f && *tB <= 1.0f
         && *tC >= 0.0f && *tC <= 1.0f;
    return result_valid ? 0 : -1;
}

#endif /* UTILS_UTILS_H_ */
