/*	SCCS Id: @(#)mondata.h	3.4	2003/01/08	*/
/* Copyright (c) 1989 Mike Threepoint				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef MONDATA_H
#define MONDATA_H

#define verysmall(ptr)		((ptr)->msize < MZ_SMALL)
#define bigmonst(ptr)		((ptr)->msize >= MZ_LARGE)

#define pm_resistance(ptr,typ)	(((ptr)->mresists & (typ)) != 0)

#define resists_fire(mon)	(((mon)->mintrinsics & MR_FIRE) != 0)
#define resists_cold(mon)	(((mon)->mintrinsics & MR_COLD) != 0)
#define resists_sleep(mon)	(((mon)->mintrinsics & MR_SLEEP) != 0)
#define resists_disint(mon)	(((mon)->mintrinsics & MR_DISINT) != 0)
#define resists_elec(mon)	(((mon)->mintrinsics & MR_ELEC) != 0)
#define resists_poison(mon)	(((mon)->mintrinsics & MR_POISON) != 0)
#define resists_acid(mon)	(((mon)->mintrinsics & MR_ACID) != 0)
#define resists_ston(mon)	(((mon)->mintrinsics & MR_STONE) != 0)

#define resists_drain(mon)      (((mon)->mintrinsics & MR_DRAIN) != 0)
#define resists_death(mon)      (((mon)->mintrinsics & MR_DEATH) != 0)

#define need_one(mon)           (((mon)->mintrinsics & MR_PLUSONE) != 0)
#define need_two(mon)           (((mon)->mintrinsics & MR_PLUSTWO) != 0)
#define need_three(mon)         (((mon)->mintrinsics & MR_PLUSTHREE) != 0)
#define need_four(mon)          (((mon)->mintrinsics & MR_PLUSFOUR) != 0)
#define hit_as_one(mon)         (((mon)->mintrinsics & MR_HITASONE) != 0)
#define hit_as_two(mon)         (((mon)->mintrinsics & MR_HITASTWO) != 0)
#define hit_as_three(mon)       (((mon)->mintrinsics & MR_HITASTHREE) != 0)
#define hit_as_four(mon)        (((mon)->mintrinsics & MR_HITASFOUR) != 0)

#define is_lminion(mon)		(is_minion((mon)->data) && \
				 (mon)->data->maligntyp >= A_COALIGNED && \
				 ((mon)->data != &mons[PM_ANGEL] || \
				  EPRI(mon)->shralign > 0))

#define is_fire_resistant(ptr)		(((ptr)->mresists & MR_FIRE) != 0L)
#define is_cold_resistant(ptr)		(((ptr)->mresists & MR_COLD) != 0L)
#define is_sleep_resistant(ptr)		(((ptr)->mresists & MR_SLEEP) != 0L)
#define is_disint_resistant(ptr)		(((ptr)->mresists & MR_DISINT) != 0L)
#define is_elec_resistant(ptr)		(((ptr)->mresists & MR_ELEC) != 0L)
#define is_poison_resistant(ptr)		(((ptr)->mresists & MR_POISON) != 0L)
#define is_acid_resistant(ptr)		(((ptr)->mresists & MR_ACID) != 0L)
#define is_ston_resistant(ptr)		(((ptr)->mresists & MR_STONE) != 0L)
#define is_drain_resistant(ptr)		(((ptr)->mresists & MR_DRAIN) != 0L)
#define is_death_resistant(ptr)		(((ptr)->mresists & MR_DEATH) != 0L)

#define does_need_one(ptr)           (((mon)->mresists & MR_PLUSONE) != 0)
#define does_need_two(ptr)           (((mon)->mresists & MR_PLUSTWO) != 0)
#define does_need_three(ptr)         (((mon)->mresists & MR_PLUSTHREE) != 0)
#define does_need_four(ptr)          (((mon)->mresists & MR_PLUSFOUR) != 0)
#define does_hit_as_one(ptr)         (((mon)->mresists & MR_HITASONE) != 0)
#define does_hit_as_two(ptr)         (((mon)->mresists & MR_HITASTWO) != 0)
#define does_hit_as_three(ptr)       (((mon)->mresists & MR_HITASTHREE) != 0)
#define does_hit_as_four(ptr)        (((mon)->mresists & MR_HITASFOUR) != 0)

#define is_flyer(ptr)		(((ptr)->mflags1 & M1_FLY) != 0L)
#define is_floater(ptr)		((ptr)->mlet == S_EYE)
#define is_clinger(ptr)		(((ptr)->mflags1 & M1_CLING) != 0L)
#define is_swimmer(ptr)		(((ptr)->mflags1 & M1_SWIM) != 0L)
#define breathless(ptr)		(((ptr)->mflags1 & M1_BREATHLESS) != 0L)
#define amphibious(ptr)		(((ptr)->mflags1 & (M1_AMPHIBIOUS | M1_BREATHLESS)) != 0L)
#define passes_walls(ptr)	(((ptr)->mflags1 & M1_WALLWALK) != 0L)
#define amorphous(ptr)		(((ptr)->mflags1 & M1_AMORPHOUS) != 0L)
#define noncorporeal(ptr)	((ptr)->mlet == S_GHOST)
#define tunnels(ptr)		(((ptr)->mflags1 & M1_TUNNEL) != 0L)
#define needspick(ptr)		(((ptr)->mflags1 & M1_NEEDPICK) != 0L)
#define hides_under(ptr)	(((ptr)->mflags1 & M1_CONCEAL) != 0L)
#define is_hider(ptr)		(((ptr)->mflags1 & M1_HIDE) != 0L)
#define haseyes(ptr)		(((ptr)->mflags1 & M1_NOEYES) == 0L)
#define eyecount(ptr)		(!haseyes(ptr) ? 0 : \
				 ((ptr) == &mons[PM_CYCLOPS] || \
				  (ptr) == &mons[PM_FLOATING_EYE]) ? 1 : 2)
#define nohands(ptr)		(((ptr)->mflags1 & M1_NOHANDS) != 0L)
#define nolimbs(ptr)		(((ptr)->mflags1 & M1_NOLIMBS) == M1_NOLIMBS)
#define notake(ptr)		(((ptr)->mflags1 & M1_NOTAKE) != 0L)
#define has_head(ptr)		(((ptr)->mflags1 & M1_NOHEAD) == 0L)
#define has_horns(ptr)		(num_horns(ptr) > 0)
#define is_whirly(ptr)		((ptr)->mlet == S_VORTEX || \
				 (ptr) == &mons[PM_AIR_ELEMENTAL] || (ptr) == &mons[PM_GREATER_AIR_ELEMENTAL] || (ptr) == &mons[PM_PETTY_AIR_ELEMENTAL])
#define is_fire(ptr)		((ptr) == &mons[PM_FIRE_VORTEX] || (ptr) == &mons[PM_FIRE_GOLEM] || \
				 (ptr) == &mons[PM_FIRE_ELEMENTAL] || (ptr) == &mons[PM_GREATER_FIRE_ELEMENTAL])
#define flaming(ptr)		((ptr) == &mons[PM_FIRE_VORTEX] || \
				 (ptr) == &mons[PM_FLAMING_SPHERE] || \
				 (ptr) == &mons[PM_FIRE_ELEMENTAL] || \
				 (ptr) == &mons[PM_GREATER_FIRE_ELEMENTAL] || \
				 (ptr) == &mons[PM_CHARMANDER] || (ptr) == &mons[PM_CHARMELEON] || (ptr) == &mons[PM_FIRE_GOLEM] || (ptr) == &mons[PM_CHARIZARD] || (ptr) == &mons[PM_SALAMANDER])
#define is_silent(ptr)		((ptr)->msound == MS_SILENT)
#define unsolid(ptr)		(((ptr)->mflags1 & M1_UNSOLID) != 0L)
#define mindless(ptr)		(((ptr)->mflags1 & M1_MINDLESS) != 0L)
#define humanoid(ptr)		(((ptr)->mflags1 & M1_HUMANOID) != 0L)
#define is_animal(ptr)		(((ptr)->mflags1 & M1_ANIMAL) != 0L)
#define slithy(ptr)		(((ptr)->mflags1 & M1_SLITHY) != 0L)
#define is_wooden(ptr)		((ptr) == &mons[PM_WOOD_GOLEM])
#define thick_skinned(ptr)	(((ptr)->mflags1 & M1_THICK_HIDE) != 0L)
#define lays_eggs(ptr)		(((ptr)->mflags1 & M1_OVIPAROUS) != 0L)
#define regenerates(ptr)	(((ptr)->mflags1 & M1_REGEN) != 0L)
#define perceives(ptr)		(((ptr)->mflags1 & M1_SEE_INVIS) != 0L)
#define can_teleport(ptr)	(((ptr)->mflags1 & M1_TPORT) != 0L)
#define control_teleport(ptr)	(((ptr)->mflags1 & M1_TPORT_CNTRL) != 0L)
#define telepathic(ptr)		((ptr) == &mons[PM_FLOATING_EYE] || (ptr) == &mons[PM_SWEEPING_EYE] || (ptr) == &mons[PM_FLOATING_MERMAN] || (ptr) == &mons[PM_TELEPATHIC_EYE] || (ptr) == &mons[PM_PSION] || (ptr) == &mons[PM_LEVITATOR] || (ptr) == &mons[PM_UNDEAD_PSION] || \
				 is_mind_flayer(ptr))
#define is_armed(ptr)		(attacktype(ptr, AT_WEAP) || (ptr) == &mons[PM_ANIMATED_WEDGE_SANDAL] || (ptr) == &mons[PM_ANIMATED_SEXY_LEATHER_PUMP] || (ptr) == &mons[PM_ANIMATED_LEATHER_PEEP_TOE] || (ptr) == &mons[PM_ANIMATED_COMBAT_STILETTO])
#define acidic(ptr)		(((ptr)->mflags1 & M1_ACID) != 0L)
#define poisonous(ptr)		(((ptr)->mflags1 & M1_POIS) != 0L)
#define carnivorous(ptr)	(((ptr)->mflags1 & M1_CARNIVORE) != 0L)
#define herbivorous(ptr)	(((ptr)->mflags1 & M1_HERBIVORE) != 0L)
#define metallivorous(ptr)	(((ptr)->mflags1 & M1_METALLIVORE) != 0L)
#define monpolyok(ptr)		(((ptr)->mflags2 & M2_NOPOLY) == 0L) /* monsters may poly into this */
#define polyok(ptr)		(((ptr)->mflags2 & M2_NOPOLY) == 0L && (ptr)->mlevel < 30 && (ptr) != &mons[PM_LEPRECHAUN_KING] && (ptr) != &mons[PM_NYMPH_QUEEN] && (ptr) != &mons[PM_MIGO_QUEEN] && (ptr) != &mons[PM_MIGO_EMPRESS] && (ptr) != &mons[PM_QUEEN_BEE] && (ptr) != &mons[PM_ONG_SEPHIRAH] && (ptr) != &mons[PM_KRONG_SEPHIRAH] && (ptr) != &mons[PM_ZAKRONG_SEPHIRAH] && (ptr) != &mons[PM_CATOBLEPAS] && (ptr) != &mons[PM_RAINBOW_MODE_DRAGON] && (ptr) != &mons[PM_MULTICOLOR_GRUE] && (ptr) != &mons[PM_COLORLESS_MOLD] && (ptr) != &mons[PM_COLORLESS_FUNGUS] && (ptr) != &mons[PM_SIZZLING_VORTEX] && (ptr) != &mons[PM_SHINING_PIERCER] && (ptr) != &mons[PM_DEEP_THOUGHT] && (ptr) != &mons[PM_EDDIE] && (ptr) != &mons[PM_SHADE] && (ptr) != &mons[PM_ROBOT] && (ptr) != &mons[PM_SHADOW_WARRIOR] && (ptr) != &mons[PM_ANCIENT_MULTI_HUED_DRAGON] && (ptr) != &mons[PM_DRAGON_LORD] && (ptr) != &mons[PM_VORPAL_JABBERWOCK] && (ptr) != &mons[PM_KOP_KATCHER] && (ptr) != &mons[PM_BOOJUM] && (ptr) != &mons[PM_HELLFIRE_COLOSSUS] && (ptr) != &mons[PM_POLICEMAN_KATCHER] && (ptr) != &mons[PM_OFFICER_KATCHER] && (ptr) != &mons[PM_WARDER_KATCHER] && (ptr) != &mons[PM_DETECTIVE_KATCHER] && (ptr) != &mons[PM_PROSTETNIK_VOGON_JELTZ] && (ptr) != &mons[PM_MARVIN] && (ptr) != &mons[PM_MASTER_ARCHON] && (ptr) != &mons[PM_GRANDMASTER_ARCHON] && (ptr) != &mons[PM_AK_THIEF_IS_DEAD_] && (ptr) != &mons[PM_UN_IN_PROTECT_MODE] && (ptr) != &mons[PM_ZAPHOD_BREEBLEBROX] && (ptr) != &mons[PM_SIZZLE] && (ptr) != &mons[PM_PLANETAR] && (ptr) != &mons[PM_UNDEAD_KATCHER] && (ptr) != &mons[PM_MULTICOLOR_GRUE] && (ptr) != &mons[PM_LOW_PRIEST] && (ptr) != &mons[PM_WHITE_MARKETEER] && (ptr) != &mons[PM_TRACKER_JACKER] && (ptr) != &mons[PM_MYSTIC_EYE] && (ptr) != &mons[PM_INSPECTOR_KATCHER] && (ptr) != &mons[PM_UNIFORM_KATCHER] && (ptr) !=&mons[PM_NITROHACK_HORROR] && (ptr) != &mons[PM_SPEEDHACK_HORROR] && (ptr) != &mons[PM_NETHACKFOUR_HORROR] && (ptr) != &mons[PM_DEVTEAM_HORROR] && (ptr) != &mons[PM_DNETHACK_HORROR] && (ptr) != &mons[PM_NETHACKBRASS_HORROR] && (ptr) != &mons[PM_INTERHACK_HORROR] && (ptr) != &mons[PM_NHTNG_HORROR] && (ptr) != &mons[PM_UNNETHACK_HORROR] && (ptr) != &mons[PM_UNNETHACKPLUS_HORROR] && (ptr) != &mons[PM_ANGBAND_HORROR] && (ptr) != &mons[PM_ADOM_HORROR] && (ptr) != &mons[PM_PETTY_ANGBAND_HORROR] && (ptr) != &mons[PM_PETTY_ADOM_HORROR] && (ptr) != &mons[PM_SPORKHACK_HORROR] && (ptr) != &mons[PM_SLASHEM_HORROR] && (ptr) != &mons[PM_NETHACK_HORROR] && (ptr) != &mons[PM_ROGUE_HORROR] && (ptr) != &mons[PM_GRUNTHACK_HORROR] && (ptr) != &mons[PM_ACEHACK_HORROR] && (ptr) != &mons[PM_MAIDENHACK_HORROR] && (ptr) != &mons[PM_YASD_HORROR] && (ptr) != &mons[PM_PETTY_GRUNTHACK_HORROR] && (ptr) != &mons[PM_PETTY_ACEHACK_HORROR] && (ptr) != &mons[PM_STONE_COLD_HORROR] && (ptr) != &mons[PM_PETROGRAPHY_HORROR] && (ptr) != &mons[PM_PUPURIN] && (ptr) != &mons[PM_SAPUSAUR] && (ptr) != &mons[PM_ODDOSHISHI] && (ptr) != &mons[PM_TSUBOTSUBO] && (ptr) != &mons[PM_OKUTAN] && (ptr) != &mons[PM_RATICLAW] && (ptr) != &mons[PM_PSYBUR] && (ptr) != &mons[PM_HARISEN] && (ptr) != &mons[PM_SUIKUN] && (ptr) != &mons[PM_HOUOU] && (ptr) != &mons[PM_LOCUSTOD] && (ptr) != &mons[PM_FORETOSU] && (ptr) != &mons[PM_CHARCOLT] && (ptr) != &mons[PM_MILLENUM] && (ptr) != &mons[PM_BLACK_MISSINGNO] && (ptr) != &mons[PM_GRAY_MISSINGNO] && (ptr) != &mons[PM_WHITE_MISSINGNO] && (ptr) != &mons[PM_RED_MISSINGNO] && (ptr) != &mons[PM_BROWN_MISSINGNO] && (ptr) != &mons[PM_CYAN_MISSINGNO] && (ptr) != &mons[PM_MAGENTA_MISSINGNO] && (ptr) != &mons[PM_GREEN_MISSINGNO] && (ptr) != &mons[PM_ORANGE_MISSINGNO] && (ptr) != &mons[PM_YELLOW_MISSINGNO] && (ptr) != &mons[PM_BRIGHT_GREEN_MISSINGNO] && (ptr) != &mons[PM_BRIGHT_MAGENTA_MISSINGNO] && (ptr) != &mons[PM_BRIGHT_BLUE_MISSINGNO] && (ptr) != &mons[PM_BRIGHT_CYAN_MISSINGNO] ) /* players may poly into this */
#define is_undead(ptr)		(((ptr)->mflags2 & M2_UNDEAD) != 0L)
#define is_were(ptr)		(((ptr)->mflags2 & M2_WERE) != 0L)
#define is_vampire(ptr)		(((ptr)->mflags2 & M2_VAMPIRE) != 0L)
#define is_elf(ptr)		(((ptr)->mflags2 & M2_ELF) != 0L)
#define is_dwarf(ptr)		(((ptr)->mflags2 & M2_DWARF) != 0L)
#define is_gnome(ptr)		(((ptr)->mflags2 & M2_GNOME) != 0L)
#define is_orc(ptr)		(((ptr)->mflags2 & M2_ORC) != 0L)
#define is_human(ptr)		(((ptr)->mflags2 & M2_HUMAN) != 0L)
#define is_hobbit(ptr)		(((ptr)->mflags2 & M2_HOBBIT) != 0L)
#define your_race(ptr)		(((ptr)->mflags2 & urace.selfmask) != 0L)
#define is_bat(ptr)		((ptr) == &mons[PM_BAT] || \
				 (ptr) == &mons[PM_GIANT_BAT] || \
				 (ptr) == &mons[PM_LARGE_BAT] || \
				 (ptr) == &mons[PM_ZUBAT] || (ptr) == &mons[PM_GOLBAT] || (ptr) == &mons[PM_CROBAT] || \
				 (ptr) == &mons[PM_HUGE_BAT] || \
				 (ptr) == &mons[PM_ATHOL] || \
				 (ptr) == &mons[PM_RHUMBAT] || \
				 (ptr) == &mons[PM_PETTY_RHUMBAT] || \
				 (ptr) == &mons[PM_HELLBAT] || \
				 (ptr) == &mons[PM_MONGBAT] || \
				 (ptr) == &mons[PM_MOBAT] || \
				 (ptr) == &mons[PM_HARPY] || \
				 (ptr) == &mons[PM_BYAKHEE] || \
				 (ptr) == &mons[PM_GHOST_BAT] || \
				 (ptr) == &mons[PM_NIGHTGAUNT] || \
				 (ptr) == &mons[PM_VAMPIRE_BAT])
