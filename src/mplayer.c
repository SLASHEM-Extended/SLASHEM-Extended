/*	SCCS Id: @(#)mplayer.c	3.4	1997/02/04	*/
/*	Copyright (c) Izchak Miller, 1992.			  */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

STATIC_DCL const char *dev_name(void);
STATIC_DCL void get_mplname(struct monst *, char *);

/* These are the names of those who
 * contributed to the development of NetHack 3.2/3.3/3.4.
 *
 * Keep in alphabetical order within teams.
 * Same first name is entered once within each team.
 */
static const char *developers[] = {
	/* devteam */
	"Dave", "Dean", "Eric", "Izchak", "Janet", "Jessie",
	"Ken", "Kevin", "Michael", "Mike", "Pat", "Paul", "Steve", "Timo",
	"Warwick",
	/* PC team */
	"Bill", "Eric", "Keizo", "Ken", "Kevin", "Michael", "Mike", "Paul",
	"Stephen", "Steve", "Timo", "Yitzhak",
	/* Amiga team */
	"Andy", "Gregg", "Janne", "Keni", "Mike", "Olaf", "Richard",
	/* Mac team */
	"Andy", "Chris", "Dean", "Jon", "Jonathan", "Kevin", "Wang",
	/* Atari team */
	"Eric", "Marvin", "Warwick",
	/* NT team */
	"Alex", "Dion", "Michael",
	/* OS/2 team */
	"Helge", "Ron", "Timo",
	/* VMS team */
	"Joshua", "Pat",
	""};


/* return a randomly chosen developer name */
STATIC_OVL const char *
dev_name()
{
	register int i, m = 0, n = SIZE(developers);
	register struct monst *mtmp;
	register boolean match;

	do {
	    match = FALSE;
	    i = rn2(n);
	    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		if(!is_mplayer(mtmp->data)) continue;
		if(!strncmp(developers[i], NAME(mtmp),
			               strlen(developers[i]))) {
		    match = TRUE;
		    break;
	        }
	    }
	    m++;
	} while (match && m < 100); /* m for insurance */

	if (match) return (const char *)0;
	return(developers[i]);
}

