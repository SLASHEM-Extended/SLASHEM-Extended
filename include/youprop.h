/*	SCCS Id: @(#)youprop.h	3.4	1999/07/02	*/
/* Copyright (c) 1989 Mike Threepoint				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef YOUPROP_H
#define YOUPROP_H

#include "prop.h"
#include "permonst.h"
#include "mondata.h"
#include "pm.h"


/* KMH, intrinsics patch.
 * Reorganized and rewritten for >32-bit properties.
 * HXxx refers to intrinsic bitfields while in human form.
 * EXxx refers to extrinsic bitfields from worn objects.
 * BXxx refers to the cause of the property being blocked.
 * Xxx refers to any source, including polymorph forms.
 * NoXxx refers to a property being deactivated (addition by Amy).
 */


#define maybe_polyd(if_so,if_not)	(Upolyd ? (if_so) : (if_not))

#define EnglandMode	(uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "english gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "angliyskiye perchatki") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "ingliz tili qo'lqop") ))

#define FingerlessGloves	(uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "fingerless gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "mitenki") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "kam qo'lqop barmoq") ))

#define PlayerInHighHeels	(uarmf && ( (uarmf->otyp == WEDGE_SANDALS) || (uarmf->otyp == FEMININE_PUMPS) || (uarmf->otyp == LEATHER_PEEP_TOES) || (uarmf->otyp == HIPPIE_HEELS) || (uarmf->oartifact == ART_ABSURD_HEELED_TILESET) || (uarmf->oartifact == ART_SWARM_SOFT_HIGH_HEELS) || (uarmf->oartifact == ART_KATIE_MELUA_S_FEMALE_WEAPO) || (uarmf->oartifact == ART_MANUELA_S_UNKNOWN_HEELS) || (uarmf->oartifact == ART_RITA_S_TENDER_STILETTOS) || (uarmf->oartifact == ART_ANASTASIA_S_UNEXPECTED_ABI) || (uarmf->oartifact == ART_HIGH_HEELED_HUG) || (Role_if(PM_BINDER) && uarmf->oartifact == ART_BINDER_CRASH) || (uarmf->otyp == PET_STOMPING_PLATFORM_BOOTS) || (uarmf->otyp == SENTIENT_HIGH_HEELED_SHOES) || (uarmf->otyp == ATSUZOKO_BOOTS) || (uarmf->otyp == COMBAT_STILETTOS) || (uarmf->otyp == HIGH_STILETTOS) || (uarmf->otyp == UNFAIR_STILETTOS) || (uarmf->otyp == SKY_HIGH_HEELS) || (OBJ_DESCR(objects[uarmf->otyp]) && ( (!strcmp(OBJ_DESCR(objects[uarmf->otyp]), "irregular boots") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "neregulyarnyye sapogi") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "tartibsizlik chizilmasin") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "wedge boots") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "klin sapogi") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "xanjar chizilmasin") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "winter stilettos") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "zima stilety") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "qish sandal chizilmasin") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "clunky heels") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "neuklyuzhiye kabluki") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "qisqa ko'chirish to'piqlarni") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "ankle boots") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "botil'ony") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "bilagi zo'r chizilmasin") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "block-heeled boots") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "blok kablukakh sapogi") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "blok-o'tish chizilmasin") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "beautiful heels") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "krasivyye kabluki") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "chiroyli ko'chirish to'piqlarni") ) ) ) ) )

/*** Resistances to troubles ***/
/* With intrinsics and extrinsics */
#define HFire_resistance	u.uprops[FIRE_RES].intrinsic
#define EFire_resistance	u.uprops[FIRE_RES].extrinsic
#define Fire_resistance		((HFire_resistance || EFire_resistance || (uarmh && uarmh->oartifact == ART_SPECTRAL_RESISTANCE) || (uarmf && uarmf->oartifact == ART_YET_ANOTHER_STUPID_IDEA) || (uwep && uwep->oartifact == ART_AND_YOUR_MORTAL_WORLD_SHAL) || (uarmf && uarmf->oartifact == ART_JOSEFINE_S_EVILNESS) || (uwep && uwep->oartifact == ART_PRISMATIC_PROTECTION) || (uarmh && uarmh->oartifact == ART_HAVE_ALL_YOU_NEED) || (uwep && uwep->oartifact == ART_FAEAEAEAEAEAU) || (uarmh && uarmh->oartifact == ART_WAR_MASK_OF_DURIN) || (uarmu && uarmu->oartifact == ART_BIENVENIDO_A_MIAMI) || (uarmf && uarmf->oartifact == ART_GRENEUVENIA_S_HUG) || (uarmh && uarmh->oartifact == ART_MASSIVE_IRON_CROWN_OF_MORG) || (uarmf && uarmf->oartifact == ART_TEMPERATOR) || (uarm && uarm->oartifact == ART_ARMOR_OF_EREBOR) || (uarmc && uarmc->oartifact == ART_FIREBURN_COLDSHATTER) || (uwep && uwep->oartifact == ART_STAFF_OF_THE_ARCHMAGI) || (uwep && uwep->oartifact == ART_WHAT_IT_SAYS_ON_THE_TIN) || (uarmg && uarmg->oartifact == ART_SPECTRATOR && (moves % 5 == 0) ) || (uarmh && uarmh->oartifact == ART_SECURE_BATHMASTER) || (uwep && uwep->oartifact == ART_WRATH_OF_HEAVEN) || (uwep && uwep->oartifact == ART_COLD_SOUL) || u.uprops[STORM_HELM].extrinsic || u.uprops[ELEMENT_RES].extrinsic || \
				 resists_fire(&youmonst) || (uarmf && OBJ_DESCR(objects[uarmf->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmf->otyp]), "korean sandals") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "koreyskiye sandalii") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "janubiy koreyaning kavushlari") ) && (moves % 3 == 0) ) || is_fire_resistant(youmonst.data)) && !Race_if(PM_TROLLOR) && !Race_if(PM_ANCIPITAL) && !Race_if(PM_SEA_ELF) && !Race_if(PM_SPRIGGAN) && !Role_if(PM_ALTMER) && !NoFire_resistance)
#define NoFire_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_FIRE_RES].intrinsic || (uarm && uarm->oartifact == ART_SUPERESCAPE_MAIL) || (uarm && uarm->oartifact == ART_ABSOLUTE_MONSTER_MAIL) || (uarm && uarm->oartifact == ART_FLEEING_MINE_MAIL) || (uarmf && uarmf->oartifact == ART_MEPHISTO_S_BROGUES) || (uarmf && uarmf->oartifact == ART_VERA_S_FREEZER) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HCold_resistance	u.uprops[COLD_RES].intrinsic
#define ECold_resistance	u.uprops[COLD_RES].extrinsic
#define Cold_resistance		((HCold_resistance || ECold_resistance || (uarmh && uarmh->oartifact == ART_SPECTRAL_RESISTANCE) || (uarmg && uarmg->oartifact == ART_SIGNONS_STEEL_TOTAL) || (uarmf && uarmf->oartifact == ART_MEPHISTO_S_BROGUES) || (uarmf && uarmf->oartifact == ART_RARE_ASIAN_LADY) || (uarmf && uarmf->oartifact == ART_KATRIN_S_PARALYSIS) || (uarmf && uarmf->oartifact == ART_JESSICA_S_TENDERNESS) || (uarmf && uarmf->oartifact == ART_JOSEFINE_S_EVILNESS) || (uarmg && uarmg->oartifact == ART_SPECTRATOR && (moves % 5 == 0) ) || (uarmh && uarmh->oartifact == ART_SECURE_BATHMASTER) || (uwep && uwep->oartifact == ART_PRISMATIC_PROTECTION) || (uarmh && uarmh->oartifact == ART_MASSIVE_IRON_CROWN_OF_MORG) || (uarmf && uarmf->oartifact == ART_CHOICE_OF_MATTER) || (uarmu && uarmu->oartifact == ART_BIENVENIDO_A_MIAMI) || (uarmf && uarmf->oartifact == ART_CORINA_S_UNFAIR_SCRATCHER) || (uarmh && uarmh->oartifact == ART_HAVE_ALL_YOU_NEED) || (uarmc && uarmc->oartifact == ART_FIREBURN_COLDSHATTER) || (uarm && uarm->oartifact == ART_ARMOR_OF_EREBOR) || (uarmf && uarmf->oartifact == ART_CORINA_S_SNOWY_TREAD) || (uarmc && uarmc->oartifact == ART_CLOAK_OF_THE_CONSORT) || (uwep && uwep->oartifact == ART_STAFF_OF_THE_ARCHMAGI) || (uwep && uwep->oartifact == ART_COLD_SOUL) || (uwep && uwep->oartifact == ART_RELEASE_FROM_CARE) || u.uprops[STORM_HELM].extrinsic || u.uprops[ELEMENT_RES].extrinsic || \
				 resists_cold(&youmonst) || is_cold_resistant(youmonst.data)) && !Role_if(PM_ALTMER) && !NoCold_resistance )
#define NoCold_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_COLD_RES].intrinsic || (uleft && uleft->oartifact == ART_DEATHLY_COLD) || (uright && uright->oartifact == ART_DEATHLY_COLD) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HSleep_resistance	u.uprops[SLEEP_RES].intrinsic
#define ESleep_resistance	u.uprops[SLEEP_RES].extrinsic
#define Sleep_resistance	( !( (Thirst || u.uprops[THIRST].extrinsic || have_thirststone()) && (u.uhunger > 2000 ) ) && !Race_if(PM_KOBOLT) && !Race_if(PM_DEEP_ELF) && !have_sleepstone() && !NoSleep_resistance && (HSleep_resistance || ESleep_resistance || (uwep && uwep->oartifact == ART_SLEEPLESS_NIGHTS) || (uarmf && uarmf->oartifact == ART_JESSICA_S_TENDERNESS) || (uarmh && uarmh->oartifact == ART_HAVE_ALL_YOU_NEED) || (uamul && uamul->oartifact == ART_SNOREFEST && (moves % 10 != 0) ) || \
				 resists_sleep(&youmonst) || is_sleep_resistant(youmonst.data)) )
#define NoSleep_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SLEEP_RES].intrinsic || (uarms && uarms->oartifact == ART_LITTLE_THORN_ROSE) || (uamul && uamul->oartifact == ART_SPELLCASTER_S_DREAM) || (uarmh && uarmh->oartifact == ART_DULLIFIER) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HDisint_resistance	u.uprops[DISINT_RES].intrinsic
#define EDisint_resistance	u.uprops[DISINT_RES].extrinsic
#define Disint_resistance	((HDisint_resistance || EDisint_resistance || (uarmu && uarmu->oartifact == ART_GENTLE_SOFT_CLOTHING) || (uarmc && uarmc->oartifact == ART_INA_S_LAB_COAT) || (uarmc && uarmc->oartifact == ART_INA_S_OVERCOAT) || (uleft && uleft->oartifact == ART_RING_OF_THE_SCHWARTZ) || (uright && uright->oartifact == ART_RING_OF_THE_SCHWARTZ) || (uleft && uleft->oartifact == ART_ACHROMANTIC_RING) || (uright && uright->oartifact == ART_ACHROMANTIC_RING) || (uamul && uamul->oartifact == ART_STINGING_MEDALLION) || \
				 resists_disint(&youmonst) || is_disint_resistant(youmonst.data)) && !NoDisint_resistance)
