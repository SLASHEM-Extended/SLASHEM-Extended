/*	SCCS Id: @(#)artifact.c 3.4	2003/08/11	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "artifact.h"
#ifdef OVLB
#include "artilist.h"
#else
STATIC_DCL struct artifact artilist[];
#endif
/*
 * Note:  both artilist[] and artiexist[] have a dummy element #0,
 *	  so loops over them should normally start at #1.  The primary
 *	  exception is the save & restore code, which doesn't care about
 *	  the contents, just the total size.
 */


#ifndef OVLB

STATIC_DCL NEARDATA const short skill_names_indices[];
STATIC_DCL NEARDATA const char *odd_skill_names[];

#else	/* OVLB */

/* KMH, balance patch -- updated */
STATIC_OVL NEARDATA const short skill_names_indices[P_NUM_SKILLS] = {
	0,                DAGGER,         KNIFE,        AXE,
	PICK_AXE,         SHORT_SWORD,    BROADSWORD,   LONG_SWORD,
	TWO_HANDED_SWORD, SCIMITAR,       PN_SABER,     CLUB,
	PN_PADDLE,        MACE,           MORNING_STAR,   FLAIL,
	PN_HAMMER,        QUARTERSTAFF,   PN_POLEARMS,  SPEAR,
	JAVELIN,          TRIDENT,        LANCE,        BOW,
	SLING,            PN_FIREARMS,    CROSSBOW,       DART,
	SHURIKEN,         BOOMERANG,      PN_WHIP,      UNICORN_HORN,
	PN_LIGHTSABER,
	PN_ATTACK_SPELL,     PN_HEALING_SPELL,
	PN_DIVINATION_SPELL, PN_ENCHANTMENT_SPELL,
	PN_PROTECTION_SPELL,            PN_BODY_SPELL,
	PN_OCCULT_SPELL,
	PN_ELEMENTAL_SPELL,
	PN_CHAOS_SPELL,
	PN_MATTER_SPELL,
	PN_BARE_HANDED,	PN_HIGH_HEELS,
	PN_GENERAL_COMBAT,	PN_SHIELD,	PN_BODY_ARMOR,
	PN_TWO_HANDED_WEAPON,	PN_POLYMORPHING,	PN_DEVICES,
	PN_SEARCHING,	PN_SPIRITUALITY,	PN_PETKEEPING,
	PN_MISSILE_WEAPONS,	PN_TECHNIQUES,	PN_IMPLANTS,	PN_SEXY_FLATS,
	PN_MEMORIZATION,	PN_GUN_CONTROL,	PN_SQUEAKING,	PN_SYMBIOSIS,
	PN_SHII_CHO,	PN_MAKASHI,	PN_SORESU,
	PN_ATARU,	PN_SHIEN,	PN_DJEM_SO,
	PN_NIMAN,	PN_JUYO,	PN_VAAPAD,	PN_WEDI,
	PN_MARTIAL_ARTS, 
	PN_TWO_WEAPONS,
	PN_RIDING,
};


STATIC_OVL NEARDATA const char * const odd_skill_names[] = {
    "no skill",
    "polearms",
    "saber",
    "hammer",
    "whip",
    "paddle",
    "firearms",
    "attack spells",
    "healing spells",
    "divination spells",
    "enchantment spells",
    "protection spells",
    "body spells",
    "occult spells",
    "elemental spells",
    "chaos spells",
    "matter spells",
    "bare-handed combat",
    "high heels",
    "general combat",
    "shield",
    "body armor",
    "two-handed weapons",
    "polymorphing",
    "devices",
    "searching",
    "spirituality",
    "petkeeping",
    "missile weapons",
    "techniques",
    "implants",
    "sexy flats",
    "memorization",
    "gun control",
    "squeaking",
    "symbiosis",
    "form I (Shii-Cho)",
    "form II (Makashi)",
    "form III (Soresu)",
    "form IV (Ataru)",
    "form V (Shien)",
    "form V (Djem So)",
    "form VI (Niman)",
    "form VII (Juyo)",
    "form VII (Vaapad)",
    "form VIII (Wedi)",
    "martial arts",
    "riding",
    "two-weapon combat",
    "lightsaber"
};

#endif	/* OVLB */

#define P_NAME(type) (skill_names_indices[type] > 0 ? \
		      OBJ_NAME(objects[skill_names_indices[type]]) : \
			odd_skill_names[-skill_names_indices[type]])

extern boolean notonhead;	/* for long worms */

#define get_artifact(o) \
		(((o)&&(o)->oartifact) ? &artilist[(int) ((unsigned int) (o)->oartifact)] : 0)
/* big thanks to Chris, I'd never have found out that the "unsigned char" part would fix the bug where
 * any artifacts after #127 were segfaulting and giving random effects instead of the desired ones... --Amy */

STATIC_DCL int spec_applies(const struct artifact *,struct monst *);
STATIC_DCL int arti_invoke(struct obj*);
STATIC_DCL boolean Mb_hit(struct monst *magr,struct monst *mdef,
			  struct obj *,int *,int,BOOLEAN_P,char *);

/* The amount added to the victim's total hit points to insure that the
   victim will be killed even after damage bonus/penalty adjustments.
   Most such penalties are small, and 200 is plenty; the exception is
   half physical damage.  3.3.1 and previous versions tried to use a very
   large number to account for this case; now, we just compute the fatal
   damage by adding it to 2 times the total hit points instead of 1 time.
   Note: this will still break if they have more than about half the number
   of hit points that will fit in a 15 bit integer. */
#define FATAL_DAMAGE_MODIFIER 200

#ifndef OVLB
STATIC_DCL int spec_dbon_applies;
STATIC_DCL int artidisco[NROFARTIFACTS];
#else	/* OVLB */
/* coordinate effects from spec_dbon() with messages in artifact_hit() */
STATIC_OVL int spec_dbon_applies = 0;

/* flags including which artifacts have already been created */
static boolean artiexist[1+NROFARTIFACTS+1];
/* and a discovery list for them (no dummy first entry here) */

STATIC_OVL int artidisco[NROFARTIFACTS];

STATIC_DCL void hack_artifacts(void);
STATIC_DCL boolean attacks(int,struct obj *);


STATIC_PTR void
do_terrainfloodP(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}

	if (In_sokoban(&u.uz) && rn2(5)) return;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = randomwalltype();
			blockorunblock_point(randomx,randomy);
			if (!(levl[randomx][randomy].wall_info & W_EASYGROWTH)) levl[randomx][randomy].wall_info |= W_HARDGROWTH;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = randomwalltype();
		blockorunblock_point(x,y);
		if (!(levl[x][y].wall_info & W_EASYGROWTH)) levl[x][y].wall_info |= W_HARDGROWTH;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}


/* handle some special cases; must be called after role_init() */
STATIC_OVL void
hack_artifacts()
{
	struct artifact *art;
	int alignmnt = aligns[flags.initalign].value;

	/* Fix up the alignments of "gift" artifacts */
	for (art = artilist+1; art->otyp; art++)
	    if (art->role == Role_switch && art->alignment != A_NONE)
		art->alignment = alignmnt;

	/* Excalibur can be used by any lawful character, not just knights */
	if (!Role_if(PM_KNIGHT))
	    artilist[ART_EXCALIBUR].role = NON_PM;

	/* randarts - the code in this file is from dnethack (thanks Chris) with adaptations --Amy */
	/* base item: LONG_SWORD = melee weapon, BOW = launcher, DART = missile, LEATHER_ARMOR = suit,
	 * VICTORIAN_UNDERWEAR = shirt, ORCISH_CLOAK = cloak, HELMET = helm, LEATHER_GLOVES = gloves,
	 * SHIELD = shield, LOW_BOOTS = boots, RIN_AGGRAVATE_MONSTER = ring, AMULET_OF_STRANGULATION = amulet,
	 * WAN_MAKE_INVISIBLE = wand, SPE_MAGICTORCH = spellbook */

	artilist[ART_SOULCALIBUR].otyp = randartmeleeweapon();
	artilist[ART_UNDEADBANE].otyp = randartmeleeweapon();
	artilist[ART_RAINBOWSWANDIR].otyp = randartmeleeweapon();
	artilist[ART_WIZARDBANE].otyp = randartmeleeweapon();
	artilist[ART_VORPAL_EDGE].otyp = randartmeleeweapon();
	artilist[ART_DARK_MAGIC].otyp = randartsuit();
	artilist[ART_BEAM_CONTROL].otyp = randartmeleeweapon();
	artilist[ART_SANDRA_S_SECRET_WEAPON].otyp = randartmeleeweapon();
	artilist[ART_DUMBOAK_S_HEW].otyp = randartmeleeweapon();
	artilist[ART_POWER_AMMO].otyp = randartmissile();
	artilist[ART_BLOBLOBLOBLOBLO].otyp = randartlauncher();
	artilist[ART_PSCHIUDITT].otyp = randartlauncher();
	artilist[ART_RATTATTATTATTATT].otyp = randartlauncher();
	artilist[ART_FLAM_R].otyp = randartlauncher();
	artilist[ART_SURESHOT].otyp = randartlauncher();
	artilist[ART_STINGWING].otyp = randartmissile();
	artilist[ART_NOBILE_MOBILITY].otyp = randartshirt();
	artilist[ART_ANTIMAGIC_FIELD].otyp = randartshirt();
	artilist[ART_NATALIA_IS_LOVELY_BUT_DANG].otyp = randartshirt();
	artilist[ART_TAPE_ARMAMENT].otyp = randartsuit();
	artilist[ART_CATHAN_S_SIGIL].otyp = randartring();
	artilist[ART_VERSION_CONTROL].otyp = randartring();
	artilist[ART_FLEEING_MINE_MAIL].otyp = randartsuit();
	artilist[ART_GREY_FUCKERY].otyp = randartsuit();
	artilist[ART_LITTLE_PENIS_WANKER].otyp = randartsuit();
	artilist[ART_D_TYPE_EQUIPMENT].otyp = randartsuit();
	artilist[ART_INCREDIBLE_SWEETNESS].otyp = randartsuit();
	artilist[ART_QUEEN_ARTICUNO_S_HULL].otyp = randartsuit();
	artilist[ART_DON_SUICUNE_USED_SELFDESTR].otyp = randartsuit();
	artilist[ART_WONDERCLOAK].otyp = randartcloak();
	artilist[ART_EVELINE_S_CIVIL_MANTLE].otyp = randartcloak();
	artilist[ART_INA_S_OVERCOAT].otyp = randartcloak();
	artilist[ART_GROUNDBUMMER].otyp = randartcloak();
	artilist[ART_RITA_S_LOVELY_OVERGARMENT].otyp = randartcloak();
	artilist[ART_LUNAR_ECLIPSE_TONIGHT].otyp = randartcloak();
	artilist[ART_LORSKEL_S_SPEED].otyp = randarthelm();
	artilist[ART____DOT__ALIEN_RADIO].otyp = randarthelm();
	artilist[ART_NADJA_S_DARKNESS_GENERATOR].otyp = randarthelm();
	artilist[ART_THA_WALL].otyp = randartsuit();
	artilist[ART_LLLLLLLLLLLLLM].otyp = randarthelm();
	artilist[ART_ARABELLA_S_GREAT_BANISHER].otyp = randartgloves();
	artilist[ART_NO_FUTURE_BUT_AGONY].otyp = randartshield();
	artilist[ART_BONUS_HOLD].otyp = randartshield();
	artilist[ART_GREXIT_IS_NEAR].otyp = randartshield();
	artilist[ART_REAL_MEN_WEAR_PSYCHOS].otyp = randartshield();
	artilist[ART_AMYBSOD_S_NEW_FOOTWEAR].otyp = randartboots();
	artilist[ART_MANUELA_S_UNKNOWN_HEELS].otyp = randartboots();
	artilist[ART_HADES_THE_MEANIE].otyp = randartboots();
	artilist[ART_AMY_LOVES_AUTOCURSING_ITEM].otyp = randartboots();
	artilist[ART_ALLYNONE].otyp = randartboots();
	artilist[ART_KHOR_S_REQUIRED_IDEA].otyp = randartboots();
	artilist[ART_ERROR_IN_PLAY_ENCHANTMENT].otyp = randartboots();
	artilist[ART_WHOA_HOLD_ON_DUDE].otyp = randartring();
	artilist[ART_ACHROMANTIC_RING].otyp = randartring();
	artilist[ART_GOLDENIVY_S_ENGAGEMENT_RIN].otyp = randartring();
	artilist[ART_TYRANITAR_S_OWN_GAME].otyp = randartamulet();
	artilist[ART_ONE_MOMENT_IN_TIME].otyp = randartamulet();
	artilist[ART_BUEING].otyp = randartamulet();
	artilist[ART_NAZGUL_S_REVENGE].otyp = randartamulet();
	artilist[ART_HARRY_S_BLACKTHORN_WAND].otyp = randartwand();
	artilist[ART_PROFESSOR_SNAPE_S_DILDO].otyp = randartwand();
	artilist[ART_FRENCH_MAGICAL_DEVICE].otyp = randartwand();
	artilist[ART_SAGGITTII].otyp = randartmissile();
	artilist[ART_BENTSHOT].otyp = randartlauncher();
	artilist[ART_JELLYWHACK].otyp = randartmeleeweapon();
	artilist[ART_ONE_THROUGH_FOUR_SCEPTER].otyp = randartmeleeweapon();
	artilist[ART_AL_CANONE].otyp = randartlauncher();
	artilist[ART_VEIL_OF_MINISTRY].otyp = randartcloak();
	artilist[ART_ZANKAI_HUNG_ZE_TUNG_DO_HAI].otyp = randartmeleeweapon();
	artilist[ART_AWKWARDNESS].otyp = randartmeleeweapon();
	artilist[ART_SCHWANZUS_LANGUS].otyp = randartmeleeweapon();
	artilist[ART_TRAP_DUNGEON_OF_SHAMBHALA].otyp = randartshirt();
	artilist[ART_ZERO_PERCENT_FAILURE].otyp = randarthelm();
	artilist[ART_HENRIETTA_S_HEAVY_CASTER].otyp = randartcloak();
	artilist[ART_ROFLCOPTER_WEB].otyp = randartsuit();
	artilist[ART_SHIVANHUNTER_S_UNUSED_PRIZ].otyp = randartsuit();
	artilist[ART_ARABELLA_S_ARTIFACT_CREATI].otyp = randartmeleeweapon();
	artilist[ART_TIARA_OF_AMNESIA].otyp = randarthelm();
	artilist[ART_FLUE_FLUE_FLUEFLUE_FLUE].otyp = randarthelm();
	artilist[ART_LIXERTYPIE].otyp = randartmeleeweapon();
	artilist[ART_SAMENESS_OF_CHRIS].otyp = randartmeleeweapon();
	artilist[ART_DONALD_TRUMP_S_RAGE].otyp = randartmeleeweapon();
	artilist[ART_PRICK_PASS].otyp = randartmeleeweapon();
	artilist[ART_THRANDUIL_LOSSEHELIN].otyp = randartmeleeweapon();
	artilist[ART_FEANARO_SINGOLLO].otyp = randartmeleeweapon();
	artilist[ART_WINSETT_S_BIG_DADDY].otyp = randartlauncher();
	artilist[ART_FEMINIST_GIRL_S_PURPLE_WEA].otyp = randartshirt();
	artilist[ART_LEA_S_SPOKESWOMAN_UNIFORM].otyp = randartcloak();
	artilist[ART_HERETICAL_FIGURE].otyp = randartcloak();
	artilist[ART_JANA_S_SECRET_CAR].otyp = randartcloak();
	artilist[ART_UNIMPLEMENTED_FEATURE].otyp = randarthelm();
	artilist[ART_FLAT_INSANITY].otyp = randarthelm();
	artilist[ART_FREEZE_OR_YES].otyp = randartgloves();
	artilist[ART_PRINCESS_BITCH].otyp = randartgloves();
	artilist[ART_WOULD_YOU_RAIGHT_THAT].otyp = randartgloves();
	artilist[ART_DIFFICULTY__].otyp = randartgloves();
	artilist[ART_SWARM_SOFT_HIGH_HEELS].otyp = randartboots();
	artilist[ART_WEAK_FROM_HUNGER].otyp = randartboots();
	artilist[ART_ARABELLA_S_RESIST_COLD].otyp = randartring();
	artilist[ART_RATSCH_WATSCH].otyp = randartamulet();
	artilist[ART_ARABELLA_S_PRECIOUS_GADGET].otyp = randartamulet();
	artilist[ART_ARABELLA_S_WARDING_HOE].otyp = randartmeleeweapon();
	artilist[ART_SHAPETAKE_NUMBER_FIVE].otyp = randartcloak();
	artilist[ART_ARABELLA_S_WAND_BOOSTER].otyp = randartcloak();
	artilist[ART_INTELLIGENT_POPE].otyp = randartlauncher();
	artilist[ART_RNG_S_PRIDE].otyp = randartsuit();
	artilist[ART_RNG_S_JOY].otyp = randartgloves();
	artilist[ART_RNG_S_SEXINESS].otyp = randartboots();
	artilist[ART_RNG_S_EMBRACE].otyp = randartcloak();
	artilist[ART_RNG_S_GRIMACE].otyp = randarthelm();
	artilist[ART_COMPLETE_MON_DIEU].otyp = randartspellbook();
	artilist[ART_AGATHE_BAUER].otyp = randartspellbook();
	artilist[ART_ANNELIESE_BROWN].otyp = randartspellbook();
	artilist[ART_I_WILL_THINK_ABOUT_YOU].otyp = randartspellbook();
	artilist[ART_DEL_OLELONG].otyp = randartspellbook();
	artilist[ART_JUBELJUBIJEEAH].otyp = randartspellbook();
	artilist[ART_DUEDELDUEDELDUEDELDUEDELDU].otyp = randartspellbook();
	artilist[ART_CAUSE_I_M_A_CHEATER].otyp = randartspellbook();
	artilist[ART_BATMAN_NIGHT].otyp = randartspellbook();
	artilist[ART_NIKKENIKKENIK].otyp = randartspellbook();

	artilist[ART_JANA_S_GRAVE_WALL].otyp = randartcloakX();
	artilist[ART_HENRIETTA_S_DOGSHIT_BOOTS].otyp = randartbootsX();
	artilist[ART_FIREPROOF_WALL].otyp = randartmeleeweaponX();
	artilist[ART_SPEARBLADE].otyp = randartmeleeweaponX();
	artilist[ART_RADIATOR_AREA].otyp = randartmeleeweaponX();
	artilist[ART_JESSICA_S_WINNING_STRIKE].otyp = randartmeleeweaponX();
	artilist[ART_MARKUS_S_JUSTICE].otyp = randartmeleeweaponX();
	artilist[ART_KATHARINA_S_MELEE_PROWESS].otyp = randartmeleeweaponX();
	artilist[ART_LICHBANE].otyp = randartmeleeweaponX();
	artilist[ART_FORKED_TONGUE].otyp = randartmeleeweaponX();
	artilist[ART_ALL_SERIOUSNESS].otyp = randartmeleeweaponX();
	artilist[ART_SPECIAL_LACK].otyp = randartmeleeweaponX();
	artilist[ART_WING_WING].otyp = randartmeleeweaponX();
	artilist[ART_CARMOUFALSCH].otyp = randartspellbookX();
	artilist[ART_WIE_ES_AUCH_SEI].otyp = randartspellbookX();
	artilist[ART_MORTON_THEIRS_OF_RAVEL_RAD].otyp = randartspellbookX();
	artilist[ART_DEEP_FRIENDS].otyp = randartspellbookX();
	artilist[ART_HAE_HAE_HIIII].otyp = randartspellbookX();
	artilist[ART_FOR_MOMMY_EVER_FORSELESSAU].otyp = randartspellbookX();
	artilist[ART_LAWFIRE].otyp = randartspellbookX();
	artilist[ART_WAE_WAE_WAE_DAEDELDAEDELDA].otyp = randartspellbookX();
	artilist[ART_PRESIDENT_SCHIESSKANISTA].otyp = randartspellbookX();
	artilist[ART_KNBLOELOELOELODRIO].otyp = randartspellbookX();
	artilist[ART_DESERT_MAID].otyp = randartmeleeweaponX();
	artilist[ART_CYGNISWAN].otyp = randartmeleeweaponX();
	artilist[ART_TALKATOR].otyp = randartmeleeweaponX();
	artilist[ART_MAGESOOZE].otyp = randartmeleeweaponX();
	artilist[ART_RESISTANT_RESISTOR].otyp = randartmeleeweaponX();
	artilist[ART_VERNON_S_POTTERBASHER].otyp = randartmeleeweaponX();
	artilist[ART_SCHWOINGELOINGELOING_OOOAR].otyp = randartmeleeweaponX();
	artilist[ART_FEMMY_S_LASH].otyp = randartmeleeweaponX();
	artilist[ART_CASQUE_OUTLOOK].otyp = randartmeleeweaponX();
	artilist[ART_UNFAIR_PEE].otyp = randartmeleeweaponX();
	artilist[ART_SEA_CAPTAIN_PIERCER].otyp = randartmeleeweaponX();
	artilist[ART_END_OF_LOOK_WORSE].otyp = randartmeleeweaponX();
	artilist[ART_SPACE_BEGINS_AFTER_HERE].otyp = randartmeleeweaponX();
	artilist[ART_CORINA_S_THUNDER].otyp = randartmeleeweaponX();
	artilist[ART_INNER_TUBE].otyp = randartmeleeweaponX();
	artilist[ART_SOLO_SLACKER].otyp = randartlauncherX();
	artilist[ART_AMMO_OF_THE_MACHINE].otyp = randartmissileX();
	artilist[ART_DAE_OE_OE_OE_OE_OE].otyp = randartlauncherX();
	artilist[ART_CANNONEER].otyp = randartlauncherX();
	artilist[ART_SPEEDHACK].otyp = randartlauncherX();
	artilist[ART_EARTH_GAS_GUN].otyp = randartlauncherX();
	artilist[ART_FIRE_ALREADY].otyp = randartmissileX();
	artilist[ART_HUMAN_WIPEOUT].otyp = randartmeleeweaponX();
	artilist[ART_SPLINTER_ARMAMENT].otyp = randartsuitX();
	artilist[ART_ABSOLUTE_MONSTER_MAIL].otyp = randartsuitX();
	artilist[ART_RITA_S_TENDER_STILETTOS].otyp = randartbootsX();
	artilist[ART_HALF_MOON_TONIGHT].otyp = randartcloakX();
	artilist[ART_PANTAP].otyp = randartcloakX();
	artilist[ART_RUTH_S_DARK_FORCE].otyp = randarthelmX();
	artilist[ART_HAMBURG_ONE].otyp = randarthelmX();
	artilist[ART_ARABELLA_S_MELEE_POWER].otyp = randartmeleeweaponX();
	artilist[ART_ASBESTOS_MATERIAL].otyp = randartmeleeweaponX();
	artilist[ART_TANKS_A_LOT].otyp = randartglovesX();
	artilist[ART_DIMVISION].otyp = randartglovesX();
	artilist[ART_I_M_GETTING_HUNGRY].otyp = randartshieldX();
	artilist[ART_CCC_CCC_CCCCCCC].otyp = randartshieldX();
	artilist[ART_FIVE_STAR_PARTY].otyp = randartshieldX();
	artilist[ART_GUDRUN_S_STOMPING].otyp = randartbootsX();
	artilist[ART_GOEFFELBOEFFEL].otyp = randartbootsX();
	artilist[ART_TEMPERATOR].otyp = randartbootsX();
	artilist[ART_GREEN_COLOR].otyp = randartringX();
	artilist[ART_SCARAB_OF_ADORNMENT].otyp = randartringX();
	artilist[ART_SCHWUEU].otyp = randartamuletX();
	artilist[ART_FULLY_THE_LONG_PENIS].otyp = randartwandX();
	artilist[ART_WORLD_OF_COQ].otyp = randartwandX();
	artilist[ART_WHOOSHZAP].otyp = randartwandX();
	artilist[ART_NADJA_S_SILENCE].otyp = randartmeleeweaponX();
	artilist[ART_A_SWORD_NAMED_SWORD].otyp = randartmeleeweaponX();
	artilist[ART_HERITAGE_IGNORER].otyp = randartmeleeweaponX();
	artilist[ART_MIMICBANE].otyp = randartmeleeweaponX();
	artilist[ART_HAHAHAHAHAHAHAAAAAAAAAAAA].otyp = randartmeleeweaponX();
	artilist[ART_POISON_PEN_LETTER].otyp = randartmeleeweaponX();
	artilist[ART_SUNALI_S_SUMMONING_STORM].otyp = randartbootsX();
	artilist[ART_FILTHY_PRESS].otyp = randartlauncherX();
	artilist[ART_MUB_PUH_MUB_DIT_DIT].otyp = randartmeleeweaponX();
	artilist[ART_DONNNNNNNNNNNNG].otyp = randartmeleeweaponX();
	artilist[ART_PROVOCATEUR].otyp = randartmeleeweaponX();
	artilist[ART_FOEOEOEOEOEOEOE].otyp = randartlauncherX();
	artilist[ART_NEVER_WILL_THIS_BE_USEFUL].otyp = randartmeleeweaponX();
	artilist[ART_QUARRY].otyp = randartsuitX();
	artilist[ART_CONNY_S_COMBAT_COAT].otyp = randartcloakX();
	artilist[ART_ACIDSHOCK_CASTLECRUSHER].otyp = randartcloakX();
	artilist[ART_LAURA_S_SWIMSUIT].otyp = randartcloakX();
	artilist[ART_PROTECT_WHAT_CANNOT_BE_PRO].otyp = randartcloakX();
	artilist[ART_GIRLFUL_FARTING_NOISES].otyp = randarthelmX();
	artilist[ART_YOU_SEE_HERE_AN_ARTIFACT].otyp = randarthelmX();
	artilist[ART_NUMB_OR_MAYBE].otyp = randartglovesX();
	artilist[ART_DEAD_SLAM_THE_TIME_SHUT].otyp = randartglovesX();
	artilist[ART_ANASTASIA_S_UNEXPECTED_ABI].otyp = randartbootsX();
	artilist[ART_ELIANE_S_SHIN_SMASH].otyp = randartbootsX();
	artilist[ART_MYSTERIOUS_MAGIC].otyp = randartamuletX();
	artilist[ART_BANGCOCK].otyp = randartlauncherX();
	artilist[ART_RNG_S_COMPLETION].otyp = randartsuitX();
	artilist[ART_RNG_S_BEAUTY].otyp = randartbootsX();
	artilist[ART_RNG_S_SAFEGUARD].otyp = randartshieldX();
	artilist[ART_BLACK_DARKNESS].otyp = randartamuletX();
	artilist[ART_FLEECY_GREEN].otyp = randartamuletX();
	artilist[ART_PEEK].otyp = randartmeleeweaponX();
	artilist[ART_TAILCUTTER].otyp = randartmeleeweaponX();

	artilist[ART_ELLI_S_PSEUDOBAND_OF_POS].otyp = randartmeleeweaponX();
	artilist[ART_HIGHEST_FEELING].otyp = find_fetish_heels();
	artilist[ART_LORSKEL_S_INTEGRITY].otyp = find_lolita_boots();

	artilist[ART_PHANTOM_OF_THE_OPERA].otyp = find_opera_cloak();
	artilist[ART_HIGH_DESIRE_OF_FATALITY].otyp = find_explosive_boots();
	artilist[ART_CHOICE_OF_MATTER].otyp = find_irregular_boots();
	artilist[ART_MELISSA_S_BEAUTY].otyp = find_wedge_boots();
	artilist[ART_CORINA_S_SNOWY_TREAD].otyp = find_skates3();
	artilist[ART_NUMBER___].otyp = find_aluminium_helmet();
	artilist[ART_HAUNTNIGHT].otyp = find_ghostly_cloak();
	artilist[ART_LORSKEL_S_SPECIAL_PROTECTI].otyp = find_polnish_gloves();
	artilist[ART_ROBBERY_GONE_RIGHT].otyp = find_polnish_gloves();
	artilist[ART_JOSEFINE_S_EVILNESS].otyp = find_velcro_boots();
	artilist[ART_WHINY_MARY].otyp = find_clumsy_gloves();
	artilist[ART_WARP_SPEED].otyp = find_fin_boots();
	artilist[ART_GRENEUVENIA_S_HUG].otyp = find_profiled_boots();
	artilist[ART_SHELLY].otyp = find_void_cloak();
	artilist[ART_SPREAD_YOUR_LEGS_WIDE].otyp = find_grey_shaded_gloves();
	artilist[ART_GREEB].otyp = find_weeb_cloak();
	artilist[ART_PRINCE_OF_PERSIA].otyp = find_persian_boots();
	artilist[ART_ANASTASIA_S_PLAYFULNESS].otyp = find_hugging_boots();
	artilist[ART_KATIE_MELUA_S_FEMALE_WEAPO].otyp = find_fleecy_boots();
	artilist[ART_COCKUETRY].otyp = find_fingerless_gloves();
	artilist[ART_PERCENTIOEOEPSPERCENTD_THI].otyp = find_mantle_of_coat();
	artilist[ART_PEEPING_GROOVE].otyp = find_fatal_gloves();
	artilist[ART_RARE_ASIAN_LADY].otyp = find_beautiful_heels();
	artilist[ART_JANA_S_FAIRNESS_CUP].otyp = find_homicidal_cloak();
	artilist[ART_OUT_OF_TIME].otyp = find_castlevania_boots();
	artilist[ART_PALEOLITHIC_ELBOW_CONTRACT].otyp = find_greek_cloak();
	artilist[ART_NUCLEAR_BOMB].otyp = find_celtic_helmet();
	artilist[ART_BEEEEEEEANPOLE].otyp = find_english_gloves();
	artilist[ART_LEGMA].otyp = find_korean_sandals();
	artilist[ART_TERRY_PRATCHETT_S_INGENUIT].otyp = find_octarine_robe();
	artilist[ART_ARABELLA_S_SEXY_GIRL_BUTT].otyp = find_chinese_cloak();
	artilist[ART_LONG_LASTING_JOY].otyp = find_polyform_cloak();
	artilist[ART_LIGHT_ABSORPTION].otyp = find_absorbing_cloak();
	artilist[ART_CATHERINE_S_SEXUALITY].otyp = find_birthcloth();
	artilist[ART_POKEWALKER].otyp = find_poke_mongo_cloak();
	artilist[ART_WINDS_OF_CHANGE].otyp = find_levuntation_cloak();
	artilist[ART_LIGHTSPEED_TRAVEL].otyp = find_quicktravel_cloak();
	artilist[ART_T_O_M_E].otyp = find_angband_cloak();
	artilist[ART_FEMMY_FATALE].otyp = find_anorexia_cloak();
	artilist[ART_ARTIFICIAL_FAKE_DIFFICULTY].otyp = find_dnethack_cloak();
	artilist[ART_JUNETHACK______WINNER].otyp = find_team_splat_cloak();
	artilist[ART_YOG_SOTHOTH_HELP_ME].otyp = find_eldritch_cloak();
	artilist[ART_LUISA_S_IRRESISTIBLE_CHARM].otyp = find_erotic_boots();
	artilist[ART_JANA_S_DECEPTIVE_MASK].otyp = find_secret_helmet();
	artilist[ART_NOW_IT_BECOMES_DIFFERENT].otyp = find_difficult_cloak();
	artilist[ART_NATASCHA_S_STROKING_UNITS].otyp = find_velvet_gloves();
	artilist[ART_SPEAK_TO_OJ].otyp = find_sputa_boots();
	artilist[ART_DUE_DUE_DUE_DUE_BRMMMMMMM].otyp = find_formula_one_helmet();
	artilist[ART_TOILET_NOISES].otyp = find_excrement_cloak();
	artilist[ART_LINE_CAN_PLAY_BY_YOURSELF].otyp = find_racer_gloves();
	artilist[ART_TOO_FAST__TOO_FURIOUS].otyp = find_turbo_boots();
	artilist[ART_NOUROFIBROMA].otyp = find_guild_cloak();
	artilist[ART_MADELINE_S_STUPID_GIRL].otyp = find_shitty_gloves();

	artilist[ART_JANA_S_ROULETTE_OF_LIFE].otyp = find_foundry_cloak();
	artilist[ART_MAGIC_JUGGULATE].otyp = find_spellsucking_cloak();
	artilist[ART_HIGH_KING_OF_SKIRIM].otyp = find_storm_coat();
	artilist[ART_ALLCOLOR_PRISM].otyp = find_fleeceling_cloak();
	artilist[ART_MARY_INSCRIPTION].otyp = find_princess_gloves();
	artilist[ART_FATHIEN_ELDER_S_SECRET_POW].otyp = find_uncanny_gloves();
	artilist[ART_SI_OH_WEE].otyp = find_slaying_gloves();
	artilist[ART_JOHANETTA_S_ROUGH_GENTLENE].otyp = find_blue_sneakers();
	artilist[ART_JANA_S_VAGINAL_FUN].otyp = find_blue_sneakers();
	artilist[ART_VERY_NICE_PERSON].otyp = find_femmy_boots();
	artilist[ART_JULIA_S_REAL_LOVE].otyp = find_red_sneakers();
	artilist[ART_ELIANE_S_COMBAT_SNEAKERS].otyp = find_yellow_sneakers();
	artilist[ART_MAILIE_S_CHALLENGE].otyp = find_pink_sneakers();
	artilist[ART_MERLOT_FUTURE].otyp = find_skates4();
	artilist[ART_MADELEINE_S_GIRL_FOOTSTEPS].otyp = find_calf_leather_sandals();
	artilist[ART_RUTH_S_MORTAL_ENEMY].otyp = find_velcro_sandals();
	artilist[ART_LARISSA_S_ANGER].otyp = find_buffalo_boots();
	artilist[ART_PRETTY_ROOMMAID].otyp = find_heroine_mocassins();
	artilist[ART_ALISEH_S_RED_COLOR].otyp = find_lolita_boots();
	artilist[ART_KATIE_MELUA_S_FLEECINESS].otyp = find_weapon_light_boots();
	artilist[ART_ELONA_S_SNAIL_TRAIL].otyp = find_rubynus_helmet();

	artilist[ART_ARABELLA_S_WEAPON_STORAGE].otyp = find_cursed_called_cloak();
	artilist[ART_KYS_YOURSELF].otyp = find_fourchan_cloak();
	artilist[ART_INA_S_SORROW].otyp = find_inalish_cloak();
	artilist[ART_WHY_NOT_DO_THE_REAL_THING].otyp = find_grunter_helmet();
	artilist[ART_CLAUDIA_S_SEXY_SCENT].otyp = find_cloudy_helmet();
	artilist[ART_VACUUM_CLEANER_DEATH].otyp = find_breath_control_helmet();
	artilist[ART_NYPHERISBANE].otyp = find_gas_mask();
	artilist[ART_CHRISTMAS_MEDAL].otyp = find_sages_helmet();
	artilist[ART_LONGEST_RAY].otyp = find_rayductnay_gloves();
	artilist[ART_ENDORPHIC_SCRATCHING].otyp = find_orgasm_pumps();
	artilist[ART_SONJA_S_TORN_SOUL].otyp = find_worn_out_sneakers();
	artilist[ART_RUEA_S_FAILED_CONVERSION].otyp = find_noble_sandals();
	artilist[ART_HAWAIIAN_KAMEHAMEHA].otyp = find_doctor_claw_boots();
	artilist[ART_LARISSA_S_GENTLE_SLEEP].otyp = find_feelgood_heels();
	artilist[ART_STEFANJE_S_PROBLEM].otyp = find_plof_heels();
	artilist[ART_MALENA_S_LADYNESS].otyp = find_princess_pumps();
	artilist[ART_ELEVECULT].otyp = find_ballet_heels();
	artilist[ART_SHIT_KICKERS].otyp = find_steel_toed_boots();
	artilist[ART_SARAH_S_GRANNY_WEAR].otyp = find_marji_shoes();
	artilist[ART_CLICHE_WEAR].otyp = find_mary_janes();
	artilist[ART_YVONNE_S_MODEL_AMBITION].otyp = find_velvet_pumps();

	artilist[ART_BOWSER_S_FUN_ARENA].otyp = find_volcanic_cloak();
	artilist[ART_BILLS_PAID].otyp = find_fuel_cloak();
	artilist[ART_FINDING_THYSELF].otyp = find_cloister_cloak();
	artilist[ART_SALADIN_S_DESERT_FOX].otyp = find_shemagh();
	artilist[ART_JAMILA_S_BELIEF].otyp = find_shemagh();
	artilist[ART_SLIPPING_SUCKING].otyp = find_serrated_helmet();
	artilist[ART_INERT_GREAVES].otyp = find_thick_boots();
	artilist[ART_DESERT_MEDITATION].otyp = find_sand_als();
	artilist[ART_GORGEOUS_VEIL_MODEL].otyp = find_shadowy_heels();
	artilist[ART_SUBLEVEL_FLOODING].otyp = find_weight_attachment_boots();
	artilist[ART_USE_THE_NORMALNESS_TURNS].otyp = find_fungal_sandals();
	artilist[ART_UNFELLABLE_TREE].otyp = find_standing_footwear();

#if 0
	/* Fix up the gifts */
	if (urole.gift1arti) {
		artilist[urole.gift1arti].alignment = alignmnt;
		artilist[urole.gift1arti].role = Role_switch;
	}
	if (urole.gift2arti) {
		artilist[urole.gift2arti].alignment = alignmnt;
		artilist[urole.gift2arti].role = Role_switch;
	}
#endif
	/* Fix up the quest artifact */
	if (urole.questarti) {
	    artilist[urole.questarti].alignment = alignmnt;
	    artilist[urole.questarti].role = Role_switch;
	}
	return;
}