STATIC_OVL void
get_mplname(mtmp, nam)
register struct monst *mtmp;
char *nam;
{
	boolean fmlkind = mtmp->female; /* wtf why did that use "is_female"??? that's retarded! --Amy */
	const char *devnam;

	/* Amy edit: more names, mainly ones of active players and variant developers */
	if (!rn2(2)) {

		if (fmlkind) { /* awwwwww there's way too few female players :( */

			switch (rnd(17)) {

				case 1: strcpy(nam, "GoldenIvy"); break;
				case 2: strcpy(nam, "AmyBSOD"); break;
				case 3: strcpy(nam, "Dolores"); break;
				case 4: strcpy(nam, "Mariari"); break;
				case 5: strcpy(nam, "Lynn"); break;
				case 6: strcpy(nam, "Emily"); break;
				case 7: strcpy(nam, "Elenmirie"); break;
				case 8: strcpy(nam, "Raisse"); break;
				case 9: strcpy(nam, "MiseryMyra"); break;
				case 10: strcpy(nam, "Lymia"); break;
				case 11: strcpy(nam, "Naganadel"); break;
				case 12: strcpy(nam, "Icerose"); break;
				case 13: strcpy(nam, "AntiGulp"); break;
				case 14: strcpy(nam, "Malena"); break;
				case 15: strcpy(nam, "Aoei"); break;
				case 16: strcpy(nam, "CntFai"); break;
				case 17: strcpy(nam, "Cebolla"); break;
				default: strcpy(nam, "BugFemName"); break;
			}

		} else {

			switch (rnd(87)) {

				case 1: strcpy(nam, "Bhaak"); break;
				case 2: strcpy(nam, "Jonadab"); break;
				case 3: strcpy(nam, "Ais523"); break;
				case 4: strcpy(nam, "Chris_ANG"); break;
				case 5: strcpy(nam, "Tungtn"); break;
				case 6: strcpy(nam, "SGrunt"); break;
				case 7: strcpy(nam, "Lorimer"); break;
				case 8: strcpy(nam, "Adeon"); break;
				case 9: strcpy(nam, "FIQ"); break;
				case 10: strcpy(nam, "Mandevil"); break;
				case 11: strcpy(nam, "Luxidream"); break;
				case 12: strcpy(nam, "Noty"); break;
				case 13: strcpy(nam, "Psymar"); break;
				case 14: strcpy(nam, "ProzacElf"); break;
				case 15: strcpy(nam, "Tangles"); break;
				case 16: strcpy(nam, "Yasdorian"); break;
				case 17: strcpy(nam, "Bugsniper"); break;
				case 18: strcpy(nam, "Heliokopis"); break;
				case 19: strcpy(nam, "ShivanHunter"); break;
				case 20: strcpy(nam, "Khor"); break;
				case 21: strcpy(nam, "Kerio"); break;
				case 22: strcpy(nam, "Stenno"); break;
				case 23: strcpy(nam, "Deepy"); break;
				case 24: strcpy(nam, "Damerell"); break;
				case 25: strcpy(nam, "Elronnd"); break;
				case 26: strcpy(nam, "Blindcoder"); break;
				case 27: strcpy(nam, "Ilbelkyr"); break;
				case 28: strcpy(nam, "GreyKnight"); break;
				case 29: strcpy(nam, "Paxed"); break;
				case 30: strcpy(nam, "Crawldragon"); break;
				case 31: strcpy(nam, "Glycan"); break;
				case 32: strcpy(nam, "QDesjardin"); break;
				case 33: strcpy(nam, "Demo"); break;
				case 34: strcpy(nam, "GagarinX"); break;
				case 35: strcpy(nam, "Introspective"); break;
				case 36: strcpy(nam, "Grasshopper"); break;
				case 37: strcpy(nam, "Tarmunora"); break;
				case 38: strcpy(nam, "SourSlime"); break;
				case 39: strcpy(nam, "Mr0t"); break;
				case 40: strcpy(nam, "Irrenhaus"); break;
				case 41: strcpy(nam, "Greqrg"); break;
				case 42: strcpy(nam, "Bouquet"); break;
				case 43: strcpy(nam, "Lorskel"); break;
				case 44: strcpy(nam, "Regret"); break;
				case 45: strcpy(nam, "Tariru"); break;
				case 46: strcpy(nam, "Goreval"); break;
				case 47: strcpy(nam, "Winter"); break;
				case 48: strcpy(nam, "StatueSurfer"); break;
				case 49: strcpy(nam, "Yer Mivvagah"); break;
				case 50: strcpy(nam, "Arrhythmia"); break;
				case 51: strcpy(nam, "Tubs"); break;
				case 52: strcpy(nam, "Winsalot"); break;
				case 53: strcpy(nam, "Hothraxxa"); break;
				case 54: strcpy(nam, "Aosdict"); break;
				case 55: strcpy(nam, "Ziratha"); break;
				case 56: strcpy(nam, "Volt"); break;
				case 57: strcpy(nam, "Kritixilithos"); break;
				case 58: strcpy(nam, "Rikersan"); break;
				case 59: strcpy(nam, "K2"); break;
				case 60: strcpy(nam, "Leeroy"); break;
				case 61: strcpy(nam, "Hypnotist"); break;
				case 62: strcpy(nam, "Anerag"); break;
				case 63: strcpy(nam, "Madotsuki"); break;
				case 64: strcpy(nam, "PeterQ"); break;
				case 65: strcpy(nam, "PavelB"); break;
				case 66: strcpy(nam, "FlamingGuacamole"); break;
				case 67: strcpy(nam, "Dracopent"); break;
				case 68: strcpy(nam, "Andrio"); break;
				case 69: strcpy(nam, "Metanite"); break;
				case 70: strcpy(nam, "Greyberyl"); break;
				case 71: strcpy(nam, "Pellsson"); break;
				case 72: strcpy(nam, "Recluse"); break;
				case 73: strcpy(nam, "Pinkbeast"); break;
				case 74: strcpy(nam, "Mickmane"); break;
				case 75: strcpy(nam, "Porkman"); break;
				case 76: strcpy(nam, "Micromoog"); break;
				case 77: strcpy(nam, "Malor"); break;
				case 78: strcpy(nam, "Merlek"); break;
				case 79: strcpy(nam, "Musicdemon"); break;
				case 80: strcpy(nam, "Amateurhour"); break;
				case 81: strcpy(nam, "Mobileuser"); break;
				case 82: strcpy(nam, "Rebatela"); break;
				case 83: strcpy(nam, "Microlance"); break;
				case 84: strcpy(nam, "NetSysFire"); break;
				case 85: strcpy(nam, "Umbire"); break;
				case 86: strcpy(nam, "Roho"); break;
				case 87: strcpy(nam, "Pineapple Tycoon"); break;
				default: strcpy(nam, "BugMalName"); break;

			}

		}

		goto namedecided;
	}

	devnam = dev_name();
	if (!devnam)
	    strcpy(nam, fmlkind ? "Eve" : "Adam");
	else if (fmlkind && !!strcmp(devnam, "Janet"))
	    strcpy(nam, rn2(2) ? "Maud" : "Eve");
	else strcpy(nam, devnam);

namedecided:
	/* why don't we just let the RNG decide whether the player monster is female... --Amy */
/*	if (fmlkind || !strcmp(nam, "Janet"))
	    mtmp->female = 1;
	else
	    mtmp->female = 0;*/
	if (is_mplayer(mtmp->data)) {
		strcat(nam, " the ");
		strcat(nam, rank_of((int)mtmp->m_lev, monsndx(mtmp->data), (boolean)mtmp->female));
	}
}

void
mk_mplayer_armor(mon, typ)
struct monst *mon;
short typ;
{
	struct obj *obj;

	if (typ == STRANGE_OBJECT) return;
	obj = mksobj(typ, FALSE, FALSE, FALSE);
	if (obj) {
		if (!rn2(3)) obj->oerodeproof = 1;
		if (!rn2(3)) curse(obj);
		if (!rn2(3)) bless(obj);
		/* Most players who get to the endgame who have cursed equipment
		 * have it because the wizard or other monsters cursed it, so its
		 * chances of having plusses is the same as usual....
		 */
		obj->spe = rn2(10) ? (rn2(3) ? rn2(5) : rn1(4,4)) : -rnd(3);
		if (obj->spe > 0 && !rn2(5)) obj->spe += rne(2);
		else if (obj->spe < 0 && !rn2(3)) obj->spe -= rne(2);

		/* Come on, this isn't vanilla. Artifact armors are a thing. --Amy */
		if (rn2(2)) obj = mk_artifact(obj, A_NONE, TRUE);

		(void) mpickobj(mon, obj, TRUE);
	}
}