#define NoDisint_resistance (!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_DISINT_RES].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HShock_resistance	u.uprops[SHOCK_RES].intrinsic
#define EShock_resistance	u.uprops[SHOCK_RES].extrinsic
#define Shock_resistance	((HShock_resistance || EShock_resistance || (uarmh && uarmh->oartifact == ART_SPECTRAL_RESISTANCE) || (uarmf && uarmf->oartifact == ART_KATRIN_S_PARALYSIS) || (uarmf && uarmf->oartifact == ART_SOLVEJG_S_STINKING_SLIPPER) || (uarmc && uarmc->oartifact == ART_ACIDSHOCK_CASTLECRUSHER) || (uwep && uwep->oartifact == ART_PRISMATIC_PROTECTION) || (uarmf && uarmf->oartifact == ART_JOSEFINE_S_EVILNESS) || (uarmg && uarmg->oartifact == ART_SPECTRATOR && (moves % 5 == 0) ) || (uarmh && uarmh->oartifact == ART_HAVE_ALL_YOU_NEED) || (uarmf && uarmf->oartifact == ART_CORINA_S_UNFAIR_SCRATCHER) || (uarmh && uarmh->oartifact == ART_MASSIVE_IRON_CROWN_OF_MORG) || (uarmh && uarmh->oartifact == ART_STORMHELM) || (uarmc && uarmc->oartifact == ART_WEB_OF_THE_CHOSEN) || (uwep && uwep->oartifact == ART_STAFF_OF_THE_ARCHMAGI) || (uwep && uwep->oartifact == ART_FIRE_OF_HEAVEN) || (uwep && uwep->oartifact == ART_COLD_SOUL) || u.uprops[STORM_HELM].extrinsic || u.uprops[ELEMENT_RES].extrinsic || \
				 resists_elec(&youmonst) || is_elec_resistant(youmonst.data)) && !Role_if(PM_ALTMER) && !NoShock_resistance )
#define NoShock_resistance (!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SHOCK_RES].intrinsic || (uwep && uwep->oartifact == ART_SHOCKING_THERAPY) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HPoison_resistance	u.uprops[POISON_RES].intrinsic
#define EPoison_resistance	u.uprops[POISON_RES].extrinsic
#define Poison_resistance	((HPoison_resistance || EPoison_resistance || (uarmh && uarmh->oartifact == ART_SPECTRAL_RESISTANCE) || (uarmu && uarmu->oartifact == ART_GENTLE_SOFT_CLOTHING) || (uarmc && uarmc->oartifact == ART_ACQUIRED_POISON_RESISTANCE) || (uarmc && uarmc->oartifact == ART_PREMIUM_VISCOSITY) || (uarmh && uarmh->oartifact == ART_BIG_BONNET) || (uarmg && uarmg->oartifact == ART_SIGNONS_STEEL_TOTAL) || (uarmf && uarmf->oartifact == ART_MEPHISTO_S_BROGUES) || (uwep && uwep->oartifact == ART_PRISMATIC_PROTECTION) || (uarmh && uarmh->oartifact == ART_MASSIVE_IRON_CROWN_OF_MORG) || (uleft && uleft->oartifact == ART_GREEN_COLOR) || (uright && uright->oartifact == ART_GREEN_COLOR) || (uarmh && uarmh->oartifact == ART_TARI_FEFALAS) || (uarmg && uarmg->oartifact == ART_SPECTRATOR && (moves % 5 == 0) ) || (uarmf && uarmf->oartifact == ART_LEATHER_PUMPS_OF_HORROR) || (uarmf && uarmf->oartifact == ART_RHEA_S_COMBAT_PUMPS) || (uamul && uamul->oartifact == ART_WARNED_AND_PROTECTED) || (uarmh && uarmh->oartifact == ART_GREEN_STATUS) || (uamul && uamul->oartifact == ART_GOOD_BEE) || (uamul && uamul->oartifact == ART_BUEING) || (uwep && uwep->oartifact == ART_ERU_ILUVATAR_S_BIBLE) || (uarmh && uarmh->oartifact == ART_WAR_MASK_OF_DURIN) || (uwep && uwep->oartifact == ART_SERPENT_S_TOOTH) || \
				 resists_poison(&youmonst) || is_poison_resistant(youmonst.data)) && !NoPoison_resistance && !Race_if(PM_POISONER) )
#define NoPoison_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_POISON_RES].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HDrain_resistance	u.uprops[DRAIN_RES].intrinsic
#define EDrain_resistance	u.uprops[DRAIN_RES].extrinsic
#define Drain_resistance	((HDrain_resistance || EDrain_resistance || (uarmc && uarmc->oartifact == ART_ALL_IN_ONE_ASCENSION_KIT) || (uleft && uleft->oartifact == ART_MAGICAL_SHOCK) || (uright && uright->oartifact == ART_MAGICAL_SHOCK) || (uarmh && uarmh->oartifact == ART_LLLLLLLLLLLLLM) || (uwep && uwep->oartifact == ART_CUBIC_BONE) || (uarmh && uarmh->oartifact == ART_YOU_ARE_ALREADY_DEAD) || (uarmg && uarmg->oartifact == ART_SPECTRATOR && (moves % 5 == 0) ) || (uarmh && uarmh->oartifact == ART_LOW_LOCAL_MEMORY) || (uwep && uwep->oartifact == ART_SANDMAN_VOLUME__) || (uwep && uwep->oartifact == ART_ORTHODOX_MANIFEST) || (uwep && uwep->oartifact == ART_UN_DEATH) || (uarmc && uarmc->oartifact == ART_VEIL_OF_LATONA)  || (uarmc && uarmc->oartifact == ART_VEIL_OF_MINISTRY) || (uwep && uwep->oartifact == ART_LANCE_OF_LONGINUS) || \
				 resists_drli(&youmonst) || is_drain_resistant(youmonst.data) || ((moves % 3 == 0) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "deep cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "glubokiy plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "chuqur plash") ) ) || (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "vampiric cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "vampir plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "sindirishi plash") ) && (moves % 10 == 0) ) ) && !NoDrain_resistance)
#define NoDrain_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_DRAIN_RES].intrinsic || (uamul && uamul->oartifact == ART___TH_NAZGUL) || (uamul && uamul->oartifact == ART_NAZGUL_S_REVENGE) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

/* Intrinsics only */
#define HSick_resistance	u.uprops[SICK_RES].intrinsic
#define ESick_resistance	u.uprops[SICK_RES].extrinsic
#define Sick_resistance		((HSick_resistance || ESick_resistance || (uarm && uarm->oartifact == ART_ANTISEPSIS_COAT) || (uarmc && uarmc->oartifact == ART_INA_S_LAB_COAT) || (uarmc && uarmc->oartifact == ART_INA_S_OVERCOAT) || (uarm && uarm->oartifact == ART_CHECK_YOUR_ESCAPES) || (uarmu && uarmu->oartifact == ART_THERMAL_BATH) || (uarmc && uarmc->oartifact == ART_CLEANLINESS_LAB) || (uarmf && uarmf->oartifact == ART_RHEA_S_COMBAT_PUMPS) || (uarmf && uarmf->oartifact == ART_BASE_FOR_SPEED_ASCENSION) || (uamul && uamul->oartifact == ART_ONE_MOMENT_IN_TIME) || (uleft && uleft->oartifact == ART_GOLDENIVY_S_RAGE) || (uright && uright->oartifact == ART_GOLDENIVY_S_RAGE) || (uleft && uleft->oartifact == ART_GOLDENIVY_S_ENGAGEMENT_RIN) || (uright && uright->oartifact == ART_GOLDENIVY_S_ENGAGEMENT_RIN) || (uamul && uamul->oartifact == ART_PRECIOUS_UNOBTAINABLE_PROP) || \
				 youmonst.data->mlet == S_FUNGUS || \
				 youmonst.data->mlet == S_MUMMY || \
				 youmonst.data->mlet == S_ZOMBIE || \
				 youmonst.data->mlet == S_WRAITH || \
				 youmonst.data->mlet == S_GHOST || \
				 youmonst.data->mlet == S_LICH || \
				 defends(AD_DISE,uwep)) && !NoSick_resistance)
#define NoSick_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SICK_RES].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))
#define Invulnerable		u.uprops[INVULNERABLE].intrinsic    /* [Tom] */

/* Extrinsics only */
#define HAntimagic		u.uprops[ANTIMAGIC].intrinsic
#define EAntimagic		u.uprops[ANTIMAGIC].extrinsic
#define Antimagic		((HAntimagic || EAntimagic || (uarmc && uarmc->oartifact == ART_ALL_IN_ONE_ASCENSION_KIT) || (uarmh && uarmh->oartifact == ART_YOU_ARE_ALREADY_DEAD) || (uarmg && uarmg->oartifact == ART_SPECTRATOR && (moves % 5 == 0) ) || (uarm && uarm->oartifact == ART_WEB_OF_LOLTH) || (uarm && uarm->oartifact == ART_ROFLCOPTER_WEB) || u.antimagicshell || Role_if(PM_UNBELIEVER) || have_magicresstone() || \
				 (Upolyd && resists_magm(&youmonst))) && !NoAntimagic_resistance && !Race_if(PM_UNMAGIC_FISH))
#define NoAntimagic_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_ANTIMAGIC].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HAcid_resistance	u.uprops[ACID_RES].intrinsic
#define EAcid_resistance	u.uprops[ACID_RES].extrinsic
#define Acid_resistance		((HAcid_resistance || EAcid_resistance || (uwep && uwep->oartifact == ART_ACTA_METALLURGICA_VOL___) || (uarmc && uarmc->oartifact == ART_ACIDSHOCK_CASTLECRUSHER) || (uarmg && uarmg->oartifact == ART_SPECTRATOR && (moves % 5 == 0) ) || (uarmh && uarmh->oartifact == ART_MASSIVE_IRON_CROWN_OF_MORG) || (uarmh && uarmh->oartifact == ART_WAR_MASK_OF_DURIN) || resists_acid(&youmonst) || is_acid_resistant(youmonst.data)) && !NoAcid_resistance)
#define NoAcid_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_ACID_RES].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HStone_resistance	u.uprops[STONE_RES].intrinsic
#define EStone_resistance	u.uprops[STONE_RES].extrinsic
#define Stone_resistance	((HStone_resistance || EStone_resistance || (uarmh && uarmh->oartifact == ART_STONE_EROSION) || (uarmg && uarmg->oartifact == ART_SIGNONS_STEEL_TOTAL) || (uwep && uwep->oartifact == ART_GILRAEN_SEREGON) || (uarmf && uarmf->oartifact == ART_NATALIA_S_PUNISHER) || (uarmg && uarmg->oartifact == ART_COCKUETRY) || (uarmg && uarmg->oartifact == ART_SPECTRATOR && (moves % 5 == 0) ) || (uarmf && uarmf->oartifact == ART_CORINA_S_UNFAIR_SCRATCHER) || (uarmf && uarmf->oartifact == ART_YET_ANOTHER_STUPID_IDEA) || (uamul && uamul->oartifact == ART_PRECIOUS_UNOBTAINABLE_PROP) || (uamul && uamul->oartifact == ART_ONE_MOMENT_IN_TIME) || (uamul && uamul->oartifact == ART_BALLSY_BASTARD) || resists_ston(&youmonst) || is_ston_resistant(youmonst.data) || Role_if(PM_ROCKER) ) && !NoStone_resistance)
#define NoStone_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_STONE_RES].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HFear_resistance	u.uprops[FEAR_RES].intrinsic
#define EFear_resistance	u.uprops[FEAR_RES].extrinsic
#define Fear_resistance		((HFear_resistance || EFear_resistance || (uarmf && uarmf->oartifact == ART_GRANDPA_S_BROGUES) || (uarmf && uarmf->oartifact == ART_PARANOIA_STRIDE) ) || (uwep && uwep->oartifact == ART_USELESSNESS_OF_PLENTY) && !NoFear_resistance)
#define NoFear_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_FEAR_RES].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))


/*** Troubles ***/
/* Pseudo-property */
#define Punished		(uball)

/* Those implemented solely as timeouts (we use just intrinsic) */
#define HStun			u.uprops[STUNNED].intrinsic
#define EStun			u.uprops[STUNNED].extrinsic
#define Stunned			(HStun || EStun || (uarms && uarms->oartifact == ART_VENOMAT) || (uwep && uwep->oartifact == ART_LANCE_OF_LONGINUS) || (uwep && uwep->oartifact == ART_SCHWANZUS_LANGUS) || (uarm && uarm->oartifact == ART_ROBE_OF_THE_ARCHMAGI) || (uarm && uarm->oartifact == ART_SHIVANHUNTER_S_UNUSED_PRIZ) || HeavyStunned || ((u.umonnum == PM_STALKER || youmonst.data->mlet == S_BAT) && !Race_if(PM_TRANSFORMER) ) || Race_if(PM_TUMBLRER) )
		/* Note: birds will also be stunned */
