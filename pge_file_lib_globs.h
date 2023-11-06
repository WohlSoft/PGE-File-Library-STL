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

/*! \file pge_file_lib_globs.h
    \brief Contains internal settings and references for PGE File Library

    All defined here macroses are allows to build PGE File Library for both
    Qt and STL libraries set.
*/
#pragma once
#ifndef PGE_FILE_LIB_GLOBS_H_
#define PGE_FILE_LIB_GLOBS_H_

/*! \def PGE_FILES_QT
    \brief If this macro is defined, Qt version of PGE File Library will be built
*/

/*! \def PGEChar
    \brief A macro which equal to 'char' if PGE File Library built in the STL mode
           and equal to QChar if PGE File Library built in the Qt mode
*/

/*! \def PGESTRING
    \brief A macro which equal to std::string if PGE File Library built in the STL mode
           and equal to QString if PGE File Library built in the Qt mode
*/

/*! \def PGESTRINGList
    \brief A macro which equal to std::vector<std::string> if PGE File Library built in the STL mode
           and equal to QStringList if PGE File Library built in the Qt mode
*/

/*! \def PGELIST
    \brief A macro which equal to std::vector if PGE File Library built in the STL mode
           and equal to QList if PGE File Library built in the Qt mode
*/

/*! \def PGEVECTOR
    \brief A macro which equal to std::vector if PGE File Library built in the STL mode
           and equal to QVector if PGE File Library built in the Qt mode
*/

#include <cstdint>

#ifdef PGE_FILES_QT
#include <QString>
#include <QStringList>
#include <QList>
#include <QVector>
#include <QPair>
#include <QFile>
#include <QMap>
#include <QTextStream>

#define PGE_FILES_INHERED : public QObject

typedef QString PGESTRING;
typedef QStringList PGESTRINGList;
typedef QChar   PGEChar;
#define PGELIST QList
#define PGEVECTOR QVector
#define PGEPAIR QPair
#define PGEMAP QMap
#define PGEMAPKEY(it) (it.key())
#define PGEMAPVAL(it) (it.value())

#define PGESTRING_EMPLACE(x) push_back(x)


#else /* PGE_FILES_QT */

#include <string>
#include <vector>
#include <cstdio>
#include <utility>
#include <map>

#define PGE_FILES_INHERED

typedef std::string                 PGESTRING;
typedef std::vector<std::string>    PGESTRINGList;
typedef char PGEChar;
#define PGELIST std::vector
#define PGEVECTOR std::vector
#define PGEPAIR std::pair
#define PGEMAP std::map
#define PGEMAPKEY(it) (it->first)
#define PGEMAPVAL(it) (it->second)

#define PGESTRING_EMPLACE(x) emplace_back(x)

#endif /* PGE_FILES_QT */

/*!
 * Misc I/O classes used by PGE File Library internally
 */
namespace PGE_FileFormats_misc
{
/**
 * @brief Check the header to identify valid SMBX64 file format
 * @param src Header source string
 * @return true if the header was identified as SMBX64 file
 */
bool PGE_DetectSMBXFile(PGESTRING src);

/*!
 * \brief Provides cross-platform file path calculation for a file names or paths
 */
class FileInfo
{
public:
    /*!
     * \brief Constructor
     */
    FileInfo();
    /*!
     * \brief Constructor with pre-opening of a file
     * \param filepath relative or absolute file path
     */
    FileInfo(const PGESTRING &filepath);
    /*!
     * \brief Sets file which will be used to calculate file information
     * \param filepath
     */
    void setFile(const PGESTRING &filepath);
    /*!
     * \brief Returns file extension (last part of filename after last dot)
     * \return file suffix or name extension (last part of filename after last dot)
     */
    PGESTRING suffix();
    /*!
     * \brief Returns full filename without path
     * \return full filename without path
     */
    PGESTRING filename();
    /*!
     * \brief Returns absolute path to file
     * \return absolute path to file
     */
    PGESTRING fullPath();
    /*!
     * \brief Returns base name part (first part of file name before first dot)
     * \return base name part (first part of file name before first dot)
     */
    PGESTRING basename();
    /*!
     * \brief Returns full directory path where actual file is located
     * \return full directory path where actual file is located
     */
    PGESTRING dirpath();
private:
    /*!
     * \brief Recalculates all internal fields
     */
    void rebuildData();
    /*!
     * \brief Current filename
     */
    PGESTRING m_filePath;
    /*!
     * \brief Current filename without directory path
     */
    PGESTRING m_fileName;
    /*!
     * \brief File name suffix (last part of file name after last dot)
     */
    PGESTRING m_suffix;
    /*!
     * \brief Base name (first part of file name before first dot)
     */
    PGESTRING m_baseName;
    /*!
     * \brief Full directory path where file is located
     */
    PGESTRING m_dirPath;
};


class TextInput
{
public:
    /*!
     * \brief Relative positions of carriage
     */
    enum positions
    {
        //! Relative to current position
        current = 0,
        //! Relative to begin of file
        begin,
        //! Relative to end of file
        end
    };

