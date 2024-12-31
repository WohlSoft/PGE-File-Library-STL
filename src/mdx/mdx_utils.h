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
 *  \file mdx_utils.h
 *  \brief Contains save and load utility functions
 */

#pragma once
#ifndef MDX_UTILS_H
#define MDX_UTILS_H

const char* MDX_load_int(int& dest, const char* field_data);
const char* MDX_load_long(long& dest, const char* field_data);
const char* MDX_load_longlong(long long& dest, const char* field_data);

const char* MDX_load_uint(int& dest, const char* field_data);
const char* MDX_load_ulong(long& dest, const char* field_data);
const char* MDX_load_ulonglong(long long& dest, const char* field_data);

const char* MDX_load_uint(unsigned int& dest, const char* field_data);
const char* MDX_load_ulong(unsigned long& dest, const char* field_data);
const char* MDX_load_ulonglong(unsigned long long& dest, const char* field_data);

#endif // #ifndef MDX_UTILS_H
