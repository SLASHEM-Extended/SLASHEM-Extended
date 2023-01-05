/*	SCCS Id: @(#)dungeon.c	3.4	1999/10/30	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "dgn_file.h"
#include "dlb.h"

#ifdef OVL1

#define DUNGEON_AREA    FILE_AREA_UNSHARE
#define DUNGEON_FILE	"dungeon"
#define DUNGEON_FILE2	"dungeon2"
#define DUNGEON_FILE3	"dungeon3"
#define DUNGEON_FILE4	"dungeon4"
#define DUNGEON_FILE5	"dungeon5"
#define DUNGEON_FILE6	"dungeon6"
#define DUNGEON_FILE7	"dungeon7"
#define DUNGEON_FILE8	"dungeon8"

#define X_START		"x-strt"
#define X_LOCATE	"x-loca"
#define X_GOAL		"x-goal"
#define X_XTONE		"x-1"
#define X_XTTWO		"x-2"
#define X_XTTHREE		"x-3"
#define X_XTFOUR		"x-4"
#define X_XTFIVE		"x-5"
#define X_XTSIX		"x-6"
#define X_XTSEVEN		"x-7"
#define X_XTEIGHT		"x-8"
#define X_XTNINE		"x-9"
#define X_XTTEN		"x-0"
#define Y_YTSIX		"y-6"
#define Y_FILA		"y-fila"
#define Y_LOCA		"y-loca"
#define Y_FILB		"y-filb"
#define Y_GOAL		"y-goal"

struct proto_dungeon {
	struct	tmpdungeon tmpdungeon[MAXDUNGEON];
	struct	tmplevel   tmplevel[LEV_LIMIT];
	s_level *final_lev[LEV_LIMIT];	/* corresponding level pointers */
	struct	tmpbranch  tmpbranch[BRANCH_LIMIT];
	int	tmpparent[BRANCH_LIMIT];

	int	start;	/* starting index of current dungeon sp levels */
	int	n_levs;	/* number of tmplevel entries */
	int	n_brs;	/* number of tmpbranch entries */
};

int n_dgns;				/* number of dungeons (used here,  */
					/*   and mklev.c)		   */
static branch *branches = (branch *) 0;	/* dungeon branch list		   */

struct lchoice {
	int idx;
	schar lev[MAXLINFO];
	schar playerlev[MAXLINFO];
	xchar dgn[MAXLINFO];
	char menuletter;
};

static void Fread(void *, int, int, dlb *);
STATIC_DCL int find_branch(const char *, struct proto_dungeon *);
STATIC_DCL int level_range(XCHAR_P,int,int,int,struct proto_dungeon *,int *);
STATIC_DCL xchar parent_dlevel(int, struct proto_dungeon *);
STATIC_DCL int correct_branch_type(struct tmpbranch *);
STATIC_DCL branch *add_branch(int, int, struct proto_dungeon *);
STATIC_DCL void add_level(s_level *);
STATIC_DCL void init_level(int,int,struct proto_dungeon *);
STATIC_DCL int possible_places(int, boolean *, struct proto_dungeon *);
STATIC_DCL xchar pick_level(boolean *, int);
STATIC_DCL boolean place_level(int, struct proto_dungeon *);
#ifdef WIZARD
STATIC_DCL const char *br_string(int);
STATIC_DCL void print_branch(winid, int, int, int, BOOLEAN_P, struct lchoice *);
#endif

mapseen *mapseenchn = (struct mapseen *)0;
STATIC_DCL mapseen *load_mapseen(int);
STATIC_DCL void save_mapseen(int, mapseen *);
STATIC_DCL mapseen *find_mapseen(d_level *);
STATIC_DCL void print_mapseen(winid,mapseen *,boolean,boolean,boolean);
STATIC_DCL boolean interest_mapseen(mapseen *);

#if defined(DEBUG) || defined(DEBUG_420942)
#define DD	dungeons[i]
STATIC_DCL void dumpit(void);

STATIC_OVL void
dumpit()
{
	int	i;
	s_level	*x;
	branch *br;

	for(i = 0; i < n_dgns; i++)  {
	    fprintf(stderr, "\n#%d \"%s\" (%s):\n", i,
				DD.dname, DD.proto);
	    fprintf(stderr, "    num_dunlevs %d, dunlev_ureached %d\n",
				DD.num_dunlevs, DD.dunlev_ureached);
	    fprintf(stderr, "    depth_start %d, ledger_start %d\n",
				DD.depth_start, DD.ledger_start);
	    fprintf(stderr, "    flags:%s%s%s\n",
		    DD.flags.rogue_like ? " rogue_like" : "",
		    DD.flags.maze_like  ? " maze_like"  : "",
		    DD.flags.hellish    ? " hellish"    : "");
	    getchar();
	}
	fprintf(stderr,"%s","\nSpecial levels:\n");
	for(x = sp_levchn; x; x = x->next) {
	    fprintf(stderr, "%s (%d): ", x->proto, x->rndlevs);
	    fprintf(stderr, "on %d, %d; ", x->dlevel.dnum, x->dlevel.dlevel);
	    fprintf(stderr, "flags:%s%s%s%s\n",
		    x->flags.rogue_like	? " rogue_like" : "",
		    x->flags.maze_like  ? " maze_like"  : "",
		    x->flags.hellish    ? " hellish"    : "",
		    x->flags.town       ? " town"       : "");
	    getchar();
	}
	fprintf(stderr,"%s","\nBranches:\n");
	for (br = branches; br; br = br->next) {
	    fprintf(stderr, "%d: %s, end1 %d %d, end2 %d %d, %s\n",
		br->id,
		br->type == BR_STAIR ? "stair" :
		    br->type == BR_NO_END1 ? "no end1" :
		    br->type == BR_NO_END2 ? "no end2" :
		    br->type == BR_PORTAL  ? "portal"  :
					     "unknown",
		br->end1.dnum, br->end1.dlevel,
		br->end2.dnum, br->end2.dlevel,
		br->end1_up ? "end1 up" : "end1 down");
	}
	getchar();
	fprintf(stderr,"%s","\nDone\n");
	getchar();
}
#endif

/* Save the dungeon structures. */
void
save_dungeon(fd, perform_write, free_data)
    int fd;
    boolean perform_write, free_data;
{
    branch *curr, *next;
    mapseen *curr_ms, *next_ms;
    int    count;

    if (perform_write) {
	bwrite(fd, (void *) &n_dgns, sizeof n_dgns);
	bwrite(fd, (void *) dungeons, sizeof(dungeon) * (unsigned)n_dgns);
	bwrite(fd, (void *) &dungeon_topology, sizeof dungeon_topology);
	bwrite(fd, (void *) tune, sizeof tune);

	for (count = 0, curr = branches; curr; curr = curr->next)
	    count++;
	bwrite(fd, (void *) &count, sizeof(count));

	for (curr = branches; curr; curr = curr->next)
	    bwrite(fd, (void *) curr, sizeof (branch));

	count = maxledgerno();
	bwrite(fd, (void *) &count, sizeof count);
	bwrite(fd, (void *) level_info,
			(unsigned)count * sizeof (struct linfo));
	bwrite(fd, (void *) &inv_pos, sizeof inv_pos);

    for (count = 0, curr_ms = mapseenchn; curr_ms; curr_ms = curr_ms->next)
        count++;
    bwrite(fd, (void *) &count, sizeof(count));

    for (curr_ms = mapseenchn; curr_ms; curr_ms = curr_ms->next)
        save_mapseen(fd, curr_ms);

    }

    if (free_data) {
	for (curr = branches; curr; curr = next) {
	    next = curr->next;
	    free((void *) curr);
	}
	branches = 0;
    for (curr_ms = mapseenchn; curr_ms; curr_ms = next_ms) {
        next_ms = curr_ms->next;
        if (curr_ms->custom)
            free((void *)curr_ms->custom);
        free((void *) curr_ms);
    }
    mapseenchn = 0;
    }
}

/* Restore the dungeon structures. */
void
restore_dungeon(fd)
    int fd;
{
    branch *curr, *last;
    mapseen *curr_ms, *last_ms;
    int    count, i;

    mread(fd, (void *) &n_dgns, sizeof(n_dgns));
    mread(fd, (void *) dungeons, sizeof(dungeon) * (unsigned)n_dgns);
    mread(fd, (void *) &dungeon_topology, sizeof dungeon_topology);
    mread(fd, (void *) tune, sizeof tune);

    last = branches = (branch *) 0;

    mread(fd, (void *) &count, sizeof(count));
    for (i = 0; i < count; i++) {
	curr = (branch *) alloc(sizeof(branch));
	mread(fd, (void *) curr, sizeof(branch));
	curr->next = (branch *) 0;
	if (last)
	    last->next = curr;
	else
	    branches = curr;
	last = curr;
    }

    mread(fd, (void *) &count, sizeof(count));
    if (count >= MAXLINFO)
	panic("level information count larger (%d) than allocated size", count);
    mread(fd, (void *) level_info, (unsigned)count*sizeof(struct linfo));
    mread(fd, (void *) &inv_pos, sizeof inv_pos);

    mread(fd, (void *) &count, sizeof(count));
    last_ms = (mapseen *) 0;
    for (i = 0; i < count; i++) {
        curr_ms = load_mapseen(fd);
        curr_ms->next = (mapseen *) 0;
        if (last_ms)
            last_ms->next = curr_ms;
        else
            mapseenchn = curr_ms;
        last_ms = curr_ms;
    }
}

static void
Fread(ptr, size, nitems, stream)
	void *	ptr;
	int	size, nitems;
	dlb	*stream;
{
	int cnt;

	if((cnt = dlb_fread(ptr, size, nitems, stream)) != nitems) {
	    panic(
 "Premature EOF on dungeon description file!\r\nExpected %d bytes - got %d.",
		  (size * nitems), (size * cnt));
	    terminate(EXIT_FAILURE);
	}
}

xchar
dname_to_dnum(s)
const char	*s;
{
	xchar	i;

	for (i = 0; i < n_dgns; i++)
	    if (!strcmp(dungeons[i].dname, s)) return i;

	panic("Couldn't resolve dungeon number for name \"%s\".", s);
	/*NOT REACHED*/
	return (xchar)0;
}

s_level *
find_level(s)
	const char *s;
{
	s_level *curr;
	for(curr = sp_levchn; curr; curr = curr->next)
	    if (!strcmpi(s, curr->proto)) break;
	return curr;
}

/* Find the branch that links the named dungeon. */
STATIC_OVL int
find_branch(s, pd)
	const char *s;		/* dungeon name */
	struct proto_dungeon *pd;
{
	int i;

	if (pd) {
	    for (i = 0; i < pd->n_brs; i++)
		if (!strcmp(pd->tmpbranch[i].name, s)) break;
	    if (i == pd->n_brs) panic("find_branch: can't find %s", s);
	} else {
	    /* support for level tport by name */
	    branch *br;
	    const char *dnam;

	    for (br = branches; br; br = br->next) {
		dnam = dungeons[br->end2.dnum].dname;
		if (!strcmpi(dnam, s) ||
			(!strncmpi(dnam, "The ", 4) && !strcmpi(dnam + 4, s)))
		    break;
	    }
	    i = br ? ((ledger_no(&br->end1) << 8) | ledger_no(&br->end2)) : -1;
	}
	return i;
}

/*
 * Return a starting point and number of successive positions a level
 * or dungeon entrance can occupy.
 *
 * Note: This follows the acouple (instead of the rcouple) rules for a
 *	 negative random component (rand < 0).  These rules are found
 *	 in dgn_comp.y.  The acouple [absolute couple] section says that
 *	 a negative random component means from the (adjusted) base to the
 *	 end of the dungeon.
 */
STATIC_OVL int
level_range(dgn, base, rand, chain, pd, adjusted_base)
	xchar	dgn;
	int	base, rand, chain;
	struct proto_dungeon *pd;
	int *adjusted_base;
{
	int lmax = dungeons[dgn].num_dunlevs;

	if (chain >= 0) {		 /* relative to a special level */
	    s_level *levtmp = pd->final_lev[chain];
	    if (!levtmp) panic("level_range: empty chain level!");

	    base += levtmp->dlevel.dlevel;
	} else {			/* absolute in the dungeon */
	    /* from end of dungeon */
	    if (base < 0) base = (lmax + base + 1);
	}

	if (base < 1 || base > lmax)
	    panic("level_range: base value out of range");

	*adjusted_base = base;

	if (rand == -1) {	/* from base to end of dungeon */
	    return (lmax - base + 1);
	} else if (rand) {
	    /* make sure we don't run off the end of the dungeon */
	    return (((base + rand - 1) > lmax) ? lmax-base+1 : rand);
	} /* else only one choice */
	return 1;
}

STATIC_OVL xchar
parent_dlevel(i, pd)
	int i;
	struct proto_dungeon *pd;
{
	int j, num, base, dnum = pd->tmpparent[i];
	branch *curr;

	num = level_range(dnum, pd->tmpbranch[i].lev.base,
					      pd->tmpbranch[i].lev.rand,
					      pd->tmpbranch[i].chain,
					      pd, &base);

	/* KMH -- Try our best to find a level without an existing branch */
	i = j = rn2(num);
	do {
		if (++i >= num) i = 0;
		for (curr = branches; curr; curr = curr->next)
			if ((curr->end1.dnum == dnum && curr->end1.dlevel == base+i) ||
				(curr->end2.dnum == dnum && curr->end2.dlevel == base+i))
				break;
	} while (curr && i != j);
	return (base + i);
}

