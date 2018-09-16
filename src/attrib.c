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
}	arc_abil[] = { {	 1, &(HStealth), "", "" },
		     {   1, &(HFast), "", "" },
		     {  10, &(HSearching), "perceptive", "unaware" },
		     {	 0, 0, 0, 0 } },

	ana_abil[] = { {	 1, &(HFull_nutrient), "", "" },
		     {   7, &(HFast), "quick", "slow" },  
		     {	15, &(HWarning), "precognitive", "noncognitive" },  
		     {	15, &(HCont_resist), "protected from contamination", "vulnerable to contamination" },  
		     {	 0, 0, 0, 0 } },  

	pic_abil[] = { {	 1, &(HStealth), "", "" },
		     {   6, &(HFast), "quick", "slow" },
		     {  8, &(HSearching), "perceptive", "unaware" },
		     {	 0, 0, 0, 0 } },

	bar_abil[] = { {	 1, &(HPoison_resistance), "", "" },
		     {   7, &(HFast), "quick", "slow" },
		     {  15, &(HStealth), "stealthy", "noisy" },
		     {	 0, 0, 0, 0 } },

	ass_abil[] = { {	 1, &(HStealth), "", "" },
		     {   10, &(HFast), "quick", "slow" },
		     {	15, &(HWarning), "sensitive", "careless" },
		     {	 0, 0, 0, 0 } },

	ama_abil[] = { {	 1, &(HStealth), "", "" },
		     {   7, &(HFast), "quick", "slow" },
		     {  10, &(HSwimming), "ready to swim","afraid of the water" },
		     {	 0, 0, 0, 0 } },

	sup_abil[] = { {	 1, &(HTeleportation), "", "" },
			{	3, &(HSleep_resistance), "awake", "tired" },
		     {	8, &(HWarning), "sensitive", "careless" },
		     {   22, &(HTeleport_control), "controlled", "helpless" },
		     {	 0, 0, 0, 0 } },

	cou_abil[] = { {	 1, &(HHunger), "", "" },
		     {   1, &(HAggravate_monster), "", "" },
		     {  1, &(HConflict), "", "" },
		     {	 0, 0, 0, 0 } },

	brd_abil[] = { {	5, &(HSleep_resistance), "awake", "tired" },
		     {	 10, &(HStealth), "stealthy", "noisy" },
		     {	 0, 0, 0, 0 } },

	gam_abil[] = { {	3, &(HSleep_resistance), "awake", "tired" },
		     {	 13, &(HSlow_digestion), "a certain satiation", "the need to eat more" },
			{   23, &(HManaleech), "magically attuned", "no longer magically attuned" },
		     {	 0, 0, 0, 0 } },

	cav_abil[] = { {	 7, &(HFast), "quick", "slow" },
		     {	15, &(HWarning), "sensitive", "careless" },
		     {	 0, 0, 0, 0 } },

	con_abil[] = { {   1, &(HSick_resistance), "", "" },
	         {	 7, &(HPoison_resistance), "healthy", "less healthy" },
		     {  20, &(HSearching), "perceptive", "unaware" },
		     {	 0, 0, 0, 0 } },
	mur_abil[] = { {   1, &(HAggravate_monster), "", "" },
	           {	 2, &(HSearching), "perceptive", "unaware" },
	           {	 3, &(HSearching), "perceptive", "unaware" },
	           {	 4, &(HSearching), "perceptive", "unaware" },
	           {	 5, &(HSearching), "perceptive", "unaware" },
	           {	 6, &(HSearching), "perceptive", "unaware" },
	           {	 7, &(HSearching), "perceptive", "unaware" },
	           {	 8, &(HSearching), "perceptive", "unaware" },
	           {	 9, &(HSearching), "perceptive", "unaware" },
	           {	 10, &(HSearching), "perceptive", "unaware" },
	           {	 11, &(HSearching), "perceptive", "unaware" },
	           {	 12, &(HSearching), "perceptive", "unaware" },
	           {	 13, &(HSearching), "perceptive", "unaware" },
	           {	 14, &(HSearching), "perceptive", "unaware" },
	           {	 15, &(HSearching), "perceptive", "unaware" },
	           {	 16, &(HSearching), "perceptive", "unaware" },
	           {	 17, &(HSearching), "perceptive", "unaware" },
	           {	 18, &(HSearching), "perceptive", "unaware" },
	           {	 19, &(HSearching), "perceptive", "unaware" },
	           {	 20, &(HSearching), "perceptive", "unaware" },
	           {	 20, &(HSearching), "perceptive", "unaware" },
	           {	 21, &(HSearching), "perceptive", "unaware" },
	           {	 22, &(HSearching), "perceptive", "unaware" },
	           {	 23, &(HSearching), "perceptive", "unaware" },
	           {	 24, &(HSearching), "perceptive", "unaware" },
	           {	 25, &(HSearching), "perceptive", "unaware" },
	           {	 26, &(HSearching), "perceptive", "unaware" },
	           {	 27, &(HSearching), "perceptive", "unaware" },
	           {	 28, &(HSearching), "perceptive", "unaware" },
	           {	 29, &(HSearching), "perceptive", "unaware" },
	           {	 30, &(HSearching), "perceptive", "unaware" },
		     {	 0, 0, 0, 0 } },

	bul_abil[] = { {   1, &(HAggravate_monster), "", "" },
	         {	 7, &(HPoison_resistance), "healthy", "less healthy" },
		     {	 0, 0, 0, 0 } },

	fai_abil[] = { {   1, &(HAggravate_monster), "", "" },
			{	1, &(BadEffectProblem), "", "" },
			{	1, &(AutomaticVulnerabilitiy), "", "" },
		     {	 0, 0, 0, 0 } },

	sta_abil[] = { {   1, &(HTechnicality), "", "" },
		     {	 0, 0, 0, 0 } },

	trs_abil[] = { {   5, &(LevelTrapEffect), "", "" },
			{	10, &(CaptchaProblem), "", "" },
			{	15, &(ChaosTerrain), "", "" },
			{	20, &(PlayerCannotTrainSkills), "", "" },
			{	25, &(FoodIsAlwaysRotten), "", "" },
			{	30, &(InventoryLoss), "", "" },
		     {	 0, 0, 0, 0 } },

	kur_abil[] = { {   4, &(NotSeenBug), "", "" },
			{	8, &(GrayoutBug), "", "" },
			{	12, &(DifficultyIncreased), "", "" },
			{	16, &(UnfairAttackBug), "", "" },
			{	20, &(EnmityBug), "", "" },
			{	25, &(ElongationBug), "", "" },
			{	30, &(Antileveling), "", "" },
		     {	 0, 0, 0, 0 } },

	mah_abil[] = { {   1, &(HAggravate_monster), "", "" },
			{	1, &(HAntimagic), "", "" },
			{   7, &(HSee_invisible), "your vision sharpen", "your vision blurring" },
			{  15, &(HTeleport_control), "controlled","uncontrolled" },
		       {   15, &(HPolymorph_control), "your choices improve", "choiceless" },
		     {	 0, 0, 0, 0 } },

	sla_abil[] = { {   1, &(HAggravate_monster), "", "" },
		     {  1, &(HCold_resistance), "", "" },
			{	5, &(HSleep_resistance), "awake", "tired" },
		     {   15, &(HTeleport_control), "controlled", "helpless" },
			{   25, &(HManaleech), "magically attuned", "no longer magically attuned" },
		     {	 0, 0, 0, 0 } },

	fla_abil[] = { {   1, &(HFire_resistance), "", "" },
		     {  13, &(HCold_resistance), "warm", "cooler" },
		     {   0, 0, 0, 0 } },

	sli_abil[] = { {   1, &(HAcid_resistance), "", "" },
			{	1, &(MenuBug), "", "" },
			{	5, &(AutoDestruct), "", "" },
			{   6, &(HSee_invisible), "your vision sharpen", "your vision blurring" },
			{	7, &(DSTWProblem), "", "" },
			{     9, &(HManaleech), "magically attuned", "no longer magically attuned" },
			{	10, &(HCorrosivity), "corrosive", "less acidic" },
			{	12, &(FreeHandLoss), "", "" },
			{	15, &(HSleep_resistance), "awake", "tired" },
			{	17, &(DifficultyIncreased), "", "" },
		       {   18, &(HPolymorph_control), "your choices improve", "choiceless" },
			{	20, &(HSick_resistance), "immune to diseases", "no longer immune to diseases" },
			{	22, &(Deafness), "", "" },
			{	25, &(HDisint_resistance), "stable", "brittle" },
			{	26, &(HDiscount_action), "resistant to paralysis", "less resistant to paralysis" },
			{	28, &(ShadesOfGrey), "", "" },
			{	30, &(HStone_resistance), "rock solid", "breakable" },
		     {   0, 0, 0, 0 } },

	erd_abil[] = { {   1, &(HFree_action), "", "" },
		     {   0, 0, 0, 0 } },

	blo_abil[] = { {   1, &(HWarning), "", "" },
			{  8, &(HSearching), "perceptive", "unaware" },
		     {	 12, &(HStealth), "stealthy", "noisy" },
		     {  15, &(HFire_resistance), "cool", "warmer" },
		     {   0, 0, 0, 0 } },

	scr_abil[] = { {   1, &(HFire_resistance), "", "" },
			{	5, &(HSleep_resistance), "awake", "tired" },
		     {   14, &(HTeleport_control), "controlled", "helpless" },
		     {  16, &(HCold_resistance), "warm", "cooler" },
		     {   0, 0, 0, 0 } },

	coo_abil[] = { {   1, &(HFire_resistance), "", "" },
		     {  1, &(HFull_nutrient), "", "" },
		     {   0, 0, 0, 0 } },

	fnc_abil[] = { {	 7, &(HFast), "quick", "slow" },
		     {   0, 0, 0, 0 } },

	fig_abil[] = { {	 7, &(HFast), "quick", "slow" },
		     {   0, 0, 0, 0 } },

	med_abil[] = { {	1, &(HStealth), "", "" },
		       {	7, &(HSearching), "perceptive", "unaware" },
		     {  10, &(HFull_nutrient), "your metabolism slowing down", "your metabolism speeding up" },
			{   14, &(HSee_invisible), "your vision sharpen", "your vision blurring" },
			{   20, &(HManaleech), "magically attuned", "no longer magically attuned" },
		     {   0, 0, 0, 0 } },

	lad_abil[] = { {   1, &(HSleep_resistance), "", "" },
			{  4, &(HSearching), "perceptive", "unaware" },
			{   7, &(HSee_invisible), "your vision sharpen", "your vision blurring" },
		     {   0, 0, 0, 0 } },

	jes_abil[] = { {	 5, &(HStealth), "stealthy", "noisy" },
		     {   7, &(HFast), "quick", "slow" },
		     {	10, &(HWarning), "sensitive", "careless" },
			{  12, &(HSearching), "perceptive", "unaware" },
		     {   0, 0, 0, 0 } },

	art_abil[] = { {   7, &(HSee_invisible), "your vision sharpen", "your vision blurring" },
		       {   20, &(HPolymorph_control), "your choices improve", "choiceless" },
		     {   0, 0, 0, 0 } },

	gun_abil[] = { {   1, &(HPoison_resistance), "", "" },
		     {  1, &(HStealth), "", "" },
		     {  5, &(HFire_resistance), "cool", "warmer" },
		       {   7, &(HSee_invisible), "your vision sharpen", "your vision blurring" },
		     {  10, &(HCold_resistance), "warm", "cooler" },
		     {   0, 0, 0, 0 } },

	mar_abil[] = { {   1, &(HFast), "", "" },
		     {   0, 0, 0, 0 } },

	stu_abil[] = { {  2, &(HSleep_resistance), "awake", "tired" },
		     {   5, &(HFast), "quick", "slow" },
		     {   14, &(HFree_action), "freed", "a loss of freedom" },
		     {   0, 0, 0, 0 } },

	dol_abil[] = { {   1, &(HPoison_resistance), "", "" },
		     {	3, &(HWarning), "sensitive", "careless" },
			{   9, &(HManaleech), "magically attuned", "no longer magically attuned" },
		     {   0, 0, 0, 0 } },

	sag_abil[] = { {   1, &(HFull_nutrient), "", "" },
		     {   5, &(HManaleech), "magically attuned", "no longer magically attuned" },
		     {  10, &(HSearching), "perceptive", "unaware" },
		     {	17, &(HWarning), "sensitive", "careless" },
		     {	20, &(HDiscount_action), "resistant to paralysis", "less resistant to paralysis" },
		     {	 0, 0, 0, 0 } },

	ota_abil[] = { {	8, &(HWarning), "sensitive", "careless" },
			{  16, &(HSearching), "perceptive", "unaware" },
		     {	 0, 0, 0, 0 } },

	aug_abil[] = { {   1, &(HTelepat), "", "" },
		     {   1, &(HStealth), "", "" },
		     {   1, &(HTechnicality), "", "" },
			{   3, &(HManaleech), "magically attuned", "no longer magically attuned" },
		     {	12, &(HWarning), "sensitive", "careless" },
		     {   0, 0, 0, 0 } },

	fir_abil[] = { {   1, &(HFire_resistance), "", "" },
		     {  10, &(HSearching), "perceptive", "unaware" },
		     {   0, 0, 0, 0 } },

	nin_abil[] = { {   1, &(HStealth), "", ""  },
		     {   3, &(HInvis), "hidden", "more visible" },
		     {   0, 0, 0, 0 } },

	off_abil[] = { {   1, &(HSearching), "", "" },
		     {  10, &(HWarning), "sensitive", "careless" },
		     {   0, 0, 0, 0 } },

	aci_abil[] = { {   1, &(HAcid_resistance), "", "" },
		     {   0, 0, 0, 0 } },

	loc_abil[] = { {   1, &(HSearching), "", "" },
		     {   0, 0, 0, 0 } },
	sha_abil[] = {/* {   1, &(HPolymorph), "", "" },*/
		       {   9, &(HPolymorph_control), "your choices improve", "choiceless" },
		       {   0, 0, 0, 0 } },

	unt_abil[] = { {   1, &(HPoison_resistance), "", "" },
		     {   7, &(HCold_resistance), "warm", "cooler" },
		     {   0, 0, 0, 0 } },

	gee_abil[] = { {   1, &(HPoison_resistance), "", "" },
		     {   1, &(HShock_resistance), "", "" },
		     {   8, &(HTeleport_control), "controlled", "helpless" },
		     {  15, &(HWarning), "sensitive", "careless" },
		     {   0, 0, 0, 0, } },

	gra_abil[] = { {   1, &(HAggravate_monster), "", "" },
		     {   1, &(HFast), "", "" },
		     {   1, &(HSleep_resistance), "", "" },
		     {   0, 0, 0, 0 } },

	hea_abil[] = { {	 1, &(HPoison_resistance), "", "" },
		     {	15, &(HWarning), "sensitive", "careless" },
			{   20, &(HManaleech), "magically attuned", "no longer magically attuned" },
		     {	 0, 0, 0, 0 } },

	dru_abil[] = { {	 1, &(HPoison_resistance), "", "" },
			{   24, &(HManaleech), "magically attuned", "no longer magically attuned" },
		     {	 0, 0, 0, 0 } },

	sci_abil[] = { {	 1, &(HAcid_resistance), "", "" },
		     {   1, &(HTechnicality), "", "" },
		     {	25, &(HCont_resist), "protected from contamination", "vulnerable to contamination" },  
		     {	 0, 0, 0, 0 } },

	mus_abil[] = { {	 7, &(HStealth), "stealthy", "noisy" },
		     {	 0, 0, 0, 0 } },

	ice_abil[] = { {   1, &(HCold_resistance), "", "" },
		     {  13, &(HFire_resistance), "cool", "warmer" },
		     {   0, 0, 0, 0 } },

	ele_abil[] = { {   1, &(HShock_resistance), "", "" },
		     {   0, 0, 0, 0 } },

	psn_abil[] = { {   1, &(HPoison_resistance), "", "" },
		     {   1, &(HFull_nutrient), "", "" },
			{   5, &(HManaleech), "magically attuned", "no longer magically attuned" },
		     {   0, 0, 0, 0 } },

	occ_abil[] = { {   10, &(HInfravision), "perceptive", "half blind"},
		     {   0, 0, 0, 0 } },

	emn_abil[] = { {   15, &(HPsi_resist), "psionic", "less psionic"},
		     {   0, 0, 0, 0 } },

	cha_abil[] = { {   20, &(HEnergy_regeneration), "charged with mana", "a loss of mana"},
		     {   0, 0, 0, 0 } },

	zoo_abil[] = { {   1, &(HWarning), "", "" },
		     {  10, &(HPoison_resistance), "hardy", "less healthy" },
		     {   0, 0, 0, 0 } },

	jed_abil[] = { {   1, &(HStealth), "", ""},
		       {   3, &(HTelepat), "disturbances in the force", "your grip on the force lessen" },
		       {   5, &(HSee_invisible), "your vision sharpen", "your vision blurring" },
		       {   7, &(HFast), "quick", "slow" },
		       { 0, 0, 0, 0 } },
			
	kni_abil[] = { {	 7, &(HFast), "quick", "slow" },
		     {	 0, 0, 0, 0 } },

	nuc_abil[] = { {	 1, &(HCont_resist), "", "" },
		     {   1, &(HTechnicality), "", "" },
		     {	 0, 0, 0, 0 } },

	for_abil[] = { {	 1, &(HPolymorph_control), "", "" },
		       {   5, &(HPolymorph), "polymorphic", "form-stable" },
		     {	 0, 0, 0, 0 } },

	war_abil[] = { {	 7, &(HFast), "quick", "slow" },
		     {	 0, 0, 0, 0 } },

	mon_abil[] = { {   1, &(HFast), "", "" },
		     {   1, &(HSleep_resistance), "", "" },
		     {   1, &(HSee_invisible), "", "" },
		     {   3, &(HPoison_resistance), "healthy", "less healthy" },
		     {   5, &(HStealth), "stealthy", "noisy" },
		     {   7, &(HWarning), "sensitive", "careless" },
		     {   9, &(HSearching), "perceptive", "unaware" },
#if 0
		     {  11, &(HFire_resistance), "cool", "warmer" },
		     {  13, &(HCold_resistance), "warm", "cooler" },
		     {  15, &(HShock_resistance), "insulated", "conductive" },
		     /* WAC -- made the above three attribs techs */
#endif
		     {  17, &(HTeleport_control), "controlled","uncontrolled" },
		     {   0, 0, 0, 0 } },

	psi_abil[] = { {   1, &(HTelepat), "", "" },
		     {   1, &(HClairvoyant), "","" },
		     {   3, &(HSee_invisible), "your vision sharpen", "your vision blur" },
		     {   5, &(ETelepat), "aware", "unaware" },
		     {   7, &(HAggravate_monster), "noisy", "quiet" },
		     {  10, &(HDetect_monsters), "connected","detached" },
		     {  15, &(HFlying), "weightless", "grounded" },
		     {  20, &(HPasses_walls), "ethereal","mundane" },
			{   25, &(HManaleech), "magically attuned", "no longer magically attuned" },
		     {   0, 0, 0, 0 } },

	fox_abil[] = { {   1, &(ETelepat), "", "" },
		     {   1, &(HClairvoyant), "","" },
		     {   1, &(HStealth), "","" },
		     {   5, &(HSearching), "perceptive", "unaware" },
		     {   10, &(HFast), "quick", "slow" },
		     {   15, &(HWarning), "sensitive", "careless" },
		     {   0, 0, 0, 0 } },

	nec_abil[] = { {   1, &(HDrain_resistance), "", "" },
		     {   1, &(HSick_resistance), "", "" },
		     {   3, &(HUndead_warning), "sensitive", "careless" },
			{   15, &(HManaleech), "magically attuned", "no longer magically attuned" },
		     {   0, 0, 0, 0 } },

	pok_abil[] = { {   1, &(HDisint_resistance), "", "" },
		     {   4, &(HWarning), "sensitive", "careless" },
		     {   7, &(HSearching), "perceptive", "unaware" },
		     {   10, &(HDrain_resistance), "more resistant to drain life", "less resistant to drain life" },
		     {   13, &(HFast), "quick", "slow" },
		     {   16, &(HSleep_resistance), "awake", "tired" },
		     {   19, &(HAcid_resistance), "warded", "endangered" },
		     {   22, &(HTelepat), "telepathic", "nontelepathic" },
		     {   24, &(HTechnicality), "evolved", "a loss of PP" },
		     {   25, &(HSick_resistance), "resistant", "non-resistant" },
		     {   26, &(HCont_resist), "protected from contamination", "vulnerable to contamination" },  
		     {   28, &(HTeleport_control), "controlled", "uncontrolled" },
		     {   30, &(HFlying), "airborne", "stalled" },
		     {   0, 0, 0, 0 } },

	pir_abil[] = {	{1, &(HSwimming), "", ""  },
			 {	7, &(HStealth), "stealthy", "noisy"  },	/* with cat-like tread ... */
		     {  11, &(HFast), "quick", "slow" },
		     {	 0, 0, 0, 0 } },

	gof_abil[] = {	{1, &(HDrain_resistance), "", ""  },
		     {   1, &(HInfravision), "", "" },
		     {  10, &(HReflecting), "reflexive", "nonreflexive" },
		     {	 0, 0, 0, 0 } },
	gol_abil[] = { {   5, &(HStealth), "stealthy", "noisy" },
		     {   9, &(HFast), "quick", "slow" },
		     {   11, &(HSearching), "perceptive", "unaware" },
		     {   0, 0, 0, 0 } },

	gla_abil[] = {	{3, &(HFast), "quick", "slow"  },
			 {	6, &(HStealth), "stealthy", "noisy"  },	/* with cat-like tread ... */
		     {  10, &(HAggravate_monster), "noisy", "quiet" },
		     {  15, &(HWarning), "sensitive", "careless" },
		     {	 0, 0, 0, 0 } },
	kor_abil[] = {	{2, &(HSearching), "perceptive", "unaware"  },
			 {	7, &(HStealth), "stealthy", "noisy"  },	/* with cat-like tread ... */
		     {	 0, 0, 0, 0 } },
	div_abil[] = {	{15, &(HMagical_breathing), "aquatic", "hydrophobic"  },
		     {	 0, 0, 0, 0 } },


	lun_abil[] = { {   1, &(HPoison_resistance), "", "" },
		     {   1, &(HRegeneration), "", "" },
		     {   7, &(HStealth), "stealthy", "noisy" },
		     {   0, 0, 0, 0 } },

			 
	pri_abil[] = { {	15, &(HWarning), "sensitive", "careless" },
		     {  20, &(HFire_resistance), "cool", "warmer" },
		     {	 0, 0, 0, 0 } },

	ran_abil[] = { {   1, &(HSearching), "", "" },
		     {	 7, &(HStealth), "stealthy", "noisy" },
		     {	15, &(HSee_invisible), "your vision sharpen", "your vision blur" },
		     {	 0, 0, 0, 0 } },

	elp_abil[] = { {   1, &(HSearching), "", "" },
		     {	4, &(HSleep_resistance), "awake", "tired" },
		     {	 7, &(HStealth), "stealthy", "noisy" },
		     {	15, &(HSee_invisible), "your vision sharpen", "your vision blur" },
		     {	 0, 0, 0, 0 } },
	rog_abil[] = { {	 1, &(HStealth), "", ""  },
		     {  10, &(HSearching), "perceptive", "unaware" },
		     {	 0, 0, 0, 0 } },

	sam_abil[] = { {	 1, &(HFast), "", "" },
		     {  15, &(HStealth), "stealthy", "noisy" },
		     {	 0, 0, 0, 0 } },

	tou_abil[] = { {	10, &(HSearching), "perceptive", "unaware" },
		     {	20, &(HPoison_resistance), "hardy", "less healthy" },
		     {	 0, 0, 0, 0 } },
	rin_abil[] = { {  1, &(HStealth), "", "" },
		     {   7, &(HFast), "quick", "slow" },
		     {   0, 0, 0, 0 } },

	und_abil[] = { {   1, &(HStealth), "", "" },
		     {   1, &(HDrain_resistance), "", "" },
		     {   1, &(HSick_resistance), "", "" },
		     {   1, &(HUndead_warning), "", "" },
		     {   7, &(HFast), "quick", "slow" },
		     {   9, &(HPoison_resistance), "hardy", "less healthy" },
		     {   0, 0, 0, 0 } },

	pal_abil[] = { {   1, &(HFast), "", "" },
		     {   1, &(HUndead_warning), "", "" },
		     {   5, &(HPoison_resistance), "hardy", "less healthy" },
		     {	10, &(HSleep_resistance), "awake", "tired" },
			{   24, &(HManaleech), "magically attuned", "no longer magically attuned" },
		     {   0, 0, 0, 0 } },

	act_abil[] = { {   1, &(HPeacevision), "", "" },
		     {   0, 0, 0, 0 } },

	jus_abil[] = { {   1, &(HPeacevision), "", "" },
		     {   0, 0, 0, 0 } },

	val_abil[] = { {	 1, &(HCold_resistance), "", "" },
		     {	 1, &(HStealth), "", "" },
		     {   7, &(HFast), "quick", "slow" },
		     {	 0, 0, 0, 0 } },

	wan_abil[] = { {	 1, &(HCold_resistance), "", "" },
		     {   1, &(HFire_resistance), "", "" },
		     {   1, &(HShock_resistance), "", "" },
		     {   1, &(HSleep_resistance), "", "" },
			{	10, &(HWarning), "sensitive", "careless" },
			{   10, &(HSee_invisible), "your vision sharpen", "your vision blurring" },
			{   15, &(HManaleech), "magically attuned", "no longer magically attuned" },
			{  20, &(HTeleport_control), "controlled","uncontrolled" },
		       {   20, &(HPolymorph_control), "your choices improve", "choiceless" },
		     {	 0, 0, 0, 0 } },
	mid_abil[] = { { 1, &(HSearching), "", "" },
			{  10, &(HFire_resistance), "cool", "warmer" },
			{   0, 0, 0, 0 } },

	yeo_abil[] = {
		     {   7, &(HFast), "quick", "slow" },
		     {  15, &(HSwimming), "ready to swim","afraid of the water" },
		     {   0, 0, 0, 0 } },

	wiz_abil[] = { {   1, &(HManaleech), "", "" },
			 {	15, &(HWarning), "sensitive", "careless" },
		     {  17, &(HTeleport_control), "controlled","uncontrolled" },
		     {	 0, 0, 0, 0 } },

	alt_abil[] = { {   3, &(HUndead_warning), "sensitive", "careless" },
			{	7, &(HWarning), "sensitive", "careless" },
			{  15, &(HTeleport_control), "controlled","uncontrolled" },
		       {   25, &(HPolymorph_control), "your choices improve", "choiceless" },
		     {  30, &(HReflecting), "reflexive", "nonreflexive" },
		     {	 0, 0, 0, 0 } },

	dun_abil[] = { {	1, &(HFire_resistance), "", "" },
		     {	 0, 0, 0, 0 } },

	bos_abil[] = { {	1, &(HPoison_resistance), "", "" },
		     {  7, &(HStealth), "stealthy", "noisy" },
		     {	 0, 0, 0, 0 } },

	tha_abil[] = { {	4, &(HSleep_resistance), "awake", "tired" },
		     {   10, &(HAggravate_monster), "intolerable", "more acceptable" },
		     {	 0, 0, 0, 0 } },

	ord_abil[] = { {   5, &(HSee_invisible), "your vision sharpen", "your vision blurring" },
		     {   10, &(HUndead_warning), "sensitive", "careless" },
		       { 0, 0, 0, 0 } },

	/* Intrinsics conferred by race */
	dop_abil[] = {/* {   1, &(HPolymorph), "", "" },*/
		       {   9, &(HPolymorph_control), "your choices improve", "choiceless" },
		       {   0, 0, 0, 0 } },

	dwa_abil[] = { { 1, &(HSearching), "", "" },
			{  10, &(HFire_resistance), "cool", "warmer" },
			{   0, 0, 0, 0 } },

	slm_abil[] = { { 1, &(HCold_resistance), "", "" },
		     {	 1, &(HStone_resistance), "", "" },
		     {	 1, &(HShock_resistance), "", "" },
		     {	 1, &(HPoison_resistance), "", "" },
		     {	 1, &(HAcid_resistance), "", "" },
			{   0, 0, 0, 0 } },

	elf_abil[] = { {	4, &(HSleep_resistance), "awake", "tired" },
		     {	 0, 0, 0, 0 } },

	inc_abil[] = { {	1, &(HAntimagic), "", "" },
			{   1, &(HManaleech), "", "" },
		     {	 0, 0, 0, 0 } },

	gre_abil[] = { {	1, &(HFire_resistance), "", "" },
			{   15, &(HFast), "quick", "slow" },
		     {	 0, 0, 0, 0 } },

	clk_abil[] = { {	1, &(HPoison_resistance), "", "" },
		     {	 1, &(HSick_resistance), "", "" },
		     {	 1, &(HStone_resistance), "", "" },
		     {	 5, &(HShock_resistance), "shock resistant", "less shock resistant" },
		     {	 10, &(HCold_resistance), "cold resistant", "less cold resistant" },
		     {  10, &(HFull_nutrient), "your metabolism slowing down", "your metabolism speeding up" },
		     {	 15, &(HFire_resistance), "heat resistant", "less heat resistant" },
		     {   20, &(HTechnicality), "technically knowledgable", "your techniques becoming weaker" },
			{	25, &(HDiscount_action), "resistant to paralysis", "less resistant to paralysis" },
		     {	 0, 0, 0, 0 } },

	vor_abil[] = { {	1, &(HFlying), "", "" },
			{1, &(HMagical_breathing), "", ""  },
		     {   1, &(HSleep_resistance), "", "" },
		     {   1, &(HPoison_resistance), "", "" },
		     {   1, &(HStone_resistance), "", "" },
			{   1, &(HManaleech), "", "" },
		     {	 0, 0, 0, 0 } },

	cor_abil[] = { {	1, &(HFlying), "", "" },
			{1, &(HMagical_breathing), "", ""  },
		     {   1, &(HSleep_resistance), "", "" },
		     {   1, &(HPoison_resistance), "", "" },
		     {   1, &(HStone_resistance), "", "" },
			{   1, &(HManaleech), "", "" },
		     {	 0, 0, 0, 0 } },

	alb_abil[] = { {   1, &(HAntimagic), "", "" },
		     {   3, &(HUndead_warning), "sensitive", "careless" },
		     {   10, &(HSee_invisible), "your vision sharpen", "your vision blur" },
			{  12, &(HStealth), "stealthy", "noisy" },
			{15, &(HMagical_breathing), "aquatic", "hydrophobic"  },
		     {	 25, &(HEnergy_regeneration), "charged with mana", "a loss of mana" },
		     {   0, 0, 0, 0 } },

	alc_abil[] = { {	 1, &(HAcid_resistance), "", "" },
		     {	 0, 0, 0, 0 } },

	ent_abil[] = { {	 1, &(HFull_nutrient), "", "" },
		     {	 0, 0, 0, 0 } },

	ang_abil[] = { {   1, &(HAntimagic), "", "" },
		     {   1, &(HFlying), "", "" },
		     {   1, &(HCold_resistance), "", "" },
		     {   1, &(HShock_resistance), "", "" },
		     {   1, &(HSleep_resistance), "", "" },
		     {   1, &(HPoison_resistance), "", "" },
		     {   1, &(HSee_invisible), "", "" },
		     {   0, 0, 0, 0 } },

	dev_abil[] = { {   1, &(HFire_resistance), "", "" },
		     {   1, &(HPoison_resistance), "", "" },
		     {   0, 0, 0, 0 } },

	anc_abil[] = { {   1, &(HCold_resistance), "", "" },
		     {	 1, &(HSlow_digestion), "", "" },
		     {   0, 0, 0, 0 } },

	fen_abil[] = { {   1, &(HSearching), "", "" },
			{   5, &(HStealth), "stealthy", "noisy" },
		     {   7, &(HFast), "quick", "slow" },
		     {	 10, &(HFire_resistance), "heat resistant", "less heat resistant" },
		     {  15, &(HSwimming), "ready to swim","afraid of the water" },
		     {   0, 0, 0, 0 } },

	nor_abil[] = { {   1, &(HCold_resistance), "", "" },
		     {	 15, &(HShock_resistance), "shock resistant", "less shock resistant" },
		     {   0, 0, 0, 0 } },

	dee_abil[] = { {   1, &(HFire_resistance), "", "" },
		     {	 1, &(HShock_resistance), "", "" },
		       {   10, &(HSee_invisible), "your vision sharpen", "your vision blurring" },
		     {   0, 0, 0, 0 } },

	sea_abil[] = { {   1, &(HCold_resistance), "", "" },
		     {	 1, &(HMagical_breathing), "", "" },
		     {	 1, &(HSwimming), "", "" },
		     {   0, 0, 0, 0 } },

	spr_abil[] = { {   1, &(HFast), "", "" },
		     {	 1, &(HRegeneration), "", "" },
		     {	 1, &(HHalf_spell_damage), "", "" },
		     {	 1, &(HHalf_physical_damage), "", "" },
		     {   0, 0, 0, 0 } },

	syl_abil[] = { {1,  &(HHunger), "", ""},
		    {	1,  &(HSee_invisible), "", ""},
		    { 4,  &(HStealth), "stealthy", "obvious"},
		    { 6,  &(HInfravision), "perceptive", "half blind"},
		    {18, &(HDetect_monsters), "perceptive", "dull"},
		    {18, &(HAggravate_monster), "noisy", "quiet" },
		    {0, 0, 0, 0} },

	gno_abil[] = { {   5, &(HStealth), "stealthy", "noisy" },
		     {   9, &(HFast), "quick", "slow" },
		     {   11, &(HSearching), "perceptive", "unaware" },
		     {   0, 0, 0, 0 } },

	hob_abil[] = { {  1, &(HStealth), "", "" },
		     {   7, &(HFast), "quick", "slow" },
		     {   0, 0, 0, 0 } },

	liz_abil[] = { {  1, &(HStone_resistance), "", "" },
		     {   0, 0, 0, 0 } },

	hrb_abil[] = { {	 1, &(HPoison_resistance), "", "" },
		     {	15, &(HWarning), "sensitive", "careless" },
		     {	 0, 0, 0, 0 } },

	bor_abil[] = { {   3, &(HTelepat), "disturbances in the force", "your grip on the force lessen" },
		       {   5, &(HSee_invisible), "your vision sharpen", "your vision blurring" },
		       { 0, 0, 0, 0 } },

	grd_abil[] = { {	 1, &(HPoison_resistance), "", "" },
		     {	 1, &(HShock_resistance), "", "" },
		     {	 0, 0, 0, 0 } },

	bat_abil[] = { {  1, &(HFlying), "", "" },
		     {  1, &(HDrain_resistance), "", "" },
		     {  1, &(HFear_resistance), "", "" },
		     {  1, &(HTechnicality), "", "" },
		     {   0, 0, 0, 0 } },

	nym_abil[] = { {  1, &(HTeleportation), "", "" },
		     {   0, 0, 0, 0 } },

	dvp_abil[] = { {  1, &(HTechnicality), "", "" },
		     {   0, 0, 0, 0 } },

	tur_abil[] = { {  1, &(HFull_nutrient), "", "" },
		     {   0, 0, 0, 0 } },

	roh_abil[] = { {  1, &(HFast), "", "" },
		     {   0, 0, 0, 0 } },

	thu_abil[] = { {  1, &(HFire_resistance), "", "" },
		     {   1, &(HFlying), "", "" },
		     {   0, 0, 0, 0 } },

	dlg_abil[] = { {  1, &(HDrain_resistance), "", "" },
		     {   0, 0, 0, 0 } },

	gig_abil[] = { {  10, &(HFire_resistance), "cool", "warmer" },
		     {   15, &(HCold_resistance), "warm", "cooler" },
		     {  20, &(HShock_resistance), "insulated", "conductive" },
		     {   0, 0, 0, 0 } },

	bre_abil[] = { {  1, &(HFire_resistance), "", "" },
		     {   1, &(HCold_resistance), "", "" },
		     {  1, &(HShock_resistance), "", "" },
		     {   0, 0, 0, 0 } },

	elm_abil[] = { {  1, &(HFire_resistance), "", "" },
		     {   1, &(HCold_resistance), "", "" },
		     {  1, &(HShock_resistance), "", "" },
		     {   0, 0, 0, 0 } },

	red_abil[] = { {  1, &(HSick_resistance), "", "" },
		     {   1, &(HPoison_resistance), "", "" },
		     {   0, 0, 0, 0 } },

	yok_abil[] = { {  1, &(HSick_resistance), "", "" },
		     {   1, &(HPoison_resistance), "", "" },
		     {   1, &(HFire_resistance), "", "" },
		     {   0, 0, 0, 0 } },

	sin_abil[] = { {  1, &(HConf_resist), "", "" },
		     {   0, 0, 0, 0 } },

	rdt_abil[] = { {  1, &(HStun_resist), "", "" },
		     {   10, &(HStun), "slightly stunned", "less stunned" },
		     {   10, &(HStunnopathy), "", "" }, /* previous one already gives message --Amy */
		     {   0, 0, 0, 0 } },

	kob_abil[] = { {  1, &(HPoison_resistance), "", "" },
		     {   0, 0, 0, 0 } },

	coc_abil[] = { {  1, &(HPoison_resistance), "", "" },
		     {   1, &(HStone_resistance), "", "" },
		     {   0, 0, 0, 0 } },

	sna_abil[] = { {  1, &(HPoison_resistance), "", "" },
		     {   0, 0, 0, 0 } },

	ill_abil[] = { {  1, &(HTelepat), "", "" },
		     {   0, 0, 0, 0 } },

	spi_abil[] = { {  1, &(HPoison_resistance), "", "" },
		     {   0, 0, 0, 0 } },

	gel_abil[] = { {  1, &(HPoison_resistance), "", "" },
		     {   1, &(HCold_resistance), "", "" },
		     {   1, &(HFire_resistance), "", "" },
		     {   1, &(HShock_resistance), "", "" },
		     {   1, &(HSleep_resistance), "", "" },
		     {   1, &(HAcid_resistance), "", "" },
		     {   1, &(HStone_resistance), "", "" },
		     {   1, &(HFull_nutrient), "", "" },
		     {   1, &(HDiscount_action), "", "" },
		     {   0, 0, 0, 0 } },

	ins_abil[] = { {  1, &(HPoison_resistance), "", "" },
		     {   1, &(HFull_nutrient), "", "" },
			{   20, &(HManaleech), "magically attuned", "no longer magically attuned" },
		     {   0, 0, 0, 0 } },

	tro_abil[] = { {  1, &(HSick_resistance), "", "" },
		     {   1, &(HRegeneration), "", "" },
		     {   0, 0, 0, 0 } },

	ung_abil[] = { {  1, &(HSearching), "", "" },
		     {   1, &(HWarning), "", "" },
		     {   0, 0, 0, 0 } },

	gas_abil[] = { {  1, &(HSick_resistance), "", "" },
		     {   1, &(HCold_resistance), "", "" },
		     {   1, &(HSleep_resistance), "", "" },
		     {   1, &(HPoison_resistance), "", "" },
		     {   0, 0, 0, 0 } },

	mum_abil[] = { {  1, &(HSick_resistance), "", "" },
		     {   1, &(HCold_resistance), "", "" },
		     {   1, &(HSleep_resistance), "", "" },
		     {   1, &(HPoison_resistance), "", "" },
		     {   1, &(HUndead_warning), "", "" },
		     {   0, 0, 0, 0 } },

	din_abil[] = { {  1, &(HDiscount_action), "", "" },
		     {   0, 0, 0, 0 } },

	ske_abil[] = { {  1, &(HSick_resistance), "", "" },
		     {   1, &(HCold_resistance), "", "" },
		     {   1, &(HSleep_resistance), "", "" },
		     {   1, &(HPoison_resistance), "", "" },
		     {   1, &(HStone_resistance), "", "" },
		     {   1, &(HMagical_breathing), "", "" },
		     {   0, 0, 0, 0 } },

	lev_abil[] = { {  1, &(HTelepat), "", "" },
		     {   1, &(HHallu_party), "", "" },
		     {   1, &(HDrunken_boxing), "", "" },
		     {   1, &(HStunnopathy), "", "" },
		     {   1, &(HNumbopathy), "", "" },
		     {   1, &(HDimmopathy), "", "" },
		     {   1, &(HFreezopathy), "", "" },
		     {   1, &(HStoned_chiller), "", "" },
		     {   1, &(HCorrosivity), "", "" },
		     {   1, &(HFear_factor), "", "" },
		     {   1, &(HBurnopathy), "", "" },
		     {   1, &(HSickopathy), "", "" },
		     {   1, &(HWonderlegs), "", "" },
		     {   1, &(HGlib_combat), "", "" },
		     {   0, 0, 0, 0 } },

	wra_abil[] = { {  1, &(HSick_resistance), "", "" },
		     {   1, &(HCold_resistance), "", "" },
		     {   1, &(HSleep_resistance), "", "" },
		     {   1, &(HPoison_resistance), "", "" },
		     {   1, &(HStone_resistance), "", "" },
		     {   1, &(HUndead_warning), "", "" },
		     {   1, &(HFlying), "", "" },
		     {   0, 0, 0, 0 } },

	lic_abil[] = { {  1, &(HSick_resistance), "", "" },
		     {   1, &(HCold_resistance), "", "" },
		     {   1, &(HSleep_resistance), "", "" },
		     {   1, &(HPoison_resistance), "", "" },
		     {   1, &(HUndead_warning), "", "" },
		     {   1, &(HRegeneration), "", "" },
		     {   1, &(HTeleportation), "", "" },
		     {   1, &(HTeleport_control), "", "" },
		     {   0, 0, 0, 0 } },

	jel_abil[] = { {	 1, &(HPoison_resistance), "", "" },
		     {   1, &(HSleep_resistance), "", "" },
			{   1, &(HManaleech), "", "" },
			{   1, &(HDiscount_action), "", "" },
		     {	 0, 0, 0, 0 } },

	wim_abil[] = { {	 1, &(HFire_resistance), "", "" },
		     {   1, &(HRegeneration), "", "" },
		     {   1, &(HTeleportation), "", "" },
		     {	 0, 0, 0, 0 } },

	trn_abil[] = { {	 1, &(HExtra_wpn_practice), "", "" },
		     {	 0, 0, 0, 0 } },

	exp_abil[] = { {	 1, &(HPsi_resist), "", "" },
		     {	 0, 0, 0, 0 } },

	rod_abil[] = { {   1, &(HFire_resistance), "", "" },
		     {   1, &(HPoison_resistance), "", "" },
		     {   1, &(HMagical_breathing), "", "" },
		     {   1, &(HRegeneration), "", "" },
		     {   1, &(HTeleportation), "", "" },
		     {   1, &(HTeleport_control), "", "" },
		     {   1, &(HSee_invisible), "", "" },
		     {   0, 0, 0, 0 } },

	esp_abil[] = { {  1, &(HSick_resistance), "", "" },
		     {   1, &(HCold_resistance), "", "" },
		     {   1, &(HSleep_resistance), "", "" },
		     {   1, &(HPoison_resistance), "", "" },
		     {   1, &(HDisint_resistance), "", "" },
		     {   1, &(HStone_resistance), "", "" },
		     {   1, &(HUndead_warning), "", "" },
		     {   0, 0, 0, 0 } },

	lyc_abil[] = { {   1, &(HPoison_resistance), "", "" },
		     {   1, &(HRegeneration), "", "" },
		     {   7, &(HStealth), "stealthy", "noisy" },
		     {   0, 0, 0, 0 } },

	aur_abil[] = { {   1, &(HReflecting), "", "" },
		     {   0, 0, 0, 0 } },

	sat_abil[] = { {   1, &(HTechnicality), "", "" },
		     {   0, 0, 0, 0 } },

	sho_abil[] = { {   1, &(HDiscount_action), "", "" },
		     {   0, 0, 0, 0 } },

	ink_abil[] = { {   1, &(HPeacevision), "", "" },
		     {   1, &(HTechnicality), "", "" },
		     {   0, 0, 0, 0 } },

	sed_abil[] = { {   1, &(HAntimagic), "", "" },
		     {   0, 0, 0, 0 } },

	orc_abil[] = { {	1, &(HPoison_resistance), "", "" },
		     {	 0, 0, 0, 0 } };

