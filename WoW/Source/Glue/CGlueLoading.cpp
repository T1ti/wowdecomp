#include "CGlueLoading.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Os/TimeManager.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/LoadingScreen.h"

// STATIC: 0x00B6B4A0
uint32_t CGlueLoading::m_loadStartTime;

// STATIC: 0x00B6B4A4
uint32_t CGlueLoading::m_loadCompleteTime;

// STATIC: 0x00B6B4A8
int32_t CGlueLoading::m_loadState;

// STATIC: 0x00B6B4AC
float CGlueLoading::m_startProgress;

// STATIC: 0x00B6B4B0
float CGlueLoading::m_loadProgress;

// VERIFY: 961ab384 auto
// OFFSET: 0x004E68E0
float CGlueLoading::ComputeLoadProgress(CCharacterComponent* component, int64_t& loaded, int64_t& total) {
    loaded = 0;
    total = 0;

    if (component) {
        component->ComputeLoadProgress(loaded, total);
        component->m_data.m_model->ComputeLoadProgress(loaded, total);

        if (total) {
            return static_cast<float>(loaded) / static_cast<float>(total);
        }
    }

    return 0.0f;
}

// VERIFY: 18ba530c auto
// OFFSET: 0x004E6940
int32_t CGlueLoading::Idle(CCharacterComponent* component) {
    uint32_t time = OsGetAsyncTimeMs();
    float elapsed = (time - CGlueLoading::m_loadStartTime) * 0.001f;
    CGlueLoading::m_loadStartTime = time;

    switch (CGlueLoading::m_loadState) {
        case 1: {
            int64_t loaded;
            int64_t total;
            float progress = CGlueLoading::ComputeLoadProgress(component, loaded, total);

            CGlueLoading::m_loadProgress += elapsed * 0.25f;

            if (CGlueLoading::m_loadProgress > progress) {
                CGlueLoading::m_loadProgress = progress;
            }

            bool done = false;

            if (loaded == total) {
                CGlueLoading::m_loadState = 2;
                CGlueLoading::m_loadCompleteTime = time + 250;
                done = true;
            }

            if (component) {
                CM2Model* model = component->m_data.m_model;

                if (model->m_attachParent) {
                    model->m_runtimeFlags = (model->m_runtimeFlags & ~0x80u) | (done << 7);
                } else {
                    model->m_runtimeFlags = (model->m_runtimeFlags & ~0x8u) | (done << 3);
                }

                if (model->m_attachParent) {
                    model->m_runtimeFlags = (model->m_runtimeFlags & ~0x20000u) | (done << 17);
                } else {
                    model->m_runtimeFlags = (model->m_runtimeFlags & ~0x10000u) | (done << 16);
                }
            }

            LoadingScreenMiniUpdate((CGlueLoading::m_loadProgress - CGlueLoading::m_startProgress) / (1.0f - CGlueLoading::m_startProgress));
            break;
        }

        case 2: {
            CGlueLoading::m_loadProgress += elapsed * 4.0f;

            if (CGlueLoading::m_loadProgress > 1.0f) {
                CGlueLoading::m_loadProgress = 1.0f;
            }

            LoadingScreenMiniUpdate((CGlueLoading::m_loadProgress - CGlueLoading::m_startProgress) / (1.0f - CGlueLoading::m_startProgress));

            if (time > CGlueLoading::m_loadCompleteTime) {
                LoadingScreenMiniDisable();
                CGlueLoading::m_loadState = 3;
            }

            break;
        }

        default: {
            break;
        }
    }

    return 1;
}

// VERIFY: 57ed6fc9 manual
// OFFSET: 0x004E6AE0
void CGlueLoading::StartLoad(CCharacterComponent* component, int32_t keepProgress) {
    if (SFile::IsTrial()) {
        int64_t loaded;
        int64_t total;
        float progress = CGlueLoading::ComputeLoadProgress(component, loaded, total);

        if (loaded == total) {
            LoadingScreenMiniDisable();
            CGlueLoading::m_loadState = 3;
        } else {
            if (component) {
                CM2Model* model = component->m_data.m_model;

                if (model->m_attachParent) {
                    model->m_runtimeFlags &= ~0x80u;
                } else {
                    model->m_runtimeFlags &= ~0x8u;
                }

                if (model->m_attachParent) {
                    model->m_runtimeFlags &= ~0x20000u;
                } else {
                    model->m_runtimeFlags &= ~0x10000u;
                }
            }

            CGlueLoading::m_startProgress = keepProgress ? progress : 0.0f;
            CGlueLoading::m_loadProgress = progress;
            CGlueLoading::m_loadState = 1;
            CGlueLoading::m_loadStartTime = OsGetAsyncTimeMs();
        }
    } else {
        if (component) {
            component->RenderPrep(1);
            component->m_data.m_model->IsDrawable(1, 1);
        }

        CGlueLoading::m_loadState = 3;
    }
}
