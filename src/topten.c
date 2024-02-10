/*	SCCS Id: @(#)topten.c	3.4	2000/01/21	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "dlb.h"
#include "patchlevel.h"

#ifdef VMS
 /* We don't want to rewrite the whole file, because that entails	 */
 /* creating a new version which requires that the old one be deletable. */
# define UPDATE_RECORD_IN_PLACE
#endif

/*
 * Updating in place can leave junk at the end of the file in some
 * circumstances (if it shrinks and the O.S. doesn't have a straightforward
 * way to truncate it).  The trailing junk is harmless and the code
 * which reads the scores will ignore it.
 */
#ifdef UPDATE_RECORD_IN_PLACE
static long final_fpos;
#endif

#define done_stopprint program_state.stopprint

#define newttentry() (struct toptenentry *) alloc(sizeof(struct toptenentry))
#define dealloc_ttentry(ttent) free((void *) (ttent))
#define NAMSZ	10	/* several parts of this code depend on this, don't just change this value! --Amy */
#define DTHSZ	1000
#define ROLESZ   3
#define PERSMAX	 10000		/* entries per name/uid per char. allowed */
#define POINTSMIN	1	/* must be > 0 */
#define ENTRYMAX	10000	/* must be >= 10 */

#if !defined(MICRO) && !defined(MAC) && !defined(WIN32) && !defined(PUBLIC_SERVER)
#define PERS_IS_UID		/* delete for PERSMAX per name; now per uid */
#endif
struct toptenentry {
	struct toptenentry *tt_next;
#ifdef UPDATE_RECORD_IN_PLACE
	long fpos;
#endif
	long points;
	int deathdnum, deathlev;
	int maxlvl, hp, maxhp, deaths;
	int ver_major, ver_minor, patchlevel;
	long deathdate, birthdate;
#ifdef RECORD_CONDUCT
	long conduct;
#endif
	int uid;
	char plrole[ROLESZ+1];
	char plrace[ROLESZ+1];
	char plgend[ROLESZ+1];
	char plalign[ROLESZ+1];
	char name[NAMSZ+1];
	char death[DTHSZ+1];
} *tt_head;

STATIC_DCL void topten_print(const char *);
STATIC_DCL void topten_print_bold(const char *);
STATIC_DCL xchar observable_depth(d_level *);
STATIC_DCL void outheader(void);
STATIC_DCL void outentry(int,struct toptenentry *,BOOLEAN_P);
STATIC_DCL void readentry(FILE *,struct toptenentry *);
STATIC_DCL void writeentry(FILE *,struct toptenentry *);
STATIC_DCL void free_ttlist(struct toptenentry *);
#ifdef XLOGFILE
STATIC_DCL void write_xlentry(FILE *,struct toptenentry *);
STATIC_DCL long encodexlogflags(void);
#endif
STATIC_DCL int classmon(char *,BOOLEAN_P);
STATIC_DCL int score_wanted(BOOLEAN_P, int,struct toptenentry *,int,const char **,int);
#ifdef RECORD_CONDUCT
STATIC_DCL long encodeconduct(void);
#endif
#ifdef RECORD_ACHIEVE
STATIC_DCL long encodeachieve(void);
STATIC_DCL char* encodeachieveX(void);
#endif
#ifdef NO_SCAN_BRACK
STATIC_DCL void nsb_mung_line(char*);
STATIC_DCL void nsb_unmung_line(char*);
#endif

/* must fit with end.c; used in rip.c */
NEARDATA const char * const killed_by_prefix[] = {
	"killed by ", "betrayed by ", "choked on ", "poisoned by ", "died of ", 
	"drowned in ", "burned by ", "dissolved in ", "crushed to death by ", 
	"petrified by ", "turned to slime by ", "killed by ", 
	"", "", "", "", ""
};

static winid toptenwin = WIN_ERR;

#ifdef RECORD_START_END_TIME
static time_t deathtime = 0L;
#endif

STATIC_OVL void
topten_print(x)
const char *x;
{
	if (toptenwin == WIN_ERR)
	    raw_print(x);
	else
	    putstr(toptenwin, ATR_NONE, x);
}

STATIC_OVL void
topten_print_bold(x)
const char *x;
{
	if (toptenwin == WIN_ERR)
	    raw_print_bold(x);
	else
	    putstr(toptenwin, ATR_BOLD, x);
}

STATIC_OVL xchar
observable_depth(lev)
d_level *lev;
{
#if 0	/* if we ever randomize the order of the elemental planes, we
	   must use a constant external representation in the record file */
	if (In_endgame(lev)) {
	    if (Is_astralevel(lev))	 return -5;
	    else if (Is_waterlevel(lev)) return -4;
	    else if (Is_firelevel(lev))	 return -3;
	    else if (Is_airlevel(lev))	 return -2;
	    else if (Is_earthlevel(lev)) return -1;
	    else			 return 0;	/* ? */
	} else
#endif
	    return depth(lev);
}

#ifdef RECORD_CONDUCT
long
encodeconduct(void)
{
       long e = 0L;

       if(u.uconduct.unvegetarian)    e |= 0x1L;
       if(u.uconduct.unvegan)         e |= 0x2L;
       if(u.uconduct.food)            e |= 0x4L;
       if(u.uconduct.gnostic)         e |= 0x8L;
       if(u.uconduct.weaphit)         e |= 0x10L;
       if(u.uconduct.killer)          e |= 0x20L;
       if(u.uconduct.literate)        e |= 0x40L;
       if(u.uconduct.polypiles)       e |= 0x80L;
       if(u.uconduct.polyselfs)       e |= 0x100L;
       if(u.uconduct.wishes)          e |= 0x200L;
       if(u.uconduct.wisharti)        e |= 0x400L;
       if(num_genocides())            e |= 0x800L;
       if(u.uconduct.praydone)        e |= 0x1000L;

       return e;
}
#endif

STATIC_OVL void
readentry(rfile,tt)
FILE *rfile;
struct toptenentry *tt;
{
#ifdef NO_SCAN_BRACK		/* Version_ Pts DgnLevs_ Hp___ Died__Born id */
	static const char fmt[] = "%d %d %d %ld %d %d %d %d %d %d %ld %ld %d%*c";
	static const char fmt005[] = "%s %c %s %s%*c";
	static const char fmt33[] = "%s %s %s %s %s %s%*c";
#else
	static const char fmt[] = "%d.%d.%d %ld %d %d %d %d %d %d %ld %ld %d ";
	static const char fmt005[] = "%s %c %[^,],%[^\n]%*c";
	static const char fmt33[] = "%s %s %s %s %[^,],%[^\n]%*c";
#endif

#ifdef UPDATE_RECORD_IN_PLACE
	/* note: fscanf() below must read the record's terminating newline */
	final_fpos = tt->fpos = ftell(rfile);
#endif
#define TTFIELDS 13

#ifdef RECORD_CONDUCT
	tt->conduct = 8191;
#endif

