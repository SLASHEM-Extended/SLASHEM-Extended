/*	SCCS Id: @(#)mondata.h	3.4	2003/01/08	*/
/* Copyright (c) 1989 Mike Threepoint				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef MONDATA_H
#define MONDATA_H

#define verysmall(ptr)		((ptr)->msize < MZ_SMALL)
#define rathersmall(ptr)	((ptr)->msize <= MZ_SMALL)
#define normalsize(ptr)		((ptr)->msize == MZ_MEDIUM)
#define bigmonst(ptr)		((ptr)->msize >= MZ_LARGE)
#define verybigmonst(ptr)	((ptr)->msize >= MZ_HUGE)
#define hugemonst(ptr)		((ptr)->msize >= MZ_GIGANTIC)

#define pm_resistance(ptr,typ)	(((ptr)->mresists & (typ)) != 0)

#define resists_fire(mon)	((((mon)->mintrinsics & MR_FIRE) != 0) || (attackdamagetype((mon)->data, AT_BREA, AD_FIRE)) || (attackdamagetype((mon)->data, AT_MAGC, AD_FIRE)) || (mon)->egotype_elementalist)
#define resists_cold(mon)	((((mon)->mintrinsics & MR_COLD) != 0) || (attackdamagetype((mon)->data, AT_BREA, AD_COLD)) || (attackdamagetype((mon)->data, AT_MAGC, AD_COLD)) || (mon)->egotype_elementalist)
#define resists_sleep(mon)	((((mon)->mintrinsics & MR_SLEEP) != 0) || (attackdamagetype((mon)->data, AT_BREA, AD_SLEE))|| (attackdamagetype((mon)->data, AT_MAGC, AD_SLEE)) || (mon)->egotype_watcher)
#define resists_disint(mon)	((((mon)->mintrinsics & MR_DISINT) != 0) || mercedesride(ART_STABITEGRITY, mon) || (attackdamagetype((mon)->data, AT_BREA, AD_DISN)) || (mon)->egotype_resistor)
#define resists_elec(mon)	((((mon)->mintrinsics & MR_ELEC) != 0) || (attackdamagetype((mon)->data, AT_BREA, AD_ELEC)) || (attackdamagetype((mon)->data, AT_MAGC, AD_ELEC)) || (mon)->egotype_elementalist)
#define resists_poison(mon)	((((mon)->mintrinsics & MR_POISON) != 0) || (attackdamagetype((mon)->data, AT_BREA, AD_DRST)) || mercedesride(ART_HISTORY_OF_SURVIVED_COLICS, mon) || (attackdamagetype((mon)->data, AT_MAGC, AD_DRST)) || (mon)->egotype_poisoner)
#define resists_acid(mon)	((((mon)->mintrinsics & MR_ACID) != 0) || (attackdamagetype((mon)->data, AT_BREA, AD_ACID)) || (attackdamagetype((mon)->data, AT_MAGC, AD_ACID)) || (mon)->egotype_acidspiller)
#define resists_ston(mon)	((((mon)->mintrinsics & MR_STONE) != 0) || (mon)->egotype_resistor)

#define resists_drain(mon)      ((((mon)->mintrinsics & MR_DRAIN) != 0) || (mon)->egotype_watcher)
#define resists_death(mon)      ((((mon)->mintrinsics & MR_DEATH) != 0) || (attackdamagetype((mon)->data, AT_MAGC, AD_DISN)) || (mon)->egotype_resistor)

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

#define does_need_one(ptr)           (((ptr)->mresists & MR_PLUSONE) != 0)
#define does_need_two(ptr)           (((ptr)->mresists & MR_PLUSTWO) != 0)
#define does_need_three(ptr)         (((ptr)->mresists & MR_PLUSTHREE) != 0)
#define does_need_four(ptr)          (((ptr)->mresists & MR_PLUSFOUR) != 0)
#define does_hit_as_one(ptr)         (((ptr)->mresists & MR_HITASONE) != 0)
#define does_hit_as_two(ptr)         (((ptr)->mresists & MR_HITASTWO) != 0)
#define does_hit_as_three(ptr)       (((ptr)->mresists & MR_HITASTHREE) != 0)
#define does_hit_as_four(ptr)        (((ptr)->mresists & MR_HITASFOUR) != 0)

#define is_flyer(ptr)		(((ptr)->mflags1 & M1_FLY) != 0L)
#define is_floater(ptr)		((ptr)->mlet == S_EYE)
#define is_clinger(ptr)		(((ptr)->mflags1 & M1_CLING) != 0L)
#define is_swimmer(ptr)		((((ptr)->mflags1 & M1_SWIM) != 0L) || ((ptr)->mlet == S_EEL))
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
				 (ptr) == &mons[PM_AIR_ELEMENTAL] || (ptr) == &mons[PM_GREATER_AIR_ELEMENTAL] || (ptr) == &mons[PM_PETTY_AIR_ELEMENTAL] || (ptr) == &mons[PM_SHAPELESS_AIR_ELEMENTAL] || (ptr) == &mons[PM_TIME_TEMPEST] || (ptr) == &mons[PM_SKY_CRUISER] || (ptr) == &mons[PM_AERIAL_SERVENT] || (ptr) == &mons[PM_ELDER_AIR_ELEMENTAL] || (ptr) == &mons[PM_WIND_ELEMENTAL] || (ptr) == &mons[PM_VILLAGE_FOOL] || (ptr) == &mons[PM_MIST_MAN] || (ptr) == &mons[PM_DUST_ELEMENTAL] || (ptr) == &mons[PM_EGO_DUST_ELEMENTAL] || (ptr) == &mons[PM_STRIP_MINE] || (ptr) == &mons[PM_THEMATIC_AIR_ELEMENTAL] || (ptr) == &mons[PM_HURRICON] || (ptr) == &mons[PM_RUMBLER])
#define is_fire(ptr)		((ptr) == &mons[PM_FIRE_VORTEX] || (ptr) == &mons[PM_FIRE_GOLEM] || \
				 (ptr) == &mons[PM_FIRE_ELEMENTAL] || (ptr) == &mons[PM_GREATER_FIRE_ELEMENTAL] || (ptr) == &mons[PM_PORTER_FIRE_ELEMENTAL] || (ptr) == &mons[PM_HEAVY_GIRL] || (ptr) == &mons[PM_PLASMA_ELEMENTAL] || (ptr) == &mons[PM_BURNER] || (ptr) == &mons[PM_ROCK_EATER] || (ptr) == &mons[PM_STONE_ELEMENTAL] || (ptr) == &mons[PM_MAGMA_ELEMENTAL] || (ptr) == &mons[PM_FLYING_ASSHOLE] || (ptr) == &mons[PM_MAGNO_FLIER] || (ptr) == &mons[PM_ELDER_FIRE_ELEMENTAL] || (ptr) == &mons[PM_FISSURE_FISHER] || (ptr) == &mons[PM_SPEED_PHOTON] || (ptr) == &mons[PM_AIRCRAFT_CARRIER] || (ptr) == &mons[PM_FIRE_SPIRIT] || (ptr) == &mons[PM_HEIKE] || (ptr) == &mons[PM_GREATER_PLASMA_ELEMENTAL] || (ptr) == &mons[PM_VOLCANIC_ELEMENTAL] || (ptr) == &mons[PM_THEMATIC_FIRE_ELEMENTAL] || (ptr) == &mons[PM_FLAME_ATRONACH] || (ptr) == &mons[PM_LAVA_GOLEM] || (ptr) == &mons[PM_BURNING_MONSTER] || (ptr) == &mons[PM_BURNING_BRUTE] || (ptr) == &mons[PM_SWEEPING_FIRE_VORTEX] || (ptr) == &mons[PM_HEAT_VORTEX] || (ptr) == &mons[PM_DANCING_FLAME] || (ptr) == &mons[PM_MAGMA_VORTEX] || (ptr) == &mons[PM_PLASMA_VORTEX] || (ptr) == &mons[PM_HOT_LAVA_BLOB] || (ptr) == &mons[PM_LAVA_MONSTER] || (ptr) == &mons[PM_VOLCANIC_GRUE] || (ptr) == &mons[PM_LAVA_WALL] || (ptr) == &mons[PM_LAVA_TURRET])
#define flaming(ptr)		((ptr) == &mons[PM_FIRE_VORTEX] || \
				 (ptr) == &mons[PM_FLAMING_SPHERE] || \
				 (ptr) == &mons[PM_SUMMONED_FLAMING_SPHERE] || \
				 (ptr) == &mons[PM_FIRE_ELEMENTAL] || \
				 (ptr) == &mons[PM_GREATER_FIRE_ELEMENTAL] || \
				 (ptr) == &mons[PM_CHARMANDER] || (ptr) == &mons[PM_CHARMELEON] || (ptr) == &mons[PM_FIRE_GOLEM] || (ptr) == &mons[PM_CHARIZARD] || (ptr) == &mons[PM_SALAMANDER] || (ptr) == &mons[PM_PORTER_FIRE_ELEMENTAL] || (ptr) == &mons[PM_HEAVY_GIRL] || (ptr) == &mons[PM_PLASMA_ELEMENTAL] || (ptr) == &mons[PM_BURNER] || (ptr) == &mons[PM_ROCK_EATER] || (ptr) == &mons[PM_STONE_ELEMENTAL] || (ptr) == &mons[PM_MAGMA_ELEMENTAL] || (ptr) == &mons[PM_FLYING_ASSHOLE] || (ptr) == &mons[PM_MAGNO_FLIER] || (ptr) == &mons[PM_ELDER_FIRE_ELEMENTAL] || (ptr) == &mons[PM_FISSURE_FISHER] || (ptr) == &mons[PM_SPEED_PHOTON] || (ptr) == &mons[PM_AIRCRAFT_CARRIER] || (ptr) == &mons[PM_FIRE_SPIRIT] || (ptr) == &mons[PM_HEIKE] || (ptr) == &mons[PM_GREATER_PLASMA_ELEMENTAL] || (ptr) == &mons[PM_VOLCANIC_ELEMENTAL] || (ptr) == &mons[PM_THEMATIC_FIRE_ELEMENTAL] || (ptr) == &mons[PM_FLAME_ATRONACH] || (ptr) == &mons[PM_LAVA_GOLEM] || (ptr) == &mons[PM_BURNING_MONSTER] || (ptr) == &mons[PM_BURNING_BRUTE] || (ptr) == &mons[PM_SWEEPING_FIRE_VORTEX] || (ptr) == &mons[PM_HEAT_VORTEX] || (ptr) == &mons[PM_DANCING_FLAME] || (ptr) == &mons[PM_MAGMA_VORTEX] || (ptr) == &mons[PM_PLASMA_VORTEX] || (ptr) == &mons[PM_HOT_LAVA_BLOB] || (ptr) == &mons[PM_LAVA_MONSTER] || (ptr) == &mons[PM_VOLCANIC_GRUE] || (ptr) == &mons[PM_LAVA_WALL] || (ptr) == &mons[PM_LAVA_TURRET])
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
#define telepathic(ptr)		((ptr)->mflags3 & M3_TELEPATHIC)
#define is_armed(ptr)		(attacktype(ptr, AT_WEAP) || (ptr) == &mons[PM_ANIMATED_WEDGE_SANDAL] || (ptr) == &mons[PM_ANIMATED_SEXY_LEATHER_PUMP] || (ptr) == &mons[PM_ANIMATED_BEAUTIFUL_FUNNEL_HEELED_PUMP] || (ptr) == &mons[PM_ANIMATED_LEATHER_PEEP_TOE] || (ptr) == &mons[PM_ANIMATED_COMBAT_STILETTO])
#define acidic(ptr)		(((ptr)->mflags1 & M1_ACID) != 0L)
#define poisonous(ptr)		(((ptr)->mflags1 & M1_POIS) != 0L)
#define carnivorous(ptr)	(((ptr)->mflags1 & M1_CARNIVORE) != 0L)
#define herbivorous(ptr)	(((ptr)->mflags1 & M1_HERBIVORE) != 0L)
#define metallivorous(ptr)	(((ptr)->mflags1 & M1_METALLIVORE) != 0L)
#define organivorous(ptr)	(((ptr)->mflags4 & M4_ORGANIVORE) != 0L)
#define shapechanger(ptr)	(((ptr)->mflags4 & M4_SHAPESHIFT) != 0L)
#define isgridbug(ptr)		(((ptr)->mflags4 & M4_GRIDBUG) != 0L)
#define monpolyok(ptr)		(((ptr)->mflags2 & M2_NOPOLY) == 0L) /* monsters may poly into this */
#define polyok(ptr)		(((ptr)->mflags2 & M2_NOPOLY) == 0L && ((ptr)->mflags3 & M3_NOPLRPOLY) == 0L && (ptr)->mlevel < 30 ) /* players may poly into this */
#define is_stalker(ptr)		(((ptr)->mflags2 & M2_STALK) != 0L)
#define is_undead(ptr)		(((ptr)->mflags2 & M2_UNDEAD) != 0L)
#define is_were(ptr)		(((ptr)->mflags2 & M2_WERE) != 0L)
#define is_vampire(ptr)		(((ptr)->mflags2 & M2_VAMPIRE) != 0L)
#define is_elf(ptr)		(((ptr)->mflags2 & M2_ELF) != 0L)
#define is_dwarf(ptr)		(((ptr)->mflags2 & M2_DWARF) != 0L)
#define is_gnome(ptr)		(((ptr)->mflags2 & M2_GNOME) != 0L)
#define is_orc(ptr)		(((ptr)->mflags2 & M2_ORC) != 0L)
#define is_human(ptr)		(((ptr)->mflags2 & M2_HUMAN) != 0L)
#define is_hobbit(ptr)		(((ptr)->mflags2 & M2_HOBBIT) != 0L)
#define your_race(ptr)		((((ptr)->mflags2 & urace.selfmask) != 0L) || (ptr) == &mons[PM_STOANING_EYE])
#define is_bat(ptr)		(((ptr)->mflags4 & M4_BAT) != 0L)
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
#define always_tame(ptr)	(((ptr)->mflags4 & M4_TAME) != 0L)
#define race_hostile(ptr)	(((ptr)->mflags2 & urace.hatemask) != 0L)
#define race_peaceful(ptr)	(((ptr)->mflags2 & urace.lovemask) != 0L)
#define extra_nasty(ptr)	(((ptr)->mflags2 & M2_NASTY) != 0L)
#define strongmonst(ptr)	(((ptr)->mflags2 & M2_STRONG) != 0L)
#define can_breathe(ptr)	attacktype(ptr, AT_BREA)
#define cantwield(ptr)		(nohands(ptr) || verysmall(ptr) || \
				 (ptr)->mlet == S_ANT)
