#include "Engine/Source/Gxu/GxuScene.h"

// VERIFY: 900a5aaa manual
// OFFSET: 0x009A8180
GxuScene::~GxuScene() {
    STORM_ASSERT(m_sceneObjects.Count()==0);
}

// VERIFY: a86140fd auto
// OFFSET: 0x009A81C0
void GxuScene::Clear() {
    for (uint32_t i = 0; i < this->m_sceneObjects.Count(); i++) {
        this->m_sceneObjects[i]->Release();
    }

    this->m_sceneObjects.SetCount(0);
}

// VERIFY: 54392179 manual
// OFFSET: 0x009A81F0
void GxuScene::Update(uint32_t time) {
    if (!this->m_lastUpdateTime) {
        this->m_lastUpdateTime = time;
    }

    uint32_t count = this->m_sceneObjects.Count();
    uint32_t removed = 0;
    uint32_t index = 0;
    float elapsed = static_cast<float>(time - this->m_lastUpdateTime) / 1000.0f;

    if (count) {
        while (true) {
            if (removed) {
                this->m_sceneObjects[index] = this->m_sceneObjects[index + removed];
            }

            GxuSceneObject* object = this->m_sceneObjects[index];

            if (object->IsAlive()) {
                object->Update(elapsed);
                index++;
            } else {
                object->Release();
                count--;
                removed++;
            }

            if (index >= count) {
                break;
            }
        }
    }

    this->m_sceneObjects.SetCount(count);
    this->m_lastUpdateTime = time;
}

// VERIFY: c3b3d053 auto
// OFFSET: 0x009A80C0
void GxuScene::Render() {
    for (uint32_t i = 0; i < this->m_sceneObjects.Count(); i++) {
        this->m_sceneObjects[i]->Render();
    }
}

