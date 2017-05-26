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

#define SpellColorPink	(PinkSpells || u.uprops[PINK_SPELLS].extrinsic || have_pinkspellstone())
#define SpellColorCyan	(!(SpellColorPink) && (CyanSpells || u.uprops[CYAN_SPELLS].extrinsic || have_cyanspellstone()))
#define SpellColorBlack	(!(SpellColorPink) && !(SpellColorCyan) && (BlackSpells || u.uprops[BLACK_SPELLS].extrinsic || have_blackspellstone()))
#define SpellColorRed	(!(SpellColorPink) && !(SpellColorCyan) && !(SpellColorBlack) && (RedSpells || u.uprops[RED_SPELLS].extrinsic || have_redspellstone()))
#define SpellColorGreen	(!(SpellColorPink) && !(SpellColorCyan) && !(SpellColorBlack) && !(SpellColorRed) && (GreenSpells || u.uprops[GREEN_SPELLS].extrinsic || have_greenspellstone()))
#define SpellColorBlue	(!(SpellColorPink) && !(SpellColorCyan) && !(SpellColorBlack) && !(SpellColorRed) && !(SpellColorGreen) && (BlueSpells || u.uprops[BLUE_SPELLS].extrinsic || have_bluespellstone()))
#define SpellColorGray	(!(SpellColorPink) && !(SpellColorCyan) && !(SpellColorBlack) && !(SpellColorRed) && !(SpellColorGreen) && !(SpellColorBlue) && (CompleteGraySpells || u.uprops[COMPLETE_GRAY_SPELLS].extrinsic || have_greyoutstone()))
#define SpellColorWhite	(!(SpellColorPink) && !(SpellColorCyan) && !(SpellColorBlack) && !(SpellColorRed) && !(SpellColorGreen) && !(SpellColorBlue) && !(SpellColorGray) && (WhiteSpells || u.uprops[WHITE_SPELLS].extrinsic || have_whitespellstone()))
#define SpellColorYellow	(!(SpellColorPink) && !(SpellColorCyan) && !(SpellColorBlack) && !(SpellColorRed) && !(SpellColorGreen) && !(SpellColorBlue) && !(SpellColorGray) && !(SpellColorWhite) && (YellowSpells || u.uprops[YELLOW_SPELLS].extrinsic || (uamul && uamul->oartifact == ART_DIKKIN_S_DRAGON_TEETH && !(Role_if(PM_BARD) && Race_if(PM_KOBOLT) ) ) || (uwep && uwep->oartifact == ART_DIKKIN_S_DEADLIGHT) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_DIKKIN_S_DEADLIGHT) || (uwep && uwep->oartifact == ART_DIKKIN_S_FAVORITE_SPELL) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_DIKKIN_S_FAVORITE_SPELL) || have_yellowspellstone()))

#define EnglandMode	(RngeEngland || (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "long-range cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "dlinnyy plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "uzoq masofaga plash") )) || (uarmu && uarmu->oartifact == ART_BEAM_MULTIPLIER) || (uwep && uwep->oartifact == ART_MINOLONG_ELBOW) || (uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "english gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "angliyskiye perchatki") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "ingliz tili qo'lqop") )) )

#define NastyTrapNation		(NastynationBug || u.uprops[NASTY_NATION_BUG].extrinsic || have_multitrappingstone())

#define FingerlessGloves	(uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "fingerless gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "mitenki") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "kam qo'lqop barmoq") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "silk fingerlings") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "shelkovyye mal'ki") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "ipak va ling, barmoqlar") ))

#define PlayerHearsSoundEffects	(SoundEffectBug || u.uprops[SOUND_EFFECT_BUG].extrinsic || (ublindf && ublindf->oartifact == ART_SOUNDTONE_FM) || have_soundeffectstone())

#define PlayerCannotUseSkills	(AllSkillsUnskilled || u.uprops[SKILL_DEACTIVATED].extrinsic || (uarmc && uarmc->oartifact == ART_PALEOLITHIC_ELBOW_CONTRACT) || have_unskilledstone())

#define PlayerInHighHeels	(uarmf && ( (uarmf->otyp == WEDGE_SANDALS) || (uarmf->otyp == FEMININE_PUMPS) || (uarmf->otyp == LEATHER_PEEP_TOES) || (uarmf && RngeIrregularity) || (uarmf->otyp == HIPPIE_HEELS) || (uarmf->oartifact == ART_ABSURD_HEELED_TILESET) || (uarmf->oartifact == ART_SWARM_SOFT_HIGH_HEELS) || (uarmf->oartifact == ART_KATIE_MELUA_S_FEMALE_WEAPO) || (uarmf->oartifact == ART_ARVOGENIA_S_HIGH_HEELSES) || (uarmf->oartifact == ART_MANUELA_S_UNKNOWN_HEELS) || (uarmf->oartifact == ART_RITA_S_TENDER_STILETTOS) || (uarmf->oartifact == ART_ANASTASIA_S_UNEXPECTED_ABI) || (uarmf->oartifact == ART_HIGH_HEELED_HUG) || (Role_if(PM_BINDER) && uarmf->oartifact == ART_BINDER_CRASH) || (uarmf->otyp == PET_STOMPING_PLATFORM_BOOTS) || (uarmf->otyp == SENTIENT_HIGH_HEELED_SHOES) || (uarmf->otyp == ATSUZOKO_BOOTS) || (uarmf->otyp == COMBAT_STILETTOS) || (uarmf->otyp == HIGH_STILETTOS) || (uarmf->otyp == HIGH_HEELED_SKIERS) || (uarmf->otyp == UNFAIR_STILETTOS) || (uarmf->otyp == COVETED_BOOTS) || (uarmf->otyp == SKY_HIGH_HEELS) || (uarmf->otyp == RED_SPELL_HEELS) || (uarmf->otyp == DESTRUCTIVE_HEELS) || (uarmf->otyp == SINFUL_HEELS) || (uarmf->otyp == KILLER_HEELS) || (uarmf->otyp == HIGH_SCORING_HEELS) || (OBJ_DESCR(objects[uarmf->otyp]) && ( (!strcmp(OBJ_DESCR(objects[uarmf->otyp]), "irregular boots") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "neregulyarnyye sapogi") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "tartibsizlik chizilmasin") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "wedge boots") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "klin sapogi") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "xanjar chizilmasin") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "winter stilettos") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "zima stilety") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "qish sandal chizilmasin") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "clunky heels") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "neuklyuzhiye kabluki") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "qisqa ko'chirish to'piqlarni") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "ankle boots") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "botil'ony") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "bilagi zo'r chizilmasin") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "block-heeled boots") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "blok kablukakh sapogi") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "blok-o'tish chizilmasin") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "beautiful heels") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "krasivyye kabluki") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "chiroyli ko'chirish to'piqlarni") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "erotic boots") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "eroticheskiye sapogi") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "erotik chizilmasin") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "sputa boots") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "mokrota sapogi") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "sputa chizilmasin") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "femmy boots") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "zhenskiye sapogi") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "nazokat etigi") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "sharp-edged sandals") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "ostrokonechnyye sandalii") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "o'tkir xanjarday kavushlari") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "ski heels") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "lyzhnyye kabluki") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "chang'i poshnalar") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "fetish heels") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "idol kabluki") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "but poshnalar") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "velvet pumps") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "barkhatnyye nasosy") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "duxoba nasoslar") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "buffalo boots") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "buyvolovyye sapogi") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "qo'tos botlarni") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "lolita boots") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "botinki s lolitoy") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "bosh ketish etigi") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "weapon light boots") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "legkiye botinki dlya oruzhiya") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "qurol engil etigi") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "radiant heels") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "izluchayushchiye kabluki") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "yorqin ko'chirish to'piqlarni") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "sexy heels") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "seksual'nyye kabluki") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "belgila sexy ko'chirish to'piqlarni") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "stroking boots") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "poglazhivaya sapogi") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "etiklar silay") ) ) ) ) )

/*** Resistances to troubles ***/
/* With intrinsics and extrinsics */
#define HFire_resistance	u.uprops[FIRE_RES].intrinsic
#define EFire_resistance	u.uprops[FIRE_RES].extrinsic
#define Fire_resistance		(( (HFire_resistance && !(u.nonintrinsicproperty == FIRE_RES)) || EFire_resistance || (uarmh && uarmh->oartifact == ART_SPECTRAL_RESISTANCE) || (uarmu && uarmu->oartifact == ART_BEAM_MULTIPLIER) || (uarmf && uarmf->oartifact == ART_YET_ANOTHER_STUPID_IDEA) || (uwep && uwep->oartifact == ART_AND_YOUR_MORTAL_WORLD_SHAL) || (uarmf && uarmf->oartifact == ART_JOSEFINE_S_EVILNESS) || (uwep && uwep->oartifact == ART_PRISMATIC_PROTECTION) || (uarmh && uarmh->oartifact == ART_HAVE_ALL_YOU_NEED) || (uarmf && uarmf->oartifact == ART_BLUEDE) || (uwep && uwep->oartifact == ART_FAEAEAEAEAEAU) || (uarmh && uarmh->oartifact == ART_WAR_MASK_OF_DURIN) || (uarmu && uarmu->oartifact == ART_BIENVENIDO_A_MIAMI) || (uarmu && uarmu->oartifact == ART_PRISMATIC_SHIRT) || (uarmf && uarmf->oartifact == ART_GRENEUVENIA_S_HUG) || (uarmh && uarmh->oartifact == ART_MASSIVE_IRON_CROWN_OF_MORG) || (uarmf && uarmf->oartifact == ART_TEMPERATOR) || (uarm && uarm->oartifact == ART_ARMOR_OF_EREBOR) || (uarmc && uarmc->oartifact == ART_FIREBURN_COLDSHATTER) || (uarmf && uarmf->oartifact == ART_NASTIST) || (uwep && uwep->oartifact == ART_STAFF_OF_THE_ARCHMAGI) || (uwep && uwep->oartifact == ART_WHAT_IT_SAYS_ON_THE_TIN) || (uarmg && uarmg->oartifact == ART_SPECTRATOR && (moves % 5 == 0) ) || (uarmh && uarmh->oartifact == ART_SECURE_BATHMASTER) || (uwep && uwep->oartifact == ART_WRATH_OF_HEAVEN) || (uwep && uwep->oartifact == ART_COLD_SOUL) || u.uprops[STORM_HELM].extrinsic || u.uprops[ELEMENT_RES].extrinsic || \
				 (resists_fire(&youmonst) && !(u.nonintrinsicproperty == FIRE_RES)) || (uarmf && OBJ_DESCR(objects[uarmf->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmf->otyp]), "korean sandals") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "koreyskiye sandalii") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "janubiy koreyaning kavushlari") ) && (moves % 3 == 0) ) || (is_fire_resistant(youmonst.data) && !(u.nonintrinsicproperty == FIRE_RES)) ) && !Race_if(PM_TROLLOR) && !Race_if(PM_ANCIPITAL) && !Race_if(PM_SEA_ELF) && !Race_if(PM_SPRIGGAN) && !Role_if(PM_ALTMER) && !NoFire_resistance)
#define NoFire_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_FIRE_RES].intrinsic || RngeAids || (uarm && uarm->oartifact == ART_SUPERESCAPE_MAIL) || (u.nonextrinsicproperty == FIRE_RES && !HFire_resistance && !resists_fire(&youmonst) && !is_fire_resistant(youmonst.data) ) || (u.impossibleproperty == FIRE_RES) || (uarm && uarm->oartifact == ART_ABSOLUTE_MONSTER_MAIL) || (uarm && uarm->oartifact == ART_FLEEING_MINE_MAIL) || (uarmf && uarmf->oartifact == ART_MEPHISTO_S_BROGUES) || (uarmf && uarmf->oartifact == ART_VERA_S_FREEZER) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HCold_resistance	u.uprops[COLD_RES].intrinsic
#define ECold_resistance	u.uprops[COLD_RES].extrinsic
#define Cold_resistance		(( (HCold_resistance && !(u.nonintrinsicproperty == COLD_RES)) || ECold_resistance || (uarmh && uarmh->oartifact == ART_SPECTRAL_RESISTANCE) || (uarmg && uarmg->oartifact == ART_SIGNONS_STEEL_TOTAL) || (uarmf && uarmf->oartifact == ART_MEPHISTO_S_BROGUES) || (uarmf && uarmf->oartifact == ART_RARE_ASIAN_LADY) || (uarmf && uarmf->oartifact == ART_KATRIN_S_PARALYSIS) || (uarmu && uarmu->oartifact == ART_BEAM_MULTIPLIER) || (uarmf && uarmf->oartifact == ART_JESSICA_S_TENDERNESS) || (uarmu && uarmu->oartifact == ART_PRISMATIC_SHIRT) || (uarmf && uarmf->oartifact == ART_JOSEFINE_S_EVILNESS) || (uarmf && uarmf->oartifact == ART_NASTIST) || (uarmf && uarmf->oartifact == ART_JOHANETTA_S_ROUGH_GENTLENE) || (uarmf && uarmf->oartifact == ART_LARISSA_S_ANGER) || (uarmf && uarmf->oartifact == ART_BLUEDE) || (uarmg && uarmg->oartifact == ART_SPECTRATOR && (moves % 5 == 0) ) || (uarmh && uarmh->oartifact == ART_SECURE_BATHMASTER) || (uwep && uwep->oartifact == ART_PRISMATIC_PROTECTION) || (uarmh && uarmh->oartifact == ART_MASSIVE_IRON_CROWN_OF_MORG) || (uarmf && uarmf->oartifact == ART_CHOICE_OF_MATTER) || (uarmu && uarmu->oartifact == ART_BIENVENIDO_A_MIAMI) || (uarmf && uarmf->oartifact == ART_CORINA_S_UNFAIR_SCRATCHER) || (uarmh && uarmh->oartifact == ART_HAVE_ALL_YOU_NEED) || (uarmc && uarmc->oartifact == ART_FIREBURN_COLDSHATTER) || (uarm && uarm->oartifact == ART_ARMOR_OF_EREBOR) || (uarmf && uarmf->oartifact == ART_CORINA_S_SNOWY_TREAD) || (uarmc && uarmc->oartifact == ART_CLOAK_OF_THE_CONSORT) || (uwep && uwep->oartifact == ART_STAFF_OF_THE_ARCHMAGI) || (uwep && uwep->oartifact == ART_COLD_SOUL) || (uwep && uwep->oartifact == ART_RELEASE_FROM_CARE) || u.uprops[STORM_HELM].extrinsic || u.uprops[ELEMENT_RES].extrinsic || \
				 (resists_cold(&youmonst) && !(u.nonintrinsicproperty == COLD_RES)) || (is_cold_resistant(youmonst.data) && !(u.nonintrinsicproperty == COLD_RES)) ) && !Role_if(PM_ALTMER) && !NoCold_resistance )
#define NoCold_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_COLD_RES].intrinsic || RngeAids || (u.nonextrinsicproperty == COLD_RES && !HCold_resistance && !resists_cold(&youmonst) && !is_cold_resistant(youmonst.data) ) || (u.impossibleproperty == COLD_RES) || (uleft && uleft->oartifact == ART_DEATHLY_COLD) || (uright && uright->oartifact == ART_DEATHLY_COLD) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HSleep_resistance	u.uprops[SLEEP_RES].intrinsic
#define ESleep_resistance	u.uprops[SLEEP_RES].extrinsic
#define Sleep_resistance	( !( (Thirst || (uwep && uwep->oartifact == ART_LUISA_S_CHARMING_BEAUTY) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_LUISA_S_CHARMING_BEAUTY) || u.uprops[THIRST].extrinsic || have_thirststone()) && (u.uhunger > 2000 ) ) && !Race_if(PM_KOBOLT) && !Race_if(PM_DEEP_ELF) && !have_sleepstone() && !NoSleep_resistance && ( (HSleep_resistance && !(u.nonintrinsicproperty == SLEEP_RES)) || ESleep_resistance || (uwep && uwep->oartifact == ART_SLEEPLESS_NIGHTS) || (uarmf && uarmf->oartifact == ART_NASTIST) || (uarmf && uarmf->oartifact == ART_BLUEDE) || (uarmh && uarmh->oartifact == ART_UNIMPORTANT_ELEMENTS) || (uarmu && uarmu->oartifact == ART_PRISMATIC_SHIRT) || FemaleTrapNatalje || (uarmf && uarmf->oartifact == ART_JESSICA_S_TENDERNESS) || (uarmh && uarmh->oartifact == ART_HAVE_ALL_YOU_NEED) || (uamul && uamul->oartifact == ART_SNOREFEST && (moves % 10 != 0) ) || \
				 (resists_sleep(&youmonst) && !(u.nonintrinsicproperty == SLEEP_RES)) || (is_sleep_resistant(youmonst.data) && !(u.nonintrinsicproperty == SLEEP_RES )) ) )
