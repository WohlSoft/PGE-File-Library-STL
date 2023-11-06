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
#ifndef PGE_FILE_LIB_PRIVATE_H_
#define PGE_FILE_LIB_PRIVATE_H_

#include "pge_file_lib_globs.h"

#ifdef PGE_FILES_QT
#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QStringConverter>
#else
#include <QTextCodec>
#endif
#include <QVector>
#include <QPair>
#include <QMap>
#include <QHash>
#include <QObject>
#include <QUrl>
#include <cmath>
#include <QSize>

typedef int     pge_size_t;
#define PGEGetChar(chr) chr.toLatin1()
typedef QStringList PGESTRINGList;
#define PGEVECTOR QVector
typedef int pge_size_t;
#define PGEMAP  QMap
#define PGEHASH QHash
typedef QFile PGEFILE;

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
#   define QSTR_SKIP_EMPTY_PARTS Qt::SkipEmptyParts
#else
#   define QSTR_SKIP_EMPTY_PARTS QString::SkipEmptyParts
#endif

inline PGESTRING PGESTR_Simpl(const PGESTRING &str)
{
    return str.simplified();
}
inline PGESTRING PGESTR_toLower(const PGESTRING &str)
{
    return str.toLower();
}
inline PGESTRING PGESTR_Trim(const PGESTRING &str)
{
    return str.trimmed();
}
#define PGEGetChar(chr) chr.toLatin1()
typedef QChar PGEChar;
#define PGEVECTOR QVector
#define PGELIST QList
typedef int pge_size_t;
#define PGEPAIR QPair
#define PGEMAP  QMap
#define PGEHASH QHash
typedef QFile PGEFILE;
inline void PGE_SPLITSTRING(PGESTRINGList &dst, const PGESTRING &src, PGESTRING sep)
{
    dst = src.split(sep);
}
inline PGESTRING PGE_ReplSTRING(PGESTRING src, PGESTRING from, PGESTRING to)
{
    return src.replace(from, to);
}
inline PGESTRING PGE_RemSubSTRING(const PGESTRING &src, const PGESTRING &substr)
{
    return QString(src).remove(substr);
}
inline PGESTRING PGE_RemStrRng(PGESTRING &str, int pos, int len)
{
    return str.remove(pos, len);
}
inline PGESTRING PGE_SubStr(const PGESTRING &str, int pos, int len = -1)
{
    return str.mid(pos, len);
}
inline void PGE_CutLength(PGESTRING &str, int maxlength)
{
    if(str.size() > maxlength)
        str.resize(maxlength);
}
inline void PGE_FilterBinary(PGESTRING &str)
{
    for(int i = 0; i < str.size(); i++)
    {
        PGEChar c = str[i];
        if(c == PGEChar('\r'))
            str[i] = PGEChar('r');
        else if(c == PGEChar('\n'))
            str[i] = PGEChar('n');
        else if(c.toLatin1() < 0x20 || c.toLatin1() == 0x7F)
            str[i] = PGEChar('?');
    }
}
inline bool      IsNULL(const PGESTRING &str)
{
    return str.isNull();
}
inline bool      IsEmpty(const PGESTRING &str)
{
    return str.isEmpty();
}
inline bool      IsEmpty(const PGESTRINGList &str)
{
    return str.isEmpty();
}
inline int       toInt(const PGESTRING &str)
{
    return str.toInt();
}
inline unsigned int toUInt(const PGESTRING &str)
{
    return str.toUInt();
}
inline long      toLong(const PGESTRING &str)
{
    return str.toLong();
}
inline unsigned long toULong(const PGESTRING &str)
{
    return str.toULong();
}
inline float     toFloat(const PGESTRING &str)
{
    return str.toFloat();
}
inline double    toDouble(const PGESTRING &str)
{
    return str.toDouble();
}
inline PGESTRING removeSpaces(PGESTRING src)
{
    return src.remove(' ');
}

#define toPgeString(x) QString::fromStdString(x)

template<typename T>
PGESTRING fromNum(T num)
{
    return QString::number(num);
}

inline PGESTRING fromBoolToNum(bool num)
{
    return QString::number(static_cast<int>(num));
}

