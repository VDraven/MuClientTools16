# MuClientTools16 by VD (Vic)
Mu Client Tools (Season 16 part 1.1)  
This project is using test files obtained from client Lgd_Test (1.19.46)
Server files & sources released by MuLegend can be found here : https://forum.ragezone.com/f197/release-legend-mu-client-amp-1195391/  


```cpp
/*
It might (or might not) work for files from older/newer versions. 
The ones want to modify the tools in order to fit with their data files should check:
	_wkey : used for calculating CRC of a data file.
	_xor3key : used for basic Dec/Enc data.
	_LANG_ZIP_PASSWORD_ : used for (un)zipping Lang.mpr file. 
		Note: For some older versions, the Lang.mpr can be open (w/o password) 
		by zip tools such as: Winrar, 7z,...
    // (Main Season 16 1.19.46) offset lang.mpr password : 0x0163BEBC
    
	Client Data Struct: Sometime, WZ might add a few bytes to the exsiting. 
C/C++ reversing skill is a plus to see the changes in their game client.
*/
```

### THIS PROJECT WAS COMPLETED (AUG 2021)

### Data and Config Files: 100% (45 / 45) (COMPLETED)
:heavy_check_mark:	1.	Lang.mpr  
  
```cpp
/*
(7z.dll is required for (un)zipping)
The file is an archive of most newly updated data files as listed below:
4ThTreeData.txt					4ThTreeSideOption.txt			4ThTreeSkillTooltip.txt
ArcaBattleBootyMix.txt				ArcaBattleScript.txt			AttributeVariation.txt
BonusExp.txt					BuffEffect.txt				CharacterInfoSetup.txt
Credit.txt					Dialog(kor).txt				ElementalMixList.txt
Filter(kor).txt					FilterName(kor).txt			Gate.txt
GradedOption.txt				GuideQuest.txt				GuideQuestString(Kor).txt
HuntingRecord.txt				ItemLevelTooltip.txt			ItemTRSData.txt
LabyrinthOfDimensionInfo.txt			MasteryItemBonusOption.txt		MBoxShop(kor).txt
Mixslotframe.txt				movereq(kor).txt			MuunEffectInfo.txt
npcName(kor).txt				PentagramJewelOptionValue.txt		PentagramOption.txt
SeedsphereUpgrade.txt				Skill(kor).txt				SkillRequire.txt
Slide(kor).txt					SocketitemUpgrade.txt			SpellStone.txt
Text(kor).txt					WingAttribute.txt
RenderingData/EnchantEffect.txt
EffectInfo/EffectInfoxxx.txt
*/
```
  

:heavy_check_mark:	2.	Item.bmd  
:heavy_check_mark:	3.	ItemToolTip.bmd  
:heavy_check_mark:	4.	ItemToolTipText.bmd  
:heavy_check_mark:	5.	ItemAddOption.bmd  
:heavy_check_mark:	6.	ItemSetType.bmd  
:heavy_check_mark:	7.	ItemSetOption.bmd  
:heavy_check_mark:	8.	ItemSetOptionText.bmd  
:heavy_check_mark:	9.	SocketItem.bmd  
:heavy_check_mark:	10.	ExcellentCommonOption.bmd  
:heavy_check_mark:	11.	ExcellentWingOption.bmd  
:heavy_check_mark:	12.	JewelOfHarmonyOption.bmd  
~~JewelOfHarmonySmelt.bmd (client does not open/load ?)~~  
:heavy_check_mark:	13.	StatOption.bmd  
:heavy_check_mark:	14.	StatOptionText.bmd  
:heavy_check_mark:	15.	InfoTooltip.bmd  
:heavy_check_mark:	16.	InfoTooltipText.bmd  
:heavy_check_mark:	17.	SkillTooltipText.bmd  
:heavy_check_mark:	18.	MasterSkillTreeData.bmd  
:heavy_check_mark:	19.	MasterSkillTooltip.bmd  
:heavy_check_mark:	20.	MonsterSkill.bmd  
:heavy_check_mark:	21.	Mix.bmd  
:heavy_check_mark:	22.	PentagramMixNeedSource.bmd  
:heavy_check_mark:	23.	Pet.bmd  
:heavy_check_mark:	24.	PetData.bmd  
:heavy_check_mark:	25.	MuunOption.bmd  
:heavy_check_mark:	26.	MuunInfo.bmd  
:heavy_check_mark:	27.	MuunTooltipInfo.bmd  
:heavy_check_mark:	28.	MuunRenderInfo.bmd  
:heavy_check_mark:	29.	MuunMeshInfo.bmd  
:heavy_check_mark:	30.	MuunExchange.bmd  
:heavy_check_mark:	31.	MuRummyOption.bmd  
:heavy_check_mark:	32.	MuRummyCard.bmd  
:heavy_check_mark:	33.	MuRummySlot.bmd  
:heavy_check_mark:	34.	Quest.bmd  
:heavy_check_mark:	35.	QuestProgress.bmd  
:heavy_check_mark:	36.	QuestWords.bmd  
:heavy_check_mark:	37.	NPCDialogue.bmd  
:heavy_check_mark:	38.	ServerList.bmd  
:heavy_check_mark:	39.	FormulaData.bmd  
:heavy_check_mark:	40.	PlayGuide.bmd  
:heavy_check_mark:	41.	EvolutionMonsterBox.bmd  
:heavy_check_mark:	42.	MapCharacters.bmd  
:heavy_check_mark:	43.	HelpData.bmd  
:heavy_check_mark:	44.	RuudShopViewInfo.bmd  
:heavy_check_mark:	45.	Camera_Angle_Position.bmd  

### Image, 3D Model, Interface and Map Files: 100% (10 / 10)  (COMPLETED)
:heavy_check_mark:	1.	OZJ	(-> .JPG)  
:heavy_check_mark:	2.	OZT	(-> .TGA)  
:heavy_check_mark:	3.	OZB (-> .BMP)  
:heavy_check_mark:	4.	OZP (-> .PNG)  
:heavy_check_mark:	5.	OZD	(-> .DDS)  
:heavy_check_mark:	6.	OZG	(-> .GFX)  
:heavy_check_mark:	7.	BMD (-> .SMD)  
:heavy_check_mark:	8.	MAP (-> .PAM)  
:heavy_check_mark:	9.	ATT (-> .TTA)  
:heavy_check_mark:	10.	OBJ (-> .JBO)  

Notes:  
Linked this project to https://github.com/VD1988/MuOnline-WorldEditor  
Use the WorldEditor to edit map/att/obj files.  

Updated Graphic Converter AllInOne.exe with C++17 parallel algorithms.  
It benefits a PC with many cores CPU and SSD drive. (Convert 2 GB "Data" folder in ~ 1min)  
Low writting speed hard drive will bottleneck CPU.  


SMD files can be opened in Blender by using Blender Source Tools.  
(https://developer.valvesoftware.com/wiki/Blender_Source_Tools)  

![Image 1](https://i.imgur.com/NTZdSGe.png)  
![Image 2](https://i.imgur.com/7dpsr6C.png)  

Some BMD Animations (monsters walking,...) use LockPosition flags.  
SMD files have no option to represent it. It can be set manually in LockPositionData.txt instead.  
Comparation test:  
![Gif 1](https://i.imgur.com/diERktn.gif) ![Gif 2](https://i.imgur.com/ZgYMpUD.gif)  
