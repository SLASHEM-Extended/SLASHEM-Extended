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


/*** Resistances to troubles ***/
/* With intrinsics and extrinsics */
#define HFire_resistance	u.uprops[FIRE_RES].intrinsic
#define EFire_resistance	u.uprops[FIRE_RES].extrinsic
#define Fire_resistance		((HFire_resistance || EFire_resistance || \
				 resists_fire(&youmonst) || is_fire_resistant(youmonst.data)) && !Race_if(PM_TROLLOR) && !Role_if(PM_ALTMER) && !NoFire_resistance)
#define NoFire_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_FIRE_RES].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HCold_resistance	u.uprops[COLD_RES].intrinsic
#define ECold_resistance	u.uprops[COLD_RES].extrinsic
#define Cold_resistance		((HCold_resistance || ECold_resistance || \
				 resists_cold(&youmonst) || is_cold_resistant(youmonst.data)) && !Role_if(PM_ALTMER) && !NoCold_resistance )
#define NoCold_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_COLD_RES].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HSleep_resistance	u.uprops[SLEEP_RES].intrinsic
#define ESleep_resistance	u.uprops[SLEEP_RES].extrinsic
#define Sleep_resistance	( !Race_if(PM_KOBOLT) && !NoSleep_resistance && (HSleep_resistance || ESleep_resistance || \
				 resists_sleep(&youmonst) || is_sleep_resistant(youmonst.data)) )
#define NoSleep_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SLEEP_RES].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HDisint_resistance	u.uprops[DISINT_RES].intrinsic
#define EDisint_resistance	u.uprops[DISINT_RES].extrinsic
#define Disint_resistance	((HDisint_resistance || EDisint_resistance || \
				 resists_disint(&youmonst) || is_disint_resistant(youmonst.data)) && !NoDisint_resistance)
#define NoDisint_resistance (!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_DISINT_RES].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HShock_resistance	u.uprops[SHOCK_RES].intrinsic
#define EShock_resistance	u.uprops[SHOCK_RES].extrinsic
#define Shock_resistance	((HShock_resistance || EShock_resistance || \
				 resists_elec(&youmonst) || is_elec_resistant(youmonst.data)) && !Role_if(PM_ALTMER) && !NoShock_resistance )
#define NoShock_resistance (!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SHOCK_RES].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HPoison_resistance	u.uprops[POISON_RES].intrinsic
#define EPoison_resistance	u.uprops[POISON_RES].extrinsic
#define Poison_resistance	((HPoison_resistance || EPoison_resistance || \
				 resists_poison(&youmonst) || is_poison_resistant(youmonst.data)) && !NoPoison_resistance && !Race_if(PM_POISONER) )
#define NoPoison_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_POISON_RES].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HDrain_resistance	u.uprops[DRAIN_RES].intrinsic
#define EDrain_resistance	u.uprops[DRAIN_RES].extrinsic
#define Drain_resistance	((HDrain_resistance || EDrain_resistance || \
				 resists_drli(&youmonst) || is_drain_resistant(youmonst.data)) && !NoDrain_resistance)
#define NoDrain_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_DRAIN_RES].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

/* Intrinsics only */
#define HSick_resistance	u.uprops[SICK_RES].intrinsic
#define ESick_resistance	u.uprops[SICK_RES].extrinsic
#define Sick_resistance		((HSick_resistance || ESick_resistance || \
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
#define Antimagic		((HAntimagic || EAntimagic || \
				 (Upolyd && resists_magm(&youmonst))) && !NoAntimagic_resistance)
#define NoAntimagic_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_ANTIMAGIC].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HAcid_resistance	u.uprops[ACID_RES].intrinsic
#define EAcid_resistance	u.uprops[ACID_RES].extrinsic
#define Acid_resistance		((HAcid_resistance || EAcid_resistance || resists_acid(&youmonst) || is_acid_resistant(youmonst.data)) && !NoAcid_resistance)
#define NoAcid_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_ACID_RES].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HStone_resistance	u.uprops[STONE_RES].intrinsic
#define EStone_resistance	u.uprops[STONE_RES].extrinsic
#define Stone_resistance	((HStone_resistance || EStone_resistance || resists_ston(&youmonst) || is_ston_resistant(youmonst.data) || Role_if(PM_ROCKER) ) && !NoStone_resistance)
#define NoStone_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_STONE_RES].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HFear_resistance	u.uprops[FEAR_RES].intrinsic
#define EFear_resistance	u.uprops[FEAR_RES].extrinsic
#define Fear_resistance		((HFear_resistance || EFear_resistance) && !NoFear_resistance)
#define NoFear_resistance	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_FEAR_RES].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))