#define NoSleep_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SLEEP_RES].intrinsic || RngeAids || (uarms && uarms->oartifact == ART_LITTLE_THORN_ROSE) || (u.nonextrinsicproperty == SLEEP_RES && !HSleep_resistance && !resists_sleep(&youmonst) && !is_sleep_resistant(youmonst.data) ) || (u.impossibleproperty == SLEEP_RES) || (uamul && uamul->oartifact == ART_SPELLCASTER_S_DREAM) || (uarmh && uarmh->oartifact == ART_DULLIFIER) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HDisint_resistance	u.uprops[DISINT_RES].intrinsic
#define EDisint_resistance	u.uprops[DISINT_RES].extrinsic
#define Disint_resistance	(( (HDisint_resistance && !(u.nonintrinsicproperty == DISINT_RES)) || EDisint_resistance || (uarmu && uarmu->oartifact == ART_GENTLE_SOFT_CLOTHING) || (uarmc && uarmc->oartifact == ART_INA_S_LAB_COAT) || (uarmc && uarmc->oartifact == ART_INA_S_OVERCOAT) || (uarmf && uarmf->oartifact == ART_MADELEINE_S_GIRL_FOOTSTEPS) || (uarmf && uarmf->oartifact == ART_ARVOGENIA_S_HIGH_HEELSES) || (ublindf && ublindf->oartifact == ART_STABLE_STUNT) || (uleft && uleft->oartifact == ART_RING_OF_THE_SCHWARTZ) || (uarmf && uarmf->oartifact == ART_LARISSA_S_ANGER) || (uright && uright->oartifact == ART_RING_OF_THE_SCHWARTZ) || (uarms && uarms->oartifact == ART_ANTINSTANT_DEATH) || (uleft && uleft->oartifact == ART_ACHROMANTIC_RING) || (uright && uright->oartifact == ART_ACHROMANTIC_RING) || (uamul && uamul->oartifact == ART_STINGING_MEDALLION) || \
				 (resists_disint(&youmonst) && !(u.nonintrinsicproperty == DISINT_RES)) || (is_disint_resistant(youmonst.data) && !(u.nonintrinsicproperty == DISINT_RES)) ) && !NoDisint_resistance)
#define NoDisint_resistance (!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_DISINT_RES].intrinsic || (uarmh && uarmh->oartifact == ART_UBB_RUPTURE) || RngeAids || (u.nonextrinsicproperty == DISINT_RES && !HDisint_resistance && !resists_disint(&youmonst) && !is_disint_resistant(youmonst.data) ) || (u.impossibleproperty == DISINT_RES) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HShock_resistance	u.uprops[SHOCK_RES].intrinsic
#define EShock_resistance	u.uprops[SHOCK_RES].extrinsic
#define Shock_resistance	(( (HShock_resistance && !(u.nonintrinsicproperty == SHOCK_RES)) || EShock_resistance || (uarmh && uarmh->oartifact == ART_SPECTRAL_RESISTANCE) || (uarmf && uarmf->oartifact == ART_KATRIN_S_PARALYSIS) || (uarmf && uarmf->oartifact == ART_SOLVEJG_S_STINKING_SLIPPER) || (uarmu && uarmu->oartifact == ART_BEAM_MULTIPLIER) || (uarmc && uarmc->oartifact == ART_ACIDSHOCK_CASTLECRUSHER) || (uwep && uwep->oartifact == ART_PRISMATIC_PROTECTION) || (uarmf && uarmf->oartifact == ART_JOSEFINE_S_EVILNESS) || (uarmf && uarmf->oartifact == ART_LARISSA_S_ANGER) || (uarmf && uarmf->oartifact == ART_JOHANETTA_S_ROUGH_GENTLENE) || (uarmf && uarmf->oartifact == ART_PRETTY_ROOMMAID) || (uarmf && uarmf->oartifact == ART_BLUEDE) || (uwep && uwep->oartifact == ART_RESISTOMATIC) || (uarmg && uarmg->oartifact == ART_SPECTRATOR && (moves % 5 == 0) ) || (uarmh && uarmh->oartifact == ART_HAVE_ALL_YOU_NEED) || (uarmf && uarmf->oartifact == ART_CORINA_S_UNFAIR_SCRATCHER) || (uarmh && uarmh->oartifact == ART_MASSIVE_IRON_CROWN_OF_MORG) || (uarmh && uarmh->oartifact == ART_STORMHELM) || (uarmc && uarmc->oartifact == ART_WEB_OF_THE_CHOSEN) || (uwep && uwep->oartifact == ART_STAFF_OF_THE_ARCHMAGI) || (uwep && uwep->oartifact == ART_FIRE_OF_HEAVEN) || (uwep && uwep->oartifact == ART_COLD_SOUL) || u.uprops[STORM_HELM].extrinsic || u.uprops[ELEMENT_RES].extrinsic || \
				 (resists_elec(&youmonst) && !(u.nonintrinsicproperty == SHOCK_RES)) || (is_elec_resistant(youmonst.data) && !(u.nonintrinsicproperty == SHOCK_RES)) ) && !Role_if(PM_ALTMER) && !NoShock_resistance )
#define NoShock_resistance (!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SHOCK_RES].intrinsic || RngeAids || (u.nonextrinsicproperty == SHOCK_RES && !HShock_resistance && !resists_elec(&youmonst) && !is_elec_resistant(youmonst.data) ) || (u.impossibleproperty == SHOCK_RES) || (uwep && uwep->oartifact == ART_SHOCKING_THERAPY) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_SHOCKING_THERAPY) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HPoison_resistance	u.uprops[POISON_RES].intrinsic
#define EPoison_resistance	u.uprops[POISON_RES].extrinsic
#define Poison_resistance	(( (HPoison_resistance && !(u.nonintrinsicproperty == POISON_RES)) || EPoison_resistance || (uarmh && uarmh->oartifact == ART_SPECTRAL_RESISTANCE) || (uarmu && uarmu->oartifact == ART_GENTLE_SOFT_CLOTHING) || (uarmc && uarmc->oartifact == ART_ACQUIRED_POISON_RESISTANCE) || (uarmc && uarmc->oartifact == ART_PREMIUM_VISCOSITY) || (uarmh && uarmh->oartifact == ART_BIG_BONNET) || (uarmg && uarmg->oartifact == ART_SIGNONS_STEEL_TOTAL) || (uarmf && uarmf->oartifact == ART_MEPHISTO_S_BROGUES) || (uwep && uwep->oartifact == ART_ALSO_MATTE_MASK) || (uwep && uwep->oartifact == ART_PRISMATIC_PROTECTION) || (uarmf && uarmf->oartifact == ART_BLUEDE) || (uarmu && uarmu->oartifact == ART_PRISMATIC_SHIRT) || (uarmf && uarmf->oartifact == ART_MAILIE_S_CHALLENGE) || (uarmh && uarmh->oartifact == ART_MASSIVE_IRON_CROWN_OF_MORG) || (uleft && uleft->oartifact == ART_GREEN_COLOR) || (uright && uright->oartifact == ART_GREEN_COLOR) || (uarmh && uarmh->oartifact == ART_TARI_FEFALAS) || (uarmg && uarmg->oartifact == ART_SPECTRATOR && (moves % 5 == 0) ) || (uarmf && uarmf->oartifact == ART_LEATHER_PUMPS_OF_HORROR) || (uarmf && uarmf->oartifact == ART_RHEA_S_COMBAT_PUMPS) || (uamul && uamul->oartifact == ART_WARNED_AND_PROTECTED) || (uarmh && uarmh->oartifact == ART_GREEN_STATUS) || (uamul && uamul->oartifact == ART_GOOD_BEE) || (uamul && uamul->oartifact == ART_BUEING) || (uwep && uwep->oartifact == ART_ERU_ILUVATAR_S_BIBLE) || (uarmh && uarmh->oartifact == ART_WAR_MASK_OF_DURIN) || (uwep && uwep->oartifact == ART_SERPENT_S_TOOTH) || \
				 (resists_poison(&youmonst) && !(u.nonintrinsicproperty == POISON_RES)) || (is_poison_resistant(youmonst.data) && !(u.nonintrinsicproperty == POISON_RES)) ) && !NoPoison_resistance && !Race_if(PM_POISONER) )
#define NoPoison_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_POISON_RES].intrinsic || RngeAids || (u.nonextrinsicproperty == POISON_RES && !HPoison_resistance && !resists_poison(&youmonst) && !is_poison_resistant(youmonst.data) ) || (u.impossibleproperty == POISON_RES) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HDrain_resistance	u.uprops[DRAIN_RES].intrinsic
#define EDrain_resistance	u.uprops[DRAIN_RES].extrinsic
#define Drain_resistance	(((HDrain_resistance && !(u.nonintrinsicproperty == DRAIN_RES)) || EDrain_resistance || (uarmc && uarmc->oartifact == ART_ALL_IN_ONE_ASCENSION_KIT) || (uleft && uleft->oartifact == ART_MAGICAL_SHOCK) || (uright && uright->oartifact == ART_MAGICAL_SHOCK) || (uarmh && uarmh->oartifact == ART_LLLLLLLLLLLLLM) || (uwep && uwep->oartifact == ART_CUBIC_BONE) || (uarmf && uarmf->oartifact == ART_MAILIE_S_CHALLENGE) || (uarmh && uarmh->oartifact == ART_YOU_ARE_ALREADY_DEAD) || (uarmg && uarmg->oartifact == ART_SPECTRATOR && (moves % 5 == 0) ) || (uarmh && uarmh->oartifact == ART_LOW_LOCAL_MEMORY) || (uwep && uwep->oartifact == ART_SANDMAN_VOLUME__) || (uwep && uwep->oartifact == ART_ORTHODOX_MANIFEST) || (uwep && uwep->oartifact == ART_UN_DEATH) || (uarmc && uarmc->oartifact == ART_VEIL_OF_LATONA)  || (uarmc && uarmc->oartifact == ART_VEIL_OF_MINISTRY) || (uwep && uwep->oartifact == ART_LANCE_OF_LONGINUS) || \
				 (resists_drli(&youmonst) && !(u.nonintrinsicproperty == DRAIN_RES)) || (is_drain_resistant(youmonst.data) && !(u.nonintrinsicproperty == DRAIN_RES)) || ((moves % 3 == 0) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "deep cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "glubokiy plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "chuqur plash") ) ) || (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "vampiric cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "vampir plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "sindirishi plash") ) && (moves % 10 == 0) ) ) && !NoDrain_resistance)
#define NoDrain_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_DRAIN_RES].intrinsic || RngeAids || (u.nonextrinsicproperty == DRAIN_RES && !HDrain_resistance && !resists_drli(&youmonst) && !is_drain_resistant(youmonst.data) ) || (u.impossibleproperty == DRAIN_RES) || (uamul && uamul->oartifact == ART___TH_NAZGUL) || (uamul && uamul->oartifact == ART_NAZGUL_S_REVENGE) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

/* Intrinsics only */
#define HSick_resistance	u.uprops[SICK_RES].intrinsic
#define ESick_resistance	u.uprops[SICK_RES].extrinsic
#define Sick_resistance		(((HSick_resistance && !(u.nonintrinsicproperty == SICK_RES)) || ESick_resistance || (uarm && uarm->oartifact == ART_ANTISEPSIS_COAT) || (uarmc && uarmc->oartifact == ART_INA_S_LAB_COAT) || (uarmc && uarmc->oartifact == ART_INA_S_OVERCOAT) || (uarm && uarm->oartifact == ART_CHECK_YOUR_ESCAPES) || (uarmu && uarmu->oartifact == ART_THERMAL_BATH) || (uarmc && uarmc->oartifact == ART_CLEANLINESS_LAB) || (uarmf && uarmf->oartifact == ART_RHEA_S_COMBAT_PUMPS) || (uarmf && uarmf->oartifact == ART_BASE_FOR_SPEED_ASCENSION) || (uamul && uamul->oartifact == ART_ONE_MOMENT_IN_TIME) || (uleft && uleft->oartifact == ART_GOLDENIVY_S_RAGE) || (uright && uright->oartifact == ART_GOLDENIVY_S_RAGE) || (uleft && uleft->oartifact == ART_GOLDENIVY_S_ENGAGEMENT_RIN) || (uright && uright->oartifact == ART_GOLDENIVY_S_ENGAGEMENT_RIN) || (uamul && uamul->oartifact == ART_PRECIOUS_UNOBTAINABLE_PROP) || \
				 ((youmonst.data->mlet == S_FUNGUS || \
				 youmonst.data->mlet == S_MUMMY || \
				 youmonst.data->mlet == S_ZOMBIE || \
				 youmonst.data->mlet == S_WRAITH || \
				 youmonst.data->mlet == S_GHOST || \
				 youmonst.data->mlet == S_LICH) && !(u.nonintrinsicproperty == SICK_RES)) || \
				 defends(AD_DISE,uwep)) && !NoSick_resistance)
#define NoSick_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SICK_RES].intrinsic || RngeAids || (u.nonextrinsicproperty == SICK_RES && !HSick_resistance && !(youmonst.data->mlet == S_FUNGUS || youmonst.data->mlet == S_MUMMY || youmonst.data->mlet == S_ZOMBIE || youmonst.data->mlet == S_WRAITH || youmonst.data->mlet == S_GHOST || youmonst.data->mlet == S_LICH) ) || (u.impossibleproperty == SICK_RES) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))
#define Invulnerable		u.uprops[INVULNERABLE].intrinsic    /* [Tom] */

/* Extrinsics only */
#define HAntimagic		u.uprops[ANTIMAGIC].intrinsic
#define EAntimagic		u.uprops[ANTIMAGIC].extrinsic
#define Antimagic		(((HAntimagic && !(u.nonintrinsicproperty == ANTIMAGIC)) || EAntimagic || (uarmc && uarmc->oartifact == ART_ALL_IN_ONE_ASCENSION_KIT) || (uarmh && uarmh->oartifact == ART_YOU_ARE_ALREADY_DEAD) || (uarmg && uarmg->oartifact == ART_SPECTRATOR && (moves % 5 == 0) ) || (uarm && uarm->oartifact == ART_WEB_OF_LOLTH) || (uarm && uarm->oartifact == ART_ROFLCOPTER_WEB) || (u.antimagicshell && !(u.nonintrinsicproperty == ANTIMAGIC)) || (Role_if(PM_UNBELIEVER) && !(u.nonintrinsicproperty == ANTIMAGIC)) || have_magicresstone() || \
				 (Upolyd && resists_magm(&youmonst) && !(u.nonintrinsicproperty == ANTIMAGIC) )) && !NoAntimagic_resistance && !Race_if(PM_UNMAGIC_FISH))
