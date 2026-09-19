# UI click attack fix

UI button presses now remain owned by the UI even when their callback closes a dialogue or menu before the weapon input binding runs. Attacks are blocked while modal UI is open and until the consumed press is released. A same-frame guard also prevents release/dispatch ordering from leaking a shot. Opening UI stops automatic fire, aiming, and sprinting. Attack release handling remains active while paused.

The guard precedes firearm, melee, and unarmed attack dispatch. Normal gameplay attacks resume on a fresh press after release.

Validation: LethalWorldEditor Win64 Development compiled successfully. The rendered Unreal gameplay regression passed 20 checks with zero failures, covering both callback orders, dialogue close, NPC health and hostility, modal panels, held clicks across frames, and fresh gameplay firing. Logs: Saved/BuildUIClick.stdout.log and Saved/UIClickRegression.log.

Run the focused gameplay suite with UnrealEditor-Cmd.exe, the project and /Game/Maps/Wasteland, plus -game -LWV17Smoke -LWUIClickSmoke -LWAudioSmoke -RenderOffscreen -unattended. This uses the existing isolated AutomationV17 save slot.

No packaged build was created.
