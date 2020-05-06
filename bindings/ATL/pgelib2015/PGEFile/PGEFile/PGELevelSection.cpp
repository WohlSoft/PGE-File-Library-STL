// PGELevelSection.cpp: Implementierung von CPGELevelSection

#include "stdafx.h"
#include "PGELevelSection.h"
#include "Utils/ComUtils.h"

// CPGELevelSection
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelSection, LONG, m_id, ID);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelSection, LONG, m_size_top, Top);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelSection, LONG, m_size_bottom, Bottom);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelSection, LONG, m_size_left, Left);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelSection, LONG, m_size_right, Right);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelSection, LONG, m_music_id, MusicID);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelSection, LONG, m_bgcolor, BGColor);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelSection, VARIANT_BOOL, m_iswarp, IsWarp);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelSection, VARIANT_BOOL, m_is_offscreen_exit, IsOffscreenExit);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelSection, LONG, m_bg_id, BackgroundID);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelSection, VARIANT_BOOL, m_noback, NoTurnBack);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelSection, VARIANT_BOOL, m_isunderwater, IsUnderwater);
COMUTILS_BSTR_PROPIMPL_FULL(CPGELevelSection, m_custom_music_file, CustomMusicFile)


