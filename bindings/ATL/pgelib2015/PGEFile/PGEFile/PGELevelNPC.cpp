// PGELevelNPC.cpp: Implementierung von CPGELevelNPC

#include "stdafx.h"
#include "PGELevelNPC.h"

// CPGELevelNPC
#include "Utils/ComUtils.h"
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelNPC, LONG, m_x, X);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelNPC, LONG, m_y, Y);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelNPC, PGEDirection, m_direction, Direction);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelNPC, LONG, m_id, ID);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelNPC, LONG, m_special_data1, SpecialData1);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelNPC, LONG, m_special_data2, SpecialData2);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelNPC, VARIANT_BOOL, m_generator, IsGenerator);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelNPC, PGEGeneratorDirection, m_generator_direction, GeneratorDirection);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelNPC, PGEGeneratorType, m_generator_type, GeneratorType);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelNPC, LONG, m_generator_period, GeneratorPeriod);
COMUTILS_BSTR_PROPIMPL_FULL(CPGELevelNPC, m_message, Message);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelNPC, VARIANT_BOOL, m_friendly, IsFriendly);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelNPC, VARIANT_BOOL, m_nomove, NoMove);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelNPC, VARIANT_BOOL, m_legacy_boss, IsLegacyBoss);
COMUTILS_BSTR_PROPIMPL_FULL(CPGELevelNPC, m_layer_name, LayerName);
COMUTILS_BSTR_PROPIMPL_FULL(CPGELevelNPC, m_event_activate, EventActivateName);
COMUTILS_BSTR_PROPIMPL_FULL(CPGELevelNPC, m_event_die, EventDieName);
COMUTILS_BSTR_PROPIMPL_FULL(CPGELevelNPC, m_event_talk, EventTalkName);
COMUTILS_BSTR_PROPIMPL_FULL(CPGELevelNPC, m_event_no_more_object_in_layer, EventNoMoreObjectInLayerName);
COMUTILS_BSTR_PROPIMPL_FULL(CPGELevelNPC, m_layer_attached, AttachLayerName);


