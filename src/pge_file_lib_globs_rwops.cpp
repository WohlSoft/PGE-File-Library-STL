/*
 * PGE File Library - a library to process file formats, part of Moondust project
 *
 * Copyright (c) 2014-2024 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <SDL2/SDL_rwops.h>

namespace PGE_FileFormats_misc
{

/*****************SDL_RWOPS TEXT I/O CLASS***************************/

RWopsTextInput::RWopsTextInput() : TextInput()
{}

RWopsTextInput::RWopsTextInput(SDL_RWops *rwops, const PGESTRING &filePath) : TextInput()
{
    open(rwops, filePath);
}

RWopsTextInput::~RWopsTextInput()
{
    close();
}

bool RWopsTextInput::open(SDL_RWops *rwops, const PGESTRING &filePath)
{
    m_filePath = filePath;
    m_lineNumber = 0;
    m_rwops = rwops;

    m_buffer.clear();
    m_bufferStartOffset = 0;
    m_readOffset = 0;
    m_bufferIsEof = false;

    return (m_rwops != nullptr);
}

void RWopsTextInput::close()
{
    m_filePath.clear();
    m_lineNumber = 0;
    if(m_rwops)
        SDL_RWclose(m_rwops);
    m_rwops = nullptr;

    m_buffer.clear();
    m_bufferStartOffset = 0;
    m_readOffset = 0;
    m_bufferIsEof = false;
}

void RWopsTextInput::fillBuffer()
{
    if(in_buffer() && m_bufferIsEof)
        return;

    if(!m_rwops)
        return;

    // seek to requested location
    if(m_readOffset != m_rwopsOffset)
        m_rwopsOffset = SDL_RWseek(m_rwops, m_readOffset, RW_SEEK_SET);

    // failed to seek, mark as EOF
    if(m_readOffset != m_rwopsOffset)
    {
        m_bufferIsEof = true;
        m_buffer.clear();
        return;
    }

    // try to fill buffer
    m_bufferIsEof = false;
    m_bufferStartOffset = m_rwopsOffset;
    m_buffer.resize(2048);

    char * const begin = &m_buffer[0];
    char * const end = begin + m_buffer.size();
    char *dest = begin;

    while(dest < end)
    {
        size_t bytes_got = SDL_RWread(m_rwops, dest, 1, end - dest);
        dest += bytes_got;
        m_rwopsOffset += bytes_got;

        if(bytes_got == 0)
        {
            m_buffer.resize(dest - begin);
            m_bufferIsEof = true;
            return;
        }
    }
}

#ifndef PGE_FILES_QT
void RWopsTextInput::read(PGESTRING &buf, int64_t len)
#else
void RWopsTextInput::read(PGESTRING &buf_utf16, int64_t len)
#endif
{
#ifndef PGE_FILES_QT
    buf.clear();
#else
    buf_utf16.clear();
    std::string buf;
#endif

    if(!m_rwops)
        return;

    buf.resize(static_cast<size_t>(len));
    char * const begin = &buf[0];
    char * const end = begin + len;
    char *dest = begin;

    while(dest < end)
    {
        long to_copy = bytes_available();
        if(to_copy <= 0 || !in_buffer())
        {
            fillBuffer();
            to_copy = bytes_available();
        }

        if(to_copy > 0)
        {
            if(end - dest < to_copy)
                to_copy = end - dest;

            memcpy(dest, &m_buffer[m_readOffset - m_bufferStartOffset], to_copy);
            m_readOffset += to_copy;
            dest += to_copy;
        }
        // couldn't load entire requested region
        else
        {
            buf.resize(dest - begin);

#ifdef PGE_FILES_QT
            buf_utf16 = QString::fromStdString(buf);
#endif

            return;
        }
    }

#ifdef PGE_FILES_QT
    buf_utf16 = QString::fromStdString(buf);
#endif

    return;
}

