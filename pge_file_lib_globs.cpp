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

#include "pge_file_lib_globs.h"
#include "pge_file_lib_private.h"
#ifdef _WIN32
#include "windows.h"
#endif
#ifndef PGE_FILES_QT
#include <limits.h> /* PATH_MAX */
#include <sstream>
#include <algorithm>
#include <string>
#include "charsetconvert.h"
#ifndef PATH_MAX
/*
 * Needed to shut up CLang's static analyzer that showing usage of this
 * macro as "not exists" even I have limits.h included here.
 * For other cases it's just a dead code
 */
#define PATH_MAX 2048
#endif
#else
#include <QFileInfo>
#endif
#include <memory>

namespace PGE_FileFormats_misc
{

bool PGE_DetectSMBXFile(PGESTRING src)
{
    /*
     * First line of SMBX1...64 file must contain number from 0 to 64
     */
    src.push_back('\n');//Append double line feed to end (for case if sent first line only)
    src.push_back('\n');

    if((src.size() < 3))
        return false;//If line too short

    if((src[1] != '\n') && (src[2] != '\n') &&
       (src[1] != '\r') && (src[2] != '\r'))
        return false;//If line contains no line feeds (also possible CRLF)

    if(src[0] < '0' || src[0] > '9')
        return false;//If first character is not numeric

    if((src[1] != '\r') && (src[1] != '\n') && (src[1] < '0' || src[1] > '9'))
        return false;//If second character is not numeric and is not line feed

    PGESTRING number;
    number.push_back(src[0]);

    if((src[1] != '\n') && (src[1] != '\r'))
        number.push_back(src[1]);

    int version = toInt(number);

    if(version > 64)  //Unsupported version number!
        return false;

    return true;
}

#ifndef PGE_FILES_QT
#ifdef _WIN32
static std::wstring Str2WStr(const std::string &str)
{
    std::wstring dest;
    dest.resize(str.size());
    int dest_len = static_cast<int>(str.length());
    int new_len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), dest_len, &dest[0], dest_len);
    dest.resize(static_cast<size_t>(new_len));
    return dest;
}

static std::string WStr2Str(const std::wstring &wstr)
{
    std::string dest;
    dest.resize((wstr.size() * 2));
    int wstr_len = static_cast<int>(wstr.length());
    int dest_len = static_cast<int>(dest.size());
    int new_len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr_len, &dest[0], dest_len, nullptr, nullptr);
    dest.resize(static_cast<size_t>(new_len));
    return dest;
}

FILE *utf8_fopen(const char *file, const char *mode)
{
    wchar_t wfile[MAX_PATH + 1];
    wchar_t wmode[21];
    int wfile_len = int(strlen(file));
    int wmode_len = int(strlen(mode));
    wfile_len = MultiByteToWideChar(CP_UTF8, 0, file, wfile_len, wfile, MAX_PATH);
    wmode_len = MultiByteToWideChar(CP_UTF8, 0, mode, wmode_len, wmode, 20);
    wfile[wfile_len] = L'\0';
    wmode[wmode_len] = L'\0';
    return _wfopen(wfile, wmode);
}

#else
#   define utf8_fopen fopen
#endif

void split(std::vector<std::string> &dest, const std::string &str, const std::string &separator)
{
    dest.clear();
    std::string::size_type begin = 0;
    std::string::size_type end   = 0;
    std::string::size_type sepLen = separator.size();
    do
    {
        end = str.find(separator, begin);
        std::string s = str.substr(begin, end - begin);
        if(!s.empty())
            dest.push_back(s);
        begin = end + sepLen;
    }
    while(end != std::string::npos);
}