/* zero out the artifact existence list */
void
init_artifacts()
{
	(void) memset((void *) artiexist, 0, sizeof artiexist);
	(void) memset((void *) artidisco, 0, sizeof artidisco);
	hack_artifacts();
}

/* Post u_init() initialization */
void
init_artifacts1()
{
    /* KMH -- Should be expert in quest artifact */
    if (urole.questarti && !isamerican && !Role_if(PM_ANACHRONOUNBINDER) &&
	    (objects[artilist[urole.questarti].otyp].oc_class == WEAPON_CLASS ||
	     objects[artilist[urole.questarti].otyp].oc_class == TOOL_CLASS)) {
	int skill = objects[artilist[urole.questarti].otyp].oc_skill;

	if (!isamerican && !Race_if(PM_BASTARD) && !(Race_if(PM_GREURO) && (skill == P_BOW || skill == P_CROSSBOW))) {

		if (skill > P_NONE && P_SKILL(skill) < P_UNSKILLED)
		    P_SKILL(skill) = P_UNSKILLED;
		if (skill > P_NONE && P_MAX_SKILL(skill) < P_EXPERT) {
		    if (wizard) pline("Warning: %s should be at least expert.  Fixing...",
			    artilist[urole.questarti].name);
		    P_MAX_SKILL(skill) = P_EXPERT;
		}
	}
    }
}

void
save_artifacts(fd)
int fd;
{
	bwrite(fd, (void *) artiexist, sizeof artiexist);
	bwrite(fd, (void *) artidisco, sizeof artidisco);
}

void
restore_artifacts(fd)
int fd;
{
	mread(fd, (void *) artiexist, sizeof artiexist);
	mread(fd, (void *) artidisco, sizeof artidisco);
	hack_artifacts();	/* redo non-saved special cases */
}

const char *
artiname(artinum)
int artinum;
{
	if (artinum <= 0 || artinum > NROFARTIFACTS) return("");
	return(artilist[artinum].name);
}

/*
   Make an artifact.  If a specific alignment is specified, then an object of
   the appropriate alignment is created from scratch, or 0 is returned if
   none is available.  (If at least one aligned artifact has already been
   given, then unaligned ones also become eligible for this.)
   If no alignment is given, then 'otmp' is converted
   into an artifact of matching type, or returned as-is if that's not possible.
   For the 2nd case, caller should use ``obj = mk_artifact(obj, A_NONE);''
   for the 1st, ``obj = mk_artifact((struct obj *)0, some_alignment);''.
 */
struct obj *
mk_artifact(otmp, alignment, existingagain)
struct obj *otmp;	/* existing object; ignored if alignment specified */
aligntyp alignment;	/* target alignment, or A_NONE */
boolean existingagain;	/* if TRUE, existing ones can be generated again */
{
	const struct artifact *a;
	int n, m;
	boolean by_align = (alignment != A_NONE);
	/*short*/int o_typ = (by_align || !otmp) ? 0 : otmp->otyp;
	boolean unique = !by_align && otmp && objects[o_typ].oc_unique;
	/*short*/int eligible[NROFARTIFACTS];


#if 0
	/* KMH, role patch -- Try first or second gift */
	m = urole.gift1arti;
	if (by_align && m && !artiexist[m]) {
		a = (struct artifact *)&artilist[m];
		goto make_artif;
	}
	m = urole.gift2arti;
	if (by_align && m && !artiexist[m]) {
		a = (struct artifact *)&artilist[m];
		goto make_artif;
	}
#endif

	/* gather eligible artifacts */
	for (n = 0, a = artilist+1, m = 1; a->otyp; a++, m++)
	    if ((!by_align ? a->otyp == o_typ :
		    (a->alignment == alignment ||
			(a->alignment == A_NONE && u.ugifts > 0))) &&
		(!(a->spfx & SPFX_NOGEN) || unique) && !(a->otyp == WAN_DESLEXIFICATION && !issoviet) && (!artiexist[m] || (existingagain && !(a->spfx & SPFX_ONLYONE) ) )) {
		/*
		 * [ALI] The determination of whether an artifact is
		 * hostile to the player is a little more complex in
		 * Slash'EM than Vanilla since there are artifacts
		 * which are hostile to humans (eg., Deathsword) which
		 * aren't aligned to any race.
		 * Nevertheless, the rule remains the same: Gods don't
		 * grant artifacts which would be hostile to the player
		 * _in their normal form_.
		 * Amy addition: wand of deslexification cannot be gifted to non-soviet, since that thing is supposed to
		 * not exist outside of soviet mode.
		 */
		boolean hostile = FALSE;
		if (by_align) {
		    if (a->race != NON_PM && race_hostile(&mons[a->race]))
			hostile = TRUE;		/* enemies' equipment */
		    else if (a->spfx & SPFX_DBONUS) {
			struct artifact tmp;

			tmp = *a;
			tmp.spfx &= SPFX_DBONUS;
			if (Upolyd)
			    set_mon_data(&youmonst, &upermonst, 0);
			if (spec_applies(&tmp, &youmonst))
			    hostile = TRUE;	/* can blast unpolyd player */
			if (Upolyd)
			    set_mon_data(&youmonst, &mons[u.umonnum], 0);
		    }
		}
		if (hostile)
		    continue;
		/* Amy evil patch change: remove guaranteed sacrifice gifts because we want variety. */
		if (by_align && !rn2(issoviet ? 2 : 5) && Role_if(a->role) && !artiexist[m])
		/* Now, you're still more likely than usual to get artifacts that are aligned with your role, but they are
		 * by no means guaranteed. Instead, you're supposed to use whatever you get now. --Amy */
		    goto make_artif;	/* 'a' points to the desired one */
		else if(by_align && Role_if(PM_PIRATE)) continue; /* pirates are not gifted artifacts */
		else
		    eligible[n++] = m;
	    }

	if (n) {		/* found at least one candidate */
	    m = eligible[rn2(n)];	/* [0..n-1] */
	    a = &artilist[m];

	    /* make an appropriate object if necessary, then christen it */
make_artif: if (by_align) otmp = mksobj((int)a->otyp, TRUE, FALSE, FALSE);

	    if (existingagain && !(a->spfx & SPFX_ONLYONE)) otmp = onameX(otmp, a->name);
	    else otmp = oname(otmp, a->name);

	    otmp->oartifact = m;
		/* usually mark artifact as existing... but not always :D --Amy */
	    if (rn2(100)) artiexist[m] = TRUE;
	} else {
	    /* nothing appropriate could be found; return the original object */
	    if (by_align) otmp = 0;	/* (there was no original object) */
	}
	return otmp;
}

/* Make an evil artifact and automatically equip it for the player --Amy
 * This can also always spawn another copy of an existing one (not a bug) */
void
bad_artifact()
{
	const struct artifact *a;
	int n, m;
	int eligible[NROFARTIFACTS];
	register struct obj *otmp;

	/* gather eligible artifacts */
	for (n = 0, a = artilist+1, m = 1; a->otyp; a++, m++)
	    if (!(a->spfx & SPFX_NOGEN) && (a->spfx & SPFX_EVIL) && !(a->otyp == AMULET_OF_STRANGULATION) && !(a->otyp == WAN_DESLEXIFICATION && !issoviet) && (!(artiexist[m] && (a->spfx & SPFX_ONLYONE)) ) ) {

		    eligible[n++] = m;
	    }

	if (n) {		/* found at least one candidate */
	    m = eligible[rn2(n)];	/* [0..n-1] */
	    a = &artilist[m];

	    /* make an appropriate object if necessary, then christen it */
	    otmp = mksobj((int)a->otyp, TRUE, FALSE, FALSE);

		if (!otmp) return;

		otmp = onameX(otmp, a->name);

		if (!otmp) return;

	    otmp->oartifact = m;
	    if (rn2(100)) artiexist[m] = TRUE;
	} else {
		return; /* aww, there are no evil artifacts left... so don't make one. Bummer. */
	}

	if (otmp) {
		(void) pickup_object(otmp, otmp->quan, TRUE, TRUE);
	}

	/* try to equip it! */

	if (otmp) {

		u.cnd_badarticount++;

		if (otmp->oclass == WEAPON_CLASS || otmp->oclass == GEM_CLASS || otmp->oclass == BALL_CLASS || otmp->oclass == CHAIN_CLASS || is_weptool(otmp)) {
			if (uwep) setnotworn(uwep);
			if (bimanual(otmp)) {
				if (uswapwep) uswapwepgone();
				if (uarms) remove_worn_item(uarms, TRUE);
			}
			if (!uwep) setuwep(otmp, FALSE, TRUE);
			if (otmp) curse(otmp);
		}

		else if (is_blindfold_slot(otmp)) {
			if (ublindf) remove_worn_item(ublindf, TRUE);
			Blindf_on(otmp);
			if (otmp) curse(otmp);
		}

		else if (otmp->oclass == RING_CLASS) {
			boolean righthand = rn2(2);
			if (righthand) {
				if (uright) remove_worn_item(uright, TRUE);
				setworn(otmp, RIGHT_RING);
				Ring_on(otmp);
				if (otmp) curse(otmp);
			} else {
				if (uleft) remove_worn_item(uleft, TRUE);
				setworn(otmp, LEFT_RING);
				Ring_on(otmp);
				if (otmp) curse(otmp);
			}
		}

		else if (otmp->oclass == AMULET_CLASS) {
			if (uamul) remove_worn_item(uamul, TRUE);
			setworn(otmp, W_AMUL);
			Amulet_on();
			if (otmp) curse(otmp);
		}

		else if (otmp->oclass == IMPLANT_CLASS) {
			if (uimplant) remove_worn_item(uimplant, TRUE);
			setworn(otmp, W_IMPLANT);
			Implant_on();
			if (otmp) curse(otmp);
		}

		else if (is_boots(otmp) && !(Race_if(PM_ELONA_SNAIL) && !ishighheeled(otmp))) {
			if (uarmf) remove_worn_item(uarmf, TRUE);
			setworn(otmp, W_ARMF);
			Boots_on();
			if (otmp) curse(otmp);
		}

		else if (is_gloves(otmp)) {
			if (uarmg) remove_worn_item(uarmg, TRUE);
			setworn(otmp, W_ARMG);
			Gloves_on();
			if (otmp) curse(otmp);
		}

		else if (is_helmet(otmp)) {
			if (uarmh) remove_worn_item(uarmh, TRUE);
			setworn(otmp, W_ARMH);
			Helmet_on();
			if (otmp) curse(otmp);
		}

		else if (is_cloak(otmp)) {
			if (uarmc) remove_worn_item(uarmc, TRUE);
			setworn(otmp, W_ARMC);
			Cloak_on();
			if (otmp) curse(otmp);
		}

		else if (is_shield(otmp)) {
			if (uarms) remove_worn_item(uarms, TRUE);
			setworn(otmp, W_ARMS);
			Shield_on();
			if (otmp) curse(otmp);
		}

		else if (is_shirt(otmp)) {
			if (uarmu) remove_worn_item(uarmu, TRUE);
			setworn(otmp, W_ARMU);
			Shirt_on();
			if (otmp) curse(otmp);
		}

		else if (is_suit(otmp)) {
			if (uskin) skinback(FALSE);
			if (uarm) remove_worn_item(uarm, TRUE);
			setworn(otmp, W_ARM);
			Armor_on();
			if (otmp) curse(otmp);
		}

	}

}