#define could_twoweap(ptr)	((ptr)->mattk[1].aatyp == AT_WEAP)
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
#define webmaker(ptr)		((ptr)->mflags3 & M3_SPIDER)
#define is_unicorn(ptr)		((ptr)->mlet == S_UNICORN && likes_gems(ptr))	/* KMH */
#define is_longworm(ptr)	(((ptr) == &mons[PM_BABY_LONG_WORM]) || \
				 ((ptr) == &mons[PM_LONG_WORM]) || \
				 ((ptr) == &mons[PM_LONG_WORM_TAIL]))
#define is_covetous(ptr)	((ptr)->mflags3 & M3_COVETOUS)
#define infravision(ptr)	((ptr)->mflags3 & M3_INFRAVISION)
#define infravisible(ptr)	((ptr)->mflags3 & M3_INFRAVISIBLE)
#define can_betray(ptr)		(((ptr)->mflags3 & M3_TRAITOR) || (Race_if(PM_CELTIC) && ptr->mlet != S_GOLEM) )
#define cannot_be_tamed(ptr)	((ptr)->mflags3 & M3_NOTAME)
#define avoid_player(ptr)	((ptr)->mflags3 & M3_AVOIDER)
#define lithivorous(ptr)	((ptr)->mflags3 & M3_LITHIVORE)
#define is_petty(ptr)	((ptr)->mflags3 & M3_PETTY)
#define is_pokemon(ptr)	((ptr)->mflags3 & M3_POKEMON)
#define is_mplayer(ptr)		(((ptr) >= &mons[PM_ARCHEOLOGIST]) && \
				 ((ptr) <= &mons[PM_WIZARD]))