#define is_bird(ptr)		((ptr)->mlet == S_BAT && !is_bat(ptr))
#define is_giant(ptr)		(((ptr)->mflags2 & M2_GIANT) != 0L)
#define is_golem(ptr)		((ptr)->mlet == S_GOLEM)
#define is_domestic(ptr)	(((ptr)->mflags2 & M2_DOMESTIC) != 0L)
#define is_demon(ptr)		(((ptr)->mflags2 & M2_DEMON) != 0L)
#define is_mercenary(ptr)	(((ptr)->mflags2 & M2_MERC) != 0L)
#define is_male(ptr)		(((ptr)->mflags2 & M2_MALE) != 0L)
#define is_female(ptr)		(((ptr)->mflags2 & M2_FEMALE) != 0L)
#define is_neuter(ptr)		(((ptr)->mflags2 & M2_NEUTER) != 0L)
#define is_wanderer(ptr)	(((ptr)->mflags2 & M2_WANDER) != 0L)
#define always_hostile(ptr)	(((ptr)->mflags2 & M2_HOSTILE) != 0L)
#define always_peaceful(ptr)	(((ptr)->mflags2 & M2_PEACEFUL) != 0L)
#define race_hostile(ptr)	(((ptr)->mflags2 & urace.hatemask) != 0L)
#define race_peaceful(ptr)	(((ptr)->mflags2 & urace.lovemask) != 0L)
#define extra_nasty(ptr)	(((ptr)->mflags2 & M2_NASTY) != 0L)
#define strongmonst(ptr)	(((ptr)->mflags2 & M2_STRONG) != 0L)
#define can_breathe(ptr)	attacktype(ptr, AT_BREA)
#define cantwield(ptr)		(nohands(ptr) || verysmall(ptr) || \
				 (ptr)->mlet == S_ANT)