/* Convert from the temporary branch type to the dungeon branch type. */
STATIC_OVL int
correct_branch_type(tbr)
    struct tmpbranch *tbr;
{
    switch (tbr->type) {
	case TBR_STAIR:		return BR_STAIR;
	case TBR_NO_UP:		return tbr->up ? BR_NO_END1 : BR_NO_END2;
	case TBR_NO_DOWN:	return tbr->up ? BR_NO_END2 : BR_NO_END1;
	case TBR_PORTAL:	return BR_PORTAL;
    }
    impossible("correct_branch_type: unknown branch type");
    return BR_STAIR;
}

/*
 * Add the given branch to the branch list.  The branch list is ordered
 * by end1 dungeon and level followed by end2 dungeon and level.  If
 * extract_first is true, then the branch is already part of the list
 * but needs to be repositioned.
 */
void
insert_branch(new_branch, extract_first)
   branch *new_branch;
   boolean extract_first;
{
    branch *curr, *prev;
    long new_val, curr_val, prev_val;

    if (extract_first) {
	for (prev = 0, curr = branches; curr; prev = curr, curr = curr->next)
	    if (curr == new_branch) break;

	if (!curr) panic("insert_branch: not found");
	if (prev)
	    prev->next = curr->next;
	else
	    branches = curr->next;
    }
    new_branch->next = (branch *) 0;

/* Convert the branch into a unique number so we can sort them. */
#define branch_val(bp) \
	((((long)(bp)->end1.dnum * (MAXLEVEL+1) + \
	  (long)(bp)->end1.dlevel) * (MAXDUNGEON+1) * (MAXLEVEL+1)) + \
	 ((long)(bp)->end2.dnum * (MAXLEVEL+1) + (long)(bp)->end2.dlevel))

    /*
     * Insert the new branch into the correct place in the branch list.
     */
    prev = (branch *) 0;
    prev_val = -1;
    new_val = branch_val(new_branch);
    for (curr = branches; curr;
		    prev_val = curr_val, prev = curr, curr = curr->next) {
	curr_val = branch_val(curr);
	if (prev_val < new_val && new_val <= curr_val) break;
    }
    if (prev) {
	new_branch->next = curr;
	prev->next = new_branch;
    } else {
	new_branch->next = branches;
	branches = new_branch;
    }
}

/* Add a dungeon branch to the branch list. */
STATIC_OVL branch *
add_branch(dgn, branch_num, pd)
    int dgn, branch_num;
    struct proto_dungeon *pd;
{
    static int branch_id = 0;
    branch *new_branch;
    int entry_lev;

    new_branch = (branch *) alloc(sizeof(branch));
    new_branch->next = (branch *) 0;
    new_branch->id = branch_id++;
    new_branch->type = correct_branch_type(&pd->tmpbranch[branch_num]);
    new_branch->end1.dnum = pd->tmpparent[branch_num];
    new_branch->end1.dlevel = parent_dlevel(branch_num, pd);
    new_branch->end2.dnum = dgn;
    /*
     * Calculate the entry level for target dungeon.  The pd.tmpbranch entry
     * value means:
     *		< 0	from bottom (-1 == bottom level)
     *		  0	default (top)
     *		> 0	actual level (1 = top)
     */
    if (pd->tmpbranch[branch_num].entry_lev < 0) {
	entry_lev = dungeons[dgn].num_dunlevs +	pd->tmpbranch[branch_num].entry_lev + 1;
	if (entry_lev <= 0) entry_lev = 1;
    } else if (pd->tmpbranch[dgn].entry_lev > 0) {
	entry_lev = pd->tmpbranch[branch_num].entry_lev;
	if (entry_lev > dungeons[dgn].num_dunlevs)
	    entry_lev = dungeons[dgn].num_dunlevs;
    }
    else
	entry_lev = 1;	/* defaults to top level */

    new_branch->end2.dlevel = entry_lev;
    new_branch->end1_up = pd->tmpbranch[branch_num].up ? TRUE : FALSE;

    insert_branch(new_branch, FALSE);
    return new_branch;
}

/*
 * Add new level to special level chain.  Insert it in level order with the
 * other levels in this dungeon.  This assumes that we are never given a
 * level that has a dungeon number less than the dungeon number of the
 * last entry.
 */
STATIC_OVL void
add_level(new_lev)
    s_level *new_lev;
{
	s_level *prev, *curr;

	prev = (s_level *) 0;
	for (curr = sp_levchn; curr; curr = curr->next) {
	    if (curr->dlevel.dnum == new_lev->dlevel.dnum &&
		    curr->dlevel.dlevel > new_lev->dlevel.dlevel)
		break;
	    prev = curr;
	}
	if (!prev) {
	    new_lev->next = sp_levchn;
	    sp_levchn = new_lev;
	} else {
	    new_lev->next = curr;
	    prev->next = new_lev;
	}
}

STATIC_OVL void
init_level(dgn, proto_index, pd)
	int dgn, proto_index;
	struct proto_dungeon *pd;
{
	s_level	*new_level;
	struct tmplevel *tlevel = &pd->tmplevel[proto_index];

	pd->final_lev[proto_index] = (s_level *) 0; /* no "real" level */
#ifdef WIZARD
/*      if (!wizard)   */
#endif
	    if (tlevel->chance <= rn2(100)) return;

	pd->final_lev[proto_index] = new_level =
					(s_level *) alloc(sizeof(s_level));
	/* load new level with data */
	strcpy(new_level->proto, tlevel->name);
	new_level->boneid = tlevel->boneschar;
	new_level->dlevel.dnum = dgn;
	new_level->dlevel.dlevel = 0;	/* for now */

	new_level->flags.town = !!(tlevel->flags & TOWN);
	new_level->flags.hellish = !!(tlevel->flags & HELLISH);
	new_level->flags.maze_like = !!(tlevel->flags & MAZELIKE);
	new_level->flags.rogue_like = !!(tlevel->flags & ROGUELIKE);
	new_level->flags.align = ((tlevel->flags & D_ALIGN_MASK) >> 4);
	if (!new_level->flags.align) 
	    new_level->flags.align =
		((pd->tmpdungeon[dgn].flags & D_ALIGN_MASK) >> 4);

	new_level->rndlevs = tlevel->rndlevs;
	new_level->next    = (s_level *) 0;
}

STATIC_OVL int
possible_places(idx, map, pd)
    int idx;		/* prototype index */
    boolean *map;	/* array MAXLEVEL+1 in length */
    struct proto_dungeon *pd;
{
    int i, start, count;
    s_level *lev = pd->final_lev[idx];

    /* init level possibilities */
    for (i = 0; i <= MAXLEVEL; i++) map[i] = FALSE;

    /* get base and range and set those entried to true */
    count = level_range(lev->dlevel.dnum, pd->tmplevel[idx].lev.base,
					pd->tmplevel[idx].lev.rand,
					pd->tmplevel[idx].chain,
					pd, &start);
    for (i = start; i < start+count; i++)
	map[i] = TRUE;

    /* mark off already placed levels */
    for (i = pd->start; i < idx; i++) {
	if (pd->final_lev[i] && map[pd->final_lev[i]->dlevel.dlevel]) {
	    map[pd->final_lev[i]->dlevel.dlevel] = FALSE;
	    --count;
	}
    }

    return count;
}

/* Pick the nth TRUE entry in the given boolean array. */
STATIC_OVL xchar
pick_level(map, nth)
    boolean *map;	/* an array MAXLEVEL+1 in size */
    int nth;
{
    int i;
    for (i = 1; i <= MAXLEVEL; i++)
	if (map[i] && !nth--) return (xchar) i;
    panic("pick_level:  ran out of valid levels");
    return 0;
}

#ifdef DDEBUG
static void indent(int);

static void
indent(d)
int d;
{
    while (d-- > 0) fputs("    ", stderr);
}
#endif

/*
 * Place a level.  First, find the possible places on a dungeon map
 * template.  Next pick one.  Then try to place the next level.  If
 * sucessful, we're done.  Otherwise, try another (and another) until
 * all possible places have been tried.  If all possible places have
 * been exausted, return false.
 */
STATIC_OVL boolean
place_level(proto_index, pd)
    int proto_index;
    struct proto_dungeon *pd;
{
    boolean map[MAXLEVEL+1];	/* valid levels are 1..MAXLEVEL inclusive */
    s_level *lev;
    int npossible;
#ifdef DDEBUG
    int i;
#endif

    if (proto_index == pd->n_levs) return TRUE;	/* at end of proto levels */

    lev = pd->final_lev[proto_index];

    /* No level created for this prototype, goto next. */
    if (!lev) return place_level(proto_index+1, pd);

    npossible = possible_places(proto_index, map, pd);

    for (; npossible; --npossible) {
	lev->dlevel.dlevel = pick_level(map, rn2(npossible));
#ifdef DDEBUG
	indent(proto_index-pd->start);
	fprintf(stderr,"%s: trying %d [ ", lev->proto, lev->dlevel.dlevel);
	for (i = 1; i <= MAXLEVEL; i++)
	    if (map[i]) fprintf(stderr,"%d ", i);
	fprintf(stderr,"%s","]\n");
#endif
	if (place_level(proto_index+1, pd)) return TRUE;
	map[lev->dlevel.dlevel] = FALSE;	/* this choice didn't work */
    }
#ifdef DDEBUG
    indent(proto_index-pd->start);
    fprintf(stderr,"%s: failed\n", lev->proto);
#endif
    return FALSE;
}


struct level_map {
	const char *lev_name;
	d_level *lev_spec;
} level_map[] = {
	{ "air",	&air_level },
	{ "asmod",	&asmodeus_level },
	{ "demog",   &demogorgon_level },
	{ "geryo",     &geryon_level },
	{ "dispa",   &dispater_level },
	{ "yeeno",   &yeenoghu_level },
	{ "astral",	&astral_level },
	{ "baalz",	&baalzebub_level },
	{ "bigrm",	&bigroom_level },
	{ "castl",	&stronghold_level },
	{ "earth",	&earth_level },
	{ "fakewiz1",	&portal_level },
	{ "fire",	&fire_level },
	{ "juibl",	&juiblex_level },
	{ "knox",	&knox_level },
	{ "blkmar",     &blackmarket_level },
	{ "medus",	&medusa_level },
	{ "elderp",	&elderpriest_level },
	{ "mtemple",	&mtemple_level },
	{ "nymph",   &nymph_level },        
	{ "forge",   &forge_level },        
	{ "hitch",   &hitch_level },        
	{ "compu",   &compu_level },        
	{ "key",   &key_level },        
	{ "oracle",	&oracle_level },
	{ "orcus",	&orcus_level },
#ifdef REINCARNATION
	{ "rogue",	&rogue_level },
#endif
	{ "sanct",	&sanctum_level },
	{ "valley",	&valley_level },
	{ "water",	&water_level },
	{ "wizard1",	&wiz1_level },
	{ "wizard2",	&wiz2_level },
	{ "wizard3",	&wiz3_level },
	{ "night",	&lawful_quest_level },
	{ "behol",	&neutral_quest_level },
	{ "lich",	&chaotic_quest_level },
#ifdef RECORD_ACHIEVE
        { "mnend",     &mineend_level },
        { "soko1",      &sokoend_level },
        { "depen",      &deepend_level },
#endif
	{ X_START,	&qstart_level },
	{ X_LOCATE,	&qlocate_level },
	{ X_GOAL,	&nemesis_level },
	{ X_XTONE,	&qxone_level },
	{ X_XTTWO,	&qxtwo_level },
	{ X_XTTHREE,	&qxthree_level },
	{ X_XTFOUR,	&qxfour_level },
	{ X_XTFIVE,	&qxfive_level },
	{ X_XTSIX,	&qxsix_level },
	{ X_XTSEVEN,	&qxseven_level },
	{ X_XTEIGHT,	&qxeight_level },
	{ X_XTNINE,	&qxnine_level },
	{ X_XTTEN,	&qxten_level },

	{ Y_YTSIX,	&qya_level },
	{ Y_FILA,	&qyb_level },
	{ Y_LOCA,	&qyc_level },
	{ Y_FILB,	&qyd_level },
	{ Y_GOAL,	&qye_level },

	{ "",		(d_level *)0 }
};