#define NoAntimagic_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_ANTIMAGIC].intrinsic || RngeAids || (u.nonextrinsicproperty == ANTIMAGIC && !HAntimagic && !(Upolyd && resists_magm(&youmonst)) && !u.antimagicshell && !Role_if(PM_UNBELIEVER) ) || (u.impossibleproperty == ANTIMAGIC) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HAcid_resistance	u.uprops[ACID_RES].intrinsic
#define EAcid_resistance	u.uprops[ACID_RES].extrinsic
#define Acid_resistance		(((HAcid_resistance && !(u.nonintrinsicproperty == ACID_RES)) || EAcid_resistance || (uwep && uwep->oartifact == ART_ACTA_METALLURGICA_VOL___) || (uarmc && uarmc->oartifact == ART_ACIDSHOCK_CASTLECRUSHER) || (uarmg && uarmg->oartifact == ART_SPECTRATOR && (moves % 5 == 0) ) || (uwep && uwep->oartifact == ART_RESISTOMATIC) || (uwep && uwep->otyp == CHEMISTRY_SPACE_AXE) || (uarmh && uarmh->oartifact == ART_MASSIVE_IRON_CROWN_OF_MORG) || (uarmh && uarmh->oartifact == ART_WAR_MASK_OF_DURIN) || (uarmh && uarmh->oartifact == ART_UNIMPORTANT_ELEMENTS) || (resists_acid(&youmonst) && !(u.nonintrinsicproperty == ACID_RES)) || (is_acid_resistant(youmonst.data) && !(u.nonintrinsicproperty == ACID_RES)) ) && !NoAcid_resistance)
#define NoAcid_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_ACID_RES].intrinsic || RngeAids || (u.nonextrinsicproperty == ACID_RES && !HAcid_resistance && !resists_acid(&youmonst) && !is_acid_resistant(youmonst.data) ) || (u.impossibleproperty == ACID_RES) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HStone_resistance	u.uprops[STONE_RES].intrinsic
#define EStone_resistance	u.uprops[STONE_RES].extrinsic
#define Stone_resistance	(((HStone_resistance && !(u.nonintrinsicproperty == STONE_RES)) || EStone_resistance || (uarmh && uarmh->oartifact == ART_STONE_EROSION) || (uarmg && uarmg->oartifact == ART_SIGNONS_STEEL_TOTAL) || (uwep && uwep->oartifact == ART_GILRAEN_SEREGON) || (uarmf && uarmf->oartifact == ART_NATALIA_S_PUNISHER) || (uarmg && uarmg->oartifact == ART_COCKUETRY) || (uarmg && uarmg->oartifact == ART_SPECTRATOR && (moves % 5 == 0) ) || (uarmf && uarmf->oartifact == ART_CORINA_S_UNFAIR_SCRATCHER) || (uarmf && uarmf->oartifact == ART_PRETTY_ROOMMAID) || (uarmf && uarmf->oartifact == ART_NASTIST) || (uarmf && uarmf->oartifact == ART_YET_ANOTHER_STUPID_IDEA) || (uwep && uwep->oartifact == ART_HELIOKOPIS_S_WIZARDING_AID) || (uamul && uamul->oartifact == ART_PRECIOUS_UNOBTAINABLE_PROP) || (uarmf && uarmf->oartifact == ART_STONEWALL_CHECKERBOARD_DIS) || (uamul && uamul->oartifact == ART_ONE_MOMENT_IN_TIME) || (uamul && uamul->oartifact == ART_BALLSY_BASTARD) || (uarmh && uarmh->oartifact == ART_UNIMPORTANT_ELEMENTS) || (resists_ston(&youmonst) && !(u.nonintrinsicproperty == STONE_RES)) || (is_ston_resistant(youmonst.data) && !(u.nonintrinsicproperty == STONE_RES)) || (Role_if(PM_ROCKER) && !(u.nonintrinsicproperty == STONE_RES)) ) && !NoStone_resistance)
#define NoStone_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_STONE_RES].intrinsic || RngeAids || (u.nonextrinsicproperty == STONE_RES && !HStone_resistance && !resists_ston(&youmonst) && !is_ston_resistant(youmonst.data) && !Role_if(PM_ROCKER) ) || (u.impossibleproperty == STONE_RES) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HFear_resistance	u.uprops[FEAR_RES].intrinsic
#define EFear_resistance	u.uprops[FEAR_RES].extrinsic
#define Fear_resistance		(((HFear_resistance && !(u.nonintrinsicproperty == FEAR_RES)) || EFear_resistance || (uarmf && uarmf->oartifact == ART_GRANDPA_S_BROGUES) || (uarmf && uarmf->oartifact == ART_PARANOIA_STRIDE) ) || (uwep && uwep->oartifact == ART_USELESSNESS_OF_PLENTY) && !NoFear_resistance)
#define NoFear_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_FEAR_RES].intrinsic || RngeAids || (u.nonextrinsicproperty == FEAR_RES && !HFear_resistance) || (u.impossibleproperty == FEAR_RES) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))


/*** Troubles ***/
/* Pseudo-property */
#define Punished		(uball)

/* Those implemented solely as timeouts (we use just intrinsic) */
#define HStun			u.uprops[STUNNED].intrinsic
#define EStun			u.uprops[STUNNED].extrinsic
#define Stunned			(HStun || EStun || (uarms && uarms->oartifact == ART_VENOMAT) || (uwep && uwep->oartifact == ART_LANCE_OF_LONGINUS) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_LANCE_OF_LONGINUS) || (uwep && uwep->oartifact == ART_SCHWANZUS_LANGUS) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_SCHWANZUS_LANGUS) || (uarm && uarm->oartifact == ART_ROBE_OF_THE_ARCHMAGI) || (uarm && uarm->oartifact == ART_SHIVANHUNTER_S_UNUSED_PRIZ) || HeavyStunned || ((u.umonnum == PM_STALKER || youmonst.data->mlet == S_BAT) && !Race_if(PM_TRANSFORMER) ) || Race_if(PM_TUMBLRER) )
		/* Note: birds will also be stunned */
#define HeavyStunned		u.uprops[HEAVY_STUNNED].intrinsic

#define HConfusion		u.uprops[CONFUSION].intrinsic
#define EConfusion		u.uprops[CONFUSION].extrinsic
#define Confusion		(u.boomshined || HConfusion || EConfusion || HeavyConfusion || (uamul && uamul->oartifact == ART_CONFUSTICATOR) || (uarmh && uarmh->oartifact == ART_NEVER_CLEAN) || (uwep && uwep->oartifact == ART_EAMANE_LUINWE) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_EAMANE_LUINWE) || (uarmc && uarmc->oartifact == ART_VEIL_OF_LATONA) || (uarmh && uarmh->oartifact == ART_UNIMPLEMENTED_FEATURE) || (uwep && uwep->oartifact == ART_FEANARO_SINGOLLO) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_FEANARO_SINGOLLO) || (uarms && uarms->oartifact == ART_CCC_CCC_CCCCCCC) || (uarmc && uarmc->oartifact == ART_VEIL_OF_MINISTRY) || (uwep && uwep->oartifact == ART_LANCE_OF_LONGINUS) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_LANCE_OF_LONGINUS) || (uwep && uwep->oartifact == ART_SCHWANZUS_LANGUS) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_SCHWANZUS_LANGUS) || u.uprops[SENSORY_DEPRIVATION].extrinsic || Race_if(PM_ADDICT))
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
#define Frozen		(HFrozen || EFrozen || (uarmc && uarmc->oartifact == ART_BROKEN_WINGS) || (uarmc && uarmc->oartifact == ART_GROUNDBUMMER) || (uarms && uarms->oartifact == ART_TYPE_OF_ICE_BLOCK_HATES_YO) || (uwep && uwep->oartifact == ART_LANCE_OF_LONGINUS) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_LANCE_OF_LONGINUS) || (uwep && uwep->oartifact == ART_SCHWANZUS_LANGUS) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_SCHWANZUS_LANGUS) || HeavyFrozen)
#define HeavyFrozen		u.uprops[HEAVY_FROZEN].intrinsic

#define HBurned		u.uprops[BURNED].intrinsic
#define EBurned		u.uprops[BURNED].extrinsic
#define Burned		(HBurned || EBurned || HeavyBurned || Race_if(PM_BURNINATOR) || (uarms && uarms->oartifact == ART_BURNING_DISK) )
#define HeavyBurned		u.uprops[HEAVY_BURNED].intrinsic

#define HDimmed		u.uprops[DIMMED].intrinsic
#define EDimmed		u.uprops[DIMMED].extrinsic
#define Dimmed		(HDimmed || EDimmed || HeavyDimmed || Race_if(PM_INKA) || (uwep && uwep->otyp == TRAFFIC_LIGHT) )
#define HeavyDimmed		u.uprops[HEAVY_DIMMED].intrinsic

#define Blinded			u.uprops[BLINDED].intrinsic
#define EBlinded			u.uprops[BLINDED].extrinsic
#define Blindfolded		(ublindf && ublindf->otyp != LENSES && ublindf->otyp != RADIOGLASSES && ublindf->otyp != BOSS_VISOR && ublindf->otyp != CONDOME && ublindf->otyp != SOFT_CHASTITY_BELT)
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
#define IsGlib			(Glib || EGlib || (uarmg && uarmg->oartifact == ART_TANKS_A_LOT) || (uwep && uwep->oartifact == ART_FADED_USELESSNESS) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_FADED_USELESSNESS) )
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

#define DisconnectedStairs		u.uprops[DISCONNECTED_STAIRS].intrinsic
#define InterfaceScrewed		u.uprops[INTERFACE_SCREW].intrinsic
#define Bossfights		u.uprops[BOSSFIGHT].intrinsic
#define EntireLevelMode		u.uprops[ENTIRE_LEVEL].intrinsic
#define BonesLevelChange		u.uprops[BONES_CHANGE].intrinsic
#define AutocursingEquipment		u.uprops[AUTOCURSE_EQUIP].intrinsic
#define HighlevelStatus		u.uprops[HIGHLEVEL_STATUS].intrinsic
#define SpellForgetting		u.uprops[SPELL_FORGETTING].intrinsic
#define SoundEffectBug		u.uprops[SOUND_EFFECT_BUG].intrinsic
#define TimerunBug		u.uprops[TIMERUN_BUG].intrinsic

#define LootcutBug		u.uprops[LOOTCUT_BUG].intrinsic
#define MonsterSpeedBug		u.uprops[MONSTER_SPEED_BUG].intrinsic
#define ScalingBug		u.uprops[SCALING_BUG].intrinsic
#define EnmityBug			u.uprops[ENMITY_BUG].intrinsic
#define WhiteSpells		u.uprops[WHITE_SPELLS].intrinsic
#define CompleteGraySpells	u.uprops[COMPLETE_GRAY_SPELLS].intrinsic
#define QuasarVision		u.uprops[QUASAR_BUG].intrinsic
#define MommaBugEffect		u.uprops[MOMMA_BUG].intrinsic
#define HorrorBugEffect		u.uprops[HORROR_BUG].intrinsic
#define ArtificerBug		u.uprops[ARTIFICER_BUG].intrinsic
#define WereformBug		u.uprops[WEREFORM_BUG].intrinsic
#define NonprayerBug		u.uprops[NON_PRAYER_BUG].intrinsic
#define EvilPatchEffect		u.uprops[EVIL_PATCH_EFFECT].intrinsic
#define HardModeEffect		u.uprops[HARD_MODE_EFFECT].intrinsic
#define SecretAttackBug		u.uprops[SECRET_ATTACK_BUG].intrinsic
#define EaterBugEffect		u.uprops[EATER_BUG].intrinsic
#define CovetousnessBug		u.uprops[COVETOUSNESS_BUG].intrinsic
#define NotSeenBug		u.uprops[NOT_SEEN_BUG].intrinsic
#define DarkModeBug		u.uprops[DARK_MODE_BUG].intrinsic
#define AntisearchEffect	u.uprops[ANTISEARCH_EFFECT].intrinsic
#define HomicideEffect		u.uprops[HOMICIDE_EFFECT].intrinsic
#define NastynationBug		u.uprops[NASTY_NATION_BUG].intrinsic
#define WakeupCallBug		u.uprops[WAKEUP_CALL_BUG].intrinsic
#define GrayoutBug		u.uprops[GRAYOUT_BUG].intrinsic
#define GrayCenterBug		u.uprops[GRAY_CENTER_BUG].intrinsic
#define CheckerboardBug		u.uprops[CHECKERBOARD_BUG].intrinsic
#define ClockwiseSpinBug	u.uprops[CLOCKWISE_SPIN_BUG].intrinsic
#define CounterclockwiseSpin	u.uprops[COUNTERCLOCKWISE_SPIN_BUG].intrinsic
#define LagBugEffect		u.uprops[LAG_BUG].intrinsic
#define BlesscurseEffect	u.uprops[BLESSCURSE_EFFECT].intrinsic
#define DeLightBug		u.uprops[DE_LIGHT_BUG].intrinsic
#define DischargeBug		u.uprops[DISCHARGE_BUG].intrinsic
#define TrashingBugEffect	u.uprops[TRASHING_EFFECT].intrinsic
#define FilteringBug		u.uprops[FILTERING_BUG].intrinsic
#define DeformattingBug		u.uprops[DEFORMATTING_BUG].intrinsic
#define FlickerStripBug		u.uprops[FLICKER_STRIP_BUG].intrinsic
#define UndressingEffect	u.uprops[UNDRESSING_EFFECT].intrinsic
#define Hyperbluewalls		u.uprops[HYPERBLUEWALL_BUG].intrinsic
#define NoliteBug			u.uprops[NOLITE_BUG].intrinsic
#define ParanoiaBugEffect	u.uprops[PARANOIA_BUG].intrinsic
#define FleecescriptBug		u.uprops[FLEECESCRIPT_BUG].intrinsic
#define InterruptEffect		u.uprops[INTERRUPT_EFFECT].intrinsic
#define DustbinBug		u.uprops[DUSTBIN_BUG].intrinsic
#define ManaBatteryBug		u.uprops[MANA_BATTERY_BUG].intrinsic
#define Monsterfingers		u.uprops[MONSTERFINGERS_EFFECT].intrinsic
#define MiscastBug		u.uprops[MISCAST_BUG].intrinsic
#define MessageSuppression	u.uprops[MESSAGE_SUPPRESSION_BUG].intrinsic
#define StuckAnnouncement	u.uprops[STUCK_ANNOUNCEMENT_BUG].intrinsic
#define BloodthirstyEffect	u.uprops[BLOODTHIRSTY_EFFECT].intrinsic
#define MaximumDamageBug	u.uprops[MAXIMUM_DAMAGE_BUG].intrinsic
#define LatencyBugEffect	u.uprops[LATENCY_BUG].intrinsic
#define StarlitBug		u.uprops[STARLIT_BUG].intrinsic
#define KnowledgeBug		u.uprops[KNOWLEDGE_BUG].intrinsic
#define HighscoreBug		u.uprops[HIGHSCORE_BUG].intrinsic
#define PinkSpells		u.uprops[PINK_SPELLS].intrinsic
#define GreenSpells		u.uprops[GREEN_SPELLS].intrinsic
#define EvencoreEffect		u.uprops[EVC_EFFECT].intrinsic
#define UnderlayerBug		u.uprops[UNDERLAYER_BUG].intrinsic
#define DamageMeterBug		u.uprops[DAMAGE_METER_BUG].intrinsic
#define ArbitraryWeightBug	u.uprops[ARBITRARY_WEIGHT_BUG].intrinsic
#define FuckedInfoBug		u.uprops[FUCKED_INFO_BUG].intrinsic
#define BlackSpells		u.uprops[BLACK_SPELLS].intrinsic
#define CyanSpells		u.uprops[CYAN_SPELLS].intrinsic
#define HeapEffectBug		u.uprops[HEAP_EFFECT].intrinsic
#define BlueSpells		u.uprops[BLUE_SPELLS].intrinsic
#define TronEffect		u.uprops[TRON_EFFECT].intrinsic
#define RedSpells			u.uprops[RED_SPELLS].intrinsic
#define TooHeavyEffect		u.uprops[TOO_HEAVY_EFFECT].intrinsic
#define ElongationBug		u.uprops[ELONGATION_BUG].intrinsic
#define WrapoverEffect		u.uprops[WRAPOVER_EFFECT].intrinsic
#define DestructionEffect	u.uprops[DESTRUCTION_EFFECT].intrinsic
#define MeleePrefixBug		u.uprops[MELEE_PREFIX_BUG].intrinsic
#define AutomoreBug		u.uprops[AUTOMORE_BUG].intrinsic
#define UnfairAttackBug		u.uprops[UNFAIR_ATTACK_BUG].intrinsic

#define TimeStopped		u.uprops[TIME_STOPPED].intrinsic

#define FemaleTrapFemmy	u.uprops[FEMTRAP_FEMMY].intrinsic
#define FemaleTrapMadeleine	u.uprops[FEMTRAP_MADELEINE].intrinsic
#define FemaleTrapMarlena	u.uprops[FEMTRAP_MARLENA].intrinsic
#define FemaleTrapAnastasia	u.uprops[FEMTRAP_ANASTASIA].intrinsic
#define FemaleTrapJessica	u.uprops[FEMTRAP_JESSICA].intrinsic
#define FemaleTrapSolvejg	u.uprops[FEMTRAP_SOLVEJG].intrinsic
#define FemaleTrapWendy	u.uprops[FEMTRAP_WENDY].intrinsic
#define FemaleTrapKatharina	u.uprops[FEMTRAP_KATHARINA].intrinsic
#define FemaleTrapElena	u.uprops[FEMTRAP_ELENA].intrinsic
#define FemaleTrapThai	u.uprops[FEMTRAP_THAI].intrinsic
#define FemaleTrapElif	u.uprops[FEMTRAP_ELIF].intrinsic
#define FemaleTrapNadja	u.uprops[FEMTRAP_NADJA].intrinsic
#define FemaleTrapSandra	u.uprops[FEMTRAP_SANDRA].intrinsic
#define FemaleTrapNatalje	u.uprops[FEMTRAP_NATALJE].intrinsic
#define FemaleTrapJeanetta	u.uprops[FEMTRAP_JEANETTA].intrinsic
#define FemaleTrapYvonne	u.uprops[FEMTRAP_YVONNE].intrinsic
#define FemaleTrapMaurah	u.uprops[FEMTRAP_MAURAH].intrinsic