#define could_twoweap(ptr)	((ptr)->mattk[1].aatyp == AT_WEAP && \
			((ptr) != youmonst.data || \
			P_MAX_SKILL(P_TWO_WEAPON_COMBAT) >= P_SKILLED || \
			P_MAX_SKILL(P_TWO_WEAPON_COMBAT) >= P_BASIC && \
			(Race_if(PM_DWARF) || Race_if(PM_HUMAN))))
#define cantweararm(ptr)	(breakarm(ptr) || sliparm(ptr))
#define throws_rocks(ptr)	(((ptr)->mflags2 & M2_ROCKTHROW) != 0L)
#define type_is_pname(ptr)	(((ptr)->mflags2 & M2_PNAME) != 0L)
#define is_lord(ptr)		(((ptr)->mflags2 & M2_LORD) != 0L)
#define is_prince(ptr)		(((ptr)->mflags2 & M2_PRINCE) != 0L)
#define is_ndemon(ptr)		(is_demon(ptr) && \
				 (((ptr)->mflags2 & (M2_LORD|M2_PRINCE)) == 0L))
#define is_dlord(ptr)		(is_demon(ptr) && is_lord(ptr))
#define is_dprince(ptr)		(is_demon(ptr) && is_prince(ptr))
#define is_minion(ptr)		((ptr)->mflags2 & M2_MINION)
#define likes_gold(ptr)		(((ptr)->mflags2 & M2_GREEDY) != 0L)
#define likes_gems(ptr)		(((ptr)->mflags2 & M2_JEWELS) != 0L)
#define likes_objs(ptr)		(((ptr)->mflags2 & M2_COLLECT) != 0L || \
				 is_armed(ptr))
