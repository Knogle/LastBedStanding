// Southern Flint County

//Green done
//Olive done
//Color tags for textdraw use
#define FIRST_TEAM_COLOR_TAG "{F5F5DC}BEIGE"
#define SECOND_TEAM_COLOR_TAG "{4B0082}INDIGO"
#define THIRD_TEAM_COLOR_TAG "{29AB87}GREEN"
#define FOURTH_TEAM_COLOR_TAG "{808000}OLIVE"
#define FIFTH_TEAM_COLOR_TAG "{AFAFAF}GREY"
#define SIXTH_TEAM_COLOR_TAG "{8B4513}BROWN"

#define FIRST_TEAM_NAME_TAG "BEIGE"
#define SECOND_TEAM_NAME_TAG "INDIGO"
#define THIRD_TEAM_NAME_TAG "GREEN"
#define FOURTH_TEAM_NAME_TAG "OLIVE"
#define FIFTH_TEAM_NAME_TAG "GREY"
#define SIXTH_TEAM_NAME_TAG "BROWN"

#define TEAM_ONE_SEL_INTERIOR 0
#define TEAM_TWO_SEL_INTERIOR 0
#define TEAM_THREE_SEL_INTERIOR 0
#define TEAM_FOUR_SEL_INTERIOR 0
#define TEAM_FIVE_SEL_INTERIOR 0
#define TEAM_SIX_SEL_INTERIOR 0

#define TEAM_ONE_INTERIOR 0
#define TEAM_TWO_INTERIOR 0
#define TEAM_THREE_INTERIOR 0
#define TEAM_FOUR_INTERIOR 0
#define TEAM_FIVE_INTERIOR 0
#define TEAM_SIX_INTERIOR 0

// Team colors
#define COLOR_TEAM_ONE 0xF5F5DC55 //Beige
#define COLOR_TEAM_TWO 0x4B008255 //Indigo
#define COLOR_TEAM_THREE 0x29AB8755 //Green
#define COLOR_TEAM_FOUR 0x80800055 //Olive
#define COLOR_TEAM_FIVE 0xAFAFAF55 //Grey
#define COLOR_TEAM_SIX 0x8B451355 //Brown

// TD Colors
#define COLOR_TEAM_ONE_TD 0xF5F5DCFF //Beige
#define COLOR_TEAM_TWO_TD 0x4B0082FF //Indigo
#define COLOR_TEAM_THREE_TD 0x29AB87FF //Green
#define COLOR_TEAM_FOUR_TD 0x808000FF //Olive
#define COLOR_TEAM_FIVE_TD 0xAFAFAFFF //Grey
#define COLOR_TEAM_SIX_TD 0x8B4513FF //Brown

#define COLOR_TEAM_ONE_STEALTH 0xF5F5DC00 //Beige
#define COLOR_TEAM_TWO_STEALTH 0x4B008200 //Indigo
#define COLOR_TEAM_THREE_STEALTH 0x29AB8700 //Green
#define COLOR_TEAM_FOUR_STEALTH 0x80800000 //Olive
#define COLOR_TEAM_FIVE_STEALTH 0xAFAFAF00 //Grey
#define COLOR_TEAM_SIX_STEALTH 0x8B451300 //Brown



//Where the teams are located..
#define FIRST_TEAM_LOCATION "Back o' Beyond"
#define SECOND_TEAM_LOCATION "Flint Range"
#define THIRD_TEAM_LOCATION "Leafy Hollow"
#define FOURTH_TEAM_LOCATION "Whetstone Farm"
#define FIFTH_TEAM_LOCATION "Angle Pine"
#define SIXTH_TEAM_LOCATION "Flint Intersection"

#define TEAMSIZE 6


stock const Float:Center[][]={
{-747.384, -1996.917, 15.0}	
};

stock const Float:GlobalActors[][]={
	{-266.8388,-2213.5327,29.0420,116.4090}, // Beige
	{-1059.9354,-1205.5963,129.2188,269.6071}, // Indigo
	{-1112.4196,-1675.1364,76.3672,4.8145}, //Green
	{-1447.7645,-1523.2113,101.7578,268.3304},//Olive
	{-23.1916,-57.3207,1003.5469,2.8201},//Grey
	{-384.0908,-1438.7521,26.3203,268.6201},//Brown1
	{-575.6973,-1484.1497,10.5950,17.3712} //Brown2
};

stock const Float:GlobalZones[][]={
	{-362.0141, -2242.152, -210.2018, -2090.34},//Beige
	{-1296.244, -1179.465, -887.5186, -864.1628},//Indigo	
	{-1342.956, -1728.326, -1039.331, -1494.768},//Green
	{-1483.09, -1646.58, -1366.311, -1413.023},//Olive	
	{-1669.936, -2802.69, -1471.412, -2627.522}, //Grey
	{-607.2495, -1541.479, -385.3699, -1366.311} //Brown
};

stock const Float:PlayerInfoPickups[][]={
	{0.0,0.0,0.0}

};


