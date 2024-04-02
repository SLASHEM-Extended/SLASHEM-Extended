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


#define AssholeModeActive	(flags.assholemode || PlayerOnBlacklist())

#define maybe_polyd(if_so,if_not)	(Upolyd ? (if_so) : (if_not))

#define HardcoreAlienMode	((Race_if(PM_HC_ALIEN) && flags.female) || (Race_if(PM_SLYER_ALIEN) && flags.female) || (uarmh && uarmh->oartifact == ART_HABIBA_S_MATRONAGE) )

#define CanHavePinkSpells (PinkSpells || u.uprops[PINK_SPELLS].extrinsic || have_pinkspellstone())
#define CanHaveBrightCyanSpells (BrightCyanSpells || u.uprops[BRIGHT_CYAN_SPELLS].extrinsic || have_brightcyanspellstone())
#define CanHaveCyanSpells (CyanSpells || u.uprops[CYAN_SPELLS].extrinsic || have_cyanspellstone() || autismweaponcheck(ART_FOOK_THE_OBSTACLES) )
#define CanHaveBlackSpells (BlackSpells || u.uprops[BLACK_SPELLS].extrinsic || have_blackspellstone() || autismweaponcheck(ART_EGRID_BUG) )
#define CanHaveOrangeSpells (OrangeSpells || u.uprops[ORANGE_SPELLS].extrinsic || have_orangespellstone() || autismweaponcheck(ART_GORMALER) )
#define CanHaveRedSpells (RedSpells || u.uprops[RED_SPELLS].extrinsic || have_redspellstone() || autismweaponcheck(ART_HELIOPOLIS_MISTAKE) || (uarmh && uarmh->oartifact == ART_DREHTURN) )
#define CanHavePlatinumSpells (PlatinumSpells || u.uprops[PLATINUM_SPELLS].extrinsic || have_platinumspellstone() || autismweaponcheck(ART_KRONSCHER_BAR))
#define CanHaveSilverSpells (SilverSpells || u.uprops[SILVER_SPELLS].extrinsic || have_silverspellstone() || autismweaponcheck(ART_PORKMAN_S_BALLS_OF_STEEL) || autismweaponcheck(ART_DIZZY_METAL_STORM) )
#define CanHaveMetalSpells (MetalSpells || u.uprops[METAL_SPELLS].extrinsic || have_metalspellstone())
#define CanHaveGreenSpells (GreenSpells || u.uprops[GREEN_SPELLS].extrinsic || have_greenspellstone() || (ublindf && ublindf->otyp == NIGHT_VISION_GOGGLES) )
#define CanHaveBlueSpells (BlueSpells || u.uprops[BLUE_SPELLS].extrinsic || have_bluespellstone() || autismweaponcheck(ART_STARRING_INFERNO) )
#define CanHaveGraySpells (CompleteGraySpells || u.uprops[COMPLETE_GRAY_SPELLS].extrinsic || have_greyoutstone())
#define CanHaveBrownSpells (BrownSpells || u.uprops[BROWN_SPELLS].extrinsic || have_brownspellstone() || (uarmf && uarmf->oartifact == ART_ARABELLA_S_GIRL_KICK) || (uarmf && uarmf->oartifact == ART_LORENZI_S_CLEANING_RESIDUE))
#define CanHaveWhiteSpells (WhiteSpells || u.uprops[WHITE_SPELLS].extrinsic || have_whitespellstone())
#define CanHaveVioletSpells (VioletSpells || u.uprops[VIOLET_SPELLS].extrinsic || have_violetspellstone())
#define CanHaveYellowSpells (YellowSpells || u.uprops[YELLOW_SPELLS].extrinsic || have_yellowspellstone() || (uamul && uamul->oartifact == ART_DIKKIN_S_DRAGON_TEETH && !(Role_if(PM_BARD) && Race_if(PM_KOBOLT) ) ) || (uarmc && uarmc->oartifact == ART_FAILSET_GAMBLE) || autismweaponcheck(ART_DIKKIN_S_DEADLIGHT) || autismweaponcheck(ART_DIKKIN_S_FAVORITE_SPELL))

#define SpellColorPink	(whichspellcolor() == 1)
#define SpellColorBrightCyan	(whichspellcolor() == 2)
#define SpellColorCyan	(whichspellcolor() == 3)
#define SpellColorBlack	(whichspellcolor() == 4)
#define SpellColorOrange	(whichspellcolor() == 5)
#define SpellColorRed	(whichspellcolor() == 6)
#define SpellColorPlatinum	(whichspellcolor() == 7)
#define SpellColorSilver	(whichspellcolor() == 8)
#define SpellColorMetal	(whichspellcolor() == 9)
#define SpellColorGreen	(whichspellcolor() == 10)
#define SpellColorBlue	(whichspellcolor() == 11)
#define SpellColorGray	(whichspellcolor() == 12)
#define SpellColorBrown	(whichspellcolor() == 13)
#define SpellColorWhite	(whichspellcolor() == 14)
#define SpellColorViolet	(whichspellcolor() == 15)
#define SpellColorYellow	(whichspellcolor() == 16)

#define EnglandMode	(RngeEngland || (uarmc && itemhasappearance(uarmc, APP_LONG_RANGE_CLOAK)) || (uarmu && uarmu->oartifact == ART_BEAM_MULTIPLIER) || (uwep && uwep->oartifact == ART_MINOLONG_ELBOW) || (uarmg && itemhasappearance(uarmg, APP_ENGLISH_GLOVES)) )

#define NastyTrapNation		(NastynationBug || u.uprops[NASTY_NATION_BUG].extrinsic || AssholeModeActive || have_multitrappingstone() || Role_if(PM_WALSCHOLAR) || (uarm && uarm->oartifact == ART_AMMY_S_RNG_CHANGER) || (uamul && uamul->oartifact == ART_ANASTASIA_S_LURE))
#define PlayerUninformation	(UninformationProblem || u.uprops[UNINFORMATION].extrinsic || have_uninformationstone() || (uarms && uarms->oartifact == ART_FIVE_STAR_PARTY))
#define CannotSelectItemsInPrompts	(InitializationFail || u.uprops[INITIALIZATION_FAIL].extrinsic || have_initializationstone())
#define Yawming	(YawmBug || autismweaponcheck(ART_ARABELLA_S_THINNER) || u.uprops[YAWM_EFFECT].extrinsic || have_yawmstone() || (uarm && uarm->oartifact == ART_EVERYTHING_COMES_WITH_A_CO) )
#define TezActive	(TezEffect || u.uprops[TEZ_EFFECT].extrinsic || have_tezstone() || (uarm && uarm->oartifact == ART_YOU_REALLY_HAVE_A_TOTAL_DA) )
#define EnthuActive	(EnthuEffect || u.uprops[ENTHU_EFFECT].extrinsic || have_bossgodstone() || (uarmu && uarmu->oartifact == ART_TINY_THREAD))
#define WallsAreNoFun	(NoFunWallsEffect || u.uprops[NO_FUN_WALLS].extrinsic || have_funwallstone())
#define CannotCureStatusEffects	(StatusTrapProblem || u.uprops[STATUS_FAILURE].extrinsic || have_statusstone() || (uarmf && uarmf->oartifact == ART_FENG_HUO_LUN) )

#define ManlerIsChasing	(ManlerEffect || u.uprops[MANLER_EFFECT].extrinsic || have_manlerstone() || autismweaponcheck(ART_DIZZY_METAL_STORM) || autismweaponcheck(ART_BAOBHAN_MOUNTAIN))

#define PlayerHearsMessages	(u.uprops[RANDOM_MESSAGES].extrinsic || RandomMessages || have_messagestone() || autismweaponcheck(ART_FILTHY_PRESS) || autismweaponcheck(ART_BARDICHE_ASSALT) )

#define ThereIsNoLite	(NoliteBug || (uarmg && itemhasappearance(uarmg, APP_TELESCOPE) && uarmg->cursed) || u.uprops[NOLITE_BUG].extrinsic || autismweaponcheck(ART_LIGHT_____STATED_) || (uarm && uarm->oartifact == ART_DARK_L) || have_nolightstone() || autismweaponcheck(ART_WEAKITE_THRUST) )

#define LLMMessages	(MemoryLoss || u.uprops[MEMORY_LOST].extrinsic || (uarmh && uarmh->oartifact == ART_LLLLLLLLLLLLLM) || (uarmh && uarmh->oartifact == ART_SUDUNSEL) || have_memorylossstone() )

#define YouDoNotGetDeathDrops		(u.uprops[NO_DROPS_EFFECT].extrinsic || NoDropsEffect || have_droplessstone() || (ublindf && ublindf->oartifact == ART_TOTAL_PERSPECTIVE_VORTEX) )

#define MessagesSuppressed	(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() || (uarmc && uarmc->oartifact == ART_DOEDOEDOEDOEDOEDOEDOE_TEST && grayoutobscuration()) )

#define FleeceyScripts	(FleecescriptBug || u.uprops[FLEECESCRIPT_BUG].extrinsic || have_fleecestone() || (uarmh && uarmh->oartifact == ART_TELEVISION_WONDER) || (uimplant && uimplant->oartifact == ART_IME_SPEW) )

#define CurseAsYouUse	(CurseuseEffect || u.uprops[CURSEUSE_EFFECT].extrinsic || have_curseusestone() || (uamul && uamul->oartifact == ART_ARABELLA_S_DICINATOR) )

#define EpviProblemActive	(EpviEffect || u.uprops[EPVI_EFFECT].extrinsic || have_epvistone() || (uarm && uarm->oartifact == ART_YOU_REALLY_HAVE_A_TOTAL_DA) )

#define AefdeActive	(AefdeEffect || u.uprops[AEFDE_EFFECT].extrinsic || have_aefdestone() || (uarm && uarm->oartifact == ART_BECAUSE_YOU_LOSE) )

#define InventoryDoesNotGo	(InventoryLoss || u.uprops[INVENTORY_LOST].extrinsic || (uarmh && uarmh->oartifact == ART_DEEP_INSANITY) || (uarmh && uarmh->oartifact == ART_FLAT_INSANITY) || have_inventorylossstone())

#define TopLineIsFleecy		(TopLineEffect || u.uprops[TOP_LINE_EFFECT].extrinsic || have_toplinestone())
#define DarkHanceScreen		(DarkhanceEffect || u.uprops[DARKHANCE_EFFECT].extrinsic || have_darkhancestone())

#define FailuresAreCostly	(CostlyFailureBug || u.uprops[COSTLY_FAILURE_BUG].extrinsic || have_costlyfailurestone())

#define YouTakeMaximumDamage		(MaximumDamageBug || u.uprops[MAXIMUM_DAMAGE_BUG].extrinsic || have_maximumdamagestone() || autismweaponcheck(ART_OUCHFIRE) || autismweaponcheck(ART_SCHWILLSCHWILLSCHWILLSCHWI) || autismweaponcheck(ART_TUNA_CANNON) || (uarmf && uarmf->oartifact == ART_EVIL_HAIRTEAR) )

#define YouBecameThickSkinned		( (uwep && uwep->oartifact == ART_ETRUSCIAN_SWIMMING_LESSON) || (uarmf && uarmf->oartifact == ART_ANTJE_S_POWERSTRIDE) || (uarmf && uarmf->oartifact == ART_THICK_FARTING_GIRL) || (uwep && uwep->oartifact == ART_PATRICIA_S_FEMININITY) || (uarms && uarms->oartifact == ART_FETTIS_SLOT) || (uarmf && uarmf->oartifact == ART_SIXX_PAXX) || FemtrapActivePatricia || Race_if(PM_DUTHOL) )

#define StupidMojibake		(MojibakeEffect || u.uprops[MOJIBAKE].extrinsic || have_mojibakestone() || (uimplant && uimplant->oartifact == ART_ND___NND_D___NDMD__DM_D_D_) || Race_if(PM_RELEASIER) || autismweaponcheck(ART_FLOATING_PARTICLE) )

#define AutomaticMorePrompt	(AutomoreBug || u.uprops[AUTOMORE_BUG].extrinsic || have_automorestone() || (uamul && uamul->oartifact == ART_HELIOKOPIS_S_KEYBOARD_CURS) )

#define DisplayDoesNotGo	(DisplayLoss || u.uprops[DISPLAY_LOST].extrinsic || have_displaystone() || (uarmc && uarmc->oartifact == ART_CLOAK_OF_THE_CONSORT && !(moves % 10 == 0)) )
#define DisplayDoesNotGoAtAll	(DisplayLoss || u.uprops[DISPLAY_LOST].extrinsic || have_displaystone() || (uarmc && uarmc->oartifact == ART_CLOAK_OF_THE_CONSORT) )
/* special case in dungeon.c and restore.c!!! */

#define PlayerHasGiantExplorer	(GiantExplorerBug || autismweaponcheck(ART_SIYID) || autismringcheck(ART_RING_OF_EVERYTHING_RESISTA) || (uarmg && uarmg->oartifact == ART_FARERTUBE) || u.uprops[GIANT_EXPLORER].extrinsic || have_giantexplorerstone())

#define FlimmeringStrips	(FlickerStripBug || u.uprops[FLICKER_STRIP_BUG].extrinsic || have_flickerstripstone() || (uarmh && uarmh->oartifact == ART_VIDEO_DECODER) || (uimplant && uimplant->oartifact == ART_IME_SPEW) )

#define TheInfoIsFucked	(FuckedInfoBug || u.uprops[FUCKED_INFO_BUG].extrinsic || have_infofuckstone())

#define FingerlessGloves	(uarmg && (itemhasappearance(uarmg, APP_FINGERLESS_GLOVES) || itemhasappearance(uarmg, APP_SILK_FINGERLINGS)) )

#define InventorySizeLimited		(InventorySizeBug || u.uprops[INVENTORY_SIZE_BUG].extrinsic || have_inventorysizestone())

#define PlayerHearsSoundEffects	(SoundEffectBug || (uarmf && uarmf->oartifact == ART_IS_THERE_A_SOUND_) || u.uprops[SOUND_EFFECT_BUG].extrinsic || haveartileash(ART_ANNOYING_PET_MONITOR) || (ublindf && ublindf->oartifact == ART_SOUNDTONE_FM) || have_soundeffectstone() || (uarmu && uarmu->oartifact == ART_CAPITAL_RAP) )

#define TronEffectIsActive	(TronEffect || u.uprops[TRON_EFFECT].extrinsic || have_tronstone())

#define RightMouseButtonDoesNotGo	(RMBLoss || u.uprops[RMB_LOST].extrinsic || (uarmh && uarmh->oartifact == ART_NO_RMB_VACATION) || (uamul && uamul->oartifact == ART_BUEING) || (uimplant && uimplant->oartifact == ART_ARABELLA_S_SEXY_CHARM) || (uarmh && uarmh->oartifact == ART_WOLF_KING) || (uamul && uamul->oartifact == ART_YOU_HAVE_UGH_MEMORY) || have_rmbstone())

#define WeightDisplayIsArbitrary	(ArbitraryWeightBug || u.uprops[ARBITRARY_WEIGHT_BUG].extrinsic || have_weightstone())

#define SkillTrainingImpossible	(PlayerCannotTrainSkills || u.uprops[TRAINING_DEACTIVATED].extrinsic || have_trainingstone() || (uarmf && uarmf->oartifact == ART_TEACH_YA_BUT_CANNOT_LEARN) )

#define MenuIsBugged	(MenuBug || u.uprops[MENU_LOST].extrinsic || have_menubugstone() || autismweaponcheck(ART_BERSERK_RAGE))

#define DetectionMethodsDontWork	(DetectationEffect || (uarmf && uarmf->oartifact == ART_BRITTA_S_MURDER_STORY) || u.uprops[DETECTATION_EFFECT].extrinsic || have_detectationstone() || (uarm && uarm->oartifact == ART_ARABELLA_S_LIGHTSWITCH) )

#define YouHaveBigscript	(BigscriptEffect || (uarmh && uarmh->oartifact == ART_YOU_SEE_HERE_AN_ARTIFACT) || u.uprops[BIGSCRIPT].extrinsic || have_bigscriptstone() )

#define YouGetLotsOfSanity	(SanityTrebleEffect || u.uprops[SANITY_TREBLE_EFFECT].extrinsic || have_sanitytreblestone())

#define RotThirteenCipher	(RotThirteen || u.uprops[ROT_THIRTEEN].extrinsic || have_rotthirteenstone() )

#define TheGameLaaaaags	(LatencyBugEffect || (uarmh && uarmh->oartifact == ART_COMPLETE_SIGHT && !Role_if(PM_MEDIUM)) || u.uprops[LATENCY_BUG].extrinsic || have_latencystone() || (uarmh && uarmh->oartifact == ART_WERKAUF) )

#define BloodthirstyAttacking	(BloodthirstyEffect || u.uprops[BLOODTHIRSTY_EFFECT].extrinsic || have_stormstone() || (uwep && uwep->otyp == TECPATL) || (u.twoweap && uswapwep && uswapwep->otyp == TECPATL) )

#define YouHaveTheSpeedBug	(Race_if(PM_PLAYER_MECHANIC) || SpeedBug || u.uprops[SPEED_BUG].extrinsic || (uarmf && uarmf->oartifact == ART_UNEVEN_ENGINE) || autismringcheck(ART_RING_OF_STEEL_DRAGON) || (uarm && uarm->oartifact == ART_HUNKSTERMAN) || (uarmf && uarmf->oartifact == ART_ERROR_IN_PLAY_ENCHANTMENT) || (uarmf && uarmf->oartifact == ART_BUGBOOTS_OF_ERRORNESS) || have_speedbugstone())

#define YouAreThirsty	(Thirst || u.uprops[THIRST].extrinsic || have_thirststone() || autismweaponcheck(ART_LUISA_S_CHARMING_BEAUTY))

#define PlayersRaysAreInaccurate	(InaccuracyBug || u.uprops[INACCURACY_BUG].extrinsic || have_inaccuracystone())

#define YouAreScrewedEternally (LongScrewup || u.uprops[LONG_SCREWUP].extrinsic || have_longscrewupstone())

#define GravationAggravation (GravationEffect || u.uprops[GRAVATION].extrinsic || have_gravationstone() || (uarm && uarm->oartifact == ART_ASTRA_MAIL) )

#define WallsAreHyperBlue (Hyperbluewalls || u.uprops[HYPERBLUEWALL_BUG].extrinsic || have_hyperbluestone() || autismweaponcheck(ART_KRONSCHER_BAR) || (uarm && uarm->oartifact == ART_CHIP_INFACT) || (uarms && uarms->oartifact == ART_DOLORES__VIRGINITY) || (uarms && uarms->oartifact == ART_BLUE_SHIRT_OF_DEATH) || (In_emynluin(&u.uz) && !u.emynluincomplete) )

#define GushieLushie	(GushlushEffect || u.uprops[GUSHLUSH].extrinsic || have_gushlushstone() || autismweaponcheck(ART_GIRLFUL_BONKING) )
#define GushLevel	(GushieLushie ? 1 : u.ulevel)

#define StealersActive	(u.uprops[ITEM_STEALING_EFFECT].extrinsic || ItemStealingEffect || have_stealerstone() || (uarmc && uarmc->oartifact == ART_PERCENTIOEOEPSPERCENTD_THI) || (uarmf && uarmf->oartifact == ART_SARAH_S_GRANNY_WEAR) || autismweaponcheck(ART_COPPERED_OFF_FROM_ME) || (uarmc && uarmc->oartifact == ART_STEALTH_OR_STEALING) || autismweaponcheck(ART_BAT_FROM_BALTIMORE) || autismweaponcheck(ART_ASHIKAGA_S_REVENGE) || (uarmf && uarmf->oartifact == ART_ALISEH_S_RED_COLOR) )

#define CannotTeleport	((uarm && uarm->oartifact == ART_CHECK_YOUR_ESCAPES) || (uamul && uamul->oartifact == ART_ARABELLA_S_SWOONING_BEAUTY) || (uimplant && uimplant->oartifact == ART_ND___NND_D___NDMD__DM_D_D_) || (uarmh && uarmh->oartifact == ART_FLYYYYY) || (uarmh && uarmh->oartifact == ART_HAVEWINGS) || NoReturnEffect || u.uprops[NORETURN].extrinsic || have_noreturnstone() || Race_if(PM_STABILISATOR))

#define YouAreDeaf	(Deafness || autismweaponcheck(ART_MEMETAL) || autismweaponcheck(ART_SONICBOOM) || autismweaponcheck(ART_SONICBOOM_BABY__LET_S_DO_T) || autismweaponcheck(ART_POWER_POLE) || autismweaponcheck(ART_VOICESHUTTER) || (uamul && uamul->oartifact == ART_UNKNOWN_SHELL) || autismweaponcheck(ART_GUN_OF_GENOCIDE) || (uarmf && uarmf->oartifact == ART_BEND_FROM_THE_NOISE) || (uarmf && uarmf->oartifact == ART_IS_THERE_A_SOUND_) || (uleft && uleft->otyp == RIN_AURORA) || (uright && uright->otyp == RIN_AURORA) || autismweaponcheck(ART_BANG_BANG) || u.uprops[DEAFNESS].extrinsic || have_deafnessstone() || (uarmh && uarmh->oartifact == ART_YITH_VERSUS_BABY) )

#define HaveTheAlignmentProblem	(AlignmentProblem || u.uprops[ALIGNMENT_FAILURE].extrinsic || (uarms && uarms->oartifact == ART_ALTERNATE_SPELLWEAVE) || have_alignmentstone() || autismweaponcheck(ART_PROFANED_GREATSCYTHE) || (uarmf && uarmf->oartifact == ART_LISSIE_S_SHEAGENTUR) || (uarmf && uarmf->oartifact == ART_JUEN_S_WEAKNESS) || (uimplant && uimplant->oartifact == ART_SINFUL_REPENTER) || (uarm && uarm->oartifact == ART_EITHER_INTELLIGENT_OR_FAIR && u.ualign.type == A_CHAOTIC) )

#define MonsterGlyphHallu	(MonsterGlyphChange || u.uprops[MONSTER_GLYPH_BUG].extrinsic || have_monsterglyphstone())

#define EscapePastIsBad	(EscapePastEffect || u.uprops[ESCAPE_PAST_EFFECT].extrinsic || have_escapepaststone() || autismweaponcheck(ART_HELIOKOPIS_S_PLAYSTYLE) )

#define PlayerCannotUseSkills	(AllSkillsUnskilled || u.uprops[SKILL_DEACTIVATED].extrinsic || (uarmc && uarmc->oartifact == ART_PALEOLITHIC_ELBOW_CONTRACT) || autismweaponcheck(ART_GIRLFUL_BONKING) || have_unskilledstone())

/* supernasty traps go here */

#define UnidentifyXtra	((have_unidentifystone() == 2))
#define CursingThingsXtra	((have_cursingstone() == 2))
#define DifficultyIncrXtra	((have_difficultystone() == 2))
#define AmnesiaRecurrXtra	((have_amnesiastone() == 2))
#define TechBugXtra	((have_techniquestone() == 2))
#define DisenchantRepXtra	((have_disenchantmentstone() == 2))
#define ChaosTerrainXtra	((have_chaosterrainstone() == 2))
#define UninformationXtra	((have_uninformationstone() == 2))
#define IntrinsicLossXtra	((have_intrinsiclossstone() == 2))
#define TrapCreationXtra	((have_trapcreationstone() == 2) || autismringcheck(ART_CROQUE_FORD) )
#define VulnerabilityXtra	((have_vulnerabilitystone() == 2))
#define ItemportingXtra	((have_itemportstone() == 2))
#define NastinessXtra	((have_nastystone() == 2))
#define LimitationXtra	((have_limitationstone() == 2) || (uarmh && uarmh->oartifact == ART_TEJUS__VACANCY) )
#define DropcurseXtra	((have_dropcursestone() == 2))
#define WallRegrowXtra	((have_wallstone() == 2) || (uarmh && uarmh->oartifact == ART_JABONE_S_COLOR_CHANGE) )
#define AutocursingXtra	((have_autocursestone() == 2))
#define HorrorEffectXtra	((have_horrorstone() == 2))
#define ArtificialXtra	((have_artificialstone() == 2))
#define EvilPatchEffXtra	((have_evilpatchstone() == 2))
#define SecretAttackXtra	((have_secretattackstone() == 2))
#define WakeupCallXtra	((have_wakeupcallstone() == 2))
#define GrayoutXtra	((have_grayoutstone() == 2))
#define GrayCenterXtra	((have_graycenterstone() == 2))
#define CheckerboardXtra	((have_checkerboardstone() == 2))
#define LagEffectXtra	((have_lagstone() == 2))
#define UndressingXtra	((have_undressingstone() == 2))
#define LatencyEffectXtra	((have_latencystone() == 2))
#define WrapOverXtra	((have_wrapoverstone() == 2) || (uarmc && uarmc->oartifact == ART_ISHITA_S_OVERWHELMING) )
#define DestructionEffXtra	((have_destructionstone() == 2))
#define UnfairAttackXtra	((have_unfairattackstone() == 2))
#define CursedPartXtra	((have_cursedpartstone() == 2) || (uimplant && uimplant->oartifact == ART_ARABELLA_S_EXCHANGER) )
#define FalloutEffectXtra	((have_falloutstone() == 2) || (uarmg && uarmg->oartifact == ART_SIRINE_S_MELLOW_LOOK) )
#define MojibakeXtra	((have_mojibakestone() == 2))
#define LongScrewupXtra	((have_longscrewupstone() == 2))
#define SkillLossXtra	((have_skilllossstone() == 2))
#define FluctuatingSpeedXtra	((have_fluctuatingspeedstone() == 2))
#define SatanEffectXtra	((have_satanstone() == 2))
#define ManlerXtra	((have_manlerstone() == 2))
#define DoorningXtra	((have_doorningstone() == 2))
#define MonNoiseXtra	((have_monnoisestone() == 2) || (uarm && uarm->oartifact == ART_ZURA_S_DRESSCODE) )
#define RangCallXtra	((have_rangcallstone() == 2))
#define RecurringSpellLossXtra	((have_recurringspelllossstone() == 2))
#define AntiTrainingXtra	((have_antitrainingstone() == 2))
#define TechoutXtra	((have_techoutstone() == 2) || (uamul && uamul->oartifact == ART_ARVIAT_S_LOAD) )
#define StatDecayXtra	((have_statdecaystone() == 2))
#define MoveMorkXtra	((have_movemorkstone() == 2) || (uarms && uarms->oartifact == ART_CAYLEEN_S_BLUSH) )
#define UbergodXtra	((have_ubergodstone() == 2))
#define FunwallXtra	((have_funwallstone() == 2))
#define CovidEffectXtra	((have_covidstone() == 2))
#define ProofLossXtra	((have_prooflossstone() == 2))
#define UnInvisXtra	((have_uninvisstone() == 2))
#define OptionChangeXtra	((have_optionstone() == 2))
#define UnKnowledgeXtra	((have_unknowledgestone() == 2))
#define DschueueuetXtra	((have_dschueueuetstone() == 2) || (uarmf && uarmf->oartifact == ART_PARTICULARLY_HEAVY_STONE) )
#define NopeskillXtra	((have_nopeskillstone() == 2))
#define FuckfuckfuckXtra	((have_fuckfuckfuckstone() == 2))
#define UmengXtra	((have_umengstone() == 2) || (uarmc && uarmc->oartifact == ART_WOLLOH_LENGWITSCH) )
#define SpellCooldownXtra	((have_spellcooldownstone() == 2))
#define TurbodullXtra	((have_turbodullstone() == 2))
#define InventorySizeXtra	((have_inventorysizestone() == 2))

/* There used to be long-ass macros here but I think they're causing out of memory errors, so they became functions --Amy */
#define PlayerInHighHeels	(playerwearshighheels())

#define PlayerInSexyFlats	(playerwearssexyflats())

#define CanHaveColumnarHeels	(maybecolumnarheels())

#define CanHaveStilettoHeels	(maybestilettoheels())

#define CanHaveConeHeels	(maybeconeheels())

#define CanHaveBlockHeels	(maybeblockheels())

#define CanHaveWedgeHeels	(maybewedgeheels())

#define PlayerInColumnarHeels	(CanHaveColumnarHeels)

#define PlayerInStilettoHeels	(!CanHaveColumnarHeels && CanHaveStilettoHeels)

#define PlayerInConeHeels	(!CanHaveColumnarHeels && (!CanHaveStilettoHeels || tech_inuse(T_NAUGHTY_HEELOT)) && CanHaveConeHeels)

#define PlayerInBlockHeels	(!CanHaveColumnarHeels && !CanHaveStilettoHeels && !CanHaveConeHeels && CanHaveBlockHeels)

#define PlayerInWedgeHeels	(!CanHaveColumnarHeels && !CanHaveStilettoHeels && !CanHaveConeHeels && !CanHaveBlockHeels && CanHaveWedgeHeels)

#define PirateSpeakOn	((uwep && uwep->oartifact == ART_ARRRRRR_MATEY) || (uwep && uwep->oartifact == ART_PIRATTERY))

/*** Resistances to troubles ***/
/* With intrinsics and extrinsics */
#define HFire_resistance	u.uprops[FIRE_RES].intrinsic
#define EFire_resistance	u.uprops[FIRE_RES].extrinsic
#define IntFire_resistance	(HFire_resistance || FireImmunity || (uarm && uarm->oartifact == ART_DUEUEUEUET) || (uarm && uarm->oartifact == ART_COAL_PEER) || (uarmf && uarmf->oartifact == ART______DEGREES) || resists_fire(&youmonst) || (uarm && uarm->oartifact == ART_COMPLETE_ARMORING) || (uarmf && uarmf->oartifact == ART_JOHN_S_REDBLOCK) || is_fire_resistant(youmonst.data) || (uwep && uwep->oartifact == ART_HARDENED_IN_THE_FORGE) || (uball && uball->oartifact == ART_HARDENED_IN_THE_FORGE) || (uwep && uwep->oartifact == ART_ONCHANGE_STAFF) || (uarms && uarms->oartifact == ART_HEATEM_UP) || (uarms && uarms->oartifact == ART_THATS_ENOUGH_RIGHT_THERE) || (powerfulsymbiosis() && is_fire_resistant(&mons[u.usymbiote.mnum])) )
#define ExtFire_resistance	(playerextrinsicfireres())

#define Fire_resistance		(((IntFire_resistance && u.nonintrinsicproperty != FIRE_RES) || (ExtFire_resistance && u.nonextrinsicproperty != FIRE_RES)) && !hybridragontype(AD_FIRE) && !Race_if(PM_TROLLOR) && !Race_if(PM_ANCIPITAL) && !Race_if(PM_SEA_ELF) && !Race_if(PM_SPRIGGAN) && !Role_if(PM_ALTMER) && !NoFire_resistance)
#define StrongFire_resistance	(IntFire_resistance && ExtFire_resistance && Fire_resistance && u.nondoubleproperty != FIRE_RES)

#define NoFire_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_FIRE_RES].intrinsic || UHaveAids || (uarm && uarm->oartifact == ART_SUPERESCAPE_MAIL) || (u.impossibleproperty == FIRE_RES) || (uarm && uarm->oartifact == ART_ABSOLUTE_MONSTER_MAIL) || autismweaponcheck(ART_ICEBLOCK_SWORD) || (uarm && uarm->oartifact == ART_FLEEING_MINE_MAIL) || (uarmf && uarmf->oartifact == ART_MEPHISTO_S_BROGUES) || (uarmf && uarmf->oartifact == ART_VERA_S_FREEZER) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define FireImmunity		((uarm && uarm->oartifact == ART_COAL_PEER) || (uwep && uwep->oartifact == ART_HARDENED_IN_THE_FORGE) || (uarm && uarm->otyp == ASBESTOS_JACKET) || (uarmf && uarmf->oartifact == ART______DEGREES) || (uarmh && uarmh->oartifact == ART_MAGMA_BRINGER) || (uwep && uwep->oartifact == ART_ONCHANGE_STAFF) || (uball && uball->oartifact == ART_HARDENED_IN_THE_FORGE) || (uarms && uarms->oartifact == ART_OGROTIC_CHANT) || (uarms && uarms->oartifact == ART_THATS_ENOUGH_RIGHT_THERE) || Race_if(PM_HYPOTHERMIC))

#define HCold_resistance	u.uprops[COLD_RES].intrinsic
#define ECold_resistance	u.uprops[COLD_RES].extrinsic
#define IntCold_resistance	(HCold_resistance || ColdImmunity || (uwep && uwep->oartifact == ART_CUTRELEASE) || resists_cold(&youmonst) || (uchain && uchain->oartifact == ART_HEI_THE_COLD) || (uwep && uwep->oartifact == ART_WINTERHOLD_S_KEEP) || (uarm && uarm->oartifact == ART_VERY_HOT_IN_HERE) || (uwep && uwep->oartifact == ART_COOLSHIELDING) || (uwep && uwep->oartifact == ART_GOOTE_UTE) || (uwep && uwep->oartifact == ART_COLD_HAHA) || (uwep && uwep->oartifact == ART_LAINCHANTMENT) || is_cold_resistant(youmonst.data) || (powerfulsymbiosis() && is_cold_resistant(&mons[u.usymbiote.mnum])))
#define ExtCold_resistance	(playerextrinsiccoldres())

#define Cold_resistance		(((IntCold_resistance && u.nonintrinsicproperty != COLD_RES) || (ExtCold_resistance && u.nonextrinsicproperty != COLD_RES)) && !Role_if(PM_ALTMER) && !hybridragontype(AD_COLD) && !NoCold_resistance)
#define StrongCold_resistance	(IntCold_resistance && ExtCold_resistance && Cold_resistance && u.nondoubleproperty != COLD_RES)

#define NoCold_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_COLD_RES].intrinsic || UHaveAids || (u.impossibleproperty == COLD_RES) || autismweaponcheck(ART_FLARE_FLAKE) || (uarms && uarms->oartifact == ART_HEATEM_UP) || autismringcheck(ART_DEATHLY_COLD) || autismringcheck(ART_BERNCELD) || Race_if(PM_PLAYER_SALAMANDER) || Race_if(PM_GAVIL) || Race_if(PM_JAVA) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define ColdImmunity		((uwep && uwep->oartifact == ART_CUTRELEASE) || (uchain && uchain->oartifact == ART_HEI_THE_COLD) || (uarmf && uarmf->oartifact == ART_FLEEC_LIE) || (uwep && uwep->oartifact == ART_LONG_SWORD_OF_ETERNITY) || (uarmf && uarmf->oartifact == ART_LOIS_S_CHILL) || (uarmf && uarmf->oartifact == ART_BRIGHT_WHITE) || (uwep && uwep->oartifact == ART_COLD_HAHA) || (uwep && uwep->oartifact == ART_GOOTE_UTE) || (uwep && uwep->oartifact == ART_COOLSHIELDING) || (uwep && uwep->oartifact == ART_WINTERHOLD_S_KEEP))

#define HSleep_resistance	u.uprops[SLEEP_RES].intrinsic
#define ESleep_resistance	u.uprops[SLEEP_RES].extrinsic
#define IntSleep_resistance	(HSleep_resistance || FemtrapActiveNatalje || resists_sleep(&youmonst) || is_sleep_resistant(youmonst.data) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_CLEAN_ASCENSION_RUN && In_endgame(&u.uz)) || (powerfulsymbiosis() && is_sleep_resistant(&mons[u.usymbiote.mnum])) )
#define ExtSleep_resistance	(playerextrinsicsleepres())