void
init_dungeons()
{
	dlb	*dgn_file;
	register int i, cl = 0, cb = 0;
	register s_level *x;
	struct proto_dungeon pd;
	struct level_map *lev_map;
	struct version_info vers_info;

	int dungeonfileversion = 1;
	if (!rn2(100)) dungeonfileversion = 2;
	else if (!rn2(100)) dungeonfileversion = 3;
	else if (!rn2(100)) dungeonfileversion = 4;
	else if (!rn2(100)) dungeonfileversion = 5;
	else if (!rn2(100)) dungeonfileversion = 6;
	else if (!rn2(100)) dungeonfileversion = 7;
	else if (!rn2(100)) dungeonfileversion = 8;

	/* note by Amy: we now have several variations of the dungeon, which have either the quest entrances moved to Gehennom, 100% chance for special levels to exist, a bigger range of areas for special levels to appear in, or several of these at once. The chance of non-standard versions should be low though. */

	/* 1 = normal, 2 = quests in Gehennom, 3 = all special levels exist, 4 = special levels can be everywhere, 5 = quests in Gehennom and all special levels exist, 6 = quests in Gehennom and special levels can be everywhere, 7 = all special levels exist and special levels can be everywhere, 8 = quests in Gehennom, all special levels exist and special levels can be everywhere */

	/* In Soviet Russia, people HATE it if they can't get multiple ascension kits before even reaching the castle. They need the Monty Haul segment in the game with all the special levels because, well, resource management isn't taught in Communism. But, this isn't slashthem, it's slashthem extended, so they will have to contend with all the nasty special levels I added, too! Which will also have a 100% chance of existing! :D --Amy */
	if (issoviet) dungeonfileversion = 3;

	/* [ALI] Cope with being called more than once. The GTK interface
	 * can currently do this, although it really should use popen().
	 */
	free_dungeons();

	pd.n_levs = pd.n_brs = 0;

	dgn_file = dlb_fopen_area(DUNGEON_AREA, (dungeonfileversion == 8) ? DUNGEON_FILE8 : (dungeonfileversion == 7) ? DUNGEON_FILE7 : (dungeonfileversion == 6) ? DUNGEON_FILE6 : (dungeonfileversion == 5) ? DUNGEON_FILE5 : (dungeonfileversion == 4) ? DUNGEON_FILE4 : (dungeonfileversion == 3) ? DUNGEON_FILE3 : (dungeonfileversion == 2) ? DUNGEON_FILE2 : DUNGEON_FILE, RDBMODE);
	if (!dgn_file) {
	    char tbuf[BUFSZ];
	    sprintf(tbuf, "Cannot open dungeon description - \"%s",
		(dungeonfileversion == 8) ? DUNGEON_FILE8 : (dungeonfileversion == 7) ? DUNGEON_FILE7 : (dungeonfileversion == 6) ? DUNGEON_FILE6 : (dungeonfileversion == 5) ? DUNGEON_FILE5 : (dungeonfileversion == 4) ? DUNGEON_FILE4 : (dungeonfileversion == 3) ? DUNGEON_FILE3 : (dungeonfileversion == 2) ? DUNGEON_FILE2 : DUNGEON_FILE);
#ifdef DLBRSRC /* using a resource from the executable */
	    strcat(tbuf, "\" resource!");
#else /* using a file or DLB file */
# if defined(DLB)
	    strcat(tbuf, "\" from ");
#  ifdef PREFIXES_IN_USE
	    strcat(tbuf, "\n\"");
	    if (fqn_prefix[DATAPREFIX]) strcat(tbuf, fqn_prefix[DATAPREFIX]);
#  else
	    strcat(tbuf, "\"");
#  endif
	    strcat(tbuf, DLBFILE);
# endif
	    strcat(tbuf, "\" file!");
#endif
#ifdef WIN32
	    interject_assistance(1, INTERJECT_PANIC, (void *)tbuf,
				 (void *)fqn_prefix[DATAPREFIX]);
#endif
	    panic("%s", tbuf);
	}

	/* validate the data's version against the program's version */
	Fread((void *) &vers_info, sizeof vers_info, 1, dgn_file);
	/* we'd better clear the screen now, since when error messages come from
	 * check_version() they will be printed using pline(), which doesn't
	 * mix with the raw messages that might be already on the screen
	 */
	if (iflags.window_inited) clear_nhwindow(WIN_MAP);
	if (!check_version(&vers_info, DUNGEON_FILE, TRUE))
	    panic("Dungeon description not valid.");
	if (!check_version(&vers_info, DUNGEON_FILE2, TRUE))
	    panic("Dungeon description not valid.");
	if (!check_version(&vers_info, DUNGEON_FILE3, TRUE))
	    panic("Dungeon description not valid.");
	if (!check_version(&vers_info, DUNGEON_FILE4, TRUE))
	    panic("Dungeon description not valid.");
	if (!check_version(&vers_info, DUNGEON_FILE5, TRUE))
	    panic("Dungeon description not valid.");
	if (!check_version(&vers_info, DUNGEON_FILE6, TRUE))
	    panic("Dungeon description not valid.");
	if (!check_version(&vers_info, DUNGEON_FILE7, TRUE))
	    panic("Dungeon description not valid.");
	if (!check_version(&vers_info, DUNGEON_FILE8, TRUE))
	    panic("Dungeon description not valid.");

	/*
	 * Read in each dungeon and transfer the results to the internal
	 * dungeon arrays.
	 */
	sp_levchn = (s_level *) 0;
	Fread((void *)&n_dgns, sizeof(int), 1, dgn_file);
	if (n_dgns >= MAXDUNGEON)
	    panic("init_dungeons: too many dungeons");

	for (i = 0; i < n_dgns; i++) {
	    Fread((void *)&pd.tmpdungeon[i],
				    sizeof(struct tmpdungeon), 1, dgn_file);
#ifdef WIZARD
	    if(!wizard)
#endif
	      if(pd.tmpdungeon[i].chance && (pd.tmpdungeon[i].chance <= rn2(100))) {
		int j;

		/* skip over any levels or branches */
		for(j = 0; j < pd.tmpdungeon[i].levels; j++)
		    Fread((void *)&pd.tmplevel[cl], sizeof(struct tmplevel),
							1, dgn_file);

		for(j = 0; j < pd.tmpdungeon[i].branches; j++)
		    Fread((void *)&pd.tmpbranch[cb],
					sizeof(struct tmpbranch), 1, dgn_file);
		n_dgns--; i--;
		continue;
	      }

	    strcpy(dungeons[i].dname, pd.tmpdungeon[i].name);
	    strcpy(dungeons[i].proto, pd.tmpdungeon[i].protoname);
	    dungeons[i].boneid = pd.tmpdungeon[i].boneschar;

	    if(pd.tmpdungeon[i].lev.rand)
		dungeons[i].num_dunlevs = (xchar)rn1(pd.tmpdungeon[i].lev.rand,
						     pd.tmpdungeon[i].lev.base);
	    else dungeons[i].num_dunlevs = (xchar)pd.tmpdungeon[i].lev.base;

	    if(!i) {
		dungeons[i].ledger_start = 0;
		dungeons[i].depth_start = 1;
		dungeons[i].dunlev_ureached = 1;
	    } else {
		dungeons[i].ledger_start = dungeons[i-1].ledger_start +
					      dungeons[i-1].num_dunlevs;
		dungeons[i].dunlev_ureached = 0;

		if (dungeons[i].ledger_start + dungeons[i].num_dunlevs > 2555)
		    panic("init_dungeons: too many levels");
	    }

	    dungeons[i].flags.hellish = !!(pd.tmpdungeon[i].flags & HELLISH);
	    dungeons[i].flags.maze_like = !!(pd.tmpdungeon[i].flags & MAZELIKE);
	    dungeons[i].flags.rogue_like = !!(pd.tmpdungeon[i].flags & ROGUELIKE);
	    dungeons[i].flags.align = ((pd.tmpdungeon[i].flags & D_ALIGN_MASK) >> 4);
	    dungeons[i].entry_lev = 1;	/* defaults to top level */

	    if (i) {	/* set depth */
		branch *br;
		schar from_depth;
		boolean from_up;
		int branch_num;

		for (branch_num = 0; branch_num < pd.n_brs; branch_num++)
		    if (!strcmp(pd.tmpbranch[branch_num].name, dungeons[i].dname)) {
			br = add_branch(i, branch_num, &pd);
			break;
		    }
		
		/* Set the dungeon entry level from the first branch */
		dungeons[i].entry_lev = br->end2.dlevel;

		/* Get the depth of the connecting end. */
		if (br->end1.dnum == i) {
		    from_depth = depth(&br->end2);
		    from_up = !br->end1_up;
		} else {
		    from_depth = depth(&br->end1);
		    from_up = br->end1_up;
		}

		/*
		 * Calculate the depth of the top of the dungeon via
		 * its branch.  First, the depth of the entry point:
		 *
		 *	depth of branch from "parent" dungeon
		 *	+ -1 or 1 depending on a up or down stair or
		 *	  0 if portal
		 *
		 * Followed by the depth of the top of the dungeon:
		 *
		 *	- (entry depth - 1)
		 *
		 * We'll say that portals stay on the same depth.
		 */
		dungeons[i].depth_start = from_depth
					+ (br->type == BR_PORTAL ? 0 :
							(from_up ? -1 : 1))
					- (dungeons[i].entry_lev - 1);
	    }

	    /* this is redundant - it should have been flagged by dgn_comp */
	    if(dungeons[i].num_dunlevs > MAXLEVEL)
		dungeons[i].num_dunlevs = MAXLEVEL;

	    pd.start = pd.n_levs;	/* save starting point */
	    pd.n_levs += pd.tmpdungeon[i].levels;
	    if (pd.n_levs > LEV_LIMIT)
		panic("init_dungeon: too many special levels");
	    /*
	     * Read in the prototype special levels.  Don't add generated
	     * special levels until they are all placed.
	     */
	    for(; cl < pd.n_levs; cl++) {
		Fread((void *)&pd.tmplevel[cl],
					sizeof(struct tmplevel), 1, dgn_file);
		init_level(i, cl, &pd);
	    }
	    /*
	     * Recursively place the generated levels for this dungeon.  This
	     * routine will attempt all possible combinations before giving
	     * up.
	     */
	    if (!place_level(pd.start, &pd))
		panic("init_dungeon:  couldn't place levels");
#ifdef DDEBUG
	    fprintf(stderr, "--- end of dungeon %d ---\n", i);
	    fflush(stderr);
	    getchar();
#endif
	    for (; pd.start < pd.n_levs; pd.start++)
		if (pd.final_lev[pd.start]) add_level(pd.final_lev[pd.start]);


	    pd.n_brs += pd.tmpdungeon[i].branches;
	    if (pd.n_brs > BRANCH_LIMIT)
		panic("init_dungeon: too many branches");
	    for(; cb < pd.n_brs; cb++) {
		int dgn;
		Fread((void *)&pd.tmpbranch[cb],
					sizeof(struct tmpbranch), 1, dgn_file);
		pd.tmpparent[cb] = i;
		for (dgn = 0; dgn < i; dgn++)
		    if (!strcmp(pd.tmpbranch[cb].name, dungeons[dgn].dname)) {
			(void)add_branch(dgn, cb, &pd);
			break;
		    }
	    }
	}
	(void) dlb_fclose(dgn_file);

	for (i = 0; i < 5; i++) tune[i] = 'A' + rn2(7);
	tune[5] = 0;

	/*
	 * Find most of the special levels and dungeons so we can access their
	 * locations quickly.
	 */
	for (lev_map = level_map; lev_map->lev_name[0]; lev_map++) {
		x = find_level(lev_map->lev_name);
		if (x) {
			assign_level(lev_map->lev_spec, &x->dlevel);
			if (!strncmp(lev_map->lev_name, "x-", 2)) {
				/* This is where the name substitution on the
				 * levels of the quest dungeon occur.
				 */
				sprintf(x->proto, "%s%s", urole.filecode, &lev_map->lev_name[1]);
			} else if (!strncmp(lev_map->lev_name, "y-", 2)) { /* rival quest by Amy */
				sprintf(x->proto, "%s%s", u.rivalcode, &lev_map->lev_name[1]);
			} else if (lev_map->lev_spec == &knox_level) {
				branch *br;
				/*
				 * Kludge to allow floating Knox entrance.  We
				 * specify a floating entrance by the fact that
				 * its entrance (end1) has a bogus dnum, namely
				 * n_dgns.
				 */
				for (br = branches; br; br = br->next)
				    if (on_level(&br->end2, &knox_level)) break;

				if (br) br->end1.dnum = n_dgns;
				/* adjust the branch's position on the list */
				insert_branch(br, TRUE);
			}
		}
	}
/*
 *	I hate hardwiring these names. :-(
 */
	quest_dnum = dname_to_dnum("The Quest");
	sokoban_dnum = dname_to_dnum("Sokoban");
	mines_dnum = dname_to_dnum("The Gnomish Mines");
	sheol_dnum = dname_to_dnum("Sheol");
	yendoriantower_dnum = dname_to_dnum("Yendorian Tower");
	forgingchamber_dnum = dname_to_dnum("Forging Chamber");
	orderedchaos_dnum = dname_to_dnum("Ordered Chaos");
	deadgrounds_dnum = dname_to_dnum("Dead Grounds");
	subquest_dnum = dname_to_dnum("The Subquest");
	rivalquest_dnum = dname_to_dnum("Rival Quest");
	bellcaves_dnum = dname_to_dnum("Bell Caves");
	spiders_dnum = dname_to_dnum("The Spider Caves");        
	grund_dnum = dname_to_dnum("Grund's Stronghold");        
	icequeen_dnum = dname_to_dnum("The Ice Queen's Realm");        
	wyrm_dnum = dname_to_dnum("The Wyrm Caves");        
	frnkn_dnum = dname_to_dnum("Frankenstein's Lab");        
	gcavern_dnum = dname_to_dnum("The Giant Caverns");        
	mtemple_dnum = dname_to_dnum("The Temple of Moloch");        
	slsea_dnum = dname_to_dnum("The Sunless Sea");        
	tomb_dnum = dname_to_dnum("The Lost Tomb");        
	tower_dnum = dname_to_dnum("Vlad's Tower");
	dod_dnum = dname_to_dnum("The Dungeons of Doom");
	town_dnum = dname_to_dnum("Town");
	gehennom_dnum = dname_to_dnum("Gehennom");
	illusorycastle_dnum = dname_to_dnum("Illusory Castle");
	voiddungeon_dnum = dname_to_dnum("Void");
	netherrealm_dnum = dname_to_dnum("Nether Realm");
	deepmines_dnum = dname_to_dnum("Deep Mines");
	angmar_dnum = dname_to_dnum("Angmar");
	greencross_dnum = dname_to_dnum("Green Cross");
	emynluin_dnum = dname_to_dnum("Emyn Luin");
	minotaurmaze_dnum = dname_to_dnum("Minotaur Maze");
	swimmingpool_dnum = dname_to_dnum("Swimming Pool");
	hellbathroom_dnum = dname_to_dnum("Hell's Bathroom");
	minusworld_dnum = dname_to_dnum("Minus World");
	spacebase_dnum = dname_to_dnum("Space Base");
	sewerplant_dnum = dname_to_dnum("Sewer Plant");
	gammacaves_dnum = dname_to_dnum("Gamma Caves");
	mainframe_dnum = dname_to_dnum("Mainframe");
	joustchallenge_dnum = dname_to_dnum("Joust Challenge");
	pacmanchallenge_dnum = dname_to_dnum("Pacman Challenge");
	digdugchallenge_dnum = dname_to_dnum("Digdug Challenge");
	gruechallenge_dnum = dname_to_dnum("Grue Challenge");
	poolchallenge_dnum = dname_to_dnum("Pool Challenge");
	restingzone_ga_dnum = dname_to_dnum("Resting Zone GA");
	restingzone_gb_dnum = dname_to_dnum("Resting Zone GB");
	restingzone_gc_dnum = dname_to_dnum("Resting Zone GC");
	restingzone_gd_dnum = dname_to_dnum("Resting Zone GD");
	restingzone_ge_dnum = dname_to_dnum("Resting Zone GE");
	restingzone_ta_dnum = dname_to_dnum("Resting Zone TA");
	restingzone_tb_dnum = dname_to_dnum("Resting Zone TB");
	restingzone_tc_dnum = dname_to_dnum("Resting Zone TC");
	restingzone_td_dnum = dname_to_dnum("Resting Zone TD");
	restingzone_te_dnum = dname_to_dnum("Resting Zone TE");
	restingzone_tf_dnum = dname_to_dnum("Resting Zone TF");
	restingzone_tg_dnum = dname_to_dnum("Resting Zone TG");
	restingzone_th_dnum = dname_to_dnum("Resting Zone TH");
	restingzone_ti_dnum = dname_to_dnum("Resting Zone TI");
	restingzone_tj_dnum = dname_to_dnum("Resting Zone TJ");
	restingzone_a_dnum = dname_to_dnum("Resting Zone A");
	restingzone_s_dnum = dname_to_dnum("Resting Zone S");
	restingzone_e_dnum = dname_to_dnum("Resting Zone E");
/*
	blackmarket_dnum = dname_to_dnum("The Black Market");
*/

	/* one special fixup for dummy surface level */
	if ((x = find_level("dummy")) != 0) {
	    i = x->dlevel.dnum;
	    /* the code above puts earth one level above dungeon level #1,
	       making the dummy level overlay level 1; but the whole reason
	       for having the dummy level is to make earth have depth -1
	       instead of 0, so adjust the start point to shift endgame up */
	    if (dunlevs_in_dungeon(&x->dlevel) > 1 - dungeons[i].depth_start)
		dungeons[i].depth_start -= 1;
	    /* TO DO: strip "dummy" out all the way here,
	       so that it's hidden from <ctrl/O> feedback. */
	}

#ifdef DEBUG
	dumpit();
#endif
}