/* The same bad artifact effect, but if there is a skill for the generated item, have a chance of learning it --Amy
 * Again, this can create a copy of an already existing one */
void
bad_artifact_xtra()
{
	const struct artifact *a;
	int n, m;
	int eligible[NROFARTIFACTS];
	register struct obj *otmp;

	/* gather eligible artifacts */
	for (n = 0, a = artilist+1, m = 1; a->otyp; a++, m++)
	    if (!(a->spfx & SPFX_NOGEN) && (a->spfx & SPFX_EVIL) && !(a->otyp == AMULET_OF_STRANGULATION) && !(a->otyp == WAN_DESLEXIFICATION && !issoviet) && (!(artiexist[m] && (a->spfx & SPFX_ONLYONE)) ) ) {

		    eligible[n++] = m;
	    }

	if (n) {		/* found at least one candidate */
	    m = eligible[rn2(n)];	/* [0..n-1] */
	    a = &artilist[m];

	    /* make an appropriate object if necessary, then christen it */
	    otmp = mksobj((int)a->otyp, TRUE, FALSE, FALSE);

		if (!otmp) return;

		otmp = onameX(otmp, a->name);

		if (!otmp) return;

	    otmp->oartifact = m;
	    if (rn2(100)) artiexist[m] = TRUE;
	} else {
		return; /* aww, there are no evil artifacts left... so don't make one. Bummer. */
	}

	if (otmp) {
		if (P_MAX_SKILL(get_obj_skill(otmp, TRUE)) == P_ISRESTRICTED) {
		    unrestrict_weapon_skill(get_obj_skill(otmp, TRUE));
		} else if (P_MAX_SKILL(get_obj_skill(otmp, TRUE)) == P_UNSKILLED) {
			unrestrict_weapon_skill(get_obj_skill(otmp, TRUE));
			P_MAX_SKILL(get_obj_skill(otmp, TRUE)) = P_BASIC;
		} else if (rn2(2) && P_MAX_SKILL(get_obj_skill(otmp, TRUE)) == P_BASIC) {
			P_MAX_SKILL(get_obj_skill(otmp, TRUE)) = P_SKILLED;
		} else if (!rn2(4) && P_MAX_SKILL(get_obj_skill(otmp, TRUE)) == P_SKILLED) {
			P_MAX_SKILL(get_obj_skill(otmp, TRUE)) = P_EXPERT;
		} else if (!rn2(10) && P_MAX_SKILL(get_obj_skill(otmp, TRUE)) == P_EXPERT) {
			P_MAX_SKILL(get_obj_skill(otmp, TRUE)) = P_MASTER;
		} else if (!rn2(100) && P_MAX_SKILL(get_obj_skill(otmp, TRUE)) == P_MASTER) {
			P_MAX_SKILL(get_obj_skill(otmp, TRUE)) = P_GRAND_MASTER;
		} else if (!rn2(200) && P_MAX_SKILL(get_obj_skill(otmp, TRUE)) == P_GRAND_MASTER) {
			P_MAX_SKILL(get_obj_skill(otmp, TRUE)) = P_SUPREME_MASTER;
		}

		if (Race_if(PM_RUSMOT)) {
			if (P_MAX_SKILL(get_obj_skill(otmp, TRUE)) == P_ISRESTRICTED) {
			    unrestrict_weapon_skill(get_obj_skill(otmp, TRUE));
			} else if (P_MAX_SKILL(get_obj_skill(otmp, TRUE)) == P_UNSKILLED) {
				unrestrict_weapon_skill(get_obj_skill(otmp, TRUE));
				P_MAX_SKILL(get_obj_skill(otmp, TRUE)) = P_BASIC;
			} else if (rn2(2) && P_MAX_SKILL(get_obj_skill(otmp, TRUE)) == P_BASIC) {
				P_MAX_SKILL(get_obj_skill(otmp, TRUE)) = P_SKILLED;
			} else if (!rn2(4) && P_MAX_SKILL(get_obj_skill(otmp, TRUE)) == P_SKILLED) {
				P_MAX_SKILL(get_obj_skill(otmp, TRUE)) = P_EXPERT;
			} else if (!rn2(10) && P_MAX_SKILL(get_obj_skill(otmp, TRUE)) == P_EXPERT) {
				P_MAX_SKILL(get_obj_skill(otmp, TRUE)) = P_MASTER;
			} else if (!rn2(100) && P_MAX_SKILL(get_obj_skill(otmp, TRUE)) == P_MASTER) {
				P_MAX_SKILL(get_obj_skill(otmp, TRUE)) = P_GRAND_MASTER;
			} else if (!rn2(200) && P_MAX_SKILL(get_obj_skill(otmp, TRUE)) == P_GRAND_MASTER) {
				P_MAX_SKILL(get_obj_skill(otmp, TRUE)) = P_SUPREME_MASTER;
			}
		}

		(void) pickup_object(otmp, otmp->quan, TRUE, TRUE);

	}

	/* try to equip it! */

	if (otmp) {

		u.cnd_badarticount++;

		if (otmp->oclass == WEAPON_CLASS || otmp->oclass == GEM_CLASS || otmp->oclass == BALL_CLASS || otmp->oclass == CHAIN_CLASS || is_weptool(otmp)) {
			if (uwep) setnotworn(uwep);
			if (bimanual(otmp)) {
				if (uswapwep) uswapwepgone();
				if (uarms) remove_worn_item(uarms, TRUE);
			}
			if (!uwep) setuwep(otmp, FALSE, TRUE);
			if (otmp) curse(otmp);
		}

		else if (is_blindfold_slot(otmp)) {
			if (ublindf) remove_worn_item(ublindf, TRUE);
			Blindf_on(otmp);
			if (otmp) curse(otmp);
		}

		else if (otmp->oclass == RING_CLASS) {
			boolean righthand = rn2(2);
			if (righthand) {
				if (uright) remove_worn_item(uright, TRUE);
				setworn(otmp, RIGHT_RING);
				Ring_on(otmp);
				if (otmp) curse(otmp);
			} else {
				if (uleft) remove_worn_item(uleft, TRUE);
				setworn(otmp, LEFT_RING);
				Ring_on(otmp);
				if (otmp) curse(otmp);
			}
		}

		else if (otmp->oclass == AMULET_CLASS) {
			if (uamul) remove_worn_item(uamul, TRUE);
			setworn(otmp, W_AMUL);
			Amulet_on();
			if (otmp) curse(otmp);
		}

		else if (otmp->oclass == IMPLANT_CLASS) {
			if (uimplant) remove_worn_item(uimplant, TRUE);
			setworn(otmp, W_IMPLANT);
			Implant_on();
			if (otmp) curse(otmp);
		}

		else if (is_boots(otmp) && !(Race_if(PM_ELONA_SNAIL) && !ishighheeled(otmp))) {
			if (uarmf) remove_worn_item(uarmf, TRUE);
			setworn(otmp, W_ARMF);
			Boots_on();
			if (otmp) curse(otmp);
		}

		else if (is_gloves(otmp)) {
			if (uarmg) remove_worn_item(uarmg, TRUE);
			setworn(otmp, W_ARMG);
			Gloves_on();
			if (otmp) curse(otmp);
		}

		else if (is_helmet(otmp)) {
			if (uarmh) remove_worn_item(uarmh, TRUE);
			setworn(otmp, W_ARMH);
			Helmet_on();
			if (otmp) curse(otmp);
		}

		else if (is_cloak(otmp)) {
			if (uarmc) remove_worn_item(uarmc, TRUE);
			setworn(otmp, W_ARMC);
			Cloak_on();
			if (otmp) curse(otmp);
		}

		else if (is_shield(otmp)) {
			if (uarms) remove_worn_item(uarms, TRUE);
			setworn(otmp, W_ARMS);
			Shield_on();
			if (otmp) curse(otmp);
		}

		else if (is_shirt(otmp)) {
			if (uarmu) remove_worn_item(uarmu, TRUE);
			setworn(otmp, W_ARMU);
			Shirt_on();
			if (otmp) curse(otmp);
		}

		else if (is_suit(otmp)) {
			if (uskin) skinback(FALSE);
			if (uarm) remove_worn_item(uarm, TRUE);
			setworn(otmp, W_ARM);
			Armor_on();
			if (otmp) curse(otmp);
		}

	}

}

/*
 * Returns the full name (with articles and correct capitalization) of an
 * artifact named "name" if one exists, or NULL, it not.
 * The given name must be rather close to the real name for it to match.
 * The object type of the artifact is returned in otyp if the return value
 * is non-NULL.
 */
const char*
artifact_name(name, otyp)
const char *name;
int *otyp;
{
    register const struct artifact *a;
    register const char *aname;

    if(!strncmpi(name, "the ", 4)) name += 4;
    if(!strncmpi(name, "poisoned ", 9)) name += 9;

    for (a = artilist+1; a->otyp; a++) {
	aname = a->name;
	if(!strncmpi(aname, "the ", 4)) aname += 4;
	if(!strcmpi(name, aname)) {
	    *otyp = a->otyp;
	    return a->name;
	}
    }

    return (char *)0;
}

boolean
exist_artifact(otyp, name)
register int otyp;
register const char *name;
{
	register const struct artifact *a;
	register boolean *arex;

	if (otyp && *name)
	    for (a = artilist+1,arex = artiexist+1; a->otyp; a++,arex++)
		if ((int) a->otyp == otyp && !strcmp(a->name, name))
		    return *arex;
	return FALSE;
}

void
artifact_exists(otmp, name, mod)
register struct obj *otmp;
register const char *name;
register boolean mod;
{
	register const struct artifact *a;

	if (otmp && *name)
	    for (a = artilist+1; a->otyp; a++)
		if (a->otyp == otmp->otyp && !strcmp(a->name, name)) {
		    register int m = a - artilist;
		    otmp->oartifact = (/*char*/int)(mod ? m : 0);
		    if (mod) {
			/* Light up Candle of Eternal Flame and
			 * Holy Spear of Light on creation.
			 */
			if (!artiexist[m] && artifact_light(otmp) &&
			  otmp->oartifact != ART_SUNSWORD)
			    begin_burn(otmp, FALSE);
			/*otmp->quan = 1;*/ /* guarantee only one of this artifact */ /* Amy edit: artifact ammo should not suck... */
/* Artifacts are immune to unpolypile --ALI */
			if (is_hazy(otmp)) {
			    (void) stop_timer(UNPOLY_OBJ, (void *) otmp);
			    otmp->oldtyp = STRANGE_OBJECT;
			}
		    }
		    /* the age field is used both for lightsaber power and the invoke timer... ugh --Amy */
		    if (!is_lightsaber(otmp) || otmp->oartifact == ART_LIGHTSABER_PROTOTYPE) otmp->age = 0;
		    if ((mod == FALSE) || rn2(100)) artiexist[m] = mod;
		    break;
		}
	return;
}

int
nartifact_exist()
{
    int a = 0;
    int n = SIZE(artiexist);

    while(n > 1)
	if(artiexist[--n]) a++;

    return a;
}
#endif /* OVLB */
#ifdef OVL0

boolean
spec_ability(otmp, abil)
struct obj *otmp;
unsigned long abil;
{
	const struct artifact *arti = get_artifact(otmp);

	return((boolean)(arti && (arti->spfx & abil)));
}

/* determine if a given artifact is "evil" (used in pickup.c) --Amy */
boolean
arti_is_evil(obj)
struct obj *obj;
{
    return (obj->oartifact && spec_ability(obj, SPFX_EVIL));
}

/* used so that callers don't need to known about SPFX_ codes */
boolean
confers_luck(obj)
struct obj *obj;
{
    /* might as well check for this too */
    if (obj->otyp == LUCKSTONE) return TRUE;

    if (obj->otyp == FEDORA && obj == uarmh) return TRUE;

    return (obj->oartifact && spec_ability(obj, SPFX_LUCK));
}

/* used to check whether a monster is getting reflection from an artifact */
boolean
arti_reflects(obj)
struct obj *obj;
{
    const struct artifact *arti = get_artifact(obj);

    if (arti) {      
	/* while being worn */
	if ((obj->owornmask & ~W_ART) && (arti->spfx & SPFX_REFLECT))
	    return TRUE;
	/* just being carried */
	if (arti->cspfx & SPFX_REFLECT) return TRUE;
    }
    return FALSE;
}

#endif /* OVL0 */
#ifdef OVLB

boolean
restrict_name(otmp, name)  /* returns 1 if name is restricted for otmp->otyp */
register struct obj *otmp;
register const char *name;
{
	register const struct artifact *a;
	register const char *aname;

	if (!*name) return FALSE;
	if (!strncmpi(name, "the ", 4)) name += 4;

		/* Since almost every artifact is SPFX_RESTR, it doesn't cost
		   us much to do the string comparison before the spfx check.
		   Bug fix:  don't name multiple elven daggers "Sting".
		 */
	for (a = artilist+1; a->otyp; a++) {
	    /*if (a->otyp != otmp->otyp) continue;*/ /* artifact naming bug --Amy */
	    aname = a->name;
	    if (!strncmpi(aname, "the ", 4)) aname += 4;
	    if (!strcmp(aname, name))
		return ((boolean)((a->spfx & (SPFX_NOGEN|SPFX_RESTR)) != 0 ||
			otmp->quan > 1L));
	}

	return FALSE;
}

STATIC_OVL boolean
attacks(adtyp, otmp)
register int adtyp;
register struct obj *otmp;
{
	register const struct artifact *weap;

	if ((weap = get_artifact(otmp)) != 0)
		return((boolean)(weap->attk.adtyp == adtyp));
	return FALSE;
}

boolean
defends(adtyp, otmp)
register int adtyp;
register struct obj *otmp;
{
	register const struct artifact *weap;

	if ((weap = get_artifact(otmp)) != 0)
		return((boolean)(weap->defn.adtyp == adtyp));
	return FALSE;
}

/* used for monsters */
boolean
protects(adtyp, otmp)
int adtyp;
struct obj *otmp;
{
	register const struct artifact *weap;

	if ((weap = get_artifact(otmp)) != 0)
		return (boolean)(weap->cary.adtyp == adtyp);
	return FALSE;
}

/*
 * a potential artifact has just been worn/wielded/picked-up or
 * unworn/unwielded/dropped.  Pickup/drop only set/reset the W_ART mask.
 */
void
set_artifact_intrinsic(otmp,on,wp_mask)
register struct obj *otmp;
boolean on;
long wp_mask;
{
	long *mask = 0;
	register const struct artifact *oart = get_artifact(otmp);
	uchar dtyp;
	long spfx;

	if (!oart) return;

	/* effects from the defn field */
	dtyp = (wp_mask != W_ART) ? oart->defn.adtyp : oart->cary.adtyp;

	if (dtyp == AD_FIRE)
	    mask = &EFire_resistance;
	else if (dtyp == AD_COLD)
	    mask = &ECold_resistance;
	else if (dtyp == AD_ACID)
	    mask = &EAcid_resistance;
	else if (dtyp == AD_ELEC)
	    mask = &EShock_resistance;
	else if (dtyp == AD_MAGM)
	    mask = &EAntimagic;
	else if (dtyp == AD_DISN)
	    mask = &EDisint_resistance;
	else if (dtyp == AD_DRST)
	    mask = &EPoison_resistance;
	else if (dtyp == AD_DRLI) 
	    mask = &EDrain_resistance; 

	if (mask && wp_mask == W_ART && !on) {
	    /* find out if some other artifact also confers this intrinsic */
	    /* if so, leave the mask alone */
	    register struct obj* obj;
	    for(obj = invent; obj; obj = obj->nobj)
		if(obj != otmp && obj->oartifact) {
		    register const struct artifact *art = get_artifact(obj);
		    if(art->cary.adtyp == dtyp) {
			mask = (long *) 0;
			break;
		    }
		}
	}
	if(mask) {
	    if (on) *mask |= wp_mask;
	    else *mask &= ~wp_mask;
	}

	/* intrinsics from the spfx field; there could be more than one */
	spfx = (wp_mask != W_ART) ? oart->spfx : oart->cspfx;
	if(spfx && wp_mask == W_ART && !on) {
	    /* don't change any spfx also conferred by other artifacts */
	    register struct obj* obj;
	    for(obj = invent; obj; obj = obj->nobj)
		if(obj != otmp && obj->oartifact) {
		    register const struct artifact *art = get_artifact(obj);
		    spfx &= ~art->cspfx;
		}
	}

	if (spfx & SPFX_SEARCH) {
	    if(on) ESearching |= wp_mask;
	    else ESearching &= ~wp_mask;
	}
	if (spfx & SPFX_SEEK) {
	    if(on) ESearching |= wp_mask;
	    else ESearching &= ~wp_mask;
	}
	if (spfx & SPFX_HALRES) {
	    /* make_hallucinated must (re)set the mask itself to get
	     * the display right */
	    /* restoring needed because this is the only artifact intrinsic
	     * that can print a message--need to guard against being printed
	     * when restoring a game
	     */
	    (void) make_hallucinated((long)!on, restoring ? FALSE : TRUE, wp_mask);
	}
	if (spfx & SPFX_ESP) {
	    if(on) ETelepat |= wp_mask;
	    else ETelepat &= ~wp_mask;
	    see_monsters();
	}
	if (spfx & SPFX_STLTH) {
	    if (on) EStealth |= wp_mask;
	    else EStealth &= ~wp_mask;
	}
	if (spfx & SPFX_REGEN) {
	    if (on) ERegeneration |= wp_mask;
	    else ERegeneration &= ~wp_mask;
	}
	if (spfx & SPFX_TCTRL) {
	    if (on) ETeleport_control |= wp_mask;
	    else ETeleport_control &= ~wp_mask;
	}
	/* weapon warning is specially handled in mon.c */
	if (spfx & SPFX_WARN) {
	    if (spec_m2(otmp)) {
	    	if (on) {
			EWarn_of_mon |= wp_mask;
			flags.warntype |= spec_m2(otmp);
	    	} else {
			EWarn_of_mon &= ~wp_mask;
	    		flags.warntype &= ~spec_m2(otmp);
		}
		see_monsters();
	    } else {
		if (on) EWarning |= wp_mask;
	    	else EWarning &= ~wp_mask;
	    }
	}
	if (spfx & SPFX_EREGEN) {
	    if (on) EEnergy_regeneration |= wp_mask;
	    else EEnergy_regeneration &= ~wp_mask;
	}
	if (spfx & SPFX_HSPDAM) {
	    if (on) EHalf_spell_damage |= wp_mask;
	    else EHalf_spell_damage &= ~wp_mask;
	}
	if (spfx & SPFX_HPHDAM) {
	    if (on) EHalf_physical_damage |= wp_mask;
	    else EHalf_physical_damage &= ~wp_mask;
	}
	if (spfx & SPFX_PROTEC) {
	    if (on) {
		/*u.ublessed += 2;
		HProtection |= FROMOUTSIDE;*/
		u.artifactprotection = 1;
	    } else {
	    	/*u.ublessed -= 2;
		if (u.ublessed <= 0) HProtection &= ~FROMOUTSIDE;*/
		u.artifactprotection = 0;
	    }
	}
	if (spfx & SPFX_XRAY) {
	    /* this assumes that no one else is using xray_range */
	    if (on) u.xray_range = 3;
	    else u.xray_range = -1;
	    vision_full_recalc = 1;
	}
	/* KMH -- Reflection when wielded */
	if ((spfx & SPFX_REFLECT)) { /* fix by Chris_ANG - why the heck was this on being wielded only??? */
	    if (on) EReflecting |= wp_mask;
	    else EReflecting &= ~wp_mask;
	}

	if(wp_mask == W_ART && !on && oart->inv_prop) {
	    /* might have to turn off invoked power too */
	    if (oart->inv_prop <= LAST_PROP &&
		(u.uprops[oart->inv_prop].extrinsic & W_ARTI))
		(void) arti_invoke(otmp);
	}
}

/*
 * creature (usually player) tries to touch (pick up or wield) an artifact obj.
 * Returns 0 if the object refuses to be touched.
 * This routine does not change any object chains.
 * Ignores such things as gauntlets, assuming the artifact is not
 * fooled by such trappings.
 */
int
touch_artifact(obj,mon)
    struct obj *obj;
    struct monst *mon;
{
    register const struct artifact *oart = get_artifact(obj);
    boolean badclass, badalign, self_willed, yours;

    if(!oart) return 1;

    /* [ALI] Thiefbane has a special affinity with shopkeepers */
    if (mon->isshk && obj->oartifact == ART_THIEFBANE) return 1;

    yours = (mon == &youmonst);
    /* all quest artifacts are self-willed; it this ever changes, `badclass'
       will have to be extended to explicitly include quest artifacts */
    self_willed = ((oart->spfx & SPFX_INTEL) != 0);
    if (yours) {
	badclass = self_willed &&
		   ((oart->role != NON_PM && !Role_if(oart->role)) ||
		    (oart->race != NON_PM && !Race_if(oart->race)));
	badalign = (oart->spfx & SPFX_RESTR) && oart->alignment != A_NONE &&
		   (oart->alignment != u.ualign.type || u.ualign.record < 0);
    } else if (!is_covetous(mon->data) && !is_mplayer(mon->data)) {
	badclass = self_willed &&
		   oart->role != NON_PM && oart != &artilist[ART_EXCALIBUR];
	badalign = (oart->spfx & SPFX_RESTR) && oart->alignment != A_NONE &&
		   (oart->alignment != sgn(mon->data->maligntyp));
    } else {    /* an M3_WANTSxxx monster or a fake player */
	/* special monsters trying to take the Amulet, invocation tools or
	   quest item can touch anything except for `spec_applies' artifacts */
	badclass = badalign = FALSE;
    }
    /* weapons which attack specific categories of monsters are
       bad for them even if their alignments happen to match */
    if (!badalign && (oart->spfx & SPFX_DBONUS) != 0) {
	struct artifact tmp;

	tmp = *oart;
	tmp.spfx &= SPFX_DBONUS;
	badalign = !!spec_applies(&tmp, mon);
    }

    if ((((badclass || badalign) && self_willed) || (badalign && (!yours || !rn2(4)))) && !RngeBlastShielding) {
	int dmg;
	char buf[BUFSZ];

	if (!yours) return 0;
	You("are blasted by %s power!", s_suffix(the(xname(obj))));
	u.cnd_artiblastcount++;
	dmg = d((Race_if(PM_KUTAR) ? 8 : StrongAntimagic ? 3 : Antimagic ? 6 : 8), (self_willed ? 10 : 6));
	if (!issoviet && (u.ulevel < 10)) { /* picking up unknown artifacts should not be a crapshoot for low-level chars. --Amy */
		dmg *= u.ulevel;
		dmg /= 10;
	}

	/* Yes, I know, this probably means you cannot instantly die by having an artifact spawn on your location while
	 * having autopickup on, unless you're a DQ Slime or something, but still... */

	/* In Soviet Russia, artifacts are not meant to be used by little children. Parents who want to get rid of their kids place Excaliburs and Stormbringers in their house on purpose, hoping that their offspring will make the mistake of touching  it and get killed instantly. --Amy */
	if (issoviet) pline("Bum, vy tol'ko chto vzorvali dlya massivnogo povrezhdeniya. Kha-kha-kha, tip bloka l'da smeyetsya nad vami.");

	/*if (wizard) pline("damage calc");*/
	sprintf(buf, "touching an artifact (%s)", oart->name);
	/*if (wizard) pline("sprintf");*/
	losehp(dmg, buf, KILLED_BY);
	/*if (wizard) pline("losehp done");*/
	exercise(A_WIS, FALSE);
    }

    /* can pick it up unless you're totally non-synch'd with the artifact */
    if (badclass && badalign && self_willed) {
	if (yours) pline("%s your grasp!", Tobjnam(obj, "evade"));
	return 0;
    }
    /* This is a kludge, but I'm not sure where else to put it */
    if (oart == &artilist[ART_IRON_BALL_OF_LIBERATION]) {
	if (Role_if(PM_CONVICT) && (!obj->oerodeproof)) {
	    obj->oerodeproof = TRUE;
	    obj->owt = 900; /* Magically lightened, but still heavy */
	}

	if (Punished && (obj != uball)) {
	    unpunish(); /* Remove a mundane heavy iron ball */
	}
    }

    return 1;
}

#endif /* OVLB */
#ifdef OVL1

/* decide whether an artifact's special attacks apply against mtmp */
STATIC_OVL int
spec_applies(weap, mtmp)
register const struct artifact *weap;
struct monst *mtmp;
{
	int retval = TRUE;
	struct permonst *ptr;
	boolean yours;

	if(!(weap->spfx & (SPFX_DBONUS | SPFX_ATTK)))
	    return(weap->attk.adtyp == AD_PHYS);

