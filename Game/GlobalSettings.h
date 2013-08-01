#pragma once

class GlobalSettings
{
public:
    static void Init(tinyxml2::XMLElement const& element);
    static GlobalSettings const& It();

    bool DebugCells;
    bool DebugGhosts;
    bool DebugImmortal;

private:
    GlobalSettings();

    static GlobalSettings s_It; 
    static bool s_Initialised;
};
