# NPC animation

Residents, companions, raiders and ordinary zombies use `ULWNPCLife`, a cosmetic component that runs after their AI. Characters keep the existing seven collision/ragdoll parts. Nearby heads and limbs get procedural render copies; collision stays on the original components. This is procedural animation on the existing PS2 character assets, not a replacement skeletal rig or motion capture library.

## Behavior

- Twelve independently selected gesture silhouettes: explain, offer, point, shrug, count, reassure, wipe brow, check wrist, inspect hand, rub neck, stretch shoulder, fold arms. Speech makes gestures more expressive; quiet idles use smaller movements and longer intervals.
- Per-character random timing, gait phase, breathing, torso shifts, smooth acceleration lean, walk/run arm carriage, elbow bends and knee flexion.
- Conversation gaze, small listening nods, periodic eye blinks, brow tension, subtle mouth-corner expressions and jaw motion. Resident dialogue, raider voices and zombie vocalizations subscribe to the voice component's envelope. Where envelope data is unavailable, a syllabic rhythm runs only during the clip; missing audio uses a bounded text-duration fallback. This is amplitude-driven mouth animation, not phoneme recognition.
- Driver/passenger poses, combat aiming/reload motion, health-loss flinches and downed posture.
- Injury, severed limbs and death take priority. Deformed render copies follow the existing physical limbs. Passive mannequins and nonhumanoid creatures retain their specialized animation behavior.

## Cost and tuning

`ExpressionStrength`, `GestureStrength`, and `DetailDistance` are exposed on the LifeAnimation component. Vertex deformation defaults to 18 metres, at 30 Hz within 6.5 metres and 15 Hz farther out. A FIFO queue allows at most six character deformation updates per rendered frame. Distant characters retain part-based animation; beyond 50 metres the layer stops updating. Paused/menu gameplay does not advance the animation clock.

`Tools/prepare_npc_animation34.py` enables retained CPU mesh data on the twelve humanoid head/arm/leg assets. Run this after reimporting those models. Original material overrides and face vertex colors are retained by the procedural copies. The tool also creates the unlit M_NPCMouth34 cavity material.

## Verification

Automation: `LethalWorld.Animation.ArticulationAndBlink` checks eyelid timing, elbow direction, rigid upper-arm preservation, segment length and finite knee deformation over a range of angles.

Runtime gallery: `-LWV17Smoke -LWNPCLife34Smoke -LWAudioSmoke` checks creation of visual parts, collision preservation, independent gestures, mannequin stillness, speech/listening transitions and injury priority, and records close-up screenshots. Use with the existing Wasteland test map and `-RenderOffscreen`.

No packaging is required or performed by these tools.

Editor compilation passed. The full automation run passed 73 tests; the expanded runtime gallery passed 15 checks, including voice-envelope response and seven-body ragdoll simulation. Captures and logs are under Saved/ScreenshotsV17/NPC34_* and Saved/NPC34_*.