	if(fscanf(rfile, fmt,
			&tt->ver_major, &tt->ver_minor, &tt->patchlevel,
			&tt->points, &tt->deathdnum, &tt->deathlev,
			&tt->maxlvl, &tt->hp, &tt->maxhp, &tt->deaths,
			&tt->deathdate, &tt->birthdate,
			&tt->uid) != TTFIELDS)
#undef TTFIELDS
		tt->points = 0;
	else {
		/* Check for backwards compatibility */
		if (!tt->ver_major && !tt->ver_minor && tt->patchlevel < 6) {
			int i;

		    if (fscanf(rfile, fmt005,
				tt->plrole, tt->plgend,
				tt->name, tt->death) != 4)
			tt->points = 0;
		    tt->plrole[1] = '\0';
		    if ((i = str2role(tt->plrole)) >= 0)
			strcpy(tt->plrole, roles[i].filecode);
		    tt->plrole[ROLESZ] = 0;
		    strcpy(tt->plrace, "?");
		    strcpy(tt->plgend, (tt->plgend[0] == 'M') ? "Mal" : "Fem");
		    strcpy(tt->plalign, "?");
		} else if (fscanf(rfile, fmt33,
				tt->plrole, tt->plrace, tt->plgend,
				tt->plalign, tt->name, tt->death) != 6)
			tt->points = 0;
#ifdef NO_SCAN_BRACK
		if(tt->points > 0) {
			nsb_unmung_line(tt->name);
			nsb_unmung_line(tt->death);
		}
#endif

#ifdef RECORD_CONDUCT
		if(tt->points > 0) {
			/* If the string "Conduct=%d" appears, set tt->conduct and remove that
			 * portion of the string */
			char *dp, *dp2;
			for(dp = tt->death; *dp; dp++) {
				if(!strncmp(dp, " Conduct=", 9)) {
					dp2 = dp + 9;
					sscanf(dp2, "%ld", &tt->conduct);
					/* Find trailing null or space */
					while(*dp2 && *dp2 != ' ')
						dp2++;

					/* Cut out the " Conduct=" portion of the death string */
					while(*dp2) {
						*dp = *dp2;
						dp2++;
						dp++;
					}
					
					*dp = *dp2;
				}
			}

			/* Sanity check */
			if(tt->conduct < 0 || tt->conduct > 8191)
				tt->conduct = 8191;
		}
#endif

	}

	/* check old score entries for Y2K problem and fix whenever found */
	if (tt->points > 0) {
		if (tt->birthdate < 19000000L) tt->birthdate += 19000000L;
		if (tt->deathdate < 19000000L) tt->deathdate += 19000000L;
	}
}

STATIC_OVL void
writeentry(rfile,tt)
FILE *rfile;
struct toptenentry *tt;
{
#ifdef RECORD_CONDUCT
	char *cp = eos(tt->death);

