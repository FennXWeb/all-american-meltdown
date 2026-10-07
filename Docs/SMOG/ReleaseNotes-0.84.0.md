# All American Meltdown 0.84.0

Windows x64 development snapshot, including the current regional world, settlement, campaign, vehicle, and aviation systems.

- Map-data-based roads and shorelines across 36 cities, towns and villages, with an enlarged Syracuse and compressed regional travel.
- Twelve mapped communities with shops, civilian routines, and recruitable guns for hire.
- Rebuilt Syracuse Hancock Airport and functional airports at Buffalo, Rochester, Watertown, Griffiss, and Toronto.
- Private jet, 100-seat Airbus and luxury airliner, with manual flight, automatic takeoff/landing, and roaming autopilot.
- Walkable moving cabins, interactive seating/storage/shades/TVs, ownership, map tracking and airport recovery.
- High-altitude cloud layers, cockpit warnings, fuel endurance and revised explosions with lingering fire.
- Current inventory, combat, companion, settlement-building, story, saving, graphics and controller systems included.

## Install

In SMOG add `FennXWeb/all-american-meltdown`, then install the Windows ZIP. For a standalone install, extract the entire ZIP and run `smog_launch.bat`. Do not use GitHub's automatic source archives as a playable build.

Saves and settings are kept at `%LOCALAPPDATA%\AllAmericanMeltdown\Saved`; updating or uninstalling a SMOG build does not remove that directory. The ZIP includes app-local runtime dependencies and an optional Unreal prerequisite installer.

## Scope

The Windows Shipping build passes 100 packaged checks: 48 menu/launcher checks and 52 aviation/community checks. Saves were verified outside the installation directory. The release includes generated SMOG header, transparent logo and multi-size icon, plus launcher metadata and checksums.

This is a development snapshot. The airport interiors are playable adaptations of their references. Full manual campaign, long-session and 100-passenger stress testing remain outstanding. See the repository's `Docs/Update84-Implementation.md` and `Docs/SMOG/Release.md` for controls, systems, validation and known scope.