#define RngeIrregularity		(u.uprops[RNGE_IRREGULARITY].intrinsic || u.uprops[RNGE_IRREGULARITY].extrinsic)
#define RngeInternetAccess		(u.uprops[RNGE_INTERNET_ACCESS].intrinsic || u.uprops[RNGE_INTERNET_ACCESS].extrinsic)
#define RngeAntiGovernment		(u.uprops[RNGE_ANTI_GOVERNMENT].intrinsic || u.uprops[RNGE_ANTI_GOVERNMENT].extrinsic)
#define RngeGasFiltering		(u.uprops[RNGE_GAS_FILTERING].intrinsic || u.uprops[RNGE_GAS_FILTERING].extrinsic)
#define RngeVampiricDrain		(u.uprops[RNGE_VAMPIRIC_DRAIN].intrinsic || u.uprops[RNGE_VAMPIRIC_DRAIN].extrinsic)
#define RngePseudoIdentification		(u.uprops[RNGE_PSEUDO_IDENTIFICATION].intrinsic || u.uprops[RNGE_PSEUDO_IDENTIFICATION].extrinsic)
#define RngeGhostSummoning		(u.uprops[RNGE_GHOST_SUMMONING].intrinsic || u.uprops[RNGE_GHOST_SUMMONING].extrinsic)
#define RngePrayer		(u.uprops[RNGE_PRAYER].intrinsic || u.uprops[RNGE_PRAYER].extrinsic)
#define RngeAntiBeheading		(u.uprops[RNGE_ANTI_BEHEADING].intrinsic || u.uprops[RNGE_ANTI_BEHEADING].extrinsic)
#define RngePoland		(u.uprops[RNGE_POLAND].intrinsic || u.uprops[RNGE_POLAND].extrinsic)
#define RngeKicking		(u.uprops[RNGE_KICKING].intrinsic || u.uprops[RNGE_KICKING].extrinsic)
#define RngeToestomping		(u.uprops[RNGE_TOESTOMPING].intrinsic || u.uprops[RNGE_TOESTOMPING].extrinsic)
#define RngeJungleAction		(u.uprops[RNGE_JUNGLE_ACTION].intrinsic || u.uprops[RNGE_JUNGLE_ACTION].extrinsic)
#define RngeUnlikability		(u.uprops[RNGE_UNLIKABILITY].intrinsic || u.uprops[RNGE_UNLIKABILITY].extrinsic)
#define RngeSpellDisruption		(u.uprops[RNGE_SPELL_DISRUPTION].intrinsic || u.uprops[RNGE_SPELL_DISRUPTION].extrinsic)
#define RngeBDSM		(u.uprops[RNGE_BDSM].intrinsic || u.uprops[RNGE_BDSM].extrinsic)
#define RngeSlipping		(u.uprops[RNGE_SLIPPING].intrinsic || u.uprops[RNGE_SLIPPING].extrinsic)
#define RngeMirroring		(u.uprops[RNGE_MIRRORING].intrinsic || u.uprops[RNGE_MIRRORING].extrinsic)
#define RngeTrueSight		(u.uprops[RNGE_TRUE_SIGHT].intrinsic || u.uprops[RNGE_TRUE_SIGHT].extrinsic)
#define RngeLiquidDiet		(u.uprops[RNGE_LIQUID_DIET].intrinsic || u.uprops[RNGE_LIQUID_DIET].extrinsic)
#define RngeAngryGods		(u.uprops[RNGE_ANGRY_GODS].intrinsic || u.uprops[RNGE_ANGRY_GODS].extrinsic)
#define RngeRadioBroadcasts		(u.uprops[RNGE_RADIO_BROADCASTS].intrinsic || u.uprops[RNGE_RADIO_BROADCASTS].extrinsic)
#define RngeOccasionalDamage		(u.uprops[RNGE_OCCASIONAL_DAMAGE].intrinsic || u.uprops[RNGE_OCCASIONAL_DAMAGE].extrinsic)
#define RngeNoise		(u.uprops[RNGE_NOISE].intrinsic || u.uprops[RNGE_NOISE].extrinsic)
#define RngeVortices		(u.uprops[RNGE_VORTICES].intrinsic || u.uprops[RNGE_VORTICES].extrinsic)
#define RngeExplosions		(u.uprops[RNGE_EXPLOSIONS].intrinsic || u.uprops[RNGE_EXPLOSIONS].extrinsic)
#define RngeFatalAttraction		(u.uprops[RNGE_FATAL_ATTRACTION].intrinsic || u.uprops[RNGE_FATAL_ATTRACTION].extrinsic)
#define RngeVoltage		(u.uprops[RNGE_VOLTAGE].intrinsic || u.uprops[RNGE_VOLTAGE].extrinsic)
#define RngeWeeping		(u.uprops[RNGE_WEEPING].intrinsic || u.uprops[RNGE_WEEPING].extrinsic)
#define RngeCastlevania		(u.uprops[RNGE_CASTLEVANIA].intrinsic || u.uprops[RNGE_CASTLEVANIA].extrinsic)
#define RngeEngland		(u.uprops[RNGE_ENGLAND].intrinsic || u.uprops[RNGE_ENGLAND].extrinsic)
#define RngeForgetting		(u.uprops[RNGE_FORGETTING].intrinsic || u.uprops[RNGE_FORGETTING].extrinsic)
#define RngeAntimagicA		(u.uprops[RNGE_ANTIMAGIC_A].intrinsic || u.uprops[RNGE_ANTIMAGIC_A].extrinsic)
#define RngeAntimagicB		(u.uprops[RNGE_ANTIMAGIC_B].intrinsic || u.uprops[RNGE_ANTIMAGIC_B].extrinsic)
#define RngeAntimagicC		(u.uprops[RNGE_ANTIMAGIC_C].intrinsic || u.uprops[RNGE_ANTIMAGIC_C].extrinsic)
#define RngeAntimagicD		(u.uprops[RNGE_ANTIMAGIC_D].intrinsic || u.uprops[RNGE_ANTIMAGIC_D].extrinsic)
#define RngeChina		(u.uprops[RNGE_CHINA].intrinsic || u.uprops[RNGE_CHINA].extrinsic)
#define RngeLightAbsorption		(u.uprops[RNGE_LIGHT_ABSORPTION].intrinsic || u.uprops[RNGE_LIGHT_ABSORPTION].extrinsic)
#define RngeChildbirth		(u.uprops[RNGE_CHILDBIRTH].intrinsic || u.uprops[RNGE_CHILDBIRTH].extrinsic)
#define RngeRedAttunement		(u.uprops[RNGE_RED_ATTUNEMENT].intrinsic || u.uprops[RNGE_RED_ATTUNEMENT].extrinsic)
#define RngePolarOpposites		(u.uprops[RNGE_POLAR_OPPOSITES].intrinsic || u.uprops[RNGE_POLAR_OPPOSITES].extrinsic)
#define RngeSickness		(u.uprops[RNGE_SICKNESS].intrinsic || u.uprops[RNGE_SICKNESS].extrinsic)
#define RngeCoquetry		(u.uprops[RNGE_COQUETRY].intrinsic || u.uprops[RNGE_COQUETRY].extrinsic)
#define RngeIgnorance		(u.uprops[RNGE_IGNORANCE].intrinsic || u.uprops[RNGE_IGNORANCE].extrinsic)
#define RngeVengeance		(u.uprops[RNGE_VENGEANCE].intrinsic || u.uprops[RNGE_VENGEANCE].extrinsic)
#define RngeHighHeeling		(u.uprops[RNGE_HIGH_HEELING].intrinsic || u.uprops[RNGE_HIGH_HEELING].extrinsic)
#define RngeGravity		(u.uprops[RNGE_GRAVITY].intrinsic || u.uprops[RNGE_GRAVITY].extrinsic)
#define RngeWishImprovement		(u.uprops[RNGE_WISH_IMPROVEMENT].intrinsic || u.uprops[RNGE_WISH_IMPROVEMENT].extrinsic)
#define RngePocketMonsters		(u.uprops[RNGE_POCKET_MONSTERS].intrinsic || u.uprops[RNGE_POCKET_MONSTERS].extrinsic)
#define RngeLevuntation		(u.uprops[RNGE_LEVUNTATION].intrinsic || u.uprops[RNGE_LEVUNTATION].extrinsic)
#define RngePermanentOccupation		(u.uprops[RNGE_PERMANENT_OCCUPATION].intrinsic || u.uprops[RNGE_PERMANENT_OCCUPATION].extrinsic)
#define RngeNursery		(u.uprops[RNGE_NURSERY].intrinsic || u.uprops[RNGE_NURSERY].extrinsic)
#define RngeSlexuality		(u.uprops[RNGE_SLEXUALITY].intrinsic || u.uprops[RNGE_SLEXUALITY].extrinsic)
#define RngeAngband		(u.uprops[RNGE_ANGBAND].intrinsic || u.uprops[RNGE_ANGBAND].extrinsic)
#define RngeAnorexia		(u.uprops[RNGE_ANOREXIA].intrinsic || u.uprops[RNGE_ANOREXIA].extrinsic)
#define RngeDnethack		(u.uprops[RNGE_DNETHACK].intrinsic || u.uprops[RNGE_DNETHACK].extrinsic)
#define RngeUnnethack		(u.uprops[RNGE_UNNETHACK].intrinsic || u.uprops[RNGE_UNNETHACK].extrinsic)
#define RngeNethackFourk		(u.uprops[RNGE_NETHACK_FOURK].intrinsic || u.uprops[RNGE_NETHACK_FOURK].extrinsic)
#define RngeMaritalArts		(u.uprops[RNGE_MARITAL_ARTS].intrinsic || u.uprops[RNGE_MARITAL_ARTS].extrinsic)
#define RngeTeamSplat		(u.uprops[RNGE_TEAM_SPLAT].intrinsic || u.uprops[RNGE_TEAM_SPLAT].extrinsic)
#define RngeAbominations		(u.uprops[RNGE_ABOMINATIONS].intrinsic || u.uprops[RNGE_ABOMINATIONS].extrinsic)
#define RngePunishment		(u.uprops[RNGE_PUNISHMENT].intrinsic || u.uprops[RNGE_PUNISHMENT].extrinsic)
#define RngeBloodlust		(u.uprops[RNGE_BLOODLUST].intrinsic || u.uprops[RNGE_BLOODLUST].extrinsic)
#define RngeImmobility		(u.uprops[RNGE_IMMOBILITY].intrinsic || u.uprops[RNGE_IMMOBILITY].extrinsic)
#define RngeBossEncounters		(u.uprops[RNGE_BOSS_ENCOUNTERS].intrinsic || u.uprops[RNGE_BOSS_ENCOUNTERS].extrinsic)
#define RngeIronmanMode		(u.uprops[RNGE_IRONMAN_MODE].intrinsic || u.uprops[RNGE_IRONMAN_MODE].extrinsic)
#define RngeRivers		(u.uprops[RNGE_RIVERS].intrinsic || u.uprops[RNGE_RIVERS].extrinsic)
#define RngeLoudspeakers		(u.uprops[RNGE_LOUDSPEAKERS].intrinsic || u.uprops[RNGE_LOUDSPEAKERS].extrinsic)
#define RngeArmorPreservation		(u.uprops[RNGE_ARMOR_PRESERVATION].intrinsic || u.uprops[RNGE_ARMOR_PRESERVATION].extrinsic)
#define RngeWeakness		(u.uprops[RNGE_WEAKNESS].intrinsic || u.uprops[RNGE_WEAKNESS].extrinsic)
#define RngeFrailness		(u.uprops[RNGE_FRAILNESS].intrinsic || u.uprops[RNGE_FRAILNESS].extrinsic)
#define RngeIronMaiden		(u.uprops[RNGE_IRON_MAIDEN].intrinsic || u.uprops[RNGE_IRON_MAIDEN].extrinsic)
#define RngeDonors		(u.uprops[RNGE_DONORS].intrinsic || u.uprops[RNGE_DONORS].extrinsic)
#define RngeAids		(u.uprops[RNGE_AIDS].intrinsic || u.uprops[RNGE_AIDS].extrinsic)
#define RngeTrapAlert		(u.uprops[RNGE_TRAP_ALERT].intrinsic || u.uprops[RNGE_TRAP_ALERT].extrinsic)
#define RngeExtinction		(u.uprops[RNGE_EXTINCTION].intrinsic || u.uprops[RNGE_EXTINCTION].extrinsic)
#define RngeDefusing		(u.uprops[RNGE_DEFUSING].intrinsic || u.uprops[RNGE_DEFUSING].extrinsic)
#define RngeDenastification		(u.uprops[RNGE_DENASTIFICATION].intrinsic || u.uprops[RNGE_DENASTIFICATION].extrinsic)

/* Hallucination is solely a timeout; its resistance is extrinsic */
#define HHallucination		u.uprops[HALLUC].intrinsic
#define EHallucination		u.uprops[HALLUC].extrinsic
#define EHalluc_resistance	u.uprops[HALLUC_RES].extrinsic
#define Halluc_resistance	((EHalluc_resistance || \
				 (Upolyd && dmgtype(youmonst.data, AD_HALU)) || (Role_if(PM_TRANSSYLVANIAN) && uwep && (uwep->otyp == WEDGED_LITTLE_GIRL_SANDAL || uwep->otyp == SOFT_GIRL_SNEAKER || uwep->otyp == STURDY_PLATEAU_BOOT_FOR_GIRLS || uwep->otyp == HUGGING_BOOT || uwep->otyp == BLOCK_HEELED_COMBAT_BOOT || uwep->otyp == WOODEN_GETA || uwep->otyp == LACQUERED_DANCING_SHOE || uwep->otyp == HIGH_HEELED_SANDAL || uwep->otyp == SEXY_LEATHER_PUMP || uwep->otyp == SPIKED_BATTLE_BOOT || uwep->otyp == INKA_BOOT || uwep->otyp == SOFT_LADY_SHOE || uwep->otyp == STEEL_CAPPED_SANDAL || uwep->otyp == DOGSHIT_BOOT) ) ) && !u.halresdeactivated)
#define Hallucination		((HHallucination && !Halluc_resistance) || (u.uprops[MULTISHOES].extrinsic && !Halluc_resistance) || (uwep && uwep->oartifact == ART_LANCE_OF_LONGINUS) || (uwep && uwep->oartifact == ART_FADED_USELESSNESS) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_FADED_USELESSNESS) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_LANCE_OF_LONGINUS) || (uwep && uwep->oartifact == ART_SCHWANZUS_LANGUS) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_SCHWANZUS_LANGUS) || (HeavyHallu && !Halluc_resistance) || (EHallucination && !Halluc_resistance) || u.uprops[SENSORY_DEPRIVATION].extrinsic || flags.hippie || ( (u.uprops[DEHYDRATION].extrinsic || Dehydration || have_dehydratingstone()) && ((u.dehydrationtime - moves) < 1) )  )
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
#define Hunger			(HHunger || EHunger || (uarmc && uarmc->oartifact == ART_INA_S_LAB_COAT) || (uarmc && uarmc->oartifact == ART_INA_S_OVERCOAT) || (uleft && uleft->oartifact == ART_RING_OF_WOE) || (uarmh && uarmh->oartifact == ART_DRINK_COCA_COLA) || (uarmh && uarmh->oartifact == ART_HAMBURG_ONE) || (uarmc && uarmc->oartifact == ART_FULLY_LIONIZED) || (uarmc && uarmc->oartifact == ART_FAST_CAMO_PREDATOR) || (uright && uright->oartifact == ART_RING_OF_WOE) || (uwep && uwep->oartifact == ART_PICK_OF_THE_GRAVE) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_PICK_OF_THE_GRAVE) || Race_if(PM_GIGANT))


/*** Vision and senses ***/
#define HSee_invisible		u.uprops[SEE_INVIS].intrinsic
#define ESee_invisible		u.uprops[SEE_INVIS].extrinsic
#define See_invisible		(((HSee_invisible && !(u.nonintrinsicproperty == SEE_INVIS)) || ESee_invisible || (uarmc && uarmc->oartifact == ART_VISIBLE_INVISIBILITITY) || (uarmc && uarmc->oartifact == ART_CAN_T_KILL_WHAT_YOU_CAN_T_) || (uwep && uwep->oartifact == ART_SECRETS_OF_INVISIBLE_PLEAS) || \
				 (perceives(youmonst.data) && !(u.nonintrinsicproperty == SEE_INVIS)) ) && !Race_if(PM_ELEMENTAL) && !NoSee_invisible)
#define NoSee_invisible	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SEE_INVIS].intrinsic || RngeAids || (u.nonextrinsicproperty == SEE_INVIS && !HSee_invisible && !perceives(youmonst.data) ) || (u.impossibleproperty == SEE_INVIS) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HTelepat		u.uprops[TELEPAT].intrinsic
#define ETelepat		u.uprops[TELEPAT].extrinsic

