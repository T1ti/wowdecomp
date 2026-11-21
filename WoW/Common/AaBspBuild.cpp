#if defined(WOWDECOMP_TOOL_FEATURES)

#include <cmath>
#include <cstdint>
#include "WoW/Common/AaBsp.h"
#include "WoW/Common/Types.h"
#include "Engine/Source/Storm/H/storm.h"

// Tool features: the CAaBsp tree builder.
//
// Source: 0.5.3 WoW\Common\AaBsp.cpp (WoWClient.exe 3368, 0x006ACB50..0x006AD770). The 0.5.3 client links the whole
// file but only ever queries trees loaded from WMO groups (CAaBsp::Set from MOBN/MOBR); CAaBsp::Create has no caller.
// It is the art-tool side: the WMO exporter builds each group's axis-aligned BSP over its collision faces with it.
// The 3.3.5 release build compiled the builder out, yet CAaBsp keeps every field it uses (m_nodeSize, m_nodeNext,
// m_nodeFaceIndicesSize/Next, m_buildFaceIndices*, m_treeDepth, m_avgNodeFaces), and CAaBspNode, the node and face
// index encoding and the WMO chunks are unchanged since 0.5.3, so the 0.5.3 code is restored here as is.
//
// The tree: interior nodes split on an axis-aligned plane (m_flags = axis, m_planeDist), children in m_posChild /
// m_negChild (0xFFFF = none); leaves (Flag_Leaf) list their faces in m_nodeFaceIndices[m_faceStart..+m_nFaces]. A face
// straddling a plane goes to both sides. Recursion stops at depth 8 or at 16 faces or fewer (0.5.3 limits; the 3.3.5
// data was built by later tools whose limits are unknown, so trees built here need not match shipped ones exactly).

// 0.5.3 asserts (the release build has no assert macro; the tool code keeps the original SErrDisplayError calls).
#define AABSP_ASSERT(expr) ((expr) ? (void)0 : (void)SErrDisplayError(STORM_ERROR_ASSERTION, __FILE__, __LINE__, #expr, 0, 1, 0))

// Capacities: 0.5.3 allocates the work buffers at their maximum size up front.
static const uint32_t AABSP_MAX_NODES = 0x10000; // node indices are 16-bit
static const uint32_t AABSP_MAX_NODE_FACE_INDICES = 0x40000;
static const uint32_t AABSP_MAX_BUILD_FACE_INDICES = 0x200000;
static const uint32_t AABSP_MAX_VERTICES = 0x40000;
static const uint32_t AABSP_MAX_FACE_VERTEX_INDICES = 0x40000;
static const uint32_t AABSP_MAX_DEPTH = 8;
static const uint32_t AABSP_MAX_LEAF_FACES = 16;

// TOOL: 0.5.3 0x00FACA28 (CAaBsp::s_axisNormalTable)
const C3Vector CAaBsp::s_axisNormalTable[3] = {
    C3Vector(1.0f, 0.0f, 0.0f),
    C3Vector(0.0f, 1.0f, 0.0f),
    C3Vector(0.0f, 0.0f, 1.0f),
};

// Signed distance of a vertex from the axis plane, the way 0.5.3 computes it (a full dot product with the axis normal).
static inline float AxisPlaneDistance(uint32_t axis, const C3Vector& v, float dist) {
    const C3Vector& n = CAaBsp::s_axisNormalTable[axis];
    return n.x * v.x + n.y * v.y + n.z * v.z - dist;
}

