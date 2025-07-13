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

/*!
 *  \file pge_base_callbacks.h
 *  \brief Contains base structures used for PGE-FL's callbacks
 */

#pragma once
#ifndef PGE_BASE_CALLBACKS_H
#define PGE_BASE_CALLBACKS_H

#include <cstddef>
#include <string>
#include <exception>

#ifdef PGE_FILES_QT
typedef int     pge_size_t;
#else
typedef size_t  pge_size_t;
#endif

struct FileFormatsError;

namespace PGE_FileFormats_misc
{

// error that callbacks may raise when unpacking object
class callback_error : public std::exception
{
    const std::string m_messageStr;
    const char* const m_message = nullptr;
public:
    callback_error(const char* message) noexcept : m_message(message) {}
    callback_error(const std::string &message) noexcept : m_messageStr(message), m_message(m_messageStr.c_str()) {}
    virtual const char* what() const noexcept
    {
        return m_message;
    }
};

// way that callbacks may fully terminate parse process without any error
class callback_interrupt : public std::exception {};


struct LoadCallbacks
{
    template<class obj_t> using callback = bool (*)(void* userdata, obj_t& obj);
    using err_callback = void (*)(void* userdata, FileFormatsError& err);

    void* userdata = nullptr;
    err_callback on_error = nullptr;
};

struct SaveCallbacks
{
    template<class obj_t> using callback = bool (*)(const void* userdata, obj_t& obj, pge_size_t index);
    using err_callback = void (*)(void* err_userdata, FileFormatsError& err);

    const void* userdata = nullptr;
    void* err_userdata = nullptr;
    err_callback on_error = nullptr;
};

}

#include "meta_filedata.h" // def'n of FileFormatsError

#endif // #ifndef PGE_BASE_CALLBACKS_H
