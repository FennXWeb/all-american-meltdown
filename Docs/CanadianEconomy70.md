# Canadian economy and Toronto (Update 70)

Canadian Dollars are a separate, persistent whole-dollar wallet in `RPG.Canada68.CanadianDollars`. Existing saves initialize it to zero. Canadian merchants buy and sell exclusively in CAD; an American credit balance cannot cover a CAD purchase. The inventory displays both balances; the compact HUD displays the local currency. Trader transfers (including shift-click) use the same currency-aware transaction path. Take-all remains disabled for traders.

## Toronto Currency Exchange

The exchange occupies the bank parcel at `LWGen::CanadaCity68() + (13500, -13500)`. Its clerk exchanges 100, 1,000, or all available currency in either direction. Guards and Canadian merchants can mark the exchange on the map. The bank has cashier counters/registers and waiting benches. Existing Canadian city positions/IDs are retained; Maplehaven is now Toronto.

The fictional mid-market rate interpolates smoothly between seed-dependent half-game-hour anchors in the range 0.85–1.65 CAD per credit. It is derived from saved world day/time, not wall-clock time or random calls during dialogue. The quote is live while world time advances, including the exchange screen. Pausing the game pauses the market. Each direction includes a 3% spread; whole-unit proceeds round down. A same-rate round trip loses value. Trades validate funds and overflow before changing either balance. Exchange confirmations show actual proceeds. Passport purchases at the American-side checkpoint remain priced in credits.

## Lake Ontario

The existing southern shoreline is retained. Water extends north near and across the eastern border, with an irregular northern shore south/east of Toronto. The main checkpoint and Toronto highway stay on land. Canadian terrain now uses the same water-depth function as the American terrain and map. Lake regions suppress scenic road segments, trees, and checkpoint structures instead of putting them underwater. Geography remains a compressed game layout, not a geographic survey.

## Verification

- Editor target only; no packaged build.
- `LethalWorld.Update70.CurrencyAndToronto`: conversion math, rejection/overflow, changing deterministic rates, save serialization, exchange placement, lake terrain and road exclusion.
- `LethalWorld.Update68.CountryAndStreaming` and Update69 atlas/route regression tests.
- `-LWV17Smoke -LWUI46Smoke -LWCurrency70Smoke`: in-engine clerk spawn and exchange, merchant purchase refusal without CAD, CAD purchase and resale, screenshots of exchange/shop/map.

Verified: editor build succeeded; all four Update68/69/70 automation tests passed. The in-engine economy test passed 11 checks. Logs: Saved/Canada70Automation.log and Saved/Currency70Smoke.log. Screenshots: Saved/ScreenshotsV17/Currency70_*.png. A follow-up UI pass places both wallets below the menu tabs.

Final UI verification: Saved/Build70b.log succeeded; Saved/Currency70SmokeFinal.log passed all 11 checks. Both balances and Toronto/Lake Ontario labels are visible. Original graphics settings restored after the test process exited.
