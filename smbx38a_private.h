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

#pragma once
#ifndef SMBX38A_PRIVATE_H
#define SMBX38A_PRIVATE_H

#include <functional>
#include "smbx64.h"
#include "smbx64_macro.h"
#include "CSVReaderPGE.h"
#include "CSVUtils.h"
#include "pge_ff_units.h"

static constexpr uint32_t latest_version_38a = 69;

using namespace CSVReader;

extern const int SMBX38A_NpcGeneratorTypes[29];
extern const int SMBX38A_NpcGeneratorDirections[29];

//for Header readers.
//Use it if you want read file partially
//(you must create QTextStream in(&fstream); !!!)
#define SMBX38A_FileBegin() int file_format=0;  /*File format number*/\
    PGESTRING line       /*Current Line data*/
#define SMBX38A_FileBeginN() /*int file_format=0;*/  /*File format number*/\
    PGESTRING line       /*Current Line data*/

#if !defined(_MSC_VER) || _MSC_VER > 1800
#define ReadSMBX38Level ReadSMBX38ALvlFile
#else
#define ReadSMBX38Level ReadSMBX38ALvlFile_OLD
#endif

//Jump to next line
#ifdef nextLine
#undef nextLine
#endif
#define nextLine() line = in.readLine()

// Common functions
static auto PGEUrlDecodeFunc = [](PGESTRING &data)
{
    data = PGE_URLDEC(data);
};
static auto PGEBase64DecodeFunc = [](PGESTRING &data)
{
    data = PGE_BASE64DEC(data);
};
static auto PGEBase64DecodeFuncA = [](PGESTRING &data)
{
    data = PGE_BASE64DEC_A(data);
};
static auto PGELayerOrDefault = [](PGESTRING &data)
{
    data = (IsEmpty(data) ? "Default" : PGE_URLDEC(data));
};
static auto PGEFilpBool = [](bool &value)
{
    value = !value;
};

template<class T>
constexpr std::function<void(T &)> MakeMinFunc(T min)
{
    return [ = ](T & value)
    {
        if(value < min)
            value = min;
    };
}

/*!
 * \brief Converts floating point number value from the expression field.
 * \param [__inout] expression Expression field. Clears if valid floating point number has been detected
 * \param [__out] target Target value
 */
inline void SMBX38A_Exp2Float(PGESTRING &expression, float &target)
{
    if(!SMBX64::IsFloat(expression))
        target = 0.0f;
    else
    {
        target = toFloat(expression);
        expression.clear();
    }
}

/*!
 * \brief Converts floating point number value from the expression field.
 * \param [__inout] expression Expression field. Clears if valid floating point number has been detected
 * \param [__out] target Target value
 */
inline void SMBX38A_Exp2Double(PGESTRING &expression, double &target)
{
    if(!SMBX64::IsFloat(expression))
        target = 0.0;
    else
    {
        target = toDouble(expression);
        expression.clear();
    }
}


/*!
 * \brief Converts integer value from the expression field (with rounding possible floating point number value).
 * \param [__inout] expression Expression field. Clears if valid floating point number has been detected
 * \param [__out] target Target value
 */
template<typename T>
inline void SMBX38A_Exp2Int(PGESTRING &expression, T &target)
{
    if(!SMBX64::IsFloat(expression))
        target = 0;
    else
    {
        target = static_cast<T>(round(toDouble(expression)));
        expression.clear();
    }
}

template<typename T>
inline void SMBX38A_Num2Exp(T source, PGESTRING &expression)
{
    if(IsEmpty(expression))
        expression = fromNum(source);
}

template<typename T>
inline void SMBX38A_Num2Exp_URLEN(T source, PGESTRING &expression)
{
    if(IsEmpty(expression))
        expression = fromNum(source);
    else
        expression = PGE_URLENC(expression);
}

template<typename T>
inline void SMBX38A_mapBGID_From(T &bgID)
{
    if(bgID == 2)
        bgID = 13;
    else if((bgID >= 3) && (bgID <= 13))
        bgID -= 1;
}

template<typename T>
inline T SMBX38A_mapBGID_To(T bgID)
{
    if(bgID == 13)
        bgID = 2;
    else if((bgID >= 2) && (bgID <= 12))
        bgID += 1;

    return bgID;
}

template<typename NumT>
inline void SMBX38A_RestoreOrigTime(NumT &orig, const NumT &cur, PGE_FileLibrary::TimeUnit unit)
{
    NumT value = PGE_FileLibrary::TimeUnitsCVT(orig,
                 PGE_FileLibrary::TimeUnit::FrameOneOf65sec,
                 unit);
    if(value != cur)
        orig = PGE_FileLibrary::TimeUnitsCVT(cur,
                                             unit,
                                             PGE_FileLibrary::TimeUnit::FrameOneOf65sec);
}

static inline void SMBX38A_CC_decode(int32_t &destKey, int64_t &destValue, const PGESTRING &key)
{
    if(IsEmpty(key) || (key.size() < 4))
    {
        destKey = 0;
        destValue = 0;
    }

#ifdef PGE_FILES_QT
    std::string skey = key.toStdString();
#else
    const std::string &skey = key;
#endif

    std::string k = skey.substr(0, 4);
    destKey = std::strtol(k.c_str(), nullptr, 16);

    std::string v = skey.substr(4);
    if(!v.empty())
        destValue = std::strtol(v.c_str(), nullptr, 16);
}

static inline PGESTRING SMBX38A_CC_encode(int32_t srcKey, int64_t srcValue)
{
    char keyBuf[6] = {0};
    char valueBuf[52] = {0};
    int kb_l = std::snprintf(keyBuf, 5, "%04X", static_cast<unsigned int>(srcKey));
    keyBuf[kb_l] = '\0';
    int kv_l = std::snprintf(valueBuf, 51, "%X", static_cast<unsigned int>(srcValue));
    valueBuf[kv_l] = '\0';
#ifdef PGE_FILES_QT
    return QString::fromUtf8(keyBuf, 4) + QString::fromUtf8(valueBuf, kv_l);
#else
    return PGESTRING(keyBuf, 4) + PGESTRING(valueBuf, kv_l);
#endif
}

#endif // SMBX38A_PRIVATE_H
