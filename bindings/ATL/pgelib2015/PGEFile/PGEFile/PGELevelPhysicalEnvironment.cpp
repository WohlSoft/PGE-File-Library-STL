// PGELevelPhysicalEnvironment.cpp: Implementierung von CPGELevelPhysicalEnvironment

#include "stdafx.h"
#include "PGELevelPhysicalEnvironment.h"


// CPGELevelPhysicalEnvironment
#include "Utils/ComUtils.h"

COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelPhysicalEnvironment, LONG, m_x, X);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelPhysicalEnvironment, LONG, m_y, Y);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelPhysicalEnvironment, LONG, m_width, Width);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelPhysicalEnvironment, LONG, m_height, Height);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelPhysicalEnvironment, PGEPhysicalEnvironmentType, m_type, Type);
COMUTILS_BSTR_PROPIMPL_FULL(CPGELevelPhysicalEnvironment, m_layer_name, LayerName);