/*** Troubles ***/
/* Pseudo-property */
#define Punished		(uball)

/* Those implemented solely as timeouts (we use just intrinsic) */
#define HStun			u.uprops[STUNNED].intrinsic
#define EStun			u.uprops[STUNNED].extrinsic
#define Stunned			(HStun || EStun || HeavyStunned || ((u.umonnum == PM_STALKER || youmonst.data->mlet == S_BAT) && !Race_if(PM_TRANSFORMER) ) )
		/* Note: birds will also be stunned */
#define HeavyStunned		u.uprops[HEAVY_STUNNED].intrinsic

#define HConfusion		u.uprops[CONFUSION].intrinsic
#define EConfusion		u.uprops[CONFUSION].extrinsic
#define Confusion		(HConfusion || EConfusion || HeavyConfusion)
#define HeavyConfusion		u.uprops[HEAVY_CONFUSION].intrinsic

#define HNumbed		u.uprops[NUMBED].intrinsic
#define ENumbed		u.uprops[NUMBED].extrinsic
#define Numbed		(HNumbed || ENumbed || HeavyNumbed)
#define HeavyNumbed		u.uprops[HEAVY_NUMBED].intrinsic

#define HFeared		u.uprops[FEARED].intrinsic
#define EFeared		u.uprops[FEARED].extrinsic
#define Feared		(!strncmpi(plname, "Dudley", 6) || ((HFeared || EFeared || HeavyFeared) && !Fear_resistance) )
#define HeavyFeared		u.uprops[HEAVY_FEARED].intrinsic

#define HFrozen		u.uprops[FROZEN].intrinsic
#define EFrozen		u.uprops[FROZEN].extrinsic
#define Frozen		(HFrozen || EFrozen || HeavyFrozen)
#define HeavyFrozen		u.uprops[HEAVY_FROZEN].intrinsic

#define HBurned		u.uprops[BURNED].intrinsic
#define EBurned		u.uprops[BURNED].extrinsic
#define Burned		(HBurned || EBurned || HeavyBurned)
#define HeavyBurned		u.uprops[HEAVY_BURNED].intrinsic

#define Blinded			u.uprops[BLINDED].intrinsic
#define EBlinded			u.uprops[BLINDED].extrinsic
#define Blindfolded		(ublindf && ublindf->otyp != LENSES)
		/* ...means blind because of a cover */
#define Blind	((Blinded || EBlinded || Blindfolded || HeavyBlind || !strncmpi(plname, "Blindfox", 8) || (!haseyes(youmonst.data) && !Race_if(PM_TRANSFORMER) ) ) && \
		 !(ublindf && ublindf->oartifact == ART_EYES_OF_THE_OVERWORLD && strncmpi(plname, "Blindfox", 8) ))
		/* ...the Eyes operate even when you really are blind
		    or don't have any eyes */
/* added possibility of playing the entire game blind --Amy*/
#define HeavyBlind		u.uprops[HEAVY_BLIND].intrinsic

#define Sick			u.uprops[SICK].intrinsic
#define Stoned			u.uprops[STONED].intrinsic
#define Strangled		u.uprops[STRANGLED].intrinsic
#define Vomiting		u.uprops[VOMITING].intrinsic
#define Glib			u.uprops[GLIB].intrinsic
#define EGlib			u.uprops[GLIB].extrinsic
#define IsGlib			(Glib || EGlib)
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

/* Hallucination is solely a timeout; its resistance is extrinsic */
#define HHallucination		u.uprops[HALLUC].intrinsic
#define EHallucination		u.uprops[HALLUC].extrinsic
#define EHalluc_resistance	u.uprops[HALLUC_RES].extrinsic
#define Halluc_resistance	(EHalluc_resistance || \
				 (Upolyd && dmgtype(youmonst.data, AD_HALU)))
#define Hallucination		((HHallucination && !Halluc_resistance) || (HeavyHallu && !Halluc_resistance) || (EHallucination && !Halluc_resistance) || !strncmpi(plname, "Hippie", 6) )
/* added possibility of playing the entire game hallucinating --Amy*/
#define HeavyHallu		u.uprops[HEAVY_HALLU].intrinsic

/* Timeout, plus a worn mask */
#define HFumbling		u.uprops[FUMBLING].intrinsic
#define EFumbling		u.uprops[FUMBLING].extrinsic
#define Fumbling		(HFumbling || EFumbling)

