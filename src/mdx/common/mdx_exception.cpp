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

#include <cstring>
#include <cstdio>

#include "mdx/common/mdx_exception.h"

const char* MDX_exception::what() const noexcept
{
    return "MDX_exception";
}

const char* MDX_parse_error::what() const noexcept
{
    return "MDX_parse_error";
}

// misc error for MDX parsing
const char* MDX_parse_error_misc::what() const noexcept
{
    return m_message;
}

// primitive error denoting that a term is malformed
const char* MDX_bad_term::what() const noexcept
{
    return m_message;
}

// primitive error denoting that a delimiter is missing
MDX_missing_delimiter::MDX_missing_delimiter(char expected) noexcept
{
    static const char prefix[] = "Missing ";
    static_assert(sizeof(m_description) == sizeof(prefix) + 1, "Description size inconsistent with prefix size");

    memcpy(m_description, prefix, sizeof(prefix) - 1);
    m_description[sizeof(prefix) - 1] = expected;
    m_description[sizeof(prefix)] = '\0';
}

const char* MDX_missing_delimiter::what() const noexcept
{
    return m_description;
}

// derived error denoting an unexpected character
MDX_unexpected_character::MDX_unexpected_character(char unexpected) noexcept
{
    static const char prefix[] = "Unexpected ";
    static_assert(sizeof(m_description) == sizeof(prefix) + 1, "Description size inconsistent with prefix size");

    memcpy(m_description, prefix, sizeof(prefix) - 1);
    m_description[sizeof(prefix) - 1] = unexpected;
    m_description[sizeof(prefix)] = '\0';
}

const char* MDX_unexpected_character::what() const noexcept
{
    return m_description;
}

// derived error indicating which field caused a parse failure
MDX_bad_field::MDX_bad_field(const char* field_name) noexcept
{
    static const char prefix[] = "Bad field ";
    memcpy(m_description, prefix, sizeof(prefix) - 1);
    strncpy(m_description + sizeof(prefix) - 1, field_name, sizeof(m_description) - sizeof(prefix));
    m_description[sizeof(m_description) - 1] = '\0';
}

MDX_bad_field::MDX_bad_field(const char* field_name, size_t len) noexcept
{
    static const char prefix[] = "Bad field ";
    memcpy(m_description, prefix, sizeof(prefix) - 1);
    size_t buff_left = sizeof(m_description) - sizeof(prefix);
    if(buff_left > len)
        buff_left = len;
    strncpy(m_description + sizeof(prefix) - 1, field_name, buff_left);
    m_description[sizeof(m_description) - 1] = '\0';
}

const char* MDX_bad_field::what() const noexcept
{
    return m_description;
}

// derived error indicating which array item caused a parse failure

MDX_bad_array::MDX_bad_array(size_t index) noexcept
{
    static const char prefix[] = "Bad item ";
    memcpy(m_description, prefix, sizeof(prefix) - 1);
    snprintf(m_description + sizeof(prefix) - 1, sizeof(m_description) - (sizeof(prefix) - 1), "%lu", (long unsigned)index);
}

const char* MDX_bad_array::what() const noexcept
{
    return m_description;
}