namespace PGE_FileFormats_misc
{
    PGESTRING    url_encode(const PGESTRING &sSrc);
    std::string  base64_encode(uint8_t const *bytes_to_encode, size_t in_len, bool no_padding = false);
    std::string  base64_encode(std::string const &source, bool no_padding = false);
    std::string  base64_decode(std::string const &encoded_string);
    QString      base64_encode(QString &source, bool no_padding = false);
    QString      base64_decode(QString &source);
    QString      base64_encodeW(QString &source, bool no_padding = false);
    QString      base64_decodeW(QString &source);
    QString      base64_encodeA(QString &source, bool no_padding = false);
    QString      base64_decodeA(QString &source);
}
inline PGESTRING PGE_URLENC(const PGESTRING &src)
{
    return PGE_FileFormats_misc::url_encode(src);
}
inline PGESTRING PGE_URLDEC(const PGESTRING &src)
{
    /* Don't call fromPercentEncoding() on Windows with empty string,
     * or crash will happen! */
    if(IsEmpty(src))
        return PGESTRING();
    return QUrl::fromPercentEncoding(src.toUtf8());
}
#define PGE_BASE64ENC(src)   PGE_FileFormats_misc::base64_encode(src)
#define PGE_BASE64ENC_nopad(src)   PGE_FileFormats_misc::base64_encode(src, true)
#define PGE_BASE64DEC(src)   PGE_FileFormats_misc::base64_decode(src)
#define PGE_BASE64ENC_W(src) PGE_FileFormats_misc::base64_encodeW(src)
#define PGE_BASE64DEC_W(src) PGE_FileFormats_misc::base64_decodeW(src)
#define PGE_BASE64ENC_A(src) PGE_FileFormats_misc::base64_encodeA(src)
#define PGE_BASE64DEC_A(src) PGE_FileFormats_misc::base64_decodeA(src)

#else /* ------ PGE_FILES_QT ------ */

#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <map>
#include <cmath>
#include <cctype>
#include <unordered_map>

#ifdef _MSC_VER
static char ToLowerFun(char ch)
{
    return static_cast<char>(::tolower(ch));
}
#else
#define ToLowerFun ::tolower
#endif

typedef std::string::size_type pge_size_t;
inline PGESTRING PGESTR_Simpl(PGESTRING str)
{
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    return str;
}
inline PGESTRING PGESTR_toLower(PGESTRING str)
{
    std::transform(str.begin(), str.end(), str.begin(), ToLowerFun);
    return str;
}

// trim from start (in place)
static inline void PGESTR_ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch)
    {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void PGESTR_rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch)
    {
        return !std::isspace(ch);
    }).base(), s.end());
}

inline PGESTRING PGESTR_Trim(const PGESTRING &str)
{
    PGESTRING s = str;
    PGESTR_ltrim(s);
    PGESTR_rtrim(s);
    return s;
}

#define PGEGetChar(chr) chr
#define PGEVECTOR std::vector
typedef size_t pge_size_t;
#define PGEMAP  std::map
#define PGEHASH std::unordered_map
typedef std::fstream PGEFILE;

namespace PGE_FileFormats_misc
{
    void split(std::vector<std::string> &dest, const std::string &str, const std::string& separator);
    void replaceAll(std::string &str, const std::string &from, const std::string &to);
    void RemoveSub(std::string &sInput, const std::string &sub);
    bool hasEnding(std::string const &fullString, std::string const &ending);
    PGESTRING url_encode(const PGESTRING &sSrc);
    PGESTRING url_decode(const std::string &sSrc);
    std::string base64_encode(unsigned char const *bytes_to_encode, size_t in_len, bool no_padding = false);
    std::string base64_encode(std::string const &source, bool no_padding = false);
    std::string base64_decode(std::string const &encoded_string);
    std::string base64_encodeW(std::string &source, bool no_padding = false);
    std::string base64_decodeW(std::string &source);
    std::string base64_encodeA(std::string &source, bool no_padding = false);
    std::string base64_decodeA(std::string &source);
}

inline void PGE_SPLITSTRING(PGESTRINGList &dst, const PGESTRING &src, const PGESTRING &sep)
{
    dst.clear();
    PGE_FileFormats_misc::split(dst, src, sep);
}

inline PGESTRING PGE_ReplSTRING(PGESTRING src, const PGESTRING &from, const PGESTRING &to)
{
    PGE_FileFormats_misc::replaceAll(src, from, to);
    return src;
}

inline PGESTRING PGE_RemSubSTRING(PGESTRING src, const PGESTRING &substr)
{
    PGE_FileFormats_misc::RemoveSub(src, substr);
    return src;
}