#define HWounded_legs		u.uprops[WOUNDED_LEGS].intrinsic
#define EWounded_legs		u.uprops[WOUNDED_LEGS].extrinsic
#define Wounded_legs		(HWounded_legs || EWounded_legs)

#define HSleeping		u.uprops[SLEEPING].intrinsic
#define ESleeping		u.uprops[SLEEPING].extrinsic
#define Sleeping		(HSleeping || ESleeping || Race_if(PM_KOBOLT))

#define HHunger			u.uprops[HUNGER].intrinsic
#define EHunger			u.uprops[HUNGER].extrinsic
#define Hunger			(HHunger || EHunger || Race_if(PM_GIGANT))


/*** Vision and senses ***/
#define HSee_invisible		u.uprops[SEE_INVIS].intrinsic
#define ESee_invisible		u.uprops[SEE_INVIS].extrinsic
#define See_invisible		((HSee_invisible || ESee_invisible || \
				 perceives(youmonst.data)) && !NoSee_invisible)
#define NoSee_invisible	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SEE_INVIS].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HTelepat		u.uprops[TELEPAT].intrinsic
#define ETelepat		u.uprops[TELEPAT].extrinsic

#define Blind_telepat		( ((HTelepat || ETelepat || telepathic(youmonst.data)) && !NoTelepat && (!Role_if(PM_TOPMODEL) || !flags.female)) || ( Role_if(PM_TOPMODEL) && ((uarmf && uarmf->otyp == WEDGE_SANDALS) || (uarmf && uarmf->otyp == FEMININE_PUMPS) || (uarmf && uarmf->otyp == LEATHER_PEEP_TOES) || (uarmf && uarmf->otyp == HIPPIE_HEELS) || (uarmf && uarmf->otyp == COMBAT_STILETTOS) )))

#define Unblind_telepat		(ETelepat && !NoTelepat)
#define NoTelepat	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_TELEPAT].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

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

#define HWarning		u.uprops[WARNING].intrinsic
#define EWarning		u.uprops[WARNING].extrinsic
#define Warning			(( (HWarning || EWarning) && !NoWarning && (!Role_if(PM_TOPMODEL) || !flags.female)) || ( Role_if(PM_TOPMODEL) && ((uarmf && uarmf->otyp == WEDGE_SANDALS) || (uarmf && uarmf->otyp == FEMININE_PUMPS) || (uarmf && uarmf->otyp == LEATHER_PEEP_TOES) || (uarmf && uarmf->otyp == HIPPIE_HEELS) || (uarmf && uarmf->otyp == COMBAT_STILETTOS) )))
#define NoWarning	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_WARNING].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

/* Warning for a specific type of monster */
#define HWarn_of_mon		u.uprops[WARN_OF_MON].intrinsic
#define EWarn_of_mon		u.uprops[WARN_OF_MON].extrinsic
#define Warn_of_mon		(HWarn_of_mon || EWarn_of_mon)

#define HUndead_warning		u.uprops[WARN_UNDEAD].intrinsic
#define EUndead_warning		u.uprops[WARN_UNDEAD].intrinsic
#define Undead_warning		(HUndead_warning || EUndead_warning)

#define HSearching		u.uprops[SEARCHING].intrinsic
#define ESearching		u.uprops[SEARCHING].extrinsic
#define Searching		((HSearching || ESearching) && !NoSearching)
#define NoSearching	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_SEARCHING].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HClairvoyant		u.uprops[CLAIRVOYANT].intrinsic
#define EClairvoyant		u.uprops[CLAIRVOYANT].extrinsic
#define BClairvoyant		u.uprops[CLAIRVOYANT].blocked
#define Clairvoyant		((HClairvoyant || EClairvoyant) &&\
				 !BClairvoyant && !NoClairvoyant)
#define NoClairvoyant	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_CLAIRVOYANT].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HInfravision		u.uprops[INFRAVISION].intrinsic
#define EInfravision		u.uprops[INFRAVISION].extrinsic
#define Infravision		((HInfravision || EInfravision || \
				  infravision(youmonst.data)) && !NoInfravision)
#define NoInfravision	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_INFRAVISION].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HDetect_monsters	u.uprops[DETECT_MONSTERS].intrinsic
#define EDetect_monsters	u.uprops[DETECT_MONSTERS].extrinsic
#define Detect_monsters		((HDetect_monsters || EDetect_monsters) && !NoDetect_monsters)
#define NoDetect_monsters	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_DETECT_MONSTERS].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))