// TOOL: 0.5.3 0x006ACB50
// Builds the tree over nFaceVertexIndices / 3 triangles. The builder keeps the vertex and index pointers (it does not
// copy them) and owns the node and face index arrays (m_bFree), like a tree loaded and then rebuilt in the tools.
void CAaBsp::Create(C3Vector* vertices, uint32_t nVertices, uint16_t* faceVertexIndices, uint32_t nFaceVertexIndices) {
    AABSP_ASSERT(vertices);
    AABSP_ASSERT(nVertices < AABSP_MAX_VERTICES);
    AABSP_ASSERT(faceVertexIndices);
    AABSP_ASSERT(nFaceVertexIndices < AABSP_MAX_FACE_VERTEX_INDICES);

    this->m_nFaceVertexIndices = nFaceVertexIndices;
    this->m_bFree = 1;
    this->m_vertices = vertices;
    this->m_nVertices = nVertices;
    this->m_faceVertexIndices = faceVertexIndices;

    CAaBox bounds;
    bounds.b = C3Vector(3.4028235e38f, 3.4028235e38f, 3.4028235e38f);
    bounds.t = C3Vector(-3.4028235e38f, -3.4028235e38f, -3.4028235e38f);

    for (uint32_t i = 0; i < nVertices; i++) {
        const C3Vector& v = vertices[i];
        bounds.b.x = v.x < bounds.b.x ? v.x : bounds.b.x;
        bounds.b.y = v.y < bounds.b.y ? v.y : bounds.b.y;
        bounds.b.z = v.z < bounds.b.z ? v.z : bounds.b.z;
        bounds.t.x = v.x > bounds.t.x ? v.x : bounds.t.x;
        bounds.t.y = v.y > bounds.t.y ? v.y : bounds.t.y;
        bounds.t.z = v.z > bounds.t.z ? v.z : bounds.t.z;
    }

    // 0.5.3 calls CAaBox::Bounding(vertices, nVertices); 3.3.5 has no such helper, the loop above is its equivalent.
    this->m_aaBox = bounds;

    if (!this->m_nodes) {
        this->m_nodes = static_cast<CAaBspNode*>(SMemAlloc(AABSP_MAX_NODES * sizeof(CAaBspNode), __FILE__, __LINE__, 0));
        AABSP_ASSERT(this->m_nodes);
    }

    this->m_nodeSize = AABSP_MAX_NODES;
    this->m_nodeNext = 0;
    this->m_nNodes = 0;

    if (!this->m_nodeFaceIndices) {
        this->m_nodeFaceIndices = static_cast<uint16_t*>(SMemAlloc(AABSP_MAX_NODE_FACE_INDICES * sizeof(uint16_t), __FILE__, __LINE__, 0));
        AABSP_ASSERT(this->m_nodeFaceIndices);
    }

    this->m_nodeFaceIndicesSize = AABSP_MAX_NODE_FACE_INDICES;
    this->m_nodeFaceIndicesNext = 0;
    this->m_nNodeFaceIndices = 0;

    if (!this->m_buildFaceIndices) {
        this->m_buildFaceIndices = static_cast<uint16_t*>(SMemAlloc(AABSP_MAX_BUILD_FACE_INDICES * sizeof(uint16_t), __FILE__, __LINE__, 0));
        AABSP_ASSERT(this->m_buildFaceIndices);
    }

    this->m_buildFaceIndicesSize = AABSP_MAX_BUILD_FACE_INDICES;
    this->m_buildFaceIndicesNext = 0;

    // The root works on every face: 0, 1, 2, ... nFaces - 1.
    uint32_t nFaces = nFaceVertexIndices / 3;
    uint16_t* faces = this->AllocBuildFaceIndices(nFaces);

    for (uint16_t i = 0; i < nFaces; i++) {
        faces[i] = i;
    }

    this->BuildTree(faces, nFaces);
    this->FreeBuildFaceIndices(nFaces);

    // BuildTree sums the leaf face counts into m_avgNodeFaces; the average is over all nodes, interior ones included.
    this->m_rootNode = this->m_nodes;
    this->m_avgNodeFaces /= this->m_nNodes;
}

// TOOL: 0.5.3 0x006ACF10
// The build face index buffer is a stack: each BuildTree level pushes its two partitions and pops them on return.
uint16_t* CAaBsp::AllocBuildFaceIndices(uint32_t count) {
    AABSP_ASSERT(count + this->m_buildFaceIndicesNext < AABSP_MAX_BUILD_FACE_INDICES);

    uint16_t* indices = &this->m_buildFaceIndices[this->m_buildFaceIndicesNext];
    this->m_buildFaceIndicesNext += count;
    return indices;
}

// TOOL: 0.5.3 0x006ACF60
void CAaBsp::FreeBuildFaceIndices(uint32_t count) {
    this->m_buildFaceIndicesNext -= count;
}

// TOOL: 0.5.3 0x006ACF80
uint16_t CAaBsp::AllocNode() {
    AABSP_ASSERT(this->m_nNodes < AABSP_MAX_NODES);

    uint16_t node = static_cast<uint16_t>(this->m_nodeNext);
    this->m_nodeNext++;
    this->m_nNodes++;
    return node;
}