#define is_umplayer(ptr)		(((ptr) >= &mons[PM_UNDEAD_ARCHEOLOGIST]) && \
				 ((ptr) <= &mons[PM_UNDEAD_WIZARD]))
#define is_rider(ptr)		((ptr) == &mons[PM_DEATH] || \
				 (ptr) == &mons[PM_FAMINE] || \
				 (ptr) == &mons[PM_FRUSTRATION] || \
				 (ptr) == &mons[PM_BASTARD_OPERATOR_FROM_HELL] || \
				 (ptr) == &mons[PM_WAR] || \
				 (ptr) == &mons[PM_DNETHACK_ELDER_PRIEST_TM_] || \
				 (ptr) == &mons[PM_PESTILENCE])
#define is_deadlysin(ptr)		((ptr) == &mons[PM_GLUTTONY] || \
				 (ptr) == &mons[PM_LUST] || \
				 (ptr) == &mons[PM_ENVY] || \
				 (ptr) == &mons[PM_SLOTH] || \
				 (ptr) == &mons[PM_DEPRESSION] || \
				 (ptr) == &mons[PM_GREED] || \
				 (ptr) == &mons[PM_WRATH] || \
				 (ptr) == &mons[PM_PRIDE])
#define eating_is_fatal(ptr)		((ptr) == &mons[PM_DEATH] || \
				 (ptr) == &mons[PM_FAMINE] || \
				 (ptr) == &mons[PM_BLACK_HOLE_BLOB] || \
				 (ptr) == &mons[PM_LUST] || \
				 (ptr) == &mons[PM_BASTARD_OPERATOR_FROM_HELL] || \
				 (ptr) == &mons[PM_ENVY] || \
				 (ptr) == &mons[PM_SLOTH] || \
				 (ptr) == &mons[PM_DEPRESSION] || \
				 (ptr) == &mons[PM_GREED] || \
				 (ptr) == &mons[PM_WRATH] || \
				 (ptr) == &mons[PM_PRIDE] || \
				 (ptr) == &mons[PM_GLUTTONY] || \
				 (ptr) == &mons[PM_FRUSTRATION] || \
				 (ptr) == &mons[PM_WAR] || \
				 (ptr) == &mons[PM_PESTILENCE])