/*** Appearance and behavior ***/
#define Adornment		u.uprops[ADORNED].extrinsic

#define HInvis			u.uprops[INVIS].intrinsic
#define EInvis			u.uprops[INVIS].extrinsic
#define BInvis			u.uprops[INVIS].blocked
#define Invis			((HInvis || EInvis || \
				 pm_invisible(youmonst.data)) && !BInvis && !NoInvisible)
#define Invisible		(Invis && !See_invisible)
		/* Note: invisibility also hides inventory and steed */
#define NoInvisible	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_INVIS].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define EDisplaced		u.uprops[DISPLACED].extrinsic
#define Displaced		(EDisplaced && !NoDisplaced)
#define NoDisplaced	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_DISPLACED].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HStealth		u.uprops[STEALTH].intrinsic
#define EStealth		u.uprops[STEALTH].extrinsic
#define BStealth		u.uprops[STEALTH].blocked
#define Stealth			(( (HStealth || EStealth) && !BStealth && !NoStealth && !Race_if(PM_OGRO) && (!Role_if(PM_TOPMODEL) || !flags.female) ) || ( Role_if(PM_TOPMODEL) && ((uarmf && uarmf->otyp == WEDGE_SANDALS) || (uarmf && uarmf->otyp == FEMININE_PUMPS) || (uarmf && uarmf->otyp == LEATHER_PEEP_TOES) || (uarmf && uarmf->otyp == HIPPIE_HEELS) || (uarmf && uarmf->otyp == COMBAT_STILETTOS) )))
#define NoStealth	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_STEALTH].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HAggravate_monster	u.uprops[AGGRAVATE_MONSTER].intrinsic
#define EAggravate_monster	u.uprops[AGGRAVATE_MONSTER].extrinsic
#define Aggravate_monster	(HAggravate_monster || EAggravate_monster || Race_if(PM_OGRO) || (Role_if(PM_PICKPOCKET) && u.ualign.record < 0) )

#define HConflict		u.uprops[CONFLICT].intrinsic
#define EConflict		u.uprops[CONFLICT].extrinsic
#define Conflict		((HConflict || EConflict) && !Is_blackmarket(&u.uz) )
/* Sorry guys and girls, but you need to find another way to clear out Sam's assistants. --Amy */

/*** Transportation ***/
#define HJumping		u.uprops[JUMPING].intrinsic
#define EJumping		u.uprops[JUMPING].extrinsic
#define Jumping			((HJumping || EJumping || !strncmpi(plname, "IWBTG", 5) ) && !NoJumping)
#define NoJumping	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_JUMPING].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HTeleportation		u.uprops[TELEPORT].intrinsic
#define ETeleportation		u.uprops[TELEPORT].extrinsic
#define Teleportation		(HTeleportation || ETeleportation || \
				 can_teleport(youmonst.data) || Race_if(PM_MAIA) || Race_if(PM_HUMANOID_LEPRECHAUN))

#define HTeleport_control	u.uprops[TELEPORT_CONTROL].intrinsic
#define ETeleport_control	u.uprops[TELEPORT_CONTROL].extrinsic
#define Teleport_control	((HTeleport_control || ETeleport_control || \
				 control_teleport(youmonst.data)) && !Race_if(PM_MAIA) && !Race_if(PM_HUMANOID_LEPRECHAUN) && !NoTeleport_control)
#define NoTeleport_control	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_TELEPORT_CONTROL].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HLevitation		u.uprops[LEVITATION].intrinsic
#define ELevitation		u.uprops[LEVITATION].extrinsic
#define Levitation		(HLevitation || ELevitation || Race_if(PM_LEVITATOR) || \
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
# define Flying			(( (HFlying || EFlying || is_flyer(youmonst.data) || (is_floater(youmonst.data) && Race_if(PM_TRANSFORMER) ) || (u.usteed && is_flyer(u.usteed->data))) && !NoFlying && (!Role_if(PM_TOPMODEL) || !flags.female)) || ( Role_if(PM_TOPMODEL) && ((uarmf && uarmf->otyp == WEDGE_SANDALS) || (uarmf && uarmf->otyp == FEMININE_PUMPS) || (uarmf && uarmf->otyp == LEATHER_PEEP_TOES) || (uarmf && uarmf->otyp == HIPPIE_HEELS) || (uarmf && uarmf->otyp == COMBAT_STILETTOS) )))
#define NoFlying	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_FLYING].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#else
# define Flying			(( (HFlying || EFlying || is_flyer(youmonst.data) || (is_floater(youmonst.data) && Race_if(PM_TRANSFORMER) ) ) && !NoFlying && (!Role_if(PM_TOPMODEL))) || !flags.female) || ( Role_if(PM_TOPMODEL) && ((uarmf && uarmf->otyp == WEDGE_SANDALS) || (uarmf && uarmf->otyp == FEMININE_PUMPS) || (uarmf && uarmf->otyp == LEATHER_PEEP_TOES) || (uarmf && uarmf->otyp == HIPPIE_HEELS) || (uarmf && uarmf->otyp == COMBAT_STILETTOS) )))
#endif
	/* May touch surface; does not override any others */