#define Sleep_resistance	( !(YouAreThirsty && (u.uhunger > 2000)) && !(uarmf && uarmf->otyp == SLEEP_INDUCTION_BOOTS) && !Race_if(PM_KOBOLT) && !Race_if(PM_DEEP_ELF) && !have_sleepstone() && !NoSleep_resistance && !hybridragontype(AD_SLEE) && ((IntSleep_resistance && u.nonintrinsicproperty != SLEEP_RES) || (ExtSleep_resistance && u.nonextrinsicproperty != SLEEP_RES)) )
#define StrongSleep_resistance	(IntSleep_resistance && ExtSleep_resistance && Sleep_resistance && u.nondoubleproperty != SLEEP_RES)

#define NoSleep_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SLEEP_RES].intrinsic || UHaveAids || (uarms && uarms->oartifact == ART_LITTLE_THORN_ROSE) || (u.impossibleproperty == SLEEP_RES) || (uamul && uamul->oartifact == ART_SPELLCASTER_S_DREAM) || (uarmh && uarmh->oartifact == ART_DULLIFIER) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HDisint_resistance	u.uprops[DISINT_RES].intrinsic
#define EDisint_resistance	u.uprops[DISINT_RES].extrinsic
#define IntDisint_resistance	(HDisint_resistance || resists_disint(&youmonst) || is_disint_resistant(youmonst.data) || (powerfulsymbiosis() && is_disint_resistant(&mons[u.usymbiote.mnum])) )
#define ExtDisint_resistance	(playerextrinsicdisintres())

#define Disint_resistance	(((IntDisint_resistance && u.nonintrinsicproperty != DISINT_RES) || (ExtDisint_resistance && u.nonextrinsicproperty != DISINT_RES)) && !hybridragontype(AD_DISN) && !NoDisint_resistance)
#define StrongDisint_resistance	(IntDisint_resistance && ExtDisint_resistance && Disint_resistance && u.nondoubleproperty != DISINT_RES)

#define NoDisint_resistance (!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_DISINT_RES].intrinsic || (uarmf && uarmf->oartifact == ART_SATAN_S_CHOPPING_BLOCK) || (uarm && uarm->oartifact == ART_NATAS_IS_BACK) || (uarmu && uarmu->oartifact == ART_TINY_THREAD) || (uarmh && uarmh->oartifact == ART_UBB_RUPTURE) || UHaveAids || (u.impossibleproperty == DISINT_RES) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HShock_resistance	u.uprops[SHOCK_RES].intrinsic
#define EShock_resistance	u.uprops[SHOCK_RES].extrinsic
#define IntShock_resistance	(HShock_resistance || (uarms && uarms->oartifact == ART_TEH_SHOCK) || ShockImmunity || resists_elec(&youmonst) || is_elec_resistant(youmonst.data) || (powerfulsymbiosis() && is_elec_resistant(&mons[u.usymbiote.mnum])) )
#define ExtShock_resistance	(playerextrinsicshockres())

#define Shock_resistance	(((IntShock_resistance && u.nonintrinsicproperty != SHOCK_RES) || (ExtShock_resistance && u.nonextrinsicproperty != SHOCK_RES)) && !hybridragontype(AD_ELEC) && !Role_if(PM_ALTMER) && !NoShock_resistance )
#define StrongShock_resistance	(IntShock_resistance && ExtShock_resistance && Shock_resistance && u.nondoubleproperty != SHOCK_RES)

#define NoShock_resistance (!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SHOCK_RES].intrinsic || (uarmf && uarmf->otyp == SHOCK_ISOLATED_HEELS) || UHaveAids || (u.impossibleproperty == SHOCK_RES) || (uarm && uarm->oartifact == ART_FIRE_COLD_AEFF) || autismweaponcheck(ART_SHOCKING_THERAPY) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define ShockImmunity		(Race_if(PM_PLAYER_DYNAMO) || (uwep && uwep->oartifact == ART_HEAVY_CROSSBOW_OF_ETERNITY) || (uarms && uarms->oartifact == ART_TEH_SHOCK) || (uarmg && uarmg->oartifact == ART_ENNUI_STOICISM) || (uarms && uarms->oartifact == ART_GROUNDED_FOREVER && objects[uarms->otyp].oc_material == MT_WOOD) )

#define HPoison_resistance	u.uprops[POISON_RES].intrinsic
#define EPoison_resistance	u.uprops[POISON_RES].extrinsic
#define IntPoison_resistance	(HPoison_resistance || Race_if(PM_SLYER_ALIEN) || resists_poison(&youmonst) || is_poison_resistant(youmonst.data) || (powerfulsymbiosis() && is_poison_resistant(&mons[u.usymbiote.mnum])) )
#define ExtPoison_resistance	(playerextrinsicpoisonres())

#define Poison_resistance	(((IntPoison_resistance && u.nonintrinsicproperty != POISON_RES) || (ExtPoison_resistance && u.nonextrinsicproperty != POISON_RES)) && !NoPoison_resistance && !hybridragontype(AD_DRST) && !Race_if(PM_POISONER) )
#define StrongPoison_resistance	(IntPoison_resistance && ExtPoison_resistance && Poison_resistance && u.nondoubleproperty != POISON_RES)

#define NoPoison_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_POISON_RES].intrinsic || UHaveAids || (u.impossibleproperty == POISON_RES) || autismweaponcheck(ART_SWORD_OF_BHELEU) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HDrain_resistance	u.uprops[DRAIN_RES].intrinsic
#define EDrain_resistance	u.uprops[DRAIN_RES].extrinsic
#define IntDrain_resistance	(HDrain_resistance || (uwep && uwep->oartifact == ART_VLADIMIR_BE_AFRAID) || (uarm && uarm->oartifact == ART_SEVEBREAKYOU__SEVEBREAK_) || (uarmh && uarmh->oartifact == ART_JABONE_S_COLOR_CHANGE) || resists_drli(&youmonst) || is_drain_resistant(youmonst.data) || (powerfulsymbiosis() && is_drain_resistant(&mons[u.usymbiote.mnum]) ) )
#define ExtDrain_resistance	(playerextrinsicdrainres())

#define Drain_resistance	(((IntDrain_resistance && u.nonintrinsicproperty != DRAIN_RES) || (ExtDrain_resistance && u.nonextrinsicproperty != DRAIN_RES)) && !NoDrain_resistance)
#define StrongDrain_resistance	(IntDrain_resistance && ExtDrain_resistance && Drain_resistance && u.nondoubleproperty != DRAIN_RES)

#define NoDrain_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_DRAIN_RES].intrinsic || UHaveAids || (u.impossibleproperty == DRAIN_RES) || (uamul && uamul->oartifact == ART___TH_NAZGUL) || (uamul && uamul->oartifact == ART_NAZGUL_S_REVENGE) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

/* Intrinsics only */
#define HSick_resistance	u.uprops[SICK_RES].intrinsic
#define ESick_resistance	u.uprops[SICK_RES].extrinsic
#define IntSick_resistance	(HSick_resistance || tech_inuse(T_STAT_RESIST) || youmonst.data->mlet == S_FUNGUS || youmonst.data->mlet == S_MUMMY || youmonst.data->mlet == S_ZOMBIE || youmonst.data->mlet == S_WRAITH || youmonst.data->mlet == S_GHOST || youmonst.data->mlet == S_LICH)
#define ExtSick_resistance	(playerextrinsicsickres())

#define Sick_resistance		(((IntSick_resistance && u.nonintrinsicproperty != SICK_RES) || (ExtSick_resistance && u.nonextrinsicproperty != SICK_RES)) && !NoSick_resistance)
#define StrongSick_resistance	(IntSick_resistance && ExtSick_resistance && Sick_resistance && u.nondoubleproperty != SICK_RES)

#define NoSick_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SICK_RES].intrinsic || UHaveAids || (u.impossibleproperty == SICK_RES) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define Invulnerable		u.uprops[INVULNERABLE].intrinsic    /* [Tom] */

/* Extrinsics only */
#define HAntimagic		u.uprops[ANTIMAGIC].intrinsic
#define EAntimagic		u.uprops[ANTIMAGIC].extrinsic
#define IntAntimagic	(HAntimagic || (uarmc && objects[uarmc->otyp].oc_material == MT_MENGETIUM) || tech_inuse(T_POWERFUL_AURA) || (irissetbonus() >= 3) || u.antimagicshell || (uarmh && uarmh->otyp == HELM_OF_ANTI_MAGIC) || Role_if(PM_UNBELIEVER) || (Upolyd && resists_magm(&youmonst)) )
#define ExtAntimagic	(EAntimagic || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == ANTIMAGIC) ) || (uarmc && uarmc->oartifact == ART_ALL_IN_ONE_ASCENSION_KIT) || (uarmf && uarmf->oartifact == ART_FIND_THE_SECRET_COURSE) || (uarmh && uarmh->oartifact == ART_YOU_ARE_ALREADY_DEAD) || (uwep && uwep->oartifact == ART_ORB_OF_FLEECE) || (uarmf && uarmf->oartifact == ART_NOW_YOU_ARE_HOWEVER_TO) || (have_oneinfourstone() && (moves % 4 == 1) ) || (uwep && uwep->oartifact == ART_ARK_OF_THE_COVENANT) || have_haveitallstone() || (uchain && uchain->oartifact == ART_TSCHEND_FOR_ETERNITY) || (uarmg && uarmg->oartifact == ART_SPECTRATOR && (moves % 5 == 0) ) || autismringcheck(ART_CORGON_S_RING) || (uarm && uarm->oartifact == ART_CHANGERING_ENVIROMENT) || (irissetbonus() >= 3) || (uarmh && uarmh->oartifact == ART_CROWN_OF_THE_PERCIPIENT) || (uarm && uarm->oartifact == ART_WEB_OF_LOLTH) || (uarm && uarm->oartifact == ART_WEB_OF_HOHOTH) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_SLEX_WANTS_YOU_TO_DIE_A_PA) || (uarm && uarm->oartifact == ART_ROFLCOPTER_WEB) || have_magicresstone() )

#define Antimagic		(((IntAntimagic && u.nonintrinsicproperty != ANTIMAGIC) || (ExtAntimagic && u.nonextrinsicproperty != ANTIMAGIC)) && !NoAntimagic_resistance && !hybridragontype(AD_MAGM) && !Race_if(PM_UNMAGIC_FISH))
#define StrongAntimagic	(IntAntimagic && ExtAntimagic && Antimagic && u.nondoubleproperty != ANTIMAGIC)

#define NoAntimagic_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_ANTIMAGIC].intrinsic || (UHaveAids && !(uwep && uwep->oartifact == ART_IT_S_A_POLEARM)) || (u.impossibleproperty == ANTIMAGIC) || autismweaponcheck(ART_AMYSBANE) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HAcid_resistance	u.uprops[ACID_RES].intrinsic
#define EAcid_resistance	u.uprops[ACID_RES].extrinsic
#define IntAcid_resistance	(HAcid_resistance || AcidImmunity || (uarmf && uarmf->oartifact == ART_POCAHONTAS_HOME) || (uarm && uarm->oartifact == ART_NULARMOR) || (uball && uball->oartifact == ART_ACICORNER) || resists_acid(&youmonst) || is_acid_resistant(youmonst.data) || (powerfulsymbiosis() && is_acid_resistant(&mons[u.usymbiote.mnum])) )
#define ExtAcid_resistance	(playerextrinsicacidres())

#define Acid_resistance		(((IntAcid_resistance && u.nonintrinsicproperty != ACID_RES) || (ExtAcid_resistance && u.nonextrinsicproperty != ACID_RES)) && !hybridragontype(AD_ACID) && !NoAcid_resistance)
#define StrongAcid_resistance	(IntAcid_resistance && ExtAcid_resistance && Acid_resistance && u.nondoubleproperty != ACID_RES)

#define NoAcid_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_ACID_RES].intrinsic || UHaveAids || (u.impossibleproperty == ACID_RES) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define AcidImmunity		((uarm && uarm->oartifact == ART_NULARMOR) || (uarmf && uarmf->oartifact == ART_LISA_S_YELLOW_STRIPE) || (uarmf && uarmf->oartifact == ART_HEIKE_S_BIGOTRY) || (uball && uball->oartifact == ART_ACICORNER) || (uarmf && uarmf->oartifact == ART_POCAHONTAS_HOME))

#define HStone_resistance	u.uprops[STONE_RES].intrinsic
#define EStone_resistance	u.uprops[STONE_RES].extrinsic
#define IntStone_resistance	(HStone_resistance || resists_ston(&youmonst) || is_ston_resistant(youmonst.data) || Role_if(PM_ROCKER) || (powerfulsymbiosis() && is_ston_resistant(&mons[u.usymbiote.mnum])) )
#define ExtStone_resistance	(playerextrinsicstoneres())

#define Stone_resistance	(((IntStone_resistance && u.nonintrinsicproperty != STONE_RES) || (ExtStone_resistance && u.nonextrinsicproperty != STONE_RES)) && !NoStone_resistance)
#define StrongStone_resistance	(IntStone_resistance && ExtStone_resistance && Stone_resistance && u.nondoubleproperty != STONE_RES)

#define NoStone_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_STONE_RES].intrinsic || UHaveAids || (u.impossibleproperty == STONE_RES) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HFear_resistance	u.uprops[FEAR_RES].intrinsic
#define EFear_resistance	u.uprops[FEAR_RES].extrinsic
#define IntFear_resistance	(HFear_resistance || Race_if(PM_EROSATOR) || tech_inuse(T_STAT_RESIST))
#define ExtFear_resistance	(playerextrinsicfearres())

#define Fear_resistance		(((IntFear_resistance && u.nonintrinsicproperty != FEAR_RES) || (ExtFear_resistance && u.nonextrinsicproperty != FEAR_RES)) && !NoFear_resistance)
#define StrongFear_resistance	(IntFear_resistance && ExtFear_resistance && Fear_resistance && u.nondoubleproperty != FEAR_RES)

#define NoFear_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_FEAR_RES].intrinsic || UHaveAids || (u.impossibleproperty == FEAR_RES) || (uarmf && uarmf->oartifact == ART_DON_T_FALL_INTO_THE_ABYSS) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))


/*** Troubles ***/
/* Pseudo-property */
#define Punished		(uball)

/* Those implemented solely as timeouts (we use just intrinsic) */
#define HStun			u.uprops[STUNNED].intrinsic
#define EStun			u.uprops[STUNNED].extrinsic
#define Stunned			(HStun || EStun || is_bubble(u.ux, u.uy) || (uarms && uarms->oartifact == ART_VENOMAT) || autismweaponcheck(ART_LANCE_OF_LONGINUS) || autismweaponcheck(ART_SCHWANZUS_LANGUS) || (uarm && uarm->oartifact == ART_ROBE_OF_THE_ARCHMAGI) || (uarm && uarm->oartifact == ART_SHIVANHUNTER_S_UNUSED_PRIZ) || HeavyStunned || ((u.umonnum == PM_STALKER || youmonst.data->mlet == S_BAT) && !Race_if(PM_TRANSFORMER) ) || Race_if(PM_TUMBLRER) || u.tumblrerpersist)
		/* Note: birds will also be stunned */
#define HeavyStunned		u.uprops[HEAVY_STUNNED].intrinsic

#define HConfusion		u.uprops[CONFUSION].intrinsic
#define EConfusion		u.uprops[CONFUSION].extrinsic
#define Confusion		(u.boomshined || HConfusion || EConfusion || HeavyConfusion || In_illusorycastle(&u.uz) || (uamul && uamul->oartifact == ART_CONFUSTICATOR) || (uarmh && uarmh->oartifact == ART_NEVER_CLEAN) || autismweaponcheck(ART_EAMANE_LUINWE) || autismweaponcheck(ART_ATLUS_HEAVE) || autismweaponcheck(ART_SANDRA_S_EVIL_MINDDRILL) || (uarmc && uarmc->oartifact == ART_VEIL_OF_LATONA) || autismweaponcheck(ART_MILENA_S_MISGUIDING) || (uarmg && uarmg->oartifact == ART_EGASSO_S_GIBBERISH) || (uarmh && uarmh->oartifact == ART_UNIMPLEMENTED_FEATURE) || autismweaponcheck(ART_FEANARO_SINGOLLO) || (uarms && uarms->oartifact == ART_CCC_CCC_CCCCCCC) || (uarmc && uarmc->oartifact == ART_VEIL_OF_MINISTRY) || autismweaponcheck(ART_LANCE_OF_LONGINUS) || autismweaponcheck(ART_SCHWANZUS_LANGUS) || u.uprops[SENSORY_DEPRIVATION].extrinsic || Race_if(PM_ADDICT) || u.addictpersist)
#define HeavyConfusion		u.uprops[HEAVY_CONFUSION].intrinsic

#define HNumbed		u.uprops[NUMBED].intrinsic
#define ENumbed		u.uprops[NUMBED].extrinsic
#define Numbed		(HNumbed || ENumbed || Race_if(PM_IRRITATOR) || (autismweaponcheck(ART_GAYGUN) && uarmg && (u.homosexual != 1)) || (uarms && uarms->oartifact == ART_NUMBED_CAN_T_DO) || HeavyNumbed)
#define HeavyNumbed		u.uprops[HEAVY_NUMBED].intrinsic

#define HFeared		u.uprops[FEARED].intrinsic
#define EFeared		u.uprops[FEARED].extrinsic
#define Feared		(flags.dudley || ((HFeared || EFeared || HeavyFeared || (uarmf && uarmf->oartifact == ART_DON_T_FALL_INTO_THE_ABYSS) || (uarmf && uarmf->oartifact == ART_AERES_RUNNING_SHOES) || (uarm && uarm->oartifact == ART_WHAT_MISS_I_CONSTANTLY) || (uarmu && uarmu->oartifact == ART_TILLMANN_S_TARGET) || (uarmf && uarmf->oartifact == ART_JASIEEN_S_FEAR)) && !Fear_resistance) )
#define HeavyFeared		u.uprops[HEAVY_FEARED].intrinsic

#define HFrozen		u.uprops[FROZEN].intrinsic
#define EFrozen		u.uprops[FROZEN].extrinsic
#define Frozen		((HFrozen || EFrozen || Race_if(PM_HYPOTHERMIC) || (uarmc && uarmc->oartifact == ART_BROKEN_WINGS) || (uarmc && uarmc->oartifact == ART_GROUNDBUMMER) || (uimplant && uimplant->oartifact == ART_MIGHTY_MOLASS) || (uarms && uarms->oartifact == ART_TYPE_OF_ICE_BLOCK_HATES_YO) || autismweaponcheck(ART_LANCE_OF_LONGINUS) || autismweaponcheck(ART_SCHWANZUS_LANGUS) || HeavyFrozen) && !(CannotBeFrozen && !Race_if(PM_HYPOTHERMIC)) )
#define HeavyFrozen		u.uprops[HEAVY_FROZEN].intrinsic

#define CannotBeFrozen	((uwep && uwep->oartifact == ART_HAH_TSCHH) || (uarmf && uarmf->oartifact == ART_FERGIE_S_GLACIER))

#define HBurned		u.uprops[BURNED].intrinsic
#define EBurned		u.uprops[BURNED].extrinsic
#define Burned		(HBurned || EBurned || HeavyBurned || Race_if(PM_BURNINATOR) || (uimplant && uimplant->oartifact == ART_BURN_BABY_BURN) || autismweaponcheck(ART_BURN_FASTER) || (uarms && uarms->oartifact == ART_BURNING_DISK) )
#define HeavyBurned		u.uprops[HEAVY_BURNED].intrinsic

#define HDimmed		u.uprops[DIMMED].intrinsic
#define EDimmed		u.uprops[DIMMED].extrinsic
#define Dimmed		(HDimmed || EDimmed || HeavyDimmed || (!flags.female && uarmc && uarmc->oartifact == ART_OVERCOAT_OF_DAUGHTER) || Race_if(PM_INKA) || autismweaponcheck(ART_BAT_FROM_BALTIMORE) || (uarmf && uarmf->oartifact == ART_FINAL_EXAM_TIME) || (uwep && uwep->otyp == TECPATL) || (uwep && uwep->otyp == TRAFFIC_LIGHT) || (u.twoweap && uswapwep && uswapwep->otyp == TECPATL) || (u.twoweap && uswapwep && uswapwep->otyp == TRAFFIC_LIGHT) )
#define HeavyDimmed		u.uprops[HEAVY_DIMMED].intrinsic

#define Blinded			u.uprops[BLINDED].intrinsic
#define EBlinded			u.uprops[BLINDED].extrinsic
#define Blindfolded		(ublindf && ublindf->otyp != LENSES && ublindf->otyp != RADIOGLASSES && ublindf->otyp != SHIELD_PATE_GLASSES && ublindf->otyp != BOSS_VISOR && ublindf->otyp != NIGHT_VISION_GOGGLES && ublindf->otyp != CONDOME && ublindf->otyp != SOFT_CHASTITY_BELT && ublindf->otyp != CLIMBING_SET && ublindf->otyp != DEFUSING_BOX)
		/* ...means blind because of a cover */
#define Blind	((Blinded || EBlinded || Blindfolded || HeavyBlind || u.uprops[SENSORY_DEPRIVATION].extrinsic || flags.blindfox || (!haseyes(youmonst.data) && !Race_if(PM_TRANSFORMER) ) ) && !(Blind_resistance && !flags.blindfox) )
		/* ...the Eyes operate even when you really are blind
		    or don't have any eyes */
/* added possibility of playing the entire game blind --Amy*/
#define HeavyBlind		u.uprops[HEAVY_BLIND].intrinsic

#define EBlind_resistance	u.uprops[BLIND_RES].extrinsic
#define HBlind_resistance	u.uprops[BLIND_RES].intrinsic
#define ExtBlind_resistance	(EBlind_resistance || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == BLIND_RES) ) || (ublindf && ublindf->oartifact == ART_EYES_OF_THE_OVERWORLD) || (uwep && uwep->oartifact == ART_DUMBOAK_S_HEW) || (uwep && uwep->oartifact == ART_DIMOAK_S_HEW) || (uwep && uwep->oartifact == ART_SWORD_OF_ERATHAOL) || (uwep && uwep->oartifact == ART_SUNSCREEN) || (uwep && uwep->oartifact == ART_SUNSPOT) || (uwep && uwep->oartifact == ART_SABER_OF_SABAOTH) || (uwep && uwep->oartifact == ART_SWORD_OF_ONOEL) || (uwep && uwep->oartifact == ART_GLAIVE_OF_SHAMSIEL) || autismringcheck(ART_BRIGHT_OVERWHELMING) || (uarm && uarm->oartifact == ART_SCARABROOCH) || (uwep && uwep->oartifact == ART_LANCE_OF_URIEL) || (uwep && uwep->oartifact == ART_HEAVY_CROSSBOW_OF_ETERNITY) || (uwep && uwep->oartifact == ART_FAIRY_EAR) || (ublindf && ublindf->oartifact == ART_STRIKE_OVER_THE_EARS) || (uwep && uwep->oartifact == ART_SUN_PIERCER) || (uwep && uwep->oartifact == ART_LAICEPS_NIKSUD) || (uarms && uarms->oartifact == ART_YOHUALLI_TEZCATL) || (uwep && uwep->oartifact == ART_HAMMER_OF_BARQUIEL) || (uarmf && uarmf->oartifact == ART_YES_I_ROCKZ) || (uwep && uwep->oartifact == ART_SAMENESS_OF_CHRIS) || (uwep && uwep->oartifact == ART_MUB_PUH_MUB_DIT_DIT) || (uarmh && uarmh->oartifact == ART_BLINDING_FOG) || (uarmh && uarmh->oartifact == ART_DARKSIGHT_HELM) || (uarms && uarms->oartifact == ART_SHADOWDISK) )
#define IntBlind_resistance	(HBlind_resistance || Race_if(PM_PLAYER_DOLGSMAN) || tech_inuse(T_STAT_RESIST))

#define Blind_resistance	( ( (ExtBlind_resistance && u.nonextrinsicproperty != BLIND_RES) || (IntBlind_resistance && u.nonintrinsicproperty != BLIND_RES) ) && !NoBlind_resistance)
#define StrongBlind_resistance	(IntBlind_resistance && ExtBlind_resistance && Blind_resistance && u.nondoubleproperty != BLIND_RES)

#define NoBlind_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_BLIND_RES].intrinsic || UHaveAids || (u.impossibleproperty == BLIND_RES) || flags.blindfox || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define PlayerBleeds			u.uprops[PLAYERBLEEDING].intrinsic
#define Sick			u.uprops[SICK].intrinsic
#define Stoned			u.uprops[STONED].intrinsic
#define Strangled		u.uprops[STRANGLED].intrinsic

#define Prem_death		(u.uprops[PREMDEATH].intrinsic || u.uprops[PREMDEATH].extrinsic)
#define HPrem_death	u.uprops[PREMDEATH].intrinsic
#define EPrem_death	(u.uprops[PREMDEATH].extrinsic || (uimplant && uimplant->oartifact == ART_THROW_MY_LIFE_AWAY) || (flags.female && uwep && autismweaponcheck(ART_TOMORROW_WENDY_S_CHOICENA)) || (uarmc && uarmc->oartifact == ART_KYS_YOURSELF) )

#define Vomiting		u.uprops[VOMITING].intrinsic

#define Glib			u.uprops[GLIB].intrinsic
#define EGlib			u.uprops[GLIB].extrinsic
#define IsGlib			((Glib || EGlib || (uarmg && uarmg->oartifact == ART_TANKS_A_LOT) || autismweaponcheck(ART_FADED_USELESSNESS) ) && !NoGlib)
#define NoGlib		(uwep && uwep->oartifact == ART_ST_ICKYNESS)

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
#define AefdeEffect		u.uprops[AEFDE_EFFECT].intrinsic
#define EpviEffect		u.uprops[EPVI_EFFECT].intrinsic
#define ReallyBadTrapEffect		u.uprops[REALLY_BAD_TRAP_EFFECT].intrinsic
#define CovidTrapEffect		u.uprops[COVID_TRAP_EFFECT].intrinsic
#define ArtiblastEffect		u.uprops[ARTIBLAST_EFFECT].intrinsic
#define RepeatingNastycurseEffect		u.uprops[REPEATING_NASTYCURSE_EFFECT].intrinsic
#define FuckfuckfuckEffect		u.uprops[FUCKFUCKFUCK_EFFECT].intrinsic

#define MulchBug		u.uprops[MULCH_BUG].intrinsic
#define SpellCooldownBug		u.uprops[SPELL_COOLDOWN_BUG].intrinsic
#define HaveAPinBug		u.uprops[HAVE_A_PIN_BUG].intrinsic
#define TurbodullBug		u.uprops[TURBODULL_BUG].intrinsic
#define InaccuracyBug		u.uprops[INACCURACY_BUG].intrinsic
#define InventorySizeBug		u.uprops[INVENTORY_SIZE_BUG].intrinsic
#define CostlyFailureBug		u.uprops[COSTLY_FAILURE_BUG].intrinsic
#define ReddamBug		u.uprops[REDDAM_BUG].intrinsic
#define RedincBug		u.uprops[REDINC_BUG].intrinsic
#define MonsterMultishotBug		u.uprops[MONSTER_MULTISHOT_EFFECT].intrinsic

#define MeanBurdenEffect		u.uprops[MEAN_BURDEN_EFFECT].intrinsic
#define CarrcapEffect		u.uprops[CARRCAP_EFFECT].intrinsic
#define UmengEffect		u.uprops[UMENG_EFFECT].intrinsic

#define OptionBugEffect		u.uprops[OPTION_BUG_EFFECT].intrinsic
#define MiscolorEffect		u.uprops[MISCOLOR_EFFECT].intrinsic
#define OneRainbowEffect		u.uprops[ONE_RAINBOW_EFFECT].intrinsic
#define ColorshiftEffect		u.uprops[COLORSHIFT_EFFECT].intrinsic
#define TopLineEffect		u.uprops[TOP_LINE_EFFECT].intrinsic
#define CapsBugEffect		u.uprops[CAPS_BUG_EFFECT].intrinsic
#define UnKnowledgeEffect		u.uprops[UN_KNOWLEDGE_EFFECT].intrinsic
#define DarkhanceEffect		u.uprops[DARKHANCE_EFFECT].intrinsic
#define DschueueuetEffect		u.uprops[DSCHUEUEUET_EFFECT].intrinsic
#define NopeskillEffect		u.uprops[NOPESKILL_EFFECT].intrinsic

#define RealLieEffect u.uprops[REAL_LIE_EFFECT].intrinsic
#define EscapePastEffect u.uprops[ESCAPE_PAST_EFFECT].intrinsic
#define PethateEffect u.uprops[PETHATE_EFFECT].intrinsic
#define PetLashoutEffect u.uprops[PET_LASHOUT_EFFECT].intrinsic
#define PetstarveEffect u.uprops[PETSTARVE_EFFECT].intrinsic
#define PetscrewEffect u.uprops[PETSCREW_EFFECT].intrinsic
#define TechLossEffect u.uprops[TECH_LOSS_EFFECT].intrinsic
#define ProoflossEffect u.uprops[PROOFLOSS_EFFECT].intrinsic
#define UnInvisEffect u.uprops[UN_INVIS_EFFECT].intrinsic
#define DetectationEffect u.uprops[DETECTATION_EFFECT].intrinsic

#define GiantExplorerBug		u.uprops[GIANT_EXPLORER].intrinsic
#define YawmBug		u.uprops[YAWM_EFFECT].intrinsic
#define TrapwarpingBug		u.uprops[TRAPWARPING].intrinsic
#define EnthuEffect		u.uprops[ENTHU_EFFECT].intrinsic
#define MikraEffect		u.uprops[MIKRA_EFFECT].intrinsic
#define GotsTooGoodEffect		u.uprops[GOTS_TOO_GOOD_EFFECT].intrinsic
#define NoFunWallsEffect		u.uprops[NO_FUN_WALLS].intrinsic
#define CradleChaosEffect		u.uprops[CRADLE_CHAOS_EFFECT].intrinsic
#define TezEffect		u.uprops[TEZ_EFFECT].intrinsic
#define KillerRoomEffect		u.uprops[KILLER_ROOM_EFFECT].intrinsic

#define SanityTrebleEffect		u.uprops[SANITY_TREBLE_EFFECT].intrinsic
#define StatDecreaseBug		u.uprops[STAT_DECREASE_BUG].intrinsic
#define SimeoutBug		u.uprops[SIMEOUT_BUG].intrinsic

#define BadPartBug	u.uprops[BAD_PARTS].intrinsic
#define CompletelyBadPartBug	u.uprops[COMPLETELY_BAD_PARTS].intrinsic
#define EvilVariantActive	u.uprops[EVIL_VARIANT_ACTIVE].intrinsic

