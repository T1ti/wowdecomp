#include "Engine/Source/Model2/M2Write.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstring>
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Os/OsFile.h"

// Tool features: the M2 model writer (design in M2Write.h).
//
// References: 0.5.3 ENGINE\Source\MDLFile\MDLFile.cpp (MDLFileSetDefaultWriteFormat 0x0078B220, MDLFileWrite
// 0x0078B300, IWriteMdlFile 0x0078B370, ModelDataToBin 0x0078B5A0, CallBinWriteHandlers 0x0078C6C0) and the section
// files (WriteBinCollision 0x00791F20, WriteBinCameras 0x00795980, Texture.cpp WriteBinTextures 0x007B0940, ...). Their bodies target the MDX chunked format and do not carry over; only the layering
// does. The M2 layout comes from the 3.3.5 reader (M2Data.h, M2Init/M2Data.cpp): the writer is its inverse.
//
// Status: the file plumbing and flat arrays are implemented; each section writer below is a TODO naming what it must
// emit. Nested arrays (animation tracks, particle and ribbon tracks, texture name strings) need two passes: write the
// element array, then each element's sub-arrays, patching the element's M2Array to the sub-array offset.

static M2WRITEFORMAT s_defaultWriteFormat = M2WRITEFORMAT_WOTLK;

// Every array in shipped 3.3.5 .m2 files starts on a 16-byte boundary.
static const uint32_t M2_ARRAY_ALIGNMENT = 16;

static int32_t WriteBufferToFile(const char* filename, const TSGrowableArray<uint8_t>& buffer, CStatus* status) {
    HOSFILE file = OsCreateFile(filename, OS_GENERIC_WRITE, 0, OS_CREATE_ALWAYS, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);

    if (file == OS_INVALID_FILE) {
        if (status) {
            status->Add(STATUS_ERROR, "Unable to create %s", filename);
        }

        return 0;
    }

    uint32_t written = 0;
    int32_t result = OsWriteFile(file, buffer.m_data, buffer.m_count, &written) && written == buffer.m_count;
    OsCloseFile(file);

    if (!result && status) {
        status->Add(STATUS_ERROR, "Unable to write %s", filename);
    }

    return result;
}

// TOOL: 0.5.3 MDLFileSetDefaultWriteFormat 0x0078B220 counterpart (0.5.3 has one format switch for .mdl text vs .mdx binary).
void M2FileSetDefaultWriteFormat(M2WRITEFORMAT format) {
    s_defaultWriteFormat = format;
}

// TOOL: 0.5.3 MDLFileWrite 0x0078B300 counterpart.
int32_t M2FileWrite(const char* filename, const M2Data& data, CStatus* status) {
    TSGrowableArray<uint8_t> buffer;

    if (!M2DataToBin(data, buffer, status)) {
        return 0;
    }

    return WriteBufferToFile(filename, buffer, status);
}

// TOOL: no MDL counterpart (MDX keeps its geosets in the model file); 3.3.5 loads them from <model>NN.skin.
int32_t M2SkinFileWrite(const char* filename, const M2SkinProfile& skin, CStatus* status) {
    TSGrowableArray<uint8_t> buffer;

    if (!M2SkinProfileToBin(skin, buffer, status)) {
        return 0;
    }

    return WriteBufferToFile(filename, buffer, status);
}

// TOOL: 0.5.3 ModelDataToBin 0x0078B5A0 counterpart. The header is built in a local copy (section writers patch its M2Arrays) and
// copied over the reserved block at offset 0 once every array is out, since appending may move m_out's storage.
int32_t M2DataToBin(const M2Data& data, TSGrowableArray<uint8_t>& out, CStatus* status) {
    if (data.m_magic != M2Data::MAGIC || data.m_version != M2Data::VERSION) {
        if (status) {
            status->Add(STATUS_ERROR, "M2DataToBin: unsupported model version 0x%X", data.m_version);
        }

        return 0;
    }

    out.SetCount(0);

    M2Data header = data;
    CM2Writer writer(out, status);
    writer.WriteHeader(data);
    writer.WriteName(header, data);
    writer.WriteLoops(header, data);
    writer.WriteSequences(header, data);
    writer.WriteBones(header, data);
    writer.WriteVertices(header, data);
    writer.WriteColors(header, data);
    writer.WriteTextures(header, data);
    writer.WriteTextureWeights(header, data);
    writer.WriteTextureTransforms(header, data);
    writer.WriteMaterials(header, data);
    writer.WriteCombos(header, data);
    writer.WriteCollision(header, data);
    writer.WriteAttachments(header, data);
    writer.WriteEvents(header, data);
    writer.WriteLights(header, data);
    writer.WriteCameras(header, data);
    writer.WriteRibbons(header, data);
    writer.WriteParticles(header, data);

    // Runtime-only flag bits (M2Data::RUNTIME_ALLOCATION_MASK) are set by M2Init when it allocates combos; never write them.
    header.m_flags &= ~M2Data::RUNTIME_ALLOCATION_MASK;
    memcpy(out.m_data, &header, sizeof(header));

    return 1;
}