#define Wwalking		(u.uprops[WWALKING].extrinsic && \
				 !Is_waterlevel(&u.uz))
	/* Don't get wet, can't go under water; overrides others except levitation */
	/* Wwalking is meaningless on water level */

#define HSwimming		u.uprops[SWIMMING].intrinsic
#define ESwimming		u.uprops[SWIMMING].extrinsic	/* [Tom] */
#ifdef STEED
# define Swimming		(HSwimming || ESwimming || \
				 is_swimmer(youmonst.data) || \
				 (u.usteed && is_swimmer(u.usteed->data)))
#else
# define Swimming		(HSwimming || ESwimming || \
				 is_swimmer(youmonst.data))
#endif
	/* Get wet, don't go under water unless if amphibious */

#define HMagical_breathing	u.uprops[MAGICAL_BREATHING].intrinsic
#define EMagical_breathing	u.uprops[MAGICAL_BREATHING].extrinsic
#define Amphibious		((HMagical_breathing || EMagical_breathing || \
				 amphibious(youmonst.data)) && !NoBreathless)
	/* Get wet, may go under surface */

#define Breathless		( ((HMagical_breathing || EMagical_breathing || breathless(youmonst.data)) && !NoBreathless && (!Role_if(PM_TOPMODEL) || !flags.female)) || ( Role_if(PM_TOPMODEL) && ((uarmf && uarmf->otyp == WEDGE_SANDALS) || (uarmf && uarmf->otyp == FEMININE_PUMPS) || (uarmf && uarmf->otyp == LEATHER_PEEP_TOES) || (uarmf && uarmf->otyp == HIPPIE_HEELS) || (uarmf && uarmf->otyp == COMBAT_STILETTOS) )))
#define NoBreathless	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_MAGICAL_BREATHING].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define Underwater		(u.uinwater)
/* Note that Underwater and u.uinwater are both used in code.
   The latter form is for later implementation of other in-water
   states, like swimming, wading, etc. */

#define HPasses_walls		u.uprops[PASSES_WALLS].intrinsic
#define EPasses_walls		u.uprops[PASSES_WALLS].extrinsic
#define Passes_walls		((HPasses_walls || EPasses_walls || \
				 passes_walls(youmonst.data)) && !NoPasses_walls)
#ifdef CONVICT
# define Phasing            u.uprops[PASSES_WALLS].intrinsic
#endif /* CONVICT */
#define NoPasses_walls	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_PASSES_WALLS].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))


/*** Physical attributes ***/
#define HSlow_digestion		u.uprops[SLOW_DIGESTION].intrinsic
#define ESlow_digestion		u.uprops[SLOW_DIGESTION].extrinsic
#define Slow_digestion		(!Race_if(PM_GIGANT) && !NoSlow_digestion && (HSlow_digestion || ESlow_digestion) )  /* KMH */
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
#define Regeneration		((HRegeneration || ERegeneration || \
				 regenerates(youmonst.data)) && !NoRegeneration)
#define NoRegeneration	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_REGENERATION].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HEnergy_regeneration	u.uprops[ENERGY_REGENERATION].intrinsic
#define EEnergy_regeneration	u.uprops[ENERGY_REGENERATION].extrinsic
#define Energy_regeneration	((HEnergy_regeneration || EEnergy_regeneration || Race_if(PM_REDGUARD) ) && !NoEnergy_regeneration)
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
#define Polymorph		(HPolymorph || EPolymorph || Race_if(PM_MOULD) || Race_if(PM_TRANSFORMER) || Race_if(PM_WORM_THAT_WALKS) || Race_if(PM_MISSINGNO) || Race_if(PM_UNGENOMOLD) || Race_if(PM_DEATHMOLD))

