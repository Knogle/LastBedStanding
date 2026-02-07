// omp_lbs.inc - Pawn bindings for the omp-lbs component
#if defined _INC_OMP_LBS
	#endinput
#endif
#define _INC_OMP_LBS

enum E_LBS_PHASE
{
	LBS_PHASE_LOBBY = 0,
	LBS_PHASE_COUNTDOWN,
	LBS_PHASE_RUNNING,
	LBS_PHASE_FINISHED
};

// Setup / tick
native bool:LBS_Bind(teamCount = 6);
native bool:LBS_ConfigMoney(modelid = 1212, type = 19, value = 1000, teamIntervalMs = 2500, mainIntervalMs = 1000);
native bool:LBS_AddMoneySpawn(Float:xMax, Float:xMin, Float:yMax, Float:yMin, Float:zMax, Float:zMin, vw = 0, bool:isMain = false);
native bool:LBS_Update(deltaMs);
native bool:LBS_ResetMatch();

// State machine
native LBS_GetPhase();
native bool:LBS_RequestCountdown(seconds);

// Player ingestion (call from Pawn callbacks)
native bool:LBS_PlayerConnect(playerid);
native bool:LBS_PlayerDisconnect(playerid);
native bool:LBS_PlayerSetTeam(playerid, teamid, bool:chosen);
native bool:LBS_PlayerSpawned(playerid);
native bool:LBS_PlayerDied(playerid);

// Pickups
native LBS_HandlePickup(playerid, pickupid); // returns money amount or 0

// Query
native LBS_TeamAliveCount(teamid);
native LBS_ActiveTeamsCount();

// Required public: component asks Pawn to create the pickup.
forward LBS_Internal_CreatePickup(modelid, type, Float:x, Float:y, Float:z, virtualworld, bool:isMain);

// Optional events:
forward LBS_OnPhaseChange(E_LBS_PHASE:newPhase, E_LBS_PHASE:oldPhase);
forward LBS_OnCountdownTick(secondsLeft);
forward LBS_OnMatchStart();
forward LBS_OnMatchEnd(winningTeam);