#define likes_magic(ptr)	(((ptr)->mflags2 & M2_MAGIC) != 0L)
#define webmaker(ptr)		((ptr) == &mons[PM_CAVE_SPIDER] || \
				 (ptr) == &mons[PM_RECLUSE_SPIDER] || \
				 (ptr) == &mons[PM_LAND_SPIDER] || \
				 (ptr) == &mons[PM_MUD_SPIDER] || \
				 (ptr) == &mons[PM_SHADOW_RECLUSE] || \
				 (ptr) == &mons[PM_TUNNEL_QUEEN] || \
				 (ptr) == &mons[PM_LARGE_SPIDER] || \
				 (ptr) == &mons[PM_SPIDER] || \
				 (ptr) == &mons[PM_ARACHNID] || \
				 (ptr) == &mons[PM_CAVE_FISHER] || \
				 (ptr) == &mons[PM_WEB_MISTRESS] || \
				 (ptr) == &mons[PM_DARK_SPIDER] || \
				 (ptr) == &mons[PM_WIDOW_OF_DEATH] || \
				 (ptr) == &mons[PM_CHAOS_SPIDER] || \
				 (ptr) == &mons[PM_PHASE_SPIDER] || \
				 (ptr) == &mons[PM_WERESPIDER] || \
				 (ptr) == &mons[PM_CAMPER_ASSHOLE] || \
				 (ptr) == &mons[PM_ISLANDER] || \
				 (ptr) == &mons[PM_SMART_SPIDER] || \
				 (ptr) == &mons[PM_BARKING_SPIDER] || \
				 (ptr) == &mons[PM_DOMESTIC_SPIDER] || \
				 (ptr) == &mons[PM_GIANT_SPIDER] || \
				 (ptr) == &mons[PM_DEATH_WIDOW] || \
				 (ptr) == &mons[PM_SPIDERMAN] || \
				 (ptr) == &mons[PM_TARANTULA] || \
				 (ptr) == &mons[PM_GIANT_TARANTULA] || \
				 (ptr) == &mons[PM_PETTY_GIANT_SPIDER] || \
				 (ptr) == &mons[PM_WERESPIDER] || \
				 (ptr) == &mons[PM_ALL_COMMON_TRAPDOOR_SPIDER] || \
				 (ptr) == &mons[PM_SUPER_TRAPDOOR_SPIDER] || \
				 (ptr) == &mons[PM_USA_PRESIDENT] || \
				 (ptr) == &mons[PM_ARANEA] || \
				 (ptr) == &mons[PM_ELDER_ARANEA] || \
				 (ptr) == &mons[PM_BLACK_WIDOW] || \
				 (ptr) == &mons[PM_PETTY_BLACK_WIDOW] || \
				 (ptr) == &mons[PM_ARACH] || \
				 (ptr) == &mons[PM_SAND_FISHER] || \
				 (ptr) == &mons[PM_SAND_SPIDER] || \
				 (ptr) == &mons[PM_TUNNEL_SPIDER] || \
				 (ptr) == &mons[PM_WOOD_SPIDER] || \
				 (ptr) == &mons[PM_MIRKWOOD_SPIDER] || \
				 (ptr) == &mons[PM_LENG_SPIDER] || \
				 (ptr) == &mons[PM_CARRION_FANG] || \
				 (ptr) == &mons[PM_GREENER_SPIDER] || \
				 (ptr) == &mons[PM_ULTRA_SPIDER] || \
				 (ptr) == &mons[PM_SPIDERLING] || \
				 (ptr) == &mons[PM_ELECTRO_SPIDER] || \
				 (ptr) == &mons[PM_POISON_SPINNER] || \
				 (ptr) == &mons[PM_SPIDER_MAGUS] || \
				 (ptr) == &mons[PM_ICE_SPIDER] || \
				 (ptr) == &mons[PM_CLOCK_FOREST_SPIDER] || \
				 (ptr) == &mons[PM_GIANT_TRAPDOOR_SPIDER])
