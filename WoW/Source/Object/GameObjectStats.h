#ifndef WOW_SOURCE_OBJECT_GAMEOBJECTSTATS_H
#define WOW_SOURCE_OBJECT_GAMEOBJECTSTATS_H

#include <cstddef>
#include <cstdint>

class CDataStore;

class GameObjectStats {
    public:
    static const int32_t NUM_PROP_VALUES = 24;

    /* 0000 */ uint32_t m_typeID;
    /* 0004 */ uint32_t m_displayID;
    /* 0008 */ char* m_iconName;
    /* 000C */ char* m_castBarCaption;
    /* 0010 */ char* m_closeText;
    /* 0014 */ uint32_t m_propValue[NUM_PROP_VALUES];
    /* 0074 */ float m_scale;
    /* 0078 */ uint32_t m_questItems[6];
    /* 0090 */ char* m_name[4];

    ~GameObjectStats();
    const char* GetName(int32_t index);
    void Pack(CDataStore* msg);
};

#endif
