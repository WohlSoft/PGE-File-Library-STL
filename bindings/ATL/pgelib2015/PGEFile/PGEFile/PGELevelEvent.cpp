// PGELevelEvent.cpp: Implementierung von CPGELevelEvent

#include "stdafx.h"
#include "PGELevelEvent.h"


// CPGELevelEvent
#include "Utils/ComUtils.h"
COMUTILS_BSTR_PROPIMPL_FULL(CPGELevelEvent, m_name, Name);
COMUTILS_BSTR_PROPIMPL_FULL(CPGELevelEvent, m_talk_message, TalkMessage);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelEvent, LONG, m_sound_id, SoundID);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelEvent, PGEEventEndGame, m_end_game, EndGameType);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelEvent, VARIANT_BOOL, m_no_smoke, NoSmoke);
COMUTILS_BSTR_PROPIMPL_FULL(CPGELevelEvent, m_trigger_event, TriggerEventName);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelEvent, LONG, m_trigger_timer, TriggerTime);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelEvent, VARIANT_BOOL, m_key_up, KeyUp);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelEvent, VARIANT_BOOL, m_key_down, KeyDown);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelEvent, VARIANT_BOOL, m_key_left, KeyLeft);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelEvent, VARIANT_BOOL, m_key_right, KeyRight);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelEvent, VARIANT_BOOL, m_key_jump, KeyJump);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelEvent, VARIANT_BOOL, m_key_alt_jump, KeyAltJump);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelEvent, VARIANT_BOOL, m_key_run, KeyRun);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelEvent, VARIANT_BOOL, m_key_alt_run, KeyAltRun);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelEvent, VARIANT_BOOL, m_key_start, KeyStart);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelEvent, VARIANT_BOOL, m_key_drop, KeyDrop);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelEvent, VARIANT_BOOL, m_autostart, Autostart);
COMUTILS_BSTR_PROPIMPL_FULL(CPGELevelEvent, m_move_layer_name, MoveLayerName);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelEvent, FLOAT, m_layer_speed_x, LayerSpeedX);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelEvent, FLOAT, m_layer_speed_y, LayerSpeedY);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelEvent, FLOAT, m_move_camera_x, CameraSpeedX);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelEvent, FLOAT, m_move_camera_y, CameraSpeedY);
COMUTILS_SIMPLE_PROPIMPL_FULL(CPGELevelEvent, LONG, m_scroll_section, SectionToScroll);


STDMETHODIMP CPGELevelEvent::get_LayersToHide(IPGEStringCollection** pVal)
{
    return m_layers_to_hide.CopyTo(pVal);
}


STDMETHODIMP CPGELevelEvent::get_LayersToShow(IPGEStringCollection** pVal)
{
    return m_layers_to_show.CopyTo(pVal);
}


STDMETHODIMP CPGELevelEvent::get_LayersToToggle(IPGEStringCollection** pVal)
{
    return m_layers_to_toggle.CopyTo(pVal);
}


STDMETHODIMP CPGELevelEvent::get_SectionModifiers(IPGELevelSectionModifiers** pVal)
{
    return m_section_modifiers.CopyTo(pVal);
}
