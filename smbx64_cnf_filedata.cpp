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

#include "smbx64_cnf_filedata.h"

SMBX64_ConfigPlayer::SMBX64_ConfigPlayer()
{
    id = 0;
    controllerType = 0;
    k_up = 0;
    k_down = 0;
    k_left = 0;
    k_right = 0;
    k_run = 0;
    k_jump = 0;
    k_drop = 0;
    k_pause = 0;
    k_altjump = 0; //>=19
    k_altrun = 0; //>=19
    j_run = 0;
    j_jump = 0;
    j_drop = 0;
    j_pause = 0;
    j_altjump = 0; //>=19
    j_altrun = 0; //>=19
}


SMBX64_ConfigFile::SMBX64_ConfigFile()
{
    fullScreen = false;
}
