// PGELevelBGO.cpp: Implementierung von CPGELevelBGO

#include "stdafx.h"
#include "PGELevelBGO.h"
#include "Utils/ComUtils.h"

// CPGELevelBGO


COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelBGO, LONG, m_x, X);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelBGO, LONG, m_y, Y);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelBGO, LONG, m_id, ID);
COMUTILS_BSTR_PROPIMPL_FULL(CPGELevelBGO, m_layer_name, LayerName);