void replaceAll(std::string &str, const std::string &from, const std::string &to)
{
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

void RemoveSub(std::string &sInput, const std::string &sub)
{
    std::string::size_type foundpos = sInput.find(sub);
    if(foundpos != std::string::npos)
        sInput.erase(sInput.begin() + std::string::difference_type(foundpos),
                     sInput.begin() + std::string::difference_type(foundpos + sub.length()));
}

bool hasEnding(std::string const &fullString, std::string const &ending)
{
    if(fullString.length() >= ending.length())
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    else
        return false;
}
#endif

PGESTRING url_encode(const PGESTRING &sSrc)
{
    if(IsEmpty(sSrc))
        return sSrc;
    const char DEC2HEX[16 + 1] = "0123456789ABCDEF";
#ifndef PGE_FILES_QT
    const uint8_t *pSrc = reinterpret_cast<const uint8_t *>(sSrc.c_str());
#else
    std::string ssSrc = sSrc.toStdString();
    const uint8_t *pSrc = reinterpret_cast<const uint8_t *>(ssSrc.c_str());
#endif
    const size_t SRC_LEN = static_cast<size_t>(sSrc.length());
    std::unique_ptr<uint8_t[]> pStart(new uint8_t[SRC_LEN * 3]);
    uint8_t *pEnd = pStart.get();
    const uint8_t *const SRC_END = pSrc + SRC_LEN;
    for(; pSrc < SRC_END; ++pSrc)
    {
        //Do full encoding!
        *pEnd++ = '%';
        *pEnd++ = static_cast<uint8_t>(DEC2HEX[*pSrc >> 4]);
        *pEnd++ = static_cast<uint8_t>(DEC2HEX[*pSrc & 0x0F]);
    }
#ifndef PGE_FILES_QT
    PGESTRING sResult(reinterpret_cast<char *>(pStart.get()), reinterpret_cast<char *>(pEnd));
#else
    PGESTRING sResult = QString::fromUtf8(reinterpret_cast<char *>(pStart.get()), static_cast<int>(pEnd - pStart.get()));
#endif
    return sResult;
}

#ifndef PGE_FILES_QT
const int_fast8_t HEX2DEC[256] =
{
    /*       0  1  2  3   4  5  6  7   8  9  A  B   C  D  E  F */
    /* 0 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 1 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 2 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 3 */  0, 1, 2, 3,  4, 5, 6, 7,  8, 9, -1, -1, -1, -1, -1, -1,

    /* 4 */ -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 5 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 6 */ -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 7 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

    /* 8 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 9 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* A */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* B */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

    /* C */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* D */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* E */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* F */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

PGESTRING url_decode(const std::string &sSrc)
{
    if(sSrc.empty())
        return sSrc;
    // Note from RFC1630: "Sequences which start with a percent
    // sign but are not followed by two hexadecimal characters
    // (0-9, A-F) are reserved for future extension"
    const uint8_t *pSrc = reinterpret_cast<const uint8_t *>(sSrc.c_str());
    const size_t SRC_LEN = sSrc.length();
    const uint8_t *const SRC_END = pSrc + SRC_LEN;
    // last decodable '%'
    const uint8_t *const SRC_LAST_DEC = SRC_END - 2;

    char *pStart = reinterpret_cast<char *>(std::malloc(SRC_LEN + 1));
    if(!pStart)
        return "";
    std::memset(pStart, 0, SRC_LEN + 1);
    char *pEnd = pStart;

    while(pSrc < SRC_LAST_DEC)
    {
        if(*pSrc == '%')
        {
            int_fast8_t dec1, dec2;
            if(-1 != (dec1 = HEX2DEC[*(pSrc + 1)])
               && -1 != (dec2 = HEX2DEC[*(pSrc + 2)]))
            {
                *pEnd++ = static_cast<char>((dec1 << 4) + dec2);
                pSrc += 3;
                continue;
            }
        }

        *pEnd++ = static_cast<char>(*pSrc++);
    }

    // the last 2- chars
    while(pSrc < SRC_END)
        *pEnd++ = static_cast<char>(*pSrc++);

    std::string::size_type pLen = static_cast<std::string::size_type>(pEnd - pStart);
    std::string sResult(pStart, pLen);
    std::free(pStart);
    return sResult;
}
#endif


static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

static inline bool is_base64(unsigned char c)
{
    return (isalnum(c) || (c == '+') || (c == '/'));
}

#ifdef PGE_FILES_QT
/*
#define PGE_BASE64ENC_W(src) QString::fromStdString(PGE_FileFormats_misc::base64_encodeW(src.toStdWString()))
#define PGE_BASE64DEC_W(src) src.fromStdWString(PGE_FileFormats_misc::base64_decodeW(src.toStdString()))
*/
QString      base64_encodeW(QString &source, bool no_padding)
{
    return QString::fromStdString(
               base64_encode(reinterpret_cast<const uint8_t *>(source.utf16()),
                             static_cast<size_t>(source.size()) * sizeof(uint16_t), no_padding)
           );
}

QString      base64_decodeW(QString &source)
{
    std::string sout = base64_decode(source.toStdString());
    QString out;
    out.setUtf16(reinterpret_cast<const uint16_t *>(sout.data()), static_cast<int>(sout.size() / 2));
    return out;
}
QString      base64_encodeA(QString &source, bool no_padding)
{
    return QString::fromStdString(
               base64_encode(reinterpret_cast<const unsigned char *>(source.toLatin1().data()),
                             static_cast<size_t>(source.size()), no_padding)
           );
}

QString      base64_decodeA(QString &source)
{
    std::string sout = base64_decode(source.toStdString());
    return QString::fromLatin1(sout.data(), static_cast<int>(sout.size()));
}

QString      base64_encode(QString &source, bool no_padding)
{
    std::string out = source.toStdString();
    if((out.size() == 0) || (out[out.size() - 1] != '\0'))
        out.push_back('\0');
    return QString::fromStdString(
               base64_encode(reinterpret_cast<const unsigned char *>(out.data()),
                             out.size(), no_padding)
           );
}

QString      base64_decode(QString &source)
{
    std::string sout = base64_decode(source.toStdString());
    return QString::fromUtf8(sout.data(), static_cast<int>(sout.size()));
}

#else
std::string  base64_encodeW(std::string &source, bool no_padding)
{
    SI_ConvertW<wchar_t> utf8(true);
    size_t new_len = utf8.SizeFromStore(source.c_str(), source.length());
    std::wstring t;
    t.resize(new_len);
    if(utf8.ConvertFromStore(source.c_str(), source.length(), &t[0], new_len))
        return base64_encode(reinterpret_cast<const unsigned char *>(t.c_str()), t.size(), no_padding);
    return "<fail to convert charset>";
}

std::string base64_decodeW(std::string &source)
{
    std::string out = base64_decode(source);
#if 0 // def DEBUG_BUILD // Clunky debug code
    if(!out.empty())
    {
        FILE *x = utf8_fopen("test.txt", "ab");
        //        for(size_t i=0; i<out.size(); i++)
        //             printf("%i ", (int)out[i]);
        //        printf("%s", out.c_str());
        if(x)
        {
            fwrite((void *) out.c_str(), sizeof(char), out.size(), x);
            fflush(x);
            fclose(x);
        }
    }
#endif
    std::wstring outw(reinterpret_cast<const wchar_t *>(out.c_str()));
    SI_ConvertW<wchar_t> utf8(true);
    size_t new_len = outw.length() * 2; //utf8.SizeToStore(outw.c_str());
    std::string out2;
    out2.resize(new_len);
    if(utf8.ConvertToStore(outw.c_str(), &out2[0], new_len))
        return out2;
    return "<fail to convert charset>";
}

std::string base64_encodeA(std::string &source, bool no_padding)
{
    return base64_encode(reinterpret_cast<const unsigned char *>(source.c_str()), source.size(), no_padding);
}

std::string base64_decodeA(std::string &source)
{
    return base64_decode(source);
}
#endif

std::string base64_encode(std::string const &source, bool no_padding)
{
    return base64_encode(reinterpret_cast<const uint8_t *>(source.c_str()), source.size(), no_padding);
}

std::string base64_encode(const uint8_t *bytes_to_encode, size_t in_len, bool no_padding)
{
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while(in_len--)
    {
        char_array_3[i++] = *(bytes_to_encode++);
        if(i == 3)
        {
            char_array_4[0] = static_cast<uint8_t>((char_array_3[0] & 0xfc) >> 2);
            char_array_4[1] = static_cast<uint8_t>(((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4));
            char_array_4[2] = static_cast<uint8_t>(((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6));
            char_array_4[3] = static_cast<uint8_t>(char_array_3[2] & 0x3f);

            for(i = 0; (i < 4) ; i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if(i)
    {
        for(j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = static_cast<uint8_t>((char_array_3[0] & 0xfc) >> 2);
        char_array_4[1] = static_cast<uint8_t>(((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4));
        char_array_4[2] = static_cast<uint8_t>(((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6));
        char_array_4[3] = static_cast<uint8_t>(char_array_3[2] & 0x3f);

        for(j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];

        while(i++ < 3)
        {
            if(!no_padding) ret += '=';
        }
    }

    return ret;
}

std::string base64_decode(std::string const &encoded_string)
{
    size_t in_len = encoded_string.size();
    size_t i = 0;
    size_t j = 0;
    size_t in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;

    while(in_len-- && (encoded_string[in_] != '=') && is_base64(static_cast<uint8_t>(encoded_string[in_])))
    {
        char_array_4[i++] = static_cast<uint8_t>(encoded_string[in_]);
        in_++;
        if(i == 4)
        {
            for(i = 0; i < 4; i++)
                char_array_4[i] = static_cast<uint8_t>(base64_chars.find(static_cast<char>(char_array_4[i])));
            char_array_3[0] = static_cast<uint8_t>((char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4));
            char_array_3[1] = static_cast<uint8_t>(((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2));
            char_array_3[2] = static_cast<uint8_t>(((char_array_4[2] & 0x3) << 6) + char_array_4[3]);
            for(i = 0; (i < 3); i++)
                ret += static_cast<char>(char_array_3[i]);
            i = 0;
        }
    }

    if(i)
    {
        for(j = i; j < 4; j++)
            char_array_4[j] = 0;

        for(j = 0; j < 4; j++)
            char_array_4[j] = static_cast<uint8_t>(base64_chars.find(static_cast<char>(char_array_4[j])));

        char_array_3[0] = static_cast<uint8_t>((char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4));
        char_array_3[1] = static_cast<uint8_t>(((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2));
        char_array_3[2] = static_cast<uint8_t>(((char_array_4[2] & 0x3) << 6) + char_array_4[3]);

        for(j = 0; (j < i - 1); j++)
            ret += static_cast<char>(char_array_3[j]);
    }

    //Remove zero from end
    if(ret.size() > 0)
    {
        if(ret[ret.size() - 1] == '\0')
            ret.resize(ret.size() - 1);
    }

    return ret;
}

bool TextFileInput::exists(const PGESTRING &filePath)
{
#ifdef PGE_FILES_QT
    return QFile::exists(filePath);
#else
    FILE *x = utf8_fopen(filePath.c_str(), "rb");
    if(x)
    {
        fclose(x);
        return true;
    }
    return false;
#endif
}

bool TextFileOutput::exists(const PGESTRING &filePath)
{
#ifdef PGE_FILES_QT
    return QFile::exists(filePath);
#else
    FILE *x = utf8_fopen(filePath.c_str(), "rb");
    if(x)
    {
        fclose(x);
        return true;
    }
    return false;
#endif
}


/*****************BASE TEXT I/O CLASS***************************/
TextInput::TextInput() : m_lineNumber(0) {}

PGESTRING TextInput::read(int64_t)
{
    return PGESTRING();
}
PGESTRING TextInput::readLine()
{
    return PGESTRING();
}
PGESTRING TextInput::readCVSLine()
{
    return PGESTRING();
}
PGESTRING TextInput::readAll()
{
    return PGESTRING();
}

bool TextInput::eof()
{
    return true;
}
int64_t TextInput::tell()
{
    return 0;
}
int TextInput::seek(int64_t, TextInput::positions)
{
    return -1;
}
PGESTRING TextInput::getFilePath()
{
    return m_filePath;
}
void TextInput::setFilePath(const PGESTRING &path)
{
    m_filePath = path;
}
long TextInput::getCurrentLineNumber()
{
    return m_lineNumber;
}

bool TextInput::reOpen(bool)
{
    // Do nothing
    return true;
}


TextOutput::TextOutput() : m_lineNumber(0) {}

int TextOutput::write(PGESTRING)
{
    return 0;
}
int64_t TextOutput::tell()
{
    return 0;
}
int TextOutput::seek(int64_t, TextOutput::positions)
{
    return -1;
}
PGESTRING TextOutput::getFilePath()
{
    return m_filePath;
}
void TextOutput::setFilePath(const PGESTRING &path)
{
    m_filePath = path;
}
long TextOutput::getCurrentLineNumber()
{
    return m_lineNumber;
}
/*****************BASE TEXT I/O CLASS***************************/


/*****************RAW TEXT I/O CLASS***************************/
RawTextInput::RawTextInput() : TextInput(), m_pos(0), m_data(nullptr), m_isEOF(true) {}

RawTextInput::RawTextInput(PGESTRING *rawString, const PGESTRING &filepath) :
    TextInput(),
    m_pos(0),
    m_data(nullptr),
    m_isEOF(true)
{
    if(!open(rawString, filepath))
        m_data = nullptr;
}

bool RawTextInput::open(PGESTRING *rawString, const PGESTRING &filepath)
{
    if(!rawString)
        return false;
    m_data = rawString;
    m_filePath = filepath;
    m_pos = 0;
    m_isEOF = IsEmpty(*m_data);
    m_lineNumber = 0;
    return true;
}

void RawTextInput::close()
{
    m_isEOF = true;
    m_data = nullptr;
    m_filePath.clear();
    m_pos = 0;
    m_lineNumber = 0;
}

PGESTRING RawTextInput::read(int64_t len)
{
    if(!m_data)
        return PGESTRING();
    if(m_isEOF)
        return PGESTRING();

    if((m_pos + len) >= static_cast<int64_t>(m_data->size()))
    {
        len = static_cast<int64_t>(m_data->size()) - m_pos;
        m_isEOF = true;
    }
    PGESTRING buf(static_cast<pge_size_t>(len + 1), '\0');
#ifdef PGE_FILES_QT
    buf = m_data->mid(static_cast<int>(m_pos), static_cast<int>(len));
#else
    buf = m_data->substr(static_cast<size_t>(m_pos), static_cast<size_t>(len));
#endif
    m_pos += len;
    return buf;
}

PGESTRING RawTextInput::readLine()
{
    if(!m_data)
        return "";
    if(m_isEOF)
        return "";
    PGESTRING buffer;
    PGEChar cur;
    do
    {
#ifdef PGE_FILES_QT
        cur = (*m_data)[static_cast<int>(m_pos++)];
#else
        cur = (*m_data)[static_cast<size_t>(m_pos++)];
#endif
        if(m_pos >= static_cast<int64_t>(m_data->size()))
        {
            m_pos = static_cast<int64_t>(m_data->size());
            m_isEOF = true;
        }
        if((cur != '\r') && (cur != '\n'))
            buffer.push_back(cur);
    }
    while((cur != '\n') && !m_isEOF);
    m_lineNumber++;
    return buffer;
}

PGESTRING RawTextInput::readCVSLine()
{
    if(!m_data) return PGESTRING();
    if(m_isEOF) return PGESTRING();
    bool quoteIsOpen = false;
    PGESTRING buffer;
    PGEChar cur;
    do
    {
#ifdef PGE_FILES_QT
        cur = (*m_data)[static_cast<int>(m_pos++)];
#else
        cur = (*m_data)[static_cast<size_t>(m_pos++)];
#endif
        if(m_pos >= static_cast<int64_t>(m_data->size()))
        {
            m_pos = static_cast<int64_t>(m_data->size());
            m_isEOF = true;
        }
        if(cur == '\"')
            quoteIsOpen = !quoteIsOpen;
        else
        {
            if((cur != '\r') && (((cur != '\n') && (cur != ',')) || (quoteIsOpen)))
                buffer.push_back(cur);
            if(cur == '\n') m_lineNumber++;
        }
    }
    while((((cur != '\n') && (cur != ',')) || quoteIsOpen) && (!m_isEOF));
    return buffer;
}

PGESTRING RawTextInput::readAll()
{
    if(!m_data) return "";
    return *m_data;
}

bool RawTextInput::eof()
{
    return m_isEOF;
}

int64_t RawTextInput::tell()
{
    return m_pos;
}

int RawTextInput::seek(int64_t pos, TextInput::positions relativeTo)
{
    if(!m_data)
        return -1;

    switch(relativeTo)
    {
    case current:
        m_pos += pos;
        break;
    case end:
        m_pos = static_cast<int64_t>(m_data->size()) + pos;
        break;
    case begin:
    default:
        m_pos = pos;
        break;
    }
    if(m_pos < 0) m_pos = 0;
    if(m_pos >= static_cast<int64_t>(m_data->size()))
    {
        m_pos = static_cast<int64_t>(m_data->size());
        m_isEOF = true;
    }
    else
        m_isEOF = false;
    return 0;
}



RawTextOutput::RawTextOutput() : TextOutput(), m_pos(0), m_data(nullptr) {}

RawTextOutput::RawTextOutput(PGESTRING *rawString, outputMode mode) : TextOutput(), m_pos(0), m_data(nullptr)
{
    if(!open(rawString, mode))
        m_data = nullptr;
}

RawTextOutput::~RawTextOutput() {}

bool RawTextOutput::open(PGESTRING *rawString, outputMode mode)
{
    if(!rawString)
        return false;

    m_data = rawString;
    m_pos = 0;
    m_lineNumber = 0;

    if(mode == truncate)
        m_data->clear();
    else if(mode == append)
        m_pos = static_cast<long long>(m_data->size());

    return true;
}

void RawTextOutput::close()
{
    m_data = nullptr;
    m_pos = 0;
    m_lineNumber = 0;
}

int RawTextOutput::write(PGESTRING buffer)
{
    if(!m_data) return -1;
    int64_t written = 0;
fillEnd:
    if(m_pos >= static_cast<int64_t>(m_data->size()))
    {
        auto oldSize = static_cast<int64_t>(m_data->size());
        m_data->append(buffer);
        m_pos = static_cast<int64_t>(m_data->size());
        written += (static_cast<int64_t>(m_data->size()) - oldSize);
    }
    else
    {
        while((m_pos < static_cast<int64_t>(m_data->size())) && (!IsEmpty(buffer)))
        {
            (*m_data)[(pge_size_t)m_pos++] = buffer[0];
            written++;
            PGE_RemStrRng(buffer, 0, 1);
        }

        if(!IsEmpty(buffer))
            goto fillEnd;
    }
    return static_cast<int>(written);
}

int64_t RawTextOutput::tell()
{
    return m_pos;
}

int RawTextOutput::seek(int64_t pos, TextOutput::positions relativeTo)
{
    if(!m_data)
        return -1;

    switch(relativeTo)
    {
    case current:
        m_pos += pos;
        break;
    case end:
        m_pos = static_cast<int64_t>(m_data->size()) + pos;
        break;
    case begin:
    default:
        m_pos = pos;
        break;
    }

    if(m_pos < 0)
    {
        m_pos = 0;
        return -1;
    }

    if(m_pos >= static_cast<signed>(m_data->size()))
        m_pos = static_cast<long long>(m_data->size());

    return 0;
}

TextOutput &TextOutput::operator<<(const PGESTRING &s)
{
    this->write(s);
    return *this;
}

TextOutput &TextOutput::operator <<(const char *s)
{
    this->write(s);
    return *this;
}
/*****************RAW TEXT I/O CLASS***************************/



/*****************FILE TEXT I/O CLASS***************************/

TextFileInput::TextFileInput() :
    TextInput()
#ifndef PGE_FILES_QT
    , stream(nullptr)
#endif
{}

TextFileInput::TextFileInput(const PGESTRING &filePath, bool utf8) :
    TextInput()
#ifndef PGE_FILES_QT
    , stream(nullptr)
#endif
{
    if(!open(filePath, utf8))
    {
#ifndef PGE_FILES_QT
        stream = nullptr;
#else
        file.close();
#endif
    }
}

TextFileInput::~TextFileInput()
{
    close();
}

bool TextFileInput::open(const PGESTRING &filePath, bool utf8)
{
    m_filePath = filePath;
    m_lineNumber = 0;
#ifdef PGE_FILES_QT
    bool state = false;
    file.setFileName(filePath);
    m_utf8 = utf8;
    state = file.open(QIODevice::ReadOnly | QIODevice::Text);
    if(!state) return false;
    stream.setDevice(&file);

#   if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if(utf8)
        stream.setEncoding(QStringConverter::Utf8);
    else
    {
        stream.setAutoDetectUnicode(true);
        stream.setLocale(QLocale::system());
        stream.setEncoding(QStringConverter::System);
    }
#   else
    if(utf8)
        stream.setCodec("UTF-8");
    else
    {
        stream.setAutoDetectUnicode(true);
        stream.setLocale(QLocale::system());
        stream.setCodec(QTextCodec::codecForLocale());
    }
#   endif

    return true;
#else
    (void)utf8;
    stream = utf8_fopen(filePath.c_str(), "rb");
    return (stream != nullptr);
#endif
}

bool TextFileInput::reOpen(bool utf8)
{
    PGESTRING fpath = m_filePath;
    close();
    return open(fpath, utf8);
}

void TextFileInput::close()
{
    m_filePath.clear();
    m_lineNumber = 0;
#ifdef PGE_FILES_QT
    file.close();
#else
    if(stream)
        fclose(stream);
    stream = nullptr;
#endif
}

PGESTRING TextFileInput::read(int64_t len)
{
#ifdef PGE_FILES_QT
    if(!file.isOpen()) return "";
    char *buf = new char[static_cast<size_t>(len + 1)];
    buf[0] = '\0';
    int gotten = static_cast<int>(file.read(buf, static_cast<int>(len)));
    if(gotten >= 0)
        buf[gotten] = '\0';
    QString out = m_utf8 ? QString::fromUtf8(buf, gotten) : QString::fromLocal8Bit(buf, gotten);
    delete[] buf;
    return out;//stream.read(len);
#else
    if(!stream)
        return "";
    std::string buf(static_cast<size_t>(len + 1), '\0');
    size_t lenR = fread(&buf[0], 1, static_cast<size_t>(len), stream);
    (void)lenR;
    return buf;
#endif
}

PGESTRING TextFileInput::readLine()
{
#ifdef PGE_FILES_QT
    if(!file.isOpen()) return "";
    return stream.readLine();
#else
    if(!stream)
        return "";

    std::string out;
    out.reserve(1024);
    int C = 0;
    do
    {
        C = fgetc(stream);
        if((C != '\n') && (C != '\r') && (C != EOF))
            out.push_back(static_cast<char>(C));
    }
    while((C != '\n') && (C != EOF));

    if(out.size() == 0)
        return "";

    out.shrink_to_fit();

    m_lineNumber++;
    return out;
#endif
}

PGESTRING TextFileInput::readCVSLine()
{
    bool quoteIsOpen = false;
    std::string buffer;
#ifdef PGE_FILES_QT
    char cur = 0;
    if(!file.isOpen())
        return "";

    do
    {
        file.getChar(&cur);
        if(cur == '\"')
            quoteIsOpen = !quoteIsOpen;
        else
        {
            if((cur != '\r') && (((cur != '\n') && (cur != ',')) || (quoteIsOpen)))
                buffer.push_back(cur);
            if(cur == '\n') m_lineNumber++;
        }
    }
    while((((cur != '\n') && (cur != ',')) || quoteIsOpen)
          && !file.atEnd());
    return m_utf8 ?
           QString::fromStdString(buffer) :
           QString::fromLocal8Bit(buffer.c_str(), static_cast<int>(buffer.size()));
#else
    buffer.reserve(1024);
    if(!stream)
        return "";

    int  gc;
    char cur;
    if(!feof(stream))
        do
        {
            gc = fgetc(stream);
            if(gc == EOF)
                break;
            cur = static_cast<char>(gc);
            if(cur == '\"')
                quoteIsOpen = !quoteIsOpen;
            else
            {
                if((cur != '\r') && (((cur != '\n') && (cur != ',')) || (quoteIsOpen)))
                    buffer.push_back(cur);
                if(cur == '\n')
                    m_lineNumber++;
            }
        }
        while((((cur != '\n') && (cur != ',')) || quoteIsOpen));
    buffer.shrink_to_fit();
    return buffer;
#endif
}

PGESTRING TextFileInput::readAll()
{
#ifdef PGE_FILES_QT
    return stream.readAll();
#else
    if(!stream)
        return PGESTRING();
    std::string out;
    out.reserve(10240);
    fseek(stream, 0, SEEK_SET);
    int x = 0;
    do
    {
        x = fgetc(stream);
        if((x != '\r') && (x != EOF))
            out.push_back(static_cast<char>(x));
    }
    while(!feof(stream));
    return out;
#endif
}

bool TextFileInput::eof()
{
#ifdef PGE_FILES_QT
    return stream.atEnd();
#else
    return (feof(stream) != 0);
#endif
}

int64_t TextFileInput::tell()
{
#ifdef PGE_FILES_QT
    return static_cast<int64_t>(file.pos());
#else
    return static_cast<int64_t>(ftell(stream));
#endif
}

int TextFileInput::seek(int64_t pos, TextFileInput::positions relativeTo)
{
#ifdef PGE_FILES_QT
    (void)relativeTo;
    switch(relativeTo)
    {
    case current:
        file.seek(file.pos() + pos);
        break;
    case begin:
        file.seek(pos);
        stream.seek(pos);
        break;
    case end:
        file.seek(file.size() - pos);
        break;
    default:
        file.seek(pos);
        stream.seek(pos);
        break;
    }
    return 0;
#else
    int s = 0;
    switch(relativeTo)
    {
    case current:
        s = SEEK_CUR;
        break;
    case begin:
        s = SEEK_SET;
        break;
    case end:
        s = SEEK_END;
        break;
    default:
        s = SEEK_SET;
        break;
    }
    return fseek(stream, static_cast<long>(pos), static_cast<int>(s));
#endif
}



TextFileOutput::TextFileOutput() : TextOutput(), m_forceCRLF(false)
{
#ifndef PGE_FILES_QT
    stream = nullptr;
#endif
}

TextFileOutput::TextFileOutput(PGESTRING filePath, bool utf8, bool forceCRLF, TextOutput::outputMode mode) : TextOutput()
{
    if(!open(filePath, utf8, forceCRLF, mode))
    {
#ifndef PGE_FILES_QT
        stream = nullptr;
#else
        file.flush();
        file.close();
#endif
    }
}

TextFileOutput::~TextFileOutput()
{
    close();
}

bool TextFileOutput::open(PGESTRING filePath, bool utf8, bool forceCRLF, TextOutput::outputMode mode)
{
    m_forceCRLF = forceCRLF;
    m_filePath = filePath;
    m_lineNumber = 0;
#ifdef PGE_FILES_QT
    bool state = false;
    file.setFileName(filePath);
    m_utf8 = utf8;
    if(mode == truncate)
        state = file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    else if(mode == append)
        state = file.open(QIODevice::WriteOnly | QIODevice::Append);
    else
        state = file.open(QIODevice::WriteOnly);
    if(!state)
        return false;
    if(!m_forceCRLF)
    {
        stream.setDevice(&file);

#   if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        if(utf8)
            stream.setEncoding(QStringConverter::Utf8);
        else
        {
            stream.setAutoDetectUnicode(true);
            stream.setLocale(QLocale::system());
            stream.setEncoding(QStringConverter::System);
        }
#   else
        if(utf8)
            stream.setCodec("UTF-8");
        else
        {
            stream.setAutoDetectUnicode(true);
            stream.setLocale(QLocale::system());
            stream.setCodec(QTextCodec::codecForLocale());
        }
#   endif

    }
    return true;
#else
    (void)utf8;
    const char *tmode = nullptr;
    if(mode == truncate)
        tmode = "wb";
    else if(mode == append)
        tmode = "ab";
    else
        tmode = "wb";
    stream = utf8_fopen(filePath.c_str(), tmode);
    return (stream != nullptr);
#endif
}

void TextFileOutput::close()
{
    m_filePath.clear();
    m_lineNumber = 0;
#ifdef PGE_FILES_QT
    file.flush();
    file.close();
#else
    if(stream)
        fclose(stream);
    stream = nullptr;
#endif
}

int TextFileOutput::write(PGESTRING buffer)
{
    pge_size_t writtenBytes = 0;
    if(m_forceCRLF)
    {
#ifdef PGE_FILES_QT
        buffer.replace("\n", "\r\n");
        writtenBytes = static_cast<pge_size_t>(file.write(m_utf8 ? buffer.toUtf8() : buffer.toLocal8Bit()));
#else
        for(pge_size_t i = 0; i < buffer.size(); i++)
        {
            if(buffer[i] == '\n')
            {
                //Force writing CRLF to prevent fakse damage of file on SMBX in Windows
                static const char bytes[2] = {0x0D, 0x0A};
                size_t bytesNum = fwrite(&bytes, 1, 2, stream);
                if(bytesNum == 0)
                    return -1;
                writtenBytes += bytesNum;
            }
            else
            {
                pge_size_t bytesNum = 1;
                fputc(buffer[i], stream);
                writtenBytes += bytesNum;
            }
        }
#endif
    }
    else
    {
        writtenBytes = static_cast<pge_size_t>(buffer.size());
#ifdef PGE_FILES_QT
        stream << buffer;
#else
        fwrite(buffer.c_str(), 1, buffer.size(), stream);
#endif
    }
    return static_cast<int>(writtenBytes);
}

int64_t TextFileOutput::tell()
{
#ifdef PGE_FILES_QT
    if(!m_forceCRLF)
        return static_cast<int64_t>(stream.pos());
    else
        return static_cast<int64_t>(file.pos());
#else
    return ftell(stream);
#endif
}

int TextFileOutput::seek(int64_t pos, TextOutput::positions relativeTo)
{
#ifdef PGE_FILES_QT
    (void)relativeTo;
    if(!m_forceCRLF)
        stream.seek(pos);
    else
        file.seek(pos);
    return 0;
#else
    int s;
    switch(relativeTo)
    {
    case current:
        s = SEEK_CUR;
        break;
    case begin:
        s = SEEK_SET;
        break;
    case end:
        s = SEEK_END;
        break;
    default:
        s = SEEK_SET;
        break;
    }
    return fseek(stream, static_cast<long>(pos), static_cast<int>(s));
#endif
}

/*****************FILE TEXT I/O CLASS***************************/



FileInfo::FileInfo() = default;

FileInfo::FileInfo(const PGESTRING &filepath)
{
    setFile(filepath);
}

void FileInfo::setFile(const PGESTRING &filepath)
{
    m_filePath = filepath;
    rebuildData();
}

PGESTRING FileInfo::suffix()
{
    return m_suffix;
}

PGESTRING FileInfo::filename()
{
    return m_fileName;
}

PGESTRING FileInfo::fullPath()
{
    return m_filePath;
}

PGESTRING FileInfo::basename()
{
    return m_baseName;
}

PGESTRING FileInfo::dirpath()
{
    return m_dirPath;
}

void FileInfo::rebuildData()
{
#ifdef _WIN32
#   define PATH_MAXLEN MAX_PATH
#else
#   define PATH_MAXLEN PATH_MAX
#endif
    int i;
    m_suffix.clear();
    m_fileName.clear();
    m_dirPath.clear();
    m_baseName.clear();

    //Take full path

#ifdef PGE_FILES_QT
    m_filePath = QFileInfo(m_filePath).absoluteFilePath();
#elif defined(PGE_NO_REALPATH) || defined(PGE_MIN_PORT) || defined(__3DS__) || defined(VITA) || defined(__SWITCH__) || defined(__WII__) || defined(__WIIU__)
    // all paths are absolute on minimal target platforms
#elif !defined(_WIN32)
    char *rez = nullptr;
    char buf[PATH_MAXLEN + 1];
    rez = realpath(m_filePath.c_str(), buf);
    if(rez)
        m_filePath = buf;
#else
    wchar_t bufW[MAX_PATH + 1];
    DWORD ret = 0;
    ret = GetFullPathNameW(Str2WStr(m_filePath).c_str(), MAX_PATH, bufW, nullptr);
    if(ret != 0)
        m_filePath = WStr2Str(bufW);
    std::replace(m_filePath.begin(), m_filePath.end(), '\\', '/');
#endif

    //Read directory path
    i = static_cast<int>(m_filePath.size() - 1);
    for(; i >= 0; i--)
    {
        if((m_filePath[static_cast<pge_size_t>(i)] == '/') || (m_filePath[static_cast<pge_size_t>(i)] == '\\'))
            break;
    }

    if(i >= 0)
    {
#ifndef PGE_FILES_QT
        m_dirPath = m_filePath.substr(0, static_cast<pge_size_t>(i));
#else
        m_dirPath = m_filePath.left(i);
#endif
    }

    //Take file suffix
    i = static_cast<int>(m_filePath.size()) - 1;
    for(; i > 0; i--)
    {
        if(m_filePath[static_cast<pge_size_t>(i)] == '.')
            break;
    }

    if(i > 0)
        i++;
    if(i == (static_cast<int>(m_filePath.size()) - 1))
        goto skipSuffix;
    for(; i < static_cast<int>(m_filePath.size()); i++)
    {
        m_suffix.push_back(
            static_cast<char>(
                tolower(
                    PGEGetChar(m_filePath[static_cast<pge_size_t>(i)])
                )
            )
        );
    }
skipSuffix:

    //Take file name without path
    i = static_cast<int>(m_filePath.size()) - 1;
    for(; i >= 0; i--)
    {
        if((m_filePath[static_cast<pge_size_t>(i)] == '/') || (m_filePath[static_cast<pge_size_t>(i)] == '\\')) break;
    }
    if(i > 0)
        i++;

    if(i >= (static_cast<int>(m_filePath.size()) - 1))
        goto skipFilename;
    for(; i < static_cast<int>(m_filePath.size()); i++)
        m_fileName.push_back(m_filePath[static_cast<pge_size_t>(i)]);

skipFilename:
    //Take base file name
    m_baseName = m_fileName;
    for(i = static_cast<int>(m_baseName.size()) - 1; i > 0; i--)
    {
        if(m_baseName[static_cast<pge_size_t>(i)] == '.')
        {
            m_baseName.resize(static_cast<pge_size_t>(i));
            break;
        }
    }
}

} /* NameSpace */