#define is_placeholder(ptr)	((ptr) == &mons[PM_ORC] || \
				 (ptr) == &mons[PM_GIANT] || \
				 (ptr) == &mons[PM_ELF] || \
				 (ptr) == &mons[PM_HUMAN])
#define stationary(ptr)	((ptr)->mflags3 & M3_NONMOVING)

#define spawnswithblockheels(ptr)		(((ptr)->mflags4 & M4_BLOCKHEELBOOTS) != 0L)
#define spawnswithsandals(ptr)		(((ptr)->mflags4 & M4_SANDALS) != 0L)
#define spawnswithsneakers(ptr)		(((ptr)->mflags4 & M4_SNEAKERS) != 0L)
#define spawnswithhammersandal(ptr)		(((ptr)->mflags4 & M4_HAMMERSANDAL) != 0L)
#define spawnswithpumps(ptr)		(((ptr)->mflags4 & M4_PUMPS) != 0L)
#define treadedshoemonster(ptr)		(((ptr)->mflags4 & M4_TREADED) != 0L)
#define highheeledshoemonster(ptr)		(((ptr)->mflags4 & M4_HIGHHEELED) != 0L)
#define opentoeshoemonster(ptr)		(((ptr)->mflags4 & M4_OPENTOE) != 0L)

#define canalwaysride(ptr)	((ptr) == &mons[PM_SLOW_HUMPER] || (ptr) == &mons[PM_TLOTTING_HUMPER] || (ptr) == &mons[PM_HUMPER] || (ptr) == &mons[PM_DOMESTIC_HUMPER] || (ptr) == &mons[PM_FAST_HUMPER] || (ptr) == &mons[PM_PETTY_FAST_HUMPER] || (ptr) == &mons[PM_BLITZ_HUMPER] || (ptr) == &mons[PM_WEAPONIZED_HUMPER] || (ptr) == &mons[PM_EFFING_HUMPER] || (ptr) == &mons[PM_TURBO_HUMPER] || (ptr) == &mons[PM_RARE_HUMPER] || (ptr) == &mons[PM_WOODEN_RAFT] || (ptr) == &mons[PM_MINE_TROLLEY] || (ptr) == &mons[PM_CLASHING_MINE_TROLLEY] || (ptr) == &mons[PM_SHIP_O__THE_SEA] || (ptr) == &mons[PM_RIDABLE_HUMPER] || (ptr) == &mons[PM_INKA_HUMPER] || (ptr) == &mons[PM_DIGGING_ON_FARMER])

#define is_nonmoving(ptr) ( (ptr)->mmove < 1 || (ptr)->mlet == S_TURRET || ((ptr)->mflags3 & M3_NONMOVING) )

#define is_eel(ptr)	((ptr)->mlet == S_EEL)

#define is_multihued(ptr)	((ptr)->mflags4 & M4_MULTIHUED)

#define is_angbandmonster(ptr)	((ptr)->mflags5 & M5_ANGBAND)
#define is_cowmonster(ptr)	((ptr)->mflags5 & M5_SPACEWARS)
#define is_vanillamonster(ptr)	((ptr)->mflags5 & M5_VANILLA)
#define is_animemonster(ptr)	((ptr)->mflags5 & M5_ANIMEBAND)
#define is_steammonster(ptr)	((ptr)->mflags5 & M5_STEAMBAND)
#define is_dlordsmonster(ptr)	((ptr)->mflags5 & M5_DLORDS)
#define is_dnethackmonster(ptr)	((ptr)->mflags5 & M5_DNETHACK)
#define is_jokemonster(ptr)	((ptr)->mflags5 & M5_JOKE)
#define is_diablomonster(ptr)	((ptr)->mflags5 & M5_DIABLO)
#define is_jonadabmonster(ptr)	((ptr)->mflags5 & M5_JONADAB)
#define is_evilpatchmonster(ptr)	((ptr)->mflags5 & M5_EVIL)
#define is_randomizedmonster(ptr)	((ptr)->mflags5 & M5_RANDOMIZED)
#define is_elonamonster(ptr)	((ptr)->mflags5 & M5_ELONA)
#define is_aoemonster(ptr)	((ptr)->mflags5 & M5_AOE)
#define is_elderscrollsmonster(ptr)	((ptr)->mflags5 & M5_ELDERSCROLLS)
#define is_chatgptmonster(ptr)	((ptr)->mflags5 & M5_CHATGPT)
#define is_cddamonster(ptr)	((ptr)->mflags5 & M5_CDDA)

