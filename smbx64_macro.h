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
#ifndef SMBX64_MACRO_H
#define SMBX64_MACRO_H

/*! \file smbx64_macro.h
 *  \brief Contains helper macroses for making SMBX64 Standard format based parsers
 */

/*
  Small set of macroses used to organize SMBX64 specific file read functions.
*/

//(you must create and open PGE_FileFormats_misc::TextInput &in; !!!)
#define SMBX64_FileBegin() unsigned int file_format = 0;   /*File format number*/\
                           PGESTRING line                  /*Current Line data*/

//Jump to next line
#define nextLine() line = in.readCVSLine()

//Version comparison
#define ge(v) file_format>=(v)
#define gt(v) file_format>(v)
#define le(v) file_format<=(v)
#define lt(v) file_format<(v)

#endif // SMBX64_MACRO_H
