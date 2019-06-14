#ifndef SMPENGINE_DEBUG_OVERLAY_H
#define SMPENGINE_DEBUG_OVERLAY_H

#include "engine/overlay.h"
#include "engine/sprite.h"




void DebugOverlay_Init(Overlay *this, Sprite *player,
		SpriteRegistry *overlayReg, Graphics *graphics);


void DebugOverlay_Update(Overlay *this, Sprite *player);
















#endif