// TOOL: .skin image: M2SkinProfile header, then its five arrays.
int32_t M2SkinProfileToBin(const M2SkinProfile& skin, TSGrowableArray<uint8_t>& out, CStatus* status) {
    out.SetCount(0);
    CM2Writer writer(out, status);
    writer.WriteSkinProfile(skin);
    return 1;
}

CM2Writer::CM2Writer(TSGrowableArray<uint8_t>& out, CStatus* status)
    : m_out(out)
    , m_status(status) {
}

uint32_t CM2Writer::Tell() const {
    return this->m_out.m_count;
}

void CM2Writer::Align(uint32_t alignment) {
    uint32_t count = this->m_out.m_count;
    uint32_t aligned = (count + alignment - 1) & ~(alignment - 1);

    if (aligned != count) {
        this->m_out.SetCount(aligned);
        memset(&this->m_out.m_data[count], 0, aligned - count);
    }
}

uint32_t CM2Writer::Append(const void* data, uint32_t size) {
    uint32_t offset = this->m_out.m_count;
    this->m_out.SetCount(offset + size);

    if (size) {
        memcpy(&this->m_out.m_data[offset], data, size);
    }

    return offset;
}

// Flat array: copies source's elements (read through its relocated pointer) and points fileArray at their file offset.
// fileArray must not live inside m_out (Append can move it); pass the local header or an element copy.
template <class T>
void CM2Writer::WriteArray(M2Array<T>& fileArray, const M2Array<T>& source) {
    fileArray.m_count = source.m_count;
    fileArray.m_offset = 0;

    if (!source.m_count) {
        return;
    }

    this->Align(M2_ARRAY_ALIGNMENT);
    fileArray.m_offset = this->Append(source.Data(), source.m_count * sizeof(T));
}

void CM2Writer::WriteTrackBase(M2TrackBase& fileTrack, const M2TrackBase& source) {
    // TODO: write m_sequenceTimes: the outer M2Array<M2Array<uint32_t>> (one entry per sequence), then each inner
    // timestamp array, patching the written outer entries to the inner offsets. Sequences flagged as external
    // (.anim files, M2Sequence flag 0x20 clear) keep their inner arrays out of the .m2.
    fileTrack = source;
    fileTrack.m_sequenceTimes.m_count = 0;
    fileTrack.m_sequenceTimes.m_offset = 0;
}

template <class T>
void CM2Writer::WriteTrack(M2Track<T>& fileTrack, const M2Track<T>& source) {
    this->WriteTrackBase(fileTrack, source);
    // TODO: same two-level write for m_sequenceKeys (M2Array<M2Array<T>>).
    fileTrack.m_sequenceKeys.m_count = 0;
    fileTrack.m_sequenceKeys.m_offset = 0;
}

template <class T>
void CM2Writer::WritePartTrack(M2PartTrack<T>& fileTrack, const M2PartTrack<T>& source) {
    // Particle tracks are flat: one fixed16 time array and one value array.
    this->WriteArray(fileTrack.m_times, source.m_times);
    this->WriteArray(fileTrack.m_values, source.m_values);
}

void CM2Writer::WriteHeader(const M2Data& data) {
    // Reserve the header block; M2DataToBin copies the patched header over it at the end.
    M2Data blank = {};
    this->Append(&blank, sizeof(blank));
}

void CM2Writer::WriteName(M2Data& header, const M2Data& data) {
    // The name is a NUL-terminated char array (m_count includes the terminator).
    this->WriteArray(header.m_name, data.m_name);
}

void CM2Writer::WriteLoops(M2Data& header, const M2Data& data) {
    this->WriteArray(header.m_loops, data.m_loops);
}

void CM2Writer::WriteSequences(M2Data& header, const M2Data& data) {
    // M2Sequence and the id lookup are flat.
    this->WriteArray(header.m_sequences, data.m_sequences);
    this->WriteArray(header.m_sequenceIdxHashById, data.m_sequenceIdxHashById);
}

void CM2Writer::WriteBones(M2Data& header, const M2Data& data) {
    // TODO: M2CompBone holds m_translation/m_rotation/m_scale tracks: write the bone array from element copies whose
    // tracks went through WriteTrack first (tracks before elements, so the copies carry final offsets).
    header.m_bones.m_count = 0;
    header.m_bones.m_offset = 0;
    this->WriteArray(header.m_boneIndicesById, data.m_boneIndicesById);
}

void CM2Writer::WriteVertices(M2Data& header, const M2Data& data) {
    this->WriteArray(header.m_vertices, data.m_vertices);
}

void CM2Writer::WriteColors(M2Data& header, const M2Data& data) {
    // TODO: M2Color is two tracks (color, alpha); same element-copy scheme as WriteBones.
    header.m_colors.m_count = 0;
    header.m_colors.m_offset = 0;
}