#define Blind_telepat		( (((HTelepat && !(u.nonintrinsicproperty == TELEPAT)) || ETelepat || (Race_if(PM_DEVELOPER) && !(u.nonintrinsicproperty == TELEPAT)) || (telepathic(youmonst.data) && !(u.nonintrinsicproperty == TELEPAT)) ) && !NoTelepat && (!Role_if(PM_TOPMODEL) || !flags.female)) || ( Role_if(PM_TOPMODEL) && !u.powerfailure && !NoTelepat && PlayerInHighHeels ))

#define Unblind_telepat		((ETelepat || (Race_if(PM_DEVELOPER) && !(u.nonintrinsicproperty == TELEPAT)) ) && !NoTelepat)
#define NoTelepat	( (!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_TELEPAT].intrinsic || RngeAids || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0))) || (uarmc && uarmc->oartifact == ART_GODLESS_VOID) || (u.nonextrinsicproperty == TELEPAT && !HTelepat && !telepathic(youmonst.data) && !Race_if(PM_DEVELOPER) ) || (u.impossibleproperty == TELEPAT) || (uarmh && uarmh->oartifact == ART_WOLF_KING) || (uarmh && ( (uarmh->otyp == TINFOIL_HELMET) || (OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "aluminium helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "shlem iz alyuminiya") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "alyuminiy dubulg'a") ) ) ) ) )

#define HHallu_party	u.uprops[HALLU_PARTY].intrinsic
#define EHallu_party	u.uprops[HALLU_PARTY].extrinsic
#define Hallu_party	(((HHallu_party && !(u.nonintrinsicproperty == HALLU_PARTY)) || EHallu_party) && !NoHallu_party)
#define NoHallu_party	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_HALLU_PARTY].intrinsic || RngeAids || (u.nonextrinsicproperty == HALLU_PARTY && !HHallu_party) || (u.impossibleproperty == HALLU_PARTY) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HDrunken_boxing		u.uprops[DRUNKEN_BOXING].intrinsic
#define EDrunken_boxing		u.uprops[DRUNKEN_BOXING].extrinsic
#define Drunken_boxing		(((HDrunken_boxing && !(u.nonintrinsicproperty == DRUNKEN_BOXING)) || EDrunken_boxing) && !NoDrunken_boxing)
#define NoDrunken_boxing	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_DRUNKEN_BOXING].intrinsic || RngeAids || (u.nonextrinsicproperty == DRUNKEN_BOXING && !HDrunken_boxing) || (u.impossibleproperty == DRUNKEN_BOXING) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HStunnopathy	u.uprops[STUNNOPATHY].intrinsic
#define EStunnopathy	u.uprops[STUNNOPATHY].extrinsic
#define Stunnopathy	(((HStunnopathy && !(u.nonintrinsicproperty == STUNNOPATHY)) || EStunnopathy) && !NoStunnopathy)
#define NoStunnopathy	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_STUNNOPATHY].intrinsic || RngeAids || (u.nonextrinsicproperty == STUNNOPATHY && !HStunnopathy) || (u.impossibleproperty == STUNNOPATHY) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HNumbopathy	u.uprops[NUMBOPATHY].intrinsic
#define ENumbopathy	u.uprops[NUMBOPATHY].extrinsic
#define Numbopathy	(((HNumbopathy && !(u.nonintrinsicproperty == NUMBOPATHY)) || ENumbopathy) && !NoNumbopathy)
#define NoNumbopathy	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_NUMBOPATHY].intrinsic || RngeAids || (u.nonextrinsicproperty == NUMBOPATHY && !HNumbopathy) || (u.impossibleproperty == NUMBOPATHY) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HDimmopathy	u.uprops[DIMMOPATHY].intrinsic
#define EDimmopathy	u.uprops[DIMMOPATHY].extrinsic
#define Dimmopathy	(((HDimmopathy && !(u.nonintrinsicproperty == DIMMOPATHY)) || EDimmopathy) && !NoDimmopathy)
#define NoDimmopathy	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_DIMMOPATHY].intrinsic || RngeAids || (u.nonextrinsicproperty == DIMMOPATHY && !HDimmopathy) || (u.impossibleproperty == DIMMOPATHY) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HFreezopathy	u.uprops[FREEZOPATHY].intrinsic
#define EFreezopathy	u.uprops[FREEZOPATHY].extrinsic
#define Freezopathy	(((HFreezopathy && !(u.nonintrinsicproperty == FREEZOPATHY)) || EFreezopathy) && !NoFreezopathy)
#define NoFreezopathy	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_FREEZOPATHY].intrinsic || RngeAids || (u.nonextrinsicproperty == FREEZOPATHY && !HFreezopathy) || (u.impossibleproperty == FREEZOPATHY) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HStoned_chiller		u.uprops[STONED_CHILLER].intrinsic
#define EStoned_chiller		u.uprops[STONED_CHILLER].extrinsic
#define Stoned_chiller		(((HStoned_chiller && !(u.nonintrinsicproperty == STONED_CHILLER)) || EStoned_chiller) && !NoStoned_chiller)
#define NoStoned_chiller	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_STONED_CHILLER].intrinsic || RngeAids || (u.nonextrinsicproperty == STONED_CHILLER && !HStoned_chiller) || (u.impossibleproperty == STONED_CHILLER) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HCorrosivity	u.uprops[CORROSIVITY].intrinsic
#define ECorrosivity	u.uprops[CORROSIVITY].extrinsic
#define Corrosivity	(((HCorrosivity && !(u.nonintrinsicproperty == CORROSIVITY)) || ECorrosivity) && !NoCorrosivity)
#define NoCorrosivity	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_CORROSIVITY].intrinsic || RngeAids || (u.nonextrinsicproperty == CORROSIVITY && !HCorrosivity) || (u.impossibleproperty == CORROSIVITY) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HFear_factor	u.uprops[FEAR_FACTOR].intrinsic
#define EFear_factor	u.uprops[FEAR_FACTOR].extrinsic
#define Fear_factor	(((HFear_factor && !(u.nonintrinsicproperty == FEAR_FACTOR)) || EFear_factor) && !NoFear_factor)
#define NoFear_factor	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_FEAR_FACTOR].intrinsic || RngeAids || (u.nonextrinsicproperty == FEAR_FACTOR && !HFear_factor) || (u.impossibleproperty == FEAR_FACTOR) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HBurnopathy	u.uprops[BURNOPATHY].intrinsic
#define EBurnopathy	u.uprops[BURNOPATHY].extrinsic
#define Burnopathy	(((HBurnopathy && !(u.nonintrinsicproperty == BURNOPATHY)) || EBurnopathy) && !NoBurnopathy)
#define NoBurnopathy	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_BURNOPATHY].intrinsic || RngeAids || (u.nonextrinsicproperty == BURNOPATHY && !HBurnopathy) || (u.impossibleproperty == BURNOPATHY) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HSickopathy	u.uprops[SICKOPATHY].intrinsic
#define ESickopathy	u.uprops[SICKOPATHY].extrinsic
#define Sickopathy	(((HSickopathy && !(u.nonintrinsicproperty == SICKOPATHY)) || ESickopathy) && !NoSickopathy)
#define NoSickopathy	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SICKOPATHY].intrinsic || RngeAids || (u.nonextrinsicproperty == SICKOPATHY && !HSickopathy) || (u.impossibleproperty == SICKOPATHY) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HWonderlegs	u.uprops[WONDERLEGS].intrinsic
#define EWonderlegs	u.uprops[WONDERLEGS].extrinsic
#define Wonderlegs	(((HWonderlegs && !(u.nonintrinsicproperty == WONDERLEGS)) || EWonderlegs) && !NoWonderlegs)
#define NoWonderlegs	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_WONDERLEGS].intrinsic || RngeAids || (u.nonextrinsicproperty == WONDERLEGS && !HWonderlegs) || (u.impossibleproperty == WONDERLEGS) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HGlib_combat	u.uprops[GLIB_COMBAT].intrinsic
#define EGlib_combat	u.uprops[GLIB_COMBAT].extrinsic
#define Glib_combat	(((HGlib_combat && !(u.nonintrinsicproperty == GLIB_COMBAT)) || EGlib_combat) && !NoGlib_combat)
#define NoGlib_combat	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_GLIB_COMBAT].intrinsic || RngeAids || (u.nonextrinsicproperty == GLIB_COMBAT && !HGlib_combat) || (u.impossibleproperty == GLIB_COMBAT) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HManaleech	u.uprops[MANALEECH].intrinsic
#define EManaleech	u.uprops[MANALEECH].extrinsic
#define Manaleech		(((HManaleech && !(u.nonintrinsicproperty == MANALEECH)) || EManaleech || (uarmu && uarmu->oartifact == ART_VICTORIA_IS_EVIL_BUT_PRETT) || (uarmu && uarmu->oartifact == ART_NATALIA_IS_LOVELY_BUT_DANG) || (uarmf && uarmf->oartifact == ART_SOLVEJG_S_STINKING_SLIPPER) || (uarm && flags.female && uarm->oartifact == ART_PRETTY_LITTLE_MAGICAL_GIRL) || (uarmh && uarmh->oartifact == ART_REFUEL_BADLY) || (uamul && uamul->oartifact == ART_BALLSY_BASTARD) || (uamul && uamul->oartifact == ART_MADMAN_S_POWER) || (uamul && uamul->oartifact == ART___TH_NAZGUL) || (uamul && uamul->oartifact == ART_NAZGUL_S_REVENGE) ) && !NoManaleech)
#define NoManaleech	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_MANALEECH].intrinsic || RngeAids || (u.nonextrinsicproperty == MANALEECH && !HManaleech) || (u.impossibleproperty == MANALEECH) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HPeacevision	u.uprops[PEACEVISION].intrinsic
#define EPeacevision	u.uprops[PEACEVISION].extrinsic
#define Peacevision		(((HPeacevision && !(u.nonintrinsicproperty == PEACEVISION)) || EPeacevision) && !NoPeacevision)
#define NoPeacevision	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_PEACEVISION].intrinsic || RngeAids || (u.nonextrinsicproperty == PEACEVISION && !HPeacevision) || (u.impossibleproperty == PEACEVISION) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HMap_amnesia	u.uprops[MAP_AMNESIA].intrinsic
#define EMap_amnesia	u.uprops[MAP_AMNESIA].extrinsic
#define Map_amnesia	(HMap_amnesia || EMap_amnesia)

#define HWarning		u.uprops[WARNING].intrinsic
#define EWarning		u.uprops[WARNING].extrinsic
#define Warning			(( ((HWarning && !(u.nonintrinsicproperty == WARNING)) || EWarning) && !u.powerfailure && !NoWarning && (!Role_if(PM_TOPMODEL) || !flags.female)) || ( Role_if(PM_TOPMODEL) && !NoWarning && PlayerInHighHeels ))
#define NoWarning	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_WARNING].intrinsic || RngeAids || (u.nonextrinsicproperty == WARNING && !HWarning) || (u.impossibleproperty == WARNING) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

/* Warning for a specific type of monster */
#define HWarn_of_mon		u.uprops[WARN_OF_MON].intrinsic
#define EWarn_of_mon		u.uprops[WARN_OF_MON].extrinsic
#define Warn_of_mon		(HWarn_of_mon || EWarn_of_mon)

#define HUndead_warning		u.uprops[WARN_UNDEAD].intrinsic
#define EUndead_warning		u.uprops[WARN_UNDEAD].extrinsic
#define Undead_warning		(HUndead_warning || EUndead_warning || (uarmh && uarmh->oartifact == ART_ALL_SEEING_EYE_OF_THE_FLY) || (uarmh && uarmh->oartifact == ART_FLUE_FLUE_FLUEFLUE_FLUE))

#define HSearching		u.uprops[SEARCHING].intrinsic
#define ESearching		u.uprops[SEARCHING].extrinsic
#define Searching		(((HSearching && !(u.nonintrinsicproperty == SEARCHING)) || ESearching) && !NoSearching)
#define NoSearching	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SEARCHING].intrinsic || RngeAids || (u.nonextrinsicproperty == SEARCHING && !HSearching) || (u.impossibleproperty == SEARCHING) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HClairvoyant		u.uprops[CLAIRVOYANT].intrinsic
#define EClairvoyant		u.uprops[CLAIRVOYANT].extrinsic
#define BClairvoyant		u.uprops[CLAIRVOYANT].blocked
#define Clairvoyant		(((HClairvoyant && !(u.nonintrinsicproperty == CLAIRVOYANT)) || EClairvoyant || (uamul && uamul->oartifact == ART_WOUUU) || (uarmh && uarmh->otyp == ART_WSCHIIIIIE_) ) &&\
				 !BClairvoyant && !NoClairvoyant)
#define NoClairvoyant	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_CLAIRVOYANT].intrinsic || RngeAids || (u.nonextrinsicproperty == CLAIRVOYANT && !HClairvoyant) || (u.impossibleproperty == CLAIRVOYANT) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HInfravision		u.uprops[INFRAVISION].intrinsic
#define EInfravision		u.uprops[INFRAVISION].extrinsic
#define Infravision		(((HInfravision && !(u.nonintrinsicproperty == INFRAVISION)) || (uarmc && uarmc->oartifact == ART_GAGARIN_S_TRANSLATOR) || (uarmf && uarmf->oartifact == ART_ARVOGENIA_S_HIGH_HEELSES) || EInfravision || (uwep && uwep->oartifact == ART_DEMON_MACHINE) || \
				  (infravision(youmonst.data) && !(u.nonintrinsicproperty == INFRAVISION)) ) && !u.powerfailure && !NoInfravision)
#define NoInfravision	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_INFRAVISION].intrinsic || RngeAids || (u.nonextrinsicproperty == INFRAVISION && !HInfravision && !infravision(youmonst.data) ) || (u.impossibleproperty == INFRAVISION) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HDetect_monsters	u.uprops[DETECT_MONSTERS].intrinsic
#define EDetect_monsters	u.uprops[DETECT_MONSTERS].extrinsic
#define Detect_monsters		(((HDetect_monsters && !(u.nonintrinsicproperty == DETECT_MONSTERS)) || (uleft && uleft->oartifact == ART_ARABELLA_S_RADAR) || (uright && uright->oartifact == ART_ARABELLA_S_RADAR) || (uamul && uamul->oartifact == ART_SURTERSTAFF && uwep && (weapon_type(uwep) == P_QUARTERSTAFF) ) || EDetect_monsters || u.uprops[STORM_HELM].extrinsic) && !u.powerfailure && !NoDetect_monsters)
#define NoDetect_monsters	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_DETECT_MONSTERS].intrinsic || RngeAids || (u.nonextrinsicproperty == DETECT_MONSTERS && !HDetect_monsters) || (u.impossibleproperty == DETECT_MONSTERS) || (uarmh && uarmh->oartifact == ART_RADAR_NOT_WORKING) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))


/*** Appearance and behavior ***/
#define Adornment		u.uprops[ADORNED].extrinsic

#define HInvis			u.uprops[INVIS].intrinsic
#define EInvis			u.uprops[INVIS].extrinsic
#define BInvis			u.uprops[INVIS].blocked
#define Invis			(((HInvis && !(u.nonintrinsicproperty == INVIS)) || EInvis || (uwep && uwep->oartifact == ART_MOVE_IN_THE_SHADOWS) || (uwep && uwep->oartifact == ART_WHEREABOUT_OF_X) || (uarmc && uarmc->oartifact == ART_INVISIBLE_VISIBILITITY) || have_invisoloadstone() || (uarmh && uarmh->oartifact == ART_MARLENA_S_SONG) || (uarmh && uarmh->oartifact == ART_DULLIFIER) || (uarm && uarm->oartifact == ART_VERY_INVISIBLE) || (uarmc && uarmc->oartifact == ART_CAN_T_KILL_WHAT_YOU_CAN_T_) || (uwep && uwep->oartifact == ART_SECRETS_OF_INVISIBLE_PLEAS) || \
				 (pm_invisible(youmonst.data) && !(u.nonintrinsicproperty == INVIS)) ) && !BInvis && !Race_if(PM_ELEMENTAL) && !NoInvisible)
#define Invisible		(Invis && !See_invisible)
		/* Note: invisibility also hides inventory and steed */