struct monst *
mk_mplayer(ptr, x, y, special)
register struct permonst *ptr;
xchar x, y;
register boolean special;
{
	register struct monst *mtmp;
	char nam[PL_PSIZ];

	if(!is_mplayer(ptr) && !is_umplayer(ptr))
		return((struct monst *)0);

	if(MON_AT(x, y))
		(void) rloc(m_at(x, y), FALSE); /* insurance */

	/* seriously why is this here? we already set "special" in the calling function... --Amy */
	/* if(!In_endgame(&u.uz)) special = FALSE; */

	if ((mtmp = makemon(ptr, x, y, NO_MM_FLAGS)) != 0) {
	    short weapon = rn2(2) ? LONG_SWORD : rnd_class(SPEAR, BULLWHIP);

	/* Not every ascension kit includes dragon scale mail... I've ascended a monk with crystal plate mail :P --Amy */
	    short armor = rn2(5) ? rnd_class(GRAY_DRAGON_SCALE_MAIL, YELLOW_DRAGON_SCALE_MAIL) : rnd_class(PLATE_MAIL, YELLOW_DRAGON_SCALES);
	    short cloak = !rn2(8) ? STRANGE_OBJECT :
	    		rnd_class(OILSKIN_CLOAK, CLOAK_OF_DISPLACEMENT);
	    short helm = !rn2(8) ? STRANGE_OBJECT :
	    		rnd_class(ELVEN_LEATHER_HELM, HELM_OF_TELEPATHY);
	    short shield = !rn2(8) ? STRANGE_OBJECT :
	    		rnd_class(ELVEN_SHIELD, SHIELD_OF_REFLECTION);
	    int quan;
	    struct obj *otmp;

	    mtmp->m_lev = (special ? rn1(16,15) : rnd(16));
	    mtmp->mhp = mtmp->mhpmax = d((int)mtmp->m_lev,10) +
					(special ? (30 + rnd(30)) : 30);
	    if(special) {
	        get_mplname(mtmp, nam);
	        mtmp = christen_monst(mtmp, nam);
		/* that's why they are "stuck" in the endgame :-) */
		(void)mongets(mtmp, FAKE_AMULET_OF_YENDOR);
	    }
	    mtmp->mpeaceful = 0;
	    set_malign(mtmp); /* peaceful may have changed again */

	    switch(monsndx(ptr)) {
		case PM_ARCHEOLOGIST:
		case PM_UNDEAD_ARCHEOLOGIST:
		    if (rn2(2)) weapon = BULLWHIP;
		    break;
		case PM_ANACHRONIST:
		case PM_UNDEAD_ANACHRONIST:
		    weapon = FORCE_PIKE;
		    break;
		case PM_BARBARIAN:
		case PM_UNDEAD_BARBARIAN:
		case PM_NOOB_MODE_BARB:
		case PM_UNDEAD_NOOB_MODE_BARB:
		    if (rn2(2)) {
		    	weapon = rn2(2) ? TWO_HANDED_SWORD : BATTLE_AXE;
		    	shield = STRANGE_OBJECT;
		    }
		    if (rn2(2)) armor = rnd_class(PLATE_MAIL, CHAIN_MAIL);
		    if (helm == HELM_OF_BRILLIANCE) helm = STRANGE_OBJECT;
		    break;
		case PM_BLEEDER:
		case PM_UNDEAD_BLEEDER:
			weapon = KNIFE;
		    break;
		case PM_ANACHRONOUNBINDER:
		case PM_UNDEAD_ANACHRONOUNBINDER:
			weapon = SILVER_SHORT_SWORD;
		    break;
		case PM_HUSSY:
		case PM_UNDEAD_HUSSY:
			weapon = HIGH_HEELED_SANDAL;
		    break;
		case PM_MASON:
		case PM_UNDEAD_MASON:
			weapon = DWARVISH_MATTOCK;
			armor = FULL_PLATE_MAIL;
		    break;
		case PM_DEMAGOGUE:
		case PM_UNDEAD_DEMAGOGUE:
			armor = FULL_PLATE_MAIL;
			weapon = THUNDER_HAMMER;
		    break;
		case PM_CELLAR_CHILD:
		case PM_UNDEAD_CELLAR_CHILD:
			weapon = BATTLE_STAFF;
			armor = MAGE_PLATE_MAIL;
		    break;
		case PM_GRENADONIN:
		case PM_UNDEAD_GRENADONIN:
			weapon = KATANA;
		    break;
		case PM_SOFTWARE_ENGINEER:
		case PM_UNDEAD_SOFTWARE_ENGINEER:
			weapon = AXE;
		    break;
		case PM_CRACKER:
		case PM_UNDEAD_CRACKER:
			weapon = BATTLE_AXE;
		    break;
		case PM_JANITOR:
		case PM_UNDEAD_JANITOR:
			weapon = SPECIAL_MOP;
		    break;
		case PM_SPACE_MARINE:
		case PM_UNDEAD_SPACE_MARINE:
			weapon = WAR_HAMMER;
		    break;
		case PM_STORMBOY:
		case PM_UNDEAD_STORMBOY:
			weapon = TSURUGI;
		    break;
		case PM_YAUTJA:
		case PM_UNDEAD_YAUTJA:
			weapon = !rn2(3) ? DRAGON_SPEAR : rn2(2) ? BULLETPROOF_CHAINWHIP : HUGE_CLUB;
		    break;
		case PM_QUARTERBACK:
		case PM_UNDEAD_QUARTERBACK:
			weapon = METAL_CLUB;
		    break;
		case PM_PSYKER:
		case PM_UNDEAD_PSYKER:
		    break;
		case PM_EMPATH:
		case PM_UNDEAD_EMPATH:
			weapon = CRYSKNIFE;
		    break;
		case PM_MASTERMIND:
		case PM_UNDEAD_MASTERMIND:
		    break;
		case PM_WEIRDBOY:
		case PM_UNDEAD_WEIRDBOY:
			weapon = BRONZE_MORNING_STAR;
		    break;
		case PM_ASTRONAUT:
		case PM_UNDEAD_ASTRONAUT:
		    break;
		case PM_CYBERNINJA:
		case PM_UNDEAD_CYBERNINJA:
			weapon = KATANA;
		    break;
		case PM_DISSIDENT:
		case PM_UNDEAD_DISSIDENT:
			weapon = SPIKED_CLUB;
		    break;
		case PM_XELNAGA:
		case PM_UNDEAD_XELNAGA:
		    break;
		case PM_SINGSLAVE:
		case PM_UNDEAD_SINGSLAVE:
		    weapon = STRANGE_OBJECT;
		    break;
		case PM_WALSCHOLAR:
		case PM_UNDEAD_WALSCHOLAR:
			weapon = CONGLOMERATE_PICK;
			armor = EVIL_LEATHER_ARMOR;
		    break;
		case PM_SOCIAL_JUSTICE_WARRIOR:
		case PM_UNDEAD_SOCIAL_JUSTICE_WARRIOR:
			helm = FEDORA;
			weapon = INSECT_SQUASHER;
		    break;
		case PM_BINDER:
		case PM_UNDEAD_BINDER:
			weapon = RANSEUR;
			armor = ELVEN_MITHRIL_COAT;
			cloak = LEATHER_CLOAK;
		    break;
		case PM_CAVEMAN:
		case PM_CAVEWOMAN:
		case PM_UNDEAD_CAVEMAN:
		case PM_UNDEAD_CAVEWOMAN:
		    if (rn2(4)) weapon = MACE;
		    else if (rn2(2)) weapon = CLUB;
		    if (helm == HELM_OF_BRILLIANCE) helm = STRANGE_OBJECT;
		    break;
		case PM_CONVICT:
		case PM_UNDEAD_CONVICT:
		    weapon = HEAVY_IRON_BALL;
		    break;
		case PM_MURDERER:
		case PM_UNDEAD_MURDERER:
		    weapon = HEAVY_IRON_BALL;
		    break;
		case PM_TRACER:
		case PM_UNDEAD_TRACER:
		    weapon = SHORT_SWORD;
		    break;
		case PM_FEMINIST:
		case PM_UNDEAD_FEMINIST:
		    weapon = HIGH_HEELED_SANDAL;
		    break;
		case PM_FORM_CHANGER:
		case PM_UNDEAD_FORM_CHANGER:
		    weapon = CLUB;
		    break;
		case PM_NUCLEAR_PHYSICIST:
		case PM_UNDEAD_NUCLEAR_PHYSICIST:
		    weapon = INKA_SPEAR;
		    break;
		case PM_GANG_SCHOLAR:
		case PM_UNDEAD_GANG_SCHOLAR:
		    weapon = TWO_HANDED_SWORD;
		    break;
		case PM_GEEK:
		case PM_UNDEAD_GEEK:
		    weapon = ELECTRIC_SWORD;
		    break;
		case PM_GRADUATE:
		case PM_UNDEAD_GRADUATE:
		    weapon = FLY_SWATTER;
		    break;
		case PM_GANGSTER:
		case PM_UNDEAD_GANGSTER:
		    weapon = SCOURGE;
		    break;
		case PM_PIRATE:
		case PM_UNDEAD_PIRATE:
		    weapon = SCIMITAR;
		    break;
		case PM_DRUNK:
		case PM_UNDEAD_DRUNK:
		    weapon = RUBBER_HOSE;
		    break;
		case PM_OFFICER:
		case PM_UNDEAD_OFFICER:
		    weapon = CLUB;
		    break;
		case PM_KORSAIR:
		case PM_UNDEAD_KORSAIR:
		    weapon = SCIMITAR;
		    break;
		case PM_DIVER:
		case PM_UNDEAD_DIVER:
		    weapon = TRIDENT;
		    break;
		case PM_BARD:
		case PM_UNDEAD_BARD:
		    weapon = WAR_HAMMER;
		    break;
		case PM_GLADIATOR:
		case PM_UNDEAD_GLADIATOR:
		    weapon = SILVER_SABER;
		    break;
		case PM_GOFF:
		case PM_UNDEAD_GOFF:
		    weapon = QUARTERSTAFF;
		    break;
		case PM_AMAZON:
		case PM_UNDEAD_AMAZON:
		    weapon = JAVELIN;
		    break;
		case PM_ALTMER:
		case PM_UNDEAD_ALTMER:
		    weapon = QUARTERSTAFF;
		    break;
		case PM_BOSMER:
		case PM_UNDEAD_BOSMER:
		    weapon = ELVEN_DAGGER;
		    break;
		case PM_INTEL_SCRIBE:
		case PM_UNDEAD_INTEL_SCRIBE:
		    weapon = DAGGER;
		    break;
		case PM_DUNMER:
		case PM_UNDEAD_DUNMER:
		    weapon = DARK_ELVEN_DAGGER;
		    break;
		case PM_THALMOR:
		case PM_UNDEAD_THALMOR:
		    weapon = ELVEN_BROADSWORD;
		    break;
		case PM_ORDINATOR:
		case PM_UNDEAD_ORDINATOR:
		    weapon = TSURUGI;
		    break;
		case PM_ARTIST:
		case PM_UNDEAD_ARTIST:
		    weapon = MALLET;
		    break;
		case PM_GAMER:
		case PM_UNDEAD_GAMER:
		    weapon = CLUB;
		    break;
		case PM_LIBRARIAN:
		case PM_UNDEAD_LIBRARIAN:
		    weapon = KNIFE;
		    break;
		case PM_COOK:
		case PM_UNDEAD_COOK:
		    weapon = KNIFE;
		    break;
		case PM_JESTER:
		case PM_UNDEAD_JESTER:
		    weapon = HEAVY_IRON_BALL;
		    break;
		case PM_CRUEL_ABUSER:
		case PM_UNDEAD_CRUEL_ABUSER:
		    weapon = KNOUT;
		    break;
		case PM_SUPERMARKET_CASHIER:
		case PM_UNDEAD_SUPERMARKET_CASHIER:
		    weapon = KNIFE;
		    break;
		case PM_LADIESMAN:
		case PM_UNDEAD_LADIESMAN:
		    weapon = KNIFE;
		    break;
		case PM_FIGHTER:
		case PM_UNDEAD_FIGHTER:
		    weapon = LONG_SWORD;
		    break;
		case PM_FENCER:
		case PM_UNDEAD_FENCER:
		    weapon = BLACK_AESTIVALIS;
		    break;
		case PM_MEDIUM:
		case PM_UNDEAD_MEDIUM:
		    weapon = QUARTERSTAFF;
		    break;
		case PM_STAND_USER:
		case PM_UNDEAD_STAND_USER:
		    weapon = LOG;
		    break;
		case PM_JUSTICE_KEEPER:
		case PM_UNDEAD_JUSTICE_KEEPER:
		    weapon = SLEDGE_HAMMER;
		    break;
		case PM_SEXYMATE:
		case PM_UNDEAD_SEXYMATE:
		    weapon = STURDY_PLATEAU_BOOT_FOR_GIRLS;
		    break;
		case PM_GUNNER:
		case PM_UNDEAD_GUNNER:
		    weapon = KNIFE;
		    break;
		case PM_DOOM_MARINE:
		case PM_UNDEAD_DOOM_MARINE:
		    weapon = CHAINSWORD;
		    break;
		case PM_WANDKEEPER:
		case PM_UNDEAD_WANDKEEPER:
		    weapon = KNIFE;
		    break;
		case PM_FOXHOUND_AGENT:
		case PM_UNDEAD_FOXHOUND_AGENT:
		    weapon = KNIFE;
		    break;
		case PM_FEAT_MASTER:
		case PM_UNDEAD_FEAT_MASTER:
		    weapon = FLAIL;
		    break;
		case PM_MAHOU_SHOUJO:
		case PM_UNDEAD_MAHOU_SHOUJO:
		    weapon = WEDGED_LITTLE_GIRL_SANDAL;
		    break;
		case PM_BLOODSEEKER:
		case PM_UNDEAD_BLOODSEEKER:
		    weapon = SHORT_SWORD;
		    break;
		case PM_SAIYAN:
		case PM_UNDEAD_SAIYAN:
		    weapon = BROADSWORD;
		    break;
		case PM_DOLL_MISTRESS:
		case PM_UNDEAD_DOLL_MISTRESS:
		    weapon = CRYSKNIFE;
		    break;
		case PM_MYSTIC:
		case PM_UNDEAD_MYSTIC:
		    weapon = TORPEDO;
		    break;
		case PM_ASSASSIN:
		case PM_UNDEAD_ASSASSIN:
		    weapon = GREAT_DAGGER;
		    break;
		case PM_BULLY:
		case PM_UNDEAD_BULLY:
		    weapon = KNIFE;
		    break;
		case PM_PICKPOCKET:
		case PM_UNDEAD_PICKPOCKET:
		    weapon = STILETTO;
		    break;
		case PM_OTAKU:
		case PM_UNDEAD_OTAKU:
		    weapon = KATANA;
		    break;
		case PM_PALADIN:
		case PM_UNDEAD_PALADIN:
		    weapon = LONG_SWORD;
		    break;
		case PM_SAGE:
		case PM_UNDEAD_SAGE:
		    weapon = SILVER_MACE;
		    break;
		case PM_SLAVE_MASTER:
		case PM_UNDEAD_SLAVE_MASTER:
		    weapon = STEEL_WHIP;
		    break;
		case PM_NOBLEMAN:
		case PM_NOBLEWOMAN:
		case PM_UNDEAD_NOBLEMAN:
		case PM_UNDEAD_NOBLEWOMAN:
		    weapon = RAPIER;
		    break;
		case PM_POKEMON:
		case PM_UNDEAD_POKEMON:
		case PM_MUSICIAN:
		case PM_UNDEAD_MUSICIAN:
		    weapon = CLUB;
		    break;
		case PM_DEATH_EATER:
		case PM_UNDEAD_DEATH_EATER:
		    weapon = QUARTERSTAFF;
		    break;
		case PM_HEALER:
		case PM_SCIENTIST:
		case PM_UNDEAD_HEALER:
		case PM_UNDEAD_SCIENTIST:
		    if (rn2(4)) weapon = QUARTERSTAFF;
		    else if (rn2(2)) weapon = rn2(2) ? UNICORN_HORN : SCALPEL;
		    if (rn2(4)) helm = rn2(2) ? HELM_OF_BRILLIANCE : HELM_OF_TELEPATHY;
		    if (rn2(2)) shield = STRANGE_OBJECT;
		    break;
		case PM_YEOMAN:
		case PM_KNIGHT:
		case PM_UNDEAD_YEOMAN:
		case PM_UNDEAD_KNIGHT:
		    if (rn2(4)) weapon = LONG_SWORD;
		    if (rn2(2)) armor = rnd_class(PLATE_MAIL, CHAIN_MAIL);
		    break;
		case PM_WARRIOR:
		case PM_UNDEAD_WARRIOR:
		    if (rn2(4)) weapon = MACE;
		    if (rn2(2)) armor = rnd_class(PLATE_MAIL, CHAIN_MAIL);
		case PM_MONK:
		case PM_PSION:
		case PM_UNDEAD_MONK:
		case PM_UNDEAD_PSION:
		    weapon = STRANGE_OBJECT;
		    armor = STRANGE_OBJECT;
		    cloak = ROBE;
		    if (rn2(2)) shield = STRANGE_OBJECT;
		    break;
		case PM_HALF_BAKED:
		case PM_UNDEAD_HALF_BAKED:
		    weapon = STRANGE_OBJECT;
		    armor = STRANGE_OBJECT;
		    if (rn2(2)) shield = STRANGE_OBJECT;
		    break;
		case PM_CHEVALIER:
		case PM_UNDEAD_CHEVALIER:
		    weapon = TRIDENT;
		    break;
		case PM_AUGURER:
		case PM_UNDEAD_AUGURER:
		    weapon = STRANGE_OBJECT;
		    break;
		case PM_POLITICIAN:
		case PM_UNDEAD_POLITICIAN:
		    weapon = STRANGE_OBJECT;
		    break;
		case PM_UNDERTAKER:
		case PM_UNDEAD_UNDERTAKER:
		    weapon = SCALPEL;
		    break;
		case PM_ZOOKEEPER:
		case PM_UNDEAD_ZOOKEEPER:
		    weapon = BULLWHIP;
		    break;
		case PM_CLIMACTERIAL:
		case PM_UNDEAD_CLIMACTERIAL:
		    weapon = RUBBER_HOSE;
		    break;
		case PM_WOMANIZER:
		case PM_UNDEAD_WOMANIZER:
		    weapon = SEXPLAY_WHIP;
		    break;
		case PM_FIREFIGHTER:
		case PM_UNDEAD_FIREFIGHTER:
		    weapon = AXE;
		    break;
		case PM_LOCKSMITH:
		case PM_UNDEAD_LOCKSMITH:
		    weapon = DAGGER;
		    break;
		case PM_ERDRICK:
		case PM_DQ_SLIME:
		case PM_UNDEAD_ERDRICK:
		case PM_UNDEAD_DQ_SLIME:
		    weapon = BATTLE_STAFF;
		    break;
		case PM_NINJA:
		case PM_UNDEAD_NINJA:
		    weapon = KATANA;
		    break;
		case PM_SPACEWARS_FIGHTER:
		case PM_UNDEAD_SPACEWARS_FIGHTER:
		    weapon = BLUE_LIGHTSABER;
		    break;
		case PM_CAMPERSTRIKER:
		case PM_UNDEAD_CAMPERSTRIKER:
		    weapon = GREEN_LIGHTSABER;
		    break;
		case PM_CARTOMANCER:
		case PM_UNDEAD_CARTOMANCER:
		    weapon = GREAT_DAGGER;
		    break;
		case PM_DRAGONMASTER:
		case PM_UNDEAD_DRAGONMASTER:
		    weapon = ELVEN_BROADSWORD;
		    break;
		case PM_COMBATANT:
		case PM_UNDEAD_COMBATANT:
		    break;
		case PM_FJORDE:
		case PM_UNDEAD_FJORDE:
		    weapon = TRIDENT;
		    break;
		case PM_PRACTICANT:
		case PM_UNDEAD_PRACTICANT:
		    weapon = STEEL_CAPPED_SANDAL;
		    break;
		case PM_EMERA:
		case PM_UNDEAD_EMERA:
		    weapon = BULLETPROOF_CHAINWHIP;
		    break;
		case PM_TOSSER:
		case PM_UNDEAD_TOSSER:
		    weapon = TORPEDO;
		    break;
		case PM_AKLYST:
		case PM_UNDEAD_AKLYST:
		    weapon = AKLYS;
		    break;
		case PM_MILL_SWALLOWER:
		case PM_UNDEAD_MILL_SWALLOWER:
		    weapon = CHEMISTRY_SPACE_AXE;
		    break;
		case PM_SYMBIANT:
		case PM_UNDEAD_SYMBIANT:
		    weapon = ORNATE_MACE;
		    break;
		case PM_GENDERSTARIST:
		case PM_UNDEAD_GENDERSTARIST:
		    weapon = TELEPHONE;
		    break;
		case PM_BUTT_LOVER:
		case PM_UNDEAD_BUTT_LOVER:
		    weapon = SEXPLAY_WHIP;
		    break;
		case PM_DANCER:
		case PM_UNDEAD_DANCER:
		    weapon = MOP;
		    break;
		case PM_DIABLIST:
		case PM_UNDEAD_DIABLIST:
		    weapon = !rn2(3) ? CAMO_QATAR : !rn2(2) ? VARIED_GRINDER : CIGARETTE;
		    break;
		case PM_PREVERSIONER:
		case PM_UNDEAD_PREVERSIONER:
		    weapon = GREAT_DAGGER;
		    break;
		case PM_SECRET_ADVICE_MEMBER:
		case PM_UNDEAD_SECRET_ADVICE_MEMBER:
		    weapon = SHADOWBLADE;
		    break;
		case PM_SHOE_FETISHIST:
		case PM_UNDEAD_SHOE_FETISHIST:
		    weapon = PROSTITUTE_SHOE;
		    break;
		case PM_PRIEST:
		case PM_PRIESTESS:
		case PM_UNDEAD_PRIEST:
		case PM_UNDEAD_PRIESTESS:
		    if (rn2(2)) weapon = MACE;
		    if (rn2(2)) armor = rnd_class(PLATE_MAIL, CHAIN_MAIL);
		    if (rn2(4)) cloak = ROBE;
		    if (rn2(4)) helm = rn2(2) ? HELM_OF_BRILLIANCE : HELM_OF_TELEPATHY;
		    if (rn2(2)) shield = STRANGE_OBJECT;
		    break;
		case PM_RANGER:
		case PM_UNDEAD_RANGER:
		    if (rn2(2)) weapon = ELVEN_DAGGER;
		    break;
		case PM_ELPH:
		case PM_UNDEAD_ELPH:
		    weapon = ELVEN_DAGGER;
		    break;
		case PM_TWELPH:
		case PM_UNDEAD_TWELPH:
		    weapon = DARK_ELVEN_DAGGER;
		    break;
		case PM_GOLDMINER:
		case PM_UNDEAD_GOLDMINER:
		    weapon = PICK_AXE;
		    break;
		case PM_MIDGET:
		case PM_UNDEAD_MIDGET:
		    weapon = DWARVISH_MATTOCK;
		    break;
		case PM_RINGSEEKER:
		case PM_UNDEAD_RINGSEEKER:
		    weapon = ELVEN_DAGGER;
		    break;
		case PM_DRUID:
		case PM_UNDEAD_DRUID:
		    weapon = BATTLE_STAFF;
		    break;
		case PM_SHAPESHIFTER:
		case PM_UNDEAD_SHAPESHIFTER:
		    weapon = ATHAME;
		    break;
		case PM_COURIER:
		case PM_UNDEAD_COURIER:
		    weapon = KNIFE;
		    break;
		case PM_ZYBORG:
		case PM_UNDEAD_ZYBORG:
		    weapon = RED_LIGHTSABER;
		    break;
		case PM_LUNATIC:
		case PM_UNDEAD_LUNATIC:
		    weapon = STEEL_WHIP;
		    break;
		case PM_PROSTITUTE:
		case PM_KURWA:
		case PM_UNDEAD_PROSTITUTE:
		case PM_UNDEAD_KURWA:
		    weapon = INKA_SHACKLE;
		    break;
		case PM_TRANSVESTITE:
		case PM_UNDEAD_TRANSVESTITE:
		    weapon = SEXY_LEATHER_PUMP;
		    break;
		case PM_TRANSSYLVANIAN:
		case PM_UNDEAD_TRANSSYLVANIAN:
		    weapon = BLOCK_HEELED_COMBAT_BOOT;
		    break;
		case PM_TOPMODEL:
		case PM_UNDEAD_TOPMODEL:
		    weapon = KNIFE;
		    break;
		case PM_UNBELIEVER:
		case PM_UNDEAD_UNBELIEVER:
		    weapon = BLACK_AESTIVALIS;
		    break;
		case PM_FAILED_EXISTENCE:
		case PM_UNDEAD_FAILED_EXISTENCE:
		    weapon = KNIFE;
		    break;
		case PM_ACTIVISTOR:
		case PM_UNDEAD_ACTIVISTOR:
		    weapon = FLY_SWATTER;
		    break;
		case PM_ROGUE:
		case PM_UNDEAD_ROGUE:
		    weapon = SHORT_SWORD;
		    armor = LEATHER_ARMOR;
		    break;
		case PM_SAMURAI:
		case PM_UNDEAD_SAMURAI:
		    if (rn2(2)) weapon = KATANA;
		    break;
		case PM_TOURIST:
		case PM_UNDEAD_TOURIST:
		    /* Defaults are just fine */
		    break;
		case PM_ROCKER:
		case PM_UNDEAD_ROCKER:
			weapon = HEAVY_HAMMER;
		    break;
		case PM_UNDEAD_SLAYER:
		case PM_NON_UNDEAD_SLAYER:
		    if (rn2(2)) weapon = SILVER_SPEAR;
		    if (rn2(2)) armor = rnd_class(PLATE_MAIL, CHAIN_MAIL);
		    break;
		case PM_VALKYRIE:
		case PM_UNDEAD_VALKYRIE:
		case PM_VANILLA_VALK:
		case PM_UNDEAD_VANILLA_VALK:
		    if (rn2(2)) weapon = WAR_HAMMER;
		    if (rn2(2)) armor = rnd_class(PLATE_MAIL, CHAIN_MAIL);
		    break;
		case PM_FLAME_MAGE:
		case PM_ACID_MAGE:
		case PM_ICE_MAGE:
		case PM_ELECTRIC_MAGE:
		case PM_POISON_MAGE:
		case PM_NECROMANCER:
		case PM_WIZARD:
		case PM_UNDEAD_FLAME_MAGE:
		case PM_UNDEAD_ACID_MAGE:
		case PM_UNDEAD_ICE_MAGE:
		case PM_UNDEAD_ELECTRIC_MAGE:
		case PM_UNDEAD_POISON_MAGE:
		case PM_UNDEAD_NECROMANCER:
		case PM_UNDEAD_WIZARD:
		    if (rn2(4)) weapon = rn2(2) ? QUARTERSTAFF : ATHAME;
		    if (rn2(2)) {
		    	armor = rn2(2) ? BLACK_DRAGON_SCALE_MAIL :
		    			SILVER_DRAGON_SCALE_MAIL;
		    	cloak = CLOAK_OF_MAGIC_RESISTANCE;
		    }
		    if (rn2(4)) helm = HELM_OF_BRILLIANCE;
		    shield = STRANGE_OBJECT;
		    break;

		case PM_OCCULT_MASTER:
		case PM_UNDEAD_OCCULT_MASTER:
		    weapon = COLLUSION_KNIFE;
		    break;

		case PM_ELEMENTALIST:
		case PM_UNDEAD_ELEMENTALIST:
		    weapon = FIRE_STICK;
		    break;

		case PM_CHAOS_SORCEROR:
		case PM_UNDEAD_CHAOS_SORCEROR:
		    weapon = IMPACT_STAFF;
		    break;

		case PM_WILD_TALENT:
		case PM_UNDEAD_WILD_TALENT:
		    weapon = HEAVY_IRON_BALL;
		    break;

		case PM_JEDI:
		case PM_SHADOW_JEDI:
		case PM_PADAWAN:
		case PM_UNDEAD_JEDI:
		case PM_UNDEAD_PADAWAN:
		case PM_UNDEAD_SHADOW_JEDI:
		case PM_HEDDERJEDI:
		case PM_UNDEAD_HEDDERJEDI:
		    switch(rnd(12)){
		      case 1: weapon = RED_LIGHTSABER; break;
		      case 2: weapon = BLUE_LIGHTSABER; break;
		      case 3: weapon = GREEN_LIGHTSABER; break;
		      case 4: weapon = WHITE_LIGHTSABER; break;
		      case 5: weapon = YELLOW_LIGHTSABER; break;
		      case 6: weapon = VIOLET_LIGHTSABER; break;
		      case 7: weapon = RED_DOUBLE_LIGHTSABER; break;
		      case 8: weapon = WHITE_DOUBLE_LIGHTSABER; break;
		      case 9: weapon = MYSTERY_LIGHTSABER; break;
		      case 10: weapon = ORANGE_LIGHTSABER; break;
		      case 11: weapon = BLACK_LIGHTSABER; break;
		      case 12: weapon = CYAN_DOUBLE_LIGHTSABER; break;
		    }
		    cloak = STRANGE_OBJECT;
		    armor = rn2(2) ? ROBE : ROBE_OF_POWER;
		    helm = STRANGE_OBJECT;
		    shield = STRANGE_OBJECT;
		    break;
		default: impossible("bad mplayer monster");
		    weapon = /*0*/LONG_SWORD;
		    break;
	    }

	    if (weapon != STRANGE_OBJECT) {
		otmp = mksobj(weapon, TRUE, FALSE, FALSE);
		if (otmp) {
			otmp->spe = (special ? rn1(5,4) : (rn2(4) - rn2(7) ) );
			if (!rn2(3)) otmp->oerodeproof = 1;
			else if (!rn2(2)) otmp->greased = 1;
			if (special && rn2(2))
			    otmp = mk_artifact(otmp, A_NONE, TRUE);
			/* mplayers knew better than to overenchant Magicbane */
			if (otmp->oartifact == ART_MAGICBANE && otmp->spe > 0)
			    otmp->spe = rnd(4);
			(void) mpickobj(mtmp, otmp, TRUE);
		}
	    }

	    if(special) {
		if (!rn2(10))
		    (void) mongets(mtmp, rn2(3) ? LUCKSTONE : LOADSTONE);
		mk_mplayer_armor(mtmp, armor);
		mk_mplayer_armor(mtmp, cloak);
		mk_mplayer_armor(mtmp, helm);
		mk_mplayer_armor(mtmp, shield);
		if (rn2(8))
		    mk_mplayer_armor(mtmp, rnd_class(LEATHER_GLOVES,
					       GAUNTLETS_OF_DEXTERITY));
		if (rn2(8))
		    mk_mplayer_armor(mtmp, rnd_class(LOW_BOOTS, LEVITATION_BOOTS));

		/* These are supposed to be ascension kits. Why were the shirts missing??? --Amy */
		if (rn2(15))
		    mk_mplayer_armor(mtmp, rnd_class(HAWAIIAN_SHIRT, VICTORIAN_UNDERWEAR));
		m_dowear(mtmp, TRUE);

		quan = rn2(3) ? rn2(3) : rn2(16);
		while(quan--)
		    (void)mongets(mtmp, rnd_class(DILITHIUM_CRYSTAL, JADE));
		/* To get the gold "right" would mean a player can double his */
		/* gold supply by killing one mplayer.  Not good. */
#ifndef GOLDOBJ
		mtmp->mgold = rn2(1000);
#else
		mkmonmoney(mtmp, rn2(1000));
#endif
		quan = rn2(10);
		while(quan--)
		    (void) mpickobj(mtmp, mkobj(RANDOM_CLASS, FALSE, FALSE), TRUE);
	    } else { /* wandering characters... */
#ifndef GOLDOBJ
	       mtmp->mgold = rn2((mtmp->m_lev)*100);
#else
	       mkmonmoney(mtmp, rn2((mtmp->m_lev)*100));
#endif
	    }
	    quan = rnd(3);
	    while(quan--) {
		(void)mongets(mtmp, rnd_offensive_item(mtmp));
		if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		}
	    quan = rnd(3);
	    while(quan--) {
		(void)mongets(mtmp, rnd_defensive_item(mtmp));
	      if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		}
	    quan = rnd(3);
	    while(quan--) {
		(void)mongets(mtmp, rnd_misc_item(mtmp));
		if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		}
	}

	return(mtmp);
}