stock const Float:MoneySpawns[][]={	
	{-281.5139,-288.0401,-2150.3667,-2150.9785,28.5469,28.2469},//Beige
	{-1137.5593,-1156.9827,-1133.3253,-1135.6730,129.2188,129.0188},//Indigo
	{-1097.7299,-1120.2886,-1615.2725,-1627.4698,76.3672,76.2672},//Green
	{-1417.7566,-1429.5403,-1476.2448,-1505.1635,101.6719,101.4719},//Olive
	{-1603.4077,-1608.7372,-2706.5579,-2720.9233,48.9453,48.5391},//Grey
	{-362.8252,-370.6793,-1416.7439,-1431.0531,25.7266,25.2266},// Brown
	{-758.1005,-780.4428,-1927.2896,-1995.5969,5.332,4.5090}//Main
};


stock const Float:MAP_WORLDBOUNDS[][]={
	{116.7788, -2580.811, -712.3504, -2931.147}
};

stock const Float:ClassSel_SetupTeamTEAM_POS[][]={
	{-194.5519,-2185.7693,55.2641},//Beige
	{-1161.4105,-1159.7512,160.8101},//Indigo
	{-1034.5977,-1697.3887,125.2309},//Green
	{-1386.9755,-1486.5024,133.6250},//Olive
	{-1560.7073,-2780.8167,81.7027},//Grey
	{-338.0222,-1402.7343,48.8300}// Brown
	
};

stock const Float:ClassSel_SetupTeamTEAM_LOOK_AT[][]={
	{-307.2654,-2179.0979,30.0899},//Beige
	{-1075.6758,-1205.5614,129.2188},//Indigo
	{-1117.2101,-1626.9524,82.0539},//Green
	{-1466.1613,-1526.1077,108.1411},//Olive
	{-1597.2477,-2694.0398,48.5391},//Grey
	{-399.8748,-1436.3070,29.6107}// Brown
};


stock const Float:beds[][] = {
	{-263.7699,-2184.5242,29.5661},//Beige
	{-1032.8093,-1182.1780,129.7661},//Indigo
	{-1111.1438,-1637.3147,77.0410},//Green
	{-1445.4979,-1576.8613,105.6072},//Olive
	{-1563.9752,-2716.8035,49.0745},//Grey
	{-367.0952,-1421.1415,30.2410}//Brown
};

stock const Float:gSpawnsTeam_TEAM_ONE[][] = {//Beige
	{-248.8334,-2237.6855,28.6040,31.1579}, // beige1
	{-275.1207,-2194.2092,28.7036,9.8510}, // beige2
	{-284.2011,-2142.8682,28.5469,119.8321}, // beige3
	{-291.3966,-2163.6677,28.6311,201.9262} // beige4

};

stock const Float:gSpawnsTeam_TEAM_TWO[][] = {//Indigo
	{-1022.5338,-1204.9518,129.2188,89.1488}, // indigo1
	{-1101.7047,-1104.0308,129.2188,177.1963}, // indigo2
	{-1180.3268,-1123.7338,129.2188,270.2571}, // indigo3
	{-1145.1595,-1168.4891,129.2188,267.7504}, // indigo4
	{-1077.8253,-1210.0328,129.2188,70.3720} // indigo5
};

stock const Float:gSpawnsTeam_TEAM_THREE[][] = {//Green
{-1123.9664,-1681.1921,76.3672,357.6312}, // barn1
	{-1107.8180,-1631.2953,76.3739,263.0270}, // barn2
	{-1117.3840,-1616.5784,76.3739,258.9536}, // barn3
	{-1023.0239,-1614.2980,76.3672,164.0127}, // barn4
	{-1009.2422,-1670.3392,76.3672,216.6532}, // barn5
	{-1031.8291,-1705.3724,77.6030,50.2716} // barn6
};

stock const Float:gSpawnsTeam_TEAM_FOUR[][] = {//Olive
	{-1401.7219,-1512.4167,101.8005,93.5120}, 
	{-1453.5708,-1576.5625,105.1250,268.6671}, 
	{-1411.8771,-1509.7389,101.6803,92.2821}, 
	{-1463.9825,-1511.2113,101.7513,269.6071}, 
	{-1441.2903,-1474.8058,101.7438,178.1129}, 
	{-1427.1520,-1586.9354,101.7578,89.1488} 
};

stock const Float:gSpawnsTeam_TEAM_FIVE[][] = {//Grey
	{-1535.9114,-2742.1367,48.5368,141.7457}, // grey1
	{-1562.2350,-2720.5669,48.5411,52.1315}, // grey2
	{-1571.6976,-2730.1567,48.5452,140.1790}, // grey3
	{-1606.7721,-2733.9941,48.7141,340.0641}, // grey4
	{-1640.9877,-2687.0300,48.5388,236.0365} // grey5
};

stock const Float:gSpawnsTeam_TEAM_SIX[][] = {//Brown
	{-367.0023,-1439.4407,25.7266,95.3685}, // brown1
	{-378.1396,-1419.7402,25.7266,1.3677}, // brown2
	{-576.1035,-1482.6141,10.7110,26.7479}, // brown3
	{-401.6338,-1442.2949,25.7209,263.3168}, // brown4
	{-374.6583,-1454.8862,25.7266,2.0177} // brown5
};