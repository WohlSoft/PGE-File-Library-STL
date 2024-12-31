/*
 * PGE File Library - a library to process file formats, part of Moondust project
 *
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
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

/*! \file mdx_value_numeric.cpp
 *
 *  \brief Contains code to save/load numeric values
 *
 */

#include <cstdlib>
#include <cfloat>
#include <limits>
#include <cmath>
#include <cstdarg>

#include "pge_file_lib_private.h"

#include "mdx/common/mdx_value.h"
#include "mdx/common/mdx_exception.h"
#include "mdx/common/value/milo_yip/itoa.h"

/******************************
 * private utility functions *
 ******************************/

template<class uint_t>
static const char* s_MDX_load_uint(uint_t& dest, const char* field_data)
{
    const char* const ret_error = field_data;

    uint_t value = 0;

    while(true)
    {
        char c = *field_data;

        if(c < '0' || c > '9')
        {
            dest = value;
            return field_data;
        }

        field_data++;

        uint_t digit = c - '0';

        if(value >= std::numeric_limits<uint_t>::max() / 10) PGE_ATTR_UNLIKELY
        {
            if(value > std::numeric_limits<uint_t>::max() / 10 || digit > std::numeric_limits<uint_t>::max() % 10) PGE_ATTR_LIKELY
                return ret_error;
        }

        value *= 10;
        value += digit;
    }
}

template<class int_t, int sign>
static const char* s_MDX_load_int(int_t& dest, const char* field_data)
{
    const char* const ret_error = field_data;

    if(sign == 1 && *field_data == '-')
        return s_MDX_load_int<int_t, -1>(dest, field_data);

    if(sign == -1)
    {
        field_data++;
        if(*field_data < '0' || *field_data > '9')
            return ret_error;
    }

    int_t value = 0;

    while(true)
    {
        char c = *field_data;

        if(c < '0' || c > '9')
        {
            dest = value;
            return field_data;
        }

        field_data++;

        int_t digit = c - '0';

        if(sign == 1)
        {
            if(value >= std::numeric_limits<int_t>::max() / 10) PGE_ATTR_UNLIKELY
            {
                if(value > std::numeric_limits<int_t>::max() / 10 || digit > std::numeric_limits<int_t>::max() % 10) PGE_ATTR_LIKELY
                    return ret_error;
            }
        }
        else
        {
            if(value <= std::numeric_limits<int_t>::min() / 10) PGE_ATTR_UNLIKELY
            {
                if(value < std::numeric_limits<int_t>::min() / 10 || digit > -(std::numeric_limits<int_t>::min() % 10)) PGE_ATTR_LIKELY
                    return ret_error;
            }
        }

        value *= 10;
        if(sign == 1)
            value += digit;
        else
            value -= digit;
    }
}

static const char* s_MDX_load_double(double& dest, const char* field_data)
{
    const char* const ret_error = field_data;

    int sign = 1;

    if(*field_data == '-')
    {
        sign = -1;
        field_data++;
    }

    if((*field_data < '0' || *field_data > '9') && *field_data != '.')
        return ret_error;

    double value = 0;
    double divisor = 0.1;

    while(true)
    {
        char c = *field_data;

        if(c == '.')
        {
            break;
        }
        else if(c < '0' || c > '9')
        {
            if(c == 'e')
                goto exponent;

            dest = sign * value;
            return field_data;
        }

        field_data++;

        if(value >= std::numeric_limits<double>::max() / 10) PGE_ATTR_UNLIKELY
            return ret_error;

        value *= 10;
        value += static_cast<double>(c - '0');
    }

    field_data++;

    while(true)
    {
        char c = *field_data;

        if(c < '0' || c > '9')
        {
            // don't allow "." and "-."
            if(field_data == ret_error + 1 || (field_data == ret_error + 2 && sign == -1))
                return ret_error;

            if(c == 'e')
                goto exponent;

            dest = sign * value;
            return field_data;
        }

        field_data++;

        value += divisor * static_cast<double>(c - '0');
        divisor *= 0.1;
    }

exponent:
    field_data++;

    // allow plus sign (but not followed by minus sign)
    if(field_data[0] == '+' && field_data[1] != '-')
        field_data++;

    const char* exp_start = field_data;
    int allowed_chars = (*exp_start == '-') ? 5 : 4;

    int exponent;
    field_data = s_MDX_load_int<int, 1>(exponent, field_data);
    if(field_data == exp_start || field_data - exp_start > allowed_chars)
        return ret_error;

    if(exponent > DBL_MAX_10_EXP)
        return ret_error;

    dest = sign * value * std::pow(10, exponent);
    return field_data;
}

static void s_MDX_sprintf_append(std::string& out, const char* format, ...)
{
    size_t old_size = out.size();
    out.resize(old_size + 32);

    va_list values;
    va_start(values, format);
    int printed = vsnprintf(&out[old_size], 32, format, values);
    va_end(values);

    if(printed >= 32)
    {
        // we would have lost something, but none of the numbers should ever take this long
        out.resize(old_size + 31);
    }
    else
        out.resize(old_size + printed);
}


/******************************
 * exported utility functions *
 ******************************/

const char* MDX_load_int(int& dest, const char* field_data)
{
    return s_MDX_load_int<int, 1>(dest, field_data);
}

const char* MDX_load_long(long& dest, const char* field_data)
{
    return s_MDX_load_int<long, 1>(dest, field_data);
}

const char* MDX_load_longlong(long long& dest, const char* field_data)
{
    return s_MDX_load_int<long long, 1>(dest, field_data);
}