#define monsterflees(ptr)	((ptr)->mflags6 & M6_FLEE)
#define monsterrandomwalk(ptr)	((ptr)->mflags6 & M6_RANDWALK)

#define nocorpsedecay(ptr)		((ptr)->mflags3 & M3_NO_DECAY)

#define standardmimic(ptr)		((ptr)->mflags3 & M3_MIMIC)
#define permamimic(ptr)		((ptr)->mflags3 & M3_PERMAMIMIC)

#define slime_on_touch(ptr)		((ptr)->mflags3 & M3_SLIME)

/* will the monster split in water (only regular gremlin in vanilla but different types here in slex)? */
#define splittinggremlin(ptr)		((ptr) == &mons[PM_GREMLIN] || (ptr) == &mons[PM_GAME_OVER] || (ptr) == &mons[PM_BLACK_PUDDLIN] || (ptr) == &mons[PM_NAYSAYER_GREMLIN] || (ptr) == &mons[PM_HEA_LOL_GREMLIN] || (ptr) == &mons[PM_DISSOLIN] || (ptr) == &mons[PM_GRRGRRGRR] || (ptr) == &mons[PM_GREMLIN_MANYFOLDCURSER] || (ptr) == &mons[PM_GREMLIN_VIRER] || (ptr) == &mons[PM_DISTANCE_GREMLIN] || (ptr) == &mons[PM_SPREAD_CONTACT_GREMLIN] || (ptr) == &mons[PM_YAY_AMNESIA_GREMLIN] || (ptr) == &mons[PM_GREMLIN_LEADER] || (ptr) == &mons[PM_GREMLIN_WARLORD] || (ptr) == &mons[PM_GREMLIN_DEATHSUCKER])
#define splittinglavagremlin(ptr)	((ptr) == &mons[PM_LAVA_GREMLIN])

#define always_egotype(ptr)		((ptr)->mflags3 & M3_EGOTYPE)

#define uncommon2(ptr)		((ptr)->mflags3 & M3_FREQ_UNCOMMON2)
#define uncommon3(ptr)		((ptr)->mflags3 & M3_FREQ_UNCOMMON3)
#define uncommon5(ptr)		((ptr)->mflags3 & M3_FREQ_UNCOMMON5)
#define uncommon7(ptr)		((ptr)->mflags3 & M3_FREQ_UNCOMMON7)
#define uncommon10(ptr)		((ptr)->mflags3 & M3_FREQ_UNCOMMON10)

/* return TRUE if the monster tends to revive */
#define is_reviver(ptr)		(is_rider(ptr) || is_deadlysin(ptr) || ((ptr)->mlet == S_FUNGUS && !nocorpsedecay(ptr) ) || \
				 (ptr)->mlet == S_TROLL || ((ptr)->mflags4 & M4_REVIVE) )

/* does the monster only eat old corpses? (both ptr and monster number!!!) */
#define saprovorous(ptr)	((ptr) == &mons[PM_GHOUL] || (ptr) == &mons[PM_LOL_WE_INVENTED_ANOTHER_GHAST] || (ptr) == &mons[PM_SAPROVOROUS_NYMPH] || (ptr) == &mons[PM_GHAST] || (ptr) == &mons[PM_CORPSEWORM] || (ptr) == &mons[PM_DOGSHIT_SEARCHER] || (ptr) == &mons[PM_STINKING_ALIEN] || (ptr) == &mons[PM_DOSTICH] || (ptr) == &mons[PM_CHTO] || (ptr) == &mons[PM_GASTLY] || (ptr) == &mons[PM_PHANTOM_GHOST] || (ptr) == &mons[PM_HAUNTER] || (ptr) == &mons[PM_GENGAR])
#define saprovorousnumber(mnnm)	(mnnm == PM_GHOUL || mnnm == PM_LOL_WE_INVENTED_ANOTHER_GHAST || mnnm == PM_SAPROVOROUS_NYMPH || mnnm == PM_GHAST || mnnm == PM_CORPSEWORM || mnnm == PM_DOGSHIT_SEARCHER || mnnm == PM_STINKING_ALIEN || mnnm == PM_DOSTICH || mnnm == PM_CHTO || mnnm == PM_GASTLY || mnnm == PM_PHANTOM_GHOST || mnnm == PM_HAUNTER || mnnm == PM_GENGAR)

/* this returns the light's range, or 0 if none; if we add more light emitting
   monsters, we'll likely have to add a new light range field to mons[] */
