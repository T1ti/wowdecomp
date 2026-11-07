#ifndef WOW_SOURCE_WORLDCLIENT_WORLDCONSTANTS_H
#define WOW_SOURCE_WORLDCLIENT_WORLDCONSTANTS_H

static const float MD_AREA_POLY_SIZE = 533.33331f; // adt size
// half tile size : 266.66666f
static const float TERRAIN_SPEC_EXP = 20.0f;
static const float HALF_WORLD_SIZE = 17066.666f; // adt size * 32

static const float LIQUID_TILE_GRID_SIZE = 33.333332f; // 8 x 8 liquid tiles at SMOLTILE_SIZE.
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