	yours = (mtmp == &youmonst);
	ptr = mtmp->data;

	/* [ALI] Modified to support multiple DBONUS and ATTK flags set.
	 * Not all combinations are possible because many DBONUS flags
	 * use mtype and would conflict. Where combinations are possible,
	 * both checks must pass in order for the special attack to
	 * apply against mtmp.
	 */
	if (weap->spfx & SPFX_DMONS) {
	    retval &= (ptr == &mons[(int)weap->mtype]);
	} else if (weap->spfx & SPFX_DCLAS) {
	    retval &= (weap->mtype == (unsigned long)ptr->mlet);
	} else if (weap->spfx & SPFX_DFLAG1) {
	    retval &= ((ptr->mflags1 & weap->mtype) != 0L);
	} else if (weap->spfx & SPFX_DFLAG2) {
	    retval &= ((ptr->mflags2 & weap->mtype) || (yours &&
			   ((!Upolyd && (urace.selfmask & weap->mtype)) ||
			    ((weap->mtype & M2_WERE) && u.ulycn >= LOW_PM))));
	}
	if (weap->spfx & SPFX_DALIGN) {
	    retval &= yours ? (u.ualign.type != weap->alignment) :
			   (ptr->maligntyp == A_NONE ||
				sgn(ptr->maligntyp) != weap->alignment);
	}
	if (weap->spfx & SPFX_ATTK) {
	    struct obj *defending_weapon = (yours ? uwep : MON_WEP(mtmp));

	    if (defending_weapon && defending_weapon->oartifact &&
		    defends((int)weap->attk.adtyp, defending_weapon))
		return FALSE;
	    switch(weap->attk.adtyp) {
		case AD_FIRE:
			if (yours ? Fire_resistance : resists_fire(mtmp))
			    retval = FALSE;
			break;
		case AD_ACID:
			if (yours ? Acid_resistance : resists_acid(mtmp))
			    retval = FALSE;
			break;
		case AD_COLD:
			if (yours ? Cold_resistance : resists_cold(mtmp))
			    retval = FALSE;
			break;
		case AD_ELEC:
			if (yours ? Shock_resistance : resists_elec(mtmp))
			    retval = FALSE;
			break;
		case AD_MAGM:
		case AD_STUN:
			if (yours ? Antimagic : (rn2(100) < ptr->mr))
			    retval = FALSE;
			break;
		case AD_DRST:
			if (yours ? Poison_resistance : resists_poison(mtmp))
			    retval = FALSE;
			break;
		case AD_DRLI:
			if (yours ? Drain_resistance : resists_drli(mtmp))
			    retval = FALSE;
			break;
		case AD_STON:
			if (yours ? Stone_resistance : resists_ston(mtmp))
			    retval = FALSE;
			break;
		case AD_PHYS:
			break;
		default:	impossible("Weird weapon special attack.");
	    }
	}
	return retval;
}

/* return the M2 flags of monster that an artifact's special attacks apply against */
long
spec_m2(otmp)
struct obj *otmp;
{
	register const struct artifact *artifact = get_artifact(otmp);
	if (artifact)
		return artifact->mtype;
	return 0L;
}

/* special attack bonus */
int
spec_abon(otmp, mon)
struct obj *otmp;
struct monst *mon;
{
	register const struct artifact *weap = get_artifact(otmp);

	/* no need for an extra check for `NO_ATTK' because this will
	   always return 0 for any artifact which has that attribute */

	if (weap && weap->attk.damn && spec_applies(weap, mon))
	    return (int)weap->attk.damn;
	return 0;
}

/* special damage bonus */
int
spec_dbon(otmp, mon, tmp)
struct obj *otmp;
struct monst *mon;
int tmp;
{
	register const struct artifact *weap = get_artifact(otmp);

	if (!weap || (weap->attk.adtyp == AD_PHYS && /* check for `NO_ATTK' */
			weap->attk.damn == 0 && weap->attk.damd == 0))
	    spec_dbon_applies = FALSE;
	else
	    spec_dbon_applies = spec_applies(weap, mon);

	/* Amy edit: the fact that they always did max damage was fucked up, IMHO. */

	if (spec_dbon_applies)
	    return weap->attk.damd ? rnd((int)weap->attk.damd) :
		    /* [ALI] Unlike melee weapons, damd == 0 means no
		     * bonus for launchers.
		     */
		    is_launcher(otmp) ? 0 : rnd(max(tmp,1)); /* double damage was too strong --Amy */
	return 0;
}

/* add identified artifact to discoveries list */
void
discover_artifact(m)
int m;
{
    int i;

    /* look for this artifact in the discoveries list;
       if we hit an empty slot then it's not present, so add it */
    for (i = 0; i < NROFARTIFACTS; i++)
	if (artidisco[i] == 0 || artidisco[i] == m) {
	    artidisco[i] = m;
	    return;
	}
    /* there is one slot per artifact, so we should never reach the
       end without either finding the artifact or an empty slot... */
    impossible("couldn't discover artifact (%d)", (int)m);
}

/* used to decide whether an artifact has been fully identified */
boolean
undiscovered_artifact(m)
int m;
{
    int i;

    /* look for this artifact in the discoveries list;
       if we hit an empty slot then it's undiscovered */
    for (i = 0; i < NROFARTIFACTS; i++)
	if (artidisco[i] == m)
	    return FALSE;
	else if (artidisco[i] == 0)
	    break;
    return TRUE;
}

/* display a list of discovered artifacts; return their count */
int
disp_artifact_discoveries(tmpwin)
winid tmpwin;		/* supplied by dodiscover() */
{
    int i, m, otyp;
    char buf[BUFSZ];
    anything any;

    any.a_void = 0;
    for (i = 0; i < NROFARTIFACTS; i++) {
	if (artidisco[i] == 0) break;	/* empty slot implies end of list */
	if (i == 0)
	    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings,
	      "Artifacts", MENU_UNSELECTED);
	m = artidisco[i];
	otyp = artilist[m].otyp;
	sprintf(buf, "  %s [%s %s]", artiname(m),
		align_str(artilist[m].alignment), simple_typename(otyp));
	add_menu(tmpwin, objnum_to_glyph(otyp), &any, 0, 0, ATR_NONE,
	  buf, MENU_UNSELECTED);
    }
    return i;
}

void
dump_artifact_discoveries()
{
    int i, m, otyp;
    char buf[BUFSZ];

    for (i = 0; i < NROFARTIFACTS; i++) {
	if (artidisco[i] == 0) break;	/* empty slot implies end of list */
	if (i == 0)
		    dump("", "  Artifacts");
	m = artidisco[i];
	otyp = artilist[m].otyp;
	sprintf(buf, "  %s [%s %s]", artiname(m),
		align_str(artilist[m].alignment), simple_typename(otyp));
		dump("  ", buf);
    }
}

#endif /* OVL1 */

#ifdef OVLB


	/*
	 * Magicbane's intrinsic magic is incompatible with normal
	 * enchantment magic.  Thus, its effects have a negative
	 * dependence on spe.  Against low mr victims, it typically
	 * does "double athame" damage, 2d4.  Occasionally, it will
	 * cast unbalancing magic which effectively averages out to
	 * 4d4 damage (3d4 against high mr victims), for spe = 0.
	 *
	 * Prior to 3.4.1, the cancel (aka purge) effect always
	 * included the scare effect too; now it's one or the other.
	 * Likewise, the stun effect won't be combined with either
	 * of those two; it will be chosen separately or possibly
	 * used as a fallback when scare or cancel fails.
	 *
	 * [Historical note: a change to artifact_hit() for 3.4.0
	 * unintentionally made all of Magicbane's special effects
	 * be blocked if the defender successfully saved against a
	 * stun attack.  As of 3.4.1, those effects can occur but
	 * will be slightly less likely than they were in 3.3.x.]
	 */
#define MB_MAX_DIEROLL		8	/* rolls above this aren't magical */
static const char * const mb_verb[2][4] = {
	{ "probe", "stun", "scare", "cancel" },
	{ "prod", "amaze", "tickle", "purge" },
};
#define MB_INDEX_PROBE		0
#define MB_INDEX_STUN		1
#define MB_INDEX_SCARE		2
#define MB_INDEX_CANCEL		3

/* called when someone is being hit by Magicbane */
STATIC_OVL boolean
Mb_hit(magr, mdef, mb, dmgptr, dieroll, vis, hittee)
struct monst *magr, *mdef;	/* attacker and defender */
struct obj *mb;			/* Magicbane */
int *dmgptr;			/* extra damage target will suffer */
int dieroll;			/* d20 that has already scored a hit */
boolean vis;			/* whether the action can be seen */
char *hittee;			/* target's name: "you" or mon_nam(mdef) */
{
    struct permonst *old_uasmon;
    const char *verb;
    boolean youattack = (magr == &youmonst),
	    youdefend = (mdef == &youmonst),
	    resisted = FALSE, do_stun, do_confuse, result;
    int attack_indx, scare_dieroll = MB_MAX_DIEROLL / 2;

    result = FALSE;		/* no message given yet */
    /* the most severe effects are less likely at higher enchantment */
    if (mb->spe >= 3)
	scare_dieroll /= (1 << (mb->spe / 3));
    /* if target successfully resisted the artifact damage bonus,
       reduce overall likelihood of the assorted special effects */
    if (!spec_dbon_applies) dieroll += 1;

    /* might stun even when attempting a more severe effect, but
       in that case it will only happen if the other effect fails;
       extra damage will apply regardless; 3.4.1: sometimes might
       just probe even when it hasn't been enchanted */
    do_stun = (max(mb->spe,0) < rn2(spec_dbon_applies ? 11 : 7));

    /* the special effects also boost physical damage; increments are
       generally cumulative, but since the stun effect is based on a
       different criterium its damage might not be included; the base
       damage is either 1d4 (athame) or 2d4 (athame+spec_dbon) depending
       on target's resistance check against AD_STUN (handled by caller)
       [note that a successful save against AD_STUN doesn't actually
       prevent the target from ending up stunned] */
    attack_indx = MB_INDEX_PROBE;
    *dmgptr += rnd(4);			/* (2..3)d4 */
    if (do_stun) {
	attack_indx = MB_INDEX_STUN;
	*dmgptr += rnd(4);		/* (3..4)d4 */
    }
    if (dieroll <= scare_dieroll) {
	attack_indx = MB_INDEX_SCARE;
	*dmgptr += rnd(4);		/* (3..5)d4 */
    }
    if (dieroll <= (scare_dieroll / 2)) {
	attack_indx = MB_INDEX_CANCEL;
	*dmgptr += rnd(4);		/* (4..6)d4 */
    }

    /* give the hit message prior to inflicting the effects */
    verb = mb_verb[!!Hallucination][attack_indx];
    if (youattack || youdefend || vis) {
	result = TRUE;
	pline_The("magic-absorbing weapon %s %s!",
		  vtense((const char *)0, verb), hittee);
	/* assume probing has some sort of noticeable feedback
	   even if it is being done by one monster to another */
	if (attack_indx == MB_INDEX_PROBE && !canspotmon(mdef))
	    map_invisible(mdef->mx, mdef->my);
    }

    /* now perform special effects */
    switch (attack_indx) {
    case MB_INDEX_CANCEL:
	old_uasmon = youmonst.data;
	/* No mdef->mcan check: even a cancelled monster can be polymorphed
	 * into a golem, and the "cancel" effect acts as if some magical
	 * energy remains in spellcasting defenders to be absorbed later.
	 */
	boolean alreadycancel = (youattack && mdef && mdef->mcan);

	if (!cancel_monst(mdef, mb, youattack, FALSE, FALSE)) {
	    resisted = TRUE;
	} else {
	    do_stun = FALSE;
	    if (youdefend) {
		if (youmonst.data != old_uasmon)
		    *dmgptr = 0;    /* rehumanized, so no more damage */
		if (u.uenmax > 0) {
		    You("lose magical energy!");
		    u.uenmax--;
		    if (u.uen > 0) u.uen--;
		    flags.botl = 1;
		}
	    } else {
		if (mdef->data == &mons[PM_CLAY_GOLEM])
		    mdef->mhp = 1;	/* cancelled clay golems will die */
		if (youattack && !alreadycancel && attacktype(mdef->data, AT_MAGC)) {
		    You("absorb magical energy!");
		    u.uenmax++;
		    u.uen++;
		    flags.botl = 1;
		}
	    }
	}
	break;

    case MB_INDEX_SCARE:
	if (youdefend) {
	    if (Antimagic) {
		resisted = TRUE;
	    } else {
		nomul(-3, "being scared stiff", TRUE);
		nomovemsg = "";
		if (magr && magr == u.ustuck && sticks(youmonst.data)) {
		    setustuck((struct monst *)0);
		    You("release %s!", mon_nam(magr));
		}
	    }
	} else {
	    if (rn2(2) && resist(mdef, WEAPON_CLASS, 0, NOTELL))
		resisted = TRUE;
	    else
		monflee(mdef, 3, FALSE, (mdef->mhp > *dmgptr));
	}
	if (!resisted) do_stun = FALSE;
	break;

    case MB_INDEX_STUN:
	do_stun = TRUE;		/* (this is redundant...) */
	break;

    case MB_INDEX_PROBE:
	if (youattack && (mb->spe == 0 || !rn2(3 * abs(mb->spe)))) {
	    pline_The("%s is insightful.", verb);
	    /* pre-damage status */
	    probe_monster(mdef);
	}
	break;
    }
    /* stun if that was selected and a worse effect didn't occur */
    if (do_stun) {
	if (youdefend)
	    make_stunned((HStun + 3), FALSE);
	else
	    mdef->mstun = 1;
	/* avoid extra stun message below if we used mb_verb["stun"] above */
	if (attack_indx == MB_INDEX_STUN) do_stun = FALSE;
    }
    /* lastly, all this magic can be confusing... */
    do_confuse = !rn2(12);
    if (do_confuse) {
	if (youdefend)
	    make_confused(HConfusion + 4, FALSE);
	else
	    mdef->mconf = 1;
    }

    if (youattack || youdefend || vis) {
	(void) upstart(hittee);	/* capitalize */
	if (resisted) {
	    pline("%s %s!", hittee, vtense(hittee, "resist"));
	    shieldeff(youdefend ? u.ux : mdef->mx,
		      youdefend ? u.uy : mdef->my);
	}
	if ((do_stun || do_confuse) && flags.verbose) {
	    char buf[BUFSZ];

	    buf[0] = '\0';
	    if (do_stun) strcat(buf, "stunned");
	    if (do_stun && do_confuse) strcat(buf, " and ");
	    if (do_confuse) strcat(buf, "confused");
	    pline("%s %s %s%c", hittee, vtense(hittee, "are"),
		  buf, (do_stun && do_confuse) ? '!' : '.');
	}
    }

    return result;
}
  
/* Function used when someone attacks someone else with an artifact
 * weapon.  Only adds the special (artifact) damage, and returns a 1 if it
 * did something special (in which case the caller won't print the normal
 * hit message).  This should be called once upon every artifact attack;
 * dmgval() no longer takes artifact bonuses into account.  Possible
 * extension: change the killer so that when an orc kills you with
 * Stormbringer it's "killed by Stormbringer" instead of "killed by an orc".
 */
