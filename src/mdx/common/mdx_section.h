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

#pragma once
#ifndef MDX_SECTION_H
#define MDX_SECTION_H

/*! \file mdx_section.h
 *
 *  \brief Code to represent sections of PGE-X files
 *
 * This is a new implementation but supports precisely the same format as PGE-X
 *
 */

#include <string>

#include "pge_file_lib_globs.h"
#include "mdx/common/mdx_field.h"
#include "mdx/common/mdx_object.h"

void MDX_skip_section(PGE_FileFormats_misc::TextInput& inf, std::string& cur_line, const char* section_name);

struct MDX_BaseSection
{
    friend struct MDX_BaseFile;

protected:

    const char* const m_section_name = "";
    const MDX_BaseObject& m_obj_loader;
    void* const m_obj_ptr;
    const void* const m_ref_ptr;
    bool m_combine_objects = false;

    MDX_BaseSection(struct MDX_BaseFile* parent, const char* section_name, bool combine_objects, const MDX_BaseObject& obj_loader, void* obj_ptr, const void* ref_ptr);

    virtual bool has_load_callback(const void* load_callbacks_table) const = 0;
    virtual bool has_save_callback(const void* save_callbacks_table) const = 0;

    virtual bool load_callback(const void* load_callbacks_table) = 0;
    virtual bool save_callback(const void* save_callbacks_table, size_t index) = 0;

    /* attempts to match the field name. if successful, returns true and leaves the file pointer following the end of the section. */
    bool try_load(const void* cb, PGE_FileFormats_misc::TextInput& inf, std::string& cur_line);

    void do_save(const void* cb, PGE_FileFormats_misc::TextOutput& outf, std::string& out_buffer);

public:
    virtual void reset() = 0;
};

template<class load_callbacks_t, class save_callbacks_t, class _obj_t>
struct MDX_Section : public MDX_BaseSection
{
    using obj_t = _obj_t;

private:
    // private fields for load-time
    MDX_Object<_obj_t> m_obj_loader;

    obj_t m_obj{};
    const obj_t m_ref{};

    using load_callback_t = typename load_callbacks_t::template callback<obj_t>;
    using save_callback_t = typename save_callbacks_t::template callback<obj_t>;

    typedef load_callback_t load_callbacks_t::* const load_callback_ptr_t;
    typedef save_callback_t save_callbacks_t::* const save_callback_ptr_t;

    load_callback_ptr_t m_load_callback = nullptr;
    save_callback_ptr_t m_save_callback = nullptr;

protected:
    virtual bool has_load_callback(const void* load_callbacks_table) const
    {
        const auto& cb = *reinterpret_cast<const load_callbacks_t*>(load_callbacks_table);
        return cb.*m_load_callback;
    }

    virtual bool has_save_callback(const void* save_callbacks_table) const
    {
        const auto& cb = *reinterpret_cast<const save_callbacks_t*>(save_callbacks_table);
        return cb.*m_save_callback;
    }

    virtual bool load_callback(const void* load_callbacks_table)
    {
        const auto& cb = *reinterpret_cast<const load_callbacks_t*>(load_callbacks_table);
        return (cb.*m_load_callback)(cb.userdata, m_obj);
    }

    virtual bool save_callback(const void* save_callbacks_table, size_t index)
    {
        const auto& cb = *reinterpret_cast<const save_callbacks_t*>(save_callbacks_table);
        return (cb.*m_save_callback)(cb.userdata, m_obj, index);
    }

public:
    MDX_Section(MDX_BaseFile* parent, const char* section_name, bool combine_objects, load_callback_ptr_t load_callback, save_callback_ptr_t save_callback)
        : MDX_BaseSection(parent, section_name, combine_objects, m_obj_loader, &m_obj, &m_ref), m_load_callback(load_callback), m_save_callback(save_callback) {}

    virtual void reset()
    {
        m_obj = obj_t();
    }
};

#endif // #ifndef MDX_SECTION_H