#define OrangeSpells		u.uprops[ORANGE_SPELLS].intrinsic
#define VioletSpells		u.uprops[VIOLET_SPELLS].intrinsic
#define LongingEffect		u.uprops[LONGING_EFFECT].intrinsic
#define CursedParts		u.uprops[CURSED_PARTS].intrinsic
#define Quaversal		u.uprops[QUAVERSAL].intrinsic
#define AppearanceShuffling		u.uprops[APPEARANCE_SHUFFLING].intrinsic
#define BrownSpells		u.uprops[BROWN_SPELLS].intrinsic
#define Choicelessness		u.uprops[CHOICELESSNESS].intrinsic
#define Goldspells		u.uprops[GOLDSPELLS].intrinsic
#define Deprovement		u.uprops[DEPROVEMENT].intrinsic
#define InitializationFail		u.uprops[INITIALIZATION_FAIL].intrinsic
#define GushlushEffect		u.uprops[GUSHLUSH].intrinsic
#define SoiltypeEffect		u.uprops[SOILTYPE].intrinsic
#define DangerousTerrains		u.uprops[DANGEROUS_TERRAINS].intrinsic
#define FalloutEffect		u.uprops[FALLOUT_EFFECT].intrinsic
#define MojibakeEffect		u.uprops[MOJIBAKE].intrinsic
#define GravationEffect		u.uprops[GRAVATION].intrinsic
#define UncalledEffect		u.uprops[UNCALLED_EFFECT].intrinsic
#define ExplodingDiceEffect		u.uprops[EXPLODING_DICE].intrinsic
#define PermacurseEffect		u.uprops[PERMACURSE_EFFECT].intrinsic
#define ShroudedIdentity		u.uprops[SHROUDED_IDENTITY].intrinsic
#define FeelerGauges		u.uprops[FEELER_GAUGES].intrinsic
#define LongScrewup		u.uprops[LONG_SCREWUP].intrinsic
#define WingYellowChange		u.uprops[WING_YELLOW_GLYPHS].intrinsic
#define LifeSavingBug		u.uprops[LIFE_SAVING_BUG].intrinsic
#define CurseuseEffect		u.uprops[CURSEUSE_EFFECT].intrinsic
#define CutNutritionEffect		u.uprops[CUT_NUTRITION].intrinsic
#define SkillLossEffect		u.uprops[SKILL_LOSS_EFFECT].intrinsic
#define AutopilotEffect		u.uprops[AUTOPILOT_EFFECT].intrinsic
#define MysteriousForceActive		u.uprops[MYSTERIOUS_FORCE_EFFECT].intrinsic
#define MonsterGlyphChange		u.uprops[MONSTER_GLYPH_BUG].intrinsic
#define ChangingDirectives		u.uprops[CHANGING_DIRECTIVES].intrinsic
#define ContainerKaboom		u.uprops[CONTAINER_KABOOM].intrinsic
#define StealDegrading		u.uprops[STEAL_DEGRADING].intrinsic
#define LeftInventoryBug		u.uprops[LEFT_INVENTORY].intrinsic
#define FluctuatingSpeed		u.uprops[FLUCTUATING_SPEED].intrinsic
#define TarmuStrokingNora		u.uprops[TARMU_STROKING_NORA].intrinsic
#define FailureEffects		u.uprops[FAILURE_EFFECTS].intrinsic
#define BrightCyanSpells		u.uprops[BRIGHT_CYAN_SPELLS].intrinsic
#define FrequentationSpawns		u.uprops[FREQUENTATION_SPAWNS].intrinsic
#define PetAIScrewed		u.uprops[PET_AI_SCREWED].intrinsic
#define SatanEffect		u.uprops[SATAN_EFFECT].intrinsic
#define RememberanceEffect		u.uprops[REMEMBERANCE_EFFECT].intrinsic
#define PokelieEffect		u.uprops[POKELIE_EFFECT].intrinsic
#define AlwaysAutopickup		u.uprops[AUTOPICKUP_ALWAYS].intrinsic
#define DywypiProblem		u.uprops[DYWYPI_PROBLEM].intrinsic
#define SilverSpells		u.uprops[SILVER_SPELLS].intrinsic
#define MetalSpells		u.uprops[METAL_SPELLS].intrinsic
#define PlatinumSpells		u.uprops[PLATINUM_SPELLS].intrinsic
#define ManlerEffect		u.uprops[MANLER_EFFECT].intrinsic
#define DoorningEffect		u.uprops[DOORNING_EFFECT].intrinsic
#define NownsibleEffect		u.uprops[NOWNSIBLE_EFFECT].intrinsic
#define ElmStreetEffect		u.uprops[ELM_STREET_EFFECT].intrinsic
#define MonnoiseEffect		u.uprops[MONNOISE_EFFECT].intrinsic
#define RangCallEffect		u.uprops[RANG_CALL_EFFECT].intrinsic
#define RecurringSpellLoss		u.uprops[RECURRING_SPELL_LOSS].intrinsic
#define AntitrainingEffect		u.uprops[ANTI_TRAINING_EFFECT].intrinsic
#define TechoutBug		u.uprops[TECHOUT_BUG].intrinsic
#define StatDecay		u.uprops[STAT_DECAY].intrinsic
#define Movemork		u.uprops[MOVEMORKING].intrinsic

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
#define FemaleTrapTanja	u.uprops[FEMTRAP_TANJA].intrinsic
#define FemaleTrapSonja	u.uprops[FEMTRAP_SONJA].intrinsic
#define FemaleTrapRhea	u.uprops[FEMTRAP_RHEA].intrinsic
#define FemaleTrapLara	u.uprops[FEMTRAP_LARA].intrinsic
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
#define FemaleTrapMeltem	u.uprops[FEMTRAP_MELTEM].intrinsic
#define FemaleTrapSarah	u.uprops[FEMTRAP_SARAH].intrinsic
#define FemaleTrapClaudia	u.uprops[FEMTRAP_CLAUDIA].intrinsic
#define FemaleTrapLudgera	u.uprops[FEMTRAP_LUDGERA].intrinsic
#define FemaleTrapKati	u.uprops[FEMTRAP_KATI].intrinsic
#define FemaleTrapNelly	u.uprops[FEMTRAP_NELLY].intrinsic
#define FemaleTrapEveline	u.uprops[FEMTRAP_EVELINE].intrinsic
#define FemaleTrapKarin	u.uprops[FEMTRAP_KARIN].intrinsic
#define FemaleTrapJuen	u.uprops[FEMTRAP_JUEN].intrinsic
#define FemaleTrapKristina	u.uprops[FEMTRAP_KRISTINA].intrinsic
#define FemaleTrapLou	u.uprops[FEMTRAP_LOU].intrinsic
#define FemaleTrapAlmut	u.uprops[FEMTRAP_ALMUT].intrinsic
#define FemaleTrapJulietta	u.uprops[FEMTRAP_JULIETTA].intrinsic
#define FemaleTrapArabella	u.uprops[FEMTRAP_ARABELLA].intrinsic
#define FemaleTrapKristin	u.uprops[FEMTRAP_KRISTIN].intrinsic
#define FemaleTrapAnna	u.uprops[FEMTRAP_ANNA].intrinsic
#define FemaleTrapRuea	u.uprops[FEMTRAP_RUEA].intrinsic
#define FemaleTrapDora	u.uprops[FEMTRAP_DORA].intrinsic
#define FemaleTrapMarike	u.uprops[FEMTRAP_MARIKE].intrinsic
#define FemaleTrapJette	u.uprops[FEMTRAP_JETTE].intrinsic
#define FemaleTrapIna	u.uprops[FEMTRAP_INA].intrinsic
#define FemaleTrapSing	u.uprops[FEMTRAP_SING].intrinsic
#define FemaleTrapVictoria	u.uprops[FEMTRAP_VICTORIA].intrinsic
#define FemaleTrapMelissa	u.uprops[FEMTRAP_MELISSA].intrinsic
#define FemaleTrapNadine	u.uprops[FEMTRAP_NADINE].intrinsic
#define FemaleTrapLuisa	u.uprops[FEMTRAP_LUISA].intrinsic
#define FemaleTrapIrina	u.uprops[FEMTRAP_IRINA].intrinsic
#define FemaleTrapLiselotte	u.uprops[FEMTRAP_LISELOTTE].intrinsic
#define FemaleTrapGreta	u.uprops[FEMTRAP_GRETA].intrinsic
#define FemaleTrapJane	u.uprops[FEMTRAP_JANE].intrinsic
#define FemaleTrapSueLyn	u.uprops[FEMTRAP_SUE_LYN].intrinsic
#define FemaleTrapCharlotte	u.uprops[FEMTRAP_CHARLOTTE].intrinsic
#define FemaleTrapHannah	u.uprops[FEMTRAP_HANNAH].intrinsic
#define FemaleTrapLittleMarie	u.uprops[FEMTRAP_LITTLE_MARIE].intrinsic
#define FemaleTrapAnita	u.uprops[FEMTRAP_ANITA].intrinsic
#define FemaleTrapHenrietta	u.uprops[FEMTRAP_HENRIETTA].intrinsic
#define FemaleTrapVerena	u.uprops[FEMTRAP_VERENA].intrinsic
#define FemaleTrapAnnemarie	u.uprops[FEMTRAP_ANNEMARIE].intrinsic
#define FemaleTrapJil	u.uprops[FEMTRAP_JIL].intrinsic
#define FemaleTrapJana	u.uprops[FEMTRAP_JANA].intrinsic
#define FemaleTrapKatrin	u.uprops[FEMTRAP_KATRIN].intrinsic
#define FemaleTrapGudrun	u.uprops[FEMTRAP_GUDRUN].intrinsic
#define FemaleTrapElla	u.uprops[FEMTRAP_ELLA].intrinsic
#define FemaleTrapManuela	u.uprops[FEMTRAP_MANUELA].intrinsic
#define FemaleTrapJennifer	u.uprops[FEMTRAP_JENNIFER].intrinsic
#define FemaleTrapPatricia	u.uprops[FEMTRAP_PATRICIA].intrinsic
#define FemaleTrapAntje	u.uprops[FEMTRAP_ANTJE].intrinsic
#define FemaleTrapAntjeX	u.uprops[FEMTRAP_ANTJE_X].intrinsic
#define FemaleTrapKerstin	u.uprops[FEMTRAP_KERSTIN].intrinsic
#define FemaleTrapLaura	u.uprops[FEMTRAP_LAURA].intrinsic
#define FemaleTrapLarissa	u.uprops[FEMTRAP_LARISSA].intrinsic
#define FemaleTrapNora	u.uprops[FEMTRAP_NORA].intrinsic
#define FemaleTrapNatalia	u.uprops[FEMTRAP_NATALIA].intrinsic
#define FemaleTrapSusanne	u.uprops[FEMTRAP_SUSANNE].intrinsic
#define FemaleTrapLisa	u.uprops[FEMTRAP_LISA].intrinsic
#define FemaleTrapBridghitte	u.uprops[FEMTRAP_BRIDGHITTE].intrinsic
#define FemaleTrapJulia	u.uprops[FEMTRAP_JULIA].intrinsic
#define FemaleTrapNicole	u.uprops[FEMTRAP_NICOLE].intrinsic
#define FemaleTrapRita	u.uprops[FEMTRAP_RITA].intrinsic
#define FemaleTrapJanina	u.uprops[FEMTRAP_JANINA].intrinsic
#define FemaleTrapRosa	u.uprops[FEMTRAP_ROSA].intrinsic
#define FemaleTrapKsenia	u.uprops[FEMTRAP_KSENIA].intrinsic
#define FemaleTrapLydia	u.uprops[FEMTRAP_LYDIA].intrinsic
#define FemaleTrapConny	u.uprops[FEMTRAP_CONNY].intrinsic
#define FemaleTrapKatia	u.uprops[FEMTRAP_KATIA].intrinsic
#define FemaleTrapMariya	u.uprops[FEMTRAP_MARIYA].intrinsic
#define FemaleTrapElise	u.uprops[FEMTRAP_ELISE].intrinsic
#define FemaleTrapRonja	u.uprops[FEMTRAP_RONJA].intrinsic
#define FemaleTrapAriane	u.uprops[FEMTRAP_ARIANE].intrinsic
#define FemaleTrapJohanna	u.uprops[FEMTRAP_JOHANNA].intrinsic
#define FemaleTrapInge	u.uprops[FEMTRAP_INGE].intrinsic
#define FemaleTrapRuth	u.uprops[FEMTRAP_RUTH].intrinsic
#define FemaleTrapMagdalena	u.uprops[FEMTRAP_MAGDALENA].intrinsic
#define FemaleTrapMarleen	u.uprops[FEMTRAP_MARLEEN].intrinsic
#define FemaleTrapKlara	u.uprops[FEMTRAP_KLARA].intrinsic
#define FemaleTrapFriederike	u.uprops[FEMTRAP_FRIEDERIKE].intrinsic
#define FemaleTrapNaomi	u.uprops[FEMTRAP_NAOMI].intrinsic
#define FemaleTrapUte	u.uprops[FEMTRAP_UTE].intrinsic
#define FemaleTrapJasieen	u.uprops[FEMTRAP_JASIEEN].intrinsic
#define FemaleTrapYasaman	u.uprops[FEMTRAP_YASAMAN].intrinsic
#define FemaleTrapMayBritt	u.uprops[FEMTRAP_MAY_BRITT].intrinsic
#define FemaleTrapSabrina	u.uprops[FEMTRAP_SABRINA].intrinsic

#define FemtrapActiveFemmy	(FemaleTrapFemmy || u.uprops[FEMTRAP_FEMMY].extrinsic || have_femtrapfemmy())
#define FemtrapActiveMadeleine	(FemaleTrapMadeleine || u.uprops[FEMTRAP_MADELEINE].extrinsic || have_femtrapmadeleine())
#define FemtrapActiveMarlena	(FemaleTrapMarlena || u.uprops[FEMTRAP_MARLENA].extrinsic || have_femtrapmarlena())
#define FemtrapActiveSabrina	(FemaleTrapSabrina || u.uprops[FEMTRAP_SABRINA].extrinsic || have_femtrapsabrina() || (uarm && uarm->oartifact == ART_YOU_REALLY_HAVE_A_TOTAL_DA) || (uarmf && uarmf->oartifact == ART_SABRINA_S_FREEDOM) )
#define FemtrapActiveTanja	(FemaleTrapTanja || u.uprops[FEMTRAP_TANJA].extrinsic || have_femtraptanja() || (uarmu && uarmu->oartifact == ART_THEY_ALL_FEEL_FLEECY) )
#define FemtrapActiveSonja	(FemaleTrapSonja || u.uprops[FEMTRAP_SONJA].extrinsic || have_femtrapsonja() || (uarmu && uarmu->oartifact == ART_TILLMANN_S_TARGET) )
#define FemtrapActiveRhea	(FemaleTrapRhea || u.uprops[FEMTRAP_RHEA].extrinsic || have_femtraprhea() || (uarmf && uarmf->oartifact == ART_HIGHWAY_HUNTER) || (uarmf && uarmf->oartifact == ART_HIGHWAY_FIGHTER) )
#define FemtrapActiveLara	(FemaleTrapLara || u.uprops[FEMTRAP_LARA].extrinsic || have_femtraplara() || (uarms && uarms->oartifact == ART_CASTLE_CRUSH_GLITCH) || (uarmg && uarmg->oartifact == ART_ELARA_S_AGILITY) )
#define FemtrapActiveAnastasia	(FemaleTrapAnastasia || u.uprops[FEMTRAP_ANASTASIA].extrinsic || have_femtrapanastasia() || (uarmf && uarmf->oartifact == ART_THAT_S_SUPER_UNFAIR) || (uarmf && uarmf->oartifact == ART_PLAYING_ANASTASIA) || (uarmf && uarmf->oartifact == ART_WHITE_LOAFERS) || (uarmf && uarmf->oartifact == ART_SOFT_COW_DUNG) || (uarmf && uarmf->oartifact == ART_THICK_PLATFORM_CRAZE) || (uarmf && uarmf->oartifact == ART_LORENZI_S_CLEANING_RESIDUE) || (uarmf && uarmf->oartifact == ART_NANCY_S_ASSISTANCE) || (uarmf && uarmf->oartifact == ART_SHE_S_STILL_AN_ANASTASIA) || autismweaponcheck(ART_REAL_WALKING) || (autismweaponcheck(ART_DAMN_SKI_WEDGE) && !uarmf) || (uarmf && uarmf->oartifact == ART_FASTER_THAN_ALL_OTHERS_INT))
#define FemtrapActiveJessica	(FemaleTrapJessica || u.uprops[FEMTRAP_JESSICA].extrinsic || have_femtrapjessica() || (uarmf && uarmf->oartifact == ART_WEDDING_CHALLENGE))
#define FemtrapActiveSolvejg	(FemaleTrapSolvejg || u.uprops[FEMTRAP_SOLVEJG].extrinsic || have_femtrapsolvejg())
#define FemtrapActiveWendy	(FemaleTrapWendy || u.uprops[FEMTRAP_WENDY].extrinsic || have_femtrapwendy() || autismweaponcheck(ART_TOMORROW_WENDY_S_CHOICENA) )
#define FemtrapActiveKatharina	(FemaleTrapKatharina || u.uprops[FEMTRAP_KATHARINA].extrinsic || have_femtrapkatharina() || (uarmf && uarmf->oartifact == ART_KATHARINA_S_SIGH) )
#define FemtrapActiveElena	(FemaleTrapElena || u.uprops[FEMTRAP_ELENA].extrinsic || have_femtrapelena())
#define FemtrapActiveThai	(FemaleTrapThai || u.uprops[FEMTRAP_THAI].extrinsic || have_femtrapthai() || Race_if(PM_TAYIIN) || autismweaponcheck(ART_THAIFORCE) || autismweaponcheck(ART_BIDETHANDER) )
#define FemtrapActiveElif	(FemaleTrapElif || u.uprops[FEMTRAP_ELIF].extrinsic || have_femtrapelif())
#define FemtrapActiveNadja	(FemaleTrapNadja || u.uprops[FEMTRAP_NADJA].extrinsic || have_femtrapnadja())
#define FemtrapActiveSandra	(FemaleTrapSandra || u.uprops[FEMTRAP_SANDRA].extrinsic || have_femtrapsandra())
#define FemtrapActiveNatalje	(FemaleTrapNatalje || u.uprops[FEMTRAP_NATALJE].extrinsic || have_femtrapnatalje())
#define FemtrapActiveJeanetta	(FemaleTrapJeanetta || u.uprops[FEMTRAP_JEANETTA].extrinsic || have_femtrapjeanetta() || (uarm && uarm->oartifact == ART_JEANETTA_S_REVENGE) )
#define FemtrapActiveYvonne	(FemaleTrapYvonne || u.uprops[FEMTRAP_YVONNE].extrinsic || have_femtrapyvonne())
#define FemtrapActiveMaurah	(FemaleTrapMaurah || u.uprops[FEMTRAP_MAURAH].extrinsic || have_femtrapmaurah() || (uarmf && uarmf->oartifact == ART_THICK_FARTING_GIRL) || (uarmu && uarmu->oartifact == ART_SUE_LYN_S_USAGE) || (uarmf && uarmf->oartifact == ART_KATHARINA_S_LOVELINESS))
#define FemtrapActiveMeltem	(FemaleTrapMeltem || u.uprops[FEMTRAP_MELTEM].extrinsic || have_femtrapmeltem() || have_magicrystal() )
#define FemtrapActiveSarah	(FemaleTrapSarah || u.uprops[FEMTRAP_SARAH].extrinsic || have_femtrapsarah())
#define FemtrapActiveClaudia	(FemaleTrapClaudia || u.uprops[FEMTRAP_CLAUDIA].extrinsic || have_femtrapclaudia() || (uarmu && uarmu->oartifact == ART_CLOUDYBAND) )
#define FemtrapActiveLudgera	(FemaleTrapLudgera || u.uprops[FEMTRAP_LUDGERA].extrinsic || have_femtrapludgera() || (uarm && uarm->oartifact == ART_LU_NONNAME) || autismweaponcheck(ART_BIDETHANDER) || (uarmf && uarmf->oartifact == ART_DAPHNE_S_BOREDOM) )
#define FemtrapActiveKati	(FemaleTrapKati || u.uprops[FEMTRAP_KATI].extrinsic || have_femtrapkati())
#define FemtrapActiveNelly	(FemaleTrapNelly || u.uprops[FEMTRAP_NELLY].extrinsic || have_femtrapnelly())
#define FemtrapActiveEveline	(FemaleTrapEveline || u.uprops[FEMTRAP_EVELINE].extrinsic || have_femtrapeveline())
#define FemtrapActiveKarin	(FemaleTrapKarin || u.uprops[FEMTRAP_KARIN].extrinsic || have_femtrapkarin())
#define FemtrapActiveJuen	(FemaleTrapJuen || u.uprops[FEMTRAP_JUEN].extrinsic || have_femtrapjuen() || (uarmf && uarmf->oartifact == ART_SHIN_KICKING_GAME) || autismweaponcheck(ART_BLU_TOE))
#define FemtrapActiveKristina	(FemaleTrapKristina || u.uprops[FEMTRAP_KRISTINA].extrinsic || have_femtrapkristina() || (uarmf && uarmf->oartifact == ART_JUEN_S_WEAKNESS) || (uamul && uamul->oartifact == ART_HIBA_CHIBA) )
#define FemtrapActiveLou	(FemaleTrapLou || u.uprops[FEMTRAP_LOU].extrinsic || have_femtraplou())
#define FemtrapActiveAlmut	(FemaleTrapAlmut || u.uprops[FEMTRAP_ALMUT].extrinsic || have_femtrapalmut())
#define FemtrapActiveJulietta	(FemaleTrapJulietta || u.uprops[FEMTRAP_JULIETTA].extrinsic || have_femtrapjulietta() || (autismweaponcheck(ART_DAMN_SKI_WEDGE) && !uarmf) || autismweaponcheck(ART_ST_ICKYNESS) || autismweaponcheck(ART_HOL_ON_MAN) )
#define FemtrapActiveArabella	(FemaleTrapArabella || u.uprops[FEMTRAP_ARABELLA].extrinsic || have_femtraparabella() || (uarm && uarm->oartifact == ART_ARABELLA_S_FEMINIZER) || autismringcheck(ART_ARABELLA_S_NASTYGUARD) )
#define FemtrapActiveKristin	(FemaleTrapKristin || u.uprops[FEMTRAP_KRISTIN].extrinsic || have_femtrapkristin())
#define FemtrapActiveAnna	(FemaleTrapAnna || u.uprops[FEMTRAP_ANNA].extrinsic || have_femtrapanna() || (uarmf && uarmf->oartifact == ART_WHOA_LOOK_AT_THOSE) || (uarmu && uarmu->oartifact == ART_FIRST_THERE_WE_WERE) )
#define FemtrapActiveRuea	(FemaleTrapRuea || u.uprops[FEMTRAP_RUEA].extrinsic || have_femtrapruea())
#define FemtrapActiveDora	(FemaleTrapDora || u.uprops[FEMTRAP_DORA].extrinsic || have_femtrapdora() || autismweaponcheck(ART_DESERT_MAID) || (uarmf && uarmf->oartifact == ART_SHE_REALLY_LIKES_IT) )
#define FemtrapActiveMarike	(FemaleTrapMarike || u.uprops[FEMTRAP_MARIKE].extrinsic || have_femtrapmarike())
#define FemtrapActiveJette	(FemaleTrapJette || u.uprops[FEMTRAP_JETTE].extrinsic || have_femtrapjette() || autismweaponcheck(ART_HER_UNREACHABLE_BROOK) || (uarm && uarm->oartifact == ART_WHOA_THAT_S_THE_ONE_) || (uarmu && uarmu->oartifact == ART_HEEEEELEEEEEN && !Role_if(PM_ACTIVISTOR)) )
#define FemtrapActiveIna	(FemaleTrapIna || u.uprops[FEMTRAP_INA].extrinsic || have_femtrapina())
#define FemtrapActiveSing	(FemaleTrapSing || u.uprops[FEMTRAP_SING].extrinsic || have_femtrapsing() || autismweaponcheck(ART_VAMPIRE_SING_S_BLOODRUSH) || autismweaponcheck(ART_TONA_S_GAMES) || (Role_if(PM_SINGSLAVE)) || (uarmf && uarmf->oartifact == ART_SINGHETA) )
#define FemtrapActiveVictoria	(FemaleTrapVictoria || u.uprops[FEMTRAP_VICTORIA].extrinsic || have_femtrapvictoria() || (uarmu && uarmu->oartifact == ART_NOW_YOU_MADE_HER_SAD))
#define FemtrapActiveMelissa	(FemaleTrapMelissa || u.uprops[FEMTRAP_MELISSA].extrinsic || have_femtrapmelissa())
#define FemtrapActiveAnita	(FemaleTrapAnita || u.uprops[FEMTRAP_ANITA].extrinsic || have_femtrapanita())
#define FemtrapActiveHenrietta	(FemaleTrapHenrietta || u.uprops[FEMTRAP_HENRIETTA].extrinsic || have_femtraphenrietta() || (uarmf && uarmf->oartifact == ART_SPFLOTCH__HAHAHAHAHA_) || (uarmf && uarmf->oartifact == ART_PLAY_THE_GAME_YOURSELF) )
#define FemtrapActiveVerena	(FemaleTrapVerena || u.uprops[FEMTRAP_VERENA].extrinsic || have_femtrapverena() || (uarmh && uarmh->oartifact == ART_IGNORANCE_IS_BLISS))
#define FemtrapActiveAnnemarie	(FemaleTrapAnnemarie || u.uprops[FEMTRAP_ANNEMARIE].extrinsic || have_femtrapannemarie())
#define FemtrapActiveJil	(FemaleTrapJil || u.uprops[FEMTRAP_JIL].extrinsic || have_femtrapjil())
#define FemtrapActiveJana	(FemaleTrapJana || u.uprops[FEMTRAP_JANA].extrinsic || have_femtrapjana() || (uarmf && uarmf->oartifact == ART_MARJI_JANA) )
#define FemtrapActiveKatrin	(FemaleTrapKatrin || u.uprops[FEMTRAP_KATRIN].extrinsic || have_femtrapkatrin())
#define FemtrapActiveGudrun	(FemaleTrapGudrun || u.uprops[FEMTRAP_GUDRUN].extrinsic || have_femtrapgudrun() || (uarm && uarm->oartifact == ART_CUDDLE_CHALLENGE) || (autismweaponcheck(ART_OZYZEVPDWTVP) && u.homosexual == 1) )
#define FemtrapActiveElla	(FemaleTrapElla || u.uprops[FEMTRAP_ELLA].extrinsic || have_femtrapella() || (uarm && uarm->oartifact == ART_ICE_FROM_THE_NORTH) || (autismweaponcheck(ART_OZYZEVPDWTVP) && u.homosexual == 2) )
#define FemtrapActiveManuela	(FemaleTrapManuela || u.uprops[FEMTRAP_MANUELA].extrinsic || have_femtrapmanuela() || (autismweaponcheck(ART_OZYZEVPDWTVP) && u.homosexual == 0) )
#define FemtrapActiveJennifer	(FemaleTrapJennifer || u.uprops[FEMTRAP_JENNIFER].extrinsic || have_femtrapjennifer() || (uarmf && uarmf->oartifact == ART_JENNY_SUPERSOFT) )
#define FemtrapActivePatricia	(FemaleTrapPatricia || u.uprops[FEMTRAP_PATRICIA].extrinsic || have_femtrappatricia() || autismweaponcheck(ART_MAEVE_S_OPULENCE) )
#define FemtrapActiveAntje	(FemaleTrapAntje || u.uprops[FEMTRAP_ANTJE].extrinsic || have_femtrapantje() || (uarm && uarm->oartifact == ART_MEET_WOMAN_ANTJE) || (uarmf && uarmf->oartifact == ART_FANNY_S_ANNOYANCE) )
#define FemtrapActiveAntjeX	(FemaleTrapAntjeX || u.uprops[FEMTRAP_ANTJE_X].extrinsic || have_femtrapantjex() || (uarmu && uarmu->oartifact == ART_SOME_CHAMBER_DOOR) )
#define FemtrapActiveKerstin	(FemaleTrapKerstin || u.uprops[FEMTRAP_KERSTIN].extrinsic || have_femtrapkerstin() || bmwride(ART_KERSTIN_S_COWBOY_BOOST) || autismweaponcheck(ART_GONDOLIN_S_HIDDEN_PASSAGE) )
#define FemtrapActiveLaura	(FemaleTrapLaura || u.uprops[FEMTRAP_LAURA].extrinsic || have_femtraplaura() || (uarmf && uarmf->oartifact == ART_POCAHONTAS_HOME) )
#define FemtrapActiveLarissa	(FemaleTrapLarissa || u.uprops[FEMTRAP_LARISSA].extrinsic || have_femtraplarissa() || (uarmf && uarmf->oartifact == ART_SHE_REALLY_LIKES_IT) || autismweaponcheck(ART_LARISSA_S_LAUGHTER) )
#define FemtrapActiveNora	(FemaleTrapNora || u.uprops[FEMTRAP_NORA].extrinsic || have_femtrapnora())
#define FemtrapActiveNatalia	(FemaleTrapNatalia || u.uprops[FEMTRAP_NATALIA].extrinsic || have_femtrapnatalia() || (uarmu && uarmu->oartifact == ART_MENSTRUATION_HURTS) )
#define FemtrapActiveSusanne	(FemaleTrapSusanne || u.uprops[FEMTRAP_SUSANNE].extrinsic || have_femtrapsusanne() || (uarm && uarm->oartifact == ART_SUSA_MAIL) )
#define FemtrapActiveLisa	(FemaleTrapLisa || u.uprops[FEMTRAP_LISA].extrinsic || have_femtraplisa() || (uarmu && uarmu->oartifact == ART_LISE_S_UNDERWEAR) || (uarmh && uarmh->oartifact == ART_ADORNING_BUNDLE) )
#define FemtrapActiveBridghitte	(FemaleTrapBridghitte || u.uprops[FEMTRAP_BRIDGHITTE].extrinsic || have_femtrapbridghitte() || (uarmf && uarmf->oartifact == ART_FIGHT_WITH_PLATFORM_INA) || (uarmh && uarmh->oartifact == ART_CLELIA_S_TONGUE_BREAKER) )
#define FemtrapActiveJulia	(FemaleTrapJulia || u.uprops[FEMTRAP_JULIA].extrinsic || have_femtrapjulia() || (uarmf && uarmf->oartifact == ART_SASSY_JULIA) )
#define FemtrapActiveNicole	(FemaleTrapNicole || u.uprops[FEMTRAP_NICOLE].extrinsic || have_femtrapnicole() || (uarmc && uarmc->oartifact == ART_SHROUD) || (uarmc && uarmc->oartifact == ART_CLANCY_S_FURTIVENESS) )
#define FemtrapActiveRita	(FemaleTrapRita || u.uprops[FEMTRAP_RITA].extrinsic || have_femtraprita())
#define FemtrapActiveJanina	(FemaleTrapJanina || u.uprops[FEMTRAP_JANINA].extrinsic || have_femtrapjanina())
#define FemtrapActiveRosa	(FemaleTrapRosa || u.uprops[FEMTRAP_ROSA].extrinsic || have_femtraprosa() || (uarm && uarm->oartifact == ART_WHOA_THAT_S_THE_ONE_) )
#define FemtrapActiveKsenia	(FemaleTrapKsenia || u.uprops[FEMTRAP_KSENIA].extrinsic || have_femtrapksenia())
#define FemtrapActiveLydia	(FemaleTrapLydia || u.uprops[FEMTRAP_LYDIA].extrinsic || have_femtraplydia())
#define FemtrapActiveConny	(FemaleTrapConny || u.uprops[FEMTRAP_CONNY].extrinsic || have_femtrapconny() || (uarm && uarm->oartifact == ART_AMMY_S_BAND) )
#define FemtrapActiveKatia	(FemaleTrapKatia || u.uprops[FEMTRAP_KATIA].extrinsic || have_femtrapkatia() || (uarmu && uarmu->oartifact == ART_COVER_THE_SEXY_BUTT) )
#define FemtrapActiveMariya	(FemaleTrapMariya || u.uprops[FEMTRAP_MARIYA].extrinsic || have_femtrapmariya() || (uarmf && uarmf->oartifact == ART_MARIYA_S_SEXYCHARM) || autismweaponcheck(ART_AXELLE_S_CHAIN) )
#define FemtrapActiveElise	(FemaleTrapElise || u.uprops[FEMTRAP_ELISE].extrinsic || have_femtrapelise() || (uarmu && uarmu->oartifact == ART_GREENTOP) || (uarmf && uarmf->oartifact == ART_RONIKA_S_NORM) )
#define FemtrapActiveRonja	(FemaleTrapRonja || u.uprops[FEMTRAP_RONJA].extrinsic || have_femtrapronja())
#define FemtrapActiveAriane	(FemaleTrapAriane || u.uprops[FEMTRAP_ARIANE].extrinsic || have_femtrapariane() || (uarmf && uarmf->oartifact == ART_CANNOT_WALK_WITH_THE_ARIAN) )
#define FemtrapActiveJohanna	(FemaleTrapJohanna || u.uprops[FEMTRAP_JOHANNA].extrinsic || have_femtrapjohanna() || autismweaponcheck(ART_GODAWFUL_ENCHANTMENT) || (uarmf && uarmf->oartifact == ART_SHE_S_NOT_FORGOTTEN) )
#define FemtrapActiveInge	(FemaleTrapInge || u.uprops[FEMTRAP_INGE].extrinsic || have_femtrapinge())
#define FemtrapActiveRuth	(FemaleTrapRuth || u.uprops[FEMTRAP_RUTH].extrinsic || have_femtrapruth() || autismweaponcheck(ART_PROFANED_GREATSCYTHE))
#define FemtrapActiveMagdalena	(FemaleTrapMagdalena || u.uprops[FEMTRAP_MAGDALENA].extrinsic || have_femtrapmagdalena() || (autismweaponcheck(ART_MAGDALENA_S_CUDDLEWEAPON)) )
#define FemtrapActiveMarleen	(FemaleTrapMarleen || u.uprops[FEMTRAP_MARLEEN].extrinsic || have_femtrapmarleen() || (uarmf && uarmf->oartifact == ART_EROTIC_STAT_TRAIN) )
#define FemtrapActiveKlara	(FemaleTrapKlara || u.uprops[FEMTRAP_KLARA].extrinsic || have_femtrapklara() || (uarm && uarm->oartifact == ART_KUSE_MUSE) )
#define FemtrapActiveFriederike	(FemaleTrapFriederike || u.uprops[FEMTRAP_FRIEDERIKE].extrinsic || have_femtrapfriederike() || (uarmu && uarmu->oartifact == ART_FRIEDERIKE_S_BUNDLING) )
#define FemtrapActiveNaomi	(FemaleTrapNaomi || u.uprops[FEMTRAP_NAOMI].extrinsic || have_femtrapnaomi())
#define FemtrapActiveUte	(FemaleTrapUte || u.uprops[FEMTRAP_UTE].extrinsic || have_femtrapute() || autismweaponcheck(ART_DE_SID))
#define FemtrapActiveJasieen	(FemaleTrapJasieen || u.uprops[FEMTRAP_JASIEEN].extrinsic || have_femtrapjasieen())
#define FemtrapActiveYasaman	(FemaleTrapYasaman || u.uprops[FEMTRAP_YASAMAN].extrinsic || have_femtrapyasaman() || (uarms && uarms->oartifact == ART_FROM_MASHHAD_TO_AHVAZ) )
#define FemtrapActiveMayBritt	(FemaleTrapMayBritt || u.uprops[FEMTRAP_MAY_BRITT].extrinsic || have_femtrapmaybritt() || (uarm && uarm->oartifact == ART_SCHOOL_SATCHEL) )
#define FemtrapActiveNadine	(FemaleTrapNadine || u.uprops[FEMTRAP_NADINE].extrinsic || have_femtrapnadine())
#define FemtrapActiveLuisa	(FemaleTrapLuisa || u.uprops[FEMTRAP_LUISA].extrinsic || have_femtrapluisa() || (uamul && uamul->oartifact == ART_REGGIE_S_POINTSTEP) )
#define FemtrapActiveIrina	(FemaleTrapIrina || u.uprops[FEMTRAP_IRINA].extrinsic || have_femtrapirina())
#define FemtrapActiveLiselotte	(FemaleTrapLiselotte || u.uprops[FEMTRAP_LISELOTTE].extrinsic || have_femtrapliselotte())
#define FemtrapActiveGreta	(FemaleTrapGreta || u.uprops[FEMTRAP_GRETA].extrinsic || have_femtrapgreta() || autismweaponcheck(ART_FREAK_VERSUS_BANGER) || (uamul && uamul->oartifact == ART_HIBA_CHIBA) )
#define FemtrapActiveJane	(FemaleTrapJane || u.uprops[FEMTRAP_JANE].extrinsic || have_femtrapjane() || autismweaponcheck(ART_FREAK_VERSUS_BANGER) || (uarm && uarm->oartifact == ART_SUIT_OF_JANE_MAIL) )
#define FemtrapActiveSueLyn	(FemaleTrapSueLyn || u.uprops[FEMTRAP_SUE_LYN].extrinsic || have_femtrapsuelyn())
#define FemtrapActiveCharlotte	(FemaleTrapCharlotte || u.uprops[FEMTRAP_CHARLOTTE].extrinsic || have_femtrapcharlotte() || (uarm && uarm->oartifact == ART_FARTHER_INTO_THE_JUNGLE) )
#define FemtrapActiveHannah	(FemaleTrapHannah || u.uprops[FEMTRAP_HANNAH].extrinsic || have_femtraphannah() || (uarm && uarm->oartifact == ART_STACHEL_SATCHEL) )
#define FemtrapActiveLittleMarie	(FemaleTrapLittleMarie || u.uprops[FEMTRAP_LITTLE_MARIE].extrinsic || have_femtraplittlemarie() || (uarmu && uarmu->oartifact == ART_HA_HA_HA_HA___))

