# Loading screens — update 79

Six full-screen illustrations cycle independently from 81 messages (57 gameplay tips and 24 lore entries). Four illustrations were generated for this update: the shelter, rural road, occupied mall and Lake Ontario crossing. Two existing menu paintings round out the collection.

- Artwork changes every 18 seconds with a 0.9-second dissolve and very slow movement.
- Messages change every 14 seconds with a short fade-in.
- The two collections are independently shuffled at startup. Each loading screen continues after the last displayed entry, so consecutive short loads do not keep showing the same image and tip. Every entry is used before its shuffled order cycles.
- Pictures fill the viewport without stretching, including ultrawide displays. Text wraps inside a shaded lower region.
- Reduced-motion mode removes movement and dissolves. High-contrast mode adds a solid dark message backing.
- Loading completion never waits for an artwork or message cycle. The existing map-load minimum of half a second is unchanged; manual scopes add no minimum.

## Content editing

`Content/Loading79/Messages.csv` contains `id,category,text`. Use unique IDs, quote text containing commas, and keep messages below 230 characters for the current layout (the reader rejects entries over 400 characters). No input keys are hardcoded in the tips, so custom bindings and controllers remain valid.

`Content/Loading79/Images.csv` contains `id,file`. Add a PNG to the same directory and reference its filename. IDs and filenames must be unique. Nested paths are intentionally rejected. Artwork dimensions are read from each file; images should preferably be at least 1600 pixels wide. Rebuild the Editor target after adding files so runtime-dependency staging includes them. Restart the game after editing the collection.

The bank is cached before expensive loads, and no UObject, world, file read, or texture decode is performed by the loading widget's paint callback. Rotation uses a monotonic clock, independent of game ticks. The existing new-game, save restore, underground, campaign-transfer and PreLoadMap loading paths all use this widget. Missing/broken images are skipped; a missing message bank falls back to a built-in survival tip.

Lore follows `Docs/Campaign76/Canon.md`: autumn 2030, attacks in September 2028, a collapsed United States and a functioning Canada. Hidden campaign history, character outcomes and late revelations are excluded.

## Art provenance

Generated with the built-in imagegen tool on 2026-09-24. Full prompts are in `ArtSource/Loading79/prompts.json`. Generated originals remain in the tool's output directory; project copies are in `Content/Loading79`.

| Project image | Original generated filename |
| --- | --- |
| Shelter.png | exec-ec2559da-76df-4160-a91a-7248a5c16eab.png |
| Road.png | exec-294fdafa-0b39-466b-af5e-54351ca3ee35.png |
| Mall.png | exec-39834eae-e903-4dcb-83b0-d62c85665760.png |
| Ontario.png | exec-95ea8e43-6902-4d2e-9ece-009df5befdf2.png |

Reused project artwork: `ArtSource/World78/T_Menu78.png` as Roadside.png and `ArtSource/UI55/T_Menu55.png` as Diner.png.

## Validation

- Editor Development build passed: `Saved/BuildLoading79Release.log`. No package was built.
- Four native automation tests passed: `Saved/NativeLoading79.log` (`LethalWorld.Loading79`). Coverage includes bank integrity, CSV/fallback handling, shuffled rotation, clock boundaries, reduced-motion behavior and aspect ratios from 4:3 through 32:9.
- Rendered smoke checks passed at 1600x900 and 2560x1080, 10 checks each: `Saved/Loading79Smoke.log` and `Saved/Loading79Ultrawide.log`.
- A real MoviePlayer scope blocked the game thread for 20 seconds. Independent rendering, artwork changes and tip changes all continued; completion and short consecutive loads returned promptly.
- Actual new-game initialization rendered the loading screen and resumed into the opening dialogue with the mouse cursor available.
- Reviewed artwork, wrapping, gradient and high-contrast screenshots. `LoadingScreen.png` shows the final ultrawide layout. All six screenshots and the accessible variant are in `Saved/ScreenshotsV17/Loading79_*.png`.
- Original user settings were restored and verified by matching hashes after test processes exited.

The smoke run uses `-game -LWV17Smoke -LWUI46Smoke -LWLoading79Smoke -LWLoading45Smoke`. It operates on the existing isolated automation save paths. Real shipping-package staging has not been executed; the Editor build receipt includes all six PNGs and both CSVs as UFS runtime dependencies.