boolean
artifact_hit(magr, mdef, otmp, dmgptr, dieroll)
struct monst *magr, *mdef;
struct obj *otmp;
int *dmgptr;
int dieroll; /* needed for Magicbane and vorpal blades */
{
	boolean youattack = (magr == &youmonst);
	boolean youdefend = (mdef == &youmonst);
	boolean vis = (!youattack && magr && cansee(magr->mx, magr->my))
	    || (!youdefend && cansee(mdef->mx, mdef->my))
	    || (youattack && u.uswallow && mdef == u.ustuck && !Blind);
	boolean realizes_damage;
	const char *wepdesc;
	static const char you[] = "you";
	char hittee[BUFSIZ];
	boolean special_applies;
	boolean willreturntrue = 0;

	strcpy(hittee, youdefend ? you : mon_nam(mdef));

	/* The following takes care of most of the damage, but not all--
	 * the exception being for level draining, which is specially
	 * handled.  Messages are done in this function, however.
	 */
	*dmgptr += spec_dbon(otmp, mdef, *dmgptr);

	if (spec_dbon_applies)
	    special_applies = TRUE;
	else {
	    const struct artifact *weap = get_artifact(otmp);
	    special_applies = weap && spec_applies(weap, mdef);
	}

	if (youattack && youdefend) {
	    impossible("attacking yourself with weapon?");
	    return FALSE;
	}

	realizes_damage = (youdefend || vis || 
			   /* feel the effect even if not seen */
			   (youattack && mdef == u.ustuck));

	/* the four basic attacks: fire, cold, shock and missiles */
	if (attacks(AD_FIRE, otmp)) {
	    if (realizes_damage)
		pline_The("fiery weapon %s %s%c",
			!spec_dbon_applies ? "hits" :
			(mdef->data == &mons[PM_WATER_ELEMENTAL]) ?
			"vaporizes part of" : "burns",
			hittee, !spec_dbon_applies ? '.' : '!');
	    if (!rn2(50)) (void) destroy_mitem(mdef, POTION_CLASS, AD_FIRE);
	    if (!rn2(50)) (void) destroy_mitem(mdef, SCROLL_CLASS, AD_FIRE);
	    if (!rn2(75)) (void) destroy_mitem(mdef, SPBOOK_CLASS, AD_FIRE);
	    if (youdefend && Slimed) burn_away_slime();
	    if (realizes_damage) willreturntrue = 1;
	}
	if (attacks(AD_COLD, otmp)) {
	    if (realizes_damage)
		pline_The("ice-cold weapon %s %s%c",
			!spec_dbon_applies ? "hits" : "freezes",
			hittee, !spec_dbon_applies ? '.' : '!');
	    if (!rn2(100)) (void) destroy_mitem(mdef, POTION_CLASS, AD_COLD);
	    if (realizes_damage) willreturntrue = 1;
	}
	if (attacks(AD_ELEC, otmp)) {
	    if (realizes_damage)
		pline_The("electrified weapon hits%s %s%c",
			  !spec_dbon_applies ? "" : "!  Lightning strikes",
			  hittee, !spec_dbon_applies ? '.' : '!');
	    if (!rn2(150)) (void) destroy_mitem(mdef, RING_CLASS, AD_ELEC);
	    if (!rn2(150)) (void) destroy_mitem(mdef, WAND_CLASS, AD_ELEC);
	    if (realizes_damage) willreturntrue = 1;
	}
	if (attacks(AD_MAGM, otmp)) {
	    if (realizes_damage)
		pline_The("imaginary widget hits%s %s%c",
			  !spec_dbon_applies ? "" :
				"!  A hail of magic missiles strikes",
			  hittee, !spec_dbon_applies ? '.' : '!');
	    if (realizes_damage) willreturntrue = 1;
	}

	if (attacks(AD_STUN, otmp) && dieroll <= MB_MAX_DIEROLL) {
	    /* Magicbane's special attacks (possibly modifies hittee[]) */
	    if (Mb_hit(magr, mdef, otmp, dmgptr, dieroll, vis, hittee)) willreturntrue = 1;
	}

	if (!special_applies) {
	    /* since damage bonus didn't apply, nothing more to do;  
	       no further attacks have side-effects on inventory */
	    /* [ALI] The Tsurugi of Muramasa has no damage bonus but
	       is handled below so avoid early exit if SPFX_BEHEAD set
	       and the defender is vulnerable */
	    /* Amy edit: way too many special cases, in the case of
		 doubt we have to go through the remaining possibilities
		 anyway to ensure none are missed
	     * edit again: but we need to make sure only susceptible
		 monsters are affected! */

	    return FALSE;
	}

	if(otmp->oartifact == ART_REAVER){
	 if(youattack){
	  if(mdef->minvent && (Role_if(PM_PIRATE) || !rn2(10) ) ){
		struct obj *otmp2, **minvent_ptr;
		long unwornmask;

		if((otmp2 = mdef->minvent) != 0) {
			/* take the object away from the monster */
			obj_extract_self(otmp2);
			if ((unwornmask = otmp2->owornmask) != 0L) {
				mdef->misc_worn_check &= ~unwornmask;
				if (otmp2->owornmask & W_WEP) {
					setmnotwielded(mdef,otmp2);
					MON_NOWEP(mdef);
				}
				otmp2->owornmask = 0L;
				update_mon_intrinsics(mdef, otmp2, FALSE, FALSE);
			}
			/* give the object to the character */
			otmp2 = (Role_if(PM_PIRATE) || Role_if(PM_KORSAIR) || (uwep && uwep->oartifact == ART_ARRRRRR_MATEY) ) ? 
				hold_another_object(otmp2, "Ye snatched but dropped %s.",
						   doname(otmp2), "Ye steal: ") :
				hold_another_object(otmp2, "You snatched but dropped %s.",
						   doname(otmp2), "You steal: ");
			if (otmp2->otyp == CORPSE &&
				touch_petrifies(&mons[otmp2->corpsenm]) && (!uarmg || FingerlessGloves)) {
				char kbuf[BUFSZ];

				sprintf(kbuf, "stolen %s corpse", mons[otmp2->corpsenm].mname);
				instapetrify(kbuf);
			}
			/* more take-away handling, after theft message */
			if (unwornmask & W_WEP) {		/* stole wielded weapon */
				possibly_unwield(mdef, FALSE);
			} else if (unwornmask & W_ARMG) {	/* stole worn gloves */
				mselftouch(mdef, (const char *)0, TRUE);
				if (mdef->mhp <= 0)	/* it's now a statue */
					return 1; /* monster is dead */
			}
		}
	  }
	 }
	 else if(youdefend){
		char buf[BUFSZ];
		buf[0] = '\0';
		steal(magr, buf, FALSE, FALSE);
	 }
	 else{
		struct obj *obj;
		/* find an object to steal, non-cursed if magr is tame */
		for (obj = mdef->minvent; obj; obj = obj->nobj)
		    if (!magr->mtame || !obj->cursed)
				break;

		if (obj) {
			char buf[BUFSZ], onambuf[BUFSZ], mdefnambuf[BUFSZ];

			/* make a special x_monnam() call that never omits
			   the saddle, and save it for later messages */
			strcpy(mdefnambuf, x_monnam(mdef, ARTICLE_THE, (char *)0, 0, FALSE));
			if (u.usteed == mdef &&
					obj == which_armor(mdef, W_SADDLE))
				/* "You can no longer ride <steed>." */
				dismount_steed(DISMOUNT_POLY);
			obj_extract_self(obj);
			if (obj->owornmask) {
				mdef->misc_worn_check &= ~obj->owornmask;
				if (obj->owornmask & W_WEP)
				    setmnotwielded(mdef,obj);
				obj->owornmask = 0L;
				update_mon_intrinsics(mdef, obj, FALSE, FALSE);
			}
			/* add_to_minv() might free obj [if it merges] */
			if (vis)
				strcpy(onambuf, doname(obj));
			(void) add_to_minv(magr, obj);
			if (vis) {
				strcpy(buf, Monnam(magr));
				pline("%s steals %s from %s!", buf,
				    onambuf, mdefnambuf);
			}
			possibly_unwield(mdef, FALSE);
			mdef->mstrategy &= ~STRAT_WAITFORU;
			mselftouch(mdef, (const char *)0, FALSE);
			if (mdef->mhp <= 0)
				return 1;
		}
	 }
	}

	/* STEPHEN WHITE'S NEW CODE */
	if (otmp->oartifact == ART_SERPENT_S_TONGUE || otmp->oartifact == ART_DIRGE || otmp->oartifact == ART_HALLOW_MOONFALL || otmp->oartifact == ART_QUEUE_STAFF || otmp->oartifact == ART_SNAKELASH || otmp->oartifact == ART_SWORD_OF_BHELEU) {
	    otmp->dknown = TRUE;
	    pline_The("twisted weapon poisons %s!",
		    youdefend ? "you" : mon_nam(mdef));
	    if (youdefend ? (Poison_resistance && (StrongPoison_resistance || rn2(10)) ) : resists_poison(mdef)) {
		if (youdefend)
		    You("are not affected by the poison.");
		else
		    pline("%s seems unaffected by the poison.", Monnam(mdef));
		willreturntrue = 1;
	    }
	    switch (rnd(10)) {
		case 1:
		case 2:
		case 3:
		case 4:
		    *dmgptr += d(1,6) + 2;
		    break;
		case 5:
		case 6:
		case 7:
		    *dmgptr += d(2,6) + 4;
		    break;
		case 8:
		case 9:
		    *dmgptr += d(3,6) + 6;
		    break;
		case 10:
			if (!rn2(20) && !(youdefend && uarms && uarms->oartifact == ART_ANTINSTANT_DEATH) ) {
		    pline_The("poison was deadly...");
		    *dmgptr = 2 *
			    (youdefend ? Upolyd ? u.mh : u.uhp : mdef->mhp) +
			    FATAL_DAMAGE_MODIFIER;
			}
			else { *dmgptr += d(4,6) + 6; }
		    break;
	    }
	    willreturntrue = 1;
	}

       if (otmp->oartifact == ART_DOOMBLADE && dieroll < 6) {
	    if (youattack)
		You("plunge the Doomblade deeply into %s!",
			mon_nam(mdef));
	    else
		pline("%s plunges the Doomblade deeply into %s!",
			Monnam(magr), hittee);
		if (youattack && (PlayerHearsSoundEffects)) pline(issoviet ? "Tak chto vy dumayete, vy mozhete bit' igru tol'ko potomu, chto vy nashli artefakt. Bednyy zabluzhdayutsya dusha." : "Doaaaaaai!");
	    *dmgptr += rnd(4) * 5;
	    willreturntrue = 1;
       }
      /* END OF STEPHEN WHITE'S NEW CODE */

#if 0
	   if (otmp->oartifact == ART_SCALPEL && dieroll < 5) {
		/* faster than a speeding bullet is the Gray Mouser... */
		pline("There is a flurry of blows!");
		/* I suppose this could theoretically continue forever... */
		while (dieroll < 5) {
		   *dmgptr += rnd(8) + 1 + otmp->spe;
		   dieroll = rn2(11);
		}
	   }
	   if (otmp->oartifact == ART_HEARTSEEKER && dieroll < 3) {
		/* this weapon just sounds nasty... yuck... */
		if (!youdefend) {
		   You("plunge Heartseeker into %s!",mon_nam(mdef));
		} else {
		   pline("%s plunges Heartseeker into you!",mon_nam(mdef));
		}
		*dmgptr += rnd(6)+rnd(6)+rnd(6)+rnd(6)+4;
	   }
#endif


	/* We really want "on a natural 20" but Nethack does it in */
	/* reverse from AD&D. */
	if (spec_ability(otmp, SPFX_BEHEAD)) {
	    if ( (otmp->oartifact == ART_TSURUGI_OF_MURAMASA || otmp->oartifact == ART_GAYSECT || otmp->oartifact == ART_DRAGONCLAN_SWORD || otmp->oartifact == ART_KILLING_EDGE) && dieroll < 2) {
		wepdesc = "The razor-sharp blade";
		/* not really beheading, but so close, why add another SPFX */
		if (youattack && u.uswallow && mdef == u.ustuck) {
		    You("slice %s wide open!", mon_nam(mdef));
		    *dmgptr = 2 * mdef->mhp + FATAL_DAMAGE_MODIFIER;
		    willreturntrue = 1;
		    goto beheadingdone;
		}
		if (!youdefend) {
			/* allow normal cutworm() call to add extra damage */
			if(notonhead) {
				goto beheadingdone;
			}

			if (bigmonst(mdef->data)) {
				if (youattack)
					You("slice deeply into %s!",
						mon_nam(mdef));
				else if (vis)
					pline("%s cuts deeply into %s!",
					      Monnam(magr), hittee);
				*dmgptr *= 2;
				willreturntrue = 1;
				goto beheadingdone;
			}
			*dmgptr = 2 * mdef->mhp + FATAL_DAMAGE_MODIFIER;
			pline("%s cuts %s in half!", wepdesc, mon_nam(mdef));
			otmp->dknown = TRUE;
			willreturntrue = 1;
			goto beheadingdone;
		} else {
			/* Invulnerable player won't be bisected */
			if (bigmonst(youmonst.data) || StrongDiminishedBleeding || Invulnerable || (Stoned_chiller && Stoned) ) {
				pline("%s cuts deeply into you!",
				      magr ? Monnam(magr) : wepdesc);
				*dmgptr *= 2;
				willreturntrue = 1;
				goto beheadingdone;
			}

			/* Players with negative AC's take less damage instead
			 * of just not getting hit.  We must add a large enough
			 * value to the damage so that this reduction in
			 * damage does not prevent death.
			 */
			*dmgptr = 2 * (Upolyd ? u.mh : u.uhp) + FATAL_DAMAGE_MODIFIER;
			pline("%s cuts you in half!", wepdesc);
			otmp->dknown = TRUE;
			willreturntrue = 1;
			goto beheadingdone;
		}
	    } else if (dieroll < 2 || otmp->oartifact == ART_VORPAL_BLADE && mdef->data->mlet == S_JABBERWOCK) {
		static const char * const behead_msg[2] = {
		     "%s beheads %s!",
		     "%s decapitates %s!"
		};

		if (youattack && u.uswallow && mdef == u.ustuck) {
			goto beheadingdone;
		}
		wepdesc = artilist[otmp->oartifact].name;
		if (!youdefend) {
			if (!has_head(mdef->data) || notonhead || u.uswallow) {
				if (youattack)
					pline("Somehow, you miss %s wildly.",
						mon_nam(mdef));
				else if (vis)
					pline("Somehow, %s misses wildly.",
						mon_nam(magr));
				*dmgptr = 0;
				if (youattack || vis) willreturntrue = 1;
				goto beheadingdone;
			}
			if (noncorporeal(mdef->data) || amorphous(mdef->data)) {
				pline("%s slices through %s %s.", wepdesc,
				      s_suffix(mon_nam(mdef)),
				      mbodypart(mdef,NECK));
				willreturntrue = 1;
				goto beheadingdone;
			}
			*dmgptr = 2 * mdef->mhp + FATAL_DAMAGE_MODIFIER;
			pline(behead_msg[rn2(SIZE(behead_msg))],
			      wepdesc, mon_nam(mdef));
			otmp->dknown = TRUE;
			willreturntrue = 1;
			goto beheadingdone;
		} else {

			if (!has_head(youmonst.data) || Role_if(PM_COURIER)) {
				pline("Somehow, %s misses you wildly.",
				      magr ? mon_nam(magr) : wepdesc);
				*dmgptr = 0;
				willreturntrue = 1;
				goto beheadingdone;
			}

			if (uamul && uamul->otyp == AMULET_OF_NECK_BRACE) {
				pline("Somehow, %s misses you wildly.",
				      magr ? mon_nam(magr) : wepdesc);
				*dmgptr = 0;
				willreturntrue = 1;
				goto beheadingdone;
			}

			if (powerfulimplants() && uimplant && uimplant->oartifact == ART_DECAPITATION_UP) {
				pline("Somehow, %s misses you wildly.",
				      magr ? mon_nam(magr) : wepdesc);
				*dmgptr = 0;
				willreturntrue = 1;
				goto beheadingdone;

			}

			if (uarmh && itemhasappearance(uarmh, APP_COMPLETE_HELMET) ) {
				pline("%s slices into your %s.",
				      wepdesc, body_part(NECK));
				willreturntrue = 1;
				goto beheadingdone;

			}
			if (RngeAntiBeheading) {
				pline("%s slices into your %s.",
				      wepdesc, body_part(NECK));
				willreturntrue = 1;
				goto beheadingdone;

			}

			if (noncorporeal(youmonst.data) || amorphous(youmonst.data)) {
				pline("%s slices through your %s.",
				      wepdesc, body_part(NECK));
				willreturntrue = 1;
				goto beheadingdone;
			}
			*dmgptr = 2 * (Upolyd ? u.mh : u.uhp)
				  + FATAL_DAMAGE_MODIFIER;
			if (Invulnerable || (Stoned_chiller && Stoned)) {
				pline("%s slices into your %s.",
				      wepdesc, body_part(NECK));
				willreturntrue = 1;
				goto beheadingdone;
			}
			pline(behead_msg[rn2(SIZE(behead_msg))],
			      wepdesc, "you");
			otmp->dknown = TRUE;
			/* Should amulets fall off? */
			willreturntrue = 1;
			goto beheadingdone;
		}
	    }
	}
beheadingdone:
	if (spec_ability(otmp, SPFX_DRLI)) {

		if (!youdefend) {
		    if (!resists_drli(mdef)) {
			if (vis) {
			    if(otmp->oartifact == ART_STORMBRINGER)
				pline_The("%s blade draws the life from %s!",
				      hcolor(NH_BLACK),
				      mon_nam(mdef));
#if 0	/* OBSOLETE */
			    else if(otmp->oartifact == ART_TENTACLE_STAFF)
				pline("The writhing tentacles draw the life from %s!",
				      mon_nam(mdef));
#endif
			    else
				pline("%s draws the life from %s!",
				      The(distant_name(otmp, xname)),
				      mon_nam(mdef));
			}
			if (mdef->m_lev == 0) {
			    *dmgptr = 2 * mdef->mhp + FATAL_DAMAGE_MODIFIER;
			} else {
			    int drain = rnd(8);
			    *dmgptr += drain;
			    mdef->mhpmax -= drain;
			    mdef->m_lev--;
			    drain /= 2;
			    if (drain && youattack) healup(drain, 0, FALSE, FALSE);
			    else if (drain && magr) healup_mon(magr, drain, 0, FALSE, FALSE);
			}
			if (vis) willreturntrue = 1;
		    }
		} else if (!Drain_resistance) { /* youdefend */
			int oldhpmax = u.uhpmax;

			if (Blind)
				You_feel("an %s drain your life!",
				    otmp->oartifact == ART_STORMBRINGER ?
				    "unholy blade" : "object");
			else if (otmp->oartifact == ART_STORMBRINGER)
				pline_The("%s blade drains your life!",
				      hcolor(NH_BLACK));
			else
				pline("%s drains your life!",
				      The(distant_name(otmp, xname)));
			losexp("life drainage", FALSE, TRUE);
			if (magr && magr->mhp < magr->mhpmax) {
			    magr->mhp += (oldhpmax - u.uhpmax)/2;
			    if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			}
			willreturntrue = 1;
		}
	}
	/* WAC -- 1/6 chance of cancellation with foobane weapons */
	if (otmp->oartifact == ART_GIANTKILLER ||
	    otmp->oartifact == ART_ORCRIST ||
	    otmp->oartifact == ART_DRAGONBANE ||
	    otmp->oartifact == ART_DEMONBANE ||
	    otmp->oartifact == ART_WEREBANE ||
	    otmp->oartifact == ART_TROLLSBANE ||
	    otmp->oartifact == ART_THIEFBANE ||
	    otmp->oartifact == ART_OGRESMASHER ||
	    otmp->oartifact == ART_ELFRIST) {
		if (dieroll < 4) {
		    if (realizes_damage) {
			pline("%s %s!", The(distant_name(otmp, xname)), Blind ?
				"roars deafeningly" : "shines brilliantly");
			pline("It strikes %s!", hittee);
		    }
		    cancel_monst(mdef, otmp, youattack, TRUE, magr == mdef);
		    willreturntrue = 1;
		}
	}
	if (willreturntrue) return TRUE;

	return FALSE;
}

static NEARDATA const char recharge_type[] = { ALLOW_COUNT, ALL_CLASSES, 0 };
static NEARDATA const char invoke_types[] = { ALL_CLASSES, 0 };
		/* #invoke: an "ugly check" filters out most objects */