xchar
dunlev(lev)	/* return the level number for lev in *this* dungeon */
d_level	*lev;
{
	return(lev->dlevel);
}

xchar
dunlevs_in_dungeon(lev)	/* return the lowest level number for *this* dungeon*/
d_level	*lev;
{
	/* lowest level of Gnome Mines is gone for Gnomes */        
	if (Role_if(PM_GNOME) && lev->dnum == mines_dnum) {
		return((dungeons[lev->dnum].num_dunlevs)-1);
	} else return(dungeons[lev->dnum].num_dunlevs);
}

xchar
real_dunlevs_in_dungeon(lev)  /* return the lowest level number for *this* dungeon*/
d_level       *lev;
{
     /* this one is not altered for Gnomes */
	return(dungeons[lev->dnum].num_dunlevs);
}

xchar
deepest_lev_reached(noquest) /* return the lowest level explored in the game*/
boolean noquest;
{
	/* this function is used for three purposes: to provide a factor
	 * of difficulty in monster generation; to provide a factor of
	 * difficulty in experience calculations (botl.c and end.c); and
	 * to insert the deepest level reached in the game in the topten
	 * display.  the 'noquest' arg switch is required for the latter.
	 *
	 * from the player's point of view, going into the Quest is _not_
	 * going deeper into the dungeon -- it is going back "home", where
	 * the dungeon starts at level 1.  given the setup in dungeon.def,
	 * the depth of the Quest (thought of as starting at level 1) is
	 * never lower than the level of entry into the Quest, so we exclude
	 * the Quest from the topten "deepest level reached" display
	 * calculation.  _However_ the Quest is a difficult dungeon, so we
	 * include it in the factor of difficulty calculations.
	 */
	register int i;
	d_level tmp;
	register schar ret = 0;

	for(i = 0; i < n_dgns; i++) {
	    if((tmp.dlevel = dungeons[i].dunlev_ureached) == 0) continue;
	    if(!strcmp(dungeons[i].dname, "The Quest") && noquest) continue;

	    tmp.dnum = i;
	    if(depth(&tmp) > ret) ret = depth(&tmp);
	}
	return((xchar) ret);
}

/* return a bookkeeping level number for purpose of comparisons and
 * save/restore */
xchar
ledger_no(lev)
d_level	*lev;
{
	return((xchar)(lev->dlevel + dungeons[lev->dnum].ledger_start));
}

/*
 * The last level in the bookkeeping list of level is the bottom of the last
 * dungeon in the dungeons[] array.
 *
 * Maxledgerno() -- which is the max number of levels in the bookkeeping
 * list, should not be confused with dunlevs_in_dungeon(lev) -- which
 * returns the max number of levels in lev's dungeon, and both should
 * not be confused with deepest_lev_reached() -- which returns the lowest
 * depth visited by the player.
 */
xchar
maxledgerno()
{
    return (xchar) (dungeons[n_dgns-1].ledger_start +
				dungeons[n_dgns-1].num_dunlevs);
}

/* return the dungeon that this ledgerno exists in */
xchar
ledger_to_dnum(ledgerno)
xchar	ledgerno;
{
	register int i;

	/* find i such that (i->base + 1) <= ledgerno <= (i->base + i->count) */
	for (i = 0; i < n_dgns; i++)
	    if (dungeons[i].ledger_start < ledgerno &&
		ledgerno <= dungeons[i].ledger_start + dungeons[i].num_dunlevs)
		return (xchar)i;

	panic("level number out of range [ledger_to_dnum(%d)]", (int)ledgerno);
	/*NOT REACHED*/
	return (xchar)0;
}

/* return the level of the dungeon this ledgerno exists in */
xchar
ledger_to_dlev(ledgerno)
xchar	ledgerno;
{
	return((xchar)(ledgerno - dungeons[ledger_to_dnum(ledgerno)].ledger_start));
}

#endif /* OVL1 */
#ifdef OVL0

/* returns the depth of a level, in floors below the surface	*/
/* (note levels in different dungeons can have the same depth).	*/
schar
depth(lev)
d_level	*lev;
{
	return((schar)( dungeons[lev->dnum].depth_start + lev->dlevel - 1));
}

boolean
on_level(lev1, lev2)	/* are "lev1" and "lev2" actually the same? */
d_level	*lev1, *lev2;
{
	return((boolean)((lev1->dnum == lev2->dnum) && (lev1->dlevel == lev2->dlevel)));
}

#endif /* OVL0 */
#ifdef OVL1

/* is this level referenced in the special level chain? */
s_level *
Is_special(lev)
d_level	*lev;
{
	s_level *levtmp;

	for (levtmp = sp_levchn; levtmp; levtmp = levtmp->next)
	    if (on_level(lev, &levtmp->dlevel)) return(levtmp);

	return((s_level *)0);
}

/*
 * Is this a multi-dungeon branch level?  If so, return a pointer to the
 * branch.  Otherwise, return null.
 */
branch *
Is_branchlev(lev)
d_level *lev;
{
	branch *curr;

	for (curr = branches; curr; curr = curr->next) {
	    if (on_level(lev, &curr->end1) || on_level(lev, &curr->end2))
		return curr;
	}
	return (branch *) 0;
}

/* goto the next level (or appropriate dungeon) */
void
next_level(at_stairs)
boolean	at_stairs;
{
	if (at_stairs && u.ux == sstairs.sx && u.uy == sstairs.sy) {
		/* Taking a down dungeon branch. */
		goto_level(&sstairs.tolev, at_stairs, FALSE, FALSE);

		if (!rn2(isfriday ? 3 : 5)) u.stairscumslowing += rn1(5,5);

		if ((!rn2(ishaxor ? 250 : 500)) || StairsProblem || u.uprops[STAIRSTRAP].extrinsic || (uarmc && uarmc->oartifact == ART_PERCENTIOEOEPSPERCENTD_THI) || have_stairstrapstone() ) {

			pline(FunnyHallu ? "Wow! A welcoming committee!" : "Stairs trap!");
			u.cnd_stairstrapcount++;
			NoStaircase = 10 + rnz(monster_difficulty() + 1);
			pushplayer(TRUE);
			(void)nasty((struct monst *)0);

		}

		if (!rn2(ishaxor ? 2500 : 5000)) {
			makerandomtrap(FALSE);
			if (!rn2(2)) makerandomtrap(FALSE);
			if (!rn2(3)) makerandomtrap(FALSE);
			if (!rn2(4)) makerandomtrap(FALSE);
			if (!rn2(6)) makerandomtrap(FALSE);
			if (!rn2(8)) makerandomtrap(FALSE);
			if (!rn2(12)) makerandomtrap(FALSE);
			if (!rn2(16)) makerandomtrap(FALSE);
			if (!rn2(24)) makerandomtrap(FALSE);
			if (!rn2(32)) makerandomtrap(FALSE);
			if (!rn2(48)) makerandomtrap(FALSE);
			if (!rn2(64)) makerandomtrap(FALSE);
			if (!rn2(96)) makerandomtrap(FALSE);
			if (!rn2(128)) makerandomtrap(FALSE);
			if (!rn2(192)) makerandomtrap(FALSE);
			if (!rn2(256)) makerandomtrap(FALSE);

		}
		if (!rn2(10)) pushplayer(TRUE);

	} else {
		/* Going down a stairs or jump in a trap door. */
		d_level	newlevel;

		newlevel.dnum = u.uz.dnum;
		newlevel.dlevel = u.uz.dlevel + 1;
		goto_level(&newlevel, at_stairs, !at_stairs, FALSE);

		if (!rn2(isfriday ? 3 : 5)) u.stairscumslowing += rn1(5,5);

		if (at_stairs && (!rn2(ishaxor ? 250 : 500) || StairsProblem || u.uprops[STAIRSTRAP].extrinsic || (uarmc && uarmc->oartifact == ART_PERCENTIOEOEPSPERCENTD_THI) || have_stairstrapstone() ) ) {

			pline(FunnyHallu ? "Wow! A welcoming committee!" : "Stairs trap!");
			u.cnd_stairstrapcount++;
			NoStaircase = 10 + rnz(monster_difficulty() + 1);
			pushplayer(TRUE);
			(void)nasty((struct monst *)0);

		}

		if (!rn2(ishaxor ? 2500 : 5000)) {
			makerandomtrap(FALSE);
			if (!rn2(2)) makerandomtrap(FALSE);
			if (!rn2(3)) makerandomtrap(FALSE);
			if (!rn2(4)) makerandomtrap(FALSE);
			if (!rn2(6)) makerandomtrap(FALSE);
			if (!rn2(8)) makerandomtrap(FALSE);
			if (!rn2(12)) makerandomtrap(FALSE);
			if (!rn2(16)) makerandomtrap(FALSE);
			if (!rn2(24)) makerandomtrap(FALSE);
			if (!rn2(32)) makerandomtrap(FALSE);
			if (!rn2(48)) makerandomtrap(FALSE);
			if (!rn2(64)) makerandomtrap(FALSE);
			if (!rn2(96)) makerandomtrap(FALSE);
			if (!rn2(128)) makerandomtrap(FALSE);
			if (!rn2(192)) makerandomtrap(FALSE);
			if (!rn2(256)) makerandomtrap(FALSE);

		}
		if (!rn2(10)) pushplayer(TRUE);
	}
}