const char* MDX_load_uint(int& dest, const char* field_data)
{
    return s_MDX_load_uint(dest, field_data);
}

const char* MDX_load_ulong(long& dest, const char* field_data)
{
    return s_MDX_load_uint(dest, field_data);
}

const char* MDX_load_ulonglong(long long& dest, const char* field_data)
{
    return s_MDX_load_uint(dest, field_data);
}

const char* MDX_load_uint(unsigned int& dest, const char* field_data)
{
    return s_MDX_load_uint(dest, field_data);
}

const char* MDX_load_ulong(unsigned long& dest, const char* field_data)
{
    return s_MDX_load_uint(dest, field_data);
}

const char* MDX_load_ulonglong(unsigned long long& dest, const char* field_data)
{
    return s_MDX_load_uint(dest, field_data);
}

/******************************
 * exported utility functions *
 ******************************/

template<>
const char* MDX_Value<int>::load(int& dest, const char* field_data)
{
    const char* str_end = s_MDX_load_int<int, 1>(dest, field_data);

    if(str_end == field_data)
        throw MDX_bad_term("Bad int");

    return str_end;
}

template<>
bool MDX_Value<int>::save(std::string& out, const int& src)
{
    out.resize(out.size() + 32);
    char* dest = &out[out.size() - 32];

    char* end = milo_yip::i32toa((int32_t)src, dest);

    out.resize(out.size() - 32 + end - dest);

    return true;
}

template<>
const char* MDX_Value<unsigned>::load(unsigned& dest, const char* field_data)
{
    const char* str_end = s_MDX_load_uint(dest, field_data);

    if(str_end == field_data)
        throw MDX_bad_term("Bad uint");

    return str_end;
}

template<>
bool MDX_Value<unsigned>::save(std::string& out, const unsigned& src)
{
    out.resize(out.size() + 32);
    char* dest = &out[out.size() - 32];

    char* end = milo_yip::u32toa((uint32_t)src, dest);

    out.resize(out.size() - 32 + end - dest);

    return true;
}

template<>
const char* MDX_Value<bool>::load(bool& dest, const char* field_data)
{
    if(*field_data == '1')
        dest = true;
    else if(*field_data == '0')
        dest = false;
    else
        throw MDX_bad_term("Bad bool");

    return field_data + 1;
}

template<>
bool MDX_Value<bool>::save(std::string& out, const bool& src)
{
    if(src)
        out += '1';
    else
        out += '0';

    return true;
}

template<>
const char* MDX_Value<long>::load(long& dest, const char* field_data)
{
    const char* str_end = s_MDX_load_int<long, 1>(dest, field_data);

    if(str_end == field_data)
        throw MDX_bad_term("Bad long");

    return str_end;
}

template<>
bool MDX_Value<long>::save(std::string& out, const long& src)
{
    out.resize(out.size() + 32);
    char* dest = &out[out.size() - 32];

    char* end = milo_yip::i64toa((int64_t)src, dest);

    out.resize(out.size() - 32 + end - dest);

    return true;
}

template<>
const char* MDX_Value<unsigned long>::load(unsigned long& dest, const char* field_data)
{
    const char* str_end = s_MDX_load_uint(dest, field_data);

    if(str_end == field_data)
        throw MDX_bad_term("Bad ulong");

    return str_end;
}

template<>
bool MDX_Value<unsigned long>::save(std::string& out, const unsigned long& src)
{
    out.resize(out.size() + 32);
    char* dest = &out[out.size() - 32];

    char* end = milo_yip::u64toa((uint64_t)src, dest);

    out.resize(out.size() - 32 + end - dest);

    return true;
}

template<>
const char* MDX_Value<long long>::load(long long& dest, const char* field_data)
{
    const char* str_end = s_MDX_load_int<long long, 1>(dest, field_data);

    if(str_end == field_data)
        throw MDX_bad_term("Bad llong");

    return str_end;
}

template<>
bool MDX_Value<long long>::save(std::string& out, const long long& src)
{
    s_MDX_sprintf_append(out, "%lld", src);
    return true;
}

template<>
const char* MDX_Value<unsigned long long>::load(unsigned long long& dest, const char* field_data)
{
    const char* str_end = s_MDX_load_uint(dest, field_data);

    if(str_end == field_data)
        throw MDX_bad_term("Bad ullong");

    return str_end;
}

template<>
bool MDX_Value<unsigned long long>::save(std::string& out, const unsigned long long& src)
{
    s_MDX_sprintf_append(out, "%llu", src);
    return true;
}

template<>
const char* MDX_Value<float>::load(float& dest, const char* field_data)
{
    double ret;
    const char* str_end = s_MDX_load_double(ret, field_data);

    if(ret > std::numeric_limits<float>::max()
        || ret < -std::numeric_limits<float>::max()
        || str_end == field_data)
    {
        throw MDX_bad_term("Bad float");
    }

    dest = ret;

    return str_end;
}

template<>
bool MDX_Value<float>::save(std::string& out, const float& src)
{
    s_MDX_sprintf_append(out, "%.10g", src);
    return true;
}

template<>
const char* MDX_Value<double>::load(double& dest, const char* field_data)
{
    const char* str_end = s_MDX_load_double(dest, field_data);

    if(str_end == field_data)
        throw MDX_bad_term("Bad double");

    return str_end;
}

template<>
bool MDX_Value<double>::save(std::string& out, const double& src)
{
    s_MDX_sprintf_append(out, "%.10g", src);
    return true;
}
