# Vehicle views, recovery, and giant attacks

Driver viewpoints now match the detailed interiors: higher eye positions and a small forward adjustment improve the view over the dashboard. The supercar no longer applies the extra camera drop. Gauge needles now sweep from the low end of the dial to the high end in the correct direction.

Look at a stopped, empty vehicle and press **Shift+E** to flip/unstick it. Recovery searches nearby ground for an upright position with enough clearance. It preserves the vehicle, keys, cargo, upgrades, and ownership. Recovery is unavailable for occupied, moving, held, or airborne cars; blocked attempts report that no clear space is available.

Behemoths and colossi can grab nearby cars, including the player's occupied car. They raise the car during a 1.8-second wind-up and then throw it. Empty cars are aimed toward the player; an occupied player car is thrown away from the giant. Attacks have cooldowns, require reach and line of sight, and can be interrupted by staggering the giant or disabling its arms. A dead or removed grabber drops the car.

Thrown cars follow a swept, substepped ballistic path with gravity, impact damage and a short bounce. Occupants remain aboard and take collision damage. Cars can strike pedestrians or other vehicles. Normal driving and terrain recovery are suspended during the throw. The vehicle's saved position remains its last grounded position until it lands.

No packaged build is generated.

The sedan, muscle car, and supercar cabin meshes also have stowed sun visors to clear the forward sightline. Source patch: Tools/stow_visors49.py; imports: Tools/import_visors49.py and ArtSource/ModelsV49. The main vehicle generator retains the revised visor position for future regeneration.

Validation: editor compilation succeeded (Saved/Build49Final.log). Three cabin assets passed bounds/material import validation and LOD rebuilding (Saved/Visors49_Import.log). All 59 runtime checks passed (Saved/Vehicle49_Final.log), and all 11 driver views were reviewed. No packaged build was generated.