#define SuperFemtrapFemmy	( (have_femtrapfemmy() == 2) )
#define SuperFemtrapAnastasia	( (have_femtrapanastasia() == 2) )
#define SuperFemtrapWendy	( (have_femtrapwendy() == 2) )
#define SuperFemtrapKatharina	( (have_femtrapkatharina() == 2) )
#define SuperFemtrapElif	( (have_femtrapelif() == 2) )
#define SuperFemtrapMaurah	( (have_femtrapmaurah() == 2) )
#define SuperFemtrapSarah	( (have_femtrapsarah() == 2) )
#define SuperFemtrapClaudia	( (have_femtrapclaudia() == 2) )
#define SuperFemtrapLudgera	( (have_femtrapludgera() == 2) || (uarmf && uarmf->oartifact == ART_DAPHNE_S_BOREDOM) )
#define SuperFemtrapEveline	( (have_femtrapeveline() == 2) )
#define SuperFemtrapJulietta	( (have_femtrapjulietta() == 2) )
#define SuperFemtrapArabella	( (have_femtraparabella() == 2) )
#define SuperFemtrapAnna	( (have_femtrapanna() == 2) )
#define SuperFemtrapRuea	( (have_femtrapruea() == 2) )
#define SuperFemtrapDora	( (have_femtrapdora() == 2) )
#define SuperFemtrapMarike	( (have_femtrapmarike() == 2) )
#define SuperFemtrapSing	( (have_femtrapsing() == 2) )
#define SuperFemtrapVictoria	( (have_femtrapvictoria() == 2) )
#define SuperFemtrapVerena	( (have_femtrapverena() == 2) )
#define SuperFemtrapJil	( (have_femtrapjil() == 2) )
#define SuperFemtrapJana	( (have_femtrapjana() == 2) )
#define SuperFemtrapManuela	( (have_femtrapmanuela() == 2) )
#define SuperFemtrapJennifer	( (have_femtrapjennifer() == 2) )
#define SuperFemtrapPatricia	( (have_femtrappatricia() == 2) || autismweaponcheck(ART_MAEVE_S_OPULENCE) )
#define SuperFemtrapKerstin	( (have_femtrapkerstin() == 2) )
#define SuperFemtrapLaura	( (have_femtraplaura() == 2) )
#define SuperFemtrapLarissa	( (have_femtraplarissa() == 2) )
#define SuperFemtrapSusanne	( (have_femtrapsusanne() == 2) )
#define SuperFemtrapBridghitte	( (have_femtrapbridghitte() == 2) || (uarmh && uarmh->oartifact == ART_CLELIA_S_TONGUE_BREAKER) )
#define SuperFemtrapNicole	( (have_femtrapnicole() == 2) || (uarmc && uarmc->oartifact == ART_CLANCY_S_FURTIVENESS) )
#define SuperFemtrapJanina	( (have_femtrapjanina() == 2) )
#define SuperFemtrapRosa	( (have_femtraprosa() == 2) )
#define SuperFemtrapKsenia	( (have_femtrapksenia() == 2) )
#define SuperFemtrapKatia	( (have_femtrapkatia() == 2) )
#define SuperFemtrapMariya	( (have_femtrapmariya() == 2) || autismweaponcheck(ART_AXELLE_S_CHAIN) )
#define SuperFemtrapElise	( (have_femtrapelise() == 2) || (uarmf && uarmf->oartifact == ART_RONIKA_S_NORM) )
#define SuperFemtrapJohanna	( (have_femtrapjohanna() == 2) )
#define SuperFemtrapInge	( (have_femtrapinge() == 2) )
#define SuperFemtrapMarleen	( (have_femtrapmarleen() == 2) )
#define SuperFemtrapKlara	( (have_femtrapklara() == 2) )
#define SuperFemtrapFriederike	( (have_femtrapfriederike() == 2) )
#define SuperFemtrapUte	( (have_femtrapute() == 2) )
#define SuperFemtrapYasaman	( (have_femtrapyasaman() == 2) )
#define SuperFemtrapMayBritt	( (have_femtrapmaybritt() == 2) )
#define SuperFemtrapNadine	( (have_femtrapnadine() == 2) )
#define SuperFemtrapLuisa	( (have_femtrapluisa() == 2) || (uamul && uamul->oartifact == ART_REGGIE_S_POINTSTEP) )
#define SuperFemtrapLiselotte	( (have_femtrapliselotte() == 2) )
#define SuperFemtrapGreta	( (have_femtrapgreta() == 2) || (uamul && uamul->oartifact == ART_HIBA_CHIBA) )
#define SuperFemtrapCharlotte	( (have_femtrapcharlotte() == 2) )
#define SuperFemtrapLittleMarie	( (have_femtraplittlemarie() == 2) )
#define SuperFemtrapTanja	( (have_femtraptanja() == 2) )
#define SuperFemtrapSonja	( (have_femtrapsonja() == 2) )
#define SuperFemtrapRhea	( (have_femtraprhea() == 2) )
#define SuperFemtrapLara	( (have_femtraplara() == 2) || (uarmg && uarmg->oartifact == ART_ELARA_S_AGILITY) )

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
#define RngeMultishot		(u.uprops[RNGE_MULTISHOT].intrinsic || u.uprops[RNGE_MULTISHOT].extrinsic)
#define RngeSkillReveal		(u.uprops[RNGE_SKILL_REVEAL].intrinsic || u.uprops[RNGE_SKILL_REVEAL].extrinsic)
#define RngeTechInsight		(u.uprops[RNGE_TECH_INSIGHT].intrinsic || u.uprops[RNGE_TECH_INSIGHT].extrinsic)
#define RngeListening		(u.uprops[RNGE_LISTENING].intrinsic || u.uprops[RNGE_LISTENING].extrinsic)
#define RngeCannibalism		(u.uprops[RNGE_CANNIBALISM].intrinsic || u.uprops[RNGE_CANNIBALISM].extrinsic)
#define RngePolyvision		(u.uprops[RNGE_POLYVISION].intrinsic || u.uprops[RNGE_POLYVISION].extrinsic)
#define RngeHemophilia		(u.uprops[RNGE_HEMOPHILIA].intrinsic || u.uprops[RNGE_HEMOPHILIA].extrinsic)
#define RngeBlastShielding		(u.uprops[RNGE_BLAST_SHIELDING].intrinsic || u.uprops[RNGE_BLAST_SHIELDING].extrinsic)
#define RngeFunnyHallu		(u.uprops[RNGE_FUNNY_HALLU].intrinsic || u.uprops[RNGE_FUNNY_HALLU].extrinsic)
#define RngePolypiling		(u.uprops[RNGE_POLYPILING].intrinsic || u.uprops[RNGE_POLYPILING].extrinsic)
#define RngeSapGeneration		(u.uprops[RNGE_SAP_GENERATION].intrinsic || u.uprops[RNGE_SAP_GENERATION].extrinsic)
#define RngeWealth		(u.uprops[RNGE_WEALTH].intrinsic || u.uprops[RNGE_WEALTH].extrinsic)
#define RngeWhoring		(u.uprops[RNGE_WHORING].intrinsic || u.uprops[RNGE_WHORING].extrinsic)
#define RngeBossing		(u.uprops[RNGE_BOSSING].intrinsic || u.uprops[RNGE_BOSSING].extrinsic)
#define RngeStench		(u.uprops[RNGE_STENCH].intrinsic || u.uprops[RNGE_STENCH].extrinsic)
#define RngeEcholocation		(u.uprops[RNGE_ECHOLOCATION].intrinsic || u.uprops[RNGE_ECHOLOCATION].extrinsic)
#define RngeStackMessaging		(u.uprops[RNGE_STACKMESSAGING].intrinsic || u.uprops[RNGE_STACKMESSAGING].extrinsic)
#define RngeOverviewImprovement		(u.uprops[RNGE_OVERVIEW_IMPROVEMENT].intrinsic || u.uprops[RNGE_OVERVIEW_IMPROVEMENT].extrinsic)
#define RngeNastyReduction		(u.uprops[RNGE_NASTY_REDUCTION].intrinsic || u.uprops[RNGE_NASTY_REDUCTION].extrinsic)
#define RngeTechReuse		(u.uprops[RNGE_TECH_REUSE].intrinsic || u.uprops[RNGE_TECH_REUSE].extrinsic)
#define RngeCarryingBoost		(u.uprops[RNGE_CARRYING_BOOST].intrinsic || u.uprops[RNGE_CARRYING_BOOST].extrinsic)
#define RngeKillerFilling		(u.uprops[RNGE_KILLER_FILLING].intrinsic || u.uprops[RNGE_KILLER_FILLING].extrinsic)
#define RngeMulchingDisplay		(u.uprops[RNGE_MULCHING_DISPLAY].intrinsic || u.uprops[RNGE_MULCHING_DISPLAY].extrinsic)
#define RngeStatcapIncrease		(u.uprops[RNGE_STATCAP_INCREASE].intrinsic || u.uprops[RNGE_STATCAP_INCREASE].extrinsic)
#define RngeAntiCovid		(u.uprops[RNGE_ANTI_COVID].intrinsic || u.uprops[RNGE_ANTI_COVID].extrinsic)
#define RngeFluidatorReduction		(u.uprops[RNGE_FLUIDATOR_REDUCTION].intrinsic || u.uprops[RNGE_FLUIDATOR_REDUCTION].extrinsic)
#define RngeScrounging		(u.uprops[RNGE_SCROUNGING].intrinsic || u.uprops[RNGE_SCROUNGING].extrinsic)
#define RngeFarming		(u.uprops[RNGE_FARMING].intrinsic || u.uprops[RNGE_FARMING].extrinsic)

#define UHaveAids		(RngeAids || autismweaponcheck(ART_IT_S_A_POLEARM))

/* Hallucination is solely a timeout; its resistance is extrinsic; Amy edit: now also an intrinsic */
#define HHallucination		u.uprops[HALLUC].intrinsic
#define EHallucination		u.uprops[HALLUC].extrinsic

#define EHalluc_resistance	u.uprops[HALLUC_RES].extrinsic
#define HHalluc_resistance	u.uprops[HALLUC_RES].intrinsic
#define IntHalluc_resistance	(HHalluc_resistance || (Upolyd && dmgtype(youmonst.data, AD_HALU)) || Race_if(PM_EROSATOR) || tech_inuse(T_STAT_RESIST) || (Role_if(PM_TRANSSYLVANIAN) && uwep && (uwep->otyp == WEDGED_LITTLE_GIRL_SANDAL || uwep->otyp == SOFT_GIRL_SNEAKER || uwep->otyp == STURDY_PLATEAU_BOOT_FOR_GIRLS || uwep->otyp == HUGGING_BOOT || uwep->otyp == BLOCK_HEELED_COMBAT_BOOT || uwep->otyp == WOODEN_GETA || uwep->otyp == LACQUERED_DANCING_SHOE || uwep->otyp == SEXY_MARY_JANE || uwep->otyp == KITTEN_HEEL_PUMP || uwep->otyp == HIGH_HEELED_SANDAL || uwep->otyp == SEXY_LEATHER_PUMP || uwep->otyp == STICKSHOE || uwep->otyp == SPIKED_BATTLE_BOOT || uwep->otyp == INKA_BOOT || uwep->otyp == SOFT_LADY_SHOE || uwep->otyp == STEEL_CAPPED_SANDAL || uwep->otyp == BLOCK_HEELED_SANDAL || uwep->otyp == PROSTITUTE_SHOE || uwep->otyp == DOGSHIT_BOOT) ) )
#define ExtHalluc_resistance	(EHalluc_resistance || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == HALLUC_RES) ) || have_allisloststone() || (Race_if(PM_BATMAN) && uwep && uwep->oartifact == ART_BLACKSWANDIR))

#define Halluc_resistance	( ( (ExtHalluc_resistance && u.nonextrinsicproperty != HALLUC_RES) || (IntHalluc_resistance && u.nonintrinsicproperty != HALLUC_RES) ) && !u.halresdeactivated && !NoHalluc_resistance)
#define StrongHalluc_resistance	(IntHalluc_resistance && ExtHalluc_resistance && Halluc_resistance && u.nondoubleproperty != HALLUC_RES)

#define NoHalluc_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_HALLUC_RES].intrinsic || UHaveAids || (u.impossibleproperty == HALLUC_RES) || (uarmh && uarmh->oartifact == ART_SEE_THE_TOTAL) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define Hallucination		((HHallucination && !Halluc_resistance) || (u.uprops[MULTISHOES].extrinsic && !Halluc_resistance) || autismweaponcheck(ART_FADED_USELESSNESS) || autismweaponcheck(ART_LANCE_OF_LONGINUS) || autismweaponcheck(ART_SCHWANZUS_LANGUS) || (HeavyHallu && !Halluc_resistance) || (EHallucination && !Halluc_resistance) || u.uprops[SENSORY_DEPRIVATION].extrinsic || flags.hippie || ( (u.uprops[DEHYDRATION].extrinsic || Dehydration || have_dehydratingstone()) && ((u.dehydrationtime - moves) < 1) )  )
#define FunnyHallu		(Role_if(PM_GOFF) || (uarmc && uarmc->oartifact == ART_BE_HI_WITHOUT_DRUGS) || RngeFunnyHallu || ishallucinator || HHallucination || u.uprops[MULTISHOES].extrinsic || autismweaponcheck(ART_FADED_USELESSNESS) || autismweaponcheck(ART_LANCE_OF_LONGINUS) || (uamul && uamul->oartifact == ART_FUN_ALL) || have_leohighlantern() || autismweaponcheck(ART_SCHWANZUS_LANGUS) || HeavyHallu || EHallucination || u.uprops[SENSORY_DEPRIVATION].extrinsic || (u.funnyhalluroll != 9999999 && u.usanity > u.funnyhalluroll) || flags.hippie || ( (u.uprops[DEHYDRATION].extrinsic || Dehydration || have_dehydratingstone()) && ((u.dehydrationtime - moves) < 1) )  )
/* added possibility of playing the entire game hallucinating --Amy*/
#define HeavyHallu		u.uprops[HEAVY_HALLU].intrinsic

/* Timeout, plus a worn mask */
#define HFumbling		u.uprops[FUMBLING].intrinsic
#define EFumbling		u.uprops[FUMBLING].extrinsic
#define Fumbling		(HFumbling || EFumbling || (uarmf && uarmf->otyp == ROLLER_BLADE) || u.uprops[MULTISHOES].extrinsic)

#define HWounded_legs		u.uprops[WOUNDED_LEGS].intrinsic
#define EWounded_legs		u.uprops[WOUNDED_LEGS].extrinsic
#define Wounded_legs		((HWounded_legs || EWounded_legs || (uarmf && uarmf->oartifact == ART_CINDERELLA_S_SLIPPERS) || (uarmf && uarmf->oartifact == ART_UNTRAINED_HALF_MARATHON) || u.uprops[MULTISHOES].extrinsic) && !(uarmf && uarmf->oartifact == ART_ELIANE_S_SHIN_SMASH) )

#define HSleeping		u.uprops[SLEEPING].intrinsic
#define ESleeping		u.uprops[SLEEPING].extrinsic
#define Sleeping		(HSleeping || ESleeping || Race_if(PM_KOBOLT))

#define HHunger			u.uprops[HUNGER].intrinsic
#define EHunger			u.uprops[HUNGER].extrinsic
#define IntHunger	(HHunger || Role_if(PM_ALLTECHER) || u.alltecherpersist || (uarm && uarm->oartifact == ART_COAL_PEER) || Race_if(PM_GIGANT) || Race_if(PM_GAVIL))
#define ExtHunger	(EHunger || (uarm && uarm->oartifact == ART_COAL_PEER) || (uarmc && uarmc->oartifact == ART_INA_S_LAB_COAT) || autismweaponcheck(ART_GIANT_MEAT_STICK) || autismweaponcheck(ART_CALF_CUTLET_WITHOUT_BEOD) || autismweaponcheck(ART_GUARDIAN_OF_ARANOCH) || (uarmc && uarmc->oartifact == ART_INA_S_OVERCOAT) || (uarmh && uarmh->oartifact == ART_DRINK_COCA_COLA) || (uarm && uarm->oartifact == ART_BELLY_W) || (uarmh && uarmh->oartifact == ART_HAMBURG_ONE) || autismweaponcheck(ART_MAEVE_S_OPULENCE) || (uarmc && uarmc->oartifact == ART_FULLY_LIONIZED) || (uarmc && uarmc->oartifact == ART_FAST_CAMO_PREDATOR) || autismringcheck(ART_RING_OF_WOE) || (uimplant && uimplant->oartifact == ART_BUCKET_HOUSE) || autismweaponcheck(ART_PICK_OF_THE_GRAVE) )

#define Hunger			(IntHunger || ExtHunger)
#define StrongHunger	(IntHunger && ExtHunger && Hunger)


/*** Vision and senses ***/
#define HSee_invisible		u.uprops[SEE_INVIS].intrinsic
#define ESee_invisible		u.uprops[SEE_INVIS].extrinsic
#define IntSee_invisible	(HSee_invisible || perceives(youmonst.data))
#define ExtSee_invisible	(ESee_invisible || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == SEE_INVIS) ) || (uarmc && uarmc->oartifact == ART_VISIBLE_INVISIBILITITY) || (uarmh && uarmh->oartifact == ART_CHRISTMAS_MEDAL) || (uarmh && uarmh->oartifact == ART_APOTHEOSIS_VEIL) || (uwep && uwep->oartifact == ART_GONDOLIN_S_HIDDEN_PASSAGE) || (uwep && uwep->oartifact == ART_MIRACLE_PAINT) || (uimplant && uimplant->oartifact == ART_CORONATION_CULMINATION) || (ublindf && ublindf->oartifact == ART_EYES_OF_THE_ORACLE) || (ublindf && ublindf->oartifact == ART_LENSES_OF_TRUTH) || (uarmh && uarmh->oartifact == ART_SAGE_S_HELM) || (uwep && uwep->oartifact == ART_MAUSER_C___CUSTOM) || (uwep && uwep->oartifact == ART_LUCKY_DAGGER) || (uwep && uwep->oartifact == ART_THIN_DAGGER) || (uwep && uwep->oartifact == ART_HEAVY_CROSSBOW_OF_ETERNITY) || (uwep && uwep->oartifact == ART_LONG_SWORD_OF_ETERNITY) || (uwep && uwep->oartifact == ART_MAGE_STAFF_OF_ETERNITY) || (uarmc && uarmc->oartifact == ART_RUFFLED_SHIRT_OF_THE_ARIST) || (uimplant && uimplant->oartifact == ART_POTATOROK) || (uarmf && uarmf->oartifact == ART_JULIA_S_SLIPPERS) || (uamul && uamul->oartifact == ART_ARABELLA_S_DICINATOR) || (uarmc && uarmc->oartifact == ART_CAN_T_KILL_WHAT_YOU_CAN_T_) || (uwep && uwep->oartifact == ART_SECRETS_OF_INVISIBLE_PLEAS))

#define See_invisible		(((IntSee_invisible && u.nonintrinsicproperty != SEE_INVIS) || (ExtSee_invisible && u.nonextrinsicproperty != SEE_INVIS)) && !Race_if(PM_ELEMENTAL) && !NoSee_invisible)
#define StrongSee_invisible	(IntSee_invisible && ExtSee_invisible && See_invisible && u.nondoubleproperty != SEE_INVIS)

#define NoSee_invisible	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SEE_INVIS].intrinsic || UHaveAids || (u.impossibleproperty == SEE_INVIS) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HTelepat		u.uprops[TELEPAT].intrinsic
#define ETelepat		u.uprops[TELEPAT].extrinsic
#define IntTelepat	(HTelepat || (uarmf && uarmf->oartifact == ART_GLITCH_IN_THE_MONITOR) || (uarmf && uarmf->oartifact == ART_SCAN_ME) || (uarmh && uarmh->oartifact == ART_SUPERPATHY) || Race_if(PM_DEVELOPER) || telepathic(youmonst.data) || (Role_if(PM_TOPMODEL) && PlayerInHighHeels) )
#define ExtTelepat	(ETelepat || (uarmf && uarmf->oartifact == ART_SCAN_ME) || (uamul && uamul->oartifact == ART_AMULET_OF_SPLENDOR) || (uarmh && uarmh->oartifact == ART_INCREDIBLE_VIEW) || (uarms && uarms->oartifact == ART_IT_IS_COMPLETE_NOW) || Race_if(PM_DEVELOPER))

#define Blind_telepat		(!(u.nonintrinsicproperty == TELEPAT) && !Race_if(PM_KUTAR) && IntTelepat && !NoTelepat && !DetectionMethodsDontWork && !u.powerfailure && !(Role_if(PM_TOPMODEL) && flags.female && !PlayerInHighHeels) )

#define Unblind_telepat		(!(u.nonextrinsicproperty == TELEPAT) && !Race_if(PM_KUTAR) && ExtTelepat && !NoTelepat && !DetectionMethodsDontWork)
#define StrongTelepat		(IntTelepat && ExtTelepat && Blind_telepat && Unblind_telepat && u.nondoubleproperty != TELEPAT)

#define NoTelepat	( (!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_TELEPAT].intrinsic || Race_if(PM_KUTAR) || UHaveAids || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0))) || (uarmc && uarmc->oartifact == ART_GODLESS_VOID) || (u.impossibleproperty == TELEPAT) || (FemtrapActiveNaomi && !PlayerInHighHeels) || (uarmh && uarmh->oartifact == ART_WOLF_KING) || (uarmh && ( (uarmh->otyp == TINFOIL_HELMET) || itemhasappearance(uarmh, APP_ALUMINIUM_HELMET) ) ) )

#define HHallu_party	u.uprops[HALLU_PARTY].intrinsic
#define EHallu_party	u.uprops[HALLU_PARTY].extrinsic
#define IntHallu_party	(HHallu_party)
#define ExtHallu_party	(EHallu_party || (uwep && uwep->oartifact == ART_SA_BLA_NDO_MWA) || (uarmh && uarmh->oartifact == ART_INCREDIBLE_VIEW) || (uarms && uarms->oartifact == ART_IT_IS_COMPLETE_NOW) )

#define Hallu_party	(((IntHallu_party && !(u.nonintrinsicproperty == HALLU_PARTY)) || (ExtHallu_party && u.nonextrinsicproperty != HALLU_PARTY)) && !NoHallu_party)
#define StrongHallu_party	(IntHallu_party && ExtHallu_party && Hallu_party && u.nondoubleproperty != HALLU_PARTY)

#define NoHallu_party	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_HALLU_PARTY].intrinsic || UHaveAids || (u.impossibleproperty == HALLU_PARTY) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HDrunken_boxing		u.uprops[DRUNKEN_BOXING].intrinsic
#define EDrunken_boxing		u.uprops[DRUNKEN_BOXING].extrinsic
#define IntDrunken_boxing	(HDrunken_boxing)
#define ExtDrunken_boxing	(EDrunken_boxing || autismringcheck(ART_PUZZLE_ME_MAYBE) || (uarmh && uarmh->oartifact == ART_INCREDIBLE_VIEW) || (uarms && uarms->oartifact == ART_IT_IS_COMPLETE_NOW) || (uarmf && uarmf->oartifact == ART_WILD_SEX_GAME))

#define Drunken_boxing		(((IntDrunken_boxing && !(u.nonintrinsicproperty == DRUNKEN_BOXING)) || (ExtDrunken_boxing && u.nonextrinsicproperty != DRUNKEN_BOXING)) && !NoDrunken_boxing)
#define StrongDrunken_boxing	(IntDrunken_boxing && ExtDrunken_boxing && Drunken_boxing && u.nondoubleproperty != DRUNKEN_BOXING)

#define NoDrunken_boxing	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_DRUNKEN_BOXING].intrinsic || UHaveAids || (u.impossibleproperty == DRUNKEN_BOXING) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HStunnopathy	u.uprops[STUNNOPATHY].intrinsic
#define EStunnopathy	u.uprops[STUNNOPATHY].extrinsic
#define IntStunnopathy	(HStunnopathy)
#define ExtStunnopathy	(EStunnopathy || (uarmh && uarmh->oartifact == ART_INCREDIBLE_VIEW) || (uarms && uarms->oartifact == ART_IT_IS_COMPLETE_NOW) || (uarmc && uarmc->oartifact == ART_PORTER_S_THINK) )

#define Stunnopathy	(((IntStunnopathy && !(u.nonintrinsicproperty == STUNNOPATHY)) || (ExtStunnopathy && u.nonextrinsicproperty != STUNNOPATHY)) && !NoStunnopathy && !DetectionMethodsDontWork)
#define StrongStunnopathy	(IntStunnopathy && ExtStunnopathy && Stunnopathy && u.nondoubleproperty != STUNNOPATHY)

#define NoStunnopathy	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_STUNNOPATHY].intrinsic || UHaveAids || (u.impossibleproperty == STUNNOPATHY) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HNumbopathy	u.uprops[NUMBOPATHY].intrinsic
#define ENumbopathy	u.uprops[NUMBOPATHY].extrinsic
#define IntNumbopathy	(HNumbopathy)
#define ExtNumbopathy	(ENumbopathy || (uarmh && uarmh->oartifact == ART_INCREDIBLE_VIEW) || (uarms && uarms->oartifact == ART_IT_IS_COMPLETE_NOW) || (uarmf && uarmf->oartifact == ART_END_OF_LEWDNESS) )

#define Numbopathy	(((IntNumbopathy && !(u.nonintrinsicproperty == NUMBOPATHY)) || (ExtNumbopathy && u.nonextrinsicproperty != NUMBOPATHY)) && !NoNumbopathy && !DetectionMethodsDontWork)
#define StrongNumbopathy	(IntNumbopathy && ExtNumbopathy && Numbopathy && u.nondoubleproperty != NUMBOPATHY)

#define NoNumbopathy	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_NUMBOPATHY].intrinsic || UHaveAids || (u.impossibleproperty == NUMBOPATHY) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HDimmopathy	u.uprops[DIMMOPATHY].intrinsic
#define EDimmopathy	u.uprops[DIMMOPATHY].extrinsic
#define IntDimmopathy	(HDimmopathy)
#define ExtDimmopathy	(EDimmopathy || (uarmh && uarmh->oartifact == ART_INCREDIBLE_VIEW) || (uarms && uarms->oartifact == ART_IT_IS_COMPLETE_NOW) || (uwep && uwep->oartifact == ART_HOPE_OF_SOKOBAN) || (uarmf && uarmf->oartifact == ART_FINAL_EXAM_TIME))

#define Dimmopathy	(((IntDimmopathy && !(u.nonintrinsicproperty == DIMMOPATHY)) || (ExtDimmopathy && u.nonextrinsicproperty != DIMMOPATHY)) && !NoDimmopathy && !DetectionMethodsDontWork)
#define StrongDimmopathy	(IntDimmopathy && ExtDimmopathy && Dimmopathy && u.nondoubleproperty != DIMMOPATHY)

#define NoDimmopathy	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_DIMMOPATHY].intrinsic || UHaveAids || (u.impossibleproperty == DIMMOPATHY) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HFreezopathy	u.uprops[FREEZOPATHY].intrinsic
#define EFreezopathy	u.uprops[FREEZOPATHY].extrinsic
#define IntFreezopathy	(HFreezopathy)
#define ExtFreezopathy	(EFreezopathy || (uchain && uchain->oartifact == ART_HEI_THE_COLD) || (uarmf && uarmf->oartifact == ART_LITTLE_ICE_BLOCK_WITH_THE_) || (uarmh && uarmh->oartifact == ART_INCREDIBLE_VIEW) || (uarms && uarms->oartifact == ART_IT_IS_COMPLETE_NOW) || (uarmc && uarmc->oartifact == ART_SIECHELALUER) )

#define Freezopathy	(((IntFreezopathy && !(u.nonintrinsicproperty == FREEZOPATHY)) || (ExtFreezopathy && u.nonextrinsicproperty != FREEZOPATHY)) && !NoFreezopathy && !DetectionMethodsDontWork)
#define StrongFreezopathy	(IntFreezopathy && ExtFreezopathy && Freezopathy && u.nondoubleproperty != FREEZOPATHY)

#define NoFreezopathy	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_FREEZOPATHY].intrinsic || UHaveAids || (u.impossibleproperty == FREEZOPATHY) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HStoned_chiller		u.uprops[STONED_CHILLER].intrinsic
#define EStoned_chiller		u.uprops[STONED_CHILLER].extrinsic
#define IntStoned_chiller	(HStoned_chiller)
#define ExtStoned_chiller	(EStoned_chiller || (uarmh && uarmh->oartifact == ART_INCREDIBLE_VIEW) || (uarms && uarms->oartifact == ART_IT_IS_COMPLETE_NOW) || (uwep && uwep->oartifact == ART_BRIGHTE_SEE__EEEE) )

#define Stoned_chiller		(((IntStoned_chiller && !(u.nonintrinsicproperty == STONED_CHILLER)) || (ExtStoned_chiller && u.nonextrinsicproperty != STONED_CHILLER)) && !NoStoned_chiller)
#define StrongStoned_chiller	(IntStoned_chiller && ExtStoned_chiller && Stoned_chiller && u.nondoubleproperty != STONED_CHILLER)

#define NoStoned_chiller	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_STONED_CHILLER].intrinsic || UHaveAids || (u.impossibleproperty == STONED_CHILLER) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HCorrosivity	u.uprops[CORROSIVITY].intrinsic
#define ECorrosivity	u.uprops[CORROSIVITY].extrinsic
#define IntCorrosivity	(HCorrosivity)
#define ExtCorrosivity	(ECorrosivity || (uarmh && uarmh->oartifact == ART_INCREDIBLE_VIEW) || (uarms && uarms->oartifact == ART_IT_IS_COMPLETE_NOW) || (uarmh && uarmh->oartifact == ART_VERSCENT_) )

#define Corrosivity	(((IntCorrosivity && !(u.nonintrinsicproperty == CORROSIVITY)) || (ExtCorrosivity && u.nonextrinsicproperty != CORROSIVITY)) && !NoCorrosivity)
#define StrongCorrosivity	(IntCorrosivity && ExtCorrosivity && Corrosivity && u.nondoubleproperty != CORROSIVITY)

#define NoCorrosivity	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_CORROSIVITY].intrinsic || UHaveAids || (u.impossibleproperty == CORROSIVITY) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HFear_factor	u.uprops[FEAR_FACTOR].intrinsic
#define EFear_factor	u.uprops[FEAR_FACTOR].extrinsic
#define IntFear_factor	(HFear_factor)
#define ExtFear_factor	(EFear_factor || (uarmf && uarmf->oartifact == ART_AERES_RUNNING_SHOES) || (uarmh && uarmh->oartifact == ART_INCREDIBLE_VIEW) || (uarmf && uarmf->oartifact == ART_ELENA_S_EPITOME) || (uarms && uarms->oartifact == ART_IT_IS_COMPLETE_NOW) || (uarmf && uarmf->oartifact == ART_FAR_EAST_RELATION) || (uarmf && uarmf->oartifact == ART_LISSIE_S_SHEAGENTUR))

#define Fear_factor	(((IntFear_factor && !(u.nonintrinsicproperty == FEAR_FACTOR)) || (ExtFear_factor && u.nonextrinsicproperty != FEAR_FACTOR)) && !NoFear_factor)
#define StrongFear_factor	(IntFear_factor && ExtFear_factor && Fear_factor && u.nondoubleproperty != FEAR_FACTOR)

#define NoFear_factor	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_FEAR_FACTOR].intrinsic || UHaveAids || (u.impossibleproperty == FEAR_FACTOR) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HBurnopathy	u.uprops[BURNOPATHY].intrinsic
#define EBurnopathy	u.uprops[BURNOPATHY].extrinsic
#define IntBurnopathy	(HBurnopathy)
#define ExtBurnopathy	(EBurnopathy || (uarmh && uarmh->oartifact == ART_INCREDIBLE_VIEW) || (uarms && uarms->oartifact == ART_IT_IS_COMPLETE_NOW) || (uarmf && uarmf->oartifact == ART_BITCHSMOKE) || (uarmf && uarmf->oartifact == ART_BALE_OF_BODEN_SPEEDSTOCK))

#define Burnopathy	(((IntBurnopathy && !(u.nonintrinsicproperty == BURNOPATHY)) || (ExtBurnopathy && u.nonextrinsicproperty != BURNOPATHY)) && !NoBurnopathy && !DetectionMethodsDontWork)
#define StrongBurnopathy	(IntBurnopathy && ExtBurnopathy && Burnopathy && u.nondoubleproperty != BURNOPATHY)

#define NoBurnopathy	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_BURNOPATHY].intrinsic || UHaveAids || (u.impossibleproperty == BURNOPATHY) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HSickopathy	u.uprops[SICKOPATHY].intrinsic
#define ESickopathy	u.uprops[SICKOPATHY].extrinsic
#define IntSickopathy	(HSickopathy)
#define ExtSickopathy	(ESickopathy || (uarmh && uarmh->oartifact == ART_INCREDIBLE_VIEW) || (uarms && uarms->oartifact == ART_IT_IS_COMPLETE_NOW) || autismringcheck(ART_JUBILEX_S_CODE) || (uarmf && uarmf->oartifact == ART_GREEN_MYSTERY && uwep && (objects[uwep->otyp].oc_color == CLR_BRIGHT_GREEN)) || (uwep && uwep->oartifact == ART_SCHOSCHO_BARBITUER) )

#define Sickopathy	(((IntSickopathy && !(u.nonintrinsicproperty == SICKOPATHY)) || (ExtSickopathy && u.nonextrinsicproperty != SICKOPATHY)) && !NoSickopathy && !DetectionMethodsDontWork)
#define StrongSickopathy	(IntSickopathy && ExtSickopathy && Sickopathy && u.nondoubleproperty != SICKOPATHY)

#define NoSickopathy	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SICKOPATHY].intrinsic || UHaveAids || (u.impossibleproperty == SICKOPATHY) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HWonderlegs	u.uprops[WONDERLEGS].intrinsic
#define EWonderlegs	u.uprops[WONDERLEGS].extrinsic
#define IntWonderlegs	(HWonderlegs)
#define ExtWonderlegs	(EWonderlegs || (uarmh && uarmh->oartifact == ART_INCREDIBLE_VIEW) || (uarms && uarms->oartifact == ART_IT_IS_COMPLETE_NOW) || (uwep && uwep->oartifact == ART_DREAMOULE_ELEYELE) || (uarmf && uarmf->oartifact == ART_STAR_SOLES))

#define Wonderlegs	(((IntWonderlegs && !(u.nonintrinsicproperty == WONDERLEGS)) || (ExtWonderlegs && u.nonextrinsicproperty != WONDERLEGS)) && !NoWonderlegs)
#define StrongWonderlegs	(IntWonderlegs && ExtWonderlegs && Wonderlegs && u.nondoubleproperty != WONDERLEGS)

#define NoWonderlegs	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_WONDERLEGS].intrinsic || UHaveAids || (u.impossibleproperty == WONDERLEGS) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HGlib_combat	u.uprops[GLIB_COMBAT].intrinsic
#define EGlib_combat	u.uprops[GLIB_COMBAT].extrinsic
#define IntGlib_combat	(HGlib_combat)
#define ExtGlib_combat	(EGlib_combat || (uarmh && uarmh->oartifact == ART_INCREDIBLE_VIEW) || (uarms && uarms->oartifact == ART_IT_IS_COMPLETE_NOW) || (uarmg && uarmg->oartifact == ART_KAMAHEWA) || (uarmf && uarmf->oartifact == ART_HIT_THEIR_HANDS) || (uarmf && uarmf->oartifact == ART_DON_T_FALL_INTO_THE_ABYSS))

#define Glib_combat	(((IntGlib_combat && !(u.nonintrinsicproperty == GLIB_COMBAT)) || (ExtGlib_combat && u.nonextrinsicproperty != GLIB_COMBAT)) && !NoGlib_combat)
#define StrongGlib_combat	(IntGlib_combat && ExtGlib_combat && Glib_combat && u.nondoubleproperty != GLIB_COMBAT)