#define emits_light(ptr)	( ((ptr) == &mons[PM_BRIGHTLIGHT]) ? 4 : \
				 ((ptr) == &mons[PM_VERY_BRIGHT_LIGHT]) ? 4 : \
				 ((ptr) == &mons[PM_SHINY_LUCOZADE]) ? 4 : \
				 ((ptr) == &mons[PM_YOMAGNTHO]) ? 4 : \
				 ((ptr) == &mons[PM_CTHUGHA]) ? 1 : \
				 ((ptr) == &mons[PM_CROUPIER]) ? 1 : \
				 ((ptr) == &mons[PM_MASTER_CROUPIER]) ? 1 : \
				 ((ptr) == &mons[PM_EXCEPTIONAL_CROUPIER]) ? 1 : \
				 ((ptr) == &mons[PM_EXPERIENCED_CROUPIER]) ? 1 : \
				 ((ptr) == &mons[PM_ELITE_CROUPIER]) ? 1 : \
				 ((ptr) == &mons[PM_ALLURING_SERPENT]) ? 1 : \
				 ((ptr) == &mons[PM_RADIANT_BLUE_CENTIPEDE]) ? 1 : \
				 ((ptr) == &mons[PM_INDIVIDUAL_WILL_O_THE_WISP]) ? 1 : \
				 ((ptr) == &mons[PM_BANG_BRIGHT_LIGHT]) ? 5 : \
				 ( (ptr)->mlet == S_LIGHT || (ptr) == &mons[PM_FIRE_VORTEX]) ? 3 : \
				 ((ptr) == &mons[PM_FIRE_ELEMENTAL]) ? 2 : \
				 ((ptr) == &mons[PM_CORONA_SENTAI]) ? 2 : \
				 ((ptr) == &mons[PM_LAMP_GOLEM]) ? 2 : \
				 ((ptr) == &mons[PM_POFF_LICH]) ? 2 : \
				 ((ptr) == &mons[PM_GLOWING_ONE]) ? 2 : \
				 ((ptr) == &mons[PM_PILE_OF_LIGHT_RADIUS_COINS]) ? 2 : \
				 ((ptr) == &mons[PM_GREATER_FIRE_ELEMENTAL]) ? 3 : \
				 ((ptr) == &mons[PM_RADIANT_ARCHON]) ? 3 : \
				 ((ptr) == &mons[PM_PHANTOM_LUMINA]) ? 3 : \
				 ((ptr) == &mons[PM_RADIANT_GUARDIAN]) ? 3 : \
				 ((ptr) == &mons[PM_GLOW_FLY]) ? 3 : \
				 ((ptr) == &mons[PM_SABINE_S_BLOCK_HEELED_BRIDAL_PUMP]) ? 4 : \
				 ((ptr) == &mons[PM_LIGHT_SHAPE]) ? 3 : \
				 ((ptr) == &mons[PM_SATANIC_GNOME]) ? 3 : \
				 ((ptr) == &mons[PM_LUMINEOUS_FIREFLY]) ? 3 : \
				 ((ptr) == &mons[PM_LANTERN_STALKER]) ? 3 : \
				 ((ptr) == &mons[PM_CORONA_WRAITH]) ? 3 : \
				 ((ptr) == &mons[PM_GLOWWORM]) ? 3 : \
				 ((ptr) == &mons[PM_RADIATION_WORM]) ? 3 : \
				 ((ptr) == &mons[PM_BRIGHT_DISEASE_JABBERWOCK]) ? 3 : \
				 ((ptr) == &mons[PM_LUMINESCENT_MOLD]) ? 3 : \
				 ((ptr) == &mons[PM_LUMINESCENT_GROWTH]) ? 3 : \
				 ((ptr) == &mons[PM_LUMINESCENT_FUNGUS]) ? 3 : \
				 ((ptr) == &mons[PM_LUMINESCENT_PATCH]) ? 3 : \
				 ((ptr) == &mons[PM_LUMINESCENT_STALK]) ? 3 : \
				 ((ptr) == &mons[PM_LUMINESCENT_MUSHROOM]) ? 3 : \
				 ((ptr) == &mons[PM_LUMINESCENT_SPORE]) ? 3 : \
				 ((ptr) == &mons[PM_LUMINESCENT_COLONY]) ? 3 : \
				 ((ptr) == &mons[PM_LUMINESCENT_FORCE_FUNGUS]) ? 3 : \
				 ((ptr) == &mons[PM_LUMINESCENT_WORT]) ? 3 : \
				 ((ptr) == &mons[PM_LUMINESCENT_FORCE_PATCH]) ? 3 : \
				 ((ptr) == &mons[PM_LUMINESCENT_WARP_FUNGUS]) ? 3 : \
				 ((ptr) == &mons[PM_LUMINESCENT_WARP_PATCH]) ? 3 : \
				 ((ptr) == &mons[PM_BRIGHTER_SHADOW]) ? 3 : \
				 ((ptr) == &mons[PM_LUMINOUS_WING]) ? 3 : \
				 ((ptr) == &mons[PM_TAME_SPEED___MONSTER]) ? 3 : \
				 ((ptr) == &mons[PM_SMOKING_LIBAVIUS_SPIRIT]) ? 3 : \
				 ((ptr) == &mons[PM_FIRE_VAMPIRE])? 2 : \
				 ((ptr) == &mons[PM_GLOWING_DRACONIAN]) ? 2 : \
				 ((ptr) == &mons[PM_UNDERWATER_THREAT]) ? 2 : \
				 ((ptr) == &mons[PM_GLOSSO_GOBY]) ? 2 : \
				 ((ptr) == &mons[PM_GLAMOROUS_GOBY]) ? 2 : \
				 ((ptr) == &mons[PM_FLAMING_SPHERE]) ? 1 : \
				 ((ptr) == &mons[PM_LUXY_SHOPWRIGHT]) ? 1 : \
				 ((ptr) == &mons[PM_SUMMONED_FLAMING_SPHERE]) ? 1 : \
				 ((ptr) == &mons[PM_CAVE_EXPLORER]) ? 1 : \
				 ((ptr) == &mons[PM_TORCH_CONVERTER]) ? 1 : \
				 ((ptr) == &mons[PM_BLORCHING_DARKBREAD_ZURTY]) ? 1 : \
				 ((ptr) == &mons[PM_UNEXPECTED_LIGHT]) ? 1 : \
				 ((ptr) == &mons[PM_POFFISH]) ? 1 : \
				 ((ptr) == &mons[PM_SCAN_ME_MISSYS]) ? 1 : \
				 ((ptr) == &mons[PM_SPASHCO]) ? 1 : \
				 ((ptr) == &mons[PM_GOBLIN_WHO_CAN_T_SEE_IN_THE_DARK]) ? 1 : \
				 ((ptr) == &mons[PM_SHOCKING_SPHERE]) ? 1 : \
				 ((ptr) == &mons[PM_CORONA_GHOST]) ? 1 : \
				 ((ptr) == &mons[PM_GOLD_DRACONIAN]) ? 1 : \
				 ((ptr) == &mons[PM_FIRECRACKER_KANGAROO]) ? 1 : \
				 ((ptr) == &mons[PM_ANIMATED_TORCH]) ? 1 : \
				 ((ptr) == &mons[PM_CENTAUR_FLAMESTICKER]) ? 1 : \
				 ((ptr) == &mons[PM_SIMMERING_MUSHROOM_PATCH]) ? 1 : \
				 ((ptr) == &mons[PM_WAX_GOLEM]) ? 1 : 0)