#define HeavyStunned		u.uprops[HEAVY_STUNNED].intrinsic

#define HConfusion		u.uprops[CONFUSION].intrinsic
#define EConfusion		u.uprops[CONFUSION].extrinsic
#define Confusion		(u.boomshined || HConfusion || EConfusion || HeavyConfusion || (uamul && uamul->oartifact == ART_CONFUSTICATOR) || (uarmh && uarmh->oartifact == ART_NEVER_CLEAN) || (uwep && uwep->oartifact == ART_EAMANE_LUINWE) || (uarmc && uarmc->oartifact == ART_VEIL_OF_LATONA) || (uarmh && uarmh->oartifact == ART_UNIMPLEMENTED_FEATURE) || (uwep && uwep->oartifact == ART_FEANARO_SINGOLLO) || (uarms && uarms->oartifact == ART_CCC_CCC_CCCCCCC) || (uarmc && uarmc->oartifact == ART_VEIL_OF_MINISTRY)|| (uwep && uwep->oartifact == ART_LANCE_OF_LONGINUS) || (uwep && uwep->oartifact == ART_SCHWANZUS_LANGUS) || u.uprops[SENSORY_DEPRIVATION].extrinsic || Race_if(PM_ADDICT))
#define HeavyConfusion		u.uprops[HEAVY_CONFUSION].intrinsic

#define HNumbed		u.uprops[NUMBED].intrinsic
#define ENumbed		u.uprops[NUMBED].extrinsic
#define Numbed		(HNumbed || ENumbed || (uarms && uarms->oartifact == ART_NUMBED_CAN_T_DO) || HeavyNumbed)
#define HeavyNumbed		u.uprops[HEAVY_NUMBED].intrinsic

#define HFeared		u.uprops[FEARED].intrinsic
#define EFeared		u.uprops[FEARED].extrinsic
#define Feared		(flags.dudley || ((HFeared || EFeared || HeavyFeared) && !Fear_resistance) )
#define HeavyFeared		u.uprops[HEAVY_FEARED].intrinsic

#define HFrozen		u.uprops[FROZEN].intrinsic
#define EFrozen		u.uprops[FROZEN].extrinsic
#define Frozen		(HFrozen || EFrozen || (uarmc && uarmc->oartifact == ART_BROKEN_WINGS) || (uarmc && uarmc->oartifact == ART_GROUNDBUMMER) || (uarms && uarms->oartifact == ART_TYPE_OF_ICE_BLOCK_HATES_YO) || (uwep && uwep->oartifact == ART_LANCE_OF_LONGINUS) || (uwep && uwep->oartifact == ART_SCHWANZUS_LANGUS) || HeavyFrozen)
#define HeavyFrozen		u.uprops[HEAVY_FROZEN].intrinsic

#define HBurned		u.uprops[BURNED].intrinsic
#define EBurned		u.uprops[BURNED].extrinsic
#define Burned		(HBurned || EBurned || HeavyBurned || Race_if(PM_BURNINATOR) || (uarms && uarms->oartifact == ART_BURNING_DISK) )
#define HeavyBurned		u.uprops[HEAVY_BURNED].intrinsic

#define Blinded			u.uprops[BLINDED].intrinsic
#define EBlinded			u.uprops[BLINDED].extrinsic
#define Blindfolded		(ublindf && ublindf->otyp != LENSES && ublindf->otyp != CONDOME)
		/* ...means blind because of a cover */
#define Blind	((Blinded || EBlinded || Blindfolded || HeavyBlind || u.uprops[SENSORY_DEPRIVATION].extrinsic || flags.blindfox || (!haseyes(youmonst.data) && !Race_if(PM_TRANSFORMER) ) ) && \
		 !(ublindf && ublindf->oartifact == ART_EYES_OF_THE_OVERWORLD && !flags.blindfox ) && !(Race_if(PM_PLAYER_DOLGSMAN) && !flags.blindfox) && !(uwep && uwep->oartifact == ART_DUMBOAK_S_HEW) && !(uwep && uwep->oartifact == ART_DIMOAK_S_HEW && !flags.blindfox ) && !(uarmh && uarmh->oartifact == ART_BLINDING_FOG && !flags.blindfox ) && !(uarms && uarms->oartifact == ART_SHADOWDISK && !flags.blindfox ) )
		/* ...the Eyes operate even when you really are blind
		    or don't have any eyes */
/* added possibility of playing the entire game blind --Amy*/
#define HeavyBlind		u.uprops[HEAVY_BLIND].intrinsic

#define Sick			u.uprops[SICK].intrinsic
#define Stoned			u.uprops[STONED].intrinsic
#define Strangled		u.uprops[STRANGLED].intrinsic
#define Prem_death		(u.uprops[PREMDEATH].intrinsic || u.uprops[PREMDEATH].extrinsic)
#define Vomiting		u.uprops[VOMITING].intrinsic
#define Glib			u.uprops[GLIB].intrinsic
#define EGlib			u.uprops[GLIB].extrinsic
#define IsGlib			(Glib || EGlib || (uarmg && uarmg->oartifact == ART_TANKS_A_LOT) )
#define Slimed			u.uprops[SLIMED].intrinsic	/* [Tom] */

/* nasty traps with timeouts --Amy */
#define SpeedBug		u.uprops[SPEED_BUG].intrinsic
#define MenuBug		u.uprops[MENU_LOST].intrinsic
#define RMBLoss		u.uprops[RMB_LOST].intrinsic
#define DisplayLoss	u.uprops[DISPLAY_LOST].intrinsic
#define SpellLoss		u.uprops[SPELLS_LOST].intrinsic
#define YellowSpells		u.uprops[YELLOW_SPELLS].intrinsic
#define AutoDestruct		u.uprops[AUTO_DESTRUCT].intrinsic
#define MemoryLoss		u.uprops[MEMORY_LOST].intrinsic
#define InventoryLoss		u.uprops[INVENTORY_LOST].intrinsic
#define BlackNgWalls		u.uprops[BLACK_NG_WALLS].intrinsic
#define Superscroller		u.uprops[SUPERSCROLLER_ACTIVE].intrinsic

#define FreeHandLoss		u.uprops[FREE_HAND_LOST].intrinsic
#define Unidentify		u.uprops[UNIDENTIFY].intrinsic
#define Thirst		u.uprops[THIRST].intrinsic
#define LuckLoss		u.uprops[LUCK_LOSS].intrinsic
#define ShadesOfGrey 		u.uprops[SHADES_OF_GREY].intrinsic
#define IncreasedGravity		u.uprops[INCREASED_GRAVITY].intrinsic
#define FaintActive		u.uprops[FAINT_ACTIVE].intrinsic
#define Itemcursing		u.uprops[ITEMCURSING].intrinsic
#define DifficultyIncreased		u.uprops[DIFFICULTY_INCREASED].intrinsic
#define Deafness		u.uprops[DEAFNESS].intrinsic
#define CasterProblem		u.uprops[CASTER_PROBLEM].intrinsic
#define WeaknessProblem		u.uprops[WEAKNESS_PROBLEM].intrinsic
#define RotThirteen		u.uprops[ROT_THIRTEEN].intrinsic
#define BishopGridbug		u.uprops[BISHOP_GRIDBUG].intrinsic
#define ConfusionProblem		u.uprops[CONFUSION_PROBLEM].intrinsic
#define NoDropProblem		u.uprops[DROP_BUG].intrinsic
#define DSTWProblem		u.uprops[DSTW_BUG].intrinsic
#define StatusTrapProblem		u.uprops[STATUS_FAILURE].intrinsic
#define AlignmentProblem		u.uprops[ALIGNMENT_FAILURE].intrinsic
#define StairsProblem		u.uprops[STAIRSTRAP].intrinsic
#define UninformationProblem		u.uprops[UNINFORMATION].intrinsic

#define IntrinsicLossProblem		u.uprops[INTRINSIC_LOSS].intrinsic
#define BloodLossProblem		u.uprops[BLOOD_LOSS].intrinsic
#define BadEffectProblem		u.uprops[BAD_EFFECTS].intrinsic
#define TrapCreationProblem		u.uprops[AUTOMATIC_TRAP_CREATION].intrinsic
#define AutomaticVulnerabilitiy		u.uprops[REPEATING_VULNERABILITY].intrinsic
#define TeleportingItems		u.uprops[TELEPORTING_ITEMS].intrinsic

#define NastinessProblem		u.uprops[NASTINESS_EFFECTS].intrinsic

#define RecurringAmnesia		u.uprops[RECURRING_AMNESIA].intrinsic
#define BigscriptEffect		u.uprops[BIGSCRIPT].intrinsic
#define BankTrapEffect		u.uprops[BANKBUG].intrinsic
#define MapTrapEffect		u.uprops[MAPBUG].intrinsic
#define TechTrapEffect		u.uprops[TECHBUG].intrinsic
#define RecurringDisenchant		u.uprops[RECURRING_DISENCHANT].intrinsic
#define verisiertEffect		u.uprops[VERISIERTEFFECT].intrinsic
#define ChaosTerrain		u.uprops[CHAOS_TERRAIN].intrinsic
#define Muteness		u.uprops[MUTENESS].intrinsic
#define EngravingDoesntWork		u.uprops[ENGRAVINGBUG].intrinsic
#define MagicDeviceEffect		u.uprops[MAGIC_DEVICE_BUG].intrinsic
#define BookTrapEffect		u.uprops[BOOKBUG].intrinsic
#define LevelTrapEffect		u.uprops[LEVELBUG].intrinsic
#define QuizTrapEffect		u.uprops[QUIZZES].intrinsic

#define CaptchaProblem		u.uprops[CAPTCHA].intrinsic
#define FarlookProblem		u.uprops[FARLOOK_BUG].intrinsic
#define RespawnProblem		u.uprops[RESPAWN_BUG].intrinsic
#define MCReduction		u.uprops[MC_REDUCTION].intrinsic

#define NoStaircase		u.uprops[NO_STAIRCASE].intrinsic

#define FastMetabolismEffect		u.uprops[FAST_METABOLISM].intrinsic
#define NoReturnEffect		u.uprops[NORETURN].intrinsic
#define AlwaysEgotypeMonsters		u.uprops[ALWAYS_EGOTYPES].intrinsic
#define TimeGoesByFaster		u.uprops[FAST_FORWARD].intrinsic
#define FoodIsAlwaysRotten		u.uprops[FOOD_IS_ROTTEN].intrinsic
#define AllSkillsUnskilled		u.uprops[SKILL_DEACTIVATED].intrinsic
#define AllStatsAreLower		u.uprops[STATS_LOWERED].intrinsic
#define PlayerCannotTrainSkills		u.uprops[TRAINING_DEACTIVATED].intrinsic
#define PlayerCannotExerciseStats		u.uprops[EXERCISE_DEACTIVATED].intrinsic

#define TurnLimitation		u.uprops[TURNLIMITATION].intrinsic
#define WeakSight		u.uprops[WEAKSIGHT].intrinsic
#define RandomMessages		u.uprops[RANDOM_MESSAGES].intrinsic

#define Desecration		u.uprops[DESECRATION].intrinsic
#define StarvationEffect		u.uprops[STARVATION_EFFECT].intrinsic
#define NoDropsEffect		u.uprops[NO_DROPS_EFFECT].intrinsic
#define LowEffects		u.uprops[LOW_EFFECTS].intrinsic
#define InvisibleTrapsEffect		u.uprops[INVIS_TRAPS_EFFECT].intrinsic
#define GhostWorld		u.uprops[GHOST_WORLD].intrinsic
#define Dehydration		u.uprops[DEHYDRATION].intrinsic
#define HateTrapEffect		u.uprops[HATE_TRAP_EFFECT].intrinsic
#define TotterTrapEffect		u.uprops[TOTTER_EFFECT].intrinsic
#define Nonintrinsics		u.uprops[NONINTRINSIC_EFFECT].intrinsic
#define Dropcurses		u.uprops[DROPCURSES_EFFECT].intrinsic
#define Nakedness		u.uprops[NAKEDNESS].intrinsic
#define Antileveling		u.uprops[ANTILEVELING].intrinsic
#define ItemStealingEffect		u.uprops[ITEM_STEALING_EFFECT].intrinsic
#define Rebellions		u.uprops[REBELLION_EFFECT].intrinsic
#define CrapEffect		u.uprops[CRAP_EFFECT].intrinsic
#define ProjectilesMisfire		u.uprops[PROJECTILES_MISFIRE].intrinsic
#define WallTrapping		u.uprops[WALL_TRAP_EFFECT].intrinsic

