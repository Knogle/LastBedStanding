

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
#include <YSI\y_ini>





#define FIRST_TEAM 0 
#define SECOND_TEAM 1 
#define THIRD_TEAM 2
#define FOURTH_TEAM 3 
#define FIFTH_TEAM 4
#define SIXTH_TEAM 5 
#define TEAM_SPECTATOR 6 	


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

enum pInfo
{
	pBombs,
	pPass,
	pBeds,
	pKills,
	pDeaths,
	pAdmin
};

enum
{
	INVALID_PICKUP_TYPE,
	MONEY_TYPE,
	ACTOR_TYPE,
	INFO_TYPE
};

#define MAPTYPE BONE_COUNTY
#pragma tabsize 0

#define PATH "/Users/%s.ini"




#define PRESSED(%0) \
	(((newkeys & (%0)) == (%0)) && ((oldkeys & (%0)) != (%0)))


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




//----------------------------------------------------------
forward SaveUser_data(playerid);
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
forward BlowUpThisBed();
forward SendTeamMessage(teamid, color, const message[]);
forward SellPlayerWeapon(playerid,cost,weaponid,ammo);
forward SpecPlayer(playerid,targetplayer);
forward UpdateTimeAndWeather();
forward CountDown();
//----------------------------------------------------------
new Warppowder[MAX_PLAYERS];
new PBeds[MAX_PLAYERS];
new PKills[MAX_PLAYERS];
new PSkill[MAX_PLAYERS];
new PBomb[MAX_PLAYERS];
new LastMoney[MAX_PLAYERS];
new PStealth[MAX_PLAYERS];
new String[128], Float:SpecX[MAX_PLAYERS], Float:SpecY[MAX_PLAYERS], Float:SpecZ[MAX_PLAYERS], vWorld[MAX_PLAYERS], Inter[MAX_PLAYERS];
new IsSpecing[MAX_PLAYERS], Name[MAX_PLAYER_NAME], IsBeingSpeced[MAX_PLAYERS],spectatorid[MAX_PLAYERS];
new Float:pX[MAX_PLAYERS];
new Float:pY[MAX_PLAYERS];
new Float:pZ[MAX_PLAYERS];
new Float:wX[MAX_PLAYERS];
new Float:wY[MAX_PLAYERS];
new Float:wZ[MAX_PLAYERS];
new PBombID[MAX_PLAYERS];
new Beam[MAX_PLAYERS];
new gPlayerTeamSelection[MAX_PLAYERS];
new gPlayerHasTeamSelected[MAX_PLAYERS];
new gPlayerLastTeamSelectionTick[MAX_PLAYERS];
new PlayerInfo[MAX_PLAYERS][pInfo];
new BombObject[MAX_PLAYERS];
new Shop_Counter;

//--------------------------------------------------------------



new totaltime;




//--------------------------------------------------------------
new Menu:infomenu;
new Menu:shotungs;
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
new CountDownVar = 4;
new WarpVar[MAX_PLAYERS];
new WarpTimer;
new CountDownTimer;




new hour, minute;
new timestr[32];
new timestrex[32];
new delay=1000;// only allow new selection every ~1000 ms; may differ due to server load
new bombtimer=5000;// time in ms to blow up closest bedwars bed
new MoneyDropTimer=2500;// Time to generate new moneypickups


//--------------------------------------------------------------
stock udb_hash(buf[]) {
	new length=strlen(buf);
	new s1 = 1;
	new s2 = 0;
	new n;
	for (n=0; n<length; n++)
	{
		s1 = (s1 + buf[n]) % 65521;
		s2 = (s2 + s1)     % 65521;
	}
	return (s2 << 16) + s1;
}

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

new ActorPickups[sizeof(GlobalActors)];
new InfoPickups[sizeof(PlayerInfoPickups)];
new MoneyPickups[MAX_PICKUPS-sizeof(ActorPickups)-sizeof(InfoPickups)-1];

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

new TEAM_ZONE_ONE;
new TEAM_ZONE_TWO;

#if defined TEAMSIZE
#if TEAMSIZE >=3
new TEAM_ZONE_THREE;
#endif
#endif
#if defined TEAMSIZE
#if TEAMSIZE >=4
new TEAM_ZONE_FOUR;
#endif
#endif
#if defined TEAMSIZE
#if TEAMSIZE >=5
new TEAM_ZONE_FIVE;
#endif
#endif
#if defined TEAMSIZE
#if TEAMSIZE == 6
new TEAM_ZONE_SIX;
#endif
#endif


new BED_STATE_TEAM_ONE;
new BED_STATE_TEAM_TWO;

#if defined TEAMSIZE
#if TEAMSIZE >=3
new BED_STATE_TEAM_THREE;
#endif
#endif
#if defined TEAMSIZE
#if TEAMSIZE >=4
new BED_STATE_TEAM_FOUR;
#endif
#endif
#if defined TEAMSIZE
#if TEAMSIZE >=5
new BED_STATE_TEAM_FIVE;
#endif
#endif
#if defined TEAMSIZE
#if TEAMSIZE ==6
new BED_STATE_TEAM_SIX;
#endif
#endif