/*	[note: the light ranges above were reduced to 1 for performance...] */
/*  WAC increased to 3 and 2?*/
#define likes_lava(ptr)		(ptr == &mons[PM_FIRE_ELEMENTAL] || ptr == &mons[PM_LAVA_SPIDER] || ptr == &mons[PM_GREATER_FIRE_ELEMENTAL] || \
				 ptr == &mons[PM_SALAMANDER] || ptr == &mons[PM_FRYGUY] || ptr == &mons[PM_ASHEN_GOLEM] || ptr == &mons[PM_TELEPORT_TRAP] || ptr == &mons[PM_LAVA_GREMLIN] || ptr == &mons[PM_STEAM_NAGA] || ptr == &mons[PM_STEAM_NAGA_HATCHLING] || ptr == &mons[PM_PLAYER_SALAMANDER] || ptr == &mons[PM_FIRE_GOLEM] || ptr == &mons[PM_CHARMANDER] || ptr == &mons[PM_CHARMELEON]  || ptr == &mons[PM_CHARIZARD] || (ptr) == &mons[PM_PORTER_FIRE_ELEMENTAL] || (ptr) == &mons[PM_HEAVY_GIRL] || (ptr) == &mons[PM_PLASMA_ELEMENTAL] || (ptr) == &mons[PM_BURNER] || (ptr) == &mons[PM_ROCK_EATER] || (ptr) == &mons[PM_STONE_ELEMENTAL] || (ptr) == &mons[PM_MAGMA_ELEMENTAL] || (ptr) == &mons[PM_FLYING_ASSHOLE] || (ptr) == &mons[PM_MAGNO_FLIER] || (ptr) == &mons[PM_MAGMA_GOLEM] || (ptr) == &mons[PM_ELDER_FIRE_ELEMENTAL] || (ptr) == &mons[PM_FISSURE_FISHER] || (ptr) == &mons[PM_SPEED_PHOTON] || (ptr) == &mons[PM_AIRCRAFT_CARRIER] || (ptr) == &mons[PM_FIRE_SPIRIT] || (ptr) == &mons[PM_HEIKE] || (ptr) == &mons[PM_GREATER_PLASMA_ELEMENTAL] || (ptr) == &mons[PM_VOLCANIC_ELEMENTAL] || (ptr) == &mons[PM_THEMATIC_FIRE_ELEMENTAL] || (ptr) == &mons[PM_FLAME_ATRONACH] || (ptr) == &mons[PM_LAVA_GOLEM] || (ptr) == &mons[PM_BURNING_MONSTER] || (ptr) == &mons[PM_BURNING_BRUTE] || (ptr) == &mons[PM_SWEEPING_FIRE_VORTEX] || (ptr) == &mons[PM_HEAT_VORTEX] || (ptr) == &mons[PM_DANCING_FLAME] || (ptr) == &mons[PM_MAGMA_VORTEX] || (ptr) == &mons[PM_PLASMA_VORTEX] || (ptr) == &mons[PM_HOT_LAVA_BLOB] || (ptr) == &mons[PM_LAVA_MONSTER] || (ptr) == &mons[PM_VOLCANIC_GRUE] || (ptr) == &mons[PM_LAVA_WALL] || (ptr) == &mons[PM_LAVA_TURRET])