#define NoInvisible	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_INVIS].intrinsic || RngeAids || (u.nonextrinsicproperty == INVIS && !HInvis && !pm_invisible(youmonst.data) ) || (u.impossibleproperty == INVIS) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HDisplaced		u.uprops[DISPLACED].intrinsic
#define EDisplaced		u.uprops[DISPLACED].extrinsic
#define Displaced		( (EDisplaced || (HDisplaced && !(u.nonintrinsicproperty == DISPLACED)) || (uarmc && uarmc->oartifact == ART_IMAGE_PROJECTOR) || (uwep && uwep->oartifact == ART_SKY_RENDER) || (uarm && uarm->oartifact == ART_VERY_INVISIBLE) || (uarmf && uarmf->oartifact == ART_WATER_FLOWERS) || (uwep && uwep->oartifact == ART_BELTHRONDING) || (uarm && uarm->oartifact == ART_ROBE_OF_THE_ARCHMAGI) || (uarmh && uarmh->oartifact == ART_MARLENA_S_SONG) || (uarm && uarm->oartifact == ART_SHIVANHUNTER_S_UNUSED_PRIZ) || (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "shrouded cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "okutana plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "kafan plash") ) && (moves % 10 == 0) ) || ((moves % 3 == 0) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "deep cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "glubokiy plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "chuqur plash") ) ) || (uarmf && OBJ_DESCR(objects[uarmf->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmf->otyp]), "korean sandals") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "koreyskiye sandalii") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "janubiy koreyaning kavushlari") ) && (moves % 3 == 0) ) ) && !NoDisplaced)
#define NoDisplaced	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_DISPLACED].intrinsic || RngeAids || (u.nonextrinsicproperty == DISPLACED && !HDisplaced) || (u.impossibleproperty == DISPLACED) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HStealth		u.uprops[STEALTH].intrinsic
#define EStealth		u.uprops[STEALTH].extrinsic
#define BStealth		u.uprops[STEALTH].blocked
#define Stealth			(( ((HStealth && !(u.nonintrinsicproperty == STEALTH)) || EStealth || u.uprops[MULTISHOES].extrinsic || (ublindf && ublindf->otyp == EYECLOSER) || ((Race_if(PM_MOON_ELF) && ((flags.moonphase >= 1 && flags.moonphase <= 3) || (flags.moonphase >= 5 && flags.moonphase <= 7) ) ) && !(u.nonintrinsicproperty == STEALTH)) ) && !BStealth && !NoStealth && !Race_if(PM_OGRO) && !(uwep && uwep->oartifact == ART_ARMORWREAKER) && !(u.twoweap && uswapwep && uswapwep->oartifact == ART_ARMORWREAKER) && !Race_if(PM_ROHIRRIM) && !Race_if(PM_THUNDERLORD) && (!Role_if(PM_TOPMODEL) || !flags.female) ) || ( Role_if(PM_TOPMODEL) && !NoStealth && PlayerInHighHeels ))
#define NoStealth	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_STEALTH].intrinsic || RngeAids || (u.nonextrinsicproperty == STEALTH && !HStealth && !(Race_if(PM_MOON_ELF) && ((flags.moonphase >= 1 && flags.moonphase <= 3) || (flags.moonphase >= 5 && flags.moonphase <= 7) ) ) ) || (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "excrement cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "ekskrementy plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "chiqindi plash") ) ) || (u.impossibleproperty == STEALTH) || (uarmf && OBJ_DESCR(objects[uarmf->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "heap of shit boots") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "kucha der'movykh sapog") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "boktan etik to'p") )) || (uarmf && uarmf->oartifact == ART_HENRIETTA_S_DOGSHIT_BOOTS) || FemaleTrapSolvejg || (uwep && uwep->oartifact == ART_HENRIETTA_S_MISTAKE) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_HENRIETTA_S_MISTAKE) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HExtra_wpn_practice	u.uprops[EXTRA_WEAPON_PRACTICE].intrinsic
#define EExtra_wpn_practice	u.uprops[EXTRA_WEAPON_PRACTICE].extrinsic
#define Extra_wpn_practice	(HExtra_wpn_practice || EExtra_wpn_practice || (uarmc && uarmc->oartifact == ART_SKILLS_BEAT_STATS) )

#define HDeath_resistance	u.uprops[DTBEEM_RES].intrinsic
#define EDeath_resistance	u.uprops[DTBEEM_RES].extrinsic
#define Death_resistance	(HDeath_resistance || EDeath_resistance || (uarmf && uarmf->oartifact == ART_MADELEINE_S_GIRL_FOOTSTEPS) || (uarms && uarms->oartifact == ART_ANTINSTANT_DEATH) )

/* according to Yasdorian, I love aggravate monster. Indeed, many of my artifacts have it. --Amy */
#define HAggravate_monster	u.uprops[AGGRAVATE_MONSTER].intrinsic
#define EAggravate_monster	u.uprops[AGGRAVATE_MONSTER].extrinsic
#define Aggravate_monster	(HAggravate_monster || EAggravate_monster || Race_if(PM_OGRO) || Race_if(PM_ROHIRRIM) || Race_if(PM_THUNDERLORD) || (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "avenger cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "mstitel' plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "qasoskor plash") )) || (uwep && uwep->oartifact == ART_ASIAN_WINTER) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_ASIAN_WINTER) || RngePunishment || (uwep && uwep->oartifact == ART_FN_M____PARA) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_FN_M____PARA) || (uwep && uwep->oartifact == ART_KILLER_PIANO) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_KILLER_PIANO) || (uwep && uwep->oartifact == ART_PICK_OF_THE_GRAVE) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_PICK_OF_THE_GRAVE) || FemaleTrapSolvejg || (uwep && uwep->oartifact == ART_CRUEL_PUNISHER) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_CRUEL_PUNISHER) || (uwep && uwep->oartifact == ART_SANDRA_S_EVIL_MINDDRILL) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_SANDRA_S_EVIL_MINDDRILL) || (uarmf && OBJ_DESCR(objects[uarmf->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "heap of shit boots") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "kucha der'movykh sapog") || !strcmp(OBJ_DESCR(objects[uarmf->otyp]), "boktan etik to'p") )) || (uarmu && uarmu->oartifact == ART_HELEN_S_DISCARDED_SHIRT) || (uarm && uarm->oartifact == ART_HO_OH_S_FEATHERS) || (uarm && uarm->oartifact == ART_QUEEN_ARTICUNO_S_HULL) || (uarm && uarm->oartifact == ART_DON_SUICUNE_DOES_NOT_APPRO) || (uarm && uarm->oartifact == ART_DON_SUICUNE_USED_SELFDESTR) || (uarmc && uarmc->oartifact == ART_BROKEN_WINGS) || (uarmc && uarmc->oartifact == ART_GROUNDBUMMER) || (uwep && uwep->oartifact == ART_SANDRA_S_SECRET_WEAPON) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_SANDRA_S_SECRET_WEAPON) || (uarmf && uarmf->oartifact == ART_VERY_NICE_PERSON) || (uarmf && uarmf->oartifact == ART_MAILIE_S_CHALLENGE) || (uwep && uwep->oartifact == ART_WHY_DO_YOU_HAVE_SUCH_A_LIT) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_WHY_DO_YOU_HAVE_SUCH_A_LIT) || (Role_if(PM_PSION) && u.ulevel >= 7) || (Race_if(PM_SYLPH) && u.ulevel >= 18) || (uarmf && uarmf->oartifact == ART_LARISSA_S_ANGER) || (uarmf && uarmf->oartifact == ART_CORINA_S_SNOWY_TREAD) || (uarm && uarm->oartifact == ART_NON_BLADETURNER) || (uarmc && uarmc->oartifact == ART_UBERJACKAL_EFFECT) || (uwep && uwep->oartifact == ART_MANUELA_S_PRACTICANT_TERRO) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_MANUELA_S_PRACTICANT_TERRO) || (uarmc && uarmc->oartifact == ART_HENRIETTA_S_HEAVY_CASTER) || (uarmf && uarmf->oartifact == ART_HENRIETTA_S_DOGSHIT_BOOTS) || (uarmh && uarmh->oartifact == ART_SEVERE_AGGRAVATION) || (uarms && uarms->oartifact == ART_SHATTERED_DREAMS) || (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "excrement cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "ekskrementy plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "chiqindi plash") ) ) || (uarms && uarms->oartifact == ART_NO_FUTURE_BUT_AGONY) || (uarmf && uarmf->oartifact == ART_CINDERELLA_S_SLIPPERS) || (uwep && uwep->oartifact == ART_GIRLFUL_BONKING) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_GIRLFUL_BONKING) || (uwep && uwep->oartifact == ART_HENRIETTA_S_MISTAKE) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_HENRIETTA_S_MISTAKE) || (uarmf && uarmf->oartifact == ART_ELLA_S_BLOODLUST) || (uarmf && uarmf->oartifact == ART_LOVELY_GIRL_PLATEAUS) || (uarmf && uarmf->oartifact == ART_SOLVEJG_S_STINKING_SLIPPER) || (uwep && uwep->oartifact == ART_RATTATTATTATTATT) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_RATTATTATTATTATT) || (uarmf && uarmf->oartifact == ART_I_M_A_BITCH__DEAL_WITH_IT) || (uarmf && uarmf->oartifact == ART_MANUELA_S_TORTURE_HEELS) || (uarmf && uarmf->oartifact == ART_MANUELA_S_UNKNOWN_HEELS) || (uarmf && uarmf->oartifact == ART_ALLYNONE) || (uarmf && uarmf->oartifact == ART_HERMES__UNFAIRNESS) || (uarmf && uarmf->oartifact == ART_HADES_THE_MEANIE) || (uarmf && uarmf->oartifact == ART_FUN_ALL_IN_ONE) || (uarmf && uarmf->oartifact == ART_PORCELAIN_ELEPHANT) || (uleft && uleft->oartifact == ART_RING_OF_WOE) || (uright && uright->oartifact == ART_RING_OF_WOE) || (uleft && uleft->oartifact == ART_GOLDENIVY_S_RAGE) || (uright && uright->oartifact == ART_GOLDENIVY_S_RAGE) || (uleft && uleft->oartifact == ART_GOLDENIVY_S_ENGAGEMENT_RIN) || (uright && uright->oartifact == ART_GOLDENIVY_S_ENGAGEMENT_RIN) || (uamul && uamul->oartifact == ART_SNOREFEST) || (uwep && uwep->oartifact == ART_ARMORWREAKER) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_ARMORWREAKER) || (uwep && uwep->oartifact == ART_HARKENSTONE) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_HARKENSTONE) || (uwep && uwep->oartifact == ART_KUSANAGI_NO_TSURUGI) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_KUSANAGI_NO_TSURUGI) || (uwep && uwep->oartifact == ART_ARABELLA_S_ARTIFACT_CREATI) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_ARABELLA_S_ARTIFACT_CREATI) || (Role_if(PM_PICKPOCKET) && u.ualign.record < 0) )

#define HConflict		u.uprops[CONFLICT].intrinsic
#define EConflict		u.uprops[CONFLICT].extrinsic
#define Conflict		((HConflict || EConflict || (uarm && uarm->oartifact == ART_HO_OH_S_FEATHERS) || (uarms && uarms->oartifact == ART_SHATTERED_DREAMS) || (uarms && uarms->oartifact == ART_NO_FUTURE_BUT_AGONY) || (uarms && uarms->oartifact == ART_SYSTEMATIC_CHAOS) || (uarms && uarms->oartifact == ART_BONUS_HOLD) || (uarmf && uarmf->oartifact == ART_MANUELA_S_TORTURE_HEELS) || (uarmf && uarmf->oartifact == ART_WEAK_FROM_HUNGER) || (uarmf && uarmf->oartifact == ART_MANUELA_S_UNKNOWN_HEELS) || (uarmc && uarmc->oartifact == ART_NOW_IT_S_FOR_REAL) || (uarmf && uarmf->oartifact == ART_ALLYNONE) || (uarmh && uarmh->oartifact == ART_CERTAIN_SLOW_DEATH) || (uarm && uarm->oartifact == ART_QUEEN_ARTICUNO_S_HULL) || (uwep && uwep->oartifact == ART_PROVOCATEUR) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_PROVOCATEUR) || (uwep && uwep->oartifact == ART_CIVIL_WAR) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_CIVIL_WAR) || (uarmf && uarmf->oartifact == ART_FATALITY) || (uarmf && uarmf->oartifact == ART_FUN_ALL_IN_ONE) || (uleft && uleft->oartifact == ART_RING_OF_WOE) || (uright && uright->oartifact == ART_RING_OF_WOE) ) && !Is_blackmarket(&u.uz) )
/* Sorry guys and girls, but you need to find another way to clear out Sam's assistants. --Amy */

/*** Transportation ***/
#define HJumping		u.uprops[JUMPING].intrinsic
#define EJumping		u.uprops[JUMPING].extrinsic
#define Jumping			(((HJumping && !(u.nonintrinsicproperty == JUMPING)) || EJumping || (uarmc && uarmc->oartifact == ART_ROKKO_CHAN_S_SUIT) || (uarm && uarm->oartifact == ART_LUKE_S_JEDI_POWER) || (uarmf && uarmf->oartifact == ART_PRINCE_OF_PERSIA) || (uwep && uwep->oartifact == ART_SUPERGIRL_S_JUMP_AND_RUN_F) || (uamul && uamul->oartifact == ART_ONLY_ONE_ESCAPE) || (flags.iwbtg && !(u.nonintrinsicproperty == JUMPING)) || u.uprops[MULTISHOES].extrinsic ) && !NoJumping)
#define NoJumping	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_JUMPING].intrinsic || RngeAids || (u.nonextrinsicproperty == JUMPING && !HJumping && !flags.iwbtg) || (u.impossibleproperty == JUMPING) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HTeleportation		u.uprops[TELEPORT].intrinsic
#define ETeleportation		u.uprops[TELEPORT].extrinsic
#define Teleportation		(HTeleportation || ETeleportation || (uarmh && uarmh->oartifact == ART_GO_OTHER_PLACE) || (uarmg && uarmg->oartifact == ART_OUT_OF_CONTROL) || (uarmh && uarmh->oartifact == ART_DOUBLE_JEOPARDY) || (uarms && uarms->oartifact == ART_REFLECTOR_EJECTOR) || (uwep && uwep->otyp == POKER_STICK) || (uarmf && uarmf->oartifact == ART_ERROR_IN_PLAY_ENCHANTMENT) || (uleft && uleft->oartifact == ART_HENRIETTA_S_MAGICAL_AID) || (uright && uright->oartifact == ART_HENRIETTA_S_MAGICAL_AID) || (uarmc && uarmc->oartifact == ART_SECANT_WHERELOCATION) || (uarmh && uarmh->oartifact == ART_RANDOMNESS_PREVAILS) || (uleft && uleft->oartifact == ART_GOLDENIVY_S_RAGE) || (uright && uright->oartifact == ART_GOLDENIVY_S_RAGE) || (uleft && uleft->oartifact == ART_GOLDENIVY_S_ENGAGEMENT_RIN) || (uright && uright->oartifact == ART_GOLDENIVY_S_ENGAGEMENT_RIN) || (uleft && uleft->oartifact == ART_WARPCHANGE) || (uright && uright->oartifact == ART_WARPCHANGE) || (uamul && uamul->oartifact == ART_SPACE_CYCLE) ||  \
				 can_teleport(youmonst.data) || Race_if(PM_MAIA) || Race_if(PM_HUMANOID_LEPRECHAUN))

#define HTeleport_control	u.uprops[TELEPORT_CONTROL].intrinsic
#define ETeleport_control	u.uprops[TELEPORT_CONTROL].extrinsic
#define Teleport_control	(((HTeleport_control && !(u.nonintrinsicproperty == TELEPORT_CONTROL)) || ETeleport_control || \
				 (control_teleport(youmonst.data) && !(u.nonintrinsicproperty == TELEPORT_CONTROL)) ) && !Race_if(PM_MAIA) && !(u.uprops[STORM_HELM].extrinsic) && !Race_if(PM_HUMANOID_LEPRECHAUN) && !NoTeleport_control)
#define NoTeleport_control	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_TELEPORT_CONTROL].intrinsic || RngeAids || (uarmc && uarmc->oartifact == ART_A_REASON_TO_LIVE) || (uleft && uleft->oartifact == ART_HENRIETTA_S_MAGICAL_AID) || (uright && uright->oartifact == ART_HENRIETTA_S_MAGICAL_AID) || (uarmh && uarmh->oartifact == ART_RANDOMNESS_PREVAILS) || (u.nonextrinsicproperty == TELEPORT_CONTROL && !HTeleport_control && !control_teleport(youmonst.data) ) || (u.impossibleproperty == TELEPORT_CONTROL) || (uarmg && uarmg->oartifact == ART_OUT_OF_CONTROL) || (uwep && uwep->otyp == POKER_STICK) || (uarmg && uarmg->oartifact == ART_ARABELLA_S_GREAT_BANISHER) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HLevitation		u.uprops[LEVITATION].intrinsic
#define ELevitation		u.uprops[LEVITATION].extrinsic
#define Levitation		(HLevitation || ELevitation || (uleft && uleft->oartifact == ART_FLOAT_EYELER_S_CONDITION) || (uright && uright->oartifact == ART_FLOAT_EYELER_S_CONDITION) || (uwep && uwep->oartifact == ART_IRON_BALL_OF_LEVITATION) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_IRON_BALL_OF_LEVITATION) || u.uprops[MULTISHOES].extrinsic || Race_if(PM_LEVITATOR) || \
				 (is_floater(youmonst.data) && !Race_if(PM_TRANSFORMER) ) )
	/* Can't touch surface, can't go under water; overrides all others */
