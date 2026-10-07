# Update 83: road furniture, electric coach finish and Tab menus

- Electric motorhome factory panels now use a neutral pearl-white dielectric finish. Shell, cargo doors and slide-out paint share the finish. Glass, solar panels, interior upholstery and purchased garage paints retain their own materials. Existing vehicles receive the finish when loaded; their identity, storage and modifications are preserved.
- Unsignalized intersections use priority-road rules: the terminating arm of a T junction stops, while continuous roads have priority. Crossroads stop the secondary approaches. Companion/autopilot drivers obey the same rule as the signs. Removed unnecessary stop lines and the generic route-number signs attached to every traffic-light installation.
- Traffic-light housings and lenses are 20% smaller. Slender masts retain safe overhead clearance. Signals still block pedestrians and visibility/weapon traces but do not block vehicles, suspension probes or vehicle obstacle checks.
- Inventory, Map, Skills, Contracts, Crew, Collection and Settlements share a full-width canvas. Inventory panes, scrollbars, drag targets and action buttons reflow without stretching grid cells. The map uses the additional width. The card collection adds columns on wider displays. Perk/crew controls remain anchored to the right edge.

No save format migration or packaged build is required. Existing streamed road furniture is rebuilt on the next game/chunk load.

Validation:

- Editor Development target compiled successfully (`Saved/BuildUpdate83.log`).
- Junction/signal and stop-priority automation tests passed. The sampled Syracuse neighborhoods went from 1,376 stop signs to 580: a 57.8% reduction (`Saved/Update83Automation.log`).
- Final rendered regression: 184 checks passed, zero failures (`Saved/Update83Final.log`). Includes factory paint, signal suspension probes, chassis sweeps, all seven Tab pages, and real mouse clicks transferring and sorting container contents.
- Layouts visually reviewed at 1920x1080, 2560x1080 and 3200x900. Captures are in `Saved/Update83StandardCaptures`, `Saved/Update83WideCaptures` and `Saved/Update83SuperwideCaptures`.
- Initial testing caught retained per-instance physics filters; rebuilding already-created signal bodies corrected them. Final probes hit the road at Z=60010, and chassis sweeps through signal masts are unobstructed. Unrelated highway guardrails remain solid.
- Tests used isolated save slots. Original display settings restored afterward; Unreal left closed. No packaging performed.
