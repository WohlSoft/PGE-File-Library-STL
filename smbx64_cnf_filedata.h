/*
 * PGE File Library - a library to process file formats, part of Moondust project
 *
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

/*! \file smbx64_cnf_filedata.h
 *  \brief Contains definition of SMBX Engine settings data structure
 */

#pragma once
#ifndef SMBX64_CNF_FILEDATA_H
#define SMBX64_CNF_FILEDATA_H

#include "pge_file_lib_globs.h"
#include "meta_filedata.h"

struct SMBX64_ConfigPlayer
{
    SMBX64_ConfigPlayer();
    unsigned int id;
    unsigned int controllerType;
    unsigned int k_up;
    unsigned int k_down;
    unsigned int k_left;
    unsigned int k_right;
    unsigned int k_run;
    unsigned int k_jump;
    unsigned int k_drop;
    unsigned int k_pause;
    unsigned int k_altjump;//>=19
    unsigned int k_altrun;//>=19
    unsigned int j_run;
    unsigned int j_jump;
    unsigned int j_drop;
    unsigned int j_pause;
    unsigned int j_altjump;//>=19
    unsigned int j_altrun;//>=19
};

struct SMBX64_ConfigFile
{
    SMBX64_ConfigFile();

    //!Helper meta-data
    FileFormatMeta meta;

    bool fullScreen;//>=16
    PGELIST<SMBX64_ConfigPlayer> players;
};

#endif // SMBX64_CNF_FILEDATA_H