#define TimeStopped		u.uprops[TIME_STOPPED].intrinsic

/* Hallucination is solely a timeout; its resistance is extrinsic */
#define HHallucination		u.uprops[HALLUC].intrinsic
#define EHallucination		u.uprops[HALLUC].extrinsic
#define EHalluc_resistance	u.uprops[HALLUC_RES].extrinsic
#define Halluc_resistance	((EHalluc_resistance || \
				 (Upolyd && dmgtype(youmonst.data, AD_HALU)) || (Role_if(PM_TRANSSYLVANIAN) && uwep && (uwep->otyp == WEDGED_LITTLE_GIRL_SANDAL || uwep->otyp == SOFT_GIRL_SNEAKER || uwep->otyp == STURDY_PLATEAU_BOOT_FOR_GIRLS || uwep->otyp == HUGGING_BOOT || uwep->otyp == BLOCK_HEELED_COMBAT_BOOT || uwep->otyp == WOODEN_GETA || uwep->otyp == LACQUERED_DANCING_SHOE || uwep->otyp == HIGH_HEELED_SANDAL || uwep->otyp == SEXY_LEATHER_PUMP || uwep->otyp == SPIKED_BATTLE_BOOT) ) ) && !u.halresdeactivated)
#define Hallucination		((HHallucination && !Halluc_resistance) || (u.uprops[MULTISHOES].extrinsic && !Halluc_resistance) || (uwep && uwep->oartifact == ART_LANCE_OF_LONGINUS) || (uwep && uwep->oartifact == ART_SCHWANZUS_LANGUS) || (HeavyHallu && !Halluc_resistance) || (EHallucination && !Halluc_resistance) || u.uprops[SENSORY_DEPRIVATION].extrinsic || flags.hippie || ( (u.uprops[DEHYDRATION].extrinsic || Dehydration || have_dehydratingstone()) && ((u.dehydrationtime - moves) < 1) )  )
/* added possibility of playing the entire game hallucinating --Amy*/
#define HeavyHallu		u.uprops[HEAVY_HALLU].intrinsic

/* Timeout, plus a worn mask */
#define HFumbling		u.uprops[FUMBLING].intrinsic
#define EFumbling		u.uprops[FUMBLING].extrinsic
#define Fumbling		(HFumbling || EFumbling || u.uprops[MULTISHOES].extrinsic)

#define HWounded_legs		u.uprops[WOUNDED_LEGS].intrinsic
#define EWounded_legs		u.uprops[WOUNDED_LEGS].extrinsic
#define Wounded_legs		((HWounded_legs || EWounded_legs || (uarmf && uarmf->oartifact == ART_CINDERELLA_S_SLIPPERS) || (uarmf && uarmf->oartifact == ART_UNTRAINED_HALF_MARATHON) || u.uprops[MULTISHOES].extrinsic) && !(uarmf && uarmf->oartifact == ART_ELIANE_S_SHIN_SMASH) )

#define HSleeping		u.uprops[SLEEPING].intrinsic
#define ESleeping		u.uprops[SLEEPING].extrinsic
#define Sleeping		(HSleeping || ESleeping || Race_if(PM_KOBOLT))

#define HHunger			u.uprops[HUNGER].intrinsic
#define EHunger			u.uprops[HUNGER].extrinsic
#define Hunger			(HHunger || EHunger || (uarmc && uarmc->oartifact == ART_INA_S_LAB_COAT) || (uarmc && uarmc->oartifact == ART_INA_S_OVERCOAT) || (uleft && uleft->oartifact == ART_RING_OF_WOE) || (uarmh && uarmh->oartifact == ART_DRINK_COCA_COLA) || (uarmh && uarmh->oartifact == ART_HAMBURG_ONE) || (uarmc && uarmc->oartifact == ART_FULLY_LIONIZED) || (uarmc && uarmc->oartifact == ART_FAST_CAMO_PREDATOR) || (uright && uright->oartifact == ART_RING_OF_WOE) || (uwep && uwep->oartifact == ART_PICK_OF_THE_GRAVE) || Race_if(PM_GIGANT))


/*** Vision and senses ***/
#define HSee_invisible		u.uprops[SEE_INVIS].intrinsic
#define ESee_invisible		u.uprops[SEE_INVIS].extrinsic
#define See_invisible		((HSee_invisible || ESee_invisible || (uarmc && uarmc->oartifact == ART_VISIBLE_INVISIBILITITY) || (uarmc && uarmc->oartifact == ART_CAN_T_KILL_WHAT_YOU_CAN_T_) || (uwep && uwep->oartifact == ART_SECRETS_OF_INVISIBLE_PLEAS) || \
				 perceives(youmonst.data)) && !Race_if(PM_ELEMENTAL) && !NoSee_invisible)
#define NoSee_invisible	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SEE_INVIS].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HTelepat		u.uprops[TELEPAT].intrinsic
#define ETelepat		u.uprops[TELEPAT].extrinsic

#define Blind_telepat		( ((HTelepat || ETelepat || Race_if(PM_DEVELOPER) || telepathic(youmonst.data)) && !NoTelepat && (!Role_if(PM_TOPMODEL) || !flags.female)) || ( Role_if(PM_TOPMODEL) && !u.powerfailure && !NoTelepat && PlayerInHighHeels ))

#define Unblind_telepat		((ETelepat || Race_if(PM_DEVELOPER)) && !NoTelepat)
#define NoTelepat	( (!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_TELEPAT].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0))) || (uarmc && uarmc->oartifact == ART_GODLESS_VOID) || (uarmh && uarmh->oartifact == ART_WOLF_KING) || (uarmh && ( (uarmh->otyp == TINFOIL_HELMET) || (OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "aluminium helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "shlem iz alyuminiya") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "alyuminiy dubulg'a") ) ) ) ) )

#define HHallu_party	u.uprops[HALLU_PARTY].intrinsic
#define Hallu_party	(HHallu_party && !NoHallu_party)
#define NoHallu_party	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_HALLU_PARTY].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HDrunken_boxing		u.uprops[DRUNKEN_BOXING].intrinsic
#define Drunken_boxing		(HDrunken_boxing && !NoDrunken_boxing)
#define NoDrunken_boxing	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_DRUNKEN_BOXING].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HStunnopathy	u.uprops[STUNNOPATHY].intrinsic
#define Stunnopathy	(HStunnopathy && !NoStunnopathy)
#define NoStunnopathy	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_STUNNOPATHY].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HNumbopathy	u.uprops[NUMBOPATHY].intrinsic
#define Numbopathy	(HNumbopathy && !NoNumbopathy)
#define NoNumbopathy	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_NUMBOPATHY].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HFreezopathy	u.uprops[FREEZOPATHY].intrinsic
#define Freezopathy	(HFreezopathy && !NoFreezopathy)
#define NoFreezopathy	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_FREEZOPATHY].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HStoned_chiller		u.uprops[STONED_CHILLER].intrinsic
#define Stoned_chiller		(HStoned_chiller && !NoStoned_chiller)
#define NoStoned_chiller	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_STONED_CHILLER].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HCorrosivity	u.uprops[CORROSIVITY].intrinsic
#define Corrosivity	(HCorrosivity && !NoCorrosivity)
#define NoCorrosivity	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_CORROSIVITY].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HFear_factor	u.uprops[FEAR_FACTOR].intrinsic
#define Fear_factor	(HFear_factor && !NoFear_factor)
#define NoFear_factor	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_FEAR_FACTOR].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HBurnopathy	u.uprops[BURNOPATHY].intrinsic
#define Burnopathy	(HBurnopathy && !NoBurnopathy)
#define NoBurnopathy	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_BURNOPATHY].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HSickopathy	u.uprops[SICKOPATHY].intrinsic
#define Sickopathy	(HSickopathy && !NoSickopathy)
#define NoSickopathy	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SICKOPATHY].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HWonderlegs	u.uprops[WONDERLEGS].intrinsic
#define Wonderlegs	(HWonderlegs && !NoWonderlegs)
#define NoWonderlegs	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_WONDERLEGS].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HGlib_combat	u.uprops[GLIB_COMBAT].intrinsic
#define Glib_combat	(HGlib_combat && !NoGlib_combat)
#define NoGlib_combat	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_GLIB_COMBAT].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HManaleech	u.uprops[MANALEECH].intrinsic
#define EManaleech	u.uprops[MANALEECH].extrinsic
#define Manaleech		((HManaleech || EManaleech || (uarmu && uarmu->oartifact == ART_VICTORIA_IS_EVIL_BUT_PRETT) || (uarmu && uarmu->oartifact == ART_NATALIA_IS_LOVELY_BUT_DANG) || (uarmf && uarmf->oartifact == ART_SOLVEJG_S_STINKING_SLIPPER) || (uarm && flags.female && uarm->oartifact == ART_PRETTY_LITTLE_MAGICAL_GIRL) || (uarmh && uarmh->oartifact == ART_REFUEL_BADLY) || (uamul && uamul->oartifact == ART_BALLSY_BASTARD) || (uamul && uamul->oartifact == ART_MADMAN_S_POWER) || (uamul && uamul->oartifact == ART___TH_NAZGUL) || (uamul && uamul->oartifact == ART_NAZGUL_S_REVENGE) ) && !NoManaleech)
#define NoManaleech	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_MANALEECH].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HMap_amnesia	u.uprops[MAP_AMNESIA].intrinsic
#define EMap_amnesia	u.uprops[MAP_AMNESIA].extrinsic
#define Map_amnesia	(HMap_amnesia || EMap_amnesia)

#define HWarning		u.uprops[WARNING].intrinsic
#define EWarning		u.uprops[WARNING].extrinsic
#define Warning			(( (HWarning || EWarning) && !u.powerfailure && !NoWarning && (!Role_if(PM_TOPMODEL) || !flags.female)) || ( Role_if(PM_TOPMODEL) && !NoWarning && PlayerInHighHeels ))
#define NoWarning	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_WARNING].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

/* Warning for a specific type of monster */
#define HWarn_of_mon		u.uprops[WARN_OF_MON].intrinsic
#define EWarn_of_mon		u.uprops[WARN_OF_MON].extrinsic
#define Warn_of_mon		(HWarn_of_mon || EWarn_of_mon)

#define HUndead_warning		u.uprops[WARN_UNDEAD].intrinsic
#define EUndead_warning		u.uprops[WARN_UNDEAD].extrinsic
#define Undead_warning		(HUndead_warning || EUndead_warning || (uarmh && uarmh->oartifact == ART_ALL_SEEING_EYE_OF_THE_FLY) || (uarmh && uarmh->oartifact == ART_FLUE_FLUE_FLUEFLUE_FLUE))

#define HSearching		u.uprops[SEARCHING].intrinsic
#define ESearching		u.uprops[SEARCHING].extrinsic
#define Searching		((HSearching || ESearching) && !NoSearching)
#define NoSearching	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SEARCHING].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HClairvoyant		u.uprops[CLAIRVOYANT].intrinsic
#define EClairvoyant		u.uprops[CLAIRVOYANT].extrinsic
#define BClairvoyant		u.uprops[CLAIRVOYANT].blocked
#define Clairvoyant		((HClairvoyant || EClairvoyant || (uarmh && uarmh->otyp == ART_WSCHIIIIIE_) ) &&\
				 !BClairvoyant && !NoClairvoyant)
#define NoClairvoyant	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_CLAIRVOYANT].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HInfravision		u.uprops[INFRAVISION].intrinsic
#define EInfravision		u.uprops[INFRAVISION].extrinsic
#define Infravision		((HInfravision || EInfravision || (uwep && uwep->oartifact == ART_DEMON_MACHINE) || \
				  infravision(youmonst.data)) && !u.powerfailure && !NoInfravision)
