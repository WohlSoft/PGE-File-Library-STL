/*
 * PGE File Library - a library to process file formats, part of Moondust project
 *
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once
#ifndef PGE_FF_UNITS_H
#define PGE_FF_UNITS_H

namespace PGE_FileLibrary
{

enum class TimeUnit
{
    FrameOneOf65sec = -1,
    Millisecond = 0,
    Decisecond,
    Second
};

template<typename NumT>
NumT TimeUnitsCVT(const NumT &in, TimeUnit from, TimeUnit into)
{
    if(from == into)
        return in;
    long double value = static_cast<long double>(in);
    switch(from)
    {
    case TimeUnit::FrameOneOf65sec:
        switch(into)
        {
        case TimeUnit::Millisecond:
            return static_cast<NumT>( (value * 1000.0l) / 65.0l );
        case TimeUnit::Decisecond:
            return static_cast<NumT>( (value * 10.0l) / 65.0l );
        case TimeUnit::Second:
            return static_cast<NumT>( value / 65.0l );
        default:;
        case TimeUnit::FrameOneOf65sec:
            return in;//Nothing to do
        };
    case TimeUnit::Millisecond:
        switch(into)
        {
        case TimeUnit::FrameOneOf65sec:
            return static_cast<NumT>( (value / 1000.0l) * 65.0l );
        case TimeUnit::Decisecond:
            return static_cast<NumT>( value / 1000.0l );
        case TimeUnit::Second:
            return static_cast<NumT>( value / 10.0l );
        default:;
        case TimeUnit::Millisecond:
            return in;//Nothing to do
        };
    case TimeUnit::Decisecond:
        switch(into)
        {
        case TimeUnit::FrameOneOf65sec:
            return static_cast<NumT>( (value / 10.0l) * 65.0l );
        case TimeUnit::Millisecond:
            return static_cast<NumT>( value * 100.0l );
        case TimeUnit::Second:
            return static_cast<NumT>( value / 10.0l );
        default:;
        case TimeUnit::Decisecond:
            return in;//Nothing to do
        };
    case TimeUnit::Second:
        switch(into)
        {
        case TimeUnit::FrameOneOf65sec:
            return static_cast<NumT>( value * 65.0l );
        case TimeUnit::Millisecond:
            return static_cast<NumT>( value * 1000.0l );
        case TimeUnit::Decisecond:
            return static_cast<NumT>( value * 10.0l );
        default:;
        case TimeUnit::Second:
            return in;//Nothing to do
        };
    };
    return in;
}

}//PGE_FileLibrary
#endif // PGE_FF_UNITS_H