int
doinvoke()
{
    register struct obj *obj, *acqo;

	if (is_pentagram(u.ux, u.uy)) {
		if (yn("Invoke the magical pentagram?") == 'y') {

			You("attune yourself with the magical energy.");
			u.cnd_pentagramamount++;

			if (u.ualign.type == A_CHAOTIC) adjalign(1);

			if (rn2(3)) {
				pline("Your mana increases.");
				u.uenmax++;
			} else switch (rnd(28)) {

				case 1:
					HTeleport_control += 2;
					tele();
					break;
				case 2:
					{
					acqo = mkobj_at(SPBOOK_CLASS, u.ux, u.uy, FALSE, FALSE);
					if (acqo) {
						acqo->bknown = acqo->known = TRUE;
						pline("A book appeared at your %s!", makeplural(body_part(FOOT)));
					} else {
						pline("Nothing happens...");
						if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
							pline("Oh wait, actually something bad happens...");
							badeffect();
						}
					  }
					}
					break;
				case 3:
					(void) monster_detect((struct obj *)0, 0);
					exercise(A_WIS, TRUE);
					break;
				case 4:
					trap_detect((struct obj *)0);
					break;
				case 5:
					object_detect((struct obj *)0, 0);
					break;
				case 6:
					{
					boolean havegifts = u.ugifts;

					if (!havegifts) u.ugifts++;

					acqo = mk_artifact((struct obj *)0, !rn2(3) ? A_CHAOTIC : rn2(2) ? A_NEUTRAL : A_LAWFUL, TRUE);
					if (acqo) {
					    dropy(acqo);
						if (P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_ISRESTRICTED) {
						    unrestrict_weapon_skill(get_obj_skill(acqo, TRUE));
						} else if (P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_UNSKILLED) {
							unrestrict_weapon_skill(get_obj_skill(acqo, TRUE));
							P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_BASIC;
						} else if (rn2(2) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_BASIC) {
							P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_SKILLED;
						} else if (!rn2(4) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_SKILLED) {
							P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_EXPERT;
						} else if (!rn2(10) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_EXPERT) {
							P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_MASTER;
						} else if (!rn2(100) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_MASTER) {
							P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_GRAND_MASTER;
						} else if (!rn2(200) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_GRAND_MASTER) {
							P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_SUPREME_MASTER;
						}

						if (Race_if(PM_RUSMOT)) {
							if (P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_ISRESTRICTED) {
							    unrestrict_weapon_skill(get_obj_skill(acqo, TRUE));
							} else if (P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_UNSKILLED) {
								unrestrict_weapon_skill(get_obj_skill(acqo, TRUE));
								P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_BASIC;
							} else if (rn2(2) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_BASIC) {
								P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_SKILLED;
							} else if (!rn2(4) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_SKILLED) {
								P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_EXPERT;
							} else if (!rn2(10) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_EXPERT) {
								P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_MASTER;
							} else if (!rn2(100) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_MASTER) {
								P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_GRAND_MASTER;
							} else if (!rn2(200) && P_MAX_SKILL(get_obj_skill(acqo, TRUE)) == P_GRAND_MASTER) {
								P_MAX_SKILL(get_obj_skill(acqo, TRUE)) = P_SUPREME_MASTER;
							}
						}

					    discover_artifact(acqo->oartifact);

						if (!havegifts) u.ugifts--;
						pline("An artifact appeared beneath you!");

					}	

					else pline("Opportunity knocked, but nobody was home.  Bummer.");

					}

					break;
				case 7:
					pline("The RNG decides to curse-weld an item to you.");
					bad_artifact_xtra();
					break;
				case 8:
					{
					int aggroamount = rnd(6);
					if (isfriday) aggroamount *= 2;
					u.aggravation = 1;
					reset_rndmonst(NON_PM);
					while (aggroamount) {
						u.cnd_aggravateamount++;
						makemon((struct permonst *)0, u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
						aggroamount--;
						if (aggroamount < 0) aggroamount = 0;
					}
					u.aggravation = 0;
					pline("Several monsters come out of a portal.");

					}

					break;
				case 9:
					pline("Your body suddenly becomes all stiff!");
					nomul(-rnd(15), "paralyzed by a pentagram", TRUE);
					break;
				case 10:

					pline("The dungeon is getting more chaotic!");
					{
					int madepool = 0;
					do_clear_areaX(u.ux, u.uy, 12, do_terrainfloodP, (void *)&madepool);
					}

					break;
				case 11:
					You_feel("powered up!");
					u.uenmax += rnd(5);
					u.uen = u.uenmax;
					break;
				case 12:
					pline("Suddenly, you gain a new companion!");
					(void) make_familiar((struct obj *)0, u.ux, u.uy, FALSE);
					break;
				case 13:
					{

					if (Aggravate_monster) {
						u.aggravation = 1;
						reset_rndmonst(NON_PM);
					}

					    coord dd;
					    coord cc;
					    int cx,cy;
						int i;
						int randsp, randmnst, randmnsx;
						struct permonst *randmonstforspawn;
						int monstercolor;

				      cx = rn2(COLNO);
				      cy = rn2(ROWNO);

					if (!rn2(4)) {

					randsp = (rn2(14) + 2);
					if (!rn2(10)) randsp *= 2;
					if (!rn2(100)) randsp *= 3;
					if (!rn2(1000)) randsp *= 5;
					if (!rn2(10000)) randsp *= 10;
					randmnst = (rn2(187) + 1);
					randmnsx = (rn2(100) + 1);

					pline("You suddenly feel a surge of tension!");

					for (i = 0; i < randsp; i++) {
					/* This function will fill the map with a random amount of monsters of one class. --Amy */

					if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) continue;

					if (randmnst < 6)
				 	    (void) makemon(mkclass(S_ANT,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 9)
				 	    (void) makemon(mkclass(S_BLOB,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 11)
				 	    (void) makemon(mkclass(S_COCKATRICE,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 15)
				 	    (void) makemon(mkclass(S_DOG,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 18)
				 	    (void) makemon(mkclass(S_EYE,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 22)
				 	    (void) makemon(mkclass(S_FELINE,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 24)
				 	    (void) makemon(mkclass(S_GREMLIN,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 29)
				 	    (void) makemon(mkclass(S_HUMANOID,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 33)
				 	    (void) makemon(mkclass(S_IMP,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 36)
				 	    (void) makemon(mkclass(S_JELLY,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 41)
				 	    (void) makemon(mkclass(S_KOBOLD,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 44)
				 	    (void) makemon(mkclass(S_LEPRECHAUN,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 47)
				 	    (void) makemon(mkclass(S_MIMIC,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 50)
				 	    (void) makemon(mkclass(S_NYMPH,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 54)
				 	    (void) makemon(mkclass(S_ORC,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 55)
				 	    (void) makemon(mkclass(S_PIERCER,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 58)
				 	    (void) makemon(mkclass(S_QUADRUPED,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 62)
				 	    (void) makemon(mkclass(S_RODENT,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 65)
				 	    (void) makemon(mkclass(S_SPIDER,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 66)
				 	    (void) makemon(mkclass(S_TRAPPER,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 69)
				 	    (void) makemon(mkclass(S_UNICORN,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 71)
				 	    (void) makemon(mkclass(S_VORTEX,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 73)
				 	    (void) makemon(mkclass(S_WORM,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 75)
				 	    (void) makemon(mkclass(S_XAN,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 76)
				 	    (void) makemon(mkclass(S_LIGHT,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 77)
				 	    (void) makemon(mkclass(S_ZOUTHERN,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 78)
				 	    (void) makemon(mkclass(S_ANGEL,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 81)
				 	    (void) makemon(mkclass(S_BAT,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 83)
				 	    (void) makemon(mkclass(S_CENTAUR,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 86)
				 	    (void) makemon(mkclass(S_DRAGON,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 89)
				 	    (void) makemon(mkclass(S_ELEMENTAL,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 94)
				 	    (void) makemon(mkclass(S_FUNGUS,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 99)
				 	    (void) makemon(mkclass(S_GNOME,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 102)
				 	    (void) makemon(mkclass(S_GIANT,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 103)
				 	    (void) makemon(mkclass(S_JABBERWOCK,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 104)
				 	    (void) makemon(mkclass(S_KOP,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 105)
				 	    (void) makemon(mkclass(S_LICH,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 108)
				 	    (void) makemon(mkclass(S_MUMMY,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 110)
				 	    (void) makemon(mkclass(S_NAGA,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 113)
				 	    (void) makemon(mkclass(S_OGRE,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 115)
				 	    (void) makemon(mkclass(S_PUDDING,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 116)
				 	    (void) makemon(mkclass(S_QUANTMECH,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 118)
				 	    (void) makemon(mkclass(S_RUSTMONST,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 121)
				 	    (void) makemon(mkclass(S_SNAKE,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 123)
				 	    (void) makemon(mkclass(S_TROLL,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 124)
				 	    (void) makemon(mkclass(S_UMBER,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 125)
				 	    (void) makemon(mkclass(S_VAMPIRE,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 127)
				 	    (void) makemon(mkclass(S_WRAITH,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 128)
				 	    (void) makemon(mkclass(S_XORN,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 130)
				 	    (void) makemon(mkclass(S_YETI,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 135)
				 	    (void) makemon(mkclass(S_ZOMBIE,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 145)
				 	    (void) makemon(mkclass(S_HUMAN,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 147)
				 	    (void) makemon(mkclass(S_GHOST,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 149)
				 	    (void) makemon(mkclass(S_GOLEM,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 152)
				 	    (void) makemon(mkclass(S_DEMON,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 155)
				 	    (void) makemon(mkclass(S_EEL,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 160)
				 	    (void) makemon(mkclass(S_LIZARD,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 162)
				 	    (void) makemon(mkclass(S_BAD_FOOD,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 165)
				 	    (void) makemon(mkclass(S_BAD_COINS,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 166) {
						if (randmnsx < 96)
				 	    (void) makemon(mkclass(S_HUMAN,0), cx, cy, MM_ADJACENTOK);
						else
				 	    (void) makemon(mkclass(S_NEMESE,0), cx, cy, MM_ADJACENTOK);
						}
					else if (randmnst < 171)
				 	    (void) makemon(mkclass(S_GRUE,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 176)
				 	    (void) makemon(mkclass(S_WALLMONST,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 180)
				 	    (void) makemon(mkclass(S_RUBMONST,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 181) {
						if (randmnsx < 99)
				 	    (void) makemon(mkclass(S_HUMAN,0), cx, cy, MM_ADJACENTOK);
						else
				 	    (void) makemon(mkclass(S_ARCHFIEND,0), cx, cy, MM_ADJACENTOK);
						}
					else if (randmnst < 186)
				 	    (void) makemon(mkclass(S_TURRET,0), cx, cy, MM_ADJACENTOK);
					else if (randmnst < 187)
				 	    (void) makemon(mkclass(S_FLYFISH,0), cx, cy, MM_ADJACENTOK);
					else
				 	    (void) makemon((struct permonst *)0, cx, cy, MM_ADJACENTOK);

					}

					}

					else if (!rn2(3)) {

					randsp = (rn2(14) + 2);
					if (!rn2(10)) randsp *= 2;
					if (!rn2(100)) randsp *= 3;
					if (!rn2(1000)) randsp *= 5;
					if (!rn2(10000)) randsp *= 10;
					randmonstforspawn = rndmonst();

					You_feel("the arrival of monsters!");

					for (i = 0; i < randsp; i++) {

						if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

						(void) makemon(randmonstforspawn, cx, cy, MM_ADJACENTOK);
					}

					}

					else if (!rn2(2)) {

					randsp = (rn2(14) + 2);
					if (!rn2(10)) randsp *= 2;
					if (!rn2(100)) randsp *= 3;
					if (!rn2(1000)) randsp *= 5;
					if (!rn2(10000)) randsp *= 10;
					monstercolor = rnd(15);
					do { monstercolor = rnd(15); } while (monstercolor == CLR_BLUE);

					You_feel("a colorful sensation!");

					for (i = 0; i < randsp; i++) {

						if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

						(void) makemon(colormon(monstercolor), cx, cy, MM_ADJACENTOK);
					}

					}

					else {

					randsp = (rn2(14) + 2);
					if (!rn2(10)) randsp *= 2;
					if (!rn2(100)) randsp *= 3;
					if (!rn2(1000)) randsp *= 5;
					if (!rn2(10000)) randsp *= 10;
					monstercolor = rnd(376);

					You_feel("that a group has arrived!");

					for (i = 0; i < randsp; i++) {

						if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

						(void) makemon(specialtensmon(monstercolor), cx, cy, MM_ADJACENTOK);
					}

					}

					u.aggravation = 0;

					}
					break;
				case 14:

					if (u.uhunger < 1500) {
						pline("Your %s fills.", body_part(STOMACH));
						u.uhunger = 1500;
						u.uhs = 1; /* NOT_HUNGRY */
						flags.botl = 1;
					} else {
						pline("Nothing happens...");
						if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
							pline("Oh wait, actually something bad happens...");
							badeffect();
						}
					}
					break;
				case 15:
					if (u.ualign.record < -1) {
						adjalign(-(u.ualign.record / 2));
						You_feel("partially absolved.");
					} else {
						u.alignlim++;
						adjalign(10);
						You_feel("appropriately %s.", align_str(u.ualign.type));
					}
					break;
				case 16:
					{

					/* occasionally get extremely lucky --Amy */
					if (!rn2(50)) {
						u.weapon_slots++;
						You("feel very skillful, and gain an extra skill slot!");
						break;
					}

					int nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
					if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
					int blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
					if (!blackngdur ) blackngdur = 500; /* fail safe */

					pline("Your mana increases.");
					u.uenmax++;
					/* nasty trap effect - no extra message because, well, nastiness! --Amy */
					randomnastytrapeffect(rnz(nastytrapdur * (monster_difficulty() + 1)), (blackngdur - (monster_difficulty() * 3)));

					}
					break;
				case 17:
					{
					int i = rn2(A_MAX);
					adjattrib(i, 1, 0, TRUE);
					adjattrib(i, 1, 0, TRUE);
					adjattrib(i, 1, 0, TRUE);
					adjattrib(i, 1, 0, TRUE);
					adjattrib(i, 1, 0, TRUE);
					}
					break;
				case 18:
					Your("intrinsics change.");
					intrinsicgainorloss();
					break;
				case 19:
					{
					struct obj *pseudo;
					pseudo = mksobj(SCR_ITEM_GENOCIDE, FALSE, 2, FALSE);
					if (!pseudo) {
						pline("Nothing happens...");
						if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
							pline("Oh wait, actually something bad happens...");
							badeffect();
						}
						break;
					}
					if (pseudo->otyp == GOLD_PIECE) pseudo->otyp = SCR_ITEM_GENOCIDE;
					(void) seffects(pseudo);
					obfree(pseudo, (struct obj *)0);	/* now, get rid of it */

					}

					break;
				case 20:
					doubleskilltraining();
					break;
				case 21:
					if (!(HAggravate_monster & INTRINSIC) && !(HAggravate_monster & TIMEOUT)) {

						int maxtrainingamount = 0;
						int skillnumber = 0;
						int actualskillselection = 0;
						int amountofpossibleskills = 1;
						int i;

						for (i = 0; i < P_NUM_SKILLS; i++) {
							if (P_SKILL(i) != P_ISRESTRICTED) continue;

							if (P_ADVANCE(i) > 0 && P_ADVANCE(i) >= maxtrainingamount) {
								if (P_ADVANCE(i) > maxtrainingamount) {
									amountofpossibleskills = 1;
									skillnumber = i;
									maxtrainingamount = P_ADVANCE(i);
								} else if (!rn2(amountofpossibleskills + 1)) {
									amountofpossibleskills++;
									skillnumber = i;
								} else {
									amountofpossibleskills++;
								}
							}
						}

						if (skillnumber > 0 && maxtrainingamount > 0) {
							unrestrict_weapon_skill(skillnumber);

							register int maxcap = P_BASIC;
							if (!rn2(2)) {
								maxcap = P_SKILLED;
								if (!rn2(2)) {
									maxcap = P_EXPERT;
									if (maxtrainingamount >= 20 && !rn2(2)) {
										maxcap = P_MASTER;
										if (maxtrainingamount >= 160 && !rn2(2)) {
											maxcap = P_GRAND_MASTER;
											if (maxtrainingamount >= 540 && !rn2(2)) {
												maxcap = P_SUPREME_MASTER;
											}
										}
									}
								}
							}

							P_MAX_SKILL(skillnumber) = maxcap;
							pline("You can now learn the %s skill, with a new cap of %s.", P_NAME(skillnumber), maxcap == P_SUPREME_MASTER ? "supreme master" : maxcap == P_GRAND_MASTER ? "grand master" : maxcap == P_MASTER ? "master" : maxcap == P_EXPERT ? "expert" : maxcap == P_SKILLED ? "skilled" : "basic");
						} else {
							pline("Nothing happens...");
							if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
								pline("Oh wait, actually something bad happens...");
								badeffect();
							}
						}

					}

					if (HAggravate_monster & INTRINSIC) {
						HAggravate_monster &= ~INTRINSIC;
						You_feel("more acceptable!");
					}
					if (HAggravate_monster & TIMEOUT) {
						HAggravate_monster &= ~TIMEOUT;
						You_feel("more acceptable!");
					}
					break;
				case 22:
					{
						u.aggravation = 1;
						reset_rndmonst(NON_PM);
						int attempts = 0;
						register struct permonst *ptrZ;
newboss:
					do {

						ptrZ = rndmonst();
						attempts++;
						if (!rn2(2000)) reset_rndmonst(NON_PM);

					} while ( (!ptrZ || (ptrZ && !(ptrZ->geno & G_UNIQ))) && attempts < 50000);

					if (ptrZ && ptrZ->geno & G_UNIQ) {
						if (wizard) pline("monster generation: %s", ptrZ->mname);
						(void) makemon(ptrZ, u.ux, u.uy, MM_ANGRY);
					}
					else if (rn2(50)) {
						attempts = 0;
						goto newboss;
					}
					if (!rn2(10) ) {
						attempts = 0;
						goto newboss;
					}
					pline("Boss monsters appear from nowhere!");

					}
					u.aggravation = 0;

					break;
				case 23:
					if (!rn2(6400)) {
						ragnarok(TRUE);
						if (evilfriday) evilragnarok(TRUE,level_difficulty());

					}

					u.aggravation = 1;
					u.heavyaggravation = 1;
					DifficultyIncreased += 1;
					HighlevelStatus += 1;
					EntireLevelMode += 1;

					(void) makemon(mkclass(S_NEMESE,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);

					u.aggravation = 0;
					u.heavyaggravation = 0;

					break;
				case 24:
					wonderspell();
					break;
				case 25:

					{
					int tryct = 0;
					int x, y;
					register struct trap *ttmp;
					for (tryct = 0; tryct < 2000; tryct++) {
						x = rn1(COLNO-3,2);
						y = rn2(ROWNO);

						if (x && y && isok(x, y) && (levl[x][y].typ > DBWALL) && !(t_at(x, y)) ) {
								ttmp = maketrap(x, y, randomtrap(), 0);
							if (ttmp) {
								ttmp->tseen = 0;
								ttmp->hiddentrap = 1;
							}
							if (!rn2(5)) break;
						}
					}

					You_feel("in grave danger...");
					}
					break;
				case 26:
					badeffect();
					break;
				case 27:
					if (!uinsymbiosis) {
						getrandomsymbiote(FALSE);
						pline("Suddenly you have a symbiote!");
					} else {
						u.usymbiote.mhpmax += rnd(10);
						if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
						flags.botl = TRUE;
						Your("symbiote seems much stronger now.");
					}
					break;
				case 28:
					decontaminate(100);
					You_feel("decontaminated.");
					break;
				default:
					impossible("undefined pentagram effect");
					break;

			}

			if (!rn2(6) && IS_PENTAGRAM(levl[u.ux][u.uy].typ)) {
				levl[u.ux][u.uy].typ = ROOM;
				pline_The("pentagram fades away completely.");
				newsym(u.ux,u.uy);
			}

			return 1;
		}
	}

    obj = getobj(invoke_types, "invoke");
    if (!obj) return 0;
    if (obj->oartifact && !touch_artifact(obj, &youmonst)) return 1;
    return arti_invoke(obj);
}

STATIC_OVL int
arti_invoke(obj)
    register struct obj *obj;
{
    register const struct artifact *oart = get_artifact(obj);
	    register struct monst *mtmp;
	    register struct monst *mtmp2;
	    register struct permonst *pm;

    int summon_loop;
    int unseen;
/*
    int kill_loop;
 */

	/* highly randomized timeout; squeaking skill helps --Amy */
	int artitimeout = rnz(2000);
	if (!rn2(5)) artitimeout = rnz(20000);
	if (!PlayerCannotUseSkills) {
		switch (P_SKILL(P_SQUEAKING)) {
	      	case P_BASIC:	artitimeout *= 9; artitimeout /= 10; break;
	      	case P_SKILLED:	artitimeout *= 8; artitimeout /= 10; break;
	      	case P_EXPERT:	artitimeout *= 7; artitimeout /= 10; break;
	      	case P_MASTER:	artitimeout *= 6; artitimeout /= 10; break;
	      	case P_GRAND_MASTER:	artitimeout *= 5; artitimeout /= 10; break;
	      	case P_SUPREME_MASTER:	artitimeout *= 4; artitimeout /= 10; break;
	      	default: break;
		}
	}
	if (artitimeout < 1) artitimeout = 1; /* fail safe */

    if(!oart || !oart->inv_prop) {
	if(obj->otyp == CRYSTAL_BALL)
	    use_crystal_ball(obj);
	else {
		pline("%s", nothing_happens);
		if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
			pline("Oh wait, actually something bad happens...");
			badeffect();
		}
	}
	return 1;
    }

    if(oart->inv_prop > LAST_PROP) {
	/* It's a special power, not "just" a property */

	if (obj->oartifact >= ART_ORB_OF_DETECTION && !is_quest_artifact(obj)) {
		pline("Tapping into the powers of artifacts that don't belong to you is dangerous.");
		contaminate(rnz(100), TRUE);
		increasesanity(rnz(100));
		adjalign(-rnz(100));
	}

	if(obj->age > monstermoves) {
	    /* the artifact is tired :-) */
	    You_feel("that %s %s ignoring you.",
		     the(xname(obj)), otense(obj, "are"));
	    /* and just got more so; patience is essential... */
	    obj->age += (long) d(3,10);
	    if (!rn2(5)) obj->age += (long) rnz(100);
	    return 1;
	}
	obj->age = monstermoves + artitimeout;
	use_skill(P_SQUEAKING, rnd(10));

	u.cnd_invokecount++;

	switch(oart->inv_prop) {
	case TAMING: {
	    struct obj pseudo;

	    pseudo = zeroobj;	/* neither cursed nor blessed */
	    pseudo.otyp = SCR_TAMING;
	    (void) seffects(&pseudo);
	    break;
	  }
	case IDENTIFY: {
		struct obj *pseudo = mksobj(SPE_IDENTIFY, FALSE, 2, FALSE);
		if (!pseudo) break;
		pseudo->blessed = pseudo->cursed = 0;
		pseudo->quan = 42L;		/* do not let useup get it */
		(void) seffects(pseudo);
		obfree(pseudo, (struct obj *) 0);

		if (obj && obj->oartifact == ART_ARKENSTONE_OF_THRAIN) {
			curse(obj);
			pline("The power of the Arkenstone is uncontrollable, but you knew that.");
			badeffect(); badeffect(); badeffect(); badeffect(); badeffect(); badeffect(); badeffect(); badeffect(); badeffect(); badeffect();
		}

		break;
	    }
	case HEALING: {
	    int healamt = (u.uhpmax + 1 - u.uhp) / 2;
	    long creamed = (long)u.ucreamed;

	    if (Upolyd) healamt = (u.mhmax + 1 - u.mh) / 2;
	    if (healamt || Sick || Slimed || Blinded > creamed)
		You_feel("better.");
	    else
		goto nothing_special;
	    if (healamt > 0) {
		if (Upolyd) u.mh += healamt;
		else u.uhp += healamt;
	    }
	    if(Sick) make_sick(0L,(char *)0,FALSE,SICK_ALL);
	    if(Slimed) Slimed = 0L;
	    if (Blinded > creamed) make_blinded(creamed, FALSE);
	    flags.botl = 1;
	    break;
	  }
	case ENERGY_BOOST: {
	    int epboost = (u.uenmax + 1 - u.uen) / 2;
	    if (epboost > 120) epboost = 120;		/* arbitrary */
	    else if (epboost < 12) epboost = u.uenmax - u.uen;
	    if(epboost) {
		You_feel("re-energized.");
		u.uen += epboost;
		flags.botl = 1;
	    } else
		goto nothing_special;
	    break;
	  }
	case UNTRAP: {
	    if(!untrap(TRUE)) {
		obj->age = 0; /* don't charge for changing their mind */
		return 0;
	    }
	    break;
	  }
	case CHARGE_OBJ:
chargingchoice:
	    {
	    struct obj *otmp = getobj(recharge_type, "charge");
	    boolean b_effect;

	    if (!otmp) {
		if (yn("Really exit with no object selected?") == 'y')
			pline("You just wasted the opportunity to charge your items.");
		else goto chargingchoice;
		obj->age = 0;
		return 0;
	    }
	    b_effect = obj->blessed &&
		(Role_switch == oart->role || !oart->role);
	    recharge(otmp, b_effect ? 1 : obj->cursed ? -1 : 0);
	    update_inventory();
	    break;
	  }
	case LEV_TELE:
	      if (!flags.lostsoul && !flags.uberlostsoul && !(flags.wonderland && !(u.wonderlandescape)) && !(iszapem && !(u.zapemescape)) && !(u.uprops[STORM_HELM].extrinsic) && !(In_bellcaves(&u.uz)) && !(In_subquest(&u.uz)) && !(In_voiddungeon(&u.uz)) && !(In_netherrealm(&u.uz))) level_tele();
		else pline("You are disallowed to use this ability.");
	    break;
	case DRAGON_BREATH:
	    getdir(NULL);
	    buzz(20+AD_FIRE-1,6,u.ux,u.uy,u.dx,u.dy);
	/*       ^^^^^^^^^^^^ - see zap.c / ZT_* defines */
	    break;
	case LIGHT_AREA:
	    if (!Blind)
		pline("%s shines brightly for an instant!", The(xname(obj)));
	    else
		pline("%s grows warm for a second!", The(xname(obj)));
	    litroom(TRUE, obj);
	    vision_recalc(0);
	    if (is_undead(youmonst.data)) {
		You("burn in the radiance!");
		/* This is ground zero.  Not good news ... */
		u.uhp /= 100;
		if (u.uhp < 1) {
		    u.youaredead = 1;
		    u.uhp = 0;
		    killer_format = KILLED_BY;
		    killer = "the Holy Spear of Light";
		    done(DIED);
		    u.youaredead = 0;
		}
	    }
	    /* Undead and Demonics can't stand the light */
	    unseen = 0;
	    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		if (DEADMONSTER(mtmp)) continue;
	    	if (distu(mtmp->mx, mtmp->my) > 9*9) continue;
		if (couldsee(mtmp->mx, mtmp->my) &&
			(is_undead(mtmp->data) || is_demon(mtmp->data)) &&
			!resist(mtmp, '\0', 0, TELL)) {
		    if (canseemon(mtmp))
			pline("%s burns in the radiance!", Monnam(mtmp));
		    else
			unseen++;
		    /* damage depends on distance, divisor ranges from 10 to 2 */
		    mtmp->mhp /= (10 - (distu(mtmp->mx, mtmp->my) / 10));
		    if (mtmp->mhp < 1) mtmp->mhp = 1;
		}
	    }
	    if (unseen)
		You_hear("%s of intense pain!", unseen > 1 ? "cries" : "a cry");
	    break;
	case DEATH_GAZE:
	    if (u.uluck < -9) {
		u.youaredead = 1;
		pline_The("Eye turns on you!");
		u.uhp = 0;
		killer_format = KILLED_BY;
		killer = "the Eye of the Beholder";
		done(DIED);
		u.youaredead = 0;
	    }
	    pline_The("Eye looks around with its icy gaze!");
	    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		if (DEADMONSTER(mtmp)) continue;
		/* The eye is never blind ... */
		if (couldsee(mtmp->mx, mtmp->my) && !is_undead(mtmp->data)) {
		    pline("%s screams in agony!", Monnam(mtmp));
		    mtmp->mhp /= 3;
		    if (mtmp->mhp < 1) mtmp->mhp = 1;
		}
	    }
	    /* Tsk,tsk.. */
	    adjalign(-3);
	    u.uluck -= 3;
	    break;
	case SUMMON_UNDEAD:
	    if (u.uluck < -9) {
		u.uhp -= rn2(20) + 5;
		pline_The("Hand claws you with its icy nails!");
		if (u.uhp <= 0) {
		    u.youaredead = 1;
		    killer_format = KILLED_BY;
		    killer="the Hand of Vecna";
		    done(DIED);
		    u.youaredead = 0;
		}
	    }
	    summon_loop = rn2(4) + 4;
	    pline("Creatures from the grave surround you!");

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

	    do {
		switch (rn2(6) + 1) {
		    case 1:
			pm = mkclass(S_VAMPIRE, 0);
			break;
		    case 2:
		    case 3:
			pm = mkclass(S_ZOMBIE, 0);
			break;
		    case 4:
			pm = mkclass(S_MUMMY, 0);
			break;
		    case 5:
			pm = mkclass(S_GHOST, 0);
			break;
		    default:
			pm = mkclass(S_WRAITH, 0);
			break;
		}
		mtmp = makemon(pm, u.ux, u.uy, NO_MM_FLAGS);
	        if ((mtmp2 = tamedog(mtmp, (struct obj *)0, FALSE)) != 0)
		    mtmp = mtmp2;
		mtmp->mtame = 30;
		summon_loop--;
	    } while (summon_loop);

		u.aggravation = 0;

	    /* Tsk,tsk.. */
	    adjalign(-3);
	    u.uluck -= 3;
	    break;
	case PROT_POLY:
	    You_feel("more observant.");
	    rescham();
	    break;
	case SUMMON_FIRE_ELEMENTAL:
	    pm = &mons[PM_FIRE_ELEMENTAL];
	    mtmp = makemon(pm, u.ux, u.uy, NO_MM_FLAGS);
   
	    pline("You summon an elemental.");
   
	    if ((mtmp2 = tamedog(mtmp, (struct obj *)0, FALSE) ) != 0)
			mtmp = mtmp2;
	    mtmp->mtame = 30;
	    break;
	case SUMMON_WATER_ELEMENTAL:
	    pm = &mons[PM_WATER_ELEMENTAL];
	    mtmp = makemon(pm, u.ux, u.uy, NO_MM_FLAGS);
   
	    pline("You summon an elemental.");
	    
	    if ((mtmp2 = tamedog(mtmp, (struct obj *)0, FALSE) ) != 0)
			mtmp = mtmp2;
	    mtmp->mtame = 30;
	    break;
	case OBJ_DETECTION:
		(void)object_detect(obj, 0);
		break;
	case CREATE_PORTAL: 
		if (flags.lostsoul || flags.uberlostsoul || (flags.wonderland && !(u.wonderlandescape)) || (iszapem && !(u.zapemescape)) || u.uprops[STORM_HELM].extrinsic || In_bellcaves(&u.uz) || In_subquest(&u.uz) || In_voiddungeon(&u.uz) || In_netherrealm(&u.uz)) break;
				{
	    int i, num_ok_dungeons, last_ok_dungeon = 0;
	    d_level newlev;
	    extern int n_dgns; /* from dungeon.c */
	    winid tmpwin = create_nhwindow(NHW_MENU);
	    anything any;

	    any.a_void = 0;	/* set all bits to zero */
	    if (Is_blackmarket(&u.uz) && *u.ushops) {
		You_feel("very disoriented for a moment.");
		break;
	    }
	    start_menu(tmpwin);
	    /* use index+1 (cant use 0) as identifier */
	    for (i = num_ok_dungeons = 0; i < n_dgns; i++) {
		if (!dungeons[i].dunlev_ureached) continue;

		/* prevent player from ch3ating past the invocation ritual in wonderland mode --Amy */
		if (flags.wonderland && !achieve.perform_invocation) {
			if (!strcmp(dungeons[i].dname, "Yendorian Tower")) continue;
			if (!strcmp(dungeons[i].dname, "Forging Chamber")) continue;
			if (!strcmp(dungeons[i].dname, "Dead Grounds")) continue;
			if (!strcmp(dungeons[i].dname, "Ordered Chaos")) continue;
			if (!strcmp(dungeons[i].dname, "Resting Zone TA")) continue;
			if (!strcmp(dungeons[i].dname, "Resting Zone TB")) continue;
			if (!strcmp(dungeons[i].dname, "Resting Zone TC")) continue;
			if (!strcmp(dungeons[i].dname, "Resting Zone TD")) continue;
			if (!strcmp(dungeons[i].dname, "Resting Zone TE")) continue;
			if (!strcmp(dungeons[i].dname, "Resting Zone TF")) continue;
			if (!strcmp(dungeons[i].dname, "Resting Zone TG")) continue;
			if (!strcmp(dungeons[i].dname, "Resting Zone TH")) continue;
			if (!strcmp(dungeons[i].dname, "Resting Zone TI")) continue;
			if (!strcmp(dungeons[i].dname, "Resting Zone TJ")) continue;
		}
		any.a_int = i+1;
		add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
			 dungeons[i].dname, MENU_UNSELECTED);
		num_ok_dungeons++;
		last_ok_dungeon = i;
	    }
	    end_menu(tmpwin, "Open a portal to which dungeon?");
	    if (num_ok_dungeons > 1) {
		/* more than one entry; display menu for choices */
		menu_item *selected;
		int n;

		n = select_menu(tmpwin, PICK_ONE, &selected);
		if (n <= 0) {
		    destroy_nhwindow(tmpwin);
		    goto nothing_special;
		}
		i = selected[0].item.a_int - 1;
		free((void *)selected);
	    } else
		i = last_ok_dungeon;	/* also first & only OK dungeon */
	    destroy_nhwindow(tmpwin);

	    /*
	     * i is now index into dungeon structure for the new dungeon.
	     * Find the closest level in the given dungeon, open
	     * a use-once portal to that dungeon and go there.
	     * The closest level is either the entry or dunlev_ureached.
	     */
	    newlev.dnum = i;
	    if(dungeons[i].depth_start >= depth(&u.uz))
		newlev.dlevel = dungeons[i].entry_lev;
	    else
		newlev.dlevel = dungeons[i].dunlev_ureached;
	    if((u.uhave.amulet && !u.freeplaymode) || CannotTeleport || In_endgame(&u.uz) || (u.usteed && mon_has_amulet(u.usteed)) || In_endgame(&newlev) ||
	       newlev.dnum == u.uz.dnum) {
		You_feel("very disoriented for a moment.");
	    } else {
		if(!Blind) You("are surrounded by a shimmering sphere!");
		else You_feel("weightless for a moment.");
		if (practicantterror) {
			pline("%s thunders: 'That's not an allowed form of transportation! Are you really too lazy to take the elevator or the stairs? Well wait, I'll ground you for that offense. And additionally you also pay 10000 zorkmids to me!'", noroelaname());
			fineforpracticant(10000, 0, 0);
			NoReturnEffect += rnz(10000);

		}
		u.cnd_branchportcount++;
		goto_level(&newlev, FALSE, FALSE, FALSE);
	    }
	    if (obj && obj->oartifact == ART_BIZARRO_ORGASMATRON) {
		if (u.bizarrotries > 2) {
			u.uprops[DEAC_FAST].intrinsic += ((u.bizarrotries - 2) * 500);
			pline("Due to overexertion and too many orgasms, you feel very lethargic...");
			pline(u.inertia ? "You feel even slower." : "You slow down to a crawl.");
			u.inertia += ((u.bizarrotries - 2) * 500);

		}
		u.bizarrotries++;
	    }
	    break;
	  }
	case ENLIGHTENING:
	    enlightenment(0, 1);
	    break;
	case CREATE_AMMO: {
	    struct obj *otmp = mksobj(ARROW, TRUE, FALSE, FALSE);

	    if (!otmp) goto nothing_special;
	    otmp->blessed = obj->blessed;
	    otmp->cursed = obj->cursed;
	    otmp->bknown = obj->bknown;
	    if (obj->blessed) {
		if (otmp->spe < 0) otmp->spe = 0;
		otmp->quan += rnd(10);
	    } else if (obj->cursed) {
		if (otmp->spe > 0) otmp->spe = 0;
	    } else
		otmp->quan += rnd(5);
	    otmp->owt = weight(otmp);
	    otmp = hold_another_object(otmp, "Suddenly %s out.",
				       aobjnam(otmp, "fall"), (const char *)0);
	    break;
	case OBJECT_DET:
		object_detect(obj, 0);
		artifact_detect(obj);
		break;
	case PHASING:   /* Walk through walls and stone like a xorn */
        if (Passes_walls) goto nothing_special;
	    if (oart == &artilist[ART_IRON_BALL_OF_LIBERATION]) {
		if (Punished && (obj != uball)) {
		    unpunish(); /* Remove a mundane heavy iron ball */
		}
		
		if (!Punished) {
		    setworn(mkobj(CHAIN_CLASS, TRUE, FALSE), W_CHAIN);
		    setworn(obj, W_BALL);
		    /*uball->spe = 1;*/
		    if (!u.uswallow) {
			placebc();
			if (Blind) set_bc(1);	/* set up ball and chain variables */
			newsym(u.ux,u.uy);		/* see ball&chain if can't see self */
		    }
		    Your("%s chains itself to you!", xname(obj));
		}
	    }
        if (!FunnyHallu) {    
            Your("body begins to feel less solid.");
        } else {
            You_feel("one with the spirit world.");
        }
        incr_itimeout(&Phasing, (50 + rnd(100)));
        obj->age += Phasing; /* Time begins after phasing ends */
        break;
	  }
	}
    } else {
	long eprop = (u.uprops[oart->inv_prop].extrinsic ^= W_ARTI),
	     iprop = u.uprops[oart->inv_prop].intrinsic;
	boolean on = (eprop & W_ARTI) != 0; /* true if invoked prop just set */

	if (obj->oartifact >= ART_ORB_OF_DETECTION && !is_quest_artifact(obj)) {
		pline("Tapping into the powers of artifacts that don't belong to you is dangerous.");
		contaminate(rnz(100), TRUE);
		increasesanity(rnz(100));
		adjalign(-rnz(100));
	}

	if(on && obj->age > monstermoves) {
	    /* the artifact is tired :-) */
	    u.uprops[oart->inv_prop].extrinsic ^= W_ARTI;
	    You_feel("that %s %s ignoring you.",
		     the(xname(obj)), otense(obj, "are"));
	    /* can't just keep repeatedly trying */
	    obj->age += (long) d(3,10);
	    if (!rn2(5)) obj->age += (long) rnz(100);
	    return 1;
	} else if(!on) {
	    /* when turning off property, determine downtime */
	    /* arbitrary for now until we can tune this -dlc */
	    obj->age = monstermoves + artitimeout;
	    use_skill(P_SQUEAKING, rnd(10));
	}

	if ((eprop & ~W_ARTI) || iprop) {
nothing_special:
	    /* you had the property from some other source too */
	    if (carried(obj))
		You_feel("a surge of power, but nothing seems to happen.");
	    return 1;
	}
	switch(oart->inv_prop) {
	case CONFLICT:
	    if(on) You_feel("like a rabble-rouser.");
	    else You_feel("the tension decrease around you.");
	    break;
	case LEVITATION:
	    if(on) {
		float_up();
		spoteffects(FALSE);
	    } else (void) float_down(I_SPECIAL|TIMEOUT, W_ARTI);
	    break;
	case INVIS:
	    if (BInvis || Blind) goto nothing_special;
	    newsym(u.ux, u.uy);
	    if (on)
		    Your("body takes on a %s transparency...",
			 FunnyHallu ? "normal" : "strange");
	    else
		    Your("body seems to unfade...");
	    break;
	}
    }

    return 1;
}

/*
 * Artifact is dipped into water
 * -1 not handled here (not used up here)
 *  0 no effect but used up
 *  else return
 *  AD_FIRE, etc.
 *  Note caller should handle what happens to the medium in these cases.
 *      This only prints messages about the actual artifact.
 */

int
artifact_wet(obj, silent)
struct obj *obj;
boolean silent;
{
	 if (!obj->oartifact) return (-1);
	 switch (artilist[(int) (obj)->oartifact].attk.adtyp) {
		 case AD_FIRE:
			 if (!silent) {
				pline("A cloud of steam rises.");
				pline("%s is untouched.", The(xname(obj)));
			 }
			 return (AD_FIRE);
		 case AD_COLD:
			 if (!silent) {
				pline("Icicles form and fall from the freezing %s.",
			             the(xname(obj)));
			 }
			 return (AD_COLD);
		 case AD_ELEC:
			 if (!silent) {
				pline_The("humid air crackles with electricity from %s.",
						the(xname(obj)));
			 }
			 return (AD_ELEC);
		 case AD_DRLI:
			 if (!silent) {
				pline("%s absorbs the water!", The(xname(obj)));
			 }
			 return (AD_DRLI);
		 default:
			 break;
	}
	return (-1);
}

/* WAC return TRUE if artifact is always lit */
boolean
artifact_light(obj)
    struct obj *obj;
{
    return get_artifact(obj) && (obj->oartifact == ART_SUNSWORD ||
	    obj->oartifact == ART_HOLY_SPEAR_OF_LIGHT ||
	    obj->oartifact == ART_CANDLE_OF_ETERNAL_FLAME);
}

/* KMH -- Talking artifacts are finally implemented */
void
arti_speak(obj)
    struct obj *obj;
{
	if (!obj) {
		impossible("arti_speak called with no valid object");
		return;
	}

	register const struct artifact *oart = get_artifact(obj);
	const char *line;
	char buf[BUFSZ];


	/* Is this a speaking artifact? */
	if (!oart || !(oart->spfx & SPFX_SPEAK))
		return;

	line = getrumor(bcsign(obj), buf, TRUE);
	if (!*line)
		line = "Slash'EM rumors file closed for renovation.";
	pline("%s:", Tobjnam(obj, "whisper"));
	verbalize("%s", line);
	return;
}

boolean
artifact_has_invprop(otmp, inv_prop)
struct obj *otmp;
/*uchar*/int inv_prop;
{
	const struct artifact *arti = get_artifact(otmp);

	return((boolean)(arti && (arti->inv_prop == inv_prop)));
}

/* Return the price sold to the hero of a given artifact or unique item */
long
arti_cost(otmp)
struct obj *otmp;
{
	if (!otmp->oartifact)
	    return ((long)objects[otmp->otyp].oc_cost);
	else if (artilist[(int) otmp->oartifact].cost)
	    return (artilist[(int) otmp->oartifact].cost);
	else
	    return (100L * (long)objects[otmp->otyp].oc_cost);
}

static const char *random_seasound[] = {
	"distant waves",
	"distant surf",
	"the distant sea",
	"the call of the ocean",
	"waves against the shore",
	"flowing water",
	"the sighing of waves",
	"quarrelling gulls",
	"the song of the deep",
	"rumbling in the deeps",
	"the singing of Eidothea",
	"the laughter of the protean nymphs",
	"rushing tides",
	"the elusive sea change",
	"the silence of the sea",
	"the passage of the albatross",
	"dancing raindrops",
	"coins rolling on the seabed",
	"treasure galleons crumbling in the depths",
	"waves lapping against a hull"
};

/* Polymorph obj contents */
void
arti_poly_contents(obj)
    struct obj *obj;
{
    struct obj *dobj = 0;  /*object to be deleted*/
    struct obj *otmp;
	You_hear("%s.",random_seasound[rn2(SIZE(random_seasound))]);
	for (otmp = obj->cobj; otmp; otmp = otmp->nobj){
		if (dobj) {
			delobj(dobj);
			dobj = 0;
		}
		if(!obj_resists(otmp, 5, 95) && !stack_too_big(otmp) ){
			/* KMH, conduct */
			u.uconduct.polypiles++;
			/* any saved lock context will be dangerously obsolete */
			if (Is_box(otmp)) (void) boxlock(otmp, obj);

			if (obj_shudders(otmp)) {
				dobj = otmp;
			}
			else otmp = poly_obj(otmp, STRANGE_OBJECT, TRUE);
		}
	}
	if (dobj) {
		delobj(dobj);
		dobj = 0;
	}
}

int
find_wildtalentartifact()
{
	int artifactnumber = artilist[ART_ELLI_S_PSEUDOBAND_OF_POS].otyp;
	return artifactnumber;
}

int
find_prostituteartifact()
{
	int artifactnumber = artilist[ART_HIGHEST_FEELING].otyp;
	return artifactnumber;
}

int
find_kurwaartifact()
{
	int artifactnumber = artilist[ART_LORSKEL_S_INTEGRITY].otyp;
	return artifactnumber;
}

/* Function that adds or removes a random intrinsic from the player. --Amy */
void
intrinsicgainorloss()
{
	register boolean intloss = rn2(2);

	boolean hasmadeachange = 0;
	int tryct = 0;

retrytrinsic:
	if (!rn2(5)) intloss = rn2(2);

	if (rn2(4)) { /* ones that can easily be gained by eating things */
		switch (rnd(16)) {

			case 1:
				if (intloss) {
					if (HFire_resistance & INTRINSIC) {
						HFire_resistance &= ~INTRINSIC;
						You_feel("warmer.");
						hasmadeachange = 1;
					}
					if (HFire_resistance & TIMEOUT) {
						HFire_resistance &= ~TIMEOUT;
						You_feel("warmer.");
						hasmadeachange = 1;
					}
				} else {
					if(!(HFire_resistance & FROMOUTSIDE)) {
						You(FunnyHallu ? "be chillin'." : "feel a momentary chill.");
						HFire_resistance |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 2:
				if (intloss) {
					if (HTeleportation & INTRINSIC) {
						HTeleportation &= ~INTRINSIC;
						You_feel("less jumpy.");
						hasmadeachange = 1;
					}
					if (HTeleportation & TIMEOUT) {
						HTeleportation &= ~TIMEOUT;
						You_feel("less jumpy.");
						hasmadeachange = 1;
					}
				} else {
					if(!(HTeleportation & FROMOUTSIDE)) {
						You_feel(FunnyHallu ? "diffuse." : "very jumpy.");
						HTeleportation |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 3:
				if (intloss) {
					if (HPoison_resistance & INTRINSIC) {
						HPoison_resistance &= ~INTRINSIC;
						You_feel("a little sick!");
						hasmadeachange = 1;
					}
					if (HPoison_resistance & TIMEOUT) {
						HPoison_resistance &= ~TIMEOUT;
						You_feel("a little sick!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HPoison_resistance & FROMOUTSIDE)) {
						You_feel(Poison_resistance ? "especially healthy." : "healthy.");
						HPoison_resistance |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 4:
				if (intloss) {
					if (HTelepat & INTRINSIC) {
						HTelepat &= ~INTRINSIC;
						Your("senses fail!");
						hasmadeachange = 1;
					}
					if (HTelepat & TIMEOUT) {
						HTelepat &= ~TIMEOUT;
						Your("senses fail!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HTelepat & FROMOUTSIDE)) {
						You_feel(FunnyHallu ? "in touch with the cosmos." : "a strange mental acuity.");
						HTelepat |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 5:
				if (intloss) {
					if (HCold_resistance & INTRINSIC) {
						HCold_resistance &= ~INTRINSIC;
						You_feel("cooler.");
						hasmadeachange = 1;
					}
					if (HCold_resistance & TIMEOUT) {
						HCold_resistance &= ~TIMEOUT;
						You_feel("cooler.");
						hasmadeachange = 1;
					}
				} else {
					if(!(HCold_resistance & FROMOUTSIDE)) {
						You_feel("full of hot air.");
						HCold_resistance |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 6:
				if (intloss) {
					if (HInvis & INTRINSIC) {
						HInvis &= ~INTRINSIC;
						You_feel("paranoid.");
						hasmadeachange = 1;
					}
					if (HInvis & TIMEOUT) {
						HInvis &= ~TIMEOUT;
						You_feel("paranoid.");
						hasmadeachange = 1;
					}
				} else {
					if(!(HInvis & FROMOUTSIDE)) {
						You_feel("less visible.");
						HInvis |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 7:
				if (intloss) {
					if (HSee_invisible & INTRINSIC) {
						HSee_invisible &= ~INTRINSIC;
						You("%s!", FunnyHallu ? "tawt you taw a puttie tat" : "thought you saw something");
						hasmadeachange = 1;
					}
					if (HSee_invisible & TIMEOUT) {
						HSee_invisible &= ~TIMEOUT;
						You("%s!", FunnyHallu ? "tawt you taw a puttie tat" : "thought you saw something");
						hasmadeachange = 1;
					}
				} else {
					if(!(HSee_invisible & FROMOUTSIDE)) {
						You_feel("your vision sharpen.");
						HSee_invisible |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 8:
				if (intloss) {
					if (HFast & INTRINSIC) {
						HFast &= ~INTRINSIC;
						You_feel("slower.");
						hasmadeachange = 1;
					}
					if (HFast & TIMEOUT) {
						HFast &= ~TIMEOUT;
						You_feel("slower.");
						hasmadeachange = 1;
					}
				} else {
					if(!(HFast & FROMOUTSIDE)) {
						You_feel("faster.");
						HFast |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 9:
				if (intloss) {
					if (HAggravate_monster & INTRINSIC) {
						HAggravate_monster &= ~INTRINSIC;
						You_feel("less attractive.");
						hasmadeachange = 1;
					}
					if (HAggravate_monster & TIMEOUT) {
						HAggravate_monster &= ~TIMEOUT;
						You_feel("less attractive.");
						hasmadeachange = 1;
					}
				} else {
					if(!(HAggravate_monster & FROMOUTSIDE)) {
						You_feel("monsters setting up portals.");
						HAggravate_monster |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 10:
				if (intloss) {
					if (HSleep_resistance & INTRINSIC) {
						HSleep_resistance &= ~INTRINSIC;
						You_feel("tired all of a sudden.");
						hasmadeachange = 1;
					}
					if (HSleep_resistance & TIMEOUT) {
						HSleep_resistance &= ~TIMEOUT;
						You_feel("tired all of a sudden.");
						hasmadeachange = 1;
					}
				} else {
					if(!(HSleep_resistance & FROMOUTSIDE)) {
						You_feel("wide awake.");
						HSleep_resistance |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 11:
				if (intloss) {
					if (HDisint_resistance & INTRINSIC) {
						HDisint_resistance &= ~INTRINSIC;
						You_feel("like you're going to break apart.");
						hasmadeachange = 1;
					}
					if (HDisint_resistance & TIMEOUT) {
						HDisint_resistance &= ~TIMEOUT;
						You_feel("like you're going to break apart.");
						hasmadeachange = 1;
					}
				} else {
					if(!(HDisint_resistance & FROMOUTSIDE)) {
						You_feel(FunnyHallu ? "totally together, man." : "very firm.");
						HDisint_resistance |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 12:
				if (intloss) {
					if (HShock_resistance & INTRINSIC) {
						HShock_resistance &= ~INTRINSIC;
						You_feel("like someone has zapped you.");
						hasmadeachange = 1;
					}
					if (HShock_resistance & TIMEOUT) {
						HShock_resistance &= ~TIMEOUT;
						You_feel("like someone has zapped you.");
						hasmadeachange = 1;
					}
				} else {
					if(!(HShock_resistance & FROMOUTSIDE)) {
						if (FunnyHallu)
							You_feel("grounded in reality.");
						else
							Your("health currently feels amplified!");
						HShock_resistance |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 13:
				if (intloss) {
					if (HTeleport_control & INTRINSIC) {
						HTeleport_control &= ~INTRINSIC;
						You_feel("unable to control where you're going.");
						hasmadeachange = 1;
					}
					if (HTeleport_control & TIMEOUT) {
						HTeleport_control &= ~TIMEOUT;
						You_feel("unable to control where you're going.");
						hasmadeachange = 1;
					}
				} else {
					if(!(HTeleport_control & FROMOUTSIDE)) {
						You_feel(FunnyHallu ? "centered in your personal space." : "in control of yourself.");
						HTeleport_control |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 14:
				if (intloss) {
					if (HAcid_resistance & INTRINSIC) {
						HAcid_resistance &= ~INTRINSIC;
						You_feel("worried about corrosion!");
						hasmadeachange = 1;
					}
					if (HAcid_resistance & TIMEOUT) {
						HAcid_resistance &= ~TIMEOUT;
						You_feel("worried about corrosion!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HAcid_resistance & FROMOUTSIDE)) {
						You(FunnyHallu ? "wanna do more acid!" : "feel less afraid of corrosives.");
						HAcid_resistance |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 15:
				{
				int foopathynumber = rnd(13);
				switch (foopathynumber) {

					case 1:

						if (intloss) {
							if (HHallu_party & INTRINSIC) {
								HHallu_party &= ~INTRINSIC;
								You_feel("that the party is over!");
								hasmadeachange = 1;
							}
							if (HHallu_party & TIMEOUT) {
								HHallu_party &= ~TIMEOUT;
								You_feel("that the party is over!");
								hasmadeachange = 1;
							}
						} else {
							if(!(HHallu_party & FROMOUTSIDE)) {
								You_feel(FunnyHallu ?
								    "like throwing wild parties with lots of sexy girls! Yeah!" :
								    "a strange desire to celebrate.");
								HHallu_party |= FROMOUTSIDE;
								hasmadeachange = 1;
							}
						}
						break;
					case 2:

						if (intloss) {
							if (HDrunken_boxing & INTRINSIC) {
								HDrunken_boxing &= ~INTRINSIC;
								You_feel("a little drunk!");
								hasmadeachange = 1;
							}
							if (HDrunken_boxing & TIMEOUT) {
								HDrunken_boxing &= ~TIMEOUT;
								You_feel("a little drunk!");
								hasmadeachange = 1;
							}
						} else {
							if(!(HDrunken_boxing & FROMOUTSIDE)) {
								You_feel(FunnyHallu ? "like Mike Tyson!" : "ready for a good brawl.");
								HDrunken_boxing |= FROMOUTSIDE;
								hasmadeachange = 1;
							}
						}
						break;
					case 3:

						if (intloss) {
							if (HStunnopathy & INTRINSIC) {
								HStunnopathy &= ~INTRINSIC;
								You_feel("an uncontrolled stunning!");
								hasmadeachange = 1;
							}
							if (HStunnopathy & TIMEOUT) {
								HStunnopathy &= ~TIMEOUT;
								You_feel("an uncontrolled stunning!");
								hasmadeachange = 1;
							}
						} else {
							if(!(HStunnopathy & FROMOUTSIDE)) {
								You_feel(FunnyHallu ? "a controlled wobbling! Feels like being on a bouncy ship!" : "steadily observant.");
								HStunnopathy |= FROMOUTSIDE;
								hasmadeachange = 1;
							}
						}
						break;
					case 4:

						if (intloss) {
							if (HNumbopathy & INTRINSIC) {
								HNumbopathy &= ~INTRINSIC;
								You_feel("numbness spreading through your body!");
								hasmadeachange = 1;
							}
							if (HNumbopathy & TIMEOUT) {
								HNumbopathy &= ~TIMEOUT;
								You_feel("numbness spreading through your body!");
								hasmadeachange = 1;
							}
						} else {
							if(!(HNumbopathy & FROMOUTSIDE)) {
								You_feel(FunnyHallu ?
						    "as if a sweet woman were clamping your toes with a block-heeled combat boot!" :
					    "a numb feeling spreading through your body. Somehow, it doesn't feel bad at all...");
								HNumbopathy |= FROMOUTSIDE;
								hasmadeachange = 1;
							}
						}
						break;
					case 5:

						if (intloss) {
							if (HDimmopathy & INTRINSIC) {
								HDimmopathy &= ~INTRINSIC;
								You_feel(FunnyHallu ? "that your marriage is no longer safe..." : "worried about the future!");
								hasmadeachange = 1;
							}
							if (HDimmopathy & TIMEOUT) {
								HDimmopathy &= ~TIMEOUT;
								You_feel(FunnyHallu ? "that your marriage is no longer safe..." : "worried about the future!");
								hasmadeachange = 1;
							}
						} else {
							if(!(HDimmopathy & FROMOUTSIDE)) {
								HDimmopathy |= FROMOUTSIDE;
								You_feel(FunnyHallu ?
								    "like your wife was contemplating a breakup, but then you realize that she's gonna stay with you to the end of all time." :
								    "a little down. But then, good feelings overcome you.");
								hasmadeachange = 1;
							}
						}
						break;
					case 6:

						if (intloss) {
							if (HFreezopathy & INTRINSIC) {
								HFreezopathy &= ~INTRINSIC;
								You_feel("ice-cold!");
								hasmadeachange = 1;
							}
							if (HFreezopathy & TIMEOUT) {
								HFreezopathy &= ~TIMEOUT;
								You_feel("ice-cold!");
								hasmadeachange = 1;
							}
						} else {
							if(!(HFreezopathy & FROMOUTSIDE)) {
								You_feel(FunnyHallu ? "like eating a big cone of ice-cream - mmmmmmmm!" : "icy.");
								HFreezopathy |= FROMOUTSIDE;
								hasmadeachange = 1;
							}
						}
						break;
					case 7:

						if (intloss) {
							if (HStoned_chiller & INTRINSIC) {
								HStoned_chiller &= ~INTRINSIC;
								You_feel("that you ain't gonna get time for relaxing anymore!");
								hasmadeachange = 1;
							}
							if (HStoned_chiller & TIMEOUT) {
								HStoned_chiller &= ~TIMEOUT;
								You_feel("that you ain't gonna get time for relaxing anymore!");
								hasmadeachange = 1;
							}
						} else {
							if(!(HStoned_chiller & FROMOUTSIDE)) {
								You_feel(FunnyHallu ? "that you're simply the best - yeah, no shit, man!" :     "like relaxing on a couch.");
								HStoned_chiller |= FROMOUTSIDE;
								hasmadeachange = 1;
							}
						}
						break;
					case 8:

						if (intloss) {
							if (HCorrosivity & INTRINSIC) {
								HCorrosivity &= ~INTRINSIC;
								You_feel("the protective layer on your skin disappearing!");
								hasmadeachange = 1;
							}
							if (HCorrosivity & TIMEOUT) {
								HCorrosivity &= ~TIMEOUT;
								You_feel("the protective layer on your skin disappearing!");
								hasmadeachange = 1;
							}
						} else {
							if(!(HCorrosivity & FROMOUTSIDE)) {
								You_feel(FunnyHallu ? "like you just got splashed with gunks of acid!" : "an acidic burning.");
								HCorrosivity |= FROMOUTSIDE;
								hasmadeachange = 1;
							}
						}
						break;
					case 9:

						if (intloss) {
							if (HFear_factor & INTRINSIC) {
								HFear_factor &= ~INTRINSIC;
								You_feel("fearful!");
								hasmadeachange = 1;
							}
							if (HFear_factor & TIMEOUT) {
								HFear_factor &= ~TIMEOUT;
								You_feel("fearful!");
								hasmadeachange = 1;
							}
						} else {
							if(!(HFear_factor & FROMOUTSIDE)) {
								You_feel(FunnyHallu ? "like you're always running - from something! And the 'something' is a prostitute." : "ready to face your fears.");
								HFear_factor |= FROMOUTSIDE;
								hasmadeachange = 1;
							}
						}
						break;
					case 10:

						if (intloss) {
							if (HBurnopathy & INTRINSIC) {
								HBurnopathy &= ~INTRINSIC;
								You_feel("red-hot!");
								hasmadeachange = 1;
							}
							if (HBurnopathy & TIMEOUT) {
								HBurnopathy &= ~TIMEOUT;
								You_feel("red-hot!");
								hasmadeachange = 1;
							}
						} else {
							if(!(HBurnopathy & FROMOUTSIDE)) {
								You_feel(FunnyHallu ? "super burninated by enemy with very tired!" : "a burning inside. Strangely, it feels quite soothing.");
								HBurnopathy |= FROMOUTSIDE;
								hasmadeachange = 1;
							}
						}
						break;
					case 11:

						if (intloss) {
							if (HSickopathy & INTRINSIC) {
								HSickopathy &= ~INTRINSIC;
								You_feel("a loss of medical knowledge!");
								hasmadeachange = 1;
							}
							if (HSickopathy & TIMEOUT) {
								HSickopathy &= ~TIMEOUT;
								You_feel("a loss of medical knowledge!");
								hasmadeachange = 1;
							}
						} else {
							if(!(HSickopathy & FROMOUTSIDE)) {
								You_feel(FunnyHallu ?
							    "that you just smoked some really wacky stuff! What the heck was in there?" :
						    "ill for a moment, but get the feeling that you know more about diseases now.");
								HSickopathy |= FROMOUTSIDE;
								hasmadeachange = 1;
							}
						}
						break;
					case 12:

						if (intloss) {
							if (HWonderlegs & INTRINSIC) {
								HWonderlegs &= ~INTRINSIC;
								You_feel("that all girls and women will scratch bloody wounds on your legs with their high heels!");
								hasmadeachange = 1;
							}
							if (HWonderlegs & TIMEOUT) {
								HWonderlegs &= ~TIMEOUT;
								You_feel("that all girls and women will scratch bloody wounds on your legs with their high heels!");
								hasmadeachange = 1;
							}
						} else {
							if(!(HWonderlegs & FROMOUTSIDE)) {
								You_feel(FunnyHallu ?
								    "a wonderful sensation in your shins, like they were just kicked by female hugging boots! How lovely!" :
							    "like having your legs scratched up and down by sexy leather pumps.");
								HWonderlegs |= FROMOUTSIDE;
								hasmadeachange = 1;
							}
						}
						break;
					case 13:

						if (intloss) {
							if (HGlib_combat & INTRINSIC) {
								HGlib_combat &= ~INTRINSIC;
								You_feel("fliction in your %s!", makeplural(body_part(HAND)));
								hasmadeachange = 1;
							}
							if (HGlib_combat & TIMEOUT) {
								HGlib_combat &= ~TIMEOUT;
								You_feel("fliction in your %s!", makeplural(body_part(HAND)));
								hasmadeachange = 1;
							}
						} else {
							if(!(HGlib_combat & FROMOUTSIDE)) {
								You_feel(FunnyHallu ?
					    "like an absolute marital arts champion, so you can start fighting off your spouse!" :
								    "the fliction in your hands disappearing.");
								HGlib_combat |= FROMOUTSIDE;
								hasmadeachange = 1;
							}
						}
						break;
					}
				}
				break;
			case 16:
				if (intloss) {
					if (HStone_resistance & INTRINSIC) {
						HStone_resistance &= ~INTRINSIC;
						You_feel("less solid!");
						hasmadeachange = 1;
					}
					if (HStone_resistance & TIMEOUT) {
						HStone_resistance &= ~TIMEOUT;
						You_feel("less solid!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HStone_resistance & FROMOUTSIDE)) {
						You(FunnyHallu ? "feel stony and groovy, man." : "feel rock solid.");
						HStone_resistance |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			default: /* shouldn't happen */
				break;

		}
	} else switch (rnd(43)) { /* ones that require eating jewelry or other weird actions */

			case 1:
				if (intloss) {
					if (HStealth & INTRINSIC) {
						HStealth &= ~INTRINSIC;
						You_feel("clumsy.");
						hasmadeachange = 1;
					}
					if (HStealth & TIMEOUT) {
						HStealth &= ~TIMEOUT;
						You_feel("clumsy.");
						hasmadeachange = 1;
					}
				} else {
					if(!(HStealth & FROMOUTSIDE)) {
						HStealth |= FROMOUTSIDE;
						You_feel("stealthy.");
						hasmadeachange = 1;
					}
				}

				break;
			case 2:
				if (intloss) {
					if (HProtection & INTRINSIC) {
						HProtection &= ~INTRINSIC;
						You_feel("vulnerable.");
						hasmadeachange = 1;
					}
					if (HProtection & TIMEOUT) {
						HProtection &= ~TIMEOUT;
						You_feel("vulnerable.");
						hasmadeachange = 1;
					}
				} else {
					if(!(HProtection & FROMOUTSIDE)) {
						HProtection |= FROMOUTSIDE;
						You_feel("protected.");
						hasmadeachange = 1;
					}
				}
				break;
			case 3:
				if (intloss) {
					if (HDrain_resistance & INTRINSIC) {
						HDrain_resistance &= ~INTRINSIC;
						You_feel("like someone is sucking out your life-force.");
						hasmadeachange = 1;
					}
					if (HDrain_resistance & TIMEOUT) {
						HDrain_resistance &= ~TIMEOUT;
						You_feel("like someone is sucking out your life-force.");
						hasmadeachange = 1;
					}
				} else {
					if(!(HDrain_resistance & FROMOUTSIDE)) {
						You_feel("that your life force is safe now.");
						HDrain_resistance |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 4:
				if (intloss) {
					if (HSick_resistance & INTRINSIC) {
						HSick_resistance &= ~INTRINSIC;
						You_feel("no longer immune to diseases!");
						hasmadeachange = 1;
					}
					if (HSick_resistance & TIMEOUT) {
						HSick_resistance &= ~TIMEOUT;
						You_feel("no longer immune to diseases!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HSick_resistance & FROMOUTSIDE)) {
						You_feel("immune to diseases.");
						HSick_resistance |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 5:
				if (intloss) {
					if (HWarning & INTRINSIC) {
						HWarning &= ~INTRINSIC;
						You_feel("that your radar has just stopped working!");
						hasmadeachange = 1;
					}
					if (HWarning & TIMEOUT) {
						HWarning &= ~TIMEOUT;
						You_feel("that your radar has just stopped working!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HWarning & FROMOUTSIDE)) {
						You_feel("that your radar was turned on.");
						HWarning |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 6:
				if (intloss) {
					if (HSearching & INTRINSIC) {
						HSearching &= ~INTRINSIC;
						You_feel("unable to find something you lost!");
						hasmadeachange = 1;
					}
					if (HSearching & TIMEOUT) {
						HSearching &= ~TIMEOUT;
						You_feel("unable to find something you lost!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HSearching & FROMOUTSIDE)) {
						You_feel("capable of finding hidden secrets.");
						HSearching |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 7:
				if (intloss) {
					if (HClairvoyant & INTRINSIC) {
						HClairvoyant &= ~INTRINSIC;
						You_feel("a loss of mental capabilities!");
						hasmadeachange = 1;
					}
					if (HClairvoyant & TIMEOUT) {
						HClairvoyant &= ~TIMEOUT;
						You_feel("a loss of mental capabilities!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HClairvoyant & FROMOUTSIDE)) {
						You_feel("your consciousness expand!");
						HClairvoyant |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 8:
				if (intloss) {
					if (HInfravision & INTRINSIC) {
						HInfravision &= ~INTRINSIC;
						You_feel("shrouded in darkness.");
						hasmadeachange = 1;
					}
					if (HInfravision & TIMEOUT) {
						HInfravision &= ~TIMEOUT;
						You_feel("shrouded in darkness.");
						hasmadeachange = 1;
					}
				} else {
					if(!(HInfravision & FROMOUTSIDE)) {
						You_feel("capable of seeing in the dark.");
						HInfravision |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 9:
				if (!rn2(200)) { /* detect monsters is the grand daddy and thus very rare --Amy */

					if (intloss) {
						if (HDetect_monsters & INTRINSIC) {
							HDetect_monsters &= ~INTRINSIC;
							You_feel("that you can no longer sense monsters.");
							hasmadeachange = 1;
						}
						if (HDetect_monsters & TIMEOUT) {
							HDetect_monsters &= ~TIMEOUT;
							You_feel("that you can no longer sense monsters.");
							hasmadeachange = 1;
						}
					} else {
						if(!(HDetect_monsters & FROMOUTSIDE)) {
							HDetect_monsters |= FROMOUTSIDE;
							You("sense monsters.");
							hasmadeachange = 1;
						}
					}

				} else {

					if (intloss) {
						if (HJumping & INTRINSIC) {
							HJumping &= ~INTRINSIC;
							You_feel("your legs shrinking.");
							hasmadeachange = 1;
						}
						if (HJumping & TIMEOUT) {
							HJumping &= ~TIMEOUT;
							You_feel("your legs shrinking.");
							hasmadeachange = 1;
						}
					} else {
						if(!(HJumping & FROMOUTSIDE)) {
							HJumping |= FROMOUTSIDE;
							You_feel("a sudden ability to jump.");
							hasmadeachange = 1;
						}
					}
				}
				break;
			case 10:
				if (intloss) {
					if (HMagical_breathing & INTRINSIC) {
						HMagical_breathing &= ~INTRINSIC;
						You_feel("you suddenly need to breathe!");
						hasmadeachange = 1;
					}
					if (HMagical_breathing & TIMEOUT) {
						HMagical_breathing &= ~TIMEOUT;
						You_feel("you suddenly need to breathe!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HMagical_breathing & FROMOUTSIDE)) {
						You_feel("that you don't need to breathe any longer.");
						HMagical_breathing |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 11:
				if (intloss) {
					if (HRegeneration & INTRINSIC) {
						HRegeneration &= ~INTRINSIC;
						You_feel("your wounds are healing slower!");
						hasmadeachange = 1;
					}
					if (HRegeneration & TIMEOUT) {
						HRegeneration &= ~TIMEOUT;
						You_feel("your wounds are healing slower!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HRegeneration & FROMOUTSIDE)) {
						You_feel("your wounds healing more quickly.");
						HRegeneration |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 12:
				if (intloss) {
					if (HEnergy_regeneration & INTRINSIC) {
						HEnergy_regeneration &= ~INTRINSIC;
						You_feel("a loss of mystic power!");
						hasmadeachange = 1;
					}
					if (HEnergy_regeneration & TIMEOUT) {
						HEnergy_regeneration &= ~TIMEOUT;
						You_feel("a loss of mystic power!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HEnergy_regeneration & FROMOUTSIDE)) {
						HEnergy_regeneration |= FROMOUTSIDE;
						You_feel("a surge of mystic power.");
						hasmadeachange = 1;
					}
				}
				break;
			case 13:
				if (intloss) {
					if (HPolymorph & INTRINSIC) {
						HPolymorph &= ~INTRINSIC;
						You_feel("unable to change form!");
						hasmadeachange = 1;
					}
					if (HPolymorph & TIMEOUT) {
						HPolymorph &= ~TIMEOUT;
						You_feel("unable to change form!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HPolymorph & FROMOUTSIDE)) {
						HPolymorph |= FROMOUTSIDE;
						You_feel("unstable.");
						hasmadeachange = 1;
					}
				}
				break;
			case 14:
				if (intloss) {
					if (HPolymorph_control & INTRINSIC) {
						HPolymorph_control &= ~INTRINSIC;
						You_feel("less control over your own body.");
						hasmadeachange = 1;
					}
					if (HPolymorph_control & TIMEOUT) {
						HPolymorph_control &= ~TIMEOUT;
						You_feel("less control over your own body.");
						hasmadeachange = 1;
					}
				} else {
					if(!(HPolymorph_control & FROMOUTSIDE)) {
						HPolymorph_control |= FROMOUTSIDE;
						You_feel("more control over your own body.");
						hasmadeachange = 1;
					}
				}
				break;
			case 15:
				if (intloss) {
					if (HHunger & INTRINSIC) {
						HHunger &= ~INTRINSIC;
						You_feel("like you just ate a chunk of meat.");
						hasmadeachange = 1;
					}
					if (HHunger & TIMEOUT) {
						HHunger &= ~TIMEOUT;
						You_feel("like you just ate a chunk of meat.");
						hasmadeachange = 1;
					}
				} else {
					if(!(HHunger & FROMOUTSIDE)) {
						HHunger |= FROMOUTSIDE;
						You_feel("very hungry.");
						hasmadeachange = 1;
					}
				}
				break;
			case 16:
				if (intloss) {
					if (HConflict & INTRINSIC) {
						HConflict &= ~INTRINSIC;
						You_feel("more acceptable.");
						hasmadeachange = 1;
					}
					if (HConflict & TIMEOUT) {
						HConflict &= ~TIMEOUT;
						You_feel("more acceptable.");
						hasmadeachange = 1;
					}
				} else {
					if(!(HConflict & FROMOUTSIDE)) {
						HConflict |= FROMOUTSIDE;
						You_feel("like a rabble-rouser.");
						hasmadeachange = 1;
					}
				}
				break;
			case 17:
				if (intloss) {
					if (HSlow_digestion & INTRINSIC) {
						HSlow_digestion &= ~INTRINSIC;
						You_feel("like you're burning calories faster.");
						hasmadeachange = 1;
					}
					if (HSlow_digestion & TIMEOUT) {
						HSlow_digestion &= ~TIMEOUT;
						You_feel("like you're burning calories faster.");
						hasmadeachange = 1;
					}
				} else {
					if(!(HSlow_digestion & FROMOUTSIDE)) {
						You_feel("constipated.");
						HSlow_digestion |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 18:
				if (intloss) {
					if (HFlying & INTRINSIC) {
						HFlying &= ~INTRINSIC;
						You_feel("like you just lost your wings!");
						hasmadeachange = 1;
					}
					if (HFlying & TIMEOUT) {
						HFlying &= ~TIMEOUT;
						You_feel("like you just lost your wings!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HFlying & FROMOUTSIDE)) {
						You_feel("airborne.");
						HFlying |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 19:

				if (!rn2(200)) { /* passwall is teh uber pwnz0r and therefore rare */

					if (intloss) {
						if (HPasses_walls & INTRINSIC) {
							HPasses_walls &= ~INTRINSIC;
							You_feel("less ethereal!");
							hasmadeachange = 1;
						}
						if (HPasses_walls & TIMEOUT) {
							HPasses_walls &= ~TIMEOUT;
							You_feel("less ethereal!");
							hasmadeachange = 1;
						}
					} else {
						if(!(HPasses_walls & FROMOUTSIDE)) {
							You_feel("ethereal.");
							HPasses_walls |= FROMOUTSIDE;
							hasmadeachange = 1;
						}
					}
				} else {

					if (intloss) {
						if (HAntimagic & INTRINSIC) {
							HAntimagic &= ~INTRINSIC;
							You_feel("less protected from magic!");
							hasmadeachange = 1;
						}
						if (HAntimagic & TIMEOUT) {
							HAntimagic &= ~TIMEOUT;
							You_feel("less protected from magic!");
							hasmadeachange = 1;
						}
					} else {
						if(!(HAntimagic & FROMOUTSIDE)) {
							You_feel("magic-protected.");
							HAntimagic |= FROMOUTSIDE;
							hasmadeachange = 1;
						}
					}

				}

				break;
			case 20:
				if (intloss) {
					if (HReflecting & INTRINSIC) {
						HReflecting &= ~INTRINSIC;
						You_feel("less reflexive!");
						hasmadeachange = 1;
					}
					if (HReflecting & TIMEOUT) {
						HReflecting &= ~TIMEOUT;
						You_feel("less reflexive!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HReflecting & FROMOUTSIDE)) {
						You_feel("reflexive.");
						HReflecting |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 21:
				if (intloss) {
					if (HSwimming & INTRINSIC) {
						HSwimming &= ~INTRINSIC;
						You_feel("less aquatic!");
						hasmadeachange = 1;
					}
					if (HSwimming & TIMEOUT) {
						HSwimming &= ~TIMEOUT;
						You_feel("less aquatic!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HSwimming & FROMOUTSIDE)) {
						You_feel("aquatic.");
						HSwimming |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 22:
				if (intloss) {
					if (HFree_action & INTRINSIC) {
						HFree_action &= ~INTRINSIC;
						You_feel("a loss of freedom!");
						hasmadeachange = 1;
					}
					if (HFree_action & TIMEOUT) {
						HFree_action &= ~TIMEOUT;
						You_feel("a loss of freedom!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HFree_action & FROMOUTSIDE)) {
						You_feel("free.");
						HFree_action |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 23:
				if (intloss) {
					if (HFear_resistance & INTRINSIC) {
						HFear_resistance &= ~INTRINSIC;
						You_feel("a little anxious!");
						hasmadeachange = 1;
					}
					if (HFear_resistance & TIMEOUT) {
						HFear_resistance &= ~TIMEOUT;
						You_feel("a little anxious!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HFear_resistance & FROMOUTSIDE)) {
						You_feel("unafraid.");
						HFear_resistance |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 24:
				if (intloss) {
					if (HKeen_memory & INTRINSIC) {
						HKeen_memory &= ~INTRINSIC;
						You_feel("a case of selective amnesia...");
						hasmadeachange = 1;
					}
					if (HKeen_memory & TIMEOUT) {
						HKeen_memory &= ~TIMEOUT;
						You_feel("a case of selective amnesia...");
						hasmadeachange = 1;
					}
				} else {
					if(!(HKeen_memory & FROMOUTSIDE)) {
						You_feel("capable of remembering everything.");
						HKeen_memory |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 25:
				if (intloss) {
					if (HVersus_curses & INTRINSIC) {
						HVersus_curses &= ~INTRINSIC;
						You_feel("cursed!");
						hasmadeachange = 1;
					}
					if (HVersus_curses & TIMEOUT) {
						HVersus_curses &= ~TIMEOUT;
						You_feel("cursed!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HVersus_curses & FROMOUTSIDE)) {
						You_feel("protected from curse words.");
						HVersus_curses |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 26:
				if (intloss) {
					if (HStun_resist & INTRINSIC) {
						HStun_resist &= ~INTRINSIC;
						You_feel("a little stunned!");
						hasmadeachange = 1;
					}
					if (HStun_resist & TIMEOUT) {
						HStun_resist &= ~TIMEOUT;
						You_feel("a little stunned!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HStun_resist & FROMOUTSIDE)) {
						You_feel("the ability to control stunning.");
						HStun_resist |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 27:
				if (intloss) {
					if (HConf_resist & INTRINSIC) {
						HConf_resist &= ~INTRINSIC;
						You_feel("a little confused!");
						hasmadeachange = 1;
					}
					if (HConf_resist & TIMEOUT) {
						HConf_resist &= ~TIMEOUT;
						You_feel("a little confused!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HConf_resist & FROMOUTSIDE)) {
						You_feel("capable of concentrating even while confused.");
						HConf_resist |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 28:
				if (intloss) {
					if (HExtra_wpn_practice & INTRINSIC) {
						HExtra_wpn_practice &= ~INTRINSIC;
						You_feel("less able to learn new stuff!");
						hasmadeachange = 1;
					}
					if (HExtra_wpn_practice & TIMEOUT) {
						HExtra_wpn_practice &= ~TIMEOUT;
						You_feel("less able to learn new stuff!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HExtra_wpn_practice & FROMOUTSIDE)) {
						You_feel("like a quick learner.");
						HExtra_wpn_practice |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 29:
				if (intloss) {
					if (HDisplaced & INTRINSIC) {
						HDisplaced &= ~INTRINSIC;
						You_feel("a little exposed!");
						hasmadeachange = 1;
					}
					if (HDisplaced & TIMEOUT) {
						HDisplaced &= ~TIMEOUT;
						You_feel("a little exposed!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HDisplaced & FROMOUTSIDE)) {
						You_feel("less exposed.");
						HDisplaced |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 30:
				if (intloss) {
					if (HPsi_resist & INTRINSIC) {
						HPsi_resist &= ~INTRINSIC;
						You_feel("empty-minded!");
						hasmadeachange = 1;
					}
					if (HPsi_resist & TIMEOUT) {
						HPsi_resist &= ~TIMEOUT;
						You_feel("empty-minded!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HPsi_resist & FROMOUTSIDE)) {
						You_feel("mentally strong.");
						HPsi_resist |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 31:
				if (intloss) {
					if (HSight_bonus & INTRINSIC) {
						HSight_bonus &= ~INTRINSIC;
						You_feel("less perceptive!");
						hasmadeachange = 1;
					}
					if (HSight_bonus & TIMEOUT) {
						HSight_bonus &= ~TIMEOUT;
						You_feel("less perceptive!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HSight_bonus & FROMOUTSIDE)) {
						You_feel("the presence of a globe of light.");
						HSight_bonus |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 32:
				if (intloss) {
					if (HManaleech & INTRINSIC) {
						HManaleech &= ~INTRINSIC;
						You_feel("less magically attuned!");
						hasmadeachange = 1;
					}
					if (HManaleech & TIMEOUT) {
						HManaleech &= ~TIMEOUT;
						You_feel("less magically attuned!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HManaleech & FROMOUTSIDE)) {
						You_feel("magically attuned.");
						HManaleech |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 33:
				if (intloss) {
					if (HMap_amnesia & INTRINSIC) {
						HMap_amnesia &= ~INTRINSIC;
						You_feel("less forgetful!");
						hasmadeachange = 1;
					}
					if (HMap_amnesia & TIMEOUT) {
						HMap_amnesia &= ~TIMEOUT;
						You_feel("less forgetful!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HMap_amnesia & FROMOUTSIDE)) {
						You_feel("very forgetful!");
						HMap_amnesia |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 34:
				if (intloss) {
					if (HPeacevision & INTRINSIC) {
						HPeacevision &= ~INTRINSIC;
						You_feel("less peaceful!");
						hasmadeachange = 1;
					}
					if (HPeacevision & TIMEOUT) {
						HPeacevision &= ~TIMEOUT;
						You_feel("less peaceful!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HPeacevision & FROMOUTSIDE)) {
						You_feel("a sense of peace.");
						HPeacevision |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 35:
				if (intloss) {
					if (HCont_resist & INTRINSIC) {
						HCont_resist &= ~INTRINSIC;
						You_feel("less resistant to contamination!");
						hasmadeachange = 1;
					}
					if (HCont_resist & TIMEOUT) {
						HCont_resist &= ~TIMEOUT;
						You_feel("less resistant to contamination!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HCont_resist & FROMOUTSIDE)) {
						You_feel("protected from contamination.");
						HCont_resist |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 36:
				if (intloss) {
					if (HDiscount_action & INTRINSIC) {
						HDiscount_action &= ~INTRINSIC;
						You_feel("less resistant to paralysis!");
						hasmadeachange = 1;
					}
					if (HDiscount_action & TIMEOUT) {
						HDiscount_action &= ~TIMEOUT;
						You_feel("less resistant to paralysis!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HDiscount_action & FROMOUTSIDE)) {
						You_feel("more resistant to paralysis!");
						HDiscount_action |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 37:
				if (intloss) {
					if (HFull_nutrient & INTRINSIC) {
						HFull_nutrient &= ~INTRINSIC;
						You_feel("a hole in your %s!", body_part(STOMACH));
						hasmadeachange = 1;
					}
					if (HFull_nutrient & TIMEOUT) {
						HFull_nutrient &= ~TIMEOUT;
						You_feel("a hole in your %s!", body_part(STOMACH));
						hasmadeachange = 1;
					}
				} else {
					if(!(HFull_nutrient & FROMOUTSIDE)) {
						You_feel("that your %s is now rather full.", body_part(STOMACH));
						HFull_nutrient |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 38:
				if (intloss) {
					if (HTechnicality & INTRINSIC) {
						HTechnicality &= ~INTRINSIC;
						You_feel("less capable of using your techniques...");
						hasmadeachange = 1;
					}
					if (HTechnicality & TIMEOUT) {
						HTechnicality &= ~TIMEOUT;
						You_feel("less capable of using your techniques...");
						hasmadeachange = 1;
					}
				} else {
					if(!(HTechnicality & FROMOUTSIDE)) {
						You_feel("that your techniques are more powerful now!");
						HTechnicality |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 39:
				if (intloss) {
					if (HHalf_spell_damage & INTRINSIC) {
						HHalf_spell_damage &= ~INTRINSIC;
						You_feel("vulnerable to spells!");
						hasmadeachange = 1;
					}
					if (HHalf_spell_damage & TIMEOUT) {
						HHalf_spell_damage &= ~TIMEOUT;
						You_feel("vulnerable to spells!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HHalf_spell_damage & FROMOUTSIDE)) {
						You_feel("protected from spells.");
						HHalf_spell_damage |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 40:
				if (intloss) {
					if (HHalf_physical_damage & INTRINSIC) {
						HHalf_physical_damage &= ~INTRINSIC;
						You_feel("vulnerable to damage!");
						hasmadeachange = 1;
					}
					if (HHalf_physical_damage & TIMEOUT) {
						HHalf_physical_damage &= ~TIMEOUT;
						You_feel("vulnerable to damage!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HHalf_physical_damage & FROMOUTSIDE)) {
						You_feel("protected from damage.");
						HHalf_physical_damage |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 41:
				if (intloss) {
					if (HUseTheForce & INTRINSIC) {
						HUseTheForce &= ~INTRINSIC;
						You_feel("that you lost your jedi powers!");
						hasmadeachange = 1;
					}
					if (HUseTheForce & TIMEOUT) {
						HUseTheForce &= ~TIMEOUT;
						You_feel("that you lost your jedi powers!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HUseTheForce & FROMOUTSIDE)) {
						You_feel("able to use the force like a true jedi!");
						HUseTheForce |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 42:
				if (intloss) {
					if (HScentView & INTRINSIC) {
						HScentView &= ~INTRINSIC;
						You_feel("unable to smell things!");
						hasmadeachange = 1;
					}
					if (HScentView & TIMEOUT) {
						HScentView &= ~TIMEOUT;
						You_feel("unable to smell things!");
						hasmadeachange = 1;
					}
				} else {
					if(!(HScentView & FROMOUTSIDE)) {
						You_feel("a tingling in your %s!", body_part(NOSE));
						HScentView |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			case 43:
				if (intloss) {
					if (HDiminishedBleeding & INTRINSIC) {
						HDiminishedBleeding &= ~INTRINSIC;
						You_feel("your %s coagulants failing!", body_part(BLOOD));
						hasmadeachange = 1;
					}
					if (HDiminishedBleeding & TIMEOUT) {
						HDiminishedBleeding &= ~TIMEOUT;
						You_feel("your %s coagulants failing!", body_part(BLOOD));
						hasmadeachange = 1;
					}
				} else {
					if(!(HDiminishedBleeding & FROMOUTSIDE)) {
						You_feel("a %s coagulation factor being injected into your body!", body_part(BLOOD));
						HDiminishedBleeding |= FROMOUTSIDE;
						hasmadeachange = 1;
					}
				}
				break;
			default: /* shouldn't happen */
				break;

	}

	if (!hasmadeachange) {
		tryct++;
		if (tryct < 100 && rn2(intloss ? 10 : 2)) goto retrytrinsic;
	}

}

#endif /* OVLB */

/*artifact.c*/