void CM2Writer::WriteTextures(M2Data& header, const M2Data& data) {
    // TODO: M2Texture carries its file name as an M2Array<char>; write the names, then the texture array.
    header.m_textures.m_count = 0;
    header.m_textures.m_offset = 0;
    this->WriteArray(header.m_textureIndicesById, data.m_textureIndicesById);
}

void CM2Writer::WriteTextureWeights(M2Data& header, const M2Data& data) {
    // TODO: M2TextureWeight is one fixed16 track.
    header.m_textureWeights.m_count = 0;
    header.m_textureWeights.m_offset = 0;
}

void CM2Writer::WriteTextureTransforms(M2Data& header, const M2Data& data) {
    // TODO: M2TextureTransform is three tracks (translation, rotation, scale).
    header.m_textureTransforms.m_count = 0;
    header.m_textureTransforms.m_offset = 0;
}

void CM2Writer::WriteMaterials(M2Data& header, const M2Data& data) {
    this->WriteArray(header.m_materials, data.m_materials);
}

void CM2Writer::WriteCombos(M2Data& header, const M2Data& data) {
    // Lookup tables, all flat uint16 arrays. m_textureCombinerCombos is only present with HAS_TEXTURE_COMBINER_COMBOS.
    this->WriteArray(header.m_boneCombos, data.m_boneCombos);
    this->WriteArray(header.m_textureCombos, data.m_textureCombos);
    this->WriteArray(header.m_textureCoordCombos, data.m_textureCoordCombos);
    this->WriteArray(header.m_textureWeightCombos, data.m_textureWeightCombos);
    this->WriteArray(header.m_textureTransformCombos, data.m_textureTransformCombos);
    this->WriteArray(header.m_textureCombinerCombos, data.m_textureCombinerCombos);
}

void CM2Writer::WriteCollision(M2Data& header, const M2Data& data) {
    // Collision mesh (MDL::WriteBinCollision counterpart): triangle indices, positions and per-face normals. The tools
    // also derive m_collisionBounds from the positions; the header copy keeps the source bounds.
    this->WriteArray(header.m_collisionIndices, data.m_collisionIndices);
    this->WriteArray(header.m_collisionPositions, data.m_collisionPositions);
    this->WriteArray(header.m_collisionFaceNormals, data.m_collisionFaceNormals);
}

void CM2Writer::WriteAttachments(M2Data& header, const M2Data& data) {
    // TODO: M2Attachment holds a visibility track.
    header.m_attachments.m_count = 0;
    header.m_attachments.m_offset = 0;
    this->WriteArray(header.m_attachmentIndicesById, data.m_attachmentIndicesById);
}

void CM2Writer::WriteEvents(M2Data& header, const M2Data& data) {
    // TODO: M2Event holds a timestamp-only track (M2TrackBase), MDL::WriteBinEventObjects counterpart.
    header.m_events.m_count = 0;
    header.m_events.m_offset = 0;
}

void CM2Writer::WriteLights(M2Data& header, const M2Data& data) {
    // TODO: M2Light holds color/intensity/attenuation/visibility tracks, MDL::WriteBinLights counterpart.
    header.m_lights.m_count = 0;
    header.m_lights.m_offset = 0;
}

void CM2Writer::WriteCameras(M2Data& header, const M2Data& data) {
    // TODO: M2Camera holds position/target spline tracks and a roll track, MDL::WriteBinCameras counterpart.
    header.m_cameras.m_count = 0;
    header.m_cameras.m_offset = 0;
    this->WriteArray(header.m_cameraIndicesById, data.m_cameraIndicesById);
}

void CM2Writer::WriteRibbons(M2Data& header, const M2Data& data) {
    // TODO: M2Ribbon holds texture/material index arrays and color/alpha/height/visibility tracks,
    // MDL::WriteBinRibbonEmitters counterpart.
    header.m_ribbons.m_count = 0;
    header.m_ribbons.m_offset = 0;
}

void CM2Writer::WriteParticles(M2Data& header, const M2Data& data) {
    // TODO: M2Particle holds the emitter tracks plus the WritePartTrack color/alpha/scale/head/tail tracks,
    // MDL::WriteBinParticleEmitters2 counterpart.
    header.m_particles.m_count = 0;
    header.m_particles.m_offset = 0;
}

void CM2Writer::WriteSkinProfile(const M2SkinProfile& skin) {
    // The .skin header ('SKIN' magic, set by the caller in skin.m_magic) followed by its flat arrays.
    M2SkinProfile header = skin;
    uint32_t headerOffset = this->Append(&header, sizeof(header));

    this->WriteArray(header.m_vertices, skin.m_vertices);
    this->WriteArray(header.m_indices, skin.m_indices);
    this->WriteArray(header.m_bones, skin.m_bones);
    this->WriteArray(header.m_skinSections, skin.m_skinSections);
    this->WriteArray(header.m_batches, skin.m_batches);

    memcpy(&this->m_out.m_data[headerOffset], &header, sizeof(header));
}

#endif