/* goto the previous level (or appropriate dungeon) */
void
prev_level(at_stairs)
boolean	at_stairs;
{
	if (at_stairs && u.ux == sstairs.sx && u.uy == sstairs.sy) {
		/* Taking an up dungeon branch. */
		/* KMH -- Upwards branches are okay if not level 1 */
		/* (Just make sure it doesn't go above depth 1) */
		if(!u.uz.dnum && u.uz.dlevel == 1 && !u.uhave.amulet && !(u.freeplaymode && u.freeplayplanes) ) done(ESCAPED);
		else { 

			goto_level(&sstairs.tolev, at_stairs, FALSE, FALSE);

			if (!rn2(isfriday ? 3 : 5)) u.stairscumslowing += rn1(5,5);

			if ((!rn2(ishaxor ? 50 : 100)) || StairsProblem || u.uprops[STAIRSTRAP].extrinsic || (uarmc && uarmc->oartifact == ART_PERCENTIOEOEPSPERCENTD_THI) || have_stairstrapstone() ) {

				pline(FunnyHallu ? "Wow! A welcoming committee!" : "Stairs trap!");
				u.cnd_stairstrapcount++;
				NoStaircase = 10 + rnz(monster_difficulty() + 1);
				pushplayer(TRUE);
				(void)nasty((struct monst *)0);

			}

			if (!rn2(ishaxor ? 2500 : 5000)) {
				makerandomtrap(FALSE);
				if (!rn2(2)) makerandomtrap(FALSE);
				if (!rn2(3)) makerandomtrap(FALSE);
				if (!rn2(4)) makerandomtrap(FALSE);
				if (!rn2(6)) makerandomtrap(FALSE);
				if (!rn2(8)) makerandomtrap(FALSE);
				if (!rn2(12)) makerandomtrap(FALSE);
				if (!rn2(16)) makerandomtrap(FALSE);
				if (!rn2(24)) makerandomtrap(FALSE);
				if (!rn2(32)) makerandomtrap(FALSE);
				if (!rn2(48)) makerandomtrap(FALSE);
				if (!rn2(64)) makerandomtrap(FALSE);
				if (!rn2(96)) makerandomtrap(FALSE);
				if (!rn2(128)) makerandomtrap(FALSE);
				if (!rn2(192)) makerandomtrap(FALSE);
				if (!rn2(256)) makerandomtrap(FALSE);
	
			}

		}
		if (!rn2(10)) pushplayer(TRUE);
	} else {
		/* Going up a stairs or rising through the ceiling. */
		d_level	newlevel;
		newlevel.dnum = u.uz.dnum;
		newlevel.dlevel = u.uz.dlevel - 1;
		goto_level(&newlevel, at_stairs, FALSE, FALSE);

		if (!rn2(isfriday ? 3 : 5)) u.stairscumslowing += rn1(5,5);

		if (at_stairs && (!rn2(ishaxor ? 50 : 100) || StairsProblem || u.uprops[STAIRSTRAP].extrinsic || (uarmc && uarmc->oartifact == ART_PERCENTIOEOEPSPERCENTD_THI) || have_stairstrapstone() ) ) {

			pline(FunnyHallu ? "Wow! A welcoming committee!" : "Stairs trap!");
			u.cnd_stairstrapcount++;
			NoStaircase = 10 + rnz(monster_difficulty() + 1);
			pushplayer(TRUE);
			(void)nasty((struct monst *)0);
		}

			if (!rn2(ishaxor ? 2500 : 5000)) {
				makerandomtrap(FALSE);
				if (!rn2(2)) makerandomtrap(FALSE);
				if (!rn2(3)) makerandomtrap(FALSE);
				if (!rn2(4)) makerandomtrap(FALSE);
				if (!rn2(6)) makerandomtrap(FALSE);
				if (!rn2(8)) makerandomtrap(FALSE);
				if (!rn2(12)) makerandomtrap(FALSE);
				if (!rn2(16)) makerandomtrap(FALSE);
				if (!rn2(24)) makerandomtrap(FALSE);
				if (!rn2(32)) makerandomtrap(FALSE);
				if (!rn2(48)) makerandomtrap(FALSE);
				if (!rn2(64)) makerandomtrap(FALSE);
				if (!rn2(96)) makerandomtrap(FALSE);
				if (!rn2(128)) makerandomtrap(FALSE);
				if (!rn2(192)) makerandomtrap(FALSE);
				if (!rn2(256)) makerandomtrap(FALSE);
	
			}
		if (!rn2(10)) pushplayer(TRUE);
	}
}

void
u_on_newpos(x, y)
int x, y;
{
	u.ux = x;
	u.uy = y;
#ifdef CLIPPING
	cliparound(u.ux, u.uy);
#endif
	/* ridden steed always shares hero's location */
	if (u.usteed) u.usteed->mx = u.ux, u.usteed->my = u.uy;
}

void
u_on_sstairs() {	/* place you on the special staircase */

	if (sstairs.sx) {
	    u_on_newpos(sstairs.sx, sstairs.sy);
	} else {
	    /* code stolen from goto_level */
	    int trycnt = 0;
	    xchar x, y;
#ifdef DEBUG
	    pline("u_on_sstairs: picking random spot");
#endif
#define badspot(x,y) ((levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
	    do {
		x = rnd(COLNO-1);
		y = rn2(ROWNO);
		if (!badspot(x, y)) {
		    u_on_newpos(x, y);
		    return;
		}
	    } while (++trycnt <= 500);
	    panic("u_on_sstairs: could not relocate player!");
#undef badspot
	}
}

void
u_on_upstairs()	/* place you on upstairs (or special equivalent) */
{
	if (xupstair) {
		u_on_newpos(xupstair, yupstair);
	} else
		u_on_sstairs();
}

void
u_on_dnstairs()	/* place you on dnstairs (or special equivalent) */
{
	if (xdnstair) {
		u_on_newpos(xdnstair, ydnstair);
	} else
		u_on_sstairs();
}

boolean
On_stairs(x, y)
xchar x, y;
{
	return((boolean)((x == xupstair && y == yupstair) ||
	       (x == xdnstair && y == ydnstair) ||
	       (x == xdnladder && y == ydnladder) ||
	       (x == xupladder && y == yupladder) ||
	       (x == sstairs.sx && y == sstairs.sy)));
}

boolean
Is_botlevel(lev)
d_level *lev;
{
	return((boolean)(lev->dlevel == dungeons[lev->dnum].num_dunlevs));
}

boolean
Can_dig_down(lev)
d_level *lev;
{
	return((boolean)(!level.flags.hardfloor
	    && !Is_botlevel(lev) && !Invocation_lev(lev)));
}

/*
 * Like Can_dig_down (above), but also allows falling through on the
 * stronghold level.  Normally, the bottom level of a dungeon resists
 * both digging and falling.
 */
boolean
Can_fall_thru(lev)
d_level *lev;
{
	return((boolean)(Can_dig_down(lev) || Is_stronghold(lev)));
}

/*
 * True if one can rise up a level (e.g. cursed gain level).
 * This happens on intermediate dungeon levels or on any top dungeon
 * level that has a stairwell style branch to the next higher dungeon.
 * Checks for amulets and such must be done elsewhere.
 */
boolean
Can_rise_up(x, y, lev)
int	x, y;
d_level *lev;
{
    /* can't rise up from inside the top of the Wizard's tower */
    /* KMH -- or in sokoban */
    if (In_endgame(lev) || In_sokoban_real(lev) ||
			(Is_wiz1_level(lev) && In_W_tower(x, y, lev)))
	return FALSE;
    return (boolean)(lev->dlevel > 1 ||
		(dungeons[lev->dnum].entry_lev == 1 && ledger_no(lev) != 1 &&
		 sstairs.sx && sstairs.up));
}

/*
 * It is expected that the second argument of get_level is a depth value,
 * either supplied by the user (teleport control) or randomly generated.
 * But more than one level can be at the same depth.  If the target level
 * is "above" the present depth location, get_level must trace "up" from
 * the player's location (through the ancestors dungeons) the dungeon
 * within which the target level is located.  With only one exception
 * which does not pass through this routine (see level_tele), teleporting
 * "down" is confined to the current dungeon.  At present, level teleport
 * in dungeons that build up is confined within them.
 */
void
get_level(newlevel, levnum)
d_level *newlevel;
int levnum;
{
	branch *br;
	xchar dgn = u.uz.dnum;

	if (levnum <= 0) {
	    /* can only currently happen in endgame */
	    levnum = u.uz.dlevel;
	} else if (levnum > dungeons[dgn].depth_start
			    + dungeons[dgn].num_dunlevs - 1) {
	    /* beyond end of dungeon, jump to last level */
	    levnum = dungeons[dgn].num_dunlevs;
	} else {
	    /* The desired level is in this dungeon or a "higher" one. */

	    /*
	     * Branch up the tree until we reach a dungeon that contains the
	     * levnum.
	     */
	    if (levnum < dungeons[dgn].depth_start) {

		do {
		    /*
		     * Find the parent dungeon of this dungeon.
		     *
		     * This assumes that end2 is always the "child" and it is
		     * unique.
		     */
		    for (br = branches; br; br = br->next)
			if (br->end2.dnum == dgn) break;
		    if (!br)
			panic("get_level: can't find parent dungeon");

		    dgn = br->end1.dnum;
		} while (levnum < dungeons[dgn].depth_start);
	    }

	    /* We're within the same dungeon; calculate the level. */
	    levnum = levnum - dungeons[dgn].depth_start + 1;
	}

	newlevel->dnum = dgn;
	newlevel->dlevel = levnum;
}

#endif /* OVL1 */
#ifdef OVL0

boolean
In_quest(lev)	/* are you in the quest dungeon? */
d_level *lev;
{
	return((boolean)(lev->dnum == quest_dnum));
}

#endif /* OVL0 */
#ifdef OVL1

boolean
In_mines(lev)	/* are you in the mines dungeon? */
d_level	*lev;
{
	return((boolean)(lev->dnum == mines_dnum));
}

boolean
In_sheol(lev)	/* are you in the sheol dungeon? */
d_level	*lev;
{
	return((boolean)(lev->dnum == sheol_dnum));
}

boolean
In_yendorian(lev)	/* are you in the yendorian tower dungeon? */
d_level	*lev;
{
	return((boolean)(lev->dnum == yendoriantower_dnum));
}

boolean
In_forging(lev)	/* are you in the forging chamber dungeon? */
d_level	*lev;
{
	return((boolean)(lev->dnum == forgingchamber_dnum));
}

boolean
In_ordered(lev)	/* are you in the ordered chaos dungeon? */
d_level	*lev;
{
	return((boolean)(lev->dnum == orderedchaos_dnum));
}

boolean
In_deadground(lev)	/* are you in the dead grounds dungeon? */
d_level	*lev;
{
	return((boolean)(lev->dnum == deadgrounds_dnum));
}

boolean
In_subquest(lev)	/* are you in the subquest dungeon? */
d_level	*lev;
{
	return((boolean)(lev->dnum == subquest_dnum));
}

boolean
In_rivalquest(lev)	/* are you in the rival quest dungeon? */
d_level	*lev;
{
	return((boolean)(lev->dnum == rivalquest_dnum));
}

boolean
In_bellcaves(lev)	/* are you in the bell caves dungeon? */
d_level	*lev;
{
	return((boolean)(lev->dnum == bellcaves_dnum));
}

boolean
In_gehennom(lev)	/* are you in the gehennom dungeon? */
d_level	*lev;
{
	return((boolean)(lev->dnum == gehennom_dnum));
}

boolean
In_towndungeon(lev)	/* are you in the town dungeon? */
d_level	*lev;
{
	return((boolean)(lev->dnum == town_dnum));
}

boolean
In_illusorycastle(lev)	/* are you in the illusory castle dungeon? */
d_level	*lev;
{
	return((boolean)(lev->dnum == illusorycastle_dnum));
}

boolean
In_voiddungeon(lev)	/* are you in the void dungeon? */
d_level	*lev;
{
	return((boolean)(lev->dnum == voiddungeon_dnum));
}

boolean
In_netherrealm(lev)	/* are you in the nether realm dungeon? */
d_level	*lev;
{
	return((boolean)(lev->dnum == netherrealm_dnum));
}

boolean
In_deepmines(lev)	/* are you in the deep mines dungeon? */
d_level	*lev;
{
	return((boolean)(lev->dnum == deepmines_dnum));
}

boolean
In_angmar(lev)	/* are you in the angmar dungeon? */
d_level	*lev;
{
	return((boolean)(lev->dnum == angmar_dnum));
}

boolean
In_greencross(lev)	/* are you in the green cross dungeon? */
d_level	*lev;
{
	return((boolean)(lev->dnum == greencross_dnum));
}

boolean
In_emynluin(lev)	/* are you in the emyn luin dungeon? */
d_level	*lev;
{
	return((boolean)(lev->dnum == emynluin_dnum));
}

boolean
In_minotaurmaze(lev)	/* are you in the minotaur maze dungeon? */
d_level	*lev;
{
	return((boolean)(lev->dnum == minotaurmaze_dnum));
}

boolean
In_swimmingpool(lev)	/* are you in the swimming pool of hell dungeon? */
d_level	*lev;
{
	return((boolean)(lev->dnum == swimmingpool_dnum));
}

boolean
In_hellbathroom(lev)	/* are you in the hell's bathroom dungeon? */
d_level	*lev;
{
	return((boolean)(lev->dnum == hellbathroom_dnum));
}

boolean
In_minusworld(lev)	/* are you in the minus world? */
d_level	*lev;
{
	return((boolean)(lev->dnum == minusworld_dnum));
}

boolean
In_spacebase(lev)	/* are you in the space base dungeon? */
d_level	*lev;
{
	return((boolean)(lev->dnum == spacebase_dnum));
}

boolean
In_sewerplant(lev)	/* are you in the sewer plant dungeon? */
d_level	*lev;
{
	return((boolean)(lev->dnum == sewerplant_dnum));
}

boolean
In_gammacaves(lev)	/* are you in the gamma caves dungeon? */
d_level	*lev;
{
	return((boolean)(lev->dnum == gammacaves_dnum));
}

boolean
In_mainframe(lev)	/* are you in the mainframe dungeon? */
d_level	*lev;
{
	return((boolean)(lev->dnum == mainframe_dnum));
}

boolean
In_poolchallenge(lev)
d_level	*lev;
{
	return((boolean)(lev->dnum == poolchallenge_dnum));
}

boolean
In_digdugchallenge(lev)
d_level	*lev;
{
	return((boolean)(lev->dnum == digdugchallenge_dnum));
}

boolean
In_pacmanchallenge(lev)
d_level	*lev;
{
	return((boolean)(lev->dnum == pacmanchallenge_dnum));
}

boolean
In_gruechallenge(lev)
d_level	*lev;
{
	return((boolean)(lev->dnum == gruechallenge_dnum));
}

boolean
In_joustchallenge(lev)
d_level	*lev;
{
	return((boolean)(lev->dnum == joustchallenge_dnum));
}

boolean
In_dod(lev)	/* are you in the dungeons of doom? */
d_level	*lev;
{
	return((boolean)(lev->dnum == dod_dnum));
}

boolean
In_spiders(lev) /* are you in the spider dungeon? */
d_level *lev;
{
	return((boolean)(lev->dnum == spiders_dnum));
}

boolean
In_grund(lev) /* are you in grund's stronghold? */
d_level *lev;
{
	return((boolean)(lev->dnum == grund_dnum));
}

boolean
In_icequeen(lev) /* are you in the Ice Queen's realm? */
d_level *lev;
{
	return((boolean)(lev->dnum == icequeen_dnum));
}

boolean
In_wyrm(lev) /* are you in the wyrm caves? */
d_level *lev;
{
	return((boolean)(lev->dnum == wyrm_dnum));
}

boolean
In_frnkn(lev) /* are you in Frankenstein's lab? */
d_level *lev;
{
	return((boolean)(lev->dnum == frnkn_dnum));
}

