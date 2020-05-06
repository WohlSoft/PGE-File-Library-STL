// PGEBlock.cpp: Implementierung von CPGELevelBlock

#include "stdafx.h"
#include "PGELevelBlock.h"
#include "Utils/ComUtils.h"

// CPGELevelBlock
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelBlock, LONG, m_x, X);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelBlock, LONG, m_y, Y);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelBlock, LONG, m_width, Width);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelBlock, LONG, m_height, Height);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelBlock, LONG, m_id, ID);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelBlock, LONG, m_npc_id, NPCID);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelBlock, VARIANT_BOOL, m_invisible, Invisible);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelBlock, VARIANT_BOOL, m_slippery, Slippery);
COMUTILS_BSTR_PROPIMPL_FULL(CPGELevelBlock, m_layer_name, LayerName);
COMUTILS_BSTR_PROPIMPL_FULL(CPGELevelBlock, m_event_destroyed_name, EventDestroyedName);
COMUTILS_BSTR_PROPIMPL_FULL(CPGELevelBlock, m_event_hit_name, EventHitName);
COMUTILS_BSTR_PROPIMPL_FULL(CPGELevelBlock, m_event_no_more_object_in_layer_name, EventNoMoreObjectInLayerName);