#define pm_invisible(ptr)	((ptr) == &mons[PM_STALKER] || \
				 (ptr) == &mons[PM_WEAKERER_STALKER] || \
				 (ptr) == &mons[PM_ROAMING_STALKER] || \
				 (ptr) == &mons[PM_BLACK_LIGHT] || \
				 (ptr) == &mons[PM_BLACK_LEYE] || \
				 (ptr) == &mons[PM_BLACK_BEAM] || \
				 (ptr) == &mons[PM_BLACK_RAY] || \
				 (ptr) == &mons[PM_WEREBLACKLIGHT] || \
				 (ptr) == &mons[PM_HUMAN_WEREBLACKLIGHT] || \
				 (ptr) == &mons[PM_CHEATING_BLACK_LIGHT] || \
				 (ptr) == &mons[PM_FORCE_STALKER] || \
				 (ptr) == &mons[PM_LANTERN_STALKER] || \
				 (ptr) == &mons[PM_GREEN_STALKER] || \
				 (ptr) == &mons[PM_WHITE_STALKER] || \
				 (ptr) == &mons[PM_RED_STALKER] || \
				 (ptr) == &mons[PM_YELLOW_STALKER] || \
				 (ptr) == &mons[PM_CYAN_STALKER] || \
				 (ptr) == &mons[PM_BLACK_LASER] || \
				 (ptr) == &mons[PM_BLACK_BULB] || \
				 (ptr) == &mons[PM_BLACK_STAR] || \
				 (ptr) == &mons[PM_BLACK_BOOMER] || \
				 (ptr) == &mons[PM_THE_HIDDEN] || \
				 (ptr) == &mons[PM_INVISIBLE_BADGUY] || \
				 (ptr) == &mons[PM_UNSEEN_POTATO] || \
				 (ptr) == &mons[PM_STONE_STALKER] || \
				 (ptr) == &mons[PM_ILLUSION_WEAVER] || \
				 (ptr) == &mons[PM_INVIS_SAMER] || \
				 (ptr) == &mons[PM_INVIS_SCORER] || \
				 (ptr) == &mons[PM_MIRAGE_WEAVER] || \
				 (ptr) == &mons[PM_PAIN_MASTER] || \
				 (ptr) == &mons[PM_PAIN_MISTER] || \
				 (ptr) == &mons[PM_CAMOUFLAGED_WATCHER] || \
				 (ptr) == &mons[PM_HIDDEN_TRACKER] || \
				 (ptr) == &mons[PM_SILENT_KILLER] || \
				 (ptr) == &mons[PM_SCHEDAU_STALKER] || \
				 (ptr) == &mons[PM_UNSEEN_SERVANT] || \
				 (ptr) == &mons[PM_WERESTALKER] || \
				 (ptr) == &mons[PM_HUMAN_WERESTALKER] || \
				 (ptr) == &mons[PM_STAR_VAMPIRE])

/* could probably add more */
#define likes_fire(ptr)		((ptr) == &mons[PM_FIRE_VORTEX] || \
				  (ptr) == &mons[PM_FLAMING_SPHERE] || \
				  (ptr) == &mons[PM_SUMMONED_FLAMING_SPHERE] || \
				  (ptr) == &mons[PM_FIRE_VAMPIRE] || \
				 likes_lava(ptr))


#define is_rat(ptr)		((ptr)->mflags4 & M4_RAT)
#define is_reflector(ptr)	((ptr)->mflags4 & M4_REFLECT)
#define is_amberite(ptr)	((ptr)->mflags4 & M4_AMBERITE)

#define nonliving(ptr)		(is_golem(ptr) || is_undead(ptr) || \
				 (ptr)->mlet == S_VORTEX || \
				 (ptr)->mlet == S_TURRET || \
				 (ptr)->mlet == S_TROVE || \
				 (ptr) == &mons[PM_MANES] || (ptr) == &mons[PM_SPECTRAL_WARRIOR])

#define touch_petrifies(ptr)	((ptr)->mflags3 & M3_PETRIFIES)

#define is_mind_flayer(ptr)	((ptr)->mflags3 & M3_IS_MIND_FLAYER)

#define made_of_rock(ptr)	((passes_walls(ptr) && thick_skinned(ptr)) || \
				 (ptr) == &mons[PM_STONE_GOLEM] || \
				 (ptr) == &mons[PM_LARGE_STONE_GOLEM] || \
				 (ptr) == &mons[PM_HUGE_STONE_GOLEM] || \
				 (ptr) == &mons[PM_GIANT_STONE_GOLEM] || \
				 (ptr) == &mons[PM_HIGH_END_STONE_GOLEM] || \
				 (ptr) == &mons[PM_UBER_STONE_GOLEM] || \
				 (ptr) == &mons[PM_ULTRA_STONE_GOLEM] || \
				 (ptr) == &mons[PM_STONE_STATUE] || \
				 (ptr) == &mons[PM_STATUE_GARGOYLE])
#define hates_silver(ptr)	(is_were(ptr) || is_vampire(ptr) || \
				 is_demon(ptr) || ((ptr)->mflags4 & M4_SHADE) || \
				 ((ptr)->mlet==S_IMP && (ptr) != &mons[PM_TENGU]))
#define is_shade(ptr)	((ptr)->mflags4 & M4_SHADE)
#define hates_viva(ptr)		(is_golem(ptr))
#define hates_copper(ptr)		((ptr)->mlet == S_FUNGUS)
#define hates_platinum(ptr)		(dmgtype(ptr, AD_CONT))
#define hates_cursed(ptr)		((ptr)->mlet == S_ANGEL)
#define hates_inka(ptr)		(is_animal(ptr))
#define hates_odor(ptr)		(is_animal(ptr) || humanoid(ptr))
/* Used for conduct with corpses, tins, and digestion attacks */
/* G_NOCORPSE monsters might still be swallowed as a purple worm */
/* Maybe someday this could be in mflags... */
#define vegan(ptr)		((ptr)->mlet == S_BLOB || \
				 (ptr)->mlet == S_JELLY ||            \
				 (ptr)->mlet == S_FUNGUS ||           \
				 (ptr)->mlet == S_VORTEX ||           \
				 (ptr)->mlet == S_TROVE ||           \
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
				 (ptr)->mlet == S_EEL ||            \
				 (ptr)->mlet == S_FLYFISH ||            \
				((ptr)->mlet == S_PUDDING &&         \
				 (ptr) != &mons[PM_BLACK_PUDDING]))
/* For vampires */
#define has_blood(ptr)		( (!vegetarian(ptr) || (ptr)->mlet == S_EEL || (ptr)->mlet == S_FLYFISH || Race_if(PM_SUCKING_FIEND) ) && \
				   (ptr)->mlet != S_GOLEM && \
				   (!is_undead(ptr) || is_vampire(ptr)))

#endif /* MONDATA_H */