#define is_unicorn(ptr)		((ptr)->mlet == S_UNICORN && likes_gems(ptr))	/* KMH */
#define is_longworm(ptr)	(((ptr) == &mons[PM_BABY_LONG_WORM]) || \
				 ((ptr) == &mons[PM_LONG_WORM]) || \
				 ((ptr) == &mons[PM_LONG_WORM_TAIL]))
#define is_covetous(ptr)	((ptr)->mflags3 & M3_COVETOUS)
#define infravision(ptr)	((ptr->mflags3 & M3_INFRAVISION))
#define infravisible(ptr)	((ptr->mflags3 & M3_INFRAVISIBLE))
#define can_betray(ptr)		((ptr->mflags3 & M3_TRAITOR))
#define cannot_be_tamed(ptr)	((ptr->mflags3 & M3_NOTAME))
#define avoid_player(ptr)	((ptr->mflags3 & M3_AVOIDER))
#define lithivorous(ptr)	((ptr->mflags3 & M3_LITHIVORE))
#define is_petty(ptr)	((ptr->mflags3 & M3_PETTY))
#define is_pokemon(ptr)	((ptr->mflags3 & M3_POKEMON))
#define is_mplayer(ptr)		(((ptr) >= &mons[PM_ARCHEOLOGIST]) && \
				 ((ptr) <= &mons[PM_WIZARD]))
