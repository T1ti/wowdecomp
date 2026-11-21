#ifndef ENGINE_SOURCE_MODEL2_M2WRITE_H
#define ENGINE_SOURCE_MODEL2_M2WRITE_H

// Tool features: the M2 model writer.
//
// The 0.5.3 client links the art tools' model module, ENGINE\Source\MDLFile, whole: the reader (MDLFileRead,
// MDLFileBinaryLoad) the game uses and a writer nothing in the game calls (MDLFileWrite -> IWriteMdlFile ->
// ModelDataToText for .mdl text / ModelDataToBin for .mdx binary -> CallTextWriteHandlers / CallBinWriteHandlers ->
// one MDL::Write<Section> / MDL::WriteBin<Section> per section, errors in CMDLStatus, "// Exported on %s" header).
// The 0.9.1 Mac build already dropped the writer, and 3.3.5 replaced MDX with M2 (Engine/Source/Model2), whose
// writer never shipped in any client. This shell restores the same design for the 3.3.5 format:
//
//   M2FileWrite / M2SkinFileWrite      MDLFileWrite              public entry points, write one file
//   M2DataToBin / M2SkinProfileToBin   ModelDataToBin            build the file image in memory
//   CM2Writer::Write<Section>          MDL::WriteBin<Section>    one writer per M2Data member
//
// M2 has no text form (the .mdl text writer has no 3.3.5 counterpart) and a model is split in two files: the .m2
// (M2Data: header plus every M2Array the header points to) and one .skin per level of detail (M2SkinProfile, the
// 3.3.5 client loads "<model>00.skin".."<model>03.skin", m_numSkinProfiles of them).
//
// The writer works on an M2Data as the client holds it after M2Init (M2Array offsets relocated to self-relative
// pointers, see M2Array::Relocate/Data) and writes file-relative offsets back, padding every array to 16 bytes like
// the shipped files.

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdint>
#include "Engine/Source/Model2/M2Data.h"
#include "Engine/Source/Storm/H/stpl.h"

class CStatus;

// Output format, like MDLFileSetDefaultWriteFormat. Only the 3.3.5 layout (M2Data::VERSION 0x108) is written.
enum M2WRITEFORMAT : int32_t {
    M2WRITEFORMAT_WOTLK = 0, // 'MD20' version 0x108, external .skin profiles
};

// Public entry points (MDLFileWrite counterparts). Return nonzero on success; failures are added to status.
int32_t M2FileWrite(const char* filename, const M2Data& data, CStatus* status);
int32_t M2SkinFileWrite(const char* filename, const M2SkinProfile& skin, CStatus* status);
void M2FileSetDefaultWriteFormat(M2WRITEFORMAT format);

// In-memory images (ModelDataToBin counterparts).
int32_t M2DataToBin(const M2Data& data, TSGrowableArray<uint8_t>& out, CStatus* status);
int32_t M2SkinProfileToBin(const M2SkinProfile& skin, TSGrowableArray<uint8_t>& out, CStatus* status);

// Builds one file image: a header block followed by the arrays it references, each array 16-byte aligned. Section
// writers append their array (and the arrays nested in it: animation tracks hold M2Array<M2Array<T>> per sequence)
// and patch the header's M2Array to the file offset.
class CM2Writer {
    public:
    TSGrowableArray<uint8_t>& m_out;
    CStatus* m_status;

    CM2Writer(TSGrowableArray<uint8_t>& out, CStatus* status);

    // Low level.
    uint32_t Tell() const;
    void Align(uint32_t alignment);
    uint32_t Append(const void* data, uint32_t size);
    template <class T>
    void WriteArray(M2Array<T>& fileArray, const M2Array<T>& source);
    void WriteTrackBase(M2TrackBase& fileTrack, const M2TrackBase& source);
    template <class T>
    void WriteTrack(M2Track<T>& fileTrack, const M2Track<T>& source);
    template <class T>
    void WritePartTrack(M2PartTrack<T>& fileTrack, const M2PartTrack<T>& source);

    // One writer per M2Data member group, in file order (MDL::WriteBin<Section> counterparts).
    void WriteHeader(const M2Data& data);
    void WriteName(M2Data& header, const M2Data& data);
    void WriteLoops(M2Data& header, const M2Data& data);
    void WriteSequences(M2Data& header, const M2Data& data);
    void WriteBones(M2Data& header, const M2Data& data);
    void WriteVertices(M2Data& header, const M2Data& data);
    void WriteColors(M2Data& header, const M2Data& data);
    void WriteTextures(M2Data& header, const M2Data& data);
    void WriteTextureWeights(M2Data& header, const M2Data& data);
    void WriteTextureTransforms(M2Data& header, const M2Data& data);
    void WriteMaterials(M2Data& header, const M2Data& data);
    void WriteCombos(M2Data& header, const M2Data& data);
    void WriteCollision(M2Data& header, const M2Data& data);
    void WriteAttachments(M2Data& header, const M2Data& data);
    void WriteEvents(M2Data& header, const M2Data& data);
    void WriteLights(M2Data& header, const M2Data& data);
    void WriteCameras(M2Data& header, const M2Data& data);
    void WriteRibbons(M2Data& header, const M2Data& data);
    void WriteParticles(M2Data& header, const M2Data& data);

    // .skin profile.
    void WriteSkinProfile(const M2SkinProfile& skin);
};

#endif

#endif