#define NoInfravision	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_INFRAVISION].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HDetect_monsters	u.uprops[DETECT_MONSTERS].intrinsic
#define EDetect_monsters	u.uprops[DETECT_MONSTERS].extrinsic
#define Detect_monsters		((HDetect_monsters || EDetect_monsters || u.uprops[STORM_HELM].extrinsic) && !u.powerfailure && !NoDetect_monsters)
#define NoDetect_monsters	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_DETECT_MONSTERS].intrinsic || (uarmh && uarmh->oartifact == ART_RADAR_NOT_WORKING) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))


/*** Appearance and behavior ***/
#define Adornment		u.uprops[ADORNED].extrinsic

#define HInvis			u.uprops[INVIS].intrinsic
#define EInvis			u.uprops[INVIS].extrinsic
#define BInvis			u.uprops[INVIS].blocked
#define Invis			((HInvis || EInvis || (uwep && uwep->oartifact == ART_MOVE_IN_THE_SHADOWS) || (uarmc && uarmc->oartifact == ART_INVISIBLE_VISIBILITITY) || (uarmh && uarmh->oartifact == ART_DULLIFIER) || (uarm && uarm->oartifact == ART_VERY_INVISIBLE) || (uarmc && uarmc->oartifact == ART_CAN_T_KILL_WHAT_YOU_CAN_T_) || (uwep && uwep->oartifact == ART_SECRETS_OF_INVISIBLE_PLEAS) || \
				 pm_invisible(youmonst.data)) && !BInvis && !Race_if(PM_ELEMENTAL) && !NoInvisible)
#define Invisible		(Invis && !See_invisible)
		/* Note: invisibility also hides inventory and steed */
#define NoInvisible	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_INVIS].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HDisplaced		u.uprops[DISPLACED].intrinsic
#define EDisplaced		u.uprops[DISPLACED].extrinsic
#define Displaced		( (EDisplaced || HDisplaced || (uarmc && uarmc->oartifact == ART_IMAGE_PROJECTOR) || (uwep && uwep->oartifact == ART_SKY_RENDER) || (uarm && uarm->oartifact == ART_VERY_INVISIBLE) || (uarmf && uarmf->oartifact == ART_WATER_FLOWERS) || (uwep && uwep->oartifact == ART_BELTHRONDING) || (uarm && uarm->oartifact == ART_ROBE_OF_THE_ARCHMAGI) || (uarm && uarm->oartifact == ART_SHIVANHUNTER_S_UNUSED_PRIZ) || (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "shrouded cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "okutana plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "kafan plash") ) && (moves % 10 == 0) ) || ((moves % 3 == 0) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "deep cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "glubokiy plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "chuqur plash") ) ) || (uarmf && OBJ_DESCR(objects[uarmf->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmf->otyp]), "korean sandals") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "koreyskiye sandalii") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "janubiy koreyaning kavushlari") ) && (moves % 3 == 0) ) ) && !NoDisplaced)
#define NoDisplaced	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_DISPLACED].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HStealth		u.uprops[STEALTH].intrinsic
#define EStealth		u.uprops[STEALTH].extrinsic
#define BStealth		u.uprops[STEALTH].blocked
#define Stealth			(( (HStealth || EStealth || u.uprops[MULTISHOES].extrinsic || (Race_if(PM_MOON_ELF) && ((flags.moonphase >= 1 && flags.moonphase <= 3) || (flags.moonphase >= 5 && flags.moonphase <= 7) ) ) ) && !BStealth && !NoStealth && !Race_if(PM_OGRO) && !(uwep && uwep->oartifact == ART_ARMORWREAKER) && !Race_if(PM_ROHIRRIM) && !Race_if(PM_THUNDERLORD) && (!Role_if(PM_TOPMODEL) || !flags.female) ) || ( Role_if(PM_TOPMODEL) && !NoStealth && PlayerInHighHeels ))
#define NoStealth	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_STEALTH].intrinsic || (uarmf && uarmf->oartifact == ART_HENRIETTA_S_DOGSHIT_BOOTS) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HExtra_wpn_practice	u.uprops[EXTRA_WEAPON_PRACTICE].intrinsic
#define EExtra_wpn_practice	u.uprops[EXTRA_WEAPON_PRACTICE].extrinsic
#define Extra_wpn_practice	(HExtra_wpn_practice || EExtra_wpn_practice || (uarmc && uarmc->oartifact == ART_SKILLS_BEAT_STATS) )

#define HDeath_resistance	u.uprops[DTBEEM_RES].intrinsic
#define EDeath_resistance	u.uprops[DTBEEM_RES].extrinsic
#define Death_resistance	(HDeath_resistance || EDeath_resistance)

/* according to Yasdorian, I love aggravate monster. Indeed, many of my artifacts have it. --Amy */
#define HAggravate_monster	u.uprops[AGGRAVATE_MONSTER].intrinsic
#define EAggravate_monster	u.uprops[AGGRAVATE_MONSTER].extrinsic
#define Aggravate_monster	(HAggravate_monster || EAggravate_monster || Race_if(PM_OGRO) || Race_if(PM_ROHIRRIM) || Race_if(PM_THUNDERLORD) || (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "avenger cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "mstitel' plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "qasoskor plash") )) || (uwep && uwep->oartifact == ART_ASIAN_WINTER) || (uwep && uwep->oartifact == ART_FN_M____PARA) || (uwep && uwep->oartifact == ART_PICK_OF_THE_GRAVE) || (uwep && uwep->oartifact == ART_CRUEL_PUNISHER) || (uwep && uwep->oartifact == ART_SANDRA_S_EVIL_MINDDRILL) || (uarmu && uarmu->oartifact == ART_HELEN_S_DISCARDED_SHIRT) || (uarm && uarm->oartifact == ART_HO_OH_S_FEATHERS) || (uarm && uarm->oartifact == ART_QUEEN_ARTICUNO_S_HULL) || (uarm && uarm->oartifact == ART_DON_SUICUNE_DOES_NOT_APPRO) || (uarm && uarm->oartifact == ART_DON_SUICUNE_USED_SELFDESTR) || (uarmc && uarmc->oartifact == ART_BROKEN_WINGS) || (uarmc && uarmc->oartifact == ART_GROUNDBUMMER) || (uwep && uwep->oartifact == ART_SANDRA_S_SECRET_WEAPON) || (uarmf && uarmf->oartifact == ART_CORINA_S_SNOWY_TREAD) || (uarmc && uarmc->oartifact == ART_UBERJACKAL_EFFECT) || (uarmc && uarmc->oartifact == ART_HENRIETTA_S_HEAVY_CASTER) || (uarmf && uarmf->oartifact == ART_HENRIETTA_S_DOGSHIT_BOOTS) || (uarmh && uarmh->oartifact == ART_SEVERE_AGGRAVATION) || (uarms && uarms->oartifact == ART_SHATTERED_DREAMS) || (uarms && uarms->oartifact == ART_NO_FUTURE_BUT_AGONY) || (uarmf && uarmf->oartifact == ART_CINDERELLA_S_SLIPPERS) || (uwep && uwep->oartifact == ART_GIRLFUL_BONKING) || (uarmf && uarmf->oartifact == ART_ELLA_S_BLOODLUST) || (uarmf && uarmf->oartifact == ART_LOVELY_GIRL_PLATEAUS) || (uarmf && uarmf->oartifact == ART_SOLVEJG_S_STINKING_SLIPPER) || (uwep && uwep->oartifact == ART_RATTATTATTATTATT) || (uarmf && uarmf->oartifact == ART_I_M_A_BITCH__DEAL_WITH_IT) || (uarmf && uarmf->oartifact == ART_MANUELA_S_TORTURE_HEELS) || (uarmf && uarmf->oartifact == ART_MANUELA_S_UNKNOWN_HEELS) || (uarmf && uarmf->oartifact == ART_ALLYNONE) || (uarmf && uarmf->oartifact == ART_HERMES__UNFAIRNESS) || (uarmf && uarmf->oartifact == ART_HADES_THE_MEANIE) || (uarmf && uarmf->oartifact == ART_FUN_ALL_IN_ONE) || (uarmf && uarmf->oartifact == ART_PORCELAIN_ELEPHANT) || (uleft && uleft->oartifact == ART_RING_OF_WOE) || (uright && uright->oartifact == ART_RING_OF_WOE) || (uleft && uleft->oartifact == ART_GOLDENIVY_S_RAGE) || (uright && uright->oartifact == ART_GOLDENIVY_S_RAGE) || (uleft && uleft->oartifact == ART_GOLDENIVY_S_ENGAGEMENT_RIN) || (uright && uright->oartifact == ART_GOLDENIVY_S_ENGAGEMENT_RIN) || (uamul && uamul->oartifact == ART_SNOREFEST) || (uwep && uwep->oartifact == ART_ARMORWREAKER) || (uwep && uwep->oartifact == ART_HARKENSTONE) || (uwep && uwep->oartifact == ART_KUSANAGI_NO_TSURUGI) || (uwep && uwep->oartifact == ART_ARABELLA_S_ARTIFACT_CREATI) || (Role_if(PM_PICKPOCKET) && u.ualign.record < 0) )

#define HConflict		u.uprops[CONFLICT].intrinsic
#define EConflict		u.uprops[CONFLICT].extrinsic
#define Conflict		((HConflict || EConflict || (uarm && uarm->oartifact == ART_HO_OH_S_FEATHERS) || (uarms && uarms->oartifact == ART_SHATTERED_DREAMS) || (uarms && uarms->oartifact == ART_NO_FUTURE_BUT_AGONY) || (uarms && uarms->oartifact == ART_SYSTEMATIC_CHAOS) || (uarms && uarms->oartifact == ART_BONUS_HOLD) || (uarmf && uarmf->oartifact == ART_MANUELA_S_TORTURE_HEELS) || (uarmf && uarmf->oartifact == ART_WEAK_FROM_HUNGER) || (uarmf && uarmf->oartifact == ART_MANUELA_S_UNKNOWN_HEELS) || (uarmc && uarmc->oartifact == ART_NOW_IT_S_FOR_REAL) || (uarmf && uarmf->oartifact == ART_ALLYNONE) || (uarmh && uarmh->oartifact == ART_CERTAIN_SLOW_DEATH) || (uarm && uarm->oartifact == ART_QUEEN_ARTICUNO_S_HULL) || (uwep && uwep->oartifact == ART_PROVOCATEUR) || (uwep && uwep->oartifact == ART_CIVIL_WAR) || (uarmf && uarmf->oartifact == ART_FATALITY) || (uarmf && uarmf->oartifact == ART_FUN_ALL_IN_ONE) || (uleft && uleft->oartifact == ART_RING_OF_WOE) || (uright && uright->oartifact == ART_RING_OF_WOE) ) && !Is_blackmarket(&u.uz) )
/* Sorry guys and girls, but you need to find another way to clear out Sam's assistants. --Amy */

/*** Transportation ***/
#define HJumping		u.uprops[JUMPING].intrinsic
#define EJumping		u.uprops[JUMPING].extrinsic
#define Jumping			((HJumping || EJumping || (uarmc && uarmc->oartifact == ART_ROKKO_CHAN_S_SUIT) || (uarm && uarm->oartifact == ART_LUKE_S_JEDI_POWER) || (uarmf && uarmf->oartifact == ART_PRINCE_OF_PERSIA) || (uwep && uwep->oartifact == ART_SUPERGIRL_S_JUMP_AND_RUN_F) || (uamul && uamul->oartifact == ART_ONLY_ONE_ESCAPE) || flags.iwbtg || u.uprops[MULTISHOES].extrinsic ) && !NoJumping)
#define NoJumping	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_JUMPING].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HTeleportation		u.uprops[TELEPORT].intrinsic
#define ETeleportation		u.uprops[TELEPORT].extrinsic
#define Teleportation		(HTeleportation || ETeleportation || (uarmh && uarmh->oartifact == ART_GO_OTHER_PLACE) || (uarmg && uarmg->oartifact == ART_OUT_OF_CONTROL) || (uarmh && uarmh->oartifact == ART_DOUBLE_JEOPARDY) || (uarms && uarms->oartifact == ART_REFLECTOR_EJECTOR) || (uarmf && uarmf->oartifact == ART_ERROR_IN_PLAY_ENCHANTMENT) || (uarmh && uarmh->oartifact == ART_RANDOMNESS_PREVAILS) || (uleft && uleft->oartifact == ART_GOLDENIVY_S_RAGE) || (uright && uright->oartifact == ART_GOLDENIVY_S_RAGE) || (uleft && uleft->oartifact == ART_GOLDENIVY_S_ENGAGEMENT_RIN) || (uright && uright->oartifact == ART_GOLDENIVY_S_ENGAGEMENT_RIN) || (uleft && uleft->oartifact == ART_WARPCHANGE) || (uright && uright->oartifact == ART_WARPCHANGE) || (uamul && uamul->oartifact == ART_SPACE_CYCLE) ||  \
				 can_teleport(youmonst.data) || Race_if(PM_MAIA) || Race_if(PM_HUMANOID_LEPRECHAUN))