#define is_umplayer(ptr)		(((ptr) >= &mons[PM_UNDEAD_ARCHEOLOGIST]) && \
				 ((ptr) <= &mons[PM_UNDEAD_WIZARD]))
#define is_rider(ptr)		((ptr) == &mons[PM_DEATH] || \
				 (ptr) == &mons[PM_FAMINE] || \
				 (ptr) == &mons[PM_PESTILENCE])
#define eating_is_fatal(ptr)		((ptr) == &mons[PM_DEATH] || \
				 (ptr) == &mons[PM_FAMINE] || \
				 (ptr) == &mons[PM_BLACK_HOLE_BLOB] || \
				 (ptr) == &mons[PM_PESTILENCE])
#define is_placeholder(ptr)	((ptr) == &mons[PM_ORC] || \
				 (ptr) == &mons[PM_GIANT] || \
				 (ptr) == &mons[PM_ELF] || \
				 (ptr) == &mons[PM_HUMAN])
#define is_nonmoving(ptr) ((ptr)->mmove < 1 || (ptr)->mlet == S_TURRET || (ptr) == &mons[PM_BUS] || (ptr) == &mons[PM_LANDMINE] || (ptr) == &mons[PM_HAND_DRUJ] || (ptr) == &mons[PM_EYE_DRUJ] || (ptr) == &mons[PM_DWELLER_ON_THE_THRESHOLD] || (ptr) == &mons[PM_QUIVER_SLOT] || (ptr) == &mons[PM_SKULL_DRUJ] || (ptr) == &mons[PM_DEATH_SWORD] || (ptr) == &mons[PM_EMPIRE_STATE_BUILDING] || (ptr) == &mons[PM_DIVISION_THIEF] || (ptr) == &mons[PM_DIVISION_JEDI] || (ptr) == &mons[PM_STYGIAN_WATCHER] || (ptr) == &mons[PM_RIVER_STALKER] || (ptr) == &mons[PM_CAR_DRIVING_SUPERTHIEF] || (ptr) == &mons[PM_SUPERTHIEF] || (ptr) == &mons[PM_HUGE_OGRE_THIEF] || (ptr) == &mons[PM_SUPERJEDI] || (ptr) == &mons[PM_DEATH_SPECTRE] || (ptr) == &mons[PM_CRITICALLY_INJURED_JEDI] || (ptr) == &mons[PM_CRITICALLY_INJURED_THIEF] || (ptr) == &mons[PM_WAR_ASSHOLE] || (ptr) == &mons[PM_TRAPPED_SOUL] || (ptr) == &mons[PM_WATER_WATCHER] || (ptr) == &mons[PM_SILENT_WATCHER] || (ptr) == &mons[PM_TRAILER] || (ptr) == &mons[PM_DEMONIC_ENTITY] || (ptr) == &mons[PM_LAVA_GAZER] || (ptr) == &mons[PM_ATHENA_BASHER] || (ptr) == &mons[PM_ATHENA_BLOCKER] || (ptr) == &mons[PM_ATHENA_PROTECTOR] || (ptr) == &mons[PM_ATHENA_GIANT] || (ptr) == &mons[PM_GREY_MUSHROOM_PATCH] || (ptr) == &mons[PM_SHRIEKER_MUSHROOM_PATCH] || (ptr) == &mons[PM_MEMORY_MOSS] || (ptr) == &mons[PM_MAGIC_MUSHROOM_PATCH] || (ptr) == &mons[PM_PURPLE_MUSHROOM_PATCH] || (ptr) == &mons[PM_SPOTTED_MUSHROOM_PATCH] || (ptr) == &mons[PM_YELLOW_MUSHROOM_PATCH] || (ptr) == &mons[PM_CLEAR_MUSHROOM_PATCH] || (ptr) == &mons[PM_ATHENA_GUARDIAN] )
#define is_eel(ptr)	((ptr)->mlet == S_EEL)

/* return TRUE if the monster tends to revive */
#define is_reviver(ptr)		(is_rider(ptr) || (ptr)->mlet == S_FUNGUS && \
				 (ptr) != &mons[PM_LICHEN] || \
				 (ptr)->mlet == S_TROLL || (ptr) == &mons[PM_ROT_WALKER] || (ptr) == &mons[PM_ROT_CONVERTER] || (ptr) == &mons[PM_RED_DEATH] || (ptr) == &mons[PM_LITCH_DEMON] || (ptr) == &mons[PM_UNDEAD_BALROG] || (ptr) == &mons[PM_BONE_DEMON] || (ptr) == &mons[PM_RUST_CONVERTER] || (ptr) == &mons[PM_REANIMATED_HORDE] || (ptr) == &mons[PM_PROWLING_DEAD_POTATO] || (ptr) == &mons[PM_UNHOLY_CORPSE] || (ptr) == &mons[PM_DEFILED_WARRIOR]  )

/* this returns the light's range, or 0 if none; if we add more light emitting
   monsters, we'll likely have to add a new light range field to mons[] */
#define emits_light(ptr)	(((ptr)->mlet == S_LIGHT || \
				  (ptr) == &mons[PM_FIRE_VORTEX]) ? 3 : \
				 ((ptr) == &mons[PM_FIRE_ELEMENTAL]) ? 2 : \
				 ((ptr) == &mons[PM_GREATER_FIRE_ELEMENTAL]) ? 3 : \
				 ((ptr) == &mons[PM_FIRE_VAMPIRE])? 2 : \
				 ((ptr) == &mons[PM_FLAMING_SPHERE]) ? 1 : \
				 ((ptr) == &mons[PM_SHOCKING_SPHERE]) ? 1 : \
				 ((ptr) == &mons[PM_WAX_GOLEM]) ? 1 : 0)