boolean
In_gcavern(lev) /* are you in the giant caverns? */
d_level *lev;
{
	return((boolean)(lev->dnum == gcavern_dnum));
}

boolean
In_mtemple(lev) /* are you in the temple of moloch? */
d_level *lev;
{
	return((boolean)(lev->dnum == mtemple_dnum));
}

boolean
In_slsea(lev) /* are you in the sunless sea? */
d_level *lev;
{
	return((boolean)(lev->dnum == slsea_dnum));
}

boolean
In_tomb(lev) /* are you in the lost tomb? */
d_level *lev;
{
	return((boolean)(lev->dnum == tomb_dnum));
}

boolean
In_restingzone(lev) /* are you in one of the resting zone dungeons? */
d_level *lev;
{
	return((boolean)(lev->dnum == restingzone_ga_dnum || lev->dnum == restingzone_gb_dnum || lev->dnum == restingzone_gc_dnum || lev->dnum == restingzone_gd_dnum || lev->dnum == restingzone_ge_dnum || lev->dnum == restingzone_ta_dnum || lev->dnum == restingzone_tb_dnum || lev->dnum == restingzone_tc_dnum || lev->dnum == restingzone_td_dnum || lev->dnum == restingzone_te_dnum || lev->dnum == restingzone_tf_dnum || lev->dnum == restingzone_tg_dnum || lev->dnum == restingzone_th_dnum || lev->dnum == restingzone_ti_dnum || lev->dnum == restingzone_tj_dnum || lev->dnum == restingzone_a_dnum || lev->dnum == restingzone_s_dnum));
}

boolean
In_ZAPM(lev)	/* are you in one of the ZAPM levels? */
d_level	*lev;
{
	return((boolean)(lev->dnum == spacebase_dnum || lev->dnum == sewerplant_dnum || lev->dnum == gammacaves_dnum || lev->dnum == mainframe_dnum));
}

boolean
In_Devnull(lev)	/* are you in one of the devnull challenge levels? */
d_level	*lev;
{
	return((boolean)(lev->dnum == poolchallenge_dnum || lev->dnum == digdugchallenge_dnum || lev->dnum == pacmanchallenge_dnum || lev->dnum == gruechallenge_dnum || lev->dnum == joustchallenge_dnum));
}

boolean
In_lategame(lev)	/* are you in Gehennom or a branch that comes after it? */
d_level	*lev;
{
	if (In_yendorian(lev) || In_forging(lev) || In_ordered(lev) || In_deadground(lev) || In_voiddungeon(lev) || In_restingzone(lev) || In_netherrealm(lev) || In_angmar(lev) || In_emynluin(lev) || In_swimmingpool(lev) || In_hellbathroom(lev) || In_rivalquest(lev) || In_gehennom(lev) || In_frnkn(lev) || In_sheol(lev) || In_V_tower(lev) || In_endgame(lev) ) return TRUE;

	return FALSE;
}

/* can the player levelport, branchport, be banished etc. here? --Amy */
boolean
playerlevelportdisabled()
{
	if (flags.lostsoul || flags.uberlostsoul) return TRUE;
	if (flags.wonderland && !(u.wonderlandescape)) return TRUE;
	if (iszapem && !(u.zapemescape)) return TRUE;
	if (u.preversionmode && !u.preversionescape) return TRUE;
	if (u.uprops[STORM_HELM].extrinsic) return TRUE;
	if (In_bellcaves(&u.uz) || In_subquest(&u.uz) || In_rivalquest(&u.uz) || In_voiddungeon(&u.uz) || In_netherrealm(&u.uz) || In_minusworld(&u.uz)) return TRUE;

	return FALSE;
}

/*
 * Return the branch for the given dungeon.
 *
 * This function assumes:
 *	+ This is not called with "Dungeons of Doom".
 *	+ There is only _one_ branch to a given dungeon.
 *	+ Field end2 is the "child" dungeon.
 */
branch *
dungeon_branch(s)
    const char *s;
{
    branch *br;
    xchar  dnum;

    dnum = dname_to_dnum(s);

    /* Find the branch that connects to dungeon i's branch. */
    for (br = branches; br; br = br->next)
	if (br->end2.dnum == dnum) break;

    if (!br) panic("dgn_entrance: can't find entrance to %s", s);

    return br;
}

/*
 * This returns true if the hero is on the same level as the entrance to
 * the named dungeon.
 *
 * Called from do.c and mklev.c.
 *
 * Assumes that end1 is always the "parent".
 */
boolean
at_dgn_entrance(s)
    const char *s;
{
    branch *br;

    br = dungeon_branch(s);
    return((boolean)(on_level(&u.uz, &br->end1) ? TRUE : FALSE));
}

boolean
In_V_tower(lev)	/* is `lev' part of Vlad's tower? */
d_level	*lev;
{
	return((boolean)(lev->dnum == tower_dnum));
}

boolean
On_W_tower_level(lev)	/* is `lev' a level containing the Wizard's tower? */
d_level	*lev;
{
	return (boolean)(Is_wiz1_level(lev) ||
			 Is_wiz2_level(lev) ||
			 Is_wiz3_level(lev));
}

boolean
In_W_tower(x, y, lev)	/* is <x,y> of `lev' inside the Wizard's tower? */
int	x, y;
d_level	*lev;
{
	if (!On_W_tower_level(lev)) return FALSE;
	/*
	 * Both of the exclusion regions for arriving via level teleport
	 * (from above or below) define the tower's boundary.
	 *	assert( updest.nIJ == dndest.nIJ for I={l|h},J={x|y} );
	 */
	if (dndest.nlx > 0)
	    return (boolean)within_bounded_area(x, y, dndest.nlx, dndest.nly,
						dndest.nhx, dndest.nhy);
	else
	    impossible("No boundary for Wizard's Tower?");
	return FALSE;
}

#endif /* OVL1 */
#ifdef OVL0

boolean
In_hell(lev)	/* are you in one of the Hell levels? */
d_level	*lev;
{
	return((boolean)(dungeons[lev->dnum].flags.hellish));
}

#endif /* OVL0 */
#ifdef OVL1

void
find_hell(lev)	/* sets *lev to be the gateway to Gehennom... */
d_level *lev;
{
	lev->dnum = valley_level.dnum;
	lev->dlevel = 1;
}

void
goto_hell(at_stairs, falling)	/* go directly to hell... */
boolean	at_stairs, falling;
{
	d_level lev;

	find_hell(&lev);
	goto_level(&lev, at_stairs, falling, FALSE);
}

void
assign_level(dest, src)		/* equivalent to dest = source */
d_level	*dest, *src;
{
	dest->dnum = src->dnum;
	dest->dlevel = src->dlevel;
}

void
assign_rnd_level(dest, src, range)	/* dest = src + rn1(range) */
d_level	*dest, *src;
int range;
{
	dest->dnum = src->dnum;
	dest->dlevel = src->dlevel + ((range > 0) ? rnd(range) : -rnd(-range)) ;

	if(dest->dlevel > dunlevs_in_dungeon(dest))
		dest->dlevel = dunlevs_in_dungeon(dest);
	else if(dest->dlevel < 1)
		dest->dlevel = 1;
}

#endif /* OVL1 */
#ifdef OVL0

int
induced_align(pct)
int	pct;
{
	s_level	*lev = Is_special(&u.uz);
	aligntyp al;

	if (lev && lev->flags.align)
		if(rn2(100) < pct) return(lev->flags.align);

	if(dungeons[u.uz.dnum].flags.align)
		if(rn2(100) < pct) return(dungeons[u.uz.dnum].flags.align);

	al = rn2(3) - 1;
	return(Align2amask(al));
}

#endif /* OVL0 */
#ifdef OVL1

boolean
Invocation_lev(lev)
d_level *lev;
{
	return((boolean)(In_gehennom(lev) &&
		lev->dlevel == (dungeons[lev->dnum].num_dunlevs - 1)));
}

/* use instead of depth() wherever a degree of difficulty is made
 * dependent on the location in the dungeon (eg. monster creation).
 */
