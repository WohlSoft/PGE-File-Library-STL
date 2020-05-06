// PGELevelPlayerPoint.cpp: Implementierung von CPGELevelPlayerPoint

#include "stdafx.h"
#include "PGELevelPlayerPoint.h"


// CPGELevelPlayerPoint

#include "Utils/ComUtils.h"

COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelPlayerPoint, LONG, m_id, ID)
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelPlayerPoint, LONG, m_x, X)
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelPlayerPoint, LONG, m_y, Y)
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelPlayerPoint, LONG, m_width, Width)
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelPlayerPoint, LONG, m_height, Height)