#define NoGlib_combat	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_GLIB_COMBAT].intrinsic || UHaveAids || (u.impossibleproperty == GLIB_COMBAT) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HManaleech	u.uprops[MANALEECH].intrinsic
#define EManaleech	u.uprops[MANALEECH].extrinsic
#define IntManaleech	(HManaleech || (uamul && uamul->oartifact == ART_HUUUUUGE_POWER) )
#define ExtManaleech	(EManaleech || (uamul && uamul->oartifact == ART_HUUUUUGE_POWER) || (uwep && uwep->oartifact == ART_MANA_EATER) || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == MANALEECH) ) || (uarmu && uarmu->oartifact == ART_VICTORIA_IS_EVIL_BUT_PRETT) || (uarmu && uarmu->oartifact == ART_NATALIA_IS_LOVELY_BUT_DANG) || (uarmf && uarmf->oartifact == ART_HIT_THEIR_HANDS) || (uarmf && uarmf->oartifact == ART_SOLVEJG_S_STINKING_SLIPPER) || (uarm && uarm->oartifact == ART_SILKS_OF_THE_VICTOR) || (uarm && flags.female && uarm->oartifact == ART_PRETTY_LITTLE_MAGICAL_GIRL) || (uarmh && uarmh->oartifact == ART_REFUEL_BADLY) || (uamul && uamul->oartifact == ART_BALLSY_BASTARD) || (uamul && uamul->oartifact == ART_MADMAN_S_POWER) || (uamul && uamul->oartifact == ART___TH_NAZGUL) || (uamul && uamul->oartifact == ART_NAZGUL_S_REVENGE) )

#define Manaleech		(((IntManaleech && u.nonintrinsicproperty != MANALEECH) || (ExtManaleech && u.nonextrinsicproperty != MANALEECH)) && !NoManaleech)
#define StrongManaleech	(IntManaleech && ExtManaleech && Manaleech && u.nondoubleproperty != MANALEECH)

#define NoManaleech	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_MANALEECH].intrinsic || UHaveAids || (u.impossibleproperty == MANALEECH) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HPeacevision	u.uprops[PEACEVISION].intrinsic
#define EPeacevision	u.uprops[PEACEVISION].extrinsic
#define IntPeacevision	(HPeacevision || (uwep && objects[uwep->otyp].oc_material == MT_MENGETIUM) )
#define ExtPeacevision	(EPeacevision || (powerfulimplants() && uimplant && uimplant->oartifact == ART_SAATUSPLEASE) || (uarmf && uarmf->oartifact == ART_BUNNY_ROCKZ) || (uarmc && uarmc->oartifact == ART_PEACE_BROTHER) || (uwep && uwep->oartifact == ART_LENA_S_PEACE_OFFERING) || have_burnerdream() || (uwep && uwep->oartifact == ART_GRIENE_EIDI) || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == PEACEVISION) ) )

#define Peacevision		(((IntPeacevision && u.nonintrinsicproperty != PEACEVISION) || (ExtPeacevision && u.nonextrinsicproperty != PEACEVISION)) && !NoPeacevision)
#define StrongPeacevision	(IntPeacevision && ExtPeacevision && Peacevision && u.nondoubleproperty != PEACEVISION)

#define NoPeacevision	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_PEACEVISION].intrinsic || UHaveAids || (u.impossibleproperty == PEACEVISION) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HMap_amnesia	u.uprops[MAP_AMNESIA].intrinsic
#define EMap_amnesia	u.uprops[MAP_AMNESIA].extrinsic
#define IntMap_amnesia	(HMap_amnesia)
#define ExtMap_amnesia	(EMap_amnesia || autismweaponcheck(ART_MINOPOWER) )
#define Map_amnesia	(IntMap_amnesia || ExtMap_amnesia || In_minotaurmaze(&u.uz) || In_voiddungeon(&u.uz) || In_netherrealm(&u.uz))
#define StrongMap_amnesia	(IntMap_amnesia && ExtMap_amnesia && Map_amnesia)

#define HWarning		u.uprops[WARNING].intrinsic
#define EWarning		u.uprops[WARNING].extrinsic
#define IntWarning	(HWarning)
#define ExtWarning	(EWarning || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == WARNING) ) || (Role_if(PM_TOPMODEL) && PlayerInHighHeels) )

#define Warning			(((IntWarning && u.nonintrinsicproperty != WARNING) || (ExtWarning && u.nonextrinsicproperty != WARNING)) && !Race_if(PM_KUTAR) && !u.powerfailure && !NoWarning && !DetectionMethodsDontWork && !(Role_if(PM_TOPMODEL) && flags.female && !PlayerInHighHeels) )
#define StrongWarning	(IntWarning && ExtWarning && Warning && u.nondoubleproperty != WARNING)

#define NoWarning	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_WARNING].intrinsic || UHaveAids || (u.impossibleproperty == WARNING) || autismweaponcheck(ART_HYPER_INTELLIGENCE) || (FemtrapActiveNaomi && !PlayerInHighHeels) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

/* Warning for a specific type of monster */
#define HWarn_of_mon		u.uprops[WARN_OF_MON].intrinsic
#define EWarn_of_mon		u.uprops[WARN_OF_MON].extrinsic
#define Warn_of_mon		(HWarn_of_mon || EWarn_of_mon)

#define HUndead_warning		u.uprops[WARN_UNDEAD].intrinsic
#define EUndead_warning		u.uprops[WARN_UNDEAD].extrinsic
#define Undead_warning		(HUndead_warning || EUndead_warning || (uarmh && uarmh->oartifact == ART_ALL_SEEING_EYE_OF_THE_FLY) || (uarm && uarm->oartifact == ART_CRYPT_SMELL) || (uarmf && uarmf->oartifact == ART_UNDEAD_STINK) || (uarms && uarms->oartifact == ART_SHIELD_OF_THE_PALADIN) || (uarms && uarms->oartifact == ART_SHIELD_OF_THE_CHEVALIER__W) || (uwep && uwep->oartifact == ART_SEARSHARP) || (uwep && uwep->oartifact == ART_LOVEMECHAIN) || (uwep && uwep->oartifact == ART_VLADSBANE) || (uarmh && uarmh->oartifact == ART_FLUE_FLUE_FLUEFLUE_FLUE) || (uarmh && uarmh->oartifact == ART_CONTROLLER_HAT))

#define HSearching		u.uprops[SEARCHING].intrinsic
#define ESearching		u.uprops[SEARCHING].extrinsic
#define IntSearching	(HSearching || (ublindf && ublindf->oartifact == ART_SUB_ETHA) )
#define ExtSearching	(ESearching || (ublindf && ublindf->oartifact == ART_SUB_ETHA) || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == SEARCHING) ) )

#define Searching		(((IntSearching && u.nonintrinsicproperty != SEARCHING) || (ExtSearching && u.nonextrinsicproperty != SEARCHING)) && !Race_if(PM_KUTAR) && !NoSearching)
#define StrongSearching	(IntSearching && ExtSearching && Searching && u.nondoubleproperty != SEARCHING)

#define NoSearching	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SEARCHING].intrinsic || UHaveAids || (u.impossibleproperty == SEARCHING) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HClairvoyant		u.uprops[CLAIRVOYANT].intrinsic
#define EClairvoyant		u.uprops[CLAIRVOYANT].extrinsic
#define BClairvoyant		u.uprops[CLAIRVOYANT].blocked
#define IntClairvoyant	(HClairvoyant)
#define ExtClairvoyant	(EClairvoyant || (uarmf && uarmf->oartifact == ART_TRULY_MAGNIFIED) || (uwep && uwep->oartifact == ART_VARIANT_SOURCE_PACKAGE) || (uwep && uwep->oartifact == ART_TOOTH_OF_SHAI_HULUD) || (uwep && uwep->oartifact == ART_SOUL_EDGE) || (uwep && uwep->oartifact == ART_ANDY_S_SIGNWEISER) || (uwep && uwep->oartifact == ART_VIHAT_BAGUETTEN_BUS_STOP) || (uwep && uwep->oartifact == ART_MYSTERIOUS_FORCE) || (uwep && uwep->oartifact == ART_ZUSE_S_COMP) || (uwep && uwep->oartifact == ART_HELIOPOLIS_MISTAKE) || (uwep && uwep->oartifact == ART_GAUNTLET_ABBREVIATION) || (uarmc && uarmc->oartifact == ART_EAST_PARTY) || (uarm && uarm->oartifact == ART_BECAUSE_YOU_LOSE) || (uarm && uarm->oartifact == ART_ARABELLA_S_FEMINIZER) || autismringcheck(ART_LOOKIE_LOOKIE) || (uwep && uwep->oartifact == ART_TIME_FOR_A_CALL) || (uarmu && uarmu->oartifact == ART_HA_HA_HA_HA___) || (uarmh && uarmh->oartifact == ART_COMPLETE_SIGHT) || (uamul && uamul->oartifact == ART_DEMOBLING) || (uwep && uwep->oartifact == ART_GAME_KNOWLEDGE_CHEAT_SHEET) || (uarmf && uarmf->oartifact == ART_SKETCH_IT) || (uamul && uamul->oartifact == ART_ARABELLA_S_SWOONING_BEAUTY) || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == CLAIRVOYANT) ) || (uamul && uamul->oartifact == ART_WOUUU) || (uarmh && uarmh->oartifact == ART_WSCHIIIIIE_))

#define Clairvoyant		(((IntClairvoyant && u.nonintrinsicproperty != CLAIRVOYANT) || (ExtClairvoyant && u.nonextrinsicproperty != CLAIRVOYANT)) && !BClairvoyant && !NoClairvoyant)
#define StrongClairvoyant	(IntClairvoyant && ExtClairvoyant && Clairvoyant && u.nondoubleproperty != CLAIRVOYANT)

#define NoClairvoyant	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_CLAIRVOYANT].intrinsic || UHaveAids || (u.impossibleproperty == CLAIRVOYANT) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HInfravision		u.uprops[INFRAVISION].intrinsic
#define EInfravision		u.uprops[INFRAVISION].extrinsic
#define IntInfravision	(HInfravision || infravision(youmonst.data) || (uarm && uarm->oartifact == ART_BECAUSE_YOU_LOSE) )
#define ExtInfravision	(EInfravision || (uarmc && uarmc->oartifact == ART_GAGARIN_S_TRANSLATOR) || (uarmf && uarmf->oartifact == ART_ARVOGENIA_S_HIGH_HEELSES) || (ublindf && ublindf->oartifact == ART_TRINNIE_S_SNIPERSIGHTS) || (uarmu && uarmu->oartifact == ART_SOME_CHAMBER_DOOR) || (uarm && uarm->oartifact == ART_ARABELLA_S_LIGHTSWITCH) || (uarmg && itemhasappearance(uarmg, APP_TELESCOPE) && !uarmg->cursed) || (uarm && uarm->oartifact == ART_SEE_THE_MULCH_STATE) || autismringcheck(ART_BERNCELD) || (uwep && uwep->oartifact == ART_MAGE_STAFF_OF_ETERNITY) || (uarmh && uarmh->oartifact == ART_RAT_PROBLEM && (u.uhp == u.uhpmax)) || (ublindf && ublindf->oartifact == ART_MANY_DIOPTRIES) || (uarmg && uarmg->oartifact == ART_SPELLSNIPE) || (uarm && uarm->oartifact == ART_VISIBILITY_INVERTED) || (uarmu && uarmu->oartifact == ART_HA_HA_HA_HA___) || (uarm && uarm->oartifact == ART_BECAUSE_YOU_LOSE) || (uwep && uwep->oartifact == ART_HEAVY_CROSSBOW_OF_ETERNITY) || (uwep && uwep->oartifact == ART_FLING_IN_THE_DARK) || (uarm && uarm->oartifact == ART_DARK_L) || (uwep && uwep->oartifact == ART_SIGHTS_ZOOMED) || (uwep && uwep->oartifact == ART_BLUE_CORSAR_SWIMMING) || (uwep && uwep->oartifact == ART_LETS_MAKE_IT_OFFICIAL && u.kliuskill >= 160) || (uwep && uwep->oartifact == ART_HAL_SCOPE) || (uarmf && uarmf->oartifact == ART_GRAVY_HIDE) || (uwep && uwep->oartifact == ART_ZIRATHA_S_IRON_SIGHTS) || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == INFRAVISION) ) || (uwep && uwep->oartifact == ART_DEMON_MACHINE) )

#define Infravision		(((IntInfravision && u.nonintrinsicproperty != INFRAVISION) || (ExtInfravision && u.nonextrinsicproperty != INFRAVISION)) && !Race_if(PM_KUTAR) && !u.powerfailure && !NoInfravision)
#define StrongInfravision	(IntInfravision && ExtInfravision && Infravision && u.nondoubleproperty != INFRAVISION)

#define NoInfravision	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_INFRAVISION].intrinsic || UHaveAids || (u.impossibleproperty == INFRAVISION) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HDetect_monsters	u.uprops[DETECT_MONSTERS].intrinsic
#define EDetect_monsters	u.uprops[DETECT_MONSTERS].extrinsic
#define IntDetect_monsters	(HDetect_monsters)
#define ExtDetect_monsters	(EDetect_monsters || autismringcheck(ART_ARABELLA_S_RADAR) || (uwep && uwep->oartifact == ART_HOL_ON_MAN) || (ublindf && ublindf->oartifact == ART_TOTAL_PERSPECTIVE_VORTEX) || (uchain && uchain->oartifact == ART_DO_THE_CONTROL) || (uarmh && uarmh->oartifact == ART_CROWN_OF_THE_PERCIPIENT) || (uarm && uarm->oartifact == ART_RADAR_FELL_UP) || (uarms && uarms->oartifact == ART_CAYLEEN_S_BLUSH && u.uhunger >= 4500) || autismringcheck(ART_BILL_S_CHIP) || (uarm && uarm->oartifact == ART_ARABELLA_S_FEMINIZER) || (uwep && uwep->oartifact == ART_ARABELLA_S_THINNER && uball && (uwep == uball) ) || (uamul && uamul->oartifact == ART_SURTERSTAFF && uwep && (weapon_type(uwep) == P_QUARTERSTAFF) ) || (uwep && uwep->oartifact == ART_RAFSCHAR_S_SUPERWEAPON) || u.uprops[STORM_HELM].extrinsic )

#define Detect_monsters		(((IntDetect_monsters && u.nonintrinsicproperty != DETECT_MONSTERS) || (ExtDetect_monsters && u.nonextrinsicproperty != DETECT_MONSTERS)) && !Race_if(PM_KUTAR) && !u.powerfailure && !NoDetect_monsters && !DetectionMethodsDontWork)
#define StrongDetect_monsters	(IntDetect_monsters && ExtDetect_monsters && Detect_monsters && u.nondoubleproperty != DETECT_MONSTERS)

#define NoDetect_monsters	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_DETECT_MONSTERS].intrinsic || UHaveAids || (u.impossibleproperty == DETECT_MONSTERS) || (uarmh && uarmh->oartifact == ART_RADAR_NOT_WORKING) || (isselfhybrid && (moves % 3 == 0) ) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))


/*** Appearance and behavior ***/
#define Adornment		u.uprops[ADORNED].extrinsic

#define HInvis			u.uprops[INVIS].intrinsic
#define EInvis			u.uprops[INVIS].extrinsic
#define BInvis			u.uprops[INVIS].blocked
#define IntInvis	(HInvis || (uarmc && uarmc->oartifact == ART_WHISPERWIND_CLOAK) || (uarmc && uarmc->oartifact == ART_FIND_ME_NEVERTHELESS) || (uarmc && uarmc->oartifact == ART_DOEDOEDOEDOEDOEDOEDOE_TEST) || autismringcheck(ART_CAN_T_FIND_ME) || pm_invisible(youmonst.data))
#define ExtInvis	(EInvis || (uarmc && uarmc->oartifact == ART_WHISPERWIND_CLOAK) || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == INVIS) ) || (uwep && uwep->oartifact == ART_MOVE_IN_THE_SHADOWS) || (uwep && uwep->oartifact == ART_WHEREABOUT_OF_X) || (uarmc && uarmc->oartifact == ART_INVISIBLE_VISIBILITITY) || (uarm && uarm->oartifact == ART_LYNN_S_SECLUDEDNESS) || (uarmf && uarmf->oartifact == ART_BACKGROUND_HOLDING) || (uarmc && uarmc->oartifact == ART_LIBRARY_HIDING) || (uarmc && uarmc->oartifact == ART_FIND_ME_NEVERTHELESS) || (uwep && uwep->oartifact == ART_HEAVY_CROSSBOW_OF_ETERNITY) || (uarm && uarm->oartifact == ART_CLOAK_OF_SHADOWS) || (uarm && uarm->oartifact == ART_ROBE_OF_CLOSED_EYES) || (uarm && uarm->oartifact == ART_STAND_YOU_FORWARDS__THERE_) || (uwep && uwep->oartifact == ART_ORB_OF_FLEECE) || have_invisoloadstone() || (Race_if(PM_BATMAN) && uwep && uwep->oartifact == ART_BLACKSWANDIR) || (uarmc && uarmc->oartifact == ART_DOEDOEDOEDOEDOEDOEDOE_TEST) || (uarmf && uarmf->oartifact == ART_GORGEOUS_VEIL_MODEL) || (uamul && uamul->oartifact == ART_ARABELLA_S_DICINATOR) || (uarmh && uarmh->oartifact == ART_MARLENA_S_SONG) || (uarmh && uarmh->oartifact == ART_DULLIFIER) || (uwep && uwep->oartifact == ART_JARMEN_CHEATERTYPE_KELL) || (uarm && uarm->oartifact == ART_VERY_INVISIBLE) || (uarmc && uarmc->oartifact == ART_CAN_T_KILL_WHAT_YOU_CAN_T_) || (uwep && uwep->oartifact == ART_SECRETS_OF_INVISIBLE_PLEAS) )

#define Invis			(((IntInvis && u.nonintrinsicproperty != INVIS) || (ExtInvis && u.nonextrinsicproperty != INVIS)) && !BInvis && !autismringcheck(ART_BILL_S_CHIP) && !(uarmc && itemhasappearance(uarmc, APP_WARNING_COAT)) && !Race_if(PM_ELEMENTAL) && !(uarm && uarm->oartifact == ART_PLANTOPLIM) && !NoInvisible && !((uwep && uwep->otyp == GROM_AXE) || (u.twoweap && uswapwep && uswapwep->otyp == GROM_AXE)) )
#define Invisible		(Invis && !See_invisible)
		/* Note: invisibility also hides inventory and steed */
#define StrongInvis	(IntInvis && ExtInvis && Invis && u.nondoubleproperty != INVIS)

#define NoInvisible	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_INVIS].intrinsic || UHaveAids || (u.impossibleproperty == INVIS) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HDisplaced		u.uprops[DISPLACED].intrinsic
#define EDisplaced		u.uprops[DISPLACED].extrinsic
#define IntDisplaced	(HDisplaced)
#define ExtDisplaced	(EDisplaced || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == DISPLACED) ) || (uarmc && uarmc->oartifact == ART_IMAGE_PROJECTOR) || (uwep && uwep->oartifact == ART_SKY_RENDER) || (uarm && uarm->oartifact == ART_VERY_INVISIBLE) || (uarmf && uarmf->oartifact == ART_COVER_UP) || (uwep && uwep->oartifact == ART_LAUGHTERSNEE) || (uwep && uwep->oartifact == ART_STAFF_OF_TWELVE_MIRRORS) || (uwep && uwep->oartifact == ART_CHRIS_S_SPIGGL) || (uarmg && uarmg->oartifact == ART_STORMING_FISTS) || (uwep && uwep->oartifact == ART_SESDERIRO) || (uarmf && uarmf->oartifact == ART_WATER_FLOWERS) || (uwep && uwep->oartifact == ART_BELTHRONDING) || (uarm && uarm->oartifact == ART_ROBE_OF_THE_ARCHMAGI) || (uarmh && uarmh->oartifact == ART_MARLENA_S_SONG) || (uarm && uarm->oartifact == ART_SHIVANHUNTER_S_UNUSED_PRIZ) || (uarmc && itemhasappearance(uarmc, APP_SHROUDED_CLOAK) && (moves % 10 == 0) ) || ((moves % 3 == 0) && uarmc && itemhasappearance(uarmc, APP_DEEP_CLOAK) ) || (uarmf && itemhasappearance(uarmf, APP_KOREAN_SANDALS) && (moves % 3 == 0) ) )

#define Displaced		(((IntDisplaced && u.nonintrinsicproperty != DISPLACED) || (ExtDisplaced && u.nonextrinsicproperty != DISPLACED)) && !NoDisplaced && !autismringcheck(ART_BILL_S_CHIP) && !(uarm && uarm->oartifact == ART_PLANTOPLIM))
#define StrongDisplaced	(IntDisplaced && ExtDisplaced && Displaced && u.nondoubleproperty != DISPLACED)

#define NoDisplaced	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_DISPLACED].intrinsic || UHaveAids || (u.impossibleproperty == DISPLACED) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HStealth		u.uprops[STEALTH].intrinsic
#define EStealth		u.uprops[STEALTH].extrinsic
#define BStealth		u.uprops[STEALTH].blocked
#define IntStealth	(HStealth || autismringcheck(ART_XANATHAR_S_RING_OF_PROOF) || (uarmc && uarmc->oartifact == ART_STEALTH_OR_STEALING) || (uarmc && uarmc->oartifact == ART_WHISPERWIND_CLOAK) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_RESTROOM_DRENCHING) || (Race_if(PM_MOON_ELF) && ((flags.moonphase >= 1 && flags.moonphase <= 3) || (uwep && uwep->oartifact == ART_SUPERSTAB) || (flags.moonphase >= 5 && flags.moonphase <= 7))) )
#define ExtStealth	(EStealth || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == STEALTH) ) || u.uprops[MULTISHOES].extrinsic || (uarmf && uarmf->oartifact == ART_BACKGROUND_HOLDING) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_RESTROOM_DRENCHING) || (uarmc && uarmc->oartifact == ART_STEALTH_OR_STEALING) || (Race_if(PM_BATMAN) && uwep && uwep->oartifact == ART_BLACKSWANDIR) || (ublindf && ublindf->otyp == EYECLOSER) || (Role_if(PM_TOPMODEL) && PlayerInHighHeels) )

#define Stealth			(((IntStealth && u.nonintrinsicproperty != STEALTH) || (ExtStealth && u.nonextrinsicproperty != STEALTH)) && !BStealth && !NoStealth && !Race_if(PM_OGRO) && !autismweaponcheck(ART_ARMORWREAKER) && !Race_if(PM_ROHIRRIM) && !Race_if(PM_THUNDERLORD) && !(Role_if(PM_TOPMODEL) && flags.female && !PlayerInHighHeels) )
#define StrongStealth	(IntStealth && ExtStealth && Stealth && u.nondoubleproperty != STEALTH)

#define NoStealth	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_STEALTH].intrinsic || UHaveAids || (uarmc && itemhasappearance(uarmc, APP_EXCREMENT_CLOAK) ) || (u.impossibleproperty == STEALTH) || (uarmf && itemhasappearance(uarmf, APP_HEAP_OF_SHIT_BOOTS)) || have_fireringcandle() || (uarm && uarm->oartifact == ART_HAZARDOUS_EQUIPMENT) || (uarm && uarm->oartifact == ART_YESEXI) || (FemtrapActiveNaomi && !PlayerInHighHeels) || (uarmf && uarmf->oartifact == ART_TOO_MUCH_BRAVERY) || (uarmf && uarmf->oartifact == ART_HENRIETTA_S_DOGSHIT_BOOTS) || FemtrapActiveSolvejg || autismweaponcheck(ART_HENRIETTA_S_MISTAKE) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HExtra_wpn_practice	u.uprops[EXTRA_WEAPON_PRACTICE].intrinsic
#define EExtra_wpn_practice	u.uprops[EXTRA_WEAPON_PRACTICE].extrinsic
#define IntExtra_wpn_practice	(HExtra_wpn_practice || (uimplant && objects[uimplant->otyp].oc_material == MT_MENGETIUM) || have_superjonadabstone())
#define ExtExtra_wpn_practice	(EExtra_wpn_practice || have_superjonadabstone() || (powerfulimplants() && uimplant && uimplant->oartifact == ART_FASTPLANT) || (uwep && uwep->oartifact == ART_ZUSE_S_COMP) || (uwep && uwep->oartifact == ART_ARABELLA_S_BLACK_PRONG) || have_minimejewel() || (uarm && uarm->oartifact == ART_GENERAL_TA_S_ANNOYANCE_FAC) || (uwep && uwep->oartifact == ART_SKYSTORMER) || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == EXTRA_WEAPON_PRACTICE) ) || (uarm && uarm->oartifact == ART_EVERYTHING_COMES_WITH_A_CO) || (uarmu && uarmu->oartifact == ART_CAPITAL_RAP) || (uarmc && uarmc->oartifact == ART_SKILLS_BEAT_STATS) || (uarm && uarm->oartifact == ART_MOTHERFUCKER_TROPHY) )

#define Extra_wpn_practice	(IntExtra_wpn_practice || ExtExtra_wpn_practice)
#define StrongExtra_wpn_practice	(IntExtra_wpn_practice && ExtExtra_wpn_practice && Extra_wpn_practice)

#define HDeath_resistance	u.uprops[DTBEEM_RES].intrinsic
#define EDeath_resistance	u.uprops[DTBEEM_RES].extrinsic
#define IntDeath_resistance	(HDeath_resistance || (Race_if(PM_ETHEREALOID) && !Upolyd) || (Race_if(PM_INCORPOREALOID) && !Upolyd) || Race_if(PM_PLAYER_GOLEM) || (powerfulsymbiosis() && is_death_resistant(&mons[u.usymbiote.mnum]) ) || resists_death(&youmonst) )
#define ExtDeath_resistance	(playerextrinsicdeathres())

#define Death_resistance	(HDeath_resistance || EDeath_resistance)

#define PlayerResistsDeathRays	(Death_resistance || Antimagic)

/* according to Yasdorian, I love aggravate monster. Indeed, many of my artifacts have it. --Amy */
#define HAggravate_monster	u.uprops[AGGRAVATE_MONSTER].intrinsic
#define EAggravate_monster	u.uprops[AGGRAVATE_MONSTER].extrinsic
#define IntAggravate_monster	(HAggravate_monster || Race_if(PM_NEMESIS) || (Race_if(PM_HC_ALIEN) && !flags.female) || (Race_if(PM_SLYER_ALIEN) && !flags.female) || (Race_if(PM_HUMANOID_ANGEL) && (u.ualign.record < 0)) || Race_if(PM_OGRO) || autismweaponcheck(ART_SMASHIN) || Race_if(PM_ROHIRRIM) || Race_if(PM_THUNDERLORD) || (Role_if(PM_PSION) && u.ulevel >= 7) || (Race_if(PM_SYLPH) && u.ulevel >= 18) || (Role_if(PM_PICKPOCKET) && (u.ualign.record < 0)))
#define ExtAggravate_monster	(playerextrinsicaggravatemon())

#define Aggravate_monster	(IntAggravate_monster || ExtAggravate_monster)
#define StrongAggravate_monster	(IntAggravate_monster && ExtAggravate_monster && Aggravate_monster)

#define HConflict		u.uprops[CONFLICT].intrinsic
#define EConflict		u.uprops[CONFLICT].extrinsic
#define IntConflict	(HConflict)
#define ExtConflict	(EConflict || (uarm && uarm->oartifact == ART_HO_OH_S_FEATHERS) || (uarms && uarms->oartifact == ART_SHATTERED_DREAMS) || (uarms && uarms->oartifact == ART_NO_FUTURE_BUT_AGONY) || (uarms && uarms->oartifact == ART_SYSTEMATIC_CHAOS) || autismringcheck(ART_TASTY_TAME_NASTY) || autismweaponcheck(ART_DRAMA_STAFF) || autismweaponcheck(ART_WAR_S_SWORD) || autismweaponcheck(ART_SEVEN_IN_ONE_BLOW) || autismweaponcheck(ART_REVOLTER) || autismweaponcheck(ART_ONE_STRIKE) || (uarms && uarms->oartifact == ART_BONUS_HOLD) || (uarmf && uarmf->oartifact == ART_MANUELA_S_TORTURE_HEELS) || (uarmf && uarmf->oartifact == ART_WEAK_FROM_HUNGER) || (uarmf && uarmf->oartifact == ART_MANUELA_S_UNKNOWN_HEELS) || (uarmc && uarmc->oartifact == ART_NOW_IT_S_FOR_REAL) || (uarmf && uarmf->oartifact == ART_ALLYNONE) || (uarmh && uarmh->oartifact == ART_CERTAIN_SLOW_DEATH) || (uarm && uarm->oartifact == ART_QUEEN_ARTICUNO_S_HULL) || autismweaponcheck(ART_PROVOCATEUR) || autismweaponcheck(ART_CIVIL_WAR) || (uarmf && uarmf->oartifact == ART_FATALITY) || (uarmf && uarmf->oartifact == ART_FUN_ALL_IN_ONE) || autismringcheck(ART_RING_OF_WOE))

#define Conflict		((IntConflict || ExtConflict) && !Is_blackmarket(&u.uz) )
/* Sorry guys and girls, but you need to find another way to clear out Sam's assistants. --Amy */
#define StrongConflict	(IntConflict && ExtConflict && Conflict)

/*** Transportation ***/
#define HJumping		u.uprops[JUMPING].intrinsic
#define EJumping		u.uprops[JUMPING].extrinsic
#define IntJumping	(HJumping || flags.iwbtg)
#define ExtJumping	(EJumping || (uwep && uwep->oartifact == ART_DIZZY_METAL_STORM) || (uarmc && uarmc->oartifact == ART_ROKKO_CHAN_S_SUIT) || (uarmf && uarmf->oartifact == ART_AIRSHIP_DANCING) || (uarmf && uarmf->oartifact == ART_SEVEN_LEAGUE_BOOTS) || (uwep && uwep->oartifact == ART_ON_CLOUD_SEVEN) || (uwep && uwep->oartifact == ART_JUMP_HURRIES) || (uarmf && uarmf->oartifact == ART_JUMP_KICK_ACTION) || (uarmf && uarmf->oartifact == ART_ARVOGENIA_S_BIKER_HEELS && u.usteed) || (uwep && uwep->oartifact == ART_KHAKKHARA_OF_THE_MONKEY) || (uarm && uarm->oartifact == ART_LUKE_S_JEDI_POWER) || (uarmf && uarmf->oartifact == ART_PRINCE_OF_PERSIA) || (uwep && uwep->oartifact == ART_SUPERGIRL_S_JUMP_AND_RUN_F) || (uwep && uwep->oartifact == ART_STRINGFELLOW_HOUKI) || (uamul && uamul->oartifact == ART_ONLY_ONE_ESCAPE) || u.uprops[MULTISHOES].extrinsic)

#define Jumping			(((IntJumping && u.nonintrinsicproperty != JUMPING) || (ExtJumping && u.nonextrinsicproperty != JUMPING)) && !NoJumping)
#define StrongJumping	(IntJumping && ExtJumping && Jumping && u.nondoubleproperty != JUMPING)

#define NoJumping	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_JUMPING].intrinsic || UHaveAids || (u.impossibleproperty == JUMPING) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HTeleportation		u.uprops[TELEPORT].intrinsic
#define ETeleportation		u.uprops[TELEPORT].extrinsic
#define IntTeleportation	(HTeleportation || can_teleport(youmonst.data) || Race_if(PM_MAIA) || Race_if(PM_HUMANOID_LEPRECHAUN))
#define ExtTeleportation	(ETeleportation || (uarmh && uarmh->oartifact == ART_GO_OTHER_PLACE) || (uarmg && uarmg->oartifact == ART_OUT_OF_CONTROL) || (uarmh && uarmh->oartifact == ART_DOUBLE_JEOPARDY) || (uarms && uarms->oartifact == ART_REFLECTOR_EJECTOR) || (uwep && uwep->otyp == POKER_STICK) || (uarmf && uarmf->oartifact == ART_ERROR_IN_PLAY_ENCHANTMENT) || autismweaponcheck(ART_SACRIFICE_TONFA) || (uarm && uarm->oartifact == ART_LYNN_S_ELUSION) || autismweaponcheck(ART_MYSTERIOUS_FORCE) || (uarm && uarm->oartifact == ART_DUEDUEUEUEUEUE) || (uimplant && uimplant->oartifact == ART_ARABELLA_S_SEXY_CHARM) || autismringcheck(ART_LOMYA) || (uarmc && uarmc->oartifact == ART_KLALTER) || autismweaponcheck(ART_POGO_STICK) || autismweaponcheck(ART_COVETOR_SABER) || autismweaponcheck(ART_ACTUAL_BEAMER) || (uarmu && uarmu->oartifact == ART_TELEPORTITS) || autismringcheck(ART_HENRIETTA_S_MAGICAL_AID) || autismringcheck(ART_BELKAR_S_WARPOPORTATION) || (uarmh && uarmh->oartifact == ART_CLELIA_S_TONGUE_BREAKER) || (uarm && uarm->oartifact == ART_VENGEANCE_OF_ETERNITY) || (uimplant && uimplant->oartifact == ART_POTATOROK) || (uarmu && uarmu->oartifact == ART_LEPRE_LUCK) || (uarmc && uarmc->oartifact == ART_PORTER_S_THINK) || autismweaponcheck(ART_NEX_XUS) || (uarmc && uarmc->oartifact == ART_SECANT_WHERELOCATION) || (uarmh && uarmh->oartifact == ART_RANDOMNESS_PREVAILS) || autismringcheck(ART_GOLDENIVY_S_RAGE) || autismringcheck(ART_GOLDENIVY_S_ENGAGEMENT_RIN) || autismringcheck(ART_WARPCHANGE) || (uamul && uamul->oartifact == ART_SPACE_CYCLE))

#define Teleportation		(IntTeleportation || ExtTeleportation)
#define StrongTeleportation	(IntTeleportation && ExtTeleportation && Teleportation)

#define HTeleport_control	u.uprops[TELEPORT_CONTROL].intrinsic
#define ETeleport_control	u.uprops[TELEPORT_CONTROL].extrinsic
#define IntTeleport_control	(HTeleport_control || (uleft && objects[uleft->otyp].oc_material == MT_MENGETIUM) || control_teleport(youmonst.data))
#define ExtTeleport_control	(ETeleport_control || (powerfulimplants() && uimplant && uimplant->oartifact == ART_ARABELLA_S_SEXY_CHARM) || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == TELEPORT_CONTROL) ) )

#define Teleport_control	(((IntTeleport_control && u.nonintrinsicproperty != TELEPORT_CONTROL) || (ExtTeleport_control && u.nonextrinsicproperty != TELEPORT_CONTROL)) && !Race_if(PM_MAIA) && !(u.uprops[STORM_HELM].extrinsic) && !Race_if(PM_HUMANOID_LEPRECHAUN) && !NoTeleport_control)
#define StrongTeleport_control	(IntTeleport_control && ExtTeleport_control && Teleport_control && u.nondoubleproperty != TELEPORT_CONTROL && !(uarmc && uarmc->oartifact == ART_KLALTER) )