/* create the indicated number (num) of monster-players,
 * randomly chosen, and in randomly chosen (free) locations
 * on the level.  If "special", the size of num should not
 * be bigger than the number of _non-repeated_ names in the
 * developers array, otherwise a bunch of Adams and Eves will
 * fill up the overflow.
 */
void
create_mplayers(num, special)
register int num;
boolean special;
{
	int pm, x, y;
	struct monst fakemon;

	while(num) {
		int tryct = 0;

		/* roll for character class */
		pm = PM_ARCHEOLOGIST + rn2(PM_WIZARD - PM_ARCHEOLOGIST + 1);
		fakemon.data = &mons[pm];

		/* roll for an available location */
		do {
		    x = rn1(COLNO-4, 2);
		    y = rnd(ROWNO-2);
		} while(!goodpos(x, y, &fakemon, 0) && tryct++ <= 50);

		/* if pos not found in 50 tries, don't bother to continue */
		if(tryct > 50) return;

		(void) mk_mplayer(&mons[pm], (xchar)x, (xchar)y, special);
		num--;
	}
}

void
create_umplayers(num, special)
register int num;
boolean special;
{
	int pm, x, y;
	struct monst fakemon;

	while(num) {
		int tryct = 0;

		/* roll for character class */
		pm = PM_UNDEAD_ARCHEOLOGIST + rn2(PM_UNDEAD_WIZARD - PM_UNDEAD_ARCHEOLOGIST + 1);
		fakemon.data = &mons[pm];

		/* roll for an available location */
		do {
		    x = rn1(COLNO-4, 2);
		    y = rnd(ROWNO-2);
		} while(!goodpos(x, y, &fakemon, 0) && tryct++ <= 50);

		/* if pos not found in 50 tries, don't bother to continue */
		if(tryct > 50) return;

		(void) mk_mplayer(&mons[pm], (xchar)x, (xchar)y, special);
		num--;
	}
}

void
mplayer_talk(mtmp)
register struct monst *mtmp;
{
	static const char *same_class_msg[5] = {
		"I can't win, and neither will you!",
		"You don't deserve to win!",
		"Mine should be the honor, not yours!",
		"Just give me that amulet so I can ascend, or I'll take it by force!",
		"I have many more artifacts than you! You're a disgrace to your profession!",
	},		  *other_class_msg[5] = {
		"The low-life wants to talk, eh?",
		"Fight, scum!",
		"Here is what I have to say!",
		"If I had the Vorpal Blade, I would decapitate you on the spot!",
		"Well your profession is stupid, I'm something much better than you.",
	};

	if(mtmp->mpeaceful) return; /* will drop to humanoid talk */

	pline("Talk? -- %s",
		(mtmp->data == &mons[urole.malenum] ||
		mtmp->data == &mons[urole.femalenum]) ?
		same_class_msg[rn2(4)] : other_class_msg[rn2(4)]);
}

/*mplayer.c*/
