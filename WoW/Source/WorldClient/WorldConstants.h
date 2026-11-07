#ifndef WOW_SOURCE_WORLDCLIENT_WORLDCONSTANTS_H
#define WOW_SOURCE_WORLDCLIENT_WORLDCONSTANTS_H

// 3.3.5 CMapArea::BatchChunks uses a 16-element row stride and a 16 x 16 area grid.
static const int MAP_CHUNKS_PER_AREA_AXIS = 16;
// 16-axis chunk coordinates use the low four bits: 0xF (15).
static const int MAP_CHUNK_INDEX_MASK = MAP_CHUNKS_PER_AREA_AXIS - 1;
// 16 x 16 entries = 256 map chunks per area.
static const int MAP_CHUNKS_PER_AREA = MAP_CHUNKS_PER_AREA_AXIS * MAP_CHUNKS_PER_AREA_AXIS;

// 3.3.5 CMapArea::BatchChunks selects the four chunks in a 2 x 2 neighborhood.
static const int MAP_CHUNKS_PER_BATCH_AXIS = 2;
// 2 x 2 neighborhood = 4 candidate chunks.
static const int MAP_CHUNKS_PER_BATCH = MAP_CHUNKS_PER_BATCH_AXIS * MAP_CHUNKS_PER_BATCH_AXIS;
// 3.3.5 CMapRenderChunk::RenderPrepBufs stores at most two chunk pointers.
static const int MAP_CHUNKS_PER_RENDER_BATCH = 2;

// 3.3.5 terrain loops visit eight tile intervals along each chunk axis.
static const int TERRAIN_TILES_PER_CHUNK_AXIS = 8;
// The 3.3.5 vertex grid has nine corner samples along each axis.
static const int TERRAIN_VERTICES_PER_CHUNK_AXIS = TERRAIN_TILES_PER_CHUNK_AXIS + 1;
// Rows contain nine corner vertices followed by eight center vertices (17 total).
static const int TERRAIN_CHUNK_VERTEX_ROW_STRIDE = TERRAIN_VERTICES_PER_CHUNK_AXIS + TERRAIN_TILES_PER_CHUNK_AXIS;
// 3.3.5 CMapChunk::InitializeVertexGrid emits 9 x 9 corner vertices plus 8 x 8 centers = 145.
static const int TERRAIN_CHUNK_VERTEX_COUNT = TERRAIN_VERTICES_PER_CHUNK_AXIS * TERRAIN_VERTICES_PER_CHUNK_AXIS
    + TERRAIN_TILES_PER_CHUNK_AXIS * TERRAIN_TILES_PER_CHUNK_AXIS;
// 3.3.5 render buffers pair two chunks: 2 x 145 = 290 vertices.
static const int TERRAIN_RENDER_BATCH_VERTEX_COUNT = TERRAIN_CHUNK_VERTEX_COUNT * MAP_CHUNKS_PER_RENDER_BATCH;
// 8 x 8 terrain cells = 64 tiles per chunk.
static const int TERRAIN_TILE_COUNT_PER_CHUNK = TERRAIN_TILES_PER_CHUNK_AXIS * TERRAIN_TILES_PER_CHUNK_AXIS;
// 3.3.5 CMapChunk::CreateIndices emits twelve indices for each non-hole tile.
static const int TERRAIN_INDICES_PER_TILE = 12;
// Maximum per chunk: 8 x 8 tiles x 12 indices = 768 (the render buffer capacity).
static const int TERRAIN_INDEX_COUNT_PER_CHUNK = TERRAIN_TILE_COUNT_PER_CHUNK * TERRAIN_INDICES_PER_TILE;
// 3.3.5 render buffer capacity for two chunks: 2 x 768 = 1536 indices.
static const int TERRAIN_RENDER_BATCH_INDEX_COUNT = TERRAIN_INDEX_COUNT_PER_CHUNK * MAP_CHUNKS_PER_RENDER_BATCH;

// 3.3.5 MCLQ flag and terrain-liquid loops use an 8 x 8 tile grid per chunk.
static const int LIQUID_TILES_PER_CHUNK_AXIS = 8;
// 8 x 8 liquid cells = 64 tiles per chunk.
static const int LIQUID_TILE_COUNT_PER_CHUNK = LIQUID_TILES_PER_CHUNK_AXIS * LIQUID_TILES_PER_CHUNK_AXIS;
// Liquid vertex grids add the edge sample: 8 tile intervals + 1 = 9 per axis.
static const int LIQUID_VERTICES_PER_CHUNK_AXIS = LIQUID_TILES_PER_CHUNK_AXIS + 1;
// 3.3.5 CMapChunk::CreateLiquids sets tile bounds to 0..8; CreateVertXY adds the edge samples: 9 x 9 = 81.
static const int LIQUID_VERTEX_COUNT_PER_CHUNK = LIQUID_VERTICES_PER_CHUNK_AXIS * LIQUID_VERTICES_PER_CHUNK_AXIS;

static const float MD_AREA_POLY_SIZE = 533.33331f; // adt size
// half tile size : 266.66666f
static const float TERRAIN_SPEC_EXP = 20.0f;
static const float HALF_WORLD_SIZE = 17066.666f; // adt size * 32

static const float LIQUID_TILE_GRID_SIZE = 33.333332f; // Span of an 8 x 8 liquid tile grid.
static const float HALF_LIQUID_TILE_GRID_SIZE = 16.666666f;
static const float HALF_LIQUID_TILE_GRID_SIZE_SQ = 277.77777f;

// Keep the stored float values: calculating reciprocals can round differently.
static const float SMOLTILE_SIZE = 4.1666665f; // 0.5.3 symbol: MD_LIQUID_POLY_SIZE.
static const float OOSMOLTILE_SIZE = 0.240000010f; // 1.0 / 4.1666665f.
static const float OO_COORD_TO_CHUNK = 0.029999999f;
static const float OO_COORD_TO_CHUNK_RADIUS = 0.030000001f; // Distinct upward-rounded scale used before ceil.
static const float OO_COORD_TO_SUBCHUNK = 0.23999999f;

// Distance-to-bucket scale for world scene sorting.
static const float CBDIVISOR = 0.029999999f;

static const float OO_COORD_TO_SHADOW = 1.92f;
static const float HALF_SMOLTILE_SIZE = 2.0833333f;

#endif