#ifndef PGE_FILES_QT
void RWopsTextInput::readLine(PGESTRING &out)
#else
void RWopsTextInput::readLine(PGESTRING &out_utf16)
#endif
{
#ifndef PGE_FILES_QT
    out.clear();
#else
    out_utf16.clear();
    std::string out;
#endif

    if(!m_rwops)
        return;

    while(true)
    {
        long to_copy = bytes_available();
        if(to_copy <= 0 || !in_buffer())
        {
            fillBuffer();
            to_copy = bytes_available();
        }

        if(to_copy <= 0)
        {
            // EOF
#ifdef PGE_FILES_QT
            out_utf16 = QString::fromStdString(out);
#endif
            return;
        }

        const char * const begin = &m_buffer[0] + (m_readOffset - m_bufferStartOffset);
        const char * const end = &m_buffer[0] + m_buffer.size();

        const char * append_begin = begin;

        for(const char *byte = begin; byte != end; byte++)
        {
            if(*byte != '\r' && *byte != '\n')
                continue;

            out.append(append_begin, byte - append_begin);

            if(*byte == '\r')
            {
                append_begin = byte + 1;
                continue;
            }

            // newline
            m_lineNumber++;
            m_readOffset += (byte + 1) - begin;

#ifdef PGE_FILES_QT
            out_utf16 = QString::fromStdString(out);
#endif

            return;
        }

        out.append(append_begin, end - append_begin);
        m_readOffset += end - begin;
    }
}

#ifndef PGE_FILES_QT
void RWopsTextInput::readCVSLine(PGESTRING &out)
#else
void RWopsTextInput::readCVSLine(PGESTRING &out_utf16)
#endif
{
#ifndef PGE_FILES_QT
    out.clear();
#else
    out_utf16.clear();
    std::string out;
#endif

    if(!m_rwops)
        return;

    bool quoteIsOpen = false;

    while(true)
    {
        long to_copy = bytes_available();
        if(to_copy <= 0 || !in_buffer())
        {
            fillBuffer();
            to_copy = bytes_available();
        }

        if(to_copy <= 0)
        {
            // EOF
#ifdef PGE_FILES_QT
            out_utf16 = QString::fromStdString(out);
#endif
            return;
        }

        const char * const begin = &m_buffer[0] + (m_readOffset - m_bufferStartOffset);
        const char * const end = &m_buffer[0] + m_buffer.size();

        for(const char *byte = begin; byte != end; byte++)
        {
            char cur = static_cast<char>(*byte);
            if(cur == '\"')
                quoteIsOpen = !quoteIsOpen;
            else
            {
                if((cur != '\r') && (((cur != '\n') && (cur != ',')) || (quoteIsOpen)))
                    out.push_back(cur);
                if(cur == '\n')
                    m_lineNumber++;
            }

            if((((cur != '\n') && (cur != ',')) || quoteIsOpen))
                continue;

            m_readOffset += (byte + 1) - begin;

#ifdef PGE_FILES_QT
            out_utf16 = QString::fromStdString(out);
#endif

            return;
        }

        m_readOffset += end - begin;
    }
}

PGESTRING RWopsTextInput::readAll()
{
    if(!m_rwops)
        return "";

    seek(0, RWopsTextInput::positions::begin);

    std::string out;
    out.reserve(10240);

    while(true)
    {
        if(!buffer_okay())
            fillBuffer();

        if(bytes_available() <= 0)
        {
            // EOF
#ifndef PGE_FILES_QT
            return out;
#else
            return QString::fromStdString(out);
#endif
        }

        const char * const begin = &m_buffer[0] + (m_readOffset - m_bufferStartOffset);
        const char * const end = &m_buffer[0] + m_buffer.size();

        for(const char *byte = begin; byte != end; byte++)
        {
            char cur = static_cast<char>(*byte);
            if(cur != '\r')
                out.push_back(cur);
        }

        m_readOffset += end - begin;
    }
}

bool RWopsTextInput::eof()
{
    if(!m_rwops)
        return true;

    return (m_bufferIsEof) && (m_readOffset >= m_bufferStartOffset + (int64_t)m_buffer.size());
}

int64_t RWopsTextInput::tell()
{
    return m_readOffset;
}

int RWopsTextInput::seek(int64_t pos, RWopsTextInput::positions relativeTo)
{
    if(!m_rwops)
        return -1;

    int s = 0;
    switch(relativeTo)
    {
    case current:
        s = RW_SEEK_CUR;
        break;
    case begin:
        s = RW_SEEK_SET;
        break;
    case end:
        s = RW_SEEK_END;
        break;
    default:
        s = RW_SEEK_SET;
        break;
    }

    return m_readOffset = m_rwopsOffset = SDL_RWseek(m_rwops, pos, s);
}

/*****************SDL_RWOPS TEXT I/O CLASS***************************/

}
