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

#pragma once
#ifndef CSVREADERQT
#define CSVREADERQT

#include "CSVReader.h"

#include "pge_file_lib_private.h"

#if !defined(_MSC_VER) || _MSC_VER > 1800

namespace CSVReader
{
    struct CSVPGEReader
    {
        typedef PGESTRING string_type;
        CSVPGEReader(PGE_FileFormats_misc::TextInput *reader) : _reader(reader) {}

        PGESTRING read_line()
        {
            return _reader->readLine();
        }
    private:
        PGE_FileFormats_misc::TextInput *_reader;
    };

    #ifdef PGE_FILES_QT
    struct CSVPGESTRINGUtils
    {
        static bool find(const QString &str, QChar sep, size_t &findIndex)
        {
            int pos = str.indexOf(sep, findIndex);
            if(pos == -1)
                return false;
            findIndex = static_cast<size_t>(pos);
            return true;
        }

        static size_t length(const QString &str)
        {
            return static_cast<size_t>(str.length());
        }

        static QString substring(const QString &str, size_t pos, size_t count)
        {
            return str.mid(pos, count);
        }
    };
    #else
    struct CSVPGESTRINGUtils : DefaultStringWrapper<char, std::char_traits<char>, std::allocator<char>> {};
    #endif

    #ifdef PGE_FILES_QT
    struct CSVPGESTRINGConverter
    {
        static void Convert(double *out, const QString &field)
        {
            bool ok = true;
            *out = field.toDouble(&ok);
            if(!ok)
                throw std::invalid_argument("Could not convert to double");
        }
        static void Convert(float *out, const QString &field)
        {
            bool ok = true;
            *out = field.toFloat(&ok);
            if(!ok)
                throw std::invalid_argument("Could not convert to float");
        }
        static void Convert(int *out, const QString &field)
        {
            bool ok = true;
            *out = field.toInt(&ok);
            if(!ok)
                throw std::invalid_argument("Could not convert to int");
        }
        static void Convert(long *out, const QString &field)
        {
            bool ok = true;
            *out = field.toLong(&ok);
            if(!ok)
                throw std::invalid_argument("Could not convert to long");
        }
        static void Convert(long long *out, const QString &field)
        {
            bool ok = true;
            *out = field.toLongLong(&ok);
            if(!ok)
                throw std::invalid_argument("Could not convert to long long");
        }
        static void Convert(long double *out, const QString &field)
        {
            bool ok = true;
            *out = field.toDouble(&ok);
            if(!ok)
                throw std::invalid_argument("Could not convert to long double");
        }
        static void Convert(unsigned int *out, const QString &field)
        {
            bool ok = true;
            *out = static_cast<unsigned int>(field.toULong(&ok));
            if(!ok)
                throw std::invalid_argument("Could not convert to unsigned int");
        }
        static void Convert(unsigned long *out, const QString &field)
        {
            bool ok = true;
            *out = field.toULong(&ok);
            if(!ok)
                throw std::invalid_argument("Could not convert to unsigned long");
        }
        static void Convert(unsigned long long *out, const QString &field)
        {
            bool ok = true;
            *out = field.toULongLong(&ok);
            if(!ok)
                throw std::invalid_argument("Could not convert to unsigned long long");
        }
        static void Convert(bool *out, const QString &field)
        {
            if(field == "0" || field == "") // FIXME: Is it correct? Or too hackish?
                *out = false;
            else if(field == "!0" || field == "1") // FIXME: Is it correct? Or too hackish?
                *out = true;
            else
                throw std::invalid_argument(std::string("Could not convert to bool (must be empty, \"0\", \"!0\" or \"1\"), got \"") + field.toStdString() + std::string("\""));
        }
        static void Convert(QString *out, const QString &field)
        {
            *out = field;
        }
    };
    #else
    struct CSVPGESTRINGConverter : DefaultCSVConverter<std::string> {};
    #endif

    namespace detail
    {
        template<class Reader>
        struct CSVReaderFromPGESTRING
        {
            typedef typename Reader::string_type string_type;
            typedef typename string_type::value_type value_type;

            typedef CSVReader<Reader, string_type, value_type, CSVPGESTRINGUtils, CSVPGESTRINGConverter> full_type;
        };
    }

    template<class Reader, class CharT>
    constexpr typename detail::CSVReaderFromPGESTRING<Reader>::full_type MakeCSVReaderForPGESTRING(Reader *reader, CharT sep)
    {
        return typename detail::CSVReaderFromPGESTRING<Reader>::full_type(reader, sep);
    }

}

#endif


#endif // CSVREADERQT