// TOOL: 0.5.3 0x006ACFC0
uint32_t CAaBsp::AllocNodeFaceIndices(uint32_t count) {
    AABSP_ASSERT(count + this->m_nodeFaceIndicesNext < AABSP_MAX_NODE_FACE_INDICES);

    uint32_t start = this->m_nodeFaceIndicesNext;
    this->m_nodeFaceIndicesNext += count;
    this->m_nNodeFaceIndices += count;
    return start;
}

// TOOL: 0.5.3 0x006AD010
// Returns the index of the node built for these faces. Nodes are allocated depth first, parent before children, so
// node 0 is the root.
uint16_t CAaBsp::BuildTree(uint16_t* buildFaceIndices, uint32_t count) {
    if (this->m_treeDepth >= AABSP_MAX_DEPTH || count <= AABSP_MAX_LEAF_FACES) {
        uint16_t leafIndex = this->AllocNode();
        CAaBspNode* leaf = &this->m_nodes[leafIndex];
        leaf->m_flags = CAaBspNode::Flag_Leaf;
        leaf->m_posChild = CAaBspNode::Flag_NoChild;
        leaf->m_negChild = CAaBspNode::Flag_NoChild;
        leaf->m_faceStart = this->AllocNodeFaceIndices(count);
        leaf->m_nFaces = static_cast<uint16_t>(count);
        leaf->m_planeDist = 0.0f;

        for (uint32_t i = 0; i < leaf->m_nFaces; i++) {
            this->m_nodeFaceIndices[leaf->m_faceStart + i] = buildFaceIndices[i];
        }

        this->m_avgNodeFaces += count;
        return leafIndex;
    }

    uint32_t axis;
    float dist;
    this->ChoosePlane(axis, dist, buildFaceIndices, count);

    uint16_t nodeIndex = this->AllocNode();
    CAaBspNode* node = &this->m_nodes[nodeIndex];
    node->m_flags = static_cast<uint16_t>(axis);
    node->m_planeDist = dist;
    node->m_faceStart = 0;
    node->m_nFaces = 0;

    uint16_t* posIndices = this->AllocBuildFaceIndices(count);
    uint16_t* negIndices = this->AllocBuildFaceIndices(count);
    uint32_t posCount = 0;
    uint32_t negCount = 0;
    this->PartitionFaceList(axis, dist, buildFaceIndices, count, posIndices, posCount, negIndices, negCount);

    this->m_treeDepth++;

    // node may move if m_nodes were reallocated in a later version; 0.5.3 allocates m_nodes once, so it stays valid.
    node->m_posChild = posCount ? this->BuildTree(posIndices, posCount) : static_cast<uint16_t>(CAaBspNode::Flag_NoChild);
    node = &this->m_nodes[nodeIndex];
    node->m_negChild = negCount ? this->BuildTree(negIndices, negCount) : static_cast<uint16_t>(CAaBspNode::Flag_NoChild);

    this->m_treeDepth--;
    this->FreeBuildFaceIndices(count);
    this->FreeBuildFaceIndices(count);

    return nodeIndex;
}

// TOOL: 0.5.3 0x006AD1A0
void CAaBsp::GenBoundingBox(CAaBox& aaBox, uint16_t* buildFaceIndices, uint32_t count) {
    aaBox.t = C3Vector(-3.4028235e38f, -3.4028235e38f, -3.4028235e38f);
    aaBox.b = C3Vector(3.4028235e38f, 3.4028235e38f, 3.4028235e38f);

    for (uint32_t i = 0; i < count; i++) {
        const uint16_t* indices = &this->m_faceVertexIndices[buildFaceIndices[i] * 3];

        for (uint32_t j = 0; j < 3; j++) {
            const C3Vector& v = this->m_vertices[indices[j]];
            aaBox.b.x = v.x <= aaBox.b.x ? v.x : aaBox.b.x;
            aaBox.b.y = v.y <= aaBox.b.y ? v.y : aaBox.b.y;
            aaBox.b.z = v.z <= aaBox.b.z ? v.z : aaBox.b.z;
            aaBox.t.x = v.x >= aaBox.t.x ? v.x : aaBox.t.x;
            aaBox.t.y = v.y >= aaBox.t.y ? v.y : aaBox.t.y;
            aaBox.t.z = v.z >= aaBox.t.z ? v.z : aaBox.t.z;
        }
    }
}