#define Lev_at_will		(!Race_if(PM_LEVITATOR) && ((HLevitation & I_SPECIAL) != 0L || \
				 (ELevitation & W_ARTI) != 0L) && \
				 (HLevitation & ~(I_SPECIAL|TIMEOUT)) == 0L && \
				 (ELevitation & ~W_ARTI) == 0L && \
				 (!is_floater(youmonst.data) || Race_if(PM_TRANSFORMER) ) )

#define HFlying			u.uprops[FLYING].intrinsic
#define EFlying			u.uprops[FLYING].extrinsic
# define Flying			(( ((HFlying && !(u.nonintrinsicproperty == FLYING)) || EFlying || (uwep && uwep->oartifact == ART_PENGUIN_S_THRUSTING_SWORD) || (uarm && uarm->oartifact == ART_FLY_LIKE_AN_EAGLE) || (uarmc && uarmc->oartifact == ART_FULL_WINGS) || (uarmc && uarmc->oartifact == ART_DARK_ANGELS) || (uarmf && uarmf->oartifact == ART_MEPHISTO_S_BROGUES) || (uarmf && uarmf->oartifact == ART_CORINA_S_UNFAIR_SCRATCHER) || (uleft && uleft->oartifact == ART_GOLDENIVY_S_RAGE) || (uright && uright->oartifact == ART_GOLDENIVY_S_RAGE) || (uarmh && uarmh->oartifact == ART_GIRLFUL_FARTING_NOISES) || (uleft && uleft->oartifact == ART_GOLDENIVY_S_ENGAGEMENT_RIN) || (uarmc && uarmc->oartifact == ART_JANA_S_FAIRNESS_CUP) || (uright && uright->oartifact == ART_GOLDENIVY_S_ENGAGEMENT_RIN) || (uamul && uamul->oartifact == ART_STINGING_MEDALLION) || (uwep && uwep->oartifact == ART_SOURCE_CODES_OF_WORK_AVOID) || (uarmf && uarmf->oartifact == ART_RUTH_S_MORTAL_ENEMY) || (uamul && uamul->oartifact == ART_DIKKIN_S_DRAGON_TEETH) || (uarmg && uarmg->oartifact == ART_EXPERTENGAME_THE_ENTIRE_LE) || (uarmf && uarmf->oartifact == ART_EQUIPPED_FOR_TROUBLE) || (uarmg && uarmg->oartifact == ART_DIFFICULTY__) || (Race_if(PM_MOON_ELF) && flags.moonphase == FULL_MOON && !(u.nonintrinsicproperty == FLYING) ) || (uarmf && uarmf->oartifact == ART_GRENEUVENIA_S_HUG) || (uarmh && uarmh->oartifact == ART_FLUE_FLUE_FLUEFLUE_FLUE) || (is_flyer(youmonst.data) && !(u.nonintrinsicproperty == FLYING)) || (is_floater(youmonst.data) && Race_if(PM_TRANSFORMER) ) || (u.usteed && is_flyer(u.usteed->data) && !(u.nonintrinsicproperty == FLYING)) ) && !(uarm && uarm->oartifact == ART_WATER_SHYNESS) && !NoFlying && (!Role_if(PM_TOPMODEL) || !flags.female)) || ( Role_if(PM_TOPMODEL) && !(uarm && uarm->oartifact == ART_WATER_SHYNESS) && !NoFlying && PlayerInHighHeels ))
#define NoFlying	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_FLYING].intrinsic || RngeAids || (uarmc && uarmc->oartifact == ART_BROKEN_WINGS) || (uarmc && uarmc->oartifact == ART_GROUNDBUMMER) || (u.nonextrinsicproperty == FLYING && !HFlying && !(Race_if(PM_MOON_ELF) && flags.moonphase == FULL_MOON) && !is_flyer(youmonst.data) && !(u.usteed && is_flyer(u.usteed->data) ) ) || (u.impossibleproperty == FLYING) || (uarm && uarm->oartifact == ART_ARMOR_OF_EREBOR) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

	/* May touch surface; does not override any others */

#define Wwalking		( (u.uprops[WWALKING].extrinsic || u.uprops[MULTISHOES].extrinsic) && \
				 !Is_waterlevel(&u.uz))
	/* Don't get wet, can't go under water; overrides others except levitation */
	/* Wwalking is meaningless on water level */

#define HSwimming		u.uprops[SWIMMING].intrinsic
#define ESwimming		u.uprops[SWIMMING].extrinsic	/* [Tom] */
# define Swimming		((HSwimming || ESwimming || (uarmc && uarmc->oartifact == ART_WATERFORCE____) || (uarmc && uarmc->oartifact == ART_LAURA_S_SWIMSUIT) || (uarmc && uarmc->oartifact == ART_WATERS_OF_OBLIVION) || (uarmf && uarmf->oartifact == ART_SANDRA_S_BEAUTIFUL_FOOTWEA) || (uarmu && uarmu->oartifact == ART_THERMAL_BATH) || \
				 is_swimmer(youmonst.data) || \
				 (u.usteed && is_swimmer(u.usteed->data))) && !(uarm && uarm->oartifact == ART_WATER_SHYNESS) )
	/* Get wet, don't go under water unless if amphibious */

#define HMagical_breathing	u.uprops[MAGICAL_BREATHING].intrinsic
#define EMagical_breathing	u.uprops[MAGICAL_BREATHING].extrinsic
#define Amphibious		((HMagical_breathing || EMagical_breathing || (Race_if(PM_MOON_ELF) && flags.moonphase == NEW_MOON && !(u.nonintrinsicproperty == MAGICAL_BREATHING) ) || \
				 (amphibious(youmonst.data) && !(u.nonintrinsicproperty == MAGICAL_BREATHING)) ) && !NoBreathless)
	/* Get wet, may go under surface */

#define Breathless		( (((HMagical_breathing && !(u.nonintrinsicproperty == MAGICAL_BREATHING)) || EMagical_breathing || (uarmh && uarmh->oartifact == ART_SHPX_GUVF_FUVG) || (Role_if(PM_ANACHRONIST) && uwep && uwep->oartifact == ART_ANACHRONONONONAUT_PACKAGE) || (uarmf && uarmf->oartifact == ART_HERMES__UNFAIRNESS) || (uarmf && uarmf->oartifact == ART_HADES_THE_MEANIE) || (uarmc && uarmc->oartifact == ART_WATERFORCE____) || (uarmc && uarmc->oartifact == ART_JANA_S_SECRET_CAR) || (uarmc && uarmc->oartifact == ART_JANA_S_GRAVE_WALL) || (uarmc && uarmc->oartifact == ART_LAURA_S_SWIMSUIT) || (uarmf && uarmf->oartifact == ART_JANA_S_VAGINAL_FUN) || (uarmf && uarmf->oartifact == ART_SHORTFALL) || (uarmf && uarmf->oartifact == ART_SANDRA_S_BEAUTIFUL_FOOTWEA) || (uarmf && uarmf->oartifact == ART_FUN_ALL_IN_ONE) || (uarmf && uarmf->oartifact == ART_ALLYNONE) || (breathless(youmonst.data) && !(u.nonintrinsicproperty == MAGICAL_BREATHING)) ) && !NoBreathless && (!Role_if(PM_TOPMODEL) || !flags.female)) || ( Role_if(PM_TOPMODEL) && !NoBreathless && PlayerInHighHeels ))
#define NoBreathless	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_MAGICAL_BREATHING].intrinsic || RngeAids || (u.nonextrinsicproperty == MAGICAL_BREATHING && !HMagical_breathing && !(Race_if(PM_MOON_ELF) && flags.moonphase == NEW_MOON) && !amphibious(youmonst.data) && !breathless(youmonst.data) ) || (u.impossibleproperty == MAGICAL_BREATHING) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define Underwater		(u.uinwater)
/* Note that Underwater and u.uinwater are both used in code.
   The latter form is for later implementation of other in-water
   states, like swimming, wading, etc. */

#define HPasses_walls		u.uprops[PASSES_WALLS].intrinsic
#define EPasses_walls		u.uprops[PASSES_WALLS].extrinsic
#define Passes_walls		(((HPasses_walls && !(u.nonintrinsicproperty == PASSES_WALLS)) || EPasses_walls || \
				 (passes_walls(youmonst.data) && !(u.nonintrinsicproperty == PASSES_WALLS)) ) && !NoPasses_walls)
# define Phasing            u.uprops[PASSES_WALLS].intrinsic
#define NoPasses_walls	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_PASSES_WALLS].intrinsic || RngeAids || (u.nonextrinsicproperty == PASSES_WALLS && !HPasses_walls && !passes_walls(youmonst.data) ) || (u.impossibleproperty == PASSES_WALLS) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))


/*** Physical attributes ***/
#define HSlow_digestion		u.uprops[SLOW_DIGESTION].intrinsic
#define ESlow_digestion		u.uprops[SLOW_DIGESTION].extrinsic
#define Slow_digestion		(!Race_if(PM_GIGANT) && !NoSlow_digestion && ((HSlow_digestion && !(u.nonintrinsicproperty == SLOW_DIGESTION)) || ESlow_digestion || (uarmh && uarmh->oartifact == ART_NEVEREATER) || (uarmc && uarmc->oartifact == ART_FEMMY_FATALE) || (uarmh && uarmh->oartifact == ART_WOLF_KING) ) )  /* KMH */
#define NoSlow_digestion	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SLOW_DIGESTION].intrinsic || RngeAids || (u.nonextrinsicproperty == SLOW_DIGESTION && !HSlow_digestion) || (u.impossibleproperty == SLOW_DIGESTION) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

/* Half spell/physical damage only works 50% of the time, so it's more like three quarter spell/physical damage. --Amy */

#define HHalf_spell_damage	u.uprops[HALF_SPDAM].intrinsic
#define EHalf_spell_damage	u.uprops[HALF_SPDAM].extrinsic
#define Half_spell_damage	(((HHalf_spell_damage && !(u.nonintrinsicproperty == HALF_SPDAM)) || EHalf_spell_damage) && !NoHalf_spell_damage)
#define NoHalf_spell_damage	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_HALF_SPDAM].intrinsic || RngeAids || (u.nonextrinsicproperty == HALF_SPDAM && !HHalf_spell_damage) || (u.impossibleproperty == HALF_SPDAM) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HHalf_physical_damage	u.uprops[HALF_PHDAM].intrinsic
#define EHalf_physical_damage	u.uprops[HALF_PHDAM].extrinsic
#define Half_physical_damage	(((HHalf_physical_damage && !(u.nonintrinsicproperty == HALF_PHDAM)) || EHalf_physical_damage || (uwep && uwep->otyp == SECRET_WHIP) || (uarmc && (moves % 10 == 0) && OBJ_DESCR(objects[uarmc->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "soft cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "myagkiy plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "yumshoq plash") ) ) || (ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) ) && !NoHalf_physical_damage)
#define NoHalf_physical_damage	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_HALF_PHDAM].intrinsic || RngeAids || (u.nonextrinsicproperty == HALF_PHDAM && !HHalf_physical_damage) || (u.impossibleproperty == HALF_PHDAM) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define Second_chance		u.uprops[SECOND_CHANCE].extrinsic

#define HRegeneration		u.uprops[REGENERATION].intrinsic
#define ERegeneration		u.uprops[REGENERATION].extrinsic
#define Regeneration		(((HRegeneration && !(u.nonintrinsicproperty == REGENERATION)) || ERegeneration || (uarmf && uarmf->oartifact == ART_ERROR_IN_PLAY_ENCHANTMENT) || \
				 (regenerates(youmonst.data) && !(u.nonintrinsicproperty == REGENERATION)) ) && !NoRegeneration && !Race_if(PM_SYLPH) )
#define NoRegeneration	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_REGENERATION].intrinsic || RngeAids || (u.nonextrinsicproperty == REGENERATION && !HRegeneration && !regenerates(youmonst.data) ) || (u.impossibleproperty == REGENERATION) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HEnergy_regeneration	u.uprops[ENERGY_REGENERATION].intrinsic
#define EEnergy_regeneration	u.uprops[ENERGY_REGENERATION].extrinsic
#define Energy_regeneration	(((HEnergy_regeneration && !(u.nonintrinsicproperty == ENERGY_REGENERATION)) || EEnergy_regeneration || (Race_if(PM_REDGUARD) && !(u.nonintrinsicproperty == REGENERATION)) ) && !NoEnergy_regeneration && !Race_if(PM_SYLPH) )
#define NoEnergy_regeneration	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_ENERGY_REGENERATION].intrinsic || RngeAids || (u.nonextrinsicproperty == ENERGY_REGENERATION && !HEnergy_regeneration && !Race_if(PM_REDGUARD) ) || (u.impossibleproperty == ENERGY_REGENERATION) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

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
#define Polymorph_control	(((HPolymorph_control && !(u.nonintrinsicproperty == POLYMORPH_CONTROL)) || EPolymorph_control || (uarmu && uarmu->oartifact == ART_VICTORIA_IS_EVIL_BUT_PRETT) || (uarmu && uarmu->oartifact == ART_NATALIA_IS_LOVELY_BUT_DANG) || (uleft && uleft->oartifact == ART_HYPOCRITICAL_FUN) || (uright && uright->oartifact == ART_HYPOCRITICAL_FUN) || (uarmh && uarmh->oartifact == ART_DICTATORSHIP) ) && !Race_if(PM_MOULD) && !NoPolymorph_control && !Race_if(PM_TRANSFORMER) && !Race_if(PM_POLYINITOR) && !Race_if(PM_DESTABILIZER) && !Race_if(PM_WORM_THAT_WALKS) && !Race_if(PM_WARPER) && !Race_if(PM_MISSINGNO) && !Race_if(PM_UNGENOMOLD) && !Race_if(PM_DEATHMOLD) && !Race_if(PM_AK_THIEF_IS_DEAD_))
#define NoPolymorph_control	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic || RngeAids || (uarmc && uarmc->oartifact == ART_PERMANENTITIS) || (uarmc && uarmc->oartifact == ART_SHAPETAKE_NUMBER_FIVE) || (u.nonextrinsicproperty == POLYMORPH_CONTROL && !HPolymorph_control) || (u.impossibleproperty == POLYMORPH_CONTROL) || (uarmh && uarmh->oartifact == ART_RANDOMNESS_PREVAILS) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HUnchanging		u.uprops[UNCHANGING].intrinsic
#define EUnchanging		u.uprops[UNCHANGING].extrinsic
#define Unchanging		((HUnchanging || EUnchanging) && !(uarmc && uarmc->oartifact == ART_SHAPETAKE_NUMBER_FIVE) && !Race_if(PM_MOULD) && !Race_if(PM_MISSINGNO) && !Race_if(PM_TRANSFORMER) && !Race_if(PM_POLYINITOR) && !Race_if(PM_DESTABILIZER) && !Race_if(PM_WORM_THAT_WALKS) && !Race_if(PM_WARPER) && !Race_if(PM_UNGENOMOLD) && !Race_if(PM_DEATHMOLD) && !Race_if(PM_AK_THIEF_IS_DEAD_))	/* KMH */

