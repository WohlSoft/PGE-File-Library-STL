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

/*! \file mdx_field.h
 *
 *  \brief Code to represent single fields (marker:value pairs)
 *
 * This is a new implementation but supports precisely the same format as PGE-X
 *
 */

#pragma once
#ifndef MDX_FIELD_H
#define MDX_FIELD_H

#include "pge_file_lib_globs.h"
#include "mdx/common/mdx_exception.h"
#include "mdx/common/mdx_value.h"

#include <string>


const char* MDX_skip_field(const char* line);

struct MDX_BaseObject;

struct MDX_BaseField
{
public:
    enum class SaveMode
    {
        normal,   //!< store if can_save returns true
        no_skip,  //!< always store
        not_only, //!< if only these values are non-default, the object will be ignored
    } m_save_mode = SaveMode::normal;

protected:
    const char* const m_field_name;

    MDX_BaseField(MDX_BaseObject* parent, const char* field_name, SaveMode save_mode = SaveMode::normal);

    /* attempts to load the matched field to the destination, and returns the new load pointer following the ';'. */
    virtual const char* do_load(void* dest, const char* field_data) const = 0;
    /* confirms whether the field is non-default. */
    virtual bool can_save(const void* src, const void* /* obj_t */ ref) const = 0;
    /* tries to write the field, and returns false if this is impossible. */
    virtual bool do_save(std::string& out, const void* src) const = 0;

public:
    /* attempts to match the field name. if successful, returns true and modifies the load pointer. */
    bool try_load(void* dest, const char*& field_name) const;

    /* confirms whether the field is non-default, and writes it to out if so. */
    bool try_save(std::string& out, const void* src, const void* ref) const;
};

template<class obj_t, class field_t>
struct MDX_Field : public MDX_BaseField
{
    using MDX_BaseField::m_field_name;
    using SaveMode = typename MDX_BaseField::SaveMode;

    field_t obj_t::* const m_field = nullptr;

    MDX_Field(MDX_BaseObject* parent, const char* field_name, field_t obj_t::* field, SaveMode save_mode = SaveMode::normal)
        : MDX_BaseField(parent, field_name, save_mode), m_field(field) {}

    virtual const char* do_load(void* _dest, const char* field_data) const
    {
        obj_t& dest = *reinterpret_cast<obj_t*>(_dest);

        return MDX_load_value(dest.*m_field, field_data);
    }

    virtual bool can_save(const void* _src, const void* _ref) const
    {
        const obj_t& src = *reinterpret_cast<const obj_t*>(_src);
        const obj_t& ref = *reinterpret_cast<const obj_t*>(_ref);

        return !MDX_Value<field_t>::is_ref(src.*m_field, ref.*m_field);
    }

    virtual bool do_save(std::string& out, const void* _src) const
    {
        const obj_t& src = *reinterpret_cast<const obj_t*>(_src);

        return MDX_save_value(out, src.*m_field);
    }
};

template<class obj_t, class field_t>
struct MDX_NonNegField : public MDX_Field<obj_t, field_t>
{
    using MDX_Field<obj_t, field_t>::MDX_Field;
    using MDX_Field<obj_t, field_t>::m_field;

    virtual const char* do_load(void* _dest, const char* field_data) const
    {
        obj_t& dest = *reinterpret_cast<obj_t*>(_dest);

        const char* ret = MDX_load_value(dest.*m_field, field_data);

        if(*field_data == '-' || dest.*m_field < 0)
            throw(MDX_bad_term("Negative value"));

        return ret;
    }
};

template<class obj_t>
struct MDX_UniqueField : public MDX_BaseField
{
    using MDX_BaseField::m_field_name;

    using load_func_t = const char* (*)(obj_t& dest, const char* field_data);
    using save_func_t = bool (*)(std::string& out, const obj_t& src);

    load_func_t m_load_func = nullptr;
    save_func_t m_save_func = nullptr;

