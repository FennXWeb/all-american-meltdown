# Update 58 — Controller input and scalable rendering

## Controller

Windows gamepad input uses Unreal's standard gamepad keys (Xbox/XInput layout). Controllers exposed through Steam Input can use the same layout. Native PlayStation-specific drivers, adaptive triggers, and PlayStation glyphs are not included.

| Control | On foot | Driving |
|---|---|---|
| Left stick | Analog movement | Steering; forward/back also available |
| Right stick | Look | Look around cabin |
| RT / LT | Fire / aim | Accelerate / brake and reverse; fire / aim from gunner seat |
| A | Jump / vault / climb | Handbrake; ascend in helicopter |
| B | Crouch; sprint-slide; hold prone | Descend in helicopter |
| X | Reload | Ignition or turret reload |
| Y | Interact | Use targeted control or exit |
| LS click / RS click | Sprint / gun bash | — |
| RB / D-pad left/right | Cycle weapons or quick-loot rows | Cycle seats |
| D-pad up/down | Heal / flashlight | Heal / headlights |
| View / Menu | Inventory / pause | Inventory / pause |

Hold **LB** for secondary actions: up map, down inventory, left cargo, right radio, A take all, B open full container, X fire mode/wipers, Y night vision, LS lean left, RS lean right. In bunker build mode, LS moves, RS looks, A/B raise/lower, RT places, RB selects, X rotates, LB+B dismantles.

While driving, hold **LB+RB** and use the D-pad to adjust the seat forward/back and up/down. Paused-menu cursor movement uses real elapsed time rather than paused game time.

Menus: LS moves the cursor; D-pad navigates controls where directional navigation is available; A selects or holds a drag; B goes back; triggers scroll/zoom. RB is the secondary mouse action (use/equip in inventory, rotate in workbench). LB+RB pans the workbench, and LB holds the quick-transfer modifier. A virtual text-entry panel supports character names, numeric map seeds, and custom weapon names: D-pad chooses characters, A types, X deletes, B finishes.

Advanced Graphics page 4 includes independent controller look speed, dead zone, inverted Y and cursor speed. Controller held inputs are released on UI transitions, focus flush and device disconnection, preventing stuck movement/fire. Mouse/keyboard bindings remain separate.

## Graphics

Advanced Graphics now has four pages and six presets: Minimum, Low/Laptop, Balanced, High, Ultra and Cinematic. Individual controls include texture streaming budget, streamed chunk radius, mesh detail distance, volumetric fog, contact shadows, ground cover, and standard/virtual/no shadows, alongside the existing quality controls. Manual render scale ranges from 35–150%; DLSS chooses its own appropriate scale.

Minimum uses a 256 MB streaming pool, one-chunk radius, reduced mesh distance, 40% render scale, a 30 FPS cap and no dynamic shadows, volumetric fog or Lumen. Ultra/Cinematic increase streaming/detail budgets and use hardware ray-traced Lumen where supported, with a software fallback. These are workload controls, not a guarantee of a particular frame rate or support for hardware below Unreal's requirements. Low-end hardware has not been physically benchmarked.

For a DirectX 11 fallback, run `Tools/LaunchLowSpec58.ps1`, optionally passing `-Editor` with the local UnrealEditor.exe path. It starts the project directly at 720p using `-dx11 -LWLowSpec58`; it does not package. For an eventual packaged executable, the same arguments select this startup path. SM5 is included as a Windows targeted shader format.

## NVIDIA integration

Official [NVIDIA DLSS Unreal plugin](https://developer.nvidia.com/rtx/dlss), version **8.8.0 for UE 5.8**, downloaded from NVIDIA on September 19, 2026. DLSS, Streamline Core, DLSS Frame Generation and Reflex are enabled as project plugins. Vendor source, runtime libraries and local precompiled files are under `Plugins/`; NVIDIA's setup guide and license are in `Docs/NVIDIA58/`.

Download SHA-256: `05830BA47E5A34C402702DB127D5876E024C52583F34F69582A6C6CF68E76E0A`.

`Tools/InstallDLSS58.ps1` restores the exact official distribution, including precompiled plugin files, after a clean checkout. It verifies the archive checksum before extracting. Vendor runtime DLLs are exempted from the project's generated-binaries ignore rule.

The settings use NVIDIA's support queries rather than GPU-name guesses. Available options include DLSS Quality/Balanced/Performance/Ultra Performance, DLAA, supported 2x–6x/dynamic frame-generation modes, Reflex and conditional ray reconstruction. Unsupported saved requests fall back to TSR or disable frame generation. Frame generation enables Reflex and disables VSync only if the plugin reports that it is required. UE 5.8 console-variable priorities are initialized before calling the vendor frame-generation setter.

Ray reconstruction additionally requires DLSS, hardware RT and a positive runtime capability result. It is not advertised as active when those requirements fail. The camera-decay filter remains separately adjustable in the main settings screen.

## Verification

Controller validation injects Unreal gamepad events into the actual player input path; it is not a physical-controller hardware test. The `-LWV17Smoke -LWUI46Smoke -LWController58Smoke` scenario checks directional menu focus, safe confirmation, analog motion/dead zones, crouching, inventory cursor/dragging, modifiers, held-input cleanup, virtual text entry, graphics budgets and actual DLSS/frame-generation state.

The final DirectX 12 scenario passed all **23 checks** on an RTX 5070 (`Saved/Smoke58c.log`), including DLSS Quality and 2x frame generation being enabled in the renderer. DirectX 11 passed all **22 applicable checks** (`Saved/Smoke58DX11.log`), with frame generation correctly unavailable. The nine existing inventory regression tests passed. Screenshots of the menus and inventory were reviewed for layout. Multi-frame modes above 2x are capability-filtered but have not each been separately exercised.

The editor target compiled successfully (`Saved/Build58j.log`). The keyboard-binding regression also passed (`Saved/Tests58b.log`). User settings were restored from the pre-test backup after validation.

No packaged build is created.