#define HTeleport_control	u.uprops[TELEPORT_CONTROL].intrinsic
#define ETeleport_control	u.uprops[TELEPORT_CONTROL].extrinsic
#define Teleport_control	((HTeleport_control || ETeleport_control || \
				 control_teleport(youmonst.data)) && !Race_if(PM_MAIA) && !(u.uprops[STORM_HELM].extrinsic) && !Race_if(PM_HUMANOID_LEPRECHAUN) && !NoTeleport_control)
#define NoTeleport_control	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_TELEPORT_CONTROL].intrinsic || (uarmc && uarmc->oartifact == ART_A_REASON_TO_LIVE) || (uarmh && uarmh->oartifact == ART_RANDOMNESS_PREVAILS) || (uarmg && uarmg->oartifact == ART_OUT_OF_CONTROL) || (uarmg && uarmg->oartifact == ART_ARABELLA_S_GREAT_BANISHER) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HLevitation		u.uprops[LEVITATION].intrinsic
#define ELevitation		u.uprops[LEVITATION].extrinsic
#define Levitation		(HLevitation || ELevitation || (uleft && uleft->oartifact == ART_FLOAT_EYELER_S_CONDITION) || (uright && uright->oartifact == ART_FLOAT_EYELER_S_CONDITION) || (uwep && uwep->oartifact == ART_IRON_BALL_OF_LEVITATION) || u.uprops[MULTISHOES].extrinsic || Race_if(PM_LEVITATOR) || \
				 (is_floater(youmonst.data) && !Race_if(PM_TRANSFORMER) ) )
	/* Can't touch surface, can't go under water; overrides all others */
#define Lev_at_will		(!Race_if(PM_LEVITATOR) && ((HLevitation & I_SPECIAL) != 0L || \
				 (ELevitation & W_ARTI) != 0L) && \
				 (HLevitation & ~(I_SPECIAL|TIMEOUT)) == 0L && \
				 (ELevitation & ~W_ARTI) == 0L && \
				 (!is_floater(youmonst.data) || Race_if(PM_TRANSFORMER) ) )

#define HFlying			u.uprops[FLYING].intrinsic
#define EFlying			u.uprops[FLYING].extrinsic
#ifdef STEED
# define Flying			(( (HFlying || EFlying || (uwep && uwep->oartifact == ART_PENGUIN_S_THRUSTING_SWORD) || (uarm && uarm->oartifact == ART_FLY_LIKE_AN_EAGLE) || (uarmc && uarmc->oartifact == ART_FULL_WINGS) || (uarmc && uarmc->oartifact == ART_DARK_ANGELS) || (uarmf && uarmf->oartifact == ART_MEPHISTO_S_BROGUES) || (uarmf && uarmf->oartifact == ART_CORINA_S_UNFAIR_SCRATCHER) || (uleft && uleft->oartifact == ART_GOLDENIVY_S_RAGE) || (uright && uright->oartifact == ART_GOLDENIVY_S_RAGE) || (uarmh && uarmh->oartifact == ART_GIRLFUL_FARTING_NOISES) || (uleft && uleft->oartifact == ART_GOLDENIVY_S_ENGAGEMENT_RIN) || (uarmc && uarmc->oartifact == ART_JANA_S_FAIRNESS_CUP) || (uright && uright->oartifact == ART_GOLDENIVY_S_ENGAGEMENT_RIN) || (uamul && uamul->oartifact == ART_STINGING_MEDALLION) || (uwep && uwep->oartifact == ART_SOURCE_CODES_OF_WORK_AVOID) || (uamul && uamul->oartifact == ART_DIKKIN_S_DRAGON_TEETH) || (uarmg && uarmg->oartifact == ART_EXPERTENGAME_THE_ENTIRE_LE) || (uarmg && uarmg->oartifact == ART_DIFFICULTY__) || (Race_if(PM_MOON_ELF) && flags.moonphase == FULL_MOON) || (uarmf && uarmf->oartifact == ART_GRENEUVENIA_S_HUG) || (uarmh && uarmh->oartifact == ART_FLUE_FLUE_FLUEFLUE_FLUE) || is_flyer(youmonst.data) || (is_floater(youmonst.data) && Race_if(PM_TRANSFORMER) ) || (u.usteed && is_flyer(u.usteed->data))) && !(uarm && uarm->oartifact == ART_WATER_SHYNESS) && !NoFlying && (!Role_if(PM_TOPMODEL) || !flags.female)) || ( Role_if(PM_TOPMODEL) && !(uarm && uarm->oartifact == ART_WATER_SHYNESS) && !NoFlying && PlayerInHighHeels ))
#define NoFlying	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_FLYING].intrinsic || (uarmc && uarmc->oartifact == ART_BROKEN_WINGS) || (uarmc && uarmc->oartifact == ART_GROUNDBUMMER) || (uarm && uarm->oartifact == ART_ARMOR_OF_EREBOR) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#else
# define Flying			(( (HFlying || EFlying || (uwep && uwep->oartifact == ART_PENGUIN_S_THRUSTING_SWORD) || (uarm && uarm->oartifact == ART_FLY_LIKE_AN_EAGLE) || (uarmc && uarmc->oartifact == ART_FULL_WINGS) || (uarmc && uarmc->oartifact == ART_DARK_ANGELS) || (uarmf && uarmf->oartifact == ART_MEPHISTO_S_BROGUES) || (uarmf && uarmf->oartifact == ART_CORINA_S_UNFAIR_SCRATCHER) || (uleft && uleft->oartifact == ART_GOLDENIVY_S_RAGE) || (uright && uright->oartifact == ART_GOLDENIVY_S_RAGE) || (uleft && uleft->oartifact == ART_GOLDENIVY_S_ENGAGEMENT_RIN) || (uright && uright->oartifact == ART_GOLDENIVY_S_ENGAGEMENT_RIN) || (uamul && uamul->oartifact == ART_STINGING_MEDALLION) || (uwep && uwep->oartifact == ART_SOURCE_CODES_OF_WORK_AVOID) || is_flyer(youmonst.data) || (is_floater(youmonst.data) && Race_if(PM_TRANSFORMER) ) ) && !(uarm && uarm->oartifact == ART_WATER_SHYNESS) && !NoFlying && (!Role_if(PM_TOPMODEL))) || !flags.female) || ( Role_if(PM_TOPMODEL) && !(uarm && uarm->oartifact == ART_WATER_SHYNESS) && !NoFlying && PlayerInHighHeels ))
#endif
	/* May touch surface; does not override any others */

#define Wwalking		( (u.uprops[WWALKING].extrinsic || u.uprops[MULTISHOES].extrinsic) && \
				 !Is_waterlevel(&u.uz))
	/* Don't get wet, can't go under water; overrides others except levitation */
	/* Wwalking is meaningless on water level */

#define HSwimming		u.uprops[SWIMMING].intrinsic
#define ESwimming		u.uprops[SWIMMING].extrinsic	/* [Tom] */
#ifdef STEED
# define Swimming		((HSwimming || ESwimming || (uarmc && uarmc->oartifact == ART_WATERFORCE____) || (uarmc && uarmc->oartifact == ART_LAURA_S_SWIMSUIT) || (uarmc && uarmc->oartifact == ART_WATERS_OF_OBLIVION) || (uarmf && uarmf->oartifact == ART_SANDRA_S_BEAUTIFUL_FOOTWEA) || (uarmu && uarmu->oartifact == ART_THERMAL_BATH) || \
				 is_swimmer(youmonst.data) || \
				 (u.usteed && is_swimmer(u.usteed->data))) && !(uarm && uarm->oartifact == ART_WATER_SHYNESS) )
#else
# define Swimming		((HSwimming || ESwimming || (uarmc && uarmc->oartifact == ART_WATERFORCE____) || (uarmc && uarmc->oartifact == ART_WATERS_OF_OBLIVION) || (uarmf && uarmf->oartifact == ART_SANDRA_S_BEAUTIFUL_FOOTWEA) || (uarmu && uarmu->oartifact == ART_THERMAL_BATH) || \
				 is_swimmer(youmonst.data)) && !(uarm && uarm->oartifact == ART_WATER_SHYNESS) )
#endif
	/* Get wet, don't go under water unless if amphibious */

#define HMagical_breathing	u.uprops[MAGICAL_BREATHING].intrinsic
#define EMagical_breathing	u.uprops[MAGICAL_BREATHING].extrinsic
#define Amphibious		((HMagical_breathing || EMagical_breathing || (Race_if(PM_MOON_ELF) && flags.moonphase == NEW_MOON) || \
				 amphibious(youmonst.data)) && !NoBreathless)
	/* Get wet, may go under surface */

#define Breathless		( ((HMagical_breathing || EMagical_breathing || (uarmh && uarmh->oartifact == ART_SHPX_GUVF_FUVG) || (Role_if(PM_ANACHRONIST) && uwep && uwep->oartifact == ART_ANACHRONONONONAUT_PACKAGE) || (uarmf && uarmf->oartifact == ART_HERMES__UNFAIRNESS) || (uarmf && uarmf->oartifact == ART_HADES_THE_MEANIE) || (uarmc && uarmc->oartifact == ART_WATERFORCE____) || (uarmc && uarmc->oartifact == ART_JANA_S_SECRET_CAR) || (uarmc && uarmc->oartifact == ART_JANA_S_GRAVE_WALL) || (uarmc && uarmc->oartifact == ART_LAURA_S_SWIMSUIT) || (uarmf && uarmf->oartifact == ART_SANDRA_S_BEAUTIFUL_FOOTWEA) || (uarmf && uarmf->oartifact == ART_FUN_ALL_IN_ONE) || (uarmf && uarmf->oartifact == ART_ALLYNONE) || breathless(youmonst.data)) && !NoBreathless && (!Role_if(PM_TOPMODEL) || !flags.female)) || ( Role_if(PM_TOPMODEL) && !NoBreathless && PlayerInHighHeels ))
#define NoBreathless	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_MAGICAL_BREATHING].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define Underwater		(u.uinwater)
/* Note that Underwater and u.uinwater are both used in code.
   The latter form is for later implementation of other in-water
   states, like swimming, wading, etc. */

#define HPasses_walls		u.uprops[PASSES_WALLS].intrinsic
#define EPasses_walls		u.uprops[PASSES_WALLS].extrinsic
#define Passes_walls		((HPasses_walls || EPasses_walls || \
				 passes_walls(youmonst.data)) && !NoPasses_walls)
# define Phasing            u.uprops[PASSES_WALLS].intrinsic
#define NoPasses_walls	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_PASSES_WALLS].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))


/*** Physical attributes ***/
#define HSlow_digestion		u.uprops[SLOW_DIGESTION].intrinsic
#define ESlow_digestion		u.uprops[SLOW_DIGESTION].extrinsic
#define Slow_digestion		(!Race_if(PM_GIGANT) && !NoSlow_digestion && (HSlow_digestion || ESlow_digestion || (uarmh && uarmh->oartifact == ART_NEVEREATER) || (uarmc && uarmc->oartifact == ART_FEMMY_FATALE) || (uarmh && uarmh->oartifact == ART_WOLF_KING) ) )  /* KMH */
#define NoSlow_digestion	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SLOW_DIGESTION].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

/* Half spell/physical damage only works 50% of the time, so it's more like three quarter spell/physical damage. --Amy */

