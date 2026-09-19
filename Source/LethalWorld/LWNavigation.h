#pragma once

#include "CoreMinimal.h"

namespace LWNavigation
{
    // Pure, deterministic XY route through LWGen's road network (world units).
    // Projects both ends onto the nearest roads and includes their local offroad legs.
    // Complete is true only when the returned path reaches the exact Goal; Start == Goal
    // returns one point. An empty path signals invalid input or an unavailable connection.
    // Each call gathers a padded region bounding box: at most 12 per axis, 64 total.
    // Distant routes end at an intermediate regional hub strictly closer to Goal, with
    // Complete=false. Recompute from the moving player's position toward the SAME Goal.
    // Game-thread API; a bounded topology cache includes seed and generation settings.
    // No global shortest-path guarantee outside that box.
    LETHALWORLD_API TArray<FVector2D> FindPath(
        FVector2D Start, FVector2D Goal, int32 Seed, bool* Complete = nullptr);
}
