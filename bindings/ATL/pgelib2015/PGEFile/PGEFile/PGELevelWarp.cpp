// PGELevelWarp.cpp: Implementierung von CPGELevelWarp

#include "stdafx.h"
#include "PGELevelWarp.h"
#include "Utils/ComUtils.h"

// CPGELevelWarp
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelWarp, LONG, m_entrance_x, EntranceX);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelWarp, LONG, m_entrance_y, EntranceY);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelWarp, VARIANT_BOOL, m_is_entrance_active, IsEntranceActive);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelWarp, LONG, m_exit_x, ExitX);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelWarp, LONG, m_exit_y, ExitY);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelWarp, VARIANT_BOOL, m_is_exit_active, IsExitActive);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelWarp, PGEWarpEntrance, m_entrance_direction, EntranceDirection);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelWarp, PGEWarpExit, m_exit_direction, ExitDirection);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelWarp, PGEWarpType, m_type, Type);
COMUTILS_BSTR_PROPIMPL_FULL(CPGELevelWarp, m_level_target_name, LevelName);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelWarp, LONG, m_target_warp, LevelWarpID);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelWarp, VARIANT_BOOL, m_is_level_entrance, IsLevelEntrance);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelWarp, VARIANT_BOOL, m_is_level_exit, IsLevelExit);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelWarp, LONG, m_world_x, WorldX);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelWarp, LONG, m_world_y, WorldY);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelWarp, LONG, m_required_stars, RequiredStars);
COMUTILS_BSTR_PROPIMPL_FULL(CPGELevelWarp, m_layer_name, LayerName);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelWarp, VARIANT_BOOL, m_no_vehicle, NoVehicle);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelWarp, VARIANT_BOOL, m_allow_npc, AllowNPC);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelWarp, VARIANT_BOOL, m_is_locked, IsLocked);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelWarp, LONG, m_warp_number, WarpNumber);