    TextInput();
    virtual ~TextInput() = default;
    virtual PGESTRING read(int64_t len);
    virtual PGESTRING readLine();
    virtual PGESTRING readCVSLine();
    virtual PGESTRING readAll();
    virtual bool eof();
    virtual int64_t tell();
    virtual int seek(int64_t pos, positions relativeTo);
    virtual PGESTRING getFilePath();
    virtual void setFilePath(const PGESTRING &path);
    virtual long getCurrentLineNumber();
    virtual bool reOpen(bool utf8);

protected:
    PGESTRING m_filePath;
    long  m_lineNumber = 0;
};

class TextOutput
{
public:
    /*!
     * \brief Relative positions of carriage
     */
    enum positions
    {
        //! Relative to current position
        current = 0,
        //! Relative to begin of file
        begin,
        //! Relative to end of file
        end
    };
    enum outputMode
    {
        truncate = 0,
        append,
        overwrite
    };

    TextOutput();
    virtual ~TextOutput() = default;
    virtual int write(PGESTRING buffer);
    virtual int64_t tell();
    virtual int seek(int64_t pos, positions relativeTo);
    virtual PGESTRING getFilePath();
    virtual void setFilePath(const PGESTRING &path);
    virtual long getCurrentLineNumber();
    TextOutput &operator<<(const PGESTRING &s);
    TextOutput &operator<<(const char *s);

protected:
    PGESTRING m_filePath;
    long  m_lineNumber = 0;
};


class RawTextInput: public TextInput
{
public:
    RawTextInput();
    RawTextInput(PGESTRING *rawString, const PGESTRING &filepath = PGESTRING());
    virtual ~RawTextInput() = default;
    bool open(PGESTRING *rawString, const PGESTRING &filepath = PGESTRING());
    void close();
    virtual PGESTRING read(int64_t len);
    virtual PGESTRING readLine();
    virtual PGESTRING readCVSLine();
    virtual PGESTRING readAll();
    virtual bool eof();
    virtual int64_t tell();
    virtual int seek(int64_t pos, positions relativeTo);

private:
    int64_t m_pos = 0;
    PGESTRING *m_data = nullptr;
    bool m_isEOF = false;
};

class RawTextOutput: public TextOutput
{
public:
    RawTextOutput();
    RawTextOutput(PGESTRING *rawString, outputMode mode = truncate);
    virtual ~RawTextOutput();
    bool open(PGESTRING *rawString, outputMode mode = truncate);
    void close();
    int write(PGESTRING buffer);
    int64_t tell();
    int seek(int64_t pos, positions relativeTo);
private:
    long long m_pos = 0ll;
    PGESTRING *m_data = nullptr;
};



/*!
 * \brief Provides cross-platform text file reading interface
 */
class TextFileInput: public TextInput
{
public:
    /*!
     * \brief Checks is requested file exist
     * \param filePath Full or relative path to the file
     * \return true if file exists
     */
    static bool exists(const PGESTRING &filePath);

    /*!
     * \brief Constructor
     */
    TextFileInput();

    /*!
     * \brief Constructor with pre-opening of the file
     * \param filePath Full or relative path to the file
     * \param utf8 Use UTF-8 encoding or will be used local 8-bin encoding
     */
    TextFileInput(const PGESTRING &filePath, bool utf8 = false);