#define NoTeleport_control	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_TELEPORT_CONTROL].intrinsic || UHaveAids || (uarmc && uarmc->oartifact == ART_A_REASON_TO_LIVE) || In_mainframe(&u.uz) || In_bellcaves(&u.uz) || In_forging(&u.uz) || In_ordered(&u.uz) || In_deadground(&u.uz) || (uarm && uarm->oartifact == ART_LYNN_S_ELUSION) || (uimplant && uimplant->oartifact == ART_POTATOROK && !(powerfulimplants()) ) || autismringcheck(ART_HENRIETTA_S_MAGICAL_AID) || (uarmh && uarmh->oartifact == ART_RANDOMNESS_PREVAILS) || (u.impossibleproperty == TELEPORT_CONTROL) || (uarmg && uarmg->oartifact == ART_OUT_OF_CONTROL) || (uwep && uwep->otyp == POKER_STICK) || (uarmg && uarmg->oartifact == ART_ARABELLA_S_GREAT_BANISHER) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HLevitation		u.uprops[LEVITATION].intrinsic
#define ELevitation		u.uprops[LEVITATION].extrinsic
#define IntLevitation	(HLevitation || Race_if(PM_LEVITATOR) || (is_floater(youmonst.data) && !Race_if(PM_TRANSFORMER)) )
#define ExtLevitation	(ELevitation || (uarmf && uarmf->oartifact == ART_FERGIE_S_GLACIER && uwep && (weapon_type(uwep) == P_QUARTERSTAFF) ) || (uarmf && uarmf->oartifact == ART_ULTRA_P) || autismringcheck(ART_FLOAT_EYELER_S_CONDITION) || autismweaponcheck(ART_IRON_BALL_OF_LEVITATION) || autismweaponcheck(ART_XIUHCOATL) || u.uprops[MULTISHOES].extrinsic)

#define Levitation		((IntLevitation || ExtLevitation) && !(uarmf && uarmf->oartifact == ART_EFFECT_THAT_IS_DEACTIVATED))
	/* Can't touch surface, can't go under water; overrides all others */
#define Lev_at_will		(!Race_if(PM_LEVITATOR) && ((HLevitation & I_SPECIAL) != 0L || \
				 (ELevitation & W_ARTI) != 0L) && \
				 (HLevitation & ~(I_SPECIAL|TIMEOUT)) == 0L && \
				 (ELevitation & ~W_ARTI) == 0L && \
				 (!is_floater(youmonst.data) || Race_if(PM_TRANSFORMER) ) )
#define StrongLevitation	(IntLevitation && ExtLevitation && Levitation)

#define HFlying			u.uprops[FLYING].intrinsic
#define EFlying			u.uprops[FLYING].extrinsic
#define IntFlying	(HFlying || (Race_if(PM_DUTHOL) && u.usteed) || (Race_if(PM_MOON_ELF) && flags.moonphase == FULL_MOON) || is_flyer(youmonst.data) || (is_floater(youmonst.data) && Race_if(PM_TRANSFORMER) ) || (u.usteed && is_flyer(u.usteed->data)) )
#define ExtFlying		(playerextrinsicflying())

#define Flying			(((IntFlying && u.nonintrinsicproperty != FLYING) || (ExtFlying && u.nonextrinsicproperty != FLYING)) && !(uarm && uarm->oartifact == ART_WATER_SHYNESS) && !NoFlying && !(Role_if(PM_TOPMODEL) && flags.female && !PlayerInHighHeels) )
#define StrongFlying	(IntFlying && ExtFlying && Flying && u.nondoubleproperty != FLYING)

#define NoFlying	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_FLYING].intrinsic || (Race_if(PM_NEMESIS) && uarmc) || UHaveAids || (uarmc && uarmc->oartifact == ART_BROKEN_WINGS) || (uarmf && itemhasappearance(uarmf, APP_WEIGHT_ATTACHMENT_BOOTS)) || (uarms && uarms->oartifact == ART_GROUNDED_FOREVER) || (uarmc && uarmc->oartifact == ART_GROUNDBUMMER) || (FemtrapActiveNaomi && !PlayerInHighHeels) || (u.impossibleproperty == FLYING) || (uarm && uarm->oartifact == ART_ARMOR_OF_EREBOR) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

	/* May touch surface; does not override any others */

#define Wwalking		( (u.uprops[WWALKING].extrinsic || (ublindf && ublindf->oartifact == ART_FLOTATION_DEVICE) || (uarmf && uarmf->oartifact == ART_SANDALS_OF_JESUS_CHRIST) || (uwep && uwep->oartifact == ART_POSEIDON_S_OTHER_TRIDENT) || (uwep && uwep->oartifact == ART_SEAFOAM) || (uwep && uwep->oartifact == ART_LOGGERS_S_OVERKILL) || (uwep && uwep->oartifact == ART_MAKESHIFT_BRIDGE) || (uwep && uwep->oartifact == ART_POLAR_STAR) || (uwep && uwep->oartifact == ART_POLARIS) || u.uprops[MULTISHOES].extrinsic) && \
				 !Is_waterlevel(&u.uz))
	/* Don't get wet, can't go under water; overrides others except levitation */
	/* Wwalking is meaningless on water level */

#define HSwimming		u.uprops[SWIMMING].intrinsic
#define ESwimming		u.uprops[SWIMMING].extrinsic	/* [Tom] */
#define IntSwimming	(HSwimming || tech_inuse(T_SILENT_OCEAN) || is_swimmer(youmonst.data) || (u.usteed && is_swimmer(u.usteed->data)) )
#define ExtSwimming	(ESwimming || (uwep && uwep->oartifact == ART_QUICKER_RHEOLOGY) || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == SWIMMING) ) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_SIGNIFICANT_RNG_JITTER) || (uarmc && itemhasappearance(uarmc, APP_WETSUIT)) || (uarmf && uarmf->oartifact == ART_SNAILHUNT) || (uwep && uwep->oartifact == ART_ETRUSCIAN_SWIMMING_LESSON) || (uarmf && uarmf->oartifact == ART_FAR_EAST_RELATION) || (uarms && uarms->oartifact == ART_PLANK_OF_CARNEADES) || (uwep && uwep->oartifact == ART_SKIN_DEGREE) || (uarmf && uarmf->oartifact == ART_PECTORAL_HEEL) || (uwep && uwep->oartifact == ART_GREAT_ANTILLES) || (uwep && uwep->oartifact == ART_TEZCATLIPOCA_S_BUBBLESTORM) || (uarmc && uarmc->oartifact == ART_MANTLE_OF_THE_MANTA_RAY) || (uarmf && uarmf->oartifact == ART_STEERBOAT) || (uarmf && uarmf->oartifact == ART_WELCOME_ON_BOARD) || (uarm && uarm->oartifact == ART_GO_UNDER_OR_UP) || autismringcheck(ART_CROQUE_FORD) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_NEWFOUND_AND_USEFUL) || (uarmc && uarmc->oartifact == ART_WATERFORCE____) || (uarmc && uarmc->oartifact == ART_LAURA_S_SWIMSUIT) || (uwep && uwep->oartifact == ART_PLLTSCH) || (uwep && uwep->oartifact == ART_FOAMONIA_WATER) || (uwep && uwep->oartifact == ART_TRIDENT_OF_POSEIDON) || (uarmc && uarmc->oartifact == ART_WATERS_OF_OBLIVION) || (uarmf && uarmf->oartifact == ART_SANDRA_S_BEAUTIFUL_FOOTWEA) || (uarmu && uarmu->oartifact == ART_THERMAL_BATH) )

# define Swimming		((IntSwimming || ExtSwimming) && !(uarmf && itemhasappearance(uarmf, APP_FLIPFLOPS) ) && !(uarm && uarm->oartifact == ART_WATER_SHYNESS) )
	/* Get wet, don't go under water unless if amphibious */
#define StrongSwimming	(IntSwimming && ExtSwimming && Swimming)

/* amphibious is special-cased because breathless != amphibious --Amy */
#define HMagical_breathing	u.uprops[MAGICAL_BREATHING].intrinsic
#define EMagical_breathing	u.uprops[MAGICAL_BREATHING].extrinsic
#define IntMagical_breathing	(HMagical_breathing || (uamul && objects[uamul->otyp].oc_material == MT_MENGETIUM) || (uarmh && uarmh->oartifact == ART_HABIBA_S_MATRONAGE) || tech_inuse(T_SILENT_OCEAN) || (Race_if(PM_MOON_ELF) && flags.moonphase == NEW_MOON) || amphibious(youmonst.data) || breathless(youmonst.data) )
#define ExtMagical_breathing	(EMagical_breathing || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == MAGICAL_BREATHING) ) || (uarmh && uarmh->oartifact == ART_SHPX_GUVF_FUVG) || (Role_if(PM_ANACHRONIST) && uwep && uwep->oartifact == ART_ANACHRONONONONAUT_PACKAGE) || (uwep && uwep->oartifact == ART_POSEIDON_S_OTHER_TRIDENT) || (uwep && uwep->oartifact == ART_SEAFOAM) || (uarm && uarm->oartifact == ART_INCREDIBLY_SEXY_SQUEAKING) || (uarmf && uarmf->oartifact == ART_HERMES__UNFAIRNESS) || (uwep && uwep->oartifact == ART_SKIN_DEGREE) || (uwep && uwep->oartifact == ART_MUSICAL_SNORKEL) || (uarmf && uarmf->oartifact == ART_FAR_EAST_RELATION) || (uwep && uwep->otyp == UNDERWATER_TRIDENT) || (uwep && uwep->oartifact == ART_GARY_S_RIVALRY) || (uarmf && uarmf->oartifact == ART_HADES_THE_MEANIE) || (uarm && uarm->oartifact == ART_ACTUAL_GRADIATING_WORK) || (uarmc && itemhasappearance(uarmc, APP_AIR_CLOAK)) || (uarmh && uarmh->oartifact == ART_WAR_HELM_OF_THE_DREAMING) || (uarmf && uarmf->oartifact == ART_MARJI_JANA) || (uwep && uwep->oartifact == ART_HELL_OR_HIGH_WATER) || (uwep && uwep->oartifact == ART_LOGGERS_S_OVERKILL) || (uarmh && uarmh->oartifact == ART_HABIBA_S_MATRONAGE) || (uwep && uwep->oartifact == ART_ON_CLOUD_SEVEN) || autismringcheck(ART_VILYA) || (uwep && uwep->oartifact == ART_HK_CLEAR) || (uarmc && uarmc->oartifact == ART_WATERFORCE____) || (uarm && uarm->oartifact == ART_OUTMINE_OF_METAL) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_JANA_S_MAKE_UP_PUTTY) || (uwep && uwep->oartifact == ART_FOAMONIA_WATER) || (uarmf && uarmf->oartifact == ART_CLONE_) || (uamul && uamul->oartifact == ART_MAGICAL_SPELLING) || (uamul && uamul->oartifact == ART_AMULET_OF_SPLENDOR) || autismringcheck(ART_JANA_S_DIMINISHER) || (uarmc && uarmc->oartifact == ART_SATAN_S_SUGGESTION) || (uarmc && uarmc->oartifact == ART_MANTLE_OF_THE_MANTA_RAY) || (uwep && uwep->oartifact == ART_TEZCATLIPOCA_S_BUBBLESTORM) || (uarmc && uarmc->oartifact == ART_JANA_S_SECRET_CAR) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_NEWFOUND_AND_USEFUL) || (uarmc && uarmc->oartifact == ART_JANA_S_GRAVE_WALL) || (uarmf && uarmf->oartifact == ART_UNDERWATER_LOVE) || (uarm && uarm->oartifact == ART_ROCKET_IMPULSE) || (uwep && uwep->oartifact == ART_TRIDENT_OF_POSEIDON) || (uwep && uwep->oartifact == ART_HIGH_ORIENTAL_PRAISE) || (uwep && uwep->oartifact == ART_PLLTSCH) || (uarmh && uarmh->oartifact == ART_VACUUM_CLEANER_DEATH) || (uarmc && uarmc->oartifact == ART_LAURA_S_SWIMSUIT) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_SIGNIFICANT_RNG_JITTER) || (uarmf && uarmf->oartifact == ART_JANA_S_VAGINAL_FUN) || (uarmf && uarmf->oartifact == ART_SHORTFALL) || (uarmf && uarmf->oartifact == ART_SANDRA_S_BEAUTIFUL_FOOTWEA) || (uarmf && uarmf->oartifact == ART_FUN_ALL_IN_ONE) || (uarmf && uarmf->oartifact == ART_ALLYNONE) || (Role_if(PM_TOPMODEL) && PlayerInHighHeels) )

/* like IntMagical_breathing but without the breathless case */
#define AmphibiousMagical_breathing	((HMagical_breathing && u.nonintrinsicproperty != MAGICAL_BREATHING) || tech_inuse(T_SILENT_OCEAN) || (Race_if(PM_MOON_ELF) && flags.moonphase == NEW_MOON) || amphibious(youmonst.data) )

#define Amphibious		(AmphibiousMagical_breathing && u.nonintrinsicproperty != MAGICAL_BREATHING && !NoBreathless)
	/* Get wet, may go under surface */

#define Breathless		(((IntMagical_breathing && u.nonintrinsicproperty != MAGICAL_BREATHING) || (ExtMagical_breathing && u.nonextrinsicproperty != MAGICAL_BREATHING)) && !Role_if(PM_FJORDE) && !NoBreathless && !(Role_if(PM_TOPMODEL) && flags.female && !PlayerInHighHeels) )
#define StrongMagical_breathing	(IntMagical_breathing && ExtMagical_breathing && (Breathless || Amphibious) && u.nondoubleproperty != MAGICAL_BREATHING)

#define NoBreathless	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_MAGICAL_BREATHING].intrinsic || UHaveAids || (u.impossibleproperty == MAGICAL_BREATHING) || (FemtrapActiveNaomi && !PlayerInHighHeels) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define Underwater		(u.uinwater)
/* Note that Underwater and u.uinwater are both used in code.
   The latter form is for later implementation of other in-water
   states, like swimming, wading, etc. */

#define HPasses_walls		u.uprops[PASSES_WALLS].intrinsic
#define EPasses_walls		u.uprops[PASSES_WALLS].extrinsic
#define IntPasses_walls	(HPasses_walls || passes_walls(youmonst.data))
#define ExtPasses_walls	(EPasses_walls || (uimplant && uimplant->oartifact == ART_IRON_OF_INNERMOST_JOY) || (uwep && uwep->oartifact == ART_HOL_ON_MAN) || (uarmf && uarmf->oartifact == ART_PHANTO_S_RETARDEDNESS) || (uarmf && uarmf->oartifact == ART_SPIRIT_ROCKZ) || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == PASSES_WALLS) ) )

#define Passes_walls		(((IntPasses_walls && u.nonintrinsicproperty != PASSES_WALLS) || (ExtPasses_walls && u.nonextrinsicproperty != PASSES_WALLS)) && !NoPasses_walls)
#define Phasing            u.uprops[PASSES_WALLS].intrinsic
#define StrongPasses_walls	(IntPasses_walls && ExtPasses_walls && Passes_walls && u.nondoubleproperty != PASSES_WALLS)

#define NoPasses_walls	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_PASSES_WALLS].intrinsic || UHaveAids || (u.impossibleproperty == PASSES_WALLS) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))


/*** Physical attributes ***/
#define HSlow_digestion		u.uprops[SLOW_DIGESTION].intrinsic
#define ESlow_digestion		u.uprops[SLOW_DIGESTION].extrinsic
#define IntSlow_digestion	(HSlow_digestion)
#define ExtSlow_digestion	(ESlow_digestion || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == SLOW_DIGESTION) ) || (uwep && uwep->oartifact == ART_BREAD_FOR_THE_WORLD) || (uwep && uwep->oartifact == ART_HOLY_GRAIL) || (uarmh && uarmh->oartifact == ART_NEVEREATER) || autismringcheck(ART_SCHATZKI_S_RING) || (uarm && uarm->oartifact == ART_FULL_DOSE_LIFE) || (uwep && uwep->oartifact == ART_HEAVY_CROSSBOW_OF_ETERNITY) || (uwep && uwep->oartifact == ART_LONG_SWORD_OF_ETERNITY) || (uarm && uarm->oartifact == ART_BELLY_W) || (uarms && uarms->oartifact == ART_OMG_UBER_IMBA___) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_ENTEROGASTER_ALTO) || (uimplant && uimplant->oartifact == ART_BUCKET_HOUSE) || (uarmc && uarmc->oartifact == ART_FEMMY_FATALE) || (uarmh && uarmh->oartifact == ART_WOLF_KING) )

#define Slow_digestion		(((IntSlow_digestion && u.nonintrinsicproperty != SLOW_DIGESTION) || (ExtSlow_digestion && u.nonextrinsicproperty != SLOW_DIGESTION)) && !Race_if(PM_PERVERT) && !Race_if(PM_GIGANT) && !NoSlow_digestion)  /* KMH */
#define StrongSlow_digestion	(IntSlow_digestion && ExtSlow_digestion && Slow_digestion && u.nondoubleproperty != SLOW_DIGESTION)

#define NoSlow_digestion	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SLOW_DIGESTION].intrinsic || UHaveAids || (u.impossibleproperty == SLOW_DIGESTION) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

/* Half spell/physical damage only works 50% of the time, so it's more like three quarter spell/physical damage. --Amy */

#define HHalf_spell_damage	u.uprops[HALF_SPDAM].intrinsic
#define EHalf_spell_damage	u.uprops[HALF_SPDAM].extrinsic
#define IntHalf_spell_damage	(HHalf_spell_damage || (powerfulimplants() && uimplant && uimplant->oartifact == ART_CLEAN_ASCENSION_RUN && In_endgame(&u.uz)) || (uarms && uarms->oartifact == ART_ALTERNATE_SPELLWEAVE) || (uarmh && uarmh->oartifact == ART_TEJUS__VACANCY) || (Stoned_chiller && Stoned && !(u.stonedchilltimer)) || (Race_if(PM_BOVER) && u.usteed))
#define ExtHalf_spell_damage	(EHalf_spell_damage || (powerfulimplants() && uimplant && uimplant->oartifact == ART_WARY_PROTECTORATE) || (uwep && uwep->oartifact == ART_KATI_GAVE_YOU_THE_ENGLISH_ && !Upolyd && (u.uhp < (u.uhpmax / 4)) ) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_IRON_OF_INNERMOST_JOY) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_POTATOROK) || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == HALF_SPDAM) ) )

#define Half_spell_damage	(((IntHalf_spell_damage && u.nonintrinsicproperty != HALF_SPDAM) || (ExtHalf_spell_damage && u.nonextrinsicproperty != HALF_SPDAM)) && !Race_if(PM_KUTAR) && !NoHalf_spell_damage)
#define StrongHalf_spell_damage	(IntHalf_spell_damage && ExtHalf_spell_damage && Half_spell_damage && u.nondoubleproperty != HALF_SPDAM)

#define NoHalf_spell_damage	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_HALF_SPDAM].intrinsic || UHaveAids || (u.impossibleproperty == HALF_SPDAM) || autismweaponcheck(ART_ATLUS_HEAVE) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HHalf_physical_damage	u.uprops[HALF_PHDAM].intrinsic
#define EHalf_physical_damage	u.uprops[HALF_PHDAM].extrinsic
#define IntHalf_physical_damage	(HHalf_physical_damage || (powerfulimplants() && uimplant && uimplant->oartifact == ART_CLEAN_ASCENSION_RUN && In_endgame(&u.uz)) || (uarm && uarm->oartifact == ART_SWORDHILTSTOP) || (uarms && uarms->oartifact == ART_PROTECTION_SET) || (Stoned_chiller && Stoned && !(u.stonedchilltimer)) || (uarmf && uarmf->oartifact == ART_SO_WONDERFULLY_FLUFFY_SOFT) || (uarmh && uarmh->oartifact == ART_HABIBA_S_MATRONAGE) || (Race_if(PM_BOVER) && u.usteed))
#define ExtHalf_physical_damage	(EHalf_physical_damage || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == HALF_PHDAM) ) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_SHIELD_TONFA) || (bmwride(ART_PANZER_TANK)) || (uwep && uwep->otyp == SECRET_WHIP) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_IRON_OF_INNERMOST_JOY) || (uarmf && uarmf->otyp == FLEECLE_SHOES && PlayerInSexyFlats) || (uwep && uwep->oartifact == ART_KATI_GAVE_YOU_THE_ENGLISH_ && !Upolyd && (u.uhp < (u.uhpmax / 4)) ) || (uwep && uwep->otyp == ETERNAL_POLE) || (!PlayerInSexyFlats && !PlayerInHighHeels && uarmf && uarmf->oartifact == ART_FORMO____) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_RNG_S_EXTRAVAGANZA) || (uarmf && uarmf->oartifact == ART_HUGGING__GROPING_AND_STROK && u.ustuck) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_POTATOROK) || (uarmf && itemhasappearance(uarmf, APP_LEATHER_FLEECE_BOOTS) && (multi < 0)) || (uarmh && itemhasappearance(uarmh, APP_CLOUDY_HELMET) && isok(u.ux, u.uy) && IS_CLOUD(levl[u.ux][u.uy].typ) ) || (uarmc && (moves % 10 == 0) && itemhasappearance(uarmc, APP_SOFT_CLOAK) ) || (ublindf && ublindf->otyp == SOFT_CHASTITY_BELT) )

#define Half_physical_damage	(((IntHalf_physical_damage && u.nonintrinsicproperty != HALF_PHDAM) || (ExtHalf_physical_damage && u.nonextrinsicproperty != HALF_PHDAM)) && !NoHalf_physical_damage)
#define StrongHalf_physical_damage	(IntHalf_physical_damage && ExtHalf_physical_damage && Half_physical_damage && u.nondoubleproperty != HALF_PHDAM)

#define NoHalf_physical_damage	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_HALF_PHDAM].intrinsic || UHaveAids || (uarmf && uarmf->oartifact == ART_WILD_SEX_GAME) || autismweaponcheck(ART_ATLUS_HEAVE) || (u.impossibleproperty == HALF_PHDAM) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HAstral_vision	u.uprops[ASTRAL_VISION].intrinsic
#define EAstral_vision	u.uprops[ASTRAL_VISION].extrinsic
#define IntAstral_vision	(HAstral_vision || (irissetbonus() >= 4) || Race_if(PM_ETHEREALOID) || Race_if(PM_INCORPOREALOID))
#define ExtAstral_vision	(EAstral_vision || (irissetbonus() >= 4) || (uchain && uchain->oartifact == ART_SIYID) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_IRON_OF_INNERMOST_JOY) || (uwep && uwep->oartifact == ART_ASTRALTOR_TSCHH && uwep->lamplit) || (uwep && uwep->oartifact == ART_ASTRAL_LIGHTWELL && uwep->lamplit) || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == ASTRAL_VISION) ) )

#define Astral_vision	(((IntAstral_vision && u.nonintrinsicproperty != ASTRAL_VISION) || (ExtAstral_vision && u.nonextrinsicproperty != ASTRAL_VISION)) && !NoAstral_vision)
#define StrongAstral_vision	(IntAstral_vision && ExtAstral_vision && Astral_vision && u.nondoubleproperty != ASTRAL_VISION)

#define NoAstral_vision	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_ASTRAL_VISION].intrinsic || UHaveAids || (u.impossibleproperty == ASTRAL_VISION) || (!flags.female && uarmh && uarmh->oartifact == ART_HAT_OF_LADY) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define Second_chance		u.uprops[SECOND_CHANCE].extrinsic

#define HRegeneration		u.uprops[REGENERATION].intrinsic
#define ERegeneration		u.uprops[REGENERATION].extrinsic
#define IntRegeneration	(HRegeneration || (uarmg && uarmg->oartifact == ART_GAUNTLETS_OF_THE_HEALING_H) || autismringcheck(ART_RING_OF_HYGIENE_S_DISCIPLE) || (uwep && uwep->oartifact == ART_MAEVE_S_OPULENCE) || (uarmh && uarmh->oartifact == ART_HABIBA_S_MATRONAGE) || regenerates(youmonst.data))
#define ExtRegeneration	(ERegeneration || (uimplant && uimplant->oartifact == ART_SIGNIFICANT_RNG_JITTER) || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == REGENERATION) ) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_THEY_RE_ALL_YELLOW) || (uwep && uwep->oartifact == ART_BREAD_FOR_THE_WORLD) || (uarmf && uarmf->oartifact == ART_ERROR_IN_PLAY_ENCHANTMENT) )

#define Regeneration		(((IntRegeneration && u.nonintrinsicproperty != REGENERATION) || (ExtRegeneration && u.nonextrinsicproperty != REGENERATION)) && !NoRegeneration && !Race_if(PM_SYLPH) )
#define StrongRegeneration	(IntRegeneration && ExtRegeneration && Regeneration && u.nondoubleproperty != REGENERATION)

#define NoRegeneration	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_REGENERATION].intrinsic || UHaveAids || (u.impossibleproperty == REGENERATION) || (uleft && uleft->otyp == RIN_REVERSE_REGENERATION) || (uright && uright->otyp == RIN_REVERSE_REGENERATION)|| (uimplant && uimplant->oartifact == ART_RESTROOM_DRENCHING) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HEnergy_regeneration	u.uprops[ENERGY_REGENERATION].intrinsic
#define EEnergy_regeneration	u.uprops[ENERGY_REGENERATION].extrinsic
#define IntEnergy_regeneration	(HEnergy_regeneration || (uamul && uamul->oartifact == ART_HUUUUUGE_POWER) || (Race_if(PM_REDGUARD)) )
#define ExtEnergy_regeneration	(EEnergy_regeneration || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == ENERGY_REGENERATION) ) )

#define Energy_regeneration	(((IntEnergy_regeneration && u.nonintrinsicproperty != ENERGY_REGENERATION) || (ExtEnergy_regeneration && u.nonextrinsicproperty != ENERGY_REGENERATION)) && !NoEnergy_regeneration && !Race_if(PM_SYLPH) )
#define StrongEnergy_regeneration	(IntEnergy_regeneration && ExtEnergy_regeneration && Energy_regeneration && u.nondoubleproperty != ENERGY_REGENERATION)

#define NoEnergy_regeneration	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_ENERGY_REGENERATION].intrinsic || UHaveAids || (u.impossibleproperty == ENERGY_REGENERATION) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HProtection		u.uprops[PROTECTION].intrinsic
#define EProtection		u.uprops[PROTECTION].extrinsic
#define Protection		(HProtection || EProtection)

#define HProtection_from_shape_changers \
				u.uprops[PROT_FROM_SHAPE_CHANGERS].intrinsic
#define EProtection_from_shape_changers \
				u.uprops[PROT_FROM_SHAPE_CHANGERS].extrinsic
#define Protection_from_shape_changers \
				(HProtection_from_shape_changers || \
				 EProtection_from_shape_changers || (uamul && uamul->oartifact == ART_VARIANT_GUARD) )

#define HPolymorph		u.uprops[POLYMORPH].intrinsic
#define EPolymorph		u.uprops[POLYMORPH].extrinsic
#define IntPolymorph	(HPolymorph || Race_if(PM_MOULD) || Race_if(PM_PLAYER_MIMIC) || Race_if(PM_TRANSFORMER) || Race_if(PM_WORM_THAT_WALKS) || Race_if(PM_MISSINGNO) || (Race_if(PM_PLAYER_CHANGELING) && moves >= 1000) || Race_if(PM_WARPER) || Race_if(PM_UNGENOMOLD) || Race_if(PM_DEATHMOLD))
#define ExtPolymorph	(EPolymorph || ((moves % 10 == 0) && uarmc && itemhasappearance(uarmc, APP_CHANGING_CLOAK) ) || (uimplant && uimplant->oartifact == ART_SIGNIFICANT_RNG_JITTER) || (uamul && uamul->oartifact == ART_AMULET_OF_SPLENDOR) || autismringcheck(ART_BELKAR_S_WARPOPORTATION) || (uarmf && uarmf->oartifact == ART_CLONE_) || (uarmf && uarmf->oartifact == ART_MALENA_S_LADYNESS) || (uarmc && uarmc->oartifact == ART_NOW_YOUR_ABLE_TO_POLY) || (uarmf && uarmf->oartifact == ART_NOSE_ENCHANTMENT) || (uarmh && uarmh->oartifact == ART_RANDOMNESS_PREVAILS) || (uarmh && uarmh->oartifact == ART_DOUBLE_JEOPARDY) || (uarmf && uarmf->oartifact == ART_ERROR_IN_PLAY_ENCHANTMENT) || (uarmc && uarmc->oartifact == ART_SHAPETAKE_NUMBER_FIVE) || autismringcheck(ART_SEMI_SHAPE_CONTROL) || (uamul && uamul->oartifact == ART_SPACE_CYCLE) )

#define Polymorph		(IntPolymorph || ExtPolymorph)
#define StrongPolymorph	(IntPolymorph && ExtPolymorph && Polymorph)

#define HPolymorph_control	u.uprops[POLYMORPH_CONTROL].intrinsic
#define EPolymorph_control	u.uprops[POLYMORPH_CONTROL].extrinsic
#define IntPolymorph_control	(HPolymorph_control || (uright && objects[uright->otyp].oc_material == MT_MENGETIUM) )
#define ExtPolymorph_control	(EPolymorph_control || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == POLYMORPH_CONTROL) ) || (uarmu && uarmu->oartifact == ART_VICTORIA_IS_EVIL_BUT_PRETT) || (uarmu && uarmu->oartifact == ART_NATALIA_IS_LOVELY_BUT_DANG) || (uwep && uwep->oartifact == ART_GRANDLEON) || (uamul && uamul->oartifact == ART_AMULET_OF_SPLENDOR) || autismringcheck(ART_HYPOCRITICAL_FUN) || (uwep && uwep->oartifact == ART_UNICORN_DRILL) || (uarmf && uarmf->oartifact == ART_MALENA_S_LADYNESS) || (uimplant && uimplant->oartifact == ART_YES_YOU_CAN) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_SIGNIFICANT_RNG_JITTER) || (uarmh && uarmh->oartifact == ART_DICTATORSHIP) )

/* Amy edit: races that cannot have poly control ***must*** also have semi control disabled in polyself.c!!! */
#define Polymorph_control	(((IntPolymorph_control && u.nonintrinsicproperty != POLYMORPH_CONTROL) || (ExtPolymorph_control && u.nonextrinsicproperty != POLYMORPH_CONTROL)) && !Race_if(PM_MOULD) && !NoPolymorph_control && !Race_if(PM_TRANSFORMER) && !Race_if(PM_POLYINITOR) && !Race_if(PM_DESTABILIZER) && !Race_if(PM_WORM_THAT_WALKS) && !Race_if(PM_WARPER) && !Race_if(PM_MISSINGNO) && !Race_if(PM_UNGENOMOLD) && !Race_if(PM_DEATHMOLD) && !Race_if(PM_AK_THIEF_IS_DEAD_))
#define StrongPolymorph_control	(IntPolymorph_control && ExtPolymorph_control && Polymorph_control && u.nondoubleproperty != POLYMORPH_CONTROL)

#define NoPolymorph_control	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic || UHaveAids || (uarmc && uarmc->oartifact == ART_PERMANENTITIS) || (uarmf && uarmf->oartifact == ART_CLONE_) || (uarmc && uarmc->oartifact == ART_SHAPETAKE_NUMBER_FIVE) || (u.impossibleproperty == POLYMORPH_CONTROL) || (uarmh && uarmh->oartifact == ART_RANDOMNESS_PREVAILS) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HUnchanging		u.uprops[UNCHANGING].intrinsic
#define EUnchanging		u.uprops[UNCHANGING].extrinsic
#define Unchanging		((HUnchanging || EUnchanging || autismweaponcheck(ART_BRUTISH_CLAYMORE) || (uarm && uarm->oartifact == ART_RADAR_FELL_UP) || (uarmh && uarmh->oartifact == ART_FIVE_HORNED_HELM) || (uarmf && uarmf->oartifact == ART_SHE_S_STILL_AN_ANASTASIA) || autismringcheck(ART_NENYA)) && !(uarmc && uarmc->oartifact == ART_SHAPETAKE_NUMBER_FIVE) && !Race_if(PM_MOULD) && !Race_if(PM_MISSINGNO) && !Race_if(PM_TRANSFORMER) && !Race_if(PM_POLYINITOR) && !Race_if(PM_DESTABILIZER) && !Race_if(PM_WORM_THAT_WALKS) && !Race_if(PM_WARPER) && !Race_if(PM_UNGENOMOLD) && !Race_if(PM_DEATHMOLD) && !Race_if(PM_AK_THIEF_IS_DEAD_))	/* KMH */

#define CannotBeUnchanging	( (uarmc && uarmc->oartifact == ART_SHAPETAKE_NUMBER_FIVE) || Race_if(PM_MOULD) || Race_if(PM_MISSINGNO) || Race_if(PM_TRANSFORMER) || Race_if(PM_POLYINITOR) || Race_if(PM_DESTABILIZER) || Race_if(PM_WORM_THAT_WALKS) || Race_if(PM_WARPER) || Race_if(PM_UNGENOMOLD) || Race_if(PM_DEATHMOLD) || Race_if(PM_AK_THIEF_IS_DEAD_) )

#define HFast			u.uprops[FAST].intrinsic
#define EFast			u.uprops[FAST].extrinsic
#define IntFast	(HFast || (uarmf && objects[uarmf->otyp].oc_material == MT_MENGETIUM) || (uarmc && uarmc->oartifact == ART_LIGHTWEIGHTNESS) || (uarm && uarm->oartifact == ART_SEVEBREAKYOU__SEVEBREAK_) || (uarm && uarm->oartifact == ART_IS_ONLY_OWWE) || (uarmg && uarmg->oartifact == ART_ELARA_S_AGILITY) || (uwep && uwep->oartifact == ART_LIGHT_____STATED_) || (uimplant && uimplant->oartifact == ART_REAL_TIME_SWITCHING) || (uwep && uwep->oartifact == ART_NO_PRISONERS_TAKEN) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_SONIC_TONFA) || (uarmg && itemhasappearance(uarmg, APP_SPEEDY_GLOVES)) || autismringcheck(ART_PALMIA_PRIDE) || autismringcheck(ART_RING_OF_STEEL_DRAGON) || (uwep && uwep->oartifact == ART_PURE_BLACK_DIABLO) || (uwep && uwep->oartifact == ART_GLANGGLANG) || (uwep && uwep->oartifact == ART_HECTIC_OH_YEAH) || (uarmg && uarmg->otyp == GAUNTLETS_OF_CELERITY) || (uwep && uwep->oartifact == ART_DONGDODONGDODONG) || (uarmc && uarmc->oartifact == ART_WHISPERWIND_CLOAK) || (uleft && uleft->otyp == RIN_SPEEDY_TRINKET) || (uright && uright->otyp == RIN_SPEEDY_TRINKET) || (uarmf && uarmf->oartifact == ART_ENERGEEN_S) || (uwep && uwep->oartifact == ART_UNIQUE_WIND_BOW) || (uwep && uwep->oartifact == ART_BOW_SURROUNDED_BY_WIND) || (uarmf && uarmf->oartifact == ART_SEVENLEAGUEBOOTS) || (night() && uarmg && uarmg->oartifact == ART_NIGHTLY_HIGHWAY) || (uarm && uarm->oartifact == ART_BE_FAT_AND_STILL_HEALTHY) || (uwep && uwep->oartifact == ART_EASTERN_CLASH) || (uarmf && uarmf->oartifact == ART_EI_KEN_WOOK_WIF_DIS) || (uarmf && uarmf->oartifact == ART_SPEEEEEED) || (uamul && uamul->oartifact == ART_FASTFUCK) || (uarmf && uarmf->oartifact == ART_PRECURSOR_TO_THE___) || (Race_if(PM_WYLVAN)) || (uarmf && itemhasappearance(uarmf, APP_CHRISTMAS_CHILD_MODE_BOOTS) && Feared) )
#define ExtFast	(playerextrinsicspeed())

