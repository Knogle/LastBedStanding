
//Color tags for textdraw use
#define FIRST_TEAM_COLOR_TAG "{AFAFAF}GREY"
#define SECOND_TEAM_COLOR_TAG "{AA3333}RED"
#define THIRD_TEAM_COLOR_TAG "{0000BB}BLUE"

#define FIRST_TEAM_NAME_TAG "GREY"
#define SECOND_TEAM_NAME_TAG "RED"
#define THIRD_TEAM_NAME_TAG "BLUE"


#define TEAM_ONE_SEL_INTERIOR 0
#define TEAM_TWO_SEL_INTERIOR 0
#define TEAM_THREE_SEL_INTERIOR 0

#define TEAM_ONE_INTERIOR 0
#define TEAM_TWO_INTERIOR 17
#define TEAM_THREE_INTERIOR 0


// Team colors
#define COLOR_TEAM_ONE 0xAFAFAF55 //Grey
#define COLOR_TEAM_TWO 0xAA333355 //Red
#define COLOR_TEAM_THREE 0x0000BB55 //Blue

// TD Colors
#define COLOR_TEAM_ONE_TD 0xAFAFAFFF //Grey
#define COLOR_TEAM_TWO_TD 0xAA3333FF //Red
#define COLOR_TEAM_THREE_TD 0x0000BBFF //Blue

#define COLOR_TEAM_ONE_STEALTH 0xAFAFAF00 //Grey
#define COLOR_TEAM_TWO_STEALTH 0xAA333300 //Red
#define COLOR_TEAM_THREE_STEALTH 0x0000BB00 //Blue



//Where the teams are located..
#define FIRST_TEAM_LOCATION "Area 51"
#define SECOND_TEAM_LOCATION "The Sherman Damn"
#define THIRD_TEAM_LOCATION "Verdant Meadows"

#define TEAMSIZE 3


stock const Float:Center[][]={
{-326.9609,2229.9983,43.4798}	
};

stock const Float:GlobalActors[][]={
	{275.1789,1857.4131,8.7578, 357.9678},//Grey
	{-957.0817,1931.4672,5.0000,265.5104},//Red
	{417.8459,2540.2747,10.0000,269.3721}//Blue
};

stock const Float:GlobalZones[][]={
	{-70.06725, 1658.258, 385.3699, 2125.373},//Grey
	{-934.23, 1868.46, -478.7929, 2148.729},//Red
	{-58.38938, 2370.609, 455.4371, 2674.233}//Blue
};

stock const Float:MoneySpawns[][]={	
	{253.7534,239.1062,1805.2826,1797.3673,7.9141,7.4141},//Grey
	{-949.3178,-958.2505,1914.4623,1907.0569,5.0000,4.7000},//Red
	{416.3385,412.4385,2536.9675,2530.5198,19.5839,19.3839},//Blue
	{-328.7060,-332.9677,2230.9785,2211.9797,42.4844,42.2844}//Main
};


stock const Float:MAP_WORLDBOUNDS[][]={
	{1086.0, -992.0 , 2944.0, 797.0}
};

stock const Float:ClassSel_SetupTeamTEAM_POS[][]={
	{136.5640,1937.6123,68.7587},//Grey
	{-761.1128,2108.5681,120.9924},//Red
	{264.1714,2442.7229,58.8193}//Blue
};

stock const Float:ClassSel_SetupTeamTEAM_LOOK_AT[][]={
	{227.9328,1860.6154,20.6406},//Grey
	{-579.4916,1931.1588,34.3891},//Red
	{371.2788,2536.0828,16.6359}//Blue
};

stock const Float:beds[][] = {
	{268.1000100,1881.5000000,-31.4000000},//Grey
	{-955.2665,1867.7350,5.6661},//Red
	{417.6000100,2536.2000000,8.9000000}//Blue
};

stock const Float:gSpawnsTeam_TEAM_ONE[][] = {//Grey
	{176.9377,1842.0581,17.6406,348.2309},
	{134.4745,1841.0414,17.6406,82.5685},
	{185.5383,1925.9650,17.7767,184.0660},
	{262.1147,1807.6191,25.4985,269.8967},
	{201.9949,1870.0468,13.1406,275.8736},
	{246.7988,1859.1957,14.0840,87.5818},
	{244.9222,1830.5947,7.5547,271.1736},
	{254.0137,1803.0964,7.4141,98.2354},
	{235.9334,1829.6948,7.4141,186.2829},
	{298.9872,1815.8099,4.7109,95.7521},
	{298.4415,1845.9108,7.7266,359.2679},
	{278.0970,1862.2509,8.7578,92.3521},
	{268.4646,1879.8407,-30.3906,272.2072},
	{275.2321,1963.1943,17.6406,265.3138},
	{293.7956,1830.1534,17.6481,179.7730}
};

stock const Float:gSpawnsTeam_TEAM_TWO[][] = {//Red
	{-944.7980,1915.1990,5.0000,181.6165}, 
	{-938.5313,1874.5580,5.0000,76.3355}, 
	{-957.5322,1866.9474,5.0000,272.1472}, 
	{-961.6938,1868.2571,9.0000,359.8580}, 
	{-946.0905,1922.2120,9.0061,3.3046}, 
	{-961.4773,1946.1267,9.0000,267.1337} 
};

stock const Float:gSpawnsTeam_TEAM_THREE[][] = {//Blue
	{414.0306,2534.3665,19.1484,183.3928}, 
	{347.8530,2548.9377,16.6874,181.8028}, 
	{381.9026,2602.8035,16.4844,191.5162}, 
	{293.0901,2557.3284,16.4630,183.6828}, 
	{319.3600,2440.8088,16.4864,348.4977}, 
	{400.2691,2452.9202,16.5000,358.2111}, 
	{426.2568,2530.7734,16.6257,92.2120} 
};