#define HPolymorph_control	u.uprops[POLYMORPH_CONTROL].intrinsic
#define EPolymorph_control	u.uprops[POLYMORPH_CONTROL].extrinsic
#define Polymorph_control	((HPolymorph_control || EPolymorph_control) && !Race_if(PM_MOULD) && !NoPolymorph_control && !Race_if(PM_TRANSFORMER) && !Race_if(PM_WORM_THAT_WALKS) && !Race_if(PM_MISSINGNO) && !Race_if(PM_UNGENOMOLD) && !Race_if(PM_DEATHMOLD) && !Race_if(PM_AK_THIEF_IS_DEAD_))
#define NoPolymorph_control	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HUnchanging		u.uprops[UNCHANGING].intrinsic
#define EUnchanging		u.uprops[UNCHANGING].extrinsic
#define Unchanging		((HUnchanging || EUnchanging) && !Race_if(PM_MOULD) && !Race_if(PM_MISSINGNO) && !Race_if(PM_TRANSFORMER) && !Race_if(PM_WORM_THAT_WALKS) && !Race_if(PM_UNGENOMOLD) && !Race_if(PM_DEATHMOLD) && !Race_if(PM_AK_THIEF_IS_DEAD_))	/* KMH */

#define HFast			u.uprops[FAST].intrinsic
#define EFast			u.uprops[FAST].extrinsic
#define Fast			( (HFast || EFast) && !NoFast && (!Role_if(PM_TRANSVESTITE) || flags.female) && (!Role_if(PM_TOPMODEL) || !flags.female) )
#define Very_fast		(( ( (HFast & ~INTRINSIC) || EFast) && !NoFast && (!Role_if(PM_TRANSVESTITE) || flags.female) && (!Role_if(PM_TOPMODEL) || !flags.female) ) || ( (Role_if(PM_TRANSVESTITE) || Role_if(PM_TOPMODEL) ) && ((uarmf && uarmf->otyp == WEDGE_SANDALS) || (uarmf && uarmf->otyp == FEMININE_PUMPS) || (uarmf && uarmf->otyp == LEATHER_PEEP_TOES) || (uarmf && uarmf->otyp == HIPPIE_HEELS) || (uarmf && uarmf->otyp == COMBAT_STILETTOS) ) ))
#define NoFast	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_FAST].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HReflecting		u.uprops[REFLECTING].intrinsic
#define EReflecting		u.uprops[REFLECTING].extrinsic
#define Reflecting		((EReflecting || HReflecting || \
				 (youmonst.data == &mons[PM_SILVER_DRAGON] ||\
				  youmonst.data == &mons[PM_CRYSTAL_GOLEM] ||\
				  youmonst.data == &mons[PM_SAPPHIRE_GOLEM] ||\
				  youmonst.data == &mons[PM_SHAMBLING_MOUND] ||\
				  youmonst.data == &mons[PM_SILVER_WOLF] ||\
				  youmonst.data == &mons[PM_VENOM_ATRONACH] ||\
				  youmonst.data == &mons[PM_DIVISION_THIEF] ||\
				  youmonst.data == &mons[PM_DIVISION_JEDI] ||\
				  youmonst.data == &mons[PM_ECM_NERVE_HEAD] ||\
				  youmonst.data == &mons[PM_ECM_ARCHER] ||\
				  youmonst.data == &mons[PM_ILLUSION_WEAVER] ||\
				  youmonst.data == &mons[PM_SEDUCER_SAINT] ||\
				  youmonst.data == &mons[PM_OLD_SILVER_DRAGON] ||\
				  youmonst.data == &mons[PM_VERY_OLD_SILVER_DRAGON] ||\
				  youmonst.data == &mons[PM_ANCIENT_SILVER_DRAGON] ||\
				  youmonst.data == &mons[PM_DIAMOND_GOLEM])) && !NoReflecting && !Race_if(PM_ANGBANDER) )
#define NoReflecting	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_REFLECTING].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define HFree_action	u.uprops[FREE_ACTION].intrinsic
#define EFree_action	u.uprops[FREE_ACTION].extrinsic
#define Free_action		((HFree_action || EFree_action) && !NoFree_action) /* [Tom] */
#define NoFree_action	(!Race_if(PM_IMMUNIZER) && (u.uprops[DEAC_FREE_ACTION].intrinsic || (Race_if(PM_HUMANOID_ANGEL) && u.ualign.record < 0)))

#define Fixed_abil		u.uprops[FIXED_ABIL].extrinsic	/* KMH */

#define Lifesaved		u.uprops[LIFESAVED].extrinsic


#endif /* YOUPROP_H */