/*	[note: the light ranges above were reduced to 1 for performance...] */
/*  WAC increased to 3 and 2?*/
#define likes_lava(ptr)		(ptr == &mons[PM_FIRE_ELEMENTAL] || ptr == &mons[PM_GREATER_FIRE_ELEMENTAL] || \
				 ptr == &mons[PM_SALAMANDER] || ptr == &mons[PM_FIRE_GOLEM] || ptr == &mons[PM_CHARMANDER] || ptr == &mons[PM_CHARMELEON]  || ptr == &mons[PM_CHARIZARD])
#define pm_invisible(ptr)	((ptr) == &mons[PM_STALKER] || \
				 (ptr) == &mons[PM_BLACK_LIGHT] || \
				 (ptr) == &mons[PM_BLACK_LASER] || \
				 (ptr) == &mons[PM_THE_HIDDEN] || \
				 (ptr) == &mons[PM_INVISIBLE_BADGUY] || \
				 (ptr) == &mons[PM_UNSEEN_POTATO] || \
				 (ptr) == &mons[PM_STONE_STALKER] || \
				 (ptr) == &mons[PM_ILLUSION_WEAVER] || \
				 (ptr) == &mons[PM_PAIN_MASTER] || \
				 (ptr) == &mons[PM_STAR_VAMPIRE])

/* could probably add more */
#define likes_fire(ptr)		((ptr) == &mons[PM_FIRE_VORTEX] || \
				  (ptr) == &mons[PM_FLAMING_SPHERE] || \
				  (ptr) == &mons[PM_FIRE_VAMPIRE] || \
				 likes_lava(ptr))


#ifdef CONVICT
# define is_rat(ptr)		((ptr) == &mons[PM_SEWER_RAT] || \
				 (ptr) == &mons[PM_GIANT_RAT] || \
				 (ptr) == &mons[PM_PACK_RAT] || \
				 (ptr) == &mons[PM_PACKER_RAT] || \
				 (ptr) == &mons[PM_RABBIT] || \
				 (ptr) == &mons[PM_RABID_RABBIT] || \
				 (ptr) == &mons[PM_RABID_RAT] || \
				 (ptr) == &mons[PM_HELLRAT] || \
				 (ptr) == &mons[PM_DOUR_RAT] || \
				 (ptr) == &mons[PM_BLACK_RAT] || \
				 (ptr) == &mons[PM_DISEASED_RAT] || \
				 (ptr) == &mons[PM_VAPOR_RAT] || \
				 (ptr) == &mons[PM_GARGANTUAN_RAT] || \
				 (ptr) == &mons[PM_CHAOS_RAT] || \
				 (ptr) == &mons[PM_RATTATA] || (ptr) == &mons[PM_RATICATE] || \
				 (ptr) == &mons[PM_PIKACHU] || (ptr) == &mons[PM_RAICHU] || \
				 (ptr) == &mons[PM_ENORMOUS_RAT] || \
				 (ptr) == &mons[PM_EEVEE] || /* actually a hare... */\
				 (ptr) == &mons[PM_RODENT_OF_UNUSUAL_SIZE])
#endif /* CONVICT */

#define nonliving(ptr)		(is_golem(ptr) || is_undead(ptr) || \
				 (ptr)->mlet == S_VORTEX || \
				 (ptr) == &mons[PM_MANES] || (ptr) == &mons[PM_SPECTRAL_WARRIOR])

#define touch_petrifies(ptr)	(ptr == &mons[PM_COCKATRICE] || \
				 ptr == &mons[PM_PETTY_COCKATRICE] || \
				 ptr == &mons[PM_DOMESTIC_COCKATRICE] || \
				 ptr == &mons[PM_ARMED_COCKATRICE] || \
				 ptr == &mons[PM_HIDDEN_COCKATRICE] || \
				 ptr == &mons[PM_BASILISK] || \
				 ptr == &mons[PM_DRACOLISK] || \
				 ptr == &mons[PM_GREATER_BASILISK] || \
				 ptr == &mons[PM_CHICKATRICE] || \
				 ptr == &mons[PM_WERECOCKATRICE] || \
				 ptr == &mons[PM_HUMAN_WERECOCKATRICE] || \
				 ptr == &mons[PM_ASPHYNX] || \
				 ptr == &mons[PM_STONING_MONSTER] || \
				 ptr == &mons[PM_GORGON_FLY] || \
				 ptr == &mons[PM_GORGON_BEETLE] || \
				 ptr == &mons[PM_PETRO_CENTIPEDE] || \
				 ptr == &mons[PM_CENTAURTRICE] || \
				 ptr == &mons[PM_PETRO_MOLD] || \
				 ptr == &mons[PM_PETRO_FUNGUS] || \
				 ptr == &mons[PM_COCKTAUR] || \
				 ptr == &mons[PM_PETRO_VORTEX] || \
				 ptr == &mons[PM_GORGON] || \
				 ptr == &mons[PM_RUBBER_CHICKEN] || \
				 ptr == &mons[PM_MIMIC_CHICKEN] || \
				 ptr == &mons[PM_GRATING_CHICKEN] || \
				 ptr == &mons[PM_OLOG_HAI_GORGON] || \
				 ptr == &mons[PM_STONE_COLD_HORROR] || \
				 ptr == &mons[PM_PETROGRAPHY_HORROR] || \
				 ptr == &mons[PM_UNDEAD_COCKATRICE] || \
				 ptr == &mons[PM_MEDUSA_S_PET_FISH] || \
				 ptr == &mons[PM_BLACK_HOLE_BLOB] || \
				 ptr == &mons[PM_PETRO_MIMIC] || \
				 ptr == &mons[PM_PSEUDO_MEDUSA] || \
				 ptr == &mons[PM_OBERON_PRINCIPAL] || \
				 ptr == &mons[PM_OBERON_WORKER] || \
				 ptr == &mons[PM_TURBO_CHICKEN])