inline PGESTRING PGE_RemStrRng(PGESTRING &str, int pos, int len)
{
    str.erase(static_cast<size_t>(pos), static_cast<size_t>(len));
    return str;
}
inline PGESTRING PGE_SubStr(const PGESTRING &str, int pos, int len = -1)
{
    return str.substr(static_cast<std::string::size_type>(pos), static_cast<std::string::size_type>(len));
}

inline void PGE_CutLength(PGESTRING &str, int maxlength)
{
    if(str.size() > static_cast<size_t>(maxlength))
        str.resize(static_cast<size_t>(maxlength));
}

inline void PGE_FilterBinary(PGESTRING &str)
{
    for(PGEChar &c : str)
    {
        if(c == '\r')
            c = 'r';
        else if(c == '\n')
            c = 'n';
        else if(c < 0x20 || c == 0x7F)
            c = '?';
    }
}

inline bool IsNULL(const PGESTRING &str)
{
    return (str.empty());
}

inline bool IsEmpty(const PGESTRING &str)
{
    return str.empty();
}

inline bool IsEmpty(const PGESTRINGList &str)
{
    return str.empty();
}

inline int toInt(const PGESTRING &str)
{
    return std::atoi(str.c_str());
}

inline unsigned int toUInt(const PGESTRING &str)
{
    return static_cast<unsigned int>(std::stoul(str, nullptr, 10));
}

inline long toLong(const PGESTRING &str)
{
    return std::atol(str.c_str());
}

inline unsigned long toULong(const PGESTRING &str)
{
    return static_cast<unsigned long>(std::atoll(str.c_str()));
}

inline float toFloat(const PGESTRING &str)
{
    return static_cast<float>(std::atof(str.c_str()));
}

inline double toDouble(const PGESTRING &str)
{
    return std::atof(str.c_str());
}

inline PGESTRING removeSpaces(const PGESTRING &src)
{
    return PGE_RemSubSTRING(src, " ");
}

#define toPgeString(x) (x)

template<typename T>
PGESTRING fromNum(T num)
{
    std::ostringstream n;
    n << num;
    return n.str();
}

inline PGESTRING fromBoolToNum(bool num)
{
    std::ostringstream n;
    n << static_cast<int>(num);
    return n.str();
}
#define PGE_URLENC(src) PGE_FileFormats_misc::url_encode(src)
#define PGE_URLDEC(src) PGE_FileFormats_misc::url_decode(src)
#define PGE_BASE64ENC(src)   PGE_FileFormats_misc::base64_encode(src)
#define PGE_BASE64ENC_nopad(src) PGE_FileFormats_misc::base64_encode(src, true)
#define PGE_BASE64DEC(src)   PGE_FileFormats_misc::base64_decode(src)
#define PGE_BASE64ENC_W(src) PGE_FileFormats_misc::base64_encodeW(src)
#define PGE_BASE64DEC_W(src) PGE_FileFormats_misc::base64_decodeW(src)
#define PGE_BASE64ENC_A(src) PGE_FileFormats_misc::base64_encodeA(src)
#define PGE_BASE64DEC_A(src) PGE_FileFormats_misc::base64_decodeA(src)
#endif /* ------ PGE_FILES_QT ------ */

inline bool PGE_floatEqual(double l, double r, double precission)
{
    return static_cast<long long>(l * std::pow(10.0, precission)) ==
           static_cast<long long>(r * std::pow(10.0, precission));
}

inline bool PGE_floatEqual(float l, float r, float precission)
{
    return static_cast<long long>(l * std::pow(10.0f, precission)) ==
           static_cast<long long>(r * std::pow(10.0f, precission));
}

inline bool PGE_StartsWith(const PGESTRING &src, const PGESTRING &with)
{
#ifdef PGE_FILES_QT
    return src.startsWith(with, Qt::CaseSensitive);
#else
    return !src.compare(0, with.size(), with);
#endif
}

static inline uint32_t PGE_toNearestU(double o)
{
    if(uint32_t(o) % 2 == 0 && (uint32_t(o * 100000.0) == uint32_t((uint32_t(o) * 100000) + 50000)))
        return uint32_t(o);
    else if(uint32_t(o) % 2 != 0 && (uint32_t(o * 100000.0) == uint32_t((uint32_t(o) * 100000) + 50000)))
        return uint32_t(o + 1.0);
    else
        return uint32_t(o + 0.5);
}

static inline int PGE_toNearestS(double o)
{
    int sign = o >= 0 ? +1 : -1;
    if(o < 0)
        o = -o;

    return int(PGE_toNearestU(o)) * sign;
}

#endif // PGE_FILE_LIB_PRIVATE_H_
