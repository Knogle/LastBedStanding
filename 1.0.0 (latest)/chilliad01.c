

/*
	Knogles Last Bed Standing Masterpiece for SA-MP
	Fork of Minecrafts Bedwars 
	Little TDM Gamemode
	Published under GNU GPL Public License.
	Copyright (C) 2017  Fabian Druschke
	Contact: webmaster@knogleinsi.de
	Mail: Postfach 32 22 50147 Kerpen Germany

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see http://www.gnu.org/licenses/

*/

#include <a_samp>
#include <sscanf2>
#include <a_mysql>
#include <YSI\y_ini>
#include <md-sort>

#define MYSQL_HOST        "localhost" // Change this to your MySQL Remote IP or "localhost".
#define MYSQL_USER        "server" // Change this to your MySQL Database username.
#define MYSQL_PASS        "ylw5dchQKpMNLSuxc4uVYHrCu51nSfXF" // Change this to your MySQL Database password.
#define MYSQL_DATABASE    "samp" // Change this to your MySQL Database name.


#define DIALOG_REGISTER        (0)
#define DIALOG_LOGIN           (1)


new MySQL: Database, Corrupt_Check[MAX_PLAYERS];

enum
{
	CHILLIAD,
	SAN_FIERRO_DOCKS,
	BONE_COUNTY,
	MAD_DOGGS,
	COUNTRYSIDE_1,
	GREEN_PALMS,
	COUNTRYSIDE_2
};

#define MAPTYPE CHILLIAD

enum pInfo
{
	ID,
	pName[25],
	
	pPass[65],
	Salt[11],
	pAdmin,
	pPassFails,
	
	pKills,
	pDeaths,
	
	pBeds,
	pBombs,
	
	
Cache: Player_Cache,
bool:LoggedIn,
	pSpawned
}

new playerdataInfo[MAX_PLAYERS][pInfo];

enum
{
	INVALID_PICKUP_TYPE,
	MONEY_TYPE,
	ACTOR_TYPE,
	INFO_TYPE
};
enum weather_info
{
	wt_id,
	wt_text[255]
};



#define PRESSED(%0) \
	(((newkeys & (%0)) == (%0)) && ((oldkeys & (%0)) != (%0)))	

#define FIRST_TEAM 0 
#define SECOND_TEAM 1 
#define THIRD_TEAM 2
#define FOURTH_TEAM 3 
#define FIFTH_TEAM 4
#define SIXTH_TEAM 5 
#define TEAM_SPECTATOR 6 	

#define ADMIN_SPEC_TYPE_NONE 0
#define ADMIN_SPEC_TYPE_PLAYER 1
#define ADMIN_SPEC_TYPE_VEHICLE 2
#define COLOR_WHITE 		0xFFFFFFFF
#define COLOR_NORMAL_PLAYER 0xFFBB7777
#define COLOR_GREY 0xAFAFAFAA
#define COLOR_RED 0xAA3333AA
#define COLOR_GREEN 0x33AA33AA
#define COLOR_YELLOW 0xFFFF0055
#define COLOR_BLUE 0x0000BBAA
#define COLOR_NAVY 0x000080AA

#define SPECTATOR_TEAM_COLOR_TAG "{FFFFFF}SPECTATOR"

#define MoneyVal 1000
#define UnusedVehTimer 300000 //time in ms to respawn unoccupied vehicles
#define BEDSSYSTEM 1




#define DIALOG_REGISTER        (0)
#define DIALOG_LOGIN           (1)

//Register/Login system is enabled by default




//----------------------------------------------------------
forward WeatherTimer();
forward GMX();
forward RespawnAllVehicles();
forward RandomWeather();
forward SaveUser_data(playerid);
forward OnPlayerDataSave(playerid);
forward ResetPlayerData(playerid);
forward warpcount(playerid);
forward TeleportPlayerToBase(playerid);
forward reveal(playerid);
forward quickSort(array[], left, right);
forward PrepareToBlowUp(playerid);
forward GetPickupType(pickupid, &index);
forward Float:frandom(Float:max, Float:min = 0.0, dp = 4);
forward GetXYInFrontOfActor(actorid, &Float:x, &Float:y, Float:distance);
forward GetXYInFrontOfPlayer(playerid, &Float:x, &Float:y, Float:distance);
forward Float:GetDistance( Float:a1, Float:b1, Float:c1, Float:a2, Float:b2, Float:c2 );
forward GenerateRandomPickup(modelid,type,Float:x_max,Float:x_min,Float:y_max,Float:y_min,Float:z_max,Float:z_min,virtualworld);
forward LoadUser_data(playerid,name[],value[]);
forward MONEY_MAIN();
forward TEAM_MONEY();
forward BlowUpThisBed(teamid);
forward SendTeamMessage(teamid, color, const message[]);
forward SellPlayerFightingStyle(playerid,cost,style);
forward SellPlayerWeapon(playerid,cost,weaponid,ammo);
forward SpecPlayer(playerid,targetplayer);
forward UpdateTimeAndWeather();
forward CountDown();
forward StartGame();
forward TeamRemaining();
forward FinishedGame();
forward LockAllVehicles();
forward UnlockAllVehicles();


//----------------------------------------------------------
new GameHasStarted;
new GameHasFinished;
new CountDownTimer2;
new Warppowder[MAX_PLAYERS];
new PSkill[MAX_PLAYERS];
new PBomb[MAX_PLAYERS];
new LastMoney[MAX_PLAYERS];
new PStealth[MAX_PLAYERS];
new Float:SpecX[MAX_PLAYERS], Float:SpecY[MAX_PLAYERS], Float:SpecZ[MAX_PLAYERS], vWorld[MAX_PLAYERS], Inter[MAX_PLAYERS];
new IsSpecing[MAX_PLAYERS], Name[MAX_PLAYER_NAME], IsBeingSpeced[MAX_PLAYERS],spectatorid[MAX_PLAYERS];
new Float:pX[MAX_PLAYERS];
new Float:pY[MAX_PLAYERS];
new Float:pZ[MAX_PLAYERS];
new Float:wX[MAX_PLAYERS];
new Float:wY[MAX_PLAYERS];
new Float:wZ[MAX_PLAYERS];

new SpectatingPlayer[MAX_PLAYERS];
new PBombID[MAX_PLAYERS];
new Beam[MAX_PLAYERS];
new gPlayerTeamSelection[MAX_PLAYERS];
new gPlayerHasTeamSelected[MAX_PLAYERS];
new gPlayerLastTeamSelectionTick[MAX_PLAYERS];
new PlayerInfo[MAX_PLAYERS][pInfo];
new Helmet[MAX_PLAYERS];
new BombObject[MAX_PLAYERS];
new timestr[32];
new timestrex[32];
new delay=1000;// only allow new selection every ~1000 ms; may differ due to server load
new bombtimer=5000;// time in ms to blow up closest bed
new MoneyDropTimer=2500;// Time to generate new moneypickups
new Shop_Counter;
new CountDownVar;
new AFK_SYS[MAX_PLAYERS];


//--------------------------------------------------------------

new totaltime;
new gRandomWeatherIDs[][weather_info] =
{
	{1,"Sunny"},
	{8,"Stormy"},
	{9,"Foggy Sky"},
	{11,"Extreme Sunny"},
	{16,"Rainy"},
	{19,"Thunderstorm"},
	{20,"Foggy"}	
};



//--------------------------------------------------------------
new Menu:infomenu;
new Menu:fightstyles;
new Menu:shotguns;
new Menu:items;
new Menu:smg;
new Menu:rifles;
new Menu:yesno;
new Menu:assaultrifle;
new Menu:Grenades;
new Menu:melee;
new Menu:special;
new Menu:shopmenu;
new Menu:ammunation;
new Menu:pistols;
new Menu:microsmg;
//--------------------------------------------------------------
new Text:txtSpectatorHelper;
new Text:txtClassSelHelper;
new Text:txtTimeDisp;
new Text:Textcover;
new PlayerText:TeamCover[MAX_PLAYERS];
new PlayerText:TeamText[MAX_PLAYERS];
new PlayerText:LocText[MAX_PLAYERS];
//--------------------------------------------------------------
new moneyval;
new maxmoney;
new Float:x1, Float:y1, Float:z1;
new running;
new CountDownVarBlowUp = 4;
new WarpVar[MAX_PLAYERS];
new WarpTimer;
new CountDownTimer;
new hour, minute;
//--------------------------------------------------------------


#if defined MAPTYPE
#if MAPTYPE==CHILLIAD
#include <chilliad.pwn>
#elseif MAPTYPE==BONE_COUNTY
#include <bone_county.pwn>
#elseif MAPTYPE==SAN_FIERRO_DOCKS
#include <../../gamemodes/san_fierro_docks.pwn>
#elseif MAPTYPE==MAD_DOGGS
#include <mad_doggs.pwn>
#elseif MAPTYPE==GREEN_PALMS
#include <green_palms.pwn>
#elseif MAPTYPE==COUNTRYSIDE_1
#include <countryside_1.pwn>
#elseif MAPTYPE==COUNTRYSIDE_2
#include <countryside_2.pwn>
#endif
#endif

#if defined TEAMSIZE
#if TEAMSIZE > 6
#error Amount of Teams is limited to 6!
#endif
#if TEAMSIZE < 2
#error Minimum Amount of Teams is limited to 2!
#endif
#endif
#if defined BEDSSYSTEM
#if BEDSSYSTEM > 1
#error Scoresystem val must be 0 or 1 (false true)
#endif
#if BEDSSYSTEM < 0
#error Scoresystem val must be 0 or 1 (false true)
#endif
#endif

#define getInv(%0) (((%0)>>8)<<8)

#define COLOR_TEAM_ONE_STEALTH getInv(COLOR_TEAM_ONE)  //Brown
#define COLOR_TEAM_TWO_STEALTH getInv(COLOR_TEAM_TWO)  //Jade
#define COLOR_TEAM_THREE_STEALTH getInv(COLOR_TEAM_THREE)  //Olive
#define COLOR_TEAM_FOUR_STEALTH getInv(COLOR_TEAM_FOUR)  //Aqua
#define COLOR_TEAM_FIVE_STEALTH getInv(COLOR_TEAM_FIVE) //Olive
#define COLOR_TEAM_SIX_STEALTH getInv(COLOR_TEAM_SIX) //Aqua

//Never change these lines below if you don't know what to do
new ActorPickups[sizeof(GlobalActors)];
new InfoPickups[sizeof(PlayerInfoPickups)];
new MoneyPickups[MAX_PICKUPS-sizeof(ActorPickups)-sizeof(InfoPickups)-1];
//
new BedArray[TEAMSIZE];
new BedStates[TEAMSIZE];
new TEAM_ZONE[TEAMSIZE];






#define BYTECOUNT  (cellbits / 8)

stock binarysearch(a[],key,l,r)
{
	new k;
	while(r >=l)
	{
		k = (l+r)/2;
		if(key == a[k])
		{
			return k;
		}
		if(key < a[k]) 
		{
			r = k-1;
		}
		else 
		{    
			l= k+1;
		}        
	}    
	return -1;
}
stock binarysearch2(a[][],idx,key,l,r)
{
	new k;
	while(r >=l)
	{
		k = (l+r)/2;
		if(key == a[k][idx])
		{
			return k;
		}
		if(key < a[k][idx])
		{
			r = k-1;
		}
		else
		{
			l= k+1;
		}
	}
	return -1;
}

stock SendClientMessageEx(playerid, color, fstring[], {Float, _}:...)
{
	static const
	ARGC = 3;
	new n = (numargs() - ARGC) * BYTECOUNT;
	if (n)
	{
		new message[128], arg_start, arg_end;
		#emit CONST.alt        fstring
		#emit LCTRL          5
		#emit ADD
		#emit STOR.S.pri        arg_start
		#emit LOAD.S.alt        n
		#emit ADD
		#emit STOR.S.pri        arg_end
		do
		{
			#emit LOAD.I
			#emit PUSH.pri
			arg_end -= BYTECOUNT;
			#emit LOAD.S.pri      arg_end
		}
		while (arg_end > arg_start);
		#emit PUSH.S          fstring
		#emit PUSH.C          128
		#emit PUSH.ADR         message
		n += BYTECOUNT * 3;
		#emit PUSH.S          n
		#emit SYSREQ.C         format
		n += BYTECOUNT;
		#emit LCTRL          4
		#emit LOAD.S.alt        n
		#emit ADD
		#emit SCTRL          4
		return SendClientMessage(playerid, color, message);
	}
	else
	{
		return SendClientMessage(playerid, color, fstring);
	}
}






