/*
 * PGE File Library - a library to process file formats, part of Moondust project
 *
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "smbx64.h"

namespace smbx64Format
{
    //const char *uint_vc = "0123456789";
    //const int   uint_vc_len = 10;

    static bool isDegit(PGEChar c)
    {
        return ((c >= '0') && (c <= '9'));
    }

#if 0
    static bool isValid(PGESTRING &s, const char *valid_chars, pge_size_t valid_chars_len)
    {
        if(IsEmpty(s)) return false;
        pge_size_t i, j;

        for(i = 0; i < s.size(); i++)
        {
            bool found = false;
            for(j = 0; j < valid_chars_len; j++)
            {
                if(PGEGetChar(s[i]) == valid_chars[j])
                {
                    found = true;
                    break;
                }
            }

            if(!found)
                return false;
        }

        return true;
    }
#endif
}

// /////////////Validators///////////////
//returns FALSE on wrong data

bool SMBX64::IsUInt(const PGESTRING &in) // UNSIGNED INT
{
    using namespace smbx64Format;

    if(IsEmpty(in))
        return false;

#ifdef PGE_FILES_QT
    const PGEChar *data = in.data();
#else
    const PGEChar *data = in.data();
#endif

    pge_size_t strSize = in.size();
    for(pge_size_t i = 0; i < strSize; i++)
    {
        const PGEChar c = *data++;
        if((c < '0') || (c > '9'))
            return false;
    }

    return true;
}

bool SMBX64::IsSInt(const PGESTRING &in) // SIGNED INT
{
    using namespace smbx64Format;
    if(IsEmpty(in)) return false;

    if((in.size() == 1) && (!isDegit(in[0])))
        return false;

    if((!isDegit(in[0])) && (PGEGetChar(in[0]) != '-'))
        return false;

#ifdef PGE_FILES_QT
    const PGEChar *data = in.data() + 1;
#else
    const PGEChar *data = &in[0] + 1;
#endif

    pge_size_t strSize = in.size();
    for(pge_size_t i = 1; i < strSize; i++)
    {
        const PGEChar c = *data++;
        if((c < '0') || (c > '9'))
            return false;
    }

    return true;
}

bool SMBX64::IsFloat(PGESTRING &in) // SIGNED FLOAT
{
    using namespace smbx64Format;

    if(IsEmpty(in))
        return true;

    if((in.size() == 1) && (!isDegit(in[0])))
        return false;

    if((!isDegit(in[0])) && (PGEGetChar(in[0]) != '-') && (PGEGetChar(in[0]) != '.') && (PGEGetChar(in[0]) != ','))
        return false;

    bool decimal = false;
    bool pow10  = false;
    bool sign   = false;

    for(pge_size_t i = ((PGEGetChar(in[0]) == '-') ? 1 : 0); i < in.size(); i++)
    {
        if((!decimal) && (!pow10))
        {
            if((PGEGetChar(in[i]) == '.') || (PGEGetChar(in[i]) == ','))
            {
                in[i] = '.'; //replace comma with a dot
                decimal = true;
                if(i == (in.size() - 1)) return false;
                continue;
            }
        }
        if(!pow10)
        {
            if((PGEGetChar(in[i]) == 'E') || (PGEGetChar(in[i]) == 'e'))
            {
                pow10 = true;
                if(i == (in.size() - 1)) return false;
                continue;
            }
        }
        else
        {
            if(!sign)
            {
                sign = true;
                if((PGEGetChar(in[i]) == '+') || (PGEGetChar(in[i]) == '-'))
                {
                    if(i == (in.size() - 1)) return false;
                    continue;
                }
            }
        }
        if(!isDegit(in[i])) return false;
    }

    return true;
}

bool SMBX64::IsQuotedString(const PGESTRING &in)
{
    //This is INVERTED validator. If false - good, true - bad.
#define QStrGOOD true
#define QStrBAD false
    pge_size_t i = 0;

    for(i = 0; i < in.size(); i++)
    {
        if(i == 0)
        {
            if(in[i] != '"')
                return QStrBAD;
        }
        else if(i == in.size() - 1)
        {
            if(in[i] != '"')
                return QStrBAD;
        }
        else if(in[i] == '"')
            return QStrBAD;
    }

    if(i == 0)
        return QStrBAD; //This is INVERTED validator. If false - good, true - bad.

    return QStrGOOD;
#undef QStrGOOD
#undef QStrBAD
}