    /*!
     * \brief Destructor
     */
    virtual ~TextFileInput();
    /*!
     * \brief Opening of the file
     * \param filePath Full or relative path to the file
     * \param utf8 Use UTF-8 encoding or will be used local 8-bin encoding
     */
    bool open(const PGESTRING &filePath, bool utf8 = false);

    /*!
     * \brief Re-open opened file with or without UTF8 mode enabled
     * \param utf8 Use UTF-8 encoding or will be used local 8-bin encoding
     */
    bool reOpen(bool utf8 = false);

    /*!
     * \brief Close currently opened file
     */
    void close();

    /*!
     * \brief Reads requested number of characters from a file
     * \param Maximal lenght of characters to read from file
     * \return string contains requested line of characters
     */
    PGESTRING read(int64_t len);

    /*!
     * \brief Reads whole line before line feed character
     * \return string contains gotten line
     */
    PGESTRING readLine();

    /*!
     * \brief Reads whole line before line feed character or before first unquoted comma
     * \return string contains gotten line
     */
    PGESTRING readCVSLine();

    /*!
     * \brief Reads all data from a file at current position of carriage
     * \return
     */
    PGESTRING readAll();

    /*!
     * \brief Is carriage position at end of file
     * \return true if carriage position at end of file
     */
    bool eof();

    /*!
     * \brief Returns current position of carriage relative to begin of file
     * \return current position of carriage relative to begin of file
     */
    int64_t tell();

    /*!
     * \brief Changes position of carriage to specific file position
     * \param pos Target position of carriage
     * \param relativeTo defines relativity of target position of carriage (current position, begin of file or end of file)
     */
    int seek(int64_t pos, positions relativeTo);

private:
#ifdef PGE_FILES_QT
    //! Read as UTF8 or as ANSI
    bool m_utf8 = true;
    //! File handler used in Qt version of PGE file Library
    QFile file;
    //! File input stream used in Qt version of PGE file Library
    QTextStream stream;
#else
    //! File input stream used in STL version of PGE file Library
    FILE *stream = nullptr;
#endif
};


class TextFileOutput: public TextOutput
{
public:
    /*!
     * \brief Checks is requested file exist
     * \param filePath Full or relative path to the file
     * \return true if file exists
     */
    static bool exists(const PGESTRING &filePath);
    /*!
     * \brief Constructor
     */
    TextFileOutput();
    /*!
     * \brief Constructor with pre-opening of the file
     * \param filePath Full or relative path to the file
     * \param utf8 Use UTF-8 encoding or will be used local 8-bin encoding
     */
    TextFileOutput(PGESTRING filePath, bool utf8 = false, bool forceCRLF = false, outputMode mode = truncate);
    /*!
     * \brief Destructor
     */
    virtual ~TextFileOutput();
    /*!
     * \brief Opening of the file
     * \param filePath Full or relative path to the file
     * \param utf8 Use UTF-8 encoding or will be used local 8-bin encoding
     */
    bool open(PGESTRING filePath, bool utf8 = false, bool forceCRLF = false, outputMode mode = truncate);
    /*!
     * \brief Close currently opened file
     */
    void close();
    /*!
     * \brief Reads requested number of characters from a file
     * \param Maximal lenght of characters to read from file
     * \return string contains requested line of characters
     */
    int write(PGESTRING buffer);
    /*!
     * \brief Returns current position of carriage relative to begin of file
     * \return current position of carriage relative to begin of file
     */
    int64_t tell();
    /*!
     * \brief Changes position of carriage to specific file position
     * \param pos Target position of carriage
     * \param relativeTo defines relativity of target position of carriage (current position, begin of file or end of file)
     */
    int seek(int64_t pos, positions relativeTo);

private:
    //! Enfoce CRLF line ending in a file, even on non-Windows platforms
    bool m_forceCRLF = false;
#ifdef PGE_FILES_QT
    //! Write in UTF8 or in ANSI
    bool m_utf8 = true;
    //! File handler used in Qt version of PGE file Library
    QFile file;
    //! File input stream used in Qt version of PGE file Library
    QTextStream stream;
#else
    //! File input stream used in STL version of PGE file Library
    FILE *stream = nullptr;
#endif
};

}

#endif // PGE_FILE_LIB_GLOBS_H_
