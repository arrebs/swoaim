#pragma once

void OverlayConfig_MarkDirty();
// nullptr means the active config path (AppPaths::defaultConfigFile()).
void OverlayConfig_TrySave(const char* filename = nullptr);
void OverlayConfig_SaveNow(const char* filename = nullptr);