public OnPlayerDataSave(playerid)
{
	Corrupt_Check[playerid]++;

	new DB_Query[256];
	//Running a query to save the player's data using the stored stuff.
	mysql_format(Database, DB_Query, sizeof(DB_Query), "UPDATE `PLAYERS` SET `BEDS` = %d, `BOMBS` = %d, `KILLS` = %d, `DEATHS` = %d WHERE `ID` = %d LIMIT 1",
	playerdataInfo[playerid][pBeds], playerdataInfo[playerid][pBombs], playerdataInfo[playerid][pKills], playerdataInfo[playerid][pDeaths], playerdataInfo[playerid][ID]);

	mysql_tquery(Database, DB_Query);

	if(cache_is_valid(playerdataInfo[playerid][Player_Cache])) //Checking if the player's cache ID is valid.
	{
		cache_delete(playerdataInfo[playerid][Player_Cache]); // Deleting the cache.
		playerdataInfo[playerid][Player_Cache] = MYSQL_INVALID_CACHE; // Setting the stored player Cache as invalid.
	}

	playerdataInfo[playerid][LoggedIn] = false;
}
public OnGameModeExit()
{
	for(new i = 0; i < MAX_PLAYERS; i++)
	{
		if(IsPlayerConnected(i)) // Checking if the players stored in "i" are connected.
		{
			Corrupt_Check[i]++;
			

			new DB_Query[256];
			//Running a query to save the player's data using the stored stuff.
			mysql_format(Database, DB_Query, sizeof(DB_Query), "UPDATE `PLAYERS` SET `BEDS` = %d, `BOMBS` = %d, `KILLS` = %d, `DEATHS` = %d WHERE `ID` = %d LIMIT 1",
			playerdataInfo[i][pBeds], playerdataInfo[i][pBombs], playerdataInfo[i][pKills], playerdataInfo[i][pDeaths], playerdataInfo[i][ID]);

			mysql_tquery(Database, DB_Query);

			if(cache_is_valid(playerdataInfo[i][Player_Cache])) //Checking if the player's cache ID is valid.
			{
				cache_delete(playerdataInfo[i][Player_Cache]); // Deleting the cache.
				playerdataInfo[i][Player_Cache] = MYSQL_INVALID_CACHE; // Setting the stored player Cache as invalid.
			}

		}
	}

	mysql_close(Database); // Closing the database.
	return 1;
}
@AFK_CHECK();@AFK_CHECK() {
	/*for(new i=GetPlayerPoolSize(); i!=-1; i--)
	{
		if(!IsPlayerConnected(i) || IsPlayerNPC(i) || !AFK_SYS[i] || (gettime()-AFK_SYS[i]) < 120) continue;
		Kick(i); //i = playerid die 2min (120 Sekunden) AFK war
		new name[MAX_PLAYER_NAME + 1];
		GetPlayerName(i, name, sizeof(name));
		for(new k=MAX_PLAYERS;k-->0;)
		{
			SendClientMessageEx(k,COLOR_WHITE,"SERVER: %s (%d) has been kicked due to inactivity",name,i);	
		}
		
		AFK_SYS[i] = 0;
	}*/
	return 1;
}
public OnGameModeInit()
{
	for(new r;r<TEAMSIZE;r++)//Beginning with Team 0 (FIRST_TEAM) increasing.
	{
		BedArray[r]=CreateObject(1801,beds[r][0],beds[r][1],beds[r][2],0.0000000,0.0000000,0.0000000); //object(swank_bed_4) (1)
	}
	
	
	
	for(new i;i<sizeof(BedStates);i++)
	{
		BedStates[i]=1;
	}
	mysql_log(ALL);
	new MySQLOpt: option_id = mysql_init_options();
	mysql_set_option(option_id, AUTO_RECONNECT, true); // We will set that option to automatically reconnect on timeouts.
	mysql_set_option(option_id,SERVER_PORT,3306);

	Database = mysql_connect(MYSQL_HOST, MYSQL_USER, MYSQL_PASS, MYSQL_DATABASE, option_id); // Setting up the "Database" handle on the given MySQL details above.

	if(Database == MYSQL_INVALID_HANDLE || mysql_errno(Database) != 0) // Checking if the database connection is invalid to shutdown.
	{
		print("I couldn't connect to the MySQL server, closing."); // Printing a message to the log.
		
		sleep(5);

		SendRconCommand("exit"); // Sending console command to shut down server.
		return 1;
	}

	print("I have connected to the MySQL server."); // If the given MySQL details were all okay, this message prints to the log.

	// Now, we will set up the information table of the player's information.
	
	mysql_tquery(Database, "CREATE TABLE IF NOT EXISTS `PLAYERS` (`ID` int(11) NOT NULL AUTO_INCREMENT,`USERNAME` varchar(24) NOT NULL,`PASSWORD` char(65) NOT NULL,`SALT` char(11) NOT NULL,`BEDS` mediumint(7), `KILLS` mediumint(7), `BOMBS` mediumint(7) NOT NULL DEFAULT '0',`DEATHS` mediumint(7) NOT NULL DEFAULT '0', PRIMARY KEY (`ID`), UNIQUE KEY `USERNAME` (`USERNAME`))");
	CountDownTimer2 = SetTimer("StartGame",1000,true);
	
	#if defined MAPTYPE
	#if MAPTYPE==CHILLIAD
	#tryinclude <chilliad_vehicles.txt>
	#tryinclude <chilliad_objects.txt>
	#elseif MAPTYPE==BONE_COUNTY
	#tryinclude <bone_vehicles.txt>
	#tryinclude <bone_objects.txt>
	#elseif MAPTYPE==MAD_DOGGS
	#tryinclude <md_vehicles.txt>
	#tryinclude <md_objects.txt>
	#elseif MAPTYPE==SAN_FIERRO_DOCKS
	#tryinclude <sf_vehicles.txt>
	#tryinclude <sf_objects.txt>
	#elseif MAPTYPE==GREEN_PALMS
	#tryinclude <gp_vehicles.txt>
	#tryinclude <gp_objects.txt>
	#elseif MAPTYPE==COUNTRYSIDE_1
	#tryinclude <cs_vehicles.txt>
	#tryinclude <cs_objects.txt>
	#elseif MAPTYPE==COUNTRYSIDE_2
	#tryinclude <cs2_vehicles.txt>
	#tryinclude <cs2_objects.txt>
	#endif
	#endif

	
	
	for(new t;t<sizeof(MoneyPickups);t++)
	{
		MoneyPickups[t]=-1;
	}
	for(new d;d<sizeof(ActorPickups);d++)
	{
		ActorPickups[d]=-1;
	}
	for(new f;f<sizeof(InfoPickups);f++)
	{
		InfoPickups[f]=-1;
	}
	LockAllVehicles();
	SendRconCommand("mapname "#MAPTYPE);
	SetTimer("@AFK_CHECK",9973,1);
	SetTimer("RespawnAllVehicles",UnusedVehTimer, 1);
	SetTimer("RandomWeather", 300000, 1);
	

	for(new zone;zone<TEAMSIZE;zone++)
	{
		TEAM_ZONE[zone]=GangZoneCreate(GlobalZones[zone][0],GlobalZones[zone][1],GlobalZones[zone][2],GlobalZones[zone][3]);//Create Gangzones
	}
	
	for(new g=0;g<sizeof(GlobalActors);g++)
	{
		CreateGlobalActor(g+1,274,GlobalActors[g][0],GlobalActors[g][1],GlobalActors[g][2],GlobalActors[g][3],3.0,g);		
		SetActorInvulnerable(g+1, true);
	}
	for(new t=0;t<sizeof(InfoPickups);t++)
	{
		InfoPickups[t]=CreatePickup(1239,2,PlayerInfoPickups[t][0],PlayerInfoPickups[t][1],PlayerInfoPickups[t][2],0);
	}


	
	
	infomenu = CreateMenu("Information", 1,20,200,200);
	if(IsValidMenu(infomenu)){
		AddMenuItem(infomenu, 0, "How to play");
		AddMenuItem(infomenu, 0, "FAQ");
		AddMenuItem(infomenu, 0, "Rules");
	}
	shopmenu = CreateMenu("Ammu-Nation", 1,20,200,200);
	if(IsValidMenu(shopmenu)){
		AddMenuItem(shopmenu, 0, "Weapons");
		AddMenuItem(shopmenu, 0, "Improve Skill");
		AddMenuItem(shopmenu, 0, "Stealth $85000");
		AddMenuItem(shopmenu, 0, "Bomb $100000");
		AddMenuItem(shopmenu, 0, "Warpkit $50000");
		AddMenuItem(shopmenu, 0, "Helmet $5000");
		AddMenuItem(shopmenu, 0, "Suicide");
	}
	yesno=CreateMenu("~w~Confirmation",1,20,150,150);
	if(IsValidMenu(yesno)){
		SetMenuColumnHeader(yesno, 0, "Suicide");
		AddMenuItem(yesno, 0, "Yes");
		AddMenuItem(yesno, 0, "No");
		AddMenuItem(yesno, 0, "Back");
	}
	ammunation=CreateMenu("~w~Weaponlist",1,20,150,150);
	if(IsValidMenu(ammunation)){
		SetMenuColumnHeader(ammunation, 0, "Weapon Type");
		AddMenuItem(ammunation,0,"  Pistols");
		AddMenuItem(ammunation,0,"  Micro SMG");
		AddMenuItem(ammunation,0,"  Shotguns");
		AddMenuItem(ammunation,0,"  Misc");
		AddMenuItem(ammunation,0,"  SMG");
		AddMenuItem(ammunation,0,"  Rifles");
		AddMenuItem(ammunation,0,"  Assault rifle");
		AddMenuItem(ammunation,0,"  Grenades");
		AddMenuItem(ammunation,0,"  Hand Guns");
		AddMenuItem(ammunation,0,"  Special");
		AddMenuItem(ammunation, 0, "Back");
	}
	pistols=CreateMenu("~w~Ammu-Nation",1,20,150,150);
	if(IsValidMenu(pistols)){
		SetMenuColumnHeader(pistols, 0, "Pistole");
		AddMenuItem(pistols,0,"  9mm $1500");
		AddMenuItem(pistols,0,"  9mm Silenced $2500");
		AddMenuItem(pistols,0,"  Desert Eagle $2500");
		AddMenuItem(pistols, 0, "Back");
	}
	microsmg=CreateMenu("~w~Ammu-Nation",1,20,150,150);
	if(IsValidMenu(microsmg)){
		SetMenuColumnHeader(microsmg, 0, "Micro SMG");
		AddMenuItem(microsmg,0,"  Tec9 $5500");
		AddMenuItem(microsmg,0,"  Micro SMG $5000");
		AddMenuItem(microsmg, 0, "Back");
	}
	shotguns=CreateMenu("~w~Ammu-Nation",1,20,150,150);
	if(IsValidMenu(shotguns)){
		SetMenuColumnHeader(shotguns, 0, "Shotguns");
		AddMenuItem(shotguns,0,"  Shotgun $12500");
		AddMenuItem(shotguns,0,"  Sawn Off $15500");
		AddMenuItem(shotguns,0,"  Combat Shotgun $55500");
		AddMenuItem(shotguns, 0, "Back");
	}
	items=CreateMenu("~w~Ammu-Nation",1,20,150,150);
	if(IsValidMenu(items)){
		SetMenuColumnHeader(items, 0, "Item");
		AddMenuItem(items,0,"  Armour $2500");
		AddMenuItem(items,0,"  Parachute $2500");
		AddMenuItem(items,0,"  Spraycan $2500");
		AddMenuItem(items,0,"  Camera $2500");
		AddMenuItem(items,0,"  Health $2500");
		AddMenuItem(items,0,"  Fightstyles");
		AddMenuItem(items,0,"Back");
	}
	fightstyles=CreateMenu("~w~Ammu-Nation",1,20,150,150);
	if(IsValidMenu(fightstyles)){
		SetMenuColumnHeader(fightstyles, 0, "Fightstyles");
		AddMenuItem(fightstyles,0,"  Kung Fu $2500");
		AddMenuItem(fightstyles,0,"  Kneehead $2500");
		AddMenuItem(fightstyles,0,"  Boxing $2500");
		AddMenuItem(fightstyles,0,"  Grabkick $2500");
		AddMenuItem(fightstyles,0,"  Elbow $2500");
		AddMenuItem(fightstyles,0,"  Normal");
		AddMenuItem(fightstyles,0,"Back");
	}
	smg=CreateMenu("~w~Ammu-Nation",1,30,150,150);
	if(IsValidMenu(smg)){
		SetMenuColumnHeader(smg, 0, "SMG");
		AddMenuItem(smg,0," SMG $20000");
		AddMenuItem(smg, 0, "Back");
	}
	rifles=CreateMenu("~w~Ammu-Nation",1,20,150,150);
	if(IsValidMenu(rifles)){
		SetMenuColumnHeader(rifles, 0, "Rifle");
		AddMenuItem(rifles,0," Cunt Gun $12500");
		AddMenuItem(rifles,0," Sniper Rifle $40000");
		AddMenuItem(rifles, 0, "Back");
	}
	assaultrifle=CreateMenu("~w~Ammu-Nation",1,20,150,150);
	if(IsValidMenu(assaultrifle)){
		SetMenuColumnHeader(assaultrifle, 0, "Assault Rifle");
		AddMenuItem(assaultrifle,0," AK-47 $40000");
		AddMenuItem(assaultrifle,0," M4 $45000");
		AddMenuItem(assaultrifle, 0, "Back");
	}
	special=CreateMenu("~w~Ammu-Nation",1,20,150,150);
	if(IsValidMenu(special)){
		SetMenuColumnHeader(special, 0, "special");
		AddMenuItem(special,0," Minigun $2000000");
		AddMenuItem(special,0," Bazooka $250000");
		AddMenuItem(special,0," Flame Thrower $250000");
		AddMenuItem(special, 0, "Back");
	}
	melee=CreateMenu("~w~Ammu-Nation",1,20,150,150);
	if(IsValidMenu(melee)){
		SetMenuColumnHeader(melee, 0, "Hand Guns");
		AddMenuItem(melee,0," Baseball");
		AddMenuItem(melee,0," Dildo");
		AddMenuItem(melee,0," Chainsaw $100000");
		AddMenuItem(melee,0," Katana");
		AddMenuItem(melee,0," Pool cue");
		AddMenuItem(melee,0," Knife");
		AddMenuItem(melee,0," Night Stick");
		AddMenuItem(melee,0," Golf Club");
		AddMenuItem(melee, 0, "Back");
	}
	Grenades=CreateMenu("~w~Ammu-Nation",1,20,150,150);
	if(IsValidMenu(Grenades)){
		SetMenuColumnHeader(Grenades, 0, "Grenades");
		AddMenuItem(Grenades,0," Grenades $125000");
		AddMenuItem(Grenades,0," Tear Gas $25000");
		AddMenuItem(Grenades,0," Molotovs $25000");
		AddMenuItem(Grenades, 0, "Back");
	}


	SetTimer("TEAM_MONEY", MoneyDropTimer, true);
	SetTimer("MONEY_MAIN", 1000, true);
	


	

	UpdateTimeAndWeather();
	SetTimer("UpdateTimeAndWeather",1000 * 60,1);
	SetGameModeText("Bedwars");
	ShowPlayerMarkers(PLAYER_MARKERS_MODE_GLOBAL);
	ShowNameTags(1);
	SetNameTagDrawDistance(40.0);

	EnableStuntBonusForAll(0);
	SetWeather(2);


	UsePlayerPedAnims();


	ClassSel_InitTextDraws();

	// Player Classes/
	for(new a=299; a!=0;a--)
	{
		AddPlayerClass(a,1759.0189,-1898.1260,13.5622,266.4503,-1,-1,-1,-1,-1,-1);
	}
	
	txtSpectatorHelper = TextDrawCreate(10.0, 415.0,
	" Press ~b~~k~~GO_LEFT~ ~w~or ~b~~k~~GO_RIGHT~ ~w~to switch between players.");
	TextDrawUseBox(txtSpectatorHelper, 1);
	TextDrawBoxColor(txtSpectatorHelper,0x22222266);
	TextDrawLetterSize(txtSpectatorHelper,0.3,1.0);
	TextDrawTextSize(txtSpectatorHelper,400.0,40.0);
	TextDrawFont(txtSpectatorHelper, 2);
	TextDrawSetShadow(txtSpectatorHelper,0);
	TextDrawSetOutline(txtSpectatorHelper,1);
	TextDrawBackgroundColor(txtSpectatorHelper,0x000000FF);
	TextDrawColor(txtSpectatorHelper,0xFFFFFFFF);
	
	txtTimeDisp = TextDrawCreate(605.0,25.0,"00:00");
	TextDrawUseBox(txtTimeDisp, 0);
	TextDrawFont(txtTimeDisp, 3);
	TextDrawSetShadow(txtTimeDisp,0); // no shadow
	TextDrawSetOutline(txtTimeDisp,2); // thickness 1
	TextDrawBackgroundColor(txtTimeDisp,0x000000FF);
	TextDrawColor(txtTimeDisp,0xFFFFFFFF);
	TextDrawAlignment(txtTimeDisp,3);
	TextDrawLetterSize(txtTimeDisp,0.5,1.5);
	return 1;
}
stock GetXYInFrontOfPlayer(playerid, &Float:x, &Float:y, Float:distance)
{
	new Float:a;
	GetPlayerPos(playerid, x, y, a);
	GetPlayerFacingAngle(playerid, a);
	if (GetPlayerVehicleID(playerid))
	{
		GetVehicleZAngle(GetPlayerVehicleID(playerid), a);
	}
	x += (distance * floatsin(-a, degrees));
	y += (distance * floatcos(-a, degrees));
}
stock Float:frandom(Float:max, Float:min = 0.0, dp = 4)
{
	new
Float:mul = floatpower(10.0, dp),
	imin = floatround(min * mul),
	imax = floatround(max * mul);
	return float(random(imax - imin) + imin) / mul;
}

public RandomWeather()
{
	new rand = random(sizeof(gRandomWeatherIDs));
	new strout[256];
	format(strout, sizeof(strout), "SERVER: Weather changed to %s", gRandomWeatherIDs[rand][wt_text]);
	SetWeather(gRandomWeatherIDs[rand][wt_id]);
	SendClientMessageToAll(COLOR_WHITE,strout);
	print(strout);
}
LockAllVehicles()
{
	for(new i;i<MAX_VEHICLES;i++)
	{
		SetVehicleParamsEx(i,0, 0, 0, 1, 0, 0, 0);
	}
	return 1;
}
UnlockAllVehicles()
{
	for(new i;i<MAX_VEHICLES;i++)
	{
		SetVehicleParamsEx(i,0, 0, 0, 0, 0, 0, 0);
	}
	return 1;
}
public TeleportPlayerToBase(playerid)
{
	#if defined TEAMSIZE
	#if TEAMSIZE >= 2	
	new randSpawn=0;
	if(gPlayerTeamSelection[playerid] == TEAM_SPECTATOR)
	{
		return 1;
	}
	switch(gPlayerTeamSelection[playerid])
	{
	case FIRST_TEAM:
		{
			new str[80];
			randSpawn = random(sizeof(gSpawnsTeam_TEAM_ONE));
			format(str,sizeof(str),"SERVER: You have been teleported to the base of your team "#FIRST_TEAM_COLOR_TAG);
			SendClientMessage(playerid,COLOR_WHITE,str);
			SetPlayerPos(playerid,
			gSpawnsTeam_TEAM_ONE[randSpawn][0],
			gSpawnsTeam_TEAM_ONE[randSpawn][1],
			gSpawnsTeam_TEAM_ONE[randSpawn][2]);	
			SetPlayerInterior(playerid,TEAM_ONE_INTERIOR);
		}
	case SECOND_TEAM:
		{
			new str[80];
			randSpawn = random(sizeof(gSpawnsTeam_TEAM_TWO));
			format(str,sizeof(str),"SERVER: You have been teleported to the base of your team "#SECOND_TEAM_COLOR_TAG);
			SendClientMessage(playerid,COLOR_WHITE,str);
			SetPlayerPos(playerid,
			gSpawnsTeam_TEAM_TWO[randSpawn][0],
			gSpawnsTeam_TEAM_TWO[randSpawn][1],
			gSpawnsTeam_TEAM_TWO[randSpawn][2]);
			SetPlayerInterior(playerid,TEAM_TWO_INTERIOR);			
		}
		
		
		#endif
		#endif	
		#if defined TEAMSIZE
		#if TEAMSIZE >= 3

		
	case THIRD_TEAM:
		{
			new str[80];
			randSpawn = random(sizeof(gSpawnsTeam_TEAM_THREE));
			format(str,sizeof(str),"SERVER: You have been teleported to the base of your team "#THIRD_TEAM_COLOR_TAG);
			SendClientMessage(playerid,COLOR_WHITE,str);
			SetPlayerPos(playerid,
			gSpawnsTeam_TEAM_THREE[randSpawn][0],
			gSpawnsTeam_TEAM_THREE[randSpawn][1],
			gSpawnsTeam_TEAM_THREE[randSpawn][2]);
			SetPlayerInterior(playerid,TEAM_THREE_INTERIOR);			
		}
		
		
		#endif
		#endif	
		#if defined TEAMSIZE
		#if TEAMSIZE >= 4

		
	case FOURTH_TEAM:
		{
			new str[80];
			randSpawn = random(sizeof(gSpawnsTeam_TEAM_FOUR));
			format(str,sizeof(str),"SERVER: You have been teleported to the base of your team "#FOURTH_TEAM_COLOR_TAG);
			SendClientMessage(playerid,COLOR_WHITE,str);
			SetPlayerPos(playerid,
			gSpawnsTeam_TEAM_FOUR[randSpawn][0],
			gSpawnsTeam_TEAM_FOUR[randSpawn][1],
			gSpawnsTeam_TEAM_FOUR[randSpawn][2]);
			SetPlayerInterior(playerid,TEAM_FOUR_INTERIOR);	
		}
		
		#endif
		#endif	
		#if defined TEAMSIZE
		#if TEAMSIZE >= 5	

		
	case FIFTH_TEAM:
		{
			new str[80];
			randSpawn = random(sizeof(gSpawnsTeam_TEAM_FIVE));
			format(str,sizeof(str),"SERVER: You have been teleported to the base of your team "#FIFTH_TEAM_COLOR_TAG);
			SendClientMessage(playerid,COLOR_WHITE,str);
			SetPlayerPos(playerid,
			gSpawnsTeam_TEAM_FIVE[randSpawn][0],
			gSpawnsTeam_TEAM_FIVE[randSpawn][1],
			gSpawnsTeam_TEAM_FIVE[randSpawn][2]);
			SetPlayerInterior(playerid,TEAM_FIVE_INTERIOR);	
		}
		
		#endif
		#endif
		#if defined TEAMSIZE
		#if TEAMSIZE == 6	

		
	case SIXTH_TEAM:
		{
			new str[80];
			randSpawn = random(sizeof(gSpawnsTeam_TEAM_SIX));
			format(str,sizeof(str),"SERVER: You have been teleported to the base of your team "#SIXTH_TEAM_COLOR_TAG);
			SendClientMessage(playerid,COLOR_WHITE,str);
			SetPlayerPos(playerid,
			gSpawnsTeam_TEAM_SIX[randSpawn][0],
			gSpawnsTeam_TEAM_SIX[randSpawn][1],
			gSpawnsTeam_TEAM_SIX[randSpawn][2]);	
			SetPlayerInterior(playerid,TEAM_SIX_INTERIOR);	
		}
		
		#endif
		#endif	
		
	}
	
	return 1;
	
}

stock SpecPlayer(playerid,targetplayer)
{
	new String[128];
	GetPlayerPos(playerid,SpecX[playerid],SpecY[playerid],SpecZ[playerid]);
	Inter[playerid] = GetPlayerInterior(playerid);
	vWorld[playerid] = GetPlayerVirtualWorld(playerid);
	TogglePlayerSpectating(playerid, true);
	if(IsPlayerInAnyVehicle(targetplayer))
	{
		if(GetPlayerInterior(targetplayer) > 0)
		{
			SetPlayerInterior(playerid,GetPlayerInterior(targetplayer));
		}
		if(GetPlayerVirtualWorld(targetplayer) > 0)
		{
			SetPlayerVirtualWorld(playerid,GetPlayerVirtualWorld(targetplayer));
		}
		PlayerSpectateVehicle(playerid,GetPlayerVehicleID(targetplayer));
	}
	else
	{
		if(GetPlayerInterior(targetplayer) > 0)
		{
			SetPlayerInterior(playerid,GetPlayerInterior(targetplayer));
		}
		if(GetPlayerVirtualWorld(targetplayer) > 0)
		{
			SetPlayerVirtualWorld(playerid,GetPlayerVirtualWorld(targetplayer));
		}
		PlayerSpectatePlayer(playerid,targetplayer);
	}
	GetPlayerName(targetplayer, Name, sizeof(Name));
	format(String, sizeof(String),"SERVER: You have started to spectate %s.",Name);
	SendClientMessage(playerid,COLOR_WHITE,String);

	IsSpecing[playerid] = 1;
	IsBeingSpeced[targetplayer] = 1;
	spectatorid[playerid] = targetplayer;
}

stock SpecRandomPlayer(playerid)
{
	new randoms[MAX_PLAYERS],idx;
	for(new i=GetPlayerPoolSize(); i!=-1; i--)
	{
		if(IsPlayerConnected(i) && gPlayerTeamSelection[i] != TEAM_SPECTATOR && playerid != i)
		{
			randoms[idx++]=i;
		}
	}
	TextDrawShowForPlayer(playerid,txtSpectatorHelper);
	if(!idx) return 0; // INVALID_PLAYER_ID
	SpecPlayer(playerid, randoms[random(idx)]);
	return 1;
}

public reveal(playerid)
{
	
	SendClientMessage(playerid,COLOR_WHITE,"SERVER: You are visible again!");
	if(PStealth[playerid] == -1 && (gPlayerTeamSelection[playerid] == FIRST_TEAM))
	{
		PStealth[playerid]=0;
		SetPlayerColor(playerid,COLOR_TEAM_ONE);
		
	}
	if(PStealth[playerid] == -1 && (gPlayerTeamSelection[playerid] == SECOND_TEAM))
	{
		PStealth[playerid]=0;
		SetPlayerColor(playerid,COLOR_TEAM_TWO);
		
	}
	#if defined TEAMSIZE
	#if TEAMSIZE >= 3
	if(PStealth[playerid] == -1 && (gPlayerTeamSelection[playerid] == THIRD_TEAM))
	{
		PStealth[playerid]=0;
		SetPlayerColor(playerid,COLOR_TEAM_THREE);
		
	}
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE >= 4
	if(PStealth[playerid] == -1 && (gPlayerTeamSelection[playerid] == FOURTH_TEAM))
	{
		PStealth[playerid]=0;
		SetPlayerColor(playerid,COLOR_TEAM_FOUR);
		
	}
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE >= 5
	if(PStealth[playerid] == -1 && (gPlayerTeamSelection[playerid] == FIFTH_TEAM))
	{
		PStealth[playerid]=0;
		SetPlayerColor(playerid,COLOR_TEAM_FIVE);
		
	}
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE == 6
	if(PStealth[playerid] == -1 && (gPlayerTeamSelection[playerid] == SIXTH_TEAM))
	{
		PStealth[playerid]=0;
		SetPlayerColor(playerid,COLOR_TEAM_SIX);
		
	}	
	#endif
	#endif		
	
}

stock SellPlayerWeapon(playerid,cost,weaponid,ammo)
{
	if(GetPlayerMoney(playerid) >= cost)
	{

		new spentstring[128];
		format(spentstring,sizeof(spentstring),"SERVER: You spent $%d to purchase this weapon!",cost);
		SendClientMessage(playerid,COLOR_WHITE,spentstring);
		printf("%d spent %d to purchase weapon %d",playerid,cost,weaponid);
		GivePlayerWeapon(playerid,weaponid,ammo);
		GivePlayerMoneyText(playerid,-cost);
		TogglePlayerControllable(playerid,true);
	}
	else
	{
		new leftmoney = (GetPlayerMoney(playerid)-cost);
		new insufstring[128];
		format(insufstring,sizeof(insufstring),"SERVER: You still need $%d to purchase this weapon!",-leftmoney);
		SendClientMessage(playerid,COLOR_WHITE,"SERVER: Insufficient balance! You can not afford this weapon!");
		printf("%d tried to purchase weapon %d but still needs %d",playerid,weaponid,-leftmoney);
		SendClientMessage(playerid,COLOR_WHITE,insufstring);
		TogglePlayerControllable(playerid,true);
	}
}

stock SellPlayerFightingStyle(playerid,cost,style)
{
	if(GetPlayerMoney(playerid) >= cost)
	{

		new spentstring[128];
		format(spentstring,sizeof(spentstring),"SERVER: You spent $%d to purchase this style!",cost);
		SendClientMessage(playerid,COLOR_WHITE,spentstring);
		SetPlayerFightingStyle(playerid,style);
		GivePlayerMoneyText(playerid,-cost);
		TogglePlayerControllable(playerid,true);
	}
	else
	{
		new leftmoney = (GetPlayerMoney(playerid)-cost);
		new insufstring[128];
		format(insufstring,sizeof(insufstring),"SERVER: You still need $%d to purchase this fightstyle!",-leftmoney);
		SendClientMessage(playerid,COLOR_WHITE,"SERVER: Insufficient balance! You can not afford this fightstyle!");
		SendClientMessage(playerid,COLOR_WHITE,insufstring);
		TogglePlayerControllable(playerid,true);
	}
}
stock GenerateRandomPickup(modelid,type,Float:x_max,Float:x_min,Float:y_max,Float:y_min,Float:z_max,Float:z_min,virtualworld)
{
	if(maxmoney <= sizeof(MoneyPickups))
	{
		
		moneyval= moneyval+1000;
		new Float:rx1=((frandom((floatabs(x_max-x_min)))+x_min));
		new Float:ry2=((frandom((floatabs(y_max-y_min)))+y_min));
		new Float:rz3=((frandom((floatabs(z_max-z_min)))+z_min));
		maxmoney = maxmoney +1;
		MoneyPickups[maxmoney] = CreatePickup(modelid,type,rx1,ry2,rz3,virtualworld);
		HeapSort(MoneyPickups);
		
	}


}


FinishedGame()
{
	GameHasFinished=1;
	new scoreTable[MAX_PLAYERS][2], counter;
	for(new i = GetPlayerPoolSize(); i!=-1; i--)
	{
		if(!IsPlayerConnected(i) || IsPlayerNPC(i)) continue;
		scoreTable[i][0] = playerdataInfo[i][pKills];
		scoreTable[i][1] = i;
		counter++;
	}
	SortDeepArray(scoreTable, 0, .order = SORT_DESC);
	new scorestring[512], minutes;
	minutes = totaltime / 60;
	new remainingTeamID;
	if(GetActiveTeamCount() < 2)
	{
		
		for(new i;i<TEAMSIZE;i++)
		{
			if(GetTeamPlayerCount(i) > 0)
			{
				remainingTeamID=i;
				break;
			}
			else
			{
				continue;
			}
			
		}
	}
	format(scorestring,sizeof(scorestring),"Duration of the game: %d minutes (%d seconds) \n Team: %s{A9C4E4} has won the game! ",minutes,totaltime,GetTeamColorTag(remainingTeamID));
	for(new i,pID; i<counter; i++)
	{
		pID = scoreTable[i][1];
		new name[MAX_PLAYER_NAME + 1];
		GetPlayerName(pID, name, sizeof(name));
		format(scorestring,sizeof(scorestring),"%s\n%d. Place: %s (%d) Team: %s{A9C4E4} Total kills: %d Beds destroyed: %d Times died: %d",scorestring,(i+1),name,pID,GetPlayerTeamColorTag(pID),playerdataInfo[pID][pKills],PlayerInfo[pID][pBeds],PlayerInfo[pID][pDeaths]);
	}
	for(new i;i<MAX_PLAYERS;i++)
	{
		ShowPlayerDialog(i, 1234, DIALOG_STYLE_MSGBOX, "Scoreboard",scorestring,"OK","");
	}
	SendClientMessageToAll(COLOR_WHITE,"SERVER: Game over! Map is changing. Please standby..");
	SetTimer("GMX",30000,false);
	return 1;
}

SetPlayerInvulnerable(playerid,godmode)
{
	if(godmode == 1)
	{
		SetPlayerHealth(playerid,0x7F800000);

	}
	else
	{
		SetPlayerHealth(playerid,100.0);
	}
	return 1;
}


public TeamRemaining()
{
	printf("TeamRemaining called");
	if(!GameHasStarted) return 0;
	new remainingTeamID;
	if(GetActiveTeamCount() < 2)
	{
		
		for(new i;i<TEAMSIZE;i++)
		{
			if(GetTeamPlayerCount(i) > 0)
			{
				remainingTeamID=i;
				break;
			}
			else
			{
				continue;
			}
			
		}
	}
	else
	{
		return 0;
	}
	
	for(new d;d<MAX_PLAYERS;d++)
	{
		SendClientMessageEx(d,COLOR_WHITE,"SERVER: All remaining teams have been wiped. Team %s {FFFFFF}has won the game!",GetTeamColorTag(remainingTeamID));
		
	}	
	printf("SERVER: All remaining teams have been wiped. Team %d has won the game!",remainingTeamID);
	for(new o;o<MAX_PLAYERS;o++)
	{
		ResetPlayerData(o);
	}
	FinishedGame();
	return 1;
	
}

public StartGame()
{
	
	
	if(GetActiveTeamCount() > 1)//Amount of teams we need to start the game. Only a value greater than 1 makes sense for a deathmatch like mode.
	{
		CountDownVar--; 
		new str[128];
		if(CountDownVar == 0)
		{
			KillTimer(CountDownTimer2);
			CountDownVar = 59; 
			GameHasStarted =1;
			GameTextForAll("~g~ Start!",1000,6);
			UnlockAllVehicles();
			
			for(new i = 0; i < MAX_PLAYERS; i++)
			{
				if(gPlayerHasTeamSelected[i] == 1)
				{
					SendClientMessageEx(i,COLOR_WHITE,"SERVER: The game will now start! Get ready! We have %d active teams!",GetActiveTeamCount());
					SpawnPlayer(i);
					TogglePlayerControllable(i,1);
					SetTimerEx("TeleportPlayerToBase",100,false,"i",i);
					ResetPlayerData(i);
					SetPlayerInvulnerable(i,0);
					
				}
			}	
		}
		else if(CountDownVar == 30)
		{
			for(new i = 0; i < MAX_PLAYERS; i++)
			{
				PlayerPlaySound(i,1137,0,0,0);
			}
			format(str, sizeof(str), "~g~ %d", CountDownVar);
			GameTextForAll(str, 1000, 6);
			new currentplayers;
			for(new k = 0; k < TEAMSIZE; k++)
			{
				currentplayers = currentplayers + GetTeamPlayerCount(k);
			}
			
			for(new i = 0; i < MAX_PLAYERS; i++)
			{
				SendClientMessageEx(i,COLOR_WHITE,"SERVER: The game will start in 30 seconds. Check /help, or use /report if you need further information. ",currentplayers,GetActiveTeamCount());
			}
			
		}
		else if(CountDownVar == 15)
		{
			for(new i = 0; i < MAX_PLAYERS; i++)
			{
				PlayerPlaySound(i,1137,0,0,0);
			}
			format(str, sizeof(str), "~g~ %d", CountDownVar);
			GameTextForAll(str, 1000, 6);
			new currentplayers;
			for(new k = 0; k < TEAMSIZE; k++)
			{
				currentplayers = currentplayers + GetTeamPlayerCount(k);
			}
			
			for(new i = 0; i < MAX_PLAYERS; i++)
			{
				SendClientMessageEx(i,COLOR_WHITE,"SERVER: The game will start in 15 seconds, prepare to start. There are %d Players in %d teams.",currentplayers,GetActiveTeamCount());
			}
			
		}
		else if(CountDownVar == 5)
		{
			for(new i = 0; i < MAX_PLAYERS; i++)
			{
				PlayerPlaySound(i,1137,0,0,0);
			}
			format(str, sizeof(str), "~g~ %d", CountDownVar);
			GameTextForAll(str, 1000, 6);
			new currentplayers;
			for(new k = 0; k < TEAMSIZE; k++)
			{
				currentplayers = currentplayers + GetTeamPlayerCount(k);
			}
			
			for(new i = 0; i < MAX_PLAYERS; i++)
			{
				SendClientMessageEx(i,COLOR_WHITE,"SERVER: The game will start in 5 seconds, prepare to start. There are %d Players in %d teams.",currentplayers,GetActiveTeamCount());
			}
			
		}
		else
		{
			for(new i = 0; i < MAX_PLAYERS; i++)
			{
				PlayerPlaySound(i,1137,0,0,0);
			}
			format(str, sizeof(str), "~g~ %d", CountDownVar);
			GameTextForAll(str, 1000, 6);
			
		}
		return 1;
		
	}
	else 
	{
		CountDownVar = 61;
	}	
	return 1;
}


public CountDown()
{
	CountDownVarBlowUp--;
	for(new i = 0; i < MAX_PLAYERS; i++)
	{
		if(CountDownVarBlowUp == 0)
		{
			KillTimer(CountDownTimer);
			CountDownVar = 4;
			PlayerPlaySound(i,7419,0,0,0);
		}
		if(CountDownVarBlowUp == 1)
		{
			PlayerPlaySound(i,7418,0,0,0);
		}
		if(CountDownVarBlowUp == 2)
		{
			PlayerPlaySound(i,7417,0,0,0);
		}
	}
	return 1;
}

public warpcount(playerid)
{
	WarpVar[playerid]--;
	new Float:x,Float:y,Float:z;
	GetPlayerPos(playerid,x,y,z);
	
	if(WarpVar[playerid] == 0)
	{
		if(!IsPlayerInRangeOfPoint(playerid,2.0,wX[playerid],wY[playerid],wZ[playerid]))
		{
			KillTimer(WarpTimer);
			WarpVar[playerid] = 4;
			Warppowder[playerid]=1;
			if(IsValidObject(Beam[playerid]))
			DestroyObject(Beam[playerid]);	
			return SendClientMessage(playerid,COLOR_WHITE,"SERVER: You moved right away! Teleport aborted.");
		}
		if(IsValidObject(Beam[playerid]))
		DestroyObject(Beam[playerid]);	
		TeleportPlayerToBase(playerid);
		
		WarpVar[playerid] = 4;	
		KillTimer(WarpTimer);
		PlayerPlaySound(playerid,1137,x,y,z);
	}
	if(WarpVar[playerid] == 1)
	{
		if(!IsPlayerInRangeOfPoint(playerid,2.0,wX[playerid],wY[playerid],wZ[playerid]))
		{
			KillTimer(WarpTimer);
			WarpVar[playerid] = 4;
			Warppowder[playerid]=1;
			if(IsValidObject(Beam[playerid]))
			DestroyObject(Beam[playerid]);	
			return SendClientMessage(playerid,COLOR_WHITE,"SERVER: You moved right away! Teleport aborted.");
			
		}
		PlayerPlaySound(playerid,1137,x,y,z);
		
	}
	if(WarpVar[playerid] == 2)
	{
		if(!IsPlayerInRangeOfPoint(playerid,2.0,wX[playerid],wY[playerid],wZ[playerid]))
		{
			KillTimer(WarpTimer);
			WarpVar[playerid] = 4;
			Warppowder[playerid]=1;
			if(IsValidObject(Beam[playerid]))
			DestroyObject(Beam[playerid]);	
			return SendClientMessage(playerid,COLOR_WHITE,"SERVER: You moved right away! Teleport aborted.");
		}
		PlayerPlaySound(playerid,1137,x,y,z);
		
	}
	if(WarpVar[playerid] == 3)
	{
		if(!IsPlayerInRangeOfPoint(playerid,2.0,wX[playerid],wY[playerid],wZ[playerid]))
		{
			KillTimer(WarpTimer);
			WarpVar[playerid] = 4;
			Warppowder[playerid]=1;
			if(IsValidObject(Beam[playerid]))
			DestroyObject(Beam[playerid]);	
			return SendClientMessage(playerid,COLOR_WHITE,"SERVER: You moved right away! Teleport aborted.");
		}
		PlayerPlaySound(playerid,1137,x,y,z);
		
	}
	
	
	return 1;
}



public SendTeamMessage(teamid, color, const message[])
{
	for(new i; i < MAX_PLAYERS; i++)
	{
		if(!IsPlayerConnected(i)) continue;
		if(gPlayerTeamSelection[i] != teamid) continue;

		SendClientMessage(i, color, message);
	}
	return 1;
}

public OnPlayerExitedMenu(playerid)
{
	TogglePlayerControllable(playerid,true);
	return 1;
}

stock DropPlayerWeaponPickup(weaponid,ammo,Float:x,Float:y,Float:z)
{
	
}

public OnPlayerPickUpPickup(playerid, pickupid)
{
	
	
	new index;
	switch(GetPickupType(pickupid,index))
	{
	case INVALID_PICKUP_TYPE:
		{
		}
	case MONEY_TYPE:
		{
			maxmoney -= 1;
			if(GameHasStarted == 1) GivePlayerMoney(playerid, MoneyVal);
			MoneyPickups[index]=-1;
			quickSort(MoneyPickups,0,sizeof(MoneyPickups)-1);
			DestroyPickup(pickupid);
		}
	case ACTOR_TYPE:
		{
			if(GameHasStarted == 1)
			{
				ShowMenuForPlayer(shopmenu,playerid);
				TogglePlayerControllable(playerid,false);
			}
		}
	case INFO_TYPE:
		{
			ShowMenuForPlayer(infomenu,playerid);
			TogglePlayerControllable(playerid,false);
		}
	}
	return 1;
	
}




stock GetPickupType(pickupid, &index)
{
	new mres = binarysearch(MoneyPickups,pickupid,0,sizeof(MoneyPickups)-1);
	if(mres > -1 && MoneyPickups[mres] == pickupid) return index=mres,MONEY_TYPE;
	new ares = binarysearch(ActorPickups,pickupid,0,sizeof(ActorPickups)-1);
	if(ares > -1 && ActorPickups[ares] == pickupid) return index=ares,ACTOR_TYPE;
	new ires = binarysearch(InfoPickups,pickupid,0,sizeof(InfoPickups)-1);
	if(ires > -1 && InfoPickups[ires] == pickupid) return index=ires,INFO_TYPE;
	
	
	
	return INVALID_PICKUP_TYPE;
}

public UpdateTimeAndWeather()
{
	
	//Check if all players have left the game because no one can join.
	if(GetPlayerCount() == 0 && GameHasStarted)
	{
		SendRconCommand("gmx");
	}

	gettime(hour, minute);
	if(GameHasStarted)
	{
		totaltime = totaltime + 60;
	}
	format(timestr,32,"%02d:%02d",hour,minute);
	format(timestrex,32,"Time: %02d:%02d",hour,minute);
	TextDrawSetString(txtTimeDisp,timestr);
	if(!GameHasStarted)
	{
		SendClientMessageToAll(COLOR_WHITE,"SERVER: The game has not started yet. We are still waiting for players.");
		SendClientMessageToAll(COLOR_WHITE,"SERVER: You can get further information using /help, or ask your question using /report.");
	}

	for(new i;i<MAX_PLAYERS;i++)
	{
		if(!IsPlayerConnected(i)) continue;
		if(IsPlayerAdmin(i))
		PlayerInfo[i][pAdmin]=1;	
	}

	
	SetWorldTime(hour);

	new x=0;
	while(x!=MAX_PLAYERS) {
		if(IsPlayerConnected(x)) {
			SetPlayerTime(x,hour,minute);
		}
		x++;
	}

	
}
main()
{
	print(" Knogles Last Bed Standing Masterpiece for SA-MP");
	print("	Fork of Minecrafts Bedwars");
	print("	Little TDM Gamemode");
	print("	Published under GNU GPL Public License.");
	print("	Copyright (C) 2017  Fabian Druschke");
	print("	Contact: webmaster@knogleinsi.de");
	print("	Mail: Postfach 32 22 50147 Kerpen Germany");
	print("   \n This program is free software: you can redistribute it and/or modify");
	print("    it under the terms of the GNU General Public License as published by");
	print("    the Free Software Foundation, either version 3 of the License, or");
	print("    (at your option) any later version.");
	print("   \n This program is distributed in the hope that it will be useful,");
	print("    but WITHOUT ANY WARRANTY; without even the implied warranty of");
	print("    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the");
	print("    GNU General Public License for more details.");
	print("   \n You should have received a copy of the GNU General Public License");
	print("    along with this program.  If not, see http://www.gnu.org/licenses/");
	printf("\n\nHeapspace: %i kilobytes", heapspace() / 1024);

}

//----------------------------------------------------------

stock Float:GetDistance( Float:a1, Float:b1, Float:c1, Float:a2, Float:b2, Float:c2 )
{
	return  floatsqroot(((a1-a2)*(a1-a2))+((b1-b2)*(b1-b2))+((c1-c2)*(c1-c2)));
}

stock HeapSort (array [], n = sizeof (array))
{
	new i;
	for (i = n / 2; i > 0; --i) // Make heap
	SinkDown (array, i, n);

	for (i = n; i > 1; --i)
	{
		Swap (array [i - 1], array [0]);
		SinkDown (array, 1, i - 1);
	}
}/*
stock strcpy(dest[], const source[], maxlength=sizeof dest)
{
	strcat((dest[0] = EOS, dest), source, maxlength);
}*/
stock quickSort(array[], left, right)
{
	new
	tempLeft = left,
	tempRight = right,
	pivot = array[(left + right) / 2],
	tempVar
	;
	while(tempLeft <= tempRight)
	{
		while(array[tempLeft] < pivot) tempLeft++;
		while(array[tempRight] > pivot) tempRight--;

		if(tempLeft <= tempRight)
		{
			tempVar = array[tempLeft], array[tempLeft] = array[tempRight], array[tempRight] = tempVar;
			tempLeft++, tempRight--;
		}
	}
	if(left < tempRight) quickSort(array, left, tempRight);
	if(tempLeft < right) quickSort(array, tempLeft, right);
}



//----------------------------------------------------------

static stock SinkDown (array [], i, m)
{
	new j;
	while (2 * i <= m)
	{
		j = 2 * i;
		if (j < m && array [j - 1] < array [j])
		++j;
		
		if (array [i - 1] < array [j - 1])
		{
			Swap (array [i - 1], array [j - 1]);
			i = j;
		}
		else
		i = m;
	}
}

//----------------------------------------------------------

static stock Swap (&a, &b)
{
	new s;
	s = a;
	a = b;
	b = s;
}

//----------------------------------------------------------
stock PreloadAnimLib(playerid, animlib[]) 
{
	ApplyAnimation(playerid,animlib,"null",0.0,0,0,0,0,0);
}


public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	switch (dialogid)
	{
	case DIALOG_LOGIN:
		{
			if(!response) return Kick(playerid);

			new Salted_Key[65];
			SHA256_PassHash(inputtext, playerdataInfo[playerid][Salt], Salted_Key, 65);

			if(strcmp(Salted_Key, playerdataInfo[playerid][pPass]) == 0)
			{
				// Now, password should be correct as well as the strings
				// Matched with each other, so nothing is wrong until now.
				
				// We will activate the cache of player to make use of it e.g.
				// Retrieve their data.
				
				cache_set_active(playerdataInfo[playerid][Player_Cache]);

				// Okay, we are retrieving the information now..
				cache_get_value_int(0, "ID", playerdataInfo[playerid][ID]);
				
				cache_get_value_int(0, "KILLS", playerdataInfo[playerid][pKills]);
				cache_get_value_int(0, "DEATHS", playerdataInfo[playerid][pDeaths]);

				cache_get_value_int(0, "BEDS", playerdataInfo[playerid][pBeds]);
				cache_get_value_int(0, "BOMBS", playerdataInfo[playerid][pBombs]);
				
				SetPlayerScore(playerid, playerdataInfo[playerid][pBeds]);
				
				ResetPlayerMoney(playerid);
				GivePlayerMoney(playerid, playerdataInfo[playerid][pBombs]);

				// So, we have successfully retrieved data? Now deactivating the cache.
				
				cache_delete(playerdataInfo[playerid][Player_Cache]);
				playerdataInfo[playerid][Player_Cache] = MYSQL_INVALID_CACHE;

				playerdataInfo[playerid][LoggedIn] = true;
				SendClientMessage(playerid, 0x00FF00FF, "You have been successfully logged in!");
			}
			else
			{
				new String[150];
				
				playerdataInfo[playerid][pPassFails] += 1;
				printf("%s has been failed to login. (%d)", playerdataInfo[playerid][pName], playerdataInfo[playerid][pPassFails]);
				// Printing the message that someone has failed to login to his account.

				if (playerdataInfo[playerid][pPassFails] >= 3) // If the fails exceeded the limit we kick the player.
				{
					format(String, sizeof(String), "%s has been kicked Reason: {FF0000}(%d/3) Login fails.", playerdataInfo[playerid][pName], playerdataInfo[playerid][pPassFails]);
					SendClientMessageToAll(0x969696FF, String);
					Kick(playerid);
				}
				else
				{
					// If the player didn't exceed the limits we send him a message that the password is wrong.
					format(String, sizeof(String), "Wrong password, you have %d out of 3 tries.", playerdataInfo[playerid][pPassFails]);
					SendClientMessage(playerid, 0xFF0000FF, String);
					
					format(String, sizeof(String), "{FFFFFF}Welcome back, %s.\n\n{0099FF}This account is already registered.\n\
					{FFFFFF}Type in your password below to login.\n\n", playerdataInfo[playerid][pName]);
					ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, "Login System", String, "Login", "Leave");
				}
			}
		}
	case DIALOG_REGISTER:
		{
			if(!response) return Kick(playerid);

			if(strlen(inputtext) <= 5 || strlen(inputtext) > 60)
			{
				// If the password length is less than or equal to 5 and more than 60
				// It repeats the process and shows error message as seen below.
				
				SendClientMessage(playerid, 0x969696FF, "Invalid password length, should be 5 - 60.");

				new String[150];
				
				format(String, sizeof(String), "{FFFFFF}Welcome %s.\n\n{0099FF}This account is not registered.\n\
				{0099FF}Type in your password below to register a new account.\n\n", playerdataInfo[playerid][pName]);
				SendClientMessage(playerid,COLOR_WHITE,"SERVER: We found out, you are playing for the first time on this server.");
				SendClientMessage(playerid,COLOR_WHITE,"SERVER: Please consider using the /help command to know how the game works.");
				ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_PASSWORD, "Registration System", String, "Register", "Leave");
			}
			else
			{

				// Salting the player's password using SHA256 for a better security.
				
				for (new i = 0; i < 10; i++)
				{
					playerdataInfo[playerid][Salt][i] = random(79) + 47;
				}
				
				playerdataInfo[playerid][Salt][10] = 0;
				SHA256_PassHash(inputtext, playerdataInfo[playerid][Salt], playerdataInfo[playerid][pPass], 65);

				new DB_Query[225];
				
				// Storing player's information if everything goes right.
				mysql_format(Database, DB_Query, sizeof(DB_Query), "INSERT INTO `PLAYERS` (`USERNAME`, `PASSWORD`, `SALT`, `BEDS`, `KILLS`, `BOMBS`, `DEATHS`)\
				VALUES ('%e', '%s', '%e', '0', '0', '0', '0')", playerdataInfo[playerid][pName], playerdataInfo[playerid][pPass], playerdataInfo[playerid][Salt]);
				mysql_tquery(Database, DB_Query, "OnPlayerRegister", "d", playerid);
			}
		}
	}
	return 1;
}

forward public OnPlayerDataCheck(playerid, corrupt_check);
public OnPlayerDataCheck(playerid, corrupt_check)
{
	if (corrupt_check != Corrupt_Check[playerid]) return Kick(playerid);
	// You'd have asked already what's corrput_check and how it'd benefit me?
	// Well basically MySQL query takes long, incase a player leaves while its not proceeded
	// With ID 1 for example, then another player comes as ID 1 it'll basically corrupt the data
	// So, once the query is done, the player will have the wrong data assigned for himself.

	new String[150];
	
	if(cache_num_rows() > 0)
	{
		// If the player exists, everything is okay and nothing is wrongly detected
		// The player's password and Saltion key gets stored as seen below
		// So we won't have to get a headache just to match player's password.
		
		cache_get_value(0, "PASSWORD", playerdataInfo[playerid][pPass], 65);
		cache_get_value(0, "SALT", playerdataInfo[playerid][Salt], 11);

		playerdataInfo[playerid][Player_Cache] = cache_save();
		// ^ Storing the cache ID of the player for further use later.

		format(String, sizeof(String), "{FFFFFF}Welcome back, %s.\n\n{0099FF}This account is already registered.\n\
		{0099FF}Please, input your password below to proceed to the game.\n\n", playerdataInfo[playerid][pName]);
		ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, "Login System", String, "Login", "Leave");
	}
	else
	{
		format(String, sizeof(String), "{FFFFFF}Welcome %s.\n\n{0099FF}This account is not registered.\n\
		{0099FF}Please, input your password below to proceed to the game.\n\n", playerdataInfo[playerid][pName]);
		ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_PASSWORD, "Registration System", String, "Register", "Leave");
	}
	return 1;
}

forward public OnPlayerRegister(playerid);
public OnPlayerRegister(playerid)
{
	// This gets called only when the player registers a new account.
	SendClientMessage(playerid, 0x00FF00FF, "You are now registered and has been logged in.");
	playerdataInfo[playerid][LoggedIn] = true;
	return 1;
}
public OnPlayerConnect(playerid)
{
	WarpVar[playerid]=4;
	PreloadAnimLib(playerid,"BOMBER");
	
	SetPlayerMapIcon(playerid, 12,Center[0][0],Center[0][1],Center[0][2], 52, 0, MAPICON_GLOBAL);

	new DB_Query[115];

	//Resetting player information.
	playerdataInfo[playerid][pKills] = 0;
	playerdataInfo[playerid][pDeaths] = 0;
	playerdataInfo[playerid][pPassFails] = 0;

	GetPlayerName(playerid, playerdataInfo[playerid][pName], MAX_PLAYER_NAME); // Getting the player's name.
	Corrupt_Check[playerid]++;
	
	mysql_format(Database, DB_Query, sizeof(DB_Query), "SELECT * FROM `PLAYERS` WHERE `USERNAME` = '%e' LIMIT 1", playerdataInfo[playerid][pName]);
	mysql_tquery(Database, DB_Query, "OnPlayerDataCheck", "ii", playerid, Corrupt_Check[playerid]);
	RemoveBuildingForPlayer(playerid, 694,-2406.2607,-1949.2020,303.7252, 200.0);
	RemoveBuildingForPlayer(playerid, 784,-2406.2607,-1949.2020,303.7252, 200.0);
	RemoveBuildingForPlayer(playerid, 3636, 1220.2734375,2590.296875,16.4453125, 10000.0);
	RemoveBuildingForPlayer(playerid,3683, 2377.390625,1743.234375,16.5390625,10000);
	RemoveBuildingForPlayer(playerid, 3673, -2137.1953125,70.28125,42.3125, 10000.0);
	RemoveBuildingForPlayer(playerid, 3682,1249.5546875,-905.4609375,45.03125,10000);
	RemoveBuildingForPlayer(playerid, 3259,1249.5546875,-905.4609375,45.03125,10000);
	RemoveBuildingForPlayer(playerid, 3424, 1568.2890625,-1677.78125,10.8203125, 10000.0);
	RemoveBuildingForPlayer(playerid, 3674, 1568.2890625,-1677.78125,10.8203125, 10000.0);
	RemoveBuildingForPlayer(playerid, 3675, 1568.2890625,-1677.78125,10.8203125, 10000.0);
	RemoveBuildingForPlayer(playerid, 3256, 1568.2890625,-1677.78125,10.8203125, 10000.0);
	RemoveBuildingForPlayer(playerid, 3258, 1220.2734375,2590.296875,16.4453125, 10000.0);
	RemoveBuildingForPlayer(playerid,3673, 2377.390625,1743.234375,16.5390625,10000);
	RemoveBuildingForPlayer(playerid, 3255, -2137.1953125,70.28125,42.3125, 10000.0);
	RemoveBuildingForPlayer(playerid, 3257,1249.5546875,-905.4609375,45.03125,10000);

	RemoveBuildingForPlayer(playerid, 3290, 1220.2734375,2590.296875,16.4453125, 10000.0);
	RemoveBuildingForPlayer(playerid,3289, 2377.390625,1743.234375,16.5390625,10000);
	RemoveBuildingForPlayer(playerid, 3682, -2137.1953125,70.28125,42.3125, 10000.0);
	RemoveBuildingForPlayer(playerid, 3682,1249.5546875,-905.4609375,45.03125,10000);
	RemoveBuildingForPlayer(playerid, 3291,1249.5546875,-905.4609375,45.03125,10000);
	RemoveBuildingForPlayer(playerid, 3288, 1568.2890625,-1677.78125,10.8203125, 10000.0);
	RemoveBuildingForPlayer(playerid, 3291, 1568.2890625,-1677.78125,10.8203125, 10000.0);
	RemoveBuildingForPlayer(playerid, 3683, 1568.2890625,-1677.78125,10.8203125, 10000.0);
	RemoveBuildingForPlayer(playerid, 16086, 1568.2890625,-1677.78125,10.8203125, 10000.0);
	PStealth[playerid]=0;
	LastMoney[playerid]=0;
	new pname[MAX_PLAYER_NAME], string[22 + MAX_PLAYER_NAME];
	GetPlayerName(playerid, pname, sizeof(pname));
	format(string, sizeof(string), "%s (%d) has joined the server.", pname,playerid);
	SendClientMessageToAll(0xAAAAAAAA, string);
	PlayerPlaySound(playerid,1185,0,0,0);
	GameTextForPlayer(playerid,"~w~Last Bed Standing",3000,4);
	SendClientMessage(playerid,COLOR_WHITE,"Welcome to {88AA88}B{FFFFFF}ed {88AA88}W{FFFFFF}ars");
	gPlayerTeamSelection[playerid] = -1;
	gPlayerHasTeamSelected[playerid] = 0;
	gPlayerLastTeamSelectionTick[playerid] = GetTickCount();
	return 1;
}
stock ClassSel_SetupCharSelection(playerid)
{
	TextDrawHideForPlayer(playerid,Textcover);
	if(gPlayerTeamSelection[playerid] > -1) 
	{
		SetPlayerVirtualWorld(playerid,1);
		SetPlayerPos(playerid,243.1261,1805.2798,8.3794);
		SetPlayerFacingAngle(playerid,180.0);
		SetPlayerCameraPos(playerid,243.2876,1802.5547,7.4141);
		SetPlayerCameraLookAt(playerid,243.1261,1805.2798,8.3794);
		
		#if defined TEAMSIZE
		#if TEAMSIZE >= 2	

		
		switch(gPlayerTeamSelection[playerid])
		{
		case FIRST_TEAM:
			{
				SetPlayerColor(playerid,COLOR_TEAM_ONE);	
			}
		case SECOND_TEAM:
			{
				SetPlayerColor(playerid,COLOR_TEAM_TWO);	
			}
			
			
			#endif
			#endif	
			#if defined TEAMSIZE
			#if TEAMSIZE >= 3	
		case THIRD_TEAM:
			{
				SetPlayerColor(playerid,COLOR_TEAM_THREE);	
			}
			#endif
			#endif	
			#if defined TEAMSIZE
			#if TEAMSIZE >= 4

		case FOURTH_TEAM:
			{
				SetPlayerColor(playerid,COLOR_TEAM_FOUR);	
			}
			
			
			#endif
			#endif	
			#if defined TEAMSIZE
			#if TEAMSIZE >= 5	

			
			
		case FIFTH_TEAM:
			{
				SetPlayerColor(playerid,COLOR_TEAM_FIVE);	
			}
			
			
			#endif
			#endif
			#if defined TEAMSIZE
			#if TEAMSIZE >= 6	

			
			
		case SIXTH_TEAM:
			{
				SetPlayerColor(playerid,COLOR_TEAM_SIX);	
			}
			
			
			#endif
			#endif	
		}
	}
}

stock ClassSel_InitTeamNameText(playerid,PlayerText:txtInit)
{
	PlayerTextDrawBackgroundColor(playerid,txtInit,0x000000ff);
	PlayerTextDrawFont(playerid,txtInit,2);
	PlayerTextDrawLetterSize(playerid,txtInit,0.399999,1.500000);
	PlayerTextDrawColor(playerid,txtInit,0xffffffff);
	PlayerTextDrawSetOutline(playerid,txtInit,1);
	PlayerTextDrawSetProportional(playerid,txtInit,1);
	PlayerTextDrawSetShadow(playerid,txtInit,1);
	PlayerTextDrawBackgroundColor(playerid,txtInit,0x000000ff);
	PlayerTextDrawFont(playerid,txtInit,2);
	PlayerTextDrawLetterSize(playerid,txtInit,0.399999,1.500000);
	PlayerTextDrawColor(playerid,txtInit,0xffffffff);
	PlayerTextDrawSetOutline(playerid,txtInit,1);
	PlayerTextDrawSetProportional(playerid,txtInit,1);
	PlayerTextDrawSetShadow(playerid,txtInit,1);
}

stock ClassSel_SwitchToNextTeam(playerid)
{
	gPlayerTeamSelection[playerid]++;

	
	switch(TEAMSIZE)
	{
	case 2:
		{
			if(gPlayerTeamSelection[playerid] > SECOND_TEAM) 
			{
				gPlayerTeamSelection[playerid] = FIRST_TEAM;
			}
		}
	case 3:
		{
			if(gPlayerTeamSelection[playerid] > THIRD_TEAM) 
			{
				gPlayerTeamSelection[playerid] = FIRST_TEAM;
			}
		}
	case 4:
		{
			if(gPlayerTeamSelection[playerid] > FOURTH_TEAM) 
			{
				gPlayerTeamSelection[playerid] = FIRST_TEAM;
			}
		}
	case 5:
		{
			if(gPlayerTeamSelection[playerid] > FIFTH_TEAM) 
			{
				gPlayerTeamSelection[playerid] = FIRST_TEAM;
			}
		}
	case 6:
		{
			if(gPlayerTeamSelection[playerid] > SIXTH_TEAM) 
			{
				gPlayerTeamSelection[playerid] = FIRST_TEAM;
			}
		}
	}
	PlayerPlaySound(playerid,1052,0.0,0.0,0.0);
	gPlayerLastTeamSelectionTick[playerid] = GetTickCount();
	ClassSel_SetupSelectedTeam(playerid);
}


stock ClassSel_SwitchToPreviousTeam(playerid)
{
	gPlayerTeamSelection[playerid]--;

	switch(TEAMSIZE)
	{
	case 2:
		{
			if(gPlayerTeamSelection[playerid] < FIRST_TEAM) 
			{
				gPlayerTeamSelection[playerid] = SECOND_TEAM;
			}
		}
	case 3:
		{
			if(gPlayerTeamSelection[playerid] < FIRST_TEAM) 
			{
				gPlayerTeamSelection[playerid] = THIRD_TEAM;
			}
		}
	case 4:
		{
			if(gPlayerTeamSelection[playerid] < FIRST_TEAM) 
			{
				gPlayerTeamSelection[playerid] = FOURTH_TEAM;
			}
		}
	case 5:
		{
			if(gPlayerTeamSelection[playerid] < FIRST_TEAM) 
			{
				gPlayerTeamSelection[playerid] = FIFTH_TEAM;
			}
		}
	case 6:
		{
			if(gPlayerTeamSelection[playerid] < FIRST_TEAM) 
			{
				gPlayerTeamSelection[playerid] = SIXTH_TEAM;
			}
		}
	}
	PlayerPlaySound(playerid,1053,0.0,0.0,0.0);
	gPlayerLastTeamSelectionTick[playerid] = GetTickCount();
	ClassSel_SetupSelectedTeam(playerid);
}

stock getActingPlayer(playerid,&idx)
{
	new randoms[MAX_PLAYERS],tmp;
	for(new i=GetPlayerPoolSize(); i!=-1; i--)
	{
		if(IsPlayerConnected(i) && gPlayerTeamSelection[i] != TEAM_SPECTATOR && playerid != i)
		{
			randoms[tmp++]=i;
		}
	}
	if(!tmp) return INVALID_PLAYER_ID;
	if(idx>tmp) idx=0;
	if(idx<0) idx=tmp-1;
	return randoms[idx];
}
stock HandlePlayerSpectating(playerid)
{
	if( (GetTickCount() - gPlayerLastTeamSelectionTick[playerid]) < delay ) return 1;
	new Keys,ud,lr;
	GetPlayerKeys(playerid,Keys,ud,lr);
	if(lr == KEY_RIGHT)
	{
		SpectatingPlayer[playerid]++;
		ud = getActingPlayer(playerid,SpectatingPlayer[playerid]);
		if(ud == INVALID_PLAYER_ID)
		{
			SpectatingPlayer[playerid]--;
			return 1;
		}
		SpecPlayer(playerid,ud);
	}
	else if(lr == KEY_LEFT)
	{
		SpectatingPlayer[playerid]--;
		ud = getActingPlayer(playerid,SpectatingPlayer[playerid]);
		if(ud == INVALID_PLAYER_ID)
		{
			SpectatingPlayer[playerid]++;
			return 1;
		}
		SpecPlayer(playerid,ud);
	}
	return 1;
}
stock ClassSel_HandleTeamSelection(playerid)
{

	new Keys,ud,lr;
	GetPlayerKeys(playerid,Keys,ud,lr);

	if(gPlayerTeamSelection[playerid] == -1) {
		ClassSel_SwitchToNextTeam(playerid);
		return;
	}


	if( (GetTickCount() - gPlayerLastTeamSelectionTick[playerid]) < delay ) return;

	if(Keys & KEY_FIRE) {
		gPlayerHasTeamSelected[playerid] = 1;
		
		PlayerTextDrawHide(playerid,TeamText[playerid]);
		PlayerTextDrawHide(playerid,LocText[playerid]);
		PlayerTextDrawHide(playerid,TeamCover[playerid]);
		TextDrawHideForPlayer(playerid,txtClassSelHelper);//Observer textdraw
		TogglePlayerSpectating(playerid,0);
		return;
	}
	if(lr > 0) {
		ClassSel_SwitchToNextTeam(playerid);
	}
	else if(lr < 0) {
		ClassSel_SwitchToPreviousTeam(playerid);
	}

}


public OnPlayerRequestClass(playerid, classid)
{
	if(IsPlayerNPC(playerid)) return 1;
	if(gPlayerHasTeamSelected[playerid]) {
		ClassSel_SetupCharSelection(playerid);
		return 1;
	} 
	else 
	{
		if(GetPlayerState(playerid) != PLAYER_STATE_SPECTATING) {
			TogglePlayerSpectating(playerid,1);
			TextDrawShowForPlayer(playerid, txtClassSelHelper);
			gPlayerTeamSelection[playerid] = -1;
		}
	}

	return 0;
}

//----------------------------------------------------------
public OnPlayerSelectedMenuRow(playerid, row)
{
	new Menu:CurrentMenu = GetPlayerMenu(playerid);
	new Menu:Current= GetPlayerMenu(playerid);
	TogglePlayerControllable(playerid,false);
	if(Current==infomenu) 
	{
		TogglePlayerControllable(playerid,false);
		switch(row) 
		{
		case 0:
			{
				TogglePlayerControllable(playerid,true);
				SendClientMessage(playerid,COLOR_WHITE,"SERVER: How to play"); 
				SendClientMessage(playerid,COLOR_WHITE,"The goal of this mode, is to destroy the enemie's bed and wipe out all the remaining players to determine the winner team."); 
				SendClientMessage(playerid,COLOR_WHITE,"You can use the /blowup command to destroy and enemies bed!");
			}
		case 1:
			{
				TogglePlayerControllable(playerid,true);
				SendClientMessage(playerid,COLOR_WHITE,"SERVER: Q: How to destroy a bed?");
				SendClientMessage(playerid,COLOR_WHITE,"SERVER: A: Get close to the enemie's bed and use /blowup to destroy it!");
				SendClientMessage(playerid,COLOR_WHITE,"SERVER: Q: What can i do if i am unable to respawn due to a destroyed bed?");
				SendClientMessage(playerid,COLOR_WHITE,"SERVER: A: You can use the command /spec [playerid] to spectate other players!");
				SendClientMessage(playerid,COLOR_WHITE,"SERVER: Q: When does the game ends?");
				SendClientMessage(playerid,COLOR_WHITE,"SERVER: A: After a winning team is determined, means if only one team remains.");
			}
		case 2:
			{
				TogglePlayerControllable(playerid,true);
				SendClientMessage(playerid,COLOR_WHITE,"SERVER: If you commit one of these things it will get you banned!");
				SendClientMessage(playerid,COLOR_WHITE,"SERVER: NO Cheating");
				SendClientMessage(playerid,COLOR_WHITE,"SERVER: NO Spamming");
				SendClientMessage(playerid,COLOR_WHITE,"SERVER: NO Quitting to avoid anything! Better die in honor for a honorable cause!");
				SendClientMessage(playerid,COLOR_WHITE,"SERVER: NEVER kill your own team mates, it will get you banned!");
				SendClientMessage(playerid,COLOR_WHITE,"SERVER: NEVER exploit bugs in any way! Report them instead!");
			}
		}
	}
	if(Current==ammunation) {
		TogglePlayerControllable(playerid,false);
		switch(row) {
		case 0:	{
				ShowMenuForPlayer(pistols,playerid);
			}
		case 1: {
				ShowMenuForPlayer(microsmg,playerid);
			}
		case 2: {
				ShowMenuForPlayer(shotguns,playerid);
			}
		case 3: {
				ShowMenuForPlayer(items,playerid);
			}
		case 4: {
				ShowMenuForPlayer(smg,playerid);
			}
		case 5: {
				ShowMenuForPlayer(rifles,playerid);
			}
		case 6: {
				ShowMenuForPlayer(assaultrifle,playerid);
			}
		case 7: {
				ShowMenuForPlayer(Grenades,playerid);
			}
		case 8: {
				ShowMenuForPlayer(melee,playerid);
			}
		case 9: {
				ShowMenuForPlayer(special,playerid);
			}
		case 10: {
				HideMenuForPlayer(ammunation,playerid);
				ShowMenuForPlayer(shopmenu,playerid);
				
			}
		default: {
				print("Fail");
			}
		}
	}
	else if(Current==pistols) {
		ShowMenuForPlayer(pistols,playerid);
		switch(row) {
		case 0:
			{
				SellPlayerWeapon(playerid,1500,22,50);
			}
		case 1:
			{
				SellPlayerWeapon(playerid,2500,23,50);


			}
		case 2:

			{
				SellPlayerWeapon(playerid,2500,24,50);
			}
		case 3: {

				HideMenuForPlayer(pistols,playerid);
				ShowMenuForPlayer(ammunation,playerid);
			}
		default: {
				print("Fail");
			}
		}
	}
	else if(Current==microsmg) {
		ShowMenuForPlayer(microsmg,playerid);
		switch(row) {
		case 0:


			{
				SellPlayerWeapon(playerid,5500,32,50);

			}

		case 1:

			{
				SellPlayerWeapon(playerid,5000,28,50);

			}
		case 2: {
				HideMenuForPlayer(microsmg,playerid);
				ShowMenuForPlayer(ammunation,playerid);
			}
		default: {
				print("Fail");
			}
		}
	}
	else if(Current==shotguns) {
		ShowMenuForPlayer(shotguns,playerid);
		switch(row) {
		case 0:	{
				SellPlayerWeapon(playerid,12500,25,50);

			}
		case 1: {
				SellPlayerWeapon(playerid,15500,26,50);

			}
		case 2: {
				SellPlayerWeapon(playerid,55500,27,50);
			}
		case 3: {
				HideMenuForPlayer(shotguns,playerid);
				ShowMenuForPlayer(ammunation,playerid);
			}
		default: {
				print("Fail");
			}
		}
	}
	else if(Current==items) {
		ShowMenuForPlayer(items,playerid);
		switch(row) {
		case 0:
			if(GetPlayerMoney(playerid) >= 2500)
			{
				GivePlayerMoneyText(playerid,-2500);
				SetPlayerArmour(playerid,100.0);
			}
			else
			{
				SendClientMessage(playerid,COLOR_WHITE,"SERVER: Insufficient balance! You can not afford this item!");
			}
		case 1:	{
				SellPlayerWeapon(playerid,2500,46,1);
			}
		case 2:	{
				SellPlayerWeapon(playerid,2500,41,100);
			}
		case 3:	{
				SellPlayerWeapon(playerid,2500,43,50);

			}

		case 4:	if(GetPlayerMoney(playerid) >= 2500)
			{
				GivePlayerMoneyText(playerid,-2500);
				SetPlayerHealth(playerid,100.0);
			}
			else
			{
				SendClientMessage(playerid,COLOR_WHITE,"SERVER: Insufficient balance! You can not afford this item!");
			}
		case 5:		
			{
				HideMenuForPlayer(items,playerid);
				ShowMenuForPlayer(fightstyles,playerid);
			}
		case 6: {
				HideMenuForPlayer(items,playerid);
				ShowMenuForPlayer(ammunation,playerid);
			}
		default: {
				print("Fail");
			}
		}
	}
	
	else if(Current==fightstyles) {
		ShowMenuForPlayer(fightstyles,playerid);
		switch(row) {
		case 0:

			{
				SellPlayerFightingStyle (playerid,2500, FIGHT_STYLE_KUNGFU);
			}
		case 1: {
				SellPlayerFightingStyle (playerid,2500, FIGHT_STYLE_KNEEHEAD);

			}
		case 2:
			{
				SellPlayerFightingStyle (playerid,2500, FIGHT_STYLE_BOXING);
			}
		case 3:
			{
				SellPlayerFightingStyle (playerid,2500, FIGHT_STYLE_GRABKICK);
			}
		case 4:
			{
				SellPlayerFightingStyle (playerid,2500, FIGHT_STYLE_ELBOW);
			}
		case 5:
			{
				SetPlayerFightingStyle (playerid, FIGHT_STYLE_NORMAL);
			}
		case 6:
			{
				HideMenuForPlayer(fightstyles,playerid);
				ShowMenuForPlayer(items,playerid);
			}
		default: {
				print("Fail");
			}
		}
	}
	else if(Current==smg) {
		ShowMenuForPlayer(smg,playerid);
		switch(row) {
		case 0:

			{
				SellPlayerWeapon(playerid,20000,29,50);



			}
		case 1: {
				HideMenuForPlayer(smg,playerid);
				ShowMenuForPlayer(ammunation,playerid);

			}
		default: {
				print("Fail");
			}
		}
	}
	
	else if(Current==assaultrifle) {
		ShowMenuForPlayer(assaultrifle,playerid);
		switch(row) {
		case 0:
			{
				SellPlayerWeapon(playerid,40000,30,500);
			}

		case 1:	{
				SellPlayerWeapon(playerid,45000,31,500);

			}
		case 2: {
				HideMenuForPlayer(assaultrifle,playerid);
				ShowMenuForPlayer(ammunation,playerid);
			}
		default: {
				print("Fail");
			}
		}
	}
	else if(Current==rifles) {
		ShowMenuForPlayer(rifles,playerid);
		switch(row) {
		case 0:	{
				SellPlayerWeapon(playerid,12500,33,50);


			}
		case 1:	{
				SellPlayerWeapon(playerid,40000,34,50);
			}
		case 2: {
				HideMenuForPlayer(rifles,playerid);
				ShowMenuForPlayer(ammunation,playerid);

			}
		default: {
				print("Fail");
			}
		}
	}
	else if(Current==Grenades) {
		ShowMenuForPlayer(Grenades,playerid);
		switch(row) {
		case 0:	{
				SellPlayerWeapon(playerid,125000,16,50);
			}
		case 1:	{
				SellPlayerWeapon(playerid,25000,17,50);
			}
		case 2:	{
				SellPlayerWeapon(playerid,25000,18,50);
			}
		case 3: {
				HideMenuForPlayer(Grenades,playerid);
				ShowMenuForPlayer(ammunation,playerid);
			}
		default: {
				print("Fail");
			}
		}
	}
	else if(Current==yesno) {
		ShowMenuForPlayer(yesno,playerid);
		switch(row) {
		case 0:	{
				SetPlayerHealth(playerid,0.0);
				SendDeathMessage(playerid, playerid, 22);
			}
		case 1:	{
				HideMenuForPlayer(yesno,playerid);
				TogglePlayerControllable(playerid,true);
			}
		case 2:	{
				HideMenuForPlayer(yesno,playerid);
				ShowMenuForPlayer(shopmenu,playerid);
			}
		default: {
				print("Fail");
			}
		}
	}
	else if(Current==melee) {
		ShowMenuForPlayer(melee,playerid);
		switch(row) {
		case 0:	{
				GivePlayerWeapon(playerid,5,1);
			}
		case 1:	{
				GivePlayerWeapon(playerid,11,1);
			}
		case 2:	{
				SellPlayerWeapon(playerid,100000,9,1);
			}
		case 3:	{
				GivePlayerWeapon(playerid,8,1);
			}
		case 4:	{
				GivePlayerWeapon(playerid,7,1);
			}
		case 5:	{
				GivePlayerWeapon(playerid,4,1);
			}
		case 6:	{
				GivePlayerWeapon(playerid,3,1);
			}
		case 7:	{
				GivePlayerWeapon(playerid,2,1);
			}
		case 8: {
				HideMenuForPlayer(melee,playerid);
				ShowMenuForPlayer(ammunation,playerid);
			}
		default: {
				print("Fail");
			}
		}
	}
	else if(Current==special) {
		ShowMenuForPlayer(special,playerid);
		switch(row) {
		case 0:	{
				SellPlayerWeapon(playerid,2000000,38,1000);
			}
		case 1:	{
				SellPlayerWeapon(playerid,250000,35,5);
			}
		case 2:	{
				SellPlayerWeapon(playerid,250000,37,300);

			}
		case 3: {
				HideMenuForPlayer(special,playerid);
				ShowMenuForPlayer(ammunation,playerid);
			}
		default: {
				print("Fail");
			}
		}
	}
	if(CurrentMenu == shopmenu)
	{

		switch(row)
		{
		case 0:
			{
				TogglePlayerControllable(playerid,false);
				HideMenuForPlayer(shopmenu,playerid);
				ShowMenuForPlayer(ammunation,playerid);

			}
		case 1:
			{
				if(GetPlayerMoney(playerid) > 10000 && PSkill[playerid] == 0)
				{
					PSkill[playerid]=1;
					GivePlayerMoneyText(playerid,-10000);
					SendClientMessage(playerid, 0xFFFFFFFF, "SERVER: Skill upgraded");
					SetPlayerSkillLevel(playerid,WEAPONSKILL_PISTOL,1000);
					SetPlayerSkillLevel(playerid,WEAPONSKILL_PISTOL_SILENCED,1000);
					SetPlayerSkillLevel(playerid,WEAPONSKILL_DESERT_EAGLE,1000);
					SetPlayerSkillLevel(playerid,WEAPONSKILL_SHOTGUN,1000);
					SetPlayerSkillLevel(playerid,WEAPONSKILL_SAWNOFF_SHOTGUN,1000);
					SetPlayerSkillLevel(playerid,WEAPONSKILL_SPAS12_SHOTGUN,1000);
					SetPlayerSkillLevel(playerid,WEAPONSKILL_MICRO_UZI,1000);
					SetPlayerSkillLevel(playerid,WEAPONSKILL_MP5,1000);
					SetPlayerSkillLevel(playerid,WEAPONSKILL_AK47,1000);
					SetPlayerSkillLevel(playerid,WEAPONSKILL_M4,1000);
					SetPlayerSkillLevel(playerid,WEAPONSKILL_SNIPERRIFLE,1000);
					ShowMenuForPlayer(shopmenu,playerid);
				}
				else if(GetPlayerMoney(playerid) < 10000)
				{
					SendClientMessage(playerid,COLOR_WHITE,"SERVER: Insufficient balance! You can not afford this item!");
					ShowMenuForPlayer(shopmenu,playerid);
					TogglePlayerControllable(playerid,false);
				}
				else if(PSkill[playerid] != 0)
				{
					SendClientMessage(playerid,COLOR_WHITE,"SERVER: You already bought this item!");
					TogglePlayerControllable(playerid,true);
				}

			}
		case 2:
			{
				if(GetPlayerMoney(playerid) >= 85000)
				{
					PStealth[playerid]=1;
					SendClientMessage(playerid,COLOR_WHITE,"SERVER: You are equipped with a stealth kit. You can use /stealth to hide yourself");
					GivePlayerMoneyText(playerid,-85000);
					TogglePlayerControllable(playerid,1);
				}
				else
				{
					SendClientMessage(playerid,COLOR_WHITE,"SERVER: Insufficient balance! You can not afford this item!");
					TogglePlayerControllable(playerid,1);
				}
				
			}
		case 3: if(GetPlayerMoney(playerid) >= 100000 && PBomb[playerid] == 0)
			{
				PBomb[playerid]=1;
				SendClientMessage(playerid, 0xFFFFFFFF, "SERVER: You have bought a bomb! Now you can place bombs by using /dropbomb!");
				ShowMenuForPlayer(shopmenu,playerid);
				TogglePlayerControllable(playerid,false);
				GivePlayerMoneyText(playerid,-100000);
			}
			else if(PBomb[playerid] != 0)
			{
				SendClientMessage(playerid,COLOR_WHITE,"SERVER: You already bought this item!");
				TogglePlayerControllable(playerid,1);
			}
			else if(GetPlayerMoney(playerid) < 100000)
			{
				SendClientMessage(playerid,COLOR_WHITE,"SERVER: Insufficient balance! You can not afford this item!");
				ShowMenuForPlayer(shopmenu,playerid);
				TogglePlayerControllable(playerid,false);
			}
		case 4: if(GetPlayerMoney(playerid) >= 50000 && Warppowder[playerid] == 0)
			
			{
				Warppowder[playerid]=1;
				SendClientMessage(playerid, 0xFFFFFFFF, "SERVER: You have bought a Warpkit! You can teleport yourself back to your base by using /warp");
				ShowMenuForPlayer(shopmenu,playerid);
				TogglePlayerControllable(playerid,false);
				GivePlayerMoneyText(playerid,-50000);
			}
			else if(Warppowder[playerid] != 0)
			{
				SendClientMessage(playerid,COLOR_WHITE,"SERVER: You already bought this item!");
				TogglePlayerControllable(playerid,1);
			}
			else if(GetPlayerMoney(playerid) < 50000)
			{
				SendClientMessage(playerid,COLOR_WHITE,"SERVER: Insufficient balance! You can not afford this item!");
				ShowMenuForPlayer(shopmenu,playerid);
				TogglePlayerControllable(playerid,false);
			}
		case 5: if(GetPlayerMoney(playerid) >= 5000 && Helmet[playerid] == 0)
			
			{
				Helmet[playerid]=1;
				SendClientMessage(playerid, 0xFFFFFFFF, "SERVER: You have bought a Helmet! You are now protected against headshots!");
				ShowMenuForPlayer(shopmenu,playerid);
				TogglePlayerControllable(playerid,false);
				GivePlayerMoneyText(playerid,-5000);
			}
			else if(Helmet[playerid] != 0)
			{
				SendClientMessage(playerid,COLOR_WHITE,"SERVER: You already bought this item!");
				TogglePlayerControllable(playerid,1);
			}
			else if(GetPlayerMoney(playerid) < 5000)
			{
				SendClientMessage(playerid,COLOR_WHITE,"SERVER: Insufficient balance! You can not afford this item!");
				ShowMenuForPlayer(shopmenu,playerid);
				TogglePlayerControllable(playerid,false);
			}
		case 6:
			{
				HideMenuForPlayer(shopmenu,playerid);
				ShowMenuForPlayer(yesno,playerid);
			}

		}
	}
}

stock GetTeamPlayerCount(teamid)
{
	new playercount = 0;
	for(new i = 0; i < MAX_PLAYERS; i++)
	{
		if(!PlayerInfo[i][pSpawned]) continue;
		if(!gPlayerHasTeamSelected[i]) continue;
		if(GetPlayerState(i) == PLAYER_STATE_NONE) continue;
		if(gPlayerTeamSelection[i] != teamid) continue;
		playercount++;
	}
	return playercount;
}
stock GetActiveTeamCount()
{
	new i=0,count=0;
	while(i<TEAMSIZE)
	{
		if(GetTeamPlayerCount(i) > 0)
		{
			count++;
		}
		i++;	
	}
	return count;
	
}


public OnPlayerTakeDamage(playerid, issuerid, Float: amount, weaponid, bodypart)
{
	if(issuerid != INVALID_PLAYER_ID && weaponid == 34 && bodypart == 9 && Helmet[playerid] != 1)
	{
		// One shot to the head to kill with sniper rifle
		SetPlayerHealth(playerid, 0.0);
	}
	return 1;
}

public GMX()
{
	printf("Total game time: %d",totaltime);
	SendRconCommand("gmx");	
}


public OnPlayerDisconnect(playerid, reason)
{
	Corrupt_Check[playerid]++;
	new pname[MAX_PLAYER_NAME], string[128 + MAX_PLAYER_NAME];
	GetPlayerName(playerid, pname, sizeof(pname));

	new DB_Query[256];
	//Running a query to save the player's data using the stored stuff.
	mysql_format(Database, DB_Query, sizeof(DB_Query), "UPDATE `PLAYERS` SET `BEDS` = %d, `BOMBS` = %d, `KILLS` = %d, `DEATHS` = %d WHERE `ID` = %d LIMIT 1",
	playerdataInfo[playerid][pBeds], playerdataInfo[playerid][pBombs], playerdataInfo[playerid][pKills], playerdataInfo[playerid][pDeaths], playerdataInfo[playerid][ID]);

	mysql_tquery(Database, DB_Query);

	if(cache_is_valid(playerdataInfo[playerid][Player_Cache])) //Checking if the player's cache ID is valid.
	{
		cache_delete(playerdataInfo[playerid][Player_Cache]); // Deleting the cache.
		playerdataInfo[playerid][Player_Cache] = MYSQL_INVALID_CACHE; // Setting the stored player Cache as invalid.
	}


	playerdataInfo[playerid][LoggedIn] = false;
	print("OnPlayerDisconnect has been called."); // Sending message once OnPlayerDisconnect is called.
	//SaveUser_data(playerid);	
	LastMoney[playerid]=0;
	
	ResetPlayerData(playerid);
	if(IsBeingSpeced[playerid] == 1)
	{
		for(new i;i<MAX_PLAYERS;i++)
		{
			if(spectatorid[i] == playerid)
			{
				SpecRandomPlayer(i);
			}
		}
	}
	TextDrawHideForPlayer(playerid,txtSpectatorHelper);
	if(!GameHasStarted)
	{
		switch(reason)
		{
		case 0: format(string, sizeof(string), "%s (%d) has left the server. (Lost Connection)", pname,playerid);
		case 1: format(string, sizeof(string), "%s (%d) has left the server. (Leaving)", pname,playerid);
		case 2: format(string, sizeof(string), "%s (%d) has left the server. (Kicked)", pname,playerid);
		}
	}
	else
	{
		switch(reason)
		{
		case 0: format(string, sizeof(string), "%s (%d) has left the server. (Lost Connection). There are only %d players left in Team %s", pname,playerid,GetTeamPlayerCount(gPlayerTeamSelection[playerid]),GetPlayerTeamColorTag(playerid));
		case 1: format(string, sizeof(string), "%s (%d) has left the server. (Leaving). There are only %d players left in Team %s", pname,playerid,GetTeamPlayerCount(gPlayerTeamSelection[playerid]),GetPlayerTeamColorTag(playerid));
		case 2: format(string, sizeof(string), "%s (%d) has left the server. (Kicked). There are only %d players left in Team %s", pname,playerid,GetTeamPlayerCount(gPlayerTeamSelection[playerid]),GetPlayerTeamColorTag(playerid));
		}
	}
	if(GetActiveTeamCount() < 2 && !GameHasFinished)
	{
		TeamRemaining();
	}
	
	SendClientMessageToAll(0xAAAAAAAA, string);
	return 1;
}

stock GetPlayerTeamColorTag(playerid)
{
	new gTeam[144];
	switch(gPlayerTeamSelection[playerid])
	{
	case FIRST_TEAM: strcpy(gTeam,FIRST_TEAM_COLOR_TAG);
	case SECOND_TEAM: strcpy(gTeam,SECOND_TEAM_COLOR_TAG);
		#if defined TEAMSIZE
		#if TEAMSIZE >= 3
	case THIRD_TEAM: strcpy(gTeam,THIRD_TEAM_COLOR_TAG);
		#endif
		#endif
		#if defined TEAMSIZE
		#if TEAMSIZE >= 4
	case FOURTH_TEAM: strcpy(gTeam,FOURTH_TEAM_COLOR_TAG);
		#endif
		#endif
		#if defined TEAMSIZE
		#if TEAMSIZE >= 5
	case FIFTH_TEAM: strcpy(gTeam,FIFTH_TEAM_COLOR_TAG);
		#endif
		#endif
		#if defined TEAMSIZE
		#if TEAMSIZE == 6
	case SIXTH_TEAM: strcpy(gTeam,SIXTH_TEAM_COLOR_TAG);
		#endif
		#endif
	case TEAM_SPECTATOR: strcpy(gTeam,SPECTATOR_TEAM_COLOR_TAG);
	}
	return gTeam;
}
stock GetTeamColorTag(teamid)
{
	new gTeam[144];
	switch(teamid)
	{
	case FIRST_TEAM: strcpy(gTeam,FIRST_TEAM_COLOR_TAG);
	case SECOND_TEAM: strcpy(gTeam,SECOND_TEAM_COLOR_TAG);
		#if defined TEAMSIZE
		#if TEAMSIZE >= 3
	case THIRD_TEAM: strcpy(gTeam,THIRD_TEAM_COLOR_TAG);
		#endif
		#endif
		#if defined TEAMSIZE
		#if TEAMSIZE >= 4
	case FOURTH_TEAM: strcpy(gTeam,FOURTH_TEAM_COLOR_TAG);
		#endif
		#endif
		#if defined TEAMSIZE
		#if TEAMSIZE >= 5
	case FIFTH_TEAM: strcpy(gTeam,FIFTH_TEAM_COLOR_TAG);
		#endif
		#endif
		#if defined TEAMSIZE
		#if TEAMSIZE == 6
	case SIXTH_TEAM: strcpy(gTeam,SIXTH_TEAM_COLOR_TAG);
		#endif
		#endif
	case TEAM_SPECTATOR: strcpy(gTeam,SPECTATOR_TEAM_COLOR_TAG);
	}
	return gTeam;
}
public OnPlayerUpdate(playerid)
{
	
	if(IsPlayerNPC(playerid)) return 1;
	if(!IsSpecing[playerid] && gPlayerHasTeamSelected[playerid] == 1) AFK_SYS[playerid] = gettime();
	if(IsSpecing[playerid] == 1)
	{
		HandlePlayerSpectating(playerid);
		return 1;
	}
	// changing teams by inputs
	if( !gPlayerHasTeamSelected[playerid] && GetPlayerState(playerid) == PLAYER_STATE_SPECTATING ) 
	{
		ClassSel_HandleTeamSelection(playerid);
		return 1;
	}
	return 1;

}
IsPlayerInRangeOfBed(playerid,Float:range)
{
	for(new i;i<TEAMSIZE;i++)
	{
		new Float:px, Float:py, Float:pz, Float:ox, Float:oy, Float:oz;
		GetObjectPos(BedArray[i],ox,oy,oz);
		GetPlayerPos(playerid,px,py,pz);
		new Float:dist = floatabs(GetDistance(px,py,pz,ox,oy,oz));
		if(dist <= range) return i;
	}
	return -1;
}
/*
IsPlayerInRangeOfObject(playerid,range,objectid)
{
	new Float:px, Float:py, Float:pz, Float:ox, Float:oy, Float:oz;
	GetPlayerPos(playerid,px,py,pz);
	GetObjectPos(objectid, ox, oy, oz);
	new Float:dist = floatabs(GetDistance(px,py,pz,ox,oy,oz));
	if(dist == range || dist < range) 
	{		
		return 1;
	}
	else
	{
		return 0;
	}
}*/
public OnPlayerClickPlayer(playerid, clickedplayerid, source)
{
	new deaths = PlayerInfo[clickedplayerid][pDeaths];
	new kills = PlayerInfo[clickedplayerid][pKills];
	if(deaths == 0)
	{
		deaths = 1;	
	}
	deaths = PlayerInfo[clickedplayerid][pDeaths];
	kills = PlayerInfo[clickedplayerid][pKills];
	new Float:kd =  floatdiv(kills,deaths);
	new name[MAX_PLAYER_NAME];
	GetPlayerName(clickedplayerid, name, sizeof(name));
	SendClientMessageEx(playerid, COLOR_WHITE,"SERVER: Player Stats of %s(%d) Team: %s ",name,clickedplayerid,GetPlayerTeamColorTag(clickedplayerid));
	SendClientMessageEx(playerid, COLOR_WHITE,"Kills:%d Deaths:%d Ratio:%0.2f Blown beds:%d Bombs detonated:%d",kills,deaths,kd,PlayerInfo[clickedplayerid][pBeds],PlayerInfo[clickedplayerid][pBombs]);
	return 1;
}															 
public OnPlayerCommandText(playerid, cmdtext[])
{
	new cmd[32], idx;
	sscanf(cmdtext,"s[32]d",cmd,idx);
	if(strcmp(cmd, "/weather", true) == 0) 
	{
		if(!IsPlayerAdmin(playerid))
		return 0;	
		new targetweatherid;
		if(sscanf(cmdtext[strlen("/weather")+1], "i", targetweatherid))
		return SendClientMessage(playerid, COLOR_WHITE, "USAGE: /weather [weatherid]");
		new wres = binarysearch2(gRandomWeatherIDs,0,targetweatherid,0,sizeof(gRandomWeatherIDs)-1);
		if(wres == -1)
		return SendClientMessage(playerid,COLOR_WHITE,"SERVER: Invalid weather ID, check weather array for valid IDs");
		new strout[64];
		format(strout, sizeof(strout), "SERVER: Weather changed to %s", gRandomWeatherIDs[targetweatherid][wt_text]);
		SetWeather(gRandomWeatherIDs[targetweatherid][wt_id]);
		SendClientMessageToAll(COLOR_WHITE,strout);
		print(strout);
	}
	if(strcmp(cmd, "/help", true) == 0) 
	{
		SendClientMessage(playerid,COLOR_WHITE,"How to play"); 
		SendClientMessage(playerid,COLOR_WHITE,"The goal of this mode, is to destroy the enemie's bed and wipe out all the remaining players to determine the winner team."); 
		SendClientMessage(playerid,COLOR_WHITE,"You can use the /blowup command to destroy and enemies bed!");
		SendClientMessage(playerid,COLOR_WHITE,"If the game has not started, you can use F4 + /kill to switch to another team. Otherwise /kill is disabled.");
		SendClientMessage(playerid,COLOR_WHITE,"Use /report to report any players or bugs. You can also report bugs by emailing to webmaster@knogleinsi.de.");
		SendClientMessage(playerid,COLOR_WHITE,"SERVER: Q: How to destroy a bed?");
		SendClientMessage(playerid,COLOR_WHITE,"SERVER: A: Get close to the enemie's bed and use /blowup to destroy it!");
		SendClientMessage(playerid,COLOR_WHITE,"SERVER: Q: What can i do if i am unable to respawn due to a destroyed bed?");
		SendClientMessage(playerid,COLOR_WHITE,"SERVER: A: You can use the command /spec [playerid] to spectate other players!");
		SendClientMessage(playerid,COLOR_WHITE,"SERVER: Q: When does the game ends?");
		SendClientMessage(playerid,COLOR_WHITE,"SERVER: A: After a winning team is determined, means if only one team remains.");
	}
	if(strcmp(cmd, "/getplayerteam", true) == 0) 
	{
		new targetplayer;
		if(sscanf(cmdtext[strlen("/getplayerteam")+1], "i", targetplayer))
		return SendClientMessage(playerid, COLOR_WHITE, "USAGE: /getplayerteam [playerid]");
		if(!IsPlayerConnected(targetplayer))
		return SendClientMessage(playerid, COLOR_WHITE, "SERVER: Player not connected");
		SendClientMessageEx(playerid,COLOR_WHITE,"SERVER: Player %d is part of Team: %s",targetplayer,GetPlayerTeamColorTag(targetplayer));
	}
	if(strcmp(cmd, "/stealth", true) == 0) 
	{
		if(PStealth[playerid] == 1)
		{
			PStealth[playerid]=-1;
			SendClientMessage(playerid,COLOR_WHITE,"SERVER: You are now invisible for 60 seconds!");
			SetTimerEx("reveal", 60000, false,"i",playerid);
		}
		else
		{
			return SendClientMessage(playerid,COLOR_WHITE,"SERVER: You do not have a stealth package!");
		}
		#if defined TEAMSIZE
		#if TEAMSIZE >= 2	

		{
			switch(gPlayerTeamSelection[playerid])
			{
			case 0:
				{
					SetPlayerColor(playerid,COLOR_TEAM_ONE_STEALTH);	
				}
			case 1:
				{
					SetPlayerColor(playerid,COLOR_TEAM_TWO_STEALTH);	
				}
			}
		}
		#endif
		#endif	
		#if defined TEAMSIZE
		#if TEAMSIZE >= 3

		{
			switch(gPlayerTeamSelection[playerid])
			{
			case 2:
				{
					SetPlayerColor(playerid,COLOR_TEAM_THREE_STEALTH);	
				}
			}
		}
		#endif
		#endif	
		#if defined TEAMSIZE
		#if TEAMSIZE >= 4

		{
			switch(gPlayerTeamSelection[playerid])
			{
			case 3:
				{
					SetPlayerColor(playerid,COLOR_TEAM_FOUR_STEALTH);	
				}
			}
		}
		#endif
		#endif	
		#if defined TEAMSIZE
		#if TEAMSIZE >= 5	

		{
			switch(gPlayerTeamSelection[playerid])
			{
			case 4:
				{
					SetPlayerColor(playerid,COLOR_TEAM_FIVE_STEALTH);	
				}
			}
		}
		#endif
		#endif
		#if defined TEAMSIZE
		#if TEAMSIZE >= 6	

		{
			switch(gPlayerTeamSelection[playerid])
			{
			case 5:
				{
					SetPlayerColor(playerid,COLOR_TEAM_SIX_STEALTH);	
				}
			}
		}
		#endif
		#endif	
		
		return 1;
	}
	if(strcmp(cmd, "/warp", true) == 0) //Warppowder
	{
		if(Warppowder[playerid] == 1 && !IsPlayerInAnyVehicle(playerid))
		{
			Warppowder[playerid]=0;
			SendClientMessage(playerid,COLOR_WHITE,"SERVER: Warning! Don't move until teleport!");
			WarpTimer = SetTimerEx("warpcount", 1000, true,"i",playerid);
			new Float:x,Float:y,Float:z;
			GetPlayerPos(playerid,x,y,z);
			wX[playerid]=x;
			wY[playerid]=y;
			wZ[playerid]=z;
			Beam[playerid] = CreateObject(18671,x,y,z-1,0,0,0,300.0);
		}
		else if(Warppowder[playerid] != 1)
		{
			return SendClientMessage(playerid,COLOR_WHITE,"SERVER: You do not have Warppowder!");
		}
		else if(IsPlayerInAnyVehicle(playerid))
		{
			return SendClientMessage(playerid,COLOR_WHITE,"SERVER: You can not use Warpkits inside of vehicles");
		}
		
		return 1;
	}
	if(strcmp(cmd, "/jetpack", true) == 0) {
		if(!IsPlayerAdmin(playerid))
		{
			return 0;
		}
		SendClientMessage(playerid,COLOR_WHITE, "SERVER: Jetpack activated!");
		SetPlayerSpecialAction(playerid,SPECIAL_ACTION_USEJETPACK);
		return 1;
	}
	if(strcmp(cmd, "/sound", true) == 0)
	{
		new targetsoundid;
		if(sscanf(cmdtext[strlen("/sound")+1], "i", targetsoundid))
		return SendClientMessage(playerid, COLOR_WHITE, "USAGE: /sound [soundid]");
		
		for(new i = 0; i < MAX_PLAYERS; i++)
		{
			PlayerPlaySound(i, targetsoundid, 0,0,0);
		}
		printf("Playing SoundID: %i",targetsoundid);
		return 1;


	}


	if(strcmp(cmd, "/spec", true) == 0)
	{
		if(!IsPlayerAdmin(playerid)) return 0;
		new targetplayer;
		if(sscanf(cmdtext[strlen("/spec")+1], "u", targetplayer))
		return SendClientMessage(playerid, COLOR_WHITE, "USAGE: /spec [playerid]");
		if(!IsPlayerConnected(targetplayer))
		return SendClientMessage(playerid, COLOR_WHITE, "SERVER: Player not connected");
		if(playerid == targetplayer)
		return SendClientMessage(playerid, COLOR_WHITE, "SERVER: You cannot spectate yourself!");
		if(gPlayerTeamSelection[playerid] == TEAM_SPECTATOR	|| IsPlayerAdmin(playerid) )
		{

			SpecPlayer(playerid,targetplayer);
		}
		else
		{
			return 0;
		}
		return 1;
	}
	if(strcmp(cmd, "/specoff", true) == 0)
	{
		if(!IsPlayerAdmin(playerid)) return 0;
		if(IsSpecing[playerid] == 0)return SendClientMessage(playerid,COLOR_WHITE,"SERVER: You are not spectating");
		TogglePlayerSpectating(playerid, 0);
		return 1;
	}
	// Prototyp
	if(strcmp(cmd, "/dropmoney", true) == 0)
	{
		new dropval;
		if(sscanf(cmdtext[strlen("/dropmoney")+1], "i", dropval))
		return SendClientMessage(playerid, COLOR_WHITE, "USAGE: /dropmoney [value]");
		if(GetPlayerMoney(playerid) < dropval)
		return SendClientMessage(playerid, COLOR_WHITE, "SERVER: You do not have enough money.");
		if(maxmoney >= 4000 && dropval > 0 && dropval <= 100000 && (maxmoney+(dropval/MoneyVal))<(MAX_PICKUPS-sizeof(ActorPickups)-sizeof(InfoPickups)-1-((dropval/MoneyVal)+maxmoney)) && dropval != 0 && !IsPlayerInAnyVehicle(playerid))
		{
			if(dropval%MoneyVal == 0)
			{
				new dropstring[32];
				format(dropstring,sizeof(dropstring),"SERVER: Dropped $%d",dropval);
				SendClientMessage(playerid, COLOR_WHITE, dropstring);
				GivePlayerMoneyText(playerid,-dropval);

				for(new i; i <dropval/MoneyVal; i++)
				{

					maxmoney = maxmoney +1;
					new Float:x, Float:y, Float:z;
					GetPlayerPos(playerid,x,y,z);
					GetXYInFrontOfPlayer(playerid,x,y,4);
					GenerateRandomPickup(1212,19,x+0.5,x-0.5,y+0.5,y-0.5,z+0.5,z-0.5,0);
				}

			}
			else
			{

				if(dropval%MoneyVal != 0)
				{
					SendClientMessageEx(playerid, COLOR_WHITE, "SERVER: The desired value must be divisible by $%d",MoneyVal);
				}

			}

		}
		else
		{
			if(GetPlayerMoney(playerid) < dropval )
			{
				SendClientMessage(playerid, COLOR_WHITE, "SERVER: You do not have enough money.");
			}
			if(dropval > 100000)
			{

				SendClientMessage(playerid, COLOR_WHITE, "SERVER: The desired value has to be $100000 or less");
			}
			if(dropval==0)
			{

				SendClientMessage(playerid, COLOR_WHITE, "SERVER: You can not drop nothing!");
			}
			if((maxmoney+dropval/MoneyVal)>=4095 && dropval <= 100000)
			{
				new allowmoney[128];
				format(allowmoney,sizeof(allowmoney),"SERVER: Because of limitations you can not drop more than $%d right now!",(4095-maxmoney)*MoneyVal);
				SendClientMessage(playerid,COLOR_WHITE,allowmoney);
				printf("Player may only drop %d",(4095-maxmoney)*MoneyVal);
			}
			if(maxmoney > 4000)
			{
				SendClientMessage(playerid,COLOR_WHITE,"SERVER: Due to limitations you can not drop anything right now. Try again later.");
			}
		}

	}
	if(strcmp(cmd, "/freeze", true) == 0)
	{
		new target;	
		if(!sscanf(cmdtext[strlen("/freeze")+1], "u", target))	
		{
			if(IsPlayerAdmin(playerid))
			{
				if(!IsPlayerConnected(target))
				return SendClientMessage(playerid, COLOR_WHITE, "SERVER: Player not connected");
				TogglePlayerControllable(target,false);
			}	
		}
		else
		{
			return SendClientMessage(playerid, COLOR_WHITE, "USAGE: /freeze [playerid]");
			
		}
	}
	if(strcmp(cmd, "/kill", true) == 0)
	{
		if(IsPlayerAdmin(playerid))
		{
			new targetplayer;
			if(sscanf(cmdtext[strlen("/kill")+1], "u", targetplayer))
			return SendClientMessage(playerid, COLOR_WHITE, "USAGE: /kill [playerid]");
			if(!IsPlayerConnected(targetplayer))
			return SendClientMessage(playerid, COLOR_WHITE, "SERVER: Player not connected");
			if(gPlayerTeamSelection[targetplayer] == TEAM_SPECTATOR)
			return SendClientMessageEx(playerid, COLOR_WHITE,"SERVER: Cannot kill Player %d, player %d is part of Team %s",targetplayer,targetplayer,GetPlayerTeamColorTag(targetplayer));
			SetPlayerHealth(targetplayer,0.0);
		}	
		else
		{
			if(!GameHasStarted) SetPlayerHealth(playerid,0);
		}
	}
	if(strcmp(cmd, "/givecash", true) == 0)
	{
		if(IsPlayerAdmin(playerid))
		{
			new targetplayer,moneyamount;
			new tname[MAX_PLAYER_NAME];
			new aname[MAX_PLAYER_NAME];
			new givemoneystring[128];
			if(sscanf(cmdtext[strlen("/givecash")+1], "ui", targetplayer,moneyamount))
			return SendClientMessage(playerid, COLOR_WHITE, "USAGE: /givecash [playerid] [amount]");
			if(!IsPlayerConnected(targetplayer))
			return SendClientMessage(playerid, COLOR_WHITE, "SERVER: Player not connected");
			GivePlayerMoneyText(targetplayer,moneyamount);
			GetPlayerName(targetplayer,tname,sizeof(tname));
			GetPlayerName(playerid,aname,sizeof(aname));
			format(givemoneystring,sizeof(givemoneystring),"SERVER: Admin %s (%d) gave %s (%d) $%d.",aname,playerid,tname,targetplayer,moneyamount);
			SendClientMessageToAll(COLOR_WHITE,givemoneystring);
		}	
	}
	if(strcmp(cmd, "/slap", true) == 0)
	{
		if(IsPlayerAdmin(playerid))
		{
			new targetplayer;
			new Float:a, Float:b, Float:c;
			if(sscanf(cmdtext[strlen("/slap")+1], "u", targetplayer))
			return SendClientMessage(playerid, COLOR_WHITE, "USAGE: /slap [playerid]");
			if(!IsPlayerConnected(targetplayer))
			return SendClientMessage(playerid, COLOR_WHITE, "SERVER: Player not connected");
			GetPlayerPos(targetplayer,a,b,c);
			SetPlayerPos(targetplayer,a,b,c+10);
		}	
	}
	if(strcmp(cmd, "/reset", true) == 0)
	{
		if(IsPlayerAdmin(playerid))
		{
			new targetplayer;
			if(sscanf(cmdtext[strlen("/reset")+1], "u", targetplayer))
			return SendClientMessage(playerid, COLOR_WHITE, "USAGE: /reset [playerid]");
			if(!IsPlayerConnected(targetplayer))
			return SendClientMessage(playerid, COLOR_WHITE, "SERVER: Player not connected");
			if(gPlayerHasTeamSelected[targetplayer] == -1)
			return SendClientMessage(playerid, COLOR_WHITE, "SERVER: Player is still in team selection!");
			ResetPlayerData(targetplayer);
			ResetPlayerMoney(targetplayer);
			TeleportPlayerToBase(targetplayer);
		}	
	}
	if(strcmp(cmd, "/checkmoney", true) == 0)
	{
		if(IsPlayerAdmin(playerid))
		{
			new targetplayer;
			new tname[144];
			if(sscanf(cmdtext[strlen("/checkmoney")+1], "u", targetplayer))
			return SendClientMessage(playerid, COLOR_WHITE, "USAGE: /checkmoney [playerid]");
			if(!IsPlayerConnected(targetplayer))
			return SendClientMessage(playerid, COLOR_WHITE, "SERVER: Player not connected");
			if(gPlayerHasTeamSelected[targetplayer] == -1)
			return SendClientMessage(playerid, COLOR_WHITE, "SERVER: Player is still in team selection!");
			GetPlayerName(targetplayer,tname,sizeof(tname));
			SendClientMessageEx(playerid,COLOR_WHITE,"SERVER: Player %s(%d) is in posession of $%d at this moment",tname,targetplayer,GetPlayerMoney(targetplayer));
		}	
	}
	if(strcmp(cmd, "/spawn", true) == 0)
	{
		if(IsPlayerAdmin(playerid))
		{
			new targetplayer;
			if(sscanf(cmdtext[strlen("/spawn")+1], "u", targetplayer))
			return SendClientMessage(playerid, COLOR_WHITE, "USAGE: /spawn [playerid]");
			if(!IsPlayerConnected(targetplayer))
			return SendClientMessage(playerid, COLOR_WHITE, "SERVER: Player not connected");
			if(gPlayerHasTeamSelected[targetplayer] == -1)
			return SendClientMessage(playerid, COLOR_WHITE, "SERVER: Player is still in team selection!");
			TeleportPlayerToBase(targetplayer);
		}	
	}
	if(strcmp(cmd, "/unfreeze", true) == 0)
	{
		if(IsPlayerAdmin(playerid))
		{
			new targetplayer;
			if(sscanf(cmdtext[strlen("/unfreeze")+1], "u", targetplayer))
			return SendClientMessage(playerid, COLOR_WHITE, "USAGE: /unfreeze [playerid]");
			if(!IsPlayerConnected(targetplayer))
			return SendClientMessage(playerid, COLOR_WHITE, "SERVER: Player not connected");
			TogglePlayerControllable(targetplayer,true);
		}	
	}
	if(strcmp(cmd, "/goto", true) == 0)
	{
		if(IsPlayerAdmin(playerid))
		{
			new targetplayer;
			if(sscanf(cmdtext[strlen("/goto")+1], "u", targetplayer))
			return SendClientMessage(playerid, COLOR_WHITE, "USAGE: /goto [playerid]");
			if(!IsPlayerConnected(targetplayer))
			return SendClientMessage(playerid, COLOR_WHITE, "SERVER: Player not connected");
			if(playerid == targetplayer)
			return SendClientMessage(playerid, COLOR_WHITE, "SERVER: You cannot goto yourself!");
			if(IsPlayerConnected(targetplayer))
			{
				new Float:x, Float:y, Float:z;
				GetPlayerPos(targetplayer, x, y, z);
				SetPlayerInterior(playerid,GetPlayerInterior(targetplayer));
				SetPlayerPos(playerid, x+1, y+1, z);
				return 1;
			}
		}
		else
		{
			return 0;
		}
	}
	if(strcmp(cmd, "/acmds", true) == 0)
	{
		SendClientMessage(playerid,COLOR_WHITE,"SERVER: Commands for admin use only below.");	
		SendClientMessage(playerid,COLOR_WHITE,"SERVER: /goto [playerid] /playertoplayer [playerid] [targetplayerid]");	
		SendClientMessage(playerid,COLOR_WHITE,"SERVER: /getmoneycount /get [playerid] /netstats [playerid]");
		SendClientMessage(playerid,COLOR_WHITE,"SERVER: /kill [playerid] /spawn [playerid] /slap [playerid]");
		SendClientMessage(playerid,COLOR_WHITE,"SERVER: /givecash [playerid] [value] /freeze [playerid] /unfreeze [playerid]");
		SendClientMessage(playerid,COLOR_WHITE,"SERVER: /sound [soundid] /spec [playerid]");		
	}	
	if(strcmp(cmd, "/playertoplayer", true) == 0)
	{
		if(IsPlayerAdmin(playerid))
		{
			new targetplayer1,targetplayer2;
			if(sscanf(cmdtext[strlen("/playertoplayer")+1], "uu", targetplayer1,targetplayer2))
			return SendClientMessage(playerid, COLOR_WHITE, "USAGE: /playertoplayer [playerid] [playerid]");
			if(!IsPlayerConnected(targetplayer1) || !IsPlayerConnected(targetplayer2))
			return SendClientMessage(playerid, COLOR_WHITE, "SERVER: Player not connected");
			if(playerid == targetplayer1 && playerid == targetplayer2 )
			return SendClientMessage(playerid, COLOR_WHITE, "SERVER: You cannot teleport yourself to yourself!");
			new Float:d1,Float:d2,Float:d3;
			GetPlayerPos(targetplayer2,d1,d2,d3);
			SetPlayerPos(targetplayer1,d1,d2,d3);
			
		}
		else
		{
			return 0;
		}
	}
	if(strcmp(cmd, "/getmoneycount", true) == 0)
	{
		new moneyvalstring[128];
		format(moneyvalstring,sizeof(moneyvalstring),"SERVER: $%d have been generated.%d money pickups have been created",moneyval,maxmoney);
		SendClientMessage(playerid,COLOR_WHITE,moneyvalstring);
	}
	if(strcmp(cmd, "/get", true) == 0)
	{

		if(IsPlayerAdmin(playerid))
		{
			new targetplayer;
			if(sscanf(cmdtext[strlen("/get")+1], "u", targetplayer))
			return SendClientMessage(playerid, COLOR_WHITE, "USAGE: /get [playerid]");
			if(!IsPlayerConnected(targetplayer))
			return SendClientMessage(playerid, COLOR_WHITE, "SERVER: Player not connected");
			if(playerid == targetplayer)
			return SendClientMessage(playerid, COLOR_WHITE, "SERVER: You cannot get yourself!");
			if(IsPlayerConnected(targetplayer))
			{
				new Float:x, Float:y, Float:z;
				GetPlayerPos(playerid, x, y, z);
				SetPlayerInterior(targetplayer,GetPlayerInterior(playerid));
				SetPlayerPos(targetplayer, x+1, y+1, z);
				return 1;
			}
		}
		else
		{
			return 0;
		}
	}
	if(strcmp(cmd, "/dropbomb", true) == 0)
	{
		if(PBomb[playerid] == 1 && GetPlayerInterior(playerid) == 0 && !IsPlayerInAnyVehicle(playerid))
		{
			ApplyAnimation(playerid, "BOMBER", "BOM_Plant_Loop", 4.0, 1, 0, 0, 1, 1);
			PBombID[playerid]=1;
			PBomb[playerid]=-1;
			GetPlayerPos(playerid,pX[playerid],pY[playerid],pZ[playerid]);
			BombObject[playerid] = CreateObject(363,pX[playerid],pY[playerid],(pZ[playerid]-0.4),0,0,0);//Bomb object
			SendClientMessage(playerid, COLOR_WHITE, "SERVER: Bomb placed! Use ~k~~CONVERSATION_YES~ or /detonate to blow it up!");
		}
		else if(GetPlayerInterior(playerid) != 0)
		{
			SendClientMessage(playerid, COLOR_WHITE, "SERVER: You cannot use /dropbomb inside of interiors.");
		}
		else if(PBomb[playerid] != 1)
		{
			SendClientMessage(playerid, COLOR_WHITE, "SERVER: You have to buy a bomb to use this command.");
		}
	}
	if(strcmp(cmd, "/detonate", true) == 0)
	{
		if(PBomb[playerid] == -1 && PBombID[playerid] == 1 && IsPlayerInRangeOfPoint(playerid,150,pX[playerid],pY[playerid],pZ[playerid]))
		{
			PlayerInfo[playerid][pBombs]++;
			PBombID[playerid]=0;
			PBomb[playerid]=0;
			CreateExplosion(pX[playerid],pY[playerid],pZ[playerid],7,10);
			CreateExplosion(pX[playerid],pY[playerid],pZ[playerid],9,10);
			SendClientMessage(playerid, COLOR_WHITE, "SERVER: Bomb detonated!");
			PlayerPlaySound(playerid, 21001, 0, 0, 0);
			DestroyObject(BombObject[playerid]);
			new Float:health;
			GetPlayerHealth(playerid,health);
			new bedteamid=IsPlayerInRangeOfBed(playerid,3.0);
			if(running !=1 && bedteamid != 0 && health > 0 && !IsPlayerInAnyVehicle(playerid)) // In this script we deal with players only
			{
				if(bedteamid == gPlayerTeamSelection[playerid])
				return SendClientMessage(playerid, COLOR_WHITE, "SERVER: You cannot blow up the bed of your own team!");  
				running = 1;
				PlayerInfo[playerid][pBeds]=GetPlayerScore(playerid);
				if(BedStates[bedteamid] != 0)
				{
					SendClientMessageEx(playerid, COLOR_WHITE, "SERVER: You are blowing up the bed of Team %s ",GetTeamColorTag(bedteamid));
					BedStates[bedteamid] = 0;
				}
				PlayerInfo[playerid][pBeds]+=1;	
				SendClientMessage(playerid, COLOR_WHITE, "SERVER: Planted Bomb Successfully. Lets Blow This Bed Up!");
				SetPlayerScore(playerid, GetPlayerScore(playerid) + 1);
				SendClientMessage(playerid, COLOR_WHITE, "SERVER: Keep Running! Bed Will Blow Up In 5 seconds!");
				GetPlayerPos(playerid, x1, y1, z1);
				ApplyAnimation(playerid, "BOMBER", "BOM_Plant_Loop", 4.0, 1, 0, 0, 1, 1);
				for(new k = 0; k < MAX_PLAYERS; k++)
				{
					PlayerPlaySound(k,7416,x1,y1,z1);
				}
				CountDownTimer = SetTimer("CountDown", 1000, true);
				return 1;

			}
		}
	}
	if(strcmp(cmd, "/netstats", true) == 0)
	{
		new stats[400+1];
		new targetplayer;
		if(sscanf(cmdtext[strlen("/netstats")+1], "u", targetplayer))
		return SendClientMessage(playerid, COLOR_WHITE, "USAGE: /netstats [playerid]");
		if(!IsPlayerConnected(targetplayer))
		return SendClientMessage(playerid, COLOR_WHITE, "SERVER: Player not connected");
		
		GetPlayerNetworkStats(targetplayer, stats, sizeof(stats)); // get a players networkstats
		ShowPlayerDialog(playerid, 0, DIALOG_STYLE_MSGBOX, "Networkstats", stats, "Okay", "");
	}
	if(strcmp(cmd, "/stats", true) == 0)
	{
		new targetplayer;
		if(sscanf(cmdtext[strlen("/stats")+1], "u", targetplayer))
		return SendClientMessage(playerid, COLOR_WHITE, "USAGE: /stats [playerid]");
		if(!IsPlayerConnected(targetplayer))
		return SendClientMessage(playerid, COLOR_WHITE, "SERVER: Player not connected");
		new statsstring1[128];
		new statsstring2[256];
		new deaths = PlayerInfo[targetplayer][pDeaths];
		new kills = PlayerInfo[targetplayer][pKills];
		if(deaths == 0)
		{
			deaths = 1;	
		}
		deaths = PlayerInfo[targetplayer][pDeaths];
		kills = PlayerInfo[targetplayer][pKills];
		new Float:kd =  floatdiv(kills,deaths);
		new name[MAX_PLAYER_NAME];
		GetPlayerName(targetplayer, name, sizeof(name));
		format(statsstring1,sizeof(statsstring1),"SERVER: Player stats of %s(%d) below.",name,targetplayer); 
		SendClientMessage(playerid, COLOR_WHITE,statsstring1);
		format(statsstring2,sizeof(statsstring2),"Kills:%d Deaths:%d Ratio:%0.2f Blown beds:%d Bombs detonated:%d",kills,deaths,kd,PlayerInfo[targetplayer][pBeds],PlayerInfo[targetplayer][pBombs]); 
		SendClientMessage(playerid, COLOR_WHITE,statsstring2);
	}
	if(strcmp(cmd, "/pm", true) == 0)
	{
		new str[256], str2[256], id, Name1[MAX_PLAYER_NAME], Name2[MAX_PLAYER_NAME];
		if(sscanf(cmdtext[strlen("/pm")+1], "us", id, str2))
		return SendClientMessage(playerid, COLOR_WHITE, "USAGE: /pm <id> <message>");
		if(!IsPlayerConnected(id))
		return SendClientMessage(playerid, COLOR_WHITE, "SERVER: Player not connected");
		if(playerid == id)
		return SendClientMessage(playerid, COLOR_WHITE, "SERVER: You cannot pm yourself!");

		GetPlayerName(playerid, Name1, sizeof(Name1));
		GetPlayerName(id, Name2, sizeof(Name2));
		format(str, sizeof(str), "PM To %s(ID %d): %s", Name2, id, str2);
		SendClientMessage(playerid, COLOR_YELLOW, str);
		format(str, sizeof(str), "PM From %s(ID %d): %s", Name1, playerid, str2);
		SendClientMessage(id, COLOR_YELLOW, str);
		return 1;
	}
	if(strcmp(cmd, "/r", true) == 0)
	{
		new str[256], str2[256], Name1[MAX_PLAYER_NAME];
		if(sscanf(cmdtext[strlen("/r")+1], "s", str2))
		return SendClientMessage(playerid, COLOR_WHITE, "USAGE: /r <message>");
		GetPlayerName(playerid, Name1, sizeof(Name1));
		format(str, sizeof(str), "[%s{FFFFFF}]%s(%d): %s",GetPlayerTeamColorTag(playerid), Name1, playerid, str2);
		SendTeamMessage(gPlayerTeamSelection[playerid],COLOR_WHITE,str);
		return 1;
	}
	if(strcmp(cmd, "/report", true) == 0)
	{
		new str[256], str2[256], Name1[MAX_PLAYER_NAME];
		if(sscanf(cmdtext[strlen("/report")+1], "s", str2))
		return SendClientMessage(playerid, COLOR_WHITE, "USAGE: /report <message>");
		GetPlayerName(playerid, Name1, sizeof(Name1));
		format(str, sizeof(str), "{A9C4E4}SERVER: Incoming report from: %s(%d) regarding: %s", Name1, playerid, str2);
		SendTeamMessage(gPlayerTeamSelection[playerid],COLOR_WHITE,str);
		printf(str);
		return 1;
	}
	if(strcmp(cmdtext, "/blowup", true) == 0 && GameHasStarted == 1)
	{
		
		new Float:health;
		GetPlayerHealth(playerid,health);
		new bedteamid=IsPlayerInRangeOfBed(playerid,3.0);
		if(running !=1 && bedteamid != -1 && health > 0 && !IsPlayerInAnyVehicle(playerid)) // In this script we deal with players only
		{
			if(bedteamid == gPlayerTeamSelection[playerid])
			return SendClientMessage(playerid, COLOR_WHITE, "SERVER: You cannot blow up the bed of your own team!");  
			running = 1;
			PlayerInfo[playerid][pBeds]=GetPlayerScore(playerid);
			SetTimerEx("BlowUpThisBed", bombtimer, false,"i",bedteamid);
			if(BedStates[bedteamid] != 0)
			{
				SendClientMessageEx(playerid, COLOR_WHITE, "SERVER: You are blowing up the bed of Team %s ",GetTeamColorTag(bedteamid));
				BedStates[bedteamid] = 0;
			}
			PlayerInfo[playerid][pBeds]+=1;	
			SendClientMessage(playerid, COLOR_WHITE, "SERVER: Planted Bomb Successfully. Lets Blow This Bed Up!");
			SetPlayerScore(playerid, GetPlayerScore(playerid) + 1);
			SendClientMessage(playerid, COLOR_WHITE, "SERVER: Keep Running! Bed Will Blow Up In 5 seconds!");
			GetPlayerPos(playerid, x1, y1, z1);
			ApplyAnimation(playerid, "BOMBER", "BOM_Plant_Loop", 4.0, 1, 0, 0, 1, 1);
			for(new k = 0; k < MAX_PLAYERS; k++)
			{
				PlayerPlaySound(k,7416,x1,y1,z1);
			}
			CountDownTimer = SetTimer("CountDown", 1000, true);
			return 1;

		}



		
		return 0;

	}
	return 1;

}
public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if(PRESSED(KEY_YES) && PBomb[playerid] == -1 && PBombID[playerid] == 1 && IsPlayerInRangeOfPoint(playerid,150,pX[playerid],pY[playerid],pZ[playerid]))
	{
		PlayerInfo[playerid][pBombs]++;
		PBombID[playerid]=0;
		PBomb[playerid]=0;
		CreateExplosion(pX[playerid],pY[playerid],pZ[playerid],7,10);
		CreateExplosion(pX[playerid],pY[playerid],pZ[playerid],9,10);
		SendClientMessage(playerid, COLOR_WHITE, "SERVER: Bomb detonated!");
		PlayerPlaySound(playerid, 21001, 0, 0, 0);
		DestroyObject(BombObject[playerid]);
		new Float:health;
		GetPlayerHealth(playerid,health);
		new bedteamid=IsPlayerInRangeOfBed(playerid,3.0);
		if(running !=1 && bedteamid != -1 && health > 0 && !IsPlayerInAnyVehicle(playerid)) // In this script we deal with players only
		{
			if(bedteamid == gPlayerTeamSelection[playerid])
			return SendClientMessage(playerid, COLOR_WHITE, "SERVER: You cannot blow up the bed of your own team!");  
			running = 1;
			PlayerInfo[playerid][pBeds]=GetPlayerScore(playerid);
			if(BedStates[bedteamid] != 0)
			{
				SendClientMessageEx(playerid, COLOR_WHITE, "SERVER: You are blowing up the bed of Team %s ",GetTeamColorTag(bedteamid));
				BedStates[bedteamid] = 0;
			}
			PlayerInfo[playerid][pBeds]+=1;	
			SendClientMessage(playerid, COLOR_WHITE, "SERVER: Planted Bomb Successfully. Lets Blow This Bed Up!");
			SetPlayerScore(playerid, GetPlayerScore(playerid) + 1);
			SendClientMessage(playerid, COLOR_WHITE, "SERVER: Keep Running! Bed Will Blow Up In 5 seconds!");
			GetPlayerPos(playerid, x1, y1, z1);
			ApplyAnimation(playerid, "BOMBER", "BOM_Plant_Loop", 4.0, 1, 0, 0, 1, 1);
			for(new k = 0; k < MAX_PLAYERS; k++)
			{
				PlayerPlaySound(k,7416,x1,y1,z1);
			}
			CountDownTimer = SetTimer("CountDown", 1000, true);
			return 1;

		}
	}
	return 1;
}
public OnPlayerDeath(playerid, killerid, reason)
{
	PlayerInfo[playerid][pDeaths]++;
	if(killerid != INVALID_PLAYER_ID) PlayerInfo[killerid][pKills]++;
	ResetPlayerData(playerid);
	
	new pname[MAX_PLAYER_NAME];
	GetPlayerName(playerid, pname, sizeof(pname));
	new matediedstring[128];

	
	HideMenuForPlayer(ammunation,playerid);
	HideMenuForPlayer(pistols,playerid);
	HideMenuForPlayer(microsmg,playerid);
	HideMenuForPlayer(shotguns,playerid);
	HideMenuForPlayer(items,playerid);
	HideMenuForPlayer(smg,playerid);
	HideMenuForPlayer(rifles,playerid);
	HideMenuForPlayer(assaultrifle,playerid);
	HideMenuForPlayer(Grenades,playerid);
	HideMenuForPlayer(melee,playerid);
	HideMenuForPlayer(special,playerid);
	HideMenuForPlayer(shopmenu,playerid);
	HideMenuForPlayer(yesno,playerid);
	

	if(GetPlayerWeapon(playerid) == 16)
	{
		new Float:x, Float:y, Float:z;
		GetPlayerPos(playerid,x,y,z);
		CreateExplosion(x,y,z,7,10);
		CreateExplosion(x,y,z,9,10);
	}

	if(BedStates[gPlayerTeamSelection[playerid]] == 0)
	{
		format(matediedstring,sizeof(matediedstring),"SERVER: %s (%d) has been killed, watch out! %d players left in Team %s",pname,playerid,GetTeamPlayerCount(gPlayerTeamSelection[playerid])-1,GetPlayerTeamColorTag(playerid));
		SendClientMessageToAll(COLOR_WHITE,matediedstring);
	}
	new playercash;
	playercash = GetPlayerMoney(playerid);
	SendDeathMessage(killerid, playerid, reason); // Shows the kill in the killfeed
	gPlayerHasTeamSelected[playerid] = 0;
	TextDrawHideForPlayer(playerid,txtTimeDisp);
	playercash = GetPlayerMoney(playerid);
	ResetPlayerMoney(playerid);
	GivePlayerMoneyText(playerid, -playercash);
	LastMoney[playerid]=playercash;
	
	#if defined BEDSSYSTEM
	#if BEDSSYSTEM==0
	if(killerid != playerid && killerid != INVALID_PLAYER_ID)
	{
		GivePlayerMoneyText(killerid,playercash);
	}
	#elseif BEDSSYSTEM==1
	
	if(killerid != playerid && killerid != INVALID_PLAYER_ID)
	{
		if(GetPlayerScore(killerid) >= 100)
		GivePlayerMoneyText(killerid,playercash+playercash/4);
		if(GetPlayerScore(killerid) >= 500)
		GivePlayerMoneyText(killerid,playercash+playercash/2);
		if(GetPlayerScore(killerid) >= 1000)
		GivePlayerMoneyText(killerid,playercash * 2);
	}
	#endif
	#else
	if(killerid != playerid && killerid != INVALID_PLAYER_ID)
	{
		GivePlayerMoneyText(killerid,playercash);
	}	
	#endif
	

	if(BedStates[gPlayerTeamSelection[playerid]] == 0)
	{
		SendClientMessageEx(playerid,COLOR_WHITE,"SERVER: Unable to respawn. The bed of team %s {FFFFFF}already has been destroyed.",GetTeamColorTag(playerid));
		SendClientMessage(playerid,COLOR_WHITE,"SERVER: Entering spectator mode..");
		SetPlayerCameraPos(playerid,243.2876,1802.5547,7.4141);
		SetPlayerCameraLookAt(playerid,243.1261,1805.2798,8.3794);
		gPlayerTeamSelection[playerid] =TEAM_SPECTATOR;
		SetPlayerColor(playerid,COLOR_WHITE);
		SpecRandomPlayer(playerid);
	}
	if(GetActiveTeamCount() < 2 && GameHasStarted)
	{
		SetTimer("TeamRemaining",1000,false);
		printf("Timer set");
	}
	if(IsBeingSpeced[playerid] == 1)
	{
		for(new i;i<MAX_PLAYERS;i++)
		{
			if(spectatorid[i] == playerid)
			{
				SpecRandomPlayer(i);
			}
		}
	}

	return 1;
}

ClassSel_SetupSelectedTeam(playerid)
{
	PlayerTextDrawDestroy(playerid, TeamText[playerid]);
	PlayerTextDrawDestroy(playerid, TeamCover[playerid]);
	PlayerTextDrawDestroy(playerid, LocText[playerid]);
	TeamText[playerid] = CreatePlayerTextDraw(playerid, 65.000000,349.000000,"Team:"); 
	ClassSel_InitTeamNameText(playerid,TeamText[playerid]);
	TeamCover[playerid] = CreatePlayerTextDraw(playerid, 10.000000,349.000000,"Team:"); 
	ClassSel_InitTeamNameText(playerid,TeamCover[playerid]);
	LocText[playerid] = CreatePlayerTextDraw(playerid, 10.000000,379.000000,"Location:"); 
	ClassSel_InitTeamNameText(playerid,LocText[playerid]);
	PlayerTextDrawShow(playerid,TeamCover[playerid]);
	PlayerPlaySound(playerid,1185,0,0,0);
	SetPlayerColor(playerid,COLOR_WHITE);
	gettime(hour, minute);
	SetPlayerTime(playerid,hour,minute);
	if(GameHasStarted == 1)
	{
		gPlayerTeamSelection[playerid] = TEAM_SPECTATOR;
		SpecRandomPlayer(playerid);
		new Float:time = totaltime / 60;
		SendClientMessageEx(playerid,COLOR_WHITE,"SERVER: Game has started %d minute(s) ago so you are spectating now.",time);
	}
	if(gPlayerTeamSelection[playerid] == -1) {
		gPlayerTeamSelection[playerid] = FIRST_TEAM;
	}

	if(gPlayerTeamSelection[playerid] == FIRST_TEAM) {
		SetPlayerInterior(playerid,TEAM_ONE_SEL_INTERIOR);
		SetPlayerCameraPos(playerid,ClassSel_SetupTeamTEAM_POS[0][0],ClassSel_SetupTeamTEAM_POS[0][1],ClassSel_SetupTeamTEAM_POS[0][2]);
		SetPlayerCameraLookAt(playerid,ClassSel_SetupTeamTEAM_LOOK_AT[0][0],ClassSel_SetupTeamTEAM_LOOK_AT[0][1],ClassSel_SetupTeamTEAM_LOOK_AT[0][2]);
		new astring[64];
		PlayerTextDrawHide(playerid,TeamText[playerid]);
		PlayerTextDrawHide(playerid,LocText[playerid]);
		new FIRST_TEAM_TEXTDRAW_STRING[64];
		format(FIRST_TEAM_TEXTDRAW_STRING,sizeof(FIRST_TEAM_TEXTDRAW_STRING),"%s (%d Players)",FIRST_TEAM_NAME_TAG,GetTeamPlayerCount(FIRST_TEAM));
		PlayerTextDrawSetString(playerid, TeamText[playerid],FIRST_TEAM_TEXTDRAW_STRING);
		PlayerTextDrawColor(playerid,TeamText[playerid],COLOR_TEAM_ONE_TD);
		PlayerTextDrawShow(playerid,TeamText[playerid]);
		format(astring,sizeof(astring),"Location:"#FIRST_TEAM_LOCATION);
		PlayerTextDrawSetString(playerid, LocText[playerid], astring);
		PlayerTextDrawShow(playerid,LocText[playerid]);

		SetPlayerWeather(playerid,15);
		PlayerPlaySound(playerid,2403,0,0,0);

	}
	#if defined TEAMSIZE
	#if TEAMSIZE >= 2
	else if(gPlayerTeamSelection[playerid] == SECOND_TEAM) {
		SetPlayerInterior(playerid,TEAM_TWO_SEL_INTERIOR);
		SetPlayerWeather(playerid,15);
		SetPlayerCameraPos(playerid,ClassSel_SetupTeamTEAM_POS[1][0],ClassSel_SetupTeamTEAM_POS[1][1],ClassSel_SetupTeamTEAM_POS[1][2]);
		SetPlayerCameraLookAt(playerid,ClassSel_SetupTeamTEAM_LOOK_AT[1][0],ClassSel_SetupTeamTEAM_LOOK_AT[1][1],ClassSel_SetupTeamTEAM_LOOK_AT[1][2]);
		new astring[64];
		PlayerTextDrawHide(playerid,TeamText[playerid]);
		PlayerTextDrawHide(playerid,LocText[playerid]);
		new SECOND_TEAM_TEXTDRAW_STRING[64];
		format(SECOND_TEAM_TEXTDRAW_STRING,sizeof(SECOND_TEAM_TEXTDRAW_STRING),"%s (%d Players)",SECOND_TEAM_NAME_TAG,GetTeamPlayerCount(SECOND_TEAM));
		PlayerTextDrawSetString(playerid, TeamText[playerid],SECOND_TEAM_TEXTDRAW_STRING);
		PlayerTextDrawColor(playerid,TeamText[playerid],COLOR_TEAM_TWO_TD);
		PlayerTextDrawShow(playerid,TeamText[playerid]);
		format(astring,sizeof(astring),"Location:"#SECOND_TEAM_LOCATION);
		PlayerTextDrawSetString(playerid, LocText[playerid], astring);
		PlayerTextDrawShow(playerid,LocText[playerid]);
		PlayerPlaySound(playerid,2402,0,0,0);

	}
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE >= 3
	else if(gPlayerTeamSelection[playerid] == THIRD_TEAM) {
		SetPlayerInterior(playerid,TEAM_THREE_SEL_INTERIOR);
		SetPlayerCameraPos(playerid,ClassSel_SetupTeamTEAM_POS[2][0],ClassSel_SetupTeamTEAM_POS[2][1],ClassSel_SetupTeamTEAM_POS[2][2]);
		SetPlayerCameraLookAt(playerid,ClassSel_SetupTeamTEAM_LOOK_AT[2][0],ClassSel_SetupTeamTEAM_LOOK_AT[2][1],ClassSel_SetupTeamTEAM_LOOK_AT[2][2]);
		new astring[64];
		PlayerTextDrawHide(playerid,TeamText[playerid]);
		PlayerTextDrawHide(playerid,LocText[playerid]);
		new THIRD_TEAM_TEXTDRAW_STRING[64];
		format(THIRD_TEAM_TEXTDRAW_STRING,sizeof(THIRD_TEAM_TEXTDRAW_STRING),"%s (%d Players)",THIRD_TEAM_NAME_TAG,GetTeamPlayerCount(THIRD_TEAM));
		PlayerTextDrawSetString(playerid, TeamText[playerid],THIRD_TEAM_TEXTDRAW_STRING);
		PlayerTextDrawColor(playerid,TeamText[playerid],COLOR_TEAM_THREE_TD);
		PlayerTextDrawShow(playerid,TeamText[playerid]);
		format(astring,sizeof(astring),"Location:"#THIRD_TEAM_LOCATION);
		PlayerTextDrawSetString(playerid, LocText[playerid], astring);
		PlayerTextDrawShow(playerid,LocText[playerid]);
		PlayerPlaySound(playerid,2404,0,0,0);

	}
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE >= 4
	else if(gPlayerTeamSelection[playerid] == FOURTH_TEAM) {
		SetPlayerInterior(playerid,TEAM_FOUR_SEL_INTERIOR);
		SetPlayerCameraPos(playerid,ClassSel_SetupTeamTEAM_POS[3][0],ClassSel_SetupTeamTEAM_POS[3][1],ClassSel_SetupTeamTEAM_POS[3][2]);
		SetPlayerCameraLookAt(playerid,ClassSel_SetupTeamTEAM_LOOK_AT[3][0],ClassSel_SetupTeamTEAM_LOOK_AT[3][1],ClassSel_SetupTeamTEAM_LOOK_AT[3][2]);
		new astring[64];
		PlayerTextDrawHide(playerid,TeamText[playerid]);
		PlayerTextDrawHide(playerid,LocText[playerid]);
		new FOURTH_TEAM_TEXTDRAW_STRING[64];
		format(FOURTH_TEAM_TEXTDRAW_STRING,sizeof(FOURTH_TEAM_TEXTDRAW_STRING),"%s (%d Players)",FOURTH_TEAM_NAME_TAG,GetTeamPlayerCount(FOURTH_TEAM));
		PlayerTextDrawSetString(playerid, TeamText[playerid],FOURTH_TEAM_TEXTDRAW_STRING);
		PlayerTextDrawColor(playerid,TeamText[playerid],COLOR_TEAM_FOUR_TD);
		PlayerTextDrawShow(playerid,TeamText[playerid]);
		format(astring,sizeof(astring),"Location:"#FOURTH_TEAM_LOCATION);
		PlayerTextDrawSetString(playerid, LocText[playerid], astring);
		PlayerTextDrawShow(playerid,LocText[playerid]);
		SetPlayerWeather(playerid,0);
		PlayerPlaySound(playerid,2404,0,0,0);

	}
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE >= 5
	else if(gPlayerTeamSelection[playerid] == FIFTH_TEAM) {
		SetPlayerInterior(playerid,TEAM_FIVE_SEL_INTERIOR);
		SetPlayerCameraPos(playerid,ClassSel_SetupTeamTEAM_POS[4][0],ClassSel_SetupTeamTEAM_POS[4][1],ClassSel_SetupTeamTEAM_POS[4][2]);
		SetPlayerCameraLookAt(playerid,ClassSel_SetupTeamTEAM_LOOK_AT[4][0],ClassSel_SetupTeamTEAM_LOOK_AT[4][1],ClassSel_SetupTeamTEAM_LOOK_AT[4][2]);
		new astring[64];
		PlayerTextDrawHide(playerid,TeamText[playerid]);
		PlayerTextDrawHide(playerid,LocText[playerid]);
		new FIFTH_TEAM_TEXTDRAW_STRING[64];
		format(FIFTH_TEAM_TEXTDRAW_STRING,sizeof(FIFTH_TEAM_TEXTDRAW_STRING),"%s (%d Players)",FIFTH_TEAM_NAME_TAG,GetTeamPlayerCount(FIFTH_TEAM));
		PlayerTextDrawSetString(playerid, TeamText[playerid],FIFTH_TEAM_TEXTDRAW_STRING);
		PlayerTextDrawColor(playerid,TeamText[playerid],COLOR_TEAM_FIVE_TD);
		PlayerTextDrawShow(playerid,TeamText[playerid]);
		format(astring,sizeof(astring),"Location:"#FIFTH_TEAM_LOCATION);
		PlayerTextDrawSetString(playerid, LocText[playerid], astring);
		PlayerTextDrawShow(playerid,LocText[playerid]);

		SetPlayerWeather(playerid,0);
		PlayerPlaySound(playerid,2404,0,0,0);

	}
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE == 6
	else if(gPlayerTeamSelection[playerid] == SIXTH_TEAM) {
		SetPlayerInterior(playerid,TEAM_SIX_SEL_INTERIOR);
		SetPlayerCameraPos(playerid,ClassSel_SetupTeamTEAM_POS[5][0],ClassSel_SetupTeamTEAM_POS[5][1],ClassSel_SetupTeamTEAM_POS[5][2]);
		SetPlayerCameraLookAt(playerid,ClassSel_SetupTeamTEAM_LOOK_AT[5][0],ClassSel_SetupTeamTEAM_LOOK_AT[5][1],ClassSel_SetupTeamTEAM_LOOK_AT[5][2]);
		new astring[64];
		PlayerTextDrawHide(playerid,TeamText[playerid]);
		PlayerTextDrawHide(playerid,LocText[playerid]);
		new SIXTH_TEAM_TEXTDRAW_STRING[64];
		format(SIXTH_TEAM_TEXTDRAW_STRING,sizeof(SIXTH_TEAM_TEXTDRAW_STRING),"%s (%d Players)",SIXTH_TEAM_NAME_TAG,GetTeamPlayerCount(SIXTH_TEAM));
		PlayerTextDrawSetString(playerid, TeamText[playerid],SIXTH_TEAM_TEXTDRAW_STRING);
		PlayerTextDrawColor(playerid,TeamText[playerid],COLOR_TEAM_SIX_TD);
		PlayerTextDrawShow(playerid,TeamText[playerid]);
		format(astring,sizeof(astring),"Location:"#SIXTH_TEAM_LOCATION);
		PlayerTextDrawSetString(playerid, LocText[playerid], astring);
		PlayerTextDrawShow(playerid,LocText[playerid]);
		PlayerPlaySound(playerid,2404,0,0,0);

	}
	#endif
	#endif
}

//----------------------------------------------------------
public BlowUpThisBed(teamid)
{
	GetObjectPos(BedArray[teamid],x1,y1,z1);
	CreateExplosion(x1, y1, z1, 7, 10.0);//'heavy' explosion close to bed
	CreateExplosion(x1, y1, z1, 9, 10.0);
	running=0;
	printf("Blow Up This Bed called");
	if(BedStates[teamid] == 0)
	{
		SendTeamMessage(teamid,COLOR_WHITE,"SERVER: Sudden death! Keep running, your bed has been blown up! There is no respawn after death!");
		new adstring[64];
		format(adstring,sizeof(adstring),"SERVER: The bed of team %s {FFFFFF}blew up!",GetTeamColorTag(teamid));
		SendClientMessageToAll(-1,adstring);
		GangZoneHideForAll(TEAM_ZONE[teamid]);
		DestroyObject(BedArray[teamid]);//Destroy/Remove the bed of this team after blowing up

	}
}
stock GetVehicleDriver(vid)
{
	for(new i; i < GetMaxPlayers(); i++) 
	{
		if(!IsPlayerConnected(i)) continue;
		if(GetPlayerVehicleID(i) == vid && GetPlayerSeat(playerid) == 0) return i;
	}
	return INVALID_PLAYER_ID;
}
stock RespawnAllVehicles()
{
	for(new i = GetVehiclePoolSize(); i > 0; i--)
	{
		if(GetVehicleDriver(i) != INVALID_PLAYER_ID) continue;
		SetVehicleToRespawn(i);
	}
}
stock GetPlayerCount()
{
	new counter=0;
	for(new k;k<MAX_PLAYERS;k++)
	{
		if(!IsPlayerConnected(k) || IsPlayerNPC(k)) continue;

		counter++;
	}
	return counter;
}

public OnPlayerStateChange(playerid, newstate, oldstate)
{
	if(newstate == PLAYER_STATE_DRIVER || newstate == PLAYER_STATE_PASSENGER)
	{
		if(IsBeingSpeced[playerid] == 1)
		{
			for(new i;i<MAX_PLAYERS;i++)
			{
				if(spectatorid[i] == playerid)
				{
					PlayerSpectateVehicle(i, GetPlayerVehicleID(playerid));
				}
			}
		}
	}
	if(newstate == PLAYER_STATE_ONFOOT)
	{
		if(IsBeingSpeced[playerid] == 1)
		{
			for(new i;i<MAX_PLAYERS;i++)
			{
				if(spectatorid[i] == playerid)
				{
					PlayerSpectatePlayer(i, playerid);
				}
			}
		}
	}
	return 1;
}
public OnPlayerInteriorChange(playerid, newinteriorid, oldinteriorid)
{
	if(IsBeingSpeced[playerid] == 1)
	{
		for(new i;i<MAX_PLAYERS;i++)
		{
			if(spectatorid[i] == playerid)
			{
				SetPlayerInterior(i,GetPlayerInterior(playerid));
				SetPlayerVirtualWorld(i,GetPlayerVirtualWorld(playerid));
			}
		}
	}
	return 1;
}

public OnPlayerSpawn(playerid)
{
	if(IsSpecing[playerid] == 1)
	{
		SetPlayerPos(playerid,SpecX[playerid],SpecY[playerid],SpecZ[playerid]);
		SetPlayerInterior(playerid,Inter[playerid]);
		SetPlayerVirtualWorld(playerid,vWorld[playerid]);
		IsSpecing[playerid] = 0;
		IsBeingSpeced[spectatorid[playerid]] = 0;
	}
	if(IsPlayerNPC(playerid)) return 1;
	if(IsSpecing[playerid] == 1) return 1;
	if(gPlayerTeamSelection[playerid] == TEAM_SPECTATOR) return 1;
	
	TextDrawShowForPlayer(playerid,txtTimeDisp);
	PlayerPlaySound(playerid,1188,0,0,0);
	SetPlayerWorldBounds(playerid,MAP_WORLDBOUNDS[0][0], MAP_WORLDBOUNDS[0][1], MAP_WORLDBOUNDS[0][2], MAP_WORLDBOUNDS[0][3]);

	new randSpawn = 0;

	if(!GameHasStarted) SetPlayerInvulnerable(playerid,1);
	SetPlayerVirtualWorld(playerid,0);
	TogglePlayerClock(playerid,0);
	ResetPlayerMoney(playerid);
	PlayerInfo[playerid][pSpawned] = 1;
	
	switch(gPlayerTeamSelection[playerid])
	{
	case FIRST_TEAM: 
		{
			randSpawn = random(sizeof(gSpawnsTeam_TEAM_ONE));
			SetPlayerPos(playerid,gSpawnsTeam_TEAM_ONE[randSpawn][0],gSpawnsTeam_TEAM_ONE[randSpawn][1],gSpawnsTeam_TEAM_ONE[randSpawn][2]);
			SetPlayerFacingAngle(playerid,gSpawnsTeam_TEAM_ONE[randSpawn][3]);
		}
		#if defined TEAMSIZE
		#if TEAMSIZE >= 2
	case SECOND_TEAM:
		{
			randSpawn = random(sizeof(gSpawnsTeam_TEAM_TWO));
			SetPlayerPos(playerid,
			gSpawnsTeam_TEAM_TWO[randSpawn][0],
			gSpawnsTeam_TEAM_TWO[randSpawn][1],
			gSpawnsTeam_TEAM_TWO[randSpawn][2]);
			SetPlayerFacingAngle(playerid,gSpawnsTeam_TEAM_TWO[randSpawn][3]);
		}
		#endif
		#endif
		#if defined TEAMSIZE
		#if TEAMSIZE >= 3
	case THIRD_TEAM:
		{
			randSpawn = random(sizeof(gSpawnsTeam_TEAM_THREE));
			SetPlayerPos(playerid,
			gSpawnsTeam_TEAM_THREE[randSpawn][0],
			gSpawnsTeam_TEAM_THREE[randSpawn][1],
			gSpawnsTeam_TEAM_THREE[randSpawn][2]);
			SetPlayerFacingAngle(playerid,gSpawnsTeam_TEAM_THREE[randSpawn][3]);
		}
		#endif
		#endif
		#if defined TEAMSIZE
		#if TEAMSIZE >= 4
	case FOURTH_TEAM:
		{
			randSpawn = random(sizeof(gSpawnsTeam_TEAM_FOUR));
			SetPlayerPos(playerid,
			gSpawnsTeam_TEAM_FOUR[randSpawn][0],
			gSpawnsTeam_TEAM_FOUR[randSpawn][1],
			gSpawnsTeam_TEAM_FOUR[randSpawn][2]);
			SetPlayerFacingAngle(playerid,gSpawnsTeam_TEAM_FOUR[randSpawn][3]);
		}
		#endif
		#endif
		#if defined TEAMSIZE
		#if TEAMSIZE >= 5
	case FIFTH_TEAM:
		{
			randSpawn = random(sizeof(gSpawnsTeam_TEAM_FIVE));
			SetPlayerPos(playerid,
			gSpawnsTeam_TEAM_FIVE[randSpawn][0],
			gSpawnsTeam_TEAM_FIVE[randSpawn][1],
			gSpawnsTeam_TEAM_FIVE[randSpawn][2]);
			SetPlayerFacingAngle(playerid,gSpawnsTeam_TEAM_FIVE[randSpawn][3]);
		}
		#endif
		#endif
		#if defined TEAMSIZE
		#if TEAMSIZE == 6
	case SIXTH_TEAM:
		{
			randSpawn = random(sizeof(gSpawnsTeam_TEAM_SIX));
			SetPlayerPos(playerid,
			gSpawnsTeam_TEAM_SIX[randSpawn][0],
			gSpawnsTeam_TEAM_SIX[randSpawn][1],
			gSpawnsTeam_TEAM_SIX[randSpawn][2]);
			SetPlayerFacingAngle(playerid,gSpawnsTeam_TEAM_SIX[randSpawn][3]);
		}
		#endif
		#endif
	}
	
	
	GangZoneShowForPlayer(playerid, TEAM_ZONE[FIRST_TEAM], COLOR_TEAM_ONE);
	GangZoneShowForPlayer(playerid, TEAM_ZONE[SECOND_TEAM], COLOR_TEAM_TWO);
	#if defined TEAMSIZE
	#if TEAMSIZE >= 3
	GangZoneShowForPlayer(playerid, TEAM_ZONE[THIRD_TEAM], COLOR_TEAM_THREE);
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE >= 4
	GangZoneShowForPlayer(playerid, TEAM_ZONE[FOURTH_TEAM], COLOR_TEAM_FOUR);
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE >= 5
	GangZoneShowForPlayer(playerid, TEAM_ZONE[FIFTH_TEAM], COLOR_TEAM_FIVE);
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE == 6
	GangZoneShowForPlayer(playerid, TEAM_ZONE[SIXTH_TEAM], COLOR_TEAM_SIX);
	#endif
	#endif
	
	
	SetPlayerSkillLevel(playerid,WEAPONSKILL_PISTOL,100);
	SetPlayerSkillLevel(playerid,WEAPONSKILL_PISTOL_SILENCED,100);
	SetPlayerSkillLevel(playerid,WEAPONSKILL_DESERT_EAGLE,100);
	SetPlayerSkillLevel(playerid,WEAPONSKILL_SHOTGUN,100);
	SetPlayerSkillLevel(playerid,WEAPONSKILL_SAWNOFF_SHOTGUN,100);
	SetPlayerSkillLevel(playerid,WEAPONSKILL_SPAS12_SHOTGUN,100);
	SetPlayerSkillLevel(playerid,WEAPONSKILL_MICRO_UZI,100);
	SetPlayerSkillLevel(playerid,WEAPONSKILL_MP5,100);
	SetPlayerSkillLevel(playerid,WEAPONSKILL_AK47,100);
	SetPlayerSkillLevel(playerid,WEAPONSKILL_M4,100);
	SetPlayerSkillLevel(playerid,WEAPONSKILL_SNIPERRIFLE,100);

	GivePlayerWeapon(playerid,WEAPON_COLT45,100);
	GivePlayerWeapon(playerid,25,5);
	
	switch(gPlayerTeamSelection[playerid])
	{
	case FIRST_TEAM:
		{
			SetPlayerInterior(playerid,TEAM_ONE_INTERIOR);
		}
	case SECOND_TEAM:
		{
			SetPlayerInterior(playerid,TEAM_TWO_INTERIOR);
		}
		#if defined TEAMSIZE
		#if TEAMSIZE >= 3
	case THIRD_TEAM:
		{
			SetPlayerInterior(playerid,TEAM_THREE_INTERIOR);
		}
		#endif
		#endif
		#if defined TEAMSIZE
		#if TEAMSIZE >= 4
	case FOURTH_TEAM:
		{
			SetPlayerInterior(playerid,TEAM_FOUR_INTERIOR);
		}
		#endif
		#endif
		#if defined TEAMSIZE
		#if TEAMSIZE >= 5
	case FIFTH_TEAM:
		{
			SetPlayerInterior(playerid,TEAM_FIVE_INTERIOR);
		}
		#endif
		#endif
		#if defined TEAMSIZE
		#if TEAMSIZE >= 6
	case SIXTH_TEAM:
		{
			SetPlayerInterior(playerid,TEAM_SIX_INTERIOR);
		}
		#endif
		#endif
	default:
		{
			printf("Something went wrong..");
			TextDrawHideForPlayer(playerid,txtSpectatorHelper);
		}
	}


	return 1; 
}

public TEAM_MONEY()//MONEY FOR EACH TEAM
{
	for (new m=0;m<sizeof(MoneySpawns)-1;m++)
	{
		GenerateRandomPickup(1212,19,MoneySpawns[m][0],MoneySpawns[m][1],MoneySpawns[m][2],MoneySpawns[m][3],MoneySpawns[m][4],MoneySpawns[m][5],0);
	}

}

//----------------------------------------------------------


public MONEY_MAIN()//MAIN
{
	
	GenerateRandomPickup(1212,19,MoneySpawns[sizeof(MoneySpawns)-1][0],MoneySpawns[sizeof(MoneySpawns)-1][1],MoneySpawns[sizeof(MoneySpawns)-1][2],MoneySpawns[sizeof(MoneySpawns)-1][3],MoneySpawns[sizeof(MoneySpawns)-1][4],MoneySpawns[sizeof(MoneySpawns)-1][5],0);
}	


stock ResetPlayerData(playerid)
{
	IsSpecing[playerid]=0;
	IsBeingSpeced[playerid]=0;
	PStealth[playerid]=0;
	Warppowder[playerid]=0;
	Helmet[playerid]=0;
	PBombID[playerid]=0;
	PBomb[playerid]=0;
	PSkill[playerid]=0;
	WarpVar[playerid]=4;
	if(IsValidObject(BombObject[playerid]))
	DestroyObject(BombObject[playerid]);	
	pZ[playerid]=0;
	pY[playerid]=0;
	pZ[playerid]=0;
	if(IsValidObject(Beam[playerid]))
	DestroyObject(Beam[playerid]);	
	PlayerInfo[playerid][pSpawned] = 0;
	SetPlayerFightingStyle(playerid,FIGHT_STYLE_NORMAL);
}

stock CreateGlobalActor(actorid,modelid,Float:ax,Float:ay,Float:az,Float:angle,Float:distance,pickupid)
{
	actorid = CreateActor(modelid,ax,ay,az,angle);
	new Float:x, Float:y, Float:z;
	
	GetActorPos(actorid, x, y, z);
	GetXYInFrontOfActor(actorid, x, y, distance);
	ActorPickups[pickupid] = CreatePickup(1210,2,x,y,z,-1);
	SetActorInvulnerable(actorid, true);
	printf("Actor created, pickupid %d",pickupid);
	printf("Array pickupid %d",ActorPickups[Shop_Counter++]);
	return pickupid;
}

stock GetXYInFrontOfActor(actorid, &Float:x, &Float:y, Float:distance)
{
	new Float:a;
	GetActorPos(actorid, x, y, a);
	GetActorFacingAngle(actorid, a);
	x += (distance * floatsin(-a, degrees));
	y += (distance * floatcos(-a, degrees));
}

stock GivePlayerMoneyText(playerid,value)
{
	new moneyplayerid[32];
	if(value < 0)
	format(moneyplayerid,sizeof(moneyplayerid), "~r~ %d~y~$",value);
	if(value >= 0)
	format(moneyplayerid,sizeof(moneyplayerid), "~g~ %d~y~$",value);
	GameTextForPlayer(playerid,moneyplayerid,1000,1);
	GivePlayerMoney(playerid,value);	
}
ClassSel_InitTextDraws()
{
	
	// Init our observer helper text display
	txtClassSelHelper = TextDrawCreate(10.0, 415.0,
	" Press ~b~~k~~GO_LEFT~ ~w~or ~b~~k~~GO_RIGHT~ ~w~to switch teams.~n~ Press ~y~~k~~PED_FIREWEAPON~ ~y~to select.");
	TextDrawUseBox(txtClassSelHelper, 1);
	TextDrawBoxColor(txtClassSelHelper,0x22222266);
	TextDrawLetterSize(txtClassSelHelper,0.3,1.0);
	TextDrawTextSize(txtClassSelHelper,400.0,40.0);
	TextDrawFont(txtClassSelHelper, 2);
	TextDrawSetShadow(txtClassSelHelper,0);
	TextDrawSetOutline(txtClassSelHelper,1);
	TextDrawBackgroundColor(txtClassSelHelper,0x000000FF);
	TextDrawColor(txtClassSelHelper,0xFFFFFFFF);
	

}
