# Version control

GitHub repository: https://github.com/FennXWeb/all-american-meltdown (private).
Default branch: `main`.

Install Git and Git LFS before cloning:

```powershell
git lfs install
git clone https://github.com/FennXWeb/all-american-meltdown.git
```

Unreal assets, textures, audio, and model source files use Git LFS. Commit both the asset files and their source changes. Build output, caches, local saves, credentials, and temporary files are ignored. The application icon in Build/Windows is tracked.

To save and upload a change:

```powershell
git status
git add .
git commit -m "Describe the change"
git push
```

Use a branch for larger changes (`git switch -c feature/name`). Binary Unreal assets cannot be merged like code; coordinate edits when collaborating. Open the `.uproject` with Unreal Engine 5.8 after cloning and regenerate local project/build files as needed. No automatic packaging or build workflow is configured.