// TOOL: 0.5.3 0x006AD330
// Tries integer plane positions on each axis across the faces' bounding box (about 16 steps per axis) and keeps the
// lowest score: |front - back| + 2 * straddling + coplanar. A vertex on the plane counts for both sides, so a face is
// coplanar when all three vertices are on it, front or back when all three are on or past that side.
void CAaBsp::ChoosePlane(uint32_t& bestAxis, float& bestDist, uint16_t* buildFaceIndices, uint32_t count) {
    uint32_t bestPlaneAxis = 0;
    float bestPlaneDist = 0.0f;
    float bestPlaneScore = 999999.88f;

    CAaBox aaBox;
    this->GenBoundingBox(aaBox, buildFaceIndices, count);

    const float* boxMin = &aaBox.b.x;
    const float* boxMax = &aaBox.t.x;

    for (uint32_t axis = 0; axis < 3; axis++) {
        // 0.5.3 rounds with the x87 "subtract 0.5, round to nearest" trick, which floors.
        int32_t minDist = static_cast<int32_t>(std::floor(boxMin[axis]));
        int32_t maxDist = static_cast<int32_t>(std::floor(boxMax[axis]));
        int32_t step = static_cast<int32_t>(std::floor((boxMax[axis] - boxMin[axis]) * 0.0625f));

        if (step < 1) {
            step = 1;
        }

        for (int32_t dist = minDist; dist <= maxDist; dist += step) {
            float planeDist = static_cast<float>(dist);
            uint32_t on = 0;
            uint32_t front = 0;
            uint32_t back = 0;
            uint32_t straddle = 0;

            for (uint32_t i = 0; i < count; i++) {
                const uint16_t* indices = &this->m_faceVertexIndices[buildFaceIndices[i] * 3];
                uint32_t frontVerts = 0;
                uint32_t backVerts = 0;
                uint32_t onVerts = 0;

                for (uint32_t j = 0; j < 3; j++) {
                    float d = AxisPlaneDistance(axis, this->m_vertices[indices[j]], planeDist);

                    if (d > 0.0f) {
                        frontVerts++;
                    } else if (d >= 0.0f) {
                        frontVerts++;
                        backVerts++;
                        onVerts++;
                    } else {
                        backVerts++;
                    }
                }

                if (onVerts == 3) {
                    on++;
                } else if (frontVerts == 3) {
                    front++;
                } else if (backVerts == 3) {
                    back++;
                } else {
                    straddle++;
                }
            }

            // 0.5.3 takes |front - back| of the unsigned counts, so a plane with more faces behind than in front
            // wraps to ~4e9 and never wins; kept as is so trees match the 0.5.3 builder.
            float score = static_cast<float>(std::fabs(static_cast<double>(front - back)) + static_cast<double>(2 * straddle) + static_cast<double>(on));

            if (score != 0.0f && score < bestPlaneScore) {
                bestPlaneScore = score;
                bestPlaneAxis = axis;
                bestPlaneDist = planeDist;
            }
        }
    }

    bestAxis = bestPlaneAxis;
    bestDist = bestPlaneDist;
}

// TOOL: 0.5.3 0x006AD640
// Faces with no vertex behind the plane go to posIndices, faces with no vertex in front to negIndices, all others to
// both. A vertex on the plane counts for both sides, so a face touching the plane goes to both children (unlike
// ChoosePlane's scoring, which counts it as front or back; kept as in 0.5.3).
void CAaBsp::PartitionFaceList(uint32_t axis, float dist, uint16_t* buildFaceIndices, uint32_t count, uint16_t* posIndices, uint32_t& posCount, uint16_t* negIndices, uint32_t& negCount) {
    for (uint32_t i = 0; i < count; i++) {
        uint16_t face = buildFaceIndices[i];
        const uint16_t* indices = &this->m_faceVertexIndices[face * 3];
        uint32_t front = 0;
        uint32_t back = 0;

        for (uint32_t j = 0; j < 3; j++) {
            float d = AxisPlaneDistance(axis, this->m_vertices[indices[j]], dist);

            if (d > 0.0f) {
                front++;
            } else if (d < 0.0f) {
                back++;
            } else {
                front++;
                back++;
            }
        }

        if (back == 0) {
            posIndices[posCount++] = face;
        } else if (front == 0) {
            negIndices[negCount++] = face;
        } else {
            posIndices[posCount++] = face;
            negIndices[negCount++] = face;
        }
    }
}

#endif
