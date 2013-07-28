#include "stdafx.h"
#include "GlobalSettings.h"

bool GlobalSettings::s_Initialised = false;
GlobalSettings GlobalSettings::s_It;

GlobalSettings::GlobalSettings()
{
}

void GlobalSettings::Init(tinyxml2::XMLElement const& element)
{
    assert(!s_Initialised);

#if defined _DEBUG
    s_It.DebugCells = element.BoolAttribute("DebugCells");
    s_It.DebugGhosts = element.BoolAttribute("DebugGhosts");
#else
    s_It.DebugCells = false;
    s_It.DebugGhosts = false;
#endif

    s_Initialised = true;
}

GlobalSettings const& GlobalSettings::It()
{
    assert(s_Initialised);
    return s_It;
}