xchar /* xchar means it can only be 127 or lower. If a value >127 is returned, the game destabilizes! */
level_difficulty()
{
	int retvalue;

	int depthuz;
	int deepestuz;

	if (iszapem && In_ZAPM(&u.uz) && !(u.zapemescape)) {

		d_level zapemlevel;
		int zapemdepth;
		zapemlevel.dnum = dname_to_dnum("Space Base");
		zapemlevel.dlevel = dungeons[zapemlevel.dnum].entry_lev;
		zapemdepth = depth(&zapemlevel);

		depthuz = (1 + depth(&u.uz) - zapemdepth);
		if (depthuz < 1) depthuz = 1; /* fail safe */

		deepestuz = (1 + deepest_lev_reached(TRUE) - zapemdepth);
		if (deepestuz < 1) deepestuz = 1; /* fail safe */

	} else if (u.preversionmode && !u.preversionescape && In_greencross(&u.uz)) {

		d_level preverlevel;
		int preverdepth;
		preverlevel.dnum = dname_to_dnum("Green Cross");
		preverlevel.dlevel = dungeons[preverlevel.dnum].entry_lev;
		preverdepth = depth(&preverlevel);

		depthuz = (1 + depth(&u.uz) - preverdepth);
		if (depthuz < 1) depthuz = 1; /* fail safe */

		deepestuz = (1 + deepest_lev_reached(TRUE) - preverdepth);
		if (deepestuz < 1) deepestuz = 1; /* fail safe */

	} else {
		depthuz = depth(&u.uz);
		deepestuz = deepest_lev_reached(TRUE);
	}

	/* since the dungeon in SLEX is pretty deep, let's make the difficulty increase more gentle... --Amy */
	if (depthuz >= 12) {
		depthuz *= 4;
		depthuz /= 5;
	} else if (depthuz == 7) depthuz = 6;
	else if (depthuz == 8) depthuz = 7;
	else if (depthuz == 9) depthuz = 7;
	else if (depthuz == 10) depthuz = 8;
	else if (depthuz == 11) depthuz = 8;

	if (In_endgame(&u.uz))
		retvalue = (110 + (u.ulevel/2) );
	else if (u.uhave.amulet && !u.freeplaymode && (u.amuletcompletelyimbued || !rn2(5)))
		retvalue = 110;
	else if ((Race_if(PM_IMPERIAL) || (Inhell && !Race_if(PM_HERETIC) ) || flags.gehenna) && !rn2(3))
		retvalue = (depthuz + rn2(u.ulevel) + 2 );
	else
		retvalue = depthuz;

	if (u.uhave.amulet && !u.freeplaymode && (retvalue < 50)) retvalue = 50;

	if (Race_if(PM_EXPERT)) retvalue += u.ulevel;

	if ( (!rn2(10) || Race_if(PM_GASTLY) || Race_if(PM_PHANTOM_GHOST) ) && (deepestuz > retvalue) ) retvalue = deepestuz;

	/* generally increase monster difficulty gradually as the game goes on --Amy */
	if (!rn2(2) && moves > 10000) retvalue++;
	if (!rn2(2) && moves > 20000) retvalue++;
	if (!rn2(2) && moves > 30000) retvalue++;
	if (!rn2(2) && moves > 40000) retvalue++;
	if (!rn2(2) && moves > 50000) retvalue++;
	if (!rn2(2) && moves > 60000) retvalue++;
	if (!rn2(2) && moves > 70000) retvalue++;
	if (!rn2(2) && moves > 80000) retvalue++;
	if (!rn2(2) && moves > 90000) retvalue++;
	if (!rn2(2) && moves > 100000) retvalue++;
	if (!rn2(2) && moves > 200000) retvalue++;
	if (!rn2(2) && moves > 300000) retvalue++;
	if (!rn2(2) && moves > 400000) retvalue++;
	if (!rn2(2) && moves > 500000) retvalue++;
	if (!rn2(2) && moves > 600000) retvalue++;
	if (!rn2(2) && moves > 700000) retvalue++;
	if (!rn2(2) && moves > 800000) retvalue++;
	if (!rn2(2) && moves > 900000) retvalue++;
	if (!rn2(2) && moves > 1000000) retvalue++;

	/* later in the game, low-level monsters will be less common */
	if (moves > 1000 && retvalue < 2 && rn2(2) ) retvalue = 2;
	if (moves > 2000 && retvalue < 2) retvalue = 2;
	if (moves > 3000 && retvalue < 3 && rn2(2) ) retvalue = 3;
	if (moves > 4000 && retvalue < 3) retvalue = 3;
	if (moves > 5000 && retvalue < 5 && rn2(2) ) retvalue = 5;
	if (moves > 10000 && retvalue < 5) retvalue = 5;
	if (moves > 15000 && retvalue < 10 && rn2(2) ) retvalue = 10;
	if (moves > 20000 && retvalue < 10) retvalue = 10;
	if (moves > 30000 && retvalue < 15 && rn2(2) ) retvalue = 15;
	if (moves > 40000 && retvalue < 15) retvalue = 15;
	if (moves > 50000 && retvalue < 20 && rn2(2) ) retvalue = 20;
	if (moves > 60000 && retvalue < 20) retvalue = 20;
	if (moves > 70000 && retvalue < 25 && rn2(2) ) retvalue = 25;
	if (moves > 80000 && retvalue < 25) retvalue = 25;
	if (moves > 100000 && retvalue < 30 && rn2(2) ) retvalue = 30;
	if (moves > 120000 && retvalue < 30) retvalue = 30;
	if (moves > 140000 && retvalue < 35 && rn2(2) ) retvalue = 35;
	if (moves > 160000 && retvalue < 35) retvalue = 35;
	if (moves > 180000 && retvalue < 40 && rn2(2) ) retvalue = 40;
	if (moves > 200000 && retvalue < 40) retvalue = 40;
	if (moves > 250000 && retvalue < 50 && rn2(2) ) retvalue = 50;
	if (moves > 300000 && retvalue < 50) retvalue = 50;
	if (moves > 350000 && retvalue < 75 && rn2(2) ) retvalue = 75;
	if (moves > 400000 && retvalue < 75) retvalue = 75;
	if (moves > 450000 && retvalue < 100 && rn2(2) ) retvalue = 100;
	if (moves > 500000 && retvalue < 100) retvalue = 100;
	if (moves > 750000 && retvalue < 125 && rn2(2) ) retvalue = 125;
	if (moves > 1000000 && retvalue < 125) retvalue = 125;

	/* if you play for over 200k turns, you're probably procrastinating, or at the very least should be capable of
	 * handling the occasional high-level monster... --Amy
	 * after 100k turns, have a more gentle increase until 200k is reached */
	if (moves > 100000) {
		int retcrease = 1;
		if (moves > 110000) retcrease += ((moves - 100000) / 10000);
		if (retcrease > 10) retcrease = 10; /* fail safe */
		if (retcrease < 1) retcrease = 1;
		if (!rn2(3)) retvalue += retcrease;
	}
	if (moves > 200000) {
		int retcrease = 1;
		if (moves > 210000) retcrease += ((moves - 200000) / 10000);
		/* no upper limit */
		if (retcrease < 1) retcrease = 1; /* fail safe */
		retvalue += retcrease;
	}

	/* occasionally have them be just a little out of depth to keep you on your toes... --Amy */
	if (!rn2(20)) retvalue += rno(3);

	/* some variation - it's annoying if you always get max difficulty monsters --Amy */
	if ((retvalue > 1) && ((!u.aggravation && !isaggravator && !isextravator && !GravationAggravation ) || !rn2((ExtAggravate_monster || isextravator || GravationAggravation) ? 3 : 2)) && !u.outtadepthtrap && !rn2(issoviet ? 3 : 2)) {
		retvalue *= 4;
		retvalue /= 5;
	}
	if ((retvalue > 1) && ((!u.aggravation && !isaggravator && !isextravator && !GravationAggravation ) || !rn2((ExtAggravate_monster || isextravator || GravationAggravation) ? 5 : 3)) && !u.outtadepthtrap && !rn2(issoviet ? 15 : 5)) {
		retvalue *= 3;
		retvalue /= 5;
	}
	if ((retvalue > 1) && ((!u.aggravation && !isaggravator && !isextravator && !GravationAggravation ) || !rn2((ExtAggravate_monster || isextravator || GravationAggravation) ? 10 : 5)) && !u.outtadepthtrap && !rn2(issoviet ? 200 : 50)) {
		retvalue *= 2;
		retvalue /= 5;
	}
	if ((retvalue > 1) && ((!u.aggravation && !isaggravator && !isextravator && !GravationAggravation ) || !rn2((ExtAggravate_monster || isextravator || GravationAggravation) ? 20 : 10)) && !u.outtadepthtrap && !rn2(issoviet ? 1250 : 250)) {
		retvalue /= 5;
	}

	if (retvalue < 1) retvalue = 1;

	/* Psions are so overpowered if they have all their intrinsics, that I decided to make them harder. --Amy */
	if (Role_if(PM_PSION) && u.ulevel >= 7) retvalue += rnd(retvalue);

	if (DifficultyIncreased || u.uprops[DIFFICULTY_INCREASED].extrinsic || have_difficultystone() || Race_if(PM_PLAYER_DYNAMO) || (uwep && uwep->oartifact == ART_ARABELLA_S_BLACK_PRONG) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_ARABELLA_S_BLACK_PRONG) || (uwep && uwep->oartifact == ART_HOL_ON_MAN) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_HOL_ON_MAN) || (uwep && uwep->oartifact == ART_PWNHAMMER_DUECE) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_PWNHAMMER_DUECE) || (uwep && uwep->oartifact == ART_CUDGEL_OF_CUTHBERT) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_CUDGEL_OF_CUTHBERT) || (uwep && uwep->oartifact == ART_ONE_THROUGH_FOUR_SCEPTER) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_ONE_THROUGH_FOUR_SCEPTER) ) retvalue += 10;
	if (Race_if(PM_BULDOZGAR) && !rn2(10)) retvalue += 10;
	if (Race_if(PM_PHANTOM_GHOST)) retvalue++;

	if (uarmg && uarmg->oartifact == ART_DIFFICULTY__) retvalue += (6 + u.ulevel);

	if ((u.aggravation || isaggravator || isextravator || GravationAggravation) && ((ExtAggravate_monster || isextravator || GravationAggravation) || !rn2(2)) ) {

		switch (rnd(10)) {
			case 1:
				retvalue *= 2;
				if (!rn2(3)) retvalue += u.ulevel;
				break;
			case 2:
			case 3:
			case 4:
			case 5:
				retvalue *= 3;
				retvalue /= 2;
				break;
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
				break;
		}
	}

	if (u.outtadepthtrap) {
		retvalue *= 4;
	}

	/* ultra aggravate monster if you pick both hybrid races --Amy */
	if (isextravator && isaggravator) retvalue *= 2;

	if (uarmc && itemhasappearance(uarmc, APP_DIFFICULT_CLOAK) ) {
		retvalue *= 2;
	}

	if (uarmc && uarmc->oartifact == ART_ULTRAGGRAVATE) retvalue *= 2;

	if (u.cellargravate) retvalue *= 2;

	if (uarmg && uarmg->oartifact == ART_DIFFICULT_) retvalue *= 2;

	/* skew generation to make very high-level monsters much more unlikely --Amy */

	if ((retvalue > 20) && rn2(3)) retvalue = (19 + rnd(retvalue - 19));

	/* now skew it even more towards low-level stuff */

	if (retvalue > 1 && retvalue <= 6 && !rn2(5)) retvalue = rnd(retvalue);
	else if (retvalue > 6 && retvalue <= 11 && !rn2(4)) retvalue = ((retvalue - 5) + rnd(5));
	else if (retvalue > 11 && !rn2(3)) retvalue = (5 + rnd(retvalue - 5));

	if (retvalue > 126) retvalue = 126; /* fail safe */
	if (retvalue < 1) retvalue = 1;

	/*pline("%d diff", retvalue);*/

	return((xchar) retvalue);
}
/* Sadly, we cannot make the sanctum deeper than dlvl 65 thanks to the arbitrary limit of 127 dungeon levels.
 * And I don't want to remove any branches just to make room for more gehennom levels. So the deepest level
 * will always be 65, and we'll have to arbitrarily increase the level difficulty for the endgame. --Amy */

/* edit - well, actually we can; I decided to max out the amount of possible levels and put the sanctum at 72. --Amy
 * edit again: expanded it greatly, it's at 100 now */

xchar /* 127 or lower */
monster_difficulty()
{

	int tempval;

	tempval = (level_difficulty() + u.ulevel + 1)>>1;
	if (tempval < level_difficulty()) tempval = level_difficulty();
	/* this function is meant to make sure high-level characters don't get stupidly easy monsters at shallow depths,
	 * yet I also don't want monsters at deep dungeon levels to be of a lower level than they should be. --Amy */

	if (Race_if(PM_DEVELOPER)) tempval += rnd(30);

	if (tempval < 1) tempval = 1;
	if (tempval > 125) tempval = 125; /* to be on the safe side */

	/*pline("%d mondiff", tempval);*/

	return((xchar) tempval);

}

/* Take one word and try to match it to a level.
 * Recognized levels are as shown by print_dungeon().
 */
schar
lev_by_name(nam)
const char *nam;
{
    schar lev = 0;
    s_level *slev;
    d_level dlev;
    const char *p;
    int idx, idxtoo;
    char buf[BUFSZ];

    /* allow strings like "the oracle level" to find "oracle" */
    if (!strncmpi(nam, "the ", 4)) nam += 4;
    if ((p = strstri(nam, " level")) != 0 && p == eos((char*)nam) - 6) {
	nam = strcpy(buf, nam);
	*(eos(buf) - 6) = '\0';
    }
    /* hell is the old name, and wouldn't match; gehennom would match its
       branch, yielding the castle level instead of the valley of the dead */
    if (!strcmpi(nam, "gehennom") || !strcmpi(nam, "hell")) {
	if (In_V_tower(&u.uz)) nam = " to Vlad's tower";  /* branch to... */
	else nam = "valley";
    }

    if ((slev = find_level(nam)) != 0) {
	dlev = slev->dlevel;
	idx = ledger_no(&dlev);
	if ((dlev.dnum == u.uz.dnum ||
		/* within same branch, or else main dungeon <-> gehennom */
		(u.uz.dnum == valley_level.dnum &&
			dlev.dnum == medusa_level.dnum) ||
		(u.uz.dnum == medusa_level.dnum &&
			dlev.dnum == valley_level.dnum)) &&
	    (	/* either wizard mode or else seen and not forgotten */
#ifdef WIZARD
	     wizard ||
#endif
		(level_info[idx].flags & (FORGOTTEN|VISITED)) == VISITED)) {
	    lev = depth(&slev->dlevel);
	}
    } else {	/* not a specific level; try branch names */
	idx = find_branch(nam, (struct proto_dungeon *)0);
	/* "<branch> to Xyzzy" */
	if (idx < 0 && (p = strstri(nam, " to ")) != 0)
	    idx = find_branch(p + 4, (struct proto_dungeon *)0);

	if (idx >= 0) {
	    idxtoo = (idx >> 8) & 0x00FF;
	    idx &= 0x00FF;
	    if (  /* either wizard mode, or else _both_ sides of branch seen */
#ifdef WIZARD
		wizard ||
#endif
		((level_info[idx].flags & (FORGOTTEN|VISITED)) == VISITED &&
		 (level_info[idxtoo].flags & (FORGOTTEN|VISITED)) == VISITED)) {
		if (ledger_to_dnum(idxtoo) == u.uz.dnum) idx = idxtoo;
		dlev.dnum = ledger_to_dnum(idx);
		dlev.dlevel = ledger_to_dlev(idx);
		lev = depth(&dlev);
	    }
	}
    }
    return lev;
}

#ifdef WIZARD

/* Convert a branch type to a string usable by print_dungeon(). */
STATIC_OVL const char *
br_string(type)
    int type;
{
    switch (type) {
	case BR_PORTAL:	 return "Portal";
	case BR_NO_END1: return "Connection";
	case BR_NO_END2: return "One way stair";
	case BR_STAIR:	 return "Stair";
    }
    return " (unknown)";
}

/* Print all child branches between the lower and upper bounds. */
STATIC_OVL void
print_branch(win, dnum, lower_bound, upper_bound, bymenu, lchoices)
    winid win;
    int   dnum;
    int   lower_bound;
    int   upper_bound;
    boolean bymenu;
    struct lchoice *lchoices;
{
    branch *br;
    char buf[BUFSZ];
    anything any;

    /* This assumes that end1 is the "parent". */
    for (br = branches; br; br = br->next) {
	if (br->end1.dnum == dnum && lower_bound < br->end1.dlevel &&
					br->end1.dlevel <= upper_bound) {
	    sprintf(buf,"   %s to %s: %d",
		    br_string(br->type),
		    dungeons[br->end2.dnum].dname,
		    depth(&br->end1));
	    if (bymenu) {
		lchoices->lev[lchoices->idx] = br->end1.dlevel;
		lchoices->dgn[lchoices->idx] = br->end1.dnum;
		lchoices->playerlev[lchoices->idx] = depth(&br->end1);
		any.a_void = 0;
		any.a_int = lchoices->idx + 1;
		add_menu(win, NO_GLYPH, &any, lchoices->menuletter,
				0, ATR_NONE, buf, MENU_UNSELECTED);
		if (lchoices->menuletter == 'z') lchoices->menuletter = 'A';
		else if (lchoices->menuletter == 'Z') lchoices->menuletter = 'a';
		else lchoices->menuletter++;
		lchoices->idx++;
	    } else
		putstr(win, 0, buf);
	}
    }
}