#define HHalf_spell_damage	u.uprops[HALF_SPDAM].intrinsic
#define EHalf_spell_damage	u.uprops[HALF_SPDAM].extrinsic
#define Half_spell_damage	((HHalf_spell_damage || EHalf_spell_damage) && !NoHalf_spell_damage)
#define NoHalf_spell_damage	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_HALF_SPDAM].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HHalf_physical_damage	u.uprops[HALF_PHDAM].intrinsic
#define EHalf_physical_damage	u.uprops[HALF_PHDAM].extrinsic
#define Half_physical_damage	((HHalf_physical_damage || EHalf_physical_damage) && !NoHalf_physical_damage)
#define NoHalf_physical_damage	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_HALF_PHDAM].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define Second_chance		u.uprops[SECOND_CHANCE].extrinsic

#define HRegeneration		u.uprops[REGENERATION].intrinsic
#define ERegeneration		u.uprops[REGENERATION].extrinsic
#define Regeneration		((HRegeneration || ERegeneration || (uarmf && uarmf->oartifact == ART_ERROR_IN_PLAY_ENCHANTMENT) || \
				 regenerates(youmonst.data)) && !NoRegeneration && !Race_if(PM_SYLPH) )
#define NoRegeneration	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_REGENERATION].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HEnergy_regeneration	u.uprops[ENERGY_REGENERATION].intrinsic
#define EEnergy_regeneration	u.uprops[ENERGY_REGENERATION].extrinsic
#define Energy_regeneration	((HEnergy_regeneration || EEnergy_regeneration || Race_if(PM_REDGUARD) ) && !NoEnergy_regeneration && !Race_if(PM_SYLPH) )
#define NoEnergy_regeneration	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_ENERGY_REGENERATION].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HProtection		u.uprops[PROTECTION].intrinsic
#define EProtection		u.uprops[PROTECTION].extrinsic
#define Protection		(HProtection || EProtection)

#define HProtection_from_shape_changers \
				u.uprops[PROT_FROM_SHAPE_CHANGERS].intrinsic
#define EProtection_from_shape_changers \
				u.uprops[PROT_FROM_SHAPE_CHANGERS].extrinsic
#define Protection_from_shape_changers \
				(HProtection_from_shape_changers || \
				 EProtection_from_shape_changers)

#define HPolymorph		u.uprops[POLYMORPH].intrinsic
#define EPolymorph		u.uprops[POLYMORPH].extrinsic
#define Polymorph		(HPolymorph || EPolymorph || ((moves % 10 == 0) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "changing cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "izmeneniye plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "o'zgaruvchan plash") ) ) || (uarmf && uarmf->oartifact == ART_NOSE_ENCHANTMENT) || (uarmh && uarmh->oartifact == ART_RANDOMNESS_PREVAILS) || (uarmh && uarmh->oartifact == ART_DOUBLE_JEOPARDY) || (uarmf && uarmf->oartifact == ART_ERROR_IN_PLAY_ENCHANTMENT) || (uleft && uleft->oartifact == ART_SEMI_SHAPE_CONTROL) || (uarmc && uarmc->oartifact == ART_SHAPETAKE_NUMBER_FIVE) || (uright && uright->oartifact == ART_SEMI_SHAPE_CONTROL) || (uamul && uamul->oartifact == ART_SPACE_CYCLE) || Race_if(PM_MOULD) || Race_if(PM_PLAYER_MIMIC) || Race_if(PM_TRANSFORMER) || Race_if(PM_WORM_THAT_WALKS) || Race_if(PM_MISSINGNO) || Race_if(PM_WARPER) || Race_if(PM_UNGENOMOLD) || Race_if(PM_DEATHMOLD))

#define HPolymorph_control	u.uprops[POLYMORPH_CONTROL].intrinsic
#define EPolymorph_control	u.uprops[POLYMORPH_CONTROL].extrinsic
#define Polymorph_control	((HPolymorph_control || EPolymorph_control || (uarmu && uarmu->oartifact == ART_VICTORIA_IS_EVIL_BUT_PRETT) || (uarmu && uarmu->oartifact == ART_NATALIA_IS_LOVELY_BUT_DANG) || (uarmh && uarmh->oartifact == ART_DICTATORSHIP) ) && !Race_if(PM_MOULD) && !NoPolymorph_control && !Race_if(PM_TRANSFORMER) && !Race_if(PM_WORM_THAT_WALKS) && !Race_if(PM_WARPER) && !Race_if(PM_MISSINGNO) && !Race_if(PM_UNGENOMOLD) && !Race_if(PM_DEATHMOLD) && !Race_if(PM_AK_THIEF_IS_DEAD_))
#define NoPolymorph_control	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic || (uarmc && uarmc->oartifact == ART_PERMANENTITIS) || (uarmc && uarmc->oartifact == ART_SHAPETAKE_NUMBER_FIVE) || (uarmh && uarmh->oartifact == ART_RANDOMNESS_PREVAILS) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HUnchanging		u.uprops[UNCHANGING].intrinsic
#define EUnchanging		u.uprops[UNCHANGING].extrinsic
#define Unchanging		((HUnchanging || EUnchanging) && !(uarmc && uarmc->oartifact == ART_SHAPETAKE_NUMBER_FIVE) && !Race_if(PM_MOULD) && !Race_if(PM_MISSINGNO) && !Race_if(PM_TRANSFORMER) && !Race_if(PM_WORM_THAT_WALKS) && !Race_if(PM_WARPER) && !Race_if(PM_UNGENOMOLD) && !Race_if(PM_DEATHMOLD) && !Race_if(PM_AK_THIEF_IS_DEAD_))	/* KMH */

