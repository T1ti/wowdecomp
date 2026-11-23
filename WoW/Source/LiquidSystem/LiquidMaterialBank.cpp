#include "Liquid.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Services/SysMessage.h"
#include "WoW/Source/DB/WowClientDB.h"
#include "WoW/Source/DB/Rec/LiquidMaterialRec.h"
#include "WoW/Source/DB/Rec/LiquidTypeRec.h"

// STATIC: 0x00D43B24
TSGrowableArray<Liquid::IMaterial*> Liquid::CMaterialBank::m_materials;

// VERIFY: 8e2d4258 manual
// OFFSET: 0x008A1F50
void Liquid::CMaterialBank::Destroy() {
    for (uint32_t i = 0; i < Liquid::CMaterialBank::m_materials.Count(); i++) {
        if (Liquid::CMaterialBank::m_materials[i]) {
            Liquid::CMaterialBank::m_materials[i]->Release();
        }
    }

    Liquid::CMaterialBank::m_materials.SetCount(0);
}

// VERIFY: 5f91fa25 auto
// OFFSET: 0x008A4280
void Liquid::ClearMaterials() {
    Liquid::s_procWaterAllocator.Clear(".?AVCMaterialProcWater@Liquid@@", SLOG_OBJECT, 1);
    Liquid::s_procWaterFFPAllocator.Clear(".?AVCMaterialProcWaterFFP@Liquid@@", SLOG_OBJECT, 1);
    Liquid::s_waterAllocator.Clear(".?AVCMaterialWater@Liquid@@", SLOG_OBJECT, 1);
    Liquid::s_waterNoSpecAllocator.Clear(".?AVCMaterialWaterNoSpec@Liquid@@", SLOG_OBJECT, 1);
    Liquid::s_waterFFPAllocator.Clear(".?AVCMaterialWaterFFP@Liquid@@", SLOG_OBJECT, 1);
    Liquid::s_magmaAllocator.Clear(".?AVCMaterialMagma@Liquid@@", SLOG_OBJECT, 1);
    Liquid::s_magmaFFPAllocator.Clear(".?AVCMaterialMagmaFFP@Liquid@@", SLOG_OBJECT, 1);
}

// VERIFY: 85245160 auto
// OFFSET: 0x008A1FA0
Liquid::IMaterial* Liquid::CMaterialBank::GetMaterial(uint32_t liquidType) {
    STORM_ASSERT(liquidType);

    LiquidTypeRec* typeRec;

    while (true) {
        if (!Liquid::s_materialBankRefCount) {
            return nullptr;
        }

        typeRec = Liquid::s_liquidTypeDB->GetRecord(liquidType);

        if (typeRec) {
            break;
        }

        SysMsgPrintf(SYSMSG_ERROR, SYSMSG_FILTER_WORLD, "Material Bank: Liquid type [%d] not found, defaulting to water!", liquidType);
        STORM_ASSERT(liquidType != LIQUID_TYPE_WATER); // 12213: "liquidType!=1"
        liquidType = LIQUID_TYPE_WATER;
    }

    uint32_t materialId = typeRec->m_materialID;

    if (materialId < Liquid::CMaterialBank::m_materials.Count() && Liquid::CMaterialBank::m_materials.m_data[materialId]) {
        return Liquid::CMaterialBank::m_materials.m_data[materialId];
    }

    bool vertexShaders = g_theGxDevicePtr->Caps().m_shaderTargets[GxSh_Vertex] >= GxShVS_vs_1_1;
    bool shaders = Liquid::s_shadersEnabled && vertexShaders && g_theGxDevicePtr->Caps().m_shaderTargets[GxSh_Pixel] >= GxShPS_ps_2_0;

    Liquid::IMaterial* mat = nullptr;

    switch (materialId) {
        case LIQUID_MATERIAL_WATER:
            if (shaders) {
                if (Liquid::s_specularEnabled) {
                    mat = Liquid::CMaterialWater::Create();
                } else {
                    mat = Liquid::CMaterialWaterNoSpec::Create();
                }
            } else {
                mat = Liquid::CMaterialWaterFFP::Create();
            }
            break;

        case LIQUID_MATERIAL_MAGMA:
            if (shaders) {
                mat = Liquid::CMaterialMagma::Create();
            } else {
                mat = Liquid::CMaterialMagmaFFP::Create();
            }
            break;

        case LIQUID_MATERIAL_PROCEDURAL_WATER:
            if (shaders) {
                mat = Liquid::CMaterialProcWater::Create();
            } else {
                mat = Liquid::CMaterialProcWaterFFP::Create();
            }
            break;

        default:
            STORM_ASSERT(0 && "Unknown material type!");
            break;
    }

    STORM_ASSERT(mat);
    Liquid::CMaterialBank::m_materials.GrowToFit(materialId, 1);
    Liquid::CMaterialBank::m_materials.m_data[materialId] = mat;

    return mat;
}
