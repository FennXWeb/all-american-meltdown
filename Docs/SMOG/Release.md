# All American Meltdown — SMOG release

The portable Windows x64 build is distributed through [GitHub Releases](https://github.com/FennXWeb/all-american-meltdown/releases). Add `FennXWeb/all-american-meltdown` in SMOG and install the latest release asset matching `all-american-meltdown-*-windows-x64.zip`.

## Play and saves

Extract the entire ZIP before running `smog_launch.bat`, or let SMOG install and launch it. The launcher invokes the Shipping executable directly and waits for it to exit, so SMOG can track play time. Windows x64 and a DirectX 11/12-capable GPU are required. DirectX 12 is the default; `smog_launch.bat -d3d11` provides the compatibility path. Graphics presets and controller settings remain available in-game. Performance depends on hardware; no new minimum-hardware guarantee is made by this release.

Launcher saves, backups, settings, and logs live in `%LOCALAPPDATA%\AllAmericanMeltdown\Saved`, outside the versioned installation folder. Saved games are under its `SaveGames` subdirectory. Existing editor saves remain in the project's `Saved/SaveGames`; the release does not overwrite or automatically import them. To migrate, close the game and copy a backed-up set of compatible `.sav`, `.bak`, and `.meta` files into the launcher save directory.

App-local Visual C++ runtime files are staged beside the executable. The Unreal prerequisites installer is also included under `Engine/Extras/Redist/en-us` for machines that need additional prerequisites; it is not automatically run by the launcher.

## Files and artwork

The five files from the supplied [publishing contract](PUBLISHING.md) are present at both repository root and ZIP root. Root PNG/ICO artwork is ordinary Git content, not Git LFS pointers, and each file is below 8 MB. Generated source images and exact prompts are retained in `ArtSource/SMOG84`. The header is promotional illustration.

The release is published as a regular, non-prerelease GitHub Release so SMOG can discover it. Version 0.84.0 is a development snapshot with the regional world, settlements, campaign, vehicles, and aviation work currently in the project. It is not a claim that every feature has received a complete manual playthrough.

## Rebuild

With Unreal Engine 5.8, Visual Studio C++ tools, Python with Pillow, Git and Git LFS installed:

```powershell
git lfs pull
./Tools/InstallDLSS58.ps1
./Tools/PrepareSMOGArt.ps1
./Tools/PackageSMOG.ps1 -Engine 'G:\Epic\UE_5.8' -Version '0.84.0'
python Tools/ValidateSMOG.py --stage Builds/SMOG-0.84.0/Windows --zip Builds/Releases/all-american-meltdown-0.84.0-windows-x64.zip
```

The NVIDIA vendor SDK is restored through the checksum-verified installer above; vendor source is not republished in this repository. The portable build includes its required runtime libraries and license notice.

The validator checks metadata, generated alpha, icon sizes, the waiting launcher, archive paths, absence of saves/debug symbols, extraction limits, all ZIP CRCs, and GitHub's 2 GiB per-asset limit. It emits a SHA-256 file and build manifest. The ZIP command refuses to overwrite an existing artifact.

Publish the source commit and tag, upload the ZIP, checksum and manifest to a draft release, verify the remote files, then publish the draft. SMOG reads metadata/artwork from `main` and selects the latest published non-prerelease. Packaging/publication are explicit actions; this script does not schedule or automatically publish future builds.