#define Fast			(((IntFast && !(Race_if(PM_NEMESIS) && uarmf) && u.nonintrinsicproperty != FAST) || (ExtFast && u.nonextrinsicproperty != FAST)) && !autismweaponcheck(ART_HYPER_INTELLIGENCE) && !NoFast && !Race_if(PM_DEVELOPER) && !autismringcheck(ART_CORGON_S_RING) && !(uarmf && uarmf->oartifact == ART_IMPOSSIBLE_CATWALK) && !Race_if(PM_ITAQUE) && !(Race_if(PM_BOVER) && u.usteed) && !(autismweaponcheck(ART_HEAVY_POLE_SKILL) && P_SKILL(P_POLEARMS) < P_SKILLED) && !Race_if(PM_MONGUNG) && !HardcoreAlienMode && !(uarmf && uarmf->otyp == CATWALK_SANDALS) && (!Role_if(PM_TRANSVESTITE) || flags.female) && (!Role_if(PM_TOPMODEL) || !flags.female) )

#define Very_fast		((ExtFast && u.nonextrinsicproperty != FAST) && !(uwep && uwep->oartifact == ART_DONGDODONGDODONG) && !Race_if(PM_DEVELOPER) && !autismweaponcheck(ART_HYPER_INTELLIGENCE) && !autismringcheck(ART_CORGON_S_RING) && !(uarmf && uarmf->oartifact == ART_IMPOSSIBLE_CATWALK) && !(autismweaponcheck(ART_HEAVY_POLE_SKILL) && P_RESTRICTED(P_POLEARMS)) && !Race_if(PM_ITAQUE) && !(Race_if(PM_BOVER) && u.usteed) && !Race_if(PM_MONGUNG) && !(uarmf && uarmf->otyp == CATWALK_SANDALS) && !HardcoreAlienMode && !(uarmf && uarmf->oartifact == ART_THICK_PLATFORM_CRAZE) && !NoFast && !(Role_if(PM_TRANSVESTITE) && !flags.female && !PlayerInHighHeels) && !(Role_if(PM_TOPMODEL) && flags.female && !PlayerInHighHeels) )
#define StrongFast	(IntFast && ExtFast && Fast && u.nondoubleproperty != FAST)

#define NoFast	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_FAST].intrinsic || (uarmf && uarmf->oartifact == ART_END_OF_LEWDNESS) || UHaveAids || (FemtrapActiveNaomi && !PlayerInHighHeels) || (u.impossibleproperty == FAST) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HReflecting		u.uprops[REFLECTING].intrinsic
#define EReflecting		u.uprops[REFLECTING].extrinsic
#define IntReflecting	(HReflecting || (uarm && objects[uarm->otyp].oc_material == MT_MENGETIUM) || tech_inuse(T_POWERFUL_AURA) || (uarms && uarms->oartifact == ART_SIGNUM_ARGENTUM_POLISHIUM) || (uarmc && uarmc->oartifact == ART_INVSTATE) || (irissetbonus() >= 2) || (is_reflector(youmonst.data)) )
#define ExtReflecting	(EReflecting || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == REFLECTING) ) || (moves % 2 == 0 && (uarm && itemhasappearance(uarm, APP_OCTARINE_ROBE)) ) || (moves % 3 == 0 && uarmf && itemhasappearance(uarmf, APP_REFLECTIVE_SLIPPERS)) || (moves % 10 == 0 && uwep && uwep->oartifact == ART_CRABBOMAT) || (uarmu && uarmu->oartifact == ART_LEGENDARY_SHIRT) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_SLEX_WANTS_YOU_TO_DIE_A_PA) || (uarm && uarm->oartifact == ART_VOLUME_ARMAMENT) || (uarm && uarm->oartifact == ART_DON_SUICUNE_DOES_NOT_APPRO) || (uwep && uwep->otyp == BEAM_REFLECTOR_GUN) || (ublindf && ublindf->otyp == DRAGON_EYEPATCH) || (!flags.female && uarmf && uarmf->oartifact == ART_CATALIN_S_ROBBERY) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_SACRIFICE_TONFA) || (uarms && uarms->otyp == INVERSION_SHIELD) || (uarmc && uarmc->otyp == CLOAK_OF_INVERSION) || (uarmc && uarmc->oartifact == ART_A_REASON_TO_LIVE) || (uarmc && uarmc->oartifact == ART_ALL_IN_ONE_ASCENSION_KIT) || (uwep && uwep->oartifact == ART_LETS_MAKE_IT_OFFICIAL && u.kliuskill >= 2500) || (irissetbonus() >= 2) || (uamul && uamul->oartifact == ART_AMULET_OF_SPLENDOR) || (uwep && uwep->oartifact == ART_DIS_IS_E_PRISEM && uwep->lamplit) || (uarmc && uarmc->oartifact == ART_RNG_S_GAMBLE) || (uchain && uchain->oartifact == ART_TSCHEND_FOR_ETERNITY) || (uarmh && uarmh->oartifact == ART_YOU_ARE_ALREADY_DEAD) || (uarmg && uarmg->oartifact == ART_PLUG_AND_PRAY) || (uarmg && uarmg->oartifact == ART_IRIS_S_PRECIOUS_METAL) || (uarmg && uarmg->oartifact == ART_SEALED_KNOWLEDGE) || (uarmg && uarmg->oartifact == ART_UNOBTAINABLE_BEAUTIES) )

#define Reflecting		(((IntReflecting && u.nonintrinsicproperty != REFLECTING) || (ExtReflecting && u.nonextrinsicproperty != REFLECTING)) && !NoReflecting && !(uarmc && itemhasappearance(uarmc, APP_ANGBAND_CLOAK)) && !Race_if(PM_ANGBANDER) && !RngeAngband )
#define StrongReflecting	(IntReflecting && ExtReflecting && Reflecting && u.nondoubleproperty != REFLECTING)

#define NoReflecting	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_REFLECTING].intrinsic || UHaveAids || (uarm && uarm->oartifact == ART_ARMOR_OF_EREBOR && (moves % 4 == 0) ) || (u.impossibleproperty == REFLECTING) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HFree_action	u.uprops[FREE_ACTION].intrinsic
#define EFree_action	u.uprops[FREE_ACTION].extrinsic
#define IntFree_action	(HFree_action || (tech_inuse(T_SEXY_STAND) && PlayerInBlockHeels) || (uwep && uwep->oartifact == ART_YES_OCCIFER) || FemtrapActiveNatalje || (uarmg && uarmg->oartifact == ART_SIRINE_S_MELLOW_LOOK) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_CLEAN_ASCENSION_RUN && In_endgame(&u.uz)) )
#define ExtFree_action	(playerextrinsicfreeaction())

#define Free_action		(((IntFree_action && u.nonintrinsicproperty != FREE_ACTION) || (ExtFree_action && u.nonextrinsicproperty != FREE_ACTION)) && !NoFree_action) /* [Tom] */
#define StrongFree_action	(IntFree_action && ExtFree_action && Free_action && u.nondoubleproperty != FREE_ACTION)

#define NoFree_action	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_FREE_ACTION].intrinsic || UHaveAids || (u.impossibleproperty == FREE_ACTION) || autismweaponcheck(ART_HYPER_INTELLIGENCE) || (uarm && uarm->oartifact == ART_ELMHERE) || (uarmf && uarmf->otyp == PLASTEEL_BOOTS && (!Role_if(PM_BINDER) || uarmf->oartifact != ART_BINDER_CRASH) ) || (uimplant && uimplant->oartifact == ART_KATRIN_S_SUDDEN_APPEARANCE) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

/* Sustain ability */
#define Fixed_abil		u.uprops[FIXED_ABIL].extrinsic	/* KMH */
#define SustainAbilityOn	(Fixed_abil || Race_if(PM_SUSTAINER) || (uarmh && uarmh->oartifact == ART_SLIPPING_SUCKING) || (uarms && uarms->oartifact == ART_SYSTEMATIC_CHAOS) || (uarmc && uarmc->oartifact == ART_UNWERTH) || (uarms && uarms->oartifact == ART_NO_REDUCTION_ALLOWED) || (uarmf && uarmf->oartifact == ART_NO_LITTLE_BIT_OF_WISDOM) || (uimplant && uimplant->oartifact == ART_HIGHERHOLDER) || (uarm && uarm->oartifact == ART_POISUSTAIN) || (uarms && uarms->oartifact == ART_BONUS_HOLD) || (uamul && uamul->oartifact == ART_FIX_EVERYTHING) || (uarmf && uarmf->oartifact == ART_ELENETTES))
/* note by Amy: unless you're the sustainer race, there's 10% chance that stat loss still happens, stat gain is disabled */
#define SustainLossSafe		(Race_if(PM_SUSTAINER) || (uimplant && uimplant->oartifact == ART_HIGHERHOLDER) || rn2(10))

#define Lifesaved		u.uprops[LIFESAVED].extrinsic

#define HKeen_memory		u.uprops[KEEN_MEMORY].intrinsic
#define EKeen_memory		u.uprops[KEEN_MEMORY].extrinsic
#define IntKeen_memory	(HKeen_memory || (ublindf && objects[ublindf->otyp].oc_material == MT_MENGETIUM) || (youmonst.data->mlet == S_QUADRUPED))
#define ExtKeen_memory	(EKeen_memory || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == KEEN_MEMORY) ) || (uarmu && uarmu->oartifact == ART_MEMORIAL_GARMENTS) || (uarmc && uarmc->oartifact == ART_MEMORY_AID) || (uarmc && uarmc->oartifact == ART_REMEMBERING_THE_BAD_TIMES) || (uarmf && uarmf->oartifact == ART_DESERT_MEDITATION) || (uarmh && uarmh->oartifact == ART_VERY_MEMORY) || (uamul && uamul->oartifact == ART_GRAY_ENPROTECTION) || have_minimejewel() || (uwep && uwep->oartifact == ART_ICED_OUT_COMPUTER_ON_A_CHA) || (uball && uball->oartifact == ART_ICED_OUT_COMPUTER_ON_A_CHA) || (uarm && uarm->oartifact == ART_SPELLNOTFORGET) || (uwep && uwep->oartifact == ART_DESSESGA) || (uwep && uwep->oartifact == ART_FADING_FROM_MEMORY) || (uarm && uarm->oartifact == ART_REMEMBERANCE___THE_BURST_T) || (uarms && uarms->oartifact == ART_GOLDEN_DAWN) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_SARREFREE) || (uimplant && uimplant->oartifact == ART_UNFORGETTABLE_EVENT) || (uarmh && uarmh->oartifact == ART_TRANSMEMORIZER) || (uamul && uamul->oartifact == ART_ARABELLA_S_DICINATOR) || (uarms && uarms->oartifact == ART_GREXIT_IS_NEAR) || (uamul && uamul->oartifact == ART_COMPUTER_AMULET) )

#define Keen_memory		(((IntKeen_memory && u.nonintrinsicproperty != KEEN_MEMORY) || (ExtKeen_memory && u.nonextrinsicproperty != KEEN_MEMORY)) && !Role_if(PM_NOOB_MODE_BARB) && !NoKeen_memory)
#define StrongKeen_memory	(IntKeen_memory && ExtKeen_memory && Keen_memory && u.nondoubleproperty != KEEN_MEMORY)

#define NoKeen_memory	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_KEEN_MEMORY].intrinsic || UHaveAids || (u.impossibleproperty == KEEN_MEMORY) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HSight_bonus		u.uprops[SIGHT_BONUS].intrinsic
#define ESight_bonus		u.uprops[SIGHT_BONUS].extrinsic
#define IntSight_bonus	(HSight_bonus || (uwep && uwep->oartifact == ART_MAERCOFOG) || (uarm && uarm->oartifact == ART_BECAUSE_YOU_LOSE) )
#define ExtSight_bonus	(ESight_bonus || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == SIGHT_BONUS) ) || (uwep && uwep->oartifact == ART_PEOPLE_EATING_TRIDENT) || (uwep && uwep->otyp == FIRE_STICK) || (uwep && uwep->otyp == ZOOM_SHOT_CROSSBOW) || (uarmh && uarmh->oartifact == ART_THERE_ARE_SEVERAL_OF_THEM) || (uwep && uwep->oartifact == ART_ZIRATHA_S_IRON_SIGHTS) || (uarm && uarm->oartifact == ART_FACKLE_THERE) || (uarmu && uarmu->oartifact == ART_SOME_CHAMBER_DOOR) || (uwep && uwep->oartifact == ART_LUX_REGINA) || (uwep && uwep->oartifact == ART_REGINA_LUX) || autismringcheck(ART_BERNCELD) || (uwep && uwep->oartifact == ART_ALWAYS_IN_IT) || (uamul && uamul->oartifact == ART_SUPER_GRAPHICS_CARD_BUG) || (uarm && uarm->oartifact == ART_BECAUSE_YOU_LOSE) || (uarm && uarm->oartifact == ART_UPPERWEAVER_S_THREAD) || (uwep && uwep->oartifact == ART_MINE_OUT) || (ublindf && ublindf->oartifact == ART_EYES_OF_THE_ORACLE) || (uwep && uwep->oartifact == ART_AXELLE_S_CHAIN) || (uarmu && uarmu->oartifact == ART_HA_HA_HA_HA___) || (uwep && uwep->oartifact == ART_TSCHAPSNAP) || (uwep && uwep->oartifact == ART_FINALLY__A_USABLE_POLEARM_) || (uarmf && uarmf->oartifact == ART_LITE_THE_AREA) || (u.missingstaircase && uarm && uarm->oartifact == ART_ADD_THE_MISSING_STAIRCASE) || (uwep && uwep->oartifact == ART_BRIGHTE_SEE__EEEE) || (uwep && uwep->oartifact == ART_LETS_MAKE_IT_OFFICIAL && u.kliuskill >= 20) || (uarmc && uarmc->oartifact == ART_BILLS_PAID) || (uwep && uwep->oartifact == ART_HALLOW_MOONFALL) || (uwep && uwep->oartifact == ART_MAERCOFOG) || (uarmh && uarmh->oartifact == ART_BE_THE_LITE) || (uwep && uwep->oartifact == ART_LONGLOSS) || (uwep && uwep->oartifact == ART_AXE_OF_THE_DWARVISH_LORDS) || (uarms && uarms->oartifact == ART_SOLAR_POWER) || (uamul && uamul->oartifact == ART_GOOD_BEE) || (uarmf && uarmf->oartifact == ART_GRENEUVENIA_S_HUG) || (uamul && uamul->oartifact == ART_BUEING) || (uarmh && uarmh->oartifact == ART_HAT_OF_THE_ARCHMAGI) )

#define Sight_bonus		(((IntSight_bonus && u.nonintrinsicproperty != SIGHT_BONUS) || (ExtSight_bonus && u.nonextrinsicproperty != SIGHT_BONUS)) && !NoSight_bonus)
#define StrongSight_bonus	(IntSight_bonus && ExtSight_bonus && Sight_bonus && u.nondoubleproperty != SIGHT_BONUS)

#define NoSight_bonus	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SIGHT_BONUS].intrinsic || UHaveAids || (u.impossibleproperty == SIGHT_BONUS) || autismweaponcheck(ART_HYPER_INTELLIGENCE) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HVersus_curses		u.uprops[VERSUS_CURSES].intrinsic
#define EVersus_curses		u.uprops[VERSUS_CURSES].extrinsic
#define IntVersus_curses	(HVersus_curses || (u.twoweap && uswapwep && objects[uswapwep->otyp].oc_material == MT_MENGETIUM) )
#define ExtVersus_curses	(EVersus_curses || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == VERSUS_CURSES) ) || (uwep && uwep->oartifact == ART_WINCHESTER_PREMIUM) || (uwep && uwep->oartifact == ART_FAMOUS_LANCE) || (uarmf && uarmf->oartifact == ART_ARABELLA_S_GIRL_KICK) || (uwep && uwep->oartifact == ART_HOLY_LANCE) || (uwep && uwep->oartifact == ART_HOLY_RANCE) || (uarmf && uarmf->oartifact == ART_LIGHT_DAL_I_THALION) || (uarmf && uarmf->oartifact == ART_SASSY_JULIA) || (uwep && uwep->oartifact == ART_STAFF_OF_NECROMANCY) || (uarmu && uarmu->oartifact == ART_MEMORIAL_GARMENTS) )

#define Versus_curses		(((IntVersus_curses && u.nonintrinsicproperty != VERSUS_CURSES) || (ExtVersus_curses && u.nonextrinsicproperty != VERSUS_CURSES)) && !NoVersus_curses)
#define StrongVersus_curses	(IntVersus_curses && ExtVersus_curses && Versus_curses && u.nondoubleproperty != VERSUS_CURSES)

#define NoVersus_curses	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_VERSUS_CURSES].intrinsic || UHaveAids || (u.impossibleproperty == VERSUS_CURSES) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HStun_resist		u.uprops[STUN_RES].intrinsic
#define EStun_resist		u.uprops[STUN_RES].extrinsic
#define IntStun_resist	(HStun_resist || Race_if(PM_EROSATOR) || tech_inuse(T_STAT_RESIST))
#define ExtStun_resist	(EStun_resist || (uarmf && uarmf->oartifact == ART_KRISTIN_S_CHEATS) || (uwep && uwep->oartifact == ART_DEATHWRECKER) || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == STUN_RES) ) || (uarmu && uarmu->oartifact == ART_TOTAL_CONTROL) || (uwep && uwep->oartifact == ART_LAUGHTERSNEE) || (uwep && uwep->oartifact == ART_OGRESMASHER____) || (uarm && uarm->oartifact == ART_PROTECTION_WITH_A_PRICE) || (PlayerInSexyFlats && uarmf && uarmf->oartifact == ART_FORMO____) || autismringcheck(ART_BRIGHT_OVERWHELMING) || (uarm && uarm->oartifact == ART_DERANDERGRAAA) || (uarmh && uarmh->oartifact == ART_SEE_THE_TOTAL) || (uarmf && uarmf->oartifact == ART_RUBBER_LOVE) || (uwep && uwep->oartifact == ART_PENDULUM_OF_BALANCE) || (uarmf && uarmf->otyp == CATWALK_SANDALS) || (uarmf && uarmf->oartifact == ART_JENNY_SUPERSOFT) || (uwep && uwep->oartifact == ART_SPEAR_OF_PEACE) || (uwep && uwep->oartifact == ART_TURVANG_S_THUNDER) || (uarmf && uarmf->oartifact == ART_ENDLESS_DESEAMING) || autismringcheck(ART_RELIABLE_TRINSICS) || (uarmf && uarmf->oartifact == ART_CLONE_) || (uwep && uwep->oartifact == ART_JOUSTER_POWER) || (uarmc && uarmc->oartifact == ART_FREQUENT_BUT_WEAK_STATUS) || (uarmh && uarmh->oartifact == ART_CLAUDIA_S_DIRECTION_FINDER) || (uarmh && uarmh->oartifact == ART_UBB_RUPTURE) || (uarm && uarm->oartifact == ART_VENGEANCE_OF_ETERNITY) || (uarmh && uarmh->oartifact == ART_MIND_SHIELDING) || (uarms && uarms->oartifact == ART_FIVE_STAR_PARTY) || (uamul && uamul->oartifact == ART_ONE_MOMENT_IN_TIME) || (uamul && uamul->oartifact == ART_PRECIOUS_UNOBTAINABLE_PROP) || (uamul && uamul->oartifact == ART_WOBBLESTEADY) || (uamul && uamul->oartifact == ART_RECOVERED_RELIC) )

#define Stun_resist		(((IntStun_resist && u.nonintrinsicproperty != STUN_RES) || (ExtStun_resist && u.nonextrinsicproperty != STUN_RES)) && !hybridragontype(AD_LITE) && !NoStun_resist)
#define StrongStun_resist	(IntStun_resist && ExtStun_resist && Stun_resist && u.nondoubleproperty != STUN_RES)

#define NoStun_resist	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_STUN_RES].intrinsic || Race_if(PM_TUMBLRER) || UHaveAids || (u.impossibleproperty == STUN_RES) || (uarmf && uarmf->oartifact == ART_FERGIE_S_GLACIER) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HConf_resist		u.uprops[CONF_RES].intrinsic
#define EConf_resist		u.uprops[CONF_RES].extrinsic
#define IntConf_resist	(HConf_resist || Race_if(PM_EROSATOR) || tech_inuse(T_STAT_RESIST) || FemtrapActiveClaudia )
#define ExtConf_resist	(EConf_resist || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == CONF_RES) ) || (uarmf && uarmf->oartifact == ART_BOOTS_OF_THE_MACHINE) || (uwep && uwep->oartifact == ART_LAUGHTERSNEE) || (uwep && uwep->oartifact == ART_OGRESMASHER____) || (uarmg && uarmg->oartifact == ART_EGASSO_S_GIBBERISH) || (uarmf && uarmf->oartifact == ART_KRAWASAKI_STEERER) || (uwep && uwep->oartifact == ART_BLOOD_MOON) || (uwep && uwep->oartifact == ART_BLOODY_MACE) || (uarmh && uarmh->oartifact == ART_EFGOURD) || (uarmh && uarmh->oartifact == ART_CAP_OF_THENGEL) || (uarmf && uarmf->oartifact == ART_CRASHING_YOUR_SISTER_S_WED) || (uarmf && uarmf->otyp == CATWALK_SANDALS) || (uarmh && uarmh->oartifact == ART_SEE_THE_TOTAL) || (uwep && uwep->oartifact == ART_SEA_ANGERANCHOR) || (uwep && uwep->oartifact == ART_FUNE_NO_IKARI) || (uarmf && uarmf->oartifact == ART_PURPLE_SANDSTORM) || (uarmh && uarmh->oartifact == ART_CLAUDIA_S_DIRECTION_FINDER) || (uwep && uwep->oartifact == ART_SHINING_SILVER_KATANA) || (uwep && uwep->oartifact == ART_ZANTETSU) || (uarmh && uarmh->oartifact == ART_CROWN_OF_THE_PERCIPIENT) || (uarmh && uarmh->oartifact == ART_SAGE_S_HELM) || (uarmf && uarmf->oartifact == ART_SABRINA_S_FREEDOM) || (uarmh && uarmh->oartifact == ART_ENFORCED_MIND) || (uarm && uarm->oartifact == ART_GLARINGADERNADIR) || (uarmh && uarmh->oartifact == ART_ENSNARED_MIND) || (uamul && uamul->oartifact == ART_STAR_OF_HYPERNOTUS) || (uarmg && uarmg->oartifact == ART_GLOVES_OF_VESDA) || (uwep && uwep->oartifact == ART_SPEAR_OF_PEACE) || (uimplant && uimplant->oartifact == ART_SARREFREE) || (uwep && uwep->oartifact == ART_HEAVY_CROSSBOW_OF_ETERNITY) || (uarmf && uarmf->oartifact == ART_CLONE_) || (uarmf && uarmf->oartifact == ART_UNFELLABLE_TREE && u.burrowed) || (uarmf && uarmf->oartifact == ART_SANDRA_S_BEAUTIFUL_FOOTWEA) || (uarmf && uarmf->oartifact == ART_SARAH_S_GRANNY_WEAR) || (uarms && uarms->oartifact == ART_FIVE_STAR_PARTY) || (uarmf && uarmf->oartifact == ART_SARAH_S_SNEAKERS_OF_INSTAN) || (uarmu && uarmu->oartifact == ART_TOTAL_CONTROL) || (uarmc && uarmc->oartifact == ART_FREQUENT_BUT_WEAK_STATUS) )

#define Conf_resist		(((IntConf_resist && u.nonintrinsicproperty != CONF_RES) || (ExtConf_resist && u.nonextrinsicproperty != CONF_RES)) && !Race_if(PM_TONBERRY) && !hybridragontype(AD_LITE) && !NoConf_resist)
#define StrongConf_resist	(IntConf_resist && ExtConf_resist && Conf_resist && u.nondoubleproperty != CONF_RES)

#define NoConf_resist	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_CONF_RES].intrinsic || Race_if(PM_ADDICT) || (uarmh && uarmh->oartifact == ART_UBB_RUPTURE) || autismweaponcheck(ART_ATLUS_HEAVE) || (uarmf && uarmf->oartifact == ART_FERGIE_S_GLACIER) || UHaveAids || (u.impossibleproperty == CONF_RES) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HPsi_resist		u.uprops[PSI_RES].intrinsic
#define EPsi_resist		u.uprops[PSI_RES].extrinsic
#define IntPsi_resist	(HPsi_resist || (uarmh && uarmh->oartifact == ART_ALIEN_MINDWAVE) || (uarmh && objects[uarmh->otyp].oc_material == MT_MENGETIUM) || (Upolyd && attackdamagetype(youmonst.data, AT_BREA, AD_SPC2) ))
#define ExtPsi_resist	(playerextrinsicpsires())

#define Psi_resist		(((IntPsi_resist && u.nonintrinsicproperty != PSI_RES) || (ExtPsi_resist && u.nonextrinsicproperty != PSI_RES)) && !hybridragontype(AD_SPC2) && !NoPsi_resist)
#define StrongPsi_resist	(IntPsi_resist && ExtPsi_resist && Psi_resist && u.nondoubleproperty != PSI_RES)

#define NoPsi_resist	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_PSI_RES].intrinsic || UHaveAids || (u.impossibleproperty == PSI_RES) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HDouble_attack		u.uprops[DOUBLE_ATTACK].intrinsic
#define EDouble_attack		u.uprops[DOUBLE_ATTACK].extrinsic
#define IntDouble_attack	(HDouble_attack)
#define ExtDouble_attack	(EDouble_attack || (uwep && uwep->oartifact == ART_CYCLE_WHACK_WHACK && u.usteed) || (Race_if(PM_PLAYER_ASURA) && u.twoweap) || autismringcheck(ART_RING_OF_FAST_LIVING) || (uarm && uarm->oartifact == ART_OFFENSE_OWNS_DEFENSE) || autismweaponcheck(ART_WAS_TIPPT_DAS_DENN_IMMER_D) || autismringcheck(ART_RING_OF_EXTRA_ATTACKS) || (uamul && uamul->oartifact == ART_TWIN_EDGE) || (uarmh && uarmh->oartifact == ART_FIVE_HORNED_HELM) || (uimplant && uimplant->oartifact == ART_THROW_MY_LIFE_AWAY) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_YOU_SHOULD_SURRENDER) || autismweaponcheck(ART_DOUBLE_ME_) || autismweaponcheck(ART_BELLELDOUBLE) || autismweaponcheck(ART_WILD_WHIRLING) || (uarmf && uarmf->oartifact == ART_ELLA_S_BLOODLUST) || autismweaponcheck(ART_MELEE_DUALITY))

#define Double_attack		(((IntDouble_attack && u.nonintrinsicproperty != DOUBLE_ATTACK) || (ExtDouble_attack && u.nonextrinsicproperty != DOUBLE_ATTACK)) && !NoDouble_attack)
#define StrongDouble_attack	(IntDouble_attack && ExtDouble_attack && Double_attack && u.nondoubleproperty != DOUBLE_ATTACK)

#define NoDouble_attack	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_DOUBLE_ATTACK].intrinsic || UHaveAids || (u.nonextrinsicproperty == DOUBLE_ATTACK && !HDouble_attack) || (u.impossibleproperty == DOUBLE_ATTACK) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HQuad_attack		u.uprops[QUAD_ATTACK].intrinsic
#define EQuad_attack		u.uprops[QUAD_ATTACK].extrinsic
#define IntQuad_attack	(HQuad_attack)
#define ExtQuad_attack	(EQuad_attack || autismweaponcheck(ART_EXTREME_BITCH) || (uarmc && uarmc->oartifact == ART_SUPERMAN_S_SUPER_SUIT) || autismringcheck(ART_THAT_S_SUCH_A_BUG) || (uimplant && uimplant->oartifact == ART_IME_SPEW))

#define Quad_attack		(((IntQuad_attack && u.nonintrinsicproperty != QUAD_ATTACK) || (ExtQuad_attack && u.nonextrinsicproperty != QUAD_ATTACK)) && !NoQuad_attack)
#define StrongQuad_attack	(IntQuad_attack && ExtQuad_attack && Quad_attack && u.nondoubleproperty != QUAD_ATTACK)

#define NoQuad_attack	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_QUAD_ATTACK].intrinsic || UHaveAids || (u.nonextrinsicproperty == QUAD_ATTACK && !HQuad_attack) || (u.impossibleproperty == QUAD_ATTACK) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HUseTheForce		u.uprops[THE_FORCE].intrinsic
#define EUseTheForce		u.uprops[THE_FORCE].extrinsic
#define IntUseTheForce	(HUseTheForce)
#define ExtUseTheForce	(EUseTheForce || (uarm && uarm->oartifact == ART_SUSA_MAIL) || (uwep && uwep->oartifact == ART_THAIFORCE) || (uwep && uwep->otyp == DOUBLE_FORCE_BLADE) || (uwep && uwep->otyp == FORCE_WHIP) || (uwep && uwep->oartifact == ART_HENRIETTENFORCE) || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == THE_FORCE) ) || (uarm && uarm->oartifact == ART_LUKE_S_JEDI_POWER))

#define UseTheForce		(((IntUseTheForce && u.nonintrinsicproperty != THE_FORCE) || (ExtUseTheForce && u.nonextrinsicproperty != THE_FORCE)) && !NoUseTheForce)
#define StrongUseTheForce	(IntUseTheForce && ExtUseTheForce && UseTheForce && u.nondoubleproperty != THE_FORCE)

#define NoUseTheForce	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_THE_FORCE].intrinsic || UHaveAids || (u.impossibleproperty == THE_FORCE) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HCont_resist		u.uprops[CONT_RES].intrinsic
#define ECont_resist		u.uprops[CONT_RES].extrinsic
#define IntCont_resist	(HCont_resist || (uarms && objects[uarms->otyp].oc_material == MT_MENGETIUM) )
#define ExtCont_resist	(ECont_resist || (powerfulimplants() && uimplant && uimplant->oartifact == ART_ARABELLA_S_SEXY_CHARM) || (uwep && uwep->oartifact == ART_CASTOR_BOOST) || (uarm && uarm->oartifact == ART_HAZARDOUS_EQUIPMENT) || (uarm && uarm->oartifact == ART_RES_BUT_BAD) || (uamul && uamul->oartifact == ART_NUCLEAR_WORTH) || (uarm && uarm->oartifact == ART_____DUR) || (uwep && uwep->oartifact == ART_EXPERIMENTAL_CHUNK) || have_contaminatejewel() || (uwep && uwep->oartifact == ART_SHENA_S_PANTY) || (uwep && uwep->oartifact == ART_SCENTFUL_PANTY) || (have_oneinfourstone() && (moves % 4 == 2) ) || (uwep && uwep->oartifact == ART_GODAWFUL_ENCHANTMENT) || (uwep && uwep->oartifact == ART_DANGER_SIGN) || (uarmc && uarmc->oartifact == ART_EREA_S_HERITAGE) || (uarmc && uarmc->oartifact == ART_HEV_SUIT) || (uimplant && uimplant->oartifact == ART_POTATOROK) || (uarmf && uarmf->oartifact == ART_PSI_ONIC) || (uarmf && uarmf->oartifact == ART_SPACE_SHUTTLE) || (uarmc && uarmc->oartifact == ART_INA_S_SORROW && u.uhunger < 0) || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == CONT_RES) ) )

#define Cont_resist		(((IntCont_resist && u.nonintrinsicproperty != CONT_RES) || (ExtCont_resist && u.nonextrinsicproperty != CONT_RES)) && !NoCont_resist)
#define StrongCont_resist	(IntCont_resist && ExtCont_resist && Cont_resist && u.nondoubleproperty != CONT_RES)

#define NoCont_resist	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_CONT_RES].intrinsic || UHaveAids || (u.impossibleproperty == CONT_RES) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HDiscount_action	u.uprops[DISCOUNT_ACTION].intrinsic
#define EDiscount_action	u.uprops[DISCOUNT_ACTION].extrinsic
#define IntDiscount_action	(HDiscount_action || (uarmf && uarmf->oartifact == ART_HEIKE_S_BIGOTRY) || (uwep && uwep->oartifact == ART_MAILIE_S_SELF_CENTRATION) || (uarmc && uarmc->oartifact == ART_STRIPED_SHIRT_OF_THE_FALSE) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_CLEAN_ASCENSION_RUN && In_endgame(&u.uz)) )
#define ExtDiscount_action	(EDiscount_action || (uarmf && uarmf->oartifact == ART_HEIKE_S_BIGOTRY) || (uwep && uwep->oartifact == ART_MAILIE_S_SELF_CENTRATION) || (uarmf && uarmf->oartifact == ART_LISA_S_BUCKLED_BEAUTIES) || (uwep && uwep->oartifact == ART_HARMONY_VIRGIN) || (uwep && uwep->oartifact == ART_SHINING_SILVER_KATANA) || (uwep && uwep->oartifact == ART_ZANTETSU) || (uarmf && uarmf->oartifact == ART_CRASHING_YOUR_SISTER_S_WED) || (uarmf && uarmf->oartifact == ART_ELEVECULT) || (uimplant && uimplant->oartifact == ART_CLEAN_ASCENSION_RUN && In_endgame(&u.uz)) || (Role_if(PM_SINGSLAVE) && uarmf && uarmf->oartifact == ART_LORENZI_S_CLEANING_RESIDUE) || (uarm && uarm->oartifact == ART_JOHN_CENA_SPLIT_MAIL) || (uarms && uarms->oartifact == ART_SHIELD_OF_THORN) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_GOOSCH_HIDARI) || (flags.female && uarmf && uarmf->oartifact == ART_BS_____) || (!player_wears_metal() && uarmc && uarmc->oartifact == ART_NO_METAL_ALLOWED) || (uamul && uamul->oartifact == ART_PROTECTED_MODE_RUN_TIME) || (uwep && uwep->oartifact == ART_TONA_S_GAMES && (PlayerInBlockHeels || PlayerInWedgeHeels) ) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_CAN_T_BRING_US_DOWN) || (uarm && uarm->oartifact == ART_DREAD_PELT) || (uarmc && uarmc->oartifact == ART_STRIPED_SHIRT_OF_THE_FALSE) || (uarmf && uarmf->oartifact == ART_NANCY_S_ASSISTANCE) || (uwep && uwep->oartifact == ART_HELF_ME_NEVERTHELESS) || autismringcheck(ART_BUT_YOU_MUST) || (uarmf && uarmf->oartifact == ART_NADINE_S_CUTENESS) || (uwep && uwep->oartifact == ART_TRUE_MASTERPIECE) || (uarmf && uarmf->oartifact == ART_IMPOSSIBLE_CATWALK) || (uarmf && uarmf->oartifact == ART_FULL_PROGRAM) || (uarm && uarm->oartifact == ART_LYNN_S_EVERVIGILANCE) || (uwep && uwep->oartifact == ART_NOOBY_BONUS_STYLE && bimanual(uwep)) || (uwep && uwep->oartifact == ART_SMOKING_SQUIRREL) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_BUCKET_HOUSE) || (uarmu && uarmu->oartifact == ART_PEDIATRIC_GAWKING_GANGS) || (uwep && uwep->oartifact == ART_COUNTERSTRIKE_CESTUS) || (uarmu && uarmu->oartifact == ART_KING_OF_PORN) || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == DISCOUNT_ACTION) ) )