#define HFast			u.uprops[FAST].intrinsic
#define EFast			u.uprops[FAST].extrinsic
#define Fast			( ((HFast && !(u.nonintrinsicproperty == FAST)) || EFast) && !NoFast && !Race_if(PM_DEVELOPER) && (!Role_if(PM_TRANSVESTITE) || flags.female) && (!Role_if(PM_TOPMODEL) || !flags.female) )
#define Very_fast		(( ( ((HFast & ~INTRINSIC) && !(u.nonintrinsicproperty == FAST)) || EFast || (uwep && uwep->oartifact == ART_FUMATA_YARI) || (uwep && uwep->oartifact == ART_INGRAM_MAC___) || (uarmc && uarmc->oartifact == ART_SPEEDRUNNER_S_DREAM) || (uarmh && uarmh->oartifact == ART_ELONA_S_SNAIL_TRAIL && Race_if(PM_SNAIL)) || (uarmh && uarmh->oartifact == ART_REAL_SPEED_DEVIL) || (uarmc && uarmc->oartifact == ART_ROKKO_CHAN_S_SUIT) || (uwep && uwep->oartifact == ART_EAMANE_LUINWE) || (uarmc && uarmc->oartifact == ART_FULLY_LIONIZED) || (uarmh && uarmh->oartifact == ART_LORSKEL_S_SPEED) || (uarmc && uarmc->oartifact == ART_JANA_S_FAIRNESS_CUP) || (uarmc && uarmc->oartifact == ART_FAST_CAMO_PREDATOR) || (uarmh && uarmh->oartifact == ART_ELESSAR_ELENDIL) || (uarmf && uarmf->oartifact == ART_UNTRAINED_HALF_MARATHON) || (uleft && uleft->oartifact == ART_REQUIRED_GLADNESS) || (uarmf && uarmf->oartifact == ART_VRRRRRRRRRRRR) || (uarmc && uarmc->oartifact == ART_FAST_SPEED_BUMP) || (uarmh && uarmh->oartifact == ART_DUE_DUE_DUE_DUE_BRMMMMMMM) || (uarmf && uarmf->oartifact == ART_UNEVEN_ENGINE) || (uwep && uwep->oartifact == ART_FEANARO_SINGOLLO) || (uarmc && uarmc->oartifact == ART_LIGHTSPEED_TRAVEL) || (uright && uright->oartifact == ART_REQUIRED_GLADNESS) || (uarmf && uarmf->oartifact == ART_HIGH_DESIRE_OF_FATALITY) || (uwep && uwep->oartifact == ART_SPEEDHACK) || (uarmc && uarmc->oartifact == ART_JANA_S_SECRET_CAR) || (uarmc && uarmc->oartifact == ART_JANA_S_GRAVE_WALL) || (uleft && uleft->oartifact == ART_CRIMINAL_QUEEN) || (uright && uright->oartifact == ART_CRIMINAL_QUEEN) || (uwep && uwep->oartifact == ART_TENSA_ZANGETSU) || (uwep && uwep->oartifact == ART_TARMAC_CHAMPION) || (uwep && uwep->oartifact == ART_ZANKAI_HUNG_ZE_TUNG_DO_HAI) || (uwep && uwep->oartifact == ART_GARNET_ROD) || (uwep && uwep->oartifact == ART_THREE_HEADED_FLAIL) || u.uprops[MULTISHOES].extrinsic ) && !Race_if(PM_DEVELOPER) && !NoFast && (!Role_if(PM_TRANSVESTITE) || flags.female) && (!Role_if(PM_TOPMODEL) || !flags.female) ) || ( (Role_if(PM_TRANSVESTITE) || Role_if(PM_TOPMODEL) ) && !NoFast && !Race_if(PM_DEVELOPER) && PlayerInHighHeels ))
#define NoFast	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_FAST].intrinsic || RngeAids || (u.nonextrinsicproperty == FAST && !HFast) || (u.impossibleproperty == FAST) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HReflecting		u.uprops[REFLECTING].intrinsic
#define EReflecting		u.uprops[REFLECTING].extrinsic
#define Reflecting		((EReflecting || (HReflecting && !(u.nonintrinsicproperty == REFLECTING)) || (moves % 2 == 0 && (uarm && OBJ_DESCR(objects[uarm->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarm->otyp]), "octarine robe") || !strcmp(OBJ_DESCR(objects[uarm->otyp]), "vos'moy tsvet khalata") || !strcmp(OBJ_DESCR(objects[uarm->otyp]), "sakkizinchi rang to'n") )) ) || (uarmu && uarmu->oartifact == ART_LEGENDARY_SHIRT) || (uarm && uarm->oartifact == ART_VOLUME_ARMAMENT) || (uarm && uarm->oartifact == ART_DON_SUICUNE_DOES_NOT_APPRO) || (ublindf && ublindf->otyp == DRAGON_EYEPATCH) || (uarmc && uarmc->oartifact == ART_A_REASON_TO_LIVE) || (uarmc && uarmc->oartifact == ART_ALL_IN_ONE_ASCENSION_KIT) || (uarmc && uarmc->oartifact == ART_RNG_S_GAMBLE) || (uarmh && uarmh->oartifact == ART_YOU_ARE_ALREADY_DEAD) || (uarmg && uarmg->oartifact == ART_PLUG_AND_PRAY) || (uarmg && uarmg->oartifact == ART_IRIS_S_PRECIOUS_METAL) || (uarmg && uarmg->oartifact == ART_SEALED_KNOWLEDGE) || (uarmg && uarmg->oartifact == ART_UNOBTAINABLE_BEAUTIES) || (is_reflector(youmonst.data) && !(u.nonintrinsicproperty == REFLECTING)) ) && !NoReflecting && !(uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "angband cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "plashch sredizem'ye krepost'") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "o'rta yer qal'a plash") )) && !Race_if(PM_ANGBANDER) && !RngeAngband )
#define NoReflecting	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_REFLECTING].intrinsic || RngeAids || (uarm && uarm->oartifact == ART_ARMOR_OF_EREBOR && (moves % 4 == 0) ) || (u.nonextrinsicproperty == REFLECTING && !HReflecting && !(youmonst.data == &mons[PM_SILVER_DRAGON] || is_reflector(youmonst.data) ) ) || (u.impossibleproperty == REFLECTING) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HFree_action	u.uprops[FREE_ACTION].intrinsic
#define EFree_action	u.uprops[FREE_ACTION].extrinsic
#define Free_action		(((HFree_action && !(u.nonintrinsicproperty == FREE_ACTION)) || EFree_action || (uwep && uwep->oartifact == ART_KARATE_KID) || (uarmu && uarmu->oartifact == ART_PEACE_ADVOCATE) || (uarm && uarm->oartifact == ART_LITTLE_BIG_MIDGET) || (uarmh && uarmh->oartifact == ART_LOW_LOCAL_MEMORY) || (uarmf && uarmf->oartifact == ART_KHOR_S_REQUIRED_IDEA) || (uarmc && uarmc->oartifact == ART_NOUROFIBROMA) || (uarmh && uarmh->oartifact == ART_LLLLLLLLLLLLLM) || (uarm && uarm->oartifact == ART_CHECK_YOUR_ESCAPES) || (uarms && uarms->oartifact == ART_LURTZ_S_WALL) || FemaleTrapNatalje || (uarms && uarms->oartifact == ART_BRASS_GUARD) || (uarmf && uarmf->oartifact == ART_ANASTASIA_S_GENTLENESS) || (uarms && uarms->oartifact == ART_I_M_GETTING_HUNGRY) || (uarm && uarm->oartifact == ART_ESSENTIALITY_EXTREME) || (uarmg && uarmg->oartifact == ART_FREE_ACTION_CALLED_FREE_AC) || (uarmc && uarmc->oartifact == ART_PREDATORY_STABILITY) || (uwep && uwep->oartifact == ART_IMMOBILASER) || (uarmf && uarmf->oartifact == ART_FREE_FOR_ENOUGH) || (uarmf && uarmf->oartifact == ART_MANDY_S_ROUGH_BEAUTY) || (uarmf && uarmf->oartifact == ART_WAITING_TIMEOUT) || (uamul && uamul->oartifact == ART_BALLSY_BASTARD) || (uamul && uamul->oartifact == ART___TH_NAZGUL) || (uamul && uamul->oartifact == ART_NAZGUL_S_REVENGE) || (uwep && uwep->oartifact == ART_SOURCE_CODES_OF_WORK_AVOID) ) && !NoFree_action) /* [Tom] */
#define NoFree_action	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_FREE_ACTION].intrinsic || RngeAids || (u.nonextrinsicproperty == FREE_ACTION && !HFree_action) || (u.impossibleproperty == FREE_ACTION) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define Fixed_abil		u.uprops[FIXED_ABIL].extrinsic	/* KMH */

#define Lifesaved		u.uprops[LIFESAVED].extrinsic

#define HKeen_memory		u.uprops[KEEN_MEMORY].intrinsic
#define EKeen_memory		u.uprops[KEEN_MEMORY].extrinsic
#define Keen_memory		(((HKeen_memory && !(u.nonintrinsicproperty == KEEN_MEMORY)) || EKeen_memory || (uarmu && uarmu->oartifact == ART_MEMORIAL_GARMENTS) || (uarmc && uarmc->oartifact == ART_MEMORY_AID) || (uarmc && uarmc->oartifact == ART_REMEMBERING_THE_BAD_TIMES) || (uarms && uarms->oartifact == ART_GOLDEN_DAWN) || (uarms && uarms->oartifact == ART_GREXIT_IS_NEAR) || (uamul && uamul->oartifact == ART_COMPUTER_AMULET) || (youmonst.data->mlet == S_QUADRUPED && !(u.nonintrinsicproperty == KEEN_MEMORY) )) && !NoKeen_memory)
#define NoKeen_memory	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_KEEN_MEMORY].intrinsic || RngeAids || (u.nonextrinsicproperty == KEEN_MEMORY && !HKeen_memory && !(youmonst.data->mlet == S_QUADRUPED) ) || (u.impossibleproperty == KEEN_MEMORY) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HSight_bonus		u.uprops[SIGHT_BONUS].intrinsic
#define ESight_bonus		u.uprops[SIGHT_BONUS].extrinsic
#define Sight_bonus		(((HSight_bonus && !(u.nonintrinsicproperty == SIGHT_BONUS)) || ESight_bonus || (uwep && uwep->oartifact == ART_PEOPLE_EATING_TRIDENT) || (uwep && uwep->otyp == FIRE_STICK) || (uwep && uwep->otyp == ZOOM_SHOT_CROSSBOW) || (uwep && uwep->oartifact == ART_AXE_OF_THE_DWARVISH_LORDS) || (uarms && uarms->oartifact == ART_SOLAR_POWER) || (uamul && uamul->oartifact == ART_GOOD_BEE) || (uarmf && uarmf->oartifact == ART_GRENEUVENIA_S_HUG) || (uamul && uamul->oartifact == ART_BUEING) || (uarmh && uarmh->oartifact == ART_HAT_OF_THE_ARCHMAGI) ) && !NoSight_bonus)
#define NoSight_bonus	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SIGHT_BONUS].intrinsic || RngeAids || (u.nonextrinsicproperty == SIGHT_BONUS && !HSight_bonus) || (u.impossibleproperty == SIGHT_BONUS) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HVersus_curses		u.uprops[VERSUS_CURSES].intrinsic
#define EVersus_curses		u.uprops[VERSUS_CURSES].extrinsic
#define Versus_curses		(((HVersus_curses && !(u.nonintrinsicproperty == VERSUS_CURSES)) || EVersus_curses || (uarmu && uarmu->oartifact == ART_MEMORIAL_GARMENTS)) && !NoVersus_curses)
#define NoVersus_curses	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_VERSUS_CURSES].intrinsic || RngeAids || (u.nonextrinsicproperty == VERSUS_CURSES && !HVersus_curses) || (u.impossibleproperty == VERSUS_CURSES) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HStun_resist		u.uprops[STUN_RES].intrinsic
#define EStun_resist		u.uprops[STUN_RES].extrinsic
#define Stun_resist		(((HStun_resist && !(u.nonintrinsicproperty == STUN_RES)) || EStun_resist || (uarmu && uarmu->oartifact == ART_TOTAL_CONTROL) || (uarm && uarm->oartifact == ART_PROTECTION_WITH_A_PRICE) || (uarmc && uarmc->oartifact == ART_FREQUENT_BUT_WEAK_STATUS) || (uarmh && uarmh->oartifact == ART_UBB_RUPTURE) || (uarmh && uarmh->oartifact == ART_MIND_SHIELDING) || (uarms && uarms->oartifact == ART_FIVE_STAR_PARTY) || (uamul && uamul->oartifact == ART_ONE_MOMENT_IN_TIME) || (uamul && uamul->oartifact == ART_PRECIOUS_UNOBTAINABLE_PROP) || (uamul && uamul->oartifact == ART_RECOVERED_RELIC) ) && !NoStun_resist)
#define NoStun_resist	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_STUN_RES].intrinsic || RngeAids || (u.nonextrinsicproperty == STUN_RES && !HStun_resist) || (u.impossibleproperty == STUN_RES) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HConf_resist		u.uprops[CONF_RES].intrinsic
#define EConf_resist		u.uprops[CONF_RES].extrinsic
#define Conf_resist		(((HConf_resist && !(u.nonintrinsicproperty == CONF_RES)) || EConf_resist || (uarmf && uarmf->oartifact == ART_SANDRA_S_BEAUTIFUL_FOOTWEA) || (uarms && uarms->oartifact == ART_FIVE_STAR_PARTY) || (uarmu && uarmu->oartifact == ART_TOTAL_CONTROL) || (uarmc && uarmc->oartifact == ART_FREQUENT_BUT_WEAK_STATUS) ) && !NoConf_resist)
#define NoConf_resist	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_CONF_RES].intrinsic || (uarmh && uarmh->oartifact == ART_UBB_RUPTURE) || RngeAids || (u.nonextrinsicproperty == CONF_RES && !HConf_resist) || (u.impossibleproperty == CONF_RES) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HPsi_resist		u.uprops[PSI_RES].intrinsic
#define EPsi_resist		u.uprops[PSI_RES].extrinsic
#define Psi_resist		(((HPsi_resist && !(u.nonintrinsicproperty == PSI_RES)) || EPsi_resist || (uwep && uwep->oartifact == ART_SWEETHEART_PUMP) || (uarms && uarms->oartifact == ART_REAL_PSYCHOS_WEAR_PURPLE) || (uarms && uarms->oartifact == ART_REAL_MEN_WEAR_PSYCHOS) || (uarmc && uarmc->oartifact == ART_INSANE_MIND_SCREW) || (uarmf && uarmf->oartifact == ART_JESSICA_S_TENDERNESS) || (uarmf && uarmf->oartifact == ART_MAILIE_S_CHALLENGE) || (uarmf && uarmf->oartifact == ART_BLUEDE) || (uarmc && uarmc->oartifact == ART_YOG_SOTHOTH_HELP_ME) || (uarmf && uarmf->oartifact == ART_JOSEFINE_S_EVILNESS) || (uarmh && uarmh->oartifact == ART_NUMBER___) || (uamul && uamul->oartifact == ART_ONE_MOMENT_IN_TIME) || (uwep && uwep->oartifact == ART_YASDORIAN_S_JUNETHACK_IDEN) || (uamul && uamul->oartifact == ART_PRECIOUS_UNOBTAINABLE_PROP) ) && !NoPsi_resist)
#define NoPsi_resist	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_PSI_RES].intrinsic || RngeAids || (u.nonextrinsicproperty == PSI_RES && !HPsi_resist) || (u.impossibleproperty == PSI_RES) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HDouble_attack		u.uprops[DOUBLE_ATTACK].intrinsic
#define EDouble_attack		u.uprops[DOUBLE_ATTACK].extrinsic
#define Double_attack		(((HDouble_attack && !(u.nonintrinsicproperty == DOUBLE_ATTACK)) || EDouble_attack || (uarm && uarm->oartifact == ART_OFFENSE_OWNS_DEFENSE) || (uarmf && uarmf->oartifact == ART_ELLA_S_BLOODLUST) || (uwep && uwep->oartifact == ART_MELEE_DUALITY) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_MELEE_DUALITY) ) && !NoDouble_attack)
#define NoDouble_attack	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_DOUBLE_ATTACK].intrinsic || RngeAids || (u.nonextrinsicproperty == DOUBLE_ATTACK && !HDouble_attack) || (u.impossibleproperty == DOUBLE_ATTACK) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HQuad_attack		u.uprops[QUAD_ATTACK].intrinsic
#define EQuad_attack		u.uprops[QUAD_ATTACK].extrinsic
#define Quad_attack		(((HQuad_attack && !(u.nonintrinsicproperty == QUAD_ATTACK)) || EQuad_attack || (uarmc && uarmc->oartifact == ART_SUPERMAN_S_SUPER_SUIT) ) && !NoQuad_attack)
#define NoQuad_attack	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_QUAD_ATTACK].intrinsic || RngeAids || (u.nonextrinsicproperty == QUAD_ATTACK && !HQuad_attack) || (u.impossibleproperty == QUAD_ATTACK) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HUseTheForce		u.uprops[THE_FORCE].intrinsic
#define EUseTheForce		u.uprops[THE_FORCE].extrinsic
#define UseTheForce		(((HUseTheForce && !(u.nonintrinsicproperty == THE_FORCE)) || EUseTheForce || (uarm && uarm->oartifact == ART_LUKE_S_JEDI_POWER) ) && !NoUseTheForce)
#define NoUseTheForce	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_THE_FORCE].intrinsic || RngeAids || (u.nonextrinsicproperty == THE_FORCE && !HUseTheForce) || (u.impossibleproperty == THE_FORCE) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#endif /* YOUPROP_H */
