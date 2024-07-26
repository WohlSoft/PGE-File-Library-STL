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

/*! \file mdx_exception.h
 *
 *  \brief Defines a set of exceptions used to interrupt parsing in MDX
 *
 */

#pragma once
#ifndef MDX_EXCEPTION_HPP
#define MDX_EXCEPTION_HPP

#include <stdexcept>

class MDX_exception : public std::exception
{
    virtual const char* what() const noexcept;
};

// base error for MDX parsing
class MDX_parse_error : public MDX_exception
{
    virtual const char* what() const noexcept;
};

// misc error for MDX parsing
class MDX_parse_error_misc : public MDX_parse_error
{
    const char* const m_message = nullptr;
public:
    MDX_parse_error_misc(const char* message) noexcept : m_message(message) {}
    virtual const char* what() const noexcept;
};

// derived error denoting that a term is malformed
class MDX_bad_term : public MDX_parse_error
{
    const char* const m_message = nullptr;
public:
    MDX_bad_term(const char* message) noexcept : m_message(message) {}
    virtual const char* what() const noexcept;
};

// derived error denoting that a delimiter is missing
class MDX_missing_delimiter : public MDX_parse_error
{
    char m_description[10];
public:
    MDX_missing_delimiter(char expected) noexcept;
    virtual const char* what() const noexcept;
};

// derived error denoting an unexpected character
class MDX_unexpected_character : public MDX_parse_error
{
    char m_description[13];
public:
    MDX_unexpected_character(char unexpected) noexcept;
    virtual const char* what() const noexcept;
};

// derived error indicating which field caused a parse failure
class MDX_bad_field : public MDX_parse_error
{
    char m_description[16];
public:
    MDX_bad_field(const char* field_name) noexcept;
    MDX_bad_field(const char* field_name, size_t len) noexcept;
    virtual const char* what() const noexcept;
};

// derived error indicating which array item caused a parse failure
class MDX_bad_array : public MDX_parse_error
{
    char m_description[16];
public:
    MDX_bad_array(size_t index) noexcept;
    virtual const char* what() const noexcept;
};

#endif // #ifndef MDX_EXCEPTION_HPP
