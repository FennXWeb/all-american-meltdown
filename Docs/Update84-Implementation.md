# Update 84 — New York, communities and aviation

## World

The regional layout now uses OpenStreetMap town streets and Natural Earth regional roads, shorelines and country boundaries. It covers 36 named New York cities, towns and villages, the northern story corridor and Toronto. Syracuse keeps its larger playable scale; travel outside it is compressed. Source data, licensing and reproduction steps are in [Geography84-Sources.md](Geography84-Sources.md).

Lake Ontario is between New York and southern Ontario; Onondaga Lake retains its detailed Syracuse shoreline. Canada uses a geographic polygon instead of a horizontal cutoff. Three guarded crossings occupy the Peace Bridge, Lewiston–Queenston and Thousand Islands areas. Roads preserve surveyed junctions, with short authored approaches joining disconnected extracts. Buildings reserve their footprints before roads and ordinary parcels are finalized.

Twelve mapped communities reuse settlement reputation, commerce and recruitment. They have indoor shops, clinics, houses, bars, workshop and garden destinations, street furnishings and 39–55 named residents each. Civilian routines alternate between home, work, public space and the bar. Three recruitable guns for hire remain at the bars. The northern campaign keeps its persistent IDs and branch state; its locations and ferry crossing follow the revised geography. Old northern campaign and Toronto saves migrate once, with associated vehicles, containers and player-built settlement pieces.

## Airports and aircraft

Syracuse Hancock has the surveyed two-concourse terminal footprint, two occupied levels, stair openings, check-in, baggage carousels, security lanes and gate lounges. Both active runways retain their approximately real lengths and surveyed directions. The interior furnishing is a playable adaptation, not a room-for-room survey. Buffalo, Rochester, Watertown, Griffiss and Toronto Pearson provide additional functional airports.

| Aircraft | Total seats | Cabin |
|---|---:|---|
| Private jet | 8 | Bed, minibar, refrigerator, TV, adjustable seating and independent luggage bins |
| Airbus 100 | 100 | Two cockpit seats and 98 cabin seats, reclining backs, individual window shades and carry-on compartments |
| Aurelia Sky Residence | 16 | Dark lower fuselage and swept winglets, lounge, bar, refrigerator, two private bed suites with doors, TVs and configurable lighting |

The models use continuous shaped fuselages, airfoil wings, glazed openings, separate landing gear, seats and moving cabin parts. The source Blender file, FBXs, generated material atlas and import scripts are retained under `ArtSource/Aviation84` and `Tools`. Aircraft audio has replaceable turbine and cockpit-warning catalog slots.

## Flying and cabin use

- Use the aircraft door to board, then interact with an individual seat or the flight computer. Claiming at the computer adds ownership and map tracking.
- The flight computer engages autopilot. With a waypoint, it chooses the closest eligible runway, taxis, takes off, flies the approach and lands. Without a waypoint it takes off and roams; low fuel initiates a diversion. Canadian entry still requires authorization. An occupied runway triggers a go-around.
- Default manual controls: W/S pitch, A/D bank and ground steering, Q/E rudder, Shift/C increase/decrease thrust, R engine, G landing gear. Space brakes on the ground; when parked or under autopilot it lets you stand. Controls follow the existing remapping/controller system.
- To walk in flight, engage autopilot before leaving the seat. Movement, furniture collision, camera position and interaction traces use the aircraft's local frame, so banking and translation carry the player once. Changing seats, opening storage, operating shades, using TV flight information/exterior-camera modes and controlling lighting continue during autopilot.
- Return to the pilot seat before disengaging autopilot. Land and stop before opening the exit. Gear-up, off-runway and excessive-sink-rate landings can destroy the aircraft.
- Full fuel capacity is calibrated to one real hour of manual operation or five hours under autopilot. Cockpit alarms and display warnings report airframe damage and low fuel.
- Aircraft services at any airport recover, repair and refuel claimed aircraft. Recovery preserves the VIN and individual storage records, and refuses to recall an aircraft currently occupied or in use. Aircraft use airport recovery rather than bunker parking bays.

Cabin seat/recline, shades, lighting, suites, storage, autopilot phase and the player's cabin-local position persist in saves. High-altitude flight shows a layered cloud bed and a darker upper sky. A lightweight regional terrain layer covers distances beyond the detailed streamed chunks.

## Explosions

Cars, aircraft, fuel pumps, missiles and explosive rounds share layered fireballs, sparks, smoke and ground aftermath flames. Aircraft crashes use a larger blast radius and prolonged fire. Fire damages nearby actors, expires and does not remain floating after high-altitude airbursts. Concurrent effects are bounded, and particle topology stays fixed to avoid repeated render-resource recreation.

## Validation

The Development Editor build passes (`Saved/BuildUpdate84q.log`). All 11 geography, route, border, currency, campaign continuity, aircraft-contract and migration automation tests pass (`Saved/TestsUpdate84e.log`). The atlas contains 30,020 plots and reports zero road/footprint conflicts; complete routes connect Buffalo to all other 35 named towns. The final rendered aviation/community run passes all 52 checks (`Saved/SmokeUpdate84f.log`), including remapped thrust input and manual bank response. All three aircraft also pass nine forward cockpit visibility rays against their opaque hull surfaces (`Tools/validate_aviation84.py`, `Saved/ValidateMeshes84c.log`); the final corrected hull import passes in `Saved/ImportUpdate84j.log`. No packaged build is produced. The automated checks exercise mapped road reachability and footprints, Canada and story continuity, fuel endurance, cabin motion, save/recovery and automatic airport-to-airport flight. They do not constitute a sustained manual playthrough of every town or a 100-NPC boarding stress test.