/* Print available dungeon information. */
schar
print_dungeon(bymenu, rlev, rdgn)
boolean bymenu;
schar *rlev;
xchar *rdgn;
{
    int     i, last_level, nlev;
    char    buf[BUFSZ];
    boolean first;
    s_level *slev;
    dungeon *dptr;
    branch  *br;
    anything any;
    struct lchoice lchoices;

    winid   win = create_nhwindow(NHW_MENU);
    if (bymenu) {
	start_menu(win);
	lchoices.idx = 0;
	lchoices.menuletter = 'a';
    }

    for (i = 0, dptr = dungeons; i < n_dgns; i++, dptr++) {
	nlev = dptr->num_dunlevs;
	if (nlev > 1)
	    sprintf(buf, "%s: levels %d to %d", dptr->dname, dptr->depth_start,
						dptr->depth_start + nlev - 1);
	else
	    sprintf(buf, "%s: level %d", dptr->dname, dptr->depth_start);

	/* Most entrances are uninteresting. */
	if (dptr->entry_lev != 1) {
	    if (dptr->entry_lev == nlev)
		strcat(buf, ", entrance from below");
	    else
		sprintf(eos(buf), ", entrance on %d",
			dptr->depth_start + dptr->entry_lev - 1);
	}
	if (bymenu) {
	    any.a_void = 0;
	    add_menu(win, NO_GLYPH, &any, 0, 0, iflags.menu_headings, buf, MENU_UNSELECTED);
	} else
	    putstr(win, 0, buf);

	/*
	 * Circle through the special levels to find levels that are in
	 * this dungeon.
	 */
	for (slev = sp_levchn, last_level = 0; slev; slev = slev->next) {
	    if (slev->dlevel.dnum != i) continue;

	    /* print any branches before this level */
	    print_branch(win, i, last_level, slev->dlevel.dlevel, bymenu, &lchoices);

	    sprintf(buf, "   %s: %d", slev->proto, depth(&slev->dlevel));
	    if (Is_stronghold(&slev->dlevel))
		sprintf(eos(buf), " (tune %s)", tune);
	    if (bymenu) {
	    	/* If other floating branches are added, this will need to change */
	    	if (i != knox_level.dnum) {
			lchoices.lev[lchoices.idx] = slev->dlevel.dlevel;
			lchoices.dgn[lchoices.idx] = i;
		} else {
			lchoices.lev[lchoices.idx] = depth(&slev->dlevel);
			lchoices.dgn[lchoices.idx] = 0;
		}
		lchoices.playerlev[lchoices.idx] = depth(&slev->dlevel);
		any.a_void = 0;
		any.a_int = lchoices.idx + 1;
		add_menu(win, NO_GLYPH, &any, lchoices.menuletter,
				0, ATR_NONE, buf, MENU_UNSELECTED);
		if (lchoices.menuletter == 'z') lchoices.menuletter = 'A';
		else if (lchoices.menuletter == 'Z') lchoices.menuletter = 'a';
		else lchoices.menuletter++;
		lchoices.idx++;
	    } else
		putstr(win, 0, buf);

	    last_level = slev->dlevel.dlevel;
	}
	/* print branches after the last special level */
	print_branch(win, i, last_level, MAXLEVEL, bymenu, &lchoices);
    }

    /* Print out floating branches (if any). */
    for (first = TRUE, br = branches; br; br = br->next) {
	if (br->end1.dnum == n_dgns) {
	    if (first) {
	    	if (!bymenu) {
		    putstr(win, 0, "");
		    putstr(win, 0, "Floating branches");
		}
		first = FALSE;
	    }
	    sprintf(buf, "   %s to %s",
			br_string(br->type), dungeons[br->end2.dnum].dname);
	    if (!bymenu)
		putstr(win, 0, buf);
	}
    }
    if (bymenu) {
    	int n;
	menu_item *selected;
	int idx;

	end_menu(win, "Level teleport to where:");
	n = select_menu(win, PICK_ONE, &selected);
	destroy_nhwindow(win);
	if (n > 0) {
		idx = selected[0].item.a_int - 1;
		free((void *)selected);
		if (rlev && rdgn) {
			*rlev = lchoices.lev[idx];
			*rdgn = lchoices.dgn[idx];
			return lchoices.playerlev[idx];
		}
	}
	return 0;
    }

    /* I hate searching for the invocation pos while debugging. -dean */
    if (Invocation_lev(&u.uz)) {
	putstr(win, 0, "");
	sprintf(buf, "Invocation position @ (%d,%d), hero @ (%d,%d)",
		inv_pos.x, inv_pos.y, u.ux, u.uy);
	putstr(win, 0, buf);
    }
    /*
     * The following is based on the assumption that the inter-level portals
     * created by the level compiler (not the dungeon compiler) only exist
     * one per level (currently true, of course).
     */
    else if (Is_earthlevel(&u.uz) || Is_waterlevel(&u.uz)
				|| Is_firelevel(&u.uz) || Is_airlevel(&u.uz)) {
	struct trap *trap;
	for (trap = ftrap; trap; trap = trap->ntrap)
	    if (trap->ttyp == MAGIC_PORTAL) break;

	putstr(win, 0, "");
	if (trap)
	    sprintf(buf, "Portal @ (%d,%d), hero @ (%d,%d)",
		trap->tx, trap->ty, u.ux, u.uy);
	else
	    sprintf(buf, "No portal found.");
	putstr(win, 0, buf);
    }

    display_nhwindow(win, TRUE);
    destroy_nhwindow(win);
    return 0;
}
#endif /* WIZARD */

#endif /* OVL1 */

/* add a custom name to the current level */
int
donamelevel()
{
	mapseen *mptr;
	char qbuf[QBUFSZ];	/* Buffer for query text */
	char nbuf[BUFSZ];	/* Buffer for response */
	int len;

	if (!(mptr = find_mapseen(&u.uz))) return 0;

	if (!DisplayDoesNotGoAtAll && !(uarmc && uarmc->oartifact == ART_CLOAK_OF_THE_CONSORT) && mptr->custom) {

		pline("This level is currently annotated as %s.", mptr->custom);

	}

	sprintf(qbuf,"What do you want to call this dungeon level? ");
	getlin(qbuf, nbuf);

	if (index(nbuf, '\033')) return 0;

	len = strlen(nbuf) + 1;
	if (mptr->custom) {
		free((void *)mptr->custom);
		mptr->custom = (char *)0;
		mptr->custom_lth = 0;
	}
	
	if (*nbuf) {
		mptr->custom = (char *) alloc(sizeof(char) * len);
		mptr->custom_lth = len;
		strcpy(mptr->custom, nbuf);
	}
   
	return 0;
}

/* find the particular mapseen object in the chain */
/* may return 0 */
STATIC_OVL mapseen *
find_mapseen(lev)
d_level *lev;
{
	mapseen *mptr;

	for (mptr = mapseenchn; mptr; mptr = mptr->next)
		if (on_level(&(mptr->lev), lev)) break;

	return mptr;
}

void
forget_mapseen(ledger_no)
int ledger_no;
{
	mapseen *mptr;

	for (mptr = mapseenchn; mptr; mptr = mptr->next)
		if (dungeons[mptr->lev.dnum].ledger_start + 
			mptr->lev.dlevel == ledger_no) break;

	/* if not found, then nothing to forget */
	if (mptr) {

		/* custom names are erased, not forgotten until revisted */
		if (mptr->custom) {
			mptr->custom_lth = 0;
			free((void *)mptr->custom);
			mptr->custom = (char *)0;
		}

	}
}

STATIC_OVL void
save_mapseen(fd, mptr)
int fd;
mapseen *mptr;
{
	branch *curr;
	int count;

	count = 0;
	for (curr = branches; curr; curr = curr->next) {
		if (curr == mptr->br) break;
		count++;
	}

	bwrite(fd, (void *) &count, sizeof(int));
	bwrite(fd, (void *) &mptr->lev, sizeof(d_level));
	bwrite(fd, (void *) &mptr->custom_lth, sizeof(unsigned));
	if (mptr->custom_lth)
		bwrite(fd, (void *) mptr->custom, 
		sizeof(char) * mptr->custom_lth);
}

STATIC_OVL mapseen *
load_mapseen(fd)
int fd;
{
	int branchnum, count;
	mapseen *load;
	branch *curr;

	load = (mapseen *) alloc(sizeof(mapseen));
	mread(fd, (void *) &branchnum, sizeof(int));

	count = 0;
	for (curr = branches; curr; curr = curr->next) {
		if (count == branchnum) break;
		count++;
	}
	load->br = curr;

	mread(fd, (void *) &load->lev, sizeof(d_level));
	mread(fd, (void *) &load->custom_lth, sizeof(unsigned));
	if (load->custom_lth > 0) {
		load->custom = (char *) alloc(sizeof(char) * load->custom_lth);
		mread(fd, (void *) load->custom, 
			sizeof(char) * load->custom_lth);
	} else load->custom = (char *) 0;

	return load;
}

int
dooverview()
{
	winid win;
	mapseen *mptr;
	boolean first;
	boolean printdun;
	int lastdun;

	first = TRUE;

	if (DisplayLoss || u.uprops[DISPLAY_LOST].extrinsic || have_displaystone()) {

		pline("Haha, you probably wish you could do that.");
		return 0;

	}

	if (uarmc && uarmc->oartifact == ART_CLOAK_OF_THE_CONSORT) {

		pline("Well, I'm sure you'd love to see the overview, but no. Sorry.");
		return 0;

	}

	win = create_nhwindow(NHW_MENU);

	for (mptr = mapseenchn; mptr; mptr = mptr->next) {

		/* only print out info for a level or a dungeon if interest */
		if (/*interest_mapseen(mptr)*/TRUE) {
			printdun = (first || lastdun != mptr->lev.dnum);
			/* if (!first) putstr(win, 0, ""); */
			if (/*interest_mapseen(mptr)*/TRUE) {
				print_mapseen(win, mptr, printdun, FALSE, FALSE);
			}

			if (printdun) {
				first = FALSE;
				lastdun = mptr->lev.dnum;
			}
		}
	}

	display_nhwindow(win, TRUE);
	destroy_nhwindow(win);

	return 0;
}

#ifdef DUMP_LOG
void 
dump_overview()
{
	winid win;
	mapseen *mptr;
	boolean first;
	boolean printdun;
	int lastdun;

	first = TRUE;

	for (mptr = mapseenchn; mptr; mptr = mptr->next) {

		/* only print out info for a level or a dungeon if interest */
		if (/*interest_mapseen(mptr)*/TRUE) {
			printdun = (first || lastdun != mptr->lev.dnum);

			if (first) {
				/* Always print header as there will at least
				 * be the output of the current level */
				dump("", "Dungeon overview");
			}
			print_mapseen(win, mptr, printdun, TRUE, TRUE);

			if (printdun) {
				first = FALSE;
				lastdun = mptr->lev.dnum;
			}
		}
	}

	dump("", "");

	return;

}
#endif

STATIC_OVL void
print_mapseen(win, mptr, printdun, wantdump, alwaysdisplay)
winid win;
mapseen *mptr;
boolean printdun;
boolean wantdump;
boolean alwaysdisplay;
{
	char buf[BUFSZ];
	int i, depthstart;

	/* Damnable special cases */
	/* The quest and knox should appear to be level 1 to match
	 * other text.
	 * Amy edit: it's BS if Ludios doesn't display its actual depth
	 */
	if (mptr->lev.dnum == quest_dnum /* || mptr->lev.dnum == knox_level.dnum */ )
		depthstart = 1;
	else
		depthstart = dungeons[mptr->lev.dnum].depth_start;  

	if (printdun) {
		/* Sokoban lies about dunlev_ureached and we should
		 * suppress the negative numbers in the endgame.
		 * Amy edit: fuck that, the branch lengths are always the same anyway so might as well see it
		 */
		if (dungeons[mptr->lev.dnum].dunlev_ureached == 1 || In_endgame(&mptr->lev))
			sprintf(buf, "%s: level %d", dungeons[mptr->lev.dnum].dname, depthstart);
		else
			sprintf(buf, "%s: levels %d to %d", 
				dungeons[mptr->lev.dnum].dname,
				depthstart, depthstart + 
				dungeons[mptr->lev.dnum].dunlev_ureached - 1);
		if (!wantdump) {
			putstr(win, ATR_INVERSE, buf);
		} else {
#ifdef DUMP_LOG
			dump("  ", buf);
#endif
		}
	}

	/* in-game, we don't need it to display every single level... but in the dumplog we want to see everything --Amy */
	if (!alwaysdisplay && !wantdump && !(RngeOverviewImprovement && Is_special(&mptr->lev)) && !(wizard && Is_special(&mptr->lev)) && !(mptr->custom) && !(on_level(&u.uz, &mptr->lev)) )
		return;

	/* calculate level number */
	i = depthstart + mptr->lev.dlevel - 1;
	if (Is_astralevel(&mptr->lev))
		sprintf(buf, "Astral Plane:");
	else if (In_endgame(&mptr->lev))
		/* Negative numbers are mildly confusing, since they are never
		 * shown to the player, except in wizard mode.  We could show
		 * "Level -1" for the earth plane, for example.  Instead,
		 * show "Plane 1" for the earth plane to differentiate from
		 * level 1.  There's not much to show, but maybe the player
		 * wants to #annotate them for some bizarre reason.
		 */
		sprintf(buf, "Plane %i:", -i);
	else
		sprintf(buf, "Level %d:", i);
	
#ifdef WIZARD
	/* wizmode prints out proto dungeon names for clarity
	 * Amy edit: and so does the dumplog, for that matter */
	if (wizard || wantdump || RngeOverviewImprovement) {
		s_level *slev;
		if (slev = Is_special(&mptr->lev))
			sprintf(eos(buf), " [%s]", slev->proto);
	}
#endif

	if (mptr->custom)
		sprintf(eos(buf), " (%s)", mptr->custom);

	/* print out glyph or something more interesting? */
	sprintf(eos(buf), "%s", on_level(&u.uz, &mptr->lev) ? 
		" <- You are here" : "");
	if (!wantdump) {
		putstr(win, ATR_BOLD, buf);
	} else {
#ifdef DUMP_LOG
			dump("  ", buf);
#endif
	}

}

/* returns true if this level has something interesting to print out */
STATIC_OVL boolean
interest_mapseen(mptr)
mapseen *mptr;
{
	return (on_level(&u.uz, &mptr->lev) || (mptr->custom));
}

void
init_mapseen(lev)
d_level *lev;
{
	/* Create a level and insert in "sorted" order.  This is an insertion
	 * sort first by dungeon (in order of discovery) and then by level number.
	 */
	mapseen *mptr;
	mapseen *init;
	mapseen *old;
	
	init = (mapseen *) alloc(sizeof(mapseen));
	(void) memset((void *)init, 0, sizeof(mapseen));
	init->lev.dnum = lev->dnum;
	init->lev.dlevel = lev->dlevel;

	if (!mapseenchn) {
		mapseenchn = init;
		return;
	}

	/* walk until we get to the place where we should
	 * insert init between mptr and mptr->next
	 */
	for (mptr = mapseenchn; mptr->next; mptr = mptr->next) {
		if (mptr->next->lev.dnum == init->lev.dnum) break;
	}
	for (; mptr->next; mptr = mptr->next) {
		if ((mptr->next->lev.dnum != init->lev.dnum) ||
			(mptr->next->lev.dlevel > init->lev.dlevel)) break;
	}

	old = mptr->next;
	mptr->next = init;
	init->next = old;
}

/*dungeon.c*/
