# Blood settling fix

Blood traces now query WorldStatic objects rather than using WorldStatic as a trace channel. Pawns, ragdolls and moving props cannot support permanent stationary splatter. Droplets keep falling until they hit fixed geometry, then flatten and align with the hit surface. Effects still airborne at their four-second simulation limit are hidden rather than frozen. Settled stains retain the existing 25-second cleanup lifetime and instanced rendering.

The Blood22 material must enable Instanced Static Mesh usage; Tools/fix_blood_material41.py updates that flag without rebuilding the user's material graph. The original content generator also now sets it.

The Gameplay40 rendered regression harness includes floor landing, rejection of a simulated-body collision object, continued falling beyond 1.5 seconds and hiding expired airborne droplets.

Validation: Editor build succeeded (Saved/BuildBlood41Final.log); material update succeeded (Saved/Blood41Material.log); rendered gameplay regression completed with 25 checks and zero failures (Saved/Blood41Smoke.log). The combat screenshot was inspected and shows dark red blood on the ground. No packaging.
