# Build 0.8

Editor/source update only. No cooking or packaging was performed.

## Vehicles

Removed the seats baked into the old cabin mesh. Seat cushions, backrests and headrests now use the same positions as companion passengers. The sedan rear row fits inside the passenger cell, and extra rows follow the larger vehicle profiles. The dashboard and its interactions remain intact.

## POIs and storage

Furniture placement checks the actual mesh bounds, room walls, door approaches and previously placed furniture. Bedroom furniture is spaced separately, bathroom partitions precede furnishing, and strip-mall dividers are reserved before placing stock. Walls extend to the ceiling. Invalid placements are omitted rather than left clipping into other objects.

POIs no longer receive an automatic radio on every floor or beside outdoor ticket booths. The bunker communications room and settlement radio remain intentional fixtures.

Crates, shelves, cabinets, desks, service counters, barrels, lockers and refrigerators in POIs are searchable storage. Furniture in the bunker that represents storage also opens a persistent empty personal container. Resting, cooking, drinking and workbench interactions remain on their dedicated furniture.

The floating SALVAGE label and automatic light were removed from ordinary containers. Aim at storage and use E to search it; locks still require picking. Placement variation can omit optional stock and storage furniture. Newly generated containers can be empty, use the POI's loot context and retain higher-tier lock rewards. The contents persist rather than rerolling when a chunk reloads.

Previously saved loose POI crate contents migrate once into a surviving storage piece at that site, preserving item identities. Existing saves are not deleted. Fixed settlement perimeter walls were removed because they could block roads or remain after building rejection.

## Weather and time

The default complete day lasts 96 real minutes. `DayLengthMinutes` is configurable on ALWWorld under Weather. The clock and day counter persist with the survivor save, and time pauses with the player menu.

Sun angle, sunlight, ambient light, sky color and fog follow the time of day. Clear, overcast, fog, rain and thunderstorm conditions follow a seeded schedule with gradual transitions. Rain has visible streaks and a looping audio bed; buildings and enclosed vehicles shelter the player. Rain audio becomes quieter and muffled indoors. Storms include lightning flashes and thunder.

The Audio Manager exposes replaceable Rain and Thunder slots. `Tools/build_v8_content.py` imports the corrected cabin, weather audio and sky material. It does not package the project.

These are lightweight procedural rain and lighting effects; there is no accumulated snow, flooding, puddle simulation or tire traction change from weather.
