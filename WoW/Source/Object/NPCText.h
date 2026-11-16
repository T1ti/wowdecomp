#ifndef WOW_SOURCE_OBJECT_NPCTEXT_H
#define WOW_SOURCE_OBJECT_NPCTEXT_H

#include <cstddef>
#include <cstdint>

class CDataStore;

// Inferred names: weighted text entries per NPC text record, and emotes per entry.
// m_text holds a male and a female variant of each entry.
constexpr int32_t NUM_NPC_TEXT_ENTRIES = 8;
constexpr int32_t NUM_NPC_TEXT_EMOTES = 3;

class NPCText {
    public:
    static const uint32_t s_cacheVersion = 1;

    /* 0000 */ float m_probability[NUM_NPC_TEXT_ENTRIES];
    /* 0020 */ char* m_text[NUM_NPC_TEXT_ENTRIES][2];
    /* 0060 */ uint32_t m_language[NUM_NPC_TEXT_ENTRIES];
    /* 0080 */ uint32_t m_emoteDelay[NUM_NPC_TEXT_ENTRIES][NUM_NPC_TEXT_EMOTES];
    /* 00E0 */ uint32_t m_emote[NUM_NPC_TEXT_ENTRIES][NUM_NPC_TEXT_EMOTES];

    ~NPCText();
    void Pack(CDataStore* msg);
    void Unpack(CDataStore* msg);
};

#endif