/*static long next_check = 600L;*/	/* arbitrary first setting */
STATIC_DCL void exerper(void);
STATIC_DCL void postadjabil(long *);

/* adjust an attribute; return TRUE if change is made, FALSE otherwise */
boolean
adjattrib(ndx, incr, msgflg)
	int	ndx, incr;
	int	msgflg;	    /* 2 => no message at all, 1 => no message */
			    /* except encumber, zero => message, and */
{			    /* negative => conditional (msg if change made) */
	if (Fixed_abil || Race_if(PM_SUSTAINER) || (uarms && uarms->oartifact == ART_SYSTEMATIC_CHAOS) || (uarms && uarms->oartifact == ART_BONUS_HOLD) || (uamul && uamul->oartifact == ART_FIX_EVERYTHING) || !incr) return FALSE;

	if ((ndx == A_INT || ndx == A_WIS)
				&& uarmh && uarmh->otyp == DUNCE_CAP) {
		if (msgflg == 0)
		    Your("cap constricts briefly, then relaxes again.");
		return FALSE;
	}

	/* Mithril items can sometimes prevent the player's stats from decreasing --Amy */
	if (incr < 0) {

		int mithrilitemcount = 0;

		if (uwep && objects[uwep->otyp].oc_material == MITHRIL) mithrilitemcount++;
		if (u.twoweap && uswapwep && objects[uswapwep->otyp].oc_material == MITHRIL) mithrilitemcount++;
		if (uarm && objects[uarm->otyp].oc_material == MITHRIL) mithrilitemcount++;
		if (uarmc && objects[uarmc->otyp].oc_material == MITHRIL) mithrilitemcount++;
		if (uarmh && objects[uarmh->otyp].oc_material == MITHRIL) mithrilitemcount++;
		if (uarms && objects[uarms->otyp].oc_material == MITHRIL) mithrilitemcount++;
		if (uarmg && objects[uarmg->otyp].oc_material == MITHRIL) mithrilitemcount++;
		if (uarmf && objects[uarmf->otyp].oc_material == MITHRIL) mithrilitemcount++;
		if (uarmu && objects[uarmu->otyp].oc_material == MITHRIL) mithrilitemcount++;
		if (uamul && objects[uamul->otyp].oc_material == MITHRIL) mithrilitemcount++;
		if (uimplant && objects[uimplant->otyp].oc_material == MITHRIL) mithrilitemcount++;
		if (uleft && objects[uleft->otyp].oc_material == MITHRIL) mithrilitemcount++;
		if (uright && objects[uright->otyp].oc_material == MITHRIL) mithrilitemcount++;
		if (ublindf && objects[ublindf->otyp].oc_material == MITHRIL) mithrilitemcount++;

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

	    ABASE(ndx) += incr;
	    if(ABASE(ndx) > AMAX(ndx)) {
		incr = ABASE(ndx) - AMAX(ndx);
		AMAX(ndx) += incr;
		if(AMAX(ndx) > ATTRMAX(ndx))
		    AMAX(ndx) = ATTRMAX(ndx);
		ABASE(ndx) = AMAX(ndx);
	    }
	} else {
	    if (ABASE(ndx) <= ATTRMIN(ndx)) {
		if (msgflg == 0 && flags.verbose)
		    pline("You're already as %s as you can get.",
			  minusattr[ndx]);
		ABASE(ndx) = ATTRMIN(ndx); /* just in case */
		return FALSE;
	    }

	    ABASE(ndx) += incr;
	    if(ABASE(ndx) < ATTRMIN(ndx)) {
		incr = ABASE(ndx) - ATTRMIN(ndx);
		ABASE(ndx) = ATTRMIN(ndx);
		AMAX(ndx) += incr;
		if(AMAX(ndx) < ATTRMIN(ndx))
		    AMAX(ndx) = ATTRMIN(ndx);
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
	if (msgflg <= 1 && moves > 1 && (ndx == A_STR || ndx == A_CON))
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
	    if(ABASE(A_STR) < 18) num = (rn2(4) ? 1 : rnd(6) );
	    else if (ABASE(A_STR) < STR18(85)) num = rnd(10);
	}
	(void) adjattrib(A_STR, (otmp && otmp->cursed) ? -num : num, TRUE);
}

void
losestr(num)	/* may kill you; cause may be poison or monster like 'a' */
	register int num;
{
	int ustr = ABASE(A_STR) - num;

	int hpreduce = rnd(5);

	if (num > 0) pline("%d points of your strength got sapped!",num);
	if (num < 0) pline("%d points of your strength got restored!",-num);

	if (ustr < 3) pline("Since you don't have enough strength, your health is being sapped instead.");

	while(ustr < 3) {
	    ++ustr;
	    --num;
	    hpreduce = rnd(5);
	    if (Upolyd) {
		u.mh -= hpreduce;
		u.mhmax -= hpreduce;
	    } else {
		u.uhp -= hpreduce;
		u.uhpmax -= hpreduce;
	    }
	}
	(void) adjattrib(A_STR, -num, TRUE);
}

void
change_luck(n)
	register schar n;
{
	u.uluck += n;
	if (u.uluck < 0 && u.uluck < LUCKMIN)	u.uluck = LUCKMIN;
	if (!LuckLoss && !u.uprops[LUCK_LOSS].extrinsic && !have_unluckystone() && u.uluck > 0 && u.uluck > LUCKMAX)	u.uluck = LUCKMAX;
	if ( (LuckLoss || u.uprops[LUCK_LOSS].extrinsic || have_unluckystone()) && u.uluck > 0 && u.uluck > LUCKMAX) {
		u.uluck = LUCKMIN;
		You_feel("something turning around..."); /* extra vague message (evil patch idea by jonadab) */
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
}


/* KMH, balance patch -- new function for healthstones */
void
recalc_health()
{
	register struct obj *otmp;


	u.uhealbonus = 0;

	for(otmp = invent; otmp; otmp=otmp->nobj)
	    if (otmp->otyp == HEALTHSTONE)
	    	u.uhealbonus += otmp->quan *
	    			(otmp->blessed ? 2 : otmp->cursed ? -2 : 1);
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

		if (inc_or_dec && Race_if(PM_TRAINER) && rn2(10)) return;

		if (inc_or_dec && Race_if(PM_DUNADAN) && rn2(2)) return;

		if (inc_or_dec && Race_if(PM_PLAYER_SKELETON) && rn2(5)) return;

		/* note by Amy - it's stupid if you can only lose your attribute points if they're greater than 18. */

		AEXE(i) += (inc_or_dec) ? (( (rn2(19) > ACURR(i) ) || !rn2(10) ) && !(PlayerCannotExerciseStats || u.uprops[EXERCISE_DEACTIVATED].extrinsic || have_exercisestone()) ) : -rn2(2);

		if (Extra_wpn_practice) 
			AEXE(i) += (inc_or_dec) ? (( (rn2(19) > ACURR(i) ) || !rn2(10) ) && !(PlayerCannotExerciseStats || u.uprops[EXERCISE_DEACTIVATED].extrinsic || have_exercisestone()) ) : 0;

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
#define SATIATED	0
#define NOT_HUNGRY	1
#define HUNGRY		2
#define WEAK		3
#define FAINTING	4
#define FAINTED		5
#define STARVED		6

STATIC_OVL void
exerper()
{
	/* changes by Amy to make these happen less often, because they shouldn't dominate attribute training/abuse */

	if(!(moves % 10)) {
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
					if (Role_if(PM_MONK))
					    exercise(A_WIS, FALSE);

					}
					if (Role_if(PM_TOPMODEL) || RngeAnorexia || Role_if(PM_FAILED_EXISTENCE) || (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "anorexia cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "yedyat plashch rasstroystvo") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "eb buzilishi plash") )) ) { /* They strongly dislike being full --Amy */
					    exercise(A_WIS, FALSE); exercise(A_STR, FALSE); exercise(A_CON, FALSE); exercise(A_DEX, FALSE); }
					break;
		    case NOT_HUNGRY:	exercise(A_CON, TRUE); break;
		    case HUNGRY:	if (Role_if(PM_TOPMODEL) || RngeAnorexia || Role_if(PM_FAILED_EXISTENCE) || (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "anorexia cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "yedyat plashch rasstroystvo") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "eb buzilishi plash") )) ) exercise(A_WIS, TRUE); break;
		    case WEAK:		if (!Role_if(PM_TOPMODEL) && !rn2(10) && !RngeAnorexia && !Role_if(PM_FAILED_EXISTENCE) && !(uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "anorexia cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "yedyat plashch rasstroystvo") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "eb buzilishi plash") )) ) exercise(A_STR, FALSE);
					if (Role_if(PM_MONK))	/* fasting */
					    exercise(A_WIS, TRUE);
					if (Role_if(PM_TOPMODEL) || RngeAnorexia || Role_if(PM_FAILED_EXISTENCE) || (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "anorexia cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "yedyat plashch rasstroystvo") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "eb buzilishi plash") )) ) {
					    exercise(A_WIS, TRUE); exercise(A_STR, TRUE);
}					break;
		    case FAINTING:
		    case FAINTED:	 if (Role_if(PM_TOPMODEL) || RngeAnorexia || Role_if(PM_FAILED_EXISTENCE) || (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "anorexia cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "yedyat plashch rasstroystvo") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "eb buzilishi plash") )) ) { exercise(A_WIS, TRUE); exercise(A_STR, TRUE); exercise(A_DEX, TRUE);		}					
					if (!Role_if(PM_TOPMODEL) && !rn2(10) && !RngeAnorexia && !Role_if(PM_FAILED_EXISTENCE) && !(uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "anorexia cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "yedyat plashch rasstroystvo") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "eb buzilishi plash") )) ) exercise(A_CON, FALSE); break;
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
	if(!(moves % 50)) {
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
		if((Wounded_legs 
		    && !u.usteed
			    ) || Fumbling || (HStun && !Stun_resist) )	exercise(A_DEX, FALSE);
	}
}

void
exerchk()
{
	int	i, mod_val;
	boolean contaminated;
	boolean verycontaminated;

	/* contamination is guaranteed to damage wisdom if at least 100 points accumulated on the contamination counter;
	 * otherwise, have a chance of damaging it --Amy */
	contaminated = FALSE;
	verycontaminated = FALSE;
	if (u.contamination >= 100) {
		contaminated = TRUE;
		verycontaminated = TRUE;
	}
	if (u.contamination && u.contamination < 100) contaminated = (rn2(100) < u.contamination);

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
	    for(i = 0; i < A_MAX; AEXE(i++) /= 2) {

		if (contaminated && i == A_WIS) {
			pline(Hallucination ? "You have a severe coughing fit and an intense desire to vomit!" : "You are consumed by your contamination!");

			/* Sustainer race has no way of improving wisdom and should therefore be resistant --Amy */
			if (Race_if(PM_SUSTAINER) && rn2(10)) pline("But you resist the effects!");
			else {
				pline("You lose  Wisdom");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Pochemu vy ne sledite luchshe dlya vashikh atributov, tak ili inache?" : "Due-l-ue-l-ue-l!");
				ABASE(A_WIS) -= 1;
				AMAX(A_WIS) -= 1;
				flags.botl = 1;
				if(ABASE(A_WIS) < ATTRMIN(A_WIS)) {
					u.youaredead = 1;
					pline("The contamination consumes you completely...");
					killer = "being consumed by the contamination";
					killer_format = KILLED_BY;
					done(DIED);
					/* lifesaved */
					u.youaredead = 0;
					pline("WARNING: Your wisdom is still critically low and you may die from contamination again! You can cure it by using a scroll or wand of remove curse, or by successfully praying on a coaligned altar. Amnesia may also help in a pinch, or you may buy a decontamination service from a nurse.");
				} else if (ABASE(A_WIS) < 4) {
					pline("DANGER!!! Your wisdom is critically low and you're very likely to die from the contamination! You can cure it by using a scroll or wand of remove curse, or by successfully praying on a coaligned altar. Amnesia may also help in a pinch, or you may buy a decontamination service from a nurse.");
				} else if (ABASE(A_WIS) < 6) {
					pline("Warning! Your wisdom is low and if the contamination causes it to fall below 3, you die! You can cure it by using a scroll or wand of remove curse, or by successfully praying on a coaligned altar. Amnesia may also help in a pinch, or you may buy a decontamination service from a nurse.");
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
		/* different formula for abuse --Amy */
		if(sgn(AEXE(i)) <= 0 && rn2(50) > ((i != A_WIS) ? abs(AEXE(i)*2/3) : abs(AEXE(i))))
		    continue;
		if(sgn(AEXE(i)) <= 0 && !rn2(AVAL) )
		    continue;

		mod_val = sgn(AEXE(i));

#ifdef DEBUG
		pline("exerchk: changing %d.", i);
#endif
		if(adjattrib(i, mod_val, -1)) {
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

/* next_check will otherwise have its initial 600L after a game restore */
void
reset_attribute_clock()
{
	/*if (moves > 600L) next_check = moves + rn1(50,800);*/
}


void
init_attr(np)
	register int	np;
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

	    if(ABASE(i) <= ATTRMIN(i)) {

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
	    if (AMAX(i) < ATTRMIN(i)) AMAX(i) = ATTRMIN(i);
	    ABASE(i) = ABASE(i) * AMAX(i) / tmp;
	    /* ABASE(i) > ATTRMAX(i) is impossible */
	    if (ABASE(i) < ATTRMIN(i)) ABASE(i) = ATTRMIN(i);
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
	case PM_ANACHRONIST:    abil = ana_abil;	break;  
	case PM_BARBARIAN:      abil = bar_abil;	break;
	case PM_COURIER:      abil = cou_abil;	break;
	case PM_MAHOU_SHOUJO:      abil = mah_abil;	break;
	case PM_PICKPOCKET:      abil = pic_abil;	break;
	case PM_STAND_USER:      abil = sta_abil;	break;
	case PM_JESTER:      abil = jes_abil;	break;
	case PM_LADIESMAN:      abil = lad_abil;	break;
	case PM_FENCER:      abil = fnc_abil;	break;
	case PM_FIGHTER:      abil = fig_abil;	break;
	case PM_MEDIUM:      abil = med_abil;	break;
	case PM_BLOODSEEKER:      abil = blo_abil;	break;
	case PM_DQ_SLIME:      abil = sli_abil;	break;
	case PM_ERDRICK:      abil = erd_abil;	break;
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
	case PM_FLAME_MAGE:	abil = fla_abil;	break;
	case PM_COOK:	abil = coo_abil;	break;
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
	case PM_JEDI:		abil = jed_abil;	break;
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
	case PM_ROGUE:          abil = rog_abil;	break;
	case PM_RINGSEEKER:		abil = rin_abil;	break;
	case PM_SAMURAI:        abil = sam_abil;	break;
	case PM_TOURIST:        abil = tou_abil;	break;
	case PM_UNDEAD_SLAYER:	abil = und_abil;	break;
	case PM_MIDGET:		abil = mid_abil;	break;
	case PM_UNDERTAKER:	abil = unt_abil;	break;
	case PM_GOLDMINER:		abil = gol_abil;	break;
	case PM_VALKYRIE:       abil = val_abil;	break;
	case PM_OFFICER:       abil = off_abil;	break;
	case PM_WIZARD:         abil = wiz_abil;	break;
	case PM_ZOOKEEPER:         abil = zoo_abil;	break;
	case PM_YEOMAN:		abil = yeo_abil;	break;
	case PM_JUSTICE_KEEPER:		abil = jus_abil;	break;
	default:                abil = 0;		break;
	}

	switch (Race_switch) {
	case PM_DOPPELGANGER:	rabil = dop_abil;	break;
	case PM_DWARF:		rabil = dwa_abil;	break;
	case PM_PLAYER_SLIME:		rabil = slm_abil;	break;
	case PM_DROW:
	case PM_ELF:            rabil = elf_abil;	break;
	case PM_INCANTIFIER:            rabil = inc_abil;	break;
	case PM_PLAYER_GREMLIN:            rabil = gre_abil;	break;
	case PM_CLOCKWORK_AUTOMATON:            rabil = clk_abil;	break;
	case PM_KOBOLT:            rabil = kob_abil;	break;
	case PM_BRETON:            rabil = bre_abil;	break;
	case PM_ELEMENTAL:            rabil = elm_abil;	break;
	case PM_REDGUARD:            rabil = red_abil;	break;
	case PM_YOKUDA:            rabil = yok_abil;	break;
	case PM_SINNER:            rabil = sin_abil;	break;
	case PM_REDDITOR:            rabil = rdt_abil;	break;
	case PM_TROLLOR:            rabil = tro_abil;	break;
	case PM_SNAKEMAN:            rabil = sna_abil;	break;
	case PM_ILLITHID:            rabil = ill_abil;	break;
	case PM_SPIDERMAN:            rabil = spi_abil;	break;
	case PM_WEAPON_CUBE:            rabil = gel_abil;	break;
	case PM_INSECTOID:            rabil = ins_abil;	break;
	case PM_UNGENOMOLD:            rabil = ung_abil;	break;
	case PM_GASTLY:            rabil = gas_abil;	break;
	case PM_PHANTOM_GHOST:            rabil = gas_abil;	break;
	case PM_MUMMY:            rabil = mum_abil;	break;
	case PM_WEAPONIZED_DINOSAUR:            rabil = din_abil;	break;
	case PM_PLAYER_SKELETON:            rabil = ske_abil;	break;
	case PM_HUMAN_WRAITH:            rabil = wra_abil;	break;
	case PM_LICH_WARRIOR:            rabil = lic_abil;	break;
	case PM_SPIRIT:            rabil = esp_abil;	break;
	case PM_SHOE:            rabil = sho_abil;	break;
	case PM_SATRE:            rabil = sat_abil;	break;
	case PM_BORG:            rabil = bor_abil;	break;
	case PM_RODNEYAN:            rabil = rod_abil;	break;
	case PM_JELLY:            rabil = jel_abil;	break;
	case PM_WEAPON_IMP:            rabil = wim_abil;	break;
	case PM_TRAINER:            rabil = trn_abil;	break;
	case PM_EXPERT:            rabil = exp_abil;	break;
	case PM_GIGANT:            rabil = gig_abil;	break;
	case PM_NYMPH:            rabil = nym_abil;	break;
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
	case PM_ORC:            rabil = orc_abil;	break;
	case PM_HUMAN_WEREWOLF:	rabil = lyc_abil;	break;
	case PM_FENEK:	rabil = fen_abil;	break;
	case PM_AUREAL:	rabil = aur_abil;	break;
	case PM_INKA:	rabil = ink_abil;	break;
	case PM_MAZKE:	rabil = sed_abil;	break;
	case PM_ARMED_COCKATRICE:	rabil = coc_abil;	break;
	case PM_NORD:	rabil = nor_abil;	break;
	case PM_SEA_ELF:	rabil = sea_abil;	break;
	case PM_DEEP_ELF:	rabil = dee_abil;	break;
	case PM_SYLPH:	rabil = syl_abil;	break;
	case PM_SPRIGGAN:	rabil = spr_abil;	break;
	case PM_ALCHEMIST:           rabil = alc_abil;	break;
	case PM_ALBAE:	rabil = alb_abil;	break;
	case PM_VORTEX:	rabil = vor_abil;	break;
	case PM_LOWER_ENT:	rabil = ent_abil;	break;
	case PM_CORTEX:	rabil = cor_abil;	break;
	case PM_LEVITATOR:	rabil = lev_abil;	break;
	case PM_HUMANOID_ANGEL:	rabil = ang_abil;	break;
	case PM_HUMANOID_DEVIL:	rabil = dev_abil;	break;
	case PM_ANCIPITAL:	rabil = anc_abil;	break;
	case PM_TURTLE:	rabil = tur_abil;	break;
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
			if (abil->ulevel == 1)
				*(abil->ability) |= (mask|FROMOUTSIDE);
			else
				*(abil->ability) |= mask;
			if(!(*(abil->ability) & INTRINSIC & ~mask)) {
			    if(*(abil->gainstr))
				You_feel("%s!", abil->gainstr);
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

	if (ACURR(A_CON) <= 3) conplus = -2;
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
		if (uarmc && uarmc->oartifact == ART_HOSTES_AD_PULVEREM_FERIRE) tmp += (5 + uarmc->spe);
		if (uarmg && uarmg->oartifact == ART_GAUNTLETS_OF_SLAYING) tmp += (3 + uarmg->spe);
		if (uarmf && uarmf->oartifact == ART_ANASTASIA_S_GENTLENESS) tmp -= 10;
		if (uarmf && uarmf->oartifact == ART_LILAC_BEAUTY) tmp -= 5;
		if (!flags.female && uarmu && uarmu->oartifact == ART_FEMALE_BEAUTY) tmp -= 5;
		if (!flags.female && uarmu && uarmu->oartifact == ART_FEMINIST_GIRL_S_PURPLE_WEA) tmp -= 5;
		if (uarmh && uarmh->oartifact == ART_DUNCE_POUNCE) tmp += 5;
		if (uarmh && uarmh->oartifact == ART_MASSIVE_IRON_CROWN_OF_MORG) tmp += 5;
		if (uleft && uleft->oartifact == ART_UNBELIEVABLY_STRONG_PUNCH) tmp += uleft->spe;
		if (uright && uright->oartifact == ART_UNBELIEVABLY_STRONG_PUNCH) tmp += uright->spe;
		if (uleft && uleft->oartifact == ART_CATHAN_S_SIGIL) tmp += (3 + uleft->spe);
		if (uright && uright->oartifact == ART_CATHAN_S_SIGIL) tmp += (3 + uright->spe);
		if (uarmf && uarmf->oartifact == ART_OUT_OF_TIME) tmp += 5;
		if (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "thinking helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "myslyashchiy shlem") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "fikr dubulg'a") )) tmp += 1;
		if (uarmc && uarmc->oartifact == ART_HIGH_KING_OF_SKIRIM) tmp += 5;
		if (uwep && uwep->oartifact == ART_RAFSCHAR_S_SUPERWEAPON) tmp += 1;
		if (uimplant && uimplant->oartifact == ART_THAI_S_EROTIC_BITCH_FU) tmp += 5;

		if (FemaleTrapThai) tmp -= 2;
		if (uarmg && uarmg->otyp == GAUNTLETS_OF_POWER) tmp += (uarmg->spe + 3);

		if (uarmc && uarmc->oartifact == ART_ROKKO_CHAN_S_SUIT && tmp > 12) tmp = 12;
		if (uwep && uwep->oartifact == ART_JAPANESE_WOMEN && tmp > 14) tmp = 14;
		if (uswapwep && uswapwep->oartifact == ART_JAPANESE_WOMEN && tmp > 14) tmp = 14;
		if (AllStatsAreLower) tmp -= 10;
		if (u.uprops[STATS_LOWERED].extrinsic) tmp -= 10;
		if (have_lowstatstone()) tmp -= 10;

			 return((tmp >= 125) ? 125 : (tmp <= 3) ? 3 : tmp);
	} else if (x == A_CHA) {
		/*if (tmp < 18 && (youmonst.data && youmonst.data->mlet == S_NYMPH ||
		    u.umonnum == PM_SUCCUBUS || u.umonnum == PM_INCUBUS))
		    tmp = 18;*/
		if (youmonst.data && (youmonst.data->mlet == S_NYMPH || u.umonnum == PM_SUCCUBUS || u.umonnum == PM_INCUBUS)) tmp += 3;
		if (uarmh && uarmh->otyp == FEDORA) tmp += 1;        
		if (uarmf && uarmf->otyp == UGG_BOOTS) tmp -= 3;
		if (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "fleecy-colored cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "vorsistyye tsvetnoy plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "serjun rangli plash") )) tmp += 5;

		if (uarm && uarm->oartifact == ART_PLAYBOY_WITH_EARS) tmp += (5 + uarm->spe);
		if (uarmh && uarmh->oartifact == ART_PLAYBOY_SUPPLEMENT) tmp += (5 + uarmh->spe);
		if (uarmf && uarmf->oartifact == ART_GNOMISH_BOOBS) tmp += 3;
		if (uarmf && uarmf->oartifact == ART_EVELINE_S_LOVELIES) tmp += 3;
		if (uarmf && uarmf->oartifact == ART_KOKYO_NO_PAFOMANSUU_OKU) tmp += (5 + uarmf->spe);
		if (uarmf && uarmf->oartifact == ART_EVA_S_INCONSPICUOUS_CHARM) tmp += (uarmf->spe);
		if (uarmf && uarmf->oartifact == ART_SOLVEJG_S_STINKING_SLIPPER) tmp += 5;
		if (uarmf && uarmf->oartifact == ART_LEATHER_PUMPS_OF_HORROR) tmp += (5 + uarmf->spe);
		if (uarmf && uarmf->oartifact == ART_MANDY_S_ROUGH_BEAUTY) tmp += (5 + uarmf->spe);
		if (uarmf && uarmf->oartifact == ART_LILAC_BEAUTY) tmp += 10;
		if (uarmf && uarmf->oartifact == ART_MELISSA_S_BEAUTY) tmp += 10;
		if (uarmf && uarmf->oartifact == ART_BEAUTIFUL_TOPMODEL) tmp += 25;
		if (uarmf && uarmf->oartifact == ART_UNEVEN_STILTS) tmp += 15;
		if (uarmf && uarmf->oartifact == ART_RARE_ASIAN_LADY) tmp += 20;
		if (uleft && uleft->oartifact == ART_CRIMINAL_QUEEN) tmp += 5;
		if (uright && uright->oartifact == ART_CRIMINAL_QUEEN) tmp += 5;
		if (uleft && uleft->oartifact == ART_SCARAB_OF_ADORNMENT) tmp += 10;
		if (uright && uright->oartifact == ART_SCARAB_OF_ADORNMENT) tmp += 10;
		if (uamul && uamul->oartifact == ART_OH_COME_ON) tmp += 5;
		if (uamul && uamul->oartifact == ART_NECKLACE_OF_ADORNMENT) tmp += 10;
		if (uwep && uwep->oartifact == ART_BUNGA_BUNGA && uwep->spe > 0) tmp += (5 + uwep->spe);
		if (flags.female && uarmu && uarmu->oartifact == ART_FEMALE_BEAUTY) tmp += 5;
		if (flags.female && uarmu && uarmu->oartifact == ART_FEMINIST_GIRL_S_PURPLE_WEA) tmp += 5;
		if (!flags.female && uarmu && uarmu->oartifact == ART_FEMALE_BEAUTY) tmp -= 5;
		if (!flags.female && uarmu && uarmu->oartifact == ART_FEMINIST_GIRL_S_PURPLE_WEA) tmp -= 5;
		if (uarmu && uarmu->oartifact == ART_BIENVENIDO_A_MIAMI) tmp += 3;
		if (uarmh && uarmh->oartifact == ART_MASSIVE_IRON_CROWN_OF_MORG) tmp += 5;
		if (uarmc && uarmc->oartifact == ART_SEXY_STROKING_UNITS) tmp += 5;
		if (uarm && uarm->oartifact == ART_IMPRACTICAL_COMBAT_WEAR) tmp += 5;
		if (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "gentle cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "nezhnyy plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "muloyim plash") ) ) tmp += 1;
		if (uarmf && OBJ_DESCR(objects[uarmf->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "fetish heels") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "idol kabluki") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "but poshnalar") )) tmp += 5;
		if (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "thinking helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "myslyashchiy shlem") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "fikr dubulg'a") )) tmp += 1;
		if (uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "princess gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "perchatki printsessy") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "malika qo'lqop") )) tmp += 2;
		if (uarmc && uarmc->oartifact == ART_MOST_CHARISMATIC_PRESIDENT) tmp += 10;
		if (uarmc && uarmc->oartifact == ART_HIGH_KING_OF_SKIRIM) tmp += 5;
		if (uarmg && uarmg->oartifact == ART_MARY_INSCRIPTION) tmp += 10;
		if (uarmf && uarmf->oartifact == ART_JULIA_S_REAL_LOVE) tmp += 3;
		if (uarmf && uarmf->oartifact == ART_ELIANE_S_COMBAT_SNEAKERS) tmp += 20;
		if (uarmf && uarmf->oartifact == ART_ALISEH_S_RED_COLOR) tmp += 10;
		if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER) && uimplant && uimplant->oartifact == ART_THAI_S_EROTIC_BITCH_FU) tmp += 5;
		if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER) && uimplant && uimplant->oartifact == ART_WONDERLOVELINESS) tmp += 5;
		if (uimplant && uimplant->oartifact == ART_WONDERLOVELINESS) tmp += 5;
		if (uwep && uwep->oartifact == ART_CERULEAN_SMASH) tmp += 10;

		if (RngeCoquetry) tmp += 5;
		if (FemaleTrapSolvejg) tmp += 5;
		if (FemaleTrapNatalje) tmp += 10;

		if (AllStatsAreLower) tmp -= 10;
		if (u.uprops[STATS_LOWERED].extrinsic) tmp -= 10;
		if (have_lowstatstone()) tmp -= 10;
		if (uarmf && OBJ_DESCR(objects[uarmf->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "velcro sandals") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "sandalii na lipuchkakh") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "cirt kavushlari") )) tmp -= 5;

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

		if (uarmf && OBJ_DESCR(objects[uarmf->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "beautiful heels") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "krasivyye kabluki") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "chiroyli ko'chirish to'piqlarni") ) ) tmp += 5;

		return((tmp >= 25) ? 25 : (tmp <= 3) ? 3 : tmp);
	} else if (x == A_INT || x == A_WIS) {
		/* yes, this may raise int/wis if player is sufficiently
		 * stupid.  there are lower levels of cognition than "dunce".
		 */

		if (uarmf && uarmf->oartifact == ART_SOLVEJG_S_STINKING_SLIPPER) tmp -= 5; /* reduce both */
		if (uarmf && uarmf->oartifact == ART_LILAC_BEAUTY) tmp -= 5;
		if (uwep && uwep->oartifact == ART_KNOW_YOUR_INTRINSICS) tmp += 5; /* boost both */
		if (!flags.female && uarmu && uarmu->oartifact == ART_FEMALE_BEAUTY) tmp -= 5;
		if (!flags.female && uarmu && uarmu->oartifact == ART_FEMINIST_GIRL_S_PURPLE_WEA) tmp -= 5;
		if (uarmh && uarmh->oartifact == ART_MASSIVE_IRON_CROWN_OF_MORG) tmp += 5;
		if (x == A_INT && uwep && uwep->oartifact == ART_DIKKIN_S_FAVORITE_SPELL) tmp += 8;
		if (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "thinking helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "myslyashchiy shlem") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "fikr dubulg'a") )) tmp += 1;
		if (x == A_INT && uwep && uwep->oartifact == ART_RIP_STRATEGY) tmp += 5;
		if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER) && uimplant && uimplant->oartifact == ART_THAI_S_EROTIC_BITCH_FU) tmp += 5;
		if (uarmh && uarmh->otyp == HELM_OF_BRILLIANCE) tmp += uarmh->spe;

		if (x == A_INT && Race_if(PM_HUMAN_MONKEY) && tmp > 9) tmp = 9;
		if (uarmh && uarmh->oartifact == ART_DUNCE_POUNCE && tmp > 6) tmp = 6;
		if (AllStatsAreLower) tmp -= 10;
		if (u.uprops[STATS_LOWERED].extrinsic) tmp -= 10;
		if (have_lowstatstone()) tmp -= 10;

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
		if (uleft && uleft->oartifact == ART_GUARANTEED_HIT_POWER) tmp += uleft->spe;
		if (uright && uright->oartifact == ART_GUARANTEED_HIT_POWER) tmp += uright->spe;
		if (uarmf && uarmf->oartifact == ART_OUT_OF_TIME) tmp += 5;
		if (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "thinking helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "myslyashchiy shlem") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "fikr dubulg'a") )) tmp += 1;
		if (uwep && uwep->oartifact == ART_RAFSCHAR_S_SUPERWEAPON) tmp += 1;
		if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER) && uimplant && uimplant->oartifact == ART_THAI_S_EROTIC_BITCH_FU) tmp += 10;

		if (FemaleTrapThai) tmp -= 2;
		if (uarmc && uarmc->oartifact == ART_ROKKO_CHAN_S_SUIT && tmp > 12) tmp = 12;
		if (uarmg && uarmg->otyp == GAUNTLETS_OF_DEXTERITY) tmp += uarmg->spe;
		if (AllStatsAreLower) tmp -= 10;
		if (u.uprops[STATS_LOWERED].extrinsic) tmp -= 10;
		if (have_lowstatstone()) tmp -= 10;

	} else { /* A_CON */

		if (uarmf && uarmf->oartifact == ART_LILAC_BEAUTY) tmp -= 5;
		if (!flags.female && uarmu && uarmu->oartifact == ART_FEMALE_BEAUTY) tmp -= 5;
		if (!flags.female && uarmu && uarmu->oartifact == ART_FEMINIST_GIRL_S_PURPLE_WEA) tmp -= 5;
		if (uarmh && uarmh->oartifact == ART_MASSIVE_IRON_CROWN_OF_MORG) tmp += 5;
		if (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "rubynus helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "rubinovyy shlem") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "yoqut asosiy dubulg'a") )) tmp += 2;
		if (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "thinking helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "myslyashchiy shlem") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "fikr dubulg'a") )) tmp += 1;
		if (uarmh && uarmh->oartifact == ART_ELONA_S_SNAIL_TRAIL && Race_if(PM_ELONA_SNAIL)) tmp += 10;
		if (x == A_INT && uwep && uwep->oartifact == ART_RIP_STRATEGY) tmp += 5;
		if (uimplant && uimplant->oartifact == ART_THAI_S_EROTIC_BITCH_FU) tmp -= 10;

		if (uwep && uwep->oartifact == ART_JAPANESE_WOMEN && tmp > 14) tmp = 14;
		if (uswapwep && uswapwep->oartifact == ART_JAPANESE_WOMEN && tmp > 14) tmp = 14;

		if (FemaleTrapJessica) tmp -= 5;

		if (AllStatsAreLower) tmp -= 10;
		if (u.uprops[STATS_LOWERED].extrinsic) tmp -= 10;
		if (have_lowstatstone()) tmp -= 10;

	}
#ifdef WIN32_BUG
	return(x=((tmp >= 25) ? 25 : (tmp <= 3) ? 3 : tmp));
#else
	return((schar)((tmp >= 25) ? 25 : (tmp <= 3) ? 3 : tmp));
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

/* avoid possible problems with alignment overflow, and provide a centralized
 * location for any future alignment limits
 */
void
adjalign(n)
register int n;
{
	if (Race_if(PM_SINNER) && n < 0) {
		n *= 10;
		u.ualign.sins += 1; 
		u.alignlim -= 1;
	}

	if (uimplant && uimplant->oartifact == ART_SINFUL_REPENTER && n > 0) {
		if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER)) n *= 5;
		else n *= 2;

		if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER)) u.alignlim += 1;
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

	if (Race_if(PM_UNALIGNMENT_THING) && n < 0) pline("You lost %d alignment points; your new value is %d.", abs(n), u.ualign.record);
	if (Race_if(PM_UNALIGNMENT_THING) && n > 0) pline("You gained %d alignment points; your new value is %d.", n, u.ualign.record);

	/* evil patch idea by jonadab: losing alignment points can result in punishment
	 * immunizer can lose alignment during level changes, which would result in a panic; poison the player instead --Amy */
	if (n < 0 && u.ualign.record < 0 && !rn2(500)) {
		if (Race_if(PM_IMMUNIZER)) poisoned("The alignment", rn2(A_MAX), "immunized alignment failure", 30);
		else punishx();
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

/*attrib.c*/