#define Discount_action		(((IntDiscount_action && u.nonintrinsicproperty != DISCOUNT_ACTION) || (ExtDiscount_action && u.nonextrinsicproperty != DISCOUNT_ACTION)) && !NoDiscount_action)
#define StrongDiscount_action	(IntDiscount_action && ExtDiscount_action && Discount_action && u.nondoubleproperty != DISCOUNT_ACTION)

#define NoDiscount_action	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_DISCOUNT_ACTION].intrinsic || (isevilvariant && Free_action) || UHaveAids || (u.impossibleproperty == DISCOUNT_ACTION) || (uimplant && uimplant->oartifact == ART_KATRIN_S_SUDDEN_APPEARANCE) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HFull_nutrient		u.uprops[FULL_NUTRIENT].intrinsic
#define EFull_nutrient		u.uprops[FULL_NUTRIENT].extrinsic
#define IntFull_nutrient	(HFull_nutrient || have_minimejewel() )
#define ExtFull_nutrient	(EFull_nutrient || have_minimejewel() || (powerfulimplants() && uimplant && uimplant->oartifact == ART_ARABELLA_S_SEXY_CHARM) || (uarmf && uarmf->oartifact == ART_BEAUTYQUEAK) || (uarmf && uarmf->oartifact == ART_SWEET_CHERRY) || (uarmh && uarmh->oartifact == ART_POURRI_R_) || (uarmh && uarmh->oartifact == ART_SALADIN_S_DESERT_FOX) || (uarmh && uarmh->oartifact == ART_NUTRITION_AND_DIETETICS) || (uwep && uwep->oartifact == ART_SMOKING_SQUIRREL) || (uarmf && uarmf->oartifact == ART_REVELATION____VERSE__) || (uarm && uarm->oartifact == ART_YOU_REALLY_HAVE_A_TOTAL_DA) || (uarmg && uarmg->oartifact == ART_HEALING_WATERS) || (uwep && uwep->oartifact == ART_HK_CLEAR) || (uwep && uwep->oartifact == ART_USELESS_TALK) || (uwep && uwep->oartifact == ART_BREAD_FOR_THE_WORLD) || (uwep && uwep->oartifact == ART_MILENA_S_MISGUIDING) || (uwep && uwep->oartifact == ART_MEAT_S_BACK_ON_THE_MENU) || (u.umoved && uarmc && uarmc->oartifact == ART_BUT_SHES_HOMELESS) || autismringcheck(ART_MOUTHS_CRADLE) || (uimplant && uimplant->oartifact == ART_ENTEROGASTER_ALTO) || (uwep && uwep->oartifact == ART_CALF_CUTLET_WITHOUT_BEOD) || (uwep && uwep->oartifact == ART_DUMPSTERMAN) || (uwep && uwep->oartifact == ART_VIHAT_BAGUETTEN_BUS_STOP) || (uwep && uwep->oartifact == ART_EDERGRADE) || (uarmf && uarmf->oartifact == ART_DESEAMING_GAME) || (uarmf && uarmf->oartifact == ART_U_BE_CURRY) || (uwep && uwep->oartifact == ART_GIANT_MEAT_STICK) || (uwep && uwep->oartifact == ART_CUTRITION) || autismringcheck(ART_RELIABLE_TRINSICS) || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == FULL_NUTRIENT) ) )

#define Full_nutrient		(((IntFull_nutrient && u.nonintrinsicproperty != FULL_NUTRIENT) || (ExtFull_nutrient && u.nonextrinsicproperty != FULL_NUTRIENT)) && !Race_if(PM_PERVERT) && !NoFull_nutrient)
#define StrongFull_nutrient	(IntFull_nutrient && ExtFull_nutrient && Full_nutrient && u.nondoubleproperty != FULL_NUTRIENT)

#define NoFull_nutrient	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_FULL_NUTRIENT].intrinsic || UHaveAids || (u.impossibleproperty == FULL_NUTRIENT) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HTechnicality		u.uprops[TECHNICALITY].intrinsic
#define ETechnicality		u.uprops[TECHNICALITY].extrinsic
#define IntTechnicality	(HTechnicality || have_minimejewel() || autismringcheck(ART_CORONA_OF_THE_ELEMENT) )
#define ExtTechnicality	(ETechnicality || have_minimejewel() || autismringcheck(ART_CORONA_OF_THE_ELEMENT) || (uwep && uwep->oartifact == ART_TECHCTION) || (uarm && uarm->oartifact == ART_REQUIRED_POWER_PLANT_GEAR) || (uamul && uamul->oartifact == ART_ON_TRANCE) || (uarmf && uarmf->oartifact == ART_PATCHWORK) || (Role_if(PM_ASSASSIN) && uwep && uwep->oartifact == ART_FINAL_DOOR_SOLUTION) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_CORONATION_CULMINATION) || (uarm && uarm->oartifact == ART_CHIP_INFACT) || (uarmf && uarmf->oartifact == ART_CANNOT_WALK_WITH_THE_ARIAN) || (uarm && uarm->oartifact == ART_TECH_IT_UP) || (uwep && uwep->oartifact == ART_TODAY_S_REPAIR) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_SYMPLANT) || (uarmc && uarmc->oartifact == ART_HITEC_NEO) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_ARABELLA_S_SEXY_CHARM) || (uwep && uwep->oartifact == ART_PSI_TEC) || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == TECHNICALITY) ) )

#define Technicality		(((IntTechnicality && u.nonintrinsicproperty != TECHNICALITY) || (ExtTechnicality && u.nonextrinsicproperty != TECHNICALITY)) && !NoTechnicality)
#define StrongTechnicality	(IntTechnicality && ExtTechnicality && Technicality && u.nondoubleproperty != TECHNICALITY)

#define NoTechnicality	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_TECHNICALITY].intrinsic || UHaveAids || (u.impossibleproperty == TECHNICALITY) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HScentView		u.uprops[SCENT_VIEW].intrinsic
#define EScentView		u.uprops[SCENT_VIEW].extrinsic
#define IntScentView	(HScentView || (Upolyd && !haseyes(youmonst.data) && is_animal(youmonst.data)) )
#define ExtScentView	(EScentView || (uarm && uarm->oartifact == ART_FARTHER_INTO_THE_JUNGLE) || (uarmf && uarmf->oartifact == ART_CAMELIC_SCENT) || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == SCENT_VIEW) ) || (uarmh && uarmh->oartifact == ART_HEARING_EAR) || (uarmh && uarmh->oartifact == ART_COMPLETE_SIGHT) || (uwep && uwep->oartifact == ART_INSECTMASHER) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_THEY_RE_REALLY_AFTER_ME) || (uwep && uwep->oartifact == ART_STEALTHY_EAR) || (uwep && uwep->oartifact == ART_HELIOPOLIS_MISTAKE) || (uarm && uarm->oartifact == ART_CRYPT_SMELL && in_rooms(u.ux, u.uy, CRYPTROOM)) || (uarmf && uarmf->oartifact == ART_SAFARI_ROCKZ) || (ublindf && ublindf->oartifact == ART_SCENTFOLD) )

#define ScentView		(((IntScentView && u.nonintrinsicproperty != SCENT_VIEW) || (ExtScentView && u.nonextrinsicproperty != SCENT_VIEW)) && !NoScentView)
#define StrongScentView	(IntScentView && ExtScentView && ScentView && u.nondoubleproperty != SCENT_VIEW)
#define HaveEcholocation	((Role_if(PM_BARD) || (uwep && uwep->oartifact == ART_GAME_KNOWLEDGE_CHEAT_SHEET) || (uwep && uwep->oartifact == ART_PEREGRINE_OF_NIGHT) || (uarm && uarm->oartifact == ART_FARTHER_INTO_THE_JUNGLE) || (uarm && uarm->oartifact == ART_PUNKED_BY_HEIDI) || (uarmh && uarmh->oartifact == ART_HEARING_EAR) || (uarmh && uarmh->oartifact == ART_COMPLETE_SIGHT) || (uarmf && uarmf->oartifact == ART_SQUARE_ZONE) || (uwep && uwep->oartifact == ART_SKODIT) || (uwep && uwep->oartifact == ART_HELIOPOLIS_MISTAKE) || (Upolyd && !haseyes(youmonst.data) && !is_animal(youmonst.data)) || Role_if(PM_STAND_USER) || RngeEcholocation || Race_if(PM_ANCIPITAL) || Race_if(PM_BATMAN) || Race_if(PM_CHIROPTERAN) || Race_if(PM_HC_ALIEN) || Race_if(PM_SLYER_ALIEN) || u.echolocationspell || (Upolyd && youmonst.data->mlet == S_BAT) ) && !NoScentView && u.nonintrinsicproperty != SCENT_VIEW)
#define EcholocationActive	(StrongScentView || HaveEcholocation)

#define NoScentView	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SCENT_VIEW].intrinsic || UHaveAids || (u.impossibleproperty == SCENT_VIEW) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HDiminishedBleeding		u.uprops[DIMINISHED_BLEEDING].intrinsic
#define EDiminishedBleeding		u.uprops[DIMINISHED_BLEEDING].extrinsic
#define IntDiminishedBleeding	(HDiminishedBleeding || (uarmu && objects[uarmu->otyp].oc_material == MT_MENGETIUM) || (Upolyd && !has_blood(youmonst.data)) )
#define ExtDiminishedBleeding	(EDiminishedBleeding || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == DIMINISHED_BLEEDING) ) || (uwep && uwep->oartifact == ART_AL_AZZARED_GYETH_GYETH) || (uarmf && uarmf->oartifact == ART_ARABELLA_S_GIRL_KICK) || (uarmf && uarmf->oartifact == ART_LYDIA_S_SEXYROOM) || (uarmf && uarmf->oartifact == ART_ANITA_S_RASHLUST) || (uarmf && uarmf->oartifact == ART_FIRST_PAIR) || (powerfulimplants() && uimplant && uimplant->oartifact == ART_DO_NOT_WANT_SEX) || (uarmf && uarmf->oartifact == ART_JUNGLE_LOVE) || (uarmc && uarmc->oartifact == ART_EXCITEMENT_OF_HAX) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_SABER_TONFA) || (uarmf && uarmf->oartifact == ART_GOT_THAT_STARWARS_ENTRANCE) || (uwep && uwep->oartifact == ART_TLAHUIZCALPANTECUHTLI) || (uwep && uwep->oartifact == ART_SHRED_THEM_) || (uarmu && uarmu->oartifact == ART_CRIMSON_PLATE) || (uwep && uwep->oartifact == ART_SELF_PROTECTION_IMPLEMENT) || (uarmf && uarmf->oartifact == ART_SANDALMENS && flags.female) || (uarmf && uarmf->oartifact == ART_KATI_S_IRRESISTIBLE_STILET) || (uwep && uwep->oartifact == ART_DOLORES_S_SADISM) || (uwep && uwep->oartifact == ART_ARABELLA_S_BLACK_PRONG))

#define DiminishedBleeding		(((IntDiminishedBleeding && u.nonintrinsicproperty != DIMINISHED_BLEEDING) || (ExtDiminishedBleeding && u.nonextrinsicproperty != DIMINISHED_BLEEDING)) && !Role_if(PM_BLEEDER) && !Race_if(PM_HEMOPHAGE) && !NoDiminishedBleeding)
#define StrongDiminishedBleeding	(IntDiminishedBleeding && ExtDiminishedBleeding && DiminishedBleeding && u.nondoubleproperty != DIMINISHED_BLEEDING)

#define NoDiminishedBleeding	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_DIMINISHED_BLEEDING].intrinsic || UHaveAids || (u.impossibleproperty == DIMINISHED_BLEEDING) || Role_if(PM_BLEEDER) || Race_if(PM_HEMOPHAGE) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HControlMagic		u.uprops[CONTROL_MAGIC].intrinsic
#define EControlMagic		u.uprops[CONTROL_MAGIC].extrinsic
#define IntControlMagic	(HControlMagic || autismringcheck(ART_HAVE_FULL_SWAY) || (uarm && uarm->oartifact == ART_SPECIAL_ROBUNG) )
#define ExtControlMagic	(EControlMagic || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == CONTROL_MAGIC) ) || autismringcheck(ART_HAVE_FULL_SWAY) || (uarmf && uarmf->oartifact == ART_BLUE_BLOSSOM) || (uarmf && uarmf->oartifact == ART_ARABELLA_S_GIRL_KICK) || (uarmf && uarmf->oartifact == ART_CRASHING_YOUR_SISTER_S_WED) || (uwep && uwep->oartifact == ART_LETS_MAKE_IT_OFFICIAL && u.kliuskill >= 1280) || (uwep && uwep->oartifact == ART_CONTROL_HELP) || (uwep && uwep->oartifact == ART_COUNTERSTRIKE_CESTUS) || (uwep && uwep->otyp == CONTRO_STAFF))

#define ControlMagic		(((IntControlMagic && u.nonintrinsicproperty != CONTROL_MAGIC) || (ExtControlMagic && u.nonextrinsicproperty != CONTROL_MAGIC)) && !NoControlMagic)
#define StrongControlMagic	(IntControlMagic && ExtControlMagic && ControlMagic && u.nondoubleproperty != CONTROL_MAGIC)

#define NoControlMagic	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_CONTROL_MAGIC].intrinsic || UHaveAids || (u.impossibleproperty == CONTROL_MAGIC) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HExpBoost		u.uprops[EXP_BOOST].intrinsic
#define EExpBoost		u.uprops[EXP_BOOST].extrinsic
#define IntExpBoost	(HExpBoost)
#define ExtExpBoost	(EExpBoost || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == EXP_BOOST) ) || (uarmf && uarmf->oartifact == ART_NEXT_LEVEL) || have_minimejewel() || (uwep && uwep->oartifact == ART_DOUBLE_MYSTERY) || autismringcheck(ART_RING_OF_FAST_LIVING))

#define ExpBoost		(((IntExpBoost && u.nonintrinsicproperty != EXP_BOOST) || (ExtExpBoost && u.nonextrinsicproperty != EXP_BOOST)) && !NoExpBoost)
#define StrongExpBoost	(IntExpBoost && ExtExpBoost && ExpBoost && u.nondoubleproperty != EXP_BOOST)

#define NoExpBoost	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_EXP_BOOST].intrinsic || UHaveAids || (u.impossibleproperty == EXP_BOOST) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HPainSense		u.uprops[PAIN_SENSE].intrinsic
#define EPainSense		u.uprops[PAIN_SENSE].extrinsic
#define IntPainSense	(HPainSense || (youmonst.data->msound == MS_PAIN) || (dmgtype(youmonst.data, AD_PAIN)) )
#define ExtPainSense	(EPainSense || (uarmf && uarmf->oartifact == ART_CUTE_JEANS) || have_feeldasag() || (uwep && uwep->oartifact == ART_TANKFRIEND) || (uamul && objects[uamul->otyp].oc_color == CLR_BLUE) || (uleft && objects[uleft->otyp].oc_color == CLR_BLUE) || (uright && objects[uright->otyp].oc_color == CLR_BLUE) || (uimplant && objects[uimplant->otyp].oc_color == CLR_BLUE) || (ublindf && objects[ublindf->otyp].oc_color == CLR_BLUE) || (uarmf && objects[uarmf->otyp].oc_color == CLR_BLUE) || (uarmh && objects[uarmh->otyp].oc_color == CLR_BLUE) || (uarmg && objects[uarmg->otyp].oc_color == CLR_BLUE) || (uarmc && objects[uarmc->otyp].oc_color == CLR_BLUE) || (uarmu && objects[uarmu->otyp].oc_color == CLR_BLUE) || (uarms && objects[uarms->otyp].oc_color == CLR_BLUE) || (uarm && objects[uarm->otyp].oc_color == CLR_BLUE) || (uwep && objects[uwep->otyp].oc_color == CLR_BLUE) )

#define PainSense		(((IntPainSense && u.nonintrinsicproperty != PAIN_SENSE) || (ExtPainSense && u.nonextrinsicproperty != PAIN_SENSE)) && !NoPainSense)
#define StrongPainSense	(IntPainSense && ExtPainSense && PainSense && u.nondoubleproperty != PAIN_SENSE)

#define NoPainSense	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_PAIN_SENSE].intrinsic || UHaveAids || (u.impossibleproperty == PAIN_SENSE) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HInvertedState		u.uprops[INVERT_STATE].intrinsic
#define EInvertedState		u.uprops[INVERT_STATE].extrinsic
#define IntInvertedState	(HInvertedState || Role_if(PM_ALLTECHER) || u.alltecherpersist || (Race_if(PM_RETICULAN) && (u.uhp < (u.uhpmax / 5)) ) || (Race_if(PM_RETICULAN) && Upolyd && (u.mh < (u.mhmax / 5)) ) )
#define ExtInvertedState	(EInvertedState || autismweaponcheck(ART_AK_____) )
#define InvertedState		((IntInvertedState || ExtInvertedState) && !(powerfulimplants() && uimplant && uimplant->oartifact == ART_ARABELLA_S_EXCHANGER) )
#define StrongInvertedState		(IntInvertedState && ExtInvertedState && InvertedState)

#define HWinceState		u.uprops[WINCE_STATE].intrinsic
#define EWinceState		u.uprops[WINCE_STATE].extrinsic
#define IntWinceState		(HWinceState || Role_if(PM_ALLTECHER) || u.alltecherpersist || (Race_if(PM_RETICULAN) && (u.uhp < (u.uhpmax / 2)) ) || (Race_if(PM_RETICULAN) && Upolyd && (u.mh < (u.mhmax / 2)) ) )
#define ExtWinceState		(EWinceState || (uarm && uarm->oartifact == ART_AMMY_S_WAIL) )
#define WinceState		(IntWinceState || ExtWinceState )
#define StrongWinceState		(IntWinceState && ExtWinceState && WinceState)

#define HDefusing		u.uprops[DEFUSING].intrinsic
#define EDefusing		u.uprops[DEFUSING].extrinsic
#define IntDefusing	(HDefusing || (uarmg && objects[uarmg->otyp].oc_material == MT_MENGETIUM) )
#define ExtDefusing	(EDefusing || (uarmf && uarmf->oartifact == ART_GHETTO_ROCKZ) || (uarm && uarm->oartifact == ART_ARMS_OVERSIGHT) || (uwep && uwep->oartifact == ART_GO_FULLY_OFF) || (uarm && uarm->oartifact == ART_ENCHANTED__OF_DEFUSING) || (uarm && uarm->oartifact == ART_BEEPA_DEVICE) || (uwep && uwep->oartifact == ART_HALF_SUPER_ENCHANTMENT) || (uamul && uamul->oartifact == ART_ARVIAT_S_LOAD) || (ublindf && ublindf->otyp == DEFUSING_BOX) || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == DEFUSING) ) )

#define Defusing		(((IntDefusing && u.nonintrinsicproperty != DEFUSING) || (ExtDefusing && u.nonextrinsicproperty != DEFUSING)) && !NoDefusing)
#define StrongDefusing	(IntDefusing && ExtDefusing && Defusing && u.nondoubleproperty != DEFUSING)

#define NoDefusing	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_DEFUSING].intrinsic || UHaveAids || (u.impossibleproperty == DEFUSING) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HBurdenedState		u.uprops[BURDENED_STATE].intrinsic
#define EBurdenedState		u.uprops[BURDENED_STATE].extrinsic
#define IntBurdenedState	(HBurdenedState || issoviet)
#define ExtBurdenedState	(EBurdenedState || (uarms && uarms->oartifact == ART_ITS_NO_GOOD) || autismweaponcheck(ART_INDOBURDENIA_VIRII) || (uamul && uamul->oartifact == ART_EVERY_OTHER_AMULET_IS_THIS) || (uarmg && uarmg->oartifact == ART_SUPERHEAVYKLONK) )
#define BurdenedState		(IntBurdenedState || ExtBurdenedState )
#define StrongBurdenedState		(IntBurdenedState && ExtBurdenedState && BurdenedState)

#define HMagicVacuum		u.uprops[MAGIC_VACUUM].intrinsic
#define EMagicVacuum		u.uprops[MAGIC_VACUUM].extrinsic
#define IntMagicVacuum	(HMagicVacuum || Role_if(PM_ALLTECHER) || u.alltecherpersist )
#define ExtMagicVacuum	(EMagicVacuum || autismringcheck(ART_SPEECHBREAK) || autismweaponcheck(ART_SHADOWBLADE_BASED_ON_STORM) || autismringcheck(ART_ARABELLA_S_NASTYGUARD) || autismweaponcheck(ART_MCCAULEY_S_ARGUMENT) || (uarmc && uarmc->oartifact == ART_OLD_PERSON_TALK) || autismweaponcheck(ART_ARABELLA_S_THINNER) || (uwep && uwep->otyp == VACUUM_STAFF) || (u.twoweap && uswapwep && uswapwep->otyp == VACUUM_STAFF) )
#define MagicVacuum		(IntMagicVacuum || ExtMagicVacuum )
#define StrongMagicVacuum		(IntMagicVacuum && ExtMagicVacuum && MagicVacuum)

#define HResistancePiercing		u.uprops[RESISTANCE_PIERCING].intrinsic
#define EResistancePiercing		u.uprops[RESISTANCE_PIERCING].extrinsic
#define IntResistancePiercing	(HResistancePiercing || (uarmu && uarmu->oartifact == ART_HA_HA_HA_HA___) )
#define ExtResistancePiercing	(EResistancePiercing || (uwep && uwep->oartifact == ART_USELESS_TALK) || (uarmu && uarmu->oartifact == ART_HA_HA_HA_HA___) || (uarm && uarm->oartifact == ART_ALTADOON_HERMA_MORA) || (uwep && uwep->oartifact == ART_MR__OF_HIGH_HEELS) || (uwep && uwep->oartifact == ART_ETHER_DAGGER) || (uwep && uwep->oartifact == ART_DAGGER_SURROUNDED_BY_WIND) || (uarmh && uarmh->oartifact == ART_DAN_THE_MAGE) || (uarmg && uarmg->oartifact == ART_SPELLSNIPE) || (uarmf && uarmf->oartifact == ART_SMILE_Z) || (uarmh && uarmh->oartifact == ART_DREHTURN) || (uarmg && uarmg->oartifact == ART_DATLEST_KRANN) || (uarms && uarms->oartifact == ART_TARTSCH) || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == RESISTANCE_PIERCING) ) )

#define ResistancePiercing		(((IntResistancePiercing && u.nonintrinsicproperty != RESISTANCE_PIERCING) || (ExtResistancePiercing && u.nonextrinsicproperty != RESISTANCE_PIERCING)) && !NoResistancePiercing)
#define StrongResistancePiercing	(IntResistancePiercing && ExtResistancePiercing && ResistancePiercing && u.nondoubleproperty != RESISTANCE_PIERCING)

#define NoResistancePiercing	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_RESISTANCE_PIERCING].intrinsic || UHaveAids || (u.impossibleproperty == RESISTANCE_PIERCING ) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HFuckOverEffect		u.uprops[FUCK_OVER].intrinsic
#define EFuckOverEffect		u.uprops[FUCK_OVER].extrinsic
#define IntFuckOverEffect	(HFuckOverEffect || (u.startscummerpersist && u.ulevel >= 20) || (Race_if(PM_STARTSCUMMER) && u.ulevel >= 20) )
#define ExtFuckOverEffect	(EFuckOverEffect || (u.startscummerpersist && u.ulevel >= 20) || (Race_if(PM_STARTSCUMMER) && u.ulevel >= 20) || (uamul && uamul->oartifact == ART_NUCLEAR_WORTH) || (uarmh && uarmh->oartifact == ART_HOW_CAN_ONE_PLEASE_LOOK_LI) || autismringcheck(ART_ARABELLA_S_NASTYGUARD) || autismweaponcheck(ART_SKODIT) || (uarmu && uarmu->oartifact == ART_ALL_IN_ONE_EFF) || autismweaponcheck(ART_NOTHING_FOR_IT) || Race_if(PM_RODNEYAN) || (uarm && uarm->oartifact == ART_WELL_FUCK) || (uarmc && uarmc->oartifact == ART_SHROUD) )

#define FuckOverEffect		(IntFuckOverEffect || ExtFuckOverEffect)
#define StrongFuckOverEffect		(IntFuckOverEffect && ExtFuckOverEffect && FuckOverEffect)

#define HMysteryResist		u.uprops[MYSTERY_RES].intrinsic
#define EMysteryResist		u.uprops[MYSTERY_RES].extrinsic
#define IntMysteryResist	(HMysteryResist || have_doubleshelljewel() || (uwep && uwep->oartifact == ART_TONA_S_GAMES && (PlayerInConeHeels || PlayerInStilettoHeels) ) || (uarm && uarm->oartifact == ART_KUSE_MUSE) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_TOTAL_PARRY_GAUCHE) )
#define ExtMysteryResist	(EMysteryResist || have_doubleshelljewel() || (u.twoweap && uswapwep && uswapwep->oartifact == ART_TOTAL_PARRY_GAUCHE) || (uwep && uwep->otyp == MYSTERIOUS_PICK) || (uwep && uwep->oartifact == ART_MYSTERESIS_X) || (uwep && uwep->oartifact == ART_HYSTERESIS_X) || (uwep && uwep->oartifact == ART_FERRUM_D) || (uwep && uwep->oartifact == ART_LYST_ERG) || (uarmf && uarmf->oartifact == ART_SASSY_JULIA) || (uarmf && uarmf->oartifact == ART_ENDARKEN_EVERYTHING) || (uarmf && uarmf->oartifact == ART_PLAY_THE_GAME_YOURSELF) || (uarmu && uarmu->oartifact == ART_ALL_IN_ONE_EFF) || (uarm && uarm->oartifact == ART_OOOOOOH) || (uarmf && uarmf->oartifact == ART_MAGIC_BUTTERFLY) || (uarm && uarm->oartifact == ART_YOU_REALLY_HAVE_A_TOTAL_DA) || (uwep && uwep->oartifact == ART_BLOCK_THE_MYST) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_SONIC_TONFA) || (uarms && uarms->oartifact == ART_GONGDE_ERTAL) || (uamul && uamul->oartifact == ART_GIBE_DE_MYST) || (uwep && uwep->oartifact == ART_TONA_S_GAMES && (PlayerInConeHeels || PlayerInStilettoHeels) ) || (have_oneinfourstone() && (moves % 4 == 0) ) || (uarm && uarm->oartifact == ART_ELROS_EAERFALAS) || (uarm && uarm->oartifact == ART_OKAY__YOU_WIN_) || have_haveitallstone() || (uarmh && uarmh->oartifact == ART_ANDEN_DRER) || (uarmc && uarmc->oartifact == ART_DOUBLE_PLUS_GOOD) || (flags.female && uarmf && uarmf->oartifact == ART_NANCY_S_ASSISTANCE) || (uarm && uarm->oartifact == ART_JEANETTA_S_REVENGE) || (uarm && uarm->oartifact == ART_FULL_DOSE_LIFE) || (uarmc && uarmc->oartifact == ART_POW_RER) || (uarm && uarm->oartifact == ART_KUSE_MUSE) || (uarmf && uarmf->oartifact == ART_GREEN_MYSTERY) || (uarm && uarm->oartifact == ART_RAND_ARM) || (uwep && uwep->oartifact == ART_SAMA_KATA) || (uarm && uarm->oartifact == ART_ESPECIALLYRANDARM) || (uarm && uarm->oartifact == ART_GRANT_ESPECIAL) || (uarm && uarm->oartifact == ART_AMMY_S_NORMALNESS) || (uarm && uarm->oartifact == ART_TEH_NORM) || (uarm && uarm->oartifact == ART_RES_CIRCLE) || (uarmg && uarmg->oartifact == ART_IDEA_IN_HEADBACK) || (uarmf && uarmf->oartifact == ART_SUPER_PEEP_TOES) || (uwep && uwep->otyp == MAGISWORD) || (uarm && uarm->oartifact == ART_VERY_SEXUAL) || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == MYSTERY_RES) ) )

#define MysteryResist		(((IntMysteryResist && u.nonintrinsicproperty != MYSTERY_RES) || (ExtMysteryResist && u.nonextrinsicproperty != MYSTERY_RES)) && !NoMysteryResist)
#define StrongMysteryResist	(IntMysteryResist && ExtMysteryResist && MysteryResist && u.nondoubleproperty != MYSTERY_RES)

#define NoMysteryResist	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_MYSTERY_RES].intrinsic || UHaveAids || (u.impossibleproperty == MYSTERY_RES ) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HMagicFindBonus		u.uprops[MAGIC_FIND].intrinsic
#define EMagicFindBonus		u.uprops[MAGIC_FIND].extrinsic
#define IntMagicFindBonus	(HMagicFindBonus || (uarms && uarms->oartifact == ART_DNETHACKC_NOONISHNESS) || (uwep && uwep->oartifact == ART_FORGE_HAMMER_OF_THE_ARTIFI) || (uarmf && uarmf->oartifact == ART_NATALIA_S_GREAT_LUCK) || (uarmc && uarmc->oartifact == ART_VICTORIAN_UNDERWEAR_OF_THE) || (uarms && uarms->oartifact == ART_GIT_ALL_DE_ARTS_NO) )
#define ExtMagicFindBonus	(EMagicFindBonus || (uarms && uarms->oartifact == ART_DNETHACKC_NOONISHNESS) || (uwep && uwep->oartifact == ART_FORGE_HAMMER_OF_THE_ARTIFI) || (uarmc && uarmc->oartifact == ART_VICTORIAN_UNDERWEAR_OF_THE) || (uarmu && uarmu->oartifact == ART_ALL_IN_ONE_EFF) || (uarmh && uarmh->oartifact == ART_DLANGOUT) || (uarms && uarms->oartifact == ART_UNUSUAL_ENCH) || (uarmf && uarmf->oartifact == ART_LOVELY_CORNFLOWER) || (uarmf && uarmf->oartifact == ART_NATALIA_S_GREAT_LUCK) || (uamul && uamul->oartifact == ART_ARVIAT_S_LOAD) || (uarm && uarm->oartifact == ART_NATAS_IS_BACK) || (uarm && uarm->oartifact == ART_OUTMINE_OF_METAL) || (uwep && uwep->oartifact == ART_GIVE_US_A_NAME) || autismringcheck(ART_FYRYONI) || (uarm && uarm->oartifact == ART_FORTUTIOUSNESS_SIC_) || (uwep && uwep->oartifact == ART_MEAT_SCRAPER) || (uarmc && uarmc->oartifact == ART_DEEPFOUND) || autismringcheck(ART_LOOKIE_LOOKIE) || (uarmc && uarmc->oartifact == ART_FINDEET) || (uarms && uarms->oartifact == ART_GIT_ALL_DE_ARTS_NO) || (uwep && uwep->oartifact == ART_MR__OF_HIGH_HEELS) || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == MAGIC_FIND) ) )

#define MagicFindBonus		(((IntMagicFindBonus && u.nonintrinsicproperty != MAGIC_FIND) || (ExtMagicFindBonus && u.nonextrinsicproperty != MAGIC_FIND)) && !NoMagicFindBonus)
#define StrongMagicFindBonus	(IntMagicFindBonus && ExtMagicFindBonus && MagicFindBonus && u.nondoubleproperty != MAGIC_FIND)

#define NoMagicFindBonus	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_MAGIC_FIND].intrinsic || UHaveAids || (u.impossibleproperty == MAGIC_FIND ) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HSpellboost		u.uprops[SPELLBOOST].intrinsic
#define ESpellboost		u.uprops[SPELLBOOST].extrinsic
#define IntSpellboost	(HSpellboost || (uarm && uarm->oartifact == ART_THREADWEAVER_S_BLESSING) || (uwep && uwep->oartifact == ART_IRON_REINFORCEMENT) || (uarmg && uarmg->oartifact == ART_WAND_INTO_SPELL) || (uarm && uarm->oartifact == ART_SPECIAL_ROBUNG) )
#define ExtSpellboost	(ESpellboost || (uarm && uarm->oartifact == ART_THREADWEAVER_S_BLESSING) || (uwep && uwep->oartifact == ART_IRON_REINFORCEMENT) || (uarmg && uarmg->oartifact == ART_WAND_INTO_SPELL) || (uarm && uarm->oartifact == ART_YOU_REALLY_HAVE_A_TOTAL_DA) || (uwep && uwep->oartifact == ART_MAGE_STAFF_OF_ETERNITY) || (uwep && uwep->oartifact == ART_CASTOR_BOOST) || (uwep && uwep->oartifact == ART_SCYTHE_OF_VOID) || (uwep && uwep->oartifact == ART_SCYTHE_THAT_SINGS_DEATH) || (uwep && uwep->oartifact == ART_SHIVERING_STAFF) || (uwep && uwep->oartifact == ART_STAFF_OF_INSANITY) || (uarmf && uarmf->oartifact == ART_FRESH_PINK) || (uwep && uwep->oartifact == ART_HALT_SHAMAN) || (uarm && uarm->oartifact == ART_CHAOSWEAVER_ROBES) || (uwep && uwep->oartifact == ART_SAMA_KATA) || (uwep && uwep->oartifact == ART_SCHWI_SCHWI) || autismringcheck(ART_NEAR_OVERLOOK) || (uarmh && uarmh->oartifact == ART_HEAD_W) || (uarms && uarms->oartifact == ART_GONGDE_ERTAL) || (uwep && uwep->oartifact == ART_FOOTPRINTS_IN_THE_LABYRINT) || (uwep && uwep->oartifact == ART_CALLANDOR) || (uarmu && uarmu->oartifact == ART_ALL_IN_ONE_EFF) || (uarmh && uarmh->oartifact == ART_DREHTURN) || (uarmh && uarmh->oartifact == ART_CAAAAAST) || (uarm && uarm->oartifact == ART_SPECIAL_ROBUNG) || (uarm && uarm->oartifact == ART_HEART_INFACT) || (uarm && uarm->oartifact == ART_CHIP_INFACT) || (uarm && uarm->oartifact == ART_ETERNAL_BAMMELING) || (uarmh && uarmh->oartifact == ART_DAN_THE_MAGE) || (uwep && uwep->otyp == CASTER_STAFF) || (powerfulimplants() && uimplant && (goodimplanteffect(uimplant) == SPELLBOOST) ) )

#define Spellboost		(((IntSpellboost && u.nonintrinsicproperty != SPELLBOOST) || (ExtSpellboost && u.nonextrinsicproperty != SPELLBOOST)) && !NoSpellboost)
#define StrongSpellboost	(IntSpellboost && ExtSpellboost && Spellboost && u.nondoubleproperty != SPELLBOOST)

#define NoSpellboost	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SPELLBOOST].intrinsic || UHaveAids || (u.impossibleproperty == SPELLBOOST ) || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#endif /* YOUPROP_H */