public OnGameModeInit()
{
	CreateObject(1829,413.6000100,2537.6001000,18.6000000,0.0000000,0.0000000,0.0000000); //object(man_safenew) (1)
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
	
	SendRconCommand("mapname "#MAPTYPE);

	
	
	#if defined TEAMSIZE
	#if TEAMSIZE >=2
	TEAM_ZONE_ONE =   GangZoneCreate(GlobalZones[0][0],GlobalZones[0][1],GlobalZones[0][2],GlobalZones[0][3]);
	TEAM_ZONE_TWO = GangZoneCreate(GlobalZones[1][0],GlobalZones[1][1],GlobalZones[1][2],GlobalZones[1][3]);
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE >=3
	TEAM_ZONE_THREE = GangZoneCreate(GlobalZones[2][0],GlobalZones[2][1],GlobalZones[2][2],GlobalZones[2][3]);
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE >=4
	TEAM_ZONE_FOUR =   GangZoneCreate(GlobalZones[3][0],GlobalZones[3][1],GlobalZones[3][2],GlobalZones[3][3]);
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE >=5
	TEAM_ZONE_FIVE = GangZoneCreate(GlobalZones[4][0],GlobalZones[4][1],GlobalZones[4][2],GlobalZones[4][3]);
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE >=6
	TEAM_ZONE_SIX = GangZoneCreate(GlobalZones[5][0],GlobalZones[5][1],GlobalZones[5][2],GlobalZones[5][3]);
	#endif
	#endif
	
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
		AddMenuItem(shopmenu, 0, "Warpkit $100000");
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
		AddMenuItem(ammunation,0,"  Items");
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
	shotungs=CreateMenu("~w~Ammu-Nation",1,20,150,150);
	if(IsValidMenu(shotungs)){
		SetMenuColumnHeader(shotungs, 0, "Shotguns");
		AddMenuItem(shotungs,0,"  Shotgun $12500");
		AddMenuItem(shotungs,0,"  Sawn Off $15500");
		AddMenuItem(shotungs,0,"  Combat Shotgun $15500");
		AddMenuItem(shotungs, 0, "Back");
	}
	items=CreateMenu("~w~Ammu-Nation",1,20,150,150);
	if(IsValidMenu(items)){
		SetMenuColumnHeader(items, 0, "Item");
		AddMenuItem(items,0,"  Armour $2500");
		AddMenuItem(items,0,"  Parachute $2500");
		AddMenuItem(items,0,"  Spraycan $2500");
		AddMenuItem(items,0,"  Camera $2500");
		AddMenuItem(items,0,"  Armour $2500");
		AddMenuItem(items,0,"  Health $2500");
		AddMenuItem(items,0,"Back");
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
		AddMenuItem(rifles,0," Sniper Rifle $25000");
		AddMenuItem(rifles, 0, "Back");
	}
	assaultrifle=CreateMenu("~w~Ammu-Nation",1,20,150,150);
	if(IsValidMenu(assaultrifle)){
		SetMenuColumnHeader(assaultrifle, 0, "Assault Rifle");
		AddMenuItem(assaultrifle,0," AK-47 $20000");
		AddMenuItem(assaultrifle,0," M4 $25000");
		AddMenuItem(assaultrifle, 0, "Back");
	}
	special=CreateMenu("~w~Ammu-Nation",1,20,150,150);
	if(IsValidMenu(special)){
		SetMenuColumnHeader(special, 0, "special");
		AddMenuItem(special,0," Minigun $1000000");
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
stock UserPath(playerid)
{
	new string[128],playername[MAX_PLAYER_NAME];
	GetPlayerName(playerid,playername,sizeof(playername));
	format(string,sizeof(string),PATH,playername);
	return string;
}
public SaveUser_data(playerid)
{
	
	new INI:File = INI_Open(UserPath(playerid));
	INI_SetTag(File,"data");
	INI_WriteInt(File,"Bombs",PlayerInfo[playerid][pBombs]);
	INI_WriteInt(File,"Beds",PlayerInfo[playerid][pBeds]);
	INI_WriteInt(File,"Kills",PlayerInfo[playerid][pKills]);
	INI_WriteInt(File,"Deaths",PlayerInfo[playerid][pDeaths]);
	INI_WriteInt(File,"Admin",PlayerInfo[playerid][pAdmin]);
	INI_Close(File);
}
public LoadUser_data(playerid,name[],value[])
{
	//INI_Int("Password",PlayerInfo[playerid][pPass]);
	INI_Int("Bombs",PlayerInfo[playerid][pBeds]);
	INI_Int("Beds",PlayerInfo[playerid][pBeds]);
	INI_Int("Kills",PlayerInfo[playerid][pKills]);
	INI_Int("Deaths",PlayerInfo[playerid][pDeaths]);
	INI_Int("Admin",PlayerInfo[playerid][pAdmin]);
	return 1;
}

stock TeleportPlayerToBase(playerid)
{
	#if defined TEAMSIZE
	#if TEAMSIZE >= 2	
	new randSpawn=0;
	if(IsValidObject(Beam[playerid]))
	DestroyObject(Beam[playerid]);	
	switch(gPlayerTeamSelection[playerid])
	{
	case 0:
		{
			new str[80];
			randSpawn = random(sizeof(gSpawnsTeam_TEAM_ONE));
			format(str,sizeof(str),"SERVER: You have been teleported to the base of your team "#FIRST_TEAM_COLOR_TAG);
			SendClientMessage(playerid,COLOR_WHITE,str);
			SetPlayerPos(playerid,
			gSpawnsTeam_TEAM_ONE[randSpawn][0],
			gSpawnsTeam_TEAM_ONE[randSpawn][1],
			gSpawnsTeam_TEAM_ONE[randSpawn][2]);	
		}
	case 1:
		{
			new str[80];
			randSpawn = random(sizeof(gSpawnsTeam_TEAM_TWO));
			format(str,sizeof(str),"SERVER: You have been teleported to the base of your team "#SECOND_TEAM_COLOR_TAG);
			SendClientMessage(playerid,COLOR_WHITE,str);
			SetPlayerPos(playerid,
			gSpawnsTeam_TEAM_TWO[randSpawn][0],
			gSpawnsTeam_TEAM_TWO[randSpawn][1],
			gSpawnsTeam_TEAM_TWO[randSpawn][2]);	
		}
		
		
		#endif
		#endif	
		#if defined TEAMSIZE
		#if TEAMSIZE >= 3

		
	case 2:
		{
			new str[80];
			randSpawn = random(sizeof(gSpawnsTeam_TEAM_THREE));
			format(str,sizeof(str),"SERVER: You have been teleported to the base of your team "#THIRD_TEAM_COLOR_TAG);
			SendClientMessage(playerid,COLOR_WHITE,str);
			SetPlayerPos(playerid,
			gSpawnsTeam_TEAM_THREE[randSpawn][0],
			gSpawnsTeam_TEAM_THREE[randSpawn][1],
			gSpawnsTeam_TEAM_THREE[randSpawn][2]);	
		}
		
		
		#endif
		#endif	
		#if defined TEAMSIZE
		#if TEAMSIZE >= 4

		
	case 3:
		{
			new str[80];
			randSpawn = random(sizeof(gSpawnsTeam_TEAM_FOUR));
			format(str,sizeof(str),"SERVER: You have been teleported to the base of your team "#FOURTH_TEAM_COLOR_TAG);
			SendClientMessage(playerid,COLOR_WHITE,str);
			SetPlayerPos(playerid,
			gSpawnsTeam_TEAM_FOUR[randSpawn][0],
			gSpawnsTeam_TEAM_FOUR[randSpawn][1],
			gSpawnsTeam_TEAM_FOUR[randSpawn][2]);
		}
		
		#endif
		#endif	
		#if defined TEAMSIZE
		#if TEAMSIZE >= 5	

		
	case 4:
		{
			new str[80];
			randSpawn = random(sizeof(gSpawnsTeam_TEAM_FIVE));
			format(str,sizeof(str),"SERVER: You have been teleported to the base of your team "#FIFTH_TEAM_COLOR_TAG);
			SendClientMessage(playerid,COLOR_WHITE,str);
			SetPlayerPos(playerid,
			gSpawnsTeam_TEAM_FIVE[randSpawn][0],
			gSpawnsTeam_TEAM_FIVE[randSpawn][1],
			gSpawnsTeam_TEAM_FIVE[randSpawn][2]);
		}
		
		#endif
		#endif
		#if defined TEAMSIZE
		#if TEAMSIZE == 6	

		
	case 5:
		{
			new str[80];
			randSpawn = random(sizeof(gSpawnsTeam_TEAM_SIX));
			format(str,sizeof(str),"SERVER: You have been teleported to the base of your team "#SIXTH_TEAM_COLOR_TAG);
			SendClientMessage(playerid,COLOR_WHITE,str);
			SetPlayerPos(playerid,
			gSpawnsTeam_TEAM_SIX[randSpawn][0],
			gSpawnsTeam_TEAM_SIX[randSpawn][1],
			gSpawnsTeam_TEAM_SIX[randSpawn][2]);	
		}
		
		#endif
		#endif	
	
	}
	
	
	
}
stock SpecPlayer(playerid,targetplayer)
{

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

stock reveal(playerid)
{
	
	SendClientMessage(playerid,COLOR_WHITE,"SERVER: You are visible again!");
	if(PStealth[playerid] == -1 && (gPlayerTeamSelection == FIRST_TEAM))
	{
		PStealth[playerid]=0;
		SetPlayerColor(playerid,COLOR_TEAM_ONE);
		
	}
	if(PStealth[playerid] == -1 && (gPlayerTeamSelection == SECOND_TEAM))
	{
		PStealth[playerid]=0;
		SetPlayerColor(playerid,COLOR_TEAM_TWO);
		
	}
	#if defined TEAMSIZE
	#if TEAMSIZE >= 3
	if(PStealth[playerid] == -1 && (gPlayerTeamSelection == THIRD_TEAM))
	{
		PStealth[playerid]=0;
		SetPlayerColor(playerid,COLOR_TEAM_THREE);
		
	}
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE >= 4
	if(PStealth[playerid] == -1 && (gPlayerTeamSelection == FOURTH_TEAM))
	{
		PStealth[playerid]=0;
		SetPlayerColor(playerid,COLOR_TEAM_FOUR);
		
	}
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE >= 5
	if(PStealth[playerid] == -1 && (gPlayerTeamSelection == FIFTH_TEAM))
	{
		PStealth[playerid]=0;
		SetPlayerColor(playerid,COLOR_TEAM_FIVE);
		
	}
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE == 6
	if(PStealth[playerid] == -1 && (gPlayerTeamSelection == SIXTH_TEAM))
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
		GivePlayerMoney(playerid,-cost);
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
		//printf("%d,%d,%f,%f,%f,%d",modelid,type,rx1,ry2,rz3,virtualworld);
		
	}


}

public CountDown()
{
	CountDownVar--;
	for(new i = 0; i < MAX_PLAYERS; i++)
	{
		if(CountDownVar == 0)
		{
			KillTimer(CountDownTimer);
			CountDownVar = 4;
			PlayerPlaySound(i,7419,0,0,0);
		}
		if(CountDownVar == 1)
		{
			PlayerPlaySound(i,7418,0,0,0);
		}
		if(CountDownVar == 2)
		{
			PlayerPlaySound(i,7417,0,0,0);
		}
	}
	return 1;
}

public warpcount(playerid)
{
	WarpVar[playerid]--;
	

		
		if(WarpVar[playerid] == 0)
		{
			if(!IsPlayerInRangeOfPoint(playerid,2.0,wX[playerid],wY[playerid],wZ[playerid]))
			{
				KillTimer(WarpTimer);
				WarpVar[playerid] = 4;
				Warppowder[playerid]=1;
				return SendClientMessage(playerid,COLOR_WHITE,"SERVER: You moved right away! Teleport aborted.");
			}
			TeleportPlayerToBase(playerid);
			
			WarpVar[playerid] = 4;	
			KillTimer(WarpTimer);
			PlayerPlaySound(playerid,1137,0,0,0);
		
		}
		if(WarpVar[playerid] == 1)
		{
			if(!IsPlayerInRangeOfPoint(playerid,2.0,wX[playerid],wY[playerid],wZ[playerid]))
			{
				KillTimer(WarpTimer);
				WarpVar[playerid] = 4;
				Warppowder[playerid]=1;
				return SendClientMessage(playerid,COLOR_WHITE,"SERVER: You moved right away! Teleport aborted.");
				
			}
			PlayerPlaySound(playerid,1137,0,0,0);
			
		}
		if(WarpVar[playerid] == 2)
		{
			if(!IsPlayerInRangeOfPoint(playerid,2.0,wX[playerid],wY[playerid],wZ[playerid]))
			{
				KillTimer(WarpTimer);
				WarpVar[playerid] = 4;
				Warppowder[playerid]=1;
				return SendClientMessage(playerid,COLOR_WHITE,"SERVER: You moved right away! Teleport aborted.");
			}
			PlayerPlaySound(playerid,1137,0,0,0);
		
		}
		if(WarpVar[playerid] == 3)
		{
			if(!IsPlayerInRangeOfPoint(playerid,2.0,wX[playerid],wY[playerid],wZ[playerid]))
			{
				KillTimer(WarpTimer);
				WarpVar[playerid] = 4;
				Warppowder[playerid]=1;
				return SendClientMessage(playerid,COLOR_WHITE,"SERVER: You moved right away! Teleport aborted.");
			}
			PlayerPlaySound(playerid,1137,0,0,0);
		
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
		printf("TEAM MESSAGE SENT");
	}
	return 1;
}

public OnPlayerExitedMenu(playerid)
{
	TogglePlayerControllable(playerid,true);
	return 1;
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
			GivePlayerMoney(playerid, 1000);
			printf("ID picked up: %d",maxmoney);
			MoneyPickups[index]=-1;
			quickSort(MoneyPickups,0,sizeof(MoneyPickups)-1);
			DestroyPickup(pickupid);
		}
	case ACTOR_TYPE:
		{
			ShowMenuForPlayer(shopmenu,playerid);
			TogglePlayerControllable(playerid,false);
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
	
	

	gettime(hour, minute);

	format(timestr,32,"%02d:%02d",hour,minute);
	format(timestrex,32,"Time: %02d:%02d",hour,minute);
	TextDrawSetString(txtTimeDisp,timestr);


	for(new i;i<MAX_PLAYERS;i++)
	{
		if(!IsPlayerConnected(i)) continue;
		SaveUser_data(i);
		if(IsPlayerAdmin(i))
		PlayerInfo[i][pAdmin]=1;	
	}

	printf("Executing UpdateTimeAndWeather");
	
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
}
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

public OnPlayerConnect(playerid)
{
	WarpVar[playerid]=4;
	ApplyAnimation(playerid, "BOMBER", "BOM_Plant_Loop", 4.0, 1, 0, 0, 1, 1);
	if(fexist(UserPath(playerid)))
	{
		
		INI_ParseFile(UserPath(playerid), "LoadUser_%s", .bExtra = true, .extra = playerid);
		SetPlayerScore(playerid, PlayerInfo[playerid][pBeds]);
		

	}
	else
	{
		new pname[MAX_PLAYER_NAME];
		GetPlayerName(playerid,pname,sizeof(pname));
		printf("ini file for %s ID:%d does not exist, writing..",pname,playerid);
		new INI:File = INI_Open(UserPath(playerid));
		INI_SetTag(File,"data");
		INI_WriteInt(File,"Bombs",0);
		INI_WriteInt(File,"Beds",0);
		INI_WriteInt(File,"Kills",0);
		INI_WriteInt(File,"Deaths",0);
		INI_WriteInt(File,"Admin",0);
		INI_Close(File);

	}
	SetPlayerMapIcon(playerid, 12,Center[0][0],Center[0][1],Center[0][2], 52, 0, MAPICON_GLOBAL);

	
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
ClassSel_SetupCharSelection(playerid)
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
		case 0:
			{
				SetPlayerColor(playerid,COLOR_TEAM_ONE);	
			}
		case 1:
			{
				SetPlayerColor(playerid,COLOR_TEAM_TWO);	
			}
			
			
			#endif
			#endif	
			#if defined TEAMSIZE
			#if TEAMSIZE >= 3	
		case 2:
			{
				SetPlayerColor(playerid,COLOR_TEAM_THREE);	
			}
			#endif
			#endif	
			#if defined TEAMSIZE
			#if TEAMSIZE >= 4

		case 3:
			{
				SetPlayerColor(playerid,COLOR_TEAM_FOUR);	
			}
			
			
			#endif
			#endif	
			#if defined TEAMSIZE
			#if TEAMSIZE >= 5	

			
			
		case 4:
			{
				SetPlayerColor(playerid,COLOR_TEAM_FIVE);	
			}
			
			
			#endif
			#endif
			#if defined TEAMSIZE
			#if TEAMSIZE >= 6	

			
			
		case 5:
			{
				SetPlayerColor(playerid,COLOR_TEAM_SIX);	
			}
			
			
			#endif
			#endif	
		}
	}
}

ClassSel_InitTeamNameText(playerid,PlayerText:txtInit)
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

ClassSel_SwitchToNextTeam(playerid)
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
	printf("ClassSel_SwitchToNextTeam");
}


ClassSel_SwitchToPreviousTeam(playerid)
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
	printf("ClassSel_SwitchToPreviousTeam");
}

ClassSel_HandleTeamSelection(playerid)
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
	printf("ClassSel_HandleTeamSelection");
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
	printf("OnPlayerRequestClass");
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
				ShowMenuForPlayer(shotungs,playerid);
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
	else if(Current==shotungs) {
		ShowMenuForPlayer(shotungs,playerid);
		switch(row) {
		case 0:	{
				SellPlayerWeapon(playerid,12500,25,50);

			}
		case 1: {
				SellPlayerWeapon(playerid,15500,26,50);

			}
		case 2: {
				SellPlayerWeapon(playerid,15500,27,50);
			}
		case 3: {
				HideMenuForPlayer(shotungs,playerid);
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
				GivePlayerMoney(playerid,-2500);
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

		case 4:		if(GetPlayerMoney(playerid) > 2500)
			{
				GivePlayerMoney(playerid,-2500);
				SetPlayerArmour(playerid,100.0);
			}
			else
			{
				SendClientMessage(playerid,COLOR_WHITE,"SERVER: Insufficient balance! You can not afford this item!");
			}
		case 5:		if(GetPlayerMoney(playerid) >= 2500)
			{
				GivePlayerMoney(playerid,-2500);
				SetPlayerHealth(playerid,100);
			}
			else
			{
				SendClientMessage(playerid,COLOR_WHITE,"SERVER: Insufficient balance! You can not afford this item!");
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
				SellPlayerWeapon(playerid,20000,30,500);
			}

		case 1:	{
				SellPlayerWeapon(playerid,25000,31,500);

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
				SellPlayerWeapon(playerid,25000,34,50);
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
				SellPlayerWeapon(playerid,1000000,38,1000);
			}
		case 1:	{
				SellPlayerWeapon(playerid,250000,35,50);
			}
		case 2:	{
				SellPlayerWeapon(playerid,250000,37,150);

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
					GivePlayerMoney(playerid,-10000);
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
				GivePlayerMoney(playerid,-100000);
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
		case 4: if(GetPlayerMoney(playerid) >= 100000 && Warppowder[playerid] == 0)
			
			{
				Warppowder[playerid]=1;
				SendClientMessage(playerid, 0xFFFFFFFF, "SERVER: You have bought a Warpkit! You can teleport yourself back to your base by using /warp");
				ShowMenuForPlayer(shopmenu,playerid);
				TogglePlayerControllable(playerid,false);
				GivePlayerMoney(playerid,-100000);
			}
			else if(Warppowder[playerid] != 0)
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
		case 5:
			{
				HideMenuForPlayer(shopmenu,playerid);
				ShowMenuForPlayer(yesno,playerid);
			}

		}
	}
}

stock GetTeamCount(teamid)
{
	new playercount = 0;
	for(new i = 0; i < MAX_PLAYERS; i++)
	{
		if(GetPlayerState(i) == PLAYER_STATE_NONE) continue;
		if(gPlayerTeamSelection[i] != teamid) continue;
		playercount++;
	}
	return playercount;
}


public OnPlayerTakeDamage(playerid, issuerid, Float: amount, weaponid, bodypart)
{
	if(issuerid != INVALID_PLAYER_ID && weaponid == 34 && bodypart == 9)
	{
		// One shot to the head to kill with sniper rifle
		SetPlayerHealth(playerid, 0.0);
	}
	return 1;
}



public OnPlayerDisconnect(playerid, reason)
{
	SaveUser_data(playerid);	
	LastMoney[playerid]=0;
	new pname[MAX_PLAYER_NAME], string[39 + MAX_PLAYER_NAME];
	GetPlayerName(playerid, pname, sizeof(pname));
	ResetPlayerData(playerid);
	switch(reason)
	{
	case 0: format(string, sizeof(string), "%s (%d) has left the server. (Lost Connection)", pname,playerid);
	case 1: format(string, sizeof(string), "%s (%d) has left the server. (Leaving)", pname,playerid);
	case 2: format(string, sizeof(string), "%s (%d) has left the server. (Kicked)", pname,playerid);
	}
	SendClientMessageToAll(0xAAAAAAAA, string);
	return 1;
}

public OnPlayerUpdate(playerid)
{
	
	if(!IsPlayerConnected(playerid)) return 0;
	if(IsPlayerNPC(playerid)) return 1;

	// changing teams by inputs
	if( !gPlayerHasTeamSelected[playerid] &&
			GetPlayerState(playerid) == PLAYER_STATE_SPECTATING ) {
		ClassSel_HandleTeamSelection(playerid);
		return 1;
	}
	return 1;

}

public OnPlayerCommandText(playerid, cmdtext[])
{
	new cmd[32], idx;
	sscanf(cmdtext,"s[32]d",cmd,idx);
	
	printf("OnPlayerCommandText");
	
	if(strcmp(cmd, "/weather", true) == 0) 
	{
		new targetweatherid;
		if(sscanf(cmdtext[strlen("/weather")+1], "i", targetweatherid))
		return SendClientMessage(playerid, COLOR_WHITE, "USAGE: /weather [weatherid]");
		SetWeather(targetweatherid);
	}
	if(strcmp(cmd, "/search", true) == 0) 
	{
		new targetid;
		if(sscanf(cmdtext[strlen("/search")+1], "i", targetid))
		return SendClientMessage(playerid, COLOR_WHITE, "USAGE: /search [key]");
		new mres = binarysearch(MoneyPickups,targetid,0,sizeof(MoneyPickups)-1);
		new astring[54];
		format(astring,sizeof(astring),"Index of key: %d",mres);
		new ares = binarysearch(ActorPickups,targetid,0,sizeof(ActorPickups)-1);
		SendClientMessage(playerid,COLOR_WHITE,astring);
		format(astring,sizeof(astring),"Index of key: %d",ares);
		SendClientMessage(playerid,COLOR_WHITE,astring);

	}
	if(strcmp(cmd, "/compare", true) == 0) 
	{
		HeapSort(MoneyPickups);
		HeapSort(ActorPickups);
		for(new i;i<sizeof(ActorPickups);i++)
		{
			new mres = binarysearch(MoneyPickups,ActorPickups[i],0,sizeof(MoneyPickups)-1);
			if(mres > -1)
			{
				SendClientMessage(playerid,COLOR_WHITE,"Match found! MoneyPickups contains values of ActorPickups");
				new tstring[144];
				format(tstring,sizeof(tstring),"MoneyPickups Val: %d Index: %d matchs ActorPickups Val: %d Index :%d",MoneyPickups[mres],mres,ActorPickups[i],i);
				SendClientMessage(playerid,-1,tstring);
				break;
				
			}
		}

	}
	if(strcmp(cmd, "/getplayerteam", true) == 0) 
	{
		new targetplayer;
		if(sscanf(cmdtext[strlen("/getplayerteam")+1], "i", targetplayer))
		return SendClientMessage(playerid, COLOR_WHITE, "USAGE: /getplayerteam [playeridid]");
		new pTeam[144];
		format(pTeam,sizeof(pTeam),"SERVER: Player %d is part of Team: %d",targetplayer,gPlayerTeamSelection[targetplayer]);
		strcat (pTeam,FIRST_TEAM_COLOR_TAG);
		SendClientMessage(playerid, -1, pTeam);
		SendClientMessage(playerid,COLOR_WHITE,pTeam);
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
		if(Warppowder[playerid] == 1)
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
		else
		{
			return SendClientMessage(playerid,COLOR_WHITE,"SERVER: You do not have Warppowder!");
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

	if (strcmp(cmdtext, "/saveveh", true)==0)
	{
		if(IsPlayerAdmin(playerid))
		{
			new col1= random(12);
			new col2 =random(12);
			new currentveh;
			currentveh = GetPlayerVehicleID(playerid);
			new Float:z_rot;
			new Float:vehx, Float:vehy, Float:vehz;
			GetVehiclePos(currentveh, vehx, vehy, vehz);
			GetVehicleZAngle(currentveh, z_rot);
			if(!IsPlayerInVehicle(playerid,currentveh))
			return SendClientMessage(playerid, COLOR_WHITE, "SERVER: You have to be inside of a vehicle!");
			new vehpostext[256];
			format(vehpostext, sizeof(vehpostext), "\nAddStaticVehicle(%d,%f, %f, %f, %f, %d,%d);\n", GetVehicleModel(currentveh),vehx, vehy, vehz,z_rot,col1,col2);
			SendClientMessage(playerid,COLOR_WHITE,"SERVER: Saved vehicle");
			new File:pos=fopen("customvehicles.txt", io_append);
			fwrite(pos, vehpostext);
			fclose(pos);
			if(!fexist("customvehicles.txt"))
			return SendClientMessage(playerid,COLOR_WHITE,"SERVER: customvehicles.txt not created! Maybe no R/W permission?");

			return 1;
		}
		else
		{
			return 0;
		}
	}
	if(strcmp(cmd, "/spec", true) == 0)
	{
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
	if(strcmp(cmd, "/dropmoney", true) == 0)
	{
		new dropval;
		if(sscanf(cmdtext[strlen("/dropmoney")+1], "i", dropval))
		return SendClientMessage(playerid, COLOR_WHITE, "USAGE: /dropmoney [value]");
		if(GetPlayerMoney(playerid) >= dropval && maxmoney <= 4000 && dropval > 0 && dropval <= 100000 && (maxmoney+(dropval/1000))<(4095-((dropval/1000)+maxmoney)) && dropval != 0 && !IsPlayerInAnyVehicle(playerid))
		{
			if(dropval%1000 == 0)
			{
				new moneydrop[32];
				new dropstring[32];
				format(dropstring,sizeof(dropstring),"SERVER: Dropped $%d",dropval);
				format(moneydrop,sizeof(moneydrop), "~g~ %d~y~$",dropval);
				GameTextForPlayer(playerid,moneydrop,1000,1);
				SendClientMessage(playerid, COLOR_WHITE, dropstring);
				GivePlayerMoney(playerid,-dropval);

				for(new i; i <dropval/1000; i++)
				{

					maxmoney = maxmoney +1;
					new Float:x, Float:y, Float:z;
					GetPlayerPos(playerid,x,y,z);

					new Float:ra1=((frandom(3)+x+0.5));
					new Float:ra2=((frandom(3)+y+0.5));
					new Float:ra3=((frandom(1)+z-0.8));
					
					CreatePickup(1212,19,ra1,ra2,ra3, 0);
				}

			}
			else
			{

				if(dropval%1000 != 0)
				{
					SendClientMessage(playerid, COLOR_WHITE, "SERVER: The desired value must be divisible by $1000");
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
			if((maxmoney+dropval/1000)>=4095 && dropval <= 100000)
			{
				new allowmoney[128];
				format(allowmoney,sizeof(allowmoney),"SERVER: Because of limitations you can not drop more than $%d right now!",(4095-maxmoney)*1000);
				SendClientMessage(playerid,COLOR_WHITE,allowmoney);
				printf("Player may only drop %d",(4095-maxmoney)*1000);
			}
		}

	}
	/*//if(strcmp(cmd, "/freeze", true) == 0)
	new target;	
	if(!sscanf(cmdtext[strlen("/freeze")+1], "u", target))	
	{
		if(IsPlayerAdmin(playerid))
		{
	
			
			//return SendClientMessage(playerid, COLOR_WHITE, "USAGE: /freeze [playerid]");
			if(!IsPlayerConnected(target))
			return SendClientMessage(playerid, COLOR_WHITE, "SERVER: Player not connected");
			TogglePlayerControllable(target,false);
		}	
	}
	else
	{
		return SendClientMessage(playerid, COLOR_WHITE, "USAGE: /freeze [playerid]");
		
	}*/
	if(strcmp(cmd, "/kill", true) == 0)
	{
		if(IsPlayerAdmin(playerid))
		{
			new targetplayer;
			if(sscanf(cmdtext[strlen("/kill")+1], "u", targetplayer))
			return SendClientMessage(playerid, COLOR_WHITE, "USAGE: /kill [playerid]");
			if(!IsPlayerConnected(targetplayer))
			return SendClientMessage(playerid, COLOR_WHITE, "SERVER: Player not connected");
			SetPlayerHealth(targetplayer,0.0);
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
			GivePlayerMoney(targetplayer,moneyamount);
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
	if(strcmp(cmd, "/getmoneycount", true) == 0)
	{
		new moneyvalstring[128];
		format(moneyvalstring,sizeof(moneyvalstring),"SERVER: $%d have been generated.%d money pickups have been created",moneyval,maxmoney);
		SendClientMessage(playerid,COLOR_WHITE,moneyvalstring);
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
			for(new k;k<sizeof(beds[]);k++)
			{
				new Float:dist = floatabs(GetDistance(pX[playerid],pY[playerid],pZ[playerid],beds[k][0],beds[k][1],beds[k][2]));
				{
					if(dist < 8)
					{
						new distancestring[128];
						format(distancestring,sizeof(distancestring),"SERVER: Distance of dropped bomb to closest bed: %fm",dist);
					}				
				}
			}
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
		if(PBomb[playerid] == -1 && PBombID[playerid] == 1 && IsPlayerInRangeOfPoint(playerid,100,pX[playerid],pY[playerid],pZ[playerid]))
		{
			PBombID[playerid]=0;
			PBomb[playerid]=0;
			CreateExplosion(pX[playerid],pY[playerid],pZ[playerid],7,10);
			CreateExplosion(pX[playerid],pY[playerid],pZ[playerid],9,10);
			SendClientMessage(playerid, COLOR_WHITE, "SERVER: Bomb detonated!");
			DestroyObject(BombObject[playerid]);
		}
		else if(!IsPlayerInRangeOfPoint(playerid,100,pX[playerid],pY[playerid],pZ[playerid]))
		{
			SendClientMessage(playerid, COLOR_WHITE, "SERVER: You can only detonate the bomb if your distance to it is less than 50m!");
		}
		else
		{
			return 0;
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
		format(statsstring2,sizeof(statsstring2),"Kills:%d Deaths:%d Ratio:%f Blown beds:%d Bombs detonated:%d",kills,deaths,kd,PlayerInfo[targetplayer][pBeds],PlayerInfo[targetplayer][pBombs]); 
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
	if(strcmp(cmdtext, "/blowup", true) == 0)
	{
		for(new i = 0; i < sizeof(beds); i++)
		{
			new Float:health;
			GetPlayerHealth(playerid,health);
			
			if(running !=1 && IsPlayerInRangeOfPoint(playerid,3.0,beds[i][0],beds[i][1],beds[i][2]) && health > 0 && !IsPlayerInAnyVehicle(playerid)) // In this script we deal with players only
			{
				if((i == 0 &&  gPlayerTeamSelection[playerid] == FIRST_TEAM) || (i == 1 &&  gPlayerTeamSelection[playerid] == SECOND_TEAM) || (i == 2 &&  gPlayerTeamSelection[playerid] == THIRD_TEAM)  || (i == 3 &&  gPlayerTeamSelection[playerid] == FOURTH_TEAM) || (i == 4 &&  gPlayerTeamSelection[playerid] == FIFTH_TEAM)  || (i == 5 &&  gPlayerTeamSelection[playerid] == SIXTH_TEAM))
				return SendClientMessage(playerid, COLOR_WHITE, "SERVER: You cannot blow up the bed of your own team!");  
				running = 1;
				SetPlayerScore(playerid,GetPlayerScore(playerid)+1);
				PlayerInfo[playerid][pBeds]=GetPlayerScore(playerid);
				SetTimer("BlowUpThisBed", bombtimer, false);
				if(i == 0 && BED_STATE_TEAM_ONE !=-1 )
				{
					BED_STATE_TEAM_ONE=-1;
					SendClientMessage(playerid, COLOR_WHITE, "SERVER: You are blowing up the bed of Team "#FIRST_TEAM_COLOR_TAG);
				}
				if(i == 1 && BED_STATE_TEAM_TWO !=-1 )
				{
					BED_STATE_TEAM_TWO=-1;
					SendClientMessage(playerid, COLOR_WHITE, "SERVER: You are blowing up the bed of Team "#SECOND_TEAM_COLOR_TAG);
				}
				
				#if defined TEAMSIZE
				#if TEAMSIZE >=3
				if(i == 2 && BED_STATE_TEAM_THREE !=-1 )
				{
					BED_STATE_TEAM_THREE=-1;
					SendClientMessage(playerid, COLOR_WHITE, "SERVER: You are blowing up the bed of Team "#THIRD_TEAM_COLOR_TAG);
				}
				#endif
				#endif
				#if defined TEAMSIZE
				#if TEAMSIZE >=4
				if(i == 3 && BED_STATE_TEAM_FOUR !=-1 )
				{
					BED_STATE_TEAM_FOUR=-1;
					SendClientMessage(playerid, COLOR_WHITE, "SERVER: You are blowing up the bed of Team "#FOURTH_TEAM_COLOR_TAG);
				}
				#endif
				#endif
				#if defined TEAMSIZE
				#if TEAMSIZE >=5
				if(i == 4 && BED_STATE_TEAM_FIVE !=-1 )
				{
					BED_STATE_TEAM_FIVE=-1;
					SendClientMessage(playerid, COLOR_WHITE, "SERVER: You are blowing up the bed of Team "#FIFTH_TEAM_COLOR_TAG);
				}
				#endif
				#endif
				#if defined TEAMSIZE
				#if TEAMSIZE >=6
				if(i == 5 && BED_STATE_TEAM_SIX !=-1 )
				{
					BED_STATE_TEAM_SIX=-1;
					SendClientMessage(playerid, COLOR_WHITE, "SERVER: You are blowing up the bed of Team "#SIXTH_TEAM_COLOR_TAG);
				}
				#endif
				#endif
				PBeds[playerid]+=1;
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
				new Float:dist = GetDistance(beds[i][0],beds[i][1],beds[i][2],x1,y1,z1);
				printf("Distance to closest bed: %f, Index: %d", dist, i);
				
				

				return 1;

			}



		}
		return 0;

	}
	return 1;

}
public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	printf("OnPlayerKeyStateChange called by %d",playerid);
	if(PRESSED(KEY_YES) && PBomb[playerid] == -1 && PBombID[playerid] == 1 && IsPlayerInRangeOfPoint(playerid,100,pX[playerid],pY[playerid],pZ[playerid]))
	{
		PlayerInfo[playerid][pBombs]++;
		PBombID[playerid]=0;
		PBomb[playerid]=0;
		CreateExplosion(pX[playerid],pY[playerid],pZ[playerid],7,10);
		CreateExplosion(pX[playerid],pY[playerid],pZ[playerid],9,10);
		SendClientMessage(playerid, COLOR_WHITE, "SERVER: Bomb detonated!");
		PlayerPlaySound(playerid, 21001, 0, 0, 0);
		DestroyObject(BombObject[playerid]);
		for(new i = 0; i < sizeof(beds[]); i++)
		{
			new Float:health;
			GetPlayerHealth(playerid,health);
			new Float:dist = GetDistance(beds[i][0],beds[i][1],beds[i][2],pX[playerid],pY[playerid],pZ[playerid]);
			if(dist < 4)
			{
				running = 1;
				if(!(i == 0 &&  gPlayerTeamSelection[playerid] == FIRST_TEAM) || !(i == 1 &&  gPlayerTeamSelection[playerid] == THIRD_TEAM) || !(i == 2 &&  gPlayerTeamSelection[playerid] == SECOND_TEAM))
				PBeds[playerid]+=1;	
				

				if(i == 0 && BED_STATE_TEAM_ONE !=-1 )
				{
					BED_STATE_TEAM_ONE=-1;
					SendClientMessage(playerid, COLOR_WHITE, "SERVER: You blew up the bed of Team "#FIRST_TEAM_COLOR_TAG);
				}
				if(i == 1 && BED_STATE_TEAM_TWO !=-1 )
				{
					BED_STATE_TEAM_THREE=-1;
					SendClientMessage(playerid, COLOR_WHITE, "SERVER: You blew up the bed of Team "#SECOND_TEAM_COLOR_TAG);
				}
				
				#if defined TEAMSIZE
				#if TEAMSIZE >=3
				if(i == 2 && BED_STATE_TEAM_THREE !=-1 )
				{
					BED_STATE_TEAM_TWO=-1;
					SendClientMessage(playerid, COLOR_WHITE, "SERVER: You blew up the bed of Team "#THIRD_TEAM_COLOR_TAG);
				}
				#endif
				#endif
				#if defined TEAMSIZE
				#if TEAMSIZE >=4
				if(i == 3 && BED_STATE_TEAM_FOUR !=-1 )
				{
					BED_STATE_TEAM_FOUR=-1;
					SendClientMessage(playerid, COLOR_WHITE, "SERVER: You blew up the bed of Team "#FOURTH_TEAM_COLOR_TAG);
				}
				#endif
				#endif
				#if defined TEAMSIZE
				#if TEAMSIZE >=5
				if(i == 4 && BED_STATE_TEAM_FIVE !=-1 )
				{
					BED_STATE_TEAM_FIVE=-1;
					SendClientMessage(playerid, COLOR_WHITE, "SERVER: You blew up the bed of Team "#FIFTH_TEAM_COLOR_TAG);
				}
				#endif
				#endif
				#if defined TEAMSIZE
				#if TEAMSIZE >=6
				if(i == 5 && BED_STATE_TEAM_SIX !=-1 )
				{
					BED_STATE_TEAM_SIX=-1;
					SendClientMessage(playerid, COLOR_WHITE, "SERVER: You blew up the bed of Team "#SIXTH_TEAM_COLOR_TAG);
				}
				#endif
				#endif
				SetPlayerScore(playerid, GetPlayerScore(playerid) + 1);
				x1=pX[playerid];
				y1=pY[playerid];
				z1=pZ[playerid];
				ApplyAnimation(playerid, "BOMBER", "BOM_Plant_Loop", 4.0, 1, 0, 0, 1, 1);
				SetTimer("BlowUpThisBed", 130, false);
				
				
				

				return 1;
			}

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
	new moneyplayerid[145];
	new moneykillerid[145];
	new matediedstringgrey[128];
	new matediedstringred[128];
	new matediedstringblue[128];
	
	HideMenuForPlayer(ammunation,playerid);
	HideMenuForPlayer(pistols,playerid);
	HideMenuForPlayer(microsmg,playerid);
	HideMenuForPlayer(shotungs,playerid);
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

	if(BED_STATE_TEAM_ONE != 0 && gPlayerTeamSelection[playerid] == FIRST_TEAM)
	{
		format(matediedstringgrey,sizeof(matediedstringgrey),"SERVER: %s (%d) has been killed, watch out! %d players left in Team "#FIRST_TEAM_COLOR_TAG,pname,playerid,GetTeamCount(FIRST_TEAM)-1);
		SendClientMessageToAll(COLOR_WHITE,matediedstringgrey);
	}
	if(BED_STATE_TEAM_TWO != 0 && gPlayerTeamSelection[playerid] == SECOND_TEAM)
	{
		format(matediedstringred,sizeof(matediedstringred),"SERVER: %s (%d) has been killed, watch out! %d players left in Team "#SECOND_TEAM_COLOR_TAG,pname,playerid,GetTeamCount(SECOND_TEAM)-1);
		SendClientMessageToAll(COLOR_WHITE,matediedstringred);
	}
	#if defined TEAMSIZE
	#if TEAMSIZE >=3
	if(BED_STATE_TEAM_THREE != 0 && gPlayerTeamSelection[playerid] == THIRD_TEAM)
	{
		format(matediedstringblue,sizeof(matediedstringblue),"SERVER: %s (%d) has been killed, watch out! %d players left in Team "#THIRD_TEAM_COLOR_TAG,pname,playerid,GetTeamCount(THIRD_TEAM)-1);
		SendClientMessageToAll(COLOR_WHITE,matediedstringblue);
	}
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE >=4
	if(BED_STATE_TEAM_FOUR != 0 && gPlayerTeamSelection[playerid] == FOURTH_TEAM)
	{
		format(matediedstringgrey,sizeof(matediedstringgrey),"SERVER: %s (%d) has been killed, watch out! %d players left in Team "#FOURTH_TEAM_COLOR_TAG,pname,playerid,GetTeamCount(FOURTH_TEAM)-1);
		SendClientMessageToAll(COLOR_WHITE,matediedstringgrey);
	}
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE >=5
	if(BED_STATE_TEAM_FIVE != 0 && gPlayerTeamSelection[playerid] == FIFTH_TEAM)
	{
		format(matediedstringred,sizeof(matediedstringred),"SERVER: %s (%d) has been killed, watch out! %d players left in Team "#FIFTH_TEAM_COLOR_TAG,pname,playerid,GetTeamCount(FIFTH_TEAM)-1);
		SendClientMessageToAll(COLOR_WHITE,matediedstringred);
	}
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE ==6
	if(BED_STATE_TEAM_SIX != 0 && gPlayerTeamSelection[playerid] == SIXTH_TEAM)
	{
		format(matediedstringblue,sizeof(matediedstringblue),"SERVER: %s (%d) has been killed, watch out! %d players left in Team "#SIXTH_TEAM_COLOR_TAG,pname,playerid,GetTeamCount(SIXTH_TEAM)-1);
		SendClientMessageToAll(COLOR_WHITE,matediedstringblue);
	}
	#endif
	#endif
	new playercash;
	playercash = GetPlayerMoney(playerid);
	SendDeathMessage(killerid, playerid, reason); // Shows the kill in the killfeed
	gPlayerHasTeamSelected[playerid] = 0;
	TextDrawHideForPlayer(playerid,txtTimeDisp);
	playercash = GetPlayerMoney(playerid);
	ResetPlayerMoney(playerid);
	format(moneyplayerid,sizeof(moneyplayerid), "~r~ -%d~y~$",playercash);
	GameTextForPlayer(playerid,moneyplayerid,1000,1);
	GivePlayerMoney(killerid, playercash);
	LastMoney[playerid]=playercash;
	if(killerid != playerid && killerid != INVALID_PLAYER_ID)
	{
		format(moneykillerid,sizeof(moneykillerid), "~g~ %d~y~$",playercash);
		GameTextForPlayer(killerid,moneykillerid,1000,1);
	}
	
	
	ResetPlayerMoney(playerid);

	PKills[playerid] += 1;
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
		PlayerTextDrawSetString(playerid, TeamText[playerid],FIRST_TEAM_NAME_TAG);
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
		PlayerTextDrawSetString(playerid, TeamText[playerid],SECOND_TEAM_NAME_TAG);
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
		PlayerTextDrawSetString(playerid, TeamText[playerid],THIRD_TEAM_NAME_TAG);
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
		PlayerTextDrawSetString(playerid, TeamText[playerid],FOURTH_TEAM_NAME_TAG);
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
		PlayerTextDrawSetString(playerid, TeamText[playerid],FIFTH_TEAM_NAME_TAG);
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
		PlayerTextDrawSetString(playerid, TeamText[playerid],SIXTH_TEAM_NAME_TAG);
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
public BlowUpThisBed()
{
	CreateExplosion(x1, y1, z1, 7, 10.0);//'heavy' explosion close to bed
	CreateExplosion(x1, y1, z1, 9, 10.0);
	running=0;
	printf("Blow Up This Bed called");
	if(BED_STATE_TEAM_ONE == -1)
	{
		BED_STATE_TEAM_ONE = -2;
		SendTeamMessage(FIRST_TEAM,COLOR_WHITE,"SERVER: Sudden death! Keep running, your bed has been blown up! There is no respawn after death!");
		new adstring[64];
		format(adstring,sizeof(adstring),"SERVER: The bed of team %s {FFFFFF}blew up!",FIRST_TEAM_COLOR_TAG);
		SendClientMessageToAll(-1,adstring);
		GangZoneHideForAll(TEAM_ZONE_ONE);
		printf("Violet");

	}
	if(BED_STATE_TEAM_TWO == -1)
	{
		BED_STATE_TEAM_TWO = -2;
		SendTeamMessage(SECOND_TEAM,COLOR_WHITE,"SERVER: Sudden death! Keep running, your bed has been blown up! There is no respawn after death!");
		new adstring[64];
		format(adstring,sizeof(adstring),"SERVER: The bed of team %s {FFFFFF}blew up!",SECOND_TEAM_COLOR_TAG);
		SendClientMessageToAll(-1,adstring);
		GangZoneHideForAll(TEAM_ZONE_TWO);
		printf("Yellow");
	}
	#if defined TEAMSIZE
	#if TEAMSIZE >= 3
	if(BED_STATE_TEAM_THREE == -1)
	{
		BED_STATE_TEAM_THREE = -2;
		SendTeamMessage(THIRD_TEAM,COLOR_WHITE,"SERVER: Sudden death! Keep running, your bed has been blown up! There is no respawn after death!");
		new adstring[64];
		format(adstring,sizeof(adstring),"SERVER: The bed of team %s {FFFFFF}blew up!",THIRD_TEAM_COLOR_TAG);
		SendClientMessageToAll(-1,adstring);
		GangZoneHideForAll(TEAM_ZONE_THREE);
		printf("Brown");
	}
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE >= 4
	if(BED_STATE_TEAM_FOUR	== -1)
	{
		BED_STATE_TEAM_FOUR = -2;
		SendTeamMessage(FOURTH_TEAM,COLOR_WHITE,"SERVER: Sudden death! Keep running, your bed has been blown up! There is no respawn after death!");
		new adstring[64];
		format(adstring,sizeof(adstring),"SERVER: The bed of team %s {FFFFFF}blew up!",FOURTH_TEAM_COLOR_TAG);
		SendClientMessageToAll(-1,adstring);
		GangZoneHideForAll(TEAM_ZONE_FOUR);
		printf("Violet");

	}
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE >= 5
	if(BED_STATE_TEAM_FIVE == -1)
	{
		BED_STATE_TEAM_FIVE = -2;
		SendTeamMessage(FIFTH_TEAM,COLOR_WHITE,"SERVER: Sudden death! Keep running, your bed has been blown up! There is no respawn after death!");
		new adstring[64];
		format(adstring,sizeof(adstring),"SERVER: The bed of team %s {FFFFFF}blew up!",FIFTH_TEAM_COLOR_TAG);
		SendClientMessageToAll(-1,adstring);
		GangZoneHideForAll(TEAM_ZONE_FIVE);
		printf("Yellow");
	}
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE == 6
	if(BED_STATE_TEAM_SIX == -1)
	{
		BED_STATE_TEAM_SIX = -2;
		SendTeamMessage(SIXTH_TEAM,COLOR_WHITE,"SERVER: Sudden death! Keep running, your bed has been blown up! There is no respawn after death!");
		new adstring[64];
		format(adstring,sizeof(adstring),"SERVER: The bed of team %s {FFFFFF}blew up!",SIXTH_TEAM_COLOR_TAG);
		SendClientMessageToAll(-1,adstring);
		GangZoneHideForAll(TEAM_ZONE_SIX);
		printf("Brown");
	}
	#endif
	#endif
}

public OnPlayerSpawn(playerid)
{

	if(IsPlayerNPC(playerid)) return 1;
	if(GetTeamCount(FIRST_TEAM) == 0 && GetTeamCount(THIRD_TEAM) == 0 && GetTeamCount(SECOND_TEAM) > 0 && BED_STATE_TEAM_TWO != 0)
	{
		printf("SPAWN: Case 1: %d",playerid);

		for(new i;i<MAX_PLAYERS;i++)
		{
			SendClientMessageEx(i,COLOR_WHITE,"SERVER: All remaining teams have been wiped. Team %s {FFFFFF}has won the game!",FIRST_TEAM_COLOR_TAG);
		}
		printf("Total game time: %d",totaltime);
		SendRconCommand("gmx");

	}
	
	if(GetTeamCount(FIRST_TEAM) > 0 && GetTeamCount(THIRD_TEAM) == 0 && GetTeamCount(SECOND_TEAM) == 0 && BED_STATE_TEAM_ONE != 0)
	{

		
		for(new i;i<MAX_PLAYERS;i++)
		{
			SendClientMessageEx(i,COLOR_WHITE,"SERVER: All remaining teams have been wiped. Team %s {FFFFFF}has won the game!",THIRD_TEAM_COLOR_TAG);
		}
		printf("Total game time: %d",totaltime);
		printf("SPAWN: Case 3: %d",playerid);

		SendRconCommand("gmx");
	}
	#if defined TEAMSIZE
	#if TEAMSIZE >= 3
	if(GetTeamCount(FIRST_TEAM) == 0 && GetTeamCount(THIRD_TEAM) > 0 && GetTeamCount(SECOND_TEAM) == 0 && BED_STATE_TEAM_THREE != 0)
	{	
		for(new i;i<MAX_PLAYERS;i++)
		{
			SendClientMessageEx(i,COLOR_WHITE,"SERVER: All remaining teams have been wiped. Team %s {FFFFFF}has won the game!",SECOND_TEAM_COLOR_TAG);
		}
		printf("Total game time: %d",totaltime);
		printf("SPAWN: Case 2: %d",playerid);

		SendRconCommand("gmx");
	}
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE >= 3
	if(GetTeamCount(FIRST_TEAM) == 0 && GetTeamCount(THIRD_TEAM) == 0 && GetTeamCount(SECOND_TEAM) == 0 && BED_STATE_TEAM_ONE != 0 && BED_STATE_TEAM_THREE != 0 && BED_STATE_TEAM_TWO !=0)
	{

		SendClientMessageToAll(COLOR_WHITE,"SERVER: All teams have been wiped. No one has won the game!");
		SendRconCommand("gmx");
		printf("SPAWN: Case 4: %d",playerid);
	}
	#endif
	#endif
	if(BED_STATE_TEAM_ONE!=0 && FIRST_TEAM == gPlayerTeamSelection[playerid])
	{
		SendClientMessageEx(playerid,COLOR_WHITE,"SERVER: Unable to respawn. The bed of team %s {FFFFFF}already has been destroyed.",FIRST_TEAM_COLOR_TAG);
		SendClientMessage(playerid,COLOR_WHITE,"SERVER: Entering spectator mode..");
		SetPlayerCameraPos(playerid,243.2876,1802.5547,7.4141);
		SetPlayerCameraLookAt(playerid,243.1261,1805.2798,8.3794);
		gPlayerTeamSelection[playerid] =TEAM_SPECTATOR;
		SetPlayerColor(playerid,COLOR_WHITE);
		printf("SPAWN: Case UN: %d",playerid);


	}
	if(BED_STATE_TEAM_TWO!=0 && SECOND_TEAM == gPlayerTeamSelection[playerid])
	{
		SendClientMessageEx(playerid,COLOR_WHITE,"SERVER: Unable to respawn. The bed of team %s {FFFFFF}already has been destroyed.",SECOND_TEAM_COLOR_TAG);
		SendClientMessage(playerid,COLOR_WHITE,"SERVER: Entering spectator mode..");
		SetPlayerCameraPos(playerid,243.2876,1802.5547,7.4141);
		SetPlayerCameraLookAt(playerid,243.1261,1805.2798,8.3794);
		//TogglePlayerSpectating(playerid, 1);
		//PlayerSpectatePlayer(playerid, spectatekillerid);
		gPlayerTeamSelection[playerid] =TEAM_SPECTATOR;
		SetPlayerColor(playerid,COLOR_WHITE);
		printf("SPAWN: Case UN: %d",playerid);
	}
	#if defined TEAMSIZE
	#if TEAMSIZE >= 3
	if(BED_STATE_TEAM_THREE!=0 && THIRD_TEAM == gPlayerTeamSelection[playerid])
	{
		SendClientMessageEx(playerid,COLOR_WHITE,"SERVER: Unable to respawn. The bed of team %s {FFFFFF}already has been destroyed.",THIRD_TEAM_COLOR_TAG);
		SendClientMessage(playerid,COLOR_WHITE,"SERVER: Entering spectator mode..");
		SetPlayerCameraPos(playerid,243.2876,1802.5547,7.4141);
		SetPlayerCameraLookAt(playerid,243.1261,1805.2798,8.3794);
		//TogglePlayerSpectating(playerid, 1);
		//PlayerSpectatePlayer(playerid, spectatekillerid);
		gPlayerTeamSelection[playerid] =TEAM_SPECTATOR;
		SetPlayerColor(playerid,COLOR_WHITE);
		printf("SPAWN: Case UN: %d",playerid);
	}
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE >= 4
	if(BED_STATE_TEAM_FOUR!=0 && FOURTH_TEAM == gPlayerTeamSelection[playerid])
	{
		SendClientMessageEx(playerid,COLOR_WHITE,"SERVER: Unable to respawn. The bed of team %s {FFFFFF}already has been destroyed.",FOURTH_TEAM_COLOR_TAG);
		SendClientMessage(playerid,COLOR_WHITE,"SERVER: Entering spectator mode..");
		SetPlayerCameraPos(playerid,243.2876,1802.5547,7.4141);
		SetPlayerCameraLookAt(playerid,243.1261,1805.2798,8.3794);
		//TogglePlayerSpectating(playerid, 1);
		//PlayerSpectatePlayer(playerid, spectatekillerid);
		gPlayerTeamSelection[playerid] =TEAM_SPECTATOR;
		SetPlayerColor(playerid,COLOR_WHITE);
		printf("SPAWN: Case UN: %d",playerid);
	}
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE >= 5
	if(BED_STATE_TEAM_FIVE!=0 && FIFTH_TEAM == gPlayerTeamSelection[playerid])
	{
		SendClientMessageEx(playerid,COLOR_WHITE,"SERVER: Unable to respawn. The bed of team %s {FFFFFF}already has been destroyed.",FIFTH_TEAM_COLOR_TAG);
		SendClientMessage(playerid,COLOR_WHITE,"SERVER: Entering spectator mode..");
		SetPlayerCameraPos(playerid,243.2876,1802.5547,7.4141);
		SetPlayerCameraLookAt(playerid,243.1261,1805.2798,8.3794);
		//TogglePlayerSpectating(playerid, 1);
		//PlayerSpectatePlayer(playerid, spectatekillerid);
		gPlayerTeamSelection[playerid] =TEAM_SPECTATOR;
		SetPlayerColor(playerid,COLOR_WHITE);
		printf("SPAWN: Case UN: %d",playerid);
	}
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE >= 6
	if(BED_STATE_TEAM_SIX!=0 && SIXTH_TEAM == gPlayerTeamSelection[playerid])
	{
		SendClientMessageEx(playerid,COLOR_WHITE,"SERVER: Unable to respawn. The bed of team %s {FFFFFF}already has been destroyed.",SIXTH_TEAM_COLOR_TAG);
		SendClientMessage(playerid,COLOR_WHITE,"SERVER: Entering spectator mode..");
		SetPlayerCameraPos(playerid,243.2876,1802.5547,7.4141);
		SetPlayerCameraLookAt(playerid,243.1261,1805.2798,8.3794);
		//TogglePlayerSpectating(playerid, 1);
		//PlayerSpectatePlayer(playerid, spectatekillerid);
		gPlayerTeamSelection[playerid] =TEAM_SPECTATOR;
		SetPlayerColor(playerid,COLOR_WHITE);
		printf("SPAWN: Case UN: %d",playerid);
	}
	#endif
	#endif

	TextDrawShowForPlayer(playerid,txtTimeDisp);
	PlayerPlaySound(playerid,1188,0,0,0);
	SetPlayerWorldBounds(playerid,MAP_WORLDBOUNDS[0][0], MAP_WORLDBOUNDS[0][1], MAP_WORLDBOUNDS[0][2], MAP_WORLDBOUNDS[0][3]);

	new randSpawn = 0;

	
	SetPlayerVirtualWorld(playerid,0);
	TogglePlayerClock(playerid,0);
	ResetPlayerMoney(playerid);

	if(FIRST_TEAM == gPlayerTeamSelection[playerid]) {
		randSpawn = random(sizeof(gSpawnsTeam_TEAM_ONE));
		SetPlayerPos(playerid,gSpawnsTeam_TEAM_ONE[randSpawn][0],gSpawnsTeam_TEAM_ONE[randSpawn][1],gSpawnsTeam_TEAM_ONE[randSpawn][2]);
		SetPlayerFacingAngle(playerid,gSpawnsTeam_TEAM_ONE[randSpawn][3]);
	}
	#if defined TEAMSIZE
	#if TEAMSIZE >= 2
	else if(SECOND_TEAM == gPlayerTeamSelection[playerid]) {
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
	else if(THIRD_TEAM == gPlayerTeamSelection[playerid]) {
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
	else if(FOURTH_TEAM == gPlayerTeamSelection[playerid]) {
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
	else if(FIFTH_TEAM == gPlayerTeamSelection[playerid]) {
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
	else if(SIXTH_TEAM == gPlayerTeamSelection[playerid]) {
		randSpawn = random(sizeof(gSpawnsTeam_TEAM_SIX));
		SetPlayerPos(playerid,
		gSpawnsTeam_TEAM_SIX[randSpawn][0],
		gSpawnsTeam_TEAM_SIX[randSpawn][1],
		gSpawnsTeam_TEAM_SIX[randSpawn][2]);
		SetPlayerFacingAngle(playerid,gSpawnsTeam_TEAM_SIX[randSpawn][3]);
	}
	#endif
	#endif
	GangZoneShowForPlayer(playerid, TEAM_ZONE_ONE, COLOR_TEAM_ONE);
	GangZoneShowForPlayer(playerid, TEAM_ZONE_TWO, COLOR_TEAM_TWO);
	#if defined TEAMSIZE
	#if TEAMSIZE >= 3
	GangZoneShowForPlayer(playerid, TEAM_ZONE_THREE, COLOR_TEAM_THREE);
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE >= 4
	GangZoneShowForPlayer(playerid, TEAM_ZONE_FOUR, COLOR_TEAM_FOUR);
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE >= 5
	GangZoneShowForPlayer(playerid, TEAM_ZONE_FIVE, COLOR_TEAM_FIVE);
	#endif
	#endif
	#if defined TEAMSIZE
	#if TEAMSIZE == 6
	GangZoneShowForPlayer(playerid, TEAM_ZONE_SIX, COLOR_TEAM_SIX);
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
	case 0:
		{
			SetPlayerInterior(playerid,TEAM_ONE_INTERIOR);
		}
	case 1:
		{
			SetPlayerInterior(playerid,TEAM_TWO_INTERIOR);
		}
		#if defined TEAMSIZE
		#if TEAMSIZE >= 3
	case 2:
		{
			SetPlayerInterior(playerid,TEAM_THREE_INTERIOR);
		}
		#endif
		#endif
		#if defined TEAMSIZE
		#if TEAMSIZE >= 4
	case 3:
		{
			SetPlayerInterior(playerid,TEAM_FOUR_INTERIOR);
		}
		#endif
		#endif
		#if defined TEAMSIZE
		#if TEAMSIZE >= 5
	case 4:
		{
			SetPlayerInterior(playerid,TEAM_FIVE_INTERIOR);
		}
		#endif
		#endif
		#if defined TEAMSIZE
		#if TEAMSIZE >= 6
	case 5:
		{
			SetPlayerInterior(playerid,TEAM_SIX_INTERIOR);
		}
		#endif
		#endif
	default:
		{
			printf("Something went wrong..");
		}
	}


	return 1; 
}

public TEAM_MONEY()//Brown
{
	for (new m=0;m<sizeof(MoneySpawns)-1;m++)
	{
		GenerateRandomPickup(1212,19,MoneySpawns[m][0],MoneySpawns[m][1],MoneySpawns[m][2],MoneySpawns[m][3],MoneySpawns[m][4],MoneySpawns[m][5],0);
	}
	printf("Generated Pickup");
}

//----------------------------------------------------------


public MONEY_MAIN()//MAIN
{
	
	GenerateRandomPickup(1212,19,MoneySpawns[sizeof(MoneySpawns)-1][0],MoneySpawns[sizeof(MoneySpawns)-1][1],MoneySpawns[sizeof(MoneySpawns)-1][2],MoneySpawns[sizeof(MoneySpawns)-1][3],MoneySpawns[sizeof(MoneySpawns)-1][4],MoneySpawns[sizeof(MoneySpawns)-1][5],0);
}	


stock ResetPlayerData(playerid)
{
	
	PStealth[playerid]=0;
	Warppowder[playerid]=0;
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
