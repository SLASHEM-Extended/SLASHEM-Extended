/*	SCCS Id: @(#)attrib.c	3.4	2002/10/07	*/
/*	Copyright 1988, 1989, 1990, 1992, M. Stephenson		  */
/* NetHack may be freely redistributed.  See license for details. */

/*  attribute modification routines. */

#include "hack.h"

/* #define DEBUG */	/* uncomment for debugging info */

#ifdef OVLB

	/* part of the output on gain or loss of attribute */
static
const char	* const plusattr[] = {
	"strong", "smart", "wise", "agile", "tough", "charismatic"
},
		* const minusattr[] = {
	"weak", "stupid", "foolish", "clumsy", "fragile", "repulsive"
};


/* KMH, intrinsics patch -- many of these are updated */
static
const struct innate {
	schar	ulevel;
	long	*ability;
	const char *gainstr, *losestr;
	boolean goodone;
}	arc_abil[] = { {	 1, &(HStealth), "", "", TRUE },
		     {   1, &(HFast), "", "", TRUE },
		     {   1, &(HMagicFindBonus), "", "", TRUE },
		     {   4, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
		     {  10, &(HSearching), "perceptive", "unaware", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	ana_abil[] = { {	 1, &(HFull_nutrient), "", "", TRUE },
		     {   1, &(HDefusing), "", "", TRUE },
		     {   7, &(HFast), "quick", "slow", TRUE },  
		     {	15, &(HWarning), "precognitive", "noncognitive", TRUE },  
		     {	15, &(HCont_resist), "protected from contamination", "vulnerable to contamination", TRUE },  
		     {	25, &(HExpBoost), "a surge of experience", "a loss of experience", TRUE },  
		     {	 0, 0, 0, 0, 0 } },  

	eme_abil[] = { {	 5, &(SanityTrebleEffect), "", "", FALSE },
		     {   10, &(CompletelyBadPartBug), "", "", FALSE },  
		     {   15, &(SatanEffect), "", "", FALSE },  
		     {   20, &(MonnoiseEffect), "", "", FALSE },  
		     {   25, &(BrownSpells), "", "", FALSE },  
		     {   28, &(LongingEffect), "", "", FALSE },  
		     {	 0, 0, 0, 0, 0 } },  

	xel_abil[] = { {	 1, &(HSwimming), "", "", TRUE },
		     {  1, &(HAcid_resistance), "", "", TRUE },
		     {  1, &(HScentView), "", "", TRUE },
		     {  5, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
		     {  6, &(HStealth), "stealthy", "noisy", TRUE },
		     {  6, &(HJumping), "able to jump around", "unable to jump around", TRUE },
		     {  9, &(HMysteryResist), "mysteriously resistant", "afraid of mysteries", TRUE },
		     {	10, &(HFear_resistance), "unafraid", "afraid", TRUE },
			{	12, &(HDiscount_action), "resistant to paralysis", "less resistant to paralysis", TRUE },
		       {   12, &(HTelepat), "disturbances in the force", "your grip on the force lessen", TRUE },
		     {  14, &(HCold_resistance), "warm", "cooler", TRUE },
		    { 15,  &(HInfravision), "perceptive", "half blind", TRUE },
			{   16, &(HManaleech), "magically attuned", "no longer magically attuned", TRUE },
		     {   16, &(HTechnicality), "technically knowledgable", "your techniques becoming weaker", TRUE },
		     {   18, &(HRegeneration), "regenerative", "your healing rate slowing down", TRUE },
		     {	18, &(HCont_resist), "protected from contamination", "vulnerable to contamination", TRUE },
		     {	20, &(HPoison_resistance), "healthy", "less healthy", TRUE },
			{     22, &(HPsi_resist), "psionic", "less psionic", TRUE },
		     {	24, &(HDisplaced), "displaced", "easy to spot", TRUE },
			{	25, &(HStone_resistance), "rock solid", "breakable", TRUE },
			{25, &(HMagical_breathing), "aquatic", "hydrophobic", TRUE },
		     {  26, &(HShock_resistance), "insulated", "conductive", TRUE },
		     {	 28, &(HHalf_physical_damage), "resistant to normal damage", "less resistant to damage", TRUE },
		     {  30, &(HResistancePiercing), "able to pierce enemy resistances", "incapable of piercing enemy resistances", TRUE },
		     {	 0, 0, 0, 0, 0 } },  

	ast_abil[] = { {	 1, &(HSwimming), "", "", TRUE },
		     {   4, &(HFast), "quick", "slow", TRUE },
		     {   8, &(HTechnicality), "technically knowledgable", "your techniques becoming weaker", TRUE },
		     {  10, &(HFire_resistance), "cool", "warmer", TRUE },
		     {  12, &(HMysteryResist), "mysteriously resistant", "afraid of mysteries", TRUE },
		     {   14, &(HSleep_resistance), "awake", "tired", TRUE },
		     {  15, &(HTeleport_control), "controlled", "helpless", TRUE },
		     {  16, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
		     {  18, &(HFull_nutrient), "your metabolism slowing down", "your metabolism speeding up", TRUE },
		     {   19, &(HUseTheForce), "like a jedi", "a loss of your jedi powers", TRUE },
		     {  20, &(HShock_resistance), "insulated", "conductive", TRUE },
		     {	20, &(HFear_resistance), "unafraid", "afraid", TRUE },
			{	24, &(HDisint_resistance), "stable", "brittle", TRUE },
			{	25, &(HSick_resistance), "immune to diseases", "no longer immune to diseases", TRUE },
			{28, &(HMagical_breathing), "aquatic", "hydrophobic", TRUE },
		     {  28, &(HFlying), "weightless", "grounded", TRUE },
		     {  30, &(HCold_resistance), "warm", "cooler", TRUE },
		     {	 0, 0, 0, 0, 0 } },  

	pic_abil[] = { {	 1, &(HStealth), "", "", TRUE },
		     {   6, &(HFast), "quick", "slow", TRUE },
		     {  8, &(HSearching), "perceptive", "unaware", TRUE },
		     {  16, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
		     {  20, &(HMagicFindBonus), "capable of finding magical items", "less capable of finding magical items", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	sto_abil[] = { {	 1, &(HShock_resistance), "", "", TRUE },
		     {   4, &(HFast), "quick", "slow", TRUE },
		     {   10, &(HSleep_resistance), "awake", "tired", TRUE },
		     {   15, &(HFast), "quick", "slow", TRUE },
		     {   19, &(HRegeneration), "regenerative", "your healing rate slowing down", TRUE },
			{	25, &(HDisint_resistance), "stable", "brittle", TRUE },
		     {   28, &(HFast), "quick", "slow", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	sof_abil[] = { {	 7, &(HSleep_resistance), "awake", "tired", TRUE },
			{   20, &(HSee_invisible), "your vision sharpen", "your vision blurring", TRUE },
		     {	28, &(HWarning), "precognitive", "noncognitive", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	cra_abil[] = { {   1, &(HDefusing), "", "", TRUE },
		     {   7, &(HFast), "quick", "slow", TRUE },
		     {  12, &(HStealth), "stealthy", "noisy", TRUE },
		     {  14, &(HTeleport_control), "controlled", "helpless", TRUE },
		     {  20, &(HSearching), "perceptive", "unaware", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	ble_abil[] = { {	 10, &(HPainSense), "empathic", "cold-hearted", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	dem_abil[] = { {	 5, &(MysteriousForceActive), "", "", FALSE },
		     {  9, &(UnfairAttackBug), "", "", FALSE },
		     {  13, &(HighlevelStatus), "", "", FALSE },
		     {  17, &(TrapCreationProblem), "", "", FALSE },
		     {  21, &(UndressingEffect), "", "", FALSE },
		     {  25, &(OrangeSpells), "", "", FALSE },
		     {  30, &(SatanEffect), "", "", FALSE },
		     {	 0, 0, 0, 0, 0 } },

	dan_abil[] = { {	1, &(HStealth), "", "", TRUE },
                 {  3, &(HFast), "fleet of foot", "lead-footed", TRUE },
                 {  10, &(HSearching), "perceptive", "distractable", TRUE },
		     {  16, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
		     {  20, &(HTechnicality), "technically knowledgable", "your techniques becoming weaker", TRUE },
		     {  20, &(HMysteryResist), "mysteriously resistant", "afraid of mysteries", TRUE },
			{	 25, &(HStun_resist), "steady", "less steady", TRUE },
                 {	0, 0, 0, 0 } },

	but_abil[] = { {  15, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
			 {	20, &(HScentView), "your sense of smell expanding", "less capable of smelling things", TRUE },
                 {	0, 0, 0, 0 } },

	cam_abil[] = { {   1, &(HDefusing), "", "", TRUE },
                 {	0, 0, 0, 0 } },

	wal_abil[] = { {   1, &(HDefusing), "", "", TRUE },
                 {	0, 0, 0, 0 } },

	spa_abil[] = { {   1, &(HDefusing), "", "", TRUE },
                 {	0, 0, 0, 0 } },

	unb_abil[] = { {   1, &(HDefusing), "", "", TRUE },
			 {   1, &(HMysteryResist), "", "", TRUE },
                 {	0, 0, 0, 0 } },

	pre_abil[] = { {   1, &(HDefusing), "", "", TRUE },
                 {	0, 0, 0, 0 } },

	sco_abil[] = { {   1, &(HDefusing), "", "", TRUE },
                 {	0, 0, 0, 0 } },

	sec_abil[] = { {   5, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
		     {  10, &(HMagical_breathing), "aquatic", "hydrophobic", TRUE },
		     {  15, &(HScentView), "your sense of smell expanding", "less capable of smelling things", TRUE },
		     {  18, &(HPsi_resist), "psionic", "less psionic", TRUE },
		     {  20, &(HDiscount_action), "resistant to paralysis", "less resistant to paralysis", TRUE },
		     {  25, &(HSwimming), "ready to swim","afraid of the water", TRUE },
		     {  30, &(HKeen_memory), "able to memorize everything", "unable to memorize anything", TRUE },
                 {	0, 0, 0, 0 } },

	sma_abil[] = { {	 12, &(HFast), "quick", "slow", TRUE },
		     {  15, &(HExtra_wpn_practice), "skillful", "unskilled", TRUE },
		     {  20, &(HJumping), "able to jump around", "unable to jump around", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	bar_abil[] = { {	 1, &(HPoison_resistance), "", "", TRUE },
		     {   7, &(HFast), "quick", "slow", TRUE },
		     {  15, &(HStealth), "stealthy", "noisy", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	noo_abil[] = { {	 1, &(HPoison_resistance), "", "", TRUE },
		     {  1, &(HMagicVacuum), "", "", FALSE },
		     {   7, &(HFast), "quick", "slow", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	dia_abil[] = { {  20, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
		     {  24, &(HFast), "quick", "slow", TRUE },
		     {  25, &(HMagicFindBonus), "capable of finding magical items", "less capable of finding magical items", TRUE },
		     {  30, &(HControlMagic), "magic-controlled", "no longer magic-controlled", TRUE },
		     {  30, &(HPoison_resistance), "healthy", "less healthy", TRUE },
		     {  30, &(HCold_resistance), "warm", "cooler", TRUE },
		     {  30, &(HFire_resistance), "cool", "warmer", TRUE },
		     {  30, &(HShock_resistance), "insulated", "conductive", TRUE },
		     {  30, &(HResistancePiercing), "able to pierce enemy resistances", "incapable of piercing enemy resistances", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	hus_abil[] = { {	 1, &(HScentView), "", "", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	pro_abil[] = { { 10, &(HDiminishedBleeding), "your wounds closing faster", "your blood pumping", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	ass_abil[] = { {	 1, &(HStealth), "", "", TRUE },
		     {   1, &(HDefusing), "", "", TRUE },
		     {   10, &(HFast), "quick", "slow", TRUE },
		     {	15, &(HWarning), "sensitive", "careless", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	ama_abil[] = { {	 1, &(HStealth), "", "", TRUE },
		     {   5, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
		     {   7, &(HFast), "quick", "slow", TRUE },
		     {  10, &(HSwimming), "ready to swim","afraid of the water", TRUE },
		     {  20, &(HMysteryResist), "mysteriously resistant", "afraid of mysteries", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	mas_abil[] = { {	 1, &(HSee_invisible), "", "", TRUE },
			{	 1, &(HKeen_memory), "", "", TRUE },
		     {       3, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
			{     10, &(HPsi_resist), "psionic", "less psionic", TRUE },
			{   15, &(HManaleech), "magically attuned", "no longer magically attuned", TRUE },
		     {  16, &(HFull_nutrient), "your metabolism slowing down", "your metabolism speeding up", TRUE },
		     {  20, &(HMysteryResist), "mysteriously resistant", "afraid of mysteries", TRUE },
		     {  20, &(HResistancePiercing), "able to pierce enemy resistances", "incapable of piercing enemy resistances", TRUE },
		     {   24, &(HInvis), "hidden", "more visible", TRUE },
		     {   25, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	sup_abil[] = { {	 1, &(HTeleportation), "", "", FALSE },
			{	3, &(HSleep_resistance), "awake", "tired", TRUE },
		     {	8, &(HWarning), "sensitive", "careless", TRUE },
		     {   22, &(HTeleport_control), "controlled", "helpless", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	cou_abil[] = { {	 1, &(HHunger), "", "", FALSE },
		     {   1, &(HAggravate_monster), "", "", FALSE },
		     {  1, &(HConflict), "", "", FALSE },
		     {  1, &(HBurdenedState), "", "", FALSE },
		     {  1, &(HMagicVacuum), "", "", FALSE },
		     {  1, &(HFuckOverEffect), "", "", FALSE },
		     {	 0, 0, 0, 0, 0 } },

	wei_abil[] = { {   1, &(HPsi_resist), "", "", TRUE },
			 {   6, &(HTeleportation), "very jumpy", "less jumpy", FALSE },
			{	8, &(HDiscount_action), "resistant to paralysis", "less resistant to paralysis", TRUE },
		     {   9, &(HMysteryResist), "mysteriously resistant", "afraid of mysteries", TRUE },
			 {	 10, &(HDeath_resistance), "un-dead", "dead", TRUE },
			 {	 12, &(HHunger), "very hungry", "less hungry", FALSE },
		     {	13, &(HExpBoost), "a surge of experience", "a loss of experience", TRUE },  
		     {   14, &(HAcid_resistance), "warded", "endangered", TRUE },
		     {   16, &(HDrain_resistance), "more resistant to drain life", "less resistant to drain life", TRUE },
		     {  20, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
		    { 22,  &(HInfravision), "perceptive", "half blind", TRUE },
		       {   25, &(HPolymorph), "polymorphic", "form-stable", FALSE },
		     {  26, &(HCold_resistance), "warm", "cooler", TRUE },
		     {	 28, &(HSlow_digestion), "a certain satiation", "the need to eat more", TRUE },
		     {   30, &(HFast), "quick", "slow", TRUE },  
		     {	 0, 0, 0, 0, 0 } },

	brd_abil[] = { {	5, &(HSleep_resistance), "awake", "tired", TRUE },
		     {	 10, &(HStealth), "stealthy", "noisy", TRUE },
		     {	 10, &(HFuckOverEffect), "being fucked over", "not being fucked over anymore", FALSE },
		     {	 0, 0, 0, 0, 0 } },

	gam_abil[] = { {	3, &(HSleep_resistance), "awake", "tired", TRUE },
		     {   10, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
		     {	 13, &(HSlow_digestion), "a certain satiation", "the need to eat more", TRUE },
			{   23, &(HManaleech), "magically attuned", "no longer magically attuned", TRUE },
		     {  24, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	cav_abil[] = { {	 7, &(HFast), "quick", "slow", TRUE },
		     {  10, &(HScentView), "your sense of smell expanding", "less capable of smelling things", TRUE },
		     {  12, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
		     {	15, &(HWarning), "sensitive", "careless", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	jan_abil[] = { {   1, &(HDefusing), "", "", TRUE },
		     {  10, &(HSearching), "perceptive", "unaware", TRUE },
		     {	20, &(HPoison_resistance), "healthy", "less healthy", TRUE },
		     {	25, &(HFear_resistance), "unafraid", "afraid", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	con_abil[] = { {   1, &(HSick_resistance), "", "", TRUE },
		     {   1, &(HDefusing), "", "", TRUE },
	         {	 7, &(HPoison_resistance), "healthy", "less healthy", TRUE },
		     {  20, &(HSearching), "perceptive", "unaware", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	mur_abil[] = { {   1, &(HAggravate_monster), "", "", FALSE },
	           {	 2, &(HSearching), "perceptive", "unaware", TRUE },
	           {	 3, &(HSearching), "perceptive", "unaware", TRUE },
	           {	 4, &(HSearching), "perceptive", "unaware", TRUE },
	           {	 5, &(HSearching), "perceptive", "unaware", TRUE },
	           {	 6, &(HSearching), "perceptive", "unaware", TRUE },
	           {	 7, &(HSearching), "perceptive", "unaware", TRUE },
	           {	 8, &(HSearching), "perceptive", "unaware", TRUE },
	           {	 9, &(HSearching), "perceptive", "unaware", TRUE },
	           {	 10, &(HSearching), "perceptive", "unaware", TRUE },
	           {	 11, &(HSearching), "perceptive", "unaware", TRUE },
	           {	 12, &(HSearching), "perceptive", "unaware", TRUE },
	           {	 13, &(HSearching), "perceptive", "unaware", TRUE },
	           {	 14, &(HSearching), "perceptive", "unaware", TRUE },
	           {	 15, &(HSearching), "perceptive", "unaware", TRUE },
	           {	 16, &(HSearching), "perceptive", "unaware", TRUE },
	           {	 17, &(HSearching), "perceptive", "unaware", TRUE },
	           {	 18, &(HSearching), "perceptive", "unaware", TRUE },
	           {	 19, &(HSearching), "perceptive", "unaware", TRUE },
	           {	 20, &(HSearching), "perceptive", "unaware", TRUE },
	           {	 20, &(HSearching), "perceptive", "unaware", TRUE },
	           {	 21, &(HSearching), "perceptive", "unaware", TRUE },
	           {	 22, &(HSearching), "perceptive", "unaware", TRUE },
	           {	 23, &(HSearching), "perceptive", "unaware", TRUE },
	           {	 24, &(HSearching), "perceptive", "unaware", TRUE },
	           {	 25, &(HSearching), "perceptive", "unaware", TRUE },
	           {	 26, &(HSearching), "perceptive", "unaware", TRUE },
	           {	 27, &(HSearching), "perceptive", "unaware", TRUE },
	           {	 28, &(HSearching), "perceptive", "unaware", TRUE },
	           {	 29, &(HSearching), "perceptive", "unaware", TRUE },
	           {	 30, &(HSearching), "perceptive", "unaware", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	bul_abil[] = { {   1, &(HAggravate_monster), "", "", FALSE },
	         {	 7, &(HPoison_resistance), "healthy", "less healthy", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	fai_abil[] = { {   1, &(HAggravate_monster), "", "", FALSE },
			{	1, &(BadEffectProblem), "", "", FALSE },
			{	1, &(AutomaticVulnerabilitiy), "", "", FALSE },
		     {	 0, 0, 0, 0, 0 } },

	sta_abil[] = { {   1, &(HTechnicality), "", "", TRUE },
		     {   5, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	trs_abil[] = { {   5, &(LevelTrapEffect), "", "", FALSE },
			{	10, &(CaptchaProblem), "", "", FALSE },
			{	15, &(ChaosTerrain), "", "", FALSE },
			{	20, &(PlayerCannotTrainSkills), "", "", FALSE },
			{	25, &(FoodIsAlwaysRotten), "", "", FALSE },
			{	30, &(InventoryLoss), "", "", FALSE },
		     {	 0, 0, 0, 0, 0 } },

	kur_abil[] = { {   4, &(NotSeenBug), "", "", FALSE },
			{	8, &(GrayoutBug), "", "", FALSE },
			{     10, &(HDiminishedBleeding), "your wounds closing faster", "your blood pumping", TRUE },
			{	12, &(DifficultyIncreased), "", "", FALSE },
			{	16, &(UnfairAttackBug), "", "", FALSE },
			{	20, &(EnmityBug), "", "", FALSE },
			{	25, &(ElongationBug), "", "", FALSE },
			{	30, &(Antileveling), "", "", FALSE },
		     {	 0, 0, 0, 0, 0 } },

	gro_abil[] = { {	 1, &(HFast), "", "", TRUE },
			{   4, &(Unidentify), "", "", FALSE },
			{	8, &(MojibakeEffect), "", "", FALSE },
			{	12, &(Dropcurses), "", "", FALSE },
		     {  15, &(HStealth), "stealthy", "noisy", TRUE },
			{	16, &(RecurringDisenchant), "", "", FALSE },
			{	20, &(TimerunBug), "", "", FALSE },
			{	25, &(FastMetabolismEffect), "", "", FALSE },
			{	30, &(Goldspells), "", "", FALSE },
		     {	 0, 0, 0, 0, 0 } },

	mah_abil[] = { {   1, &(HAggravate_monster), "", "", FALSE },
			{	1, &(HAntimagic), "", "", TRUE },
			{   7, &(HSee_invisible), "your vision sharpen", "your vision blurring", TRUE },
			{  15, &(HTeleport_control), "controlled","uncontrolled", TRUE },
		       {   15, &(HPolymorph_control), "your choices improve", "choiceless", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	sla_abil[] = { {   1, &(HAggravate_monster), "", "", FALSE },
		     {  1, &(HCold_resistance), "", "", TRUE },
			{	5, &(HSleep_resistance), "awake", "tired", TRUE },
		     {   15, &(HTeleport_control), "controlled", "helpless", TRUE },
			{   25, &(HManaleech), "magically attuned", "no longer magically attuned", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	fla_abil[] = { {   1, &(HFire_resistance), "", "", TRUE },
		     {  12, &(HResistancePiercing), "able to pierce enemy resistances", "incapable of piercing enemy resistances", TRUE },
		     {  13, &(HCold_resistance), "warm", "cooler", TRUE },
		     {  20, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
		     {   0, 0, 0, 0, 0 } },

	sli_abil[] = { {   1, &(HAcid_resistance), "", "", TRUE },
			{	1, &(MenuBug), "", "", FALSE },
			{	5, &(AutoDestruct), "", "", FALSE },
			{   6, &(HSee_invisible), "your vision sharpen", "your vision blurring", TRUE },
			{	7, &(DSTWProblem), "", "", FALSE },
			{     9, &(HManaleech), "magically attuned", "no longer magically attuned", TRUE },
			{	10, &(HCorrosivity), "corrosive", "less acidic", TRUE },
			{	12, &(FreeHandLoss), "", "", FALSE },
		     {  14, &(HDiminishedBleeding), "your wounds closing faster", "your blood pumping", TRUE },
			{	15, &(HSleep_resistance), "awake", "tired", TRUE },
		     {	16, &(HControlMagic), "magic-controlled", "no longer magic-controlled", TRUE },
			{	17, &(DifficultyIncreased), "", "", FALSE },
		       {   18, &(HPolymorph_control), "your choices improve", "choiceless", TRUE },
			{	20, &(HSick_resistance), "immune to diseases", "no longer immune to diseases", TRUE },
			{	22, &(Deafness), "", "", FALSE },
		     {   24, &(HMysteryResist), "mysteriously resistant", "afraid of mysteries", TRUE },
			{	25, &(HDisint_resistance), "stable", "brittle", TRUE },
			{	26, &(HDiscount_action), "resistant to paralysis", "less resistant to paralysis", TRUE },
			{	28, &(ShadesOfGrey), "", "", FALSE },
			{	30, &(HStone_resistance), "rock solid", "breakable", TRUE },
		     {   0, 0, 0, 0, 0 } },

	erd_abil[] = { {   1, &(HFree_action), "", "", TRUE },
		     {  10, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
		     {   0, 0, 0, 0, 0 } },

	cel_abil[] = { {   1, &(BlueSpells), "", "", FALSE },
			{	5, &(DamageMeterBug), "", "", FALSE },
			{	9, &(RangCallEffect), "", "", FALSE },
		     {	10, &(HControlMagic), "magic-controlled", "no longer magic-controlled", TRUE },
			{     12, &(HResistancePiercing), "able to pierce enemy resistances", "incapable of piercing enemy resistances", TRUE },
			{	14, &(CrapEffect), "", "", FALSE },
		     {	 15, &(HFuckOverEffect), "being fucked over", "not being fucked over anymore", FALSE },
			{	18, &(PokelieEffect), "", "", FALSE },
			{     20, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
			{	22, &(ExplodingDiceEffect), "", "", FALSE },
			{	26, &(AutopilotEffect), "", "", FALSE },
			{	30, &(InterfaceScrewed), "", "", FALSE },
		     {   0, 0, 0, 0, 0 } },

	dea_abil[] = { {   1, &(HResistancePiercing), "", "", TRUE },
			{     10, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
			 {	18, &(HControlMagic), "magic-controlled", "no longer magic-controlled", TRUE },
		     {   0, 0, 0, 0, 0 } },

	blo_abil[] = { {   1, &(HWarning), "", "", TRUE },
			{  8, &(HSearching), "perceptive", "unaware", TRUE },
		     {	 12, &(HStealth), "stealthy", "noisy", TRUE },
		     {  15, &(HFire_resistance), "cool", "warmer", TRUE },
		     {  18, &(HPainSense), "empathic", "cold-hearted", TRUE },
		     {  20, &(HScentView), "your sense of smell expanding", "less capable of smelling things", TRUE },
		     {   0, 0, 0, 0, 0 } },

	yau_abil[] = { {   1, &(HInfravision), "", "", TRUE },
			{	10, &(HFast), "quick", "slow", TRUE },
			{  15, &(HFire_resistance), "cool", "warmer", TRUE },
		     {   16, &(HClairvoyant), "clairvoyant", "mentally poor", TRUE },
		     {	18, &(HPoison_resistance), "healthy", "less healthy", TRUE },
		       {   20, &(HTelepat), "disturbances in the force", "your grip on the force lessen", TRUE },
		     {  20, &(HKeen_memory), "able to memorize everything", "unable to memorize anything", TRUE },
		     {  22, &(HCold_resistance), "warm", "cooler", TRUE },
		     {  26, &(HShock_resistance), "insulated", "conductive", TRUE },
		     {   0, 0, 0, 0, 0 } },

	scr_abil[] = { {   1, &(HFire_resistance), "", "", TRUE },
			{	5, &(HSleep_resistance), "awake", "tired", TRUE },
		     {   14, &(HTeleport_control), "controlled", "helpless", TRUE },
		     {  16, &(HCold_resistance), "warm", "cooler", TRUE },
		     {  28, &(HMagicFindBonus), "capable of finding magical items", "less capable of finding magical items", TRUE },
		     {   0, 0, 0, 0, 0 } },

	joc_abil[] = { { 10, &(HPainSense), "empathic", "cold-hearted", TRUE },
			{   20, &(HPeacevision), "able to recognize peaceful creatures", "unable to tell friend from enemy", TRUE },
			{   28, &(HFree_action), "freed", "a loss of freedom", TRUE },
		     {   0, 0, 0, 0, 0 } },

	coo_abil[] = { {   1, &(HFire_resistance), "", "", TRUE },
		     {  1, &(HFull_nutrient), "", "", TRUE },
		     {  15, &(HScentView), "your sense of smell expanding", "less capable of smelling things", TRUE },
		     {   0, 0, 0, 0, 0 } },

	sng_abil[] = { {   1, &(HFull_nutrient), "", "", TRUE },
		     {  1, &(HFuckOverEffect), "", "", FALSE },
		     {   0, 0, 0, 0, 0 } },

	soc_abil[] = { {  1, &(HFuckOverEffect), "", "", FALSE },
		     {   0, 0, 0, 0, 0 } },

	psy_abil[] = { {   1, &(HSee_invisible), "", "", TRUE },
			{	5, &(HSleep_resistance), "awake", "tired", TRUE },
			{     7, &(HPsi_resist), "psionic", "less psionic", TRUE },
			{     10, &(HPeacevision), "able to recognize peaceful creatures", "unable to tell friend from enemy", TRUE },
		     {	12, &(HWarning), "precognitive", "noncognitive", TRUE },  
		     {	 15, &(HEnergy_regeneration), "charged with mana", "a loss of mana", TRUE },
		     {  16, &(HTeleport_control), "controlled", "helpless", TRUE },
			{	 17, &(HStealth), "stealthy", "noisy", TRUE },
			{	 18, &(HConf_resist), "more resistant to confusion", "less resistant to confusion", TRUE },
		     {  20, &(HPainSense), "empathic", "cold-hearted", TRUE },
		     {  22, &(HMysteryResist), "mysteriously resistant", "afraid of mysteries", TRUE },
			{	 24, &(HVersus_curses), "curse resistant", "vulnerable to curses", TRUE },
			{	 27, &(HStun_resist), "steady", "less steady", TRUE },
		     {	 30, &(HHalf_spell_damage), "resistant to spells", "less resistant to spells", TRUE },
		     {   0, 0, 0, 0, 0 } },

	qub_abil[] = { {   1, &(HFire_resistance), "", "", TRUE },
		     {   9, &(HSwimming), "ready to swim","afraid of the water", TRUE },
		     {   10, &(HTechnicality), "technically knowledgable", "your techniques becoming weaker", TRUE },
		     {	20, &(HFear_resistance), "unafraid", "afraid", TRUE },
		     {   0, 0, 0, 0, 0 } },

	fnc_abil[] = { {	 7, &(HFast), "quick", "slow", TRUE },
		     {   0, 0, 0, 0, 0 } },

	emp_abil[] = { {	 1, &(HTelepat), "", "", TRUE },
		       {	1, &(HStealth), "", "", TRUE },
		       {	1, &(HPeacevision), "", "", TRUE },
		     {	4, &(HWarning), "sensitive", "careless", TRUE },
		     {   5, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
		     {  6, &(HSearching), "perceptive", "unaware", TRUE },
			{	7, &(HSleep_resistance), "awake", "tired", TRUE },
		     {	8, &(HControlMagic), "magic-controlled", "no longer magic-controlled", TRUE },
			{     10, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
			{	12, &(HSick_resistance), "immune to diseases", "no longer immune to diseases", TRUE },
			{   15, &(HManaleech), "magically attuned", "no longer magically attuned", TRUE },
		       {   18, &(HPolymorph_control), "your choices improve", "choiceless", TRUE },
		     {   24, &(HClairvoyant), "clairvoyant", "mentally poor", TRUE },
		     {  25, &(HDiminishedBleeding), "your wounds closing faster", "your blood pumping", TRUE },
		     {   0, 0, 0, 0, 0 } },

	fig_abil[] = { {	 7, &(HFast), "quick", "slow", TRUE },
		     {   0, 0, 0, 0, 0 } },

	med_abil[] = { {	1, &(HStealth), "", "", TRUE },
		       {	7, &(HSearching), "perceptive", "unaware", TRUE },
		     {  10, &(HFull_nutrient), "your metabolism slowing down", "your metabolism speeding up", TRUE },
		     {  12, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
			{   14, &(HSee_invisible), "your vision sharpen", "your vision blurring", TRUE },
			{   20, &(HManaleech), "magically attuned", "no longer magically attuned", TRUE },
		     {   0, 0, 0, 0, 0 } },

	lad_abil[] = { {   1, &(HSleep_resistance), "", "", TRUE },
			{  4, &(HSearching), "perceptive", "unaware", TRUE },
			{   7, &(HSee_invisible), "your vision sharpen", "your vision blurring", TRUE },
		     {   0, 0, 0, 0, 0 } },

	jes_abil[] = { {	 5, &(HStealth), "stealthy", "noisy", TRUE },
		     {   7, &(HFast), "quick", "slow", TRUE },
		     {	10, &(HWarning), "sensitive", "careless", TRUE },
			{  12, &(HSearching), "perceptive", "unaware", TRUE },
		     {   0, 0, 0, 0, 0 } },

	art_abil[] = { {   7, &(HSee_invisible), "your vision sharpen", "your vision blurring", TRUE },
		       {   20, &(HPolymorph_control), "your choices improve", "choiceless", TRUE },
		     {   0, 0, 0, 0, 0 } },

	gun_abil[] = { {   1, &(HPoison_resistance), "", "", TRUE },
		     {  1, &(HStealth), "", "", TRUE },
		     {  5, &(HFire_resistance), "cool", "warmer", TRUE },
		       {   7, &(HSee_invisible), "your vision sharpen", "your vision blurring", TRUE },
		     {  10, &(HCold_resistance), "warm", "cooler", TRUE },
		     {   0, 0, 0, 0, 0 } },

	mar_abil[] = { {   1, &(HFast), "", "", TRUE },
		     {  20, &(HFuckOverEffect), "being fucked over", "not being fucked over anymore", FALSE },
		     {   0, 0, 0, 0, 0 } },

	car_abil[] = { { 1, &(HSearching), "", "", TRUE },
			{ 7, &(HWarning), "sensitive", "insensitive", TRUE },
			{  30, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
			{ 0, 0, 0, 0 } },

	drm_abil[] = { { 7, &(HFast), "quick", "slow", TRUE },
		     {	20, &(HControlMagic), "magic-controlled", "no longer magic-controlled", TRUE },
			{ 0, 0, 0, 0 } },

	drd_abil[] = { {	15, &(HControlMagic), "magic-controlled", "no longer magic-controlled", TRUE },
		     {  20, &(HPainSense), "empathic", "cold-hearted", TRUE },
			{  28, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
			{ 0, 0, 0, 0 } },

	fjo_abil[] = { { 1, &(HSwimming), "", "", TRUE },
			{ 10, &(HCold_resistance), "warm", "cooler", TRUE },
		     {  18, &(HMagicFindBonus), "capable of finding magical items", "less capable of finding magical items", TRUE },
			{ 0, 0, 0, 0 } },

	akl_abil[] = { { 20, &(HSight_bonus), "your surroundings lighting up", "shrouded in darkness", TRUE },
		     {	20, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
			{ 0, 0, 0, 0 } },

	mil_abil[] = { { 10, &(HSick_resistance), "immune to diseases", "no longer immune to diseases", TRUE },
		     {	20, &(HPoison_resistance), "healthy", "less healthy", TRUE },
			{ 0, 0, 0, 0 } },

	sym_abil[] = { { 1, &(HPeacevision), "", "", TRUE },
		     {  15, &(HScentView), "your sense of smell expanding", "less capable of smelling things", TRUE },
		     {  16, &(HPainSense), "empathic", "cold-hearted", TRUE },
			{ 0, 0, 0, 0 } },

	stu_abil[] = { {  2, &(HSleep_resistance), "awake", "tired", TRUE },
		     {   5, &(HFast), "quick", "slow", TRUE },
		     {   6, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
		     {   14, &(HFree_action), "freed", "a loss of freedom", TRUE },
		     {   0, 0, 0, 0, 0 } },

	dol_abil[] = { {   1, &(HPoison_resistance), "", "", TRUE },
		     {	3, &(HWarning), "sensitive", "careless", TRUE },
			{   9, &(HManaleech), "magically attuned", "no longer magically attuned", TRUE },
			{  25, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
		     {   0, 0, 0, 0, 0 } },

	sag_abil[] = { {   1, &(HFull_nutrient), "", "", TRUE },
		     {   5, &(HManaleech), "magically attuned", "no longer magically attuned", TRUE },
		     {  10, &(HSearching), "perceptive", "unaware", TRUE },
		     {  12, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
		     {  15, &(HMysteryResist), "mysteriously resistant", "afraid of mysteries", TRUE },
		     {	17, &(HWarning), "sensitive", "careless", TRUE },
		     {	20, &(HDiscount_action), "resistant to paralysis", "less resistant to paralysis", TRUE },
		     {  25, &(HResistancePiercing), "able to pierce enemy resistances", "incapable of piercing enemy resistances", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	ota_abil[] = { {	8, &(HWarning), "sensitive", "careless", TRUE },
			{  16, &(HSearching), "perceptive", "unaware", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	aug_abil[] = { {   1, &(HTelepat), "", "", TRUE },
		     {   1, &(HStealth), "", "", TRUE },
		     {   1, &(HTechnicality), "", "", TRUE },
		     {   1, &(HMagicFindBonus), "", "", TRUE },
			{   1, &(HSpellboost), "", "", TRUE },
			{   3, &(HManaleech), "magically attuned", "no longer magically attuned", TRUE },
		     {	12, &(HWarning), "sensitive", "careless", TRUE },
		     {  15, &(HMysteryResist), "mysteriously resistant", "afraid of mysteries", TRUE },
		     {	20, &(HControlMagic), "magic-controlled", "no longer magic-controlled", TRUE },
		     {   25, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
		     {   0, 0, 0, 0, 0 } },

	fir_abil[] = { {   1, &(HFire_resistance), "", "", TRUE },
		     {  10, &(HSearching), "perceptive", "unaware", TRUE },
		     {   0, 0, 0, 0, 0 } },

	nin_abil[] = { {   1, &(HStealth), "", "", TRUE },
		     {   3, &(HInvis), "hidden", "more visible", TRUE },
		     {  10, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
		     {   0, 0, 0, 0, 0 } },

	off_abil[] = { {   1, &(HSearching), "", "", TRUE },
		     {  10, &(HWarning), "sensitive", "careless", TRUE },
		     {   0, 0, 0, 0, 0 } },

	aci_abil[] = { {   1, &(HAcid_resistance), "", "", TRUE },
			{     20, &(HResistancePiercing), "able to pierce enemy resistances", "incapable of piercing enemy resistances", TRUE },
			{     20, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
		     {   0, 0, 0, 0, 0 } },

	loc_abil[] = { {   1, &(HSearching), "", "", TRUE },
		     {   5, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
		     {   0, 0, 0, 0, 0 } },

	sha_abil[] = { {  1, &(HFuckOverEffect), "", "", FALSE },
			 {   9, &(HPolymorph_control), "your choices improve", "choiceless", TRUE },
		       {   0, 0, 0, 0, 0 } },

	unt_abil[] = { {   1, &(HPoison_resistance), "", "", TRUE },
		     {   4, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
		     {   7, &(HCold_resistance), "warm", "cooler", TRUE },
		     {   9, &(HMagicFindBonus), "capable of finding magical items", "less capable of finding magical items", TRUE },
			{     18, &(HDiminishedBleeding), "your wounds closing faster", "your blood pumping", TRUE },
		     {   0, 0, 0, 0, 0 } },

	gee_abil[] = { {   1, &(HPoison_resistance), "", "", TRUE },
		     {   1, &(HShock_resistance), "", "", TRUE },
		     {   5, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
		     {   8, &(HTeleport_control), "controlled", "helpless", TRUE },
		     {  15, &(HWarning), "sensitive", "careless", TRUE },
		     {   0, 0, 0, 0, 0, } },

	gra_abil[] = { {   1, &(HAggravate_monster), "", "", FALSE },
		     {   1, &(HFast), "", "", TRUE },
		     {   1, &(HSleep_resistance), "", "", TRUE },
		     {   10, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
		     {   0, 0, 0, 0, 0 } },

	hea_abil[] = { {	 1, &(HPoison_resistance), "", "", TRUE },
		     {	1, &(HControlMagic), "", "", TRUE },
		     {	15, &(HWarning), "sensitive", "careless", TRUE },
			{   20, &(HManaleech), "magically attuned", "no longer magically attuned", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	dru_abil[] = { {	 1, &(HPoison_resistance), "", "", TRUE },
			{   24, &(HManaleech), "magically attuned", "no longer magically attuned", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	sci_abil[] = { {	 1, &(HAcid_resistance), "", "", TRUE },
		     {   1, &(HTechnicality), "", "", TRUE },
		     {	25, &(HCont_resist), "protected from contamination", "vulnerable to contamination", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	mus_abil[] = { {	 7, &(HStealth), "stealthy", "noisy", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	ice_abil[] = { {   1, &(HCold_resistance), "", "", TRUE },
		     {  13, &(HFire_resistance), "cool", "warmer", TRUE },
		     {  16, &(HResistancePiercing), "able to pierce enemy resistances", "incapable of piercing enemy resistances", TRUE },
		     {  20, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
		     {   0, 0, 0, 0, 0 } },

	lib_abil[] = { {  20, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
		     {   0, 0, 0, 0, 0 } },

	ele_abil[] = { {   1, &(HShock_resistance), "", "", TRUE },
		     {  15, &(HResistancePiercing), "able to pierce enemy resistances", "incapable of piercing enemy resistances", TRUE },
		     {  20, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
		     {   0, 0, 0, 0, 0 } },

	psn_abil[] = { {   1, &(HPoison_resistance), "", "", TRUE },
		     {   1, &(HFull_nutrient), "", "", TRUE },
			{   5, &(HManaleech), "magically attuned", "no longer magically attuned", TRUE },
		     {  20, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
		     {  22, &(HResistancePiercing), "able to pierce enemy resistances", "incapable of piercing enemy resistances", TRUE },
		     {	25, &(HControlMagic), "magic-controlled", "no longer magic-controlled", TRUE },
		     {   0, 0, 0, 0, 0 } },

	occ_abil[] = { { 5, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
			 {  6, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
			 {   10, &(HInfravision), "perceptive", "half blind", TRUE },
		     {  30, &(HMysteryResist), "mysteriously resistant", "afraid of mysteries", TRUE },
		     {   0, 0, 0, 0, 0 } },

	emn_abil[] = { {  5, &(HResistancePiercing), "able to pierce enemy resistances", "incapable of piercing enemy resistances", TRUE },
			{    8, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
			{   10, &(HControlMagic), "magic-controlled", "no longer magic-controlled", TRUE },
			{   15, &(HPsi_resist), "psionic", "less psionic", TRUE },
		     {   0, 0, 0, 0, 0 } },

	cha_abil[] = { {   8, &(HResistancePiercing), "able to pierce enemy resistances", "incapable of piercing enemy resistances", TRUE },
			{  10, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
			 {   20, &(HEnergy_regeneration), "charged with mana", "a loss of mana", TRUE },
		     {  22, &(HMysteryResist), "mysteriously resistant", "afraid of mysteries", TRUE },
		     {   0, 0, 0, 0, 0 } },

	zoo_abil[] = { {   1, &(HWarning), "", "", TRUE },
			{   1, &(HScentView), "", "", TRUE },
		     {  10, &(HPoison_resistance), "hardy", "less healthy", TRUE },
		     {   0, 0, 0, 0, 0 } },

	cli_abil[] = { {   1, &(FemaleTrapJulietta), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	wom_abil[] = { {   5, &(FemaleTrapMaurah), "the need to fart constantly", "that your farting gas is depleted", FALSE },
			 {   8, &(FemaleTrapYvonne), "the desire to find a toilet", "that toilets are actually unnecessary", FALSE },
			 {   10, &(FemaleTrapSing), "enslaved by a woman who wants you to clean shoes", "that cleaning shit from shoes is, well, shit", FALSE },
			 {   12, &(FemaleTrapMeltem), "that the women in this dungeon want to fart you in the face", "that the female butts forgot how to fart", FALSE },
			 {   16, &(FemaleTrapJette), "the power of feminism increasing", "the power of feminism decreasing", FALSE },
			 {   18, &(FemaleTrapAnastasia), "like stepping into dog shit", "that only a fool would ever step into dog shit voluntarily", FALSE },
			 {   20, &(FemaleTrapJuen), "that someone is going to kick you in the shins", "like using a shin-pad", FALSE },
			 {   23, &(FemaleTrapEveline), "that the women are running faster", "that the women stopped running", FALSE },
			 {   27, &(FemaleTrapKristina), "a sharp smoky stench in the air", "that the fire got extinguished", FALSE },
			 {   30, &(FemaleTrapElif), "the presence of a lovely ghostly girl", "that the lovely ghostly girl has departed", FALSE },
		     {   0, 0, 0, 0, 0 } },

	jed_abil[] = { {   1, &(HStealth), "", "", TRUE },
		       {   3, &(HTelepat), "disturbances in the force", "your grip on the force lessen", TRUE },
		       {   5, &(HSee_invisible), "your vision sharpen", "your vision blurring", TRUE },
		       {   7, &(HFast), "quick", "slow", TRUE },
		       { 0, 0, 0, 0, 0 } },
			
	kni_abil[] = { {	 7, &(HFast), "quick", "slow", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	nuc_abil[] = { {	 1, &(HCont_resist), "", "", TRUE },
		     {   1, &(HTechnicality), "", "", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	for_abil[] = { {	 1, &(HPolymorph_control), "", "", TRUE },
		       {   5, &(HPolymorph), "polymorphic", "form-stable", FALSE },
		     {	 0, 0, 0, 0, 0 } },

	war_abil[] = { {	 7, &(HFast), "quick", "slow", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	mon_abil[] = { {   1, &(HFast), "", "", TRUE },
		     {   1, &(HSleep_resistance), "", "", TRUE },
		     {   1, &(HSee_invisible), "", "", TRUE },
		     {   3, &(HPoison_resistance), "healthy", "less healthy", TRUE },
		     {   5, &(HStealth), "stealthy", "noisy", TRUE },
		     {   7, &(HWarning), "sensitive", "careless", TRUE },
		     {   9, &(HSearching), "perceptive", "unaware", TRUE },
#if 0
		     {  11, &(HFire_resistance), "cool", "warmer", TRUE },
		     {  13, &(HCold_resistance), "warm", "cooler", TRUE },
		     {  15, &(HShock_resistance), "insulated", "conductive", TRUE },
		     /* WAC -- made the above three attribs techs */
#endif
		     {  17, &(HTeleport_control), "controlled","uncontrolled", TRUE },
		     {   0, 0, 0, 0, 0 } },

	hba_abil[] = { {   1, &(HFast), "", "", TRUE },
		     {   1, &(HSleep_resistance), "", "", TRUE },
		     {   1, &(HSee_invisible), "", "", TRUE },
		     {   3, &(HPoison_resistance), "healthy", "less healthy", TRUE },
		     {   5, &(HStealth), "stealthy", "noisy", TRUE },
		     {   7, &(HWarning), "sensitive", "careless", TRUE },
		     {   9, &(HSearching), "perceptive", "unaware", TRUE },
		     {  17, &(HTeleport_control), "controlled","uncontrolled", TRUE },
		     {   0, 0, 0, 0, 0 } },

	psi_abil[] = { {   1, &(HTelepat), "", "", TRUE },
		     {   1, &(HClairvoyant), "","", TRUE },
		     {   3, &(HSee_invisible), "your vision sharpen", "your vision blur", TRUE },
		     {   4, &(HResistancePiercing), "able to pierce enemy resistances", "incapable of piercing enemy resistances", TRUE },
		     {   5, &(ETelepat), "aware", "unaware", TRUE },
		     {   6, &(HPainSense), "empathic", "cold-hearted", TRUE },
		     {   7, &(HAggravate_monster), "noisy", "quiet", FALSE },
		     {   8, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
		     {  10, &(HDetect_monsters), "connected","detached", TRUE },
		     {  12, &(HFuckOverEffect), "being fucked over", "not being fucked over anymore", FALSE },
		     {  15, &(HFlying), "weightless", "grounded", TRUE },
		     {  16, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
		     {  20, &(HPasses_walls), "ethereal","mundane", TRUE },
			{   25, &(HManaleech), "magically attuned", "no longer magically attuned", TRUE },
		     {   0, 0, 0, 0, 0 } },

	fox_abil[] = { {   1, &(ETelepat), "", "", TRUE },
		     {   1, &(HClairvoyant), "","", TRUE },
		     {   1, &(HStealth), "","", TRUE },
		     {   5, &(HSearching), "perceptive", "unaware", TRUE },
		     {   10, &(HFast), "quick", "slow", TRUE },
		     {   15, &(HWarning), "sensitive", "careless", TRUE },
		     {   0, 0, 0, 0, 0 } },

	nec_abil[] = { {   1, &(HDrain_resistance), "", "", TRUE },
		     {   1, &(HSick_resistance), "", "", TRUE },
		     {   3, &(HUndead_warning), "sensitive", "careless", TRUE },
		     {	5, &(HControlMagic), "magic-controlled", "no longer magic-controlled", TRUE },
			{   15, &(HManaleech), "magically attuned", "no longer magically attuned", TRUE },
		     {  15, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
		     {  30, &(HResistancePiercing), "able to pierce enemy resistances", "incapable of piercing enemy resistances", TRUE },
		     {   0, 0, 0, 0, 0 } },

	pok_abil[] = { {   1, &(HDisint_resistance), "", "", TRUE },
		     {   4, &(HWarning), "sensitive", "careless", TRUE },
		     {   7, &(HSearching), "perceptive", "unaware", TRUE },
		     {   8, &(HResistancePiercing), "able to pierce enemy resistances", "incapable of piercing enemy resistances", TRUE },
		     {   10, &(HDrain_resistance), "more resistant to drain life", "less resistant to drain life", TRUE },
		     {   12, &(HScentView), "your sense of smell expanding", "less capable of smelling things", TRUE },
		     {   13, &(HFast), "quick", "slow", TRUE },
		     {   15, &(HMagicFindBonus), "capable of finding magical items", "less capable of finding magical items", TRUE },
		     {   16, &(HSleep_resistance), "awake", "tired", TRUE },
		     {   18, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
		     {   19, &(HAcid_resistance), "warded", "endangered", TRUE },
		     {   22, &(HTelepat), "telepathic", "nontelepathic", TRUE },
		     {   24, &(HTechnicality), "evolved", "a loss of PP", TRUE },
		     {   25, &(HSick_resistance), "resistant", "non-resistant", TRUE },
		     {   26, &(HCont_resist), "protected from contamination", "vulnerable to contamination", TRUE },
		     {   28, &(HTeleport_control), "controlled", "uncontrolled", TRUE },
		     {   30, &(HFlying), "airborne", "stalled", TRUE },
		     {   0, 0, 0, 0, 0 } },

	pir_abil[] = {	{1, &(HSwimming), "", "", TRUE },
			 {	7, &(HStealth), "stealthy", "noisy", TRUE },	/* with cat-like tread ... */
		     {  11, &(HFast), "quick", "slow", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	gof_abil[] = {	{1, &(HDrain_resistance), "", "", TRUE },
		     {   1, &(HInfravision), "", "", TRUE },
		     {   6, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
		     {  10, &(HReflecting), "reflexive", "nonreflexive", TRUE },
		     {  20, &(HDiminishedBleeding), "your wounds closing faster", "your blood pumping", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	gol_abil[] = { {   1, &(HMagicFindBonus), "", "", TRUE },
		     {   5, &(HStealth), "stealthy", "noisy", TRUE },
		     {   9, &(HFast), "quick", "slow", TRUE },
		     {   11, &(HSearching), "perceptive", "unaware", TRUE },
		     {   0, 0, 0, 0, 0 } },

	gla_abil[] = {	{3, &(HFast), "quick", "slow", TRUE },
			 {	6, &(HStealth), "stealthy", "noisy", TRUE },	/* with cat-like tread ... */
		     {  10, &(HAggravate_monster), "noisy", "quiet", FALSE },
		     {  15, &(HWarning), "sensitive", "careless", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	kor_abil[] = {	{2, &(HSearching), "perceptive", "unaware", TRUE },
			 {	7, &(HStealth), "stealthy", "noisy", TRUE },	/* with cat-like tread ... */
		     {	 0, 0, 0, 0, 0 } },

	div_abil[] = {	{15, &(HMagical_breathing), "aquatic", "hydrophobic", TRUE },
		     {  20, &(HMagicFindBonus), "capable of finding magical items", "less capable of finding magical items", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	lun_abil[] = { {   1, &(HPoison_resistance), "", "", TRUE },
		     {   1, &(HRegeneration), "", "", TRUE },
		     {   1, &(HScentView), "", "", TRUE },
		     {   7, &(HStealth), "stealthy", "noisy", TRUE },
		     {   0, 0, 0, 0, 0 } },
			 
	pri_abil[] = { {  12, &(HPainSense), "empathic", "cold-hearted", TRUE },
			 {	15, &(HWarning), "sensitive", "careless", TRUE },
		     {  20, &(HFire_resistance), "cool", "warmer", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	ran_abil[] = { {   1, &(HSearching), "", "", TRUE },
		     {   1, &(HDefusing), "", "", TRUE },
		     {	 7, &(HStealth), "stealthy", "noisy", TRUE },
		     {	15, &(HSee_invisible), "your vision sharpen", "your vision blur", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	elp_abil[] = { {   1, &(HSearching), "", "", TRUE },
		     {   1, &(HDefusing), "", "", TRUE },
		     {	4, &(HSleep_resistance), "awake", "tired", TRUE },
		     {	 7, &(HStealth), "stealthy", "noisy", TRUE },
		     {	15, &(HSee_invisible), "your vision sharpen", "your vision blur", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	rog_abil[] = { {	 1, &(HStealth), "", "", TRUE },
		     {   1, &(HDefusing), "", "", TRUE },
		     {  10, &(HSearching), "perceptive", "unaware", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	sam_abil[] = { {	 1, &(HFast), "", "", TRUE },
		     {  15, &(HStealth), "stealthy", "noisy", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	cyb_abil[] = { {	 1, &(HFast), "", "", TRUE },
		     {   1, &(HUseTheForce), "", "", TRUE },
		     {  15, &(HStealth), "stealthy", "noisy", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	tou_abil[] = { {	10, &(HSearching), "perceptive", "unaware", TRUE },
		     {	20, &(HPoison_resistance), "hardy", "less healthy", TRUE },
		     {  25, &(HMagicFindBonus), "capable of finding magical items", "less capable of finding magical items", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	rin_abil[] = { {  1, &(HStealth), "", "", TRUE },
		     {   7, &(HFast), "quick", "slow", TRUE },
		     {  20, &(HMagicFindBonus), "capable of finding magical items", "less capable of finding magical items", TRUE },
		     {   0, 0, 0, 0, 0 } },

	und_abil[] = { {   1, &(HStealth), "", "", TRUE },
		     {   1, &(HDrain_resistance), "", "", TRUE },
		     {   1, &(HSick_resistance), "", "", TRUE },
		     {   1, &(HUndead_warning), "", "", TRUE },
		     {   7, &(HFast), "quick", "slow", TRUE },
		     {   9, &(HPoison_resistance), "hardy", "less healthy", TRUE },
		     {  10, &(HMysteryResist), "mysteriously resistant", "afraid of mysteries", TRUE },
		     {   0, 0, 0, 0, 0 } },

	pal_abil[] = { {   1, &(HFast), "", "", TRUE },
		     {   1, &(HUndead_warning), "", "", TRUE },
		     {   5, &(HPoison_resistance), "hardy", "less healthy", TRUE },
		     {	10, &(HSleep_resistance), "awake", "tired", TRUE },
			{   24, &(HManaleech), "magically attuned", "no longer magically attuned", TRUE },
		     {   0, 0, 0, 0, 0 } },

	act_abil[] = { {   1, &(HPeacevision), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	jus_abil[] = { {   1, &(HPeacevision), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	val_abil[] = { {	 1, &(HCold_resistance), "", "", TRUE },
		     {	 1, &(HStealth), "", "", TRUE },
		     {   7, &(HFast), "quick", "slow", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	wan_abil[] = { {	 1, &(HCold_resistance), "", "", TRUE },
		     {   1, &(HFire_resistance), "", "", TRUE },
		     {   1, &(HShock_resistance), "", "", TRUE },
		     {   1, &(HSleep_resistance), "", "", TRUE },
			{	10, &(HWarning), "sensitive", "careless", TRUE },
			{   10, &(HSee_invisible), "your vision sharpen", "your vision blurring", TRUE },
			{   15, &(HManaleech), "magically attuned", "no longer magically attuned", TRUE },
		     {	15, &(HControlMagic), "magic-controlled", "no longer magic-controlled", TRUE },
			{  20, &(HTeleport_control), "controlled","uncontrolled", TRUE },
		       {   20, &(HPolymorph_control), "your choices improve", "choiceless", TRUE },
		     {  25, &(HResistancePiercing), "able to pierce enemy resistances", "incapable of piercing enemy resistances", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	mid_abil[] = { { 1, &(HSearching), "", "", TRUE },
		     {  10, &(HMagicFindBonus), "capable of finding magical items", "less capable of finding magical items", TRUE },
			{  10, &(HFire_resistance), "cool", "warmer", TRUE },
		     {   10, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
			{   0, 0, 0, 0, 0 } },

	yeo_abil[] = {
		     {   7, &(HFast), "quick", "slow", TRUE },
		     {  15, &(HSwimming), "ready to swim","afraid of the water", TRUE },
		     {   0, 0, 0, 0, 0 } },

	wiz_abil[] = { {   1, &(HManaleech), "", "", TRUE },
		     {	4, &(HControlMagic), "magic-controlled", "no longer magic-controlled", TRUE },
		     {  10, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
			 {	15, &(HWarning), "sensitive", "careless", TRUE },
		     {  17, &(HTeleport_control), "controlled","uncontrolled", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	alt_abil[] = { {   1, &(HControlMagic), "", "", TRUE },
			 {   3, &(HUndead_warning), "sensitive", "careless", TRUE },
			{     5, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
			{	7, &(HWarning), "sensitive", "careless", TRUE },
			{  15, &(HTeleport_control), "controlled","uncontrolled", TRUE },
			{     20, &(HResistancePiercing), "able to pierce enemy resistances", "incapable of piercing enemy resistances", TRUE },
		       {   25, &(HPolymorph_control), "your choices improve", "choiceless", TRUE },
		     {  30, &(HReflecting), "reflexive", "nonreflexive", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	dun_abil[] = { {	1, &(HFire_resistance), "", "", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	bos_abil[] = { {	1, &(HPoison_resistance), "", "", TRUE },
		     {  5, &(HScentView), "your sense of smell expanding", "less capable of smelling things", TRUE },
		     {  7, &(HStealth), "stealthy", "noisy", TRUE },
		     {  12, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
		     {  20, &(HMagicFindBonus), "capable of finding magical items", "less capable of finding magical items", TRUE },
		     {  25, &(HPainSense), "empathic", "cold-hearted", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	tha_abil[] = { {	4, &(HSleep_resistance), "awake", "tired", TRUE },
		     {   10, &(HAggravate_monster), "intolerable", "more acceptable", FALSE },
		     {  20, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
		     {  22, &(HMysteryResist), "mysteriously resistant", "afraid of mysteries", TRUE },
		     {  24, &(HResistancePiercing), "able to pierce enemy resistances", "incapable of piercing enemy resistances", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	ord_abil[] = { {   5, &(HSee_invisible), "your vision sharpen", "your vision blurring", TRUE },
		     {   10, &(HUndead_warning), "sensitive", "careless", TRUE },
		       { 0, 0, 0, 0, 0 } },

	/* Intrinsics conferred by race */
	dop_abil[] = { {   9, &(HPolymorph_control), "your choices improve", "choiceless", TRUE },
		       {   0, 0, 0, 0, 0 } },

	dwa_abil[] = { { 1, &(HSearching), "", "", TRUE },
			{  10, &(HFire_resistance), "cool", "warmer", TRUE },
		     {  10, &(HMagicFindBonus), "capable of finding magical items", "less capable of finding magical items", TRUE },
			{   0, 0, 0, 0, 0 } },

	nib_abil[] = { { 1, &(HSearching), "", "", TRUE },
		     {   1, &(HMagicFindBonus), "", "", TRUE },
			{   0, 0, 0, 0, 0 } },

	ssc_abil[] = { { 1, &(HMagicFindBonus), "", "", TRUE },
		      { 10, &(HFuckOverEffect), "being fucked over", "not being fucked over anymore", FALSE },
			{   0, 0, 0, 0, 0 } },

	dra_abil[] = { {	8, &(HControlMagic), "magic-controlled", "no longer magic-controlled", TRUE },
			 { 10, &(HScentView), "your sense of smell expanding", "less capable of smelling things", TRUE },
		     {  15, &(HMagicFindBonus), "capable of finding magical items", "less capable of finding magical items", TRUE },
			{   0, 0, 0, 0, 0 } },

	hyb_abil[] = { { 10, &(HScentView), "your sense of smell expanding", "less capable of smelling things", TRUE },
			{   0, 0, 0, 0, 0 } },

	fel_abil[] = { { 20, &(HScentView), "your sense of smell expanding", "less capable of smelling things", TRUE },
			{   0, 0, 0, 0, 0 } },

	kha_abil[] = { { 5, &(HScentView), "your sense of smell expanding", "less capable of smelling things", TRUE },
			{   0, 0, 0, 0, 0 } },

	azt_abil[] = { { 1, &(HFast), "", "", TRUE },
			{   0, 0, 0, 0, 0 } },

	irr_abil[] = { { 1, &(HFree_action), "", "", TRUE },
		     {	 1, &(HShock_resistance), "", "", TRUE },
			{   0, 0, 0, 0, 0 } },

	hyp_abil[] = { { 1, &(HFire_resistance), "", "", TRUE },
			{   0, 0, 0, 0, 0 } },

	dyn_abil[] = { { 1, &(HShock_resistance), "", "", TRUE },
			 {   1, &(HMysteryResist), "", "", TRUE },
			{   0, 0, 0, 0, 0 } },

	jav_abil[] = { { 1, &(HFire_resistance), "", "", TRUE },
		     {	 1, &(HPoison_resistance), "", "", TRUE },
		     {	 1, &(HSick_resistance), "", "", TRUE },
		     {  20, &(HScentView), "your sense of smell expanding", "less capable of smelling things", TRUE },
		     {  24, &(HPainSense), "empathic", "cold-hearted", TRUE },
			{   0, 0, 0, 0, 0 } },

	chi_abil[] = { { 1, &(HFull_nutrient), "", "", TRUE },
			{   0, 0, 0, 0, 0 } },

	dmo_abil[] = { {  1, &(HFuckOverEffect), "", "", FALSE },
		     {   0, 0, 0, 0, 0 } },

	pea_abil[] = { {  1, &(HFuckOverEffect), "", "", FALSE },
		     {   0, 0, 0, 0, 0 } },

	bov_abil[] = { { 1, &(HPoison_resistance), "", "", TRUE },
		     {   1, &(HSick_resistance), "", "", TRUE },
		     {   1, &(HFull_nutrient), "", "", TRUE },
		     {   7, &(HFuckOverEffect), "being fucked over", "not being fucked over anymore", FALSE },
			{  12, &(HVersus_curses), "curse resistant", "vulnerable to curses", TRUE },
			{   0, 0, 0, 0, 0 } },

	per_abil[] = { { 1, &(HHunger), "", "", FALSE },
		     {  15, &(HFull_nutrient), "your metabolism slowing down", "your metabolism speeding up", TRUE },
		     {  20, &(HFire_resistance), "cool", "warmer", TRUE },
		     {  24, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
			{   0, 0, 0, 0, 0 } },

	mog_abil[] = { { 7, &(HShock_resistance), "insulated", "conductive", TRUE },
		     {  10, &(HDouble_attack), "able to attack faster", "no longer able to attack faster", TRUE },
		     {  20, &(HQuad_attack), "able to attack ultra fast", "no longer able to attack ultra fast", TRUE },
			{   0, 0, 0, 0, 0 } },

	arf_abil[] = { { 20, &(HMagicFindBonus), "capable of finding magical items", "less capable of finding magical items", TRUE },
			{   0, 0, 0, 0, 0 } },

	kst_abil[] = { { 8, &(HTechnicality), "technically knowledgable", "your techniques becoming weaker", TRUE },
		     {   12, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
			{	20, &(HDisint_resistance), "stable", "brittle", TRUE },
		     {  25, &(HMysteryResist), "mysteriously resistant", "afraid of mysteries", TRUE },
			{   0, 0, 0, 0, 0 } },

	vie_abil[] = { {	7, &(HWarning), "precognitive", "noncognitive", TRUE },
			 {     10, &(HPsi_resist), "psionic", "less psionic", TRUE },
		     {  10, &(HSwimming), "ready to swim","afraid of the water", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	gru_abil[] = { { 1, &(HFast), "", "", TRUE },
			{   0, 0, 0, 0, 0 } },

	bab_abil[] = { { 1, &(HDefusing), "", "", TRUE },
			{   0, 0, 0, 0, 0 } },

	nea_abil[] = { {  10, &(HScentView), "your sense of smell expanding", "less capable of smelling things", TRUE },
		     {  12, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
			{   0, 0, 0, 0, 0 } },

	rus_abil[] = { { 1, &(HCold_resistance), "", "", TRUE },
		     {   10, &(HSleep_resistance), "awake", "tired", TRUE },
		     {  20, &(HExtra_wpn_practice), "skillful", "unskilled", TRUE },
			{   0, 0, 0, 0, 0 } },

	mac_abil[] = { { 1, &(HSight_bonus), "", "", TRUE },
			{   0, 0, 0, 0, 0 } },

	vik_abil[] = { { 1, &(HSwimming), "", "", TRUE },
		     {  30, &(HMagicFindBonus), "capable of finding magical items", "less capable of finding magical items", TRUE },
			{   0, 0, 0, 0, 0 } },

	slm_abil[] = { { 1, &(HCold_resistance), "", "", TRUE },
		     {	 1, &(HStone_resistance), "", "", TRUE },
		     {	 1, &(HShock_resistance), "", "", TRUE },
		     {	 1, &(HPoison_resistance), "", "", TRUE },
		     {	 1, &(HAcid_resistance), "", "", TRUE },
			{   0, 0, 0, 0, 0 } },

	elf_abil[] = { {	4, &(HSleep_resistance), "awake", "tired", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	inc_abil[] = { {	1, &(HAntimagic), "", "", TRUE },
			{   1, &(HManaleech), "", "", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	gre_abil[] = { {	1, &(HFire_resistance), "", "", TRUE },
			{   15, &(HFast), "quick", "slow", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	bga_abil[] = { {	10, &(HAggravate_monster), "noisy", "quiet", FALSE },
		     {	15, &(HFear_resistance), "unafraid", "afraid", TRUE },
			 {	 20, &(HHunger), "very hungry", "less hungry", FALSE },
		     {  20, &(HMysteryResist), "mysteriously resistant", "afraid of mysteries", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	clk_abil[] = { {	1, &(HPoison_resistance), "", "", TRUE },
		     {	 1, &(HSick_resistance), "", "", TRUE },
		     {	 1, &(HStone_resistance), "", "", TRUE },
		     {	 5, &(HShock_resistance), "shock resistant", "less shock resistant", TRUE },
		     {   8, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
		     {	 10, &(HCold_resistance), "cold resistant", "less cold resistant", TRUE },
		     {  10, &(HFull_nutrient), "your metabolism slowing down", "your metabolism speeding up", TRUE },
		     {	 15, &(HFire_resistance), "heat resistant", "less heat resistant", TRUE },
		     {   20, &(HTechnicality), "technically knowledgable", "your techniques becoming weaker", TRUE },
			{	25, &(HDiscount_action), "resistant to paralysis", "less resistant to paralysis", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	vor_abil[] = { {	1, &(HFlying), "", "", TRUE },
			{1, &(HMagical_breathing), "", "", TRUE },
		     {   1, &(HSleep_resistance), "", "", TRUE },
		     {   1, &(HPoison_resistance), "", "", TRUE },
		     {   1, &(HStone_resistance), "", "", TRUE },
			{   1, &(HManaleech), "", "", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	cor_abil[] = { {	1, &(HFlying), "", "", TRUE },
			{1, &(HMagical_breathing), "", "", TRUE },
		     {   1, &(HSleep_resistance), "", "", TRUE },
		     {   1, &(HPoison_resistance), "", "", TRUE },
		     {   1, &(HStone_resistance), "", "", TRUE },
			{   1, &(HManaleech), "", "", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	alb_abil[] = { {   1, &(HAntimagic), "", "", TRUE },
		     {  1, &(HFuckOverEffect), "", "", FALSE },
		     {   3, &(HUndead_warning), "sensitive", "careless", TRUE },
		     {   10, &(HSee_invisible), "your vision sharpen", "your vision blur", TRUE },
			{  12, &(HStealth), "stealthy", "noisy", TRUE },
			{15, &(HMagical_breathing), "aquatic", "hydrophobic", TRUE },
		     {	 25, &(HEnergy_regeneration), "charged with mana", "a loss of mana", TRUE },
		     {   0, 0, 0, 0, 0 } },

	alc_abil[] = { {	 1, &(HAcid_resistance), "", "", TRUE },
		     {  1, &(HFuckOverEffect), "", "", FALSE },
		     {  0, 0, 0, 0, 0 } },

	and_abil[] = { {	 1, &(HDeath_resistance), "", "", TRUE },
		     {  0, 0, 0, 0, 0 } },

	goa_abil[] = { {	 1, &(HScentView), "", "", TRUE },
		     {  0, 0, 0, 0, 0 } },

	ent_abil[] = { {	 1, &(HFull_nutrient), "", "", TRUE },
		     {  15, &(HFuckOverEffect), "being fucked over", "not being fucked over anymore", FALSE },
		     {  0, 0, 0, 0, 0 } },

	ang_abil[] = { {   1, &(HAntimagic), "", "", TRUE },
		     {   1, &(HFlying), "", "", TRUE },
		     {   1, &(HCold_resistance), "", "", TRUE },
		     {   1, &(HShock_resistance), "", "", TRUE },
		     {   1, &(HSleep_resistance), "", "", TRUE },
		     {   1, &(HPoison_resistance), "", "", TRUE },
		     {   1, &(HSee_invisible), "", "", TRUE },
		     {  20, &(HFuckOverEffect), "being fucked over", "not being fucked over anymore", FALSE },
		     {   0, 0, 0, 0, 0 } },

	dev_abil[] = { {   1, &(HFire_resistance), "", "", TRUE },
		     {   1, &(HPoison_resistance), "", "", TRUE },
		     {  1, &(HFuckOverEffect), "", "", FALSE },
		     {   0, 0, 0, 0, 0 } },

	anc_abil[] = { {   1, &(HCold_resistance), "", "", TRUE },
		     {	 1, &(HSlow_digestion), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	fen_abil[] = { {   1, &(HSearching), "", "", TRUE },
		     {  5, &(HScentView), "your sense of smell expanding", "less capable of smelling things", TRUE },
			{   5, &(HStealth), "stealthy", "noisy", TRUE },
		     {   7, &(HFast), "quick", "slow", TRUE },
		     {	 10, &(HFire_resistance), "heat resistant", "less heat resistant", TRUE },
		     {  15, &(HSwimming), "ready to swim","afraid of the water", TRUE },
		     {   0, 0, 0, 0, 0 } },

	fre_abil[] = { {   2, &(DifficultyIncreased), "", "", FALSE },
		     {	 5, &(Dehydration), "", "", FALSE },
		     {	 9, &(BlackNgWalls), "", "", FALSE },
		     {	 14, &(FeelerGauges), "", "", FALSE },
		     {	 18, &(AutocursingEquipment), "", "", FALSE },
		     {	 22, &(DischargeBug), "", "", FALSE },
		     {	 25, &(HardModeEffect), "", "", FALSE },
		     {	 29, &(Superscroller), "", "", FALSE },
		     {   0, 0, 0, 0, 0 } },

	uri_abil[] = { {   3, &(BigscriptEffect), "", "", FALSE },
		     {	 6, &(TurnLimitation), "", "", FALSE },
		     {	 8, &(TimerunBug), "", "", FALSE },
		     {	 12, &(PlayerCannotTrainSkills), "", "", FALSE },
		     {	 13, &(MonnoiseEffect), "", "", FALSE },
		     {	 17, &(SanityTrebleEffect), "", "", FALSE },
		     {	 20, &(PermacurseEffect), "", "", FALSE },
		     {	 24, &(CovetousnessBug), "", "", FALSE },
		     {   0, 0, 0, 0, 0 } },

	ind_abil[] = { {   4, &(Dropcurses), "", "", FALSE },
		     {	 7, &(verisiertEffect), "", "", FALSE },
		     {	 10, &(DangerousTerrains), "", "", FALSE },
		     {	 15, &(GushlushEffect), "", "", FALSE },
		     {	 19, &(SimeoutBug), "", "", FALSE },
		     {	 23, &(WingYellowChange), "", "", FALSE },
		     {	 27, &(LowEffects), "", "", FALSE },
		     {	 30, &(EngravingDoesntWork), "", "", FALSE },
		     {   0, 0, 0, 0, 0 } },

	col_abil[] = { {   2, &(YellowSpells), "", "", FALSE },
		     {	 4, &(VioletSpells), "", "", FALSE },
		     {	 6, &(WhiteSpells), "", "", FALSE },
		     {	 8, &(BrownSpells), "", "", FALSE },
		     {	 10, &(CompleteGraySpells), "", "", FALSE },
		     {	 11, &(BlueSpells), "", "", FALSE },
		     {	 13, &(GreenSpells), "", "", FALSE },
		     {	 15, &(MetalSpells), "", "", FALSE },
		     {	 17, &(SilverSpells), "", "", FALSE },
		     {	 19, &(PlatinumSpells), "", "", FALSE },
		     {	 20, &(RedSpells), "", "", FALSE },
		     {	 22, &(OrangeSpells), "", "", FALSE },
		     {	 24, &(BlackSpells), "", "", FALSE },
		     {	 26, &(CyanSpells), "", "", FALSE },
		     {	 28, &(BrightCyanSpells), "", "", FALSE },
		     {	 30, &(PinkSpells), "", "", FALSE },
		     {   0, 0, 0, 0, 0 } },

	glm_abil[] = { {   1, &(HDiminishedBleeding), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	nor_abil[] = { {   1, &(HCold_resistance), "", "", TRUE },
		     {	 15, &(HShock_resistance), "shock resistant", "less shock resistant", TRUE },
		     {   0, 0, 0, 0, 0 } },

	dee_abil[] = { {   1, &(HFire_resistance), "", "", TRUE },
		     {	 1, &(HShock_resistance), "", "", TRUE },
		       {   10, &(HSee_invisible), "your vision sharpen", "your vision blurring", TRUE },
		     {   0, 0, 0, 0, 0 } },

	sea_abil[] = { {   1, &(HCold_resistance), "", "", TRUE },
		     {	 1, &(HMagical_breathing), "", "", TRUE },
		     {	 1, &(HSwimming), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	spr_abil[] = { {   1, &(HFast), "", "", TRUE },
		     {	 1, &(HRegeneration), "", "", TRUE },
		     {	 1, &(HHalf_spell_damage), "", "", TRUE },
		     {	 1, &(HHalf_physical_damage), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	fay_abil[] = { {   1, &(HAntimagic), "", "", TRUE },
		     {	 1, &(HDrain_resistance), "", "", TRUE },
		     {	 1, &(Deafness), "", "", FALSE },
		     {	 1, &(HShock_resistance), "", "", TRUE },
		     {	 1, &(HPoison_resistance), "", "", TRUE },
		     {	 1, &(HAcid_resistance), "", "", TRUE },
		     {	 1, &(HFree_action), "", "", TRUE },
		     {	 1, &(HFlying), "", "", TRUE },
		     {	 1, &(HBurdenedState), "", "", FALSE },
		     {   0, 0, 0, 0, 0 } },

	syl_abil[] = { {1,  &(HHunger), "", "", FALSE },
		    {	1,  &(HSee_invisible), "", "", TRUE },
		    { 4,  &(HStealth), "stealthy", "obvious", TRUE },
		    { 6,  &(HInfravision), "perceptive", "half blind", TRUE },
		    {18, &(HDetect_monsters), "perceptive", "dull", TRUE },
		    {18, &(HAggravate_monster), "noisy", "quiet", FALSE },
		    {0, 0, 0, 0, 0} },

	gno_abil[] = { {   5, &(HStealth), "stealthy", "noisy", TRUE },
		     {   9, &(HFast), "quick", "slow", TRUE },
		     {   11, &(HSearching), "perceptive", "unaware", TRUE },
		     {   0, 0, 0, 0, 0 } },

	sdi_abil[] = { {   4, &(HFast), "quick", "slow", TRUE },
			 {	20, &(HSick_resistance), "immune to diseases", "no longer immune to diseases", TRUE },
		     {   0, 0, 0, 0, 0 } },

	hob_abil[] = { {  1, &(HStealth), "", "", TRUE },
		     {   7, &(HFast), "quick", "slow", TRUE },
		     {  15, &(HMagicFindBonus), "capable of finding magical items", "less capable of finding magical items", TRUE },
		     {   0, 0, 0, 0, 0 } },

	liz_abil[] = { {  1, &(HStone_resistance), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	hrb_abil[] = { {	 1, &(HPoison_resistance), "", "", TRUE },
			{  1, &(HFuckOverEffect), "", "", FALSE },
		     {  15, &(HWarning), "sensitive", "careless", TRUE },
		     {   0, 0, 0, 0, 0 } },

	bor_abil[] = { {  1, &(HFuckOverEffect), "", "", FALSE },
			 {   3, &(HTelepat), "disturbances in the force", "your grip on the force lessen", TRUE },
		       {   5, &(HSee_invisible), "your vision sharpen", "your vision blurring", TRUE },
		       { 0, 0, 0, 0, 0 } },

	grd_abil[] = { {	 1, &(HPoison_resistance), "", "", TRUE },
		     {  1, &(HShock_resistance), "", "", TRUE },
		     {  1, &(HControlMagic), "", "", TRUE },
		     {  0, 0, 0, 0, 0 } },

	bat_abil[] = { {  1, &(HFlying), "", "", TRUE },
		     {  1, &(HDrain_resistance), "", "", TRUE },
		     {  1, &(HFear_resistance), "", "", TRUE },
		     {  1, &(HTechnicality), "", "", TRUE },
		     {  5, &(HMysteryResist), "mysteriously resistant", "afraid of mysteries", TRUE },
		     {   0, 0, 0, 0, 0 } },

	nym_abil[] = { {  1, &(HTeleportation), "", "", FALSE },
		     {   0, 0, 0, 0, 0 } },

	met_abil[] = { {  1, &(HFire_resistance), "", "", TRUE },
		     {   1, &(HCold_resistance), "", "", TRUE },
		     {  1, &(HShock_resistance), "", "", TRUE },
		     {  1, &(HSleep_resistance), "", "", TRUE },
		     {  1, &(HDisint_resistance), "", "", TRUE },
		     {  1, &(HPoison_resistance), "", "", TRUE },
		     {  1, &(HDrain_resistance), "", "", TRUE },
		     {  1, &(HSick_resistance), "", "", TRUE },
		     {  1, &(HAntimagic), "", "", TRUE },
		     {  1, &(HAcid_resistance), "", "", TRUE },
		     {  1, &(HStone_resistance), "", "", TRUE },
		     {  1, &(HDeath_resistance), "", "", TRUE },
		     {  1, &(HPsi_resist), "", "", TRUE },
		     {  1, &(HControlMagic), "", "", TRUE },
		     {   1, &(HDefusing), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	umb_abil[] = { {  1, &(HConf_resist), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	qua_abil[] = { {  1, &(HTeleportation), "", "", FALSE },
		     {   0, 0, 0, 0, 0 } },

	dvp_abil[] = { {  1, &(HTechnicality), "", "", TRUE },
		     {	15, &(HExpBoost), "a surge of experience", "a loss of experience", TRUE },  
		     {   0, 0, 0, 0, 0 } },

	tur_abil[] = { {  1, &(HFull_nutrient), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	roh_abil[] = { {  1, &(HFast), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	thu_abil[] = { {  1, &(HFire_resistance), "", "", TRUE },
		     {   1, &(HFlying), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	dlg_abil[] = { {  1, &(HDrain_resistance), "", "", TRUE },
		     {  24, &(HMagicFindBonus), "capable of finding magical items", "less capable of finding magical items", TRUE },
		     {   0, 0, 0, 0, 0 } },

	gig_abil[] = { {  10, &(HFire_resistance), "cool", "warmer", TRUE },
		     {   15, &(HCold_resistance), "warm", "cooler", TRUE },
		     {  20, &(HShock_resistance), "insulated", "conductive", TRUE },
		     {   0, 0, 0, 0, 0 } },

	bre_abil[] = { {  1, &(HFire_resistance), "", "", TRUE },
		     {   1, &(HCold_resistance), "", "", TRUE },
		     {  1, &(HShock_resistance), "", "", TRUE },
		     {  20, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
		     {   0, 0, 0, 0, 0 } },

	elm_abil[] = { {  1, &(HFire_resistance), "", "", TRUE },
		     {   1, &(HCold_resistance), "", "", TRUE },
		     {  1, &(HShock_resistance), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	red_abil[] = { {  1, &(HSick_resistance), "", "", TRUE },
		     {   1, &(HPoison_resistance), "", "", TRUE },
		     {  1, &(HMagicVacuum), "", "", FALSE },
		     {   0, 0, 0, 0, 0 } },

	dut_abil[] = { {  1, &(HCold_resistance), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	ira_abil[] = { {  1, &(HPoison_resistance), "", "", TRUE },
		     {  1, &(HFuckOverEffect), "", "", FALSE },
		     {   12, &(HTechnicality), "technically knowledgable", "your techniques becoming weaker", TRUE },
		     {  20, &(HResistancePiercing), "able to pierce enemy resistances", "incapable of piercing enemy resistances", TRUE },
		     {	26, &(HCont_resist), "protected from contamination", "vulnerable to contamination", TRUE },
		     {   0, 0, 0, 0, 0 } },

	yok_abil[] = { {  1, &(HSick_resistance), "", "", TRUE },
		     {   1, &(HPoison_resistance), "", "", TRUE },
		     {   1, &(HFire_resistance), "", "", TRUE },
		     {  15, &(HMagicVacuum), "less capable of casting magic", "capable of casting magic more effectively", FALSE },
		     {   0, 0, 0, 0, 0 } },

	sin_abil[] = { {  1, &(HConf_resist), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	rdt_abil[] = { {  1, &(HStun_resist), "", "", TRUE },
		     {   10, &(HStun), "slightly stunned", "less stunned", FALSE },
		     {   10, &(HStunnopathy), "", "", TRUE }, /* previous one already gives message --Amy */
		     {   0, 0, 0, 0, 0 } },

	kob_abil[] = { {  1, &(HPoison_resistance), "", "", TRUE },
		     {  20, &(HMagicFindBonus), "capable of finding magical items", "less capable of finding magical items", TRUE },
		     {   0, 0, 0, 0, 0 } },

	swi_abil[] = { {  1, &(HFear_resistance), "", "", TRUE },
			{     15, &(HPeacevision), "able to recognize peaceful creatures", "unable to tell friend from enemy", TRUE },
		     {   22, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
		     {   0, 0, 0, 0, 0 } },

	coc_abil[] = { {  1, &(HPoison_resistance), "", "", TRUE },
		     {   1, &(HStone_resistance), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	sna_abil[] = { {  1, &(HPoison_resistance), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	ill_abil[] = { {  1, &(HTelepat), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	kla_abil[] = { {  1, &(HConf_resist), "", "", TRUE },
		     {   1, &(HAcid_resistance), "", "", TRUE },
		     {   7, &(HFast), "quick", "slow", TRUE },  
		     {   12, &(HFast), "quick", "slow", TRUE },  
		     {   17, &(HFast), "quick", "slow", TRUE },  
			{     20, &(HPsi_resist), "psionic", "less psionic", TRUE },
		     {   22, &(HFast), "quick", "slow", TRUE },  
		     {   27, &(HFast), "quick", "slow", TRUE },  
		     {   0, 0, 0, 0, 0 } },

	kut_abil[] = { {  1, &(HConf_resist), "", "", TRUE },
			{	 1, &(HKeen_memory), "", "", TRUE },
		     {  10, &(HMysteryResist), "mysteriously resistant", "afraid of mysteries", TRUE },
		     {  20, &(HMagicVacuum), "less capable of casting magic", "capable of casting magic more effectively", FALSE },
		     {   0, 0, 0, 0, 0 } },

	maz_abil[] = { {  1, &(HMagicVacuum), "", "", FALSE },
		     {  14, &(HResistancePiercing), "able to pierce enemy resistances", "incapable of piercing enemy resistances", TRUE },
		     {   0, 0, 0, 0, 0 } },

	vee_abil[] = { {  11, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
			 {  20, &(HResistancePiercing), "able to pierce enemy resistances", "incapable of piercing enemy resistances", TRUE },
		     {   0, 0, 0, 0, 0 } },

	urg_abil[] = { {  1, &(HMagicVacuum), "", "", FALSE },
		     {   0, 0, 0, 0, 0 } },

	nav_abil[] = { {  1, &(HPainSense), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	lep_abil[] = { {  1, &(HMagicFindBonus), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	sen_abil[] = { {  1, &(HPainSense), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	ton_abil[] = { {  1, &(HScentView), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	uni_abil[] = { {  10, &(HPainSense), "empathic", "cold-hearted", TRUE },
		     {   0, 0, 0, 0, 0 } },

	ret_abil[] = { {	 1, &(HKeen_memory), "", "", TRUE },
		     {   1, &(HAggravate_monster), "", "", FALSE },
		     {	5, &(HFuckOverEffect), "being fucked over", "not being fucked over anymore", FALSE },
			{     10, &(HPsi_resist), "psionic", "less psionic", TRUE },
			{   15, &(HManaleech), "magically attuned", "no longer magically attuned", TRUE },
		     {   0, 0, 0, 0, 0 } },

	cth_abil[] = { {   16, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
		       {   0, 0, 0, 0, 0 } },

	may_abil[] = { {   12, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
		       {   0, 0, 0, 0, 0 } },

	akt_abil[] = { {	 10, &(HFuckOverEffect), "being fucked over", "not being fucked over anymore", FALSE },
		       {   0, 0, 0, 0, 0 } },

	jab_abil[] = { {	 15, &(HFuckOverEffect), "being fucked over", "not being fucked over anymore", FALSE },
		       {   0, 0, 0, 0, 0 } },

	gav_abil[] = { {	 12, &(HFuckOverEffect), "being fucked over", "not being fucked over anymore", FALSE },
		       {   0, 0, 0, 0, 0 } },

	clt_abil[] = { {   4, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
		       {   0, 0, 0, 0, 0 } },

	out_abil[] = { {  1, &(HAcid_resistance), "", "", TRUE },
		     {  1, &(HScentView), "", "", TRUE },
		     {	10, &(HFear_resistance), "unafraid", "afraid", TRUE },
		    { 15,  &(HInfravision), "perceptive", "half blind", TRUE },
		     {   16, &(HTechnicality), "technically knowledgable", "your techniques becoming weaker", TRUE },
		     {	18, &(HCont_resist), "protected from contamination", "vulnerable to contamination", TRUE },
		     {	20, &(HPoison_resistance), "healthy", "less healthy", TRUE },
			{25, &(HMagical_breathing), "aquatic", "hydrophobic", TRUE },
		     {	 0, 0, 0, 0, 0 } },  

	spi_abil[] = { {  1, &(HPoison_resistance), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	gel_abil[] = { {  1, &(HPoison_resistance), "", "", TRUE },
		     {   1, &(HCold_resistance), "", "", TRUE },
		     {   1, &(HFire_resistance), "", "", TRUE },
		     {   1, &(HShock_resistance), "", "", TRUE },
		     {   1, &(HSleep_resistance), "", "", TRUE },
		     {   1, &(HAcid_resistance), "", "", TRUE },
		     {   1, &(HStone_resistance), "", "", TRUE },
		     {   1, &(HFull_nutrient), "", "", TRUE },
		     {   1, &(HDiscount_action), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	ins_abil[] = { {  1, &(HPoison_resistance), "", "", TRUE },
		     {   1, &(HFull_nutrient), "", "", TRUE },
		     {   1, &(HScentView), "", "", TRUE },
			{   20, &(HManaleech), "magically attuned", "no longer magically attuned", TRUE },
		     {   0, 0, 0, 0, 0 } },

	tro_abil[] = { {  1, &(HSick_resistance), "", "", TRUE },
		     {   1, &(HRegeneration), "", "", TRUE },
		     {  1, &(HMagicVacuum), "", "", FALSE },
		     {   0, 0, 0, 0, 0 } },

	ung_abil[] = { {  1, &(HSearching), "", "", TRUE },
		     {   1, &(HWarning), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	gas_abil[] = { {  1, &(HSick_resistance), "", "", TRUE },
		     {   1, &(HCold_resistance), "", "", TRUE },
		     {   1, &(HSleep_resistance), "", "", TRUE },
		     {   1, &(HPoison_resistance), "", "", TRUE },
		     {   1, &(HDiminishedBleeding), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	mum_abil[] = { {  1, &(HSick_resistance), "", "", TRUE },
		     {   1, &(HCold_resistance), "", "", TRUE },
		     {   1, &(HSleep_resistance), "", "", TRUE },
		     {   1, &(HPoison_resistance), "", "", TRUE },
		     {   1, &(HUndead_warning), "", "", TRUE },
		     {   1, &(HDiminishedBleeding), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	din_abil[] = { {  1, &(HDiscount_action), "", "", TRUE },
		     {  1, &(HFuckOverEffect), "", "", FALSE },
		     {   1, &(HScentView), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	ske_abil[] = { {  1, &(HSick_resistance), "", "", TRUE },
		     {   1, &(HCold_resistance), "", "", TRUE },
		     {   1, &(HSleep_resistance), "", "", TRUE },
		     {   1, &(HPoison_resistance), "", "", TRUE },
		     {   1, &(HStone_resistance), "", "", TRUE },
		     {   1, &(HMagical_breathing), "", "", TRUE },
		     {   1, &(HDiminishedBleeding), "", "", TRUE },
		     {  17, &(HMysteryResist), "mysteriously resistant", "afraid of mysteries", TRUE },
		     {   0, 0, 0, 0, 0 } },

	lev_abil[] = { {  1, &(HTelepat), "", "", TRUE },
		     {   1, &(HHallu_party), "", "", TRUE },
		     {   1, &(HDrunken_boxing), "", "", TRUE },
		     {   1, &(HStunnopathy), "", "", TRUE },
		     {   1, &(HNumbopathy), "", "", TRUE },
		     {   1, &(HDimmopathy), "", "", TRUE },
		     {   1, &(HFreezopathy), "", "", TRUE },
		     {   1, &(HStoned_chiller), "", "", TRUE },
		     {   1, &(HCorrosivity), "", "", TRUE },
		     {   1, &(HFear_factor), "", "", TRUE },
		     {   1, &(HBurnopathy), "", "", TRUE },
		     {   1, &(HSickopathy), "", "", TRUE },
		     {   1, &(HWonderlegs), "", "", TRUE },
		     {   1, &(HGlib_combat), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	wra_abil[] = { {  1, &(HSick_resistance), "", "", TRUE },
		     {   1, &(HCold_resistance), "", "", TRUE },
		     {   1, &(HSleep_resistance), "", "", TRUE },
		     {   1, &(HPoison_resistance), "", "", TRUE },
		     {   1, &(HStone_resistance), "", "", TRUE },
		     {   1, &(HUndead_warning), "", "", TRUE },
		     {   1, &(HFlying), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	lic_abil[] = { {  1, &(HSick_resistance), "", "", TRUE },
		     {   1, &(HCold_resistance), "", "", TRUE },
		     {   1, &(HSleep_resistance), "", "", TRUE },
		     {   1, &(HPoison_resistance), "", "", TRUE },
		     {   1, &(HUndead_warning), "", "", TRUE },
		     {   1, &(HRegeneration), "", "", TRUE },
		     {   1, &(HTeleportation), "", "", FALSE },
		     {   1, &(HTeleport_control), "", "", TRUE },
		     {   1, &(HMysteryResist), "", "", TRUE },
		     {  24, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
		     {   0, 0, 0, 0, 0 } },

	jel_abil[] = { {	 1, &(HPoison_resistance), "", "", TRUE },
		     {   1, &(HSleep_resistance), "", "", TRUE },
			{   1, &(HManaleech), "", "", TRUE },
			{   1, &(HDiscount_action), "", "", TRUE },
		     {  8, &(HScentView), "your sense of smell expanding", "less capable of smelling things", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	wim_abil[] = { {	 1, &(HFire_resistance), "", "", TRUE },
		     {   1, &(HRegeneration), "", "", TRUE },
		     {   1, &(HTeleportation), "", "", FALSE },
		     {	 0, 0, 0, 0, 0 } },

	trn_abil[] = { {	 1, &(HExtra_wpn_practice), "", "", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	nem_abil[] = { { 18, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	zau_abil[] = { {	 1, &(HStone_resistance), "", "", TRUE },
		     {   1, &(HSwimming), "", "", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	wyl_abil[] = { {	 1, &(HScentView), "", "", TRUE },
		     {  12, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	thr_abil[] = { {	 1, &(HSearching), "", "", TRUE },
			{	 1, &(HStealth), "", "", TRUE },
		     {  20, &(HMagicFindBonus), "capable of finding magical items", "less capable of finding magical items", TRUE },
		     {  20, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	han_abil[] = { {	 1, &(HDrain_resistance), "", "", TRUE },
			 { 20, &(HFlying), "weightless", "grounded", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	cup_abil[] = { { 20, &(HFlying), "weightless", "grounded", TRUE },
		     {   0, 0, 0, 0, 0 } },

	bac_abil[] = { {	1, &(HControlMagic), "", "", TRUE },
			 {	10, &(HSick_resistance), "immune to diseases", "no longer immune to diseases", TRUE },
		     {  15, &(HResistancePiercing), "able to pierce enemy resistances", "incapable of piercing enemy resistances", TRUE },
		     {	20, &(HCont_resist), "protected from contamination", "vulnerable to contamination", TRUE },  
		     {   0, 0, 0, 0, 0 } },

	she_abil[] = { { 20, &(HFire_resistance), "cool", "warmer", TRUE },
		     {	20, &(HPoison_resistance), "healthy", "less healthy", TRUE },
		     {   20, &(HAcid_resistance), "warded", "endangered", TRUE },
		     {  20, &(HSwimming), "ready to swim","afraid of the water", TRUE },
			{   20, &(HSee_invisible), "your vision sharpen", "your vision blurring", TRUE },
		    { 20,  &(HInfravision), "perceptive", "half blind", TRUE },
		     {	20, &(HControlMagic), "magic-controlled", "no longer magic-controlled", TRUE },
		     {  20, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
		     {   0, 0, 0, 0, 0 } },

	cer_abil[] = { { 1, &(HFire_resistance), "", "", TRUE },
		     {  1, &(HScentView), "", "", TRUE },
		     {	12, &(HControlMagic), "magic-controlled", "no longer magic-controlled", TRUE },
		     {   0, 0, 0, 0, 0 } },

	hou_abil[] = { {  1, &(HScentView), "", "", TRUE },
		     {	3, &(HControlMagic), "magic-controlled", "no longer magic-controlled", TRUE },
		     {   0, 0, 0, 0, 0 } },

	woo_abil[] = { {  1, &(HScentView), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	sly_abil[] = { {  1, &(HFire_resistance), "", "", TRUE },
			 {  1, &(HSick_resistance), "", "", TRUE },
		     {  1, &(HFear_resistance), "", "", TRUE },
		     {   1, &(HPoison_resistance), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	wis_abil[] = { {  1, &(HControlMagic), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	exp_abil[] = { {	 1, &(HPsi_resist), "", "", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	stb_abil[] = { {	 1, &(HDisint_resistance), "", "", TRUE },
		     {	 0, 0, 0, 0, 0 } },

	rod_abil[] = { {   1, &(HFire_resistance), "", "", TRUE },
		     {   1, &(HPoison_resistance), "", "", TRUE },
		     {   1, &(HMagical_breathing), "", "", TRUE },
		     {   1, &(HRegeneration), "", "", TRUE },
		     {   1, &(HTeleportation), "", "", FALSE },
		     {   1, &(HTeleport_control), "", "", TRUE },
		     {   1, &(HSee_invisible), "", "", TRUE },
		     {   1, &(HFuckOverEffect), "", "", FALSE },
			{   1, &(HSpellboost), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	esp_abil[] = { {  1, &(HSick_resistance), "", "", TRUE },
		     {   1, &(HCold_resistance), "", "", TRUE },
		     {   1, &(HSleep_resistance), "", "", TRUE },
		     {   1, &(HPoison_resistance), "", "", TRUE },
		     {   1, &(HDisint_resistance), "", "", TRUE },
		     {   1, &(HStone_resistance), "", "", TRUE },
		     {   1, &(HUndead_warning), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	eth_abil[] = { {  1, &(HStone_resistance), "", "", TRUE },
		     {   1, &(HDisint_resistance), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	ico_abil[] = { {  1, &(HStone_resistance), "", "", TRUE },
		     {   1, &(HDisint_resistance), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	lyc_abil[] = { {   1, &(HPoison_resistance), "", "", TRUE },
		     {   1, &(HRegeneration), "", "", TRUE },
		     {   1, &(HScentView), "", "", TRUE },
		     {   7, &(HStealth), "stealthy", "noisy", TRUE },
		     {   0, 0, 0, 0, 0 } },

	aur_abil[] = { {   1, &(HReflecting), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	sat_abil[] = { {   1, &(HTechnicality), "", "", TRUE },
		     {   1, &(HScentView), "", "", TRUE },
		     {   1, &(HControlMagic), "", "", TRUE },
		     {  18, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
		     {  20, &(HMagicFindBonus), "capable of finding magical items", "less capable of finding magical items", TRUE },
		     {   0, 0, 0, 0, 0 } },

	sho_abil[] = { {   1, &(HDiscount_action), "", "", TRUE },
		     {  15, &(HMysteryResist), "mysteriously resistant", "afraid of mysteries", TRUE },
		     {   0, 0, 0, 0, 0 } },

	ink_abil[] = { {   1, &(HPeacevision), "", "", TRUE },
		     {   1, &(HTechnicality), "", "", TRUE },
		     {   1, &(HScentView), "", "", TRUE },
		     {   6, &(HPainSense), "empathic", "cold-hearted", TRUE },
		     {  10, &(HSpellboost), "a surge of spell power", "a reduction of spell power", TRUE },
		     {  20, &(HDefusing), "capable of defusing traps", "unable to defuse traps", TRUE },
		     {   0, 0, 0, 0, 0 } },

	tay_abil[] = { {   20, &(HPainSense), "empathic", "cold-hearted", TRUE },
		     {  23, &(HMysteryResist), "mysteriously resistant", "afraid of mysteries", TRUE },
		     {   0, 0, 0, 0, 0 } },

	sed_abil[] = { {   1, &(HAntimagic), "", "", TRUE },
		     {   0, 0, 0, 0, 0 } },

	orc_abil[] = { {	1, &(HPoison_resistance), "", "", TRUE },
		     {	 0, 0, 0, 0, 0 } };

/*static long next_check = 600L;*/	/* arbitrary first setting */
STATIC_DCL void exerper(void);
STATIC_DCL void postadjabil(long *);

/* adjust an attribute; return TRUE if change is made, FALSE otherwise */
boolean
adjattrib(ndx, incr, msgflg, canresist)
	int	ndx, incr;
	int	msgflg;
	boolean	canresist;

/* 3 => no message at all and no development message, 2 => no message at all (but development can be given),
 * 1 => no message except encumber, zero => message, and negative => conditional (msg if change made) */
{
	if ((SustainAbilityOn && ((incr > 0) || SustainLossSafe)) || !incr) return FALSE;

	if ((ndx == A_INT || ndx == A_WIS) && uarmh && uarmh->otyp == DUNCE_CAP && ((incr > 0) || rn2(5)) ) {
		if (msgflg == 0)
		    Your("cap constricts briefly, then relaxes again.");
		return FALSE;
	}

	/* Mithril items can sometimes prevent the player's stats from decreasing --Amy
	 * also, astronauts have such extreme body training for going into space that they're able to resist */
	if (incr < 0 && canresist) {

		if (uarmg && uarmg->otyp == GAUNTLETS_OF_PHYSICAL_SUSTENAN && ((ndx == A_STR) || (ndx == A_DEX)) ) {
			Your("gauntlets prevent the stat loss!");
			makeknown(uarmg->otyp);
			return FALSE;
		}

		if (ublindf && ublindf->oartifact == ART_LEANDRA_S_WORD && !rn2(2)) {
			Your("lenses prevent the stat loss!");
			return FALSE;
		}

		if (uarmf && uarmf->oartifact == ART_LEGENDARY_JESSICA && !rn2(2)) {
			Your("sandals prevent the stat loss!");
			return FALSE;
		}

		if (FemtrapActiveChloe && uarmf && uarmf->otyp == CHLOE_BALL_HEELS && (ndx == A_CHA)) {
			Your("beauty cannot be diminished!");
			return FALSE;
		}

		if (uarmc && uarmc->oartifact == ART_INTERNAL_CLEAR && (ndx == A_INT)) {
			Your("cloak prevents the intelligence loss!");
			return FALSE;
		}

		if (uamul && uamul->otyp == AMULET_OF_SUSTAINED_MIND && ((ndx == A_INT) || (ndx == A_WIS)) ) {
			Your("amulet prevents the stat loss!");
			makeknown(uamul->otyp);
			return FALSE;
		}

		if (uamul && uamul->oartifact == ART_BODYPURGE && rn2(10)) {
			Your("amulet prevents the stat loss!");
			return FALSE;
		}

		if (uleft && uleft->otyp == RIN_UPPER_ADORNMENT && ndx == A_CHA) return FALSE;
		if (uright && uright->otyp == RIN_UPPER_ADORNMENT && ndx == A_CHA) return FALSE;

		if (Role_if(PM_ASTRONAUT) && rn2(2)) {
			pline("Your steeled body prevents the stat loss!");
			return FALSE;
		}

		if (uarmh && uarmh->oartifact == ART_SAMIRA_S_EXPRESSION && !rn2(2)) {
			Your("helmet prevents the stat loss!");
			return FALSE;
		}

		if (uarm && uarm->oartifact == ART_CON_HOLD && ndx == A_CON) {
			Your("armor prevents the constitution drain!");
			return FALSE;
		}

		int mithrilitemcount = 0;

		if (uwep && objects[uwep->otyp].oc_material == MT_MITHRIL) mithrilitemcount++;
		if (u.twoweap && uswapwep && objects[uswapwep->otyp].oc_material == MT_MITHRIL) mithrilitemcount++;
		if (uarm && objects[uarm->otyp].oc_material == MT_MITHRIL) mithrilitemcount++;
		if (uarmc && objects[uarmc->otyp].oc_material == MT_MITHRIL) mithrilitemcount++;
		if (uarmh && objects[uarmh->otyp].oc_material == MT_MITHRIL) mithrilitemcount++;
		if (uarms && objects[uarms->otyp].oc_material == MT_MITHRIL) mithrilitemcount++;
		if (uarmg && objects[uarmg->otyp].oc_material == MT_MITHRIL) mithrilitemcount++;
		if (uarmf && objects[uarmf->otyp].oc_material == MT_MITHRIL) mithrilitemcount++;
		if (uarmu && objects[uarmu->otyp].oc_material == MT_MITHRIL) mithrilitemcount++;
		if (uamul && objects[uamul->otyp].oc_material == MT_MITHRIL) mithrilitemcount++;
		if (uimplant && objects[uimplant->otyp].oc_material == MT_MITHRIL) mithrilitemcount++;
		if (uleft && objects[uleft->otyp].oc_material == MT_MITHRIL) mithrilitemcount++;
		if (uright && objects[uright->otyp].oc_material == MT_MITHRIL) mithrilitemcount++;
		if (ublindf && objects[ublindf->otyp].oc_material == MT_MITHRIL) mithrilitemcount++;

		if (mithrilitemcount > 0 && (mithrilitemcount >= rnd(20))) {
			pline("Your mithril equipment prevents the stat loss!");
			return FALSE;
		}

	}

	if (incr > 0) {
	    if ((AMAX(ndx) >= ATTRMAX(ndx)) && (ACURR(ndx) >= AMAX(ndx))) {
		if (msgflg == 0 && flags.verbose)
		    pline("You're already as %s as you can get.",
			  plusattr[ndx]);
		ABASE(ndx) = AMAX(ndx) = ATTRMAX(ndx); /* just in case */
		return FALSE;
	    }

		/* you won't always get the increase if the attribute is already rather high --Amy */
	    if (!attr_will_go_up(ndx, msgflg <= 2 ? TRUE : FALSE)) return FALSE;

	    ABASE(ndx) += incr;
	    if(ABASE(ndx) > AMAX(ndx)) {
		incr = ABASE(ndx) - AMAX(ndx);
		AMAX(ndx) += incr;
		if(AMAX(ndx) > ATTRMAX(ndx))
		    AMAX(ndx) = ATTRMAX(ndx);
		ABASE(ndx) = AMAX(ndx);
	    }
	} else {
	    if (ABASE(ndx) <= ATTRABSMIN(ndx)) {
		if (msgflg == 0 && flags.verbose)
		    pline("You're already as %s as you can get.",
			  minusattr[ndx]);

		/* Amy edit: granted, stat instadeath would be kinda BS here... but you should at least get *some* penalty
		 * for playing badly enough to allow a stat to be drained that low! So, we're reducing your alla. */
		You_feel("a drain on your soul...");
		drain_alla(abs(incr));

		ABASE(ndx) = ATTRABSMIN(ndx); /* just in case */
		return FALSE;
	    }

	    ABASE(ndx) += incr;
	    if(ABASE(ndx) < ATTRABSMIN(ndx)) {
		incr = ABASE(ndx) - ATTRABSMIN(ndx);
		ABASE(ndx) = ATTRABSMIN(ndx);
		AMAX(ndx) += incr;
		if(AMAX(ndx) < ATTRABSMIN(ndx))
		    AMAX(ndx) = ATTRABSMIN(ndx);
	    }
	}
	if (msgflg <= 0)
	    You_feel("%s%s!",
		  (incr > 1 || incr < -1) ? "very ": "",
		  (incr > 0) ? plusattr[ndx] : minusattr[ndx]);

	if ( (msgflg == 0) && (incr < 0) ) {
		pline("You lose  %s", ndx == 0 ? "Strength" : ndx == 1 ? "Intelligence" : ndx == 2 ? "Wisdom" : ndx == 3 ? "Dexterity" : ndx == 4 ? "Constitution" : "Charisma");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Pochemu vy ne sledite luchshe dlya vashikh atributov, tak ili inache?" : "Due-l-ue-l-ue-l!");
	}

	flags.botl = 1;
	if (program_state.in_moveloop && msgflg <= 1 && moves > 1 && (ndx == A_STR || ndx == A_CON))
		(void)encumber_msg();
	return TRUE;
}

void
gainstr(otmp, incr)
	register struct obj *otmp;
	register int incr;
{
	int num = 1;

	if(incr) num = incr;
	else {
	    if (ABASE(A_STR) < 3) num = (rn2(3) ? rnd(3) : rnd(6) );
	    else if (ABASE(A_STR) < 4) num = (rn2(4) ? rnd(2) : rnd(6) );
	    else if(ABASE(A_STR) < 18) num = (rn2(4) ? 1 : rnd(6) );
	    else if (ABASE(A_STR) < STR18(85)) num = rnd(10);
	}
	(void) adjattrib(A_STR, (otmp && otmp->cursed) ? -num : num, TRUE, TRUE);
}

void
losestr(num, canresist)	/* may kill you; cause may be poison or monster like 'a' */
	register int num;
	boolean canresist;
{
	int ustr = ABASE(A_STR) - num;

	int hpreduce = rnd(5);

	if (num > 0) pline("%d points of your strength got sapped!",num);
	if (num < 0) pline("%d points of your strength got restored!",-num);

	if (ustr < 1) pline("Since you don't have enough strength, your health is being sapped instead.");

	while(ustr < 1) {
	    ++ustr;
	    --num;
	    hpreduce = rnd(5);
	    if (Upolyd) {
		u.mh -= hpreduce;
		u.mhmax -= hpreduce;
		if (u.mh < 1) u.mh = 1;
		if (u.mhmax < 1) {
			u.mhmax = 1;
			drain_alla(100);
			pline("A severe drain seems to be sucking out your soul...");
		}
	    } else {
		u.uhp -= hpreduce;
		u.uhpmax -= hpreduce;
		if (u.uhp < 1) u.uhp = 1;
		if (u.uhpmax < 1) {
			u.uhpmax = 1;
			drain_alla(100);
			pline("A severe drain seems to be sucking out your soul...");
		}
	    }
	}

	(void) adjattrib(A_STR, -num, TRUE, canresist);
}

void
change_luck(n)
	register schar n;
{
	u.uluck += n;
	if (u.uluck < 0 && u.uluck < LUCKMIN) {
		u.uluck = LUCKMIN;
	}
	if (u.uluck > 0 && u.uluck > LUCKMAX) {
		if (LuckLoss || u.uprops[LUCK_LOSS].extrinsic || autismweaponcheck(ART_PROFANED_GREATSCYTHE) || (ublindf && ublindf->oartifact == ART_GIRLFRIEND_DOESN_T_WANNA_C) || have_unluckystone() || (uarmf && uarmf->oartifact == ART_SUCCESS_LESS_SAVING) ) {
			u.uluck = LUCKMIN;
			You_feel("something turning around..."); /* extra vague message (evil patch idea by jonadab) */
			u.cnd_luckrollovercount++; /* probably extremely rare, but let's track it anyway... --Amy */
		} else {
			u.uluck = LUCKMAX;
		}
	}
}

int
stone_luck(parameter)
boolean parameter; /* So I can't think up of a good name.  So sue me. --KAA */
{
	register struct obj *otmp;
	register long bonchance = 0;

	for (otmp = invent; otmp; otmp = otmp->nobj)
	    if (confers_luck(otmp)) {
		if (otmp->cursed) bonchance -= otmp->quan;
		else if (otmp->blessed) {bonchance += otmp->quan; bonchance += otmp->quan;}
		else if (parameter) bonchance += otmp->quan;
	    }

	/* STEPHEN WHITE'S NEW CODE */
	if (uarmh && uarmh->otyp == FEDORA && !uarmh->cursed) bonchance += 2;

	if (uarmg && uarmg->otyp == GAUNTLETS_OF_GOOD_FORTUNE && !uarmg->cursed) bonchance += 2;

	/* that sign int long conversion seems to screw over the calculation, discovered by Dikkin.
	 * This is supposed to be a fix. --Amy */
	if (bonchance > 50) bonchance = 50;
	if (bonchance < -50) bonchance = -50;

	u.moreluckpts = bonchance;
	
	return sgn((int)bonchance);
}

/* there has just been an inventory change affecting a luck-granting item */
void
set_moreluck()
{
	int luckbon = stone_luck(TRUE);

	if (!luckbon && !carrying(LUCKSTONE)) u.moreluck = 0;
	else if (luckbon >= 0 && u.moreluckpts <= 3) u.moreluck = u.moreluckpts; /* not always give a flat +3 --Amy */
	else if (luckbon >= 0 && u.moreluckpts > 3) u.moreluck = LUCKADD;
	else u.moreluck = -LUCKADD;
	if (uwep && uwep->oartifact == ART_WIRE_OF_LUCK) u.moreluck += 5;
	if (ublindf && ublindf->oartifact == ART_GOOD_TO_SEE_NOTHING) u.moreluck += 5;
	if (uarmh && uarmh->otyp == LUCKY_MUSHROOM) u.moreluck += 5;
	if (uarmf && uarmf->oartifact == ART_NATALIA_S_GREAT_LUCK) u.moreluck += 5;
	if (have_amateurluckstone()) u.moreluck += 5;
	if (uarmf && uarmf->oartifact == ART_HAPPY_CLOUD) u.moreluck += 5;
	if (have_suckstonearti()) u.moreluck += 5;
	if (uarmh && uarmh->oartifact == ART_XTRALUCK) u.moreluck += 3;
	if (uwep && uwep->oartifact == ART_LUCKLESS_FOLLY) u.moreluck -= 3;
	if (u.twoweap && uswapwep && uswapwep->oartifact == ART_LUCKLESS_FOLLY) u.moreluck -= 3;
	if (uarm && uarm->oartifact == ART_AMMY_S_COMPLAINT) u.moreluck -= 5;
	if (uamul && uamul->oartifact == ART_HIBA_CHIBA) u.moreluck += 5;
	if (uarmc && uarmc->oartifact == ART_LYCIA_S_WUSH) u.moreluck += 5;
	if (uwep && uwep->enchantment == WEAPEGO_LUCK) u.moreluck += 1;
	if (uwep && uwep->enchantment == WEAPEGO_GREAT_LUCK) u.moreluck += 2;
	if (uwep && uwep->enchantment == WEAPEGO_INCREDIBLE_LUCK) u.moreluck += 3;
	if (uarmh && uarmh->oartifact == ART_VERCHANGE_CHILD) u.moreluck += 3;
	if (uimplant && uimplant->oartifact == ART_HAPPY_IGNORANCE) u.moreluck += 13;
	if (uchain && uchain->oartifact == ART_OLEA_PRETIOSA) u.moreluck += 2;
	if (bmwride(ART_WI_O_WI_O_WACKIWE_)) u.moreluck += 1;
	if (uarmc && uarmc->oartifact == ART_NUDE_PUNAM) u.moreluck += 5;
	if (uimplant && uimplant->oartifact == ART_LUCK_WEAVE_OF_ARDENT_SOLWY) u.moreluck += 2;
	if (uimplant && uimplant->oartifact == ART_LUNG_INFUSER_OF_LINK) u.moreluck += 2;
	if (uarmc && uarmc->oartifact == ART_KING_SAYS_WHERE_IT_GOES) u.moreluck += 3;

}


/* KMH, balance patch -- new function for healthstones
 * Amy balance "nerf": make the boost increase more slowly if you have several stones */
void
recalc_health()
{
	register struct obj *otmp;


	u.uhealbonus = 0;

	for(otmp = invent; otmp; otmp=otmp->nobj)
	    if (otmp->otyp == HEALTHSTONE) {
	    	u.uhealbonus += otmp->quan *
	    			(otmp->blessed ? 2 : otmp->cursed ? -2 : 1);
		if (otmp->oartifact == ART_STONE_OF_HEALTH) u.uhealbonus += (otmp->quan * 5);
		if (otmp->oartifact == ART_STONE_OF_ROTTING) u.uhealbonus -= (otmp->quan * 5);
	    }

	if (uarmh && uarmh->oartifact == ART_NIAMH_PENH) u.uhealbonus += 2;
	if (uwep && uwep->oartifact == ART_SYLVIE_S_EASY_MODE) u.uhealbonus += 10;
	if (uarmc && uarmc->oartifact == ART_VERENA_S_CLOAK_OF_BLOOMING && levl[u.ux][u.uy].typ >= WELL) u.uhealbonus += 10;

	if (u.uhealbonus > 1) u.uhealbonus = rno(u.uhealbonus);

	return;
}

int
recalc_mana()

{
	register struct obj *otmp;
	int manabonus = 0;

	for(otmp = invent; otmp; otmp=otmp->nobj)
	    if (otmp->otyp == MANASTONE)
	    	manabonus += otmp->quan *
	    			(otmp->blessed ? 2 : otmp->cursed ? -2 : 1);

	if (uchain && uchain->oartifact == ART_OLEA_PRETIOSA) manabonus += 5;
	if (uwep && uwep->oartifact == ART_SYLVIE_S_EASY_MODE) manabonus += 5;

	if (manabonus > 1) manabonus = rno(manabonus);

	return manabonus;
}


#endif /* OVLB */
#ifdef OVL1

void
restore_attrib()
{
	int	i;

	for(i = 0; i < A_MAX; i++) {	/* all temporary losses/gains */

	   if(ATEMP(i) && ATIME(i)) {
		if(!(--(ATIME(i)))) { /* countdown for change */
		    ATEMP(i) += ATEMP(i) > 0 ? -1 : 1;

		    if(ATEMP(i)) /* reset timer */
			ATIME(i) = 100 / ACURR(A_CON);
		}
	    }
	}
	(void)encumber_msg();
}

#endif /* OVL1 */
#ifdef OVLB

#define AVAL	50		/* tune value for exercise gains */

void
exercise(i, inc_or_dec)
int	i;
boolean	inc_or_dec;
{
#ifdef DEBUG
	pline("Exercise:");
#endif
	if (i == A_INT || i == A_CHA) return;	/* can't exercise these */

	/* no physical exercise while polymorphed; the body's temporary */
	/* edit by Amy - let ungenomolds have some fun. */
	/*if (Upolyd && i != A_WIS) return;*/

	if(abs(AEXE(i)) < AVAL) {
		/*
		 *	Law of diminishing returns (Part I):
		 *
		 *	Gain is harder at higher attribute values.
		 *	79% at "3" --> 0% at "18"
		 *	Loss is even at all levels (50%).
		 *
		 *	Note: *YES* ACURR is the right one to use.
		 */

		if (inc_or_dec && Race_if(PM_MAGYAR)) return;

		if (inc_or_dec && Race_if(PM_TRAINER) && rn2(10)) return;

		if (inc_or_dec && Race_if(PM_DUNADAN) && rn2(2)) return;

		if (inc_or_dec && Race_if(PM_PLAYER_SKELETON) && rn2(5)) return;

		/* note by Amy - it's stupid if you can only lose your attribute points if they're greater than 18. */

		AEXE(i) += (inc_or_dec) ? (( (rn2(19) > ACURR(i) ) || !rn2(10) ) && !(PlayerCannotExerciseStats || (uarms && uarms->oartifact == ART_NULL_) || u.uprops[EXERCISE_DEACTIVATED].extrinsic || have_exercisestone()) ) : -rn2(2);

		if (uarms && uarms->oartifact == ART_HIGHER_TRAINING && inc_or_dec && !rn2(2)) AEXE(i) += 1;
		if (uarmf && uarmf->oartifact == ART_BRITTA_S_MURDER_STORY && inc_or_dec) AEXE(i) += 2;

		if (Extra_wpn_practice) 
			AEXE(i) += (inc_or_dec) ? (( (rn2(19) > ACURR(i) ) || !rn2(10) ) && !(PlayerCannotExerciseStats || (uarms && uarms->oartifact == ART_NULL_) || u.uprops[EXERCISE_DEACTIVATED].extrinsic || have_exercisestone()) ) : 0;

		if (StrongExtra_wpn_practice) 
			AEXE(i) += (inc_or_dec) ? (( (rn2(19) > ACURR(i) ) || !rn2(10) ) && !(PlayerCannotExerciseStats || (uarms && uarms->oartifact == ART_NULL_) || u.uprops[EXERCISE_DEACTIVATED].extrinsic || have_exercisestone()) ) : 0;

#ifdef DEBUG
		pline("%s, %s AEXE = %d",
			(i == A_STR) ? "Str" : (i == A_WIS) ? "Wis" :
			(i == A_DEX) ? "Dex" : "Con",
			(inc_or_dec) ? "inc" : "dec", AEXE(i));
#endif
	}
	if (moves > 0 && (i == A_STR || i == A_CON)) (void)encumber_msg();
}

/* hunger values - from eat.c */
#define SATIATED	0 /* at least 2500 */
#define NOT_HUNGRY	1 /* at least 500 */
#define HUNGRY		2 /* at least 200 */
#define WEAK		3 /* at least 0 */
#define FAINTING	4
#define FAINTED		5
#define STARVED		6

STATIC_OVL void
exerper()
{
	/* changes by Amy to make these happen less often, because they shouldn't dominate attribute training/abuse */

	if(!rn2(10)) {
		/* Hunger Checks */

		int hs = (u.uhunger > 2500) ? SATIATED :
			 (u.uhunger > 500) ? NOT_HUNGRY :
			 (u.uhunger > 200) ? HUNGRY :
			 (u.uhunger > 0) ? WEAK : FAINTING;

#ifdef DEBUG
		pline("exerper: Hunger checks");
#endif
		switch (hs) {
		    case SATIATED:

					if ((u.uhunger > 4500) || (u.uhunger > 4000 && rn2(4)) || (u.uhunger <= 4000 && u.uhunger > 3500 && rn2(2)) || (u.uhunger <= 3500 && u.uhunger > 3000 && !rn2(5)) || (u.uhunger <= 3000 && !rn2(20)) ) {

					exercise(A_DEX, FALSE);
					if (Role_if(PM_MONK) || Role_if(PM_HALF_BAKED))
					    exercise(A_WIS, FALSE);

					}
					if (have_anorexia() || Role_if(PM_FAILED_EXISTENCE)) { /* They strongly dislike being full --Amy */
					    exercise(A_WIS, FALSE); exercise(A_STR, FALSE); exercise(A_CON, FALSE); exercise(A_DEX, FALSE); }
					break;
		    case NOT_HUNGRY:	exercise(A_CON, TRUE); break;
		    case HUNGRY:	if (have_anorexia() || Role_if(PM_FAILED_EXISTENCE)) exercise(A_WIS, TRUE); break;
		    case WEAK:
					if (!have_anorexia() && !rn2(10) && !Role_if(PM_FAILED_EXISTENCE) ) exercise(A_STR, FALSE);
					if (Role_if(PM_MONK) || Role_if(PM_HALF_BAKED))	/* fasting */
					    exercise(A_WIS, TRUE);
					if (have_anorexia() || Role_if(PM_FAILED_EXISTENCE)) {
					    exercise(A_WIS, TRUE); exercise(A_STR, TRUE);
}					break;
		    case FAINTING:
		    case FAINTED:
					if (have_anorexia() || Role_if(PM_FAILED_EXISTENCE)) { exercise(A_WIS, TRUE); exercise(A_STR, TRUE); exercise(A_DEX, TRUE);		}					
					if (!have_anorexia() && !rn2(10) && !Role_if(PM_FAILED_EXISTENCE)) exercise(A_CON, FALSE); break;
		}

		/* Encumberance Checks */
#ifdef DEBUG
		pline("exerper: Encumber checks");
#endif
		switch (near_capacity()) {
		    case MOD_ENCUMBER:	exercise(A_STR, TRUE); break;
		    case HVY_ENCUMBER:	exercise(A_STR, TRUE);
					exercise(A_DEX, FALSE); break;
		    case EXT_ENCUMBER:	exercise(A_DEX, FALSE);
					exercise(A_CON, FALSE); break;
		}

	}

	/* status checks */
	if(!rn2(50)) {
#ifdef DEBUG
		pline("exerper: Status checks");
#endif
		/* KMH, intrinsic patch */
		if ((HClairvoyant & (INTRINSIC|TIMEOUT)) &&
			!BClairvoyant)                      exercise(A_WIS, TRUE);
		if (HRegeneration)			exercise(A_STR, TRUE);

		if(Sick || Vomiting)     exercise(A_CON, FALSE);
		if( (Confusion && !Conf_resist) || Hallucination || (Dimmed && !rn2(3)) || (Feared && !rn2(5)) )		exercise(A_WIS, FALSE);
		if( (Numbed && !rn2(3)) || Frozen || (Burned && !rn2(2)) )		exercise(A_CON, FALSE);
		if((Wounded_legs && !u.usteed ) || Fumbling || (HStun && !Stun_resist) )	exercise(A_DEX, FALSE);
	}
}

void
exerchk()
{
	int	i, mod_val;
	boolean contaminated;
	boolean verycontaminated;

	int goupbuust = 0;

	/*	Check out the periodic accumulations */
	exerper();

#ifdef DEBUG
	if(moves >= u.next_check)
		pline("exerchk: ready to test. multi = %d.", multi);
#endif
	/*	Are we ready for a test?	*/
	if(moves >= u.next_check && !multi) {
#ifdef DEBUG
	    pline("exerchk: testing.");
#endif
	    /*
	     *	Law of diminishing returns (Part II):
	     *
	     *	The effects of "exercise" and "abuse" wear
	     *	off over time.  Even if you *don't* get an
	     *	increase/decrease, you lose some of the
	     *	accumulated effects.
	     *
	     *  Note that if you are polymorphed then the
	     *  effects of any physical exercise done in your
	     *  own body will just wear off with no checking
	     *  until you return to your natural form.
	     */

	/* contamination is guaranteed to damage wisdom if at least 100 points accumulated on the contamination counter;
	 * otherwise, have a chance of damaging it but also a chance of it going down on its own --Amy */
		contaminated = FALSE;
		verycontaminated = FALSE;

		if (u.contamination >= 100) {
			contaminated = TRUE;
			verycontaminated = TRUE;
		}

		if (u.contamination && u.contamination < 100) { /* low chance when only slightly contaminated --Amy */
			contaminated = (rn2(100) < u.contamination);
			if (contaminated) contaminated = (rn2(100) < u.contamination);
			/* if you get lucky, very low contamination fixes itself --Amy */
			if (!contaminated && !rn2(2)) decontaminate(rnd(u.contamination));
		}

	    for(i = 0; i < A_MAX; AEXE(i++) /= 2) {

		if (contaminated && i == A_WIS) {
			pline(FunnyHallu ? "You have a severe coughing fit and an intense desire to vomit!" : "You are consumed by your contamination!");

			/* Sustainer race has no way of improving wisdom and should therefore be resistant --Amy */
			if (Race_if(PM_SUSTAINER) && rn2(10)) pline("But you resist the effects!");
			else {
				pline("You lose  Wisdom");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Pochemu vy ne sledite luchshe dlya vashikh atributov, tak ili inache?" : "Due-l-ue-l-ue-l!");
				ABASE(A_WIS) -= 1;
				AMAX(A_WIS) -= 1;
				flags.botl = 1;

				if (uarmh && uarmh->oartifact == ART_BURN_INTO_THE_MIND) mightbooststat(A_INT);

				if(ABASE(A_WIS) < ATTRABSMIN(A_WIS)) { /* you have died */

					/* it's a total crapshoot if any little contamination can instakill you. I decided that
					 * if it's less than 100, you can't be instakilled but the contamination may go down. --Amy */
					if (u.contamination < 100) {
						if(ABASE(A_WIS) < ATTRABSMIN(A_WIS)) {
							ABASE(A_WIS) += 1;
							AMAX(A_WIS) += 1;
						}
						decontaminate(rnd(u.contamination));
						continue;
					}

					u.youaredead = 1;
					pline("The contamination consumes you completely...");
					killer = "being consumed by the contamination";
					killer_format = KILLED_BY;
					done(DIED);
					/* lifesaved */
					u.youaredead = 0;
					pline("WARNING: Your wisdom is still critically low and you may die from contamination again! You can cure it by using a scroll or wand of remove curse, or by successfully praying on a coaligned altar. Amnesia may also help in a pinch, or you may buy a decontamination service from a nurse.");
					if(ABASE(A_WIS) < ATTRABSMIN(A_WIS)) {
						ABASE(A_WIS) += 1;
						AMAX(A_WIS) += 1;
					}
				} else if (ABASE(A_WIS) < 2) {
					pline("DANGER!!! Your wisdom is critically low and you're very likely to die from the contamination! You can cure it by using a scroll or wand of remove curse, or by successfully praying on a coaligned altar. Amnesia may also help in a pinch, or you may buy a decontamination service from a nurse.");
				} else if (ABASE(A_WIS) < 4) {
					pline("Watch out!! Your wisdom is very low, the contamination may well reduce it below 1 and then you die! You can cure it by using a scroll or wand of remove curse, or by successfully praying on a coaligned altar. Amnesia may also help in a pinch, or you may buy a decontamination service from a nurse.");
				} else if (ABASE(A_WIS) < 6) {
					pline("Warning! Your wisdom is low and if the contamination causes it to fall below 1, you die! You can cure it by using a scroll or wand of remove curse, or by successfully praying on a coaligned altar. Amnesia may also help in a pinch, or you may buy a decontamination service from a nurse.");
				}

			}

			/* contamination can now go down; if at least 100, only a chance of going down --Amy */
			if (u.contamination >= 100 && !rn2(2)) decontaminate(100);
			/* otherwise, decontaminate by a random amount, meaning you'll probably still be contaminated :D */
			else if (u.contamination && u.contamination < 100) decontaminate(rnd(u.contamination));

			continue;
		}

		/*if(Upolyd && i != A_WIS) continue;*/
		if(/*ABASE(i) >= 18 ||*/ !AEXE(i)) continue;
		if(ABASE(i) >= 18 && sgn(AEXE(i)) >= 1) continue; /* can still abuse stats if they're above 18 --Amy */
		if(i == A_INT || i == A_CHA) continue;/* can't exercise these */

		/* if your contamination was at least 100, you cannot exercise any stats at all --Amy */
		if (verycontaminated && sgn(AEXE(i)) >= 1) continue;

#ifdef DEBUG
		pline("exerchk: testing %s (%d).",
			(i == A_STR) ? "Str" : (i == A_WIS) ? "Wis" :
			(i == A_DEX) ? "Dex" : "Con", AEXE(i));
#endif
		/*
		 *	Law of diminishing returns (Part III):
		 *
		 *	You don't *always* gain by exercising.
		 *	[MRS 92/10/28 - Treat Wisdom specially for balance.]
		 */
		if(sgn(AEXE(i)) >= 1 && rn2(AVAL) > ((i != A_WIS) ? abs(AEXE(i)*2/3) : abs(AEXE(i))))
		    continue;

		goupbuust = 0;

		/* having a high soft cap makes it more likely to boost the stat --Amy */
		if (!rn2(2)) {
			if (urole.attrlimt[i] > 18) goupbuust = (urole.attrlimt[i] - 18);
		} else {
			if (urace.attrtrs[i] > 18) goupbuust = (urace.attrtrs[i] - 18);
		}

		if (goupbuust < 0) goupbuust = 0; /* fail safe */

		/* make it even harder if they're already rather high --Amy */
		if (sgn(AEXE(i)) >= 1) {

			if (ABASE(i) >= (10 + goupbuust) && (ASTART(i) <= ABASE(i)) && !rn2(5)) continue;
			if (ABASE(i) >= (12 + goupbuust) && (ASTART(i) <= ABASE(i)) && !rn2(3)) continue;
			if (ABASE(i) >= (13 + goupbuust) && (ASTART(i) <= ABASE(i)) && !rn2(3)) continue;
			if (ABASE(i) >= (14 + goupbuust) && (ASTART(i) <= ABASE(i)) && !rn2(3)) continue;
			if (ABASE(i) >= (15 + goupbuust) && (ASTART(i) <= ABASE(i)) && !rn2(3)) continue;
			if (ABASE(i) >= (16 + goupbuust) && (ASTART(i) <= ABASE(i)) && !rn2(3)) continue;
			if (ABASE(i) >= (17 + goupbuust) && (ASTART(i) <= ABASE(i)) && !rn2(3)) continue;
		}

		/* different formula for abuse --Amy */
		if(sgn(AEXE(i)) <= 0 && rn2(50) > ((i != A_WIS) ? abs(AEXE(i)*2/3) : abs(AEXE(i))))
		    continue;
		if(sgn(AEXE(i)) <= 0 && !rn2(AVAL) )
		    continue;

		mod_val = sgn(AEXE(i));

#ifdef DEBUG
		pline("exerchk: changing %d.", i);
#endif
		if(adjattrib(i, mod_val, -1, TRUE)) {
#ifdef DEBUG
		    pline("exerchk: changed %d.", i);
#endif
		    /* if you actually changed an attrib - zero accumulation */
		    AEXE(i) = 0;
		    /* then print an explanation */
		    switch(i) {
		    case A_STR: You((mod_val >0) ?
				    "must have been exercising." :
				    "must have been abusing your body.");
				break;
		    case A_WIS: You((mod_val >0) ?
				    "must have been very observant." :
				    "haven't been paying attention.");
				break;
		    case A_DEX: You((mod_val >0) ?
				    "must have been working on your reflexes." :
				    "haven't been working on reflexes lately.");
				break;
		    case A_CON: You((mod_val >0) ?
				    "must be leading a healthy life-style." :
				    "haven't been watching your health.");
				break;
		    }
		}
	    }
	    u.next_check += (ishaxor ? rnz(1000) : rnz(2000) ) ;
#ifdef DEBUG
	    pline("exerchk: next check at %ld.", u.next_check);
#endif
	}
}

void
mightbooststat(stattoboost)
int stattoboost;
{
	boolean statwillgoup = FALSE;
	int statgoupchance = 100;
	int goupbuust = 0;

	/* having a high soft cap makes it more likely to boost the stat --Amy */
	if (!rn2(2)) {
		if (urole.attrlimt[stattoboost] > 18) goupbuust = (urole.attrlimt[stattoboost] - 18);
	} else {
		if (urace.attrtrs[stattoboost] > 18) goupbuust = (urace.attrtrs[stattoboost] - 18);
	}
	if (goupbuust < 0) goupbuust = 0; /* fail safe */

	if (stattoboost < 0) {
		impossible("tried to boost nonexistant stat %d", stattoboost);
		return;
	}
	if (stattoboost >= A_MAX) {
		impossible("tried to boost nonexistant stat %d", stattoboost);
		return;
	}

	if (ABASE(stattoboost) >= 18) {
		return;
	}

	if (ABASE(stattoboost) == 1) statgoupchance = 33;
	else if (ABASE(stattoboost) == 2) statgoupchance = 66;
	else if ( (ABASE(stattoboost) >= 12) && (ASTART(stattoboost) <= ABASE(stattoboost)) ) {
		if (ABASE(stattoboost) == (12 + goupbuust)) statgoupchance = 200;
		else if (ABASE(stattoboost) == (13 + goupbuust)) statgoupchance = 300;
		else if (ABASE(stattoboost) == (14 + goupbuust)) statgoupchance = 400;
		else if (ABASE(stattoboost) == (15 + goupbuust)) statgoupchance = 600;
		else if (ABASE(stattoboost) == (16 + goupbuust)) statgoupchance = 800;
		else if (ABASE(stattoboost) == (17 + goupbuust)) statgoupchance = 1000;
	}

	if (uarm && uarm->oartifact == ART_CERNY_ && stattoboost == A_CON) statgoupchance /= 3;
	if (uarmf && uarmf->oartifact == ART_BRITTA_S_MURDER_STORY) statgoupchance /= 5;

	if (statgoupchance < 1) statgoupchance = 1; /* fail safe */

	if (!rn2(statgoupchance)) statwillgoup = TRUE;
	if (statwillgoup) {
		adjattrib(stattoboost, 1, 0, TRUE);
	}
}

/* next_check will otherwise have its initial 600L after a game restore */
void
reset_attribute_clock()
{
	/*if (moves > 600L) next_check = moves + rn1(50,800);*/
}


void
init_attr(np, rerollpossible)
register int	np;
boolean rerollpossible;
{
	register int	i, x, tryct;


	for(i = 0; i < A_MAX; i++) {
	    ABASE(i) = AMAX(i) = urole.attrbase[i];
	    ATEMP(i) = ATIME(i) = 0;
	    np -= urole.attrbase[i];
	}

	tryct = 0;
	while(np > 0 && tryct < 100) {

	    x = rn2(100);
	    for (i = 0; (i < A_MAX) && ((x -= urole.attrdist[i]) > 0); i++) ;
	    if(i >= A_MAX) continue; /* impossible */

	    if(ABASE(i) >= ATTRMAX(i)) {

		tryct++;
		continue;
	    }

	    if (!attr_will_go_up(i, FALSE)) { /* aww --Amy */

		if (!rerollpossible || !rn2(1000)) np--;
		tryct = 0;
		continue;
	    }

	    tryct = 0;
	    if (ABASE(i) < 16 || (!rn2(ABASE(i) - 14) ) ) { /* very high initial attributes are more rare --Amy */
		    ABASE(i)++;
		    AMAX(i)++;
	    }
	    np--;
	}

	tryct = 0;
	while(np < 0 && tryct < 100) {		/* for redistribution */

	    x = rn2(100);
	    for (i = 0; (i < A_MAX) && ((x -= urole.attrdist[i]) > 0); i++) ;
	    if(i >= A_MAX) continue; /* impossible */

	    if(ABASE(i) <= ATTRMIN(i)) { /* not ATTRABSMIN! should always be at least 3 for all attributes --Amy */

		tryct++;
		continue;
	    }
	    tryct = 0;
	    ABASE(i)--;
	    AMAX(i)--;
	    np++;
	}

	if (Race_if(PM_ADDICT)) {

		for (i = 0; (i < A_MAX); i++) {

			if (i == A_STR) ABASE(i) = AMAX(i) = ATTRMAX(i);
			else if (ABASE(i) < 25) ABASE(i) = AMAX(i) = 25;

		}
	}
}

void
redist_attr()
{
	register int i, tmp;

	for(i = 0; i < A_MAX; i++) {
	    if ((i==A_INT || i==A_WIS) && issoviet) continue; /* wuot? --Amy */
		/* Polymorphing doesn't change your mind - wtf??? of course it does! */
		/* In Soviet Russia, nothing will ever change from vanilla and therefore soviet mode might in fact
		 * just be vanilla, but noooooo, the SLASHTHEM developer decided to fork a variant that he apparently
		 * doesn't like at all, and reverted everything. */
	    tmp = AMAX(i);
	    AMAX(i) += (rn2(5)-2);
	    if (AMAX(i) > ATTRMAX(i)) AMAX(i) = ATTRMAX(i);
	    if (AMAX(i) < ATTRABSMIN(i)) AMAX(i) = ATTRABSMIN(i);
	    ABASE(i) = ABASE(i) * AMAX(i) / tmp;
	    /* ABASE(i) > ATTRMAX(i) is impossible */
	    if (ABASE(i) < ATTRABSMIN(i)) ABASE(i) = ATTRABSMIN(i);
	}
	(void)encumber_msg();
}

STATIC_OVL
void
postadjabil(ability)
long *ability;
{
	if (!ability) return;
	if (ability == &(HWarning) || ability == &(HSee_invisible))
		see_monsters();
}

void
adjabil(oldlevel,newlevel)
int oldlevel, newlevel;
{
	register const struct innate *abil, *rabil;
	long mask = FROMEXPER;

	switch (Role_switch) {
	case PM_ARCHEOLOGIST:   abil = arc_abil;	break;
	case PM_BLEEDER:   abil = ble_abil;	break;
	case PM_SOFTWARE_ENGINEER:   abil = sof_abil;	break;
	case PM_CRACKER:   abil = cra_abil;	break;
	case PM_DEMAGOGUE:   abil = dem_abil;	break;
	case PM_YAUTJA:   abil = yau_abil;	break;
	case PM_JOCKEY:   abil = joc_abil;	break;
	case PM_STORMBOY:   abil = sto_abil;	break;
	case PM_SPACE_MARINE:   abil = sma_abil;	break;
	case PM_JANITOR:   abil = jan_abil;	break;
	case PM_ANACHRONIST:    abil = ana_abil;	break;  
	case PM_DIABLIST:    abil = dia_abil;	break;  
	case PM_DANCER:    abil = dan_abil;	break;  
	case PM_BUTT_LOVER:    abil = but_abil;	break;  
	case PM_CAMPERSTRIKER:    abil = cam_abil;	break;  
	case PM_WALSCHOLAR:    abil = wal_abil;	break;  
	case PM_UNBELIEVER:    abil = unb_abil;	break;  
	case PM_SPACEWARS_FIGHTER:    abil = spa_abil;	break;  
	case PM_PREVERSIONER:    abil = pre_abil;	break;  
	case PM_GANG_SCHOLAR:    abil = sco_abil;	break;  
	case PM_SECRET_ADVICE_MEMBER:    abil = sec_abil;	break;  
	case PM_EMERA:    abil = eme_abil;	break;  
	case PM_XELNAGA:    abil = xel_abil;	break;  
	case PM_BARBARIAN:      abil = bar_abil;	break;
	case PM_NOOB_MODE_BARB:      abil = noo_abil;	break;
	case PM_COURIER:      abil = cou_abil;	break;
	case PM_WEIRDBOY:      abil = wei_abil;	break;
	case PM_MAHOU_SHOUJO:      abil = mah_abil;	break;
	case PM_PICKPOCKET:      abil = pic_abil;	break;
	case PM_ASTRONAUT:      abil = ast_abil;	break;
	case PM_STAND_USER:      abil = sta_abil;	break;
	case PM_JESTER:      abil = jes_abil;	break;
	case PM_LADIESMAN:      abil = lad_abil;	break;
	case PM_FENCER:      abil = fnc_abil;	break;
	case PM_EMPATH:      abil = emp_abil;	break;
	case PM_HUSSY:      abil = hus_abil;	break;
	case PM_QUARTERBACK:      abil = qub_abil;	break;
	case PM_FIGHTER:      abil = fig_abil;	break;
	case PM_MEDIUM:      abil = med_abil;	break;
	case PM_BLOODSEEKER:      abil = blo_abil;	break;
	case PM_DQ_SLIME:      abil = sli_abil;	break;
	case PM_ERDRICK:      abil = erd_abil;	break;
	case PM_CELLAR_CHILD:      abil = cel_abil;	break;
	case PM_DEATH_EATER:      abil = dea_abil;	break;
	case PM_SLAVE_MASTER:      abil = sla_abil;	break;
	case PM_SUPERMARKET_CASHIER:      abil = sup_abil;	break;
	case PM_BARD:           abil = brd_abil;	break;
	case PM_BULLY:           abil = bul_abil;	break;
	case PM_ASSASSIN:           abil = ass_abil;	break;
	case PM_WANDKEEPER:           abil = wan_abil;	break;
	case PM_PALADIN:           abil = pal_abil;	break;
	case PM_ACTIVISTOR:           abil = act_abil;	break;
	case PM_INTEL_SCRIBE:           abil = scr_abil;	break;
	case PM_FOXHOUND_AGENT:           abil = fox_abil;	break;
	case PM_FEAT_MASTER:           abil = stu_abil;	break;
	case PM_DOLL_MISTRESS:           abil = dol_abil;	break;
	case PM_GAMER:           abil = gam_abil;	break;
	case PM_CAVEMAN:        abil = cav_abil;	break;
	case PM_CONVICT:        abil = con_abil;	break;
	case PM_MURDERER:        abil = mur_abil;	break;
	case PM_FAILED_EXISTENCE:        abil = fai_abil;	break;
	case PM_TRANSSYLVANIAN:        abil = trs_abil;	break;
	case PM_KURWA:        abil = kur_abil;	break;
	case PM_PROSTITUTE:        abil = pro_abil;	break;
	case PM_FLAME_MAGE:	abil = fla_abil;	break;
	case PM_COOK:	abil = coo_abil;	break;
	case PM_PSYKER:	abil = psy_abil;	break;
	case PM_FIREFIGHTER:	abil = fir_abil;	break;
	case PM_ACID_MAGE:	abil = aci_abil;	break;
	case PM_GEEK:	abil = gee_abil;	break;
	case PM_GRADUATE:	abil = gra_abil;	break;
	case PM_POKEMON:	abil = pok_abil;	break;
	case PM_NUCLEAR_PHYSICIST:	abil = nuc_abil;	break;
	case PM_HEALER:         abil = hea_abil;	break;
	case PM_ELECTRIC_MAGE:	abil = ele_abil;	break;
	case PM_POISON_MAGE:	abil = psn_abil;	break;
	case PM_OCCULT_MASTER:	abil = occ_abil;	break;
	case PM_CHAOS_SORCEROR:	abil = cha_abil;	break;
	case PM_ELEMENTALIST:	abil = emn_abil;	break;
	case PM_ICE_MAGE:	abil = ice_abil;	break;
	case PM_LIBRARIAN:	abil = lib_abil;	break;
	case PM_JEDI:		abil = jed_abil;	break;
	case PM_HEDDERJEDI:		abil = jed_abil;	break;
	case PM_KNIGHT:         abil = kni_abil;	break;
	case PM_WARRIOR:         abil = war_abil;	break;
	case PM_FORM_CHANGER:         abil = for_abil;	break;
	case PM_AUGURER:         abil = aug_abil;	break;
	case PM_ARTIST:         abil = art_abil;	break;
	case PM_OTAKU:         abil = ota_abil;	break;
	case PM_GUNNER:         abil = gun_abil;	break;
	case PM_DOOM_MARINE:         abil = mar_abil;	break;
	case PM_SAGE:         abil = sag_abil;	break;
	case PM_MONK:           abil = mon_abil;	break;
	case PM_HALF_BAKED:           abil = hba_abil;	break;
	case PM_MUSICIAN:           abil = mus_abil;	break;
	case PM_PSION:           abil = psi_abil;	break;
	case PM_SCIENTIST:           abil = sci_abil;	break;
	case PM_NECROMANCER:	abil = nec_abil;	break;
	case PM_SHAPESHIFTER:	abil = sha_abil;	break;
	case PM_PIRATE:         abil = pir_abil;	break;
	case PM_NINJA:		abil = nin_abil;	break;
	case PM_KORSAIR:         abil = kor_abil;	break;
	case PM_GLADIATOR:         abil = gla_abil;	break;
	case PM_GOFF:         abil = gof_abil;	break;
	case PM_AMAZON:         abil = ama_abil;	break;
	case PM_MASTERMIND:         abil = mas_abil;	break;
	case PM_ALTMER:         abil = alt_abil;	break;
	case PM_BOSMER:         abil = bos_abil;	break;
	case PM_DUNMER:         abil = dun_abil;	break;
	case PM_ORDINATOR:         abil = ord_abil;	break;
	case PM_THALMOR:         abil = tha_abil;	break;
	case PM_DIVER:         abil = div_abil;	break;
	case PM_DRUNK:         abil = dru_abil;	break;
	case PM_LUNATIC:         abil = lun_abil;	break;
	case PM_PRIEST:         abil = pri_abil;	break;
	case PM_RANGER:         abil = ran_abil;	break;
	case PM_TWELPH:
	case PM_ELPH:         abil = elp_abil;	break;
	case PM_LOCKSMITH:         abil = loc_abil;	break;
	case PM_SINGSLAVE:         abil = sng_abil;	break;
	case PM_SOCIAL_JUSTICE_WARRIOR:         abil = soc_abil;	break;
	case PM_ROGUE:          abil = rog_abil;	break;
	case PM_RINGSEEKER:		abil = rin_abil;	break;
	case PM_SAMURAI:        abil = sam_abil;	break;
	case PM_CYBERNINJA:        abil = cyb_abil;	break;
	case PM_TOURIST:        abil = tou_abil;	break;
	case PM_UNDEAD_SLAYER:	abil = und_abil;	break;
	case PM_MIDGET:		abil = mid_abil;	break;
	case PM_UNDERTAKER:	abil = unt_abil;	break;
	case PM_GOLDMINER:		abil = gol_abil;	break;
	case PM_VALKYRIE:       abil = val_abil;	break;
	case PM_VANILLA_VALK:       abil = val_abil;	break;
	case PM_OFFICER:       abil = off_abil;	break;
	case PM_GRENADONIN:       abil = gro_abil;	break;
	case PM_WIZARD:         abil = wiz_abil;	break;
	case PM_ZOOKEEPER:         abil = zoo_abil;	break;
	case PM_WOMANIZER:         abil = wom_abil;	break;
	case PM_CLIMACTERIAL:         abil = cli_abil;	break;
	case PM_YEOMAN:		abil = yeo_abil;	break;
	case PM_JUSTICE_KEEPER:		abil = jus_abil;	break;
	case PM_CARTOMANCER:		abil = car_abil;	break;
	case PM_DRAGONMASTER:		abil = drm_abil;	break;
	case PM_DRUID:		abil = drd_abil;	break;
	case PM_FJORDE:		abil = fjo_abil;	break;
	case PM_AKLYST:		abil = akl_abil;	break;
	case PM_MILL_SWALLOWER:		abil = mil_abil;	break;
	case PM_SYMBIANT:		abil = sym_abil;	break;
	default:                abil = 0;		break;
	}

	switch (Race_switch) {
	case PM_AZTPOK:	rabil = azt_abil;	break;
	case PM_IRRITATOR:	rabil = irr_abil;	break;
	case PM_HYPOTHERMIC:	rabil = hyp_abil;	break;
	case PM_PLAYER_DYNAMO:	rabil = dyn_abil;	break;
	case PM_JAVA:	rabil = jav_abil;	break;
	case PM_CHIQUAI:	rabil = chi_abil;	break;
	case PM_DEATHMOLD:	rabil = dmo_abil;	break;
	case PM_PERVERT:	rabil = per_abil;	break;
	case PM_BOVER:	rabil = bov_abil;	break;
	case PM_CARTHAGE:	rabil = cth_abil;	break;
	case PM_MAYMES:	rabil = may_abil;	break;
	case PM_CELTIC:	rabil = clt_abil;	break;
	case PM_PEACEMAKER:	rabil = pea_abil;	break;
	case PM_MONGUNG:	rabil = mog_abil;	break;
	case PM_ARTIFINDER:	rabil = arf_abil;	break;
	case PM_AK_THIEF_IS_DEAD_:	rabil = akt_abil;	break;
	case PM_PLAYER_JABBERWOCK:	rabil = jab_abil;	break;
	case PM_VIETIS:	rabil = vie_abil;	break;
	case PM_KORONST:	rabil = kst_abil;	break;
	case PM_BABYLONIAN:	rabil = bab_abil;	break;
	case PM_PLAYABLE_NEANDERTHAL:	rabil = nea_abil;	break;
	case PM_RUSMOT:	rabil = rus_abil;	break;
	case PM_MACTHEIST:	rabil = mac_abil;	break;
	case PM_GREURO:	rabil = gru_abil;	break;
	case PM_VIKING:	rabil = vik_abil;	break;
	case PM_DOPPELGANGER:	rabil = dop_abil;	break;
	case PM_HUMANLIKE_DRAGON:	rabil = dra_abil;	break;
	case PM_HYBRIDRAGON:	rabil = hyb_abil;	break;
	case PM_FELID:	rabil = fel_abil;	break;
	case PM_KHAJIIT:	rabil = kha_abil;	break;
	case PM_DWARF:		rabil = dwa_abil;	break;
	case PM_PLAYER_NIBELUNG:		rabil = nib_abil;	break;
	case PM_STARTSCUMMER:		rabil = ssc_abil;	break;
	case PM_PLAYER_SLIME:		rabil = slm_abil;	break;
	case PM_DROW:
	case PM_PLAYER_MYRKALFR:
	case PM_ELF:            rabil = elf_abil;	break;
	case PM_INCANTIFIER:            rabil = inc_abil;	break;
	case PM_PLAYER_GREMLIN:            rabil = gre_abil;	break;
	case PM_CLOCKWORK_AUTOMATON:            rabil = clk_abil;	break;
	case PM_KOBOLT:            rabil = kob_abil;	break;
	case PM_SWIKNI:            rabil = swi_abil;	break;
	case PM_TAYIIN:            rabil = tay_abil;	break;
	case PM_BRETON:            rabil = bre_abil;	break;
	case PM_ELEMENTAL:            rabil = elm_abil;	break;
	case PM_REDGUARD:            rabil = red_abil;	break;
	case PM_DUTHOL:            rabil = dut_abil;	break;
	case PM_IRAHA:            rabil = ira_abil;	break;
	case PM_YOKUDA:            rabil = yok_abil;	break;
	case PM_SINNER:            rabil = sin_abil;	break;
	case PM_REDDITOR:            rabil = rdt_abil;	break;
	case PM_TROLLOR:            rabil = tro_abil;	break;
	case PM_SNAKEMAN:            rabil = sna_abil;	break;
	case PM_ILLITHID:            rabil = ill_abil;	break;
	case PM_KLACKON:            rabil = kla_abil;	break;
	case PM_TONBERRY:            rabil = ton_abil;	break;
	case PM_KUTAR:            rabil = kut_abil;	break;
	case PM_NAVI:            rabil = nav_abil;	break;
	case PM_HUMANOID_LEPRECHAUN:            rabil = lep_abil;	break;
	case PM_MAZIN:            rabil = maz_abil;	break;
	case PM_URGOTH:            rabil = urg_abil;	break;
	case PM_VEELA:            rabil = vee_abil;	break;
	case PM_SENSER:            rabil = sen_abil;	break;
	case PM_PLAYER_UNICORN:            rabil = uni_abil;	break;
	case PM_RETICULAN:            rabil = ret_abil;	break;
	case PM_OUTSIDER:            rabil = out_abil;	break;
	case PM_SPIDERMAN:            rabil = spi_abil;	break;
	case PM_WEAPON_CUBE:            rabil = gel_abil;	break;
	case PM_INSECTOID:            rabil = ins_abil;	break;
	case PM_UNGENOMOLD:            rabil = ung_abil;	break;
	case PM_GASTLY:            rabil = gas_abil;	break;
	case PM_PHANTOM_GHOST:            rabil = gas_abil;	break;
	case PM_MUMMY:            rabil = mum_abil;	break;
	case PM_HAND:            rabil = han_abil;	break;
	case PM_THRALL:            rabil = thr_abil;	break;
	case PM_WEAPONIZED_DINOSAUR:            rabil = din_abil;	break;
	case PM_PLAYER_SKELETON:            rabil = ske_abil;	break;
	case PM_HUMAN_WRAITH:            rabil = wra_abil;	break;
	case PM_LICH_WARRIOR:            rabil = lic_abil;	break;
	case PM_SPIRIT:            rabil = esp_abil;	break;
	case PM_ETHEREALOID:            rabil = eth_abil;	break;
	case PM_INCORPOREALOID:            rabil = ico_abil;	break;
	case PM_SHOE:            rabil = sho_abil;	break;
	case PM_PLAYER_GOLEM:            rabil = glm_abil;	break;
	case PM_SATRE:            rabil = sat_abil;	break;
	case PM_BORG:            rabil = bor_abil;	break;
	case PM_RODNEYAN:            rabil = rod_abil;	break;
	case PM_JELLY:            rabil = jel_abil;	break;
	case PM_WEAPON_IMP:            rabil = wim_abil;	break;
	case PM_TRAINER:            rabil = trn_abil;	break;
	case PM_NEMESIS:            rabil = nem_abil;	break;
	case PM_CUPID:            rabil = cup_abil;	break;
	case PM_ZAUR:            rabil = zau_abil;	break;
	case PM_WYLVAN:            rabil = wyl_abil;	break;
	case PM_BACTERIA:            rabil = bac_abil;	break;
	case PM_PLAYER_SHEEP:            rabil = she_abil;	break;
	case PM_PLAYER_CERBERUS:            rabil = cer_abil;	break;
	case PM_PLAYER_HOUND:            rabil = hou_abil;	break;
	case PM_WOOKIE:            rabil = woo_abil;	break;
	case PM_WISP:            rabil = wis_abil;	break;
	case PM_EXPERT:            rabil = exp_abil;	break;
	case PM_STABILISATOR:            rabil = stb_abil;	break;
	case PM_GIGANT:            rabil = gig_abil;	break;
	case PM_NYMPH:            rabil = nym_abil;	break;
	case PM_METAL:            rabil = met_abil;	break;
	case PM_PLAYER_HULK:            rabil = umb_abil;	break;
	case PM_PLAYER_MECHANIC:            rabil = qua_abil;	break;
	case PM_PLAYER_DOLGSMAN:            rabil = dlg_abil;	break;
	case PM_ROHIRRIM:            rabil = roh_abil;	break;
	case PM_THUNDERLORD:            rabil = thu_abil;	break;
	case PM_GNOME:		rabil = gno_abil;	break;
	case PM_BATMAN:		rabil = bat_abil;	break;
	case PM_HERBALIST:         rabil = hrb_abil;	break;
	case PM_DEVELOPER:         rabil = dvp_abil;	break;
	case PM_LIZARDMAN:         rabil = liz_abil;	break;
	case PM_WEAPON_BUG:         rabil = grd_abil;	break;
	case PM_HOBBIT:		rabil = hob_abil;	break;
	case PM_SAMEDI:		rabil = sdi_abil;	break;
	case PM_ORC:            rabil = orc_abil;	break;
	case PM_HUMAN_WEREWOLF:	rabil = lyc_abil;	break;
	case PM_FENEK:	rabil = fen_abil;	break;
	case PM_AUREAL:	rabil = aur_abil;	break;
	case PM_INKA:	rabil = ink_abil;	break;
	case PM_MAZKE:	rabil = sed_abil;	break;
	case PM_ARMED_COCKATRICE:	rabil = coc_abil;	break;
	case PM_NORD:	rabil = nor_abil;	break;
	case PM_SEA_ELF:	rabil = sea_abil;	break;
	case PM_SLYER_ALIEN:	rabil = sly_abil;	break;
	case PM_DEEP_ELF:	rabil = dee_abil;	break;
	case PM_SYLPH:	rabil = syl_abil;	break;
	case PM_BULDOZGAR:	rabil = bga_abil;	break;
	case PM_SPRIGGAN:	rabil = spr_abil;	break;
	case PM_PLAYER_FAIRY:	rabil = fay_abil;	break;
	case PM_ALCHEMIST:           rabil = alc_abil;	break;
	case PM_PLAYER_ANDROID:           rabil = and_abil;	break;
	case PM_GOAULD:           rabil = goa_abil;	break;
	case PM_ALBAE:	rabil = alb_abil;	break;
	case PM_VORTEX:	rabil = vor_abil;	break;
	case PM_LOWER_ENT:	rabil = ent_abil;	break;
	case PM_CORTEX:	rabil = cor_abil;	break;
	case PM_LEVITATOR:	rabil = lev_abil;	break;
	case PM_HUMANOID_ANGEL:	rabil = ang_abil;	break;
	case PM_HUMANOID_DEVIL:	rabil = dev_abil;	break;
	case PM_GAVIL:	rabil = gav_abil;	break;
	case PM_ANCIPITAL:	rabil = anc_abil;	break;
	case PM_TURTLE:	rabil = tur_abil;	break;
	case PM_FRENDIAN:	rabil = fre_abil;	break;
	case PM_COLORATOR:	rabil = col_abil;	break;
	case PM_URIAN:	rabil = uri_abil;	break;
	case PM_INDRAENIAN:	rabil = ind_abil;	break;
	case PM_HUMAN:
	case PM_VAMPIRE:
	default:                rabil = 0;		break;
	}

	while (abil || rabil) {
	    long prevabil;
	    /* Have we finished with the intrinsics list? */
	    if (!abil || !abil->ability) {
	    	/* Try the race intrinsics */
	    	if (!rabil || !rabil->ability) break;
	    	abil = rabil;
	    	rabil = 0;
	    	mask = FROMRACE;
	    }
		prevabil = *(abil->ability);
		if(oldlevel < abil->ulevel && newlevel >= abil->ulevel) {
			/* Abilities gained at level 1 can never be lost
			 * via level loss, only via means that remove _any_
			 * sort of ability.  A "gain" of such an ability from
			 * an outside source is devoid of meaning, so we set
			 * FROMOUTSIDE to avoid such gains.
			 */

			boolean willget = TRUE;

			if (Race_if(PM_YUGGER) || have_haveitallstone() ) { /* doesn't get positive ones --Amy */

				if (abil->goodone) willget = FALSE;
			}

			if (willget) {
				if (abil->ulevel == 1)
					*(abil->ability) |= (mask|FROMOUTSIDE);
				else
					*(abil->ability) |= mask;
				if(!(*(abil->ability) & INTRINSIC & ~mask)) {
				    if(*(abil->gainstr))
					You_feel("%s!", abil->gainstr);
				}
			}
		} else if (oldlevel >= abil->ulevel && newlevel < abil->ulevel) {
			*(abil->ability) &= ~mask;
			if(!(*(abil->ability) & INTRINSIC)) {
			    if(*(abil->losestr))
				You_feel("%s!", abil->losestr);
			    else if(*(abil->gainstr))
				You_feel("less %s!", abil->gainstr);
			}
		}
	    if (prevabil != *(abil->ability))	/* it changed */
		postadjabil(abil->ability);
	    abil++;
	}

	if (oldlevel > 0) {
	    if (newlevel > oldlevel)
		add_weapon_skill(newlevel - oldlevel);
	    else
		lose_weapon_skill(oldlevel - newlevel);

		/* give more skill slots --Amy */
	    if (newlevel > oldlevel)
		add_weapon_skill(newlevel - oldlevel);
	    else
		lose_weapon_skill(oldlevel - newlevel);
	}

	/* ALI -- update Warn_of_mon */
	HWarn_of_mon = (Undead_warning);
	if (Undead_warning)
	    flags.warntype |= M2_UNDEAD;
	else
	    flags.warntype &= ~M2_UNDEAD;

	/* WAC -- adjust techniques */
	adjtech(oldlevel, newlevel);
}


/* STEPHEN WHITE'S NEW CODE */
int
newhp()
{
	int	hp, conplus;

	if(u.ulevel == 0) {
	    /* Initialize hit points */
	    hp = urole.hpadv.infix + urace.hpadv.infix;
	    if (urole.hpadv.inrnd > 0) hp += rnd(urole.hpadv.inrnd);
	    if (urace.hpadv.inrnd > 0) hp += rnd(urace.hpadv.inrnd);

		if (Role_if(PM_DQ_SLIME) && Race_if(PM_PLAYER_SLIME)) hp += 20;

	    /* Initialize alignment stuff */
	    u.ualign.type = aligns[flags.initalign].value;
	    u.ualign.record = urole.initrecord;
	    u.alignlim = 10 - u.ualign.sins;

	    if (u.ualign.record > u.alignlim) u.ualign.record = u.alignlim;
		return hp;
	} else {
	    if (u.ulevel < urole.xlev) {
	    	hp = urole.hpadv.lofix + urace.hpadv.lofix;
	    	if (urole.hpadv.lornd > 0) hp += rnd(urole.hpadv.lornd);
	    	if (urace.hpadv.lornd > 0) hp += rnd(urace.hpadv.lornd);
	    } else {
	    	hp = urole.hpadv.hifix + urace.hpadv.hifix;
	    	if (urole.hpadv.hirnd > 0) hp += rnd(urole.hpadv.hirnd);
	    	if (urace.hpadv.hirnd > 0) hp += rnd(urace.hpadv.hirnd);
	    }
	}

	if (ACURR(A_CON) <= 1) conplus = -4;
	else if (ACURR(A_CON) <= 2) conplus = -3;
	else if (ACURR(A_CON) <= 3) conplus = -2;
	else if (ACURR(A_CON) <= 6) conplus = -1;
	else if (ACURR(A_CON) <= 9) conplus = 0;
	else if (ACURR(A_CON) <= 12) conplus = 1;
	else if (ACURR(A_CON) <= 14) conplus = 2;
	else if (ACURR(A_CON) <= 16) conplus = 3;
	else if (ACURR(A_CON) == 17) conplus = 4;
	else if (ACURR(A_CON) <= 19) conplus = 5;
	else if (ACURR(A_CON) <= 21) conplus = 6;
	else if (ACURR(A_CON) <= 24) conplus = 7;
	else conplus = 8;
	
	hp += conplus;

	if (uleft && uleft->oartifact == ART_HITPOINT_BOOSTER) hp += 2;
	if (uright && uright->oartifact == ART_HITPOINT_BOOSTER) hp += 2;

	return((hp <= 0) ? 1 : hp);
}

#endif /* OVLB */
#ifdef OVL0

/* STEPHEN WHITE'S NEW CODE */   
/* quite some changes by Amy to make sure you can't easily boost strength to very high values */
schar
acurr(x)
int x;
{
	register int tmp = (u.abon.a[x] + u.atemp.a[x] + u.acurr.a[x]);

	int maxvalue = (x == A_STR) ? 125 : 25;

	if (x == A_STR) {


                /* WAC twiddle so that wearing rings and gauntlets have
                        a bonus */
                /* robe of weakness and gauntlets of power will cancel */
                /*int base = u.acurr.a[x];
                int bonus = tmp - base;*/
                /*boolean nobonus = (uarmg && uarmg->otyp == GAUNTLETS_OF_POWER
                        && uarm && uarm->otyp == ROBE_OF_WEAKNESS);*/

                /*if (uarmg && uarmg->otyp == GAUNTLETS_OF_POWER && !nobonus) {
                        if ((uarmg->spe > 7)
                             || ((118 + bonus + uarmg->spe) > 125)
                             || ((tmp + uarmg->spe) > 125))
                                return(125);
                        else if (base > 118) return (base + uarmg->spe + bonus);
                        else return(118 + uarmg->spe + bonus);
                } else if (uarm && uarm->otyp == ROBE_OF_WEAKNESS && !nobonus)
                        return(3 + bonus);
		else*/

		if (Race_if(PM_HUMAN_MONKEY) && tmp < 4) tmp = 4;

		if (Upolyd && youmonst.data && strongmonst(youmonst.data) ) tmp += 3;

		if (Race_if(PM_WEAPONIZED_DINOSAUR)) tmp += 5;

		if (uarm && uarm->oartifact == ART_CATHAN_S_NETWORK) tmp += (3 + uarm->spe);
		if (uwep && uwep->oartifact == ART_ARM_OF_OLYMPIA) tmp += 10;
		if (uarmc && uarmc->oartifact == ART_HOSTES_AD_PULVEREM_FERIRE) tmp += (5 + uarmc->spe);
		if (uarmg && uarmg->oartifact == ART_GAUNTLETS_OF_SLAYING) tmp += (3 + uarmg->spe);
		if (uarmf && uarmf->oartifact == ART_ANASTASIA_S_GENTLENESS) tmp -= 10;
		if (uarmf && uarmf->oartifact == ART_LILAC_BEAUTY) tmp -= 5;
		if (uarmf && uarmf->oartifact == ART_STEFANJE_S_PROBLEM) tmp += 2;
		if (uwep && uwep->oartifact == ART_GAIA_HAMMER) tmp += 6;
		if (ublindf && ublindf->oartifact == ART_SORELIE_S_ROPE) tmp += 10;
		if (uwep && uwep->oartifact == ART_SEA_ANGERANCHOR) tmp += 7;
		if (uwep && uwep->oartifact == ART_FUNE_NO_IKARI) tmp += 7;
		if (uarm && uarm->oartifact == ART_TIMONA_S_INNER_BICKER) tmp += 3;
		if (!flags.female && uarmu && uarmu->oartifact == ART_FEMALE_BEAUTY) tmp -= 5;
		if (!flags.female && uarmu && uarmu->oartifact == ART_FEMINIST_GIRL_S_PURPLE_WEA) tmp -= 5;
		if (uarmh && uarmh->oartifact == ART_DUNCE_POUNCE) tmp += 5;
		if (uarmh && uarmh->oartifact == ART_MASSIVE_IRON_CROWN_OF_MORG) tmp += 5;
		if (uleft && uleft->oartifact == ART_UNBELIEVABLY_STRONG_PUNCH) tmp += uleft->spe;
		if (uright && uright->oartifact == ART_UNBELIEVABLY_STRONG_PUNCH) tmp += uright->spe;
		if (uleft && uleft->oartifact == ART_CATHAN_S_SIGIL) tmp += (3 + uleft->spe);
		if (uright && uright->oartifact == ART_CATHAN_S_SIGIL) tmp += (3 + uright->spe);
		if (uimplant && uimplant->oartifact == ART_MEMORY_CHIP_OF_RED_SONJA) tmp += 2;
		if (uimplant && uimplant->oartifact == ART_BLOODPUMP_OF_HARKON_BLOODR) tmp += 2;
		if (uarmf && uarmf->oartifact == ART_OUT_OF_TIME) tmp += 5;
		if (uarmh && itemhasappearance(uarmh, APP_THINKING_HELMET)) tmp += 1;
		if (uarmc && uarmc->oartifact == ART_ANCHORAGE_BATTLECOAT) tmp += 5;
		if (uleft && uleft->oartifact == ART_HOH_UNNE) tmp += 5;
		if (uright && uright->oartifact == ART_HOH_UNNE) tmp += 5;
		if (uarm && uarm->otyp == CRYSTALLINE_DRAGON_SCALES) tmp += 1;
		if (uarmh && uarmh->oartifact == ART_KASPAR_S_HELM_OF_THE_HORNE && u.ualign.type == A_CHAOTIC) tmp += 3;
		if (uarm && uarm->oartifact == ART_HANDSOME_THREESOME) tmp += 3;
		if (uarm && uarm->otyp == CRYSTALLINE_DRAGON_SCALE_MAIL) tmp += 1;
		if (uarms && uarms->otyp == CRYSTALLINE_DRAGON_SCALE_SHIEL) tmp += 1;
		if (uarm && uarm->oartifact == ART_BOAHMM) tmp += 5;
		if (uarm && uarm->oartifact == ART_DONNICA_S_SUPERLATIVE) tmp += 20;
		if (uamul && uamul->otyp == AMULET_OF_FLIPPING) {
			if (uamul->blessed) tmp++;
			if (!uamul->cursed) tmp++;
			if (uamul->cursed) tmp--;
			if (uamul->hvycurse) tmp--;
			if (uamul->prmcurse) tmp--;
			if (uamul->evilcurse) tmp--;
			if (uamul->bbrcurse) tmp--;
			if (uamul->morgcurse) tmp--;
		}
		if (uarmc && uarmc->oartifact == ART_HIGH_KING_OF_SKIRIM) tmp += 5;
		if (uwep && uwep->oartifact == ART_RAFSCHAR_S_SUPERWEAPON) tmp += 1;
		if (uarmf && uarmf->oartifact == ART_STRONGEST_STOMP) tmp += 8;
		if (uimplant && uimplant->oartifact == ART_THAI_S_EROTIC_BITCH_FU) tmp += 5;
		if (uimplant && uimplant->oartifact == ART_NERVE_HOOK_OF_CONAN) tmp += 3;
		if (uarmf && uarmf->oartifact == ART_CRUEL_GODDESS_ANA) tmp -= 5;
		if (uarmh && uarmh->oartifact == ART_TEH_PHYSIQUE) tmp += 10;
		if (uleft && uleft->oartifact == ART_CROQUE_FORD) tmp += 4;
		if (uright && uright->oartifact == ART_CROQUE_FORD) tmp += 4;
		if (uarmf && uarmf->oartifact == ART_INGRID_S_IRONHOOF_BOOTS) tmp += 3;
		if (uleft && uleft->oartifact == ART_FISFIS) tmp += 3;
		if (uimplant && uimplant->oartifact == ART_EARCLIP_OF_BEOWULF) tmp += 1;
		if (uamul && uamul->oartifact == ART_ROCHELLE_S_SEY) tmp += 3;
		if (uright && uright->oartifact == ART_FISFIS) tmp += 3;
		if (uarmh && uarmh->oartifact == ART_FREDERIK_S_COMBAT_HELMET) tmp += 2;
		if (uleft && uleft->oartifact == ART_MAHPTY) tmp += 1;
		if (uarmf && uarmf->oartifact == ART_CAMISHA_THE_INCONSPICUOUS) tmp += 3;
		if (uright && uright->oartifact == ART_MAHPTY) tmp += 1;
		if (uleft && uleft->oartifact == ART_ATLAS_GLOBE) tmp += 5;
		if (uball && uball->oartifact == ART_MAREA_S_EXALTITUDE) tmp += 2;
		if (uright && uright->oartifact == ART_ATLAS_GLOBE) tmp += 5;
		if (uamul && uamul->oartifact == ART_DO_NOT_FORGET_GRACE) tmp += 5;
		if (uarmf && uarmf->oartifact == ART_JOHANNA_S_RED_CHARM) tmp += 1;
		if (uarmf && uarmf->oartifact == ART_DORA_S_SCRATCHY_HEELS) tmp += 1;
		if (ublindf && ublindf->oartifact == ART_MONIQUE_S_NONSENSE && uwep && (weapon_type(uwep) == P_AXE) ) tmp += 10;
		if (uarm && uarm->oartifact == ART_UBERGAGE) tmp += 3;
		if (uwep && uwep->oartifact == ART_DJANGO_S_CHAIN) tmp += 5;
		if (uarmf && uarmf->oartifact == ART_ASTER_IX_BOOKLET) tmp += 6;
		if (uchain && uchain->oartifact == ART_DJANGO_S_CHAIN) tmp += 5;
		if (uwep && uwep->oartifact == ART_PLAY_FIGUT_EIGHT) tmp += 8;
		if (uarm && uarm->oartifact == ART_GARYX) tmp += 1;
		if (uarmf && uarmf->oartifact == ART_LARS_S_IRONSTRIDE_BOOTS) tmp += 2;
		if (uimplant && uimplant->oartifact == ART_DIDI_S_LEG_PAINTING) tmp += 1;
		if (uimplant && uimplant->oartifact == ART_NEURAL_GRAFT_OF_VAELRIC_TH) tmp += 2;
		if (uleft && uleft->oartifact == ART_SPEECHBREAK) tmp += 5;
		if (uarms && uarms->oartifact == ART_WORKER_METAL) tmp += 2;
		if (uarm && uarm->oartifact == ART_PURPLE_SOCKET) tmp += 3;
		if (uimplant && uimplant->oartifact == ART_HEARTSHARD_OF_ARAGORN) tmp += 2;
		if (uarmc && uarmc->oartifact == ART_POWERSTINK) tmp += 3;
		if (uarmu && uarmu->oartifact == ART_GUAH_AVERT_SUEUE) tmp += 6;
		if (powerfulimplants() && uimplant && uimplant->oartifact == ART_SANTANA_S_CHICANERY) tmp += 1;
		if (uright && uright->oartifact == ART_SPEECHBREAK) tmp += 5;
		if (uleft && uleft->oartifact == ART_RING_OF_STEEL_DRAGON) tmp += 3;
		if (uright && uright->oartifact == ART_RING_OF_STEEL_DRAGON) tmp += 3;
		if (uwep && uwep->oartifact == ART_STAR_SLAY_GIANTS) tmp += 5;
		if (uarmc && uarmc->oartifact == ART_FEND_FOR_YOURSELF) tmp += 4;
		if (uarmg && uarmg->oartifact == ART_ALPHA_GLOVES) tmp += 7;
		if (uimplant && uimplant->oartifact == ART_GWEN_S_TRAUMA) tmp += 3;
		if (uwep && uwep->oartifact == ART_SPECIFI_X_) tmp += 5;
		if (uwep && uwep->oartifact == ART_ONI_CRUSHER) tmp += 5;
		if (uarm && uarm->oartifact == ART_ANTOCONT) tmp += 5;
		if (uchain && uchain->oartifact == ART_ONI_CRUSHER) tmp += 5;
		if (uarmg && uarmg->oartifact == ART_LE_MOUVEMENT_DE_LA_BOUCHE_) tmp += 4;
		if (uleft && uleft->oartifact == ART_HIGHER_LOCK) tmp += 1;
		if (uright && uright->oartifact == ART_HIGHER_LOCK) tmp += 1;
		if (uarmc && uarmc->oartifact == ART_JUST_TO_HAVE_IT) tmp += 1;
		if (ublindf && ublindf->oartifact == ART_LEANDRA_S_WORD) tmp += 1;
		if (uarm && uarm->oartifact == ART_EMRYS_BACKSCALE) tmp += 3;
		if (uarmf && uarmf->oartifact == ART_SEE_BY_LAKE) tmp += 1;
		if (uarmf && uarmf->oartifact == ART_CHLOE_S_EXAGGERATION) tmp += 20;
		if (uwep && uwep->oartifact == ART_MILL_SHOVEL) tmp += 8;
		if (uwep && uwep->oartifact == ART_KUMIROMI_SCYTHE) tmp += 3;
		if (uamul && uamul->oartifact == ART_ATLAS_WEIGHT_CRUNCH) tmp += 4;
		if (uarmg && uarmg->oartifact == ART_CRUSH_THE_OPPOSITION) tmp += 10;
		if (uarmg && itemhasappearance(uarmg, APP_ATLAS_GLOVES)) tmp += 3;
		if (uarmf && uarmf->otyp == COMMANDER_HEELS) tmp += 1;
		if (uarmg && uarmg->oartifact == ART_GLOVES_OF_VESDA) tmp += 3;
		if (uwep && uwep->oartifact == ART_RUINED_ANTJEN) tmp += 3;
		if (uwep && uwep->oartifact == ART_SOULREAPER_SCYTHE) tmp += 8;
		if (uwep && uwep->oartifact == ART_SOULSCOLLECTING) tmp += 8;
		if (uamul && uamul->otyp == AMULET_OF_PHYSICAL_BOOST) tmp += 3;
		if (uarmg && uarmg->oartifact == ART_GET_THE_OLD_VALUES_BACK) tmp += 3;
		if (uarmg && uarmg->oartifact == ART_GAUNTLETS_OF_THE_BERSERKER) tmp += 5;
		if (uwep && uwep->oartifact == ART_YORSHKA_S_SPEAR) tmp += 6;
		if (uarms && uarms->oartifact == ART_GOLDEN_KNIGHT) tmp += 5;
		if (uarmc && uarmc->oartifact == ART_DIN_DEN) tmp += 4;
		if (uwep && uwep->oartifact == ART_OLYMPIDE) tmp += 5;
		if (uwep && uwep->oartifact == ART_TITANIC_STRIKE) tmp += 20;
		if (uarmu && uarmu->oartifact == ART_FIRST_THERE_WE_WERE) tmp += 3;
		if (u.boosttimer) tmp += 10;
		if (uwep && uwep->oartifact == ART_ATLUS_HEAVE) tmp += 10;
		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_SMASH_TONFA) tmp += 3;
		if (uwep && uwep->oartifact == ART_NEEDLESSLY_MEAN) tmp += 4;
		if (uarmc && uarmc->oartifact == ART_ISHITA_S_OVERWHELMING) tmp += 3;
		if (uarm && uarm->oartifact == ART_CLANGFRIEND) tmp += 3;
		if (uwep && uwep->oartifact == ART_MONS_INFERNALIS_X_) tmp += 10;
		if (uwep && uwep->oartifact == ART_EMERALD_SWORD) tmp += 5;
		if (uarmf && uarmf->oartifact == ART_ANTJE_S_POWERSTRIDE) tmp += 10;
		if (uwep && uwep->oartifact == ART_STAFF_OF_LEIBNIZ) tmp += 1;
		if (uarmf && uarmf->oartifact == ART_DESEAMING_GAME) tmp += 3;
		if (FemtrapActiveNora && u.uhs == WEAK) tmp += 2;
		if (FemtrapActiveNora && u.uhs == FAINTING) tmp += 5;
		if (u.martialstyle == MARTIALSTYLE_WRESTLING) tmp += 1;
		if (FemtrapActiveNora && u.uhs == FAINTED) tmp += 10;
		if (uarmf && uarmf->oartifact == ART_JOHANETTA_S_TITLE) tmp += 4;
		if (FemtrapActiveNora && u.uhs == STARVED) tmp += 25;
		if (uwep && uwep->oartifact == ART_ULTRA_ANNOYANCE) tmp += 10;
		if (uarmf && uarmf->oartifact == ART_LORENZI_S_CLEANING_RESIDUE) tmp += 2;
		if (u.combatcommand) tmp += 1;
		if (Race_if(PM_URGOTH)) tmp += 5;
		if (uwep && uwep->oartifact == ART_UNATTAINABLE_NINETEEN) tmp += 2;
		if (uwep && uwep->oartifact == ART_GONDOLIN_S_HIDDEN_PASSAGE) tmp += 5;
		if (uwep && uwep->oartifact == ART_GIANTCRUSHER) tmp += 5;
		if (uwep && uwep->oartifact == ART_SWING_FOR_THE_FENCES) tmp += 10;
		if (uwep && uwep->oartifact == ART_INDOBURDENIA_VIRII) tmp += 10;
		if (uarmc && uarmc->oartifact == ART_CORNY_DOOD && !flags.female) tmp += 3;
		if (uwep && uwep->oartifact == ART_HARD_IMPACT) tmp += 5;
		if (uwep && uwep->oartifact == ART_PHYSSTATBOOST) tmp += 5;
		if (uarmg && uarmg->oartifact == ART_STRBONUS) tmp += 5;
		if (uarm && uarm->oartifact == ART_SMILEY_FACE) tmp += 1;
		if (have_superjonadabstone()) tmp += 10;
		if (uimplant && uimplant->oartifact == ART_DUNGEON_BOSS__WITH_SHARP_S) tmp += (powerfulimplants() ? 10 : 1);
		if (uleft && uleft->otyp == RIN_GAIN_STRENGTH) tmp += uleft->spe;
		if (uright && uright->otyp == RIN_GAIN_STRENGTH) tmp += uright->spe;
		if (uarmg && uarmg->oartifact == ART_LIKE_GRACE) tmp += 5;
		if ((uwep && uwep->oartifact == ART_BAEFF) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_BAEFF)) tmp += 8;
		if (uarmu && uarmu->oartifact == ART_MENSTRUATION_HURTS) tmp += 6;
		if (uwep && uwep->enchantment == WEAPEGO_STR) tmp += 2;
		if (uwep && uwep->enchantment == WEAPEGO_GREAT_STR) tmp += 5;
		if (uwep && uwep->enchantment == WEAPEGO_GRACE) tmp += 2;
		if (uwep && uwep->enchantment == WEAPEGO_GREAT_GRACE) tmp += 5;
		if (uwep && uwep->oartifact == ART_EORLINGAS) tmp += 3;

		if (FemtrapActiveThai) tmp -= 2;
		if (Race_if(PM_KNOWLEDGABLE)) tmp -= 2;
		if (uarmg && uarmg->otyp == GAUNTLETS_OF_POWER) tmp += (uarmg->spe + 3);
		if (uarmg && uarmg->otyp == SUPERPOWER_GAUNTLETS) tmp += uarmg->spe;
		if (uarm && uarm->otyp == ROBE_OF_WEAKNESS) tmp -= 3;
		if (uarmf && uarmf->oartifact == ART_FAR_EAST_RELATION) tmp -= 3;
		if (Race_if(PM_LOWER_ENT) && Burned) tmp -= 2;
		if (isbadstatter) tmp -= 2;
		if (uarmu && uarmu->oartifact == ART_TILLMANN_S_TARGET) tmp -= 5;
		if (uleft && uleft->oartifact == ART_FIRST_EXCHANGE) tmp -= 5;
		if (uright && uright->oartifact == ART_FIRST_EXCHANGE) tmp -= 5;
		if (u.uhs >= WEAK) tmp--;

		if (PlayerBleeds > 50) tmp--;
		if (PlayerBleeds > 100) tmp -= 2;

		if (FemtrapActiveNora && u.uhunger > 500) {
			int norahunger = (u.uhunger - 500);
			if (norahunger < 200) norahunger = 0;
			norahunger /= 200;
			if (norahunger < 0) norahunger = 0;
			tmp--;
			tmp -= norahunger;
		}

		if (uarmc && uarmc->oartifact == ART_ROKKO_CHAN_S_SUIT && tmp > 12) tmp = 12;
		if (uwep && uwep->oartifact == ART_JAPANESE_WOMEN && tmp > 14) tmp = 14;
		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_JAPANESE_WOMEN && tmp > 14) tmp = 14;
		if (uarmf && uarmf->oartifact == ART_SKIN_AND_BONE && tmp > 6) tmp = 6;
		if (autismweaponcheck(ART_MR__AHLBLOW_S_SIGNAGE) && tmp > 5) tmp = 5;
		if (uarmf && uarmf->oartifact == ART_STAR_SOLES) tmp -= 2;
		if (FemtrapActiveIna) tmp -= 3;
		if (Race_if(PM_HUMANOID_ANGEL)) tmp -= angelshadowstuff();
		if (u.tsloss_str > 0) tmp -= u.tsloss_str;

		if (Race_if(PM_PERVERT)) { /* debuff when not praying or having sex every once in a while --Amy */
			int reductorval = u.pervertsex;
			while (reductorval >= 10000) {
				reductorval -= 10000;
				tmp--;
			}
			reductorval = u.pervertpray;
			while (reductorval >= 10000) {
				reductorval -= 10000;
				tmp--;
			}
		}

		if (AllStatsAreLower) tmp -= min(tmp - 3, 10);
		if (u.uprops[STATS_LOWERED].extrinsic) tmp -= min(tmp - 3, 10);
		if (have_lowstatstone()) tmp -= min(tmp - 3, 10);
		if (uarmc && uarmc->oartifact == ART_SIECHELALUER) tmp -= min(tmp - 3, 10);

	} else if (x == A_CHA) {
		/*if (tmp < 18 && (youmonst.data && youmonst.data->mlet == S_NYMPH ||
		    u.umonnum == PM_SUCCUBUS || u.umonnum == PM_INCUBUS))
		    tmp = 18;*/
		if (youmonst.data && (youmonst.data->mlet == S_NYMPH || u.umonnum == PM_SUCCUBUS || u.umonnum == PM_INCUBUS)) tmp += 3;
		if (uarmh && uarmh->otyp == FEDORA) tmp += 1;        
		if (uarmf && uarmf->otyp == UGG_BOOTS) tmp -= 3;
		if (uarmc && itemhasappearance(uarmc, APP_FLEECY_COLORED_CLOAK)) tmp += 5;

		if (uarm && uarm->oartifact == ART_PLAYBOY_WITH_EARS) tmp += (5 + uarm->spe);
		if (!PlayerInSexyFlats && !PlayerInHighHeels && uarmf && uarmf->oartifact == ART_FORMO____) tmp += 2;
		if (uarmh && uarmh->oartifact == ART_PLAYBOY_SUPPLEMENT) tmp += (5 + uarmh->spe);
		if (uarmf && uarmf->oartifact == ART_GNOMISH_BOOBS) tmp += 3;
		if (uarmf && uarmf->oartifact == ART_TOO_OLD_MODEL) tmp += 3;
		if (uarmg && uarmg->oartifact == ART_NALI_THE_BNALI) tmp += 6;
		if (uarmc && uarmc->oartifact == ART_LILIANE_S_GLAMOUR_VEIL) tmp += 7;
		if (uarmu && uarmu->oartifact == ART_JEDE_SIZE_HIZE) tmp += 3;
		if (uarm && uarm->oartifact == ART_SING_S_STUNNING_BEAUTY) tmp += 10;
		if (uarms && uarms->oartifact == ART_SIDONIE_S_MIRROR) tmp += 12;
		if (uarmf && uarmf->oartifact == ART_LISSIE_S_SHEAGENTUR) tmp += 3;
		if (uarmf && uarmf->oartifact == ART_EVELINE_S_LOVELIES) tmp += 3;
		if (uarmf && uarmf->oartifact == ART_STEFANJE_S_PROBLEM) tmp += 2;
		if (uwep && uwep->oartifact == ART_AMORINA_S_CHAIN) tmp += 3;
		if (uchain && uchain->oartifact == ART_AMORINA_S_CHAIN) tmp += 3;
		if (uarmc && uarmc->oartifact == ART_TEAM_BRIDE) tmp += 10;
		if (uarmf && uarmf->oartifact == ART_LITTLE_ICE_BLOCK_WITH_THE_) tmp += 2;
		if (uarmf && uarmf->oartifact == ART_YVONNE_S_MODEL_AMBITION) tmp += 2;
		if (uarm && uarm->oartifact == ART_DONNICA_S_SUPERLATIVE) tmp += 5;
		if (uarmf && uarmf->oartifact == ART_SOFT_KARATE_KICK) tmp += 2;
		if (uarm && uarm->oartifact == ART_ZURA_S_DRESSCODE) tmp += 5;
		if (ublindf && ublindf->oartifact == ART_LEANDRA_S_WORD) tmp += 2;
		if (uarmh && uarmh->oartifact == ART_BIG_REAR_END) tmp += 3;
		if (uimplant && uimplant->oartifact == ART_USELESS_STAT) tmp += (powerfulimplants() ? 10 : 5);
		if (uarmf && uarmf->oartifact == ART_KOKYO_NO_PAFOMANSUU_OKU) tmp += (5 + uarmf->spe);
		if (uarmf && uarmf->oartifact == ART_EVA_S_INCONSPICUOUS_CHARM) tmp += (uarmf->spe);
		if (uarmf && uarmf->oartifact == ART_SOLVEJG_S_STINKING_SLIPPER) tmp += 5;
		if (uarmf && uarmf->oartifact == ART_SEE_BY_LAKE) tmp += 1;
		if (uarmf && uarmf->oartifact == ART_LYDIA_S_SEXYROOM) tmp += 5;
		if (uarmf && uarmf->oartifact == ART_INGRID_S_SECRETARY_OFFICE) tmp += 5;
		if (uimplant && uimplant->oartifact == ART_SOLAR_RING_OF_LYSIRA_DAWNF) tmp += 1;
		if (uarmf && uarmf->oartifact == ART_SUCCESS_LESS_SAVING) tmp += 10;
		if (uarmf && uarmf->oartifact == ART_HANNAH_S_INNOCENCE) tmp += 5;
		if (uimplant && uimplant->oartifact == ART_BONE_LATTICE_OF_LANCELOT) tmp += 2;
		if (uleft && uleft->oartifact == ART_CROQUE_FORD) tmp += 4;
		if (uright && uright->oartifact == ART_CROQUE_FORD) tmp += 4;
		if (uarmf && uarmf->oartifact == ART_JANET_S_DETACHMENT) tmp += 5;
		if (powerfulimplants() && uimplant && uimplant->oartifact == ART_SANTANA_S_CHICANERY) tmp += 1;
		if (uarmf && uarmf->oartifact == ART_LOVELANE) tmp += 5;
		if (uarmc && uarmc->oartifact == ART_NUDE_PUNAM) tmp += 10;
		if (uamul && uamul->oartifact == ART_ROCHELLE_S_SEY) tmp += 3;
		if (uarmf && uarmf->oartifact == ART_IRIS_S_THUNDER) tmp += 3;
		if (uamul && uamul->oartifact == ART_ZUBRIT_S_HOTNESS) tmp += 8;
		if (FemtrapActiveChloe && uarmf && uarmf->otyp == CHLOE_BALL_HEELS) tmp += 10;
		if (ublindf && ublindf->oartifact == ART_LEANDRA_S_WORD) tmp += 2;
		if (uarmf && uarmf->oartifact == ART_SUSCHEL_KUNA) tmp += 2;
		if (uarmh && uarmh->oartifact == ART_CAP_OF_THENGEL) tmp += 3;
		if (uarmh && uarmh->oartifact == ART_SHREW_WIND) tmp += 5;
		if (uamul && uamul->oartifact == ART_BEGGER_S_PENDANT) tmp += 4;
		if (uarmh && uarmh->oartifact == ART_SAMIRA_S_EXPRESSION) tmp -= 2;
		if (uarm && uarm->oartifact == ART_COLORED_OTHER) tmp += 1;
		if (uarmh && uarmh->oartifact == ART_DREHTURN) tmp += 5;
		if (uarmf && uarmf->otyp == SHOCK_ISOLATED_HEELS) tmp += 2;
		if (uball && uball->oartifact == ART_MAREA_S_EXALTITUDE) tmp += 2;
		if (uarmf && uarmf->otyp == COMMANDER_HEELS) tmp += 1;
		if (uamul && uamul->oartifact == ART_______SCORE) tmp += 1;
		if (uarmu && uarmu->oartifact == ART_SETH_S_ERROR) tmp += 3;
		if (uarmh && uarmh->oartifact == ART_NAQ_QEH) tmp += 3;
		if (uarm && uarm->oartifact == ART_LET_S_DO_IT_ALREADY && uarm->otyp == BLUE_DRAGON_SCALE_MAIL) tmp += 3;
		if (uarmf && uarmf->oartifact == ART_RONIKA_S_NORM) tmp += 1;
		if (uamul && uamul->oartifact == ART_ATTORNEY_S_BADGE) tmp += 8;
		if (uamul && uamul->oartifact == ART_WEDDING_ENGAGEMENT) tmp += 5;
		if (uarm && uarm->oartifact == ART_TOWNIE_DUMP) tmp += 5;
		if (uwep && uwep->oartifact == ART_THROW_ALL_THE_CASH_AWAY) tmp += 5;
		if (uwep && uwep->oartifact == ART_SHENA_S_PANTY) tmp += 4;
		if (uwep && uwep->oartifact == ART_SCENTFUL_PANTY) tmp += 4;
		if (uamul && uamul->oartifact == ART_DIEAMEND) tmp += 2;
		if (uarm && uarm->oartifact == ART_HELEN_S_REVEAL) tmp += 5;
		if (uarmf && uarmf->oartifact == ART_HELP_PEOPLE_AND_YOURSELF) tmp += 5;
		if (uarm && uarm->oartifact == ART_TRIANGLE_GIRL && flags.female && u.ulevel < 10) tmp += 2;
		if (uarmf && uarmf->oartifact == ART_XTRA_CUTENESS) tmp += 10;
		if (uarmf && uarmf->oartifact == ART_LEATHER_PUMPS_OF_HORROR) tmp += (5 + uarmf->spe);
		if (uarmf && uarmf->oartifact == ART_MANDY_S_ROUGH_BEAUTY) tmp += (5 + uarmf->spe);
		if (uarmf && uarmf->oartifact == ART_LILAC_BEAUTY) tmp += 10;
		if (uarmf && uarmf->oartifact == ART_WEDDING_CHALLENGE) tmp += 10;
		if (uarmf && uarmf->oartifact == ART_RUEA_S_FAILED_CONVERSION) tmp += 10;
		if (uarmf && uarmf->oartifact == ART_MELISSA_S_BEAUTY) tmp += 10;
		if (uarmf && uarmf->oartifact == ART_KRISTIN_S_INNER_FEEL && objects[uarmf->otyp].oc_color == CLR_RED) tmp += 8;
		if (uarmf && uarmf->oartifact == ART_MARIYA_S_SEXYCHARM) tmp += 5;
		if (uarmf && uarmf->oartifact == ART_HUGGING__GROPING_AND_STROK) tmp += 10;
		if (uarmf && uarmf->oartifact == ART_BEAUTIFUL_TOPMODEL) tmp += 25;
		if (uarmf && uarmf->oartifact == ART_UNEVEN_STILTS) tmp += 15;
		if (uarmc && uarmc->oartifact == ART_DIN_DEN) tmp += 4;
		if (uarmu && uarmu->oartifact == ART_LISE_S_UNDERWEAR) tmp += 6;
		if (uarmf && uarmf->oartifact == ART_RARE_ASIAN_LADY) tmp += 20;
		if (uwep && uwep->oartifact == ART_DJANGO_S_CHAIN) tmp += 5;
		if (uchain && uchain->oartifact == ART_DJANGO_S_CHAIN) tmp += 5;
		if (uarmf && uarmf->oartifact == ART_ELENETTES) tmp += 5;
		if (uarmf && uarmf->oartifact == ART_LAUGH_WHEN_YOU_FALL && (multi < 0)) tmp += 5;
		if (uarmf && uarmf->oartifact == ART_SEXROOM_FOR_FREE) tmp += 5;
		if (uarmf && uarmf->oartifact == ART_NE_PROSTO_KRASIVO) tmp += 4;
		if (uarmf && uarmf->oartifact == ART_ELEVECULT) tmp += 5;
		if (uarmu && uarmu->oartifact == ART_ARTITFACT && flags.female) tmp += 5;
		if (uarmf && uarmf->oartifact == ART_KRISTIN_S_NOBILITY) tmp += 5;
		if (uarmf && uarmf->oartifact == ART_CRUEL_GODDESS_ANA) tmp += 5;
		if (uarmf && uarmf->oartifact == ART_JUEN_S_WEAKNESS) tmp += 5;
		if (uleft && uleft->oartifact == ART_CRIMINAL_QUEEN) tmp += 5;
		if (uright && uright->oartifact == ART_CRIMINAL_QUEEN) tmp += 5;
		if (uleft && uleft->oartifact == ART_PALMIA_PRIDE) tmp += 5;
		if (uright && uright->oartifact == ART_PALMIA_PRIDE) tmp += 5;
		if (uleft && uleft->oartifact == ART_SCARAB_OF_ADORNMENT) tmp += 10;
		if (uright && uright->oartifact == ART_SCARAB_OF_ADORNMENT) tmp += 10;
		if (uamul && uamul->oartifact == ART_OH_COME_ON) tmp += 5;
		if (uwep && uwep->oartifact == ART_NEOLITHIC_ACCESSORY) tmp += 3;
		if (uamul && uamul->oartifact == ART_NECKLACE_OF_ADORNMENT) tmp += 10;
		if (uleft && uleft->oartifact == ART_HIGHER_LOCK) tmp += 1;
		if (uright && uright->oartifact == ART_HIGHER_LOCK) tmp += 1;
		if (uwep && uwep->oartifact == ART_BUNGA_BUNGA && uwep->spe > 0) tmp += (5 + uwep->spe);
		if (flags.female && uarmu && uarmu->oartifact == ART_FEMALE_BEAUTY) tmp += 5;
		if (flags.female && uarmu && uarmu->oartifact == ART_FEMINIST_GIRL_S_PURPLE_WEA) tmp += 5;
		if (!flags.female && uarmu && uarmu->oartifact == ART_FEMALE_BEAUTY) tmp -= 5;
		if (!flags.female && uarmu && uarmu->oartifact == ART_FEMINIST_GIRL_S_PURPLE_WEA) tmp -= 5;
		if (uarm && uarm->otyp == CRYSTALLINE_DRAGON_SCALES) tmp += 1;
		if (uarm && uarm->otyp == CRYSTALLINE_DRAGON_SCALE_MAIL) tmp += 1;
		if (uarms && uarms->otyp == CRYSTALLINE_DRAGON_SCALE_SHIEL) tmp += 1;
		if (uamul && uamul->otyp == AMULET_OF_FLIPPING) {
			if (uamul->blessed) tmp++;
			if (!uamul->cursed) tmp++;
			if (uamul->cursed) tmp--;
			if (uamul->hvycurse) tmp--;
			if (uamul->prmcurse) tmp--;
			if (uamul->evilcurse) tmp--;
			if (uamul->bbrcurse) tmp--;
			if (uamul->morgcurse) tmp--;
		}
		if (uarmf && uarmf->oartifact == ART_MOST_EROTIC_AIR_CURRENT_NO) {
			if (uarm && objects[uarm->otyp].oc_color == CLR_BRIGHT_MAGENTA) tmp++;
			if (uarmf && objects[uarmf->otyp].oc_color == CLR_BRIGHT_MAGENTA) tmp++;
			if (uarmc && objects[uarmc->otyp].oc_color == CLR_BRIGHT_MAGENTA) tmp++;
			if (uarmh && objects[uarmh->otyp].oc_color == CLR_BRIGHT_MAGENTA) tmp++;
			if (uarmg && objects[uarmg->otyp].oc_color == CLR_BRIGHT_MAGENTA) tmp++;
			if (uarms && objects[uarms->otyp].oc_color == CLR_BRIGHT_MAGENTA) tmp++;
			if (uarmu && objects[uarmu->otyp].oc_color == CLR_BRIGHT_MAGENTA) tmp++;
		}
		if (uarmu && uarmu->oartifact == ART_BIENVENIDO_A_MIAMI) tmp += 3;
		if (uarmh && uarmh->oartifact == ART_MASSIVE_IRON_CROWN_OF_MORG) tmp += 5;
		if (uarmc && uarmc->oartifact == ART_SEXY_STROKING_UNITS) tmp += 5;
		if (uarm && uarm->oartifact == ART_IMPRACTICAL_COMBAT_WEAR) tmp += 5;
		if (uarmc && itemhasappearance(uarmc, APP_GENTLE_CLOAK) ) tmp += 1;
		if (uarmf && itemhasappearance(uarmf, APP_FETISH_HEELS)) tmp += 5;
		if (uarmf && itemhasappearance(uarmf, APP_ICEBLOCK_HEELS)) tmp += 2;
		if (uarmh && itemhasappearance(uarmh, APP_THINKING_HELMET)) tmp += 1;
		if (uarmh && itemhasappearance(uarmh, APP_COSMETIC_HELMET)) tmp += 1;
		if (uarmg && itemhasappearance(uarmg, APP_PRINCESS_GLOVES)) tmp += 2;
		if (uarmc && uarmc->oartifact == ART_MOST_CHARISMATIC_PRESIDENT) tmp += 10;
		if (uarmc && uarmc->oartifact == ART_HIGH_KING_OF_SKIRIM) tmp += 5;
		if (uarmc && uarmc->oartifact == ART_KATHERINE_S_BEACHWEAR && flags.female) tmp += 3;
		if (uarmg && uarmg->oartifact == ART_MARY_INSCRIPTION) tmp += 10;
		if (uarmf && uarmf->oartifact == ART_JULIA_S_REAL_LOVE) tmp += 3;
		if (uarmf && uarmf->oartifact == ART_ELIANE_S_COMBAT_SNEAKERS) tmp += 20;
		if (uarmf && uarmf->oartifact == ART_ALISEH_S_RED_COLOR) tmp += 10;
		if (powerfulimplants() && uimplant && uimplant->oartifact == ART_THAI_S_EROTIC_BITCH_FU) tmp += 5;
		if (powerfulimplants() && uimplant && uimplant->oartifact == ART_WONDERLOVELINESS) tmp += 5;
		if (uimplant && uimplant->oartifact == ART_WONDERLOVELINESS) tmp += 5;
		if (uwep && uwep->oartifact == ART_CERULEAN_SMASH) tmp += 10;
		if (uarmh && uarmh->oartifact == ART_TEH_PHYSIQUE) tmp += 10;
		if (uarmf && uarmf->oartifact == ART_JOHANNA_S_RED_CHARM) tmp += 5;
		if (uarmf && uarmf->oartifact == ART_DORA_S_SCRATCHY_HEELS) tmp += 3;
		if (uarmf && uarmf->oartifact == ART_KATI_S_IRRESISTIBLE_STILET) tmp += 5;
		if (uarmf && uarmf->oartifact == ART_VERENA_S_DUELING_SANDALS) tmp += 10;
		if (uarmf && uarmf->oartifact == ART_IMPOSSIBLE_CATWALK) tmp += 10;
		if (uarmf && uarmf->oartifact == ART_IN_AWE) tmp += 7;
		if (uwep && uwep->oartifact == ART_MISTY_S_MELEE_PLEASURE) tmp += 5;
		if (Race_if(PM_CUPID)) tmp += 5;
		if (uarm && uarm->oartifact == ART_GARYX) tmp += 1;
		if (uwep && uwep->oartifact == ART_HIGH_ORIENTAL_PRAISE) tmp += 3;
		if (uarmf && itemhasappearance(uarmf, APP_BEAUTIFUL_HEELS)) tmp += 5;
		if (uarmf && uarmf->oartifact == ART_CAMELIC_SCENT) tmp += 7;
		if (uarmf && uarmf->oartifact == ART_AIRSHIP_DANCING) tmp += 5;
		if (uarmf && uarmf->oartifact == ART_DEEP_SIGH) tmp += 5;
		if (uarmf && uarmf->oartifact == ART_FINAL_CHALLENGE) tmp += 10;
		if (uarmf && uarmf->oartifact == ART_OUU_EECH) tmp += 10;
		if (uarmf && uarmf->oartifact == ART_TOTALLY_THE_SEXY_BITCH) tmp += 5;
		if (uarmf && uarmf->oartifact == ART_ARABELLA_S_GIRL_KICK) tmp += 5;
		if (uarmu && uarmu->otyp == FISHNET) tmp += 2;
		if (uarmu && uarmu->oartifact == ART_CAPITAL_RAP) tmp += 5;
		if (uamul && uamul->otyp == AMULET_OF_TIME) tmp += 5;
		if (uarmu && uarmu->oartifact == ART_SWEET_VICTORIA) tmp += 3;
		if (uarmf && uarmf->oartifact == ART_CLAUDIA_S_SELF_WILL) tmp += 5;
		if (uarmg && uarmg->oartifact == ART_EGASSO_S_GIBBERISH) tmp += 5;
		if (uwep && uwep->oartifact == ART_STAFF_OF_LEIBNIZ) tmp += 1;
		if (uarmf && uarmf->oartifact == ART_SCRATCHE_HUSSY) tmp += 3;
		if (uarmf && uarmf->oartifact == ART_KATHARINA_S_LOVELINESS) tmp += 10;
		if (uwep && uwep->oartifact == ART_DAINTY_SLOAD) tmp += 3;
		if (uwep && uwep->oartifact == ART_HACHURATE) tmp += 5;
		if (FemtrapActiveNora && u.uhs == WEAK) tmp += 2;
		if (FemtrapActiveNora && u.uhs == FAINTING) tmp += 5;
		if (FemtrapActiveNora && u.uhs == FAINTED) tmp += 10;
		if (FemtrapActiveNora && u.uhs == STARVED) tmp += 25;
		if (uarmf && itemhasappearance(uarmf, APP_HOMO_SHOES) && u.homosexual == 2) tmp += 5;
		if (uarmf && uarmf->oartifact == ART_LORENZI_S_CLEANING_RESIDUE) tmp += 2;
		if (u.combatcommand) tmp += 1;
		if (uarm && uarm->oartifact == ART_SMILEY_FACE) tmp += 3;
		if (have_superjonadabstone()) tmp += 10;
		if (uimplant && uimplant->oartifact == ART_DUNGEON_BOSS__WITH_SHARP_S) tmp += (powerfulimplants() ? 2 : 1);
		if (uarmh && uarmh->otyp == CORNUTHAUM && Role_if(PM_WIZARD)) tmp += 1;
		if (uleft && uleft->otyp == RIN_ADORNMENT && !(uleft->oartifact == ART_NIX_) ) tmp += uleft->spe;
		if (uleft && uleft->otyp == RIN_ADORNMENT && (uleft->oartifact == ART_DOUBLE_ADORNING) ) tmp += uleft->spe;
		if (uleft && uleft->otyp == RIN_UPPER_ADORNMENT) tmp += uleft->spe;
		if (uright && uright->otyp == RIN_ADORNMENT && !(uright->oartifact == ART_NIX_)) tmp += uright->spe;
		if (uright && uright->otyp == RIN_ADORNMENT && (uright->oartifact == ART_DOUBLE_ADORNING) ) tmp += uright->spe;
		if (uright && uright->otyp == RIN_UPPER_ADORNMENT) tmp += uright->spe;
		if (uarmu && uarmu->oartifact == ART_MENSTRUATION_HURTS) tmp += 6;
		if (uwep && uwep->enchantment == WEAPEGO_CHA) tmp += 2;
		if (uwep && uwep->enchantment == WEAPEGO_GREAT_CHA) tmp += 5;
		if (uwep && uwep->enchantment == WEAPEGO_PHYSIS) tmp += 2;
		if (uwep && uwep->enchantment == WEAPEGO_GREAT_PHYSIS) tmp += 5;

		if (RngeCoquetry) tmp += 5;
		if (FemtrapActiveSolvejg) tmp += 5;
		if (FemtrapActiveNatalje) tmp += 10;
		if (Race_if(PM_BULDOZGAR)) tmp += 2;

		if (PlayerInHighHeels && !(PlayerCannotUseSkills)) {

			switch (P_SKILL(P_HIGH_HEELS)) {
				case P_BASIC: tmp += 1; break;
				case P_SKILLED: tmp += 2; break;
				case P_EXPERT: tmp += 3; break;
				case P_MASTER: tmp += 5; break;
				case P_GRAND_MASTER: tmp += 7; break;
				case P_SUPREME_MASTER: tmp += 10; break;

			}

		}

		if (PlayerInSexyFlats && !(PlayerCannotUseSkills)) {

			switch (P_SKILL(P_SEXY_FLATS)) {
				case P_BASIC: tmp += 1; break;
				case P_SKILLED: tmp += 2; break;
				case P_EXPERT: tmp += 3; break;
				case P_MASTER: tmp += 4; break;
				case P_GRAND_MASTER: tmp += 5; break;
				case P_SUPREME_MASTER: tmp += 6; break;

			}

		}

		if (PlayerInStilettoHeels) {
			tmp += 1;
			if (!(PlayerCannotUseSkills)) switch (P_SKILL(P_HIGH_HEELS)) {
				case P_BASIC: tmp += 1; break;
				case P_SKILLED: tmp += 2; break;
				case P_EXPERT: tmp += 3; break;
				case P_MASTER: tmp += 4; break;
				case P_GRAND_MASTER: tmp += 5; break;
				case P_SUPREME_MASTER: tmp += 6; break;

			}

		}

		if (uarmf && uarmf->oartifact == ART_STAR_SOLES) tmp -= 2;
		if (uarmf && itemhasappearance(uarmf, APP_VELCRO_SANDALS)) tmp -= 5;
		if (uarmf && itemhasappearance(uarmf, APP_BUSINESS_SHOES)) tmp -= 5;
		if (uarmc && uarmc->oartifact == ART_INGRID_S_WEATHERED_SHROUD) tmp -= 4;
		if (uarmf && itemhasappearance(uarmf, APP_GARDEN_SLIPPERS)) tmp -= 5;
		if (uarmf && itemhasappearance(uarmf, APP_UGLY_BOOTS)) tmp -= 2;
		if (uarmf && uarmf->oartifact == ART_CARMARK) tmp -= 5;
		if (uwep && uwep->oartifact == ART_KILL_KILL_PIANO) tmp -= 2;
		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_KILL_KILL_PIANO) tmp -= 2;
		if (uarmf && uarmf->oartifact == ART_ANTJE_S_POWERSTRIDE) tmp -= 5;
		if (uarmh && itemhasappearance(uarmh, APP_CORONA_MASK)) tmp -= 5;
		if (uarmh && uarmh->oartifact == ART_FFP___MASK) tmp -= 5;
		if (uarmh && uarmh->oartifact == ART_HOW_CAN_ONE_PLEASE_LOOK_LI) tmp -= 10;
		if (uarmf && itemhasappearance(uarmf, APP_BAREFOOT_SHOES)) tmp -= 1;
		if (uarmf && itemhasappearance(uarmf, APP_SANDALS_WITH_SOCKS)) tmp -= 25;
		if (uarmh && uarmh->oartifact == ART_CLELIA_S_TONGUE_BREAKER) tmp -= 3;
		if (Race_if(PM_HUMANOID_ANGEL)) tmp -= angelshadowstuff();
		if (u.tsloss_cha > 0) tmp -= u.tsloss_cha;
		if (FemtrapActiveAntje && !PlayerInBlockHeels) tmp -= 5;
		if (ublindf && ublindf->oartifact == ART_SYLBE_S_LACK) tmp -= 4;
		if (Race_if(PM_LOWER_ENT) && Burned) tmp -= 2;
		if (uarms && uarms->oartifact == ART_AL_UD) tmp -= 3;
		if (uarmh && is_power_helm(uarmh)) tmp -= 5;
		if (isbadstatter) tmp -= 2;
		if (uarmh && uarmh->otyp == CORNUTHAUM && !Role_if(PM_WIZARD)) tmp -= 1;
		if (uleft && uleft->oartifact == ART_SECOND_EXCHANGE) tmp -= 5;
		if (uright && uright->oartifact == ART_SECOND_EXCHANGE) tmp -= 5;
		if (uarmc && uarmc->oartifact == ART_MOST_SCRAPPED_PERSON_IN_EX) tmp -= 5;

		if (FemtrapActiveNora && u.uhunger > 500) {
			int norahunger = (u.uhunger - 500);
			if (norahunger < 200) norahunger = 0;
			norahunger /= 200;
			if (norahunger < 0) norahunger = 0;
			tmp--;
			tmp -= norahunger;
		}

		/* having a hemorrhage means you don't look so good... --Amy */
		if (PlayerBleeds > 100) tmp--;

		if (uimplant && uimplant->oartifact == ART_LUCK_WEAVE_OF_ARDENT_SOLWY && tmp > 1) tmp /= 2;

		if (AllStatsAreLower) tmp -= min(tmp - 3, 10);
		if (u.uprops[STATS_LOWERED].extrinsic) tmp -= min(tmp - 3, 10);
		if (have_lowstatstone()) tmp -= min(tmp - 3, 10);
		if (uarmc && uarmc->oartifact == ART_SIECHELALUER) tmp -= min(tmp - 3, 10);

	} else if (x == A_INT || x == A_WIS) {
		/* yes, this may raise int/wis if player is sufficiently
		 * stupid.  there are lower levels of cognition than "dunce".
		 */

		if (uarmf && uarmf->oartifact == ART_SOLVEJG_S_STINKING_SLIPPER) tmp -= 5; /* reduce both */
		if (uarmf && uarmf->oartifact == ART_LILAC_BEAUTY) tmp -= 5;
		if (uarmf && uarmf->oartifact == ART_STEFANJE_S_PROBLEM) tmp += 2;
		if (uwep && uwep->oartifact == ART_KNOW_YOUR_INTRINSICS) tmp += 5; /* boost both */
		if (!flags.female && uarmu && uarmu->oartifact == ART_FEMALE_BEAUTY) tmp -= 5;
		if (!flags.female && uarmu && uarmu->oartifact == ART_FEMINIST_GIRL_S_PURPLE_WEA) tmp -= 5;
		if (uarmh && uarmh->oartifact == ART_MASSIVE_IRON_CROWN_OF_MORG) tmp += 5;
		if (x == A_INT && uwep && uwep->oartifact == ART_DIKKIN_S_FAVORITE_SPELL) tmp += 8;
		if (uarmh && itemhasappearance(uarmh, APP_THINKING_HELMET)) tmp += 1;
		if (x == A_INT && uwep && uwep->oartifact == ART_RIP_STRATEGY) tmp += 5;
		if (powerfulimplants() && uimplant && uimplant->oartifact == ART_THAI_S_EROTIC_BITCH_FU) tmp += 5;
		if (uarmh && uarmh->otyp == HELM_OF_BRILLIANCE) tmp += uarmh->spe;
		if (uarmf && uarmf->oartifact == ART_JOHANNA_S_RED_CHARM) tmp += 1;
		if (uarm && uarm->oartifact == ART_GARYX) tmp += 1;
		if (uleft && uleft->oartifact == ART_HIGHER_LOCK) tmp += 1;
		if (uright && uright->oartifact == ART_HIGHER_LOCK) tmp += 1;
		if (uimplant && uimplant->oartifact == ART_CORTEX_COPROCESSOR) tmp += 2;
		if (powerfulimplants() && uimplant && uimplant->oartifact == ART_CORTEX_COPROCESSOR) tmp += 2;
		if (uwep && uwep->oartifact == ART_STAFF_OF_LEIBNIZ) tmp += 1;
		if (uleft && uleft->oartifact == ART_CROQUE_FORD) tmp += 4;
		if (uright && uright->oartifact == ART_CROQUE_FORD) tmp += 4;
		if (x == A_WIS && uwep && uwep->oartifact == ART_SUISSE_SLAY) tmp += 1;
		if (x == A_WIS && uwep && uwep->oartifact == ART_SCJWILLX_) tmp += 5;
		if (x == A_WIS && uwep && uwep->oartifact == ART_HOLY_LANCE) tmp += 6;
		if (x == A_WIS && uwep && uwep->oartifact == ART_HOLY_RANCE) tmp += 6;
		if (x == A_WIS && uarmh && uarmh->oartifact == ART_CAP_OF_THENGEL) tmp += 3;
		if (x == A_INT && Race_if(PM_MAZIN)) tmp += 2;
		if (x == A_INT && ublindf && ublindf->oartifact == ART_BERIT_S_SAGE) tmp += 10;
		if (x == A_WIS && ublindf && ublindf->oartifact == ART_BERIT_S_SAGE) tmp += 15;
		if (uarmf && uarmf->oartifact == ART_SEE_BY_LAKE) tmp += 1;
		if (uarmh && uarmh->oartifact == ART_SENOBIA_S_CROWN) tmp += 3;
		if (x == A_INT && bmwride(ART_KERSTIN_S_CLEARSOUND)) tmp += 2;
		if (x == A_INT && uball && uball->oartifact == ART_NUCULATE) tmp += 2;
		if (x == A_INT && uwep && uwep->oartifact == ART_FRISIA_S_TAIL) tmp += 20;
		if (x == A_INT && uwep && uwep->oartifact == ART_CAT_S_TAIL) tmp += 20;
		if (x == A_WIS && Race_if(PM_MAZIN)) tmp += 1;
		if (x == A_WIS && uarmh && uarmh->oartifact == ART_CLELIA_S_TONGUE_BREAKER) tmp += 4;
		if (x == A_WIS && uwep && uwep->oartifact == ART_EMERALD_SWORD) tmp += 5;
		if (x == A_WIS && uleft && uleft->oartifact == ART_ACTUAL_CAVE_DWELLING) tmp += 7;
		if (x == A_WIS && uright && uright->oartifact == ART_ACTUAL_CAVE_DWELLING) tmp += 7;
		if (x == A_WIS && uarmf && uarmf->oartifact == ART_MY_DEAR_SLAVE) tmp += 5;
		if (x == A_WIS && uarm && uarm->oartifact == ART_JACINTA_S_PRECIOUS) tmp += 4;
		if (x == A_WIS && uarm && uarm->oartifact == ART_ROBE_OF_ELARION__THE_DREAM) tmp += 5;
		if (ublindf && ublindf->oartifact == ART_LEANDRA_S_WORD) tmp += 1;
		if (uarmc && uarmc->oartifact == ART_DIN_DEN) tmp += 4;
		if (uarm && uarm->oartifact == ART_LET_S_DO_IT_ALREADY && uarm->otyp == BLUE_DRAGON_SCALE_MAIL) tmp += 3;
		if (powerfulimplants() && uimplant && uimplant->oartifact == ART_SANTANA_S_CHICANERY) tmp += 1;
		if (FemtrapActiveNora && u.uhs == WEAK) tmp += 2;
		if (FemtrapActiveNora && u.uhs == FAINTING) tmp += 5;
		if (FemtrapActiveNora && u.uhs == FAINTED) tmp += 10;
		if (FemtrapActiveNora && u.uhs == STARVED) tmp += 25;
		if (uwep && uwep->oartifact == ART_DJANGO_S_CHAIN) tmp += 5;
		if (uchain && uchain->oartifact == ART_DJANGO_S_CHAIN) tmp += 5;
		if (uamul && uamul->oartifact == ART_ROCHELLE_S_SEY) tmp += 3;
		if (x == A_WIS && uimplant && uimplant->oartifact == ART_LUCK_WEAVE_OF_ARDENT_SOLWY) tmp += 1;
		if (x == A_WIS && uleft && uleft->oartifact == ART_FALL_WARTING_NUMBER) tmp += 2;
		if (x == A_WIS && uright && uright->oartifact == ART_FALL_WARTING_NUMBER) tmp += 2;
		if (x == A_INT && uleft && uleft->oartifact == ART_WELLBEWONDO) tmp += 2;
		if (x == A_INT && uright && uright->oartifact == ART_WELLBEWONDO) tmp += 2;
		if (x == A_WIS && uleft && uleft->oartifact == ART_WELLBEWONDO) tmp += 1;
		if (x == A_WIS && uright && uright->oartifact == ART_WELLBEWONDO) tmp += 1;
		if (x == A_WIS && uleft && uleft->oartifact == ART_SUPER_WISE) tmp += (uleft->cursed ? -10 : 10);
		if (x == A_WIS && uright && uright->oartifact == ART_SUPER_WISE) tmp += (uright->cursed ? -10 : 10);
		if (x == A_WIS && uarmf && uarmf->oartifact == ART_SEVER_SAVER) tmp += 8;
		if (uball && uball->oartifact == ART_MAREA_S_EXALTITUDE) tmp += 2;
		if (x == A_INT && uwep && uwep->oartifact == ART_SPECIFI_X_) tmp += 10;
		if (x == A_INT && uwep && uwep->oartifact == ART_SHIVERING_STAFF) tmp += 4;
		if (x == A_INT && uwep && uwep->oartifact == ART_STAFF_OF_INSANITY) tmp += 4;
		if (x == A_INT && uarmf && uarmf->oartifact == ART_GODOT_S_SUIT_SHOES) tmp += 2;
		if (uarmf && uarmf->oartifact == ART_COLLEGE_ROCKZ) tmp += 3;
		if (uarm && uarm->oartifact == ART_PAUA_FOR_ME_) tmp += 1;
		if (x == A_INT && uimplant && uimplant->oartifact == ART_TEMPORAL_LOBE_OF_ZORATH_TH) tmp += 3;
		if (x == A_INT && uwep && uwep->oartifact == ART_DO_NOT_DISPLAY_THE_CHARGES) tmp += 5;
		if (x == A_INT && uwep && uwep->oartifact == ART_STAR_SLAY_GIANTS) tmp += 5;
		if (x == A_INT && uarmc && uarmc->oartifact == ART_COWSEEKER) tmp += 3;
		if (x == A_INT && uarmc && uarmc->oartifact == ART_WISDOM_OF_THE_ANCESTRAL) tmp += 6;
		if (x == A_INT && uarmh && uarmh->oartifact == ART_LADDERSORRY_TEARS) tmp += 4;
		if (x == A_WIS && uarmh && uarmh->oartifact == ART_LADDERSORRY_TEARS) tmp += 3;
		if (x == A_INT && uarmh && uarmh->oartifact == ART_GISELA_S_TRICK) tmp += 10;
		if (x == A_INT && uarmh && uarmh->oartifact == ART_ANTON_S_CROWN_OF_ECHOES) tmp += 4;
		if (uarmf && uarmf->oartifact == ART_LORENZI_S_CLEANING_RESIDUE) tmp += 2;
		if (uleft && uleft->oartifact == ART_SERPENT_RING_OF_SET) tmp += 5;
		if (uright && uright->oartifact == ART_SERPENT_RING_OF_SET) tmp += 5;
		if (uarmf && uarmf->otyp == COMMANDER_HEELS) tmp += 1;
		if (u.combatcommand) tmp += 1;
		if (Race_if(PM_WYLVAN)) tmp += 1;
		if (x == A_INT && uamul && uamul->oartifact == ART_KLENSCHGOR) tmp += 5;
		if (x == A_INT && uwep && uwep->oartifact == ART_HYPER_INTELLIGENCE) tmp += 7;
		if (x == A_INT && uarmf && uarmf->oartifact == ART_SPELLBOOTS) tmp += 1;
		if (x == A_WIS && uarmc && uarmc->oartifact == ART_EXCITEMENT_OF_HAX) tmp += 4;
		if (x == A_WIS && uimplant && uimplant->oartifact == ART_SYNAPSE_CROWN_OF_GANDALF) tmp += 2;
		if (uarm && uarm->oartifact == ART_SMILEY_FACE) tmp += 1;
		if (x == A_INT && uarms && uarms->oartifact == ART_YELLOW_STATUS) tmp += 2;
		if (x == A_INT && uarmg && uarmg->oartifact == ART_NIA_S_NEAR_MISS) tmp += 3;
		if (have_superjonadabstone()) tmp += 10;
		if (x == A_WIS && powerfulimplants() && uimplant && uimplant->oartifact == ART_GLEN_HOSPITAL) tmp += 10;
		if (x == A_INT && powerfulimplants() && uimplant && uimplant->oartifact == ART_FOOD_FOR_THOUGHT) tmp += 2;
		if (uimplant && uimplant->oartifact == ART_DUNGEON_BOSS__WITH_SHARP_S) tmp += (powerfulimplants() ? 2 : 1);
		if (x == A_INT && uleft && uleft->otyp == RIN_GAIN_INTELLIGENCE) tmp += uleft->spe;
		if (x == A_INT && uright && uright->otyp == RIN_GAIN_INTELLIGENCE) tmp += uright->spe;
		if (x == A_WIS && uleft && uleft->otyp == RIN_GAIN_WISDOM) tmp += uleft->spe;
		if (x == A_WIS && uright && uright->otyp == RIN_GAIN_WISDOM) tmp += uright->spe;
		if (x == A_INT && uleft && uleft->oartifact == ART_DEVINE_S_PRAYER) tmp += 6;
		if (x == A_INT && uright && uright->oartifact == ART_DEVINE_S_PRAYER) tmp += 6;
		if (x == A_WIS && uleft && uleft->oartifact == ART_DEVINE_S_PRAYER) tmp += 4;
		if (x == A_WIS && uright && uright->oartifact == ART_DEVINE_S_PRAYER) tmp += 4;
		if (uarmh && uarmh->oartifact == ART_PLUSMIND) tmp += 2;
		if (x == A_WIS && uarmf && uarmf->oartifact == ART_BRITTA_S_MURDER_STORY) tmp += 5;
		if (x == A_INT && uarmf && uarmf->oartifact == ART_BRITTA_S_MURDER_STORY) tmp += 7;
		if (x == A_INT && uleft && uleft->oartifact == ART_FIRST_EXCHANGE) tmp += 5;
		if (x == A_INT && uright && uright->oartifact == ART_FIRST_EXCHANGE) tmp += 5;
		if (x == A_INT && uwep && uwep->oartifact == ART_GARBAGE_STAFF) tmp += 3;
		if (x == A_WIS && uarmc && uarmc->oartifact == ART_ARABELLA_S_SAFETY_EQUIPMEN) tmp += 4;
		if (x == A_INT && uwep && uwep->oartifact == ART_KRISTA_S_CLEAR_MIND) tmp += 15;
		if (x == A_INT && uchain && uchain->oartifact == ART_IZQUXXISQUID_S_RESEARCH) tmp += 1;
		if (x == A_INT && uwep && uwep->oartifact == ART_IZQUXXISQUID_S_RESEARCH) tmp += 1;
		if (u.tempintwisboost2) tmp += 2;
		if (uarmu && uarmu->oartifact == ART_MENSTRUATION_HURTS) tmp += 6;
		if (uarm && uarm->otyp == CRYSTALLINE_DRAGON_SCALES) tmp += 1;
		if (uarm && uarm->otyp == CRYSTALLINE_DRAGON_SCALE_MAIL) tmp += 1;
		if (uarms && uarms->otyp == CRYSTALLINE_DRAGON_SCALE_SHIEL) tmp += 1;
		if (uamul && uamul->otyp == AMULET_OF_FLIPPING) {
			if (uamul->blessed) tmp++;
			if (!uamul->cursed) tmp++;
			if (uamul->cursed) tmp--;
			if (uamul->hvycurse) tmp--;
			if (uamul->prmcurse) tmp--;
			if (uamul->evilcurse) tmp--;
			if (uamul->bbrcurse) tmp--;
			if (uamul->morgcurse) tmp--;
		}
		if (x == A_WIS && u.twoweap && uswapwep && uswapwep->oartifact == ART_SACRIFICE_TONFA) tmp += 5;
		if (x == A_INT && uwep && uwep->oartifact == ART_ONCHANGE_STAFF) tmp += 3;
		if (uwep && uwep->oartifact == ART_MAGE_STAFF_OF_ETERNITY && uwep->spe > 0) tmp += uwep->spe;
		if (uamul && uamul->otyp == AMULET_OF_MENTAL_BOOST) tmp += 3;
		if (x == A_INT && uwep && uwep->enchantment == WEAPEGO_INT) tmp += 2;
		if (x == A_INT && uwep && uwep->enchantment == WEAPEGO_GREAT_INT) tmp += 5;
		if (x == A_WIS && uwep && uwep->enchantment == WEAPEGO_WIS) tmp += 2;
		if (x == A_WIS && uwep && uwep->enchantment == WEAPEGO_GREAT_WIS) tmp += 5;
		if (uwep && uwep->enchantment == WEAPEGO_SMART) tmp += 2;
		if (uwep && uwep->enchantment == WEAPEGO_GREAT_SMART) tmp += 5;

		if (x == A_WIS && uarmf && uarmf->otyp == HEEL_WISE_SHOES && !(PlayerCannotUseSkills)) {

			switch (P_SKILL(P_HIGH_HEELS)) {
				case P_BASIC: tmp += 1; break;
				case P_SKILLED: tmp += 2; break;
				case P_EXPERT: tmp += 3; break;
				case P_MASTER: tmp += 4; break;
				case P_GRAND_MASTER: tmp += 5; break;
				case P_SUPREME_MASTER: tmp += 6; break;
			}
			switch (P_SKILL(P_STILETTO_HEELS)) {
				case P_BASIC: tmp += 1; break;
				case P_SKILLED: tmp += 2; break;
				case P_EXPERT: tmp += 3; break;
				case P_MASTER: tmp += 4; break;
				case P_GRAND_MASTER: tmp += 5; break;
				case P_SUPREME_MASTER: tmp += 6; break;
			}

		}


		if (uarmh && uarmh->oartifact == ART_YOU_DON_T_KNOW_SHIT) tmp -= 3;
		if (uarmh && uarmh->oartifact == ART_TEH_PHYSIQUE) tmp -= 10;
		if (uarmf && uarmf->oartifact == ART_DORA_S_SCRATCHY_HEELS) tmp -= 2;
		if (uwep && uwep->oartifact == ART_MINOPOWER) tmp -= 3;
		if (x == A_INT && uimplant && uimplant->oartifact == ART_NERVE_HOOK_OF_CONAN) tmp -= 2;
		if (uarm && uarm->oartifact == ART_YETA_S_BEAR_BEING) tmp -= 5;
		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_MINOPOWER) tmp -= 3;
		if (x == A_INT && uwep && uwep->oartifact == ART_SKAZKA_OB_DURAKE) tmp -= 3;
		if (x == A_INT && u.twoweap && uswapwep && uswapwep->oartifact == ART_SKAZKA_OB_DURAKE) tmp -= 3;
		if (x == A_INT && uleft && uleft->oartifact == ART_ADELISA_S_HIDING_GAME) tmp -= 2;
		if (x == A_INT && uright && uright->oartifact == ART_ADELISA_S_HIDING_GAME) tmp -= 2;
		if (uarmf && uarmf->oartifact == ART_JABINE_S_ORTHOGRAPHY) tmp -= 2;

		if (x == A_INT && uarm && uarm->oartifact == ART_EITHER_INTELLIGENT_OR_FAIR && u.ualign.type != A_CHAOTIC) tmp -= 10;

		if (x == A_INT && uwep && uwep->oartifact == ART_DAEMEL) tmp -= 2;
		if (x == A_INT && u.twoweap && uswapwep && uswapwep->oartifact == ART_DAEMEL) tmp -= 2;
		if (u.martialstyle == MARTIALSTYLE_BOOYAKASHA) tmp -= 5;

		if (x == A_INT && Race_if(PM_HUMAN_MONKEY) && tmp > 9) tmp = 9;
		if (uarmh && uarmh->oartifact == ART_DUNCE_POUNCE && tmp > 6) tmp = 6;
		if (uarmf && uarmf->oartifact == ART_STAR_SOLES) tmp -= 2;
		if (uarmf && uarmf->oartifact == ART_KILLCAP && x == A_INT) tmp -= 2;
		if (PlayerBleeds > 100) tmp -= 2;
		if (Race_if(PM_HUMANOID_ANGEL)) tmp -= angelshadowstuff();
		if (u.tsloss_int > 0 && x == A_INT) tmp -= u.tsloss_int;
		if (u.tsloss_wis > 0 && x == A_WIS) tmp -= u.tsloss_wis;
		if (Race_if(PM_LOWER_ENT) && Burned) tmp -= 2;
		if (isbadstatter) tmp -= 2;
		if (u.martialstyle == MARTIALSTYLE_WRESTLING) tmp -= 1;

		if (FemtrapActiveNora && u.uhunger > 500) {
			int norahunger = (u.uhunger - 500);
			if (norahunger < 200) norahunger = 0;
			norahunger /= 200;
			if (norahunger < 0) norahunger = 0;
			tmp--;
			tmp -= norahunger;
		}

		if (AllStatsAreLower) tmp -= min(tmp - 3, 10);
		if (u.uprops[STATS_LOWERED].extrinsic) tmp -= min(tmp - 3, 10);
		if (have_lowstatstone()) tmp -= min(tmp - 3, 10);
		if (uarmc && uarmc->oartifact == ART_SIECHELALUER) tmp -= min(tmp - 3, 10);

		if (autismweaponcheck(ART_MCCAULEY_S_INTELLIGENCE) && tmp > 6 && x == A_INT) tmp = 6;
		if (uarm && uarm->oartifact == ART_DARK_MINDDRILL && tmp > 6 && x == A_INT) tmp = 6;
		if (uarmf && uarmf->oartifact == ART_KILLCAP && tmp > 6) tmp = 6;
		if (uarmh && uarmh->otyp == DUNCE_CAP) return(Role_if(PM_JESTER) ? 9 : 6);

	} else if (x == A_DEX) {

		if (uwep && uwep->oartifact == ART_SUREFIRE_GUN) tmp += (3 + uwep->spe);
		if (uarmc && uarmc->oartifact == ART_HOSTES_AD_PULVEREM_FERIRE) tmp += (5 + uarmc->spe);
		if (uarmg && uarmg->oartifact == ART_GAUNTLETS_OF_SLAYING) tmp += (3 + uarmg->spe);
		if (uarmf && uarmf->oartifact == ART_EVA_S_INCONSPICUOUS_CHARM) tmp += (uarmf->spe);
		if (uarmf && uarmf->oartifact == ART_LILAC_BEAUTY) tmp -= 5;
		if (!flags.female && uarmu && uarmu->oartifact == ART_FEMALE_BEAUTY) tmp -= 5;
		if (!flags.female && uarmu && uarmu->oartifact == ART_FEMINIST_GIRL_S_PURPLE_WEA) tmp -= 5;
		if (uarmh && uarmh->oartifact == ART_DUNCE_POUNCE) tmp += 3;
		if (uarmh && uarmh->oartifact == ART_MASSIVE_IRON_CROWN_OF_MORG) tmp += 5;
		if (uarmf && uarmf->oartifact == ART_STEFANJE_S_PROBLEM) tmp += 2;
		if (uleft && uleft->oartifact == ART_GUARANTEED_HIT_POWER) tmp += uleft->spe;
		if (uright && uright->oartifact == ART_GUARANTEED_HIT_POWER) tmp += uright->spe;
		if (uarmf && uarmf->oartifact == ART_OUT_OF_TIME) tmp += 5;
		if (uamul && uamul->oartifact == ART_ROCHELLE_S_SEY) tmp += 3;
		if (uarmf && uarmf->oartifact == ART_LIGHT_DAL_I_THALION) tmp += 2;
		if (uarmh && itemhasappearance(uarmh, APP_THINKING_HELMET)) tmp += 1;
		if (uwep && uwep->oartifact == ART_RAFSCHAR_S_SUPERWEAPON) tmp += 1;
		if (powerfulimplants() && uimplant && uimplant->oartifact == ART_THAI_S_EROTIC_BITCH_FU) tmp += 10;
		if (uarmh && uarmh->oartifact == ART_TEH_PHYSIQUE) tmp += 10;
		if (uarmc && uarmc->oartifact == ART_DIN_DEN) tmp += 4;
		if (uarmg && uarmg->oartifact == ART_BEATRIX_S_SHADOWGRASP) tmp += 3;
		if (uamul && uamul->oartifact == ART_DO_NOT_FORGET_GRACE) tmp += 10;
		if (uarmg && uarmg->oartifact == ART_LE_MOUVEMENT_DE_LA_BOUCHE_) tmp += 2;
		if (uarmg && uarmg->oartifact == ART_SABRINA_S_UTILITY_GLOVES) tmp += 2;
		if (uarm && uarm->oartifact == ART_ELETLIGHT) tmp += 4;
		if (uimplant && uimplant->oartifact == ART_LUNG_INFUSER_OF_LINK) tmp += 1;
		if (uarmf && uarmf->oartifact == ART_JOHANNA_S_RED_CHARM) tmp += 1;
		if (uleft && uleft->oartifact == ART_HOH_UNNE) tmp += 5;
		if (uright && uright->oartifact == ART_HOH_UNNE) tmp += 5;
		if (uarm && uarm->oartifact == ART_HANDSOME_THREESOME) tmp += 3;
		if (uwep && uwep->oartifact == ART_BOW_OF_VINDERRE) tmp += 4;
		if (uwep && uwep->oartifact == ART_PLAY_FIGUT_EIGHT) tmp += 8;
		if (uarmg && uarmg->oartifact == ART_PENTECOSTE) tmp += 5;
		if (uarm && uarm->oartifact == ART_EMRYS_BACKSCALE) tmp += 3;
		if (uwep && uwep->oartifact == ART_SHAKING_BOW) tmp += 4;
		if (uimplant && uimplant->oartifact == ART_FOCUS_GEM_OF_KAEDRA_MOONVE) tmp += 1;
		if (uimplant && uimplant->oartifact == ART_MEMORY_CHIP_OF_RED_SONJA) tmp += 2;
		if (uimplant && uimplant->oartifact == ART_TENDON_COIL_OF_DRAXUN_THE_) tmp += 3;
		if (uimplant && uimplant->oartifact == ART_OCULAR_LENS_OF_DRIZZT) tmp += 3;
		if (uimplant && uimplant->oartifact == ART_ANCOURAGEOUS) tmp += (powerfulimplants() ? 10 : 3);
		if (uarmf && uarmf->oartifact == ART_CAMISHA_THE_INCONSPICUOUS) tmp += 5;
		if (uarmc && uarmc->oartifact == ART_ISHITA_S_OVERWHELMING) tmp += 3;
		if (uarmf && uarmf->oartifact == ART_DORA_S_SCRATCHY_HEELS) tmp += 1;
		if (uarmg && uarmg->oartifact == ART_ELARA_S_AGILITY) tmp += 5;
		if (uarmf && uarmf->oartifact == ART_RONIKA_S_NORM) tmp += 1;
		if (uarmc && uarmc->oartifact == ART_BERTILLE_S_MOUSY_ATTITUDE) tmp += 3;
		if (uleft && uleft->oartifact == ART_ADELISA_S_HIDING_GAME) tmp += 3;
		if (uright && uright->oartifact == ART_ADELISA_S_HIDING_GAME) tmp += 3;
		if (uarmc && uarmc->oartifact == ART_ANCHORAGE_BATTLECOAT) tmp += 5;
		if (uarmh && uarmh->oartifact == ART_CUMBERSOME_DESC) tmp += 2;
		if (uleft && uleft->oartifact == ART_MAHPTY) tmp += 1;
		if (uright && uright->oartifact == ART_MAHPTY) tmp += 1;
		if (uarm && uarm->oartifact == ART_GARYX) tmp += 1;
		if (uarmf && uarmf->oartifact == ART_AMATEURSPORTS) tmp += 3;
		if (uarmc && uarmc->oartifact == ART_EXCITEMENT_OF_HAX) tmp += 3;
		if (uleft && uleft->oartifact == ART_CROQUE_FORD) tmp += 4;
		if (uright && uright->oartifact == ART_CROQUE_FORD) tmp += 4;
		if (uleft && uleft->oartifact == ART_HEXTROSE) tmp += 6;
		if (uright && uright->oartifact == ART_HEXTROSE) tmp += 6;
		if (u.boosttimer) tmp += 10;
		if (uwep && uwep->oartifact == ART_DJANGO_S_CHAIN) tmp += 5;
		if (uchain && uchain->oartifact == ART_DJANGO_S_CHAIN) tmp += 5;
		if (uarm && uarm->oartifact == ART_AGILITATE) tmp += 7;
		if (uarmf && uarmf->oartifact == ART_SEE_BY_LAKE) tmp += 1;
		if (uwep && uwep->oartifact == ART_ISSEGADA_KEN) tmp += 3;
		if (uamul && uamul->otyp == AMULET_OF_PHYSICAL_BOOST) tmp += 3;
		if (uarm && uarm->oartifact == ART_GREYSHADE) tmp += 5;
		if (uarmg && itemhasappearance(uarmg, APP_DEXTROUS_GLOVES)) tmp += 3;
		if (uarmf && uarmf->oartifact == ART_MELTEM_S_FEELING) tmp += 2;
		if (uwep && uwep->oartifact == ART_HEAVY_CROSSBOW_OF_ETERNITY) tmp += 10;
		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_SABER_TONFA) tmp += 5;
		if (uball && uball->oartifact == ART_MAREA_S_EXALTITUDE) tmp += 2;
		if (uarmf && uarmf->oartifact == ART_EVERYWHERE_AT_ONCE) tmp += 3;
		if (uarmf && uarmf->oartifact == ART_IN_AWE) tmp += 5;
		if (uarmg && uarmg->oartifact == ART_NALI_THE_BNALI) tmp += 2;
		if (uarmf && uarmf->otyp == COMMANDER_HEELS) tmp += 1;
		if (uleft && uleft->oartifact == ART_HIGHER_LOCK) tmp += 1;
		if (uright && uright->oartifact == ART_HIGHER_LOCK) tmp += 1;
		if (uwep && uwep->oartifact == ART_STAFF_OF_LEIBNIZ) tmp += 1;
		if (uwep && uwep->oartifact == ART_STAR_SLAY_GIANTS) tmp += 5;
		if (ublindf && ublindf->oartifact == ART_LEANDRA_S_WORD) tmp += 1;
		if (uarmc && uarmc->oartifact == ART_ARABELLA_S_SAFETY_EQUIPMEN) tmp += 8;
		if (uwep && uwep->oartifact == ART_LONGBONE_OF_BANANA) tmp += 3;
		if (uarmc && uarmc->oartifact == ART_COWSEEKER) tmp += 3;
		if (powerfulimplants() && uimplant && uimplant->oartifact == ART_SANTANA_S_CHICANERY) tmp += 1;
		if (uwep && uwep->oartifact == ART_YORSHKA_S_SPEAR) tmp += 6;
		if (uarmg && uarmg->oartifact == ART_GAUNTLETS_OF_THE_BERSERKER) tmp += 5;
		if (uwep && uwep->oartifact == ART_LONGBOW_OF_BANANA) tmp += 3;
		if (uarm && uarm->oartifact == ART_SEE_THE_MULCH_STATE) tmp += 3;
		if (uarmc && uarmc->oartifact == ART_JUST_TO_HAVE_IT) tmp += 1;
		if (uleft && uleft->oartifact == ART_SPEECHBREAK) tmp += 5;
		if (u.martialstyle == MARTIALSTYLE_WRESTLING) tmp += 1;
		if (uright && uright->oartifact == ART_SPEECHBREAK) tmp += 5;
		if (uarm && uarm->oartifact == ART_LESS_PROTECTED_GLASS_CANNO) tmp += 15;
		if (uarm && uarm->otyp == CRYSTALLINE_DRAGON_SCALES) tmp += 1;
		if (uarm && uarm->otyp == CRYSTALLINE_DRAGON_SCALE_MAIL) tmp += 1;
		if (uarms && uarms->otyp == CRYSTALLINE_DRAGON_SCALE_SHIEL) tmp += 1;
		if (uamul && uamul->otyp == AMULET_OF_FLIPPING) {
			if (uamul->blessed) tmp++;
			if (!uamul->cursed) tmp++;
			if (uamul->cursed) tmp--;
			if (uamul->hvycurse) tmp--;
			if (uamul->prmcurse) tmp--;
			if (uamul->evilcurse) tmp--;
			if (uamul->bbrcurse) tmp--;
			if (uamul->morgcurse) tmp--;
		}
		if (FemtrapActiveNora && u.uhs == WEAK) tmp += 2;
		if (FemtrapActiveNora && u.uhs == FAINTING) tmp += 5;
		if (FemtrapActiveNora && u.uhs == FAINTED) tmp += 10;
		if (FemtrapActiveNora && u.uhs == STARVED) tmp += 25;
		if (uarmg && uarmg->otyp == GAUNTLETS_OF_DEXTERITY) tmp += uarmg->spe;
		if (uarmg && uarmg->otyp == SUPERPOWER_GAUNTLETS) tmp += uarmg->spe;
		if (uarms && uarms->oartifact == ART_YOHUALLI_TEZCATL) tmp += 2;
		if (uarmf && uarmf->oartifact == ART_LORENZI_S_CLEANING_RESIDUE) tmp += 2;
		if (uarmg && uarmg->oartifact == ART_A_LITTLE_SUGAR) tmp += 3;
		if (u.combatcommand) tmp += 1;
		if (Race_if(PM_THRALL)) tmp += 3;
		if (uwep && uwep->oartifact == ART_PHYSSTATBOOST) tmp += 5;
		if (uarmg && uarmg->oartifact == ART_DEXBONUS) tmp += 5;
		if (uarm && uarm->oartifact == ART_SMILEY_FACE) tmp += 1;
		if (have_superjonadabstone()) tmp += 10;
		if (uimplant && uimplant->oartifact == ART_DUNGEON_BOSS__WITH_SHARP_S) tmp += (powerfulimplants() ? 2 : 1);
		if (uleft && uleft->otyp == RIN_GAIN_DEXTERITY) tmp += uleft->spe;
		if (uright && uright->otyp == RIN_GAIN_DEXTERITY) tmp += uright->spe;
		if (uarmg && uarmg->oartifact == ART_LIKE_GRACE) tmp += 5;
		if (uleft && uleft->oartifact == ART_SECOND_EXCHANGE) tmp += 5;
		if (uright && uright->oartifact == ART_SECOND_EXCHANGE) tmp += 5;
		if (uarmu && uarmu->oartifact == ART_MENSTRUATION_HURTS) tmp += 6;

		if (uwep && uwep->enchantment == WEAPEGO_DEX) tmp += 2;
		if (uwep && uwep->enchantment == WEAPEGO_GREAT_DEX) tmp += 5;
		if (uwep && uwep->enchantment == WEAPEGO_GRACE) tmp += 2;
		if (uwep && uwep->enchantment == WEAPEGO_GREAT_GRACE) tmp += 5;
		if (uwep && uwep->oartifact == ART_EORLINGAS) tmp += 3;

		if (u.martialstyle == MARTIALSTYLE_JUDO && !uwep && (!u.twoweap || !uswapwep)) {
			tmp += 4;
			if (!(PlayerCannotUseSkills)) {
				switch (P_SKILL(P_MARTIAL_ARTS)) {
					case P_BASIC: tmp += 1; break;
					case P_SKILLED: tmp += 2; break;
					case P_EXPERT: tmp += 3; break;
					case P_MASTER: tmp += 4; break;
					case P_GRAND_MASTER: tmp += 5; break;
					case P_SUPREME_MASTER: tmp += 6; break;
				}
			}
		}

		if (!(PlayerCannotUseSkills)) {
			if (PlayerInStilettoHeels) {
				switch (P_SKILL(P_STILETTO_HEELS)) {
					case P_BASIC: tmp += 1; break;
					case P_SKILLED: tmp += 2; break;
					case P_EXPERT: tmp += 3; break;
					case P_MASTER: tmp += 4; break;
					case P_GRAND_MASTER: tmp += 5; break;
					case P_SUPREME_MASTER: tmp += 6; break;
				}
			}
			if (PlayerInConeHeels) {
				switch (P_SKILL(P_CONE_HEELS)) {
					case P_BASIC: tmp += 1; break;
					case P_SKILLED: tmp += 2; break;
					case P_EXPERT: tmp += 3; break;
					case P_MASTER: tmp += 4; break;
					case P_GRAND_MASTER: tmp += 5; break;
					case P_SUPREME_MASTER: tmp += 6; break;
				}
			}
			if (PlayerInBlockHeels) {
				switch (P_SKILL(P_BLOCK_HEELS)) {
					case P_BASIC: tmp += 1; break;
					case P_SKILLED: tmp += 2; break;
					case P_EXPERT: tmp += 3; break;
					case P_MASTER: tmp += 4; break;
					case P_GRAND_MASTER: tmp += 5; break;
					case P_SUPREME_MASTER: tmp += 6; break;
				}
			}
			if (PlayerInWedgeHeels) {
				switch (P_SKILL(P_WEDGE_HEELS)) {
					case P_BASIC: tmp += 1; break;
					case P_SKILLED: tmp += 2; break;
					case P_EXPERT: tmp += 3; break;
					case P_MASTER: tmp += 4; break;
					case P_GRAND_MASTER: tmp += 5; break;
					case P_SUPREME_MASTER: tmp += 6; break;
				}
			}
		}

		if (FemtrapActiveThai) tmp -= 2;
		if (PlayerBleeds > 100) tmp -= 2;
		if (uarmc && uarmc->oartifact == ART_ROKKO_CHAN_S_SUIT && tmp > 12) tmp = 12;
		if (uarmf && uarmf->oartifact == ART_SKIN_AND_BONE && tmp > 6) tmp = 6;
		if (uarmf && uarmf->oartifact == ART_STAR_SOLES) tmp -= 2;
		if (Race_if(PM_HUMANOID_ANGEL)) tmp -= angelshadowstuff();
		if (u.tsloss_dex > 0) tmp -= u.tsloss_dex;
		if (Race_if(PM_LOWER_ENT) && Burned) tmp -= 2;
		if (uarmh && uarmh->oartifact == ART_EKOLOT_L) tmp -= 1;
		if (uarmg && uarmg->oartifact == ART_BRIGITTE_S_SAFETY_MITTENS) tmp -= 3;
		if (isbadstatter) tmp -= 2;
		if (HWounded_legs) tmp--;
		if (EWounded_legs) tmp--;
		if (u.martialstyle == MARTIALSTYLE_MARSHALARTS) tmp -= 3;
		if (uarm && uarm->oartifact == ART_ANTOCONT) tmp -= 5;

		if (FemtrapActiveNora && u.uhunger > 500) {
			int norahunger = (u.uhunger - 500);
			if (norahunger < 200) norahunger = 0;
			norahunger /= 200;
			if (norahunger < 0) norahunger = 0;
			tmp--;
			tmp -= norahunger;
		}

		if (AllStatsAreLower) tmp -= min(tmp - 3, 10);
		if (u.uprops[STATS_LOWERED].extrinsic) tmp -= min(tmp - 3, 10);
		if (have_lowstatstone()) tmp -= min(tmp - 3, 10);
		if (uarmc && uarmc->oartifact == ART_SIECHELALUER) tmp -= min(tmp - 3, 10);

	} else { /* A_CON */

		if (uarmf && uarmf->oartifact == ART_LILAC_BEAUTY) tmp -= 5;
		if (!flags.female && uarmu && uarmu->oartifact == ART_FEMALE_BEAUTY) tmp -= 5;
		if (!flags.female && uarmu && uarmu->oartifact == ART_FEMINIST_GIRL_S_PURPLE_WEA) tmp -= 5;
		if (uarmh && uarmh->oartifact == ART_MASSIVE_IRON_CROWN_OF_MORG) tmp += 5;
		if (uarmh && itemhasappearance(uarmh, APP_RUBYNUS_HELMET)) tmp += 2;
		if (uarmh && itemhasappearance(uarmh, APP_THINKING_HELMET)) tmp += 1;
		if (uarmh && uarmh->oartifact == ART_ELONA_S_SNAIL_TRAIL && Race_if(PM_ELONA_SNAIL)) tmp += 10;
		if (uarmf && uarmf->oartifact == ART_STEFANJE_S_PROBLEM) tmp += 2;
		if (x == A_INT && uwep && uwep->oartifact == ART_RIP_STRATEGY) tmp += 5;
		if (uimplant && uimplant->oartifact == ART_THAI_S_EROTIC_BITCH_FU) tmp -= 10;
		if (uamul && uamul->oartifact == ART_ROCHELLE_S_SEY) tmp += 3;
		if (uarmf && uarmf->oartifact == ART_CRUEL_GODDESS_ANA) tmp -= 5;
		if (uarmc && uarmc->oartifact == ART_TOO_MANY_AFFIXES) tmp += 3;
		if (uarmh && uarmh->oartifact == ART_TEH_PHYSIQUE) tmp += 10;
		if (uarmf && uarmf->oartifact == ART_JOHANNA_S_RED_CHARM) tmp += 1;
		if (uleft && uleft->oartifact == ART_FISFIS) tmp += 3;
		if (uright && uright->oartifact == ART_FISFIS) tmp += 3;
		if (uarmc && uarmc->oartifact == ART_DIN_DEN) tmp += 4;
		if (uarmu && uarmu->oartifact == ART_SETH_S_ERROR) tmp += 3;
		if (uarm && uarm->oartifact == ART_GARYX) tmp += 1;
		if (uarm && uarm->oartifact == ART_FUCK_UGGHH_THAT_S_HEAVY_) tmp += 4;
		if (uwep && uwep->oartifact == ART_HIRYU_TO) tmp += 6;
		if (uwep && uwep->oartifact == ART_FLYING_DRAGON_SWORD) tmp += 6;
		if (uleft && uleft->oartifact == ART_CROQUE_FORD) tmp += 4;
		if (uright && uright->oartifact == ART_CROQUE_FORD) tmp += 4;
		if (uleft && uleft->oartifact == ART_BAHA_HARD) tmp += 5;
		if (uright && uright->oartifact == ART_BAHA_HARD) tmp += 5;
		if (uarmg && uarmg->oartifact == ART_GLENNIS_DOWNS) tmp += 8;
		if (uimplant && uimplant->oartifact == ART_EARCLIP_OF_BEOWULF) tmp += 2;
		if (uimplant && uimplant->oartifact == ART_NEURAL_GRAFT_OF_VAELRIC_TH) tmp += 2;
		if (uimplant && uimplant->oartifact == ART_DIDI_S_LEG_PAINTING) tmp += 1;
		if (uimplant && uimplant->oartifact == ART_VITAELITAET) tmp += (powerfulimplants() ? 12 : 5);
		if (Race_if(PM_BULDOZGAR)) tmp += 2;
		if (uwep && uwep->oartifact == ART_EXTREME_ENCHANTMENT) tmp += 10;
		if (uarmf && uarmf->oartifact == ART_SEE_BY_LAKE) tmp += 1;
		if (uwep && uwep->oartifact == ART_DJANGO_S_CHAIN) tmp += 5;
		if (uchain && uchain->oartifact == ART_DJANGO_S_CHAIN) tmp += 5;
		if (uarm && uarm->oartifact == ART_ULTRAVIOLET_RAINBOW) tmp += 3;
		if (uarmh && uarmh->oartifact == ART_BABY_JOYN_ME_IN_THERE) tmp += 1;
		if (uwep && uwep->oartifact == ART_STAFF_OF_LEIBNIZ) tmp += 1;
		if (uarmf && uarmf->oartifact == ART_DESEAMING_GAME) tmp += 3;
		if (uarms && uarms->oartifact == ART_CRIMSON_JACKAL) tmp += 2;
		if (uarmc && uarmc->oartifact == ART_KINLEY_S_WIDENESS) tmp += 2;
		if (bmwride(ART_PANZER_TANK)) tmp += 10;
		if (uwep && uwep->oartifact == ART_SEA_ANGERANCHOR) tmp += 7;
		if (uwep && uwep->oartifact == ART_FUNE_NO_IKARI) tmp += 7;
		if (uarmu && uarmu->oartifact == ART_GUAH_AVERT_SUEUE) tmp += 8;
		if (FemtrapActiveNora && u.uhs == WEAK) tmp += 2;
		if (FemtrapActiveNora && u.uhs == FAINTING) tmp += 5;
		if (uarm && uarm->oartifact == ART_ANTOCONT) tmp += 5;
		if (ublindf && ublindf->oartifact == ART_LEANDRA_S_WORD) tmp += 1;
		if (uarmf && uarmf->otyp == PERMANENCE_BOOTS) tmp += (uarmf->spe + 5);
		if (uarmf && uarmf->oartifact == ART_LEXI_S_POWERKICK) tmp += 5;
		if (uarmc && uarmc->oartifact == ART_SHROUD) tmp += 7;
		if (FemtrapActiveNora && u.uhs == FAINTED) tmp += 10;
		if (uball && uball->oartifact == ART_MAREA_S_EXALTITUDE) tmp += 2;
		if (uwep && uwep->oartifact == ART_LONG_SWORD_OF_ETERNITY) tmp += 10;
		if (uarms && uarms->oartifact == ART_HOSTRO) tmp += 3;
		if (uarm && uarm->oartifact == ART_EMRYS_BACKSCALE) tmp += 3;
		if (powerfulimplants() && uimplant && uimplant->oartifact == ART_SANTANA_S_CHICANERY) tmp += 1;
		if (uleft && uleft->oartifact == ART_MAHPTY) tmp += 3;
		if (uright && uright->oartifact == ART_MAHPTY) tmp += 3;
		if (FemtrapActiveNora && u.uhs == STARVED) tmp += 25;
		if (uarmf && uarmf->oartifact == ART_LORENZI_S_CLEANING_RESIDUE) tmp += 2;
		if (uarmg && uarmg->oartifact == ART_GAUNTLETS_OF_THE_BERSERKER) tmp += 10;
		if (uwep && uwep->oartifact == ART_SPECIFI_X_) tmp += 5;
		if (uarms && uarms->oartifact == ART_PLANK_OF_CARNEADES) tmp += 9;
		if (uleft && uleft->oartifact == ART_HIGHER_LOCK) tmp += 1;
		if (uright && uright->oartifact == ART_HIGHER_LOCK) tmp += 1;
		if (uarm && uarm->otyp == CRYSTALLINE_DRAGON_SCALES) tmp += 1;
		if (uarm && uarm->otyp == CRYSTALLINE_DRAGON_SCALE_MAIL) tmp += 1;
		if (uarms && uarms->otyp == CRYSTALLINE_DRAGON_SCALE_SHIEL) tmp += 1;
		if (uimplant && uimplant->oartifact == ART_CORELINK_OF_CU_CHULAINN) tmp += 1;
		if (uarmf && uarmf->oartifact == ART_JONACE_S_TALLNESS) tmp += 10;
		if (uamul && uamul->oartifact == ART_______SCORE) tmp += 2;
		if (uamul && uamul->otyp == AMULET_OF_FLIPPING) {
			if (uamul->blessed) tmp++;
			if (!uamul->cursed) tmp++;
			if (uamul->cursed) tmp--;
			if (uamul->hvycurse) tmp--;
			if (uamul->prmcurse) tmp--;
			if (uamul->evilcurse) tmp--;
			if (uamul->bbrcurse) tmp--;
			if (uamul->morgcurse) tmp--;
		}
		if (u.combatcommand) tmp += 1;
		if (uwep && uwep->oartifact == ART_YORSHKA_S_SPEAR) tmp += 6;
		if (uarms && uarms->oartifact == ART_VITALITY_STORM) tmp += 8;
		if (uarms && uarms->oartifact == ART_FETTIS_SLOT) tmp += 3;
		if (uwep && uwep->oartifact == ART_OGRE_POWER) tmp += 3;
		if (uarmf && uarmf->otyp == COMMANDER_HEELS) tmp += 1;
		if (Race_if(PM_URGOTH)) tmp += 3;
		if (uwep && uwep->oartifact == ART_GONDOLIN_S_HIDDEN_PASSAGE) tmp += 5;
		if (uarmc && uarmc->oartifact == ART_CORNY_DOOD && !flags.female) tmp += 3;
		if (uwep && uwep->oartifact == ART_PHYSSTATBOOST) tmp += 5;
		if (uarm && uarm->oartifact == ART_SMILEY_FACE) tmp += 1;
		if (have_superjonadabstone()) tmp += 10;
		if (uimplant && uimplant->oartifact == ART_DUNGEON_BOSS__WITH_SHARP_S) tmp += (powerfulimplants() ? 2 : 1);
		if (uleft && uleft->otyp == RIN_GAIN_CONSTITUTION) tmp += uleft->spe;
		if (uright && uright->otyp == RIN_GAIN_CONSTITUTION) tmp += uright->spe;
		if (uwep && uwep->oartifact == ART_GARBAGE_STAFF) tmp += 3;
		if (uarmu && uarmu->oartifact == ART_MENSTRUATION_HURTS) tmp += 6;
		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_SACRIFICE_TONFA) tmp += 2;
		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_SHIELD_TONFA) tmp += 3;

		if (uwep && uwep->enchantment == WEAPEGO_CON) tmp += 2;
		if (uwep && uwep->enchantment == WEAPEGO_GREAT_CON) tmp += 5;
		if (uwep && uwep->enchantment == WEAPEGO_PHYSIS) tmp += 2;
		if (uwep && uwep->enchantment == WEAPEGO_GREAT_PHYSIS) tmp += 5;

		if (uamul && uamul->oartifact == ART_MOSH_PIT_SCRAMBLE) {
			if (uarm && is_metallic(uarm)) tmp++;
			if (uarmu && is_metallic(uarmu)) tmp++;
			if (uarmc && is_metallic(uarmc)) tmp++;
			if (uarmh && is_metallic(uarmh)) tmp++;
			if (uarmf && is_metallic(uarmf)) tmp++;
			if (uarms && is_metallic(uarms)) tmp++;
			if (uarmg && is_metallic(uarmg)) tmp++;
		}

		if (uwep && uwep->oartifact == ART_JAPANESE_WOMEN && tmp > 14) tmp = 14;
		if (u.twoweap && uswapwep && uswapwep->oartifact == ART_JAPANESE_WOMEN && tmp > 14) tmp = 14;

		if (FemtrapActiveJessica) tmp -= 5;
		if (FemtrapActiveIna) tmp -= 3;
		if (PlayerBleeds > 50) tmp--;
		if (PlayerBleeds > 100) tmp -= 2;

		if (FemtrapActiveNora && u.uhunger > 500) {
			int norahunger = (u.uhunger - 500);
			if (norahunger < 200) norahunger = 0;
			norahunger /= 200;
			if (norahunger < 0) norahunger = 0;
			tmp--;
			tmp -= norahunger;
		}

		if (uarmf && uarmf->oartifact == ART_STAR_SOLES) tmp -= 2;
		if (Race_if(PM_HUMANOID_ANGEL)) tmp -= angelshadowstuff();
		if (u.tsloss_con > 0) tmp -= u.tsloss_con;
		if (Race_if(PM_LOWER_ENT) && Burned) tmp -= 2;
		if (isbadstatter) tmp -= 2;
		if (uarmu && uarmu->oartifact == ART_TILLMANN_S_TARGET) tmp -= 5;
		if (uarmf && uarmf->oartifact == ART_FAR_EAST_RELATION) tmp -= 3;
		if (u.martialstyle == MARTIALSTYLE_BOOYAKASHA) tmp -= 5;
		if (uleft && uleft->oartifact == ART_ADELISA_S_HIDING_GAME) tmp -= 2;
		if (uright && uright->oartifact == ART_ADELISA_S_HIDING_GAME) tmp -= 2;

		if (AllStatsAreLower) tmp -= min(tmp - 3, 10);
		if (u.uprops[STATS_LOWERED].extrinsic) tmp -= min(tmp - 3, 10);
		if (have_lowstatstone()) tmp -= min(tmp - 3, 10);
		if (uarmc && uarmc->oartifact == ART_SIECHELALUER) tmp -= min(tmp - 3, 10);

	}

	if (Race_if(PM_PERVERT)) { /* debuff when not praying or having sex every once in a while --Amy */
		int reductorval = u.pervertsex;
		while (reductorval >= 10000) {
			reductorval -= 10000;
			tmp--;
		}
		reductorval = u.pervertpray;
		while (reductorval >= 10000) {
			reductorval -= 10000;
			tmp--;
		}
	}

#ifdef WIN32_BUG
	return(x=((tmp >= maxvalue) ? maxvalue : (tmp <= 1) ? 1 : tmp));
#else
	return((schar)((tmp >= maxvalue) ? maxvalue : (tmp <= 1) ? 1 : tmp));
#endif
}

/* condense clumsy ACURR(A_STR) value into value that fits into game formulas
 */
schar
acurrstr()
{
	register int str = ACURR(A_STR);

	if (str <= 18) return str;
	if (str <= 121) return (19 + str / 50); /* map to 19-21 */
	else return str - 100;
}

#endif /* OVL0 */
#ifdef OVL2

void
increasesincounter(n)
register int n;
{
	if (uarmh && uarmh->otyp == GANGSTER_CAP && rn2(2)) return;
	
	u.ualign.sins += n;
}

/* avoid possible problems with alignment overflow, and provide a centralized
 * location for any future alignment limits
 */
void
adjalign(n)
register int n;
{
	if (uarmf && uarmf->oartifact == ART_MANDY_S_RAIDWEAR && !rn2(2) && (n < 0)) {
		return;
	}

	if (uarmf && uarmf->oartifact == ART_SUNA_S_CONTROVERSY_MATTER && !rn2(2) && (n < 0)) {
		return;
	}

	if (uwep && uwep->oartifact == ART_EN_GARDE____TOUCHE_ && !rn2(2) && (n < 0)) {
		return;
	}

	if (uarmh && uarmh->oartifact == ART_VERCHANGE_CHILD && !rn2(2) && (n < 0)) {
		return;
	}

	if ((n < 0) && uarm && uarm->otyp == BROTHERHOOD_POWER_ARMOR && uarmh && uarmh->otyp == BROTHERHOOD_POWER_HELM && !rn2(2) && (u.ualign.type == A_LAWFUL) ) {
		return;
	}

	if ((n < 0) && uarm && uarm->otyp == OUTCAST_POWER_ARMOR && uarmh && uarmh->otyp == OUTCAST_POWER_HELM && !rn2(2) && (u.ualign.type == A_NEUTRAL) ) {
		return;
	}

	if ((n < 0) && uarm && uarm->otyp == ENCLAVE_POWER_ARMOR && uarmh && uarmh->otyp == ENCLAVE_POWER_HELM && !rn2(2) && (u.ualign.type == A_CHAOTIC) ) {
		return;
	}

	if (uwep && uwep->oartifact == ART_PIPPA_S_URGE && !rn2(2) && (n < 0)) {
		return;
	}

	if (uwep && uwep->otyp == HONOR_KATANA && n < 0) attrcurse();
	if (u.twoweap && uswapwep && uswapwep->otyp == HONOR_KATANA && n < 0) attrcurse();

	if (Race_if(PM_SINNER) && n < 0) {
		n *= 10;
		increasesincounter(1);
		u.alignlim -= 1;
	}
	if (Race_if(PM_KORONST) && n < 0) n *= 3;
	if (Race_if(PM_BABYLONIAN) && n > 0 && rn2(2)) n *= 2;
	if (uleft && uleft->oartifact == ART_DEVINE_S_PRAYER && n > 0) n *= 2;
	if (uright && uright->oartifact == ART_DEVINE_S_PRAYER && n > 0) n *= 2;
	if (u.martialstyle == MARTIALSTYLE_BOOYAKASHA && n < 0) n *= 2;

	if (uimplant && uimplant->oartifact == ART_SINFUL_REPENTER && n > 0) {
		if (powerfulimplants()) n *= 5;
		else n *= 2;

		if (powerfulimplants()) u.alignlim += 1;
	}

	if (uarmh && uarmh->otyp == GANGSTER_CAP && n < 0) {
		n /= 2;
		if (n >= 0) n = -1; /* don't reduce below -1 --Amy */
	}

	if (uarmh && uarmh->oartifact == ART_B_A_L_L_A_S && n < 0 && u.ualign.type == A_CHAOTIC) {
		n /= 2;
		if (n >= 0) n = -1; /* don't reduce below -1 --Amy */
	}

	if (uarmf && uarmf->oartifact == ART_LOIS_S_CHILL && n < 0) {
		n /= 3;
		if (n >= 0) n = -1; /* don't reduce below -1 --Amy */
	}

	register int newalign = u.ualign.record + n;

	if (n < 0) {
		if (newalign < u.ualign.record)
			u.ualign.record = newalign;
	} else
		if (newalign > u.ualign.record) {
			u.ualign.record = newalign;
			if(u.ualign.record > u.alignlim)
				u.ualign.record = u.alignlim;
		}

	if (Race_if(PM_UNALIGNMENT_THING) || (uarmh && uarmh->oartifact == ART_FIVE_HORNED_HELM)) {
		if (n < 0) pline("You lost %d alignment points; your new value is %d.", abs(n), u.ualign.record);
		if (n > 0) pline("You gained %d alignment points; your new value is %d.", n, u.ualign.record);
	}

	/* evil patch idea by jonadab: losing alignment points can result in punishment
	 * immunizer can lose alignment during level changes, which would result in a panic; poison the player instead --Amy */
	if (n < 0 && u.ualign.record < 0 && !rn2(500)) {
		if (in_mklev) poisoned("The alignment", rn2(A_MAX), "immunized alignment failure", 30);
		else punishx();
	}

	if (n < 0 && Race_if(PM_CHIQUAI) && program_state.something_worth_saving) {
		badeffect();
	}

	if (n < 0 && Race_if(PM_JAPURA)) {
		losehp(rnd(-n), "violating the bushido", KILLED_BY);
		if (!rn2(20)) {
			if (u.uhpmax < 2) {
				u.youaredead = 1;
				pline("Due to all of your conduct violations, the gods declare you dead. Goodbye.");
				killer_format = KILLED_BY;
				killer = "constantly violating the holy conduct";
				done(DIED);
				u.youaredead = 0;

			} else {
				u.uhpmax--;
			}
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		}
	}

}

#endif /* OVL2 */


/** Returns the hitpoints of your current form. */
int
uhp()
{
	return (Upolyd ? u.mh : u.uhp);
}

/** Returns the maximal hitpoints of your current form. */
int
uhpmax()
{
	return (Upolyd ? u.mhmax : u.uhpmax);
}

/* restore one point of a drained stat (select one at random if several are drained) --Amy */
void
restore_drained_stat()
{
	int whichstat = rnd(6);
	int attempts = 6;
	while (attempts > 0) {

		switch (whichstat) {
			case 1:
				if (u.tsloss_str > 0) {
					u.tsloss_str--;
					You_feel("stronger!");
					return;
				}
				break;
			case 2:
				if (u.tsloss_dex > 0) {
					u.tsloss_dex--;
					You_feel("nimbler!");
					return;
				}
				break;
			case 3:
				if (u.tsloss_con > 0) {
					u.tsloss_con--;
					You_feel("hardier!");
					return;
				}
				break;
			case 4:
				if (u.tsloss_wis > 0) {
					u.tsloss_wis--;
					You_feel("wiser!");
					return;
				}
				break;
			case 5:
				if (u.tsloss_int > 0) {
					u.tsloss_int--;
					You_feel("smarter!");
					return;
				}
				break;
			case 6:
				if (u.tsloss_cha > 0) {
					u.tsloss_cha--;
					You_feel("prettier!");
					return;
				}
				break;
		}
		attempts--;
		whichstat++;
		if (whichstat > 6) whichstat = 1;
	}

}

/* Will you get an attribute increase for the target attribute? --Amy */
boolean
attr_will_go_up(targetattr, displaymessage)
int targetattr; /* the attribute that wants to increase */
boolean displaymessage;
{
	int theminimum, themaximum, rolemaximum, racemaximum, actuallimit, yourbasestat, finalchance;

	/* mongung race trains mental attributes very slowly --Amy */
	if (Race_if(PM_MONGUNG) && rn2(3) && (targetattr == A_INT || targetattr == A_WIS) ) {
		goto attrdevelop;
	}

	/* dungeon lords monster races look butt ugly to everyone else, so they train charisma slowly --Amy */
	if ((Race_if(PM_URGOTH) || Race_if(PM_ZAUR) || Race_if(PM_THRALL) || Race_if(PM_WYLVAN) ) && rn2(2) && (targetattr == A_CHA)) {
		goto attrdevelop;

	}

	/* The actual limit is calculated based on both the role and race limit. */
	rolemaximum = urole.attrlimt[targetattr];
	racemaximum = urace.attrtrs[targetattr];

	/* Are they both the same? Great! In that case the calculation is easy. */
	if (rolemaximum == racemaximum) theminimum = themaximum = rolemaximum;

	/* Otherwise, set the bounds */
	else if (rolemaximum > racemaximum) {
		themaximum = rolemaximum;
		theminimum = racemaximum;
	} else if (rolemaximum < racemaximum) {
		themaximum = racemaximum;
		theminimum = rolemaximum;
	}

	/* fail safe */
	if (theminimum > themaximum) {
		impossible("Minimum is greater than maximum (%d, %d)", theminimum, themaximum);
		themaximum = theminimum;
	}

	/* now we calculate the actual limit, which may be different every time we're called */
	while (themaximum > theminimum) {

		/* strength has to be special-cased because of the 18/** stuff *groan* --Amy */
		if (targetattr == A_STR) {

			/* in 4 out of 5 cases, the maximum is reduced */
			if (rn2(5)) {
				if (themaximum < STR18(1) || themaximum > STR18(100)) themaximum--;
				else themaximum -= 10;
			/* otherwise the minimum is increased */
			} else {
				if (theminimum < 18 || theminimum > STR18(99)) theminimum++;
				else theminimum += 10;
			}
		} else {
			if (rn2(5)) themaximum--;
			else theminimum++;
		}

	}

	/* the values must be equal now; if not, throw an error message and return */
	if (theminimum != themaximum) {
		impossible("calculation failed (%d, %d)", theminimum, themaximum);
		return TRUE;
	}

	/* male dark seducers and golden saints suck */
	if ((Race_if(PM_MAZKE) || Race_if(PM_AUREAL)) && !flags.female) {
		if (targetattr == A_STR && themaximum > 18 && themaximum < STR19(19)) themaximum -= 10;
		else themaximum--;
	}

	/* mesher generally has lower soft cap --Amy */
	if (Race_if(PM_MESHER)) {
		if (targetattr == A_STR && themaximum > 18 && themaximum < STR19(19)) themaximum -= 10;
		else themaximum--;
	}

	/* now we know our actual limit */
	actuallimit = themaximum;

	/* some adjustments based on gender and alignment */
	if (flags.female && (targetattr == A_INT || targetattr == A_WIS || targetattr == A_CHA) ) actuallimit++;
	else if (targetattr == A_STR || targetattr == A_CON || targetattr == A_DEX) {
		if (targetattr == A_STR && actuallimit >= 18 && actuallimit < STR18(100)) actuallimit += 10;
		else actuallimit++;
	}
	/* This is not sexist, after all I didn't pull a D&D "women can't have more than 16 strength" or something. --Amy
	 * Every gender gets the same # of "improved" stats and it's still possible to go beyond the maximum, it's just a
	 * little more difficult and it's important for gameplay as you shouldn't be able to max out everything easily */

	if (u.ualign.type == A_CHAOTIC && (targetattr == A_CON || targetattr == A_DEX)) actuallimit++;
	else if (u.ualign.type == A_NEUTRAL && (targetattr == A_CHA || targetattr == A_WIS)) actuallimit++;
	else if (u.ualign.type == A_LAWFUL && (targetattr == A_STR || targetattr == A_INT)) {
		if (targetattr == A_STR && actuallimit >= 18 && actuallimit < STR18(100)) actuallimit += 10;
		else actuallimit++;
	}

	if (RngeStatcapIncrease) actuallimit += 5;
	if (ublindf && ublindf->oartifact == ART_MILARY_S_DAILY_SIGH) actuallimit += 5;
	if (uarmf && uarmf->oartifact == ART_EROTIC_STAT_TRAIN) actuallimit += 5;
	if (uarmh && uarmh->oartifact == ART_GREAT_LEARNING_AMATEUR) actuallimit += 5;
	if (powerfulimplants() && uimplant && uimplant->oartifact == ART_GLEN_HOSPITAL && targetattr == A_WIS) actuallimit += 10000; /* guaranteed success */
	if (uimplant && uimplant->oartifact == ART_FOOD_FOR_THOUGHT && (targetattr == A_WIS || targetattr == A_INT)) {
		actuallimit += (powerfulimplants() ? 5 : 2);
	}
	if (uarm && uarm->oartifact == ART_CERNY_ && targetattr == A_CON) actuallimit += 5;
	if (uarms && uarms->oartifact == ART_HIGHER_TRAINING) actuallimit += 2;

	if (targetattr == A_INT && uwep && uwep->oartifact == ART_DAEMEL) actuallimit -= 5;
	if (targetattr == A_INT && u.twoweap && uswapwep && uswapwep->oartifact == ART_DAEMEL) actuallimit -= 5;

	if (StatDecreaseBug || u.uprops[STAT_DECREASE_BUG].extrinsic || have_statdecreasestone() || autismringcheck(ART_JUBILEX_S_CODE)) {
		if (targetattr == A_STR && actuallimit >= STR19(25)) actuallimit = STR18(70);
		else if (targetattr == A_STR && actuallimit >= STR19(24)) actuallimit = STR18(60);
		else if (targetattr == A_STR && actuallimit >= STR19(23)) actuallimit = STR18(50);
		else if (targetattr == A_STR && actuallimit >= STR19(22)) actuallimit = STR18(40);
		else if (targetattr == A_STR && actuallimit >= STR19(21)) actuallimit = STR18(30);
		else if (targetattr == A_STR && actuallimit >= STR19(20)) actuallimit = STR18(20);
		else if (targetattr == A_STR && actuallimit >= STR19(19)) actuallimit = STR18(10);
		else if (targetattr == A_STR && actuallimit >= STR18(100)) actuallimit = 18;
		else if (targetattr == A_STR && actuallimit >= STR18(90)) actuallimit = 17;
		else if (targetattr == A_STR && actuallimit >= STR18(80)) actuallimit = 16;
		else if (targetattr == A_STR && actuallimit >= STR18(70)) actuallimit = 15;
		else if (targetattr == A_STR && actuallimit >= STR18(60)) actuallimit = 14;
		else if (targetattr == A_STR && actuallimit >= STR18(50)) actuallimit = 13;
		else if (targetattr == A_STR && actuallimit >= STR18(40)) actuallimit = 12;
		else if (targetattr == A_STR && actuallimit >= STR18(30)) actuallimit = 11;
		else if (targetattr == A_STR && actuallimit >= STR18(20)) actuallimit = 10;
		else if (targetattr == A_STR && actuallimit >= STR18(10)) actuallimit = 9;
		else if (targetattr == A_STR && actuallimit >= 18) actuallimit = 8;
		else {
			actuallimit -= 10;
			if (actuallimit < 1) actuallimit = 1;
		}
	}

	/* set up the value to compare it to */
	yourbasestat = ABASE(targetattr);
	/* you're trying to increase the stat, so we add one */
	if (targetattr == A_STR && yourbasestat >= 18 && yourbasestat < STR18(100)) {
		yourbasestat += 10;
		if (yourbasestat > STR19(19)) yourbasestat = STR19(19);
	}
	else yourbasestat++;

	/* calculate the finalchance value, which is the 1 in X chance that you get the increase */
	finalchance = 1;

	while (yourbasestat > actuallimit) {
		if (targetattr == A_STR) {
			if (actuallimit < 18 || actuallimit > STR18(99)) {
				finalchance++;
				actuallimit++;
			} else {
				finalchance++;
				actuallimit += 10;
			}
		} else {
			finalchance++;
			actuallimit++;
		}
	}

	/* now that we finally have the actual chance, let's see whether you get lucky! */

	if (finalchance < 1) {
		impossible("finalchance is not positive (%d)", finalchance);
		finalchance = 1;
	}

	if (finalchance == 1) return TRUE;
	else if (!rn2(finalchance)) return TRUE;
	else {
attrdevelop:
		if (displaymessage) {
			switch (targetattr) {

				case A_STR:
					pline("Your strength develops.");
					break;
				case A_DEX:
					pline("Your dexterity develops.");
					break;
				case A_CHA:
					pline("Your charisma develops.");
					break;
				case A_WIS:
					pline("Your wisdom develops.");
					break;
				case A_INT:
					pline("Your intelligence develops.");
					break;
				case A_CON:
					pline("Your constitution develops.");
					break;
				default:
					impossible("weird attribute for increase check (%d)", targetattr);
					break;

			}
		}

		return FALSE;
	}

}

/* recalculate player's stats --Amy
 * This is not the same as feeling like a new man; it redistributes ALL stat points.
 * percentage strength points will be converted into single points so you're likely to experience a net loss :P */
void
bodymorph()
{
	int amounttodistribute = 0;
	register int	i, x, tryct;

	for(i = 0; i < A_MAX; i++) {
		if (i == A_STR) {
			while (ABASE(i) > 3) {
				if (ABASE(i) <= 18 || ABASE(i) >= STR19(19)) {
					ABASE(i)--;
					amounttodistribute++;
				} else {
					ABASE(i) -= 10;
					if (ABASE(i) < 18) ABASE(i) = 18;
					amounttodistribute++;
				}
			}
		} else {
			while (ABASE(i) > 3) {
				ABASE(i)--;
				amounttodistribute++;
			}
		}
		ABASE(i) = 3;
		AMAX(i) = 3;
	}
	amounttodistribute += 18; /* because of the base 3 points you always have in all attribs */
	init_attr(amounttodistribute, TRUE);
}

/*attrib.c*/
