# Last Bed Standing

Last Bed Standing is a **SA-MP/open.mp team deathmatch gamemode** inspired by Minecraft Bedwars. Teams spawn at their bases, gather money pickups, buy gear from a shop NPC, and fight to destroy enemy beds. Once a team’s bed is gone, its players can no longer respawn, so the final surviving team wins.

## Core Gameplay Loop

1. **Pick a team** in the class selection scene.
2. **Gather money** from team money spawners placed around the map.
3. **Buy equipment** at the shop NPC (weapons, skills, stealth kit, bomb, warp kit, helmet).
4. **Attack enemy bases** and blow up their bed using `/blowup` (or by placing and detonating a bomb).
5. **Finish the remaining players**: once a bed is destroyed, that team cannot respawn.

## Key Features

- **Team-based bed destruction** with sudden-death after a bed blows up (no respawn).
- **Shop system** with weapons, fighting styles, stealth kit, bomb, warp kit, and helmet items.
- **Money pickup economy** fueled by configurable money spawn zones.
- **Bomb mechanics**: buy a bomb, place it with `/dropbomb`, and detonate a bed.
- **Stealth kit** and **warp kit** purchases with dedicated commands.
- **Auto-start countdown and team victory logic** to start and finish rounds.
- **Random weather rotation** with periodic updates.
- **Persistent stats** for kills, deaths, beds blown, and bombs detonated.
- **Login/Register system** backed by MySQL.

## C++ Component and Pawn Interaction

The project uses a hybrid model:

- **C++ component (`main.cpp`)** owns match phase state, alive-team evaluation, and timed money spawn generation.
- **Pawn gamemode (`gamemodes/lastbedstanding01.pwn`)** keeps gameplay/UI flow, menus, commands, and SA-MP callbacks.

Data flow between both layers:

1. During gamemode init, Pawn binds and configures the component (`LBS_Bind`, `LBS_ConfigMoney`, `LBS_AddMoneySpawn`).
2. Pawn runs a timer (`LBS_Tick`) and calls `LBS_Update(250)` every 250 ms to advance the C++ state machine.
3. When C++ wants to spawn a pickup, it calls Pawn public `LBS_Internal_CreatePickup(...)`; Pawn performs `CreatePickup(...)` and returns the pickup id.
4. On pickup, Pawn first delegates with `LBS_HandlePickup(playerid, pickupid)`. If the id belongs to component-managed money, Pawn pays the player and destroys the pickup.
5. Pawn forwards player lifecycle events into C++ (`LBS_PlayerConnect`, `LBS_PlayerSetTeam`, `LBS_PlayerSpawned`, `LBS_PlayerDied`, `LBS_PlayerDisconnect`) so team-alive and winner logic stays consistent.

Optional callbacks (`LBS_OnPhaseChange`, `LBS_OnCountdownTick`, `LBS_OnMatchStart`, `LBS_OnMatchEnd`) are exposed by the include and can be implemented in Pawn when custom HUD/messages are needed.

## Commands (Player-Facing)

- `/blowup` - blow up an enemy bed when close enough.
- `/dropbomb` - place a bomb (requires purchased bomb).
- `/detonate` - detonate a placed bomb.
- `/stealth` - activate stealth kit.
- `/warp` - warp back to your base (warp kit).
- `/dropmoney` - drop money to the ground.

## Maps & Customization

Map data is defined in the include files under `pawno/includes/`. Each map defines team count, colors, bed locations, team spawns, shop NPCs, and money spawn zones. Adjust the constants and arrays to build new maps or rebalance existing ones.

Current map presets include:

- `bone_county.pwn` (3 teams)
- `chilliad.pwn` (4 teams)
- `countryside_1.pwn` (4 teams)
- `countryside_2.pwn` (6 teams)
- `green_palms.pwn` (3 teams)

## Configuration Notes

- **MySQL credentials** are configured at the top of the gamemode file. Update them before deployment.
- **Map selection** is controlled via the `MAPTYPE` define.

## License

This project is released under the GNU GPLv3.