	/* Add a trailing " Conduct=%d" to tt->death */
	/*if(tt->conduct != 8191) {*/
		cp = tt->death + strlen(tt->death);
		sprintf(cp, " Conduct=%ld", tt->conduct);
	/*}*/
#endif

#ifdef NO_SCAN_BRACK
	nsb_mung_line(tt->name);
	nsb_mung_line(tt->death);
	                   /* Version_ Pts DgnLevs_ Hp___ Died__Born id */
	(void) fprintf(rfile,"%d %d %d %ld %d %d %d %d %d %d %ld %ld %d ",
#else
	(void) fprintf(rfile,"%d.%d.%d %ld %d %d %d %d %d %d %ld %ld %d ",
#endif
		tt->ver_major, tt->ver_minor, tt->patchlevel,
		tt->points, tt->deathdnum, tt->deathlev,
		tt->maxlvl, tt->hp, tt->maxhp, tt->deaths,
		tt->deathdate, tt->birthdate, tt->uid);
	if (!tt->ver_major && !tt->ver_minor && tt->patchlevel < 6)
#ifdef NO_SCAN_BRACK
		(void) fprintf(rfile,"%s %c %s %s\n",
#else
		(void) fprintf(rfile,"%s %c %s,%s\n",
#endif
			tt->plrole, tt->plgend[0],
			onlyspace(tt->name) ? "_" : tt->name, tt->death);
	else
#ifdef NO_SCAN_BRACK
		(void) fprintf(rfile,"%s %s %s %s %s %s\n",
#else
		(void) fprintf(rfile,"%s %s %s %s %s,%s\n",
#endif
			tt->plrole, tt->plrace, tt->plgend, tt->plalign,
			onlyspace(tt->name) ? "_" : tt->name, tt->death);

#ifdef NO_SCAN_BRACK
	nsb_unmung_line(tt->name);
	nsb_unmung_line(tt->death);
#endif

#ifdef RECORD_CONDUCT
	/* Return the tt->death line to the original form */
	*cp = '\0';
#endif
}

#ifdef XLOGFILE
/* Use \t instead of ":" so that we don't have to mangle anything else (3.6.0)*/
#define XLOG_SEP "\t"

STATIC_OVL void
write_xlentry(rfile,tt)
FILE *rfile;
struct toptenentry *tt;
{

  char buf[DTHSZ+1];

  /* Log all of the data found in the regular logfile */
  (void)fprintf(rfile,
                "version=slex-%d.%d.%d"
                XLOG_SEP "points=%ld"
                XLOG_SEP "deathdnum=%d"
                XLOG_SEP "deathlev=%d"
                XLOG_SEP "maxlvl=%d"
                XLOG_SEP "hp=%d"
                XLOG_SEP "maxhp=%d"
                XLOG_SEP "deaths=%d"
                XLOG_SEP "deathdate=%ld"
                XLOG_SEP "birthdate=%ld"
                XLOG_SEP "uid=%d",
                tt->ver_major, tt->ver_minor, tt->patchlevel,
                tt->points, tt->deathdnum, tt->deathlev,
                tt->maxlvl, tt->hp, tt->maxhp, tt->deaths,
                tt->deathdate, tt->birthdate, tt->uid);

  (void)fprintf(rfile,
                XLOG_SEP "role=%s"
                XLOG_SEP "race=%s"
                XLOG_SEP "gender=%s"
                XLOG_SEP "align=%s",
                tt->plrole, tt->plrace, tt->plgend, tt->plalign);
   
  (void)fprintf(rfile, XLOG_SEP "hybrid=%s", hybrid_strcode());

  (void)fprintf(rfile, XLOG_SEP "gamemode=%s", gamemode_strcode());

  (void)fprintf(rfile, XLOG_SEP "name=%s", plname);

  (void)fprintf(rfile, XLOG_SEP "alias=%s", plalias);

  (void)fprintf(rfile, XLOG_SEP "death=%s", tt->death);

#ifdef RECORD_CONDUCT
  (void)fprintf(rfile, XLOG_SEP "conduct=0x%lx", 0x1fffL & ~encodeconduct());
#endif

#ifdef RECORD_TURNS
  (void)fprintf(rfile, XLOG_SEP "turns=%ld", moves);
#endif

#ifdef RECORD_ACHIEVE
  (void)fprintf(rfile, XLOG_SEP "achieve=0x%lx", encodeachieve());
  (void)fprintf(rfile, XLOG_SEP "achieveX=%s", encodeachieveX());
#endif

#ifdef RECORD_REALTIME
  (void)fprintf(rfile, XLOG_SEP "realtime=%ld", (long)realtime_data.realtime);
#endif

#ifdef RECORD_START_END_TIME
  (void)fprintf(rfile, XLOG_SEP "starttime=%ld", (long)u.ubirthday);
  (void)fprintf(rfile, XLOG_SEP "endtime=%ld", (long)deathtime);
#endif

	/* Amy addition: unlike any other variant, your role and race can change during gameplay. This is for junethack,
	 * where a certain competition is about ascending as many as possible starting combinations. */
  (void)fprintf(rfile, XLOG_SEP "role0=%s", ustartrole.filecode);

  (void)fprintf(rfile, XLOG_SEP "race0=%s", ustartrace.filecode);

#ifdef RECORD_GENDER0
  (void)fprintf(rfile, XLOG_SEP "gender0=%s", genders[flags.initgend].filecode);
#endif

#ifdef RECORD_ALIGN0
  (void)fprintf(rfile, XLOG_SEP "align0=%s", 
          aligns[1 - u.ualignbase[A_ORIGINAL]].filecode);
#endif

  fprintf(rfile, XLOG_SEP "flags=0x%lx", encodexlogflags());

  (void)fprintf(rfile,"%s", "\n");

}

static long encodexlogflags(void) {
	long tmp = 0L;

	if (wizard)
		tmp |= 1L << 0;
	if (discover)
		tmp |= 1L << 1;
	if (u.freeplaymode)
		tmp |= 1L << 2;

	return tmp;
}

#undef XLOG_SEP
#endif /* XLOGFILE */

STATIC_OVL void
free_ttlist(tt)
struct toptenentry *tt;
{
	struct toptenentry *ttnext;

	while (tt->points > 0) {
		ttnext = tt->tt_next;
		dealloc_ttentry(tt);
		tt = ttnext;
	}
	dealloc_ttentry(tt);
}

void
topten(how)
int how;
{
	int uid = getuid();
	int rank, rank0 = -1, rank1 = 0;
	int occ_cnt = PERSMAX;
	register struct toptenentry *t0, *tprev;
	struct toptenentry *t1;
	FILE *rfile;
	register int flg = 0;
	boolean t0_used;
#ifdef LOGFILE
	FILE *lfile;
#endif /* LOGFILE */
#ifdef XLOGFILE
	FILE *xlfile;
#endif /* XLOGFILE */

/* Under DICE 3.0, this crashes the system consistently, apparently due to
 * corruption of *rfile somewhere.  Until I figure this out, just cut out
 * topten support entirely - at least then the game exits cleanly.  --AC
 */
#ifdef _DCC
	return;
#endif

/* If we are in the midst of a panic, cut out topten entirely.
 * topten uses alloc() several times, which will lead to
 * problems if the panic was the result of an alloc() failure.
 */
	if (program_state.panicking)
		return;

	if (flags.toptenwin) {
	    toptenwin = create_nhwindow(NHW_TEXT);
	}

#if defined(UNIX) || defined(VMS) || defined(__EMX__)
#define HUP	if (!program_state.done_hup)
#else
#define HUP
#endif

#ifdef TOS
	restore_colors();	/* make sure the screen is black on white */
#endif
	/* create a new 'topten' entry */
	t0_used = FALSE;
	t0 = newttentry();
	/* deepest_lev_reached() is in terms of depth(), and reporting the
	 * deepest level reached in the dungeon death occurred in doesn't
	 * seem right, so we have to report the death level in depth() terms
	 * as well (which also seems reasonable since that's all the player
	 * sees on the screen anyway)
	 */
	t0->ver_major = VERSION_MAJOR;
	t0->ver_minor = VERSION_MINOR;
	t0->patchlevel = PATCHLEVEL;
	t0->points = u.urexp;
	t0->deathdnum = u.uz.dnum;
	t0->deathlev = observable_depth(&u.uz);
	t0->maxlvl = deepest_lev_reached(TRUE);
	t0->hp = u.uhp;
	t0->maxhp = u.uhpmax;
	t0->deaths = u.umortality;
	t0->uid = uid;
	(void) strncpy(t0->plrole, urole.filecode, ROLESZ);
	t0->plrole[ROLESZ] = '\0';
	(void) strncpy(t0->plrace, urace.filecode, ROLESZ);
	t0->plrace[ROLESZ] = '\0';
	(void) strncpy(t0->plgend, genders[flags.female].filecode, ROLESZ);
	t0->plgend[ROLESZ] = '\0';
	(void) strncpy(t0->plalign, aligns[1-u.ualign.type].filecode, ROLESZ);
	t0->plalign[ROLESZ] = '\0';
	(void) strncpy(t0->name, playeraliasname, NAMSZ);
	t0->name[NAMSZ] = '\0';
	t0->death[0] = '\0';
	switch (killer_format) {
		default: impossible("bad killer format?");
		case KILLED_BY_AN:
			strcat(t0->death, killed_by_prefix[how]);
			(void) strncat(t0->death, an(killer),
						DTHSZ-strlen(t0->death));
			break;
		case KILLED_BY:
			strcat(t0->death, killed_by_prefix[how]);
			(void) strncat(t0->death, killer,
						DTHSZ-strlen(t0->death));
			break;
		case NO_KILLER_PREFIX:
			(void) strncat(t0->death, killer, DTHSZ);
			break;
	}
	t0->birthdate = yyyymmdd(u.ubirthday);
	t0->deathdate = yyyymmdd((time_t)0L);

#ifdef RECORD_START_END_TIME
  /* Make sure that deathdate and deathtime refer to the same time; it
   * wouldn't be good to have deathtime refer to the day after deathdate. */

#if defined(BSD) && !defined(POSIX_TYPES)
        (void) time((long *)&deathtime);
#else
        (void) time(&deathtime);
#endif

        t0->deathdate = yyyymmdd(deathtime);
#else
        t0->deathdate = yyyymmdd((time_t)0L);
#endif /* RECORD_START_END_TIME */

#ifdef RECORD_CONDUCT
	t0->conduct = encodeconduct();
#endif
	t0->tt_next = 0;
#ifdef UPDATE_RECORD_IN_PLACE
	t0->fpos = -1L;
#endif

#ifdef LOGFILE		/* used for debugging (who dies of what, where) */
#ifdef FILE_AREAS
	if (lock_file_area(LOGAREA, LOGFILE, 10)) {
#else
	if (lock_file(LOGFILE, SCOREPREFIX, 10)) {
#endif
	    if(!(lfile = fopen_datafile_area(LOGAREA, LOGFILE, "a", SCOREPREFIX))) {
		HUP raw_print("Cannot open log file!");
	    } else {
		writeentry(lfile, t0);
		(void) fclose(lfile);
	    }
	    unlock_file_area(LOGAREA, LOGFILE);
	}
#endif /* LOGFILE */

#ifdef XLOGFILE
#ifdef FILE_AREAS
         if(lock_file_area(LOGAREA, XLOGFILE, 10)) {
#else
         if(lock_file(XLOGFILE, SCOREPREFIX, 10)) {
#endif
             if(!(xlfile = fopen_datafile_area(LOGAREA, XLOGFILE, "a", SCOREPREFIX))) {
                  HUP raw_print("Cannot open extended log file!");
             } else {
                  write_xlentry(xlfile, t0);
                  (void) fclose(xlfile);
             }
             unlock_file_area(LOGAREA, XLOGFILE);
         }
#endif /* XLOGFILE */

	if (wizard || discover || u.freeplaymode) {
	    if (how != PANICKED) HUP {
		char pbuf[BUFSZ];
		topten_print("");
		sprintf(pbuf,
	      "Since you were in %s mode, the score list will not be checked.",
		    wizard ? "wizard" : u.freeplaymode ? "freeplay" : "discover");
		topten_print(pbuf);
#ifdef DUMP_LOG
		if (dump_fn[0]) {
		  dump("", pbuf);
		  dump("", "");
		}
#endif
	    }
	    goto showwin;
	}

	/* "Forget Quitters" patch - Elronnd suggested that if the game went on for long enough, it should count --Amy */
	if (how == QUIT && (moves && moves < 100)) {
	    char pbuf[]="Since you quit, the score list will not be checked.";
	    topten_print("");
	    topten_print(pbuf);
#ifdef DUMP_LOG
	    if (dump_fn[0]) {
		dump("", pbuf);
		dump("", "");
	    }
#endif
	    goto showwin;
	}

	/* some startscummers will escape instead, but probably very early --Amy */
	if (how == ESCAPED && (moves && moves < 10)) {
	    char pbuf[]="Since you escaped early, the score list will not be checked.";
	    topten_print("");
	    topten_print(pbuf);
#ifdef DUMP_LOG
	    if (dump_fn[0]) {
		dump("", pbuf);
		dump("", "");
	    }
#endif
	    goto showwin;
	}

#ifdef FILE_AREAS
	if (!lock_file_area(NH_RECORD_AREA, NH_RECORD, 60))
#else
	if (!lock_file(NH_RECORD, SCOREPREFIX, 60))
#endif
		goto destroywin;

#ifdef UPDATE_RECORD_IN_PLACE
	rfile = fopen_datafile_area(NH_RECORD_AREA, NH_RECORD, "r+", SCOREPREFIX);
#else
	rfile = fopen_datafile_area(NH_RECORD_AREA, NH_RECORD, "r", SCOREPREFIX);
#endif

	if (!rfile) {
		HUP raw_print("Cannot open record file!");
		unlock_file_area(NH_RECORD_AREA, NH_RECORD);
		goto destroywin;
	}

	HUP topten_print("");
#ifdef DUMP_LOG
	dump("", "");
#endif

	/* assure minimum number of points */
	if(t0->points < POINTSMIN) t0->points = 0;

	t1 = tt_head = newttentry();
	tprev = 0;
	/* rank0: -1 undefined, 0 not_on_list, n n_th on list */
	for(rank = 1; ; ) {
	    readentry(rfile, t1);
	    if (t1->points < POINTSMIN) t1->points = 0;
	    if(rank0 < 0 && t1->points < t0->points) {
		rank0 = rank++;
		if(tprev == 0)
			tt_head = t0;
		else
			tprev->tt_next = t0;
		t0->tt_next = t1;
#ifdef UPDATE_RECORD_IN_PLACE
		t0->fpos = t1->fpos;	/* insert here */
#endif
		t0_used = TRUE;
		occ_cnt--;
		flg++;		/* ask for a rewrite */
	    } else tprev = t1;

	    if(t1->points == 0) break;
	    if(
#ifdef PERS_IS_UID
		t1->uid == t0->uid &&
#else
		strncmp(t1->name, t0->name, NAMSZ) == 0 &&
#endif
		!strncmp(t1->plrole, t0->plrole, ROLESZ) &&
		--occ_cnt <= 0) {
		    if(rank0 < 0) {
			rank0 = 0;
			rank1 = rank;
			HUP {
			    char pbuf[BUFSZ];
			    sprintf(pbuf,
			  "You didn't beat your previous score of %ld points.",
				    t1->points);
			    topten_print(pbuf);
			    topten_print("");
#ifdef DUMP_LOG
			    dump("", pbuf);
			    dump("", "");
#endif
			}
		    }
		    if(occ_cnt < 0) {
			flg++;
			continue;
		    }
		}
	    if(rank <= ENTRYMAX) {
		t1->tt_next = newttentry();
		t1 = t1->tt_next;
		rank++;
	    }
	    if(rank > ENTRYMAX) {
		t1->points = 0;
		break;
	    }
	}
	if(flg) {	/* rewrite record file */
#ifdef UPDATE_RECORD_IN_PLACE
		(void) fseek(rfile, (t0->fpos >= 0 ?
				     t0->fpos : final_fpos), SEEK_SET);
#else
		(void) fclose(rfile);
		if(!(rfile = fopen_datafile_area(NH_RECORD_AREA, NH_RECORD, "w", SCOREPREFIX))){
			HUP raw_print("Cannot write record file");
			unlock_file_area(NH_RECORD_AREA, NH_RECORD);
			free_ttlist(tt_head);
			goto destroywin;
		}
#endif	/* UPDATE_RECORD_IN_PLACE */
		if(!done_stopprint) if(rank0 > 0){
		    if(rank0 <= 10) {
			topten_print("You made the top ten list!");
#ifdef DUMP_LOG
                       dump("", "You made the top ten list!");
#endif
		     } else {
			char pbuf[BUFSZ];
			sprintf(pbuf,
			  "You reached the %d%s place on the top %d list.",
				rank0, ordin(rank0), ENTRYMAX);
			topten_print(pbuf);
#ifdef DUMP_LOG
                       dump("", pbuf);
#endif
		    }
			
		    topten_print("");
#ifdef DUMP_LOG
                   dump("", "");
#endif

		}
	}
	if(rank0 == 0) rank0 = rank1;
	if(rank0 <= 0) rank0 = rank;
	if(!done_stopprint) outheader();
	t1 = tt_head;
	for(rank = 1; t1->points != 0; rank++, t1 = t1->tt_next) {
	    if(flg
#ifdef UPDATE_RECORD_IN_PLACE
		    && rank >= rank0
#endif
		) writeentry(rfile, t1);
	    if (done_stopprint) continue;
	    if (rank > flags.end_top &&
		    (rank < rank0 - flags.end_around ||
		     rank > rank0 + flags.end_around) &&
		    (!flags.end_own ||
#ifdef PERS_IS_UID
					t1->uid != t0->uid
#else
					strncmp(t1->name, t0->name, NAMSZ)
#endif
		)) continue;
	    if (rank == rank0 - flags.end_around &&
		    rank0 > flags.end_top + flags.end_around + 1 &&
		    !flags.end_own) {
		topten_print("");
#ifdef DUMP_LOG
		dump("", "");
#endif
	    }
	    if(rank != rank0)
		outentry(rank, t1, FALSE);
	    else if(!rank1)
		outentry(rank, t1, TRUE);
	    else {
		outentry(rank, t1, TRUE);
		outentry(0, t0, TRUE);
	    }
	}
	if(rank0 >= rank) if(!done_stopprint)
		outentry(0, t0, TRUE);
#ifdef UPDATE_RECORD_IN_PLACE
	if (flg) {
# ifdef TRUNCATE_FILE
	    /* if a reasonable way to truncate a file exists, use it */
	    truncate_file(rfile);
# else
	    /* use sentinel record rather than relying on truncation */
	    t1->points = 0L;	/* terminates file when read back in */
	    t1->ver_major = t1->ver_minor = t1->patchlevel = 0;
	    t1->uid = t1->deathdnum = t1->deathlev = 0;
	    t1->maxlvl = t1->hp = t1->maxhp = t1->deaths = 0;
	    t1->plrole[0] = t1->plrace[0] = t1->plgend[0] = t1->plalign[0] = '-';
	    t1->plrole[1] = t1->plrace[1] = t1->plgend[1] = t1->plalign[1] = 0;
	    t1->birthdate = t1->deathdate = yyyymmdd((time_t)0L);
	    strcpy(t1->name, "@");
	    strcpy(t1->death, "<eod>\n");
	    writeentry(rfile, t1);
	    (void) fflush(rfile);
# endif	/* TRUNCATE_FILE */
	}
#endif	/* UPDATE_RECORD_IN_PLACE */
	(void) fclose(rfile);
	unlock_file_area(NH_RECORD_AREA, NH_RECORD);
	free_ttlist(tt_head);

  showwin:
	if (flags.toptenwin && !done_stopprint) display_nhwindow(toptenwin, 1);
  destroywin:
	if (!t0_used) dealloc_ttentry(t0);
	if (flags.toptenwin) {
	    destroy_nhwindow(toptenwin);
	    toptenwin=WIN_ERR;
	}
}

STATIC_OVL void
outheader()
{
	char linebuf[BUFSZ];
	register char *bp;

	strcpy(linebuf, " No  Points     Name");
	bp = eos(linebuf);
	while(bp < linebuf + COLNO - 9) *bp++ = ' ';
	strcpy(bp, "Hp [max]");
	topten_print(linebuf);
#ifdef DUMP_LOG
	dump("", linebuf);
#endif
}

/* so>0: standout line; so=0: ordinary line */
STATIC_OVL void
outentry(rank, t1, so)
struct toptenentry *t1;
int rank;
boolean so;
{
	boolean second_line = TRUE;
	char linebuf[BUFSZ];
	char *bp, hpbuf[24], linebuf3[BUFSZ];
	int hppos, lngr;


	linebuf[0] = '\0';
	if (rank) sprintf(eos(linebuf), "%3d", rank);
	else strcat(linebuf, "   ");

	sprintf(eos(linebuf), " %10ld  %.10s", t1->points, t1->name);
	sprintf(eos(linebuf), "-%s", t1->plrole);
	if (t1->plrace[0] != '?')
		sprintf(eos(linebuf), "-%s", t1->plrace);
	/* Printing of gender and alignment is intentional.  It has been
	 * part of the NetHack Geek Code, and illustrates a proper way to
	 * specify a character from the command line.
	 */
	sprintf(eos(linebuf), "-%s", t1->plgend);
	if (t1->plalign[0] != '?')
		sprintf(eos(linebuf), "-%s ", t1->plalign);
	else
		strcat(linebuf, " ");
	if (!strncmp("escaped", t1->death, 7)) {
	    sprintf(eos(linebuf), "escaped the dungeon %s[max level %d]",
		    !strncmp(" (", t1->death + 7, 2) ? t1->death + 7 + 2 : "",
		    t1->maxlvl);
	    /* fixup for closing paren in "escaped... with...Amulet)[max..." */
	    if ((bp = index(linebuf, ')')) != 0)
		*bp = (t1->deathdnum == astral_level.dnum) ? '\0' : ' ';
	    second_line = FALSE;
	} else if (!strncmp("ascended", t1->death, 8)) {

#ifdef RECORD_CONDUCT
		/* Add a notation for conducts kept */
		if(t1->conduct != 4095) {
			int i, m;
			char dash = 0, skip;
			const char *conduct_names[] = {
				"Food", "Vgn", "Vgt", "Ath", "Weap", "Pac",
				"Ill", "Poly", "Form", "Wish", "Art", "Geno",
				NULL };
		
			strcat(eos(linebuf), "(");
			for(i = 0, m = 1; conduct_names[i]; i += skip + 1, m <<= (skip + 1)) {
				skip = 0;
				if(t1->conduct & m)
					continue;
		
				/* Only show one of foodless, vegan, vegetarian */
				if(i == 0) skip = 2;
				if(i == 1) skip = 1;

				/* Only show one of wishless, artiwishless */
				if(i == 9) skip = 1;

				/* Add a hyphen for multiple conducts */
				if(dash) strcat(eos(linebuf), "-");
				strcat(eos(linebuf), conduct_names[i]);
				dash = 1;
			}
			strcat(eos(linebuf), ") ");
		}
#endif

	    sprintf(eos(linebuf), "ascended to demigod%s-hood",
		    (t1->plgend[0] == 'F') ? "dess" : "");
	    second_line = FALSE;
	} else {
	    if (!strncmp(t1->death, "quit", 4)) {
		strcat(linebuf, "quit");
		second_line = FALSE;
	    } else if (!strncmp(t1->death, "died of st", 10)) {
		strcat(linebuf, "starved to death");
		second_line = FALSE;
	    } else if (!strncmp(t1->death, "choked", 6)) {
		sprintf(eos(linebuf), "choked on h%s food",
			(t1->plgend[0] == 'F') ? "er" : "is");
	    } else if (!strncmp(t1->death, "poisoned", 8)) {
		strcat(linebuf, "was poisoned");
	    } else if (!strncmp(t1->death, "crushed", 7)) {
		strcat(linebuf, "was crushed to death");
	    } else if (!strncmp(t1->death, "petrified by ", 13)) {
		strcat(linebuf, "turned to stone");
	    } else strcat(linebuf, "died");

	    if (t1->deathdnum == astral_level.dnum) {
		int deathlev = t1->deathlev;
		const char *arg, *fmt = " on the Plane of %s";

		if (!t1->ver_major && !t1->ver_minor && t1->patchlevel < 7)
			deathlev--;

		switch (deathlev) {
		case -5:
			fmt = " on the %s Plane";
			arg = "Astral";	break;
		case -4:
			arg = "Water";	break;
		case -3:
			arg = "Fire";	break;
		case -2:
			arg = "Air";	break;
		case -1:
			arg = "Earth";	break;
		default:
			arg = "Void";	break;
		}
		sprintf(eos(linebuf), fmt, arg);
	    } else {
		sprintf(eos(linebuf), " in %s", dungeons[t1->deathdnum].dname);
		/*if (t1->deathdnum != knox_level.dnum)*/	/* not working for some reason, so let's just remove the check */
		    sprintf(eos(linebuf), " on level %d", t1->deathlev);
		if (t1->deathlev != t1->maxlvl)
		    sprintf(eos(linebuf), " [max %d]", t1->maxlvl);
	    }

	    /* kludge for "quit while already on Charon's boat" */
	    if (!strncmp(t1->death, "quit ", 5))
		strcat(linebuf, t1->death + 4);
	}
	strcat(linebuf, ".");

	/* Quit, starved, ascended, and escaped contain no second line */
	if (second_line)
	    sprintf(eos(linebuf), "  %c%s.", highc(*(t1->death)), t1->death+1);

	lngr = (int)strlen(linebuf);
	if (t1->hp <= 0) hpbuf[0] = '-', hpbuf[1] = '\0';
	else sprintf(hpbuf, "%d", t1->hp);
	/* beginning of hp column after padding (not actually padded yet) */
	hppos = COLNO - (sizeof("  Hp [max]")-1); /* sizeof(str) includes \0 */
	while (lngr >= hppos) {
	    for(bp = eos(linebuf);
		    !(*bp == ' ' && (bp-linebuf < hppos));
		    bp--)
		;
	    /* special case: word is too long, wrap in the middle */
	    if (linebuf+15 >= bp) bp = linebuf + hppos - 1;
	    /* special case: if about to wrap in the middle of maximum
	       dungeon depth reached, wrap in front of it instead */
	    if (bp > linebuf + 5 && !strncmp(bp - 5, " [max", 5)) bp -= 5;

	    /* gigabug from vanilla: infinite loop if the line is too long, fix from the variant that calls itself 3.7 */
	    if (*bp != ' ')
		strcpy(linebuf3, bp);
	    else
		strcpy(linebuf3, bp+1);

	    *bp = 0;
	    if (so) {
		while (bp < linebuf + (COLNO-1)) *bp++ = ' ';
		*bp = 0;
		topten_print_bold(linebuf);
#ifdef DUMP_LOG
		dump("*", linebuf[0]==' '? linebuf+1: linebuf);
#endif
	    } else {
		topten_print(linebuf);
#ifdef DUMP_LOG
		dump(" ", linebuf[0]==' '? linebuf+1: linebuf);
#endif
	    }
	    sprintf(linebuf, "%15s %s", "", linebuf3);
	    lngr = strlen(linebuf);
	}
	/* beginning of hp column not including padding */
	hppos = COLNO - 7 - (int)strlen(hpbuf);
	bp = eos(linebuf);

	if (bp <= linebuf + hppos) {
	    /* pad any necessary blanks to the hit point entry */
	    while (bp < linebuf + hppos) *bp++ = ' ';
	    strcpy(bp, hpbuf);
	    sprintf(eos(bp), " %s[%d]",
		    (t1->maxhp < 10) ? "  " : (t1->maxhp < 100) ? " " : "",
		    t1->maxhp);
	}

	if (so) {
	    bp = eos(linebuf);
	    if (so >= COLNO) so = COLNO-1;
	    while (bp < linebuf + so) *bp++ = ' ';
	    *bp = 0;
	    topten_print_bold(linebuf);
	} else
	    topten_print(linebuf);
#ifdef DUMP_LOG
	dump(" ", linebuf[0]==' '? linebuf+1: linebuf);
#endif
}

STATIC_OVL int
score_wanted(current_ver, rank, t1, playerct, players, uid)
boolean current_ver;
int rank;
struct toptenentry *t1;
int playerct;
const char **players;
int uid;
{
	int i;

	if (current_ver && (t1->ver_major != VERSION_MAJOR ||
			    t1->ver_minor != VERSION_MINOR ||
			    t1->patchlevel != PATCHLEVEL))
		return 0;

#ifdef PERS_IS_UID
	if (!playerct && t1->uid == uid)
		return 1;
#endif

	for (i = 0; i < playerct; i++) {
		if (players[i][0] == '-' && index("prga", players[i][1]) &&
                players[i][2] == 0 && i + 1 < playerct) {
		char *arg = (char *)players[i + 1];
		if ((players[i][1] == 'p' &&
		     str2role(arg) == str2role(t1->plrole)) ||
		    (players[i][1] == 'r' &&
		     str2race(arg) == str2race(t1->plrace)) ||
		    (players[i][1] == 'g' &&
		     str2gend(arg) == str2gend(t1->plgend)) ||
		    (players[i][1] == 'a' &&
		     str2align(arg) == str2align(t1->plalign)))
		    return 1;
		i++;
		}
		else if (strcmp(players[i], "all") == 0 ||
		    strncmp(t1->name, players[i], NAMSZ) == 0 ||
		    (players[i][0] == '-' &&
		     players[i][1] == t1->plrole[0] &&
		     players[i][2] == 0) ||
		    (digit(players[i][0]) && rank <= atoi(players[i])))
		return 1;
	}
	return 0;
}

#ifdef RECORD_ACHIEVE
long
encodeachieve(void)
{
  /* Achievement bitfield:
   * bit  meaning
   *  0   obtained the Bell of Opening
   *  1   entered gehennom (by any means)
   *  2   obtained the Candelabrum of Invocation
   *  3   obtained the Book of the Dead
   *  4   performed the invocation ritual
   *  5   obtained the amulet
   *  6   entered elemental planes
   *  7   entered astral plane
   *  8   ascended (not "escaped in celestial disgrace!")
   *  9   obtained the luckstone from the Mines
   *  10  obtained the sokoban prize
   *  11  killed medusa
   *  12  killed Nightmare
   *  13  killed Vecna
   *  14  killed Beholder
   *  15  killed Ruggo
   *  16  killed Kroo
   *  17  killed Grund
   *  18  killed The Largest Giant
   *  19  killed Shelob
   *  20  killed Girtab
   *  21  killed Aphrodite
   *  22  killed Frankenstein
   *  23  killed Croesus
   *  24  killed Dagon
   *  25  killed Hydra
   *  26  imbued the Bell of Opening
   *  27  imbued the Amulet of Yendor
   */

  long r;

  r = 0;

  if(achieve.get_bell)            r |= 1L << 0;
  if(achieve.enter_gehennom)      r |= 1L << 1;
  if(achieve.get_candelabrum)     r |= 1L << 2;
  if(achieve.get_book)            r |= 1L << 3;
  if(achieve.perform_invocation)  r |= 1L << 4;
  if(achieve.get_amulet)          r |= 1L << 5;
  if(In_endgame(&u.uz))           r |= 1L << 6;
  if(Is_astralevel(&u.uz))        r |= 1L << 7;
  if(achieve.ascended)            r |= 1L << 8;
  if(achieve.get_luckstone)       r |= 1L << 9;
  if(achieve.finish_sokoban)      r |= 1L << 10;
  if(achieve.killed_medusa)       r |= 1L << 11;
  if(achieve.killed_nightmare)    r |= 1L << 12;
  if(achieve.killed_vecna)        r |= 1L << 13;
  if(achieve.killed_beholder)     r |= 1L << 14;
  if(achieve.killed_ruggo)        r |= 1L << 15;
  if(achieve.killed_kroo)         r |= 1L << 16;
  if(achieve.killed_grund)        r |= 1L << 17;
  if(achieve.killed_largestgiant) r |= 1L << 18;
  if(achieve.killed_shelob)       r |= 1L << 19;
  if(achieve.killed_girtab)       r |= 1L << 20;
  if(achieve.killed_aphrodite)    r |= 1L << 21;
  if(achieve.killed_frankenstein) r |= 1L << 22;
  if(achieve.killed_croesus)      r |= 1L << 23;
  if(achieve.killed_dagon)        r |= 1L << 24;
  if(achieve.killed_hydra)        r |= 1L << 25;
  if(achieve.imbued_bell)         r |= 1L << 26;
  if(achieve.imbued_amulet)       r |= 1L << 27;

  return r;

}

char encoded_achievements[BUFSZ];
char* encodeachieveX(void)
{
  /* Achievement bitfield:
   * bit  meaning
   *  0   killed an elder priest
   *  1   killed the Motherfucker Glass Golem
   *  2   killed Tiksrvzllat
   *  3   killed the BOFH
   *  4   reached the bottom of the Swimming Pools
   *  5   killed Erogenous Katia
   *  6   killed the Witch King of Angmar
   *  7   obtained the stone of magic resistance from the Deep Mines
   *  8   visited all five DevNull challenge dungeons
   *  9   killed the Minotaur of the Maze
   *  10  killed Kalwina
   *  11  killed Stahngnir
   *  12  killed Ariane
   *  13  completed the Rival Quest
   *  14  completed Minus World
   *  15  killed Vera
   *  16  killed Elaine
   * but this isn't a bitfield, it's a string...
   */

  encoded_achievements[0] = '\0';

  if(achieveX.killed_elderpriest)   sprintf(eos(encoded_achievements), "%s,", "killed_elderpriest");
  if(achieveX.killed_glassgolem)    sprintf(eos(encoded_achievements), "%s,", "killed_glassgolem");
  if(achieveX.killed_tiksrvzllat)   sprintf(eos(encoded_achievements), "%s,", "killed_tiksrvzllat");
  if(achieveX.killed_bofh)          sprintf(eos(encoded_achievements), "%s,", "killed_bofh");
  if(achieveX.swimmingpool_cleared) sprintf(eos(encoded_achievements), "%s,", "swimmingpool_cleared");
  if(achieveX.killed_katia)         sprintf(eos(encoded_achievements), "%s,", "killed_katia");
  if(achieveX.killed_witchking)     sprintf(eos(encoded_achievements), "%s,", "killed_witchking");
  if(achieveX.get_magresstone)      sprintf(eos(encoded_achievements), "%s,", "get_magresstone");
  if(achieveX.devnull_complete)     sprintf(eos(encoded_achievements), "%s,", "devnull_complete");
  if(achieveX.killed_minotaur)      sprintf(eos(encoded_achievements), "%s,", "killed_minotaur");
  if(achieveX.killed_kalwina)       sprintf(eos(encoded_achievements), "%s,", "killed_kalwina");
  if(achieveX.killed_stahngnir)     sprintf(eos(encoded_achievements), "%s,", "killed_stahngnir");
  if(achieveX.killed_ariane)        sprintf(eos(encoded_achievements), "%s,", "killed_ariane");
  if(achieveX.completed_rivalquest) sprintf(eos(encoded_achievements), "%s,", "completed_rivalquest");
  if(achieveX.completed_minusworld) sprintf(eos(encoded_achievements), "%s,", "completed_minusworld");
  if(achieveX.killed_vera)          sprintf(eos(encoded_achievements), "%s,", "killed_vera");
  if(achieveX.killed_elaine)        sprintf(eos(encoded_achievements), "%s,", "killed_elaine");

  int len;
  if ((len=strlen(encoded_achievements))) { encoded_achievements[len-1] = '\0'; }
  return encoded_achievements;
}
#endif

/*
 * print selected parts of score list.
 * argc >= 2, with argv[0] untrustworthy (directory names, et al.),
 * and argv[1] starting with "-s".
 */
void
prscore(argc,argv)
int argc;
char **argv;
{
	const char **players;
	int playerct, rank;
	boolean current_ver = TRUE, init_done = FALSE;
	register struct toptenentry *t1;
	FILE *rfile;
	boolean match_found = FALSE;
	register int i;
	char pbuf[BUFSZ];
	int uid = -1;
#ifndef PERS_IS_UID
	const char *player0;
#endif

	if (argc < 2 || strncmp(argv[1], "-s", 2)) {
		raw_printf("prscore: bad arguments (%d)", argc);
		return;
	}

	rfile = fopen_datafile_area(NH_RECORD_AREA, NH_RECORD, "r", SCOREPREFIX);
	if (!rfile) {
		raw_print("Cannot open record file!");
		return;
	}

#ifdef	AMIGA
	{
	    extern winid amii_rawprwin;
	    init_nhwindows(&argc, argv);
	    amii_rawprwin = create_nhwindow(NHW_TEXT);
	}
#endif

	/* If the score list isn't after a game, we never went through
	 * initialization. */
	if (wiz1_level.dlevel == 0) {
		dlb_init();
		init_dungeons();
		init_done = TRUE;
	}

	if (!argv[1][2]){	/* plain "-s" */
		argc--;
		argv++;
	} else	argv[1] += 2;

	if (argc > 1 && !strcmp(argv[1], "-v")) {
		current_ver = FALSE;
		argc--;
		argv++;
	}

	if (argc <= 1) {
#ifdef PERS_IS_UID
		uid = getuid();
		playerct = 0;
		players = (const char **)0;
#else
		player0 = plname;
		if (!*player0)
# ifdef AMIGA
			player0 = "all";	/* single user system */
# else
			player0 = "hackplayer";
# endif
		playerct = 1;
		players = &player0;
#endif
	} else {
		playerct = --argc;
		players = (const char **)++argv;
	}
	raw_print("");

	t1 = tt_head = newttentry();
	for (rank = 1; ; rank++) {
	    readentry(rfile, t1);
	    if (t1->points == 0) break;
	    if (!match_found &&
		    score_wanted(current_ver, rank, t1, playerct, players, uid))
		match_found = TRUE;
	    t1->tt_next = newttentry();
	    t1 = t1->tt_next;
	}

	(void) fclose(rfile);
	if (init_done) {
	    free_dungeons();
	    dlb_cleanup();
	}

	if (match_found) {
	    outheader();
	    t1 = tt_head;
	    for (rank = 1; t1->points != 0; rank++, t1 = t1->tt_next) {
		if (score_wanted(current_ver, rank, t1, playerct, players, uid))
		    (void) outentry(rank, t1, 0);
	    }
	} else {
	    sprintf(pbuf, "Cannot find any %sentries for ",
				current_ver ? "current " : "");
	    if (playerct < 1) strcat(pbuf, "you.");
	    else {
		if (playerct > 1) strcat(pbuf, "any of ");
		for (i = 0; i < playerct; i++) {
		    /* stop printing players if there are too many to fit */
		    if (strlen(pbuf) + strlen(players[i]) + 2 >= BUFSZ) {
			if (strlen(pbuf) < BUFSZ-4) strcat(pbuf, "...");
			else strcpy(pbuf+strlen(pbuf)-4, "...");
			break;
		    }
		    strcat(pbuf, players[i]);
		    if (i < playerct-1) {
			if (players[i][0] == '-' &&
			    index("prga", players[i][1]) && players[i][2] == 0)
			    strcat(pbuf, " ");
			else strcat(pbuf, ":");
		    }
		}
	    }
	    raw_print(pbuf);
	    raw_printf("Usage: %s -s [-v] <playertypes> [maxrank] [playernames]",

			 hname);
	    raw_printf("Player types are: [-p role] [-r race] [-g gender] [-a align]");
	}
	free_ttlist(tt_head);
#ifdef	AMIGA
	{
	    extern winid amii_rawprwin;
	    display_nhwindow(amii_rawprwin, 1);
	    destroy_nhwindow(amii_rawprwin);
	    amii_rawprwin = WIN_ERR;
	}
#endif
}

STATIC_OVL int
classmon(plch, fem)
	char *plch;
	boolean fem;
{
	int i;

	/* Look for this role in the role table */
	for (i = 0; roles[i].name.m; i++)
	    if (!strncmp(plch, roles[i].filecode, ROLESZ)) {
		if (fem && roles[i].femalenum != NON_PM)
		    return roles[i].femalenum;
		else if (roles[i].malenum != NON_PM)
		    return roles[i].malenum;
		else
		    return PM_HUMAN;
	    }
	/* this might be from a 3.2.x score for former Elf class */
	if (!strcmp(plch, "E")) return PM_RANGER;

	impossible("What weird role is this? (%s)", plch);
	return (PM_HUMAN_MUMMY);
}

STATIC_OVL int
undeadclassmon(plch, fem)
	char *plch;
	boolean fem;
{
	int i;

	/* Look for this role in the role table */
	for (i = 0; roles[i].name.m; i++)
	    if (!strncmp(plch, roles[i].filecode, ROLESZ)) {
		if (fem && roles[i].undeadfemalenum != NON_PM)
		    return roles[i].undeadfemalenum;
		else if (roles[i].undeadmalenum != NON_PM)
		    return roles[i].undeadmalenum;
		else
		    return PM_HUMAN;
	    }
	/* this might be from a 3.2.x score for former Elf class */
	if (!strcmp(plch, "E")) return PM_RANGER;

	impossible("What weird role is this? (%s)", plch);
	return (PM_HUMAN_MUMMY);
}

/*
 * Get a random player name and class from the high score list,
 * and attach them to an object (for statues or morgue corpses).
 */
struct obj *
tt_oname(otmp)
struct obj *otmp;
{
	int rank, rankamount;
	rankamount = 1000;
	register int i;
	register struct toptenentry *tt;
	FILE *rfile;
	struct toptenentry tt_buf;

	if (!otmp) return((struct obj *) 0);

	rfile = fopen_datafile_area(NH_RECORD_AREA, NH_RECORD, "r", SCOREPREFIX);
	if (!rfile) {
		impossible("Cannot open record file!");
		return (struct obj *)0;
	}

	tt = &tt_buf;

	rank = rnd(rankamount); /* new code by Amy that allows more randomness - up to 1000 entries can be read now */
pickentry:
	rank = rnd(rankamount);
	for(i = rank; i; i--) {
	    readentry(rfile, tt);
	    if(tt->points == 0) break;
	}

	if(tt->points == 0) {
		if(rank > 1) {
			rankamount = (rank - 1);
			if (rankamount < 1) {
				impossible("Not enough records!");
				return (struct obj *)0;
			}
			rewind(rfile);
			goto pickentry;
		}
		otmp = (struct obj *) 0;
	} else {
		/* reset timer in case corpse started out as lizard or troll */
		if (otmp->otyp == CORPSE) obj_stop_timers(otmp);
		otmp->corpsenm = classmon(tt->plrole, (tt->plgend[0] == 'F'));
		otmp->owt = weight(otmp);
		otmp = oname(otmp, tt->name);
		if (otmp->otyp == CORPSE) start_corpse_timeout(otmp);
	}

	(void) fclose(rfile);
	return otmp;
}

/*
 * Get a random player name and class from the high score list,
 * and attach them to a monster (for ghost summon spell). --Amy
 */
void
tt_mname(mm, revive_corpses, mm_flags)
coord *mm;
boolean revive_corpses;
int mm_flags;
{
	struct monst *mtmp;
	int nonefound;

	int cnt = 1;
	if (!rn2(2)) cnt = (monster_difficulty() + 1)/10;
	if (!rn2(5)) cnt += rnz(5);
	if (cnt < 1) cnt = 1;
	int mdat;
	struct obj *otmp;
	coord cc;

	while (cnt--) {


	int rank, rankamount;
	rankamount = 1000;
	register int i;
	register struct toptenentry *tt;
	FILE *rfile;
	struct toptenentry tt_buf;

	/*if (!mtmp) { pline("No records!");

	}*/

	rfile = fopen_datafile_area(NH_RECORD_AREA, NH_RECORD, "r", SCOREPREFIX);
	if (!rfile) {
		impossible("Cannot open record file!");
	}

	tt = &tt_buf;

	rank = rnd(rankamount); /* new code by Amy that allows more randomness - up to 1000 entries can be read now */
pickentry:
	rank = rnd(rankamount);
	for(i = rank; i; i--) {
	    readentry(rfile, tt);
	    if(tt->points == 0) break; 
	}

	if(tt->points == 0) {
		if(rank > 1) {
			rankamount = (rank - 1);
			if (rankamount < 1) {
				impossible("Not enough records!");
			}
			rewind(rfile);
			goto pickentry;
		}

		/* we should only end up here if there are no entries --Amy */
	    if (enexto(&cc, mm->x, mm->y, youmonst.data) &&
		    (!revive_corpses ||
		     !(otmp = sobj_at(CORPSE, cc.x, cc.y)) ||
		     !revive(otmp)))

		mtmp = makemon(&mons[PM_UNDEAD_ARCHEOLOGIST + rn2(PM_UNDEAD_WIZARD - PM_UNDEAD_ARCHEOLOGIST + 1)], cc.x, cc.y, mm_flags);

		/*mtmp = (struct monst *) 0;*/
	} else {

		/*mtmp = undeadclassmon(tt->plrole, (tt->plgend[0] == 'F')) ;*/

	    if (enexto(&cc, mm->x, mm->y, youmonst.data) &&
		    (!revive_corpses ||
		     !(otmp = sobj_at(CORPSE, cc.x, cc.y)) ||
		     !revive(otmp)))

		{

		mtmp = makemon(&mons[undeadclassmon(tt->plrole, (tt->plgend[0] == 'F'))], cc.x, cc.y, mm_flags);
		christen_monst(mtmp, tt->name);
		/*mtmp = christen_monst(mtmp, tt->name);*/
		}
	}

	(void) fclose(rfile);

	}
}

#ifdef NO_SCAN_BRACK
/* Lattice scanf isn't up to reading the scorefile.  What */
/* follows deals with that; I admit it's ugly. (KL) */
/* Now generally available (KL) */
STATIC_OVL void
nsb_mung_line(p)
	char *p;
{
	while ((p = index(p, ' ')) != 0) *p = '|';
}

STATIC_OVL void
nsb_unmung_line(p)
	char *p;
{
	while ((p = index(p, '|')) != 0) *p = ' ';
}
#endif /* NO_SCAN_BRACK */

#if defined(GTK_GRAPHICS) || defined(PROXY_GRAPHICS)
winid
create_toptenwin()
{
    toptenwin = create_nhwindow(NHW_TEXT);

    return toptenwin;
}

void
destroy_toptenwin()
{
    destroy_nhwindow(toptenwin);
    toptenwin = WIN_ERR;
}
#endif

const char *
gamemode_strcode()
{
    static char string[BUFSZ];
    *string = '\0';

	if (u.freeplaymode) sprintf(eos(string), "freeplay");
	if (flags.gehenna) sprintf(eos(string), "gehenna");
	if (flags.dudley) sprintf(eos(string), "dudley");
	if (flags.iwbtg) sprintf(eos(string), "iwbtg");
	if (flags.elmstreet) sprintf(eos(string), "elmstreet");
	if (flags.hippie) sprintf(eos(string), "hippie");
	if (flags.blindfox) sprintf(eos(string), "blindfox");
	if (flags.uberlostsoul) sprintf(eos(string), "uberlostsoul");
	if (flags.lostsoul && !(flags.uberlostsoul)) sprintf(eos(string), "lostsoul");
#ifdef GMMODE
	if (flags.gmmode) sprintf(eos(string), "gmmode");
	if (flags.supergmmode) sprintf(eos(string), "supergmmode");
#endif
	if (flags.wonderland) sprintf(eos(string), "wonderland");
	if (flags.zapem) sprintf(eos(string), "zapm");
	if (flags.assholemode) sprintf(eos(string), "asshole");

	if (!u.freeplaymode && !(flags.gehenna) && !(flags.dudley)
#ifdef GMMODE
	&& !(flags.gmmode) && !(flags.supergmmode)
#endif
	&& !(flags.iwbtg) && !(flags.assholemode) && !(flags.elmstreet) && !(flags.hippie) && !(flags.blindfox) && !(flags.uberlostsoul) && !(flags.lostsoul) && !(flags.wonderland) && !(flags.zapem)) sprintf(eos(string), "none");

    return (string);
}

/*topten.c*/