#define is_mind_flayer(ptr)	((ptr) == &mons[PM_MIND_FLAYER] || \
				 (ptr) == &mons[PM_PETTY_MIND_FLAYER] || \
				 (ptr) == &mons[PM_KERRISIN] || \
				 (ptr) == &mons[PM_ICKY_FACE] || \
				 (ptr) == &mons[PM_GRANDMASTER_MIND_FLAYER] || \
				 (ptr) == &mons[PM_BRAIN_EATER] || \
				 (ptr) == &mons[PM_WEREMINDFLAYER] || \
				 (ptr) == &mons[PM_HUMAN_WEREMINDFLAYER] || \
				 (ptr) == &mons[PM_TELEMINDFLAYER] || \
				 (ptr) == &mons[PM_MIND_SUCKER] || \
				 (ptr) == &mons[PM_GIANT_MIND_FLAYER] || \
				 (ptr) == &mons[PM_UNDEAD_MIND_FLAYER] || \
				 (ptr) == &mons[PM_ILLITHID] || \
				 (ptr) == &mons[PM_CURSING_ORB] || \
				 (ptr) == &mons[PM_ONE_EYED_BUGGER] || \
				 (ptr) == &mons[PM_ONE_EYED_ERROR] || \
				 (ptr) == &mons[PM_DIMMER] || \
				 (ptr) == &mons[PM_ODDITY] || \
				 (ptr) == &mons[PM_CURSED_BLOB] || \
				 (ptr) == &mons[PM_GREATER_DIMMER] || \
				 (ptr) == &mons[PM_TERRIBLE_ORB] || \
				 (ptr) == &mons[PM_PSYCH_ORB] || \
				 (ptr) == &mons[PM_NECROMORB] || \
				 (ptr) == &mons[PM_EVIL_ORB] || \
				 (ptr) == &mons[PM_NERF_ORB] || \
				 (ptr) == &mons[PM_ABRA] || \
				 (ptr) == &mons[PM_KADABRA] || \
				 (ptr) == &mons[PM_ALAKAZAM] || \
				 (ptr) == &mons[PM_MASTER_MIND_FLAYER])

#define made_of_rock(ptr)	((passes_walls(ptr) && thick_skinned(ptr)) || \
				 (ptr) == &mons[PM_STONE_GOLEM] || \
				 (ptr) == &mons[PM_STONE_STATUE] || \
				 (ptr) == &mons[PM_STATUE_GARGOYLE])
#define hates_silver(ptr)	(is_were(ptr) || is_vampire(ptr) || \
				 is_demon(ptr) || (ptr) == &mons[PM_SHADE] || \
				 ((ptr)->mlet==S_IMP && (ptr) != &mons[PM_TENGU]))
/* Used for conduct with corpses, tins, and digestion attacks */
/* G_NOCORPSE monsters might still be swallowed as a purple worm */
/* Maybe someday this could be in mflags... */
#define vegan(ptr)		((ptr)->mlet == S_BLOB || \
				 (ptr)->mlet == S_JELLY ||            \
				 (ptr)->mlet == S_FUNGUS ||           \
				 (ptr)->mlet == S_VORTEX ||           \
				 (ptr)->mlet == S_LIGHT ||            \
				 (ptr)->mlet == S_GRUE ||            \
				((ptr)->mlet == S_ELEMENTAL &&        \
				 (ptr) != &mons[PM_STALKER]) ||       \
				((ptr)->mlet == S_GOLEM &&            \
				 (ptr) != &mons[PM_FLESH_GOLEM] &&    \
				 (ptr) != &mons[PM_FRANKENSTEIN_S_MONSTER] && \
				 (ptr) != &mons[PM_LEATHER_GOLEM]) || \
				 noncorporeal(ptr))
#define vegetarian(ptr)		(vegan(ptr) || \
				 (ptr)->mlet == S_RUBMONST ||            \
				((ptr)->mlet == S_PUDDING &&         \
				 (ptr) != &mons[PM_BLACK_PUDDING]))
/* For vampires */
#define has_blood(ptr)		(!vegetarian(ptr) && \
				   (ptr)->mlet != S_GOLEM && \
				  ((ptr)->mlet != S_BAD_FOOD || \
				   (ptr) == &mons[PM_KILLER_TRIPE_RATION]) && \
				   (!is_undead(ptr) || is_vampire(ptr)))

#define befriend_with_obj(ptr, obj) ((obj)->oclass == FOOD_CLASS && ( \
		is_domestic(ptr) || (is_animal(ptr) && Race_if(PM_HUMANOID_CENTAUR) && !(ptr->geno & G_UNIQ)) || (is_rat(ptr) && Role_if(PM_CONVICT)) || \
		/* [Tom] Dorothy wants more pets... */ \
		(obj)->otyp == CHEESE && ((ptr) == &mons[PM_GIANT_RAT] || \
		    (ptr) == &mons[PM_SEWER_RAT] || \
		    (ptr) == &mons[PM_BLACK_RAT] || \
		    (ptr) == &mons[PM_PACK_RAT]) || \
		(obj)->otyp == CARROT && ((ptr) == &mons[PM_RABBIT] || \
		    (ptr) == &mons[PM_RABID_RABBIT]) || \
		( (ptr)->mlet == S_DRAGON && Race_if(PM_HUMANLIKE_DRAGON)) || \
		(obj)->otyp == KELP_FROND && (ptr->mflags3 & M3_PETTY) || \
		(obj)->otyp == BANANA && (ptr)->mlet == S_YETI))

#endif /* MONDATA_H */
