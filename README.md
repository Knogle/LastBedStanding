# Last Bed Standing

Last Bed Standing is a **SA-MP/open.mp team deathmatch gamemode** inspired by Minecraft Bedwars. Teams spawn at their bases, gather money pickups, buy gear from a shop NPC, and fight to destroy enemy beds. Once a team’s bed is gone, its players can no longer respawn—so the final surviving team wins. 【F:gamemodes/lastbedstanding01.pwn†L3-L7】【F:gamemodes/lastbedstanding01.pwn†L2190-L2194】

## Core Gameplay Loop

1. **Pick a team** in the class selection scene. 【F:gamemodes/lastbedstanding01.pwn†L4344-L4357】
2. **Gather money** from team money spawners placed around the map. 【F:pawno/includes/bone_county.pwn†L61-L66】
3. **Buy equipment** at the shop NPC (weapons, skills, stealth kit, bomb, warp kit, helmet). 【F:gamemodes/lastbedstanding01.pwn†L551-L559】
4. **Attack enemy bases** and blow up their bed using `/blowup` (or by placing and detonating a bomb). 【F:gamemodes/lastbedstanding01.pwn†L2192-L2194】【F:gamemodes/lastbedstanding01.pwn†L3461-L3499】
5. **Finish the remaining players**—once a bed is destroyed, that team can’t respawn. 【F:gamemodes/lastbedstanding01.pwn†L3775-L3783】【F:gamemodes/lastbedstanding01.pwn†L3975-L3986】

## Key Features

- **Team-based bed destruction** with sudden-death after a bed blows up (no respawn). 【F:gamemodes/lastbedstanding01.pwn†L3975-L3986】
- **Shop system** with weapons, fighting styles, stealth kit, bomb, warp kit, and helmet items. 【F:gamemodes/lastbedstanding01.pwn†L551-L559】
- **Money pickup economy** fueled by configurable money spawn zones. 【F:pawno/includes/bone_county.pwn†L61-L66】
- **Bomb mechanics**: buy a bomb, place it with `/dropbomb`, and detonate a bed. 【F:gamemodes/lastbedstanding01.pwn†L2670-L2674】【F:gamemodes/lastbedstanding01.pwn†L3461-L3499】
- **Stealth kit** and **warp kit** purchases with dedicated commands. 【F:gamemodes/lastbedstanding01.pwn†L2654-L2694】【F:gamemodes/lastbedstanding01.pwn†L3010-L3112】
- **Auto-start countdown & team victory logic** to trigger and finish rounds. 【F:gamemodes/lastbedstanding01.pwn†L1220-L1333】
- **Random weather rotation** with periodic updates. 【F:gamemodes/lastbedstanding01.pwn†L521-L802】
- **Persistent stats** for kills, deaths, beds blown, and bombs detonated. 【F:gamemodes/lastbedstanding01.pwn†L2961-L2974】
- **Login/Register system** backed by MySQL. 【F:gamemodes/lastbedstanding01.pwn†L27-L36】

## Commands (Player-Facing)

- `/blowup` — blow up an enemy bed when close enough. 【F:gamemodes/lastbedstanding01.pwn†L2192-L2194】
- `/dropbomb` — place a bomb (requires purchased bomb). 【F:gamemodes/lastbedstanding01.pwn†L3461-L3479】
- `/detonate` — detonate a placed bomb. 【F:gamemodes/lastbedstanding01.pwn†L3481-L3491】
- `/stealth` — activate stealth kit. 【F:gamemodes/lastbedstanding01.pwn†L3010-L3021】
- `/warp` — warp back to your base (warp kit). 【F:gamemodes/lastbedstanding01.pwn†L3099-L3112】
- `/dropmoney` — drop money to the ground. 【F:gamemodes/lastbedstanding01.pwn†L2215-L2216】

## Maps & Customization

Map data is defined in the include files under `pawno/includes/`. Each map defines team count, colors, bed locations, team spawns, shop NPCs, and money spawn zones. Adjust the constants and arrays to build new maps or rebalance existing ones. 【F:pawno/includes/bone_county.pwn†L2-L90】【F:pawno/includes/bone_county.pwn†L92-L119】

Current map presets include:

- `bone_county.pwn` (3 teams) 【F:pawno/includes/bone_county.pwn†L32-L37】
- `chilliad.pwn` (4 teams) 【F:pawno/includes/chilliad.pwn†L38-L44】
- `countryside_1.pwn` (4 teams) 【F:pawno/includes/countryside_1.pwn†L42-L49】
- `countryside_2.pwn` (6 teams) 【F:pawno/includes/countryside_2.pwn†L51-L60】
- `green_palms.pwn` (3 teams) 【F:pawno/includes/green_palms.pwn†L35-L41】

## Configuration Notes

- **MySQL credentials** are configured at the top of the gamemode file. Update them before deployment. 【F:gamemodes/lastbedstanding01.pwn†L33-L36】
- **Map selection** is controlled via the `MAPTYPE` define. 【F:gamemodes/lastbedstanding01.pwn†L46-L58】

## License

This project is released under the GNU GPLv3. 【F:gamemodes/lastbedstanding01.pwn†L3-L22】
