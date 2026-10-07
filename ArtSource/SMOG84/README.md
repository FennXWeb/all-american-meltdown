# SMOG launcher artwork

Three original assets generated with the built-in `image_gen` tool on October 7, 2026. Exact prompts are preserved in `prompts.json`. The header is promotional illustration, not an in-game screenshot or a surveyed city view.

`header-source.png`, `logo-source.png`, and `icon-source.png` preserve the generated originals. Run `Tools/PrepareSMOGArt.ps1` to produce the repository-root SMOG files. This performs only format/size conversion and preserves the generated transparency.

- `smog_header.png`: 2400 × 1000.
- `smog_logo.png`: 1200 × 400 with alpha.
- `smog_icon.ico`: 16, 32, 48, and 256 px with alpha.

Root launcher artwork bypasses Git LFS because SMOG reads it through GitHub Contents. Authoring sources retain the repository's normal LFS rules.
