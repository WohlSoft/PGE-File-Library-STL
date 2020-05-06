// PGELevelLayer.cpp: Implementierung von CPGELevelLayer

#include "stdafx.h"
#include "PGELevelLayer.h"


// CPGELevelLayer
#include "Utils/ComUtils.h"
COMUTILS_BSTR_PROPIMPL_FULL(CPGELevelLayer, m_name, Name)
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelLayer, VARIANT_BOOL, m_hidden, Hidden)
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelLayer, VARIANT_BOOL, m_locked, Locked)

