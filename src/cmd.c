/*	SCCS Id: @(#)cmd.c	3.4	2003/02/06	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include <ctype.h>

#include "hack.h"
#include "extern.h"
#include "func_tab.h"
/* #define DEBUG */	/* uncomment for debugging */

/*
 * Some systems may have getchar() return EOF for various reasons, and
 * we should not quit before seeing at least NR_OF_EOFS consecutive EOFs.
 */
#if defined(SYSV) || defined(DGUX) || defined(HPUX)
#define NR_OF_EOFS	20
#endif

#define CMD_TRAVEL (char)0x90

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
	PN_MEMORIZATION,	PN_GUN_CONTROL,	PN_SQUEAKING,
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

#ifdef DEBUG
/*
 * only one "wiz_debug_cmd" routine should be available (in whatever
 * module you are trying to debug) or things are going to get rather
 * hard to link :-)
 */
extern int wiz_debug_cmd(void);
#endif

#ifdef DUMB	/* stuff commented out in extern.h, but needed here */
extern int doapply(void); /**/
extern int dorub(void); /**/
extern int dojump(void); /**/
extern int doextlist(void); /**/
extern int dodrop(void); /**/
extern int doddrop(void); /**/
extern int dodown(void); /**/
extern int doup(void); /**/
extern int donull(void); /**/
extern int dowipe(void); /**/
extern int do_mname(void); /**/
extern int ddocall(void); /**/
extern int dotakeoff(void); /**/
extern int doremring(void); /**/
extern int dowear(void); /**/
extern int doputon(void); /**/
extern int doddoremarm(void); /**/
extern int dokick(void); /**/
extern int dofire(void); /**/
extern int dofire(void); /**/
extern int dothrow(void); /**/
extern int doeat(void); /**/
extern int done2(void); /**/
extern int doengrave(void); /**/
extern int dopickup(void); /**/
extern int ddoinv(void); /**/
extern int dotypeinv(void); /**/
extern int dolook(void); /**/
extern int doprgold(void); /**/
extern int doprwep(void); /**/
extern int doprarm(void); /**/
extern int doprring(void); /**/
extern int dopramulet(void); /**/
extern int doprtool(void); /**/
extern int dosuspend(void); /**/
extern int doforce(void); /**/
extern int doopen(void); /**/
extern int doclose(void); /**/
extern int dosh(void); /**/
extern int dodiscovered(void); /**/
extern int doset(void); /**/
extern int dotogglepickup(void); /**/
extern int dowhatis(void); /**/
extern int doquickwhatis(void); /**/
extern int dowhatdoes(void); /**/
extern int dohelp(void); /**/
extern int dohistory(void); /**/
extern int doloot(void); /**/
extern int dodrink(void); /**/
extern int dodip(void); /**/
extern int dosacrifice(void); /**/
extern int dopray(void); /**/
extern int doturn(void); /**/
extern int dotech(void); /**/
extern int dotechwiz(void); /**/
extern int doredraw(void); /**/
extern int doread(void); /**/
extern int dosave(void); /**/
extern int dosearch(void); /**/
extern int doidtrap(void); /**/
extern int dopay(void); /**/
extern int dosit(void); /**/
extern int dotalk(void); /**/
extern int docast(void); /**/
extern int dovspell(void); /**/
extern int dotele(void); /**/
extern int dountrap(void); /**/
extern int doversion(void); /**/
extern int doextversion(void); /**/
extern int doswapweapon(void); /**/
extern int doswapweapon(void); /**/
extern int dowield(void); /**/
extern int dowieldquiver(void); /**/
extern int dowieldquiver(void); /**/
extern int dozap(void); /**/
extern int doorganize(void); /**/
extern int domarkforpet(void); /**/
extern int doremoveimarkers(void); /**/

#ifdef LIVELOG_SHOUT
extern int doshout(void); /**/
#endif

extern int dolistvanq(void); /**/

#endif /* DUMB */

#ifdef OVL1
static int (*timed_occ_fn)(void);
#endif /* OVL1 */

STATIC_PTR int doprev_message(void);
STATIC_PTR int timed_occupation(void);
STATIC_PTR int doextcmd(void);
#ifdef BORG
STATIC_PTR int doborgtoggle(void);
#endif
STATIC_PTR int domonability(void);
STATIC_PTR int dooverview_or_wiz_where(void);
STATIC_PTR int dotravel(void);
STATIC_PTR int playersteal(void);
#if 0
STATIC_PTR int specialpower(void); /* WAC -- use techniques */
#endif
# ifdef WIZARD
STATIC_PTR int wiz_wish(void);
STATIC_PTR int wiz_identify(void);
STATIC_PTR int wiz_map(void);
/* BEGIN TSANTH'S CODE */
STATIC_PTR int wiz_gain_ac(void);
STATIC_PTR int wiz_gain_level(void);
STATIC_PTR int wiz_toggle_invulnerability(void);
STATIC_PTR int wiz_detect_monsters(void);
/* END TSANTH'S CODE */
STATIC_PTR int wiz_genesis(void);
STATIC_PTR int wiz_where(void);
STATIC_PTR int wiz_detect(void);
STATIC_PTR int wiz_panic(void);
STATIC_PTR int wiz_polyself(void);
STATIC_PTR int wiz_level_tele(void);
STATIC_PTR int wiz_level_change(void);
STATIC_PTR int wiz_show_seenv(void);
STATIC_PTR int wiz_show_vision(void);
STATIC_PTR int wiz_mon_polycontrol(void);
STATIC_PTR int wiz_show_wmodes(void);
extern void list_vanquished(int, BOOLEAN_P); /* showborn patch */
#if defined(__BORLANDC__) && !defined(_WIN32)
extern void show_borlandc_stats(winid);
#endif
#ifdef DEBUG_MIGRATING_MONS
STATIC_PTR int wiz_migrate_mons(void);
#endif
STATIC_DCL void count_obj(struct obj *, long *, long *, BOOLEAN_P, BOOLEAN_P);
STATIC_DCL void obj_chain(winid, const char *, struct obj *, long *, long *);
STATIC_DCL void mon_invent_chain(winid, const char *, struct monst *, long *, long *);
STATIC_DCL void mon_chain(winid, const char *, struct monst *, long *, long *);
STATIC_DCL void contained(winid, const char *, long *, long *);
STATIC_PTR int wiz_show_stats(void);
#ifdef DISPLAY_LAYERS
STATIC_PTR int wiz_show_display(void);
#endif
#  ifdef PORT_DEBUG
STATIC_DCL int wiz_port_debug(void);
#  endif
# endif
int enter_explore_mode(void);
STATIC_PTR int doattributes(void);
STATIC_PTR int doconduct(void); /**/
STATIC_PTR boolean minimal_enlightenment(void);
STATIC_PTR int makemenu(const char *, struct menu_list *);

static NEARDATA struct rm *maploc;

static void bind_key(unsigned char, char*);
static void init_bind_list(void);
static void change_bind_list(void);
#ifdef WIZARD
static void add_debug_extended_commands(void);
#endif /* WIZARD */

#ifdef OVLB
STATIC_DCL void enlght_line(const char *,const char *,const char *);
STATIC_DCL char *enlght_combatinc(const char *,int,int,char *);
#ifdef UNIX
static void end_of_input(void);
#endif
#endif /* OVLB */

static const char* readchar_queue="";

STATIC_DCL char *parse(void);
STATIC_DCL boolean help_dir(CHAR_P,const char *);

STATIC_PTR int domenusystem(void); /* WAC the menus*/

STATIC_PTR int stefanjerepair(void);
static NEARDATA schar delay;            /* moves left for stefanje repairs */

#ifdef OVL1

STATIC_VAR NEARDATA const char *names[] = { 0,
	"Illegal objects", "Weapons", "Armor", "Rings", "Amulets", "Implants",
	"Tools", "Comestibles", "Potions", "Scrolls", "Spellbooks",
	"Wands", "Coins", "Gems", "Boulders/Statues", "Iron balls",
	"Chains", "Venoms"
};

STATIC_PTR int
doprev_message()
{

	if (MenuIsBugged) {
	pline("The previous message command is currently unavailable!");
	if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return 0;
	}

	if (AutomaticMorePrompt) {
	pline("No, sorry, you can't review earlier messages.");
	return 0;
	}

    return nh_doprev_message();
}

char *
montraitname(traitno) /* monstercolor function */
int traitno;
{

	const char *output;
	switch (traitno) {
		case 1:
			return "silent monsters";
		case 2:
			return "monsters that bark";
		case 3:
			return "monsters that mew";
		case 4:
			return "monsters that roar";
		case 5:
			return "monsters that growl";
		case 6:
			return "monsters that squeek";
		case 7:
			return "monsters that squawk";
		case 8:
			return "monsters that hiss";
		case 9:
			return "monsters that buzz";
		case 10:
			return "monsters that grunt";
		case 11:
			return "monsters that neigh";
		case 12:
			return "monsters that wail";
		case 13:
			return "monsters that gurgle";
		case 14:
			return "monsters that burble";
		case 15:
			return "monsters that shriek";
		case 16:
			return "monsters that rattle with their bones";
		case 17:
			return "monsters that laugh";
		case 18:
			return "monsters that mumble";
		case 19:
			return "monsters that imitate";
		case 20:
			return "monsters that make orc sounds";
		case 21:
			return "monsters that have humanoid speech";
		case 22:
			return "monsters that want to arrest the player";
		case 23:
			return "monsters with soldier dialogue";
		case 24:
			return "monsters with djinni dialogue";
		case 25:
			return "monsters with nurse dialogue";
		case 26:
			return "monsters with seduction dialogue";
		case 27:
			return "monsters with vampire dialogue";
		case 28:
			return "monsters that cuss";
		case 29:
			return "monsters that use nemesis dialogue";
		case 30:
			return "monsters that mutter cantrips";
		case 31:
			return "monsters that howl at the moon";
		case 32:
			return "monsters that boast";
		case 33:
			return "monsters that make sheep sounds";
		case 34:
			return "monsters that make chicken sounds";
		case 35:
			return "monsters that make cow sounds";
		case 36:
			return "monsters that make parrot sounds";
		case 37:
			return "monsters that talk like whores";
		case 38:
			return "monsters that produce tender farting noises";
		case 39:
			return "monsters that produce squeaky farting noises";
		case 40:
			return "monsters that produce loud farting noises";
		case 41:
			return "monsters that make boss taunts";
		case 42:
			return "monsters that make loud noise";
		case 43:
			return "monsters with fire resistance";
		case 44:
			return "monsters with cold resistance";
		case 45:
			return "monsters with sleep resistance";
		case 46:
			return "monsters with disintegration resistance";
		case 47:
			return "monsters with shock resistance";
		case 48:
			return "monsters with poison resistance";
		case 49:
			return "monsters with acid resistance";
		case 50:
			return "monsters with petrification resistance";
		case 51:
			return "monsters with death resistance";
		case 52:
			return "monsters with drain resistance";
		case 53:
			return "monsters that require +1 weapons to hit";
		case 54:
			return "monsters that require +2 weapons to hit";
		case 55:
			return "monsters that require +3 weapons to hit";
		case 56:
			return "monsters that require +4 weapons to hit";
		case 57:
			return "monsters that hit as a +1 weapon";
		case 58:
			return "monsters that hit as a +2 weapon";
		case 59:
			return "monsters that hit as a +3 weapon";
		case 60:
			return "monsters that hit as a +4 weapon";
		case 61:
			return "monsters that can fly";
		case 62:
			return "monsters that can swim";
		case 63:
			return "monsters that are amorphous";
		case 64:
			return "monsters that walk through walls";
		case 65:
			return "monsters that cling to the ceiling";
		case 66:
			return "monsters that can tunnel";
		case 67:
			return "monsters that need a pick-axe for tunneling";
		case 68:
			return "monsters that conceal under items";
		case 69:
			return "monsters that can hide";
		case 70:
			return "monsters that are amphibious";
		case 71:
			return "monsters that are breathless";
		case 72:
			return "monsters that cannot pick up items";
		case 73:
			return "monsters that have no eyes";
		case 74:
			return "monsters that have no hands";
		case 75:
			return "monsters that have no limbs";
		case 76:
			return "monsters that have no head";
		case 77:
			return "monsters that are mindless";
		case 78:
			return "monsters that are humanoid";
		case 79:
			return "monsters that count as animals";
		case 80:
			return "monsters that are slithy";
		case 81:
			return "monsters that are unsolid";
		case 82:
			return "monsters with a thick skin";
		case 83:
			return "monsters that can lay eggs";
		case 84:
			return "monsters that regenerate health";
		case 85:
			return "monsters that can see invisible";
		case 86:
			return "monsters that can teleport";
		case 87:
			return "monsters that can control teleport";
		case 88:
			return "monsters that are acidic when eaten";
		case 89:
			return "monsters that are poisonous when eaten";
		case 90:
			return "monsters that eat meat";
		case 91:
			return "monsters that eat veggies";
		case 92:
			return "monsters that eat both meat and veggies";
		case 93:
			return "monsters that eat metal";
		case 94:
			return "monsters that are not a valid polymorph form";
		case 95:
			return "monsters that count as undead";
		case 96:
			return "monsters that count as mercenaries";
		case 97:
			return "monsters that count as human";
		case 98:
			return "monsters that count as elf";
		case 99:
			return "monsters that count as dwarf";
		case 100:
			return "monsters that count as gnome";
		case 101:
			return "monsters that count as orc";
		case 102:
			return "monsters that count as hobbit";
		case 103:
			return "monsters that count as werecreatures";
		case 104:
			return "monsters that count as vampire";
		case 105:
			return "monsters that count as lord";
		case 106:
			return "monsters that count as prince";
		case 107:
			return "monsters that count as minion";
		case 108:
			return "monsters that count as giant";
		case 109:
			return "monsters that count as major demon";
		case 110:
			return "monsters that are always male";
		case 111:
			return "monsters that are always female";
		case 112:
			return "monsters that are neuter";
		case 113:
			return "monsters that have a proper name";
		case 114:
			return "monsters that are always spawned hostile";
		case 115:
			return "monsters that are always spawned peaceful";
		case 116:
			return "monsters that can be tamed with food";
		case 117:
			return "monsters that wander randomly";
		case 118:
			return "monsters that follow you up and down staircases";
		case 119:
			return "monsters that are extra nasty";
		case 120:
			return "monsters that are strong";
		case 121:
			return "monsters that can throw boulders";
		case 122:
			return "monsters that love to collect gold";
		case 123:
			return "monsters that love to collect gems";
		case 124:
			return "monsters that collect items";
		case 125:
			return "monsters that pick up magic items";
		case 126:
			return "monsters that want the Amulet of Yendor";
		case 127:
			return "monsters that want the Bell of Opening";
		case 128:
			return "monsters that want the Book of the Dead";
		case 129:
			return "monsters that want the Candelabrum of Invocation";
		case 130:
			return "monsters that want quest artifacts";
		case 131:
			return "monsters that wait for you to come";
		case 132:
			return "monsters that won't react unless you get up close";
		case 133:
			return "monsters that have infravision";
		case 134:
			return "monsters that can be seen with infravision";
		case 135:
			return "monsters that can spontaneously betray you";
		case 136:
			return "monsters that cannot be tamed";
		case 137:
			return "monsters that avoid being in a line with you";
		case 138:
			return "monsters that eat stone";
		case 139:
			return "monsters that are classified as petty";
		case 140:
			return "monsters that count as pokemon";
		case 141:
			return "monsters that are no valid polymorph forms for the player";
		case 142:
			return "monsters that never move";
		case 143:
			return "monsters that always generate with egotypes";
		case 144:
			return "monsters that are telepathic";
		case 145:
			return "monsters that can freely walk over spider webs";
		case 146:
			return "monsters which petrify others on touch";
		case 147:
			return "monsters that count as mind flayers";
		case 148:
			return "monsters whose corpses don't decay";
		case 149:
			return "monsters that like to mimic objects";
		case 150:
			return "monsters that permanently mimic objects";
		case 151:
			return "monsters that can turn you to slime when eaten";
		case 152:
			return "monsters that are uncommon by a factor 2";
		case 153:
			return "monsters that are uncommon by a factor 3";
		case 154:
			return "monsters that are uncommon by a factor 5";
		case 155:
			return "monsters that are uncommon by a factor 7";
		case 156:
			return "monsters that are uncommon by a factor 10";
		case 157:
			return "monsters that have mind flayer capabilities";
		case 158:
			return "monsters of tiny size";
		case 159:
			return "monsters of small size";
		case 160:
			return "monsters of medium size";
		case 161:
			return "monsters of large size";
		case 162:
			return "monsters of huge size";
		case 163:
			return "monsters of gigantic size";
		case 164:
			return "monsters that spawn in very large groups";
		case 165:
			return "unique monsters";
		case 166:
			return "monsters that spawn in small groups";
		case 167:
			return "monsters that spawn in large groups";
		case 168:
			return "monsters that can be genocided";
		case 169:
			return "monsters that never leave a corpse";
		case 170:
			return "monsters that have passive attacks";
		case 171:
			return "monsters that have claw attacks";
		case 172:
			return "monsters that have bite attacks";
		case 173:
			return "monsters that have kick attacks";
		case 174:
			return "monsters that have butt attacks";
		case 175:
			return "monsters that have touch attacks";
		case 176:
			return "monsters that have sting attacks";
		case 177:
			return "monsters that have hug attacks";
		case 178:
			return "monsters that have scratch attacks";
		case 179:
			return "monsters that have lash attacks";
		case 180:
			return "monsters that have spit attacks";
		case 181:
			return "monsters that have engulfing attacks";
		case 182:
			return "monsters that have breath attacks";
		case 183:
			return "monsters that have explosion attacks";
		case 184:
			return "monsters that explode when killed";
		case 185:
			return "monsters that have gaze attacks";
		case 186:
			return "monsters that have tentacle attacks";
		case 187:
			return "monsters that have trample attacks";
		case 188:
			return "monsters that have beam attacks";
		case 189:
			return "monsters that can multiply";
		case 190:
			return "monsters that have weapon attacks";
		case 191:
			return "monsters that can cast spells";
		case 192:
			return "monsters whose attacks do physical damage";
		case 193:
			return "monsters whose attacks can shoot magic missiles";
		case 194:
			return "monsters whose attacks do fire damage";
		case 195:
			return "monsters whose attacks do cold damage";
		case 196:
			return "monsters whose attacks can put you to sleep";
		case 197:
			return "monsters whose attacks can disintegrate you";
		case 198:
			return "monsters whose attacks do shock damage";
		case 199:
			return "monsters whose attacks poison you and drain strength";
		case 200:
			return "monsters whose attacks do acid damage";
		case 201:
			return "monsters whose attacks shoot light rays";
		case 202:
			return "monsters whose attacks blind you";
		case 203:
			return "monsters whose attacks stun you";
		case 204:
			return "monsters whose attacks slow you";
		case 205:
			return "monsters whose attacks paralyze you";
		case 206:
			return "monsters whose attacks can drain experience levels";
		case 207:
			return "monsters whose attacks drain magical energy";
		case 208:
			return "monsters whose attacks wound your legs";
		case 209:
			return "monsters whose attacks turn you to stone";
		case 210:
			return "monsters whose attacks cause it to stick to you";
		case 211:
			return "monsters whose attacks allow it to steal gold";
		case 212:
			return "monsters whose attacks allow it to steal items";
		case 213:
			return "monsters whose attacks allow it to seduce you and steal several items";
		case 214:
			return "monsters whose attacks teleport you";
		case 215:
			return "monsters whose attacks rust your equipment";
		case 216:
			return "monsters whose attacks confuse you";
		case 217:
			return "monsters whose attacks digest you";
		case 218:
			return "monsters whose attacks heal you";
		case 219:
			return "monsters whose attacks wrap around and drown you";
		case 220:
			return "monsters whose attacks infect you with lycanthropy";
		case 221:
			return "monsters whose attacks poison you and drain dexterity";
		case 222:
			return "monsters whose attacks poison you and drain constitution";
		case 223:
			return "monsters whose attacks drain your intelligence";
		case 224:
			return "monsters whose attacks make you deathly sick";
		case 225:
			return "monsters whose attacks rot your equipment";
		case 226:
			return "monsters whose attacks start sexual encounters";
		case 227:
			return "monsters whose attacks cause you to hallucinate";
		case 228:
			return "monsters who attack with the touch of death";
		case 229:
			return "monsters who attack to cause fever and chills";
		case 230:
			return "monsters whose attacks make you more hungry";
		case 231:
			return "monsters whose attacks turn you into a green slime";
		case 232:
			return "monsters whose attacks calm you";
		case 233:
			return "monsters whose attacks disenchant your equipment";
		case 234:
			return "monsters whose attacks polymorph you";
		case 235:
			return "monsters whose attacks corrode your equipment";
		case 236:
			return "monsters whose attacks tickle you";
		case 237:
			return "monsters whose attacks remove Elbereth engravings underneath you";
		case 238:
			return "monsters whose attacks cause slippery fingers";
		case 239:
			return "monsters whose attacks cause darkness";
		case 240:
			return "monsters whose attacks wither your equipment";
		case 241:
			return "monsters whose attacks drain your luck";
		case 242:
			return "monsters whose attacks numb you";
		case 243:
			return "monsters whose attacks freeze you solid";
		case 244:
			return "monsters whose attacks push you back";
		case 245:
			return "monsters whose attacks burn you";
		case 246:
			return "monsters whose attacks cause fear";
		case 247:
			return "monsters whose attacks reduce your divine protection";
		case 248:
			return "monsters whose attacks poison you and drain a random stat";
		case 249:
			return "monsters whose attacks allow the monster to heal its damaged hit points";
		case 250:
			return "monsters whose attacks burn your equipment with lava";
		case 251:
			return "monsters whose attacks cause fake messages to appear";
		case 252:
			return "monsters whose attacks cause Lethe waters to wash over you";
		case 253:
			return "monsters whose attacks cancel you";
		case 254:
			return "monsters whose attacks banish you";
		case 255:
			return "monsters whose attacks poison you and drain wisdom";
		case 256:
			return "monsters whose attacks shred your equipment";
		case 257:
			return "monsters whose attacks make your equipment wet";
		case 258:
			return "monsters whose attacks suck off your gear";
		case 259:
			return "monsters whose attacks shock you with high voltage";
		case 260:
			return "monsters who attack with big fat head spikes";
		case 261:
			return "monsters whose attacks abduct you";
		case 262:
			return "monsters whose attacks cause fire, cold, shock and drain life damage";
		case 263:
			return "monsters whose attacks become stronger the more often they hit";
		case 264:
			return "monsters whose attacks mirror yours";
		case 265:
			return "monsters whose attacks act like cursed unicorn horns";
		case 266:
			return "monsters whose attacks levelport you, or drain your levels if you can't levelport";
		case 267:
			return "monsters whose attacks suck your blood";
		case 268:
			return "monsters whose attacks shoot webs";
		case 269:
			return "monsters whose attacks teleport your gear away";
		case 270:
			return "monsters whose attacks cause depression";
		case 271:
			return "monsters whose attacks de-energize you";
		case 272:
			return "monsters whose attacks make you lazy";
		case 273:
			return "monsters whose attacks poison you and drain charisma";
		case 274:
			return "monsters whose attacks take you down a peg or two";
		case 275:
			return "monsters whose attacks are empowered with nexus";
		case 276:
			return "monsters whose attacks cause loud sound";
		case 277:
			return "monsters whose attacks cause gravity to warp around you";
		case 278:
			return "monsters whose attacks cause inertia";
		case 279:
			return "monsters whose attacks time you";
		case 280:
			return "monsters who attack with mana, which is completely irresistible";
		case 281:
			return "monsters who attack with plasma clouds";
		case 282:
			return "monsters whose attacks can drain your skills";
		case 283:
			return "monsters who can cast clerical spells";
		case 284:
			return "monsters who can cast arcane spells";
		case 285:
			return "monsters whose attacks do random things";
		case 286:
			return "monsters whose attacks steal the amulet";
		case 287:
			return "monsters whose attacks steal your intrinsics";
		case 288:
			return "monsters who attack with psionic blasts";
		case 289:
			return "monsters whose attacks badly poison you";
		case 290:
			return "monsters whose attacks eat your dreams";
		case 291:
			return "monsters whose attacks cause nasty stuff to happen";
		case 292:
			return "monsters whose attacks cause bad effects";
		case 293:
			return "monsters whose attacks corrode your entire inventory";
		case 294:
			return "monsters whose attacks curse your items";
		case 295:
			return "monsters whose attacks deactivate your intrinsics";
		case 296:
			return "monsters whose attacks cause you to fumble";
		case 297:
			return "monsters whose attacks dim you";
		case 298:
			return "monsters whose attacks cause map amnesia";
		case 299:
			return "monsters whose attacks freeze you with ice blocks";
		case 300:
			return "monsters whose attacks vaporize you";
		case 301:
			return "monsters who attack with razor-sharp stone edges";
		case 302:
			return "monsters whose attacks make you vomit";
		case 303:
			return "monsters who attack with highly corrosive chemical waste";
		case 304:
			return "monsters whose attacks frenzy you";
		case 305:
			return "monsters whose attacks disenchant your gear into the negatives";
		case 306:
			return "monsters whose attacks expose you to chaotic forces";
		case 307:
			return "monsters whose attacks make you insane";
		case 308:
			return "monsters whose attacks create traps";
		case 309:
			return "monsters whose attacks increase your carried weight";
		case 310:
			return "monsters whose attacks hit you with nether forces";
		case 311:
			return "monsters whose attacks are specific to their internal ID";
		case 312:
			return "monsters whose attacks are randomized with a very evil RNG";
		case 313:
			return "monsters who can cast any random monster spell";
		case 314:
			return "monsters that count as bats";
		case 315:
			return "monsters that revive after death";
		case 316:
			return "monsters that count as rats";
		case 317:
			return "monsters that are immune to non-silver weapons";
		case 318:
			return "monsters that have intrinsic reflection";
		case 319:
			return "monsters that are fleecy-colored <3";
		case 320:
			return "monsters that usually spawn tame";
		case 321:
			return "monsters that eat leather and other organics";
		case 322:
			return "monsters that originate from Castle of the Winds";
		case 323:
			return "joke monsters";
		case 324:
			return "monsters that originate from Angband";
		case 325:
			return "monsters that originate from Steamband";
		case 326:
			return "monsters that originate from Animeband";
		case 327:
			return "monsters that originate from the Diablo game series";
		case 328:
			return "monsters that originate from Dungeon Lords";
		case 329:
			return "monsters that originate from Vanilla NetHack or SLASH'EM";
		case 330:
			return "monsters that originate from dnethack";
		case 331:
			return "monsters whose stats are randomized per playthrough";
		case 332:
			return "monster shoes (AmyBSOD loves those!)";
		case 333:
			return "monsters that use an evil scentful perfume";
		case 334:
			return "monsters whose attacks drain alignment points";
		case 335:
			return "monsters whose attacks increase the sin counter";
		case 336:
			return "monsters with identity-specific nastiness attacks";
		case 337:
			return "monsters whose attacks contaminate you";
		case 338:
			return "monsters whose attacks aggravate monsters";
		case 339:
			return "monsters that were generated by jonadab's perlscript (nope, slex is not being taken over by robots)";
		case 340:
			return "monsters whose attacks delete your data";
		case 341:
			return "monsters from the Evil Variant";
		case 342:
			return "shapeshifting monsters";
		case 343:
			return "monsters that move like grid bugs";
		case 344:
			return "monsters whose attacks destroy your armor and weapons";
		case 345:
			return "monsters whose attacks cause you to tremble";
		case 346:
			return "monsters who can cause Ragnarok";
		case 347:
			return "monsters who try to convert you to their religion";
		case 348:
			return "HC aliens";
		case 349:
			return "monsters whose attacks randomly damage your items";
		case 350:
			return "monsters who attack with antimatter";
		case 351:
			return "monsters with extra painful attacks";
		case 352:
			return "monsters whose attacks increase your technique timeouts";
		case 353:
			return "monsters whose attacks reduce your spell memory";
		case 354:
			return "monsters whose attacks reduce your skill training";
		case 355:
			return "monsters whose attacks permanently damage your stats";
		case 356:
			return "monsters who deal random screwup damage";
		case 357:
			return "monsters with random thieving attacks";
		case 358:
			return "monsters with a game-specific regular attack";
		case 359:
			return "monsters with a game-specific nastiness attack";
		case 360:
			return "monsters with ranged thorns";
		case 361:
			return "monsters that originate from Elona";
		case 362:
			return "monsters that put nasty curses on your equipment";
		case 363:
			return "monsters whose attacks increase your sanity";
		case 364:
			return "monsters whose attacks cause really bad effects";
		case 365:
			return "monsters with bleedout attacks";
		case 366:
			return "monsters whose attacks shank you out of your armor";
		case 367:
			return "monsters that drain your score";
		case 368:
			return "monsters that cause terrain terror";
		case 369:
			return "monsters with feminism attacks";
		case 370:
			return "monsters with levitation attacks";
		case 371:
			return "monsters with illusion attacks";

		default:
			impossible("bad trait no %d, please update montraitname() in cmd.c", traitno);
			return "unknown";

	}

}

/* Count down by decrementing multi */
STATIC_PTR int
timed_occupation()
{
	(*timed_occ_fn)();
	if (multi > 0)
		multi--;
	return multi > 0;
}

/* If you have moved since initially setting some occupations, they
 * now shouldn't be able to restart.
 *
 * The basic rule is that if you are carrying it, you can continue
 * since it is with you.  If you are acting on something at a distance,
 * your orientation to it must have changed when you moved.
 *
 * The exception to this is taking off items, since they can be taken
 * off in a number of ways in the intervening time, screwing up ordering.
 *
 *	Currently:	Take off all armor.
 *			Picking Locks / Forcing Chests.
 *			Setting traps.
 */
void
reset_occupations()
{
	reset_remarm();
	reset_pick();
	reset_trapset();
}

/* If a time is given, use it to timeout this function, otherwise the
 * function times out by its own means.
 */
void
set_occupation(fn, txt, xtime)
int (*fn)(void);
const char *txt;
int xtime;
{
	if (xtime) {
		occupation = timed_occupation;
		timed_occ_fn = fn;
	} else
		occupation = fn;
	occtxt = txt;
	occtime = 0;
	return;
}


static char popch(void);

/* Provide a means to redo the last command.  The flag `in_doagain' is set
 * to true while redoing the command.  This flag is tested in commands that
 * require additional input (like `throw' which requires a thing and a
 * direction), and the input prompt is not shown.  Also, while in_doagain is
 * TRUE, no keystrokes can be saved into the saveq.
 */
#define BSIZE 20
static char pushq[BSIZE], saveq[BSIZE];
static NEARDATA int phead, ptail, shead, stail;

static char
popch() {
	/* If occupied, return '\0', letting tgetch know a character should
	 * be read from the keyboard.  If the character read is not the
	 * ABORT character (as checked in pcmain.c), that character will be
	 * pushed back on the pushq.
	 */
	if (occupation) return '\0';
	if (in_doagain) return(char)((shead != stail) ? saveq[stail++] : '\0');
	else		return(char)((phead != ptail) ? pushq[ptail++] : '\0');
}

char
pgetchar() {		/* curtesy of aeb@cwi.nl */
	register int ch;

	if(!(ch = popch()))
		ch = nhgetch();
	return((char)ch);
}

/* A ch == 0 resets the pushq */
void
pushch(ch)
char ch;
{
	if (!ch)
		phead = ptail = 0;
	if (phead < BSIZE)
		pushq[phead++] = ch;
	return;
}

/* A ch == 0 resets the saveq.	Only save keystrokes when not
 * replaying a previous command.
 */
void
savech(ch)
char ch;
{
	if (!in_doagain) {
		if (!ch)
			phead = ptail = shead = stail = 0;
		else if (shead < BSIZE)
			saveq[shead++] = ch;
	}
	return;
}

#endif /* OVL1 */
#ifdef OVLB

STATIC_PTR int
doextcmd()	/* here after # - now read a full-word command */
{
	int idx, retval;

	/* keep repeating until we don't run help or quit */
	do {
	    idx = get_ext_cmd();
	    if (idx < 0) return 0;	/* quit */

	    retval = (*extcmdlist[idx].ef_funct)();
	} while (extcmdlist[idx].ef_funct == doextlist);

	return retval;
}

int
doextlist()	/* here after #? - now list all full-word commands */
{
	register const struct ext_func_tab *efp;
	char	 buf[BUFSZ];
	winid datawin;

	datawin = create_nhwindow(NHW_TEXT);
	putstr(datawin, 0, "");
	putstr(datawin, 0, "            Extended Commands List");
	putstr(datawin, 0, "");
	putstr(datawin, 0, "    Press '#', then type:");
	putstr(datawin, 0, "");

	for(efp = extcmdlist; efp->ef_txt; efp++) {
		/* Show name and text for each command.  Autocompleted
		 * commands are marked with an asterisk ('*'). */
		sprintf(buf, "  %c %-15s - %s.",
		efp->autocomplete ? '*' : ' ',
		efp->ef_txt, efp->ef_desc);
		putstr(datawin, 0, buf);
	}
	putstr(datawin, 0, "");
	putstr(datawin, 0, "    Commands marked with a * will be autocompleted.");
	display_nhwindow(datawin, FALSE);
	destroy_nhwindow(datawin);
	return 0;
}

STATIC_PTR int
doremoveimarkers()
{
	int x, y;

	if (uarmc && uarmc->oartifact == ART_TURN_LOSS_EXTREME) return 0;

	for(x=0; x<COLNO; x++) for(y=0; y<ROWNO; y++) {
		if (isok(x, y)) {
			if (memory_is_invisible(x, y)) {
			    unmap_object(x,y);
			    newsym(x,y);
			}
		}
	}
	pline("Remembered monster markers removed.");

	return 0;
}


#ifdef BORG
static int doborgtoggle(void) {
	if (yn_function("Really enable cyborg?", ynqchars, 'n') == 'y') {
		borg_on = 1;
		pline("The cyborg is enabled.... Good luck!");
	}
	return 0;
}
#endif

#if defined(TTY_GRAPHICS) || defined(CURSES_GRAPHICS) || defined(GL_GRAPHICS) || defined(SDL_GRAPHICS)
#define MAX_EXT_CMD 200		/* Change if we ever have > 40 ext cmds */  /* So we can only rebind keys up to 200 times?  Although unlikely to ever be encountered, I would call this a bug --Elronnd */
/*
 * This is currently used only by the tty port and is
 * controlled via runtime option 'extmenu'
 * -AJA- The SDL/GL window systems use it too.
 */
int
extcmd_via_menu()	/* here after # - now show pick-list of possible commands */
{
    const struct ext_func_tab *efp;
    menu_item *pick_list = (menu_item *)0;
    winid win;
    anything any;
    const struct ext_func_tab *choices[MAX_EXT_CMD];
    char buf[BUFSZ];
    char cbuf[QBUFSZ], prompt[QBUFSZ], fmtstr[20];
    int i, n, nchoices, acount;
    int ret,  biggest;
    int accelerator, prevaccelerator;
    int  matchlevel = 0;

    ret = 0;
    cbuf[0] = '\0';
    biggest = 0;
    while (!ret) {
	    i = n = 0;
	    accelerator = 0;
	    any.a_void = 0;
	    /* populate choices */
	    for(efp = extcmdlist; efp->ef_txt; efp++) {
		if (!matchlevel || !strncmp(efp->ef_txt, cbuf, matchlevel)) {
			choices[i++] = efp;
			if ((int)strlen(efp->ef_desc) > biggest) {
				biggest = strlen(efp->ef_desc);
				sprintf(fmtstr,"%%-%ds", biggest + 15);
			}
#ifdef DEBUG
			if (i >= MAX_EXT_CMD - 2) {
			    impossible("Exceeded %d extended commands in doextcmd() menu",
					MAX_EXT_CMD - 2);
			    return 0;
			}
#endif
		}
	    }
	    choices[i] = (struct ext_func_tab *)0;
	    nchoices = i;
	    /* if we're down to one, we have our selection so get out of here */
	    if (nchoices == 1) {
		for (i = 0; extcmdlist[i].ef_txt != (char *)0; i++)
			if (!strncmpi(extcmdlist[i].ef_txt, cbuf, matchlevel)) {
				ret = i;
				break;
			}
		break;
	    }

	    /* otherwise... */
	    win = create_nhwindow(NHW_MENU);
	    start_menu(win);
	    prevaccelerator = 0;
	    acount = 0;
	    for(i = 0; choices[i]; ++i) {
		accelerator = choices[i]->ef_txt[matchlevel];
		if (accelerator != prevaccelerator || nchoices < (ROWNO - 3)) {
		    if (acount) {
 			/* flush the extended commands for that letter already in buf */
			sprintf(buf, fmtstr, prompt);
			any.a_char = prevaccelerator;
			add_menu(win, NO_GLYPH, &any, any.a_char, 0,
					ATR_NONE, buf, FALSE);
			acount = 0;
		    }
		}
		prevaccelerator = accelerator;
		if (!acount || nchoices < (ROWNO - 3)) {
		    sprintf(prompt, "%s [%s]", choices[i]->ef_txt,
				choices[i]->ef_desc);
		} else if (acount == 1) {
		    sprintf(prompt, "%s or %s", choices[i-1]->ef_txt,
				choices[i]->ef_txt);
		} else {
		    strcat(prompt," or ");
		    strcat(prompt, choices[i]->ef_txt);
		}
		++acount;
	    }
	    if (acount) {
		/* flush buf */
		sprintf(buf, fmtstr, prompt);
		any.a_char = prevaccelerator;
		add_menu(win, NO_GLYPH, &any, any.a_char, 0, ATR_NONE, buf, FALSE);
	    }
	    sprintf(prompt, "Extended Command: %s", cbuf);
	    end_menu(win, prompt);
	    n = select_menu(win, PICK_ONE, &pick_list);
	    destroy_nhwindow(win);
	    if (n==1) {
		if (matchlevel > (QBUFSZ - 2)) {
			free((void *)pick_list);
#ifdef DEBUG
			impossible("Too many characters (%d) entered in extcmd_via_menu()",
				matchlevel);
#endif
			ret = -1;
		} else {
			cbuf[matchlevel++] = pick_list[0].item.a_char;
			cbuf[matchlevel] = '\0';
			free((void *)pick_list);
		}
	    } else {
		if (matchlevel) {
			ret = 0;
			matchlevel = 0;
		} else
			ret = -1;
	    }
    }
    return ret;
}
#endif

STATIC_PTR int
dooverview_or_wiz_where()
{
#ifdef WIZARD
	if (wizard) return wiz_where();
	else
#endif
	dooverview();
	return 0;
}

/* #monster command - use special monster ability while polymorphed */
/* edit by Amy - some polymorph forms have several abilities, so I'll try to make the player able to choose. */
STATIC_PTR int
domonability()
{

	/* snail can't equip pick-axes, so should be able to dig without one from time to time --Amy */
	if (Race_if(PM_ELONA_SNAIL) && !u.snaildigging && yn("Do you want to fire a digging ray?")=='y' ) {
		u.snaildigging = rnz(1000);
		if (!PlayerCannotUseSkills && u.snaildigging >= 2) {
			switch (P_SKILL(P_SQUEAKING)) {
		      	case P_BASIC:	u.snaildigging *= 9; u.snaildigging /= 10; break;
		      	case P_SKILLED:	u.snaildigging *= 8; u.snaildigging /= 10; break;
		      	case P_EXPERT:	u.snaildigging *= 7; u.snaildigging /= 10; break;
		      	case P_MASTER:	u.snaildigging *= 6; u.snaildigging /= 10; break;
		      	case P_GRAND_MASTER:	u.snaildigging *= 5; u.snaildigging /= 10; break;
		      	case P_SUPREME_MASTER:	u.snaildigging *= 4; u.snaildigging /= 10; break;
		      	default: break;
			}
		}
		getdir((char *)0);
		zap_dig(FALSE); /* dig only one tile, just like in Elona */
		use_skill(P_SQUEAKING, rnd(20));
		return TRUE;
	}
	else if (uarmf && uarmf->oartifact == ART_STEFANJE_S_PROBLEM && yn("Do you want to repair your 'Stefanje' sandals?")=='y') {
		if (uarmf->spe >= 0) {
			pline("Your sandals don't need repairs right now!");
			return TRUE;
		} else {
			delay = (uarmf->spe < -10) ? (-((uarmf->spe + 51) * 3)) : (-((uarmf->spe + 23) * 10));
			set_occupation(stefanjerepair, "repairing your 'Stefanje' sandals", 0);
			return TRUE;
		}
	} else if (issokosolver && !u.sokosolveboulder && yn("Do you want to create a boulder?")=='y' ) {
		u.sokosolveboulder = rnz(1000);
		if (!PlayerCannotUseSkills && u.sokosolveboulder >= 2) {
			switch (P_SKILL(P_SQUEAKING)) {
		      	case P_BASIC:	u.sokosolveboulder *= 9; u.sokosolveboulder /= 10; break;
		      	case P_SKILLED:	u.sokosolveboulder *= 8; u.sokosolveboulder /= 10; break;
		      	case P_EXPERT:	u.sokosolveboulder *= 7; u.sokosolveboulder /= 10; break;
		      	case P_MASTER:	u.sokosolveboulder *= 6; u.sokosolveboulder /= 10; break;
		      	case P_GRAND_MASTER:	u.sokosolveboulder *= 5; u.sokosolveboulder /= 10; break;
		      	case P_SUPREME_MASTER:	u.sokosolveboulder *= 4; u.sokosolveboulder /= 10; break;
		      	default: break;
			}
		}
		register struct obj *otmp2;
		otmp2 = mksobj(BOULDER, FALSE, FALSE);
		if (!otmp2) {
			pline("For some strange reason, no boulder appeared!");
			return TRUE;
		}
		otmp2->quan = 1;
		otmp2->owt = weight(otmp2);
		place_object(otmp2, u.ux, u.uy);
		stackobj(otmp2);
		newsym(u.ux, u.uy);
		pline("Kadoom! A boulder appeared underneath you.");
		use_skill(P_SQUEAKING, rnd(10));
		return TRUE;
	} else if (issokosolver && !u.sokosolveuntrap && yn("Do you want to disarm adjacent traps? (this doesn't work on pits, holes or other boulder-swallowing traps)")=='y' ) {
		int i, j, bd = 1, trpcount = 0, undtrpcnt = 0;
		struct trap *ttmp;

		u.sokosolveuntrap = rnz(4000);
		if (!PlayerCannotUseSkills && u.sokosolveuntrap >= 2) {
			switch (P_SKILL(P_SQUEAKING)) {
		      	case P_BASIC:	u.sokosolveuntrap *= 9; u.sokosolveuntrap /= 10; break;
		      	case P_SKILLED:	u.sokosolveuntrap *= 8; u.sokosolveuntrap /= 10; break;
		      	case P_EXPERT:	u.sokosolveuntrap *= 7; u.sokosolveuntrap /= 10; break;
		      	case P_MASTER:	u.sokosolveuntrap *= 6; u.sokosolveuntrap /= 10; break;
		      	case P_GRAND_MASTER:	u.sokosolveuntrap *= 5; u.sokosolveuntrap /= 10; break;
		      	case P_SUPREME_MASTER:	u.sokosolveuntrap *= 4; u.sokosolveuntrap /= 10; break;
		      	default: break;
			}
		}

		for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {

			if ((ttmp = t_at(u.ux + i, u.uy + j)) != 0) {
				if (ttmp->ttyp == MAGIC_PORTAL || ttmp->ttyp == HOLE || ttmp->ttyp == TRAPDOOR || ttmp->ttyp == SHAFT_TRAP || ttmp->ttyp == CURRENT_SHAFT || ttmp->ttyp == PIT || ttmp->ttyp == SPIKED_PIT || ttmp->ttyp == GIANT_CHASM || ttmp->ttyp == SHIT_PIT || ttmp->ttyp == MANA_PIT || ttmp->ttyp == ANOXIC_PIT || ttmp->ttyp == ACID_PIT) {
					undtrpcnt++;
					continue;
				}
				deltrap(ttmp);
				trpcount++;
			}

		}
		(void) doredraw();
		pline("%d traps were disarmed.", trpcount);
		if (undtrpcnt) pline("%d traps could not be disarmed.", undtrpcnt);
		use_skill(P_SQUEAKING, rnd(30));
		return TRUE;
	} else if (can_breathe(youmonst.data) && yn("Do you want to use your breath attack?")=='y' ) return dobreathe();
	else if (attacktype(youmonst.data, AT_SPIT) && yn("Do you want to use your spit attack?")=='y' ) return dospit();
	else if (youmonst.data->mlet == S_NYMPH && yn("Do you want to remove an iron ball?")=='y' ) return doremove();
	else if (attacktype(youmonst.data, AT_GAZE) && yn("Do you want to use your gaze attack?")=='y' ) return dogaze();
	else if (is_were(youmonst.data) && yn("Do you want to summon help?")=='y' ) return dosummon();
	else if (webmaker(youmonst.data) && yn("Do you want to spin webs?")=='y' ) return dospinweb();
	else if (is_hider(youmonst.data) && yn("Do you want to hide?")=='y' ) return dohide();
	else if (is_mind_flayer(youmonst.data) && yn("Do you want to emit a mind blast?")=='y' ) return domindblast();
	else if (u.umonnum == PM_GREMLIN && yn("Do you want to replicate in water?")=='y' ) {
	    if(IS_FOUNTAIN(levl[u.ux][u.uy].typ)) {
		if (split_mon(&youmonst, (struct monst *)0))
		    dryup(u.ux, u.uy, TRUE);
	    } else There("is no fountain here.");
	} else if ( (is_unicorn(youmonst.data) || (Race_if(PM_PLAYER_UNICORN) && !Upolyd) ) && yn("Do you want to cure yourself with your horn?")=='y' ) {
	    use_unicorn_horn((struct obj *)0);
	    return 1;
	} else if (youmonst.data->msound == MS_SHRIEK && yn("Do you want to shriek?")=='y' ) {
	    You("shriek.");
	    if(u.uburied)
		pline("Unfortunately sound does not carry well through rock.");
	    else aggravate();
	} else if ((youmonst.data->msound == MS_FART_QUIET || (Race_if(PM_LOLI) && !Upolyd && mons[PM_LOLI].msound == MS_FART_QUIET)) && yn("Do you want to fart?")=='y' ) {
		if (u.uhunger <= 10) {
			pline("There isn't enough gas stored in your %s butt!", flags.female ? "sexy" : "ugly");
			return 0;
		} else {
			morehungry(10);
			pline("You produce %s farting noises with your %s butt.", rn2(2) ? "tender" : "soft", flags.female ? "sexy" : "ugly");
			use_skill(P_SQUEAKING, 1);
			u.cnd_fartingcount++;

			if (uarmf && uarmf->oartifact == ART_SARAH_S_GRANNY_WEAR) {
				healup((level_difficulty() + 5), 0, FALSE, FALSE);
				morehungry(200);
				return 1;
			}

			if (uarmf && uarmf->oartifact == ART_ELIANE_S_SHIN_SMASH) {
				pline("The farting gas destroys your footwear instantly.");
			      useup(uarmf);
			}
			if (uarmf && uarmf->oartifact == ART_ELIANE_S_COMBAT_SNEAKERS) {
				pline("Eek! You can't stand farting gas!");
				badeffect();
				badeffect();
				badeffect();
				badeffect();
			}
			if (!extralongsqueak()) badeffect();

			if (uarmh && itemhasappearance(uarmh, APP_BREATH_CONTROL_HELMET) ) {
				pline("Your breath control helmet keeps pumping the farting gas into your %s...", body_part(NOSE));
				badeffect();
				badeffect();
			}

			if (uarmh && uarmh->oartifact == ART_VACUUM_CLEANER_DEATH) {
				pline("The farting gas almost asphyxiates you!");
				badeffect();
				badeffect();
				badeffect();
				badeffect();
				badeffect();
				losehp(rnd(u.ulevel * 3), "suffocating on farting gas", KILLED_BY);
			}

			return 1;
		}
	} else if ((youmonst.data->msound == MS_FART_NORMAL || (Race_if(PM_LOLI) && !Upolyd && mons[PM_LOLI].msound == MS_FART_NORMAL)) && yn("Do you want to fart?")=='y' ) {
		if (u.uhunger <= 10) {
			pline("There isn't enough gas stored in your %s butt!", flags.female ? "sexy" : "ugly");
			return 0;
		} else {
			morehungry(10);
			pline("You produce %s farting noises with your %s butt.", rn2(2) ? "beautiful" : "squeaky", flags.female ? "sexy" : "ugly");
			use_skill(P_SQUEAKING, 1);
			u.cnd_fartingcount++;

			if (uarmf && uarmf->oartifact == ART_ELIANE_S_SHIN_SMASH) {
				pline("The farting gas destroys your footwear instantly.");
			      useup(uarmf);
			}
			if (uarmf && uarmf->oartifact == ART_ELIANE_S_COMBAT_SNEAKERS) {
				pline("Eek! You can't stand farting gas!");
				badeffect();
				badeffect();
				badeffect();
				badeffect();
			}
			if (!extralongsqueak()) badeffect();

			if (uarmh && itemhasappearance(uarmh, APP_BREATH_CONTROL_HELMET) ) {
				pline("Your breath control helmet keeps pumping the farting gas into your %s...", body_part(NOSE));
				badeffect();
				badeffect();
			}

			if (uarmh && uarmh->oartifact == ART_VACUUM_CLEANER_DEATH) {
				pline("The farting gas almost asphyxiates you!");
				badeffect();
				badeffect();
				badeffect();
				badeffect();
				badeffect();
				losehp(rnd(u.ulevel * 3), "suffocating on farting gas", KILLED_BY);
			}

			return 1;
		}
	} else if ((youmonst.data->msound == MS_FART_LOUD || (Race_if(PM_LOLI) && !Upolyd && mons[PM_LOLI].msound == MS_FART_LOUD)) && yn("Do you want to fart?")=='y' ) {
		if (u.uhunger <= 10) {
			pline("There isn't enough gas stored in your %s butt!", flags.female ? "sexy" : "ugly");
			return 0;
		} else {
			morehungry(10);
			pline("You produce %s farting noises with your %s butt.", rn2(2) ? "disgusting" : "loud", flags.female ? "sexy" : "ugly");
			use_skill(P_SQUEAKING, 1);
			u.cnd_fartingcount++;

			if (uarmf && uarmf->oartifact == ART_ELIANE_S_SHIN_SMASH) {
				pline("The farting gas destroys your footwear instantly.");
			      useup(uarmf);
			}
			if (uarmf && uarmf->oartifact == ART_ELIANE_S_COMBAT_SNEAKERS) {
				pline("Eek! You can't stand farting gas!");
				badeffect();
				badeffect();
				badeffect();
				badeffect();
			}
			if (!extralongsqueak()) badeffect();

			if (uarmh && itemhasappearance(uarmh, APP_BREATH_CONTROL_HELMET) ) {
				pline("Your breath control helmet keeps pumping the farting gas into your %s...", body_part(NOSE));
				badeffect();
				badeffect();
			}

			if (uarmh && uarmh->oartifact == ART_VACUUM_CLEANER_DEATH) {
				pline("The farting gas almost asphyxiates you!");
				badeffect();
				badeffect();
				badeffect();
				badeffect();
				badeffect();
				losehp(rnd(u.ulevel * 3), "suffocating on farting gas", KILLED_BY);
			}

			return 1;
		}
	} else if (Role_if(PM_HUSSY) && !u.hussyperfume && ((!Upolyd && flags.female) || (Upolyd && youmonst.data->msound == MS_STENCH)) && yn("Do you want to spread your scentful perfume?") == 'y') {
		You("spread the lovely feminine drum stint reluctance brand perfume to intoxicate monsters around you!");
		int mondistance = 0;
		struct monst *mtmp3;
		int k, l;
		for (k = -5; k <= 5; k++) for(l = -5; l <= 5; l++) {
			if (!isok(u.ux + k, u.uy + l)) continue;

			mondistance = 1;
			if (k > 1) mondistance = k;
			if (k < -1) mondistance = -k;
			if (l > 1 && l > mondistance) mondistance = l;
			if (l < -1 && (-l > mondistance)) mondistance = -l;

			if ( (mtmp3 = m_at(u.ux + k, u.uy + l)) != 0) {
				mtmp3->mcanmove = 0;
				mtmp3->mfrozen = (rnd(16 - (mondistance * 2)));
				mtmp3->mstrategy &= ~STRAT_WAITFORU;
				mtmp3->mconf = TRUE;
				pline("%s becomes dizzy from the scent!", Monnam(mtmp3));
			}
		}
		u.hussyperfume = rnz(4000);
		if (!PlayerCannotUseSkills && u.hussyperfume >= 2) {
			switch (P_SKILL(P_SQUEAKING)) {
		      	case P_BASIC:	u.hussyperfume *= 9; u.hussyperfume /= 10; break;
		      	case P_SKILLED:	u.hussyperfume *= 8; u.hussyperfume /= 10; break;
		      	case P_EXPERT:	u.hussyperfume *= 7; u.hussyperfume /= 10; break;
		      	case P_MASTER:	u.hussyperfume *= 6; u.hussyperfume /= 10; break;
		      	case P_GRAND_MASTER:	u.hussyperfume *= 5; u.hussyperfume /= 10; break;
		      	case P_SUPREME_MASTER:	u.hussyperfume *= 4; u.hussyperfume /= 10; break;
		      	default: break;
			}
		}
		use_skill(P_SQUEAKING, rnd(40));

	} else if (Role_if(PM_HUSSY) && flags.female && u.uhs <= 0 && isok(u.ux, u.uy) && yn("Do you want to take a crap?") == 'y') {

		if (uarmu && uarmu->oartifact == ART_KATIA_S_SOFT_COTTON) {
			You("produce very erotic noises.");
			if (!rn2(10)) adjattrib(rn2(A_CHA), 1, -1, TRUE);
		} else You("grunt.");
		morehungry(rn2(400)+200);

		register struct trap *shittrap;

		if (!(t_at(u.ux, u.uy))) {

			shittrap = maketrap(u.ux, u.uy, SHIT_TRAP, 0);
			if (shittrap && !(shittrap->hiddentrap)) {
				shittrap->tseen = 1;
			}

		}
		use_skill(P_SQUEAKING, rnd(10));

		return 1;

	} else if (!PlayerCannotUseSkills && u.juyofleeing && P_SKILL(P_JUYO) >= P_BASIC && yn("Do you want to turn off the increased chance of making a monster flee?") == 'y') {
		u.juyofleeing = 0;
		pline("Monsters have the regular chance of fleeing from you now, which is useful mainly if you are a role or race that gets penalties for hitting a fleeing monster.");
		return 0;
	} else if (!PlayerCannotUseSkills && !u.juyofleeing && P_SKILL(P_JUYO) >= P_BASIC && yn("You have the Juyo skill, which allows you to make it more likely that monsters flee when you hit them. That feature is currently deactivated. Do you want to activate it?") == 'y') {
		u.juyofleeing = 1;
		pline("Monsters will now be more likely to flee from you when hit, with the chance depending on your Juyo skill!");
		return 0;

	} else if (Role_if(PM_JANITOR) && yn("Do you want to clean up the trash at your location?") == 'y') {
		register struct obj *objchain, *allchain, *blahchain;
		register int trashvalue = 0;
		char objroom;
		struct monst *shkp = (struct monst *)0;

		if (Levitation && !Race_if(PM_LEVITATOR)) {
			pline("Since you're levitating, you can't reach the trash!");
			return 0;
		} else if (u.uswallow) {
			pline("Well, it seems you have some other problem to take care of first.");
			return 0;
		} else if (u.garbagecleaned >= 1000) {
			You("already filled your trash bin! You'll have to wait until the garbage truck arrives so you can empty it.");
			/* if for some reason the garbage truck time is zero, call a truck now (shouldn't happen) --Amy */
			if (!u.garbagetrucktime) u.garbagetrucktime = rn1(500,500);

			return 0;
		}

		objchain = level.objects[u.ux][u.uy];

		for (allchain = objchain; allchain; allchain = blahchain) {

			if (u.garbagecleaned >= 1000) {
				You("filled your trash bin, and call a garbage truck that will arrive shortly.");
				u.garbagetrucktime = rn1(500,500);
				break;
			}

			blahchain = allchain->nexthere;

			/* have to special-case some stuff... --Amy
			 * iron chains and balls could be chained to you, caught by wornmask check
			 * invocation artifacts are of course immune
			 * gold is immune
			 * items of variable weight: statues, corpses and containers are also immune */
			if (allchain->owornmask) continue;
			if (evades_destruction(allchain)) continue;
			if (allchain->oclass == COIN_CLASS) continue;
			if (allchain->otyp == STATUE || allchain->otyp == CORPSE || Is_container(allchain)) continue;

			if (objects[allchain->otyp].oc_weight > 0) trashvalue = (objects[allchain->otyp].oc_weight) * allchain->quan;
			else trashvalue = allchain->quan;

			u.garbagecleaned += trashvalue;
			You("clean up %s and add %d weight units to your trash bin.", doname(allchain), trashvalue);

			objroom = *in_rooms(allchain->ox, allchain->oy, SHOPBASE);
			shkp = shop_keeper(objroom);
			if (shkp && inhishop(shkp)) {
				if (costly_spot(u.ux, u.uy) && objroom == *u.ushops) {
					Norep("You trash it, you pay for it!");
					bill_dummy_object(allchain);
				} else (void) stolen_value(allchain, allchain->ox, allchain->oy, FALSE, FALSE, FALSE);
			}

			delobj(allchain);

			if (u.garbagecleaned >= 1000) {
				You("filled your trash bin, and call a garbage truck that will arrive shortly.");
				u.garbagetrucktime = rn1(500,500);
				break;
			}

		}

	} else if (Race_if(PM_PLAYER_MUSHROOM)) {

		/* This does not consume a turn, which is intentional. --Amy */
		if (!u.mushroompoles && yn("Currently your ability to use any weapon as a polearm is deactivated. Do you want to activate it?") == 'y') {
			u.mushroompoles = TRUE;
			pline("You switch to polearm mode.");
			return 0;
		} else if (u.mushroompoles && yn("Currently your ability to use any weapon as a polearm is activated. Do you want to deactivate it?") == 'y') {
			u.mushroompoles = FALSE;
			pline("You switch to regular weapon application mode.");
			return 0;
		}

		goto mushroomannoyance;
	} else
mushroomannoyance:
	if (P_SKILL(P_MARTIAL_ARTS) >= P_UNSKILLED && P_SKILL(P_BARE_HANDED_COMBAT) >= P_UNSKILLED) {

		if (!u.disablemartial && yn("You have both the martial arts and bare-handed combat skills, which are mutually exclusive. Currently martial arts is activated. Deactivate it?") == 'y') {
			u.disablemartial = TRUE;
			pline("You switch to bare-handed combat.");
			return 1;
		}
		else if (u.disablemartial && yn("You have both the martial arts and bare-handed combat skills, which are mutually exclusive. Currently martial arts is deactivated. Activate it?") == 'y') {
			u.disablemartial = FALSE;
			pline("You switch to martial arts.");
			return 1;
		}
		goto flowannoyance;
	} else
flowannoyance:
	if (Role_if(PM_DEMAGOGUE) && !u.temprecursion && !u.demagoguerecursion && u.demagogueabilitytimer == 0 && !(In_endgame(&u.uz)) && yn("Do you want to use recursion to temporarily become someone else?") == 'y') {
		use_skill(P_SQUEAKING, rnd(20));
		u.demagogueabilitytimer = rnz(2500);
		if (!PlayerCannotUseSkills && u.demagogueabilitytimer >= 2) {
			switch (P_SKILL(P_SQUEAKING)) {
		      	case P_BASIC:	u.demagogueabilitytimer *= 9; u.demagogueabilitytimer /= 10; break;
		      	case P_SKILLED:	u.demagogueabilitytimer *= 8; u.demagogueabilitytimer /= 10; break;
		      	case P_EXPERT:	u.demagogueabilitytimer *= 7; u.demagogueabilitytimer /= 10; break;
		      	case P_MASTER:	u.demagogueabilitytimer *= 6; u.demagogueabilitytimer /= 10; break;
		      	case P_GRAND_MASTER:	u.demagogueabilitytimer *= 5; u.demagogueabilitytimer /= 10; break;
		      	case P_SUPREME_MASTER:	u.demagogueabilitytimer *= 4; u.demagogueabilitytimer /= 10; break;
		      	default: break;
			}
		}
		demagoguerecursioneffect();
	} else if (Upolyd)
		pline("Any (other) special ability you may have is purely reflexive.");
	else You("don't have another special ability in your normal form!");
	return 0;
}

int	/* deathexplore patch */
enter_explore_mode()
{
	char buf[BUFSZ];
	if(!discover && !wizard) {
		pline("Beware!  From explore mode there will be no return to normal game.");

		  getlin ("Do you want to enter explore mode? [yes/no]?",buf);
		  (void) lcase (buf);
		  if (!(strcmp (buf, "yes"))) {
			clear_nhwindow(WIN_MESSAGE);
			You("are now in non-scoring explore mode.");
			discover = TRUE;
		}
		else {
			clear_nhwindow(WIN_MESSAGE);
			pline("Resuming normal game.");
		}
	/* Amy edit: For testing purposes, allow switching to explore mode while in wizard mode.
	 * Any ascensions made that way still don't get on the high-score list, and wizard mode bones stuffing
	 * was already possible anyway, so I don't really see how this could cause any harm as long as it's
	 * not possible to switch from either wizard or explore mode to normal mode. */
	} else if (wizard) {
		getlin ("Do you want to switch to explore mode? [yes/no]?",buf);
		(void) lcase (buf);
		if (!(strcmp (buf, "yes"))) {
			clear_nhwindow(WIN_MESSAGE);
			You("are now in non-scoring explore mode.");
			discover = TRUE;
			wizard = FALSE;
		}
		else {
			clear_nhwindow(WIN_MESSAGE);
			pline("Resuming wizard mode.");
		}

	}
	return 0;
}


STATIC_PTR int
playersteal()
{

	if (MenuIsBugged) {
	pline("The borrow command is currently unavailable!");
	if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return 0;
	}

	register int x, y;
        int temp, chanch, base, dexadj, statbonus = 0;
	boolean no_steal = FALSE;

	if (nohands(youmonst.data) && !Race_if(PM_TRANSFORMER)) {
		pline("Could be hard without hands ...");
		if (yn("Attempt it anyway?") == 'y') {
			if (rn2(3) && !polyskillchance()) {
				drain_en(rnz(monster_difficulty() + 1) );
				pline("You lose  Mana");
				if (!rn2(20)) badeffect();
				return 1;
			}

		}
		else no_steal = TRUE;
	}
	if (near_capacity() > SLT_ENCUMBER) {
		Your("load is too heavy to attempt to steal.");
		no_steal = TRUE;
	}
	if (no_steal) {
		/* discard direction typeahead, if any */
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return(0);
	}

	if(!getdir(NULL)) return(0);
	if(!u.dx && !u.dy) return(0);

	x = u.ux + u.dx;
	y = u.uy + u.dy;
	
	if(u.uswallow) {
		pline("You search around but don't find anything.");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return(1);
	}

	u_wipe_engr(2);

	maploc = &levl[x][y];

	if(MON_AT(x, y)) {
		register struct monst *mdat = m_at(x, y);

		/* calculate chanch of sucess */
		base = 5;
		dexadj = 1;
		if (Role_if(PM_ROGUE)) {
			base = 5 + (u.ulevel * 2);
			dexadj = 3;
		}
		if (Role_if(PM_PICKPOCKET)) {
			base = 5 + (u.ulevel * 3);
			dexadj = 5;
		}

		if (Race_if(PM_HUMANOID_LEPRECHAUN)) {

			base += u.ulevel;
			dexadj += 1;
		}

		if (ACURR(A_DEX) < 10) statbonus = (ACURR(A_DEX) - 10) * dexadj;
		else 
		if (ACURR(A_DEX) > 14) statbonus = (ACURR(A_DEX) - 14) * dexadj;

		chanch = base + statbonus;

		if (uarmg && uarmg->otyp != GAUNTLETS_OF_DEXTERITY)
				chanch -= 5;
		if (!uarmg)     chanch += 5;
		if (uarms)      chanch -= 10;
		if (uarm && uarm->owt < 75)       chanch += 10;
		else if (uarm && uarm->owt < 125) chanch += 5;
		else if (uarm && uarm->owt < 175) chanch += 0;
		else if (uarm && uarm->owt < 225) chanch -= 5;
		else if (uarm && uarm->owt < 275) chanch -= 10;
		else if (uarm && uarm->owt < 325) chanch -= 15;
		else if (uarm && uarm->owt < 375) chanch -= 20;
		else if (uarm)                    chanch -= 25;

		if (uarmg && itemhasappearance(uarmg, APP_POLNISH_GLOVES) ) {
			if (chanch < 5) chanch = 5;
			chanch *= 2;
		}

		if (RngePoland) {
			if (chanch < 5) chanch = 5;
			chanch *= 2;
		}

		if (uarm && uarm->oartifact == ART_GREGOR_S_GANGSTER_GARMENTS) {
			if (chanch < 5) chanch = 5;
			chanch *= 2;
		}

		if (uarmg && uarmg->oartifact == ART_REALSTEAL) {
			if (chanch < 5) chanch = 5;
			chanch *= 2;
		}

		if (chanch < 5) chanch = 5;
		if (chanch > 95) chanch = 95;
		if (rnd(100) < chanch || mdat->mtame) {

#ifdef GOLDOBJ
			/* [CWC] This will steal money from the monster from the
			 * first found goldobj - we could be really clever here and
			 * then move onwards to the next goldobj in invent if we
			 * still have coins left to steal, but lets leave that until
			 * we actually have other coin types to test it on.
			 */
			struct obj *gold = findgold(mdat->minvent);
			if (gold) {
				int mongold;
				int coinstolen;
				coinstolen = (u.ulevel * rn1(25,25));
				mongold = (int)gold->quan;
				if (coinstolen > mongold) coinstolen = mongold;
				if (coinstolen > 0)	{
					if (coinstolen != mongold) 
						gold = splitobj(gold, coinstolen);
					obj_extract_self(gold);
		      if (merge_choice(invent, gold) || inv_cnt() < 52) {
				    addinv(gold);
						You("steal %s.", doname(gold));
					} else {
            You("grab %s, but find no room in your knapsack.", doname(gold));
			    	dropy(gold);
					}
				}
				else
				impossible("cmd.c:playersteal() stealing negative money");
#else
			if (mdat->mgold) {
				temp = (u.ulevel * rn1(25,25));
				if (temp > mdat->mgold) temp = mdat->mgold;
				u.ugold += temp;
				mdat->mgold -= temp;
				You("steal %d gold.",temp);
#endif
			} else
				You("don't find anything to steal.");

			if (!mdat->mtame) exercise(A_DEX, TRUE);
			return(1);
		} else {
			You("failed to steal anything.");
			setmangry(mdat);
			return(1);
	       }
	} else {
		pline("I don't see anybody to rob there!");
		return(0);
	}

	return(0);
} 

#ifdef WIZARD

/* ^W command - wish for something */
STATIC_PTR int
wiz_wish()	/* Unlimited wishes for debug mode by Paul Polderman */
{
	if (wizard) {
	    boolean save_verbose = flags.verbose;

	    flags.verbose = FALSE;
	    makewish(TRUE);
	    flags.verbose = save_verbose;
	    (void) encumber_msg();
	} else
	    pline("Unavailable command '^W'.");
	return 0;
}


#if 0	/* WAC -- Now uses techniques */
STATIC_PTR int
specialpower()      /* Special class abilites [modified by Tom] */
{
	/*
	 * STEPHEN WHITE'S NEW CODE
	 *
	 * For clarification, lastuse (as declared in decl.{c|h}) is the
	 * actual length of time the power is active, nextuse is when you can
	 * next use the ability.
	 */

        /*Added wizard mode can choose to use ability - wAC*/
	if (u.unextuse) {
	    You("have to wait %s before using your ability again.",
		(u.unextuse > 500) ? "for a while" : "a little longer");
#ifdef WIZARD
            if (!wizard || (yn("Use ability anyways?") == 'n'))
#endif
                return(0);
	}

	switch (u.role) {
	    case 'A':
/*WAC stolen from the spellcasters...'A' can identify from
        historical research*/
		if(Hallucination || Stunned || Confusion) {
		    You("can't concentrate right now!");
		    break;
		} else if((ACURR(A_INT) + ACURR(A_WIS)) < rnd(60)) {
			pline("Nothing in your pack looks familiar.");
		    u.unextuse = rn1(500,500);
		    break;
		} else if(invent) {
		    int ret;
			You("examine your possessions.");
			identify_pack(1, 0);
/*WAC this should be better - made like scroll of identify*/
/* KMH -- also commented out use of 'ret' without initialization */
/*                  ret = ggetobj("identify", identify, 1, FALSE);*/
/*		    if (ret < 0) break; *//* quit or no eligible items */
/*                  ret = ggetobj("identify", identify, 1, FALSE);*/
		} else {
			/* KMH -- fixed non-compliant string */
			You("are already quite familiar with the contents of your pack.");
		    break;
		}
		u.unextuse = rn1(500,1500);
		break;
	    case 'G':
			Your("ability, gem identification, is automatic.");
			return(0);
		case 'P':
			Your("ability, bless and curse detection, is automatic.");
			return(0);
	    case 'D':
			/* KMH -- Just do it!  (Besides, Alt isn't portable...) */
			return (polyatwill());
			/*Your("ability, polymorphing, uses the alt-y key.");*/
			/*return(0);*/
	    case 'L':
		/*only when empty handed, in human form!*/
		if (Upolyd) {
			You("can't do this while polymorphed!");
			break;
		}
		if (uwep == 0) {
			Your("fingernails extend into claws!");
			aggravate();
			u.ulastuse = d(2,4) + (u.ulevel/5) + 1; /* [max] was d(2,8) */
			u.unextuse = rn1(1000,1000); /* [max] increased delay */
		}
		else You("can't do this while holding a weapon!");
		break;
	    case 'R':
	    /* KMH -- Just do it!  (Besides, Alt isn't portable...) */
	    return (playersteal());
		/*Your("ability, stealing, uses the alt-b key.");*/
        /*return(0);*/
		break;
	    case 'M':
		Your("special ability is unarmed combat, and it is automatic.");
                return(0);
		break;
	    case 'C':
	    case 'T':
		You("don't have a special ability!");
                return(0);
		break;
	    case 'B':
		You("fly into a berserk rage!");
		u.ulastuse = d(2,8) + (u.ulevel/5) + 1;
		incr_itimeout(&HFast, u.ulastuse);
		u.unextuse = rn1(1000,500);
		return(0);
		break;
	    case 'F':
	    case 'I':
	    case 'N':
	    case 'W':
            /* WAC spell-users can study their known spells*/
		if(Hallucination || Stunned || Confusion) {
		    You("can't concentrate right now!");
		    break;
		} else {
			You("concentrate...");
			studyspell(); /*in spell.c*/
		}
		break;
	    case 'E':
                Your("%s %s become blurs as they reach for your quiver!",
			uarmg ? "gloved" : "bare",      /* Del Lamb */
			makeplural(body_part(HAND)));
                u.ulastuse = rnd((int) (u.ulevel/6 + 1)) + 1;
		u.unextuse = rn1(1000,500);
		break;
	    case 'U':
	    case 'V':
		if(!uwep || (weapon_type(uwep) == P_NONE)) {
		    You("are not wielding a weapon!");
		    break;
		} else if(uwep->known == TRUE) {
                    You("study and practice with your %s %s.",
                        uarmg ? "gloved" : "bare",      /* Del Lamb */
			makeplural(body_part(HAND)));
                    practice_weapon();
		} else {
                    if (not_fully_identified(uwep)) {
                        You("examine %s.", doname(uwep));
                            if (rnd(15) <= ACURR(A_INT)) {
                                makeknown(uwep->otyp);
                                uwep->known = TRUE;
                                You("discover it is %s",doname(uwep));
                                } else
                     pline("Unfortunately, you didn't learn anything new.");
                    } 
                /*WAC Added practicing code - in weapon.c*/
                    practice_weapon();
		}
                u.unextuse = rn1(500,500);
		break;
	    case 'H':
		if (Hallucination || Stunned || Confusion) {
		    You("are in no condition to perform surgery!");
		    break;
		}
		if ((Sick) || (Slimed)) {       /* WAC cure sliming too */
		    if(carrying(SCALPEL)) {
			pline("Using your scalpel (ow!), you cure your infection!");
			make_sick(0L,(char *)0, TRUE,SICK_ALL);
			Slimed = 0;
			if(u.uhp > 6) u.uhp -= 5;
			else          u.uhp = 1;
			u.unextuse = rn1(500,500);
			break;
		    } else pline("If only you had a scalpel...");
		}
		if (u.uhp < u.uhpmax) {
		    if(carrying(MEDICAL_KIT)) {
			pline("Using your medical kit, you bandage your wounds.");
			u.uhp += (u.ulevel * (rnd(2)+1)) + rn1(5,5);
		    } else {
			pline("You bandage your wounds as best you can.");
			u.uhp += (u.ulevel) + rn1(5,5);
		    }
		    u.unextuse = rn1(1000,500);
		    if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		} else pline("You don't need your healing powers!");
		break;
	    case 'K':
		if (u.uhp < u.uhpmax || Sick || Slimed) { /*WAC heal sliming */
			if (Sick) You("lay your hands on the foul sickness...");
			pline("A warm glow spreads through your body!");
			if (Slimed) pline_The("slime is removed.");
			Slimed = 0;
			if(Sick) make_sick(0L,(char*)0, TRUE, SICK_ALL);
			else     u.uhp += (u.ulevel * 4);
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			u.unextuse = 3000;
		} else {
			pline(nothing_happens);
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;
	    case 'S':
		You("scream \"KIIILLL!\"");
		aggravate();
		u.ulastuse = rnd((int) (u.ulevel/6 + 1)) + 1;
		u.unextuse = rn1(1000,500);
		return(0);
		break;
		case 'Y':
			if (u.usteed) {
				pline("%s gets tamer.", Monnam(u.usteed));
				tamedog(u.usteed, (struct obj *) 0, FALSE);
				u.unextuse = rn1(1000,500);
			} else
				Your("special ability is only effective when riding a monster.");
			break;
	    default:
		break;
	  }
/*By default,  action should take a turn*/
	return(1);
}
#endif


/* ^I command - identify hero's inventory */
STATIC_PTR int
wiz_identify()
{
	if (wizard)	identify_pack(0, 1);
	else		pline("Unavailable command '^I'.");
	return 0;
}

/* ^F command - reveal the level map and any traps on it */
STATIC_PTR int
wiz_map()
{

	if (wizard) {
	    struct trap *t;
	    long save_Hconf = HConfusion,
		 save_Hhallu = HHallucination;

	    HConfusion = HHallucination = 0L;
	    for (t = ftrap; t != 0; t = t->ntrap) {
		if (!t->hiddentrap) t->tseen = 1;
		if (!t->hiddentrap) map_trap(t, TRUE);
	    }
	    do_mapping();
	    HConfusion = save_Hconf;
	    HHallucination = save_Hhallu;
	} else
	    pline("Unavailable command '^F'.");
	return 0;
}

/* ^G command - generate monster(s); a count prefix will be honored */
STATIC_PTR int
wiz_gain_level()
{
	if (wizard) pluslvl(FALSE);
	else            pline("Unavailable command '^J'.");
	return 0;
}

/* BEGIN TSANTH'S CODE */
STATIC_PTR int
wiz_gain_ac()
{
	if (wizard) {
		if (u.ublessed < 20) {
			pline("Intrinsic AC increased by 1.");
			HProtection |= FROMOUTSIDE;
			u.ublessed++;
			flags.botl = 1;
		} else
			pline("Intrinsic AC is already maximized.");
	}
	else
		pline("Unavailable command '^C'.");
	return 0;
}

STATIC_PTR int
wiz_toggle_invulnerability()
{
	if (wizard) {
	    if ((Invulnerable == 0) && (u.uinvulnerable == FALSE)) {
	            You("will be invulnerable for 32000 turns.");
	            Invulnerable = 32000;
	            u.uinvulnerable = TRUE;
	    }
	    else {
	            You("are no longer invulnerable.");
	            Invulnerable = 0;
	            u.uinvulnerable = FALSE;
	    }
	}
	else            pline("Unavailable command '^N'.");
	return 0;
}
/* END TSANTH'S CODE */

STATIC_PTR int
wiz_detect_monsters()
{
	if (wizard) {
	            You("can detect monsters.");
		    incr_itimeout(&HDetect_monsters, 32000);
	}
	else            pline("Unavailable command '^H'.");
	return 0;
}

STATIC_PTR int
wiz_genesis()
{
	if (wizard)	(void) create_particular();
	else		pline("Unavailable command '^G'.");
	return 0;
}

/* ^O command - display dungeon layout */
STATIC_PTR int
wiz_where()
{
	if (wizard) (void) print_dungeon(FALSE, (schar *)0, (xchar *)0);
	else	    pline("Unavailable command '^O'.");
	return 0;
}

/* ^E command - detect unseen (secret doors, traps, hidden monsters) */
STATIC_PTR int
wiz_detect()
{
	if(wizard)  (void) findit();
	else	    pline("Unavailable command '^E'.");
	return 0;
}

/* ^V command - level teleport */
STATIC_PTR int
wiz_level_tele()
{
	if (wizard)	level_tele();
	else		pline("Unavailable command '^V'.");
	return 0;
}

/* #monpolycontrol command - choose new form for shapechangers, polymorphees */
STATIC_PTR int
wiz_mon_polycontrol()
{
    iflags.mon_polycontrol = !iflags.mon_polycontrol;
    pline("Monster polymorph control is %s.",
	  iflags.mon_polycontrol ? "on" : "off");
    return 0;
}

/* #levelchange command - adjust hero's experience level */
STATIC_PTR int
wiz_level_change()
{
    char buf[BUFSZ];
    int newlevel;
    int ret;

    getlin("To what experience level do you want to be set?", buf);
    (void)mungspaces(buf);
    if (buf[0] == '\033' || buf[0] == '\0') ret = 0;
    else ret = sscanf(buf, "%d", &newlevel);

    if (ret != 1) {
	pline("%s", Never_mind);
	if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
		pline("Oh wait, actually I do mind...");
		badeffect();
	}
	return 0;
    }
    if (newlevel == u.ulevel) {
	You("are already that experienced.");
    } else if (newlevel < u.ulevel) {
	if (u.ulevel == 1) {
	    You("are already as inexperienced as you can get.");
	    return 0;
	}
	if (newlevel < 1) newlevel = 1;
	while (u.ulevel > newlevel)
	    losexp("#levelchange", TRUE, FALSE);
    } else {
	if (u.ulevel >= MAXULEV) {
	    You("are already as experienced as you can get.");
	    return 0;
	}
	if (newlevel > MAXULEV) newlevel = MAXULEV;
	while (u.ulevel < newlevel)
	    pluslvl(FALSE);
    }
    u.ulevelmax = u.ulevel;
    return 0;
}

/* #panic command - test program's panic handling */
STATIC_PTR int
wiz_panic()
{
	if (yn("Do you want to call panic() and end your game?") == 'y')
		panic("crash test.");
        return 0;
}

/* #polyself command - change hero's form */
STATIC_PTR int
wiz_polyself()
{
        polyself(TRUE);
        return 0;
}

/* #seenv command */
STATIC_PTR int
wiz_show_seenv()
{
	winid win;
	int x, y, v, startx, stopx, curx;
	char row[COLNO+1];

	win = create_nhwindow(NHW_TEXT);
	/*
	 * Each seenv description takes up 2 characters, so center
	 * the seenv display around the hero.
	 */
	startx = max(1, u.ux-(COLNO/4));
	stopx = min(startx+(COLNO/2), COLNO);
	/* can't have a line exactly 80 chars long */
	if (stopx - startx == COLNO/2) startx++;

	for (y = 0; y < ROWNO; y++) {
	    for (x = startx, curx = 0; x < stopx; x++, curx += 2) {
		if (x == u.ux && y == u.uy) {
		    row[curx] = row[curx+1] = '@';
		} else {
		    v = levl[x][y].seenv & 0xff;
		    if (v == 0)
			row[curx] = row[curx+1] = ' ';
		    else
			sprintf(&row[curx], "%02x", v);
		}
	    }
	    /* remove trailing spaces */
	    for (x = curx-1; x >= 0; x--)
		if (row[x] != ' ') break;
	    row[x+1] = '\0';

	    putstr(win, 0, row);
	}
	display_nhwindow(win, TRUE);
	destroy_nhwindow(win);
	return 0;
}

/* #vision command */
STATIC_PTR int
wiz_show_vision()
{
	winid win;
	int x, y, v;
	char row[COLNO+1];

	win = create_nhwindow(NHW_TEXT);
	sprintf(row, "Flags: 0x%x could see, 0x%x in sight, 0x%x temp lit",
		COULD_SEE, IN_SIGHT, TEMP_LIT);
	putstr(win, 0, row);
	putstr(win, 0, "");
	for (y = 0; y < ROWNO; y++) {
	    for (x = 1; x < COLNO; x++) {
		if (x == u.ux && y == u.uy)
		    row[x] = '@';
		else {
		    v = viz_array[y][x]; /* data access should be hidden */
		    if (v == 0)
			row[x] = ' ';
		    else
			row[x] = '0' + viz_array[y][x];
		}
	    }
	    /* remove trailing spaces */
	    for (x = COLNO-1; x >= 1; x--)
		if (row[x] != ' ') break;
	    row[x+1] = '\0';

	    putstr(win, 0, &row[1]);
	}
	display_nhwindow(win, TRUE);
	destroy_nhwindow(win);
	return 0;
}

/* #wmode command */
STATIC_PTR int
wiz_show_wmodes()
{
	winid win;
	int x,y;
	char row[COLNO+1];
	struct rm *lev;

	win = create_nhwindow(NHW_TEXT);
	for (y = 0; y < ROWNO; y++) {
	    for (x = 0; x < COLNO; x++) {
		lev = &levl[x][y];
		if (x == u.ux && y == u.uy)
		    row[x] = '@';
		else if (IS_WALL(lev->typ) || lev->typ == SDOOR)
		    row[x] = '0' + (lev->wall_info & WM_MASK);
		else if (lev->typ == CORR)
		    row[x] = '#';
		else if (IS_ROOM(lev->typ) || IS_DOOR(lev->typ))
		    row[x] = '.';
		else
		    row[x] = 'x';
	    }
	    row[COLNO] = '\0';
	    putstr(win, 0, row);
	}
	display_nhwindow(win, TRUE);
	destroy_nhwindow(win);

	win = create_nhwindow(NHW_TEXT);
	for (y = 0; y < ROWNO; y++) {
	    for (x = 0; x < COLNO; x++) {
		lev = &levl[x][y];
		if (x == u.ux && y == u.uy)
		    row[x] = '@';
		else if (lev->wall_info & W_HARDGROWTH)
		    row[x] = 'H';
		else if (lev->wall_info & W_EASYGROWTH)
		    row[x] = 'E';
		else
		    row[x] = 'x';
	    }
	    row[COLNO] = '\0';
	    putstr(win, 0, row);
	}
	display_nhwindow(win, TRUE);
	destroy_nhwindow(win);

	return 0;
}

#endif /* WIZARD */


/* -enlightenment and conduct- */
static winid en_win;
static const char
	You_[] = "You ",
	are[]  = "are ",  were[]  = "were ",
	have[] = "have ", had[]   = "had ",
	can[]  = "can ",  could[] = "could ";
static const char
	have_been[]  = "have been ",
	have_never[] = "have never ", never[] = "never ";

#define enl_msg(prefix,present,past,suffix) \
			enlght_line(prefix, final ? past : present, suffix)
#define you_are(attr)	enl_msg(You_,are,were,attr)
#define you_have(attr)	enl_msg(You_,have,had,attr)
#define you_can(attr)	enl_msg(You_,can,could,attr)
#define you_have_been(goodthing) enl_msg(You_,have_been,were,goodthing)
#define you_have_never(badthing) enl_msg(You_,have_never,never,badthing)
#define you_have_X(something)	enl_msg(You_,have,(const char *)"",something)

static void
enlght_line(start, middle, end)
const char *start, *middle, *end;
{
	char buf[BUFSZ];

	sprintf(buf, "%s%s%s.", start, middle, end);
	putstr(en_win, 0, buf);
}



/* KMH, intrinsic patch -- several of these are updated */
void
enlightenment(final, guaranteed)
int final;	/* 0 => still in progress; 1 => over, survived; 2 => dead */
boolean guaranteed;
{
	int ltmp;
	char buf[BUFSZ];
	struct permonst *ptr;

	en_win = create_nhwindow(NHW_MENU);
	putstr(en_win, 0, final ? "Final Attributes:" : "Current Attributes:");
	putstr(en_win, 0, "");

	if (flags.zapem) you_are("playing in ZAPM mode");
	if (flags.wonderland) you_are("playing in wonderland mode");
	if (flags.lostsoul && !flags.uberlostsoul) you_are("playing in lost soul mode");
	if (flags.uberlostsoul) you_are("playing in uber lost soul mode");
	if (flags.gehenna) you_are("playing in gehenna mode");
	if (flags.dudley) you_are("playing in dudley mode");
	if (flags.iwbtg) you_are("playing in IWBTG mode");
	if (flags.elmstreet) you_are("playing in elm street mode");
	if (flags.blindfox) you_are("playing in blindfox mode");
	if (flags.hippie) you_are("playing in hippie mode");
	if (flags.gmmode) you_are("playing in game master mode");
	if (flags.supergmmode) you_are("playing in super game master mode");

	if ((guaranteed || !rn2(10)) && u.uevent.uhand_of_elbereth) {
	    static const char * const hofe_titles[3] = {
				"the Hand of Elbereth",
				"the Envoy of Balance",
				"the Glory of Arioch"
	    };
	    you_are(hofe_titles[u.uevent.uhand_of_elbereth - 1]);
	}

	/* note: piousness 20 matches MIN_QUEST_ALIGN (quest.h) */
	if ((guaranteed || !rn2(10)) && u.ualign.record >= 20)	you_are("piously aligned");
	else if ((guaranteed || !rn2(10)) && u.ualign.record > 13)	you_are("devoutly aligned");
	else if ((guaranteed || !rn2(10)) && u.ualign.record > 8)	you_are("fervently aligned");
	else if ((guaranteed || !rn2(10)) && u.ualign.record > 3)	you_are("stridently aligned");
	else if ((guaranteed || !rn2(10)) && u.ualign.record == 3)	you_are("aligned");
	else if ((guaranteed || !rn2(10)) && u.ualign.record > 0)	you_are("haltingly aligned");
	else if ((guaranteed || !rn2(10)) && u.ualign.record == 0)	you_are("nominally aligned");
	else if ((guaranteed || !rn2(10)) && u.ualign.record >= -3)	you_have("strayed");
	else if ((guaranteed || !rn2(10)) && u.ualign.record >= -8)	you_have("sinned");
	else if (guaranteed || !rn2(10)) you_have("transgressed");
#ifdef WIZARD
	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1) ) { /* can randomly appear with enlightenment, always after death --Amy */
		sprintf(buf, " %d", u.ualign.record);
		enl_msg("Your alignment ", "is", "was", buf);
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1) ) { /* can randomly appear with enlightenment, always after death --Amy */
		sprintf(buf, " %d", u.alignlim);
		enl_msg("Your max alignment ", "is", "was", buf);
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1) ) { /* can randomly appear with enlightenment, always after death --Amy */
		sprintf(buf, " %d sins", u.ualign.sins);
		enl_msg("You ", "carry", "carried", buf);
	}

	if ((guaranteed || !rn2(10)) && (u.impossibleproperty >= 0) && ((wizard || (!rn2(10)) || final >= 1 ) )) {
		sprintf(buf, " %s", enchname(u.impossibleproperty) );
		enl_msg("You ", "cannot have this property at all:", "could not have this property at all:", buf);
	}

	if ((guaranteed || !rn2(10)) && (u.nonextrinsicproperty >= 0 ) && ((wizard || (!rn2(10)) || final >= 1 ) )) {
		sprintf(buf, " %s", enchname(u.nonextrinsicproperty) );
		enl_msg("You ", "cannot have this property extrinsically:", "could not have this property extrinsically:", buf);
	}

	if ((guaranteed || !rn2(10)) && (u.nonintrinsicproperty >= 0 ) && ((wizard || (!rn2(10)) || final >= 1 ) )) {
		sprintf(buf, " %s", enchname(u.nonintrinsicproperty) );
		enl_msg("You ", "cannot have this property intrinsically:", "could not have this property intrinsically:", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.idscrollpenalty > 100)) {
		sprintf(buf, " %d", u.idscrollpenalty);
		enl_msg("Scroll identification only ", "works 100 times out of", "worked 100 times out of", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.idpotionpenalty > 3)) {
		sprintf(buf, " %d", u.idpotionpenalty);
		enl_msg("Potion identification only ", "works 3 times out of", "worked 3 times out of", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.idringpenalty > 4)) {
		sprintf(buf, " %d", u.idringpenalty);
		enl_msg("Ring identification only ", "works 4 times out of", "worked 4 times out of", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.idamuletpenalty > 15)) {
		sprintf(buf, " %d", u.idamuletpenalty);
		enl_msg("Amulet identification only ", "works 15 times out of", "worked 15 times out of", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.idimplantpenalty > 1)) {
		sprintf(buf, " %d", u.idimplantpenalty);
		enl_msg("Implant identification only ", "works 1 times out of", "worked 1 times out of", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.idwandpenalty > 3)) {
		sprintf(buf, " %d", u.idwandpenalty);
		enl_msg("Wand identification only ", "works 3 times out of", "worked 3 times out of", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.idarmorpenalty > 15)) {
		sprintf(buf, " %d", u.idarmorpenalty);
		enl_msg("Armor identification only ", "works 15 times out of", "worked 15 times out of", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.idspellbookpenalty > 2)) {
		sprintf(buf, " %d", u.idspellbookpenalty);
		enl_msg("Spellbook identification only ", "works 2 times out of", "worked 2 times out of", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.idgempenalty > 100)) {
		sprintf(buf, " %d", u.idgempenalty);
		enl_msg("Gem identification only ", "works 100 times out of", "worked 100 times out of", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.idtoolpenalty > 5)) {
		sprintf(buf, " %d", u.idtoolpenalty);
		enl_msg("Tool identification only ", "works 5 times out of", "worked 5 times out of", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.musableremovechance)) {
		sprintf(buf, " %d%% chance", 100 - u.musableremovechance);
		enl_msg("Monsters ", "will only drop their musable items with", "only dropped their musable items with", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.equipmentremovechance)) {
		sprintf(buf, " %d%% chance", 100 - u.equipmentremovechance);
		enl_msg("Monsters ", "will only drop their equipment with", "only dropped their equipment with", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.bookspawnchance)) {
		sprintf(buf, " %d%%", 100 - u.bookspawnchance);
		enl_msg("Book drop chance ", "is reduced to", "was reduced to", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.scrollspawnchance)) {
		sprintf(buf, " %d%%", 100 - u.scrollspawnchance);
		enl_msg("Scroll drop chance ", "is reduced to", "was reduced to", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.ringspawnchance)) {
		sprintf(buf, " %d%%", 100 - u.ringspawnchance);
		enl_msg("Ring drop chance ", "is reduced to", "was reduced to", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.wandspawnchance)) {
		sprintf(buf, " %d%%", 100 - u.wandspawnchance);
		enl_msg("Wand drop chance ", "is reduced to", "was reduced to", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.amuletspawnchance)) {
		sprintf(buf, " %d%%", 100 - u.amuletspawnchance);
		enl_msg("Amulet drop chance ", "is reduced to", "was reduced to", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.implantspawnchance)) {
		sprintf(buf, " %d%%", 100 - u.implantspawnchance);
		enl_msg("Implant drop chance ", "is reduced to", "was reduced to", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.potionspawnchance)) {
		sprintf(buf, " %d%%", 100 - u.potionspawnchance);
		enl_msg("Potion drop chance ", "is reduced to", "was reduced to", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.weaponspawnchance)) {
		sprintf(buf, " %d%%", 100 - u.weaponspawnchance);
		enl_msg("Weapon drop chance ", "is reduced to", "was reduced to", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.armorspawnchance)) {
		sprintf(buf, " %d%%", 100 - u.armorspawnchance);
		enl_msg("Armor drop chance ", "is reduced to", "was reduced to", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.toolspawnchance)) {
		sprintf(buf, " %d%%", 100 - u.toolspawnchance);
		enl_msg("Tool drop chance ", "is reduced to", "was reduced to", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.foodspawnchance)) {
		sprintf(buf, " %d%%", 100 - u.foodspawnchance);
		enl_msg("Food drop chance ", "is reduced to", "was reduced to", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.antimusablebias)) {
		sprintf(buf, " %d%%", u.antimusablebias);
		enl_msg("Musable item generation frequency ", "has a negative bias of", "had a negative bias of", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.concealitemchance)) {
		sprintf(buf, " %d%% of the time only", 100 - u.concealitemchance);
		enl_msg("Concealing monsters ", "are spawned underneath items", "were spawned underneath items", buf);
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.usefulitemchance)) {
		sprintf(buf, " %d%% chance", 100 - u.usefulitemchance);
		enl_msg("Monster death drops ", "spawn with only", "spawned with only", buf);
	}

	if ((guaranteed || !rn2(10)) && u.untrainableskill && (wizard || (!rn2(10)) || final >= 1 ) ) {
		sprintf(buf, " %s", P_NAME(u.untrainableskill));
		enl_msg("The following skill ", "cannot be trained at all:", "could not be trained at all:", buf);
	}

	if ((guaranteed || !rn2(10)) && u.halfspeedskill && (wizard || (!rn2(10)) || final >= 1 ) ) {
		sprintf(buf, " %s", P_NAME(u.halfspeedskill));
		enl_msg("The following skill ", "is trained at half the usual rate:", "was trained at half the usual rate:", buf);
	}

	if ((guaranteed || !rn2(10)) && u.fifthspeedskill && (wizard || (!rn2(10)) || final >= 1 ) ) {
		sprintf(buf, " %s", P_NAME(u.fifthspeedskill));
		enl_msg("The following skill ", "is trained at one fifth the usual rate:", "was trained at one fifth the usual rate:", buf);
	}

	if ((guaranteed || !rn2(10)) && u.basiclimitskill && (wizard || (!rn2(10)) || final >= 1 ) ) {
		sprintf(buf, " %s", P_NAME(u.basiclimitskill));
		enl_msg("The following skill ", "is limited to basic proficiency:", "was limited to basic proficiency:", buf);
	}

	if ((guaranteed || !rn2(10)) && u.skilledlimitskill && (wizard || (!rn2(10)) || final >= 1 ) ) {
		sprintf(buf, " %s", P_NAME(u.skilledlimitskill));
		enl_msg("The following skill ", "is limited to skilled proficiency:", "was limited to skilled proficiency:", buf);
	}

	if ((guaranteed || !rn2(10)) && u.expertlimitskill && (wizard || (!rn2(10)) || final >= 1 ) ) {
		sprintf(buf, " %s", P_NAME(u.expertlimitskill));
		enl_msg("The following skill ", "is limited to expert proficiency:", "was limited to expert proficiency:", buf);
	}

	if ((guaranteed || !rn2(10)) && u.earlytrainingskill && (wizard || (!rn2(10)) || final >= 1 ) ) {
		sprintf(buf, " %s (turn %d)", P_NAME(u.earlytrainingskill), u.earlytrainingtimer);
		enl_msg("The following skill ", "becomes untrainable if you try to train it too early:", "became untrainable if you tried to train it too early:", buf);
		if (u.earlytrainingblown) {
			sprintf(buf, "blown your chance of training the %s skill", P_NAME(u.earlytrainingskill));
			you_have(buf);
		}
	}

	if ((guaranteed || !rn2(10)) && u.frtrainingskill && (wizard || (!rn2(10)) || final >= 1 ) ) {
		sprintf(buf, " %s (turn %d)", P_NAME(u.frtrainingskill), u.frtrainingtimer);
		enl_msg("The following skill ", "becomes untrainable if you try to train it too early:", "became untrainable if you tried to train it too early:", buf);
		if (u.frtrainingblown) {
			sprintf(buf, "blown your chance of training the %s skill", P_NAME(u.frtrainingskill));
			you_have(buf);
		}
	}

	if ((guaranteed || !rn2(10)) && u.latetrainingskill && (wizard || (!rn2(10)) || final >= 1 ) ) {
		sprintf(buf, " %s (turn %d)", P_NAME(u.latetrainingskill), u.latetrainingtimer);
		enl_msg("The following skill ", "becomes untrainable after a while:", "became untrainable after a while:", buf);
	}

	if ((guaranteed || !rn2(10)) && u.lavtrainingskill && (wizard || (!rn2(10)) || final >= 1 ) ) {
		sprintf(buf, " %s (turn %d)", P_NAME(u.lavtrainingskill), u.lavtrainingtimer);
		enl_msg("The following skill ", "can't be trained before a certain turn number is reached:", "couldn't be trained before a certain turn number is reached:", buf);
	}

	if ((guaranteed || !rn2(10)) && u.slowtrainingskill && (wizard || (!rn2(10)) || final >= 1 ) ) {
		sprintf(buf, " %s", P_NAME(u.slowtrainingskill));
		enl_msg("The following skill ", "becomes harder to train at higher skill levels:", "became harder to train at higher skill levels:", buf);
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 ) && u.stickycursechance ) {
		sprintf(buf, " %d%%", u.stickycursechance);
		enl_msg("Sticky curses ", "are more common by", "were more common by", buf);
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 ) && u.heavycursechance) {
		sprintf(buf, " %d%%", u.heavycursechance);
		enl_msg("Heavy curses ", "are more common by", "were more common by", buf);
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 ) && u.primecursechance) {
		sprintf(buf, " %d%%", u.primecursechance);
		enl_msg("Prime curses ", "are more common by", "were more common by", buf);
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 ) && u.genericcursechance) {
		sprintf(buf, " %d%%", u.genericcursechance);
		enl_msg("Items ", "are more likely to be generated cursed by", "were more likely to be generated cursed by", buf);
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) { sprintf(buf, " %d", nartifact_exist() );
		enl_msg("Number of artifacts generated ", "is", "was", buf);
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) { sprintf(buf, " %d", u.fakeartifacts );
		enl_msg("Number of fake artifacts generated ", "is", "was", buf);
	}

	if ((guaranteed || !rn2(10)) && u.legscratching > 1) { sprintf(buf, " %d", u.legscratching - 1);
		enl_msg("Your leg damage ", "is", "was", buf);
	}

	if ((guaranteed || !rn2(10)) && u.youpolyamount > 0) { sprintf(buf, " %d", u.youpolyamount);
		enl_msg("Your remaining amount of free polymorphs ", "is", "was", buf);
	}

	if ((guaranteed || !rn2(10)) && u.antimagicshell > 0) { sprintf(buf, " %d more turns", u.antimagicshell);
		enl_msg("Your antimagic shell ", "will last", "would have lasted", buf);
	}

	if ((guaranteed || !rn2(10)) && Role_if(PM_UNBELIEVER)) {
		you_are("producing a permanent antimagic shell");
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) { sprintf(buf, " turn %d", u.monstertimeout);
		enl_msg("Monster spawn increase ", "start at", "would have started at", buf);
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {sprintf(buf, " turn %d", u.monstertimefinish);
		enl_msg("Monster spawn increase ", "reaches its maximum at", "had reached its maximum at", buf);
		enl_msg("In this game, Eevee's evolution ", "is ", "was ", mons[u.eeveelution].mname );
		/* these two are tied together because the monstertimefinish variable defines the evolution --Amy */
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) { sprintf(buf, " turn %d", u.ascensiontimelimit);
		enl_msg("Your limit for ascension ", "is at", "was at", buf);
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		enl_msg("The monster class that cannot be genocided ", "is ", "was ", monexplain[u.ungenocidable] );
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 )) && u.alwaysinvisible) {
		enl_msg("Today, the monster class that always gets generated invisible ", "is ", "was ", monexplain[u.alwaysinvisible] );
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 )) && u.alwayshidden) {
		enl_msg("Today, the monster class that always gets generated hidden ", "is ", "was ", monexplain[u.alwayshidden] );
	}

	if ((guaranteed || !rn2(10)) && (u.freqmonsterbonus && (wizard || (!rn2(10)) || final >= 1 ))) {
		sprintf(buf, "spawned more often: %s (freq bonus %d)", monexplain[u.frequentmonster], u.freqmonsterbonus);
		enl_msg("The monster class that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (u.freqcolorbonus && (wizard || (!rn2(10)) || final >= 1 ))) {
		sprintf(buf, "spawned more often: %s (freq bonus %d)", c_obj_colors[u.frequentcolor], u.freqcolorbonus);
		enl_msg("The monster color that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies];
		sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies2];
		sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus2);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies3];
		sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus3);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies4];
		sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus4);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies5];
		sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus5);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies6];
		sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus6);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies7];
		sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus7);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies8];
		sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus8);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies9];
		sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus9);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies10];
		sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus10);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies11];
		sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus11);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies12];
		sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus12);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies13];
		sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus13);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies14];
		sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus14);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies15];
		sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus15);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies16];
		sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus16);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies17];
		sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus17);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies18];
		sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus18);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies19];
		sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus19);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.frequentspecies20];
		sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus20);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "more common: %s (freq bonus %d)", montraitname(u.frequenttrait1), u.freqtraitbonus1);
		enl_msg("The monster trait that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && u.frequenttrait2 && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "more common: %s (freq bonus %d)", montraitname(u.frequenttrait2), u.freqtraitbonus2);
		enl_msg("The monster trait that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && u.frequenttrait3 && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "more common: %s (freq bonus %d)", montraitname(u.frequenttrait3), u.freqtraitbonus3);
		enl_msg("The monster trait that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && u.frequenttrait4 && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "more common: %s (freq bonus %d)", montraitname(u.frequenttrait4), u.freqtraitbonus4);
		enl_msg("The monster trait that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && u.frequenttrait5 && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "more common: %s (freq bonus %d)", montraitname(u.frequenttrait5), u.freqtraitbonus5);
		enl_msg("The monster trait that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && u.frequenttrait6 && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "more common: %s (freq bonus %d)", montraitname(u.frequenttrait6), u.freqtraitbonus6);
		enl_msg("The monster trait that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && u.frequenttrait7 && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "more common: %s (freq bonus %d)", montraitname(u.frequenttrait7), u.freqtraitbonus7);
		enl_msg("The monster trait that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && u.frequenttrait8 && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "more common: %s (freq bonus %d)", montraitname(u.frequenttrait8), u.freqtraitbonus8);
		enl_msg("The monster trait that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && u.frequenttrait9 && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "more common: %s (freq bonus %d)", montraitname(u.frequenttrait9), u.freqtraitbonus9);
		enl_msg("The monster trait that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && u.frequenttrait10 && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "more common: %s (freq bonus %d)", montraitname(u.frequenttrait10), u.freqtraitbonus10);
		enl_msg("The monster trait that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.nospawnspecies];
		sprintf(buf, "never randomly spawned: %s", ptr->mname);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.nospawnspecies2];
		sprintf(buf, "never randomly spawned: %s", ptr->mname);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.nospawnspecies3];
		sprintf(buf, "never randomly spawned: %s", ptr->mname);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.nospawnspecies4];
		sprintf(buf, "never randomly spawned: %s", ptr->mname);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.nospawnspecies5];
		sprintf(buf, "never randomly spawned: %s", ptr->mname);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.nospawnspecies6];
		sprintf(buf, "never randomly spawned: %s", ptr->mname);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.nospawnspecies7];
		sprintf(buf, "never randomly spawned: %s", ptr->mname);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.nospawnspecies8];
		sprintf(buf, "never randomly spawned: %s", ptr->mname);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.nospawnspecies9];
		sprintf(buf, "never randomly spawned: %s", ptr->mname);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		ptr = &mons[u.nospawnspecies10];
		sprintf(buf, "never randomly spawned: %s", ptr->mname);
		enl_msg("The monster species that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.speedymonster)) {
		sprintf(buf, "always generated with extra speed: %s", monexplain[u.speedymonster]);
		enl_msg("The monster class that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && u.musemonster)) {
		sprintf(buf, "more likely to generate with musable stuff: %s", monexplain[u.musemonster]);
		enl_msg("The monster class that ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && u.minimalism) {sprintf(buf, " %d", u.minimalism);
		enl_msg("Items generate only 1 time in X, and X ", "is", "was", buf);
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable].oc_name, obj_descr[u.unobtainable].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable2].oc_name, obj_descr[u.unobtainable2].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable3].oc_name, obj_descr[u.unobtainable3].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable4].oc_name, obj_descr[u.unobtainable4].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable5].oc_name, obj_descr[u.unobtainable5].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable6].oc_name, obj_descr[u.unobtainable6].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable7].oc_name, obj_descr[u.unobtainable7].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable8].oc_name, obj_descr[u.unobtainable8].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable9].oc_name, obj_descr[u.unobtainable9].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable10].oc_name, obj_descr[u.unobtainable10].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && ((wizard || (!rn2(10)) || final >= 1 ) && (u.unobtainablegeno != -1) )) {
		sprintf(buf, "genocided: %s (%s)", obj_descr[u.unobtainablegeno].oc_name, obj_descr[u.unobtainablegeno].oc_descr);
		enl_msg("This item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable].oc_name, obj_descr[u.veryobtainable].oc_descr, u.veryobtainableboost);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable2].oc_name, obj_descr[u.veryobtainable2].oc_descr, u.veryobtainableboost2);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable3].oc_name, obj_descr[u.veryobtainable3].oc_descr, u.veryobtainableboost3);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable4].oc_name, obj_descr[u.veryobtainable4].oc_descr, u.veryobtainableboost4);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable5].oc_name, obj_descr[u.veryobtainable5].oc_descr, u.veryobtainableboost5);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable6].oc_name, obj_descr[u.veryobtainable6].oc_descr, u.veryobtainableboost6);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable7].oc_name, obj_descr[u.veryobtainable7].oc_descr, u.veryobtainableboost7);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable8].oc_name, obj_descr[u.veryobtainable8].oc_descr, u.veryobtainableboost8);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable9].oc_name, obj_descr[u.veryobtainable9].oc_descr, u.veryobtainableboost9);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable10].oc_name, obj_descr[u.veryobtainable10].oc_descr, u.veryobtainableboost10);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable11].oc_name, obj_descr[u.veryobtainable11].oc_descr, u.veryobtainableboost11);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable12].oc_name, obj_descr[u.veryobtainable12].oc_descr, u.veryobtainableboost12);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable13].oc_name, obj_descr[u.veryobtainable13].oc_descr, u.veryobtainableboost13);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable14].oc_name, obj_descr[u.veryobtainable14].oc_descr, u.veryobtainableboost14);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable15].oc_name, obj_descr[u.veryobtainable15].oc_descr, u.veryobtainableboost15);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable16].oc_name, obj_descr[u.veryobtainable16].oc_descr, u.veryobtainableboost16);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable17].oc_name, obj_descr[u.veryobtainable17].oc_descr, u.veryobtainableboost17);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable18].oc_name, obj_descr[u.veryobtainable18].oc_descr, u.veryobtainableboost18);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable19].oc_name, obj_descr[u.veryobtainable19].oc_descr, u.veryobtainableboost19);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable20].oc_name, obj_descr[u.veryobtainable20].oc_descr, u.veryobtainableboost20);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 ) && (u.veryobtainclass1 > 0) ) {
		sprintf(buf, "generated more often: %s (freq bonus %d)", names[u.veryobtainclass1], u.veryobtainclass1boost);
		enl_msg("The RNG hath decreed that this item class ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 ) && (u.veryobtainclass2 > 0) ) {
		sprintf(buf, "generated more often: %s (freq bonus %d)", names[u.veryobtainclass2], u.veryobtainclass2boost);
		enl_msg("The RNG hath decreed that this item class ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 ) && (u.veryobtainclass3 > 0) ) {
		sprintf(buf, "generated more often: %s (freq bonus %d)", names[u.veryobtainclass3], u.veryobtainclass3boost);
		enl_msg("The RNG hath decreed that this item class ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem].oc_name, obj_descr[u.alwayscurseditem].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem2].oc_name, obj_descr[u.alwayscurseditem2].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem3].oc_name, obj_descr[u.alwayscurseditem3].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem4].oc_name, obj_descr[u.alwayscurseditem4].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem5].oc_name, obj_descr[u.alwayscurseditem5].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem6].oc_name, obj_descr[u.alwayscurseditem6].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem7].oc_name, obj_descr[u.alwayscurseditem7].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem8].oc_name, obj_descr[u.alwayscurseditem8].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem9].oc_name, obj_descr[u.alwayscurseditem9].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem10].oc_name, obj_descr[u.alwayscurseditem10].oc_descr);
		enl_msg("The RNG hath decreed that this item ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "always invisible: %s", defsyms[trap_to_defsym(u.invisotrap)].explanation);
		enl_msg("The RNG hath decreed that this trap ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "generated more often: %s (freq bonus %d)", defsyms[trap_to_defsym(u.frequenttrap)].explanation, u.freqtrapbonus);
		enl_msg("The RNG hath decreed that this trap ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "for 1 in %d traps", u.traprandomizing);
		enl_msg("Trap randomization ", "happens ", "happened ", buf );
	}

	if ((guaranteed || !rn2(10)) && u.trapxtradiff && (wizard || (!rn2(10)) || final >= 1 )) {
		sprintf(buf, "increased by %d", u.trapxtradiff);
		enl_msg("Trap difficulty ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {sprintf(buf, " turn %d", u.next_check);
		enl_msg("Next attribute increase check ", "comes at", "would have come at", buf);
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {sprintf(buf, " %d", AEXE(A_STR));
		enl_msg("Strength training ", "is", "was", buf);
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {sprintf(buf, " %d", AEXE(A_DEX));
		enl_msg("Dexterity training ", "is", "was", buf);
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {sprintf(buf, " %d", AEXE(A_WIS));
		enl_msg("Wisdom training ", "is", "was", buf);
	}

	if ((guaranteed || !rn2(10)) && (wizard || (!rn2(10)) || final >= 1 )) {sprintf(buf, " %d", AEXE(A_CON));
		enl_msg("Constitution training ", "is", "was", buf);
	}
#endif

	if (guaranteed || !rn2(10)) {sprintf(buf, " %d", u.usanity);
		enl_msg("Your sanity ", "is", "was", buf);
	}

	if ((guaranteed || !rn2(10)) && u.negativeprotection) {sprintf(buf, " %d", u.negativeprotection);
		enl_msg("Your protection was reduced. The amount ", "is", "was", buf);
	}

	if ((guaranteed || !rn2(10)) && u.tremblingamount) {sprintf(buf, " %d", u.tremblingamount);
		enl_msg("You're trembling, which reduces your to-hit and spellcasting chances. The amount ", "is", "was", buf);
	}

	if ((guaranteed || !rn2(10)) && u.extralives == 1) {
		you_have("an extra life");
	}

	if ((guaranteed || !rn2(10)) && u.extralives > 1) {sprintf(buf, " %d", u.extralives);
		enl_msg("Your amount of extra lives ", "is", "was", buf);
	}

	if ((guaranteed || !rn2(10)) && u.chokhmahdamage) {sprintf(buf, " %d", u.chokhmahdamage);
		enl_msg("The escalating chokhmah attack damage ", "is", "was", buf);
	}

	/*** Resistances to troubles ***/
	if ((guaranteed || !rn2(10)) && Fire_resistance) you_are(StrongFire_resistance ? "doubly fire resistant" : "fire resistant");
	if ((guaranteed || !rn2(10)) && Cold_resistance) you_are(StrongCold_resistance ? "doubly cold resistant" : "cold resistant");
	if ((guaranteed || !rn2(10)) && Sleep_resistance) you_are(StrongSleep_resistance ? "doubly sleep resistant" : "sleep resistant");
	if ((guaranteed || !rn2(10)) && Disint_resistance) you_are(StrongDisint_resistance ? "doubly disintegration-resistant" : "disintegration-resistant");
	if ((guaranteed || !rn2(10)) && Shock_resistance) you_are(StrongShock_resistance ? "doubly shock resistant" : "shock resistant");
	if ((guaranteed || !rn2(10)) && Poison_resistance) you_are(StrongPoison_resistance ? "doubly poison resistant" : "poison resistant");
	if ((guaranteed || !rn2(10)) && Drain_resistance) you_are(StrongDrain_resistance ? "doubly level-drain resistant" : "level-drain resistant");
	if ((guaranteed || !rn2(10)) && Sick_resistance) you_are(StrongSick_resistance ? "completely immune to sickness" : IntSick_resistance ? "immune to sickness" : "resistant to sickness");
	if ((guaranteed || !rn2(10)) && Antimagic) you_are(StrongAntimagic ? "doubly magic-protected" : "magic-protected");
	if ((guaranteed || !rn2(10)) && Acid_resistance) you_are(StrongAcid_resistance ? "doubly acid resistant" : "acid resistant");
	if ((guaranteed || !rn2(10)) && Fear_resistance) you_are(StrongFear_resistance ? "highly resistant to fear" : "resistant to fear");
	if ((guaranteed || !rn2(10)) && Stone_resistance)
		you_are(StrongStone_resistance ? "completely immune to petrification" : IntStone_resistance ? "immune to petrification" : "petrification resistant");
	if ((guaranteed || !rn2(10)) && Invulnerable) {
		sprintf(buf, "invulnerable");
	    if (wizard || (!rn2(10)) || final >= 1  ) sprintf(eos(buf), " (%d)", Invulnerable);
		you_are(buf);
	}
	/*if (u.urealedibility) you_can("recognize detrimental food");*/

	if ((guaranteed || !rn2(10)) && u.urealedibility) {
		sprintf(buf, "recognize detrimental food");
	   sprintf(eos(buf), " (%d)", u.urealedibility);
		you_can(buf);
	}

	/*** Troubles ***/

	if ((guaranteed || !rn2(10)) && multi) {
		sprintf(buf, "paralyzed.");
	      sprintf(eos(buf), " (%d)", multi);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && Halluc_resistance)
		enl_msg("You resist", "", "ed", " hallucinations");
	/*if (final) { */
	if ((guaranteed || !rn2(10)) && Hallucination) {
		if (HeavyHallu) sprintf(buf, "badly hallucinating");
		else sprintf(buf, "hallucinating");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", HHallucination);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && Stunned) {
		if (HeavyStunned) sprintf(buf, "badly stunned");
		else sprintf(buf, "stunned");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", HStun);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && Confusion) {
		if (HeavyConfusion) sprintf(buf, "badly confused");
		else sprintf(buf, "confused");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", HConfusion);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && Blind) {
		if (HeavyBlind) sprintf(buf, "badly blinded");
		else sprintf(buf, "blinded");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", Blinded);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && Sick) {
		sprintf(buf, "sick");
			if (u.usick_type & SICK_VOMITABLE) sprintf(eos(buf), " from food poisoning");
			if (u.usick_type & SICK_NONVOMITABLE) sprintf(eos(buf), " from illness");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", Sick);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && Vomiting) {
		sprintf(buf, "nauseated");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", Vomiting);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && Feared) {
		if (HeavyFeared) sprintf(buf, "stricken with very strong fear");
		else sprintf(buf, "stricken with fear");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", HFeared);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && Numbed) {
		if (HeavyNumbed) sprintf(buf, "badly numbed");
		else sprintf(buf, "numbed");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", HNumbed);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && Frozen) {
		if (HeavyFrozen) sprintf(buf, "frozen rigid and solid");
		else sprintf(buf, "frozen solid");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", HFrozen);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && u.hanguppenalty) {
		sprintf(buf, "temporarily slower because you tried to hangup cheat");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.hanguppenalty);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && Burned) {
		if (HeavyBurned) sprintf(buf, "badly burned");
		else sprintf(buf, "burned");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", HBurned);
		you_are(buf);
	}
		
	if ((guaranteed || !rn2(10)) && Dimmed) {
		if (HeavyDimmed) sprintf(buf, "badly dimmed");
		else sprintf(buf, "dimmed");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", HDimmed);
		you_are(buf);
	}
		
        if ((guaranteed || !rn2(10)) && Punished) {
            you_are("punished");
        }

        if ((guaranteed || !rn2(10)) && u.totter) {
            you_have("inverted directional keys");
        }

        if ((guaranteed || !rn2(10)) && sengr_at("Elbereth", u.ux, u.uy) ) {
            you_are("standing on an active Elbereth engraving");
        }

	if ((guaranteed || !rn2(10)) && Stoned) {
		sprintf(buf, "turning to stone");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", Stoned);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && Slimed) {
		sprintf(buf, "turning into slime");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", Slimed);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && Strangled) {
		sprintf(buf, (u.uburied) ? "buried" : "being strangled");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", Strangled);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && PlayerBleeds) {
		sprintf(buf, "bleeding");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", PlayerBleeds);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && Prem_death) {
		sprintf(buf, "going to die prematurely");
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && IsGlib) {
		sprintf(buf, "slippery %s", makeplural(body_part(FINGER)));
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", Glib);
		you_have(buf);
	}


	if ((guaranteed || !rn2(10)) && (MenuBug || u.uprops[MENU_LOST].extrinsic || have_menubugstone())) {
		sprintf(buf, "the menu bug.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", MenuBug);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (SpeedBug || u.uprops[SPEED_BUG].extrinsic || have_speedbugstone())) {
		sprintf(buf, "the speed bug.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", SpeedBug);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (Superscroller || u.uprops[SUPERSCROLLER_ACTIVE].extrinsic || have_superscrollerstone())) {
		sprintf(buf, "the superscroller.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", Superscroller);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (RMBLoss || u.uprops[RMB_LOST].extrinsic || have_rmbstone())) {
		sprintf(buf, "the following problem: Your right mouse button failed.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", RMBLoss);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (DisplayLoss || u.uprops[DISPLAY_LOST].extrinsic || have_displaystone())) {
		sprintf(buf, "the following problem: Your display failed.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", DisplayLoss);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (SpellLoss || u.uprops[SPELLS_LOST].extrinsic || have_spelllossstone())) {
		sprintf(buf, "the following problem: You lost spells.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", SpellLoss);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (YellowSpells || u.uprops[YELLOW_SPELLS].extrinsic || have_yellowspellstone())) {
		sprintf(buf, "the following problem: Your spells became yellow.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", YellowSpells);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (AutoDestruct || u.uprops[AUTO_DESTRUCT].extrinsic || have_autodestructstone())) {
		sprintf(buf, "the following problem: An auto destruct mechanism was initiated.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", AutoDestruct);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (MemoryLoss || u.uprops[MEMORY_LOST].extrinsic || have_memorylossstone())) {
		sprintf(buf, "the following problem: There is low local memory.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", MemoryLoss);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (InventoryLoss || u.uprops[INVENTORY_LOST].extrinsic || have_inventorylossstone())) {
		sprintf(buf, "the following problem: There is not enough memory to create an inventory window.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", InventoryLoss);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (BlackNgWalls || u.uprops[BLACK_NG_WALLS].extrinsic || have_blackystone())) {
		sprintf(buf, "the following problem: Blacky and his NG walls are closing in.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", BlackNgWalls);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (FreeHandLoss || u.uprops[FREE_HAND_LOST].extrinsic || have_freehandbugstone())) {
		sprintf(buf, "the following problem: Your free hand is less likely to be free.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FreeHandLoss);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (Unidentify || u.uprops[UNIDENTIFY].extrinsic || have_unidentifystone())) {
		sprintf(buf, "the following problem: Your possessions sometimes unidentify themselves.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", Unidentify);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (Thirst || u.uprops[THIRST].extrinsic || have_thirststone())) {
		sprintf(buf, "a strong sense of thirst.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", Thirst);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (LuckLoss || u.uprops[LUCK_LOSS].extrinsic || have_unluckystone())) {
		sprintf(buf, "the following problem: You're shitting out of luck (SOL).");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", LuckLoss);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (ShadesOfGrey || u.uprops[SHADES_OF_GREY].extrinsic || have_shadesofgreystone())) {
		sprintf(buf, "the following problem: Everything displays in various shades of grey.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", ShadesOfGrey);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (FaintActive || u.uprops[FAINT_ACTIVE].extrinsic || have_faintingstone())) {
		sprintf(buf, "the following problem: You randomly faint.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FaintActive);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (Itemcursing || u.uprops[ITEMCURSING].extrinsic || have_cursingstone() || have_primecurse() )) {
		sprintf(buf, "the following problem: Your inventory gradually fills up with cursed items.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", Itemcursing);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && have_morgothiancurse() ) {
		sprintf(buf, "afflicted by the Ancient Morgothian Curse.");
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && have_topiylinencurse() ) {
		sprintf(buf, "afflicted by Topi Ylinen's Curse, a.k.a. the Ancient Foul Curse.");
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && have_blackbreathcurse() ) {
		sprintf(buf, "filled with the Black Breath.");
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (DifficultyIncreased || u.uprops[DIFFICULTY_INCREASED].extrinsic || have_difficultystone())) {
		sprintf(buf, "the following problem: The difficulty of the game was arbitrarily increased.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", DifficultyIncreased);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (Deafness || u.uprops[DEAFNESS].extrinsic || have_deafnessstone())) {
		sprintf(buf, "a hearing break.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", Deafness);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (CasterProblem || u.uprops[CASTER_PROBLEM].extrinsic || have_antimagicstone())) {
		sprintf(buf, "blood mana.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", CasterProblem);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (WeaknessProblem || u.uprops[WEAKNESS_PROBLEM].extrinsic || have_weaknessstone())) {
		sprintf(buf, "the following problem: Being weak from hunger damages your health.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", WeaknessProblem);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (RotThirteen || u.uprops[ROT_THIRTEEN].extrinsic || have_rotthirteenstone())) {
		sprintf(buf, "the following problem: A rot13 cypher has been activated for lowercase letters.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", RotThirteen);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (BishopGridbug || u.uprops[BISHOP_GRIDBUG].extrinsic || have_bishopstone())) {
		sprintf(buf, "the following problem: You cannot move diagonally.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", BishopGridbug);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (ConfusionProblem || u.uprops[CONFUSION_PROBLEM].extrinsic || have_confusionstone())) {
		sprintf(buf, "a confusing problem.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", ConfusionProblem);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (NoDropProblem || u.uprops[DROP_BUG].extrinsic || have_dropbugstone())) {
		sprintf(buf, "the following problem: You cannot drop items.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", NoDropProblem);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (DSTWProblem || u.uprops[DSTW_BUG].extrinsic || have_dstwstone())) {
		sprintf(buf, "the following problem: Your potions don't always work.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", DSTWProblem);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (StatusTrapProblem || u.uprops[STATUS_FAILURE].extrinsic || have_statusstone())) {
		sprintf(buf, "the following problem: You can't cure status effects.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", StatusTrapProblem);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (AlignmentProblem || u.uprops[ALIGNMENT_FAILURE].extrinsic || have_alignmentstone())) {
		sprintf(buf, "the following problem: Your maximum alignment will decrease over time.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", AlignmentProblem);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (StairsProblem || u.uprops[STAIRSTRAP].extrinsic || have_stairstrapstone())) {
		sprintf(buf, "the following problem: Stairs are always trapped.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", StairsProblem);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (UninformationProblem || u.uprops[UNINFORMATION].extrinsic || have_uninformationstone())) {
		sprintf(buf, "the following problem: The game gives insufficient amounts of information.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", UninformationProblem);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (TimerunBug || u.uprops[TIMERUN_BUG].extrinsic || have_timerunstone())) {
		sprintf(buf, "the following problem: All actions take turns.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", TimerunBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (SanityTrebleEffect || u.uprops[SANITY_TREBLE_EFFECT].extrinsic || have_sanitytreblestone())) {
		sprintf(buf, "the following problem: Sanity effects will increase your sanity by a much higher amount than usual.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", SanityTrebleEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (StatDecreaseBug || u.uprops[STAT_DECREASE_BUG].extrinsic || have_statdecreasestone())) {
		sprintf(buf, "the following problem: The soft cap for your attributes is much lower.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", StatDecreaseBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (SimeoutBug || u.uprops[SIMEOUT_BUG].extrinsic || have_simeoutstone())) {
		sprintf(buf, "the following problem: Your sanity slowly increases over time.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", SimeoutBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (BadPartBug || u.uprops[BAD_PARTS].extrinsic || have_badpartstone())) {
		sprintf(buf, "in the bad part.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", BadPartBug);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (CompletelyBadPartBug || u.uprops[COMPLETELY_BAD_PARTS].extrinsic || have_completelybadpartstone())) {
		sprintf(buf, "in the completely bad part.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", CompletelyBadPartBug);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (EvilVariantActive || u.uprops[EVIL_VARIANT_ACTIVE].extrinsic || have_evilvariantstone())) {
		sprintf(buf, "the following problem: You're forced to play the evil variant.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", EvilVariantActive);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (OrangeSpells || u.uprops[ORANGE_SPELLS].extrinsic || have_orangespellstone())) {
		sprintf(buf, "the following problem: Your spells became orange.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", OrangeSpells);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (VioletSpells || u.uprops[VIOLET_SPELLS].extrinsic || have_violetspellstone())) {
		sprintf(buf, "the following problem: Your spells became violet.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", VioletSpells);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (LongingEffect || u.uprops[LONGING_EFFECT].extrinsic || have_longingstone())) {
		sprintf(buf, "the following problem: The game bombards you with 'sexy' messages.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", LongingEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (CursedParts || u.uprops[CURSED_PARTS].extrinsic || have_cursedpartstone())) {
		sprintf(buf, "the following problem: Sometimes you're forced to wear cursed equipment.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", CursedParts);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (Quaversal || u.uprops[QUAVERSAL].extrinsic || have_quaversalstone())) {
		sprintf(buf, "the following problem: Unless you're standing on specific tiles, the entire screen is black.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", Quaversal);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (AppearanceShuffling || u.uprops[APPEARANCE_SHUFFLING].extrinsic || have_appearanceshufflingstone())) {
		sprintf(buf, "the following problem: The randomized appearances of items are occasionally shuffled.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", AppearanceShuffling);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (BrownSpells || u.uprops[BROWN_SPELLS].extrinsic || have_brownspellstone())) {
		sprintf(buf, "the following problem: Your spells became brown.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", BrownSpells);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (Choicelessness || u.uprops[CHOICELESSNESS].extrinsic || have_choicelessstone())) {
		sprintf(buf, "the following problem: Direction prompts often don't allow you to select anything.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", Choicelessness);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (Goldspells || u.uprops[GOLDSPELLS].extrinsic || have_goldspellstone())) {
		sprintf(buf, "the following problem: You often can't choose which spell you want to cast.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", Goldspells);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (Deprovement || u.uprops[DEPROVEMENT].extrinsic || have_deprovementstone())) {
		sprintf(buf, "the following problem: Leveling up no longer increases your maximum HP and Pw.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", Deprovement);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (InitializationFail || u.uprops[INITIALIZATION_FAIL].extrinsic || have_initializationstone())) {
		sprintf(buf, "the following problem: Prompts that let you choose an item will not appear.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", InitializationFail);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (GushlushEffect || u.uprops[GUSHLUSH].extrinsic || have_gushlushstone())) {
		sprintf(buf, "the following problem: To-hit, spellcasting chances and some other stuff always assume that you're experience level 1.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", GushlushEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (SoiltypeEffect || u.uprops[SOILTYPE].extrinsic || have_soiltypestone())) {
		sprintf(buf, "the following problem: You trigger ground-based traps even while flying or levitating.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", SoiltypeEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (DangerousTerrains || u.uprops[DANGEROUS_TERRAINS].extrinsic || have_dangerousterrainstone())) {
		sprintf(buf, "the following problem: Standing next to dangerous terrain may confuse or stun you with no warning.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", DangerousTerrains);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (FalloutEffect || u.uprops[FALLOUT_EFFECT].extrinsic || have_falloutstone())) {
		sprintf(buf, "the following problem: Your contamination increases over time.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FalloutEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (MojibakeEffect || u.uprops[MOJIBAKE].extrinsic || have_mojibakestone())) {
		sprintf(buf, "the following problem: Some glyphs are randomized.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", MojibakeEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (GravationEffect || u.uprops[GRAVATION].extrinsic || have_gravationstone())) {
		sprintf(buf, "the following problem: Random monster spawns use aggravate monster rules.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", GravationEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (UncalledEffect || u.uprops[UNCALLED_EFFECT].extrinsic || have_uncalledstone())) {
		sprintf(buf, "the following problem: You can't give names to items or monsters.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", UncalledEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (ExplodingDiceEffect || u.uprops[EXPLODING_DICE].extrinsic || have_explodingdicestone())) {
		sprintf(buf, "the following problem: Monster attacks use exploding dice for damage calculations.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", ExplodingDiceEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (PermacurseEffect || u.uprops[PERMACURSE_EFFECT].extrinsic || have_permacursestone())) {
		sprintf(buf, "the following problem: Many conventional methods of curse removal no longer work.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", PermacurseEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (ShroudedIdentity || u.uprops[SHROUDED_IDENTITY].extrinsic || have_shroudedidentitystone())) {
		sprintf(buf, "the following problem: Identifying items is much harder.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", ShroudedIdentity);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (FeelerGauges || u.uprops[FEELER_GAUGES].extrinsic || have_feelergaugesstone())) {
		sprintf(buf, "the following problem: Monsters always know where you are and come straight for you.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FeelerGauges);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (LongScrewup || u.uprops[LONG_SCREWUP].extrinsic || have_longscrewupstone())) {
		sprintf(buf, "the following problem: Triggering a nasty trap causes the effect to last for a really long time.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", LongScrewup);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (WingYellowChange || u.uprops[WING_YELLOW_GLYPHS].extrinsic || have_wingyellowstone())) {
		sprintf(buf, "the following problem: You're forced to play with a different tileset.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", WingYellowChange);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (LifeSavingBug || u.uprops[LIFE_SAVING_BUG].extrinsic || have_lifesavingstone())) {
		sprintf(buf, "the following problem: Monsters occasionally lifesave when killed.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", LifeSavingBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (CurseuseEffect || u.uprops[CURSEUSE_EFFECT].extrinsic || have_curseusestone())) {
		sprintf(buf, "the following problem: Any scroll, potion, spellbook, wand or tool you use automatically becomes cursed.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", CurseuseEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (CutNutritionEffect || u.uprops[CUT_NUTRITION].extrinsic || have_cutnutritionstone())) {
		sprintf(buf, "the following problem: You gain much less nutrition than usual.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", CutNutritionEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (SkillLossEffect || u.uprops[SKILL_LOSS_EFFECT].extrinsic || have_skilllossstone())) {
		sprintf(buf, "the following problem: Your skills will gradually lose training.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", SkillLossEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (AutopilotEffect || u.uprops[AUTOPILOT_EFFECT].extrinsic || have_autopilotstone())) {
		sprintf(buf, "the following problem: You're on autopilot.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", AutopilotEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (MysteriousForceActive || u.uprops[MYSTERIOUS_FORCE_EFFECT].extrinsic || have_forcestone())) {
		sprintf(buf, "the following problem: The dirty mysterious force full of dirt is back.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", MysteriousForceActive);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (MonsterGlyphChange || u.uprops[MONSTER_GLYPH_BUG].extrinsic || have_monsterglyphstone())) {
		sprintf(buf, "the following problem: Monsters rapidly change glyphs.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", MonsterGlyphChange);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (ChangingDirectives || u.uprops[CHANGING_DIRECTIVES].extrinsic || have_changingdirectivestone())) {
		sprintf(buf, "the following problem: The directives you've given to your pets will change randomly.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", ChangingDirectives);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (ContainerKaboom || u.uprops[CONTAINER_KABOOM].extrinsic || have_containerkaboomstone())) {
		sprintf(buf, "the following problem: Opening a container causes bad stuff to happen.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", ContainerKaboom);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (StealDegrading || u.uprops[STEAL_DEGRADING].extrinsic || have_stealdegradestone())) {
		sprintf(buf, "the following problem: If a monster steals your items, the stolen items are cursed and disenchanted.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", StealDegrading);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (LeftInventoryBug || u.uprops[LEFT_INVENTORY].extrinsic || have_leftinventorystone())) {
		sprintf(buf, "the following problem: Items that leave your inventory will be unidentified.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", LeftInventoryBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (FluctuatingSpeed || u.uprops[FLUCTUATING_SPEED].extrinsic || have_fluctuatingspeedstone())) {
		sprintf(buf, "the following problem: Your movement speed fluctuates from extremely fast to ultra slow.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FluctuatingSpeed);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (TarmuStrokingNora || u.uprops[TARMU_STROKING_NORA].extrinsic || have_tarmustrokingnorastone())) {
		sprintf(buf, "the following problem: Searching may cause a new trap to spawn.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", TarmuStrokingNora);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone())) {
		sprintf(buf, "the following problem: The messages 'nothing happens' or 'never mind' become dangerous.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FailureEffects);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (BrightCyanSpells || u.uprops[BRIGHT_CYAN_SPELLS].extrinsic || have_brightcyanspellstone())) {
		sprintf(buf, "the following problem: Your spells became bright cyan.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", BrightCyanSpells);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (FrequentationSpawns || u.uprops[FREQUENTATION_SPAWNS].extrinsic || have_frequentationspawnstone())) {
		sprintf(buf, "the following problem: Some monster trait is more common than usual.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FrequentationSpawns);
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%s)", montraitname(u.frequentationtrait));
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (PetAIScrewed || u.uprops[PET_AI_SCREWED].extrinsic || have_petaistone())) {
		sprintf(buf, "the following problem: Your pets refuse to follow you.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", PetAIScrewed);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (SatanEffect || u.uprops[SATAN_EFFECT].extrinsic || have_satanstone())) {
		sprintf(buf, "the following problem: Satan made it so that changing dungeon levels paralyzes you for a turn.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", SatanEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (RememberanceEffect || u.uprops[REMEMBERANCE_EFFECT].extrinsic || have_rememberancestone())) {
		sprintf(buf, "the following problem: Spells with low spell memory are harder to cast.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", RememberanceEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (PokelieEffect || u.uprops[POKELIE_EFFECT].extrinsic || have_pokeliestone())) {
		sprintf(buf, "the following problem: The pokedex lies to you.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", PokelieEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (AlwaysAutopickup || u.uprops[AUTOPICKUP_ALWAYS].extrinsic || have_autopickupstone())) {
		sprintf(buf, "the following problem: You always pick up items.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", AlwaysAutopickup);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (DywypiProblem || u.uprops[DYWYPI_PROBLEM].extrinsic || have_dywypistone())) {
		sprintf(buf, "the following problem: If you lifesave, you have to give the correct answer or you die anyway.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", DywypiProblem);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (SilverSpells || u.uprops[SILVER_SPELLS].extrinsic || have_silverspellstone())) {
		sprintf(buf, "the following problem: Your spells became silver.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", SilverSpells);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (MetalSpells || u.uprops[METAL_SPELLS].extrinsic || have_metalspellstone())) {
		sprintf(buf, "the following problem: Your spells became metal.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", MetalSpells);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (PlatinumSpells || u.uprops[PLATINUM_SPELLS].extrinsic || have_platinumspellstone())) {
		sprintf(buf, "the following problem: Your spells became platinum.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", PlatinumSpells);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (ManlerEffect || u.uprops[MANLER_EFFECT].extrinsic || have_manlerstone())) {
		sprintf(buf, "the following problem: The manler is chasing you.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", ManlerEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (DoorningEffect || u.uprops[DOORNING_EFFECT].extrinsic || have_doorningstone())) {
		sprintf(buf, "the following problem: Doors are dangerous.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", DoorningEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (NownsibleEffect || u.uprops[NOWNSIBLE_EFFECT].extrinsic || have_nownsiblestone())) {
		sprintf(buf, "the following problem: Triggering a trap makes it invisible.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", NownsibleEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (ElmStreetEffect || u.uprops[ELM_STREET_EFFECT].extrinsic || have_elmstreetstone())) {
		sprintf(buf, "the following problem: The game behaves as if it were Friday the 13th.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", ElmStreetEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (MonnoiseEffect || u.uprops[MONNOISE_EFFECT].extrinsic || have_monnoisestone())) {
		sprintf(buf, "the following problem: Monsters make noises.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", MonnoiseEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (RangCallEffect || u.uprops[RANG_CALL_EFFECT].extrinsic || have_rangcallstone())) {
		sprintf(buf, "the following problem: The presence of boss monsters increases your sanity.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", RangCallEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (RecurringSpellLoss || u.uprops[RECURRING_SPELL_LOSS].extrinsic || have_recurringspelllossstone())) {
		sprintf(buf, "the following problem: You repeatedly lose spell memory.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", RecurringSpellLoss);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (AntitrainingEffect || u.uprops[ANTI_TRAINING_EFFECT].extrinsic || have_antitrainingstone())) {
		sprintf(buf, "the following problem: Your skill training is occasionally reduced.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", AntitrainingEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (TechoutBug || u.uprops[TECHOUT_BUG].extrinsic || have_techoutstone())) {
		sprintf(buf, "the following problem: Your techniques may sometimes be put on timeout.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", TechoutBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (StatDecay || u.uprops[STAT_DECAY].extrinsic || have_statdecaystone())) {
		sprintf(buf, "the following problem: Your stats will gradually decay.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", StatDecay);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (Movemork || u.uprops[MOVEMORKING].extrinsic || have_movemorkstone())) {
		sprintf(buf, "the following problem: Monsters are generated with movement energy.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", Movemork);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (LootcutBug || u.uprops[LOOTCUT_BUG].extrinsic || have_lootcutstone())) {
		sprintf(buf, "the following problem: Monsters delete their musable items when killed.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", LootcutBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (MonsterSpeedBug || u.uprops[MONSTER_SPEED_BUG].extrinsic || have_monsterspeedstone())) {
		sprintf(buf, "the following problem: Monsters are moving faster.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", MonsterSpeedBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (ScalingBug || u.uprops[SCALING_BUG].extrinsic || have_scalingstone())) {
		sprintf(buf, "the following problem: Minimum monster level depends on your current depth.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", ScalingBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (EnmityBug || u.uprops[ENMITY_BUG].extrinsic || have_inimicalstone())) {
		sprintf(buf, "the following problem: Monsters always spawn hostile.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", EnmityBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (WhiteSpells || u.uprops[WHITE_SPELLS].extrinsic || have_whitespellstone())) {
		sprintf(buf, "the following problem: Your spells became white.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", WhiteSpells);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (CompleteGraySpells || u.uprops[COMPLETE_GRAY_SPELLS].extrinsic || have_greyoutstone())) {
		sprintf(buf, "the following problem: Your spells became completely gray.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", CompleteGraySpells);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (QuasarVision || u.uprops[QUASAR_BUG].extrinsic || have_quasarstone())) {
		sprintf(buf, "the following problem: You can barely see the quasars.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", QuasarVision);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (MommaBugEffect || u.uprops[MOMMA_BUG].extrinsic || have_mommystone())) {
		sprintf(buf, "the following problem: Your momma will repeatedly be insulted.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", MommaBugEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (HorrorBugEffect || u.uprops[HORROR_BUG].extrinsic || have_horrorstone())) {
		sprintf(buf, "the following problem: You will repeatedly get hit with status effects.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", HorrorBugEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (ArtificerBug || u.uprops[ARTIFICER_BUG].extrinsic || have_artificialstone())) {
		sprintf(buf, "the following problem: You will be forced to equip evil artifacts.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", ArtificerBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (WereformBug || u.uprops[WEREFORM_BUG].extrinsic || have_wereformstone())) {
		sprintf(buf, "the following problem: You randomly polymorph into werecreatures.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", WereformBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (NonprayerBug || u.uprops[NON_PRAYER_BUG].extrinsic || have_antiprayerstone())) {
		sprintf(buf, "the following problem: Your prayer timeout goes up instead of down.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", NonprayerBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (EvilPatchEffect || u.uprops[EVIL_PATCH_EFFECT].extrinsic || have_evilpatchstone())) {
		sprintf(buf, "the following problem: You will get nasty trap effects intrinsically over time.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", EvilPatchEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (HardModeEffect || u.uprops[HARD_MODE_EFFECT].extrinsic || have_hardmodestone())) {
		sprintf(buf, "the following problem: All damage you take is doubled.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", HardModeEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (SecretAttackBug || u.uprops[SECRET_ATTACK_BUG].extrinsic || have_secretattackstone())) {
		sprintf(buf, "the following problem: Monsters occasionally use their secret attacks.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", SecretAttackBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (EaterBugEffect || u.uprops[EATER_BUG].extrinsic || have_eaterstone())) {
		sprintf(buf, "the following problem: Monsters can eat all items they encounter.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", EaterBugEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (CovetousnessBug || u.uprops[COVETOUSNESS_BUG].extrinsic || have_covetousstone())) {
		sprintf(buf, "the following problem: Covetous monster AI fires each turn.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", CovetousnessBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (NotSeenBug || u.uprops[NOT_SEEN_BUG].extrinsic || have_nonseeingstone())) {
		sprintf(buf, "the following problem: The walls became invisible.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", NotSeenBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (DarkModeBug || u.uprops[DARK_MODE_BUG].extrinsic || have_darkmodestone())) {
		sprintf(buf, "the following problem: Lit tiles aren't visible unless you would also see them if they were unlit.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", DarkModeBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (AntisearchEffect || u.uprops[ANTISEARCH_EFFECT].extrinsic || have_unfindablestone())) {
		sprintf(buf, "the following problem: The search command never finds anything.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", AntisearchEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (HomicideEffect || u.uprops[HOMICIDE_EFFECT].extrinsic || have_homicidestone())) {
		sprintf(buf, "the following problem: Monsters build new traps for you to blunder into.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", HomicideEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (NastynationBug || u.uprops[NASTY_NATION_BUG].extrinsic || have_multitrappingstone())) {
		sprintf(buf, "the following problem: All trap types except beneficial ones are equally likely to generate.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", NastynationBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (WakeupCallBug || u.uprops[WAKEUP_CALL_BUG].extrinsic || have_wakeupcallstone())) {
		sprintf(buf, "the following problem: Peaceful monsters may spontaneously turn hostile.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", WakeupCallBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (GrayoutBug || u.uprops[GRAYOUT_BUG].extrinsic || have_grayoutstone())) {
		sprintf(buf, "the following problem: The display intermittently becomes all gray.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", GrayoutBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (GrayCenterBug || u.uprops[GRAY_CENTER_BUG].extrinsic || have_graycenterstone())) {
		sprintf(buf, "the following problem: Your immediate surroundings are obscured by a gray shroud.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", GrayCenterBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (CheckerboardBug || u.uprops[CHECKERBOARD_BUG].extrinsic || have_checkerboardstone())) {
		sprintf(buf, "the following problem: Only one color of checkerboard tiles is ever visible at any given time.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", CheckerboardBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (ClockwiseSpinBug || u.uprops[CLOCKWISE_SPIN_BUG].extrinsic || have_clockwisestone())) {
		sprintf(buf, "the following problem: Your directional keys are shifted 45 degrees clockwise.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", ClockwiseSpinBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (CounterclockwiseSpin || u.uprops[COUNTERCLOCKWISE_SPIN_BUG].extrinsic || have_counterclockwisestone())) {
		sprintf(buf, "the following problem: Your directional keys are shifted 90 degrees counterclockwise.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", CounterclockwiseSpin);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (LagBugEffect || u.uprops[LAG_BUG].extrinsic || have_lagstone())) {
		sprintf(buf, "the following problem: The game lags.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", LagBugEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (BlesscurseEffect || u.uprops[BLESSCURSE_EFFECT].extrinsic || have_blesscursestone())) {
		sprintf(buf, "the following problem: Picking up a blessed item instantly curses it.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", BlesscurseEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (DeLightBug || u.uprops[DE_LIGHT_BUG].extrinsic || have_delightstone())) {
		sprintf(buf, "the following problem: Tiles that you're walking on automatically become unlit.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", DeLightBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (DischargeBug || u.uprops[DISCHARGE_BUG].extrinsic || have_dischargestone())) {
		sprintf(buf, "the following problem: If you use wands or tools that have charges, more charges are used up than normal.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", DischargeBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (TrashingBugEffect || u.uprops[TRASHING_EFFECT].extrinsic || have_trashstone())) {
		sprintf(buf, "the following problem: If you equip enchanted weapons, armor or rings, their enchantment value decreases.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", TrashingBugEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (FilteringBug || u.uprops[FILTERING_BUG].extrinsic || have_filteringstone())) {
		sprintf(buf, "the following problem: Certain messages may be replaced with generic ones.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FilteringBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (DeformattingBug || u.uprops[DEFORMATTING_BUG].extrinsic || have_deformattingstone())) {
		sprintf(buf, "the following problem: Your pokedex doesn't work.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", DeformattingBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (FlickerStripBug || u.uprops[FLICKER_STRIP_BUG].extrinsic || have_flickerstripstone())) {
		sprintf(buf, "the following problem: The bottom status line only displays garbage strings.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FlickerStripBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (UndressingEffect || u.uprops[UNDRESSING_EFFECT].extrinsic || have_undressingstone())) {
		sprintf(buf, "the following problem: You spontaneously take off equipment.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", UndressingEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (Hyperbluewalls || u.uprops[HYPERBLUEWALL_BUG].extrinsic || have_hyperbluestone())) {
		sprintf(buf, "the following problem: Walls are blue, and walking into them hurts you.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", Hyperbluewalls);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (NoliteBug || u.uprops[NOLITE_BUG].extrinsic || have_nolightstone())) {
		sprintf(buf, "the following problem: Staircases and object piles are no longer highlighted.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", NoliteBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (ParanoiaBugEffect || u.uprops[PARANOIA_BUG].extrinsic || have_paranoiastone())) {
		sprintf(buf, "the following problem: There is no confirmation if you walk into traps, water or lava.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", ParanoiaBugEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (FleecescriptBug || u.uprops[FLEECESCRIPT_BUG].extrinsic || have_fleecestone())) {
		sprintf(buf, "the following problem: Inventory item descriptions appear in fleecy colors, which is actually not all that problematic. :-)");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FleecescriptBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (InterruptEffect || u.uprops[INTERRUPT_EFFECT].extrinsic || have_interruptionstone())) {
		sprintf(buf, "the following problem: The read, quaff, zap and apply commands now take several turns to use and can be interrupted.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", InterruptEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (DustbinBug || u.uprops[DUSTBIN_BUG].extrinsic || have_dustbinstone())) {
		sprintf(buf, "the following problem: If you pick up a scroll, it may disintegrate.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", DustbinBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (ManaBatteryBug || u.uprops[MANA_BATTERY_BUG].extrinsic || have_batterystone())) {
		sprintf(buf, "a living mana battery.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", ManaBatteryBug);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (Monsterfingers || u.uprops[MONSTERFINGERS_EFFECT].extrinsic || have_butterfingerstone())) {
		sprintf(buf, "the following problem: Picking up potions may break them and subject you to their vapors.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", Monsterfingers);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (MiscastBug || u.uprops[MISCAST_BUG].extrinsic || have_miscastingstone())) {
		sprintf(buf, "the following problem: Casting a spell always causes backlash, regardless of whether the spell worked.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", MiscastBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone())) {
		sprintf(buf, "the following problem: The top status line doesn't display anything.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", MessageSuppression);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (StuckAnnouncement || u.uprops[STUCK_ANNOUNCEMENT_BUG].extrinsic || have_stuckannouncementstone())) {
		sprintf(buf, "the following problem: The bottom status line only updates if you explicitly tell it to do so.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", StuckAnnouncement);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (BloodthirstyEffect || u.uprops[BLOODTHIRSTY_EFFECT].extrinsic || have_stormstone())) {
		sprintf(buf, "the following problem: There is no confirmation if you walk into a peaceful monster.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", BloodthirstyEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (MaximumDamageBug || u.uprops[MAXIMUM_DAMAGE_BUG].extrinsic || have_maximumdamagestone())) {
		sprintf(buf, "the following problem: Monster attacks always deal maximum damage to you.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", MaximumDamageBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (LatencyBugEffect || u.uprops[LATENCY_BUG].extrinsic || have_latencystone())) {
		sprintf(buf, "the following problem: There is background latency.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", LatencyBugEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (StarlitBug || u.uprops[STARLIT_BUG].extrinsic || have_starlitskystone())) {
		sprintf(buf, "the following problem: All monsters look the same and you can't tell them apart.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", StarlitBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (KnowledgeBug || u.uprops[KNOWLEDGE_BUG].extrinsic || have_trapknowledgestone())) {
		sprintf(buf, "the following problem: All traps look the same and you can't tell them apart.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", KnowledgeBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (HighscoreBug || u.uprops[HIGHSCORE_BUG].extrinsic || have_highscorestone())) {
		sprintf(buf, "the following problem: The game continuously spawns spacewars monsters and nasty traps.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", HighscoreBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (PinkSpells || u.uprops[PINK_SPELLS].extrinsic || have_pinkspellstone())) {
		sprintf(buf, "the following problem: Your spells became pink.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", PinkSpells);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (GreenSpells || u.uprops[GREEN_SPELLS].extrinsic || have_greenspellstone())) {
		sprintf(buf, "the following problem: Your spells became green.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", GreenSpells);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (EvencoreEffect || u.uprops[EVC_EFFECT].extrinsic || have_evcstone())) {
		sprintf(buf, "the following problem: The game will spawn fake invisible monster markers.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", EvencoreEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (UnderlayerBug || u.uprops[UNDERLAYER_BUG].extrinsic || have_underlaidstone())) {
		sprintf(buf, "the following problem: Invisible monster markers are not visible, but bumping into them uses up a turn.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", UnderlayerBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (DamageMeterBug || u.uprops[DAMAGE_METER_BUG].extrinsic || have_damagemeterstone())) {
		sprintf(buf, "the following problem: The game won't tell you exactly how much damage an attack does.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", DamageMeterBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (ArbitraryWeightBug || u.uprops[ARBITRARY_WEIGHT_BUG].extrinsic || have_weightstone())) {
		sprintf(buf, "the following problem: You can't see your carry capacity or the weight of items.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", ArbitraryWeightBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (FuckedInfoBug || u.uprops[FUCKED_INFO_BUG].extrinsic || have_infofuckstone())) {
		sprintf(buf, "the following problem: You can't see what character you are playing.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FuckedInfoBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (BlackSpells || u.uprops[BLACK_SPELLS].extrinsic || have_blackspellstone())) {
		sprintf(buf, "the following problem: Your spells became black.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", BlackSpells);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (CyanSpells || u.uprops[CYAN_SPELLS].extrinsic || have_cyanspellstone())) {
		sprintf(buf, "the following problem: Your spells became cyan.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", CyanSpells);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (HeapEffectBug || u.uprops[HEAP_EFFECT].extrinsic || have_heapstone())) {
		sprintf(buf, "the following problem: Messages are repeated randomly.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", HeapEffectBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (BlueSpells || u.uprops[BLUE_SPELLS].extrinsic || have_bluespellstone())) {
		sprintf(buf, "the following problem: Your spells became blue.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", BlueSpells);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (TronEffect || u.uprops[TRON_EFFECT].extrinsic || have_tronstone())) {
		sprintf(buf, "the following problem: You can't walk in the same direction twice in a row.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", TronEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (RedSpells || u.uprops[RED_SPELLS].extrinsic || have_redspellstone())) {
		sprintf(buf, "the following problem: Your spells became red.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", RedSpells);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (TooHeavyEffect || u.uprops[TOO_HEAVY_EFFECT].extrinsic || have_tooheavystone())) {
		sprintf(buf, "the following problem: Picking up an item increases your carried weight by more than what the item weighs.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", TooHeavyEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (ElongationBug || u.uprops[ELONGATION_BUG].extrinsic || have_elongatedstone())) {
		sprintf(buf, "the following problem: Monsters' hug attacks, ranged weapons and breaths can hit you from very far away.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", ElongationBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (WrapoverEffect || u.uprops[WRAPOVER_EFFECT].extrinsic || have_wrapoverstone())) {
		sprintf(buf, "the following problem: Your positively enchanted items may randomly turn into negatively enchanted ones.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", WrapoverEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (DestructionEffect || u.uprops[DESTRUCTION_EFFECT].extrinsic || have_destructionstone())) {
		sprintf(buf, "the following problem: Item destruction may happen randomly.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", DestructionEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (MeleePrefixBug || u.uprops[MELEE_PREFIX_BUG].extrinsic || have_meleeprefixstone())) {
		sprintf(buf, "the following problem: If you simply walk into monsters to attack, you lose a turn.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", MeleePrefixBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (AutomoreBug || u.uprops[AUTOMORE_BUG].extrinsic || have_automorestone())) {
		sprintf(buf, "the following problem: There are no --More-- prompts.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", AutomoreBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (UnfairAttackBug || u.uprops[UNFAIR_ATTACK_BUG].extrinsic || have_unfairattackstone())) {
		sprintf(buf, "the following problem: Monsters will occasionally use very unfair attacks on you.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", UnfairAttackBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (DisconnectedStairs || u.uprops[DISCONNECTED_STAIRS].extrinsic || have_disconnectstone())) {
		sprintf(buf, "the following problem: Staircases are disconnected.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", DisconnectedStairs);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (InterfaceScrewed || u.uprops[INTERFACE_SCREW].extrinsic || have_interfacescrewstone())) {
		sprintf(buf, "the following problem: The main game display does not update automatically.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", InterfaceScrewed);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (Bossfights || u.uprops[BOSSFIGHT].extrinsic || have_bossfightstone())) {
		sprintf(buf, "the following problem: Boss monsters spawn much more often.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", Bossfights);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (EntireLevelMode || u.uprops[ENTIRE_LEVEL].extrinsic || have_entirelevelstone())) {
		sprintf(buf, "the following problem: Monster types that used to be rare are common now.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", EntireLevelMode);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (BonesLevelChange || u.uprops[BONES_CHANGE].extrinsic || have_bonestone())) {
		sprintf(buf, "the following problem: You always find or leave bones levels if possible.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", BonesLevelChange);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (AutocursingEquipment || u.uprops[AUTOCURSE_EQUIP].extrinsic || have_autocursestone())) {
		sprintf(buf, "the following problem: Every item you put on automatically curses itself.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", AutocursingEquipment);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (HighlevelStatus || u.uprops[HIGHLEVEL_STATUS].extrinsic || have_highlevelstone())) {
		sprintf(buf, "the following problem: Higher-level monsters are more likely to spawn.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", HighlevelStatus);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (SpellForgetting || u.uprops[SPELL_FORGETTING].extrinsic || have_spellforgettingstone())) {
		sprintf(buf, "the following problem: Your spell memory decreases rapidly.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", SpellForgetting);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (SoundEffectBug || u.uprops[SOUND_EFFECT_BUG].extrinsic || have_soundeffectstone())) {
		sprintf(buf, "the following problem: Things that happen will play 'sound effects'.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", SoundEffectBug);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (CaptchaProblem || u.uprops[CAPTCHA].extrinsic || have_captchastone())) {
		sprintf(buf, "the following problem: You sometimes have to solve captchas.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", CaptchaProblem);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (FarlookProblem || u.uprops[FARLOOK_BUG].extrinsic || have_farlookstone())) {
		sprintf(buf, "the following problem: Farlooking peaceful monsters angers them.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FarlookProblem);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (RespawnProblem || u.uprops[RESPAWN_BUG].extrinsic || have_respawnstone())) {
		sprintf(buf, "the following problem: Killing monsters will cause them to respawn somewhere on the level.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", RespawnProblem);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (BigscriptEffect || u.uprops[BIGSCRIPT].extrinsic || have_bigscriptstone())) {
		sprintf(buf, "BIGscript.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", BigscriptEffect);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (BankTrapEffect || u.uprops[BANKBUG].extrinsic || have_bankstone())) {
		sprintf(buf, "the following problem: Your money will wander into a mysterious bank.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", BankTrapEffect);
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (amount stored: %d)", u.bankcashamount);
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (money limit: %d)", u.bankcashlimit);
		you_have(buf);
	} else {
	  if ((guaranteed || !rn2(10)) && u.bankcashamount) {
	    if (wizard || (!rn2(10)) || final >= 1 ) {
		sprintf(buf, "the following amount of cash stored in the bank:");
		sprintf(eos(buf), " %d", u.bankcashamount);
		you_have(buf);
	    }
	  }
	  if ((guaranteed || !rn2(10)) && u.bankcashlimit) {
	    if (wizard || (!rn2(10)) || final >= 1 ) {
		sprintf(buf, "the following bank cash limit:");
		sprintf(eos(buf), " %d", u.bankcashlimit);
		you_have(buf);
	    }
	  }
	}
	if ((guaranteed || !rn2(10)) && (MapTrapEffect || u.uprops[MAPBUG].extrinsic || have_mapstone())) {
		sprintf(buf, "the following problem: The map doesn't display correctly.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", MapTrapEffect);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (TechTrapEffect || u.uprops[TECHBUG].extrinsic || have_techniquestone())) {
		sprintf(buf, "the following problem: Your techniques frequently don't work.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", TechTrapEffect);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (RecurringDisenchant || u.uprops[RECURRING_DISENCHANT].extrinsic || have_disenchantmentstone())) {
		sprintf(buf, "the following problem: Your possessions disenchant themselves spontaneously.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", RecurringDisenchant);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (verisiertEffect || u.uprops[VERISIERTEFFECT].extrinsic || have_verisiertstone())) {
		sprintf(buf, "the following problem: Monster respawn speeds up rapidly.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", verisiertEffect);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (ChaosTerrain || u.uprops[CHAOS_TERRAIN].extrinsic || have_chaosterrainstone())) {
		sprintf(buf, "the following problem: The terrain slowly becomes ever more chaotic.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", ChaosTerrain);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (Muteness || u.uprops[MUTENESS].extrinsic || have_mutenessstone())) {
		sprintf(buf, "the following problem: You're mute, and have great difficulty when trying to cast spells.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", Muteness);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (EngravingDoesntWork || u.uprops[ENGRAVINGBUG].extrinsic || have_engravingstone())) {
		sprintf(buf, "the following problem: Monsters don't respect Elbereth.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", EngravingDoesntWork);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (MagicDeviceEffect || u.uprops[MAGIC_DEVICE_BUG].extrinsic || have_magicdevicestone())) {
		sprintf(buf, "the following problem: Zapping a wand can cause it to explode.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", MagicDeviceEffect);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (BookTrapEffect || u.uprops[BOOKBUG].extrinsic || have_bookstone())) {
		sprintf(buf, "the following problem: Reading spellbooks confuses you.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", BookTrapEffect);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (LevelTrapEffect || u.uprops[LEVELBUG].extrinsic || have_levelstone())) {
		sprintf(buf, "the following problem: Monsters become stronger if many of their species have been generated already.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", LevelTrapEffect);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (QuizTrapEffect || u.uprops[QUIZZES].extrinsic || have_quizstone())) {
		sprintf(buf, "the following problem: You have to partake in the Great NetHack Quiz.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", QuizTrapEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (FastMetabolismEffect || u.uprops[FAST_METABOLISM].extrinsic || have_metabolicstone())) {
		sprintf(buf, "the following problem: Your food consumption is much faster.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FastMetabolismEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (NoReturnEffect || u.uprops[NORETURN].extrinsic || have_noreturnstone())) {
		sprintf(buf, "the following problem: You cannot teleport at all.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", NoReturnEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (AlwaysEgotypeMonsters || u.uprops[ALWAYS_EGOTYPES].extrinsic || have_egostone())) {
		sprintf(buf, "the following problem: Monsters always spawn with egotypes.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", AlwaysEgotypeMonsters);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (TimeGoesByFaster || u.uprops[FAST_FORWARD].extrinsic || have_fastforwardstone())) {
		sprintf(buf, "the following problem: Time goes by faster.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", TimeGoesByFaster);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (FoodIsAlwaysRotten || u.uprops[FOOD_IS_ROTTEN].extrinsic || have_rottenstone())) {
		sprintf(buf, "the following problem: Your food is always rotten.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FoodIsAlwaysRotten);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (AllSkillsUnskilled || u.uprops[SKILL_DEACTIVATED].extrinsic || have_unskilledstone())) {
		sprintf(buf, "the following problem: Your skills are deactivated.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", AllSkillsUnskilled);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (AllStatsAreLower || u.uprops[STATS_LOWERED].extrinsic || have_lowstatstone())) {
		sprintf(buf, "the following problem: All of your stats are lowered.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", AllStatsAreLower);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (PlayerCannotTrainSkills || u.uprops[TRAINING_DEACTIVATED].extrinsic || have_trainingstone())) {
		sprintf(buf, "the following problem: You cannot train skills.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", PlayerCannotTrainSkills);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (PlayerCannotExerciseStats || u.uprops[EXERCISE_DEACTIVATED].extrinsic || have_exercisestone())) {
		sprintf(buf, "the following problem: You cannot exercise your stats.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", PlayerCannotExerciseStats);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && MCReduction) {
		sprintf(buf, "reduced magic cancellation.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", MCReduction);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[INTRINSIC_LOSS].extrinsic || IntrinsicLossProblem || have_intrinsiclossstone() )) {
		sprintf(buf, "a case of random intrinsic loss.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", IntrinsicLossProblem);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && u.uprops[TRAP_REVEALING].extrinsic) {
		sprintf(buf, "randomly revealing traps");
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && (u.uprops[BLOOD_LOSS].extrinsic || BloodLossProblem || have_bloodlossstone() )) {
		sprintf(buf, "bleeding out");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", BloodLossProblem);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && (u.uprops[NASTINESS_EFFECTS].extrinsic || NastinessProblem || have_nastystone() )) {
		sprintf(buf, "subjected to random nasty trap effects");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", NastinessProblem);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && (u.uprops[BAD_EFFECTS].extrinsic || BadEffectProblem || have_badeffectstone() )) {
		sprintf(buf, "subjected to random bad effects");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", BadEffectProblem);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && (u.uprops[AUTOMATIC_TRAP_CREATION].extrinsic || TrapCreationProblem || have_trapcreationstone() )) {
		sprintf(buf, "more likely to encounter traps");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", TrapCreationProblem);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && u.uprops[SENTIENT_HIGH_HEELS].extrinsic) {
		sprintf(buf, "wearing sentient high heels");
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && (u.uprops[REPEATING_VULNERABILITY].extrinsic || AutomaticVulnerabilitiy || have_vulnerabilitystone() )) {
		sprintf(buf, "the risk of temporarily losing intrinsics");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", AutomaticVulnerabilitiy);
		you_have(buf);
	}
	if ((guaranteed || !rn2(10)) && (u.uprops[TELEPORTING_ITEMS].extrinsic || TeleportingItems || have_itemportstone() )) {
		sprintf(buf, "teleporting items");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", TeleportingItems);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[RECURRING_AMNESIA].extrinsic || RecurringAmnesia || have_amnesiastone() )) {
		sprintf(buf, "going to suffer from amnesia now and then");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", RecurringAmnesia);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[TURNLIMITATION].extrinsic || TurnLimitation || have_limitationstone() )) {
		sprintf(buf, "getting your ascension turn limit reduced whenever you take damage");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", TurnLimitation);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[WEAKSIGHT].extrinsic || WeakSight || have_weaksightstone() )) {
		sprintf(buf, "short-sighted");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", WeakSight);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[RANDOM_MESSAGES].extrinsic || RandomMessages || have_messagestone() )) {
		sprintf(buf, "reading random messages");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", RandomMessages);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[DESECRATION].extrinsic || Desecration || have_nonsacredstone() )) {
		sprintf(buf, "the following problem: Altars malfunction whenever you try to use them");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", Desecration);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[STARVATION_EFFECT].extrinsic || StarvationEffect || have_starvationstone() )) {
		sprintf(buf, "not going to get corpses from defeated enemies");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", StarvationEffect);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[NO_DROPS_EFFECT].extrinsic || NoDropsEffect || have_droplessstone() )) {
		sprintf(buf, "not going to get death drops from monsters");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", NoDropsEffect);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[LOW_EFFECTS].extrinsic || LowEffects || have_loweffectstone() )) {
		sprintf(buf, "reduced power of magical effects");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", LowEffects);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[INVIS_TRAPS_EFFECT].extrinsic || InvisibleTrapsEffect || have_invisostone() )) {
		sprintf(buf, "not going to see traps");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", InvisibleTrapsEffect);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[GHOST_WORLD].extrinsic || GhostWorld || have_ghostlystone() )) {
		sprintf(buf, "in a ghost world");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", GhostWorld);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[DEHYDRATION].extrinsic || Dehydration || have_dehydratingstone() )) {
		sprintf(buf, "going to become dehydrated");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", Dehydration);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[HATE_TRAP_EFFECT].extrinsic || HateTrapEffect || have_hatestone() )) {
		sprintf(buf, "the following problem: Pets hate you with a fiery passion.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", HateTrapEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[TOTTER_EFFECT].extrinsic || TotterTrapEffect || have_directionswapstone() )) {
		sprintf(buf, "the following problem: Your directional keys got swapped.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", TotterTrapEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() )) {
		sprintf(buf, "unable to get intrinsics from eating corpses");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", Nonintrinsics);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[DROPCURSES_EFFECT].extrinsic || Dropcurses || have_dropcursestone() )) {
		sprintf(buf, "the following problem: Dropping items causes them to autocurse.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", Dropcurses);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[NAKEDNESS].extrinsic || Nakedness || have_nakedstone() )) {
		sprintf(buf, "effectively naked");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", Nakedness);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[ANTILEVELING].extrinsic || Antileveling || have_antilevelstone() )) {
		sprintf(buf, "unable to gain experience past experience level 10, and slower to reach XL10");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", Antileveling);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[ITEM_STEALING_EFFECT].extrinsic || ItemStealingEffect || have_stealerstone() )) {
		sprintf(buf, "more likely to have your items stolen");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", ItemStealingEffect);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[REBELLION_EFFECT].extrinsic || Rebellions || have_rebelstone() )) {
		sprintf(buf, "the following problem: Pets can spontaneously rebel and become hostile.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", Rebellions);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[CRAP_EFFECT].extrinsic || CrapEffect || have_shitstone() )) {
		sprintf(buf, "diarrhea");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", CrapEffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[PROJECTILES_MISFIRE].extrinsic || ProjectilesMisfire || have_misfirestone() )) {
		sprintf(buf, "the following problem: Your projectiles often misfire.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", ProjectilesMisfire);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && (u.uprops[WALL_TRAP_EFFECT].extrinsic || WallTrapping || have_wallstone() )) {
		sprintf(buf, "the following problem: Dungeon regrowth is excessively fast.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", WallTrapping);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && u.uprops[RANDOM_RUMORS].extrinsic) {
		sprintf(buf, "going to listen to random rumors");
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && IncreasedGravity) {
		sprintf(buf, "increased encumbrance due to a stronger gravity.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", IncreasedGravity);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && NoStaircase) {
		sprintf(buf, "to wait until you can use staircases again.");
	      sprintf(eos(buf), " (%d)", NoStaircase);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && u.ragnaroktimer) {
		sprintf(buf, "going to experience Ragnarok.");
	      if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.ragnaroktimer);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && u.sokosolveboulder && issokosolver) {
		sprintf(buf, "to wait until you can create boulders again.");
	      if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.sokosolveboulder);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && u.sokosolveuntrap && issokosolver) {
		sprintf(buf, "to wait until you can disarm traps again.");
	      if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.sokosolveuntrap);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && u.footererlevel) {
		sprintf(buf, "going to encounter the footerers.");
	      if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (level %d)", u.footererlevel);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && FemaleTrapFemmy) {
		sprintf(buf, "possessed by the ghost of Femmy.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FemaleTrapFemmy);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && FemaleTrapMadeleine) {
		sprintf(buf, "possessed by the ghost of Madeleine.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FemaleTrapMadeleine);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && FemaleTrapMarlena) {
		sprintf(buf, "possessed by the ghost of Marlena.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FemaleTrapMarlena);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && FemaleTrapSarah) {
		sprintf(buf, "possessed by the ghost of Sarah.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FemaleTrapSarah);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && FemaleTrapClaudia) {
		sprintf(buf, "possessed by the ghost of Claudia.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FemaleTrapClaudia);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && FemaleTrapLudgera) {
		sprintf(buf, "possessed by the ghost of Ludgera.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FemaleTrapLudgera);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && FemaleTrapKati) {
		sprintf(buf, "possessed by the ghost of Kati.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FemaleTrapKati);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && FemaleTrapAnastasia) {
		sprintf(buf, "possessed by the ghost of Anastasia.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FemaleTrapAnastasia);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && FemaleTrapJessica) {
		sprintf(buf, "possessed by the ghost of Jessica.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FemaleTrapJessica);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && FemaleTrapSolvejg) {
		sprintf(buf, "possessed by the ghost of Solvejg.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FemaleTrapSolvejg);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && FemaleTrapWendy) {
		sprintf(buf, "possessed by the ghost of Wendy.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FemaleTrapWendy);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && FemaleTrapKatharina) {
		sprintf(buf, "possessed by the ghost of Katharina.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FemaleTrapKatharina);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && FemaleTrapElena) {
		sprintf(buf, "possessed by the ghost of Elena.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FemaleTrapElena);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && FemaleTrapThai) {
		sprintf(buf, "possessed by the ghost of Thai.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FemaleTrapThai);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && FemaleTrapElif) {
		sprintf(buf, "possessed by the ghost of Elif.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FemaleTrapElif);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && FemaleTrapNadja) {
		sprintf(buf, "possessed by the ghost of Nadja.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FemaleTrapNadja);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && FemaleTrapSandra) {
		sprintf(buf, "possessed by the ghost of Sandra.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FemaleTrapSandra);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && FemaleTrapNatalje) {
		sprintf(buf, "possessed by the ghost of Natalje.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FemaleTrapNatalje);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && FemaleTrapJeanetta) {
		sprintf(buf, "possessed by the ghost of Jeanetta.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FemaleTrapJeanetta);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && FemaleTrapYvonne) {
		sprintf(buf, "possessed by the ghost of Yvonne.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FemaleTrapYvonne);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && FemaleTrapMaurah) {
		sprintf(buf, "possessed by the ghost of Maurah.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FemaleTrapMaurah);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && FemaleTrapMeltem) {
		sprintf(buf, "possessed by the ghost of Meltem.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", FemaleTrapMeltem);
		you_are(buf);
	}

	if (u.hangupamount) {
		sprintf(buf, "used the hangup function, which means you're either a FILTHY CHEAT0R or you have a shaky internet connection. Hopefully it's the latter so Amy doesn't have to reinstate the hangup penalty.");
	    	sprintf(eos(buf), " (%d)", u.hangupamount);
		you_have(buf);

	}

	if (Race_if(PM_FELID)) {
	    	sprintf(buf, "%d ", u.felidlives);
		sprintf(eos(buf), "lives left");
		you_have(buf);
	}

	if (u.homosexual == 0) {
	    	sprintf(buf, "not decided on your sexuality yet");
		you_have(buf);
	}
	if (u.homosexual == 1) {
	    	sprintf(buf, "heterosexual");
		you_are(buf);
	}
	if (u.homosexual == 2) {
	    	sprintf(buf, "homosexual");
		you_are(buf);
	}

	if (Role_if(PM_HUSSY)) {
	    	sprintf(buf, "%d ", (u.hussyhurtturn - moves));
		sprintf(eos(buf), "turns left to hit a guy.");
		you_have(buf);
	    	sprintf(buf, "%d ", (u.hussykillturn - moves));
		sprintf(eos(buf), "turns left to defeat a guy.");
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && u.contamination) {
		if (u.contamination >= 1000) sprintf(buf, "suffering from fatal contamination. Health and mana regeneration are reduced.");
		else if (u.contamination >= 800) sprintf(buf, "suffering from lethal contamination.");
		else if (u.contamination >= 600) sprintf(buf, "suffering from severe contamination.");
		else if (u.contamination >= 400) sprintf(buf, "suffering from contamination.");
		else if (u.contamination >= 200) sprintf(buf, "suffering from light contamination.");
		else if (u.contamination >= 100) sprintf(buf, "suffering from minor contamination.");
		else if (u.contamination >= 1) sprintf(buf, "suffering from very slight contamination.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.contamination);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && u.copwantedlevel) {
		sprintf(buf, "being chased by the kops.");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.copwantedlevel);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && u.gmmailsreceived) {
		sprintf(buf, "received monster-summoning mail from AmyBSOD, the Game Master.");
	    	sprintf(eos(buf), " (%d)", u.gmmailsreceived);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && TimeStopped) {
		sprintf(buf, "stopped the flow of time.");
	      sprintf(eos(buf), " (%d)", TimeStopped);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && u.drippingtread) {
		sprintf(buf, "dripping elements.");
	      sprintf(eos(buf), " (%d)", u.drippingtread);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && u.geolysis) {
		sprintf(buf, "using geolysis.");
	      sprintf(eos(buf), " (%d)", u.geolysis);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && u.disruptionshield) {
		sprintf(buf, "your disruption shield up.");
	      sprintf(eos(buf), " (%d)", u.disruptionshield);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && u.inertiacontrol) {
		sprintf(buf, "controlling the flow of a spell.");
	      sprintf(eos(buf), " (%d)", u.inertiacontrol);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && u.sterilized) {
		sprintf(buf, "sterilized the air around you.");
	      sprintf(eos(buf), " (%d)", u.sterilized);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && u.holyshield) {
		sprintf(buf, "your holy shield up.");
	      sprintf(eos(buf), " (%d)", u.holyshield);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && u.berserktime) {
		sprintf(buf, "berserk.");
	      sprintf(eos(buf), " (%d)", u.berserktime);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && u.magicshield) {
		sprintf(buf, "your magic shield up.");
	      sprintf(eos(buf), " (%d)", u.magicshield);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && u.burrowed) {
		sprintf(buf, "burrowed into the ground.");
	      sprintf(eos(buf), " (%d)", u.burrowed);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && u.stasistime) {
		sprintf(buf, "in stasis, and the monsters are too.");
	      sprintf(eos(buf), " (%d)", u.stasistime);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && u.powerfailure) {
		sprintf(buf, "experiencing a power failure.");
	      sprintf(eos(buf), " (%d)", u.powerfailure);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && u.tunnelized) {
		sprintf(buf, "reduced armor class due to tunneling dirt on your clothes.");
	      sprintf(eos(buf), " (%d)", u.tunnelized);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && u.nailpolish) {
		sprintf(buf, "applied nail polish.");
	      sprintf(eos(buf), " (%d)", u.nailpolish);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && u.enchantspell) {
		sprintf(buf, "enchanted your weapon with fire.");
	      sprintf(eos(buf), " (%d)", u.enchantspell);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && u.thornspell) {
		sprintf(buf, "thorns.");
	      sprintf(eos(buf), " (%d)", u.thornspell);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && u.egglayingtimeout) {
		sprintf(buf, "to wait until you can lay eggs again.");
	      sprintf(eos(buf), " (%d)", u.egglayingtimeout);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && u.swappositioncount) {
		sprintf(buf, "going to swap positions with the next monster you move into.");
	      sprintf(eos(buf), " (%d)", u.swappositioncount);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && u.contingencyturns) {
		sprintf(buf, "signed up a contract with the reaper.");
	      sprintf(eos(buf), " (%d)", u.contingencyturns);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && u.horsehopturns) {
		sprintf(buf, "the ability to jump as long as you're riding.");
	      sprintf(eos(buf), " (%d)", u.horsehopturns);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && u.bodyfluideffect) {
		sprintf(buf, "very acidic skin.");
	      sprintf(eos(buf), " (%d)", u.bodyfluideffect);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && u.antitelespelltimeout) {
		sprintf(buf, "erected an anti-teleportation field.");
	      sprintf(eos(buf), " (%d)", u.antitelespelltimeout);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && u.breathenhancetimer) {
		sprintf(buf, "magically enhanced breath.");
	      sprintf(eos(buf), " (%d)", u.breathenhancetimer);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && u.snaildigging) {
		sprintf(buf, "to wait until you can fire another digging ray.");
	      sprintf(eos(buf), " (%d)", u.snaildigging);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && u.hussyperfume) {
		sprintf(buf, "to wait until you can spread the perfume again.");
	      sprintf(eos(buf), " (%d)", u.hussyperfume);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && Role_if(PM_JANITOR)) {
		sprintf(buf, "the following amount of trash in your trash can:");
	      sprintf(eos(buf), " %d", u.garbagecleaned);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && Role_if(PM_JANITOR) && u.garbagetrucktime) {
		sprintf(buf, "called the garbage truck, and it will arrive in");
	      sprintf(eos(buf), " %d turns", u.garbagetrucktime);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && u.walscholarpass) {
		sprintf(buf, "able to pass through grave walls.");
	      sprintf(eos(buf), " (%d)", u.walscholarpass);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && u.demagogueabilitytimer && isdemagogue) {
		sprintf(buf, "to wait until you can use your special recursion.");
	      sprintf(eos(buf), " (%d)", u.demagogueabilitytimer);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && u.acutraining) {
		sprintf(buf, "double skill training.");
	      sprintf(eos(buf), " (%d)", u.acutraining);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && u.dragonpolymorphtime) {
		sprintf(buf, "to wait until you can polymorph into a dragon again.");
	      sprintf(eos(buf), " (%d)", u.dragonpolymorphtime);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && UseTheForce) {
		sprintf(buf, StrongUseTheForce ? "able to use the force like a jedi grandmaster" : "able to use the force like a true jedi");
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && u.uprops[SENSORY_DEPRIVATION].extrinsic) {
		sprintf(buf, "sensory deprivation.");
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && NoFire_resistance && (final || u.uprops[DEAC_FIRE_RES].intrinsic) ) {
		sprintf(buf, "prevented from having fire resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_FIRE_RES].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoCold_resistance && (final || u.uprops[DEAC_COLD_RES].intrinsic) ) {
		sprintf(buf, "prevented from having cold resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_COLD_RES].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoSleep_resistance && (final || u.uprops[DEAC_SLEEP_RES].intrinsic) ) {
		sprintf(buf, "prevented from having sleep resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_SLEEP_RES].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoDisint_resistance && (final || u.uprops[DEAC_DISINT_RES].intrinsic) ) {
		sprintf(buf, "prevented from having disintegration resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_DISINT_RES].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoShock_resistance && (final || u.uprops[DEAC_SHOCK_RES].intrinsic) ) {
		sprintf(buf, "prevented from having shock resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_SHOCK_RES].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoPoison_resistance && (final || u.uprops[DEAC_POISON_RES].intrinsic) ) {
		sprintf(buf, "prevented from having poison resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_POISON_RES].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoDrain_resistance && (final || u.uprops[DEAC_DRAIN_RES].intrinsic) ) {
		sprintf(buf, "prevented from having drain resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_DRAIN_RES].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoSick_resistance && (final || u.uprops[DEAC_SICK_RES].intrinsic) ) {
		sprintf(buf, "prevented from having sickness resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_SICK_RES].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoAntimagic_resistance && (final || u.uprops[DEAC_ANTIMAGIC].intrinsic) ) {
		sprintf(buf, "prevented from having magic resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_ANTIMAGIC].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoAcid_resistance && (final || u.uprops[DEAC_ACID_RES].intrinsic) ) {
		sprintf(buf, "prevented from having acid resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_ACID_RES].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoStone_resistance && (final || u.uprops[DEAC_STONE_RES].intrinsic) ) {
		sprintf(buf, "prevented from having petrification resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_STONE_RES].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoFear_resistance && (final || u.uprops[DEAC_FEAR_RES].intrinsic) ) {
		sprintf(buf, "prevented from having fear resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_FEAR_RES].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoSee_invisible && (final || u.uprops[DEAC_SEE_INVIS].intrinsic) ) {
		sprintf(buf, "prevented from having see invisible");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_SEE_INVIS].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoManaleech && (final || u.uprops[DEAC_MANALEECH].intrinsic) ) {
		sprintf(buf, "prevented from having manaleech");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_MANALEECH].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoPeacevision && (final || u.uprops[DEAC_PEACEVISION].intrinsic) ) {
		sprintf(buf, "prevented from having peacevision");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_PEACEVISION].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoTelepat && (final || u.uprops[DEAC_TELEPAT].intrinsic) ) {
		sprintf(buf, "prevented from having telepathy");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_TELEPAT].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoWarning && (final || u.uprops[DEAC_WARNING].intrinsic) ) {
		sprintf(buf, "prevented from having warning");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_WARNING].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoSearching && (final || u.uprops[DEAC_SEARCHING].intrinsic) ) {
		sprintf(buf, "prevented from having automatic searching");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_SEARCHING].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoClairvoyant && (final || u.uprops[DEAC_CLAIRVOYANT].intrinsic) ) {
		sprintf(buf, "prevented from having clairvoyance");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_CLAIRVOYANT].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoInfravision && (final || u.uprops[DEAC_INFRAVISION].intrinsic) ) {
		sprintf(buf, "prevented from having infravision");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_INFRAVISION].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoDetect_monsters && (final || u.uprops[DEAC_DETECT_MONSTERS].intrinsic) ) {
		sprintf(buf, "prevented from having detect monsters");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_DETECT_MONSTERS].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoInvisible && (final || u.uprops[DEAC_INVIS].intrinsic) ) {
		sprintf(buf, "prevented from having invisibility");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_INVIS].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoDisplaced && (final || u.uprops[DEAC_DISPLACED].intrinsic) ) {
		sprintf(buf, "prevented from having displacement");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_DISPLACED].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoStealth && (final || u.uprops[DEAC_STEALTH].intrinsic) ) {
		sprintf(buf, "prevented from having stealth");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_STEALTH].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoJumping && (final || u.uprops[DEAC_JUMPING].intrinsic) ) {
		sprintf(buf, "prevented from having jumping");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_JUMPING].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoTeleport_control && (final || u.uprops[DEAC_TELEPORT_CONTROL].intrinsic) ) {
		sprintf(buf, "prevented from having teleport control");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_TELEPORT_CONTROL].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoFlying && (final || u.uprops[DEAC_FLYING].intrinsic) ) {
		sprintf(buf, "prevented from having flying");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_FLYING].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoBreathless && (final || u.uprops[DEAC_MAGICAL_BREATHING].intrinsic) ) {
		sprintf(buf, "prevented from having magical breathing");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_MAGICAL_BREATHING].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoPasses_walls && (final || u.uprops[DEAC_PASSES_WALLS].intrinsic) ) {
		sprintf(buf, "prevented from having phasing");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_PASSES_WALLS].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoSlow_digestion && (final || u.uprops[DEAC_SLOW_DIGESTION].intrinsic) ) {
		sprintf(buf, "prevented from having slow digestion");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_SLOW_DIGESTION].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoHalf_spell_damage && (final || u.uprops[DEAC_HALF_SPDAM].intrinsic) ) {
		sprintf(buf, "prevented from having half spell damage");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_HALF_SPDAM].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoHalf_physical_damage && (final || u.uprops[DEAC_HALF_PHDAM].intrinsic) ) {
		sprintf(buf, "prevented from having half physical damage");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_HALF_PHDAM].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoRegeneration && (final || u.uprops[DEAC_REGENERATION].intrinsic) ) {
		sprintf(buf, "prevented from having regeneration");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_REGENERATION].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoEnergy_regeneration && (final || u.uprops[DEAC_ENERGY_REGENERATION].intrinsic) ) {
		sprintf(buf, "prevented from having mana regeneration");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_ENERGY_REGENERATION].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoPolymorph_control && (final || u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic) ) {
		sprintf(buf, "prevented from having polymorph control");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoFast && (final || u.uprops[DEAC_FAST].intrinsic) ) {
		sprintf(buf, "prevented from having speed");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_FAST].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoReflecting && (final || u.uprops[DEAC_REFLECTING].intrinsic) ) {
		sprintf(buf, "prevented from having reflection");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_REFLECTING].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoKeen_memory && (final || u.uprops[DEAC_KEEN_MEMORY].intrinsic) ) {
		sprintf(buf, "prevented from having keen memory");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_KEEN_MEMORY].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoSight_bonus && (final || u.uprops[DEAC_SIGHT_BONUS].intrinsic) ) {
		sprintf(buf, "prevented from having a sight bonus");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_SIGHT_BONUS].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoVersus_curses && (final || u.uprops[DEAC_VERSUS_CURSES].intrinsic) ) {
		sprintf(buf, "prevented from having curse resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_VERSUS_CURSES].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoStun_resist && (final || u.uprops[DEAC_STUN_RES].intrinsic) ) {
		sprintf(buf, "prevented from having stun resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_STUN_RES].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoConf_resist && (final || u.uprops[DEAC_CONF_RES].intrinsic) ) {
		sprintf(buf, "prevented from having confusion resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_CONF_RES].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoPsi_resist && (final || u.uprops[DEAC_PSI_RES].intrinsic) ) {
		sprintf(buf, "prevented from having psi resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_PSI_RES].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoDouble_attack && (final || u.uprops[DEAC_DOUBLE_ATTACK].intrinsic) ) {
		sprintf(buf, "prevented from having double attacks");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_DOUBLE_ATTACK].intrinsic);
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && NoQuad_attack && (final || u.uprops[DEAC_QUAD_ATTACK].intrinsic) ) {
		sprintf(buf, "prevented from having quad attacks");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_QUAD_ATTACK].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoFree_action && (final || u.uprops[DEAC_FREE_ACTION].intrinsic) ) {
		sprintf(buf, "prevented from having free action");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_FREE_ACTION].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoHallu_party && (final || u.uprops[DEAC_HALLU_PARTY].intrinsic) ) {
		sprintf(buf, "prevented from hallu partying");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_HALLU_PARTY].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoDrunken_boxing && (final || u.uprops[DEAC_DRUNKEN_BOXING].intrinsic) ) {
		sprintf(buf, "prevented from drunken boxing");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_DRUNKEN_BOXING].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoStunnopathy && (final || u.uprops[DEAC_STUNNOPATHY].intrinsic) ) {
		sprintf(buf, "prevented from having stunnopathy");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_STUNNOPATHY].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoNumbopathy && (final || u.uprops[DEAC_NUMBOPATHY].intrinsic) ) {
		sprintf(buf, "prevented from having numbopathy");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_NUMBOPATHY].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoDimmopathy && (final || u.uprops[DEAC_DIMMOPATHY].intrinsic) ) {
		sprintf(buf, "prevented from having dimmopathy");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_DIMMOPATHY].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoFreezopathy && (final || u.uprops[DEAC_FREEZOPATHY].intrinsic) ) {
		sprintf(buf, "prevented from having freezopathy");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_FREEZOPATHY].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoStoned_chiller && (final || u.uprops[DEAC_STONED_CHILLER].intrinsic) ) {
		sprintf(buf, "prevented from being a stoned chiller");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_STONED_CHILLER].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoCorrosivity && (final || u.uprops[DEAC_CORROSIVITY].intrinsic) ) {
		sprintf(buf, "prevented from having corrosivity");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_CORROSIVITY].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoFear_factor && (final || u.uprops[DEAC_FEAR_FACTOR].intrinsic) ) {
		sprintf(buf, "prevented from having an increased fear factor");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_FEAR_FACTOR].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoBurnopathy && (final || u.uprops[DEAC_BURNOPATHY].intrinsic) ) {
		sprintf(buf, "prevented from having burnopathy");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_BURNOPATHY].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoSickopathy && (final || u.uprops[DEAC_SICKOPATHY].intrinsic) ) {
		sprintf(buf, "prevented from having sickopathy");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_SICKOPATHY].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoWonderlegs && (final || u.uprops[DEAC_WONDERLEGS].intrinsic) ) {
		sprintf(buf, "prevented from having wonderlegs");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_WONDERLEGS].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoGlib_combat && (final || u.uprops[DEAC_GLIB_COMBAT].intrinsic) ) {
		sprintf(buf, "prevented from having glib combat");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_GLIB_COMBAT].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoUseTheForce && (final || u.uprops[DEAC_THE_FORCE].intrinsic) ) {
		sprintf(buf, "prevented from using the force like a real jedi");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_THE_FORCE].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoCont_resist && (final || u.uprops[DEAC_CONT_RES].intrinsic) ) {
		sprintf(buf, "prevented from having contamination resistance");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_CONT_RES].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoDiscount_action && (final || u.uprops[DEAC_DISCOUNT_ACTION].intrinsic) ) {
		sprintf(buf, "prevented from having discount action");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_DISCOUNT_ACTION].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoFull_nutrient && (final || u.uprops[DEAC_FULL_NUTRIENT].intrinsic) ) {
		sprintf(buf, "prevented from having full nutrients");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_FULL_NUTRIENT].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoTechnicality && (final || u.uprops[DEAC_TECHNICALITY].intrinsic) ) {
		sprintf(buf, "prevented from having technicality");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_TECHNICALITY].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoScentView && (final || u.uprops[DEAC_SCENT_VIEW].intrinsic) ) {
		sprintf(buf, "prevented from having scent view");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_SCENT_VIEW].intrinsic);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && NoDiminishedBleeding && (final || u.uprops[DEAC_DIMINISHED_BLEEDING].intrinsic) ) {
		sprintf(buf, "prevented from having diminished bleeding");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uprops[DEAC_DIMINISHED_BLEEDING].intrinsic);
		you_are(buf);
	}

	int shieldblockrate = 0;

	if ((guaranteed || !rn2(10)) && uarms) {

		switch (uarms->otyp) {

		case SMALL_SHIELD:
			shieldblockrate = 10;
			break;
		case PAPER_SHIELD:
		case DIFFICULT_SHIELD:
			shieldblockrate = 40;
			break;
		case ICKY_SHIELD:
			shieldblockrate = 0;
			break;
		case HEAVY_SHIELD:
			shieldblockrate = 10;
			break;
		case BARRIER_SHIELD:
			shieldblockrate = 30;
			break;
		case TROLL_SHIELD:
		case MAGICAL_SHIELD:
		case SPECIAL_SHIELD:
			shieldblockrate = 20;
			break;
		case TARRIER:
			shieldblockrate = 25;
			break;
		case SHIELD_OF_PEACE:
			shieldblockrate = 20;
			break;
		case ELVEN_SHIELD:
			shieldblockrate = 20;
			if (Race_if(PM_ELF) || Race_if(PM_DROW) || Role_if(PM_ELPH) || Role_if(PM_TWELPH)) shieldblockrate += 5;
			break;
		case URUK_HAI_SHIELD:
			shieldblockrate = 22;
			if (Race_if(PM_ORC)) shieldblockrate += 5;
			break;
		case ORCISH_SHIELD:
		case ORCISH_GUARD_SHIELD:
			shieldblockrate = 18;
			if (Race_if(PM_ORC)) shieldblockrate += 5;
			break;
		case DWARVISH_ROUNDSHIELD:
			shieldblockrate = 24;
			if (Race_if(PM_DWARF)) shieldblockrate += 5;
			if (Role_if(PM_MIDGET)) shieldblockrate += 5;
			break;
		case LARGE_SHIELD:
		case SHIELD:
			shieldblockrate = 25;
			break;
		case STEEL_SHIELD:
			shieldblockrate = 30;
			break;
		case CRYSTAL_SHIELD:
		case RAPIRAPI:
		case HIDE_SHIELD:
			shieldblockrate = 35;
			break;
		case SHIELD_OF_REFLECTION:
		case SILVER_SHIELD:
		case ANCIENT_SHIELD:
		case MIRROR_SHIELD:
			shieldblockrate = 25;
			break;
		case FLAME_SHIELD:
			shieldblockrate = 30;
			break;
		case ICE_SHIELD:
			shieldblockrate = 30;
			break;
		case LIGHTNING_SHIELD:
			shieldblockrate = 30;
			break;
		case VENOM_SHIELD:
		case CHROME_SHIELD:
		case ANTISHADOW_SHIELD:
			shieldblockrate = 30;
			break;
		case SHIELD_OF_LIGHT:
			shieldblockrate = 30;
			break;
		case SHIELD_OF_MOBILITY:
			shieldblockrate = 30;
			break;

		case GRAY_DRAGON_SCALE_SHIELD:
		case SILVER_DRAGON_SCALE_SHIELD:
		case MERCURIAL_DRAGON_SCALE_SHIELD:
		case SHIMMERING_DRAGON_SCALE_SHIELD:
		case DEEP_DRAGON_SCALE_SHIELD:
		case RED_DRAGON_SCALE_SHIELD:
		case WHITE_DRAGON_SCALE_SHIELD:
		case ORANGE_DRAGON_SCALE_SHIELD:
		case BLACK_DRAGON_SCALE_SHIELD:
		case BLUE_DRAGON_SCALE_SHIELD:
		case COPPER_DRAGON_SCALE_SHIELD:
		case PLATINUM_DRAGON_SCALE_SHIELD:
		case BRASS_DRAGON_SCALE_SHIELD:
		case AMETHYST_DRAGON_SCALE_SHIELD:
		case PURPLE_DRAGON_SCALE_SHIELD:
		case DIAMOND_DRAGON_SCALE_SHIELD:
		case EMERALD_DRAGON_SCALE_SHIELD:
		case SAPPHIRE_DRAGON_SCALE_SHIELD:
		case RUBY_DRAGON_SCALE_SHIELD:
		case GREEN_DRAGON_SCALE_SHIELD:
		case GOLDEN_DRAGON_SCALE_SHIELD:
		case FEMINISM_DRAGON_SCALE_SHIELD:
		case NEGATIVE_DRAGON_SCALE_SHIELD:
		case HEROIC_DRAGON_SCALE_SHIELD:
		case STONE_DRAGON_SCALE_SHIELD:
		case CYAN_DRAGON_SCALE_SHIELD:
		case PSYCHIC_DRAGON_SCALE_SHIELD:
		case RAINBOW_DRAGON_SCALE_SHIELD:
		case BLOOD_DRAGON_SCALE_SHIELD:
		case PLAIN_DRAGON_SCALE_SHIELD:
		case SKY_DRAGON_SCALE_SHIELD:
		case WATER_DRAGON_SCALE_SHIELD:
		case MAGIC_DRAGON_SCALE_SHIELD:
		case YELLOW_DRAGON_SCALE_SHIELD:

			shieldblockrate = 23;
			break;

		case EVIL_DRAGON_SCALE_SHIELD:

			shieldblockrate = 33;
			break;

		default: impossible("Unknown type of shield (%d)", uarms->otyp);

		}

		if (!(PlayerCannotUseSkills)) {
			switch (P_SKILL(P_SHIELD)) {
				case P_BASIC: shieldblockrate += 2; break;
				case P_SKILLED: shieldblockrate += 4; break;
				case P_EXPERT: shieldblockrate += 6; break;
				case P_MASTER: shieldblockrate += 8; break;
				case P_GRAND_MASTER: shieldblockrate += 10; break;
				case P_SUPREME_MASTER: shieldblockrate += 12; break;
			}

		}

		if (uarms->oartifact == ART_LURTZ_S_WALL) shieldblockrate += 20;
		if (uarms->oartifact == ART_I_M_GETTING_HUNGRY) shieldblockrate += 20;
		if (uarms->oartifact == ART_WHANG_CLINK_CLONK) shieldblockrate += 10;
		if (uarms->oartifact == ART_BLOCKING_EXTREME) shieldblockrate += 10;
		if (uarms->oartifact == ART_CUTTING_THROUGH) shieldblockrate += 5;
		if (uwep && uwep->oartifact == ART_VEST_REPLACEMENT) shieldblockrate += 10;

		if (u.holyshield) shieldblockrate += (3 + spell_damage_bonus(SPE_HOLY_SHIELD));

		if (uarms->spe > 0) shieldblockrate += (uarms->spe * 2);

		if (uarms->cursed) shieldblockrate /= 2;
		if (uarms->blessed) shieldblockrate += 5;

		if (uarms->spe < 0) shieldblockrate += (uarms->spe * 2);

		if (uarm && uarm->oartifact == ART_WOODSTOCK) shieldblockrate += 5;
		if (Numbed) shieldblockrate -= 10;

		if (!PlayerCannotUseSkills) {
			switch (P_SKILL(P_SHIEN)) {
				case P_BASIC: shieldblockrate += 1; break;
				case P_SKILLED: shieldblockrate += 2; break;
				case P_EXPERT: shieldblockrate += 3; break;
				case P_MASTER: shieldblockrate += 4; break;
				case P_GRAND_MASTER: shieldblockrate += 5; break;
				case P_SUPREME_MASTER: shieldblockrate += 6; break;
			}
		}

		if (Conflict && shieldblockrate > 0) {
			shieldblockrate *= 2;
			shieldblockrate /= 3;
		}
		if (StrongConflict && shieldblockrate > 0) {
			shieldblockrate *= 2;
			shieldblockrate /= 3;
		}

		if (shieldblockrate < 0) shieldblockrate = 0;

		sprintf(buf, "%d%%", shieldblockrate);
		enl_msg("Your chance to block ", "is ", "was ", buf );
	}

	if ((guaranteed || !rn2(10)) && Fumbling) enl_msg("You fumble", "", "d", "");
	if ((guaranteed || !rn2(10)) && (Wounded_legs && !u.usteed)) {
		sprintf(buf, "wounded %s", makeplural(body_part(LEG)));
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", HWounded_legs);
		you_have(buf);
	}
#if defined(WIZARD) /*randomly tell this to the player outside of wizard mode too --Amy */
	if ((guaranteed || !rn2(10)) && (Wounded_legs && u.usteed && (wizard || !rn2(10) || final >= 1 ))) {
	    strcpy(buf, x_monnam(u.usteed, ARTICLE_YOUR, (char *)0, 
		    SUPPRESS_SADDLE | SUPPRESS_HALLUCINATION, FALSE));
	    *buf = highc(*buf);
	    enl_msg(buf, " has", " had", " wounded legs");
	}
#endif
	if ((guaranteed || !rn2(10)) && Sleeping) enl_msg("You ", "fall", "fell", " asleep");
	if ((guaranteed || !rn2(10)) && Hunger && !StrongHunger) enl_msg("You hunger", "", "ed", " rapidly");
	if ((guaranteed || !rn2(10)) && Hunger && StrongHunger) enl_msg("You hunger", "", "ed", " extremely rapidly");

	if ((guaranteed || !rn2(10)) && have_sleepstone()) enl_msg("You ", "are", "were", " very tired");
	if ((guaranteed || !rn2(10)) && have_cursedmagicresstone()) enl_msg("You ", "take", "took", " double damage");
	if ((guaranteed || !rn2(10)) && uamul && uamul->otyp == AMULET_OF_VULNERABILITY) enl_msg("You ", "take", "took", " extra damage");

	/*** Vision and senses ***/
	if ((guaranteed || !rn2(10)) && See_invisible && !StrongSee_invisible) enl_msg(You_, "see", "saw", " invisible");
	if ((guaranteed || !rn2(10)) && See_invisible && StrongSee_invisible) enl_msg(You_, "see", "saw", " invisible and hidden");
	if ((guaranteed || !rn2(10)) && Manaleech) enl_msg(You_, "leech", "leeched", StrongManaleech ? " a lot of mana" : " mana");
	if ((guaranteed || !rn2(10)) && Peacevision) enl_msg(You_, "recognize", "recognized", StrongPeacevision ? " peaceful creatures and other stuff" : " peaceful creatures");
	if ((guaranteed || !rn2(10)) && (Blind_telepat || Unblind_telepat) && !StrongTelepat) you_are("telepathic");
	if ((guaranteed || !rn2(10)) && (Blind_telepat || Unblind_telepat) && StrongTelepat) you_are("very telepathic");
	if ((guaranteed || !rn2(10)) && Map_amnesia) enl_msg(You_, "have", "had", " map amnesia");

	if ((guaranteed || !rn2(10)) && Hallu_party) you_are("hallu partying");
	if ((guaranteed || !rn2(10)) && Drunken_boxing) you_are("a drunken boxer");
	if ((guaranteed || !rn2(10)) && Stunnopathy) you_are("stunnopathic");
	if ((guaranteed || !rn2(10)) && Numbopathy) you_are("numbopathic");
	if ((guaranteed || !rn2(10)) && Dimmopathy) you_are("dimmopathic");
	if ((guaranteed || !rn2(10)) && Freezopathy) you_are("freezopathic");
	if ((guaranteed || !rn2(10)) && Stoned_chiller) you_are("a stoned chiller");
	if ((guaranteed || !rn2(10)) && Corrosivity) you_are("extremely corrosive");
	if ((guaranteed || !rn2(10)) && Fear_factor) you_have("an increased fear factor");
	if ((guaranteed || !rn2(10)) && Burnopathy) you_are("burnopathic");
	if ((guaranteed || !rn2(10)) && Sickopathy) you_are("sickopathic");
	if ((guaranteed || !rn2(10)) && Wonderlegs) you_have("wonderlegs");
	if ((guaranteed || !rn2(10)) && Glib_combat) you_are("a glibbery fighter");

	if ((guaranteed || !rn2(10)) && Warning) you_are(StrongWarning ? "definitely warned" : "warned");
	if ((guaranteed || !rn2(10)) && (Warn_of_mon && flags.warntype)) {
	    /* [ALI] Add support for undead */
	    int i, nth = 0;
	    unsigned long warntype = flags.warntype;
	    struct { unsigned long mask; const char *str; } warntypes[] = {
		M2_ORC,		"orcs",
		M2_DEMON,	"demons",
		M2_UNDEAD,	"undead",
	    };

	    sprintf(buf, "aware of the presence of ");
	    for(i = 0; i < SIZE(warntypes); i++)
	    {
		if (warntype & warntypes[i].mask) {
		    warntype &= ~warntypes[i].mask;
		    if (nth) {
			if (warntype)
			    strcat(buf, ", ");
			else
			    strcat(buf, " and ");
		    }
		    else
			nth = 1;
		    strcat(buf, warntypes[i].str);
		}
	    }
	    if (warntype)
	    {
		if (nth)
		    strcat(buf, " and ");
		strcat(buf, something); 
	    }
		you_are(buf);
	}
#if 0	/* ALI - dealt with under Warn_of_mon */
	if ((guaranteed || !rn2(10)) && Undead_warning) you_are("warned of undead");
#endif

	if ((guaranteed || !rn2(10)) && (Role_if(PM_ACTIVISTOR) || Race_if(PM_PEACEMAKER) )) you_are("aware of the presence of topmodels");
	if ((guaranteed || !rn2(10)) && (Role_if(PM_ACTIVISTOR) && uwep && is_quest_artifact(uwep) )) you_are("aware of the presence of unique monsters");
	/*if (uamul && uamul->otyp == AMULET_OF_UNDEAD_WARNING ) you_are("aware of the presence of undead");
	if ((guaranteed || !rn2(10)) && (uarmh && uarmh->otyp == HELMET_OF_UNDEAD_WARNING )) you_are("aware of the presence of undead");*/
	if ((guaranteed || !rn2(10)) && (uamul && uamul->otyp == AMULET_OF_POISON_WARNING )) you_are("aware of the presence of poisonous monsters");
	if ((guaranteed || !rn2(10)) && (uamul && uamul->otyp == AMULET_OF_OWN_RACE_WARNING )) you_are("aware of the presence of same-race monsters");
	if ((guaranteed || !rn2(10)) && (uamul && uamul->otyp == AMULET_OF_COVETOUS_WARNING )) you_are("aware of the presence of covetous monsters");
	if ((guaranteed || !rn2(10)) && (ublindf && ublindf->otyp == BOSS_VISOR )) you_are("aware of the presence of covetous monsters");
	if ((guaranteed || !rn2(10)) && Role_if(PM_PALADIN) ) you_are("aware of the presence of demons");
	if ((guaranteed || !rn2(10)) && Race_if(PM_VORTEX) ) you_are("aware of the presence of unsolid creatures");
	if ((guaranteed || !rn2(10)) && Race_if(PM_VORTEX) ) you_are("aware of the presence of creatures without limbs");
	if ((guaranteed || !rn2(10)) && Race_if(PM_CORTEX) ) you_are("aware of the presence of unsolid creatures");
	if ((guaranteed || !rn2(10)) && Race_if(PM_CORTEX) ) you_are("aware of the presence of creatures without limbs");
	if ((guaranteed || !rn2(10)) && Race_if(PM_LEVITATOR) ) you_are("aware of the presence of flying monsters");
	if ((guaranteed || !rn2(10)) && Race_if(PM_RODNEYAN) ) you_are("able to sense monsters possessing coveted objects");
	if ((guaranteed || !rn2(10)) && isselfhybrid) you_are("aware of the presence of strong wanderers");
	if ((guaranteed || !rn2(10)) && isselfhybrid) you_are("aware of the presence of monsters that are valid polymorph forms for monsters only");

	if ((guaranteed || !rn2(10)) && Searching) you_have(StrongSearching ? "quick autosearching" : "automatic searching");

	if ((guaranteed || !rn2(10)) && Clairvoyant) {
		sprintf(buf, StrongClairvoyant ? "highly clairvoyant" : "clairvoyant");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", HClairvoyant);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && Infravision) you_have(StrongInfravision ? "strong infravision" : "infravision");

	if ((guaranteed || !rn2(10)) && u.banishmentbeam) you_are("going to be banished");
	if ((guaranteed || !rn2(10)) && u.levelporting) you_are("going to be levelported");

	if ((guaranteed || !rn2(10)) && u.inertia) {
		sprintf(buf, "slowed by inertia");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.inertia);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && u.temprecursion) {
		sprintf(buf, "under the effect of temporary recursion");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.temprecursiontime);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && u.demagoguerecursion) {
		sprintf(buf, "temporarily playing as another role");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.demagoguerecursiontime);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && Detect_monsters) {
		sprintf(buf, StrongDetect_monsters ? "sensing all monsters" : "sensing the presence of monsters");
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", HDetect_monsters);
		you_are(buf);
	}

	if ((guaranteed || !rn2(10)) && u.umconf) you_are("going to confuse monsters");

	if (guaranteed || !rn2(10)) {
		sprintf(buf, "%d points of nutrition remaining", u.uhunger); you_have(buf);
	}

	/*** Appearance and behavior ***/
	if ((guaranteed || !rn2(10)) && Adornment) {
	    int adorn = 0;

	    if(uleft && uleft->otyp == RIN_ADORNMENT) adorn += uleft->spe;
	    if(uright && uright->otyp == RIN_ADORNMENT) adorn += uright->spe;
	    if (adorn < 0)
		you_are("poorly adorned");
	    else
		you_are("adorned");
	}
	if ((guaranteed || !rn2(10)) && Invisible) you_are(StrongInvis ? "very invisible" : "invisible");
	else if ((guaranteed || !rn2(10)) && Invis) you_are(StrongInvis ? "very invisible to others" : "invisible to others");
	/* ordinarily "visible" is redundant; this is a special case for
	   the situation when invisibility would be an expected attribute */
	else if ((guaranteed || !rn2(10)) && ((HInvis || EInvis || pm_invisible(youmonst.data)) && BInvis))
	    you_are("visible");
	if ((guaranteed || !rn2(10)) && Displaced) you_are(StrongDisplaced ? "very displaced" : "displaced");
	if ((guaranteed || !rn2(10)) && Stealth) you_are(StrongStealth ? "very stealthy" : "stealthy");
	if ((guaranteed || !rn2(10)) && StrongAggravate_monster) enl_msg("You completely aggravate", "", "d", " monsters");
	if ((guaranteed || !rn2(10)) && !StrongAggravate_monster && ExtAggravate_monster) enl_msg("You strongly aggravate", "", "d", " monsters");
	if ((guaranteed || !rn2(10)) && !StrongAggravate_monster && IntAggravate_monster) enl_msg("You aggravate", "", "d", " monsters");
	if ((guaranteed || !rn2(10)) && Conflict) enl_msg("You cause", "", "d", StrongConflict ? " lots of conflict" : " conflict");

	/*** Transportation ***/
	if ((guaranteed || !rn2(10)) && Jumping) you_can(StrongJumping ? "jump far" : "jump");
	if ((guaranteed || !rn2(10)) && Teleportation) you_have(StrongTeleportation ? "chronic teleportitis" : "teleportitis");
	if ((guaranteed || !rn2(10)) && Teleport_control) you_have(StrongTeleport_control ? "complete teleport control" : "teleport control");
	/*if (Lev_at_will) you_are("levitating, at will");
	else if ((guaranteed || !rn2(10)) && Levitation) you_are(StrongLevitation ? "floatitating" : "levitating");*/	/* without control */

	if ((guaranteed || !rn2(10)) && Lev_at_will)  {
	    sprintf(buf, "levitating, at will");
#ifdef WIZARD
	    if (wizard || !rn2(10) || final >= 1 ) sprintf(eos(buf), " (%d)", HLevitation);
#endif /*same like above --Amy */
	    you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && Levitation)  {
	    sprintf(buf, StrongLevitation ? "floatitating" : "levitating");
#ifdef WIZARD
	    if (wizard || !rn2(10) || final >= 1 ) sprintf(eos(buf), " (%d)", HLevitation);
#endif /*same like above --Amy */
	    you_are(buf);
	}

	else if ((guaranteed || !rn2(10)) && Flying) you_can(StrongFlying ? "fly up and down" : "fly");
	if ((guaranteed || !rn2(10)) && Wwalking) you_can("walk on water");
	if ((guaranteed || !rn2(10)) && Swimming) you_can(StrongSwimming ? "swim like a world champion" : "swim");        
	if ((guaranteed || !rn2(10)) && Breathless) you_can(StrongMagical_breathing ? "survive everywhere without needing to breathe" : "survive without air");
	else if ((guaranteed || !rn2(10)) && Amphibious) you_can("breathe water");
	if ((guaranteed || !rn2(10)) && Passes_walls) you_can(StrongPasses_walls ? "walk through every wall" : "walk through walls");
	/* If you die while dismounting, u.usteed is still set.  Since several
	 * places in the done() sequence depend on u.usteed, just detect this
	 * special case. */
	if ((guaranteed || !rn2(10)) && (u.usteed && (final < 2 || strcmp(killer, "riding accident")))) {
	    sprintf(buf, "riding %s", y_monnam(u.usteed));
	    you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && u.uswallow) {
	    sprintf(buf, "swallowed by %s", a_monnam(u.ustuck));
#ifdef WIZARD
	    if (wizard || !rn2(10) || final >= 1 ) sprintf(eos(buf), " (%u)", u.uswldtim);
#endif /*same like above --Amy */
	    you_are(buf);
	} else if ((guaranteed || !rn2(10)) && u.ustuck) {
	    sprintf(buf, "%s %s",
		    (Upolyd && sticks(youmonst.data)) ? "holding" : "held by",
		    a_monnam(u.ustuck));
	    you_are(buf);
	}

	/*** Physical attributes ***/
	if ((guaranteed || !rn2(10)) && u.uhitinc)
	    you_have(enlght_combatinc("to hit", u.uhitinc, final, buf));
	if ((guaranteed || !rn2(10)) && u.udaminc)
	    you_have(enlght_combatinc("damage", u.udaminc, final, buf));
	if ((guaranteed || !rn2(10)) && Slow_digestion) you_have(StrongSlow_digestion ? "extremely slow digestion" : "slower digestion");
	if ((guaranteed || !rn2(10)) && Keen_memory)
		enl_msg("Your memory ", "is", "was", StrongKeen_memory ? " very keen" : " keen");
	if ((guaranteed || !rn2(10)) && Sight_bonus) enl_msg("Your sight ", "is", "was", StrongSight_bonus ? " greatly improved" : " improved");
	if ((guaranteed || !rn2(10)) && Versus_curses) you_have(StrongVersus_curses ? "double curse resistance" : "curse resistance");

	if ((guaranteed || !rn2(10)) && Stun_resist) you_have(StrongStun_resist ? "double stun resistance" : "stun resistance");
	if ((guaranteed || !rn2(10)) && Conf_resist) you_have(StrongConf_resist ? "double confusion resistance" : "confusion resistance");
	if ((guaranteed || !rn2(10)) && Cont_resist) you_have(StrongCont_resist ? "double contamination resistance" : "contamination resistance");
	if ((guaranteed || !rn2(10)) && Discount_action) you_have(StrongDiscount_action ? "high discount action" : "discount action");
	if ((guaranteed || !rn2(10)) && Full_nutrient) you_have(StrongFull_nutrient ? "very full nutrients" : "full nutrients");
	if ((guaranteed || !rn2(10)) && Technicality) you_have(StrongTechnicality ? "greatly improved technique levels" : "improved technique levels");
	if ((guaranteed || !rn2(10)) && (ScentView || EcholocationActive)) you_have(StrongScentView ? "scent view and echolocation" : (ScentView && EcholocationActive) ? "scent view and echolocation" : EcholocationActive ? "echolocation" : "scent view");
	if ((guaranteed || !rn2(10)) && DiminishedBleeding) you_have(StrongDiminishedBleeding ? "greatly diminished bleeding" : "diminished bleeding");
	if ((guaranteed || !rn2(10)) && Psi_resist) you_have(StrongPsi_resist ? "double psi resistance" : "psi resistance");
	if ((guaranteed || !rn2(10)) && Extra_wpn_practice) enl_msg("You ", "can", "could", StrongExtra_wpn_practice ? " train skills and attributes much faster" : " train skills and attributes faster");
	if ((guaranteed || !rn2(10)) && Death_resistance) you_have("resistance to death rays");
	if ((guaranteed || !rn2(10)) && Double_attack) you_have(StrongDouble_attack ? "super double attacks" : "double attacks");
	if ((guaranteed || !rn2(10)) && Quad_attack) you_have(StrongQuad_attack ? "super quad attacks" : "quad attacks");

	if ((guaranteed || !rn2(10)) && Half_physical_damage) you_have(StrongHalf_physical_damage ? "strong physical resistance" : "physical resistance");
	if ((guaranteed || !rn2(10)) && Half_spell_damage) you_have(StrongHalf_spell_damage ? "strong spell resistance" : "spell resistance");
	if ((guaranteed || !rn2(10)) && Regeneration) enl_msg(StrongRegeneration ? "You quickly regenerate" : "You regenerate", "", "d", "");
	if ((guaranteed || !rn2(10)) && Energy_regeneration) you_have(StrongEnergy_regeneration ? "quick mana regeneration" : "mana regeneration");
	if ((guaranteed || !rn2(10)) && (u.uspellprot || Protection)) {
	    int prot = 0;

	    if(uleft && uleft->otyp == RIN_PROTECTION) prot += uleft->spe;
	    if(uright && uright->otyp == RIN_PROTECTION) prot += uright->spe;
	    if (HProtection & INTRINSIC) prot += u.ublessed;
	    prot += u.uspellprot;

	    if (prot < 0)
		you_are("ineffectively protected");
	    else
		you_are("protected");
	}
	if ((guaranteed || !rn2(10)) && Protection_from_shape_changers)
		you_are("protected from shape changers");
	if ((guaranteed || !rn2(10)) && Polymorph) you_have(StrongPolymorph ? "chronic polymorphitis" : "polymorphitis");
	if ((guaranteed || !rn2(10)) && Polymorph_control) you_have(StrongPolymorph_control ? "complete polymorph control" : "polymorph control");
	if ((guaranteed || !rn2(10)) && (u.ulycn >= LOW_PM)) {
		strcpy(buf, an(mons[u.ulycn].mname));
		you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && (Upolyd && !missingnoprotect)) {
	    if (u.umonnum == u.ulycn) strcpy(buf, "in beast form");
	    else sprintf(buf, "polymorphed into %s", an(youmonst.data->mname));
#ifdef WIZARD
	    if (wizard || !rn2(10) || final >= 1)  sprintf(eos(buf), " (%d)", u.mtimedone);
#endif
	    you_are(buf);
	}
	if ((guaranteed || !rn2(10)) && Unchanging) you_can("not change from your current form");
	if ((guaranteed || !rn2(10)) && (Fast && !SpeedBug && !u.uprops[SPEED_BUG].extrinsic && !(uarmf && uarmf->oartifact == ART_UNEVEN_ENGINE) && !(uarmf && uarmf->oartifact == ART_ERROR_IN_PLAY_ENCHANTMENT) && !have_speedbugstone())) you_are(StrongFast ? "ultra fast" : Very_fast ? "very fast" : "fast");
	if ((guaranteed || !rn2(10)) && (Fast && (SpeedBug || u.uprops[SPEED_BUG].extrinsic || (uarmf && uarmf->oartifact == ART_UNEVEN_ENGINE) || (uarmf && uarmf->oartifact == ART_ERROR_IN_PLAY_ENCHANTMENT) || have_speedbugstone()) )) you_are(StrongFast ? "ultra slow" : Very_fast ? "very slow" : "slow");
	if ((guaranteed || !rn2(10)) && (!Fast && Very_fast && !SpeedBug && !u.uprops[SPEED_BUG].extrinsic && !(uarmf && uarmf->oartifact == ART_UNEVEN_ENGINE) && !(uarmf && uarmf->oartifact == ART_ERROR_IN_PLAY_ENCHANTMENT) && !have_speedbugstone())) you_are("very fast");
	if ((guaranteed || !rn2(10)) && (!Fast && Very_fast && (SpeedBug || u.uprops[SPEED_BUG].extrinsic || (uarmf && uarmf->oartifact == ART_UNEVEN_ENGINE) || (uarmf && uarmf->oartifact == ART_ERROR_IN_PLAY_ENCHANTMENT) || have_speedbugstone()) )) you_are("very slow");
	if ((guaranteed || !rn2(10)) && Reflecting) you_have(StrongReflecting ? "powerful reflection" : "reflection");
	if ((guaranteed || !rn2(10)) && Free_action) you_have(StrongFree_action ? "very free action" : "free action");
	if ((guaranteed || !rn2(10)) && SustainAbilityOn) you_have("fixed abilities");
	if ((guaranteed || !rn2(10)) && (uamul && uamul->otyp == AMULET_VERSUS_STONE))
		enl_msg("You ", "will be", "would have been", " depetrified");
	if ((guaranteed || !rn2(10)) && Lifesaved)
		enl_msg("Your life ", "will be", "would have been", " saved");
	if ((guaranteed || !rn2(10)) && Second_chance)
  	  enl_msg("You ","will be", "would have been"," given a second chance");
	if ((guaranteed || !rn2(10)) && u.metalguard)	
  	  enl_msg("The next damage you ","take will be", "took would have been"," nullified");
	if ((guaranteed || !rn2(10)) && u.twoweap) {
	    if (uwep && uswapwep)
		sprintf(buf, "wielding two weapons at once");
	    else if (uwep || uswapwep)
		sprintf(buf, "fighting with a weapon and your %s %s",
			uwep ? "left" : "right", body_part(HAND));
	    else
		sprintf(buf, "fighting with two %s",
			makeplural(body_part(HAND)));
	    you_are(buf);
	}

	/*** Miscellany ***/
	if ((guaranteed || !rn2(10)) && Luck) {
	    ltmp = abs((int)Luck);
	    sprintf(buf, "%s%slucky",
		    ltmp >= 10 ? "extremely " : ltmp >= 5 ? "very " : "",
		    Luck < 0 ? "un" : "");
#ifdef WIZARD
	    if (wizard || !rn2(10) || final >= 1 ) sprintf(eos(buf), " (%d)", Luck);
#endif /*sometimes show your actual luck too --Amy */
	    you_are(buf);
	}
#ifdef WIZARD
	 else if ((guaranteed || !rn2(10)) && (wizard || !rn2(10) || final >= 1)) enl_msg("Your luck ", "is", "was", " zero");
#endif
	if ((guaranteed || !rn2(10)) && (u.moreluck > 0)) you_have("extra luck");
	else if ((guaranteed || !rn2(10)) && (u.moreluck < 0)) you_have("reduced luck");
	if ((guaranteed || !rn2(10)) && (carrying(LUCKSTONE) || stone_luck(TRUE))) {
	    ltmp = stone_luck(FALSE);
	    if (ltmp <= 0)
		enl_msg("Bad luck ", "does", "did", " not time out for you");
	    if (ltmp >= 0)
		enl_msg("Good luck ", "does", "did", " not time out for you");
	}

	/* KMH, balance patch -- healthstones affect health */
	if ((guaranteed || !rn2(10)) && u.uhealbonus)
	{
		sprintf(buf, "%s health", u.uhealbonus > 0 ? "extra" :
			"reduced");
#ifdef WIZARD
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.uhealbonus);
#endif
		you_have(buf);
	}
#ifdef WIZARD
	 else if (wizard) enl_msg("Your health bonus ", "is", "was", " zero");
#endif

	if ((guaranteed || !rn2(10)) && recalc_mana() )
	{
		sprintf(buf, "%s mana", recalc_mana() > 0 ? "extra" :
			"reduced");
#ifdef WIZARD
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", recalc_mana() );
#endif
		you_have(buf);
	}
#ifdef WIZARD
	 else if (wizard) enl_msg("Your mana bonus ", "is", "was", " zero");
#endif
	if (guaranteed || !rn2(10)) {
		sprintf(buf, "spell retention for remembering %d spells", urole.spelrete);
		you_have(buf);
	}

	if ((guaranteed || !rn2(10)) && u.ugangr) {
	    sprintf(buf, " %sangry with you",
		    u.ugangr > 6 ? "extremely " : u.ugangr > 3 ? "very " : "");
#ifdef WIZARD
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.ugangr);
#endif
	    enl_msg(u_gname(), " is", " was", buf);
	} /*else*/
	    /*
	     * We need to suppress this when the game is over, because death
	     * can change the value calculated by can_pray(), potentially
	     * resulting in a false claim that you could have prayed safely.
	     */
	  if (guaranteed || !rn2(10)) {

	    /* "can [not] safely pray" vs "could [not] have safely prayed" */

	    sprintf(buf, "%s%ssafely pray%s", u.ugangr ? "not " : can_pray(FALSE) ? "" : "not ",
		    final ? "have " : "", final ? "ed" : "");

	    /*sprintf(buf, "%ssafely pray", can_pray(FALSE) ? "" : "not ");*/

#ifdef WIZARD
	    if (wizard || (!rn2(10)) || final >= 1 ) sprintf(eos(buf), " (%d)", u.ublesscnt);
#endif

	    you_can(buf);




#if 0	/* WAC -- replaced by techniques */
/*	    sprintf(buf, "%s%suse%s your special", !u.unextuse ? "" : "not ",
		    final ? "have " : "", final ? "d" : "");*/
	    sprintf(buf, "%suse your special", !u.unextuse ? "" : "not ");
#ifdef WIZARD
	    if (wizard) sprintf(eos(buf), " (%d)", u.unextuse);
#endif
	    you_can(buf);
#endif
	}

    {
	const char *p;

	buf[0] = '\0';
	if (final < 2) {    /* still in progress, or quit/escaped/ascended */
	    p = "survived after being killed ";
	    switch (u.umortality) {
	    case 0:  p = !final ? (char *)0 : "survived";  break;
	    case 1:  strcpy(buf, "once");  break;
	    case 2:  strcpy(buf, "twice");  break;
	    case 3:  strcpy(buf, "thrice");  break;
	    default: sprintf(buf, "%d times", u.umortality);
		     break;
	    }
	} else {		/* game ended in character's death */
	    p = "are dead";
	    switch (u.umortality) {
	    case 0:  impossible("dead without dying?");
	    case 1:  break;			/* just "are dead" */
	    default: sprintf(buf, " (%d%s time!)", u.umortality,
			     ordin(u.umortality));
		     break;
	    }
	}
	if (p) enl_msg(You_, "have been killed ", p, buf);
    }

	display_nhwindow(en_win, TRUE);
	destroy_nhwindow(en_win);
	return;
}

const char * const encx_stat[] = {
	"",
	"burdened",
	"stressed",
	"strained",
	"overtaxed",
	"overloaded"
};

const char *hux_stat[] = {
	"satiated",
	"        ",
	"hungry  ",
	"weak    ",
	"fainting",
	"fainted ",
	"starved "
};

#ifdef DUMP_LOG
void
dump_enlightenment(final)
int final;
{
	int ltmp;
	char buf[BUFSZ];
	char buf2[BUFSZ];
	const char *enc_stat[] = { /* copied from botl.c */
	     "",
	     "burdened",
	     "stressed",
	     "strained",
	     "overtaxed",
	     "overloaded"
	};
	char *youwere = "  You were ";
	char *youhave = "  You have ";
	char *youhad  = "  You had ";
	char *youcould = "  You could ";
	struct permonst *ptr;

	dump("", "Final attributes");

	if (flags.zapem) dump(youwere, "playing in ZAPM mode");
	if (flags.wonderland) dump(youwere, "playing in wonderland mode");
	if (flags.lostsoul && !flags.uberlostsoul) dump(youwere, "playing in lost soul mode");
	if (flags.uberlostsoul) dump(youwere, "playing in uber lost soul mode");
	if (flags.gehenna) dump(youwere, "playing in gehenna mode");
	if (flags.dudley) dump(youwere, "playing in dudley mode");
	if (flags.iwbtg) dump(youwere, "playing in IWBTG mode");
	if (flags.elmstreet) dump(youwere, "playing in elm street mode");
	if (flags.blindfox) dump(youwere, "playing in blindfox mode");
	if (flags.hippie) dump(youwere, "playing in hippie mode");
	if (flags.gmmode) dump(youwere, "playing in game master mode");
	if (flags.supergmmode) dump(youwere, "playing in super game master mode");

	if (u.uevent.uhand_of_elbereth) {
	    static const char * const hofe_titles[3] = {
				"the Hand of Elbereth",
				"the Envoy of Balance",
				"the Glory of Arioch"
	    };
	    dump(youwere,
		(char *)hofe_titles[u.uevent.uhand_of_elbereth - 1]);
	}

	if (u.ualign.record >= 20)
		dump(youwere, "piously aligned");
	else if (u.ualign.record > 13)
	    dump(youwere, "devoutly aligned");
	else if (u.ualign.record > 8)
	    dump(youwere, "fervently aligned");
	else if (u.ualign.record > 3)
	    dump(youwere, "stridently aligned");
	else if (u.ualign.record == 3)
	    dump(youwere, "aligned");
	else if (u.ualign.record > 0)
	    dump(youwere, "haltingly aligned");
	else if (u.ualign.record == 0)
	    dump(youwere, "nominally aligned");
	else if (u.ualign.record >= -3)	dump(youhave, "strayed");
	else if (u.ualign.record >= -8)	dump(youhave, "sinned");
	else dump("  You have ", "transgressed");

	sprintf(buf, " %d", u.ualign.record);
	dump("  Your alignment was ", buf);

	sprintf(buf, " %d", u.alignlim);
	dump("  Your max alignment was ", buf);

	sprintf(buf, " %d sins", u.ualign.sins);
	dump("  You carried ", buf);

	if (u.impossibleproperty >= 0) {
		sprintf(buf, " %s", enchname(u.impossibleproperty) );
		dump("  You could not have this property at all:", buf);
	}

	if (u.nonextrinsicproperty >= 0) {
		sprintf(buf, " %s", enchname(u.nonextrinsicproperty) );
		dump("  You could not have this property extrinsically:", buf);
	}

	if (u.nonintrinsicproperty >= 0) {
		sprintf(buf, " %s", enchname(u.nonintrinsicproperty) );
		dump("  You could not have this property intrinsically:", buf);
	}

	if (u.idscrollpenalty > 100) {
		sprintf(buf, " %d", u.idscrollpenalty);
		dump("  Scroll identification only worked 100 times out of", buf);
	}

	if (u.idpotionpenalty > 3) {
		sprintf(buf, " %d", u.idpotionpenalty);
		dump("  Potion identification only worked 3 times out of", buf);
	}

	if (u.idringpenalty > 4) {
		sprintf(buf, " %d", u.idringpenalty);
		dump("  Ring identification only worked 4 times out of", buf);
	}

	if (u.idamuletpenalty > 15) {
		sprintf(buf, " %d", u.idamuletpenalty);
		dump("  Amulet identification only worked 15 times out of", buf);
	}
	if (u.idimplantpenalty > 15) {
		sprintf(buf, " %d", u.idimplantpenalty);
		dump("  Implant identification only worked 1 times out of", buf);
	}

	if (u.idwandpenalty > 3) {
		sprintf(buf, " %d", u.idwandpenalty);
		dump("  Wand identification only worked 3 times out of", buf);
	}

	if (u.idarmorpenalty > 15) {
		sprintf(buf, " %d", u.idarmorpenalty);
		dump("  Armor identification only worked 15 times out of", buf);
	}

	if (u.idspellbookpenalty > 2) {
		sprintf(buf, " %d", u.idspellbookpenalty);
		dump("  Spellbook identification only worked 2 times out of", buf);
	}

	if (u.idgempenalty > 100) {
		sprintf(buf, " %d", u.idgempenalty);
		dump("  Gem identification only worked 100 times out of", buf);
	}

	if (u.idtoolpenalty > 5) {
		sprintf(buf, " %d", u.idtoolpenalty);
		dump("  Tool identification only worked 5 times out of", buf);
	}

	if (u.musableremovechance) {
		sprintf(buf, " %d%% chance", 100 - u.musableremovechance);
		dump("  Monsters only dropped their musable items with", buf);
	}

	if (u.equipmentremovechance) {
		sprintf(buf, " %d%% chance", 100 - u.equipmentremovechance);
		dump("  Monsters only dropped their equipment with", buf);
	}

	if (u.bookspawnchance) {
		sprintf(buf, " %d%%", 100 - u.bookspawnchance);
		dump("  Book drop chance was reduced to", buf);
	}

	if (u.scrollspawnchance) {
		sprintf(buf, " %d%%", 100 - u.scrollspawnchance);
		dump("  Scroll drop chance was reduced to", buf);
	}

	if (u.antimusablebias) {
		sprintf(buf, " %d%%", u.antimusablebias);
		dump("  Musable item generation frequency had a negative bias of", buf);
	}

	if (u.ringspawnchance) {
		sprintf(buf, " %d%%", 100 - u.ringspawnchance);
		dump("  Ring drop chance was reduced to", buf);
	}

	if (u.wandspawnchance) {
		sprintf(buf, " %d%%", 100 - u.wandspawnchance);
		dump("  Wand drop chance was reduced to", buf);
	}

	if (u.amuletspawnchance) {
		sprintf(buf, " %d%%", 100 - u.amuletspawnchance);
		dump("  Amulet drop chance was reduced to", buf);
	}

	if (u.implantspawnchance) {
		sprintf(buf, " %d%%", 100 - u.implantspawnchance);
		dump("  Implant drop chance was reduced to", buf);
	}

	if (u.potionspawnchance) {
		sprintf(buf, " %d%%", 100 - u.potionspawnchance);
		dump("  Potion drop chance was reduced to", buf);
	}

	if (u.weaponspawnchance) {
		sprintf(buf, " %d%%", 100 - u.weaponspawnchance);
		dump("  Weapon drop chance was reduced to", buf);
	}

	if (u.armorspawnchance) {
		sprintf(buf, " %d%%", 100 - u.armorspawnchance);
		dump("  Armor drop chance was reduced to", buf);
	}

	if (u.toolspawnchance) {
		sprintf(buf, " %d%%", 100 - u.toolspawnchance);
		dump("  Tool drop chance was reduced to", buf);
	}

	if (u.foodspawnchance) {
		sprintf(buf, " %d%%", 100 - u.foodspawnchance);
		dump("  Food drop chance was reduced to", buf);
	}

	if (u.concealitemchance) {
		sprintf(buf, " %d%% of the time only", 100 - u.concealitemchance);
		dump("  Concealing monsters were spawned underneath items", buf);
	}

	if (u.usefulitemchance) {
		sprintf(buf, " %d%% chance", 100 - u.usefulitemchance);
		dump("  Monster death drops spawned with only", buf);
	}

	if (u.untrainableskill) {
		sprintf(buf, " %s", P_NAME(u.untrainableskill));
		dump("  The following skill could not be trained at all:", buf);
	}

	if (u.halfspeedskill) {
		sprintf(buf, " %s", P_NAME(u.halfspeedskill));
		dump("  The following skill was trained at half the usual rate:", buf);
	}

	if (u.fifthspeedskill) {
		sprintf(buf, " %s", P_NAME(u.fifthspeedskill));
		dump("  The following skill was trained at one fifth the usual rate:", buf);
	}

	if (u.basiclimitskill) {
		sprintf(buf, " %s", P_NAME(u.basiclimitskill));
		dump("  The following skill was limited to basic proficiency:", buf);
	}

	if (u.skilledlimitskill) {
		sprintf(buf, " %s", P_NAME(u.skilledlimitskill));
		dump("  The following skill was limited to skilled proficiency:", buf);
	}

	if (u.expertlimitskill) {
		sprintf(buf, " %s", P_NAME(u.expertlimitskill));
		dump("  The following skill was limited to expert proficiency:", buf);
	}

	if (u.earlytrainingskill) {
		sprintf(buf, " %s (turn %d)", P_NAME(u.earlytrainingskill), u.earlytrainingtimer);
		dump("  The following skill became untrainable if you tried to train it too early:", buf);
	}

	if (u.earlytrainingblown) {
		sprintf(buf, "blown your chance of training the %s skill", P_NAME(u.earlytrainingskill));
		dump(youhad, buf);
	}

	if (u.frtrainingskill) {
		sprintf(buf, " %s (turn %d)", P_NAME(u.frtrainingskill), u.frtrainingtimer);
		dump("  The following skill became untrainable if you tried to train it too early:", buf);
	}

	if (u.frtrainingblown) {
		sprintf(buf, "blown your chance of training the %s skill", P_NAME(u.frtrainingskill));
		dump(youhad, buf);
	}

	if (u.latetrainingskill) {
		sprintf(buf, " %s (turn %d)", P_NAME(u.latetrainingskill), u.latetrainingtimer);
		dump("  The following skill became untrainable after a while:", buf);
	}

	if (u.lavtrainingskill) {
		sprintf(buf, " %s (turn %d)", P_NAME(u.lavtrainingskill), u.lavtrainingtimer);
		dump("  The following skill couldn't be trained before a certain turn number is reached:", buf);
	}

	if (u.slowtrainingskill) {
		sprintf(buf, " %s", P_NAME(u.slowtrainingskill));
		dump("  The following skill became harder to train at higher skill levels:", buf);
	}

	if (u.stickycursechance) {
		sprintf(buf, " %d%%", u.stickycursechance);
		dump("  Sticky curses were more common by", buf);
	}

	if (u.heavycursechance) {
		sprintf(buf, " %d%%", u.heavycursechance);
		dump("  Heavy curses were more common by", buf);
	}

	if (u.primecursechance) {
		sprintf(buf, " %d%%", u.primecursechance);
		dump("  Prime curses were more common by", buf);
	}

	if (u.genericcursechance) {
		sprintf(buf, " %d%%", u.genericcursechance);
		dump("  Items were more likely to be generated cursed by", buf);
	}

	sprintf(buf, " %d", nartifact_exist() );
	dump("  Number of artifacts generated was ", buf);

	sprintf(buf, " %d", u.fakeartifacts );
	dump("  Number of fake artifacts generated was ", buf);

	if (u.legscratching > 1) { 
		sprintf(buf, " %d", u.legscratching - 1);
		dump("  Your leg damage was ", buf);
	}

	if (u.youpolyamount > 0) {
		sprintf(buf, " %d", u.youpolyamount);
		dump("  Your remaining amount of free polymorphs was ", buf);
	}

	if (u.antimagicshell > 0) {
		sprintf(buf, " %d more turns", u.antimagicshell);
		dump("  Your antimagic shell would have lasted", buf);
	}

	if (Role_if(PM_UNBELIEVER)) {
		dump("  You were ", "producing a permanent antimagic shell");
	}

	sprintf(buf, " turn %d", u.monstertimeout);
	dump("  Monster spawn increase started at ", buf);

	sprintf(buf, " turn %d", u.monstertimefinish);
	dump("  Monster spawn increase reached its maximum at ", buf);
	dump("  In this game, Eevee's evolution was ", mons[u.eeveelution].mname );
	/* these two are tied together because the monstertimefinish variable defines the evolution --Amy */

	sprintf(buf, " turn %d", u.ascensiontimelimit);
	dump("  Your limit for ascension was at ", buf);

	dump("  The monster class that cannot be genocided was ", monexplain[u.ungenocidable] );

	if (u.alwaysinvisible) {
		dump("  Today, this monster class was always generated invisible: ", monexplain[u.alwaysinvisible] );
	}

	if (u.alwayshidden) {
		dump("  Today, this monster class was always generated hidden: ", monexplain[u.alwayshidden] );
	}

	if (u.freqmonsterbonus) {
		sprintf(buf, "spawned more often: %s (freq bonus %d)", monexplain[u.frequentmonster], u.freqmonsterbonus);
		dump("  The monster class that was ", buf );
	}

	if (u.freqcolorbonus) {
		sprintf(buf, "spawned more often: %s (freq bonus %d)", c_obj_colors[u.frequentcolor], u.freqcolorbonus);
		dump("  The monster color that was ", buf );
	}

	ptr = &mons[u.frequentspecies];
	sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies2];
	sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus2);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies3];
	sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus3);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies4];
	sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus4);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies5];
	sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus5);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies6];
	sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus6);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies7];
	sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus7);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies8];
	sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus8);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies9];
	sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus9);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies10];
	sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus10);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies11];
	sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus11);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies12];
	sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus12);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies13];
	sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus13);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies14];
	sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus14);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies15];
	sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus15);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies16];
	sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus16);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies17];
	sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus17);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies18];
	sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus18);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies19];
	sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus19);
	dump("  The monster species that ", buf );

	ptr = &mons[u.frequentspecies20];
	sprintf(buf, "spawned more often: %s (freq bonus %d)", ptr->mname, u.freqspeciesbonus20);
	dump("  The monster species that ", buf );

	sprintf(buf, "more common: %s (freq bonus %d)", montraitname(u.frequenttrait1), u.freqtraitbonus1);
	dump("  The monster trait that was ", buf );

	if (u.frequenttrait2) {
		sprintf(buf, "more common: %s (freq bonus %d)", montraitname(u.frequenttrait2), u.freqtraitbonus2);
		dump("  The monster trait that was ", buf );
	}

	if (u.frequenttrait3) {
		sprintf(buf, "more common: %s (freq bonus %d)", montraitname(u.frequenttrait3), u.freqtraitbonus3);
		dump("  The monster trait that was ", buf );
	}

	if (u.frequenttrait4) {
		sprintf(buf, "more common: %s (freq bonus %d)", montraitname(u.frequenttrait4), u.freqtraitbonus4);
		dump("  The monster trait that was ", buf );
	}

	if (u.frequenttrait5) {
		sprintf(buf, "more common: %s (freq bonus %d)", montraitname(u.frequenttrait5), u.freqtraitbonus5);
		dump("  The monster trait that was ", buf );
	}

	if (u.frequenttrait6) {
		sprintf(buf, "more common: %s (freq bonus %d)", montraitname(u.frequenttrait6), u.freqtraitbonus6);
		dump("  The monster trait that was ", buf );
	}

	if (u.frequenttrait7) {
		sprintf(buf, "more common: %s (freq bonus %d)", montraitname(u.frequenttrait7), u.freqtraitbonus7);
		dump("  The monster trait that was ", buf );
	}

	if (u.frequenttrait8) {
		sprintf(buf, "more common: %s (freq bonus %d)", montraitname(u.frequenttrait8), u.freqtraitbonus8);
		dump("  The monster trait that was ", buf );
	}

	if (u.frequenttrait9) {
		sprintf(buf, "more common: %s (freq bonus %d)", montraitname(u.frequenttrait9), u.freqtraitbonus9);
		dump("  The monster trait that was ", buf );
	}

	if (u.frequenttrait10) {
		sprintf(buf, "more common: %s (freq bonus %d)", montraitname(u.frequenttrait10), u.freqtraitbonus10);
		dump("  The monster trait that was ", buf );
	}

	ptr = &mons[u.nospawnspecies];
	sprintf(buf, "never randomly spawned: %s", ptr->mname);
	dump("  The monster species that ", buf );

	ptr = &mons[u.nospawnspecies2];
	sprintf(buf, "never randomly spawned: %s", ptr->mname);
	dump("  The monster species that ", buf );

	ptr = &mons[u.nospawnspecies3];
	sprintf(buf, "never randomly spawned: %s", ptr->mname);
	dump("  The monster species that ", buf );

	ptr = &mons[u.nospawnspecies4];
	sprintf(buf, "never randomly spawned: %s", ptr->mname);
	dump("  The monster species that ", buf );

	ptr = &mons[u.nospawnspecies5];
	sprintf(buf, "never randomly spawned: %s", ptr->mname);
	dump("  The monster species that ", buf );

	ptr = &mons[u.nospawnspecies6];
	sprintf(buf, "never randomly spawned: %s", ptr->mname);
	dump("  The monster species that ", buf );

	ptr = &mons[u.nospawnspecies7];
	sprintf(buf, "never randomly spawned: %s", ptr->mname);
	dump("  The monster species that ", buf );

	ptr = &mons[u.nospawnspecies8];
	sprintf(buf, "never randomly spawned: %s", ptr->mname);
	dump("  The monster species that ", buf );

	ptr = &mons[u.nospawnspecies9];
	sprintf(buf, "never randomly spawned: %s", ptr->mname);
	dump("  The monster species that ", buf );

	ptr = &mons[u.nospawnspecies10];
	sprintf(buf, "never randomly spawned: %s", ptr->mname);
	dump("  The monster species that ", buf );

	if (u.speedymonster) {
		sprintf(buf, "always generated with extra speed: %s", monexplain[u.speedymonster]);
		dump("  The monster class that ", buf );
	}

	if (u.musemonster) {
		sprintf(buf, "more likely to generate with musable stuff: %s", monexplain[u.musemonster]);
		dump("  The monster class that ", buf );
	}

	if (u.minimalism) {
		sprintf(buf, " %d", u.minimalism);
		dump("  Items generated only 1 time in", buf);
	}

	sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable].oc_name, obj_descr[u.unobtainable].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable2].oc_name, obj_descr[u.unobtainable2].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable3].oc_name, obj_descr[u.unobtainable3].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable4].oc_name, obj_descr[u.unobtainable4].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable5].oc_name, obj_descr[u.unobtainable5].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable6].oc_name, obj_descr[u.unobtainable6].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable7].oc_name, obj_descr[u.unobtainable7].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable8].oc_name, obj_descr[u.unobtainable8].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable9].oc_name, obj_descr[u.unobtainable9].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "never generated: %s (%s)", obj_descr[u.unobtainable10].oc_name, obj_descr[u.unobtainable10].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	if (u.unobtainablegeno != -1) {
		sprintf(buf, "%s (%s)", obj_descr[u.unobtainablegeno].oc_name, obj_descr[u.unobtainablegeno].oc_descr);
		dump("  You had genocided this item: ", buf );
	}

	sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable].oc_name, obj_descr[u.veryobtainable].oc_descr, u.veryobtainableboost);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable2].oc_name, obj_descr[u.veryobtainable2].oc_descr, u.veryobtainableboost2);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable3].oc_name, obj_descr[u.veryobtainable3].oc_descr, u.veryobtainableboost3);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable4].oc_name, obj_descr[u.veryobtainable4].oc_descr, u.veryobtainableboost4);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable5].oc_name, obj_descr[u.veryobtainable5].oc_descr, u.veryobtainableboost5);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable6].oc_name, obj_descr[u.veryobtainable6].oc_descr, u.veryobtainableboost6);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable7].oc_name, obj_descr[u.veryobtainable7].oc_descr, u.veryobtainableboost7);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable8].oc_name, obj_descr[u.veryobtainable8].oc_descr, u.veryobtainableboost8);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable9].oc_name, obj_descr[u.veryobtainable9].oc_descr, u.veryobtainableboost9);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable10].oc_name, obj_descr[u.veryobtainable10].oc_descr, u.veryobtainableboost10);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable11].oc_name, obj_descr[u.veryobtainable11].oc_descr, u.veryobtainableboost11);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable12].oc_name, obj_descr[u.veryobtainable12].oc_descr, u.veryobtainableboost12);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable13].oc_name, obj_descr[u.veryobtainable13].oc_descr, u.veryobtainableboost13);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable14].oc_name, obj_descr[u.veryobtainable14].oc_descr, u.veryobtainableboost14);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable15].oc_name, obj_descr[u.veryobtainable15].oc_descr, u.veryobtainableboost15);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable16].oc_name, obj_descr[u.veryobtainable16].oc_descr, u.veryobtainableboost16);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable17].oc_name, obj_descr[u.veryobtainable17].oc_descr, u.veryobtainableboost17);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable18].oc_name, obj_descr[u.veryobtainable18].oc_descr, u.veryobtainableboost18);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable19].oc_name, obj_descr[u.veryobtainable19].oc_descr, u.veryobtainableboost19);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "generated more often: %s (%s, freq bonus %d)", obj_descr[u.veryobtainable20].oc_name, obj_descr[u.veryobtainable20].oc_descr, u.veryobtainableboost20);
	dump("  The RNG hath decreed that this item was ", buf );

	if (u.veryobtainclass1 > 0) {
		sprintf(buf, "generated more often: %s (freq bonus %d)", names[u.veryobtainclass1], u.veryobtainclass1boost);
		dump("  The RNG hath decreed that this item class was ", buf );
	}

	if (u.veryobtainclass2 > 0) {
		sprintf(buf, "generated more often: %s (freq bonus %d)", names[u.veryobtainclass2], u.veryobtainclass2boost);
		dump("  The RNG hath decreed that this item class was ", buf );
	}

	if (u.veryobtainclass3 > 0) {
		sprintf(buf, "generated more often: %s (freq bonus %d)", names[u.veryobtainclass3], u.veryobtainclass3boost);
		dump("  The RNG hath decreed that this item class was ", buf );
	}

	sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem].oc_name, obj_descr[u.alwayscurseditem].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem2].oc_name, obj_descr[u.alwayscurseditem2].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem3].oc_name, obj_descr[u.alwayscurseditem3].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem4].oc_name, obj_descr[u.alwayscurseditem4].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem5].oc_name, obj_descr[u.alwayscurseditem5].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem6].oc_name, obj_descr[u.alwayscurseditem6].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem7].oc_name, obj_descr[u.alwayscurseditem7].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem8].oc_name, obj_descr[u.alwayscurseditem8].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem9].oc_name, obj_descr[u.alwayscurseditem9].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "always generated cursed: %s (%s)", obj_descr[u.alwayscurseditem10].oc_name, obj_descr[u.alwayscurseditem10].oc_descr);
	dump("  The RNG hath decreed that this item was ", buf );

	sprintf(buf, "always invisible: %s", defsyms[trap_to_defsym(u.invisotrap)].explanation);
	dump("  The RNG hath decreed that this trap was ", buf );

	sprintf(buf, "generated more often: %s (freq bonus %d)", defsyms[trap_to_defsym(u.frequenttrap)].explanation, u.freqtrapbonus);
	dump("  The RNG hath decreed that this trap was ", buf );

	sprintf(buf, "for 1 in %d traps", u.traprandomizing);
	dump("  Trap randomization happened ", buf );

	if (u.trapxtradiff) {
		sprintf(buf, "increased by %d", u.trapxtradiff);
		dump("  Trap difficulty was ", buf );
	}

	sprintf(buf, " turn %d", u.next_check);
	dump("  Next attribute increase check would have come at", buf);

	sprintf(buf, " %d", AEXE(A_STR));
	dump("  Strength training was", buf);

	sprintf(buf, " %d", AEXE(A_DEX));
	dump("  Dexterity training was", buf);

	sprintf(buf, " %d", AEXE(A_WIS));
	dump("  Wisdom training was", buf);

	sprintf(buf, " %d", AEXE(A_CON));
	dump("  Constitution training was", buf);

	sprintf(buf, " %d", u.usanity);
	dump("  Your sanity was", buf);

	if (u.negativeprotection) {
		sprintf(buf, " %d", u.negativeprotection);
		dump("  Your protection was reduced by", buf);
	}

	if (u.tremblingamount) {
		sprintf(buf, " %d", u.tremblingamount);
		dump("  You were tremling, reducing your to-hit and spellcasting chances. Amount:", buf);
	}

	if (u.extralives == 1) {
		dump(youhad, "an extra life");
	}

	if (u.extralives > 1) {
		sprintf(buf, " %d", u.extralives);
		dump("  Your amount of extra lives was", buf);
	}

	if (u.chokhmahdamage) {
		sprintf(buf, " %d", u.chokhmahdamage);
		dump("  The escalating chokhmah attack damage was", buf);
	}

	/*** Resistances to troubles ***/
	if (Fire_resistance) dump(youwere, StrongFire_resistance ? "doubly fire resistant" : "fire resistant");
	if (Cold_resistance) dump(youwere, StrongCold_resistance ? "doubly cold resistant" : "cold resistant");
	if (Sleep_resistance) dump(youwere, StrongSleep_resistance ? "doubly sleep resistant" : "sleep resistant");
	if (Disint_resistance) dump(youwere, StrongDisint_resistance ? "doubly disintegration-resistant" : "disintegration-resistant");
	if (Shock_resistance) dump(youwere, StrongShock_resistance ? "doubly shock resistant" : "shock resistant");
	if (Poison_resistance) dump(youwere, StrongPoison_resistance ? "doubly poison resistant" : "poison resistant");
	if (Drain_resistance) dump(youwere, StrongDrain_resistance ? "doubly level-drain resistant" : "level-drain resistant");
	if (Sick_resistance) dump(youwere, StrongSick_resistance ? "completely immune to sickness" : IntSick_resistance ? "immune to sickness" : "resistant to sickness");
	if (Antimagic) dump(youwere, StrongAntimagic ? "doubly magic-protected" : "magic-protected");
	if (Acid_resistance) dump(youwere, StrongAcid_resistance ? "doubly acid resistant" : "acid resistant");
	if (Fear_resistance) dump(youwere, StrongFear_resistance ? "highly resistant to fear" : "resistant to fear");
	if (Stone_resistance) dump(youwere, StrongStone_resistance ? "completely immune to petrification" : IntStone_resistance ? "immune to petrification" : "petrification resistant");
	if (Invulnerable) dump(youwere, "invulnerable");
	if (u.urealedibility) {
		sprintf(buf, "recognize detrimental food");
	      sprintf(eos(buf), " (%d)", u.urealedibility);
		dump(youcould, buf);
	}

	/*** Troubles ***/

	if (multi) {
		sprintf(buf, "paralyzed");
	      sprintf(eos(buf), " (%d)", multi);
		dump(youwere, buf);
	}

	if (Halluc_resistance) 	dump("  ", "You resisted hallucinations");
	if (Hallucination) {
		if (HeavyHallu) sprintf(buf, "badly hallucinating");
		else sprintf(buf, "hallucinating");
	      sprintf(eos(buf), " (%d)", HHallucination);
		dump(youwere, buf);
	}
	if (Stunned) {
		if (HeavyStunned) sprintf(buf, "badly stunned");
		else sprintf(buf, "stunned");
	      sprintf(eos(buf), " (%d)", HStun);
		dump(youwere, buf);
	}
	if (Confusion) {
		if (HeavyConfusion) sprintf(buf, "badly confused");
		else sprintf(buf, "confused");
	      sprintf(eos(buf), " (%d)", HConfusion);
		dump(youwere, buf);
	}
	if (Blinded) {
		if (HeavyBlind) sprintf(buf, "badly blinded");
		else sprintf(buf, "blinded");
	      sprintf(eos(buf), " (%d)", Blinded);
		dump(youwere, buf);
	}
	if (Sick) {
		sprintf(buf, "sick");
			if (u.usick_type & SICK_VOMITABLE) sprintf(eos(buf), " from food poisoning");
			if (u.usick_type & SICK_NONVOMITABLE) sprintf(eos(buf), " from illness");
	      sprintf(eos(buf), " (%d)", Sick);
		dump(youwere, buf);
	}
	if (Vomiting) {
		sprintf(buf, "nauseated");
	      sprintf(eos(buf), " (%d)", Vomiting);
		dump(youwere, buf);
	}
	if (Feared) {
		if (HeavyFeared) sprintf(buf, "stricken with very strong fear");
		else sprintf(buf, "stricken with fear");
	      sprintf(eos(buf), " (%d)", HFeared);
		dump(youwere, buf);
	}
	if (Numbed) {
		if (HeavyNumbed) sprintf(buf, "badly numbed");
		else sprintf(buf, "numbed");
	      sprintf(eos(buf), " (%d)", HNumbed);
		dump(youwere, buf);
	}
	if (Frozen) {
		if (HeavyFrozen) sprintf(buf, "frozen rigid and solid");
		else sprintf(buf, "frozen solid");
	      sprintf(eos(buf), " (%d)", HFrozen);
		dump(youwere, buf);
	}

	if (u.hanguppenalty) {
		sprintf(buf, "temporarily slower because you tried to hangup cheat");
	      sprintf(eos(buf), " (%d)", u.hanguppenalty);
		dump(youwere, buf);
	}

	if (Burned) {
		if (HeavyBurned) sprintf(buf, "badly burned");
		else sprintf(buf, "burned");
	      sprintf(eos(buf), " (%d)", HBurned);
		dump(youwere, buf);
	}
		
	if (Dimmed) {
		if (HeavyDimmed) sprintf(buf, "badly dimmed");
		else sprintf(buf, "dimmed");
	      sprintf(eos(buf), " (%d)", HDimmed);
		dump(youwere, buf);
	}
		
      if (Punished) dump(youwere, "punished");

      if (u.totter) dump(youhad, "inverted directional keys");

      if (sengr_at("Elbereth", u.ux, u.uy) ) dump(youwere, "standing on an active Elbereth engraving");

	if (Stoned) {
		sprintf(buf, "turning to stone");
	      sprintf(eos(buf), " (%d)", Stoned);
		dump(youwere, buf);
	}
	if (Slimed) {
		sprintf(buf, "turning into slime");
	      sprintf(eos(buf), " (%d)", Slimed);
		dump(youwere, buf);
	}
	if (Strangled) {
		sprintf(buf, (u.uburied) ? "buried" : "being strangled");
	      sprintf(eos(buf), " (%d)", Strangled);
		dump(youwere, buf);
	}
	if (PlayerBleeds) {
		sprintf(buf, "bleeding");
	      sprintf(eos(buf), " (%d)", PlayerBleeds);
		dump(youwere, buf);
	}
	if (Prem_death) {
		sprintf(buf, "going to die prematurely");
		dump(youwere, buf);
	}

	if (IsGlib) {
		sprintf(buf, "slippery %s", makeplural(body_part(FINGER)));
	      sprintf(eos(buf), " (%d)", Glib);
		dump(youhad, buf);
	}

	if (MenuBug || u.uprops[MENU_LOST].extrinsic || have_menubugstone()) {
		sprintf(buf, "the menu bug.");
	      sprintf(eos(buf), " (%d)", MenuBug);
		dump(youhad, buf);
	}
	if (SpeedBug || u.uprops[SPEED_BUG].extrinsic || have_speedbugstone()) {
		sprintf(buf, "the speed bug.");
	      sprintf(eos(buf), " (%d)", SpeedBug);
		dump(youhad, buf);
	}
	if (Superscroller || u.uprops[SUPERSCROLLER_ACTIVE].extrinsic || have_superscrollerstone()) {
		sprintf(buf, "the superscroller.");
	      sprintf(eos(buf), " (%d)", Superscroller);
		dump(youhad, buf);
	}
	if (RMBLoss || u.uprops[RMB_LOST].extrinsic || have_rmbstone()) {
		sprintf(buf, "the following problem: Your right mouse button failed.");
	      sprintf(eos(buf), " (%d)", RMBLoss);
		dump(youhad, buf);
	}
	if (DisplayLoss || u.uprops[DISPLAY_LOST].extrinsic || have_displaystone()) {
		sprintf(buf, "the following problem: Your display failed.");
	      sprintf(eos(buf), " (%d)", DisplayLoss);
		dump(youhad, buf);
	}
	if (SpellLoss || u.uprops[SPELLS_LOST].extrinsic || have_spelllossstone()) {
		sprintf(buf, "the following problem: You lost spells.");
	      sprintf(eos(buf), " (%d)", SpellLoss);
		dump(youhad, buf);
	}
	if (YellowSpells || u.uprops[YELLOW_SPELLS].extrinsic || have_yellowspellstone()) {
		sprintf(buf, "the following problem: Your spells became yellow.");
	      sprintf(eos(buf), " (%d)", YellowSpells);
		dump(youhad, buf);
	}
	if (AutoDestruct || u.uprops[AUTO_DESTRUCT].extrinsic || have_autodestructstone()) {
		sprintf(buf, "the following problem: An auto destruct mechanism was initiated.");
	      sprintf(eos(buf), " (%d)", AutoDestruct);
		dump(youhad, buf);
	}
	if (MemoryLoss || u.uprops[MEMORY_LOST].extrinsic || have_memorylossstone()) {
		sprintf(buf, "the following problem: There was low local memory.");
	      sprintf(eos(buf), " (%d)", MemoryLoss);
		dump(youhad, buf);
	}
	if (InventoryLoss || u.uprops[INVENTORY_LOST].extrinsic || have_inventorylossstone()) {
		sprintf(buf, "the following problem: There was not enough memory to create an inventory window.");
	      sprintf(eos(buf), " (%d)", InventoryLoss);
		dump(youhad, buf);
	}
	if (BlackNgWalls || u.uprops[BLACK_NG_WALLS].extrinsic || have_blackystone()) {
		sprintf(buf, "the following problem: Blacky and his NG walls were closing in.");
	      sprintf(eos(buf), " (%d)", BlackNgWalls);
		dump(youhad, buf);
	}
	if (FreeHandLoss || u.uprops[FREE_HAND_LOST].extrinsic || have_freehandbugstone()) {
		sprintf(buf, "the following problem: Your free hand was less likely to be free.");
	      sprintf(eos(buf), " (%d)", FreeHandLoss);
		dump(youhad, buf);
	}
	if (Unidentify || u.uprops[UNIDENTIFY].extrinsic || have_unidentifystone()) {
		sprintf(buf, "the following problem: Your possessions sometimes unidentified themselves.");
	      sprintf(eos(buf), " (%d)", Unidentify);
		dump(youhad, buf);
	}
	if (Thirst || u.uprops[THIRST].extrinsic || have_thirststone()) {
		sprintf(buf, "a strong sense of thirst.");
	      sprintf(eos(buf), " (%d)", Thirst);
		dump(youhad, buf);
	}
	if (LuckLoss || u.uprops[LUCK_LOSS].extrinsic || have_unluckystone()) {
		sprintf(buf, "the following problem: You were shitting out of luck (SOL).");
	      sprintf(eos(buf), " (%d)", LuckLoss);
		dump(youhad, buf);
	}
	if (ShadesOfGrey || u.uprops[SHADES_OF_GREY].extrinsic || have_shadesofgreystone()) {
		sprintf(buf, "the following problem: Everything displayed in various shades of grey.");
	      sprintf(eos(buf), " (%d)", ShadesOfGrey);
		dump(youhad, buf);
	}
	if (FaintActive || u.uprops[FAINT_ACTIVE].extrinsic || have_faintingstone()) {
		sprintf(buf, "the following problem: You randomly fainted.");
	      sprintf(eos(buf), " (%d)", FaintActive);
		dump(youhad, buf);
	}
	if (Itemcursing || u.uprops[ITEMCURSING].extrinsic || have_cursingstone() || have_primecurse() ) {
		sprintf(buf, "the following problem: Your inventory gradually filled up with cursed items.");
	      sprintf(eos(buf), " (%d)", Itemcursing);
		dump(youhad, buf);
	}
	if (have_morgothiancurse() ) {
		sprintf(buf, "afflicted by the Ancient Morgothian Curse.");
		dump(youwere, buf);
	}
	if (have_topiylinencurse() ) {
		sprintf(buf, "afflicted by Topi Ylinen's Curse, a.k.a. the Ancient Foul Curse.");
		dump(youwere, buf);
	}
	if (have_blackbreathcurse() ) {
		sprintf(buf, "filled with the Black Breath.");
		dump(youwere, buf);
	}

	if (DifficultyIncreased || u.uprops[DIFFICULTY_INCREASED].extrinsic || have_difficultystone()) {
		sprintf(buf, "the following problem: The difficulty of the game was arbitrarily increased.");
	      sprintf(eos(buf), " (%d)", DifficultyIncreased);
		dump(youhad, buf);
	}
	if (Deafness || u.uprops[DEAFNESS].extrinsic || have_deafnessstone()) {
		sprintf(buf, "a hearing break.");
	      sprintf(eos(buf), " (%d)", Deafness);
		dump(youhad, buf);
	}
	if (CasterProblem || u.uprops[CASTER_PROBLEM].extrinsic || have_antimagicstone()) {
		sprintf(buf, "blood mana.");
	      sprintf(eos(buf), " (%d)", CasterProblem);
		dump(youhad, buf);
	}
	if (WeaknessProblem || u.uprops[WEAKNESS_PROBLEM].extrinsic || have_weaknessstone()) {
		sprintf(buf, "the following problem: Being weak from hunger damaged your health.");
	      sprintf(eos(buf), " (%d)", WeaknessProblem);
		dump(youhad, buf);
	}
	if (RotThirteen || u.uprops[ROT_THIRTEEN].extrinsic || have_rotthirteenstone()) {
		sprintf(buf, "the following problem: A rot13 cypher had been activated for lowercase letters.");
	      sprintf(eos(buf), " (%d)", RotThirteen);
		dump(youhad, buf);
	}
	if (BishopGridbug || u.uprops[BISHOP_GRIDBUG].extrinsic || have_bishopstone()) {
		sprintf(buf, "the following problem: You could not move diagonally.");
	      sprintf(eos(buf), " (%d)", BishopGridbug);
		dump(youhad, buf);
	}
	if (ConfusionProblem || u.uprops[CONFUSION_PROBLEM].extrinsic || have_confusionstone()) {
		sprintf(buf, "a confusing problem.");
	      sprintf(eos(buf), " (%d)", ConfusionProblem);
		dump(youhad, buf);
	}
	if (NoDropProblem || u.uprops[DROP_BUG].extrinsic || have_dropbugstone()) {
		sprintf(buf, "the following problem: You could not drop items.");
	      sprintf(eos(buf), " (%d)", NoDropProblem);
		dump(youhad, buf);
	}
	if (DSTWProblem || u.uprops[DSTW_BUG].extrinsic || have_dstwstone()) {
		sprintf(buf, "the following problem: Your potions didn't always work.");
	      sprintf(eos(buf), " (%d)", DSTWProblem);
		dump(youhad, buf);
	}
	if (StatusTrapProblem || u.uprops[STATUS_FAILURE].extrinsic || have_statusstone()) {
		sprintf(buf, "the following problem: You could not cure status effects.");
	      sprintf(eos(buf), " (%d)", StatusTrapProblem);
		dump(youhad, buf);
	}
	if (AlignmentProblem || u.uprops[ALIGNMENT_FAILURE].extrinsic || have_alignmentstone()) {
		sprintf(buf, "the following problem: Your maximum alignment decreased over time.");
	      sprintf(eos(buf), " (%d)", AlignmentProblem);
		dump(youhad, buf);
	}
	if (StairsProblem || u.uprops[STAIRSTRAP].extrinsic || have_stairstrapstone()) {
		sprintf(buf, "the following problem: Stairs were always trapped.");
	      sprintf(eos(buf), " (%d)", StairsProblem);
		dump(youhad, buf);
	}
	if (UninformationProblem || u.uprops[UNINFORMATION].extrinsic || have_uninformationstone()) {
		sprintf(buf, "the following problem: The game gave insufficient amounts of information.");
	      sprintf(eos(buf), " (%d)", UninformationProblem);
		dump(youhad, buf);
	}
	if (TimerunBug || u.uprops[TIMERUN_BUG].extrinsic || have_timerunstone()) {
		sprintf(buf, "the following problem: All actions take turns.");
	      sprintf(eos(buf), " (%d)", TimerunBug);
		dump(youhad, buf);
	}

	if (SanityTrebleEffect || u.uprops[SANITY_TREBLE_EFFECT].extrinsic || have_sanitytreblestone()) {
		sprintf(buf, "the following problem: Sanity effects will increase your sanity by a much higher amount than usual.");
	      sprintf(eos(buf), " (%d)", SanityTrebleEffect);
		dump(youhad, buf);
	}

	if (StatDecreaseBug || u.uprops[STAT_DECREASE_BUG].extrinsic || have_statdecreasestone()) {
		sprintf(buf, "the following problem: The soft cap for your attributes is much lower.");
	      sprintf(eos(buf), " (%d)", StatDecreaseBug);
		dump(youhad, buf);
	}

	if (SimeoutBug || u.uprops[SIMEOUT_BUG].extrinsic || have_simeoutstone()) {
		sprintf(buf, "the following problem: Your sanity slowly increases over time.");
	      sprintf(eos(buf), " (%d)", SimeoutBug);
		dump(youhad, buf);
	}

	if (BadPartBug || u.uprops[BAD_PARTS].extrinsic || have_badpartstone()) {
		sprintf(buf, "in the bad part.");
	      sprintf(eos(buf), " (%d)", BadPartBug);
		dump(youwere, buf);
	}

	if (CompletelyBadPartBug || u.uprops[COMPLETELY_BAD_PARTS].extrinsic || have_completelybadpartstone()) {
		sprintf(buf, "in the completely bad part.");
	      sprintf(eos(buf), " (%d)", CompletelyBadPartBug);
		dump(youwere, buf);
	}

	if (EvilVariantActive || u.uprops[EVIL_VARIANT_ACTIVE].extrinsic || have_evilvariantstone()) {
		sprintf(buf, "the following problem: You're forced to play the evil variant.");
	      sprintf(eos(buf), " (%d)", EvilVariantActive);
		dump(youhad, buf);
	}

	if (OrangeSpells || u.uprops[ORANGE_SPELLS].extrinsic || have_orangespellstone()) {
		sprintf(buf, "the following problem: Your spells became orange.");
	      sprintf(eos(buf), " (%d)", OrangeSpells);
		dump(youhad, buf);
	}

	if (VioletSpells || u.uprops[VIOLET_SPELLS].extrinsic || have_violetspellstone()) {
		sprintf(buf, "the following problem: Your spells became violet.");
	      sprintf(eos(buf), " (%d)", VioletSpells);
		dump(youhad, buf);
	}

	if (LongingEffect || u.uprops[LONGING_EFFECT].extrinsic || have_longingstone()) {
		sprintf(buf, "the following problem: The game bombards you with 'sexy' messages.");
	      sprintf(eos(buf), " (%d)", LongingEffect);
		dump(youhad, buf);
	}

	if (CursedParts || u.uprops[CURSED_PARTS].extrinsic || have_cursedpartstone()) {
		sprintf(buf, "the following problem: Sometimes you're forced to wear cursed equipment.");
	      sprintf(eos(buf), " (%d)", CursedParts);
		dump(youhad, buf);
	}

	if (Quaversal || u.uprops[QUAVERSAL].extrinsic || have_quaversalstone()) {
		sprintf(buf, "the following problem: Unless you're standing on specific tiles, the entire screen is black.");
	      sprintf(eos(buf), " (%d)", Quaversal);
		dump(youhad, buf);
	}

	if (AppearanceShuffling || u.uprops[APPEARANCE_SHUFFLING].extrinsic || have_appearanceshufflingstone()) {
		sprintf(buf, "the following problem: The randomized appearances of items are occasionally shuffled.");
	      sprintf(eos(buf), " (%d)", AppearanceShuffling);
		dump(youhad, buf);
	}

	if (BrownSpells || u.uprops[BROWN_SPELLS].extrinsic || have_brownspellstone()) {
		sprintf(buf, "the following problem: Your spells became brown.");
	      sprintf(eos(buf), " (%d)", BrownSpells);
		dump(youhad, buf);
	}

	if (Choicelessness || u.uprops[CHOICELESSNESS].extrinsic || have_choicelessstone()) {
		sprintf(buf, "the following problem: Direction prompts often don't allow you to select anything.");
	      sprintf(eos(buf), " (%d)", Choicelessness);
		dump(youhad, buf);
	}

	if (Goldspells || u.uprops[GOLDSPELLS].extrinsic || have_goldspellstone()) {
		sprintf(buf, "the following problem: You often can't choose which spell you want to cast.");
	      sprintf(eos(buf), " (%d)", Goldspells);
		dump(youhad, buf);
	}

	if (Deprovement || u.uprops[DEPROVEMENT].extrinsic || have_deprovementstone()) {
		sprintf(buf, "the following problem: Leveling up no longer increases your maximum HP and Pw.");
	      sprintf(eos(buf), " (%d)", Deprovement);
		dump(youhad, buf);
	}

	if (InitializationFail || u.uprops[INITIALIZATION_FAIL].extrinsic || have_initializationstone()) {
		sprintf(buf, "the following problem: Prompts that let you choose an item will not appear.");
	      sprintf(eos(buf), " (%d)", InitializationFail);
		dump(youhad, buf);
	}

	if (GushlushEffect || u.uprops[GUSHLUSH].extrinsic || have_gushlushstone()) {
		sprintf(buf, "the following problem: To-hit, spellcasting chances and some other stuff always assume that you're experience level 1.");
	      sprintf(eos(buf), " (%d)", GushlushEffect);
		dump(youhad, buf);
	}

	if (SoiltypeEffect || u.uprops[SOILTYPE].extrinsic || have_soiltypestone()) {
		sprintf(buf, "the following problem: You trigger ground-based traps even while flying or levitating.");
	      sprintf(eos(buf), " (%d)", SoiltypeEffect);
		dump(youhad, buf);
	}

	if (DangerousTerrains || u.uprops[DANGEROUS_TERRAINS].extrinsic || have_dangerousterrainstone()) {
		sprintf(buf, "the following problem: Standing next to dangerous terrain may confuse or stun you with no warning.");
	      sprintf(eos(buf), " (%d)", DangerousTerrains);
		dump(youhad, buf);
	}

	if (FalloutEffect || u.uprops[FALLOUT_EFFECT].extrinsic || have_falloutstone()) {
		sprintf(buf, "the following problem: Your contamination increases over time.");
	      sprintf(eos(buf), " (%d)", FalloutEffect);
		dump(youhad, buf);
	}

	if (MojibakeEffect || u.uprops[MOJIBAKE].extrinsic || have_mojibakestone()) {
		sprintf(buf, "the following problem: Some glyphs are randomized.");
	      sprintf(eos(buf), " (%d)", MojibakeEffect);
		dump(youhad, buf);
	}

	if (GravationEffect || u.uprops[GRAVATION].extrinsic || have_gravationstone()) {
		sprintf(buf, "the following problem: Random monster spawns use aggravate monster rules.");
	      sprintf(eos(buf), " (%d)", GravationEffect);
		dump(youhad, buf);
	}

	if (UncalledEffect || u.uprops[UNCALLED_EFFECT].extrinsic || have_uncalledstone()) {
		sprintf(buf, "the following problem: You can't give names to items or monsters.");
	      sprintf(eos(buf), " (%d)", UncalledEffect);
		dump(youhad, buf);
	}

	if (ExplodingDiceEffect || u.uprops[EXPLODING_DICE].extrinsic || have_explodingdicestone()) {
		sprintf(buf, "the following problem: Monster attacks use exploding dice for damage calculations.");
	      sprintf(eos(buf), " (%d)", ExplodingDiceEffect);
		dump(youhad, buf);
	}

	if (PermacurseEffect || u.uprops[PERMACURSE_EFFECT].extrinsic || have_permacursestone()) {
		sprintf(buf, "the following problem: Many conventional methods of curse removal no longer work.");
	      sprintf(eos(buf), " (%d)", PermacurseEffect);
		dump(youhad, buf);
	}

	if (ShroudedIdentity || u.uprops[SHROUDED_IDENTITY].extrinsic || have_shroudedidentitystone()) {
		sprintf(buf, "the following problem: Identifying items is much harder.");
	      sprintf(eos(buf), " (%d)", ShroudedIdentity);
		dump(youhad, buf);
	}

	if (FeelerGauges || u.uprops[FEELER_GAUGES].extrinsic || have_feelergaugesstone()) {
		sprintf(buf, "the following problem: Monsters always know where you are and come straight for you.");
	      sprintf(eos(buf), " (%d)", FeelerGauges);
		dump(youhad, buf);
	}

	if (LongScrewup || u.uprops[LONG_SCREWUP].extrinsic || have_longscrewupstone()) {
		sprintf(buf, "the following problem: Triggering a nasty trap causes the effect to last for a really long time.");
	      sprintf(eos(buf), " (%d)", LongScrewup);
		dump(youhad, buf);
	}

	if (WingYellowChange || u.uprops[WING_YELLOW_GLYPHS].extrinsic || have_wingyellowstone()) {
		sprintf(buf, "the following problem: You're forced to play with a different tileset.");
	      sprintf(eos(buf), " (%d)", WingYellowChange);
		dump(youhad, buf);
	}

	if (LifeSavingBug || u.uprops[LIFE_SAVING_BUG].extrinsic || have_lifesavingstone()) {
		sprintf(buf, "the following problem: Monsters occasionally lifesave when killed.");
	      sprintf(eos(buf), " (%d)", LifeSavingBug);
		dump(youhad, buf);
	}

	if (CurseuseEffect || u.uprops[CURSEUSE_EFFECT].extrinsic || have_curseusestone()) {
		sprintf(buf, "the following problem: Any scroll, potion, spellbook, wand or tool you use automatically becomes cursed.");
	      sprintf(eos(buf), " (%d)", CurseuseEffect);
		dump(youhad, buf);
	}

	if (CutNutritionEffect || u.uprops[CUT_NUTRITION].extrinsic || have_cutnutritionstone()) {
		sprintf(buf, "the following problem: You gain much less nutrition than usual.");
	      sprintf(eos(buf), " (%d)", CutNutritionEffect);
		dump(youhad, buf);
	}

	if (SkillLossEffect || u.uprops[SKILL_LOSS_EFFECT].extrinsic || have_skilllossstone()) {
		sprintf(buf, "the following problem: Your skills will gradually lose training.");
	      sprintf(eos(buf), " (%d)", SkillLossEffect);
		dump(youhad, buf);
	}

	if (AutopilotEffect || u.uprops[AUTOPILOT_EFFECT].extrinsic || have_autopilotstone()) {
		sprintf(buf, "the following problem: You're on autopilot.");
	      sprintf(eos(buf), " (%d)", AutopilotEffect);
		dump(youhad, buf);
	}

	if (MysteriousForceActive || u.uprops[MYSTERIOUS_FORCE_EFFECT].extrinsic || have_forcestone()) {
		sprintf(buf, "the following problem: The dirty mysterious force full of dirt is back.");
	      sprintf(eos(buf), " (%d)", MysteriousForceActive);
		dump(youhad, buf);
	}

	if (MonsterGlyphChange || u.uprops[MONSTER_GLYPH_BUG].extrinsic || have_monsterglyphstone()) {
		sprintf(buf, "the following problem: Monsters rapidly change glyphs.");
	      sprintf(eos(buf), " (%d)", MonsterGlyphChange);
		dump(youhad, buf);
	}

	if (ChangingDirectives || u.uprops[CHANGING_DIRECTIVES].extrinsic || have_changingdirectivestone()) {
		sprintf(buf, "the following problem: The directives you've given to your pets will change randomly.");
	      sprintf(eos(buf), " (%d)", ChangingDirectives);
		dump(youhad, buf);
	}

	if (ContainerKaboom || u.uprops[CONTAINER_KABOOM].extrinsic || have_containerkaboomstone()) {
		sprintf(buf, "the following problem: Opening a container causes bad stuff to happen.");
	      sprintf(eos(buf), " (%d)", ContainerKaboom);
		dump(youhad, buf);
	}

	if (StealDegrading || u.uprops[STEAL_DEGRADING].extrinsic || have_stealdegradestone()) {
		sprintf(buf, "the following problem: If a monster steals your items, the stolen items are cursed and disenchanted.");
	      sprintf(eos(buf), " (%d)", StealDegrading);
		dump(youhad, buf);
	}

	if (LeftInventoryBug || u.uprops[LEFT_INVENTORY].extrinsic || have_leftinventorystone()) {
		sprintf(buf, "the following problem: Items that leave your inventory will be unidentified.");
	      sprintf(eos(buf), " (%d)", LeftInventoryBug);
		dump(youhad, buf);
	}

	if (FluctuatingSpeed || u.uprops[FLUCTUATING_SPEED].extrinsic || have_fluctuatingspeedstone()) {
		sprintf(buf, "the following problem: Your movement speed fluctuates from extremely fast to ultra slow.");
	      sprintf(eos(buf), " (%d)", FluctuatingSpeed);
		dump(youhad, buf);
	}

	if (TarmuStrokingNora || u.uprops[TARMU_STROKING_NORA].extrinsic || have_tarmustrokingnorastone()) {
		sprintf(buf, "the following problem: Searching may cause a new trap to spawn.");
	      sprintf(eos(buf), " (%d)", TarmuStrokingNora);
		dump(youhad, buf);
	}

	if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
		sprintf(buf, "the following problem: The messages 'nothing happens' or 'never mind' become dangerous.");
	      sprintf(eos(buf), " (%d)", FailureEffects);
		dump(youhad, buf);
	}

	if (BrightCyanSpells || u.uprops[BRIGHT_CYAN_SPELLS].extrinsic || have_brightcyanspellstone()) {
		sprintf(buf, "the following problem: Your spells became bright cyan.");
	      sprintf(eos(buf), " (%d)", BrightCyanSpells);
		dump(youhad, buf);
	}

	if (FrequentationSpawns || u.uprops[FREQUENTATION_SPAWNS].extrinsic || have_frequentationspawnstone()) {
		sprintf(buf, "the following problem: Some monster trait is more common than usual.");
	      sprintf(eos(buf), " (%d)", FrequentationSpawns);
		sprintf(eos(buf), " (%s)", montraitname(u.frequentationtrait));
		dump(youhad, buf);
	}

	if (PetAIScrewed || u.uprops[PET_AI_SCREWED].extrinsic || have_petaistone()) {
		sprintf(buf, "the following problem: Your pets refuse to follow you.");
	      sprintf(eos(buf), " (%d)", PetAIScrewed);
		dump(youhad, buf);
	}

	if (SatanEffect || u.uprops[SATAN_EFFECT].extrinsic || have_satanstone()) {
		sprintf(buf, "the following problem: Satan made it so that changing dungeon levels paralyzes you for a turn.");
	      sprintf(eos(buf), " (%d)", SatanEffect);
		dump(youhad, buf);
	}

	if (RememberanceEffect || u.uprops[REMEMBERANCE_EFFECT].extrinsic || have_rememberancestone()) {
		sprintf(buf, "the following problem: Spells with low spell memory are harder to cast.");
	      sprintf(eos(buf), " (%d)", RememberanceEffect);
		dump(youhad, buf);
	}

	if (PokelieEffect || u.uprops[POKELIE_EFFECT].extrinsic || have_pokeliestone()) {
		sprintf(buf, "the following problem: The pokedex lies to you.");
	      sprintf(eos(buf), " (%d)", PokelieEffect);
		dump(youhad, buf);
	}

	if (AlwaysAutopickup || u.uprops[AUTOPICKUP_ALWAYS].extrinsic || have_autopickupstone()) {
		sprintf(buf, "the following problem: You always pick up items.");
	      sprintf(eos(buf), " (%d)", AlwaysAutopickup);
		dump(youhad, buf);
	}

	if (DywypiProblem || u.uprops[DYWYPI_PROBLEM].extrinsic || have_dywypistone()) {
		sprintf(buf, "the following problem: If you lifesave, you have to give the correct answer or you die anyway.");
	      sprintf(eos(buf), " (%d)", DywypiProblem);
		dump(youhad, buf);
	}

	if (SilverSpells || u.uprops[SILVER_SPELLS].extrinsic || have_silverspellstone()) {
		sprintf(buf, "the following problem: Your spells became silver.");
	      sprintf(eos(buf), " (%d)", SilverSpells);
		dump(youhad, buf);
	}

	if (MetalSpells || u.uprops[METAL_SPELLS].extrinsic || have_metalspellstone()) {
		sprintf(buf, "the following problem: Your spells became metal.");
	      sprintf(eos(buf), " (%d)", MetalSpells);
		dump(youhad, buf);
	}

	if (PlatinumSpells || u.uprops[PLATINUM_SPELLS].extrinsic || have_platinumspellstone()) {
		sprintf(buf, "the following problem: Your spells became platinum.");
	      sprintf(eos(buf), " (%d)", PlatinumSpells);
		dump(youhad, buf);
	}

	if (ManlerEffect || u.uprops[MANLER_EFFECT].extrinsic || have_manlerstone()) {
		sprintf(buf, "the following problem: The manler is chasing you.");
	      sprintf(eos(buf), " (%d)", ManlerEffect);
		dump(youhad, buf);
	}

	if (DoorningEffect || u.uprops[DOORNING_EFFECT].extrinsic || have_doorningstone()) {
		sprintf(buf, "the following problem: Doors are dangerous.");
	      sprintf(eos(buf), " (%d)", DoorningEffect);
		dump(youhad, buf);
	}

	if (NownsibleEffect || u.uprops[NOWNSIBLE_EFFECT].extrinsic || have_nownsiblestone()) {
		sprintf(buf, "the following problem: Triggering a trap makes it invisible.");
	      sprintf(eos(buf), " (%d)", NownsibleEffect);
		dump(youhad, buf);
	}

	if (ElmStreetEffect || u.uprops[ELM_STREET_EFFECT].extrinsic || have_elmstreetstone()) {
		sprintf(buf, "the following problem: The game behaves as if it were Friday the 13th.");
	      sprintf(eos(buf), " (%d)", ElmStreetEffect);
		dump(youhad, buf);
	}

	if (MonnoiseEffect || u.uprops[MONNOISE_EFFECT].extrinsic || have_monnoisestone()) {
		sprintf(buf, "the following problem: Monsters make noises.");
	      sprintf(eos(buf), " (%d)", MonnoiseEffect);
		dump(youhad, buf);
	}

	if (RangCallEffect || u.uprops[RANG_CALL_EFFECT].extrinsic || have_rangcallstone()) {
		sprintf(buf, "the following problem: The presence of boss monsters increases your sanity.");
	      sprintf(eos(buf), " (%d)", RangCallEffect);
		dump(youhad, buf);
	}

	if (RecurringSpellLoss || u.uprops[RECURRING_SPELL_LOSS].extrinsic || have_recurringspelllossstone()) {
		sprintf(buf, "the following problem: You repeatedly lose spell memory.");
	      sprintf(eos(buf), " (%d)", RecurringSpellLoss);
		dump(youhad, buf);
	}

	if (AntitrainingEffect || u.uprops[ANTI_TRAINING_EFFECT].extrinsic || have_antitrainingstone()) {
		sprintf(buf, "the following problem: Your skill training is occasionally reduced.");
	      sprintf(eos(buf), " (%d)", AntitrainingEffect);
		dump(youhad, buf);
	}

	if (TechoutBug || u.uprops[TECHOUT_BUG].extrinsic || have_techoutstone()) {
		sprintf(buf, "the following problem: Your techniques may sometimes be put on timeout.");
	      sprintf(eos(buf), " (%d)", TechoutBug);
		dump(youhad, buf);
	}

	if (StatDecay || u.uprops[STAT_DECAY].extrinsic || have_statdecaystone()) {
		sprintf(buf, "the following problem: Your stats will gradually decay.");
	      sprintf(eos(buf), " (%d)", StatDecay);
		dump(youhad, buf);
	}

	if (Movemork || u.uprops[MOVEMORKING].extrinsic || have_movemorkstone()) {
		sprintf(buf, "the following problem: Monsters are generated with movement energy.");
	      sprintf(eos(buf), " (%d)", Movemork);
		dump(youhad, buf);
	}

	if (LootcutBug || u.uprops[LOOTCUT_BUG].extrinsic || have_lootcutstone()) {
		sprintf(buf, "the following problem: Monsters delete their musable items when killed.");
	      sprintf(eos(buf), " (%d)", LootcutBug);
		dump(youhad, buf);
	}

	if (MonsterSpeedBug || u.uprops[MONSTER_SPEED_BUG].extrinsic || have_monsterspeedstone()) {
		sprintf(buf, "the following problem: Monsters are moving faster.");
	      sprintf(eos(buf), " (%d)", MonsterSpeedBug);
		dump(youhad, buf);
	}

	if (ScalingBug || u.uprops[SCALING_BUG].extrinsic || have_scalingstone()) {
		sprintf(buf, "the following problem: Minimum monster level depends on your current depth.");
	      sprintf(eos(buf), " (%d)", ScalingBug);
		dump(youhad, buf);
	}

	if (EnmityBug || u.uprops[ENMITY_BUG].extrinsic || have_inimicalstone()) {
		sprintf(buf, "the following problem: Monsters always spawn hostile.");
	      sprintf(eos(buf), " (%d)", EnmityBug);
		dump(youhad, buf);
	}

	if (WhiteSpells || u.uprops[WHITE_SPELLS].extrinsic || have_whitespellstone()) {
		sprintf(buf, "the following problem: Your spells became white.");
	      sprintf(eos(buf), " (%d)", WhiteSpells);
		dump(youhad, buf);
	}

	if (CompleteGraySpells || u.uprops[COMPLETE_GRAY_SPELLS].extrinsic || have_greyoutstone()) {
		sprintf(buf, "the following problem: Your spells became completely gray.");
	      sprintf(eos(buf), " (%d)", CompleteGraySpells);
		dump(youhad, buf);
	}

	if (QuasarVision || u.uprops[QUASAR_BUG].extrinsic || have_quasarstone()) {
		sprintf(buf, "the following problem: You can barely see the quasars.");
	      sprintf(eos(buf), " (%d)", QuasarVision);
		dump(youhad, buf);
	}

	if (MommaBugEffect || u.uprops[MOMMA_BUG].extrinsic || have_mommystone()) {
		sprintf(buf, "the following problem: Your momma will repeatedly be insulted.");
	      sprintf(eos(buf), " (%d)", MommaBugEffect);
		dump(youhad, buf);
	}

	if (HorrorBugEffect || u.uprops[HORROR_BUG].extrinsic || have_horrorstone()) {
		sprintf(buf, "the following problem: You will repeatedly get hit with status effects.");
	      sprintf(eos(buf), " (%d)", HorrorBugEffect);
		dump(youhad, buf);
	}

	if (ArtificerBug || u.uprops[ARTIFICER_BUG].extrinsic || have_artificialstone()) {
		sprintf(buf, "the following problem: You will be forced to equip evil artifacts.");
	      sprintf(eos(buf), " (%d)", ArtificerBug);
		dump(youhad, buf);
	}

	if (WereformBug || u.uprops[WEREFORM_BUG].extrinsic || have_wereformstone()) {
		sprintf(buf, "the following problem: You randomly polymorph into werecreatures.");
	      sprintf(eos(buf), " (%d)", WereformBug);
		dump(youhad, buf);
	}

	if (NonprayerBug || u.uprops[NON_PRAYER_BUG].extrinsic || have_antiprayerstone()) {
		sprintf(buf, "the following problem: Your prayer timeout goes up instead of down.");
	      sprintf(eos(buf), " (%d)", NonprayerBug);
		dump(youhad, buf);
	}

	if (EvilPatchEffect || u.uprops[EVIL_PATCH_EFFECT].extrinsic || have_evilpatchstone()) {
		sprintf(buf, "the following problem: You will get nasty trap effects intrinsically over time.");
	      sprintf(eos(buf), " (%d)", EvilPatchEffect);
		dump(youhad, buf);
	}

	if (HardModeEffect || u.uprops[HARD_MODE_EFFECT].extrinsic || have_hardmodestone()) {
		sprintf(buf, "the following problem: All damage you take is doubled.");
	      sprintf(eos(buf), " (%d)", HardModeEffect);
		dump(youhad, buf);
	}

	if (SecretAttackBug || u.uprops[SECRET_ATTACK_BUG].extrinsic || have_secretattackstone()) {
		sprintf(buf, "the following problem: Monsters occasionally use their secret attacks.");
	      sprintf(eos(buf), " (%d)", SecretAttackBug);
		dump(youhad, buf);
	}

	if (EaterBugEffect || u.uprops[EATER_BUG].extrinsic || have_eaterstone()) {
		sprintf(buf, "the following problem: Monsters can eat all items they encounter.");
	      sprintf(eos(buf), " (%d)", EaterBugEffect);
		dump(youhad, buf);
	}

	if (CovetousnessBug || u.uprops[COVETOUSNESS_BUG].extrinsic || have_covetousstone()) {
		sprintf(buf, "the following problem: Covetous monster AI fires each turn.");
	      sprintf(eos(buf), " (%d)", CovetousnessBug);
		dump(youhad, buf);
	}

	if (NotSeenBug || u.uprops[NOT_SEEN_BUG].extrinsic || have_nonseeingstone()) {
		sprintf(buf, "the following problem: The walls became invisible.");
	      sprintf(eos(buf), " (%d)", NotSeenBug);
		dump(youhad, buf);
	}

	if (DarkModeBug || u.uprops[DARK_MODE_BUG].extrinsic || have_darkmodestone()) {
		sprintf(buf, "the following problem: Lit tiles aren't visible unless you would also see them if they were unlit.");
	      sprintf(eos(buf), " (%d)", DarkModeBug);
		dump(youhad, buf);
	}

	if (AntisearchEffect || u.uprops[ANTISEARCH_EFFECT].extrinsic || have_unfindablestone()) {
		sprintf(buf, "the following problem: The search command never finds anything.");
	      sprintf(eos(buf), " (%d)", AntisearchEffect);
		dump(youhad, buf);
	}

	if (HomicideEffect || u.uprops[HOMICIDE_EFFECT].extrinsic || have_homicidestone()) {
		sprintf(buf, "the following problem: Monsters build new traps for you to blunder into.");
	      sprintf(eos(buf), " (%d)", HomicideEffect);
		dump(youhad, buf);
	}

	if (NastynationBug || u.uprops[NASTY_NATION_BUG].extrinsic || have_multitrappingstone()) {
		sprintf(buf, "the following problem: All trap types except beneficial ones are equally likely to generate.");
	      sprintf(eos(buf), " (%d)", NastynationBug);
		dump(youhad, buf);
	}

	if (WakeupCallBug || u.uprops[WAKEUP_CALL_BUG].extrinsic || have_wakeupcallstone()) {
		sprintf(buf, "the following problem: Peaceful monsters may spontaneously turn hostile.");
	      sprintf(eos(buf), " (%d)", WakeupCallBug);
		dump(youhad, buf);
	}

	if (GrayoutBug || u.uprops[GRAYOUT_BUG].extrinsic || have_grayoutstone()) {
		sprintf(buf, "the following problem: The display intermittently becomes all gray.");
	      sprintf(eos(buf), " (%d)", GrayoutBug);
		dump(youhad, buf);
	}

	if (GrayCenterBug || u.uprops[GRAY_CENTER_BUG].extrinsic || have_graycenterstone()) {
		sprintf(buf, "the following problem: Your immediate surroundings are obscured by a gray shroud.");
	      sprintf(eos(buf), " (%d)", GrayCenterBug);
		dump(youhad, buf);
	}

	if (CheckerboardBug || u.uprops[CHECKERBOARD_BUG].extrinsic || have_checkerboardstone()) {
		sprintf(buf, "the following problem: Only one color of checkerboard tiles is ever visible at any given time.");
	      sprintf(eos(buf), " (%d)", CheckerboardBug);
		dump(youhad, buf);
	}

	if (ClockwiseSpinBug || u.uprops[CLOCKWISE_SPIN_BUG].extrinsic || have_clockwisestone()) {
		sprintf(buf, "the following problem: Your directional keys are shifted 45 degrees clockwise.");
	      sprintf(eos(buf), " (%d)", ClockwiseSpinBug);
		dump(youhad, buf);
	}

	if (CounterclockwiseSpin || u.uprops[COUNTERCLOCKWISE_SPIN_BUG].extrinsic || have_counterclockwisestone()) {
		sprintf(buf, "the following problem: Your directional keys are shifted 90 degrees counterclockwise.");
	      sprintf(eos(buf), " (%d)", CounterclockwiseSpin);
		dump(youhad, buf);
	}

	if (LagBugEffect || u.uprops[LAG_BUG].extrinsic || have_lagstone()) {
		sprintf(buf, "the following problem: The game lags.");
	      sprintf(eos(buf), " (%d)", LagBugEffect);
		dump(youhad, buf);
	}

	if (BlesscurseEffect || u.uprops[BLESSCURSE_EFFECT].extrinsic || have_blesscursestone()) {
		sprintf(buf, "the following problem: Picking up a blessed item instantly curses it.");
	      sprintf(eos(buf), " (%d)", BlesscurseEffect);
		dump(youhad, buf);
	}

	if (DeLightBug || u.uprops[DE_LIGHT_BUG].extrinsic || have_delightstone()) {
		sprintf(buf, "the following problem: Tiles that you're walking on automatically become unlit.");
	      sprintf(eos(buf), " (%d)", DeLightBug);
		dump(youhad, buf);
	}

	if (DischargeBug || u.uprops[DISCHARGE_BUG].extrinsic || have_dischargestone()) {
		sprintf(buf, "the following problem: If you use wands or tools that have charges, more charges are used up than normal.");
	      sprintf(eos(buf), " (%d)", DischargeBug);
		dump(youhad, buf);
	}

	if (TrashingBugEffect || u.uprops[TRASHING_EFFECT].extrinsic || have_trashstone()) {
		sprintf(buf, "the following problem: If you equip enchanted weapons, armor or rings, their enchantment value decreases.");
	      sprintf(eos(buf), " (%d)", TrashingBugEffect);
		dump(youhad, buf);
	}

	if (FilteringBug || u.uprops[FILTERING_BUG].extrinsic || have_filteringstone()) {
		sprintf(buf, "the following problem: Certain messages may be replaced with generic ones.");
	      sprintf(eos(buf), " (%d)", FilteringBug);
		dump(youhad, buf);
	}

	if (DeformattingBug || u.uprops[DEFORMATTING_BUG].extrinsic || have_deformattingstone()) {
		sprintf(buf, "the following problem: Your pokedex doesn't work.");
	      sprintf(eos(buf), " (%d)", DeformattingBug);
		dump(youhad, buf);
	}

	if (FlickerStripBug || u.uprops[FLICKER_STRIP_BUG].extrinsic || have_flickerstripstone()) {
		sprintf(buf, "the following problem: The bottom status line only displays garbage strings.");
	      sprintf(eos(buf), " (%d)", FlickerStripBug);
		dump(youhad, buf);
	}

	if (UndressingEffect || u.uprops[UNDRESSING_EFFECT].extrinsic || have_undressingstone()) {
		sprintf(buf, "the following problem: You spontaneously take off equipment.");
	      sprintf(eos(buf), " (%d)", UndressingEffect);
		dump(youhad, buf);
	}

	if (Hyperbluewalls || u.uprops[HYPERBLUEWALL_BUG].extrinsic || have_hyperbluestone()) {
		sprintf(buf, "the following problem: Walls are blue, and walking into them hurts you.");
	      sprintf(eos(buf), " (%d)", Hyperbluewalls);
		dump(youhad, buf);
	}

	if (NoliteBug || u.uprops[NOLITE_BUG].extrinsic || have_nolightstone()) {
		sprintf(buf, "the following problem: Staircases and object piles are no longer highlighted.");
	      sprintf(eos(buf), " (%d)", NoliteBug);
		dump(youhad, buf);
	}

	if (ParanoiaBugEffect || u.uprops[PARANOIA_BUG].extrinsic || have_paranoiastone()) {
		sprintf(buf, "the following problem: There is no confirmation if you walk into traps, water or lava.");
	      sprintf(eos(buf), " (%d)", ParanoiaBugEffect);
		dump(youhad, buf);
	}

	if (FleecescriptBug || u.uprops[FLEECESCRIPT_BUG].extrinsic || have_fleecestone()) {
		sprintf(buf, "the following problem: Inventory item descriptions appear in fleecy colors, which is actually not all that problematic. :-)");
	      sprintf(eos(buf), " (%d)", FleecescriptBug);
		dump(youhad, buf);
	}

	if (InterruptEffect || u.uprops[INTERRUPT_EFFECT].extrinsic || have_interruptionstone()) {
		sprintf(buf, "the following problem: The read, quaff, zap and apply commands now take several turns to use and can be interrupted.");
	      sprintf(eos(buf), " (%d)", InterruptEffect);
		dump(youhad, buf);
	}

	if (DustbinBug || u.uprops[DUSTBIN_BUG].extrinsic || have_dustbinstone()) {
		sprintf(buf, "the following problem: If you pick up a scroll, it may disintegrate.");
	      sprintf(eos(buf), " (%d)", DustbinBug);
		dump(youhad, buf);
	}

	if (ManaBatteryBug || u.uprops[MANA_BATTERY_BUG].extrinsic || have_batterystone()) {
		sprintf(buf, "a living mana battery.");
	      sprintf(eos(buf), " (%d)", ManaBatteryBug);
		dump(youwere, buf);
	}

	if (Monsterfingers || u.uprops[MONSTERFINGERS_EFFECT].extrinsic || have_butterfingerstone()) {
		sprintf(buf, "the following problem: Picking up potions may break them and subject you to their vapors.");
	      sprintf(eos(buf), " (%d)", Monsterfingers);
		dump(youhad, buf);
	}

	if (MiscastBug || u.uprops[MISCAST_BUG].extrinsic || have_miscastingstone()) {
		sprintf(buf, "the following problem: Casting a spell always causes backlash, regardless of whether the spell worked.");
	      sprintf(eos(buf), " (%d)", MiscastBug);
		dump(youhad, buf);
	}

	if (MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone()) {
		sprintf(buf, "the following problem: The top status line doesn't display anything.");
	      sprintf(eos(buf), " (%d)", MessageSuppression);
		dump(youhad, buf);
	}

	if (StuckAnnouncement || u.uprops[STUCK_ANNOUNCEMENT_BUG].extrinsic || have_stuckannouncementstone()) {
		sprintf(buf, "the following problem: The bottom status line only updates if you explicitly tell it to do so.");
	      sprintf(eos(buf), " (%d)", StuckAnnouncement);
		dump(youhad, buf);
	}

	if (BloodthirstyEffect || u.uprops[BLOODTHIRSTY_EFFECT].extrinsic || have_stormstone()) {
		sprintf(buf, "the following problem: There is no confirmation if you walk into a peaceful monster.");
	      sprintf(eos(buf), " (%d)", BloodthirstyEffect);
		dump(youhad, buf);
	}

	if (MaximumDamageBug || u.uprops[MAXIMUM_DAMAGE_BUG].extrinsic || have_maximumdamagestone()) {
		sprintf(buf, "the following problem: Monster attacks always deal maximum damage to you.");
	      sprintf(eos(buf), " (%d)", MaximumDamageBug);
		dump(youhad, buf);
	}

	if (LatencyBugEffect || u.uprops[LATENCY_BUG].extrinsic || have_latencystone()) {
		sprintf(buf, "the following problem: There is background latency.");
	      sprintf(eos(buf), " (%d)", LatencyBugEffect);
		dump(youhad, buf);
	}

	if (StarlitBug || u.uprops[STARLIT_BUG].extrinsic || have_starlitskystone()) {
		sprintf(buf, "the following problem: All monsters look the same and you can't tell them apart.");
	      sprintf(eos(buf), " (%d)", StarlitBug);
		dump(youhad, buf);
	}

	if (KnowledgeBug || u.uprops[KNOWLEDGE_BUG].extrinsic || have_trapknowledgestone()) {
		sprintf(buf, "the following problem: All traps look the same and you can't tell them apart.");
	      sprintf(eos(buf), " (%d)", KnowledgeBug);
		dump(youhad, buf);
	}

	if (HighscoreBug || u.uprops[HIGHSCORE_BUG].extrinsic || have_highscorestone()) {
		sprintf(buf, "the following problem: The game continuously spawns spacewars monsters and nasty traps.");
	      sprintf(eos(buf), " (%d)", HighscoreBug);
		dump(youhad, buf);
	}

	if (PinkSpells || u.uprops[PINK_SPELLS].extrinsic || have_pinkspellstone()) {
		sprintf(buf, "the following problem: Your spells became pink.");
	      sprintf(eos(buf), " (%d)", PinkSpells);
		dump(youhad, buf);
	}

	if (GreenSpells || u.uprops[GREEN_SPELLS].extrinsic || have_greenspellstone()) {
		sprintf(buf, "the following problem: Your spells became green.");
	      sprintf(eos(buf), " (%d)", GreenSpells);
		dump(youhad, buf);
	}

	if (EvencoreEffect || u.uprops[EVC_EFFECT].extrinsic || have_evcstone()) {
		sprintf(buf, "the following problem: The game will spawn fake invisible monster markers.");
	      sprintf(eos(buf), " (%d)", EvencoreEffect);
		dump(youhad, buf);
	}

	if (UnderlayerBug || u.uprops[UNDERLAYER_BUG].extrinsic || have_underlaidstone()) {
		sprintf(buf, "the following problem: Invisible monster markers are not visible, but bumping into them uses up a turn.");
	      sprintf(eos(buf), " (%d)", UnderlayerBug);
		dump(youhad, buf);
	}

	if (DamageMeterBug || u.uprops[DAMAGE_METER_BUG].extrinsic || have_damagemeterstone()) {
		sprintf(buf, "the following problem: The game won't tell you exactly how much damage an attack does.");
	      sprintf(eos(buf), " (%d)", DamageMeterBug);
		dump(youhad, buf);
	}

	if (ArbitraryWeightBug || u.uprops[ARBITRARY_WEIGHT_BUG].extrinsic || have_weightstone()) {
		sprintf(buf, "the following problem: You can't see your carry capacity or the weight of items.");
	      sprintf(eos(buf), " (%d)", ArbitraryWeightBug);
		dump(youhad, buf);
	}

	if (FuckedInfoBug || u.uprops[FUCKED_INFO_BUG].extrinsic || have_infofuckstone()) {
		sprintf(buf, "the following problem: You can't see what character you are playing.");
	      sprintf(eos(buf), " (%d)", FuckedInfoBug);
		dump(youhad, buf);
	}

	if (BlackSpells || u.uprops[BLACK_SPELLS].extrinsic || have_blackspellstone()) {
		sprintf(buf, "the following problem: Your spells became black.");
	      sprintf(eos(buf), " (%d)", BlackSpells);
		dump(youhad, buf);
	}

	if (CyanSpells || u.uprops[CYAN_SPELLS].extrinsic || have_cyanspellstone()) {
		sprintf(buf, "the following problem: Your spells became cyan.");
	      sprintf(eos(buf), " (%d)", CyanSpells);
		dump(youhad, buf);
	}

	if (HeapEffectBug || u.uprops[HEAP_EFFECT].extrinsic || have_heapstone()) {
		sprintf(buf, "the following problem: Messages are repeated randomly.");
	      sprintf(eos(buf), " (%d)", HeapEffectBug);
		dump(youhad, buf);
	}

	if (BlueSpells || u.uprops[BLUE_SPELLS].extrinsic || have_bluespellstone()) {
		sprintf(buf, "the following problem: Your spells became blue.");
	      sprintf(eos(buf), " (%d)", BlueSpells);
		dump(youhad, buf);
	}

	if (TronEffect || u.uprops[TRON_EFFECT].extrinsic || have_tronstone()) {
		sprintf(buf, "the following problem: You can't walk in the same direction twice in a row.");
	      sprintf(eos(buf), " (%d)", TronEffect);
		dump(youhad, buf);
	}

	if (RedSpells || u.uprops[RED_SPELLS].extrinsic || have_redspellstone()) {
		sprintf(buf, "the following problem: Your spells became red.");
	      sprintf(eos(buf), " (%d)", RedSpells);
		dump(youhad, buf);
	}

	if (TooHeavyEffect || u.uprops[TOO_HEAVY_EFFECT].extrinsic || have_tooheavystone()) {
		sprintf(buf, "the following problem: Picking up an item increases your carried weight by more than what the item weighs.");
	      sprintf(eos(buf), " (%d)", TooHeavyEffect);
		dump(youhad, buf);
	}

	if (ElongationBug || u.uprops[ELONGATION_BUG].extrinsic || have_elongatedstone()) {
		sprintf(buf, "the following problem: Monsters' hug attacks, ranged weapons and breaths can hit you from very far away.");
	      sprintf(eos(buf), " (%d)", ElongationBug);
		dump(youhad, buf);
	}

	if (WrapoverEffect || u.uprops[WRAPOVER_EFFECT].extrinsic || have_wrapoverstone()) {
		sprintf(buf, "the following problem: Your positively enchanted items may randomly turn into negatively enchanted ones.");
	      sprintf(eos(buf), " (%d)", WrapoverEffect);
		dump(youhad, buf);
	}

	if (DestructionEffect || u.uprops[DESTRUCTION_EFFECT].extrinsic || have_destructionstone()) {
		sprintf(buf, "the following problem: Item destruction may happen randomly.");
	      sprintf(eos(buf), " (%d)", DestructionEffect);
		dump(youhad, buf);
	}

	if (MeleePrefixBug || u.uprops[MELEE_PREFIX_BUG].extrinsic || have_meleeprefixstone()) {
		sprintf(buf, "the following problem: If you simply walk into monsters to attack, you lose a turn.");
	      sprintf(eos(buf), " (%d)", MeleePrefixBug);
		dump(youhad, buf);
	}

	if (AutomoreBug || u.uprops[AUTOMORE_BUG].extrinsic || have_automorestone()) {
		sprintf(buf, "the following problem: There are no --More-- prompts.");
	      sprintf(eos(buf), " (%d)", AutomoreBug);
		dump(youhad, buf);
	}

	if (UnfairAttackBug || u.uprops[UNFAIR_ATTACK_BUG].extrinsic || have_unfairattackstone()) {
		sprintf(buf, "the following problem: Monsters will occasionally use very unfair attacks on you.");
	      sprintf(eos(buf), " (%d)", UnfairAttackBug);
		dump(youhad, buf);
	}

	if (DisconnectedStairs || u.uprops[DISCONNECTED_STAIRS].extrinsic || have_disconnectstone()) {
		sprintf(buf, "the following problem: Staircases were disconnected.");
	      sprintf(eos(buf), " (%d)", DisconnectedStairs);
		dump(youhad, buf);
	}

	if (InterfaceScrewed || u.uprops[INTERFACE_SCREW].extrinsic || have_interfacescrewstone()) {
		sprintf(buf, "the following problem: The main game display did not update automatically.");
	      sprintf(eos(buf), " (%d)", InterfaceScrewed);
		dump(youhad, buf);
	}

	if (Bossfights || u.uprops[BOSSFIGHT].extrinsic || have_bossfightstone()) {
		sprintf(buf, "the following problem: Boss monsters spawned much more often.");
	      sprintf(eos(buf), " (%d)", Bossfights);
		dump(youhad, buf);
	}

	if (EntireLevelMode || u.uprops[ENTIRE_LEVEL].extrinsic || have_entirelevelstone()) {
		sprintf(buf, "the following problem: Monster types that used to be rare were common.");
	      sprintf(eos(buf), " (%d)", EntireLevelMode);
		dump(youhad, buf);
	}

	if (BonesLevelChange || u.uprops[BONES_CHANGE].extrinsic || have_bonestone()) {
		sprintf(buf, "the following problem: You always found or left bones levels if possible.");
	      sprintf(eos(buf), " (%d)", BonesLevelChange);
		dump(youhad, buf);
	}

	if (AutocursingEquipment || u.uprops[AUTOCURSE_EQUIP].extrinsic || have_autocursestone()) {
		sprintf(buf, "the following problem: Every item you put on automatically cursed itself.");
	      sprintf(eos(buf), " (%d)", AutocursingEquipment);
		dump(youhad, buf);
	}

	if (HighlevelStatus || u.uprops[HIGHLEVEL_STATUS].extrinsic || have_highlevelstone()) {
		sprintf(buf, "the following problem: Higher-level monsters were more likely to spawn.");
	      sprintf(eos(buf), " (%d)", HighlevelStatus);
		dump(youhad, buf);
	}

	if (SpellForgetting || u.uprops[SPELL_FORGETTING].extrinsic || have_spellforgettingstone()) {
		sprintf(buf, "the following problem: Your spell memory decreased rapidly.");
	      sprintf(eos(buf), " (%d)", SpellForgetting);
		dump(youhad, buf);
	}

	if (SoundEffectBug || u.uprops[SOUND_EFFECT_BUG].extrinsic || have_soundeffectstone()) {
		sprintf(buf, "the following problem: Things that happened would play 'sound effects'.");
	      sprintf(eos(buf), " (%d)", SoundEffectBug);
		dump(youhad, buf);
	}

	if (CaptchaProblem || u.uprops[CAPTCHA].extrinsic || have_captchastone()) {
		sprintf(buf, "the following problem: You sometimes had to solve captchas.");
	      sprintf(eos(buf), " (%d)", CaptchaProblem);
		dump(youhad, buf);
	}
	if (FarlookProblem || u.uprops[FARLOOK_BUG].extrinsic || have_farlookstone()) {
		sprintf(buf, "the following problem: Farlooking peaceful monsters angered them.");
	      sprintf(eos(buf), " (%d)", FarlookProblem);
		dump(youhad, buf);
	}
	if (RespawnProblem || u.uprops[RESPAWN_BUG].extrinsic || have_respawnstone()) {
		sprintf(buf, "the following problem: Killing monsters caused them to respawn somewhere on the level.");
	      sprintf(eos(buf), " (%d)", RespawnProblem);
		dump(youhad, buf);
	}

	if (BigscriptEffect || u.uprops[BIGSCRIPT].extrinsic || have_bigscriptstone()) {
		sprintf(buf, "BIGscript.");
	      sprintf(eos(buf), " (%d)", BigscriptEffect);
		dump(youhad, buf);
	}
	if (BankTrapEffect || u.uprops[BANKBUG].extrinsic || have_bankstone()) {
		sprintf(buf, "the following problem: Your money wandered into a mysterious bank.");
	      sprintf(eos(buf), " (%d)", BankTrapEffect);
	      sprintf(eos(buf), " (amount stored: %d)", u.bankcashamount);
	      sprintf(eos(buf), " (money limit: %d)", u.bankcashlimit);
		dump(youhad, buf);
	} else {
	  if (u.bankcashamount) {
		sprintf(buf, "the following amount of cash stored in the bank:");
		sprintf(eos(buf), " %d", u.bankcashamount);
		dump(youhad, buf);
	  }
	  if (u.bankcashlimit) {
		sprintf(buf, "the following bank cash limit:");
		sprintf(eos(buf), " %d", u.bankcashlimit);
		dump(youhad, buf);
	  }
	}
	if (MapTrapEffect || u.uprops[MAPBUG].extrinsic || have_mapstone()) {
		sprintf(buf, "the following problem: The map didn't display correctly.");
	      sprintf(eos(buf), " (%d)", MapTrapEffect);
		dump(youhad, buf);
	}
	if (TechTrapEffect || u.uprops[TECHBUG].extrinsic || have_techniquestone()) {
		sprintf(buf, "the following problem: Your techniques frequently didn't work.");
	      sprintf(eos(buf), " (%d)", TechTrapEffect);
		dump(youhad, buf);
	}
	if (RecurringDisenchant || u.uprops[RECURRING_DISENCHANT].extrinsic || have_disenchantmentstone()) {
		sprintf(buf, "the following problem: Your possessions disenchanted themselves spontaneously.");
	      sprintf(eos(buf), " (%d)", RecurringDisenchant);
		dump(youhad, buf);
	}
	if (verisiertEffect || u.uprops[VERISIERTEFFECT].extrinsic || have_verisiertstone()) {
		sprintf(buf, "the following problem: Monster respawn speeded up rapidly.");
	      sprintf(eos(buf), " (%d)", verisiertEffect);
		dump(youhad, buf);
	}
	if (ChaosTerrain || u.uprops[CHAOS_TERRAIN].extrinsic || have_chaosterrainstone()) {
		sprintf(buf, "the following problem: The terrain slowly became ever more chaotic.");
	      sprintf(eos(buf), " (%d)", ChaosTerrain);
		dump(youhad, buf);
	}
	if (Muteness || u.uprops[MUTENESS].extrinsic || have_mutenessstone()) {
		sprintf(buf, "the following problem: You were mute, and had great difficulty when trying to cast spells.");
	      sprintf(eos(buf), " (%d)", Muteness);
		dump(youhad, buf);
	}
	if (EngravingDoesntWork || u.uprops[ENGRAVINGBUG].extrinsic || have_engravingstone()) {
		sprintf(buf, "the following problem: Monsters didn't respect Elbereth.");
	      sprintf(eos(buf), " (%d)", EngravingDoesntWork);
		dump(youhad, buf);
	}
	if (MagicDeviceEffect || u.uprops[MAGIC_DEVICE_BUG].extrinsic || have_magicdevicestone()) {
		sprintf(buf, "the following problem: Zapping a wand could cause it to explode.");
	      sprintf(eos(buf), " (%d)", MagicDeviceEffect);
		dump(youhad, buf);
	}
	if (BookTrapEffect || u.uprops[BOOKBUG].extrinsic || have_bookstone()) {
		sprintf(buf, "the following problem: Reading spellbooks confused you.");
	      sprintf(eos(buf), " (%d)", BookTrapEffect);
		dump(youhad, buf);
	}
	if (LevelTrapEffect || u.uprops[LEVELBUG].extrinsic || have_levelstone()) {
		sprintf(buf, "the following problem: Monsters became stronger if many of their species had been generated already.");
	    sprintf(eos(buf), " (%d)", LevelTrapEffect);
		dump(youhad, buf);
	}
	if (QuizTrapEffect || u.uprops[QUIZZES].extrinsic || have_quizstone()) {
		sprintf(buf, "the following problem: You had to partake in the Great NetHack Quiz.");
	      sprintf(eos(buf), " (%d)", QuizTrapEffect);
		dump(youhad, buf);
	}

	if (FastMetabolismEffect || u.uprops[FAST_METABOLISM].extrinsic || have_metabolicstone()) {
		sprintf(buf, "the following problem: Your food consumption was much faster.");
	      sprintf(eos(buf), " (%d)", FastMetabolismEffect);
		dump(youhad, buf);
	}

	if (NoReturnEffect || u.uprops[NORETURN].extrinsic || have_noreturnstone()) {
		sprintf(buf, "the following problem: You could not teleport at all.");
	      sprintf(eos(buf), " (%d)", NoReturnEffect);
		dump(youhad, buf);
	}

	if (AlwaysEgotypeMonsters || u.uprops[ALWAYS_EGOTYPES].extrinsic || have_egostone()) {
		sprintf(buf, "the following problem: Monsters always spawned with egotypes.");
	      sprintf(eos(buf), " (%d)", AlwaysEgotypeMonsters);
		dump(youhad, buf);
	}

	if (TimeGoesByFaster || u.uprops[FAST_FORWARD].extrinsic || have_fastforwardstone()) {
		sprintf(buf, "the following problem: Time went by faster.");
	      sprintf(eos(buf), " (%d)", TimeGoesByFaster);
		dump(youhad, buf);
	}

	if (FoodIsAlwaysRotten || u.uprops[FOOD_IS_ROTTEN].extrinsic || have_rottenstone()) {
		sprintf(buf, "the following problem: Your food was always rotten.");
	      sprintf(eos(buf), " (%d)", FoodIsAlwaysRotten);
		dump(youhad, buf);
	}

	if (AllSkillsUnskilled || u.uprops[SKILL_DEACTIVATED].extrinsic || have_unskilledstone()) {
		sprintf(buf, "the following problem: Your skills were deactivated.");
	      sprintf(eos(buf), " (%d)", AllSkillsUnskilled);
		dump(youhad, buf);
	}

	if (AllStatsAreLower || u.uprops[STATS_LOWERED].extrinsic || have_lowstatstone()) {
		sprintf(buf, "the following problem: All of your stats were lowered.");
	      sprintf(eos(buf), " (%d)", AllStatsAreLower);
		dump(youhad, buf);
	}

	if (PlayerCannotTrainSkills || u.uprops[TRAINING_DEACTIVATED].extrinsic || have_trainingstone()) {
		sprintf(buf, "the following problem: You could not train skills.");
	      sprintf(eos(buf), " (%d)", PlayerCannotTrainSkills);
		dump(youhad, buf);
	}

	if (PlayerCannotExerciseStats || u.uprops[EXERCISE_DEACTIVATED].extrinsic || have_exercisestone()) {
		sprintf(buf, "the following problem: You could not exercise your stats.");
	      sprintf(eos(buf), " (%d)", PlayerCannotExerciseStats);
		dump(youhad, buf);
	}

	if (MCReduction) {
		sprintf(buf, "reduced magic cancellation.");
	      sprintf(eos(buf), " (%d)", MCReduction);
		dump(youhad, buf);
	}

	if (u.uprops[INTRINSIC_LOSS].extrinsic || IntrinsicLossProblem || have_intrinsiclossstone() ) {
		sprintf(buf, "a case of random intrinsic loss.");
	      sprintf(eos(buf), " (%d)", IntrinsicLossProblem);
		dump(youhad, buf);
	}
	if (u.uprops[TRAP_REVEALING].extrinsic) {
		sprintf(buf, "randomly revealing traps");
		dump(youwere, buf);
	}
	if (u.uprops[BLOOD_LOSS].extrinsic || BloodLossProblem || have_bloodlossstone() ) {
		sprintf(buf, "bleeding out");
	      sprintf(eos(buf), " (%d)", BloodLossProblem);
		dump(youwere, buf);
	}
	if (u.uprops[NASTINESS_EFFECTS].extrinsic || NastinessProblem || have_nastystone() ) {
		sprintf(buf, "subjected to random nasty trap effects");
	      sprintf(eos(buf), " (%d)", NastinessProblem);
		dump(youwere, buf);
	}
	if (u.uprops[BAD_EFFECTS].extrinsic || BadEffectProblem || have_badeffectstone() ) {
		sprintf(buf, "subjected to random bad effects");
	      sprintf(eos(buf), " (%d)", BadEffectProblem);
		dump(youwere, buf);
	}
	if (u.uprops[AUTOMATIC_TRAP_CREATION].extrinsic || TrapCreationProblem || have_trapcreationstone() ) {
		sprintf(buf, "more likely to encounter traps");
	      sprintf(eos(buf), " (%d)", TrapCreationProblem);
		dump(youwere, buf);
	}
	if (u.uprops[SENTIENT_HIGH_HEELS].extrinsic) {
		sprintf(buf, "wearing sentient high heels");
		dump(youwere, buf);
	}
	if (u.uprops[REPEATING_VULNERABILITY].extrinsic || AutomaticVulnerabilitiy || have_vulnerabilitystone() ) {
		sprintf(buf, "the risk of temporarily losing intrinsics");
	      sprintf(eos(buf), " (%d)", AutomaticVulnerabilitiy);
		dump(youhad, buf);
	}
	if (u.uprops[TELEPORTING_ITEMS].extrinsic || TeleportingItems || have_itemportstone() ) {
		sprintf(buf, "teleporting items");
	      sprintf(eos(buf), " (%d)", TeleportingItems);
		dump(youhad, buf);
	}

	if (u.uprops[DESECRATION].extrinsic || Desecration || have_nonsacredstone() ) {
		sprintf(buf, "the following problem: Altars malfunction whenever you try to use them");
	      sprintf(eos(buf), " (%d)", Desecration);
		dump(youhad, buf);
	}

	if (u.uprops[STARVATION_EFFECT].extrinsic || StarvationEffect || have_starvationstone() ) {
		sprintf(buf, "not going to get corpses from defeated enemies");
	      sprintf(eos(buf), " (%d)", StarvationEffect);
		dump(youwere, buf);
	}

	if (u.uprops[NO_DROPS_EFFECT].extrinsic || NoDropsEffect || have_droplessstone() ) {
		sprintf(buf, "not going to get death drops from monsters");
	      sprintf(eos(buf), " (%d)", NoDropsEffect);
		dump(youwere, buf);
	}

	if (u.uprops[LOW_EFFECTS].extrinsic || LowEffects || have_loweffectstone() ) {
		sprintf(buf, "reduced power of magical effects");
	      sprintf(eos(buf), " (%d)", LowEffects);
		dump(youhad, buf);
	}

	if (u.uprops[INVIS_TRAPS_EFFECT].extrinsic || InvisibleTrapsEffect || have_invisostone() ) {
		sprintf(buf, "not going to see traps");
	      sprintf(eos(buf), " (%d)", InvisibleTrapsEffect);
		dump(youwere, buf);
	}

	if (u.uprops[GHOST_WORLD].extrinsic || GhostWorld || have_ghostlystone() ) {
		sprintf(buf, "in a ghost world");
	      sprintf(eos(buf), " (%d)", GhostWorld);
		dump(youwere, buf);
	}

	if (u.uprops[DEHYDRATION].extrinsic || Dehydration || have_dehydratingstone() ) {
		sprintf(buf, "going to become dehydrated");
	      sprintf(eos(buf), " (%d)", Dehydration);
		dump(youwere, buf);
	}

	if (u.uprops[HATE_TRAP_EFFECT].extrinsic || HateTrapEffect || have_hatestone() ) {
		sprintf(buf, "the following problem: Pets hate you with a fiery passion.");
	      sprintf(eos(buf), " (%d)", HateTrapEffect);
		dump(youhad, buf);
	}

	if (u.uprops[TOTTER_EFFECT].extrinsic || TotterTrapEffect || have_directionswapstone() ) {
		sprintf(buf, "the following problem: Your directional keys got swapped.");
	      sprintf(eos(buf), " (%d)", TotterTrapEffect);
		dump(youhad, buf);
	}

	if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) {
		sprintf(buf, "unable to get intrinsics from eating corpses");
	      sprintf(eos(buf), " (%d)", Nonintrinsics);
		dump(youwere, buf);
	}

	if (u.uprops[DROPCURSES_EFFECT].extrinsic || Dropcurses || have_dropcursestone() ) {
		sprintf(buf, "the following problem: Dropping items causes them to autocurse.");
	      sprintf(eos(buf), " (%d)", Dropcurses);
		dump(youhad, buf);
	}

	if (u.uprops[NAKEDNESS].extrinsic || Nakedness || have_nakedstone() ) {
		sprintf(buf, "effectively naked");
	      sprintf(eos(buf), " (%d)", Nakedness);
		dump(youwere, buf);
	}

	if (u.uprops[ANTILEVELING].extrinsic || Antileveling || have_antilevelstone() ) {
		sprintf(buf, "unable to gain experience past experience level 10, and slower to reach XL10");
	      sprintf(eos(buf), " (%d)", Antileveling);
		dump(youwere, buf);
	}

	if (u.uprops[ITEM_STEALING_EFFECT].extrinsic || ItemStealingEffect || have_stealerstone() ) {
		sprintf(buf, "more likely to have your items stolen");
	      sprintf(eos(buf), " (%d)", ItemStealingEffect);
		dump(youwere, buf);
	}

	if (u.uprops[REBELLION_EFFECT].extrinsic || Rebellions || have_rebelstone() ) {
		sprintf(buf, "the following problem: Pets can spontaneously rebel and become hostile.");
	      sprintf(eos(buf), " (%d)", Rebellions);
		dump(youhad, buf);
	}

	if (u.uprops[CRAP_EFFECT].extrinsic || CrapEffect || have_shitstone() ) {
		sprintf(buf, "diarrhea");
	      sprintf(eos(buf), " (%d)", CrapEffect);
		dump(youhad, buf);
	}

	if (u.uprops[PROJECTILES_MISFIRE].extrinsic || ProjectilesMisfire || have_misfirestone() ) {
		sprintf(buf, "the following problem: Your projectiles often misfire.");
	      sprintf(eos(buf), " (%d)", ProjectilesMisfire);
		dump(youhad, buf);
	}

	if (u.uprops[WALL_TRAP_EFFECT].extrinsic || WallTrapping || have_wallstone() ) {
		sprintf(buf, "the following problem: Dungeon regrowth is excessively fast.");
	      sprintf(eos(buf), " (%d)", WallTrapping);
		dump(youhad, buf);
	}

	if (u.uprops[RECURRING_AMNESIA].extrinsic || RecurringAmnesia || have_amnesiastone() ) {
		sprintf(buf, "going to suffer from amnesia now and then");
	      sprintf(eos(buf), " (%d)", RecurringAmnesia);
		dump(youwere, buf);
	}

	if (u.uprops[TURNLIMITATION].extrinsic || TurnLimitation || have_limitationstone() ) {
		sprintf(buf, "getting your ascension turn limit reduced whenever you take damage");
	      sprintf(eos(buf), " (%d)", TurnLimitation);
		dump(youwere, buf);
	}

	if (u.uprops[WEAKSIGHT].extrinsic || WeakSight || have_weaksightstone() ) {
		sprintf(buf, "short-sighted");
	      sprintf(eos(buf), " (%d)", WeakSight);
		dump(youwere, buf);
	}

	if (u.uprops[RANDOM_MESSAGES].extrinsic || RandomMessages || have_messagestone() ) {
		sprintf(buf, "reading random messages");
	      sprintf(eos(buf), " (%d)", RandomMessages);
		dump(youwere, buf);
	}


	if (u.uprops[RANDOM_RUMORS].extrinsic) {
		sprintf(buf, "going to listen to random rumors");
		dump(youwere, buf);
	}

	if (IncreasedGravity) {
		sprintf(buf, "increased encumbrance due to a stronger gravity.");
	      sprintf(eos(buf), " (%d)", IncreasedGravity);
		dump(youhad, buf);
	}

	if (NoStaircase) {
		sprintf(buf, "to wait until you could use staircases again.");
	      sprintf(eos(buf), " (%d)", NoStaircase);
		dump(youhad, buf);
	}

	if (u.ragnaroktimer) {
		sprintf(buf, "going to experience Ragnarok.");
	      sprintf(eos(buf), " (%d)", u.ragnaroktimer);
		dump(youwere, buf);
	}

	if (u.sokosolveboulder && issokosolver) {
		sprintf(buf, "to wait until you can create boulders again.");
	      sprintf(eos(buf), " (%d)", u.sokosolveboulder);
		dump(youhad, buf);
	}

	if (u.sokosolveuntrap && issokosolver) {
		sprintf(buf, "to wait until you can disarm traps again.");
	      sprintf(eos(buf), " (%d)", u.sokosolveuntrap);
		dump(youhad, buf);
	}

	if (u.footererlevel) {
		sprintf(buf, "going to encounter the footerers.");
	      sprintf(eos(buf), " (level %d)", u.footererlevel);
		dump(youwere, buf);
	}

	if (FemaleTrapFemmy) {
		sprintf(buf, "possessed by the ghost of Femmy.");
	      sprintf(eos(buf), " (%d)", FemaleTrapFemmy);
		dump(youwere, buf);
	}

	if (FemaleTrapMadeleine) {
		sprintf(buf, "possessed by the ghost of Madeleine.");
	      sprintf(eos(buf), " (%d)", FemaleTrapMadeleine);
		dump(youwere, buf);
	}

	if (FemaleTrapMarlena) {
		sprintf(buf, "possessed by the ghost of Marlena.");
	      sprintf(eos(buf), " (%d)", FemaleTrapMarlena);
		dump(youwere, buf);
	}

	if (FemaleTrapSarah) {
		sprintf(buf, "possessed by the ghost of Sarah.");
	      sprintf(eos(buf), " (%d)", FemaleTrapSarah);
		dump(youwere, buf);
	}

	if (FemaleTrapClaudia) {
		sprintf(buf, "possessed by the ghost of Claudia.");
	      sprintf(eos(buf), " (%d)", FemaleTrapClaudia);
		dump(youwere, buf);
	}

	if (FemaleTrapLudgera) {
		sprintf(buf, "possessed by the ghost of Ludgera.");
	      sprintf(eos(buf), " (%d)", FemaleTrapLudgera);
		dump(youwere, buf);
	}

	if (FemaleTrapKati) {
		sprintf(buf, "possessed by the ghost of Kati.");
	      sprintf(eos(buf), " (%d)", FemaleTrapKati);
		dump(youwere, buf);
	}

	if (FemaleTrapAnastasia) {
		sprintf(buf, "possessed by the ghost of Anastasia.");
	      sprintf(eos(buf), " (%d)", FemaleTrapAnastasia);
		dump(youwere, buf);
	}

	if (FemaleTrapJessica) {
		sprintf(buf, "possessed by the ghost of Jessica.");
	      sprintf(eos(buf), " (%d)", FemaleTrapJessica);
		dump(youwere, buf);
	}

	if (FemaleTrapSolvejg) {
		sprintf(buf, "possessed by the ghost of Solvejg.");
	      sprintf(eos(buf), " (%d)", FemaleTrapSolvejg);
		dump(youwere, buf);
	}

	if (FemaleTrapWendy) {
		sprintf(buf, "possessed by the ghost of Wendy.");
	      sprintf(eos(buf), " (%d)", FemaleTrapWendy);
		dump(youwere, buf);
	}

	if (FemaleTrapKatharina) {
		sprintf(buf, "possessed by the ghost of Katharina.");
	      sprintf(eos(buf), " (%d)", FemaleTrapKatharina);
		dump(youwere, buf);
	}

	if (FemaleTrapElena) {
		sprintf(buf, "possessed by the ghost of Elena.");
	      sprintf(eos(buf), " (%d)", FemaleTrapElena);
		dump(youwere, buf);
	}

	if (FemaleTrapThai) {
		sprintf(buf, "possessed by the ghost of Thai.");
	      sprintf(eos(buf), " (%d)", FemaleTrapThai);
		dump(youwere, buf);
	}

	if (FemaleTrapElif) {
		sprintf(buf, "possessed by the ghost of Elif.");
	      sprintf(eos(buf), " (%d)", FemaleTrapElif);
		dump(youwere, buf);
	}

	if (FemaleTrapNadja) {
		sprintf(buf, "possessed by the ghost of Nadja.");
	      sprintf(eos(buf), " (%d)", FemaleTrapNadja);
		dump(youwere, buf);
	}

	if (FemaleTrapSandra) {
		sprintf(buf, "possessed by the ghost of Sandra.");
	      sprintf(eos(buf), " (%d)", FemaleTrapSandra);
		dump(youwere, buf);
	}

	if (FemaleTrapNatalje) {
		sprintf(buf, "possessed by the ghost of Natalje.");
	      sprintf(eos(buf), " (%d)", FemaleTrapNatalje);
		dump(youwere, buf);
	}

	if (FemaleTrapJeanetta) {
		sprintf(buf, "possessed by the ghost of Jeanetta.");
	      sprintf(eos(buf), " (%d)", FemaleTrapJeanetta);
		dump(youwere, buf);
	}

	if (FemaleTrapYvonne) {
		sprintf(buf, "possessed by the ghost of Yvonne.");
	      sprintf(eos(buf), " (%d)", FemaleTrapYvonne);
		dump(youwere, buf);
	}

	if (FemaleTrapMaurah) {
		sprintf(buf, "possessed by the ghost of Maurah.");
	      sprintf(eos(buf), " (%d)", FemaleTrapMaurah);
		dump(youwere, buf);
	}

	if (FemaleTrapMeltem) {
		sprintf(buf, "possessed by the ghost of Meltem.");
	      sprintf(eos(buf), " (%d)", FemaleTrapMeltem);
		dump(youwere, buf);
	}

	if (u.hangupamount) {
		sprintf(buf, "used the hangup function, which means you're either a FILTHY CHEAT0R or you have a shaky internet connection. Hopefully it's the latter so Amy doesn't have to reinstate the hangup penalty.");
	    	sprintf(eos(buf), " (%d)", u.hangupamount);
		dump(youhad, buf);

	}

	if (Race_if(PM_FELID)) {
	    	sprintf(buf, "%d ", u.felidlives);
		sprintf(eos(buf), "lives left");
		dump(youhad, buf);
	}
	if (u.homosexual == 0) {
	    	sprintf(buf, "not decided on your sexuality yet");
		dump(youhad, buf);
	}
	if (u.homosexual == 1) {
	    	sprintf(buf, "heterosexual");
		dump(youwere, buf);
	}
	if (u.homosexual == 2) {
	    	sprintf(buf, "homosexual");
		dump(youwere, buf);
	}

	if (Role_if(PM_HUSSY)) {
	    	sprintf(buf, "%d ", (u.hussyhurtturn - moves));
		sprintf(eos(buf), "turns left to hit a guy.");
		dump(youhad, buf);
	    	sprintf(buf, "%d ", (u.hussykillturn - moves));
		sprintf(eos(buf), "turns left to defeat a guy.");
		dump(youhad, buf);
	}

	if (u.contamination) {
		if (u.contamination >= 1000) sprintf(buf, "suffering from fatal contamination. Health and mana regeneration were reduced.");
		else if (u.contamination >= 800) sprintf(buf, "suffering from lethal contamination.");
		else if (u.contamination >= 600) sprintf(buf, "suffering from severe contamination.");
		else if (u.contamination >= 400) sprintf(buf, "suffering from contamination.");
		else if (u.contamination >= 200) sprintf(buf, "suffering from light contamination.");
		else if (u.contamination >= 100) sprintf(buf, "suffering from minor contamination.");
		else if (u.contamination >= 1) sprintf(buf, "suffering from very slight contamination.");
		sprintf(eos(buf), " (%d)", u.contamination);
		dump(youwere, buf);
	}

	if (u.copwantedlevel) {
		sprintf(buf, "being chased by the kops.");
		sprintf(eos(buf), " (%d)", u.copwantedlevel);
		dump(youwere, buf);
	}

	if (u.gmmailsreceived) {
		sprintf(buf, "received monster-summoning mail from AmyBSOD, the Game Master.");
	    	sprintf(eos(buf), " (%d)", u.gmmailsreceived);
		dump(youhad, buf);
	}

	if (TimeStopped) {
		sprintf(buf, "stopped the flow of time.");
	      sprintf(eos(buf), " (%d)", TimeStopped);
		dump(youhad, buf);
	}

	if (u.drippingtread) {
		sprintf(buf, "dripping elements.");
	      sprintf(eos(buf), " (%d)", u.drippingtread);
		dump(youwere, buf);
	}

	if (u.geolysis) {
		sprintf(buf, "using geolysis.");
	      sprintf(eos(buf), " (%d)", u.geolysis);
		dump(youwere, buf);
	}

	if (u.disruptionshield) {
		sprintf(buf, "your disruption shield up.");
	      sprintf(eos(buf), " (%d)", u.disruptionshield);
		dump(youhad, buf);
	}

	if (u.inertiacontrol) {
		sprintf(buf, "controlling the flow of a spell.");
	      sprintf(eos(buf), " (%d)", u.inertiacontrol);
		dump(youwere, buf);
	}

	if (u.sterilized) {
		sprintf(buf, "sterilized the air around you.");
	      sprintf(eos(buf), " (%d)", u.sterilized);
		dump(youhad, buf);
	}

	if (u.holyshield) {
		sprintf(buf, "your holy shield up.");
	      sprintf(eos(buf), " (%d)", u.holyshield);
		dump(youhad, buf);
	}

	if (u.berserktime) {
		sprintf(buf, "berserk.");
	      sprintf(eos(buf), " (%d)", u.berserktime);
		dump(youwere, buf);
	}

	if (u.magicshield) {
		sprintf(buf, "your magic shield up.");
	      sprintf(eos(buf), " (%d)", u.magicshield);
		dump(youhad, buf);
	}

	if (u.burrowed) {
		sprintf(buf, "burrowed into the ground.");
	      sprintf(eos(buf), " (%d)", u.burrowed);
		dump(youwere, buf);
	}

	if (u.stasistime) {
		sprintf(buf, "in stasis, and the monsters were too.");
	      sprintf(eos(buf), " (%d)", u.stasistime);
		dump(youwere, buf);
	}

	if (u.powerfailure) {
		sprintf(buf, "experiencing a power failure.");
	      sprintf(eos(buf), " (%d)", u.powerfailure);
		dump(youwere, buf);
	}

	if (u.tunnelized) {
		sprintf(buf, "reduced armor class due to tunneling dirt on your clothes.");
	      sprintf(eos(buf), " (%d)", u.tunnelized);
		dump(youhad, buf);
	}

	if (u.nailpolish) {
		sprintf(buf, "applied nail polish.");
	      sprintf(eos(buf), " (%d)", u.nailpolish);
		dump(youhad, buf);
	}

	if (u.enchantspell) {
		sprintf(buf, "enchanted your weapon with fire.");
	      sprintf(eos(buf), " (%d)", u.enchantspell);
		dump(youhad, buf);
	}

	if (u.thornspell) {
		sprintf(buf, "thorns.");
	      sprintf(eos(buf), " (%d)", u.thornspell);
		dump(youhad, buf);
	}

	if (u.egglayingtimeout) {
		sprintf(buf, "to wait until you can lay eggs again.");
	      sprintf(eos(buf), " (%d)", u.egglayingtimeout);
		dump(youhad, buf);
	}

	if (u.swappositioncount) {
		sprintf(buf, "going to swap positions with the next monster you move into.");
	      sprintf(eos(buf), " (%d)", u.swappositioncount);
		dump(youwere, buf);
	}

	if (u.contingencyturns) {
		sprintf(buf, "signed up a contract with the reaper.");
	      sprintf(eos(buf), " (%d)", u.contingencyturns);
		dump(youhad, buf);
	}

	if (u.horsehopturns) {
		sprintf(buf, "the ability to jump as long as you're riding.");
	      sprintf(eos(buf), " (%d)", u.horsehopturns);
		dump(youhad, buf);
	}

	if (u.bodyfluideffect) {
		sprintf(buf, "very acidic skin.");
	      sprintf(eos(buf), " (%d)", u.bodyfluideffect);
		dump(youhad, buf);
	}

	if (u.antitelespelltimeout) {
		sprintf(buf, "erected an anti-teleportation field.");
	      sprintf(eos(buf), " (%d)", u.antitelespelltimeout);
		dump(youhad, buf);
	}

	if (u.breathenhancetimer) {
		sprintf(buf, "magically enhanced breath.");
	      sprintf(eos(buf), " (%d)", u.breathenhancetimer);
		dump(youhad, buf);
	}

	if (u.snaildigging) {
		sprintf(buf, "to wait until you can fire another digging ray.");
	      sprintf(eos(buf), " (%d)", u.snaildigging);
		dump(youhad, buf);
	}

	if (u.hussyperfume) {
		sprintf(buf, "to wait until you can spread the perfume again.");
	      sprintf(eos(buf), " (%d)", u.hussyperfume);
		dump(youhad, buf);
	}

	if (Role_if(PM_JANITOR)) {
		sprintf(buf, "the following amount of trash in your trash can:");
	      sprintf(eos(buf), " %d", u.garbagecleaned);
		dump(youhad, buf);
	}

	if (Role_if(PM_JANITOR) && u.garbagetrucktime) {
		sprintf(buf, "called the garbage truck, and it will arrive in");
	      sprintf(eos(buf), " %d turns", u.garbagetrucktime);
		dump(youhad, buf);
	}

	if (u.walscholarpass) {
		sprintf(buf, "able to pass through grave walls.");
	      sprintf(eos(buf), " (%d)", u.walscholarpass);
		dump(youwere, buf);
	}

	if (u.demagogueabilitytimer && isdemagogue) {
		sprintf(buf, "to wait until you can use your special recursion.");
	      sprintf(eos(buf), " (%d)", u.demagogueabilitytimer);
		dump(youhad, buf);
	}

	if (u.acutraining) {
		sprintf(buf, "double skill training.");
	      sprintf(eos(buf), " (%d)", u.acutraining);
		dump(youhad, buf);
	}

	if (u.dragonpolymorphtime) {
		sprintf(buf, "to wait until you can polymorph into a dragon again.");
	      sprintf(eos(buf), " (%d)", u.dragonpolymorphtime);
		dump(youhad, buf);
	}

	if (UseTheForce) {
		sprintf(buf, StrongUseTheForce ? "able to use the force like a jedi grandmaster" : "able to use the force like a true jedi");
		dump(youwere, buf);
	}

	if (u.uprops[SENSORY_DEPRIVATION].extrinsic) {
		sprintf(buf, "sensory deprivation.");
		dump(youhad, buf);
	}

	if (NoFire_resistance) {
		sprintf(buf, "prevented from having fire resistance");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_FIRE_RES].intrinsic);
		dump(youwere, buf);
	}
	if (NoCold_resistance) {
		sprintf(buf, "prevented from having cold resistance");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_COLD_RES].intrinsic);
		dump(youwere, buf);
	}
	if (NoSleep_resistance) {
		sprintf(buf, "prevented from having sleep resistance");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_SLEEP_RES].intrinsic);
		dump(youwere, buf);
	}
	if (NoDisint_resistance) {
		sprintf(buf, "prevented from having disintegration resistance");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_DISINT_RES].intrinsic);
		dump(youwere, buf);
	}
	if (NoShock_resistance) {
		sprintf(buf, "prevented from having shock resistance");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_SHOCK_RES].intrinsic);
		dump(youwere, buf);
	}
	if (NoPoison_resistance) {
		sprintf(buf, "prevented from having poison resistance");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_POISON_RES].intrinsic);
		dump(youwere, buf);
	}
	if (NoDrain_resistance) {
		sprintf(buf, "prevented from having drain resistance");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_DRAIN_RES].intrinsic);
		dump(youwere, buf);
	}
	if (NoSick_resistance) {
		sprintf(buf, "prevented from having sickness resistance");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_SICK_RES].intrinsic);
		dump(youwere, buf);
	}
	if (NoAntimagic_resistance) {
		sprintf(buf, "prevented from having magic resistance");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_ANTIMAGIC].intrinsic);
		dump(youwere, buf);
	}
	if (NoAcid_resistance) {
		sprintf(buf, "prevented from having acid resistance");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_ACID_RES].intrinsic);
		dump(youwere, buf);
	}
	if (NoStone_resistance) {
		sprintf(buf, "prevented from having petrification resistance");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_STONE_RES].intrinsic);
		dump(youwere, buf);
	}
	if (NoFear_resistance) {
		sprintf(buf, "prevented from having fear resistance");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_FEAR_RES].intrinsic);
		dump(youwere, buf);
	}
	if (NoSee_invisible) {
		sprintf(buf, "prevented from having see invisible");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_SEE_INVIS].intrinsic);
		dump(youwere, buf);
	}
	if (NoManaleech) {
		sprintf(buf, "prevented from having manaleech");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_MANALEECH].intrinsic);
		dump(youwere, buf);
	}
	if (NoPeacevision) {
		sprintf(buf, "prevented from having peacevision");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_PEACEVISION].intrinsic);
		dump(youwere, buf);
	}
	if (NoTelepat) {
		sprintf(buf, "prevented from having telepathy");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_TELEPAT].intrinsic);
		dump(youwere, buf);
	}
	if (NoWarning) {
		sprintf(buf, "prevented from having warning");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_WARNING].intrinsic);
		dump(youwere, buf);
	}
	if (NoSearching) {
		sprintf(buf, "prevented from having automatic searching");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_SEARCHING].intrinsic);
		dump(youwere, buf);
	}
	if (NoClairvoyant) {
		sprintf(buf, "prevented from having clairvoyance");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_CLAIRVOYANT].intrinsic);
		dump(youwere, buf);
	}
	if (NoInfravision) {
		sprintf(buf, "prevented from having infravision");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_INFRAVISION].intrinsic);
		dump(youwere, buf);
	}
	if (NoDetect_monsters) {
		sprintf(buf, "prevented from having detect monsters");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_DETECT_MONSTERS].intrinsic);
		dump(youwere, buf);
	}
	if (NoInvisible) {
		sprintf(buf, "prevented from having invisibility");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_INVIS].intrinsic);
		dump(youwere, buf);
	}
	if (NoDisplaced) {
		sprintf(buf, "prevented from having displacement");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_DISPLACED].intrinsic);
		dump(youwere, buf);
	}
	if (NoStealth) {
		sprintf(buf, "prevented from having stealth");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_STEALTH].intrinsic);
		dump(youwere, buf);
	}
	if (NoJumping) {
		sprintf(buf, "prevented from having jumping");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_JUMPING].intrinsic);
		dump(youwere, buf);
	}
	if (NoTeleport_control) {
		sprintf(buf, "prevented from having teleport control");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_TELEPORT_CONTROL].intrinsic);
		dump(youwere, buf);
	}
	if (NoFlying) {
		sprintf(buf, "prevented from having flying");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_FLYING].intrinsic);
		dump(youwere, buf);
	}
	if (NoBreathless) {
		sprintf(buf, "prevented from having magical breathing");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_MAGICAL_BREATHING].intrinsic);
		dump(youwere, buf);
	}
	if (NoPasses_walls) {
		sprintf(buf, "prevented from having phasing");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_PASSES_WALLS].intrinsic);
		dump(youwere, buf);
	}
	if (NoSlow_digestion) {
		sprintf(buf, "prevented from having slow digestion");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_SLOW_DIGESTION].intrinsic);
		dump(youwere, buf);
	}
	if (NoHalf_spell_damage) {
		sprintf(buf, "prevented from having half spell damage");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_HALF_SPDAM].intrinsic);
		dump(youwere, buf);
	}
	if (NoHalf_physical_damage) {
		sprintf(buf, "prevented from having half physical damage");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_HALF_PHDAM].intrinsic);
		dump(youwere, buf);
	}
	if (NoRegeneration) {
		sprintf(buf, "prevented from having regeneration");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_REGENERATION].intrinsic);
		dump(youwere, buf);
	}
	if (NoEnergy_regeneration) {
		sprintf(buf, "prevented from having mana regeneration");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_ENERGY_REGENERATION].intrinsic);
		dump(youwere, buf);
	}
	if (NoPolymorph_control) {
		sprintf(buf, "prevented from having polymorph control");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_POLYMORPH_CONTROL].intrinsic);
		dump(youwere, buf);
	}
	if (NoFast) {
		sprintf(buf, "prevented from having speed");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_FAST].intrinsic);
		dump(youwere, buf);
	}
	if (NoReflecting) {
		sprintf(buf, "prevented from having reflection");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_REFLECTING].intrinsic);
		dump(youwere, buf);
	}
	if (NoKeen_memory) {
		sprintf(buf, "prevented from having keen memory");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_KEEN_MEMORY].intrinsic);
		dump(youwere, buf);
	}
	if (NoSight_bonus) {
		sprintf(buf, "prevented from having a sight bonus");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_SIGHT_BONUS].intrinsic);
		dump(youwere, buf);
	}
	if (NoVersus_curses) {
		sprintf(buf, "prevented from having curse resistance");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_VERSUS_CURSES].intrinsic);
		dump(youwere, buf);
	}

	if (NoStun_resist) {
		sprintf(buf, "prevented from having stun resistance");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_STUN_RES].intrinsic);
		dump(youwere, buf);
	}
	if (NoConf_resist) {
		sprintf(buf, "prevented from having confusion resistance");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_CONF_RES].intrinsic);
		dump(youwere, buf);
	}
	if (NoPsi_resist) {
		sprintf(buf, "prevented from having psi resistance");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_PSI_RES].intrinsic);
		dump(youwere, buf);
	}
	if (NoDouble_attack) {
		sprintf(buf, "prevented from having double attacks");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_DOUBLE_ATTACK].intrinsic);
		dump(youwere, buf);
	}
	if (NoQuad_attack) {
		sprintf(buf, "prevented from having quad attacks");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_QUAD_ATTACK].intrinsic);
		dump(youwere, buf);
	}

	if (NoFree_action) {
		sprintf(buf, "prevented from having free action");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_FREE_ACTION].intrinsic);
		dump(youwere, buf);
	}

	if (NoHallu_party) {
		sprintf(buf, "prevented from hallu partying");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_HALLU_PARTY].intrinsic);
		dump(youwere, buf);
	}

	if (NoDrunken_boxing) {
		sprintf(buf, "prevented from drunken boxing");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_DRUNKEN_BOXING].intrinsic);
		dump(youwere, buf);
	}

	if (NoStunnopathy) {
		sprintf(buf, "prevented from having stunnopathy");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_STUNNOPATHY].intrinsic);
		dump(youwere, buf);
	}

	if (NoNumbopathy) {
		sprintf(buf, "prevented from having numbopathy");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_NUMBOPATHY].intrinsic);
		dump(youwere, buf);
	}

	if (NoDimmopathy) {
		sprintf(buf, "prevented from having dimmopathy");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_DIMMOPATHY].intrinsic);
		dump(youwere, buf);
	}

	if (NoFreezopathy) {
		sprintf(buf, "prevented from having freezopathy");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_FREEZOPATHY].intrinsic);
		dump(youwere, buf);
	}

	if (NoStoned_chiller) {
		sprintf(buf, "prevented from being a stoned chiller");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_STONED_CHILLER].intrinsic);
		dump(youwere, buf);
	}

	if (NoCorrosivity) {
		sprintf(buf, "prevented from having corrosivity");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_CORROSIVITY].intrinsic);
		dump(youwere, buf);
	}

	if (NoFear_factor) {
		sprintf(buf, "prevented from having an increased fear factor");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_FEAR_FACTOR].intrinsic);
		dump(youwere, buf);
	}

	if (NoBurnopathy) {
		sprintf(buf, "prevented from having burnopathy");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_BURNOPATHY].intrinsic);
		dump(youwere, buf);
	}

	if (NoSickopathy) {
		sprintf(buf, "prevented from having sickopathy");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_SICKOPATHY].intrinsic);
		dump(youwere, buf);
	}

	if (NoWonderlegs) {
		sprintf(buf, "prevented from having wonderlegs");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_WONDERLEGS].intrinsic);
		dump(youwere, buf);
	}
	if (NoGlib_combat) {
		sprintf(buf, "prevented from having glib combat");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_GLIB_COMBAT].intrinsic);
		dump(youwere, buf);
	}

	if (NoUseTheForce) {
		sprintf(buf, "prevented from using the force like a real jedi");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_THE_FORCE].intrinsic);
		dump(youwere, buf);
	}
	if (NoCont_resist) {
		sprintf(buf, "prevented from having contamination resistance");
	      sprintf(eos(buf), " (%d)", u.uprops[DEAC_CONT_RES].intrinsic);
		dump(youwere, buf);
	}

	if (NoDiscount_action) {
		sprintf(buf, "prevented from having discount action");
	   	sprintf(eos(buf), " (%d)", u.uprops[DEAC_DISCOUNT_ACTION].intrinsic);
		dump(youwere, buf);
	}

	if (NoFull_nutrient) {
		sprintf(buf, "prevented from having full nutrients");
	    	sprintf(eos(buf), " (%d)", u.uprops[DEAC_FULL_NUTRIENT].intrinsic);
		dump(youwere, buf);
	}

	if (NoTechnicality) {
		sprintf(buf, "prevented from having technicality");
	    	sprintf(eos(buf), " (%d)", u.uprops[DEAC_TECHNICALITY].intrinsic);
		dump(youwere, buf);
	}

	if (NoScentView) {
		sprintf(buf, "prevented from having scent view");
		sprintf(eos(buf), " (%d)", u.uprops[DEAC_SCENT_VIEW].intrinsic);
		dump(youwere, buf);
	}

	if (NoDiminishedBleeding) {
		sprintf(buf, "prevented from having diminished bleeding");
		sprintf(eos(buf), " (%d)", u.uprops[DEAC_DIMINISHED_BLEEDING].intrinsic);
		dump(youwere, buf);
	}

	int shieldblockrate = 0;

	if (uarms) {

		switch (uarms->otyp) {

		case SMALL_SHIELD:
			shieldblockrate = 10;
			break;
		case PAPER_SHIELD:
		case DIFFICULT_SHIELD:
			shieldblockrate = 40;
			break;
		case ICKY_SHIELD:
			shieldblockrate = 0;
			break;
		case HEAVY_SHIELD:
			shieldblockrate = 10;
			break;
		case BARRIER_SHIELD:
			shieldblockrate = 30;
			break;
		case TROLL_SHIELD:
		case MAGICAL_SHIELD:
		case SPECIAL_SHIELD:
			shieldblockrate = 20;
			break;
		case TARRIER:
			shieldblockrate = 25;
			break;
		case SHIELD_OF_PEACE:
			shieldblockrate = 20;
			break;
		case ELVEN_SHIELD:
			shieldblockrate = 20;
			if (Race_if(PM_ELF) || Race_if(PM_DROW) || Role_if(PM_ELPH) || Role_if(PM_TWELPH)) shieldblockrate += 5;
			break;
		case URUK_HAI_SHIELD:
			shieldblockrate = 22;
			if (Race_if(PM_ORC)) shieldblockrate += 5;
			break;
		case ORCISH_SHIELD:
		case ORCISH_GUARD_SHIELD:
			shieldblockrate = 18;
			if (Race_if(PM_ORC)) shieldblockrate += 5;
			break;
		case DWARVISH_ROUNDSHIELD:
			shieldblockrate = 24;
			if (Race_if(PM_DWARF)) shieldblockrate += 5;
			if (Role_if(PM_MIDGET)) shieldblockrate += 5;
			break;
		case LARGE_SHIELD:
		case SHIELD:
			shieldblockrate = 25;
			break;
		case STEEL_SHIELD:
			shieldblockrate = 30;
			break;
		case CRYSTAL_SHIELD:
		case RAPIRAPI:
		case HIDE_SHIELD:
			shieldblockrate = 35;
			break;
		case SHIELD_OF_REFLECTION:
		case SILVER_SHIELD:
		case ANCIENT_SHIELD:
		case MIRROR_SHIELD:
			shieldblockrate = 25;
			break;
		case FLAME_SHIELD:
			shieldblockrate = 30;
			break;
		case ICE_SHIELD:
			shieldblockrate = 30;
			break;
		case LIGHTNING_SHIELD:
			shieldblockrate = 30;
			break;
		case VENOM_SHIELD:
		case CHROME_SHIELD:
		case ANTISHADOW_SHIELD:
			shieldblockrate = 30;
			break;
		case SHIELD_OF_LIGHT:
			shieldblockrate = 30;
			break;
		case SHIELD_OF_MOBILITY:
			shieldblockrate = 30;
			break;

		case GRAY_DRAGON_SCALE_SHIELD:
		case SILVER_DRAGON_SCALE_SHIELD:
		case MERCURIAL_DRAGON_SCALE_SHIELD:
		case SHIMMERING_DRAGON_SCALE_SHIELD:
		case DEEP_DRAGON_SCALE_SHIELD:
		case RED_DRAGON_SCALE_SHIELD:
		case WHITE_DRAGON_SCALE_SHIELD:
		case ORANGE_DRAGON_SCALE_SHIELD:
		case BLACK_DRAGON_SCALE_SHIELD:
		case BLUE_DRAGON_SCALE_SHIELD:
		case COPPER_DRAGON_SCALE_SHIELD:
		case PLATINUM_DRAGON_SCALE_SHIELD:
		case BRASS_DRAGON_SCALE_SHIELD:
		case AMETHYST_DRAGON_SCALE_SHIELD:
		case PURPLE_DRAGON_SCALE_SHIELD:
		case DIAMOND_DRAGON_SCALE_SHIELD:
		case EMERALD_DRAGON_SCALE_SHIELD:
		case SAPPHIRE_DRAGON_SCALE_SHIELD:
		case RUBY_DRAGON_SCALE_SHIELD:
		case GREEN_DRAGON_SCALE_SHIELD:
		case GOLDEN_DRAGON_SCALE_SHIELD:
		case FEMINISM_DRAGON_SCALE_SHIELD:
		case NEGATIVE_DRAGON_SCALE_SHIELD:
		case HEROIC_DRAGON_SCALE_SHIELD:
		case STONE_DRAGON_SCALE_SHIELD:
		case CYAN_DRAGON_SCALE_SHIELD:
		case PSYCHIC_DRAGON_SCALE_SHIELD:
		case RAINBOW_DRAGON_SCALE_SHIELD:
		case BLOOD_DRAGON_SCALE_SHIELD:
		case PLAIN_DRAGON_SCALE_SHIELD:
		case SKY_DRAGON_SCALE_SHIELD:
		case WATER_DRAGON_SCALE_SHIELD:
		case MAGIC_DRAGON_SCALE_SHIELD:
		case YELLOW_DRAGON_SCALE_SHIELD:

			shieldblockrate = 23;
			break;

		case EVIL_DRAGON_SCALE_SHIELD:

			shieldblockrate = 33;
			break;

		default: shieldblockrate = 0; /* we don't want to call impossible from here --Amy */

		}

		if (!(PlayerCannotUseSkills)) {
			switch (P_SKILL(P_SHIELD)) {
				case P_BASIC: shieldblockrate += 2; break;
				case P_SKILLED: shieldblockrate += 4; break;
				case P_EXPERT: shieldblockrate += 6; break;
				case P_MASTER: shieldblockrate += 8; break;
				case P_GRAND_MASTER: shieldblockrate += 10; break;
				case P_SUPREME_MASTER: shieldblockrate += 12; break;
			}

		}

		if (uarms->oartifact == ART_LURTZ_S_WALL) shieldblockrate += 20;
		if (uarms->oartifact == ART_I_M_GETTING_HUNGRY) shieldblockrate += 20;
		if (uarms->oartifact == ART_WHANG_CLINK_CLONK) shieldblockrate += 10;
		if (uarms->oartifact == ART_BLOCKING_EXTREME) shieldblockrate += 10;
		if (uarms->oartifact == ART_CUTTING_THROUGH) shieldblockrate += 5;
		if (uwep && uwep->oartifact == ART_VEST_REPLACEMENT) shieldblockrate += 10;

		if (u.holyshield) shieldblockrate += (3 + spell_damage_bonus(SPE_HOLY_SHIELD));

		if (uarms->spe > 0) shieldblockrate += (uarms->spe * 2);

		if (uarms->cursed) shieldblockrate /= 2;
		if (uarms->blessed) shieldblockrate += 5;

		if (uarms->spe < 0) shieldblockrate += (uarms->spe * 2);

		if (uarm && uarm->oartifact == ART_WOODSTOCK) shieldblockrate += 5;
		if (Numbed) shieldblockrate -= 10;

		if (!PlayerCannotUseSkills) {
			switch (P_SKILL(P_SHIEN)) {
				case P_BASIC: shieldblockrate += 1; break;
				case P_SKILLED: shieldblockrate += 2; break;
				case P_EXPERT: shieldblockrate += 3; break;
				case P_MASTER: shieldblockrate += 4; break;
				case P_GRAND_MASTER: shieldblockrate += 5; break;
				case P_SUPREME_MASTER: shieldblockrate += 6; break;
			}
		}

		if (Conflict && shieldblockrate > 0) {
			shieldblockrate *= 2;
			shieldblockrate /= 3;
		}
		if (StrongConflict && shieldblockrate > 0) {
			shieldblockrate *= 2;
			shieldblockrate /= 3;
		}

		if (shieldblockrate < 0) shieldblockrate = 0;

		sprintf(buf, "%d%%", shieldblockrate);
		dump("  Your chance to block was ", buf );
	}

	if (Fumbling) dump("  ", "You fumbled");
	if (Wounded_legs && !u.usteed ) {
		sprintf(buf, "wounded %s", makeplural(body_part(LEG)));
	      sprintf(eos(buf), " (%d)", HWounded_legs);
		dump(youhad, buf);
	}
	if (Wounded_legs && u.usteed) {
	    strcpy(buf, x_monnam(u.usteed, ARTICLE_YOUR, (char *)0, 
		    SUPPRESS_SADDLE | SUPPRESS_HALLUCINATION, FALSE));
	    *buf = highc(*buf);
	    strcat(buf, " had wounded legs");
	    dump("  ", buf);
	}
	if (Sleeping) dump("  ", "You fell asleep");
	if (Hunger && !StrongHunger) dump("  ", "You hungered rapidly");
	if (Hunger && StrongHunger) dump("  ", "You hungered extremely rapidly");

	if (have_sleepstone())  dump("  ", "You were very tired");
	if (have_cursedmagicresstone()) dump("  ", "You took double damage");
	if (uamul && uamul->otyp == AMULET_OF_VULNERABILITY) dump("  ", "You took extra damage");

	/*** Vision and senses ***/
	if (See_invisible && !StrongSee_invisible) dump("  ", "You saw invisible");
	if (See_invisible && StrongSee_invisible) dump("  ", "You saw invisible and hidden");
	if (Manaleech && !StrongManaleech) dump("  ", "You leeched mana");
	if (Manaleech && StrongManaleech) dump("  ", "You leeched a lot of mana");
	if (Peacevision) dump("  ", StrongPeacevision ? "You recognized peaceful creatures and other stuff" : "You recognized peaceful creatures");
	if (Map_amnesia) dump("  ", "You had map amnesia");
	if ((Blind_telepat || Unblind_telepat) && !StrongTelepat) dump(youwere, "telepathic");
	if ((Blind_telepat || Unblind_telepat) && StrongTelepat) dump(youwere, "very telepathic");

	if (Hallu_party) dump(youwere, "hallu partying");
	if (Drunken_boxing) dump(youwere, "a drunken boxer");
	if (Stunnopathy) dump(youwere, "stunnopathic");
	if (Numbopathy) dump(youwere, "numbopathic");
	if (Dimmopathy) dump(youwere, "dimmopathic");
	if (Freezopathy) dump(youwere, "freezopathic");
	if (Stoned_chiller) dump(youwere, "a stoned chiller");
	if (Corrosivity) dump(youwere, "extremely corrosive");
	if (Fear_factor) dump(youhad, "an increased fear factor");
	if (Burnopathy) dump(youwere, "burnopathic");
	if (Sickopathy) dump(youwere, "sickopathic");
	if (Wonderlegs) dump(youhad, "wonderlegs");
	if (Glib_combat) dump(youwere, "a glibbery fighter");

	if (Warning) dump(youwere, StrongWarning ? "definitely warned" : "warned");
	if (Warn_of_mon && flags.warntype) {
		sprintf(buf, "aware of the presence of %s",
			(flags.warntype & M2_ORC) ? "orcs" :
			(flags.warntype & M2_DEMON) ? "demons" :
			something); 
		dump(youwere, buf);
	}
	if (Undead_warning) dump(youwere, "warned of undead");

	if (Role_if(PM_ACTIVISTOR) || Race_if(PM_PEACEMAKER) ) dump(youwere, "aware of the presence of topmodels");
	if (Role_if(PM_ACTIVISTOR) && uwep && is_quest_artifact(uwep) ) dump(youwere, "aware of the presence of unique monsters");
	/*if (uamul && uamul->otyp == AMULET_OF_UNDEAD_WARNING ) dump(youwere, "aware of the presence of undead");
	if (uarmh && uarmh->otyp == HELMET_OF_UNDEAD_WARNING ) dump(youwere, "aware of the presence of undead");*/
	if (uamul && uamul->otyp == AMULET_OF_POISON_WARNING ) dump(youwere, "aware of the presence of poisonous monsters");
	if (uamul && uamul->otyp == AMULET_OF_OWN_RACE_WARNING ) dump(youwere, "aware of the presence of same-race monsters");
	if (uamul && uamul->otyp == AMULET_OF_COVETOUS_WARNING ) dump(youwere, "aware of the presence of covetous monsters");
	if (ublindf && ublindf->otyp == BOSS_VISOR ) dump(youwere, "aware of the presence of covetous monsters");
	if (Role_if(PM_PALADIN) ) dump(youwere, "aware of the presence of demons");
	if (Race_if(PM_VORTEX) ) dump(youwere, "aware of the presence of unsolid creatures");
	if (Race_if(PM_VORTEX) ) dump(youwere, "aware of the presence of creatures without limbs");
	if (Race_if(PM_CORTEX) ) dump(youwere, "aware of the presence of unsolid creatures");
	if (Race_if(PM_CORTEX) ) dump(youwere, "aware of the presence of creatures without limbs");
	if (Race_if(PM_LEVITATOR) ) dump(youwere, "aware of the presence of flying monsters");
	if (Race_if(PM_RODNEYAN) ) dump(youwere, "able to sense monsters possessing coveted objects");
	if (isselfhybrid) dump(youwere, "aware of the presence of strong wanderers");
	if (isselfhybrid) dump(youwere, "aware of the presence of monsters that are valid polymorph forms for monsters only");

	if (Searching) dump(youhad, StrongSearching ? "quick autosearching" : "automatic searching");
	if (Clairvoyant) {
		sprintf(buf, StrongClairvoyant ? "highly clairvoyant" : "clairvoyant");
	      sprintf(eos(buf), " (%d)", HClairvoyant);
		dump(youwere, buf);

	}
	if (Infravision) dump(youhad, StrongInfravision ? "strong infravision" : "infravision");

	if (u.banishmentbeam) dump(youwere, "going to be banished");
	if (u.levelporting) dump(youwere, "going to be levelported");

	if (u.inertia) {
		sprintf(buf, "slowed by inertia");
	      sprintf(eos(buf), " (%d)", u.inertia);
		dump(youwere, buf);
	}

	if (u.temprecursion) {
		sprintf(buf, "under the effect of temporary recursion");
	   	sprintf(eos(buf), " (%d)", u.temprecursiontime);
		dump(youwere, buf);
	}

	if (u.demagoguerecursion) {
		sprintf(buf, "temporarily playing as another role");
		sprintf(eos(buf), " (%d)", u.demagoguerecursiontime);
		dump(youwere, buf);
	}
	if (Detect_monsters) {
		sprintf(buf, StrongDetect_monsters ? "sensing all monsters" : "sensing the presence of monsters");
	      sprintf(eos(buf), " (%d)", HDetect_monsters);
		dump(youwere, buf);
	}

	if (u.umconf) dump(youwere, "going to confuse monsters");

	sprintf(buf, "%d points of nutrition remaining", u.uhunger);
	dump(youhad, buf);

	/*** Appearance and behavior ***/
	if (Adornment) {
	    int adorn = 0;
	    if(uleft && uleft->otyp == RIN_ADORNMENT) adorn += uleft->spe;
	    if(uright && uright->otyp == RIN_ADORNMENT) adorn += uright->spe;
	    if (adorn < 0)
		dump(youwere, "poorly adorned");
	    else
		dump(youwere, "adorned");
	}
	if (Invisible) dump(youwere, StrongInvis ? "very invisible" : "invisible");
	else if (Invis) dump(youwere, StrongInvis ? "very invisible to others" : "invisible to others");
	/* ordinarily "visible" is redundant; this is a special case for
	   the situation when invisibility would be an expected attribute */
	else if ((HInvis || EInvis || pm_invisible(youmonst.data)) && BInvis)
	    dump(youwere, "visible");
	if (Displaced) dump(youwere, StrongDisplaced ? "very displaced" : "displaced");
	if (Stealth) dump(youwere, StrongStealth ? "very stealthy" : "stealthy");
	if (StrongAggravate_monster) dump("  ", "You completely aggravated monsters");
	if (!StrongAggravate_monster && ExtAggravate_monster) dump("  ", "You strongly aggravated monsters");
	if (!StrongAggravate_monster && IntAggravate_monster) dump("  ", "You aggravated monsters");
	if (Conflict) dump("  ", StrongConflict ? "You caused lots of conflict" : "You caused conflict");

	/*** Transportation ***/
	if (Jumping) dump(youcould, StrongJumping ? "jump far" : "jump");
	if (Teleportation) dump(youhad, StrongTeleportation ? "chronic teleportitis" : "teleportitis");
	if (Teleport_control) dump(youhad, StrongTeleport_control ? "complete teleport control" : "teleport control");

	if (Lev_at_will)  {
	    sprintf(buf, "levitating, at will");
	    sprintf(eos(buf), " (%d)", HLevitation);
	    dump(youwere, buf);
	}
	if (Levitation)  {
	    sprintf(buf, StrongLevitation ? "floatitating" : "levitating");
	    sprintf(eos(buf), " (%d)", HLevitation);
	    dump(youwere, buf);
	}

	else if (Flying) dump(youcould, StrongFlying ? "fly up and down" : "fly");
	if (Wwalking) dump(youcould, "walk on water");
	if (Swimming) dump(youcould, StrongSwimming ? "swim like a world champion" : "swim");
	if (Breathless) dump(youcould, StrongMagical_breathing ? "survive everywhere without needing to breathe" : "survive without air");
	else if (Amphibious) dump(youcould, "breathe water");
	if (Passes_walls) dump(youcould, StrongPasses_walls ? "walk through every wall" : "walk through walls");
	if (u.usteed && (final < 2 || strcmp(killer, "riding accident"))) {
	    sprintf(buf, "riding %s", y_monnam(u.usteed));
	    dump(youwere, buf);
	}
	if (u.uswallow) {
	    sprintf(buf, "swallowed by %s", a_monnam(u.ustuck));
#ifdef WIZARD
	    if (wizard) sprintf(eos(buf), " (%u)", u.uswldtim);
#endif
	    dump(youwere, buf);
	} else if (u.ustuck) {
	    sprintf(buf, "%s %s",
		    (Upolyd && sticks(youmonst.data)) ? "holding" : "held by",
		    a_monnam(u.ustuck));
	    dump(youwere, buf);
	}

	/*** Physical attributes ***/
	if (u.uhitinc)
	    dump(youhad,
		enlght_combatinc("to hit", u.uhitinc, final, buf));
	if (u.udaminc)
	    dump(youhad,
		enlght_combatinc("damage", u.udaminc, final, buf));
	if (Slow_digestion) dump(youhad, StrongSlow_digestion ? "extremely slow digestion" : "slower digestion");
	if (Keen_memory) dump("  ", StrongKeen_memory ? "Your memory was very keen" : "Your memory was keen");

	if (Sight_bonus) dump("  ", StrongSight_bonus ? "Your sight was greatly improved" : "Your sight was improved");
	if (Versus_curses) dump(youhad, StrongVersus_curses ? "double curse resistance" : "curse resistance");

	if (Stun_resist) dump(youhad, StrongStun_resist ? "double stun resistance" : "stun resistance");
	if (Conf_resist) dump(youhad, StrongConf_resist ? "double confusion resistance" : "confusion resistance");
	if (Cont_resist) dump(youhad, StrongCont_resist ? "double contamination resistance" : "contamination resistance");
	if (Discount_action) dump(youhad, StrongDiscount_action ? "high discount action" : "discount action");
	if (Full_nutrient) dump(youhad, StrongFull_nutrient ? "very full nutrients" : "full nutrients");
	if (Technicality) dump(youhad, StrongTechnicality ? "greatly improved technique levels" : "improved technique levels");
	if (ScentView || EcholocationActive) dump(youhad, StrongScentView ? "scent view and echolocation" : (ScentView && EcholocationActive) ? "scent view and echolocation" : EcholocationActive ? "echolocation" : "scent view");
	if (DiminishedBleeding) dump(youhad, StrongDiminishedBleeding ? "greatly diminished bleeding" : "diminished bleeding");
	if (Psi_resist) dump(youhad, StrongPsi_resist ? "double psi resistance" : "psi resistance");
	if (Extra_wpn_practice) dump("  ", StrongExtra_wpn_practice ? "You could train skills and attributes much faster" : "You could train skills and attributes faster");
	if (Death_resistance) dump(youhad, "resistance to death rays");
	if (Double_attack) dump(youhad, StrongDouble_attack ? "super double attacks" : "double attacks");
	if (Quad_attack) dump(youhad, StrongQuad_attack ? "super quad attacks" : "quad attacks");

	if (Half_physical_damage) dump(youhad, StrongHalf_physical_damage ? "strong physical resistance" : "physical resistance");
	if (Half_spell_damage) dump(youhad, StrongHalf_spell_damage ? "strong spell resistance" : "spell resistance");

	if (Regeneration) dump("  ", StrongRegeneration ? "You quickly regenerated" : "You regenerated");
	if (Energy_regeneration) dump("  ", StrongEnergy_regeneration ? "You quickly regenerated mana" : "You regenerated mana");
	if (u.uspellprot || Protection) {
	    int prot = 0;

	    if(uleft && uleft->otyp == RIN_PROTECTION) prot += uleft->spe;
	    if(uright && uright->otyp == RIN_PROTECTION) prot += uright->spe;
	    if (HProtection & INTRINSIC) prot += u.ublessed;
	    prot += u.uspellprot;
	    
	    if (prot < 0)
		dump(youwere, "ineffectively protected");
	    else
		dump(youwere, "protected");
	}
	if (Protection_from_shape_changers)
		dump(youwere, "protected from shape changers");
	if (Polymorph) dump(youhad, StrongPolymorph ? "chronic polymorphitis" : "polymorphitis");
	if (Polymorph_control) dump(youhad, StrongPolymorph_control ? "complete polymorph control" : "polymorph control");
	if (u.ulycn >= LOW_PM) {
		strcpy(buf, an(mons[u.ulycn].mname));
		dump(youwere, buf);
	}
	if (Upolyd && !missingnoprotect) {
	    if (u.umonnum == u.ulycn) strcpy(buf, "in beast form");
	    else sprintf(buf, "polymorphed into %s",
			 an(youmonst.data->mname));
#ifdef WIZARD
	    if (wizard) sprintf(eos(buf), " (%d)", u.mtimedone);
#endif
	    dump(youwere, buf);
	}
	if (Unchanging)
	  dump(youcould, "not change from your current form");
	if (Fast && !SpeedBug && !u.uprops[SPEED_BUG].extrinsic && !(uarmf && uarmf->oartifact == ART_UNEVEN_ENGINE) && !(uarmf && uarmf->oartifact == ART_ERROR_IN_PLAY_ENCHANTMENT) && !have_speedbugstone()) dump(youwere, StrongFast ? "ultra fast" : Very_fast ? "very fast" : "fast");
	if (Fast && (SpeedBug || u.uprops[SPEED_BUG].extrinsic || (uarmf && uarmf->oartifact == ART_UNEVEN_ENGINE) || (uarmf && uarmf->oartifact == ART_ERROR_IN_PLAY_ENCHANTMENT) || have_speedbugstone()) ) dump(youwere, StrongFast ? "ultra slow" : Very_fast ? "very slow" : "slow");
	if (!Fast && Very_fast && !SpeedBug && !u.uprops[SPEED_BUG].extrinsic && !(uarmf && uarmf->oartifact == ART_UNEVEN_ENGINE) && !(uarmf && uarmf->oartifact == ART_ERROR_IN_PLAY_ENCHANTMENT) && !have_speedbugstone()) dump(youwere, "very fast");
	if (!Fast && Very_fast && (SpeedBug || u.uprops[SPEED_BUG].extrinsic || (uarmf && uarmf->oartifact == ART_UNEVEN_ENGINE) || (uarmf && uarmf->oartifact == ART_ERROR_IN_PLAY_ENCHANTMENT) || have_speedbugstone()) ) dump(youwere, "very slow");
	if (Reflecting) dump(youhad, StrongReflecting ? "powerful reflection" : "reflection");
	if (Free_action) dump(youhad, StrongFree_action ? "very free action" : "free action");
	if (SustainAbilityOn) dump(youhad, "fixed abilities");
	if (uamul && uamul->otyp == AMULET_VERSUS_STONE)
		dump("  ", "You would have been depetrified");
	if (Lifesaved)
		dump("  ", "Your life would have been saved");
	if (Second_chance)
		dump("  ", "You would have been given a second chance");
	if (u.metalguard)	
  	  	dump("  ", "The next damage you took would have been nullified");
	if (u.twoweap) dump(youwere, "wielding two weapons at once");

	/*** Miscellany ***/
	if (Luck) {
	    ltmp = abs((int)Luck);
	    sprintf(buf, "%s%slucky (%d)",
		    ltmp >= 10 ? "extremely " : ltmp >= 5 ? "very " : "",
		    Luck < 0 ? "un" : "", Luck);
	    dump(youwere, buf);
	}
	 else dump("  ", "Your luck was zero");
	if (u.moreluck > 0) dump(youhad, "extra luck");
	else if (u.moreluck < 0) dump(youhad, "reduced luck");
	if (carrying(LUCKSTONE) || stone_luck(TRUE)) {
	    ltmp = stone_luck(FALSE);
	    if (ltmp <= 0)
		dump("  ", "Bad luck did not time out for you");
	    if (ltmp >= 0)
		dump("  ", "Good luck did not time out for you");
	}

	if (u.uhealbonus)
	{
		sprintf(buf, "%s health", u.uhealbonus > 0 ? "extra" :
			"reduced");
	      sprintf(eos(buf), " (%d)", u.uhealbonus);
	      dump(youhad, buf);
	}
	else dump("  ", "Your health bonus was zero");

	if (recalc_mana() )
	{
		sprintf(buf, "%s mana", recalc_mana() > 0 ? "extra" :
			"reduced");
	      sprintf(eos(buf), " (%d)", recalc_mana() );
	      dump(youhad, buf);
	}
	else dump("  ", "Your mana bonus was zero");

	sprintf(buf, "spell retention for remembering %d spells", urole.spelrete);
	dump(youhad, buf);

	    sprintf(buf, "Your pantheon consisted of %s, %s and %s", align_gname(A_LAWFUL), align_gname(A_NEUTRAL), align_gname(A_CHAOTIC) );
	    dump("  ", buf);

	    sprintf(buf, "Your deity was %s", u_gname());
	    dump("  ", buf);

	    getversionstring(buf);
	    dump("  The version you were playing was: ", buf);

	if (u.ugangr) {
	    sprintf(buf, " %sangry with you",
		u.ugangr > 6 ? "extremely " : u.ugangr > 3 ? "very " : "");
	    sprintf(eos(buf), " (%d)", u.ugangr);
	    sprintf(buf2, "%s was %s", u_gname(), buf);
	    dump("  ", buf2);
	}

	    sprintf(buf, "You could %s have safely prayed", u.ugangr ? "not " : can_pray(FALSE) ? "" : "not ");
	    sprintf(eos(buf), " (%d)", u.ublesscnt);
	    dump("  ", buf);

    {
	const char *p;

	buf[0] = '\0';
	if (final < 2) {    /* quit/escaped/ascended */
	    p = "survived after being killed ";
	    switch (u.umortality) {
	    case 0:  p = "survived";  break;
	    case 1:  strcpy(buf, "once");  break;
	    case 2:  strcpy(buf, "twice");  break;
	    case 3:  strcpy(buf, "thrice");  break;
	    default: sprintf(buf, "%d times", u.umortality);
		     break;
	    }
	} else {		/* game ended in character's death */
	    p = "are dead";
	    switch (u.umortality) {
	    case 0:  impossible("dead without dying?");
	    case 1:  break;			/* just "are dead" */
	    default: sprintf(buf, " (%d%s time!)", u.umortality,
			     ordin(u.umortality));
		     break;
	    }
	}
	if (p) {
	  sprintf(buf2, "You %s %s", p, buf);
	  dump("  ", buf2);
	}
    }
	dump("", "");
	return;

} /* dump_enlightenment */
#endif

/*
 * Courtesy function for non-debug, non-explorer mode players
 * to help refresh them about who/what they are.
 * Returns FALSE if menu cancelled (dismissed with ESC), TRUE otherwise.
 */
STATIC_OVL boolean
minimal_enlightenment()
{

	char statline[BUFSZ];

	*statline = '\0';

	if (TheInfoIsFucked) {
		pline("Somehow the information is fucked up and won't display.");
		return 0;
	}

	if (!DisplayDoesNotGo) {
		/* Yes I know, this is far from optimized. But it's a crutch for terminals with
		 * less than 25 lines, where bot2() doesn't display everything if you have lots of status effects. --Amy */

		sprintf(eos(statline), "You are %s, a %s %s %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s %s.", playeraliasname, align_str(u.ualign.type), (flags.female ? "female" : "male"), (flags.hybridangbander ? "angbander " : ""), (flags.hybridaquarian ? "aquarian " : ""), (flags.hybridcurser ? "curser " : ""), (flags.hybridhaxor ? "haxor " : ""), (flags.hybridhomicider ? "homicider " : ""), (flags.hybridsuxxor ? "suxxor " : ""), (flags.hybridwarper ? "warper " : ""), (flags.hybridrandomizer ? "randomizer " : ""), (flags.hybridnullrace ? "null " : ""), (flags.hybridmazewalker ? "mazewalker " : ""), (flags.hybridsoviet ? "soviet " : ""), (flags.hybridxrace ? "x-race " : ""), (flags.hybridheretic ? "heretic " : ""), (flags.hybridsokosolver ? "sokosolver " : ""), (flags.hybridspecialist ? "specialist " : ""), (flags.hybridamerican ? "american " : ""), (flags.hybridminimalist ? "minimalist " : ""), (flags.hybridnastinator ? "nastinator " : ""), (flags.hybridrougelike ? "rougelike " : ""), (flags.hybridsegfaulter ? "segfaulter " : ""), (flags.hybridironman ? "ironman " : ""), (flags.hybridamnesiac ? "amnesiac " : ""), (flags.hybridproblematic ? "problematic " : ""), (flags.hybridwindinhabitant ? "windinhabitant " : ""), (flags.hybridaggravator ? "aggravator " : ""), (flags.hybridevilvariant ? "evilvariant " : ""), (flags.hybridlevelscaler ? "levelscaler " : ""), (flags.hybriderosator ? "erosator " : ""), (flags.hybridroommate ? "roommate " : ""), (flags.hybridextravator ? "extravator " : ""), urace.adj, (flags.female && urole.name.f) ? urole.name.f : urole.name.m);

		if (!Upolyd) sprintf(eos(statline), " HP: %d (max %d)", u.uhp, u.uhpmax);
		else sprintf(eos(statline), " HP: %d (max %d)", u.mh, u.mhmax);

		sprintf(eos(statline), " Pw %d (max %d)", u.uen, u.uenmax);

		sprintf(eos(statline), " AC %d", u.uac);

		sprintf(eos(statline), " Current status effects: ");

		if (Levitation) sprintf(eos(statline), "levitation, ");
		if (HeavyConfusion) sprintf(eos(statline), "xtraconfusion, ");
		else if (Confusion) sprintf(eos(statline), "confusion, ");
		if (Sick) {
			if (u.usick_type & SICK_VOMITABLE) sprintf(eos(statline), "food poisoning, ");
			if (u.usick_type & SICK_NONVOMITABLE) sprintf(eos(statline), "illness, ");
		}
		if (HeavyBlind) sprintf(eos(statline), "xtrablindness, ");
		else if (Blind) sprintf(eos(statline), "blindness, ");
		if(sengr_at("Elbereth", u.ux, u.uy)) sprintf(eos(statline), "elbereth, ");
		if (HeavyFeared) sprintf(eos(statline), "xtrafear, ");
		else if (Feared) sprintf(eos(statline), "fear, ");
		if (HeavyNumbed) sprintf(eos(statline), "xtranumbness, ");
		else if (Numbed) sprintf(eos(statline), "numbness, ");
		if (HeavyFrozen) sprintf(eos(statline), "xtrafreeze, ");
		else if (Frozen) sprintf(eos(statline), "freeze, ");
		if (HeavyBurned) sprintf(eos(statline), "xtraburn, ");
		else if (Burned) sprintf(eos(statline), "burn, ");
		if (HeavyDimmed) sprintf(eos(statline), "xtradim, ");
		else if (Dimmed) sprintf(eos(statline), "dim, ");
		if (HeavyStunned) sprintf(eos(statline), "xtrastun, ");
		else if (Stunned) sprintf(eos(statline), "stun, ");
		if (HeavyHallu) sprintf(eos(statline), "xtrahallucination, ");
		else if (Hallucination) sprintf(eos(statline), "hallucination, ");
		if (Slimed) sprintf(eos(statline), "sliming, ");
		if (Stoned) sprintf(eos(statline), "petrification, ");
		if (IsGlib) sprintf(eos(statline), "glib, ");
		if (Wounded_legs) sprintf(eos(statline), "wounded legs, ");
		if (Strangled) sprintf(eos(statline), "strangled, ");
		if (PlayerBleeds) sprintf(eos(statline), "bleeding, ");
		if (Vomiting) sprintf(eos(statline), "vomiting, ");
		if(u.ustuck && !u.uswallow && !sticks(youmonst.data)) sprintf(eos(statline), "held by a monster, ");
		if(near_capacity() > UNENCUMBERED) sprintf(eos(statline), "%s, ", encx_stat[near_capacity()]);
		if (!YouAreThirsty && u.urealedibility && u.uhunger >= 4500) sprintf(eos(statline), "oversatiated, ");
		else if(!YouAreThirsty && strcmp(hux_stat[u.uhs], "        ")) sprintf(eos(statline), "%s, ", hux_stat[u.uhs]);

		sprintf(eos(statline), ".");

		pline("%s", statline);

	/*"You are %s, a %s %s %s%s%s%s%s%s%s%s %s. Current status effects: %s%s%s%s.", , Levitation ? "levitation " : "", Confusion ? (HeavyConfusion ? "xtraconfusion " : "confusion ") : "", );
*/
	}

	winid tmpwin;
	menu_item *selected;
	anything any;
	int genidx, n;
	char buf[BUFSZ], buf2[BUFSZ];
	static const char untabbed_fmtstr[] = "%-15s: %-12s";
	static const char untabbed_deity_fmtstr[] = "%-17s%s";
	static const char tabbed_fmtstr[] = "%s:\t%-12s";
	static const char tabbed_deity_fmtstr[] = "%s\t%s";
	static const char *fmtstr;
	static const char *deity_fmtstr;

	fmtstr = iflags.menu_tab_sep ? tabbed_fmtstr : untabbed_fmtstr;
	deity_fmtstr = iflags.menu_tab_sep ?
			tabbed_deity_fmtstr : untabbed_deity_fmtstr; 
	any.a_void = 0;
	buf[0] = buf2[0] = '\0';
	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings, "Starting", FALSE);

	/* Starting name, race, role, gender */
	sprintf(buf, fmtstr, "name", playeraliasname);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	sprintf(buf, fmtstr, "race", ustartrace.noun);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	sprintf(buf, fmtstr, "role",
		(flags.initgend && ustartrole.name.f) ? ustartrole.name.f : ustartrole.name.m);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	sprintf(buf, fmtstr, "gender", genders[flags.initgend].adj);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	/* Starting alignment */
	sprintf(buf, fmtstr, "alignment", align_str(u.ualignbase[A_ORIGINAL]));
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	/* Hybridization (if any) */
	if (flags.hybridization) {

	sprintf(buf, fmtstr, "hybrid races", hybrid_str());
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	}

	/* Current name, race, role, gender */
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, "", FALSE);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings, "Current", FALSE);
	sprintf(buf, fmtstr, "race", (Upolyd && !missingnoprotect) ? youmonst.data->mname : urace.noun);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	if (Upolyd) {
	    sprintf(buf, fmtstr, "role (base)",
		(u.mfemale && urole.name.f) ? urole.name.f : urole.name.m);
	    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	} else {
	    sprintf(buf, fmtstr, "role",
		(flags.female && urole.name.f) ? urole.name.f : urole.name.m);
	    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	}
	/* don't want poly_gender() here; it forces `2' for non-humanoids */
	genidx = is_neuter(youmonst.data) ? 2 : flags.female;
	sprintf(buf, fmtstr, "gender", genders[genidx].adj);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	if (Upolyd && (int)u.mfemale != genidx) {
	    sprintf(buf, fmtstr, "gender (base)", genders[u.mfemale].adj);
	    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	}

	/* Current alignment */
	sprintf(buf, fmtstr, "alignment", align_str(u.ualign.type));
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	/* Deity list */
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, "", FALSE);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings, "Deities", FALSE);
	sprintf(buf2, deity_fmtstr, align_gname(A_CHAOTIC),
	    (u.ualignbase[A_ORIGINAL] == u.ualign.type
		&& u.ualign.type == A_CHAOTIC) ? " (s,c)" :
	    (u.ualignbase[A_ORIGINAL] == A_CHAOTIC)       ? " (s)" :
	    (u.ualign.type   == A_CHAOTIC)       ? " (c)" : "");
	sprintf(buf, fmtstr, "Chaotic", buf2);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	sprintf(buf2, deity_fmtstr, align_gname(A_NEUTRAL),
	    (u.ualignbase[A_ORIGINAL] == u.ualign.type
		&& u.ualign.type == A_NEUTRAL) ? " (s,c)" :
	    (u.ualignbase[A_ORIGINAL] == A_NEUTRAL)       ? " (s)" :
	    (u.ualign.type   == A_NEUTRAL)       ? " (c)" : "");
	sprintf(buf, fmtstr, "Neutral", buf2);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	sprintf(buf2, deity_fmtstr, align_gname(A_LAWFUL),
	    (u.ualignbase[A_ORIGINAL] == u.ualign.type &&
		u.ualign.type == A_LAWFUL)  ? " (s,c)" :
	    (u.ualignbase[A_ORIGINAL] == A_LAWFUL)        ? " (s)" :
	    (u.ualign.type   == A_LAWFUL)        ? " (c)" : "");
	sprintf(buf, fmtstr, "Lawful", buf2);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	end_menu(tmpwin, "Base Attributes");
	n = select_menu(tmpwin, PICK_NONE, &selected);
	destroy_nhwindow(tmpwin);
	return (n != -1);
}

STATIC_PTR int
doattributes()
{
	if (!minimal_enlightenment())
		return 0;
	if (wizard)
		enlightenment(0, 1);
	else if (discover && (yn("Show the attributes screen?") == 'y') )
		enlightenment(0, 1);
	return 0;
}

static const struct menu_tab game_menu[] = {
	{'O', TRUE, doset, "Options"},
	{'r', TRUE, doredraw, "Redraw Screen"},
#ifndef PUBLIC_SERVER
	{'x', TRUE, enter_explore_mode, "Enter Explore Mode"},
	{'!', TRUE, dosh, "Jump to Shell"},
#endif
	{'S', TRUE, dosave, "Save"},
	{'q', TRUE, done2, "Quit [M-q]"},
	{0,0,0,0},
};

static const struct menu_tab inv_menu[] = {
	{(char)0, TRUE, (void *)0, "View Inventory"},
	{'i', TRUE, ddoinv, "Inventory List"},
	{'I', TRUE, dotypeinv, "Inventory List by Type"},
	{'*', TRUE, doprinuse, "Items in use"},
	{(char)0, TRUE, (void *)0, "Ready Items"},
	{'w', FALSE, dowield, "Wield Weapon"},
	{'W', FALSE, dowear, "Wear Protective Gear"},
	{'Q', FALSE, dowieldquiver, "Prepare missile weapon (in Quiver)"},
	{'T', FALSE, dotakeoff, "Take off Protective Gear"},
	{(char)0, TRUE, (void *)0, "Manipulate Items"},
	{'a', FALSE, doapply, "Apply an object"},
	{'d', FALSE, dodip, "Dip an object [M-d]"},
	{'E', FALSE, doengrave, "Engrave into the ground"},
	{'f', FALSE, dofire, "Fire your prepared missile weapon"},
	{'i', TRUE, doinvoke, "Invoke your weapon"},
	{'t', FALSE, dothrow, "Throw an item"},
	{(char)0, TRUE, (void *)0, "Drop Items"},
	{'d', FALSE, dodrop, "Drop an object"},
	{'D', FALSE, doddrop, "Multi-Drop"},
	{0,0,0,0}
};

static const struct menu_tab action_menu[] = {
	{'c', FALSE, doclose, "Close a door"},
	{'e', FALSE, doeat, "Eat some food"},
	{'f', FALSE, doforce, "Force a lock [M-f]"},
	{'l', FALSE, doloot, "Loot an object"},
	{'o', FALSE, doopen, "Open a door"},
	{'q', TRUE, dodrink, "Quaff a potion"},
	{'r', FALSE, doread, "Read an object"},
	{'u', FALSE, dountrap, "Untrap"}, 
	{'z', FALSE, dozap, "Zap a wand"},
	{'Z', TRUE, docast, "Cast a spell"},
	{0,0,0,0}
};

static const struct menu_tab player_menu[] = {
	{'b', FALSE, playersteal, "Steal from Monsters [M-b]"},
	{'c', TRUE, dotalk, "Chat with Monsters [M-c]"},
	{'d', FALSE, dokick, "Do Kick"}, 
	/*        {'e', FALSE, specialpower, "Use your Class Ability [M-e]"},*/
	{'e', TRUE, enhance_weapon_skill, "Weapon Skills [M-k]"},
	{'m', TRUE, domonability, "Use your Monster Ability [M-m]"},
	{'o', FALSE, dosacrifice, "Offer a Sacrifice [M-o]"},
	{'p', FALSE, dopay, "Pay the Shopkeeper"},
	{'s', FALSE, dosit, "Sit down [M-s]"},
	{'t', TRUE, dotele, "Controlled Teleport [C-t]"},
/*	{'T', TRUE, doturn, "Turn Undead [M-t]"},*/
	{'T', TRUE, dotech, "Use Techniques [M-t]"},
	{'U', TRUE, doremoveimarkers, "Remove 'I' markers [M-u]"},
	{'x', TRUE, doattributes, "Show attributes"},
	{'y', TRUE, polyatwill, "Self-Polymorph [M-y]"},
	{0,0,0,0}
};

#ifdef WIZARD
static const struct menu_tab wizard_menu[] = {
	{'c', TRUE, wiz_gain_ac, "Increase AC"},
#ifdef DISPLAY_LAYERS
	{'d', TRUE, wiz_show_display, "Detail display layers"},
#endif
	{'e', TRUE, wiz_detect, "Detect secret doors and traps"},
	{'f', TRUE, wiz_map, "Do magic mapping"},
	{'g', TRUE, wiz_genesis, "Create monster"},
	{'i', TRUE, wiz_identify, "Identify items in pack"},
	{'j', TRUE, wiz_gain_level, "Go up an experience level"},
	{'n', TRUE, wiz_toggle_invulnerability, "Toggle invulnerability"},
	{'o', TRUE, wiz_where, "Tell locations of special levels"},
	{'v', TRUE, wiz_level_tele, "Do trans-level teleport"},
	{'w', TRUE, wiz_wish,  "Make wish"},
	{'H', TRUE, wiz_detect_monsters, "Detect monsters"},
	{'L', TRUE, wiz_light_sources, "show mobile light sources"},
	{'M', TRUE, wiz_show_stats, "show memory statistics"},
	{'S', TRUE, wiz_show_seenv, "show seen vectors"},
	{'T', TRUE, wiz_timeout_queue, "look at timeout queue"},
	{'V', TRUE, wiz_show_vision, "show vision array"},
	{'W', TRUE, wiz_show_wmodes, "show wall modes"},
#ifdef DEBUG
	{'&', TRUE, wiz_debug_cmd, "wizard debug command"},
#endif
	{0,0,0,0,0},
};
#endif

static const struct menu_tab help_menu[] = {
	{'?', TRUE, dohelp, "Help Contents"},
	{'v', TRUE, doextversion, "Version"},
	{'/', TRUE, dowhatis, "Identify an object on the screen" },
	{'&', TRUE, dowhatdoes, "Determine what a key does"},
	{0,0,0,0,0},
};

static const struct menu_tab main_menu[] = {
	{'g', TRUE, (void *)0, "Game"},
	{'i', TRUE, (void *)0, "Inventory"},
	{'a', TRUE, (void *)0, "Action"},
	{'p', TRUE, (void *)0, "Player"},
	{'d', TRUE, (void *)0, "Discoveries"},
#ifdef WIZARD
	{'w', TRUE, (void *)0, "Wizard"},
#endif
	{'?', TRUE, (void *)0, "Help"},
	{0,0,0,0},
};

static const struct menu_tab discover_menu[] = {
	{'X', TRUE, dovspell, "View known spells"},                  /* Mike Stephenson */
	{'d', TRUE, dodiscovered, "Items already discovered [\\]"},             /* Robert Viduya */
	{'C', TRUE, do_mname, "Name a monster"},
	{0,0,0,0},
};

static struct menu_list main_menustruct[] = {
	{"Game", "Main Menu", game_menu},
	{"Inventory", "Main Menu", inv_menu},
	{"Action", "Main Menu", action_menu},
	{"Player", "Main Menu", player_menu},
	{"Discoveries", "Main Menu", discover_menu},
#ifdef WIZARD
	{"Wizard", "Main Menu", wizard_menu},
#endif
	{"Help", "Main Menu", help_menu},
	{"Main Menu",(char *)0, main_menu},
	{0,0,0},
};

STATIC_PTR int
makemenu(menuname, menu_struct)
const char *menuname;
struct menu_list menu_struct[];
{
	winid win;
	anything any;
	menu_item *selected;
        int n, i, (*func)(void);
        const struct menu_tab *current_menu;

	any.a_void = 0;
	win = create_nhwindow(NHW_MENU);
	start_menu(win);

        for (i = 0; menu_struct[i].m_header; i++) {
                if (strcmp(menu_struct[i].m_header,menuname)) continue;
                current_menu = menu_struct[i].m_menu;
                for (n = 0; current_menu[n].m_item; n++) {
                        if (u.uburied && !current_menu[n].can_if_buried) continue;
#ifdef WIZARD
			if (!wizard && !current_menu[n].m_funct && !strcmp(current_menu[n].m_item,"Wizard")) continue;
#endif
                        if (current_menu[n].m_char == (char)0) {
                                any.a_int = 0; 
                                add_menu(win, NO_GLYPH, &any, 0, 0, ATR_BOLD,
                                     current_menu[n].m_item, MENU_UNSELECTED);
                                continue;
                        }
                        any.a_int = n + 1; /* non-zero */
                        add_menu(win, NO_GLYPH, &any, current_menu[n].m_char,
                        0, ATR_NONE, current_menu[n].m_item, MENU_UNSELECTED);
                }
                break;
        }
        end_menu(win, menuname);
        n = select_menu(win, PICK_ONE, &selected);
        destroy_nhwindow(win);
        if (n > 0) {
                /* we discard 'const' because some compilers seem to have
		       trouble with the pointer passed to set_occupation() */
                i = selected[0].item.a_int - 1;
                func = current_menu[i].m_funct;
                if (current_menu[i].m_text && !occupation && multi)
                      set_occupation(func, current_menu[i].m_text, multi);
                /*WAC catch void into makemenu */
                if (func == (void *)0)
                        return (makemenu(current_menu[i].m_item, menu_struct));
                else return (*func)();            /* perform the command */
        } else if (n < 0) {
                for (i = 0; menu_struct[i].m_header; i++){
                   if (menuname == menu_struct[i].m_header) {
                    if (menu_struct[i].m_parent)
                      return (makemenu(menu_struct[i].m_parent, menu_struct));
                    else return (0);
                }
        }
        }
        return 0;
}

STATIC_PTR int
domenusystem()  /* WAC add helpful menus ;B */
{
        return (makemenu("Main Menu", main_menustruct));
}

/* KMH, #conduct
 * (shares enlightenment's tense handling)
 */
STATIC_PTR int
doconduct()
{
	show_conduct(0);
	return 0;
}

/* format increased damage or chance to hit */
static char *
enlght_combatinc(inctyp, incamt, final, outbuf)
const char *inctyp;
int incamt, final;
char *outbuf;
{
	char numbuf[24];
	const char *modif, *bonus;

	if (final
#ifdef WIZARD
		|| wizard
#endif
	  ) {
	    sprintf(numbuf, "%s%d",
		    (incamt > 0) ? "+" : "", incamt);
	    modif = (const char *) numbuf;
	} else {
	    int absamt = abs(incamt);

	    if (absamt <= 3) modif = "small";
	    else if (absamt <= 6) modif = "moderate";
	    else if (absamt <= 12) modif = "large";
	    else modif = "huge";
	}
	bonus = (incamt > 0) ? "bonus" : "penalty";
	/* "bonus to hit" vs "damage bonus" */
	if (!strcmp(inctyp, "damage")) {
	    const char *ctmp = inctyp;
	    inctyp = bonus;
	    bonus = ctmp;
	}
	sprintf(outbuf, "%s %s %s", an(modif), bonus, inctyp);
	return outbuf;
}

void
show_conduct(final)
int final;
{
	char buf[BUFSZ];
	int ngenocided;

	/* Create the conduct window */
	en_win = create_nhwindow(NHW_MENU);
	putstr(en_win, 0, "Voluntary challenges:");
	putstr(en_win, 0, "");

	if (!u.uconduct.food && !u.uconduct.unvegan)
	    enl_msg(You_, "have gone", "went", " without food");
	    /* But beverages are okay */
	else if (!u.uconduct.food)
	    enl_msg(You_, "have gone", "went", " without eating");
	    /* But quaffing animal products (eg., blood) is okay */
	else if (!u.uconduct.unvegan) {
	    you_have_X("followed a strict vegan diet");
			sprintf(buf, "eaten %ld time%s",
		    u.uconduct.food, plur(u.uconduct.food));
	    you_have_X(buf);
		}
	else if (!u.uconduct.unvegetarian) {
	    you_have_been("vegetarian");
			sprintf(buf, "eaten %ld time%s",
		    u.uconduct.food, plur(u.uconduct.food));
	    you_have_X(buf);
			sprintf(buf, "eaten food with animal by-products %ld time%s",
		    u.uconduct.unvegan, plur(u.uconduct.unvegan));
	    you_have_X(buf);

		}
	else {
			sprintf(buf, "eaten %ld time%s",
		    u.uconduct.food, plur(u.uconduct.food));
	    you_have_X(buf);
			sprintf(buf, "eaten food with animal by-products %ld time%s",
		    u.uconduct.unvegan, plur(u.uconduct.unvegan));
	    you_have_X(buf);
			sprintf(buf, "eaten meat %ld time%s",
		    u.uconduct.unvegetarian, plur(u.uconduct.unvegetarian));
	    you_have_X(buf);

	}

	if (!u.uconduct.gnostic)
	    you_have_been("an atheist");
	else {	    sprintf(buf, "communicated with the gods %ld time%s",
		    u.uconduct.gnostic, plur(u.uconduct.gnostic));
	    you_have_X(buf);
	}

	if (!u.uconduct.praydone)
	    you_have_never("prayed to the gods");
	else {	    sprintf(buf, "prayed %ld time%s",
		    u.uconduct.praydone, plur(u.uconduct.praydone));
	    you_have_X(buf);
	}

	if (!u.uconduct.weaphit)
	    you_have_never("hit with a wielded weapon");
	else {
	    sprintf(buf, "used a wielded weapon %ld time%s",
		    u.uconduct.weaphit, plur(u.uconduct.weaphit));
	    you_have_X(buf);
	}
	if (!u.uconduct.killer)
	    you_have_been("a pacifist");
	else {
		sprintf(buf, "killed %ld monster%s",
		    u.uconduct.killer, plur(u.uconduct.killer));
	    you_have_X(buf);

	}

	if (!u.uconduct.literate)
	    you_have_been("illiterate");
	else {
	    sprintf(buf, "read items or engraved %ld time%s",
		    u.uconduct.literate, plur(u.uconduct.literate));
	    you_have_X(buf);
	}

	ngenocided = num_genocides();
	if (ngenocided == 0) {
	    you_have_never("genocided any monsters");
	} else {
	    sprintf(buf, "genocided %d type%s of monster%s",
		    ngenocided, plur(ngenocided), plur(ngenocided));
	    you_have_X(buf);
	}

	if (!u.uconduct.polypiles)
	    you_have_never("polymorphed an object");
	else {
	    sprintf(buf, "polymorphed %ld item%s",
		    u.uconduct.polypiles, plur(u.uconduct.polypiles));
	    you_have_X(buf);
	}

	if (!u.uconduct.polyselfs)
	    you_have_never("changed form");
	else {
	    sprintf(buf, "changed form %ld time%s",
		    u.uconduct.polyselfs, plur(u.uconduct.polyselfs));
	    you_have_X(buf);
	}

	if (!u.uconduct.wishes)
	    you_have_X("used no wishes");
	else {
	    sprintf(buf, "used %ld wish%s",
		    u.uconduct.wishes, (u.uconduct.wishes > 1L) ? "es" : "");
	    you_have_X(buf);

	    if (!u.uconduct.wisharti)
		enl_msg(You_, "have not wished", "did not wish",
			" for any artifacts");
	}


	if (!u.uconduct.celibacy)
	    you_have_X("remained celibate");
	else {
	    sprintf(buf, "your vow of celibacy %ld time%s",
		    u.uconduct.celibacy, plur(u.uconduct.celibacy));
	    enl_msg(You_, "have broken ", "broke ", buf);
	}

	/* extra statistics by Amy. Most of them can be accessed by using the #conduct command, but some would leak info
	 * that the player couldn't otherwise have during a running game and are therefore only displayed when the game is
	 * over. In fact, you could probably leak info with all of them when having nasty traps active, but oh well... */

	if (u.cnd_applycount) {
		sprintf(buf, "%ld time%s", u.cnd_applycount, plur(u.cnd_applycount));
		enl_msg(You_, "have applied items ", "applied items ", buf);
	}
	if (u.cnd_unihorncount) {
		sprintf(buf, "%ld time%s", u.cnd_unihorncount, plur(u.cnd_unihorncount));
		enl_msg(You_, "experienced the healing effects of unicorn horns ", "experienced the healing effects of unicorn horns ", buf);
	}
	if (u.cnd_markercount) {
		sprintf(buf, "%ld item%s with the magic marker", u.cnd_markercount, plur(u.cnd_markercount));
		enl_msg(You_, "have created ", "created ", buf);
	}
	if (u.cnd_chemistrycount) {
		sprintf(buf, "%ld item%s with the chemistry set", u.cnd_chemistrycount, plur(u.cnd_chemistrycount));
		enl_msg(You_, "have created ", "created ", buf);
	}
	if (u.cnd_quaffcount) {
		sprintf(buf, "%ld potion%s", u.cnd_quaffcount, plur(u.cnd_quaffcount));
		enl_msg(You_, "have quaffed ", "quaffed ", buf);
	}
	if (u.cnd_zapcount) {
		sprintf(buf, "%ld time%s", u.cnd_zapcount, plur(u.cnd_zapcount));
		enl_msg(You_, "have zapped wands ", "zapped wands ", buf);
	}
	if (u.cnd_scrollcount) {
		sprintf(buf, "%ld time%s", u.cnd_scrollcount, plur(u.cnd_scrollcount));
		enl_msg(You_, "have read scrolls ", "read scrolls ", buf);
	}
	if (u.cnd_spellbookcount) {
		sprintf(buf, "%ld time%s", u.cnd_spellbookcount, plur(u.cnd_spellbookcount));
		enl_msg(You_, "have read spellbooks ", "read spellbooks ", buf);
	}
	if (u.cnd_spellcastcount) {
		sprintf(buf, "%ld time%s", u.cnd_spellcastcount, plur(u.cnd_spellcastcount));
		enl_msg(You_, "have successfully cast spells ", "successfully cast spells ", buf);
	}
	if (u.cnd_spellfailcount) {
		sprintf(buf, "%ld time%s", u.cnd_spellfailcount, plur(u.cnd_spellfailcount));
		enl_msg(You_, "have failed to cast a spell ", "failed to cast a spell ", buf);
	}
	if (u.cnd_forgottenspellcount) {
		sprintf(buf, "%ld time%s", u.cnd_forgottenspellcount, plur(u.cnd_forgottenspellcount));
		enl_msg(You_, "have attempted to cast forgotten spells ", "attempted to cast forgotten spells ", buf);
	}
	if (u.cnd_invokecount) {
		sprintf(buf, "%ld time%s", u.cnd_invokecount, plur(u.cnd_invokecount));
		enl_msg(You_, "have used the invocation effects of artifacts ", "used the invocation effects of artifacts ", buf);
	}
	if (u.cnd_techcount) {
		sprintf(buf, "%ld time%s", u.cnd_techcount, plur(u.cnd_techcount));
		enl_msg(You_, "have used techniques ", "used techniques ", buf);
	}
	if (u.cnd_phasedoorcount) {
		sprintf(buf, "%ld time%s", u.cnd_phasedoorcount, plur(u.cnd_phasedoorcount));
		enl_msg(You_, "have phase doored ", "phase doored ", buf);
	}
	if (u.cnd_teleportcount) {
		sprintf(buf, "%ld time%s", u.cnd_teleportcount, plur(u.cnd_teleportcount));
		enl_msg(You_, "have teleported ", "teleported ", buf);
	}
	if (u.cnd_telelevelcount) {
		sprintf(buf, "%ld time%s", u.cnd_telelevelcount, plur(u.cnd_telelevelcount));
		enl_msg(You_, "have levelported ", "levelported ", buf);
	}
	if (u.cnd_branchportcount) {
		sprintf(buf, "%ld time%s", u.cnd_branchportcount, plur(u.cnd_branchportcount));
		enl_msg(You_, "have branchported ", "branchported ", buf);
	}
	if (u.cnd_banishmentcount) {
		sprintf(buf, "%ld time%s", u.cnd_banishmentcount, plur(u.cnd_banishmentcount));
		enl_msg(You_, "were banished ", "were banished ", buf);
	}
	if (u.cnd_punishmentcount) {
		sprintf(buf, "%ld time%s", u.cnd_punishmentcount, plur(u.cnd_punishmentcount));
		enl_msg(You_, "were punished ", "were punished ", buf);
	}
	/* pet death count not disclosed because you might not notice that your pet died --Amy */
	if (u.cnd_petdeathcount && final) {
		sprintf(buf, "%ld pet%s", u.cnd_petdeathcount, plur(u.cnd_petdeathcount));
		enl_msg(You_, "had to mourn the death of ", "had to mourn the death of ", buf);
	}
	/* amnesia count not disclosed because there's not always a clue that you got hit by amnesia --Amy */
	if (u.cnd_amnesiacount && final) {
		sprintf(buf, "%ld time%s", u.cnd_amnesiacount, plur(u.cnd_amnesiacount));
		enl_msg(You_, "have suffered from amnesia ", "suffered from amnesia ", buf);
	}
	if (final) {
		sprintf(buf, "%d", u.cnd_minalignment);
		enl_msg(You_, "had a minimum alignment of ", "had a minimum alignment of ", buf);
	}
	if (final) {
		sprintf(buf, "%ld", u.cnd_maxalignment);
		enl_msg(You_, "had a maximum alignment of ", "had a maximum alignment of ", buf);
	}
	if (u.cnd_maxsanity) {
		sprintf(buf, "%ld", u.cnd_maxsanity);
		enl_msg(You_, "had a maximum sanity of ", "had a maximum sanity of ", buf);
	}
	/* sanity can be displayed, contamination requires enlightenment, so the latter is not disclosed --Amy */
	if (u.cnd_maxcontamination && final) {
		sprintf(buf, "%ld", u.cnd_maxcontamination);
		enl_msg(You_, "had a maximum contamination of ", "had a maximum contamination of ", buf);
	}
	if (u.cnd_searchtrapcount) {
		sprintf(buf, "%ld time%s", u.cnd_searchtrapcount, plur(u.cnd_searchtrapcount));
		enl_msg(You_, "have successfully searched for traps ", "successfully searched for traps ", buf);
	}
	if (u.cnd_searchsecretcount) {
		sprintf(buf, "%ld time%s", u.cnd_searchsecretcount, plur(u.cnd_searchsecretcount));
		enl_msg(You_, "have successfully searched for doors or corridors ", "successfully searched for doors or corridors ", buf);
	}
	if (u.cnd_fartingcount) {
		sprintf(buf, "%ld time%s", u.cnd_fartingcount, plur(u.cnd_fartingcount));
		enl_msg(You_, "heard farting noises ", "heard farting noises ", buf);
	}
	if (u.cnd_conversioncount) {
		sprintf(buf, "%ld time%s", u.cnd_conversioncount, plur(u.cnd_conversioncount));
		enl_msg(You_, "listened to conversion sermon ", "listened to conversion sermon ", buf);
	}
	if (u.cnd_wouwoucount) {
		sprintf(buf, "%ld time%s", u.cnd_wouwoucount, plur(u.cnd_wouwoucount));
		enl_msg(You_, "heard the frightening wouwou taunts ", "heard the frightening wouwou taunts ", buf);
	}
	if (u.cnd_perfumecount) {
		sprintf(buf, "%ld time%s", u.cnd_perfumecount, plur(u.cnd_perfumecount));
		enl_msg(You_, "inhaled scentful feminine perfume ", "inhaled scentful feminine perfume ", buf);
	}
	if (u.cnd_nutkickamount) {
		sprintf(buf, "%ld time%s", u.cnd_nutkickamount, plur(u.cnd_nutkickamount));
		enl_msg(You_, "got your nuts kicked ", "got your nuts kicked ", buf);
	}
	if (u.cnd_breastripamount) {
		sprintf(buf, "%ld time%s", u.cnd_breastripamount, plur(u.cnd_breastripamount));
		enl_msg(You_, "got hit in the breasts ", "got hit in the breasts ", buf);
	}
	if (u.cnd_saveamount) {
		sprintf(buf, "%ld time%s", u.cnd_saveamount, plur(u.cnd_saveamount));
		enl_msg(You_, "have saved the game ", "saved the game ", buf);
	}
	if (u.cnd_ragnarokamount) {
		sprintf(buf, "%ld time%s", u.cnd_ragnarokamount, plur(u.cnd_ragnarokamount));
		enl_msg(You_, "encountered the ragnarok event ", "encountered the ragnarok event ", buf);
	}
	if (u.cnd_datadeleteamount) {
		sprintf(buf, "%ld time%s", u.cnd_datadeleteamount, plur(u.cnd_datadeleteamount));
		enl_msg(You_, "had your data deleted ", "had your data deleted ", buf);
	}
	/* the curse items effect seems to give no message if you're blind, so you don't always see it happen --Amy */
	if (u.cnd_curseitemsamount && final) {
		sprintf(buf, "%ld time%s", u.cnd_curseitemsamount, plur(u.cnd_curseitemsamount));
		enl_msg(You_, "encountered the curse items effect ", "encountered the curse items effect ", buf);
	}
	/* nasty traps give no message and don't reveal themselves, so I sure as heck won't leak info here --Amy */
	if (u.cnd_nastytrapamount && final) {
		sprintf(buf, "%ld time%s", u.cnd_nastytrapamount, plur(u.cnd_nastytrapamount));
		enl_msg(You_, "have triggered nasty traps ", "triggered nasty traps ", buf);
	}
	/* these do give messages, but still, there might be situations where they don't --Amy */
	if (u.cnd_feminismtrapamount && final) {
		sprintf(buf, "%ld time%s", u.cnd_feminismtrapamount, plur(u.cnd_feminismtrapamount));
		enl_msg(You_, "have triggered feminism traps ", "triggered feminism traps ", buf);
	}
	if (u.cnd_plineamount) {
		sprintf(buf, "%ld time%s", u.cnd_plineamount, plur(u.cnd_plineamount));
		enl_msg(You_, "had to read random messages ", "had to read random messages ", buf);
	}
	/* yeah we obviously can't leak how many monsters were summoned --Amy */
	if (u.cnd_aggravateamount && final) {
		sprintf(buf, "%ld monster%s come out of portals", u.cnd_aggravateamount, plur(u.cnd_aggravateamount));
		enl_msg(You_, "saw ", "saw ", buf);
	}
	if (u.cnd_eatrinsicamount) {
		sprintf(buf, "%ld time%s", u.cnd_eatrinsicamount, plur(u.cnd_eatrinsicamount));
		enl_msg(You_, "have gained intrinsics from eating corpses ", "gained intrinsics from eating corpses ", buf);
	}
	if (u.cnd_shkserviceamount) {
		sprintf(buf, "%ld time%s", u.cnd_shkserviceamount, plur(u.cnd_shkserviceamount));
		enl_msg(You_, "have purchased shopkeeper services ", "purchased shopkeeper services ", buf);
	}
	/* yet again, player doesn't always know how many were summoned --Amy */
	if (u.cnd_kopsummonamount && final) {
		sprintf(buf, "%ld time%s", u.cnd_kopsummonamount, plur(u.cnd_kopsummonamount));
		enl_msg(You_, "had the kops called on you ", "had the kops called on you ", buf);
	}
	if (u.cnd_captchaamount) {
		sprintf(buf, "%ld captcha%s", u.cnd_captchaamount, plur(u.cnd_captchaamount));
		enl_msg(You_, "have solved ", "solved ", buf);
	}
	if (u.cnd_captchafail) {
		sprintf(buf, "%ld time%s", u.cnd_captchafail, plur(u.cnd_captchafail));
		enl_msg(You_, "failed to solve a captcha ", "failed to solve a captcha ", buf);
	}
	if (u.cnd_quizamount) {
		sprintf(buf, "%ld quiz question%s", u.cnd_quizamount, plur(u.cnd_quizamount));
		enl_msg(You_, "correctly answered ", "correctly answered ", buf);
	}
	if (u.cnd_quizfail) {
		sprintf(buf, "%ld quiz question%s", u.cnd_quizfail, plur(u.cnd_quizfail));
		enl_msg(You_, "incorrectly answered ", "incorrectly answered ", buf);
	}
	/* alter reality doesn't always give a message --Amy */
	if (u.cnd_alterrealityamount && final) {
		sprintf(buf, "%ld time%s", u.cnd_alterrealityamount, plur(u.cnd_alterrealityamount));
		enl_msg(You_, "had the reality altered ", "had the reality altered ", buf);
	}
	if (u.cnd_unlockamount) {
		sprintf(buf, "%ld lock%s", u.cnd_unlockamount, plur(u.cnd_unlockamount));
		enl_msg(You_, "picked ", "picked ", buf);
	}
	if (u.cnd_altarconvertamount) {
		sprintf(buf, "%ld altar%s", u.cnd_altarconvertamount, plur(u.cnd_altarconvertamount));
		enl_msg(You_, "have converted ", "converted ", buf);
	}
	if (u.cnd_itemstealamount) {
		sprintf(buf, "%ld time%s", u.cnd_itemstealamount, plur(u.cnd_itemstealamount));
		enl_msg(You_, "had your items stolen ", "had your items stolen ", buf);
	}
	if (u.cnd_poisonamount) {
		sprintf(buf, "%ld time%s", u.cnd_poisonamount, plur(u.cnd_poisonamount));
		enl_msg(You_, "got poisoned ", "got poisoned ", buf);
	}
	if (u.cnd_nursehealamount) {
		sprintf(buf, "%ld extra hit point%s from nurses", u.cnd_nursehealamount, plur(u.cnd_nursehealamount));
		enl_msg(You_, "have gained ", "gained ", buf);
	}
	if (u.cnd_nurseserviceamount) {
		sprintf(buf, "%ld time%s", u.cnd_nurseserviceamount, plur(u.cnd_nurseserviceamount));
		enl_msg(You_, "have purchased nurse services ", "purchased nurse services ", buf);
	}
	/* if you're blind, the game doesn't tell you that elbereth did something --Amy */
	if (u.cnd_elberethamount && final) {
		sprintf(buf, "%ld monster%s with Elbereth engravings", u.cnd_elberethamount, plur(u.cnd_elberethamount));
		enl_msg(You_, "scared ", "scared ", buf);
	}
	/* disenchantment can also happen silently --Amy */
	if (u.cnd_disenchantamount && final) {
		sprintf(buf, "%ld time%s", u.cnd_disenchantamount, plur(u.cnd_disenchantamount));
		enl_msg(You_, "had your items disenchanted ", "had your items disenchanted ", buf);
	}
	if (u.cnd_permstatdamageamount) {
		sprintf(buf, "%ld time%s", u.cnd_permstatdamageamount, plur(u.cnd_permstatdamageamount));
		enl_msg(You_, "permanently lost stat points ", "permanently lost stat points ", buf);
	}
	if (u.cnd_shoedamageamount) {
		sprintf(buf, "%ld time%s", u.cnd_shoedamageamount, plur(u.cnd_shoedamageamount));
		enl_msg(You_, "got whacked or kicked by female shoes ", "got whacked or kicked by female shoes ", buf);
	}
	if (u.cnd_farmlandremoved) {
		sprintf(buf, "%ld time%s", u.cnd_farmlandremoved, plur(u.cnd_farmlandremoved));
		enl_msg(You_, "forced farmland tiles out of existence ", "forced farmland tiles out of existence ", buf);
	}
	if (u.cnd_nethermistremoved) {
		sprintf(buf, "%ld time%s", u.cnd_nethermistremoved, plur(u.cnd_nethermistremoved));
		enl_msg(You_, "dissipated nether mist ", "dissipated nether mist ", buf);
	}
	/* you don't necessarily see the square that your ammo ended up on --Amy */
	if (u.cnd_ammomulched && final) {
		sprintf(buf, "%ld ranged weapon ammo%s", u.cnd_ammomulched, plur(u.cnd_ammomulched));
		enl_msg(You_, "have mulched ", "mulched ", buf);
	}
	/* you don't always see or hear your grenades explode either --Amy */
	if (u.cnd_gunpowderused && final) {
		sprintf(buf, "%ld gram%s of gunpowder", u.cnd_gunpowderused, plur(u.cnd_gunpowderused));
		enl_msg(You_, "have used up ", "used up ", buf);
	}
	/* we won't leak whether your scroll of charging was cursed or not --Amy */
	if (u.cnd_chargingcount && final) {
		sprintf(buf, "%ld time%s", u.cnd_chargingcount, plur(u.cnd_chargingcount));
		enl_msg(You_, "have recharged your items ", "recharged your items ", buf);
	}
	if (u.cnd_offercount) {
		sprintf(buf, "%ld time%s", u.cnd_offercount, plur(u.cnd_offercount));
		enl_msg(You_, "have made offerings to the gods ", "made offerings to the gods ", buf);
	}
	if (u.cnd_forcecount) {
		sprintf(buf, "%ld time%s", u.cnd_forcecount, plur(u.cnd_forcecount));
		enl_msg(You_, "have used the force against an enemy ", "used the force against an enemy ", buf);
	}

	/* Pop up the window and wait for a key */
	display_nhwindow(en_win, TRUE);
	destroy_nhwindow(en_win);
}

#ifdef DUMP_LOG
void
dump_conduct(final)
int final;
{
	char buf[BUFSZ];
	int ngenocided;

	dump("", "Voluntary challenges");

	if (!u.uconduct.food)
	    dump("", "  You went without food");
	    /* But beverages are okay */
	else if (!u.uconduct.unvegan)
	    dump("", "  You followed a strict vegan diet");
	else if (!u.uconduct.unvegetarian)
	    dump("", "  You were a vegetarian");

	/* Yes, this is an ungrammatical sentence. It will stay that way because I think it sounds funny. --Amy */
	if (u.uconduct.food) {
	    sprintf(buf, "  You eaten %ld time%s", 
		u.uconduct.food, plur(u.uconduct.food));
	    dump("", buf);
	}
	if (u.uconduct.unvegan) {
	    sprintf(buf, "  You eaten food with animal by-products %ld time%s", 
		u.uconduct.unvegan, plur(u.uconduct.unvegan));
	    dump("", buf);
	}
	if (u.uconduct.unvegetarian) {
	    sprintf(buf, "  You eaten meat %ld time%s", 
		u.uconduct.unvegetarian, plur(u.uconduct.unvegetarian));
	    dump("", buf);
	}

	if (!u.uconduct.gnostic)
	    dump("", "  You were an atheist");
	else {
	    sprintf(buf, "  You have communicated with the gods %ld time%s", 
		    u.uconduct.gnostic, plur(u.uconduct.gnostic));
	    dump("", buf);
	}

	if (!u.uconduct.praydone)
	    dump("", "  You never prayed to the gods");
	else {
	    sprintf(buf, "  You prayed to the gods %ld time%s", 
		    u.uconduct.praydone, plur(u.uconduct.praydone));
	    dump("", buf);
	}

	if (!u.uconduct.weaphit)
	    dump("", "  You never hit with a wielded weapon");
	else {
	    sprintf(buf, "  You hit with a wielded weapon %ld time%s",
		    u.uconduct.weaphit, plur(u.uconduct.weaphit));
	    dump("", buf);
	}

	if (!u.uconduct.killer)
	    dump("", "  You were a pacifist");
	else {
	    sprintf(buf, "  You killed %ld monster%s", 
		    u.uconduct.killer, plur(u.uconduct.killer));
	    dump("", buf);
	}

	if (!u.uconduct.literate)
	    dump("", "  You were illiterate");
	else {
	    sprintf(buf, "read items or engraved %ld time%s",
		    u.uconduct.literate, plur(u.uconduct.literate));
	    dump("  You ", buf);
	}

	ngenocided = num_genocides();
	if (ngenocided == 0) {
	    dump("", "  You never genocided any monsters");
	} else {
	    sprintf(buf, "genocided %d type%s of monster%s",
		    ngenocided, plur(ngenocided), plur(ngenocided));
	    dump("  You ", buf);
	}

	if (!u.uconduct.polypiles)
	    dump("", "  You never polymorphed an object");
	else {
	    sprintf(buf, "polymorphed %ld item%s",
		    u.uconduct.polypiles, plur(u.uconduct.polypiles));
	    dump("  You ", buf);
	}

	if (!u.uconduct.polyselfs)
	    dump("", "  You never changed form");
	else {
	    sprintf(buf, "changed form %ld time%s",
		    u.uconduct.polyselfs, plur(u.uconduct.polyselfs));
	    dump("  You ", buf);
	}

	if (!u.uconduct.wishes)
	    dump("", "  You used no wishes");
	else {
	    sprintf(buf, "used %ld wish%s",
		    u.uconduct.wishes, (u.uconduct.wishes > 1L) ? "es" : "");
	    dump("  You ", buf);

	    if (!u.uconduct.wisharti)
		dump("", "  You did not wish for any artifacts");
	}

	if (!u.uconduct.celibacy)
	    dump("", "  You remained celibate");
	else {
	    sprintf(buf, "your vow of celibacy %ld time%s",
		    u.uconduct.celibacy, plur(u.uconduct.celibacy));
	    dump("  You have broken ", buf);
	}

	sprintf(buf, "%ld time%s", u.cnd_applycount, plur(u.cnd_applycount));
	dump("  You applied items ", buf);

	sprintf(buf, "%ld time%s", u.cnd_unihorncount, plur(u.cnd_unihorncount));
	dump("  You experienced the healing effects of unicorn horns ", buf);

	sprintf(buf, "%ld item%s with the magic marker", u.cnd_markercount, plur(u.cnd_markercount));
	dump("  You created ", buf);

	sprintf(buf, "%ld item%s with the chemistry set", u.cnd_chemistrycount, plur(u.cnd_chemistrycount));
	dump("  You created ", buf);

	sprintf(buf, "%ld potion%s", u.cnd_quaffcount, plur(u.cnd_quaffcount));
	dump("  You quaffed ", buf);

	sprintf(buf, "%ld time%s", u.cnd_zapcount, plur(u.cnd_zapcount));
	dump("  You zapped wands ", buf);

	sprintf(buf, "%ld time%s", u.cnd_scrollcount, plur(u.cnd_scrollcount));
	dump("  You read scrolls ", buf);

	sprintf(buf, "%ld time%s", u.cnd_spellbookcount, plur(u.cnd_spellbookcount));
	dump("  You read spellbooks ", buf);

	sprintf(buf, "%ld time%s", u.cnd_spellcastcount, plur(u.cnd_spellcastcount));
	dump("  You successfully cast spells ", buf);

	sprintf(buf, "%ld time%s", u.cnd_spellfailcount, plur(u.cnd_spellfailcount));
	dump("  You failed to cast a spell ", buf);

	sprintf(buf, "%ld time%s", u.cnd_forgottenspellcount, plur(u.cnd_forgottenspellcount));
	dump("  You attempted to cast forgotten spells ", buf);

	sprintf(buf, "%ld time%s", u.cnd_invokecount, plur(u.cnd_invokecount));
	dump("  You used the invocation effects of artifacts ", buf);

	sprintf(buf, "%ld time%s", u.cnd_techcount, plur(u.cnd_techcount));
	dump("  You used techniques ", buf);

	sprintf(buf, "%ld time%s", u.cnd_phasedoorcount, plur(u.cnd_phasedoorcount));
	dump("  You phase doored ", buf);

	sprintf(buf, "%ld time%s", u.cnd_teleportcount, plur(u.cnd_teleportcount));
	dump("  You teleported ", buf);

	sprintf(buf, "%ld time%s", u.cnd_telelevelcount, plur(u.cnd_telelevelcount));
	dump("  You levelported ", buf);

	sprintf(buf, "%ld time%s", u.cnd_branchportcount, plur(u.cnd_branchportcount));
	dump("  You branchported ", buf);

	sprintf(buf, "%ld time%s", u.cnd_banishmentcount, plur(u.cnd_banishmentcount));
	dump("  You were banished ", buf);

	sprintf(buf, "%ld time%s", u.cnd_punishmentcount, plur(u.cnd_punishmentcount));
	dump("  You were punished ", buf);

	sprintf(buf, "%ld pet%s", u.cnd_petdeathcount, plur(u.cnd_petdeathcount));
	dump("  You had to mourn the death of ", buf);

	sprintf(buf, "%ld time%s", u.cnd_amnesiacount, plur(u.cnd_amnesiacount));
	dump("  You suffered from amnesia ", buf);

	sprintf(buf, "%d", u.cnd_minalignment);
	dump("  You had a minimum alignment of ", buf);

	sprintf(buf, "%ld", u.cnd_maxalignment);
	dump("  You had a maximum alignment of ", buf);

	sprintf(buf, "%ld", u.cnd_maxsanity);
	dump("  You had a maximum sanity of ", buf);

	sprintf(buf, "%ld", u.cnd_maxcontamination);
	dump("  You had a maximum contamination of ", buf);

	sprintf(buf, "%ld time%s", u.cnd_searchtrapcount, plur(u.cnd_searchtrapcount));
	dump("  You successfully searched for traps ", buf);

	sprintf(buf, "%ld time%s", u.cnd_searchsecretcount, plur(u.cnd_searchsecretcount));
	dump("  You successfully searched for doors or corridors ", buf);

	sprintf(buf, "%ld time%s", u.cnd_fartingcount, plur(u.cnd_fartingcount));
	dump("  You heard farting noises ", buf);

	sprintf(buf, "%ld time%s", u.cnd_conversioncount, plur(u.cnd_conversioncount));
	dump("  You listened to conversion sermon ", buf);

	sprintf(buf, "%ld time%s", u.cnd_wouwoucount, plur(u.cnd_wouwoucount));
	dump("  You heard the frightening wouwou taunts ", buf);

	sprintf(buf, "%ld time%s", u.cnd_perfumecount, plur(u.cnd_perfumecount));
	dump("  You inhaled scentful feminine perfume ", buf);

	sprintf(buf, "%ld time%s", u.cnd_nutkickamount, plur(u.cnd_nutkickamount));
	dump("  You got your nuts kicked ", buf);

	sprintf(buf, "%ld time%s", u.cnd_breastripamount, plur(u.cnd_breastripamount));
	dump("  You got hit in the breasts ", buf);

	sprintf(buf, "%ld time%s", u.cnd_saveamount, plur(u.cnd_saveamount));
	dump("  You saved the game ", buf);

	sprintf(buf, "%ld time%s", u.cnd_ragnarokamount, plur(u.cnd_ragnarokamount));
	dump("  You encountered the ragnarok event ", buf);

	sprintf(buf, "%ld time%s", u.cnd_datadeleteamount, plur(u.cnd_datadeleteamount));
	dump("  You had your data deleted ", buf);

	sprintf(buf, "%ld time%s", u.cnd_curseitemsamount, plur(u.cnd_curseitemsamount));
	dump("  You encountered the curse items effect ", buf);

	sprintf(buf, "%ld time%s", u.cnd_nastytrapamount, plur(u.cnd_nastytrapamount));
	dump("  You triggered nasty traps ", buf);

	sprintf(buf, "%ld time%s", u.cnd_feminismtrapamount, plur(u.cnd_feminismtrapamount));
	dump("  You triggered feminism traps ", buf);

	sprintf(buf, "%ld time%s", u.cnd_plineamount, plur(u.cnd_plineamount));
	dump("  You had to read random messages ", buf);

	sprintf(buf, "%ld monster%s come out of portals", u.cnd_aggravateamount, plur(u.cnd_aggravateamount));
	dump("  You saw ", buf);

	sprintf(buf, "%ld time%s", u.cnd_eatrinsicamount, plur(u.cnd_eatrinsicamount));
	dump("  You gained intrinsics from eating corpses ", buf);

	sprintf(buf, "%ld time%s", u.cnd_shkserviceamount, plur(u.cnd_shkserviceamount));
	dump("  You purchased shopkeeper services ", buf);

	sprintf(buf, "%ld time%s", u.cnd_kopsummonamount, plur(u.cnd_kopsummonamount));
	dump("  You had the kops called on you ", buf);

	sprintf(buf, "%ld captcha%s", u.cnd_captchaamount, plur(u.cnd_captchaamount));
	dump("  You solved ", buf);

	sprintf(buf, "%ld time%s", u.cnd_captchafail, plur(u.cnd_captchafail));
	dump("  You failed to solve a captcha ", buf);

	sprintf(buf, "%ld quiz question%s", u.cnd_quizamount, plur(u.cnd_quizamount));
	dump("  You correctly answered ", buf);

	sprintf(buf, "%ld quiz question%s", u.cnd_quizfail, plur(u.cnd_quizfail));
	dump("  You incorrectly answered ", buf);

	sprintf(buf, "%ld time%s", u.cnd_alterrealityamount, plur(u.cnd_alterrealityamount));
	dump("  You had the reality altered ", buf);

	sprintf(buf, "%ld lock%s", u.cnd_unlockamount, plur(u.cnd_unlockamount));
	dump("  You picked ", buf);

	sprintf(buf, "%ld altar%s", u.cnd_altarconvertamount, plur(u.cnd_altarconvertamount));
	dump("  You converted ", buf);

	sprintf(buf, "%ld time%s", u.cnd_itemstealamount, plur(u.cnd_itemstealamount));
	dump("  You had your items stolen ", buf);

	sprintf(buf, "%ld time%s", u.cnd_poisonamount, plur(u.cnd_poisonamount));
	dump("  You got poisoned ", buf);

	sprintf(buf, "%ld extra hit point%s from nurses", u.cnd_nursehealamount, plur(u.cnd_nursehealamount));
	dump("  You gained ", buf);

	sprintf(buf, "%ld time%s", u.cnd_nurseserviceamount, plur(u.cnd_nurseserviceamount));
	dump("  You purchased nurse services ", buf);

	sprintf(buf, "%ld monster%s with Elbereth engravings", u.cnd_elberethamount, plur(u.cnd_elberethamount));
	dump("  You scared ", buf);

	sprintf(buf, "%ld time%s", u.cnd_disenchantamount, plur(u.cnd_disenchantamount));
	dump("  You had your items disenchanted ", buf);

	sprintf(buf, "%ld time%s", u.cnd_permstatdamageamount, plur(u.cnd_permstatdamageamount));
	dump("  You permanently lost stat points ", buf);

	sprintf(buf, "%ld time%s", u.cnd_shoedamageamount, plur(u.cnd_shoedamageamount));
	dump("  You got whacked or kicked by female shoes ", buf);

	sprintf(buf, "%ld time%s", u.cnd_farmlandremoved, plur(u.cnd_farmlandremoved));
	dump("  You forced farmland tiles out of existence ", buf);

	sprintf(buf, "%ld time%s", u.cnd_nethermistremoved, plur(u.cnd_nethermistremoved));
	dump("  You dissipated nether mist ", buf);

	sprintf(buf, "%ld ranged weapon ammo%s", u.cnd_ammomulched, plur(u.cnd_ammomulched));
	dump("  You mulched ", buf);

	sprintf(buf, "%ld gram%s of gunpowder", u.cnd_gunpowderused, plur(u.cnd_gunpowderused));
	dump("  You used up ", buf);

	sprintf(buf, "%ld time%s", u.cnd_chargingcount, plur(u.cnd_chargingcount));
	dump("  You recharged your items ", buf);

	sprintf(buf, "%ld time%s", u.cnd_offercount, plur(u.cnd_offercount));
	dump("  You made offerings to the gods ", buf);

	sprintf(buf, "%ld time%s", u.cnd_forcecount, plur(u.cnd_forcecount));
	dump("  You used the force against an enemy ", buf);

	dump("", "");
}
#endif /* DUMP_LOG */

#endif /* OVLB */
#ifdef OVL1

/* Macros for meta and ctrl modifiers:
 *   M and C return the meta/ctrl code for the given character;
 *     e.g., (C('c') is ctrl-c
 *   ISMETA and ISCTRL return TRUE iff the code is a meta/ctrl code
 *   UNMETA and UNCTRL are the opposite of M/C and return the key for a given
 *     meta/ctrl code. */
#ifndef M
# ifndef NHSTDC
#  define M(c)		(0x80 | (c))
# else
#  define M(c)		((c) - 128)
# endif /* NHSTDC */
#endif
#define ISMETA(c) (((c) & 0x80) != 0)
#define UNMETA(c) ((c) & 0x7f)

#ifndef C
#define C(c)		(0x1f & (c))
#endif
#define ISCTRL(c) ((uchar)(c) < 0x20)
#define UNCTRL(c) (ISCTRL(c) ? (0x60 | (c)) : (c))


/* maps extended ascii codes for key presses to extended command entries in extcmdlist */
static struct key_tab cmdlist[256];

/* list built upon option loading; holds list of keys to be rebound later
 * see "crappy hack" below */
static struct binding_list_tab *bindinglist = NULL;

#define AUTOCOMPLETE TRUE
#define IFBURIED TRUE

#define EXTCMDLIST_SIZE (sizeof(extcmdlist) / sizeof(extcmdlist[1]))

struct ext_func_tab extcmdlist[] = {
	{"2weapon", "toggle two-weapon combat", dotwoweapon, !IFBURIED, AUTOCOMPLETE},
	{"adjust", "adjust inventory letters", doorganize, IFBURIED, AUTOCOMPLETE},
	{"annotate", "name current level", donamelevel, TRUE, AUTOCOMPLETE},
#ifdef BORG
	{"borg", "enable borg mode", doborgtoggle, IFBURIED, AUTOCOMPLETE},
#endif
	{"borrow", "steal from monsters", playersteal, IFBURIED, AUTOCOMPLETE},  /* jla */        
	{"chat", "talk to someone", dotalk, IFBURIED, AUTOCOMPLETE},    /* converse? */
	{"conduct", "list which challenges you have adhered to", doconduct, IFBURIED, AUTOCOMPLETE},
	{"dip", "dip an object into something", dodip, !IFBURIED, AUTOCOMPLETE},
	{"enhance", "advance or check weapons skills", enhance_weapon_skill, IFBURIED, AUTOCOMPLETE},
#if 0
//	{"ethics", "list which challenges you have adhered to", doethics, TRUE},
	{"ethics", "list which challenges you have adhered to", doethics, !IFBURIED, AUTOCOMPLETE},
#endif
#ifndef PUBLIC_SERVER
	{"explore", "enter explore mode", enter_explore_mode, IFBURIED, !AUTOCOMPLETE},
#endif
	{"force", "force a lock", doforce, !IFBURIED, AUTOCOMPLETE},
	{"invoke", "invoke an object's powers", doinvoke, IFBURIED, AUTOCOMPLETE},
	{"jump", "jump to a location", dojump, !IFBURIED, AUTOCOMPLETE},
	{"loot", "loot a box on the floor", doloot, IFBURIED, AUTOCOMPLETE},
	{"look", "look at an object", dolook, !IFBURIED, !AUTOCOMPLETE},
	{"mark", "mark item so pets don't drop it", domarkforpet, IFBURIED, AUTOCOMPLETE},
	{"monster", "use a monster's special ability", domonability, IFBURIED, AUTOCOMPLETE},
	{"name", "name an item or type of object", ddocall, IFBURIED},
	{"offer", "offer a sacrifice to the gods", dosacrifice, !IFBURIED, AUTOCOMPLETE},
	{"overview", "show an overview of the dungeon", dooverview, TRUE, AUTOCOMPLETE},
	{"pray", "pray to the gods for help", dopray, IFBURIED, AUTOCOMPLETE},
	{"quit", "exit without saving current game", done2, IFBURIED, AUTOCOMPLETE},

	{"apply", "apply (use) a tool (pick-axe, key, lamp...)", doapply, !IFBURIED},
	{"removeimarkers", "remove all \"I\"s, remembered, unseen creatures from the level", doremoveimarkers, IFBURIED},
	{"attributes", "show your attributes (intrinsic ones included in debug or explore mode)", doattributes, IFBURIED},
	{"close", "close a door", doclose, !IFBURIED},
	{"cast", "zap (cast) a spell", docast, IFBURIED},
	{"discoveries", "show what object types have been discovered", dodiscovered, IFBURIED},
	{"down", "go down a staircase", dodown, !IFBURIED},
	{"drop", "drop an item", dodrop, !IFBURIED},
	{"dropall", "drop specific item types", doddrop, !IFBURIED},
	{"takeoffall", "remove all armor", doddoremarm, !IFBURIED},
	{"inventory", "show your inventory", ddoinv, IFBURIED},
	{"quaff", "quaff (drink) something", dodrink, !IFBURIED},
	{"#", "perform an extended command", doextcmd, IFBURIED},
	{"travel", "Travel to a specific location", dotravel, !IFBURIED},
	{"eat", "eat something", doeat, !IFBURIED},
	{"engrave", "engrave writing on the floor", doengrave, !IFBURIED},
	{"fire", "fire ammunition from quiver", dofire, !IFBURIED},
	{"history", "show long version and game history", dohistory, IFBURIED},
	{"help", "give a help message", dohelp, IFBURIED},
	{"seetrap", "show the type of a trap", doidtrap, IFBURIED},
	{"kick", "kick something", dokick, !IFBURIED},
	{"call", "call (name) a particular monster", ddocall, IFBURIED},
	{"callold", "call (name) a particular monster (vanilla)", do_mname, IFBURIED},
	{"wait", "rest one move while doing nothing", donull, IFBURIED, !AUTOCOMPLETE, "waiting"},
	{"previous", "toggle through previously displayed game messages", doprev_message, IFBURIED},
	{"open", "open a door", doopen, !IFBURIED},
	{"pickup", "pick up things at the current location", dopickup, !IFBURIED},
	{"pay", "pay your shopping bill", dopay, !IFBURIED},
	{"puton", "put on an accessory (ring amulet, etc)", doputon, !IFBURIED},
	{"seeweapon", "show the weapon currently wielded", doprwep, IFBURIED},
	{"seearmor", "show the armor currently worn", doprarm, IFBURIED},
	{"seerings", "show the ring(s) currently worn", doprring, IFBURIED},
	{"seeamulet", "show the amulet currently worn", dopramulet, IFBURIED},
	{"seetools", "show the tools currently in use", doprtool, IFBURIED},
	{"seeall", "show all equipment in use (generally, ),[,=,\",( commands", doprinuse, IFBURIED},
	{"seegold", "count your gold", doprgold, IFBURIED},
	{"glance", "show what type of thing a map symbol on the level corresponds to", doquickwhatis, IFBURIED},
	{"remove", "remove an accessory (ring, amulet, etc)", doremring, !IFBURIED},
	{"read", "read a scroll or spellbook", doread, !IFBURIED},
	{"redraw", "redraw screen", doredraw, IFBURIED},
#ifdef SUSPEND
	{"suspend", "suspend game (only if defined)", dosuspend, IFBURIED},
#endif /* SUSPEND */
	{"setoptions", "show option settings, possibly change them", doset, IFBURIED},
	{"search", "search for traps and secret doors", dosearch, IFBURIED, !AUTOCOMPLETE, "searching"},
	{"save", "save the game", dosave, IFBURIED},
	{"swap", "swap wielded and secondary weapons", doswapweapon, !IFBURIED},
/* Have to put the definition in an ifdef too because "shell" could be rebound
 * to something
 */
#ifndef PUBLIC_SERVER
	{"shell", "do a shell escape (only if defined)", dosh, IFBURIED},
#endif
	{"throw", "throw something", dothrow, !IFBURIED},
	{"takeoff", "take off one piece of armor", dotakeoff, !IFBURIED},
	{"teleport", "teleport around level", dotele, IFBURIED},
	{"inventoryall", "inventory specific item types", dotypeinv, IFBURIED},
	{"autopickup", "toggle the pickup option on/off", dotogglepickup, IFBURIED},
	{"up", "go up a staircase", doup, !IFBURIED},
	{"version", "show version", doversion, IFBURIED},
	{"seespells", "list known spells", dovspell, IFBURIED},
	{"quiver", "select ammunition for quiver", dowieldquiver, !IFBURIED},
	{"whatis", "show what type of thing a symbol corresponds to", dowhatis, IFBURIED},
	{"whatdoes", "tell what a command does", dowhatdoes, IFBURIED},
	{"wield", "wield (put in use) a weapon", dowield, !IFBURIED},
	{"wear", "wear a piece of armor", dowear, !IFBURIED},
	{"zap", "zap a wand", dozap, !IFBURIED},

	{"ride", "ride (or stop riding) a monster", doride, !IFBURIED, AUTOCOMPLETE},
	{"rub", "rub a lamp or a stone", dorub, !IFBURIED, AUTOCOMPLETE},
#ifdef LIVELOG_SHOUT
	{"shout", "shout something", doshout, !IFBURIED, AUTOCOMPLETE},
#endif
	{"sit", "sit down", dosit, !IFBURIED, AUTOCOMPLETE},
#ifdef SHOUT
	{"shout", "say something loud", doyell, !IFBURIED, AUTOCOMPLETE}, /* jrn */
#endif
	{"technique", "perform a technique", dotech, IFBURIED, AUTOCOMPLETE},
	{"turn", "turn undead", doturn, IFBURIED, AUTOCOMPLETE},
	{"twoweapon", "toggle two-weapon combat", dotwoweapon, !IFBURIED, AUTOCOMPLETE},
	{"untrap", "untrap something", dountrap, !IFBURIED, AUTOCOMPLETE},
	{"vanquished", "list vanquished monsters", dolistvanq, IFBURIED, !AUTOCOMPLETE},
	{"versionext", "list compile time options for this version of Slash'EM", doextversion, IFBURIED, AUTOCOMPLETE},
	{"wipe", "wipe off your face", dowipe, IFBURIED, AUTOCOMPLETE},
	{"youpoly", "polymorph at will", polyatwill, IFBURIED, AUTOCOMPLETE},  /* jla */        
	{"?", "get this list of extended commands", doextlist, IFBURIED, AUTOCOMPLETE},
#if defined(WIZARD)
	/*
	 * There must be a blank entry here for every entry in the table
	 * below.
	 */
#ifdef DISPLAY_LAYERS
	{(char *)0, (char *)0, donull, TRUE},
#endif
	{(char *)0, (char *)0, donull, TRUE}, /* #levelchange */
	{(char *)0, (char *)0, donull, TRUE}, /* #lightsources */
#ifdef DEBUG_MIGRATING_MONS
	{(char *)0, (char *)0, donull, TRUE}, /* #migratemons */
#endif
	{(char *)0, (char *)0, donull, TRUE},
	{(char *)0, (char *)0, donull, TRUE},
#ifdef PORT_DEBUG
	{(char *)0, (char *)0, donull, TRUE},
#endif
	{(char *)0, (char *)0, donull, TRUE},
	{(char *)0, (char *)0, donull, TRUE},
        {(char *)0, (char *)0, donull, TRUE},
	{(char *)0, (char *)0, donull, TRUE},
	{(char *)0, (char *)0, donull, TRUE},
#ifdef DEBUG
	{(char *)0, (char *)0, donull, TRUE},
#endif
	{(char *)0, (char *)0, donull, TRUE}, /* #wmode */
	{(char *)0, (char *)0, donull, TRUE}, /* #detect */
	{(char *)0, (char *)0, donull, TRUE}, /* #map */
	{(char *)0, (char *)0, donull, TRUE}, /* #genesis */
	{(char *)0, (char *)0, donull, TRUE}, /* #identify */
	{(char *)0, (char *)0, donull, TRUE}, /* #levelport */
	{(char *)0, (char *)0, donull, TRUE}, /* #dotechwiz */
	{(char *)0, (char *)0, donull, TRUE}, /* #wish */
	{(char *)0, (char *)0, donull, TRUE}, /* #gainac */
	{(char *)0, (char *)0, donull, TRUE}, /* #gainlevel */
	{(char *)0, (char *)0, donull, TRUE}, /* #iddqd */
	{(char *)0, (char *)0, donull, TRUE}, /* #where */
#endif
	{(char *)0, (char *)0, donull, TRUE}	/* sentinel */
};

#ifdef WIZARD
static struct ext_func_tab debug_extcmdlist[] = {
#ifdef DISPLAY_LAYERS
	{"display", "detail display layers", wiz_show_display, IFBURIED, AUTOCOMPLETE},
#endif /* DISPLAY_LAYERS */
	{"levelchange", "change experience level", wiz_level_change, IFBURIED, AUTOCOMPLETE},
	{"lightsources", "show mobile light sources", wiz_light_sources, IFBURIED, AUTOCOMPLETE},
#ifdef DEBUG_MIGRATING_MONS
	{"migratemons", "migrate n random monsters", wiz_migrate_mons, IFBURIED, AUTOCOMPLETE},
#endif
	{"detectmons", "detect monsters", wiz_detect_monsters, IFBURIED, AUTOCOMPLETE},
	{"monpolycontrol", "control monster polymorphs", wiz_mon_polycontrol, IFBURIED, AUTOCOMPLETE},
	{"panic", "test panic routine (fatal to game)", wiz_panic, IFBURIED, AUTOCOMPLETE},
	{"polyself", "polymorph self", wiz_polyself, IFBURIED, AUTOCOMPLETE},
#ifdef PORT_DEBUG
	{"portdebug", "wizard port debug command", wiz_port_debug, IFBURIED, AUTOCOMPLETE},
#endif
	{"seenv", "show seen vectors", wiz_show_seenv, IFBURIED, AUTOCOMPLETE},
	{"stats", "show memory statistics", wiz_show_stats, IFBURIED, AUTOCOMPLETE},
	{"timeout", "look at timeout queue", wiz_timeout_queue, IFBURIED, AUTOCOMPLETE},
	{"vision", "show vision array", wiz_show_vision, IFBURIED, AUTOCOMPLETE},
#ifdef DEBUG
	{"wizdebug", "wizard debug command", wiz_debug_cmd, IFBURIED, AUTOCOMPLETE},
#endif
	{"wmode", "show wall modes", wiz_show_wmodes, IFBURIED, AUTOCOMPLETE},
	{"detect", "detect secret doors and traps", wiz_detect, IFBURIED},
	{"map", "do magic mapping", wiz_map, IFBURIED},
	{"genesis", "create monster", wiz_genesis, IFBURIED},
	{"identify", "identify items in pack", wiz_identify, IFBURIED},
	{"levelport", "to trans-level teleport", wiz_level_tele, IFBURIED},
	{"twiz", "reload techniques", dotechwiz, IFBURIED, AUTOCOMPLETE},
	{"wish", "make wish", wiz_wish, IFBURIED},
	{"gainac", "gain ac", wiz_gain_ac, IFBURIED},
	{"gainlevel", "gain level", wiz_gain_level, IFBURIED},
	{"iddqd", "become invulnerable", wiz_toggle_invulnerability, IFBURIED},
	{"where", "tell locations of special levels", wiz_where, IFBURIED},
	{(char *)0, (char *)0, donull, IFBURIED}

};


static void
bind_key(key, command)
	unsigned char key;
	char* command;
{
	struct ext_func_tab * extcmd;

	/* special case: "nothing" is reserved for unbinding */
	if (!strcmp(command, "nothing")) {
		cmdlist[key].bind_cmd = NULL;
		return;
	}

	for(extcmd = extcmdlist; extcmd->ef_txt; extcmd++) {
		if (strcmp(command, extcmd->ef_txt)) continue;
		cmdlist[key].bind_cmd = extcmd;
		return;
	}

	pline("Bad command %s matched with key %c (ASCII %i). "
		"Ignoring command.\n", command, key, key);
}


static void
init_bind_list(void)
{
	bind_key(C('d'), "kick" ); /* "D" is for door!...?  Msg is in dokick.c */
#ifdef WIZARD
	if (wizard) {
		bind_key(C('e'), "detect" );
		bind_key(C('f'), "map" );
		bind_key(C('g'), "genesis" );
		bind_key(C('i'), "identify" );
		bind_key(C('o'), "where" );
		bind_key(C('v'), "levelport" );
		bind_key(C('w'), "wish" );
		bind_key(C('h'), "detectmons" );
		bind_key(C('c'), "gainac" );
		bind_key(C('j'), "gainlevel" );
		bind_key(C('n'), "iddqd" );
	}
#endif
	bind_key(C('l'), "redraw" ); /* if number_pad is set */
	bind_key(C('p'), "previous" );
	bind_key(C('r'), "redraw" );
	bind_key(C('t'), "teleport" );
	bind_key(C('x'), "attributes" );
	bind_key(C('y'), "youpoly" );
	bind_key(C('b'), "borrow" );
	bind_key(C('s'), "save" );
#ifdef SUSPEND
	if (iflags.qwertz_layout) {
		bind_key(C('y'), "suspend" );
	} else {
		bind_key(C('z'), "suspend" );
	}
#endif
	bind_key('a',    "apply" );
	bind_key('A',    "takeoffall" );
	bind_key(M('a'), "adjust" );
	/*       'b', 'B' : go sw */
	bind_key(M('b'), "borrow" );
	bind_key('c',    "close" );
	bind_key('C',    "callold" );
	bind_key(M('c'), "chat" );
	bind_key('d',    "drop" );
	bind_key('D',    "dropall" );
	bind_key(M('d'), "dip" );
	bind_key('e',    "eat" );
	bind_key('E',    "engrave" );
	bind_key(M('e'), "enhance" );
	bind_key('f',    "fire" );
	/*       'F' : fight (one time) */
	bind_key(M('f'), "force" );
	/*       'g', 'G' : multiple go */
	/*       'h', 'H' : go west */
	bind_key('h',    "help" ); /* if number_pad is set */
	bind_key('i',    "inventory" );
	bind_key('I',    "inventoryall" ); /* Robert Viduya */
	bind_key(M('i'), "invoke" );
	bind_key('j',    "jump" );
	/*       'j', 'J', 'k', 'K', 'l', 'L', 'm', 'M', 'n', 'N' : move commands */
	bind_key(M('j'), "jump" ); /* if number_pad is on */
	bind_key('k',    "kick" ); /* if number_pad is on */
	bind_key('K',	"vanquished" );  /* if number_pad is on */
	bind_key('l',    "loot" ); /* if number_pad is on */
	bind_key(M('l'), "loot" );
	bind_key(M('m'), "monster" );
	bind_key('N',    "name" );
	/*       'n' prefixes a count if number_pad is on */
	bind_key(M('n'), "name" );
	bind_key(M('N'), "name" ); /* if number_pad is on */
	bind_key('o',    "open" );
	bind_key('O',    "setoptions" );
	bind_key(M('o'), "offer" );
	bind_key('p',    "pay" );
	bind_key('P',    "puton" );
	bind_key(M('p'), "pray" );
	bind_key('q',    "quaff" );
	bind_key('Q',    "quiver" );
	bind_key(M('q'), "quit" );
	bind_key('r',    "read" );
	bind_key('R',    "remove" );
	bind_key(M('r'), "rub" );
	bind_key('s',    "search" );
	bind_key('S',    "save" );
	bind_key(M('s'), "sit" );
	bind_key('t',    "throw" );
	bind_key('T',    "takeoff" );
	bind_key(M('t'), "technique" );
	/*        'u', 'U' : go ne */
	bind_key('u',    "untrap" ); /* if number_pad is on */
	bind_key(M('u'), "untrap" );
	bind_key(C('U'), "removeimarkers" );
	bind_key('v',    "version" );
	bind_key('V',    "history" );
	bind_key(M('v'), "versionext" );
	bind_key('w',    "wield" );
	bind_key('W',    "wear" );
	bind_key(M('w'), "wipe" );
	bind_key('x',    "swap" );
	bind_key('X',    "twoweapon" );
	/*bind_key('X',    "explore_mode" );*/
	/*        'y', 'Y' : go nw */
#ifdef STICKY_COMMAND
	if (iflags.qwertz_layout) {
		bind_key(M('z'), "sticky" );
	} else {
		bind_key(M('y'), "sticky" );
	}
#endif /* STICKY_COMMAND */
	if (iflags.qwertz_layout) {
		bind_key('y',	"zap" );
		bind_key('Y',	"cast" );
	} else {
		bind_key('z',    "zap" );
		bind_key('Z',    "cast" );
	}
	bind_key('<',    "up" );
	bind_key('>',    "down" );
	bind_key('/',    "whatis" );
	bind_key('&',    "whatdoes" );
	bind_key('?',    "help" );
	bind_key(M('?'), "?" );
#ifndef PUBLIC_SERVER
	bind_key('!',    "shell" );
#endif
	bind_key('.',    "wait" );
	bind_key(' ',    "wait" );
	bind_key(',',    "pickup" );
	bind_key(':',    "look" );
	bind_key(';',    "glance" );
	bind_key('^',    "seetrap" );
	bind_key('\\',   "discoveries" ); /* Robert Viduya */
	bind_key('@',    "autopickup" );
	bind_key(M('2'), "twoweapon" );
	bind_key(WEAPON_SYM, "seeweapon" );
	bind_key(ARMOR_SYM,  "seearmor" );
	bind_key(RING_SYM,   "seerings" );
	bind_key(AMULET_SYM, "seeamulet" );
	bind_key(TOOL_SYM,   "seetools" );
	bind_key('*',        "seeall" ); /* inventory of all equipment in use */
	bind_key(GOLD_SYM,   "seegold" );
	bind_key(SPBOOK_SYM, "seespells" ); /* Mike Stephenson */
	bind_key('#', "#");
	bind_key('_', "travel");
}

/* takes the list of bindings loaded from the options file, and changes cmdlist
 * to match it */
static void
change_bind_list(void)
{
	struct binding_list_tab *binding;

	/* TODO: they must be loaded forward, not backward as they are now */
	while ((binding = bindinglist)) {
		bindinglist = bindinglist->next;
		bind_key(binding->key, binding->extcmd);
		free(binding->extcmd);
		free(binding);
	}
}


/*
 * Insert debug commands into the extended command list.  This function
 * assumes that the last entry will be the help entry.
 *
 * You must add entries in ext_func_tab every time you add one to the
 * debug_extcmdlist().
 */
static void
add_debug_extended_commands()
{
	int i, j, k, n;

	/* count the # of help entries */
	for (n = 0; extcmdlist[n].ef_txt; n++) ;

	for (i = 0; debug_extcmdlist[i].ef_txt; i++) {
	    extcmdlist[n + i] = debug_extcmdlist[i];
	}
}

/* list all keys and their bindings, like dat/hh but dynamic */
void
dokeylist(void)
{
	char	buf[BUFSZ], buf2[BUFSZ];
	uchar	key;
	boolean keys_used[256] = {0};
	register const char*	dir_keys;
	winid	datawin;
	int	i;
	char*	dir_desc[10] = {"move west",
				"move northwest",
				"move north",
				"move northeast",
				"move east",
				"move southeast",
				"move south",
				"move southwest",
				"move downward",
				"move upward"};
	char*	misc_desc[MISC_CMD_COUNT] = 
		{"rush until something interesting is seen",
		 "run until something extremely interesting is seen",
		 "fight even if you don't see a monster",
		 "move without picking up objects/fighting",
		 "run without picking up objects/fighting",
		 "escape from the current query/action",
		 "redo the previous command"
		};


	datawin = create_nhwindow(NHW_TEXT);
	putstr(datawin, 0, "");
	putstr(datawin, 0, "            Full Current Key Bindings List");
	putstr(datawin, 0, "");

	/* directional keys */
	if (iflags.num_pad) dir_keys = ndir;
	else dir_keys = sdir;
	putstr(datawin, 0, "Directional keys:");
	{
	  sprintf(buf, "  %c %c %c", dir_keys[1], dir_keys[2], dir_keys[3]);
	  putstr(datawin, 0, buf);
	  putstr(datawin, 0, "   \\|/");
	  sprintf(buf, "  %c-.-%c", dir_keys[0], dir_keys[4]);
	  putstr(datawin, 0, buf);
	  putstr(datawin, 0, "   /|\\");
	  sprintf(buf, "  %c %c %c", dir_keys[7], dir_keys[6], dir_keys[5]);
	  putstr(datawin, 0, buf);
	  putstr(datawin, 0, "");
	  sprintf(buf, "    %c  up", dir_keys[9]);
	  putstr(datawin, 0, buf);
	  sprintf(buf, "    %c  down", dir_keys[8]);
	  putstr(datawin, 0, buf);
	  putstr(datawin, 0, "");
	}
	for (i = 0; i < 10; i++) {
		key = dir_keys[i];
		keys_used[key] = TRUE;
		if (!iflags.num_pad) {
			keys_used[toupper(key)] = TRUE;
			keys_used[C(key)] = TRUE;
		}
		/*
		sprintf(buf, "%c\t%s", key, dir_desc[i]);
		putstr(datawin, 0, buf);
		*/
	}
	if (!iflags.num_pad) {
		putstr(datawin, 0, "Shift-<direction> will move in specified direction until you hit");
		putstr(datawin, 0, "        a wall or run into something.");
		putstr(datawin, 0, "Ctrl-<direction> will run in specified direction until something");
		putstr(datawin, 0, "        very interesting is seen.");
	}
	putstr(datawin, 0, "");

	/* special keys -- theoretically modifiable but many are still hard-coded*/
	putstr(datawin, 0, "Miscellaneous keys:");
	for (i = 0; i < MISC_CMD_COUNT; i++) {
		key = misc_cmds[i];
		keys_used[key] = TRUE;
		sprintf(buf, "%s\t%s", key2txt(key, buf2), misc_desc[i]);
		putstr(datawin, 0, buf);
	}
	putstr(datawin, 0, "");

	/* more special keys -- all hard-coded */
#ifndef NO_SIGNAL
	putstr(datawin, 0, "^c\tbreak out of nethack (SIGINT)");
	keys_used[C('c')] = TRUE;
	if (!iflags.num_pad) putstr(datawin, 0, "");
#endif
	if (iflags.num_pad) {
		putstr(datawin, 0, "-\tforce fight (same as above)");
		putstr(datawin, 0, "5\trun (same as above)");
		putstr(datawin, 0, "0\tinventory (as #inventory)");
		keys_used['-'] = keys_used['5'] = keys_used['0'] = TRUE;
		putstr(datawin, 0, "");
	}

	/* command keys - can be rebound or remapped*/
	putstr(datawin, 0, "Command keys:");
	for(i=0; i<=255; i++) {
		struct ext_func_tab * extcmd;
		char* mapping;
		key = i;
		/* JDS: not the most efficient way, perhaps */
		if (keys_used[i]) continue;
		if (key == ' ' && !flags.rest_on_space) continue;
		if ((extcmd = cmdlist[i].bind_cmd)) {
			sprintf(buf, "%s\t%s", key2txt(key, buf2),
				extcmd->ef_desc);
			putstr(datawin, 0, buf);
		}
	}
	putstr(datawin, 0, "");

	display_nhwindow(datawin, FALSE);
	destroy_nhwindow(datawin);
}

static const char template[] = "%-18s %4ld  %6ld";
static const char count_str[] = "                   count  bytes";
static const char separator[] = "------------------ -----  ------";

STATIC_OVL void
count_obj(chain, total_count, total_size, top, recurse)
	struct obj *chain;
	long *total_count;
	long *total_size;
	boolean top;
	boolean recurse;
{
	long count, size;
	struct obj *obj;

	for (count = size = 0, obj = chain; obj; obj = obj->nobj) {
	    if (top) {
		count++;
		size += sizeof(struct obj) + obj->oxlth + obj->onamelth;
	    }
	    if (recurse && obj->cobj)
		count_obj(obj->cobj, total_count, total_size, TRUE, TRUE);
	}
	*total_count += count;
	*total_size += size;
}

STATIC_OVL void
obj_chain(win, src, chain, total_count, total_size)
	winid win;
	const char *src;
	struct obj *chain;
	long *total_count;
	long *total_size;
{
	char buf[BUFSZ];
	long count = 0, size = 0;

	count_obj(chain, &count, &size, TRUE, FALSE);
	*total_count += count;
	*total_size += size;
	sprintf(buf, template, src, count, size);
	putstr(win, 0, buf);
}

STATIC_OVL void
mon_invent_chain(win, src, chain, total_count, total_size)
	winid win;
	const char *src;
	struct monst *chain;
	long *total_count;
	long *total_size;
{
	char buf[BUFSZ];
	long count = 0, size = 0;
	struct monst *mon;

	for (mon = chain; mon; mon = mon->nmon)
	    count_obj(mon->minvent, &count, &size, TRUE, FALSE);
	*total_count += count;
	*total_size += size;
	sprintf(buf, template, src, count, size);
	putstr(win, 0, buf);
}

STATIC_OVL void
contained(win, src, total_count, total_size)
	winid win;
	const char *src;
	long *total_count;
	long *total_size;
{
	char buf[BUFSZ];
	long count = 0, size = 0;
	struct monst *mon;

	count_obj(invent, &count, &size, FALSE, TRUE);
	count_obj(fobj, &count, &size, FALSE, TRUE);
	count_obj(level.buriedobjlist, &count, &size, FALSE, TRUE);
	count_obj(migrating_objs, &count, &size, FALSE, TRUE);
	/* DEADMONSTER check not required in this loop since they have no inventory */
	for (mon = fmon; mon; mon = mon->nmon)
	    count_obj(mon->minvent, &count, &size, FALSE, TRUE);
	for (mon = migrating_mons; mon; mon = mon->nmon)
	    count_obj(mon->minvent, &count, &size, FALSE, TRUE);

	*total_count += count; *total_size += size;

	sprintf(buf, template, src, count, size);
	putstr(win, 0, buf);
}

STATIC_OVL void
mon_chain(win, src, chain, total_count, total_size)
	winid win;
	const char *src;
	struct monst *chain;
	long *total_count;
	long *total_size;
{
	char buf[BUFSZ];
	long count, size;
	struct monst *mon;

	for (count = size = 0, mon = chain; mon; mon = mon->nmon) {
	    count++;
	    size += sizeof(struct monst) + mon->mxlth + mon->mnamelth;
	}
	*total_count += count;
	*total_size += size;
	sprintf(buf, template, src, count, size);
	putstr(win, 0, buf);
}

/*
 * Display memory usage of all monsters and objects on the level.
 */
static int
wiz_show_stats()
{
	char buf[BUFSZ];
	winid win;
	long total_obj_size = 0, total_obj_count = 0;
	long total_mon_size = 0, total_mon_count = 0;

	win = create_nhwindow(NHW_TEXT);
	putstr(win, 0, "Current memory statistics:");
	putstr(win, 0, "");
	sprintf(buf, "Objects, size %d", (int) sizeof(struct obj));
	putstr(win, 0, buf);
	putstr(win, 0, "");
	putstr(win, 0, count_str);

	obj_chain(win, "invent", invent, &total_obj_count, &total_obj_size);
	obj_chain(win, "fobj", fobj, &total_obj_count, &total_obj_size);
	obj_chain(win, "buried", level.buriedobjlist,
				&total_obj_count, &total_obj_size);
	obj_chain(win, "migrating obj", migrating_objs,
				&total_obj_count, &total_obj_size);
	mon_invent_chain(win, "minvent", fmon,
				&total_obj_count,&total_obj_size);
	mon_invent_chain(win, "migrating minvent", migrating_mons,
				&total_obj_count, &total_obj_size);

	contained(win, "contained",
				&total_obj_count, &total_obj_size);

	putstr(win, 0, separator);
	sprintf(buf, template, "Total", total_obj_count, total_obj_size);
	putstr(win, 0, buf);

	putstr(win, 0, "");
	putstr(win, 0, "");
	sprintf(buf, "Monsters, size %d", (int) sizeof(struct monst));
	putstr(win, 0, buf);
	putstr(win, 0, "");

	mon_chain(win, "fmon", fmon,
				&total_mon_count, &total_mon_size);
	mon_chain(win, "migrating", migrating_mons,
				&total_mon_count, &total_mon_size);

	putstr(win, 0, separator);
	sprintf(buf, template, "Total", total_mon_count, total_mon_size);
	putstr(win, 0, buf);

#if defined(__BORLANDC__) && !defined(_WIN32)
	show_borlandc_stats(win);
#endif

	display_nhwindow(win, FALSE);
	destroy_nhwindow(win);
	return 0;
}

void
sanity_check()
{
	obj_sanity_check();
	timer_sanity_check();
}

#ifdef DISPLAY_LAYERS
/*
 * Detail contents of each display layer at specified location(s).
 */
static int
wiz_show_display()
{
    int ans, glyph;
    coord cc;
    winid win;
    char buf[BUFSZ];
    struct rm *lev;

    cc.x = u.ux;
    cc.y = u.uy;
    pline("Pick a location.");
    ans = getpos(&cc, FALSE, "a location of interest");
    if (ans < 0 || cc.x < 0)
	return 0;	/* done */
    lev = &levl[cc.x][cc.y];
    win = create_nhwindow(NHW_MENU);
    sprintf(buf, "Contents of hero's memory at (%d, %d):", cc.x, cc.y);
    putstr(win, 0, buf);
    putstr(win, 0, "");
    sprintf(buf, "Invisible monster: %s",
	    lev->mem_invis ? "present" : "none");
    putstr(win, 0, buf);
    if (lev->mem_obj && lev->mem_corpse)
	if (mons[lev->mem_obj - 1].geno & G_UNIQ)
	    sprintf(buf, "Object: %s%s corpse",
		    type_is_pname(&mons[lev->mem_obj - 1]) ? "" : "the ",
		    s_suffix(mons[lev->mem_obj - 1].mname));
	else
	    sprintf(buf, "Object: %s corpse", mons[lev->mem_obj - 1].mname);
    else
	sprintf(buf, "Object: %s", lev->mem_obj ?
		obj_typename(lev->mem_obj - 1) : "none");
    putstr(win, 0, buf);
    sprintf(buf, "Trap: %s", lev->mem_trap ?
	    defsyms[trap_to_defsym(lev->mem_trap)].explanation : "none");
    putstr(win, 0, buf);
    sprintf(buf, "Backgroud: %s", defsyms[lev->mem_bg].explanation);
    putstr(win, 0, buf);
    putstr(win, 0, "");
    glyph = glyph_at(cc.x, cc.y);
    sprintf(buf, "Buffered (3rd screen): ");
    if (glyph_is_monster(glyph)) {
	strcat(buf, mons[glyph_to_mon(glyph)].mname);
	if (glyph_is_pet(glyph))
	    strcat(buf, " (tame)");
	if (glyph_is_ridden_monster(glyph))
	    strcat(buf, " (ridden)");
	if (glyph_is_detected_monster(glyph))
	    strcat(buf, " (detected)");
    } else if (glyph_is_object(glyph)) {
	if (glyph_is_body(glyph)) {
	    int corpse = glyph_to_body(glyph);
	    if (mons[corpse].geno & G_UNIQ)
		sprintf(eos(buf), "%s%s corpse",
			type_is_pname(&mons[corpse]) ? "" : "the ",
			s_suffix(mons[corpse].mname));
	    else
		sprintf(eos(buf), "%s corpse", mons[corpse].mname);
	} else
	    strcat(buf, obj_typename(glyph_to_obj(glyph)));
    } else if (glyph_is_invisible(glyph))
	strcat(buf, "invisible monster");
    else if (glyph_is_cmap(glyph))
	strcat(buf, defsyms[glyph_to_cmap(glyph)].explanation);
    else
	sprintf(eos(buf), "[%d]", glyph);
    putstr(win, 0, buf);
    display_nhwindow(win, FALSE);
    destroy_nhwindow(win);
    return 0;
}
#endif

#ifdef DEBUG_MIGRATING_MONS
static int
wiz_migrate_mons()
{
	int mcount = 0;
	char inbuf[BUFSZ];
	struct permonst *ptr;
	struct monst *mtmp;
	d_level tolevel;
	getlin("How many random monsters to migrate? [0]", inbuf);
	if (*inbuf == '\033') return 0;
	mcount = atoi(inbuf);
	if (mcount < 0 || mcount > (COLNO * ROWNO) || Is_botlevel(&u.uz))
		return 0;
	while (mcount > 0) {
		if (Is_stronghold(&u.uz))
		    assign_level(&tolevel, &valley_level);
		else
		    get_level(&tolevel, depth(&u.uz) + 1);
		ptr = rndmonst();
		mtmp = makemon(ptr, 0, 0, NO_MM_FLAGS);
		if (mtmp) migrate_to_level(mtmp, ledger_no(&tolevel),
				MIGR_RANDOM, (coord *)0);
		mcount--;
	}
	return 0;
}
#endif



#endif /* WIZARD */

static int
compare_commands(_cmd1, _cmd2)
     /* a wrapper function for strcmp.  Can this be done more simply? */
     void *_cmd1, *_cmd2;
{
	struct ext_func_tab *cmd1 = _cmd1, *cmd2 = _cmd2;

	return strcmp(cmd1->ef_txt, cmd2->ef_txt);
}

void
commands_init(void)
{
	int count = 0;

#ifdef WIZARD
	if (wizard) add_debug_extended_commands();
#endif
	while(extcmdlist[count].ef_txt) count++;

	qsort(extcmdlist, count, sizeof(struct ext_func_tab),
	      &compare_commands);

	init_bind_list();	/* initialize all keyboard commands */
	change_bind_list();	/* change keyboard commands based on options */
}

/* returns a one-byte character from the text (it may massacre the txt
 * buffer) */
char
txt2key(txt)
     char* txt;
{
	txt = stripspace(txt);
	if (!*txt) return 0;

	/* simple character */
	if (!txt[1]) return txt[0];

	/* a few special entries */
	if (!strcmp(txt, "<enter>")) return '\n';
	if (!strcmp(txt, "<space>")) return ' ';
	if (!strcmp(txt, "<esc>"))   return '\033';

	/* control and meta keys */
	switch (*txt) {
	    case 'm': /* can be mx, Mx, m-x, M-x */
	    case 'M':
		    txt++;
		    if(*txt == '-' && txt[1]) txt++;
		    if (txt[1]) return 0;
		    return M( *txt );
	    case 'c': /* can be cx, Cx, ^x, c-x, C-x, ^-x */
	    case 'C':
	    case '^':
		    txt++;
		    if(*txt == '-' && txt[1]) txt++;
		    if (txt[1]) return 0;
		    return C( *txt );
	}

	/* ascii codes: must be three-digit decimal */
	if (*txt >= '0' && *txt <= '9') {
		uchar key = 0;
		int i;
		for(i = 0; i < 3; i++) {
			if(txt[i]<'0' || txt[i]>'9') return 0;
			key = 10 * key + txt[i]-'0';
		}
		return key;
	}

	return 0;
}

/* returns the text for a one-byte encoding
 * must be shorter than a tab for proper formatting */
char*
key2txt(c, txt)
     char c;
     char* txt; /* sufficiently long buffer */
{
	if (c == ' ')
		sprintf(txt, "<space>");
	else if (c == '\033')
		sprintf(txt, "<esc>");
	else if (c == '\n')
		sprintf(txt, "<enter>");
	else if (ISCTRL(c))
		sprintf(txt, "^%c", UNCTRL(c));
	else if (ISMETA(c))
		sprintf(txt, "M-%c", UNMETA(c));
	else if (c >= 33 && c <= 126)
		sprintf(txt, "%c", c);		/* regular keys: ! through ~ */
	else
		sprintf(txt, "A-%i", c);	/* arbitrary ascii combinations */
	return txt;
}

/* returns the text for a string of one-byte encodings */
char*
str2txt(s, txt)
     char* s;
     char* txt;
{
	char* buf = txt;
	
	while (*s) {
		(void) key2txt(*s, buf);
		buf = eos(buf);
		*buf = ' ';
		buf++;
		*buf = 0;
		s++;
      	}
	return txt;
}


/* strips leading and trailing whitespace */
char*
stripspace(txt)
     char* txt;
{
	char* end;
	while (isspace(*txt)) txt++;
	end = eos(txt);
	while (--end >= txt && isspace(*end)) *end = 0;
	return txt;
}

void
parsebindings(bindings)
     /* closely follows parseoptions in options.c */
     char* bindings;
{
	char *bind;
	char key;
	struct binding_list_tab *newbinding = NULL;

	/* break off first binding from the rest; parse the rest */
	if ((bind = index(bindings, ',')) != 0) {
		*bind++ = 0;
		parsebindings(bind);
	}
 
	/* parse a single binding: first split around : */
	if (! (bind = index(bindings, ':'))) return; /* it's not a binding */
	*bind++ = 0;

	/* read the key to be bound */
	key = txt2key(bindings);
	if (!key) {
		raw_printf("Bad binding %s.", bindings);
		wait_synch();
		return;
	}
	
	/* JDS: crappy hack because wizard mode information
	 * isn't read until _after_ key bindings are read,
	 * and to change this would cause numerous side effects.
	 * instead, I save a list of rebindings, which are later
	 * bound. */
	bind = stripspace(bind);
	newbinding = (struct binding_list_tab *)alloc(sizeof(*newbinding));
	newbinding->key = key;
	newbinding->extcmd = (char *)alloc(strlen(bind)+1);
	strcpy(newbinding->extcmd, bind);;
	newbinding->next = bindinglist;
	bindinglist = newbinding;
}
 
void
parseautocomplete(autocomplete,condition)
     /* closesly follows parsebindings and parseoptions */
     char* autocomplete;
     boolean condition;
{
	register char *autoc;
	int i;
	
	/* break off first autocomplete from the rest; parse the rest */
	if ((autoc = index(autocomplete, ','))
	    || (autoc = index(autocomplete, ':'))) {
		*autoc++ = 0;
		parseautocomplete(autoc, condition);
	}

	/* strip leading and trailing white space */
	autocomplete = stripspace(autocomplete);
	
	if (!*autocomplete) return;

	/* take off negations */
	while (*autocomplete == '!') {
		/* unlike most options, a leading "no" might actually be a part of
		 * the extended command.  Thus you have to use ! */
		autocomplete++;
		condition = !condition;
	}

	/* find and modify the extended command */
	/* JDS: could be much faster [O(log n) vs O(n)] if done differently */
	for (i=0; extcmdlist[i].ef_txt; i++) {
		if (strcmp(autocomplete, extcmdlist[i].ef_txt)) continue;
		extcmdlist[i].autocomplete = condition;
		return;
	}

#ifdef WIZARD
	/* do the exact same thing with the wizmode list */
	/* this is a hack because wizard-mode commands haven't been loaded yet when
	 * this code is run.  See "crappy hack" elsewhere. */
	for (i=0; debug_extcmdlist[i].ef_txt; i++) {
		if (strcmp(autocomplete, debug_extcmdlist[i].ef_txt)) continue;
		debug_extcmdlist[i].autocomplete = condition;
		return;
	}
#endif

	/* not a real extended command */
	raw_printf ("Bad autocomplete: invalid extended command '%s'.", autocomplete);
	wait_synch();
}


void
rhack(cmd)
register char *cmd;
{
	boolean do_walk, do_rush, prefix_seen, bad_command,
		firsttime = (cmd == 0);


	iflags.menu_requested = FALSE;
	if (firsttime) {
		flags.nopick = 0;
		cmd = parse();
	}

	if (*cmd && (LagBugEffect || u.uprops[LAG_BUG].extrinsic || have_lagstone()) && !rn2(3) ) {
		flags.move = FALSE;
		return;

	}

	/* Autopilot means your char does random things depending on your contamination --Amy */
	if (*cmd && (AutopilotEffect || u.uprops[AUTOPILOT_EFFECT].extrinsic || have_autopilotstone() || (uarmf && uarmf->oartifact == ART_PRADA_S_DEVIL_WEAR) ) ) {
		int autopilotchance = u.contamination;
		if (isevilvariant && (autopilotchance > 900)) autopilotchance = 900;
		else if (!isevilvariant && (autopilotchance > 500)) autopilotchance = 500;

		if (u.contamination > 600 && !rn2(100)) {
			pline("Client %s sent a bogus command packet.", playeraliasname);
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			if (isevilvariant) dosave();
		}

		if (u.contamination > 1000 && !rn2(100)) {
			pline("Client %s sent an unreadable command packet.", playeraliasname);
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			if (isevilvariant) done(QUIT);
		}

		if (rn2(1000) < autopilotchance) {

			if (rn2(10)) {
				u.dx = !rn2(3) ? -1 : !rn2(2) ? 0 : 1;
				u.dy = !rn2(3) ? -1 : !rn2(2) ? 0 : 1;
				do_walk = TRUE;
				*cmd = ' ';
				goto walkwalkwalk;
			} else { switch (rnd(34)) {
				case 1:
					*cmd = 'a'; break;
				case 2:
					*cmd = 'A'; break;
				case 3:
					*cmd = 'c'; break;
				case 4:
					*cmd = 'd'; break;
				case 5:
					*cmd = 'e'; break;
				case 6:
					*cmd = 'E'; break;
				case 7:
					*cmd = 'f'; break;
				case 8:
					*cmd = 'F'; break;
				case 9:
					*cmd = 'i'; break;
				case 10:
					*cmd = 'j'; break;
				case 11:
					*cmd = 'k'; break;
				case 12:
					*cmd = 'l'; break;
				case 13:
					*cmd = 'o'; break;
				case 14:
					*cmd = 'p'; break;
				case 15:
					*cmd = 'P'; break;
				case 16:
					*cmd = 'q'; break;
				case 17:
					*cmd = 'Q'; break;
				case 18:
					*cmd = 'r'; break;
				case 19:
					*cmd = 'R'; break;
				case 20:
					*cmd = 's'; break;
				case 21:
					*cmd = 't'; break;
				case 22:
					*cmd = 'T'; break;
				case 23:
					*cmd = 'u'; break;
				case 24:
					*cmd = 'v'; break;
				case 25:
					*cmd = 'w'; break;
				case 26:
					*cmd = 'W'; break;
				case 27:
					*cmd = 'x'; break;
				case 28:
					*cmd = '<'; break;
				case 29:
					*cmd = '>'; break;
				case 30:
					*cmd = '.'; break;
				case 31:
					*cmd = ','; break;
				case 32:
					*cmd = ':'; break;
				case 33:
					*cmd = ';'; break;
				}
			}
		}
	}

    if (*cmd && !u.hangupcheat) {
        u.hangupcheat = 1;
    }

	if (*cmd == DOESCAPE) { /* <esc> key - user might be panicking */
		/* Bring up the menu */
		if (multi || !flags.menu_on_esc || !(domenusystem())) {
		flags.move = FALSE;
		    multi = 0;
		}
		return;
#if 0
		flags.move = FALSE;
		return;
#endif
	}
	if (*cmd == DOAGAIN && !in_doagain && saveq[0]) {
		in_doagain = TRUE;
		stail = 0;
		rhack((char *)0);	/* read and execute command */
		in_doagain = FALSE;
		return;
	}
	/* Special case of *cmd == ' ' handled better below */
	if(!*cmd || *cmd == (char)0377) {
		nhbell();
		flags.move = FALSE;
		return;		/* probably we just had an interrupt */
	}
	if (iflags.num_pad && iflags.num_pad_mode == 1) {
		/* This handles very old inconsistent DOS/Windows behaviour
		 * in a new way: earlier, the keyboard handler mapped these,
		 * which caused counts to be strange when entered from the
		 * number pad. Now do not map them until here. 
		 */
		switch (*cmd) {
		    case '5':       *cmd = 'g'; break;
		    case M('5'):    *cmd = 'G'; break;
		    case M('0'):    *cmd = 'I'; break;
        	}
        }
	/* handle most movement commands */
	do_walk = do_rush = prefix_seen = FALSE;
	flags.travel = iflags.travel1 = 0;
	if (*cmd == DORUSH) {

		if (TronEffectIsActive || SpellColorPink) {

			pline("Some sinister force is preventing you from using the rush command.");
			if (FunnyHallu) pline("Could this be the work of Arabella?");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			flags.move = FALSE;
			return;

		}

	    if (movecmd(cmd[1])) {
		flags.run = 2;
		do_rush = TRUE;
	    } else
		prefix_seen = TRUE;
	} else if ( (*cmd == '5' && iflags.num_pad)
		    || *cmd == DORUN) {

		if (TronEffectIsActive || SpellColorPink) {

			pline("Some sinister force is preventing you from using the run command.");
			if (FunnyHallu) pline("Could this be the work of Arabella?");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			flags.move = FALSE;
			return;

		}

	    if (movecmd(lowc(cmd[1]))) {
		flags.run = 3;
		do_rush = TRUE;
	    } else
		prefix_seen = TRUE;
	} else if ( (*cmd == '-' && iflags.num_pad)
		    || *cmd == DOFORCEFIGHT) {
		/* Effects of movement commands and invisible monsters:
		 * m: always move onto space (even if 'I' remembered)
		 * F: always attack space (even if 'I' not remembered)
		 * normal movement: attack if 'I', move otherwise
		 */
	    if (movecmd(cmd[1])) {
		flags.forcefight = 1;
		do_walk = TRUE;
	    } else
		prefix_seen = TRUE;
	} else if (*cmd == DONOPICKUP) {
	    if (movecmd(cmd[1]) || u.dz) {
		flags.run = 0;
		flags.nopick = 1;
		if (!u.dz) do_walk = TRUE;
		else cmd[0] = cmd[1];	/* "m<" or "m>" */
	    } else
		prefix_seen = TRUE;
	} else if (*cmd == DORUN_NOPICKUP) {

		if (TronEffectIsActive || SpellColorPink) {

			pline("Some sinister force is preventing you from using the run-no-pickup command.");
			if (FunnyHallu) pline("Could this be the work of Arabella?");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			flags.move = FALSE;
			return;

		}

	    if (movecmd(lowc(cmd[1]))) {
		flags.run = 1;
		flags.nopick = 1;
		do_rush = TRUE;
	    } else
		prefix_seen = TRUE;
	} else if (*cmd == '0' && iflags.num_pad) {
	    (void)ddoinv(); /* a convenience borrowed from the PC */
	    flags.move = FALSE;
	    multi = 0;
	} else if (*cmd == CMD_TRAVEL && iflags.travelcmd) {
	  flags.travel = 1;
	  iflags.travel1 = 1;
	  flags.run = 8;
	  flags.nopick = 1;
	  do_rush = TRUE;
	} else {
	    if (movecmd(*cmd)) {	/* ordinary movement */
		do_walk = TRUE;
	    } else if (movecmd(iflags.num_pad ?
			       UNMETA(*cmd) : lowc(*cmd))) {

		if (TronEffectIsActive || SpellColorPink) {

			pline("Some sinister force is preventing you from using the meta-run command.");
			if (FunnyHallu) pline("Could this be the work of Arabella?");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			flags.move = FALSE;
			return;

		}

		flags.run = 1;
		do_rush = TRUE;
	    } else if (movecmd(UNCTRL(*cmd))) {

		if (TronEffectIsActive || SpellColorPink) {

			pline("Some sinister force is preventing you from using whatever weird run command this is.");
			if (FunnyHallu) pline("Could this be the work of Arabella?");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			flags.move = FALSE;
			return;

		}

		flags.run = 3;
		do_rush = TRUE;
	    }
	}

	/* some special prefix handling */
	/* overload 'm' prefix for ',' to mean "request a menu" */
	if (prefix_seen && cmd[1] == ',') {
		iflags.menu_requested = TRUE;
		++cmd;
	}

walkwalkwalk:
	if (do_walk) {
	    if (multi) flags.mv = TRUE;
	    domove();
	    flags.forcefight = 0;
	    return;
	} else if (do_rush) {
	    if (firsttime) {
		if (!multi) multi = max(COLNO,ROWNO);
		u.last_str_turn = 0;
	    }
	    flags.mv = TRUE;
	    domove();
	    return;
	} else if (prefix_seen && cmd[1] == DOESCAPE) {	/* <prefix><escape> */
	    /* don't report "unknown command" for change of heart... */
	    bad_command = FALSE;
	} else if (*cmd == ' ' && !flags.rest_on_space) {
	    bad_command = TRUE;		/* skip cmdlist[] loop */
	/* handle bound commands */
	} else {
	    const struct key_tab *keytab = &cmdlist[(unsigned char)*cmd];
	    if (keytab->bind_cmd != NULL) {
		struct ext_func_tab *extcmd = keytab->bind_cmd;
		int res, (*func)(void);		
		if (u.uburied && !extcmd->can_if_buried) {
		    You_cant("do that while you are buried!");
		    res = 0;
		} else {
		    func = extcmd->ef_funct;
		    if (extcmd->f_text && !occupation && multi)
		    set_occupation(func, extcmd->f_text, multi);
		    res = (*func)();		/* perform the command */
		}
		if (!res) {
		    flags.move = FALSE;
		    multi = 0;
		}
		return;
	    }
	    /* if we reach here, cmd wasn't found in cmdlist[] */
	    bad_command = TRUE;
	}
	if (bad_command) {
	    char expcmd[10];
	    register char *cp = expcmd;

	    while (*cmd && (int)(cp - expcmd) < (int)(sizeof expcmd - 3)) {
		if (*cmd >= 040 && *cmd < 0177) {
		    *cp++ = *cmd++;
		} else if (*cmd & 0200) {
		    *cp++ = 'M';
		    *cp++ = '-';
		    *cp++ = *cmd++ &= ~0200;
		} else {
		    *cp++ = '^';
		    *cp++ = *cmd++ ^ 0100;
		}
	    }
	    *cp = '\0';
	    if (!prefix_seen || !iflags.cmdassist ||
		!help_dir(0, "Invalid direction key!"))
	    Norep("Unknown command '%s'.", expcmd);

		/* I've seen SO many newbies get stuck on trying to enable the number pad on the server; sadly, we can't make
		 * it the default, because some players don't have a number pad. On the other hand, not everyone has hjklyubn
		 * keys on their keyboard either :P and therefore it should be possible for them to switch the number pad on;
		 * sadly, few players ever take the in-game advice and join the IRC, so we need in-game advice too...
		 * and even that is often skipped by impatient players :( --Amy */

		/* Elronnd snuck in a commit that adds a way to disable this message, even though I wanted to code that myself.
		 * The option he added will stay, although he apparently didn't know that 'no' is used as a prefix to disable
		 * an option, so the option can't be named 'no_numpad_message' as he originally intended. But I also added a
		 * way to either disable the message in-game or, even better, a prompt to turn the number pad on or off!
		 * Because we really can't expect the average joe to fiddle around with complicated compound options that 99%
		 * of them don't even find because they get stuck looking for it between the "null" and "perm_invent" boolean
		 * options (no joke, I've seen it happen so many times). And even if they do find the number_pad option, they
		 * probably don't know how to change it and end up escaping out of the options menu, which is
		 * never a good idea because it discards all changes you made. So yeah. */

		/* I also made the message display at most 5 times per game to make it not too intrusive --Amy */

		if (moves < 50 && !(iflags.num_pad) && iflags.numpadmessage) {
			if (yn("You might want to turn on the number pad, which is done by opening the options with shift-O and navigating to the number_pad entry (it's not between null and perm_invent, you have to scroll further down to the compound options). Toggle that with the appropriate letter key and hit spacebar (not escape!) until the number_pad dialog comes up, and set it to 2. Alternatively, you can also turn on the number pad by adding this line to your options file: OPTIONS=number_pad:2 (probably requires you to start a new game). (Press y to disable this message)") == 'y') {
				iflags.numpadmessage = FALSE;
				pline("In order to turn the message off for all subsequent games too, add OPTIONS=nonumpadmessage to your configuration file.");
			} else {
				if (yn("Turn the number pad on?") == 'y') {
					iflags.num_pad = 2;
					iflags.num_pad_mode = 1;
					iflags.numpadmessage = FALSE;
				}
			}
			if (u.annoyingmessages++ > 5) iflags.numpadmessage = FALSE;
		}
		else if (moves < 50 && iflags.num_pad && iflags.numpadmessage) {
			if (yn("The number pad is currently on. If for some reason you want to turn it off and use vikeys instead, open the options with shift-O and navigate to the number_pad entry (it's not between null and perm_invent, you have to scroll further down to the compound options). Toggle that with the appropriate letter key and hit spacebar (not escape!) until the number_pad dialog comes up, and set it to 0. Alternatively, you can also turn off the number pad by adding this line to your options file: OPTIONS=number_pad:0 (probably requires you to start a new game). (Press y to disable this message)") == 'y') {
				iflags.numpadmessage = FALSE;
				pline("In order to turn the message off for all subsequent games too, add OPTIONS=nonumpadmessage to your configuration file.");
			} else {
				if (yn("Turn the number pad off?") == 'y') {
					iflags.num_pad = 0;
					iflags.num_pad_mode = 0;
					iflags.numpadmessage = FALSE;
				}
			}
			if (u.annoyingmessages++ > 5) iflags.numpadmessage = FALSE;
		}

	}
	/* didn't move */
	flags.move = FALSE;
	multi = 0;
	return;
}

int
xytod(x, y)	/* convert an x,y pair into a direction code */
schar x, y;
{
	register int dd;

	for(dd = 0; dd < 8; dd++)
	    if(x == xdir[dd] && y == ydir[dd]) return dd;

	return -1;
}

void
dtoxy(cc,dd)	/* convert a direction code into an x,y pair */
coord *cc;
register int dd;
{
	cc->x = xdir[dd];
	cc->y = ydir[dd];
	return;
}

int
movecmd(sym)	/* also sets u.dz, but returns false for <> */
char sym;
{
	register const char *dp;
	register const char *sdp;
	if(iflags.num_pad) sdp = ndir; else sdp = sdir;	/* DICE workaround */

	u.dz = 0;
	if(!(dp = index(sdp, sym))) return 0;
	u.dx = xdir[dp-sdp];
	u.dy = ydir[dp-sdp];
	u.dz = zdir[dp-sdp];

	if (u.totter || u.uprops[TOTTER_EFFECT].extrinsic || TotterTrapEffect || have_directionswapstone() ) {

		u.dx = -u.dx;
		u.dy = -u.dy;

	}

	if (ClockwiseSpinBug || u.uprops[CLOCKWISE_SPIN_BUG].extrinsic || have_clockwisestone() ) {

		if        (u.dx == 1 && !u.dy) {
			u.dy = 1;
		} else if (u.dx == 1 && u.dy == 1) {
			u.dx = 0;
		} else if (!u.dx && u.dy == -1) {
			u.dx = 1;
		} else if (u.dx == -1 && u.dy == -1) {
			u.dx = 0;
		} else if (u.dx == -1 && !u.dy) {
			u.dy = -1;
		} else if (u.dx == -1 && u.dy == 1) {
			u.dy = 0;
		} else if (!u.dx && u.dy == 1) {
			u.dx = -1;
		} else if (u.dx == 1 && u.dy == -1) {
			u.dy = 0;
		}

	}

	if (CounterclockwiseSpin || u.uprops[COUNTERCLOCKWISE_SPIN_BUG].extrinsic || have_counterclockwisestone() ) {

		if        (u.dx == 1 && !u.dy) {
			u.dx = 0; u.dy = -1;
		} else if (u.dx == 1 && u.dy == 1) {
			u.dy = -1;
		} else if (!u.dx && u.dy == -1) {
			u.dx = -1; u.dy = 0;
		} else if (u.dx == -1 && u.dy == -1) {
			u.dy = 1;
		} else if (u.dx == -1 && !u.dy) {
			u.dx = 0; u.dy = 1;
		} else if (u.dx == -1 && u.dy == 1) {
			u.dx = 1;
		} else if (!u.dx && u.dy == 1) {
			u.dx = 1; u.dy = 0;
		} else if (u.dx == 1 && u.dy == -1) {
			u.dx = -1;
		}

	}

	if (SpellColorPink && ((u.pinkspelldirection == 1 && u.dx == 1 && !u.dy) || (u.pinkspelldirection == 2 && u.dx == 1 && u.dy == 1) || (u.pinkspelldirection == 3 && !u.dx && u.dy == -1) || (u.pinkspelldirection == 4 && u.dx == -1 && u.dy == -1) || (u.pinkspelldirection == 5 && u.dx == -1 && !u.dy) || (u.pinkspelldirection == 6 && u.dx == -1 && u.dy == 1) || (u.pinkspelldirection == 7 && !u.dx && u.dy == 1) || (u.pinkspelldirection == 8 && u.dx == 1 && u.dy == -1) ) ) {
		u.dx = u.dy = 0;
		Norep("You can't use that direction!");
		return 0;

	}

	if ((TronEffect || u.uprops[TRON_EFFECT].extrinsic || have_tronstone()) && ((u.trontrapdirection == 1 && u.dx == 1 && !u.dy) || (u.trontrapdirection == 2 && u.dx == 1 && u.dy == 1) || (u.trontrapdirection == 3 && !u.dx && u.dy == -1) || (u.trontrapdirection == 4 && u.dx == -1 && u.dy == -1) || (u.trontrapdirection == 5 && u.dx == -1 && !u.dy) || (u.trontrapdirection == 6 && u.dx == -1 && u.dy == 1) || (u.trontrapdirection == 7 && !u.dx && u.dy == 1) || (u.trontrapdirection == 8 && u.dx == 1 && u.dy == -1) ) ) {
		u.dx = u.dy = 0;
		Norep("You can't use the same direction twice in a row!");
		return 0;

	}

	if (u.dx && u.dy && (BishopGridbug || u.uprops[BISHOP_GRIDBUG].extrinsic || have_bishopstone() || (uarmg && uarmg->oartifact == ART_LINE_CAN_PLAY_BY_YOURSELF) || (uwep && uwep->oartifact == ART_KILLER_PIANO) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_KILLER_PIANO) || isgridbug(youmonst.data) || (Race_if(PM_WEAPON_BUG) && !Upolyd) ) ) {
		u.dx = u.dy = 0;
		return 0;
	}
	return !u.dz;
}

/*
 * uses getdir() but unlike getdir() it specifically
 * produces coordinates using the direction from getdir()
 * and verifies that those coordinates are ok.
 *
 * If the call to getdir() returns 0, Never_mind is displayed.
 * If the resulting coordinates are not okay, emsg is displayed.
 *
 * Returns non-zero if coordinates in cc are valid.
 */
int get_adjacent_loc(prompt,emsg,x,y,cc)
const char *prompt, *emsg;
xchar x,y;
coord *cc;
{
	xchar new_x, new_y;
	if (!getdir(prompt)) {
		pline("%s", Never_mind);
		if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
			pline("Oh wait, actually I do mind...");
			badeffect();
		}
		return 0;
	}
	new_x = x + u.dx;
	new_y = y + u.dy;
	if (cc && isok(new_x,new_y)) {
		cc->x = new_x;
		cc->y = new_y;
	} else {
		if (emsg) pline("%s", emsg);
		return 0;
	}
	return 1;
}

int
getdir(s)
const char *s;
{
	char dirsym;
	/* WAC add dirsymbols to generic prompt */
	char buf[BUFSZ];

	/* choicelessness by Amy; it would be far too evil to unconditionally prevent you from choosing... */
	if (Choicelessness || u.uprops[CHOICELESSNESS].extrinsic || have_choicelessstone()) {
		int choicelesschance;
		choicelesschance = 20 + (u.uen * 80 / u.uenmax);
		if (choicelesschance < 20) choicelesschance = 20;
		if (choicelesschance > 80) choicelesschance = 80;
		if (choicelesschance > rn2(100)) {
			u.dx = u.dy = u.dz = 0;
			confdir();
			return 1;
		}
	}

	sprintf(buf, "In what direction? [%s]",
                (iflags.num_pad ? ndir : sdir));

	if(in_doagain || *readchar_queue)
	    dirsym = readchar();
	else do {
	    dirsym = yn_function ((s && *s != '^') ? s : buf, (char *)0, '\0');
	} while (!movecmd(dirsym) && !index(quitchars, dirsym)
                && dirsym == '.' && dirsym == 's' && !u.dz);
	savech(dirsym);
	if(dirsym == '.' || dirsym == 's')
		u.dx = u.dy = u.dz = 0;
	else if(!movecmd(dirsym) && !u.dz) {
		boolean did_help = FALSE;
		if(!index(quitchars, dirsym)) {
		    if (iflags.cmdassist) {
			did_help = help_dir((s && *s == '^') ? dirsym : 0,
					    "Invalid direction key!");
		    }
		    if (!did_help) pline("What a strange direction!");
		}
		return 0;
	}
	if ((Stunned && !rn2(issoviet ? 1 : StrongStun_resist ? 20 : Stun_resist ? 8 : 2)) || (Confusion && !rn2(issoviet ? 2 : StrongConf_resist ? 200 : Conf_resist ? 40 : 8))) confdir();
	return 1;
}

STATIC_OVL boolean
help_dir(sym, msg)
char sym;
const char *msg;
{
	char ctrl;
	winid win;
	static const char wiz_only_list[] = "EFGIOVW";
	char buf[BUFSZ], buf2[BUFSZ], *expln;

	win = create_nhwindow(NHW_TEXT);
	if (!win) return FALSE;
	if (msg) {
		sprintf(buf, "cmdassist: %s", msg);
		putstr(win, 0, buf);
		putstr(win, 0, "");
	}
	if (letter(sym)) { 
	    sym = highc(sym);
	    ctrl = (sym - 'A') + 1;
	    if ((expln = dowhatdoes_core(ctrl, buf2))
		&& (!index(wiz_only_list, sym)
#ifdef WIZARD
		    || wizard
#endif
	                     )) {
		sprintf(buf, "Are you trying to use ^%c%s?", sym,
			index(wiz_only_list, sym) ? "" :
			" as specified in the Guidebook");
		putstr(win, 0, buf);
		putstr(win, 0, "");
		putstr(win, 0, expln);
		putstr(win, 0, "");
		putstr(win, 0, "To use that command, you press");
		sprintf(buf,
			"the <Ctrl> key, and the <%c> key at the same time.", sym);
		putstr(win, 0, buf);
		putstr(win, 0, "");
	    }
	}
	if (iflags.num_pad && (BishopGridbug || u.uprops[BISHOP_GRIDBUG].extrinsic || have_bishopstone() || (uarmg && uarmg->oartifact == ART_LINE_CAN_PLAY_BY_YOURSELF) || (uwep && uwep->oartifact == ART_KILLER_PIANO) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_KILLER_PIANO) || isgridbug(youmonst.data) || (Race_if(PM_WEAPON_BUG) && !Upolyd))) {
	    putstr(win, 0, "Valid direction keys in your current form (with number_pad on) are:");
	    putstr(win, 0, "             8   ");
	    putstr(win, 0, "             |   ");
	    putstr(win, 0, "          4- . -6");
	    putstr(win, 0, "             |   ");
	    putstr(win, 0, "             2   ");
	} else if (BishopGridbug || u.uprops[BISHOP_GRIDBUG].extrinsic || have_bishopstone() || (uarmg && uarmg->oartifact == ART_LINE_CAN_PLAY_BY_YOURSELF) || (uwep && uwep->oartifact == ART_KILLER_PIANO) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_KILLER_PIANO) || isgridbug(youmonst.data) || (Race_if(PM_WEAPON_BUG) && !Upolyd)) {
	    putstr(win, 0, "Valid direction keys in your current form are:");
	    putstr(win, 0, "             k   ");
	    putstr(win, 0, "             |   ");
	    putstr(win, 0, "          h- . -l");
	    putstr(win, 0, "             |   ");
	    putstr(win, 0, "             j   ");
	} else if (iflags.num_pad) {
	    putstr(win, 0, "Valid direction keys (with number_pad on) are:");
	    putstr(win, 0, "          7  8  9");
	    putstr(win, 0, "           \\ | / ");
	    putstr(win, 0, "          4- . -6");
	    putstr(win, 0, "           / | \\ ");
	    putstr(win, 0, "          1  2  3");
	} else {
	    putstr(win, 0, "Valid direction keys are:");
	    putstr(win, 0, "          y  k  u");
	    putstr(win, 0, "           \\ | / ");
	    putstr(win, 0, "          h- . -l");
	    putstr(win, 0, "           / | \\ ");
	    putstr(win, 0, "          b  j  n");
	};
	putstr(win, 0, "");
	putstr(win, 0, "          <  up");
	putstr(win, 0, "          >  down");
	putstr(win, 0, "          .  direct at yourself");
	putstr(win, 0, "");
	putstr(win, 0, "(Suppress this message with !cmdassist in config file.)");
	putstr(win, 0, "Hint: Shift-O opens the options. To change between");
	putstr(win, 0, "vi-like keys and the number pad, use the number_pad option.");
	putstr(win, 0, "");
	putstr(win, 0, "Add OPTIONS=number_pad:2 to your options file to turn the number pad on.");
	putstr(win, 0, "Add OPTIONS=number_pad:0 to your options file to turn the number pad off.");
	putstr(win, 0, "(Editing the options file might not take effect until you start a new game)");
	display_nhwindow(win, FALSE);
	destroy_nhwindow(win);
	return TRUE;
}

#endif /* OVL1 */
#ifdef OVLB

void
confdir()
{
	register int x = (BishopGridbug || u.uprops[BISHOP_GRIDBUG].extrinsic || have_bishopstone() || (uarmg && uarmg->oartifact == ART_LINE_CAN_PLAY_BY_YOURSELF) || (uwep && uwep->oartifact == ART_KILLER_PIANO) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_KILLER_PIANO) || isgridbug(youmonst.data) || (Race_if(PM_WEAPON_BUG) && !Upolyd)) ? 2*rn2(4) : rn2(8);
	u.dx = xdir[x];
	u.dy = ydir[x];
	return;
}

#endif /* OVLB */
#ifdef OVL0

int
isok(x,y)
register int x, y;
{
	/* x corresponds to curx, so x==1 is the first column. Ach. %% */
	return x >= 1 && x <= COLNO-1 && y >= 0 && y <= ROWNO-1;
}

static NEARDATA int last_multi;

/*
 * convert a MAP window position into a movecmd
 */
const char *
click_to_cmd(x, y, mod)
    int x, y, mod;
{
    int dir;
    static char cmd[4];
    cmd[1]=0;

	if (!youmonst.data) goto quaakskip;

	if (MenuIsBugged) {
	pline("The travel command is currently unavailable!");
	if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	cmd[0] = ' ';
	return cmd;
	}

	if (uarmg && itemhasappearance(uarmg, APP_RACER_GLOVES) ) {
	pline("Your gloves prevent quicktravel!");
	if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	cmd[0] = ' ';
	return cmd;
	}

	if (u.uprops[TOTTER_EFFECT].extrinsic || TotterTrapEffect || have_directionswapstone() || u.totter || ClockwiseSpinBug || u.uprops[CLOCKWISE_SPIN_BUG].extrinsic || have_clockwisestone() || CounterclockwiseSpin || u.uprops[COUNTERCLOCKWISE_SPIN_BUG].extrinsic || have_counterclockwisestone() || TronEffect || u.uprops[TRON_EFFECT].extrinsic || have_tronstone() || SpellColorPink) {
		pline("A sinister force prevents you from quicktraveling!");
		if (FunnyHallu) pline("Could this be the work of Arabella?");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		cmd[0] = ' ';
		return cmd;

	}

	if (BishopGridbug || u.uprops[BISHOP_GRIDBUG].extrinsic || have_bishopstone() || (uarmg && uarmg->oartifact == ART_LINE_CAN_PLAY_BY_YOURSELF) || (uwep && uwep->oartifact == ART_KILLER_PIANO) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_KILLER_PIANO) || isgridbug(youmonst.data) || (Race_if(PM_WEAPON_BUG) && !Upolyd)) {

	pline("Due to your movement restriction, you cannot quicktravel!");
	if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	cmd[0] = ' ';
	return cmd;

	}

quaakskip:

    x -= u.ux;
    y -= u.uy;

    if (iflags.travelcmd) {
        if (abs(x) <= 1 && abs(y) <= 1 ) {
	x = sgn(x), y = sgn(y);
    } else {
	u.tx = u.ux+x;
	u.ty = u.uy+y;
	cmd[0] = CMD_TRAVEL;
	return cmd;
    }

    if(x == 0 && y == 0) {
	/* here */
	if(IS_FOUNTAIN(levl[u.ux][u.uy].typ) || IS_SINK(levl[u.ux][u.uy].typ)) {
	    cmd[0]=mod == CLICK_1 ? 'q' : M('d');
	    return cmd;
	} else if(IS_THRONE(levl[u.ux][u.uy].typ)) {
	    cmd[0]=M('s');
	    return cmd;
	} else if((u.ux == xupstair && u.uy == yupstair)
		  || (u.ux == sstairs.sx && u.uy == sstairs.sy && sstairs.up)
		  || (u.ux == xupladder && u.uy == yupladder)) {
	    return "<";
	} else if((u.ux == xdnstair && u.uy == ydnstair)
		  || (u.ux == sstairs.sx && u.uy == sstairs.sy && !sstairs.up)
		  || (u.ux == xdnladder && u.uy == ydnladder)) {
	    return ">";
	} else if(OBJ_AT(u.ux, u.uy)) {
	    cmd[0] = Is_container(level.objects[u.ux][u.uy]) ? M('l') : ',';
	    return cmd;
	} else {
	    return "."; /* just rest */
	}
    }

    /* directional commands */

    dir = xytod(x, y);

	if (!m_at(u.ux+x, u.uy+y) && !test_move(u.ux, u.uy, x, y, TEST_MOVE)) {
	cmd[1] = (iflags.num_pad ? ndir[dir] : sdir[dir]);
	cmd[2] = 0;
	if (IS_DOOR(levl[u.ux+x][u.uy+y].typ)) {
	    /* slight assistance to the player: choose kick/open for them */
	    if (levl[u.ux+x][u.uy+y].doormask & D_LOCKED) {
		cmd[0] = C('d');
		return cmd;
	    }
	    if (levl[u.ux+x][u.uy+y].doormask & D_CLOSED) {
		cmd[0] = 'o';
		return cmd;
	    }
	}
	if (levl[u.ux+x][u.uy+y].typ <= SCORR) {
	    cmd[0] = 's';
	    cmd[1] = 0;
	    return cmd;
	}
    }
    } else {
        /* convert without using floating point, allowing sloppy clicking */
        if(x > 2*abs(y))
            x = 1, y = 0;
        else if(y > 2*abs(x))
            x = 0, y = 1;
        else if(x < -2*abs(y))
            x = -1, y = 0;
        else if(y < -2*abs(x))
            x = 0, y = -1;
        else
            x = sgn(x), y = sgn(y);

        if(x == 0 && y == 0)	/* map click on player to "rest" command */
            return ".";

        dir = xytod(x, y);
    }

    /* move, attack, etc. */
    cmd[1] = 0;
    if(mod == CLICK_1) {
	cmd[0] = (iflags.num_pad ? ndir[dir] : sdir[dir]);
    } else {
	cmd[0] = (iflags.num_pad ? M(ndir[dir]) :
		(sdir[dir] - 'a' + 'A')); /* run command */
    }

    return cmd;
}

STATIC_OVL char *
parse()
{
#ifdef LINT	/* static char in_line[COLNO]; */
	char in_line[COLNO];
#else
	static char in_line[COLNO];
#endif
	register int foo;

	static char repeat_char;
	boolean prezero = FALSE;

	multi = 0;
	flags.move = 1;
	flush_screen(1); /* Flush screen buffer. Put the cursor on the hero. */

#ifdef BORG
	if (borg_on) {
	// TODO: implement kbhit for other windowports --ELR
	   if (!kbhit()) {
	       borg_input();
	       return(borg_line);
	   } else {
		 nhgetch();
		 pline("Cyborg terminated.");
		 borg_on = 0;
	   }

	} else 
#endif
	/* [Tom] for those who occasionally go insane... */
	if (repeat_hit) {
		/* Sanity checks for repeat_hit */
		if (repeat_hit < 0) repeat_hit = 0;
		else {
			/* Don't want things to get too out of hand */
			if (repeat_hit > 10) repeat_hit = 10;
			
			repeat_hit--;
			in_line[0] = repeat_char;
			in_line[1] = 0;
			return (in_line);
		}
	}

	if (!iflags.num_pad || (foo = readchar()) == 'n')
	    for (;;) {
		foo = readchar();
		if (foo >= '0' && foo <= '9') {
		    multi = 10 * multi + foo - '0';
		    if (multi < 0 || multi >= LARGEST_INT) multi = LARGEST_INT;
		    if (multi > 9) {
			clear_nhwindow(WIN_MESSAGE);
			sprintf(in_line, "Count: %d", multi);
			pline("%s", in_line);
			mark_synch();
		    }
		    last_multi = multi;
		    if (!multi && foo == '0') prezero = TRUE;
		} else break;	/* not a digit */
	    }

	if (foo == DOESCAPE) {   /* esc cancels count (TH) */
	    clear_nhwindow(WIN_MESSAGE);
	    /* multi = */ last_multi = 0;  /* WAC multi is cleared later in rhack */
	} else if (foo == DOAGAIN || in_doagain) {
	    multi = last_multi;
	} else {
	    last_multi = multi;
	    savech(0);	/* reset input queue */
	    savech((char)foo);
	}

	if (multi) {
	    multi--;
	    save_cm = in_line;
	} else {
	    save_cm = (char *)0;
	}
	in_line[0] = foo;
	in_line[1] = '\0';

	if (foo == DORUSH || foo == DORUN || foo == DOFORCEFIGHT
	    || foo == DONOPICKUP || foo == DORUN_NOPICKUP
	    || (iflags.num_pad && (foo == '5' || foo == '-'))) {
	    foo = readchar();
	    savech((char)foo);
	    in_line[1] = foo;
	    in_line[2] = 0;
	}
	clear_nhwindow(WIN_MESSAGE);

	if (prezero) in_line[0] = DOESCAPE;
	repeat_char = in_line[0];
	
	return(in_line);
}

#endif /* OVL0 */
#ifdef OVLB

#ifdef UNIX
static
void
end_of_input()
{
#ifndef NOSAVEONHANGUP
	if (!program_state.done_hup++ && program_state.something_worth_saving)
	    (void) dosave0();
#endif
	exit_nhwindows((char *)0);
	clearlocks();
	terminate(EXIT_SUCCESS);
}
#endif

#endif /* OVLB */
#ifdef OVL0

char
readchar()
{
	register int sym;
	int x = u.ux, y = u.uy, mod = 0;

	if ( *readchar_queue )
	    sym = *readchar_queue++;
	else
	    sym = in_doagain ? Getchar() : nh_poskey(&x, &y, &mod);

#ifdef UNIX
# ifdef NR_OF_EOFS
	if (sym == EOF) {
	    register int cnt = NR_OF_EOFS;
	  /*
	   * Some SYSV systems seem to return EOFs for various reasons
	   * (?like when one hits break or for interrupted systemcalls?),
	   * and we must see several before we quit.
	   */
	    do {
		clearerr(stdin);	/* omit if clearerr is undefined */
		sym = Getchar();
	    } while (--cnt && sym == EOF);
	}
# endif /* NR_OF_EOFS */
	if (sym == EOF)
	    end_of_input();
#endif /* UNIX */

	if(sym == 0) {
	    /* click event */
	    readchar_queue = click_to_cmd(x, y, mod);
	    sym = *readchar_queue++;
	}
	return((char) sym);
}

STATIC_PTR int
dotravel()
{

	if (MenuIsBugged) {
	pline("The travel command is currently unavailable!");
	if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return 0;
	}

	if (BishopGridbug || u.uprops[BISHOP_GRIDBUG].extrinsic || have_bishopstone() || (uarmg && uarmg->oartifact == ART_LINE_CAN_PLAY_BY_YOURSELF) || (uwep && uwep->oartifact == ART_KILLER_PIANO) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_KILLER_PIANO) || isgridbug(youmonst.data) || (Race_if(PM_WEAPON_BUG) && !Upolyd)) {

	/* CURSE that satanic travelto function. I pulled the nethack 3.6.0 fix that disallows quicktravelling to an
	 * adjacent diagonal square in one turn, but it does not prevent moving through diagonal openings in the
	 * quicktravel path! And I don't know where in the function it does that. So until I find out how to really fix this,
	 * I'll just disable quicktravel for grid bugs altogether. If you are annoyed by this: don't waste your time
	 * complaining, but provide the piece of code that fixes it! Then I will fix and re-enable it. --Amy */

	pline("Due to your movement restriction, you cannot quicktravel!");
	if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return 0;

	}

	if (u.uprops[TOTTER_EFFECT].extrinsic || TotterTrapEffect || have_directionswapstone() || u.totter || ClockwiseSpinBug || u.uprops[CLOCKWISE_SPIN_BUG].extrinsic || have_clockwisestone() || CounterclockwiseSpin || u.uprops[COUNTERCLOCKWISE_SPIN_BUG].extrinsic || have_counterclockwisestone() || TronEffect || u.uprops[TRON_EFFECT].extrinsic || have_tronstone() || SpellColorPink) {
		pline("A sinister force prevents you from quicktraveling!");
		if (FunnyHallu) pline("Could this be the work of Arabella?");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;

	}

	if (uarmg && itemhasappearance(uarmg, APP_RACER_GLOVES) ) {
	pline("Your gloves prevent quicktravel!");
	if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return 0;
	}

	/* Keyboard travel command */
	static char cmd[2];
	coord cc;

	if (!iflags.travelcmd) return 0;
	cmd[1]=0;
	cc.x = iflags.travelcc.x;
	cc.y = iflags.travelcc.y;
	if (cc.x == -1 && cc.y == -1) {
	    /* No cached destination, start attempt from current position */
	    cc.x = u.ux;
	    cc.y = u.uy;
	}
	pline("Where do you want to travel to?");
	if (getpos(&cc, TRUE, "the desired destination") < 0) {
		/* user pressed ESC */
		return 0;
	}
	iflags.travelcc.x = u.tx = cc.x;
	iflags.travelcc.y = u.ty = cc.y;
	cmd[0] = CMD_TRAVEL;
	readchar_queue = cmd;
	return 0;
}

#ifdef PORT_DEBUG
# ifdef WIN32CON
extern void win32con_debug_keystrokes(void);
extern void win32con_handler_info(void);
# endif

int
wiz_port_debug()
{
	int n, k;
	winid win;
	anything any;
	int item = 'a';
	int num_menu_selections;
	struct menu_selection_struct {
		char *menutext;
		char *portname;
		void (*fn)(void);
	} menu_selections[] = {
#ifdef WIN32CON
		{"test win32 keystrokes", "tty", win32con_debug_keystrokes},
		{"show keystroke handler information", "tty",
				win32con_handler_info},
#endif
		{(char *)0, (char *)0, (void (*))0}/* array terminator */
	};

	num_menu_selections = SIZE(menu_selections) - 1;
	for (k=n=0; k < num_menu_selections; ++k)
		if (!strcmp(menu_selections[k].portname, windowprocs.name))
			n++;
	if (n > 0) {
		menu_item *pick_list;
		win = create_nhwindow(NHW_MENU);
		start_menu(win);
		for (k=0; k < num_menu_selections; ++k) {
			if (strcmp(menu_selections[k].portname,
				   windowprocs.name))
				continue;
			any.a_int = k+1;
			add_menu(win, NO_GLYPH, &any, item++, 0, ATR_NONE,
				menu_selections[k].menutext, MENU_UNSELECTED);
		}
		end_menu(win, "Which port debugging feature?");
		n = select_menu(win, PICK_ONE, &pick_list);
		destroy_nhwindow(win);
		if (n > 0) {
			n = pick_list[0].item.a_int - 1;
			free((void *) pick_list);
			/* execute the function */
			(*menu_selections[n].fn)();
		}
	} else
		pline("No port-specific debug capability defined.");
	return 0;
}
# endif /*PORT_DEBUG*/

#endif /* OVL0 */
#ifdef OVLB
/*
 *   Parameter validator for generic yes/no function to prevent
 *   the core from sending too long a prompt string to the
 *   window port causing a buffer overflow there.
 */
char
yn_function(query,resp, def)
const char *query,*resp;
char def;
{
	char qbuf[QBUFSZ];
	unsigned truncspot, reduction = sizeof(" [N]  ?") + 1;

	if (resp) reduction += strlen(resp) + sizeof(" () ");
	if (strlen(query) < (QBUFSZ - reduction))
		return (*windowprocs.win_yn_function)(query, resp, def);
	paniclog("Query truncated: ", query);
	reduction += sizeof("...");
	truncspot = QBUFSZ - reduction;
	(void) strncpy(qbuf, query, (int)truncspot);
	qbuf[truncspot] = '\0';
	strcat(qbuf,"...");
	return (*windowprocs.win_yn_function)(qbuf, resp, def);
}
#endif

STATIC_PTR int
stefanjerepair()
{
	if (delay) {
		delay++;
		return(1);
	} else {
		if (uarmf && uarmf->oartifact == ART_STEFANJE_S_PROBLEM && (uarmf->spe < 0)) {
			uarmf->spe++;
			pline("Your 'Stefanje' sandals are surrounded by a cyan glow.");
		} else {
			pline("Somehow, your 'Stefanje' sandals are no longer there...");
		}
		return(0);
	}
}

/*cmd.c*/
