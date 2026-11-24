#ifndef WOW_SOURCE_CLIENTTOOL_H
#define WOW_SOURCE_CLIENTTOOL_H

#ifdef WOWDECOMP_TOOL_FEATURES

// Tool access to the client engine's data and render services. The caller owns the application lifecycle.
// Set the path before loading cvars, then open data with the configured locale and archive settings.
void ClientToolSetDataPath(const char* basePath);
void ClientToolOpenData();
void ClientToolCloseData();

// Call from the interactive event context after creating the Gx device.
void ClientToolInitializeRenderServices();
void ClientToolDestroyRenderServices();

#endif

#endif