    MDX_UniqueField(MDX_BaseObject* parent, const char* field_name, load_func_t load_func, save_func_t save_func)
        : MDX_BaseField(parent, field_name), m_load_func(load_func), m_save_func(save_func) {}

    virtual const char* do_load(void* _dest, const char* field_data) const
    {
        obj_t& dest = *reinterpret_cast<obj_t*>(_dest);

        if(!m_load_func)
            return field_data;

        return m_load_func(dest, field_data);
    }

    virtual bool can_save(const void* src, const void* ref) const
    {
        (void)src; (void)ref;
        return (bool)(m_save_func);
    }

    virtual bool do_save(std::string& out, const void* _src) const
    {
        const obj_t& src = *reinterpret_cast<const obj_t*>(_src);

        return m_save_func(out, src);
    }
};

template<class obj_t, class substruct_t, class field_t>
struct MDX_NestedField : public MDX_BaseField
{
    using MDX_BaseField::m_field_name;

    substruct_t obj_t::* const m_substruct = nullptr;
    field_t substruct_t::* const m_field = nullptr;

    MDX_NestedField(MDX_BaseObject* parent, const char* field_name, substruct_t obj_t::* substruct, field_t substruct_t::* field)
        : MDX_BaseField(parent, field_name), m_substruct(substruct), m_field(field) {}

    virtual const char* do_load(void* _dest, const char* field_data) const
    {
        obj_t& dest = *reinterpret_cast<obj_t*>(_dest);

        return MDX_load_value(dest.*m_substruct.*m_field, field_data);
    }

    virtual bool can_save(const void* _src, const void* _ref) const
    {
        const obj_t& src = *reinterpret_cast<const obj_t*>(_src);
        const obj_t& ref = *reinterpret_cast<const obj_t*>(_ref);

        return !MDX_Value<field_t>::is_ref(src.*m_substruct.*m_field, ref.*m_substruct.*m_field);
    }

    virtual bool do_save(std::string& out, const void* _src) const
    {
        const obj_t& src = *reinterpret_cast<const obj_t*>(_src);

        return MDX_save_value(out, src.*m_substruct.*m_field);
    }
};

template<class obj_t, class substruct_t, class field_t>
struct MDX_NonNegNestedField : public MDX_NestedField<obj_t, substruct_t, field_t>
{
    using MDX_NestedField<obj_t, substruct_t, field_t>::MDX_NestedField;
    using MDX_NestedField<obj_t, substruct_t, field_t>::m_substruct;
    using MDX_NestedField<obj_t, substruct_t, field_t>::m_field;

    virtual const char* do_load(void* _dest, const char* field_data) const
    {
        obj_t& dest = *reinterpret_cast<obj_t*>(_dest);

        const char* ret = MDX_load_value(dest.*m_substruct.*m_field, field_data);

        if(*field_data == '-' || dest.*m_substruct.*m_field < 0)
            throw(MDX_bad_term("Illegal negative"));

        return ret;
    }
};

template<class obj_t>
struct MDX_FieldXtra : public MDX_BaseField
{
    MDX_FieldXtra(MDX_BaseObject* parent)
        : MDX_BaseField(parent, "XTRA") {}

    virtual const char* do_load(void* _dest, const char* field_data) const
    {
        obj_t& dest = *reinterpret_cast<obj_t*>(_dest);
        return MDX_load_value(dest.meta.custom_params, field_data);
    }

    virtual bool can_save(const void* _src, const void* ref) const
    {
        (void)ref;
        const obj_t& src = *reinterpret_cast<const obj_t*>(_src);

        return src.meta.custom_params != PGESTRING();
    }

    virtual bool do_save(std::string& out, const void* _src) const
    {
        const obj_t& src = *reinterpret_cast<const obj_t*>(_src);

        return MDX_save_value(out, src.meta.custom_params);
    }
};

#include "mdx/common/mdx_field.hpp"

#endif // #ifndef MDX_FIELD_H
