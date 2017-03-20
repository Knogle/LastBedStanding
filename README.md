# Change Log
All notable changes to this project will be documented in this file.

## [0.1a] - 2017-03-14
### Added
- Binarysearch instead of iterators for pickup system.
- /stealth command to make yourself invisible for 60 seconds on minimap.

## [0.1b] - 2017-03-20
### Added
- Possibility to get a Warpkit to teleport yourself back to your base by using /warp command. Can be purchased for $100k in the shop.
- New reference map files including new PlayerInfoPickups array. 
- New pickup type InfoPickups, to show certain important server information like rules, FAQ and more..
- SaveUser_data function to save userdata periodically into the user file.
- ResetPlayerData function to reset player vars and arrays after death/disconnect.
- TeleportPlayerToBase function to teleport the player back to his base using /warp command or /spawn command (Admin only).
-

### Removed
- The Seasparrow of team YELLOW on Chilliad map due to balancing.
- Usless/Unused player vars and arrays.


### Fix
- /stealth kit which didnt work before. The player was invisible on the map all the time. No reset.
- Map bugs on CHILLIAD and GREEN PALMS map.
- String length of several strings due to display problems.
