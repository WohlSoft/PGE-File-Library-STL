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

/*!
 * \file meta_filedata.h
 * \brief Contains defition of the common meta-data structures and classes
 */
#pragma once
#ifndef META_FILEDATA_H
#define META_FILEDATA_H

#include "pge_file_lib_globs.h"
#include <memory>

/**
 * @brief Common data structure meta-data
 */
struct FileFormatMeta
{
    FileFormatMeta():
        ReadFileValid(true),
        ERROR_linenum(-1),
        RecentFormat(0),
        RecentFormatVersion(0),
        modified(true),
        untitled(true),
        smbx64strict(false)
    {}
    //! Is file parsed correctly, false if some error is occouped
    bool ReadFileValid;
    //! Error messsage
    PGESTRING ERROR_info;
    //! Line data where error was occouped
    PGESTRING ERROR_linedata;
    //! Number of line where error was occouped
    long      ERROR_linenum;
    //! Recently used (open or save) file format
    int RecentFormat;
    //! Recently used format version (for SMBX1...64 files only)
    unsigned int RecentFormatVersion;
    //! Is file was modified since open?
    bool modified;
    //! Is this level made from scratch and was not saved into file?
    bool untitled;
    //! Enable SMBX64 Standard restrictions
    //! (disable unsupported features and warn about limit exiting)
    bool smbx64strict;
    //! Recent file name since file was opened
    PGESTRING filename;
    //! Recent file path where file was located since it was opened
    PGESTRING path;
    //! A config pack identify string.
    PGESTRING configPackId;
    // Missmatching of a config pack identify string with a same string at
    // the currently loaded config pack will lead a warning of possible
    // incompatibility.
};

/**
 * @brief Common element meta-data
 */
struct ElementMeta
{
    ElementMeta() :
        array_id(0),
        index(0),
        userdata(nullptr) {}
    //! Array-ID is an unique key value identificates each unique element object.
    //! Re-counts on each file reloading
    unsigned int array_id;
    //! Recent array index where element was saved (used to speed-up settings synchronization)
    unsigned int index;
    //! JSON-like string with a custom properties (without master brackets, like "param":"value,["subparam":value])
    PGESTRING custom_params;
    //! User data pointer, Useful in the editors to have direct pointer to pre-placed elements
    void *userdata;
};

/*!
 * \brief Position bookmark entry structure
 */
struct Bookmark
{
    PGESTRING bookmarkName; //!< Name of bookmark
    double x;               //!< Bookmarked X position of the camera
    double y;               //!< Bookmarked Y position of the camera
};

/*!
 * \brief Contains backup of helpful techincal data used by PGE Editor
 */
class CrashData
{
public:
    /*!
     * \brief Constructor
     */
    explicit CrashData();
    /*!
     * \brief Copy constructor
     * \param _cd another CrashData object
     */
    CrashData(const CrashData &_cd);
    /*!
     * \brief Sets default preferences
     */
    void reset();
    //! Is crash data was used by editor (if false, LVLX writer will not record crash data into file)
    bool used;
    //! Is this level was untitled since crash occopued?
    bool untitled;
    //! Is this level was modified before crash occouped?
    bool modifyed;
    //! Turn on strict SMBX64 mode
    bool strictModeSMBX64;
    //! Recent file format ID (specific enum in the format structure declaration)
    int fmtID;
    //! Recent file format version
    unsigned int fmtVer;
    //! Full original file path file which was opened before crash occouped
    PGESTRING fullPath;
    //! Full episode path of file which was opened before crash occouped
    PGESTRING path;
    //! Base file name of file which was opened before crash occouped
    PGESTRING filename;
};

/*!
 * \brief Contains additional helpful meda-data used by PGE Applications
 */
struct MetaData
{
    //! Array of all position bookmarks presented in the opened file
    PGELIST<Bookmark> bookmarks;

    /* For Editor application only*/
    //! Crash backup of Editor's special data
    CrashData crash;

    //!Helper meta-data
    FileFormatMeta meta;
};

#endif // META_FILEDATA_H
