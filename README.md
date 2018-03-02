# Change Log
All notable changes to this project will be documented in this file.

## Initial Release

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


## [0.1b] - 2017-03-21
### Added
- Added weather system for random weather.
- Weather can be set by admins, using /weather ID
- /dropmoney uses GenerateRandomPickup function now, to create money pickups.
- system to check if a team has won the game. (if other teams are empty)
- Improved several minor bugs.
- Major changes in score system.
- The winner of the team will now be determined if there is no player left in other teams
- Recursive function will be called in OnPlayerDeath instead of OnPlayerSpawn now.
- Dead player will be thrown into TEAM_SPECTATOR after death now if the bed is missing.
- GetPlayerCount() function added to count amount of players for use in TeamsAlive(bool:value) function.
- Recursive function TeamsAlive(bool:value) function (false/true) to count teams alive or check if there is only one team left.
- Fighting Style menu to purchase different fightstyles.

### Fix
- Increased weapon prices due to balancing reasons.
- Changed bomb activation radius to 150m instead of 100m.

## [0.1b] - 2017-03-28
### Added
- Function GivePlayerMoneyText to give or remove money from a player and display gametext.
- #define to change the value of Money Pickups.

## [0.1b] - 2017-03-28
### Added
- Function GivePlayerMoneyText to give or remove money from a player and display gametext.
- #define to change the value of Money Pickups.

## [0.1.3a] - 2018-03-02
### Fix
- Fixed major bugs.
- Fixed y_ini problems.
- Fixed a huge amount of bugs.
- Code optimization