#define HFast			u.uprops[FAST].intrinsic
#define EFast			u.uprops[FAST].extrinsic
#define Fast			( (HFast || EFast) && !NoFast && !Race_if(PM_DEVELOPER) && (!Role_if(PM_TRANSVESTITE) || flags.female) && (!Role_if(PM_TOPMODEL) || !flags.female) )
#define Very_fast		(( ( (HFast & ~INTRINSIC) || EFast || (uwep && uwep->oartifact == ART_FUMATA_YARI) || (uwep && uwep->oartifact == ART_INGRAM_MAC___) || (uarmc && uarmc->oartifact == ART_SPEEDRUNNER_S_DREAM) || (uarmh && uarmh->oartifact == ART_REAL_SPEED_DEVIL) || (uarmc && uarmc->oartifact == ART_ROKKO_CHAN_S_SUIT) || (uwep && uwep->oartifact == ART_EAMANE_LUINWE) || (uarmc && uarmc->oartifact == ART_FULLY_LIONIZED) || (uarmh && uarmh->oartifact == ART_LORSKEL_S_SPEED) || (uarmc && uarmc->oartifact == ART_JANA_S_FAIRNESS_CUP) || (uarmc && uarmc->oartifact == ART_FAST_CAMO_PREDATOR) || (uarmh && uarmh->oartifact == ART_ELESSAR_ELENDIL) || (uarmf && uarmf->oartifact == ART_UNTRAINED_HALF_MARATHON) || (uleft && uleft->oartifact == ART_REQUIRED_GLADNESS) || (uwep && uwep->oartifact == ART_FEANARO_SINGOLLO) || (uarmc && uarmc->oartifact == ART_LIGHTSPEED_TRAVEL) || (uright && uright->oartifact == ART_REQUIRED_GLADNESS) || (uarmf && uarmf->oartifact == ART_HIGH_DESIRE_OF_FATALITY) || (uwep && uwep->oartifact == ART_SPEEDHACK) || (uarmc && uarmc->oartifact == ART_JANA_S_SECRET_CAR) || (uarmc && uarmc->oartifact == ART_JANA_S_GRAVE_WALL) || (uleft && uleft->oartifact == ART_CRIMINAL_QUEEN) || (uright && uright->oartifact == ART_CRIMINAL_QUEEN) || (uwep && uwep->oartifact == ART_TENSA_ZANGETSU) || (uwep && uwep->oartifact == ART_ZANKAI_HUNG_ZE_TUNG_DO_HAI) || (uwep && uwep->oartifact == ART_GARNET_ROD) || (uwep && uwep->oartifact == ART_THREE_HEADED_FLAIL) || u.uprops[MULTISHOES].extrinsic ) && !Race_if(PM_DEVELOPER) && !NoFast && (!Role_if(PM_TRANSVESTITE) || flags.female) && (!Role_if(PM_TOPMODEL) || !flags.female) ) || ( (Role_if(PM_TRANSVESTITE) || Role_if(PM_TOPMODEL) ) && !NoFast && !Race_if(PM_DEVELOPER) && PlayerInHighHeels ))
#define NoFast	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_FAST].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HReflecting		u.uprops[REFLECTING].intrinsic
#define EReflecting		u.uprops[REFLECTING].extrinsic
#define Reflecting		((EReflecting || HReflecting || (moves % 2 == 0 && (uarm && OBJ_DESCR(objects[uarm->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarm->otyp]), "octarine robe") || !strcmp(OBJ_DESCR(objects[uarm->otyp]), "vos'moy tsvet khalata") || !strcmp(OBJ_DESCR(objects[uarm->otyp]), "sakkizinchi rang to'n") )) ) || (uarmu && uarmu->oartifact == ART_LEGENDARY_SHIRT) || (uarm && uarm->oartifact == ART_VOLUME_ARMAMENT) || (uarm && uarm->oartifact == ART_DON_SUICUNE_DOES_NOT_APPRO) || (uarmc && uarmc->oartifact == ART_A_REASON_TO_LIVE) || (uarmc && uarmc->oartifact == ART_ALL_IN_ONE_ASCENSION_KIT) || (uarmc && uarmc->oartifact == ART_RNG_S_GAMBLE) || (uarmh && uarmh->oartifact == ART_YOU_ARE_ALREADY_DEAD) || (uarmg && uarmg->oartifact == ART_PLUG_AND_PRAY) || (uarmg && uarmg->oartifact == ART_IRIS_S_PRECIOUS_METAL) || (uarmg && uarmg->oartifact == ART_SEALED_KNOWLEDGE) || (uarmg && uarmg->oartifact == ART_UNOBTAINABLE_BEAUTIES) || \
				 (youmonst.data == &mons[PM_SILVER_DRAGON] ||\
				  youmonst.data == &mons[PM_CRYSTAL_GOLEM] ||\
				  youmonst.data == &mons[PM_SAPPHIRE_GOLEM] ||\
				  youmonst.data == &mons[PM_SHAMBLING_MOUND] ||\
				  youmonst.data == &mons[PM_GREAT_WYRM_OF_POWER] ||\
				  youmonst.data == &mons[PM_SILVER_WOLF] ||\
				  youmonst.data == &mons[PM_DORA_S_TENDER_STILETTO_SANDAL] ||\
				  youmonst.data == &mons[PM_GRETA_S_HUGGING_BOOT] ||\
				  youmonst.data == &mons[PM_MAEGLIN__THE_TRAITOR_OF_GONDOLIN] ||\
				  youmonst.data == &mons[PM_DAOLOTH__THE_RENDER_OF_THE_VEILS] ||\
				  youmonst.data == &mons[PM_IT] ||\
				  youmonst.data == &mons[PM_MADELEINE_S_CALF_LEATHER_SANDAL] ||\
				  youmonst.data == &mons[PM_REFLECTION] ||\
				  youmonst.data == &mons[PM_NICE_GIRL_IN_VELCRO_SHOES] ||\
				  youmonst.data == &mons[PM_GREEN_LIZARD] ||\
				  youmonst.data == &mons[PM_GREEN_GATEKEEPER] ||\
				  youmonst.data == &mons[PM_POISON_CREEPER] ||\
				  youmonst.data == &mons[PM_PLAGUE_POPPY] ||\
				  youmonst.data == &mons[PM_CARRION_VINE] ||\
				  youmonst.data == &mons[PM_CYCLE_OF_LIFE] ||\
				  youmonst.data == &mons[PM_SOLAR_CREEPER] ||\
				  youmonst.data == &mons[PM_VINES] ||\
				  youmonst.data == &mons[PM_IRIS_S_SILVER_PUMP] ||\
				  youmonst.data == &mons[PM_DESIREE_S_COMBAT_BOOT] ||\
				  youmonst.data == &mons[PM_HANH_S_TRANSLUCENT_SANDAL] ||\
				  youmonst.data == &mons[PM_MUNCHKIN] ||\
				  youmonst.data == &mons[PM_MIRROR_VORTEX] ||\
				  youmonst.data == &mons[PM_MIRROR_GOLEM] ||\
				  youmonst.data == &mons[PM_MIRROR_MONSTER] ||\
				  youmonst.data == &mons[PM_VENOM_ATRONACH] ||\
				  youmonst.data == &mons[PM_DIVISION_THIEF] ||\
				  youmonst.data == &mons[PM_DIVISION_JEDI] ||\
				  youmonst.data == &mons[PM_MIRROR_MOLD] ||\
				  youmonst.data == &mons[PM_MIRROR_GROWTH] ||\
				  youmonst.data == &mons[PM_MIRROR_FUNGUS] ||\
				  youmonst.data == &mons[PM_MIRROR_PATCH] ||\
				  youmonst.data == &mons[PM_MIRROR_SPORE] ||\
				  youmonst.data == &mons[PM_MIRROR_MUSHROOM] ||\
				  youmonst.data == &mons[PM_MIRROR_STALK] ||\
				  youmonst.data == &mons[PM_MIRROR_COLONY] ||\
				  youmonst.data == &mons[PM_MIRROR_FORCE_FUNGUS] ||\
				  youmonst.data == &mons[PM_MIRROR_FORCE_PATCH] ||\
				  youmonst.data == &mons[PM_MIRROR_WARP_FUNGUS] ||\
				  youmonst.data == &mons[PM_MIRROR_WARP_PATCH] ||\
				  youmonst.data == &mons[PM_ECM_NERVE_HEAD] ||\
				  youmonst.data == &mons[PM_ECM_ARCHER] ||\
				  youmonst.data == &mons[PM_ILLUSION_WEAVER] ||\
				  youmonst.data == &mons[PM_SEDUCER_SAINT] ||\
				  youmonst.data == &mons[PM_OLD_SILVER_DRAGON] ||\
				  youmonst.data == &mons[PM_SILVER_OGRE] ||\
				  youmonst.data == &mons[PM_VERY_OLD_SILVER_DRAGON] ||\
				  youmonst.data == &mons[PM_SILVER_DRACONIAN] ||\
				  youmonst.data == &mons[PM_AUREAL] ||\
				  youmonst.data == &mons[PM_ARIANE] ||\
				  youmonst.data == &mons[PM_MEDIATOR] ||\
				  youmonst.data == &mons[PM_GROTESQUE_GREMLIN] ||\
				  youmonst.data == &mons[PM_CLOUD_OF_GLORY] ||\
				  youmonst.data == &mons[PM_VICTORIA_S_HIGH_HEELED_SANDAL] ||\
				  youmonst.data == &mons[PM_RUEA_S_STILETTO_BOOT] ||\
				  youmonst.data == &mons[PM_NATALIYA_S_WEDGE_SANDAL] ||\
				  youmonst.data == &mons[PM_ELENA_S_COMBAT_BOOT] ||\
				  youmonst.data == &mons[PM_KATHARINA_S_LOVELY_GIRL_BOOT] ||\
				  youmonst.data == &mons[PM_RHEA_S_LADY_PUMP] ||\
				  youmonst.data == &mons[PM_KSENIA] ||\
				  youmonst.data == &mons[PM_ANCIENT_SILVER_DRAGON] ||\
				  youmonst.data == &mons[PM_DIAMOND_GOLEM])) && !NoReflecting && !(uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "angband cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "plashch sredizem'ye krepost'") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "o'rta yer qal'a plash") )) && !Race_if(PM_ANGBANDER) )
#define NoReflecting	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_REFLECTING].intrinsic || (uarm && uarm->oartifact == ART_ARMOR_OF_EREBOR && (moves % 4 == 0) ) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HFree_action	u.uprops[FREE_ACTION].intrinsic
#define EFree_action	u.uprops[FREE_ACTION].extrinsic
#define Free_action		((HFree_action || EFree_action || (uwep && uwep->oartifact == ART_KARATE_KID) || (uarmu && uarmu->oartifact == ART_PEACE_ADVOCATE) || (uarm && uarm->oartifact == ART_LITTLE_BIG_MIDGET) || (uarmh && uarmh->oartifact == ART_LOW_LOCAL_MEMORY) || (uarmf && uarmf->oartifact == ART_KHOR_S_REQUIRED_IDEA) || (uarmh && uarmh->oartifact == ART_LLLLLLLLLLLLLM) || (uarm && uarm->oartifact == ART_CHECK_YOUR_ESCAPES) || (uarms && uarms->oartifact == ART_LURTZ_S_WALL) || (uarms && uarms->oartifact == ART_BRASS_GUARD) || (uarmf && uarmf->oartifact == ART_ANASTASIA_S_GENTLENESS) || (uarms && uarms->oartifact == ART_I_M_GETTING_HUNGRY) || (uarmg && uarmg->oartifact == ART_FREE_ACTION_CALLED_FREE_AC) || (uarmc && uarmc->oartifact == ART_PREDATORY_STABILITY) || (uarmf && uarmf->oartifact == ART_FREE_FOR_ENOUGH) || (uarmf && uarmf->oartifact == ART_MANDY_S_ROUGH_BEAUTY) || (uarmf && uarmf->oartifact == ART_WAITING_TIMEOUT) || (uamul && uamul->oartifact == ART_BALLSY_BASTARD) || (uamul && uamul->oartifact == ART___TH_NAZGUL) || (uamul && uamul->oartifact == ART_NAZGUL_S_REVENGE) || (uwep && uwep->oartifact == ART_SOURCE_CODES_OF_WORK_AVOID) ) && !NoFree_action) /* [Tom] */
#define NoFree_action	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_FREE_ACTION].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define Fixed_abil		u.uprops[FIXED_ABIL].extrinsic	/* KMH */

#define Lifesaved		u.uprops[LIFESAVED].extrinsic

#define HKeen_memory		u.uprops[KEEN_MEMORY].intrinsic
#define EKeen_memory		u.uprops[KEEN_MEMORY].extrinsic
#define Keen_memory		((HKeen_memory || EKeen_memory || (uarmu && uarmu->oartifact == ART_MEMORIAL_GARMENTS) || (uarmc && uarmc->oartifact == ART_MEMORY_AID) || (uarmc && uarmc->oartifact == ART_REMEMBERING_THE_BAD_TIMES) || (uarms && uarms->oartifact == ART_GOLDEN_DAWN) || (uarms && uarms->oartifact == ART_GREXIT_IS_NEAR) || (uamul && uamul->oartifact == ART_COMPUTER_AMULET) || (youmonst.data->mlet == S_QUADRUPED)) && !NoKeen_memory)
#define NoKeen_memory	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_KEEN_MEMORY].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HSight_bonus		u.uprops[SIGHT_BONUS].intrinsic
#define ESight_bonus		u.uprops[SIGHT_BONUS].extrinsic
#define Sight_bonus		((HSight_bonus || ESight_bonus || (uwep && uwep->oartifact == ART_PEOPLE_EATING_TRIDENT) || (uwep && uwep->oartifact == ART_AXE_OF_THE_DWARVISH_LORDS) || (uarms && uarms->oartifact == ART_SOLAR_POWER) || (uamul && uamul->oartifact == ART_GOOD_BEE) || (uarmf && uarmf->oartifact == ART_GRENEUVENIA_S_HUG) || (uamul && uamul->oartifact == ART_BUEING) || (uarmh && uarmh->oartifact == ART_HAT_OF_THE_ARCHMAGI) ) && !NoSight_bonus)
#define NoSight_bonus	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SIGHT_BONUS].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HVersus_curses		u.uprops[VERSUS_CURSES].intrinsic
#define EVersus_curses		u.uprops[VERSUS_CURSES].extrinsic
#define Versus_curses		((HVersus_curses || EVersus_curses || (uarmu && uarmu->oartifact == ART_MEMORIAL_GARMENTS)) && !NoVersus_curses)
#define NoVersus_curses	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_VERSUS_CURSES].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HStun_resist		u.uprops[STUN_RES].intrinsic
#define EStun_resist		u.uprops[STUN_RES].extrinsic
#define Stun_resist		((HStun_resist || EStun_resist || (uarmu && uarmu->oartifact == ART_TOTAL_CONTROL) || (uarm && uarm->oartifact == ART_PROTECTION_WITH_A_PRICE) || (uarmc && uarmc->oartifact == ART_FREQUENT_BUT_WEAK_STATUS) || (uarmh && uarmh->oartifact == ART_MIND_SHIELDING) || (uarms && uarms->oartifact == ART_FIVE_STAR_PARTY) || (uamul && uamul->oartifact == ART_ONE_MOMENT_IN_TIME) || (uamul && uamul->oartifact == ART_PRECIOUS_UNOBTAINABLE_PROP) || (uamul && uamul->oartifact == ART_RECOVERED_RELIC) ) && !NoStun_resist)
#define NoStun_resist	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_STUN_RES].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HConf_resist		u.uprops[CONF_RES].intrinsic
#define EConf_resist		u.uprops[CONF_RES].extrinsic
#define Conf_resist		((HConf_resist || EConf_resist || (uarmf && uarmf->oartifact == ART_SANDRA_S_BEAUTIFUL_FOOTWEA) || (uarms && uarms->oartifact == ART_FIVE_STAR_PARTY) || (uarmu && uarmu->oartifact == ART_TOTAL_CONTROL) || (uarmc && uarmc->oartifact == ART_FREQUENT_BUT_WEAK_STATUS) ) && !NoConf_resist)
#define NoConf_resist	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_CONF_RES].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HPsi_resist		u.uprops[PSI_RES].intrinsic
#define EPsi_resist		u.uprops[PSI_RES].extrinsic
#define Psi_resist		((HPsi_resist || EPsi_resist || (uwep && uwep->oartifact == ART_SWEETHEART_PUMP) || (uarms && uarms->oartifact == ART_REAL_PSYCHOS_WEAR_PURPLE) || (uarms && uarms->oartifact == ART_REAL_MEN_WEAR_PSYCHOS) || (uarmf && uarmf->oartifact == ART_JESSICA_S_TENDERNESS) || (uarmc && uarmc->oartifact == ART_YOG_SOTHOTH_HELP_ME) || (uarmf && uarmf->oartifact == ART_JOSEFINE_S_EVILNESS) || (uarmh && uarmh->oartifact == ART_NUMBER___) || (uamul && uamul->oartifact == ART_ONE_MOMENT_IN_TIME) || (uwep && uwep->oartifact == ART_YASDORIAN_S_JUNETHACK_IDEN) || (uamul && uamul->oartifact == ART_PRECIOUS_UNOBTAINABLE_PROP) ) && !NoPsi_resist)
#define NoPsi_resist	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_PSI_RES].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HDouble_attack		u.uprops[DOUBLE_ATTACK].intrinsic
#define EDouble_attack		u.uprops[DOUBLE_ATTACK].extrinsic
#define Double_attack		((HDouble_attack || EDouble_attack || (uarm && uarm->oartifact == ART_OFFENSE_OWNS_DEFENSE) || (uarmf && uarmf->oartifact == ART_ELLA_S_BLOODLUST) || (uwep && uwep->oartifact == ART_MELEE_DUALITY) ) && !NoDouble_attack)
#define NoDouble_attack	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_DOUBLE_ATTACK].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HQuad_attack		u.uprops[QUAD_ATTACK].intrinsic
#define EQuad_attack		u.uprops[QUAD_ATTACK].extrinsic
#define Quad_attack		((HQuad_attack || EQuad_attack || (uarmc && uarmc->oartifact == ART_SUPERMAN_S_SUPER_SUIT) ) && !NoQuad_attack)
#define NoQuad_attack	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_QUAD_ATTACK].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HUseTheForce		u.uprops[THE_FORCE].intrinsic
#define EUseTheForce		u.uprops[THE_FORCE].extrinsic
#define UseTheForce		((HUseTheForce || EUseTheForce || (uarm && uarm->oartifact == ART_LUKE_S_JEDI_POWER) ) && !NoUseTheForce)
#define NoUseTheForce	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_THE_FORCE].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#endif /* YOUPROP_H */
