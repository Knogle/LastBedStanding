// Southern Bone County

//Color tags for textdraw use
#define FIRST_TEAM_COLOR_TAG "{8B4513}BROWN"
#define SECOND_TEAM_COLOR_TAG "{00A86B}JADE"
#define THIRD_TEAM_COLOR_TAG "{808000}OLIVE"
#define FOURTH_TEAM_COLOR_TAG "{7FFFD4}AQUA"

#define FIRST_TEAM_NAME_TAG "BROWN"
#define SECOND_TEAM_NAME_TAG "JADE"
#define THIRD_TEAM_NAME_TAG "OLIVE"
#define FOURTH_TEAM_NAME_TAG "AQUA"


#define TEAM_ONE_SEL_INTERIOR 0
#define TEAM_TWO_SEL_INTERIOR 0
#define TEAM_THREE_SEL_INTERIOR 0
#define TEAM_FOUR_SEL_INTERIOR 0

#define TEAM_ONE_INTERIOR 0
#define TEAM_TWO_INTERIOR 0
#define TEAM_THREE_INTERIOR 0
#define TEAM_FOUR_INTERIOR 0


// Team colors
#define COLOR_TEAM_ONE 0x8B451355 //Brown
#define COLOR_TEAM_TWO 0x00A86B55 //Jade
#define COLOR_TEAM_THREE 0x80800055 //Olive
#define COLOR_TEAM_FOUR 0x7FFFD455 //Aqua

// TD Colors
#define COLOR_TEAM_ONE_TD 0x8B4513FF //Brown
#define COLOR_TEAM_TWO_TD 0x00A86BFF //Jade
#define COLOR_TEAM_THREE_TD 0x808000FF //Olive
#define COLOR_TEAM_FOUR_TD 0x7FFFD4FF //Aqua

#define COLOR_TEAM_ONE_STEALTH 0x8B451300 //Brown
#define COLOR_TEAM_TWO_STEALTH 0x00A86B00 //Jade
#define COLOR_TEAM_THREE_STEALTH 0x80800000 //Olive
#define COLOR_TEAM_FOUR_STEALTH 0x7FFFD400 //Aqua



//Where the teams are located..
#define FIRST_TEAM_LOCATION "Northstar Rock"
#define SECOND_TEAM_LOCATION "Red County"
#define THIRD_TEAM_LOCATION "Fern Ridge"
#define FOURTH_TEAM_LOCATION "Fisher's Lagoon"

#define TEAMSIZE 4


stock const Float:Center[][]={
{1916.4387,-552.0197,22.2185}	
};

stock const Float:GlobalActors[][]={
	{2163.4744,-103.3103,2.7500,31.9812}, // shopaqua
	{2356.8645,-648.4787,128.0547,269.8268}, // shopbrown
	{2040.5349,-497.8884,73.3781,356.2843}, // shop 
	{1515.0361,11.7211,24.1406,282.6035}//shop Olive
};

stock const Float:GlobalZones[][]={
	{2300.542, -700.6725, 2417.32, -572.2159},//Brown
	{1961.883, -560.538, 2090.34, -326.9805},//Jade	
	{1494.768, -58.38938, 1611.547, 105.1009},//Olive
	{2066.984, -198.5239, 2300.542, -11.67788}//Aqua
};

stock const Float:PlayerInfoPickups[][]={
	{0.0,0.0,0.0}

};

stock const Float:MoneySpawns[][]={	
	{2353.2810,2349.7834,-646.6490,-658.7381,128.0547,127.8547},//Brown
	{2052.2373,2043.2620,-483.5109,-498.2212,73.3781,73.0781},//Jade
	{1550.3518,1537.9893,20.5647,12.5971,24.1475,23.1475},//Olive
	{2151.9563,2148.9758,-93.4575,-101.2074,2.8667,2.7667},//Aqua
	{1946.9860,1926.9181,-529.0431,-553.4490,19.5108,19.2362}//Main
};

stock const Float:MAP_WORLDBOUNDS[][]={
	{ 2826.046, 1132.754, 338.6584, -852.4849}
};

stock const Float:ClassSel_SetupTeamTEAM_POS[][]={
	{2399.2114,-674.9772,162.7818},//Brown
	{2059.9026,-464.9646,78.6582},//Jade
	{1576.6863,-41.1048,35.1455},//Olive
	{2177.8340,-110.6567,39.1880}//Aqua
	
};

stock const Float:ClassSel_SetupTeamTEAM_LOOK_AT[][]={
	{2344.9797,-641.9152,128.8911},//Brown
	{2027.1233,-525.1686,78.4953},//Jade
	{-343.7542,1593.1016,138.3667},//Olive
	{2114.3147,-95.4592,2.1974}//Aqua
};


stock const Float:beds[][] = {
	{2351.1406,-643.7385,128.9661},//Brown
	{2047.0985,-496.8957,74.0661},//Jade
	{1568.5222,31.8768,24.1641},//Olive
	{2148.3555,-98.9024,3.3661}//Aqua
};

stock const Float:gSpawnsTeam_TEAM_ONE[][] = {//Brown
	{2419.4351,-682.1667,126.4604,91.5618}, // brown1
	{2336.7073,-645.0444,129.3277,276.4068}, // brown2
	{2381.3049,-620.0992,125.0279,168.9325}, // brown3
	{2369.7175,-641.4078,128.0849,147.3123}, // brown4
	{2348.7217,-666.0512,129.9604,355.6576}, // brown5
	{2346.8894,-650.2871,128.0547,77.7517} // brown6

};

stock const Float:gSpawnsTeam_TEAM_TWO[][] = {//Jade
	{2042.1571,-470.3479,75.0643,179.5626}, // green1
	{2006.7892,-501.8208,74.7671,99.0352}, // green2
	{2046.0935,-550.7394,79.2145,38.5847}, // green3
	{2051.6213,-516.4318,78.0694,353.7776}, // green4
	{2061.0095,-493.2294,72.4050,85.8985}, // green5
	{2044.8813,-495.1483,73.3781,350.9576}, // green6
	{2041.1361,-484.2428,73.3781,174.8860} // green7
};

stock const Float:gSpawnsTeam_TEAM_THREE[][] = {//Olive
	{1575.4523,42.3339,25.0293,179.2259}, // olive1
	{1584.0243,-18.5926,19.4806,170.1392}, // olive2
	{1548.3904,-31.1425,21.3167,359.9976}, // olive3
	{1523.2516,-10.2966,22.9768,34.9693}, // olive4
	{1542.3862,14.6470,24.1330,2.5042}, // olive5
	{1525.8087,35.7813,24.2712,189.8559} // olive6
};

stock const Float:gSpawnsTeam_TEAM_FOUR[][] = {//Aqua
	{2104.5574,-103.3114,2.1882,126.2954}, // aqua1
	{2127.3008,-87.6152,1.9985,125.0419}, // aqua2
	{2150.9348,-87.3989,2.7279,112.5085}, // aqua3
	{2133.7456,-41.6460,2.7746,179.5624}, // aqua4
	{2183.7537,-122.9832,3.7410,56.7346}, // aqua5
	{2145.9460,-116.4935,0.6673,52.3479}, // aqua6
	{2157.2666,-28.8018,10.7600,174.5491}, // aqua7
	{2150.7722,-97.6364,2.8250,123.7886} // aqua8
};