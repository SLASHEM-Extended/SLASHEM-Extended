/*	SCCS Id: @(#)files.c	3.4	2003/11/14	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "dlb.h"

/* WAC for config file */
#include "filename.h"
/* needs to be after hack.h. Caused .slashemrc to never be read on UNIX */

#if defined(WHEREIS_FILE) && defined(UNIX)
#include <sys/types.h> /* whereis-file chmod() */
#endif

#ifdef TTY_GRAPHICS
#include "wintty.h" /* more() */
#endif

#if defined(GL_GRAPHICS) || defined(SDL_GRAPHICS)
#include "winGL.h"  /* Sdlgl_parse_options */
#endif

#ifdef PROXY_GRAPHICS
#include "winproxy.h" /* proxy_config_open() */
#endif

#include <ctype.h>

#if (!defined(MAC) && !defined(O_WRONLY) && !defined(AZTEC_C)) || defined(USE_FCNTL)
#include <fcntl.h>
#endif

#include <errno.h>
#ifdef _MSC_VER	/* MSC 6.0 defines errno quite differently */
# if (_MSC_VER >= 600)
#  define SKIP_ERRNO
# endif
#else
# ifdef NHSTDC
#  define SKIP_ERRNO
# endif
#endif
#ifndef SKIP_ERRNO
# ifdef _DCC
const
# endif
extern int errno;
#endif

#if defined(UNIX) && defined(QT_GRAPHICS)
#include <dirent.h>
#endif

#if defined(UNIX) || defined(VMS)
#include <signal.h>
#endif

#ifndef NO_SIGNAL
#include <signal.h>
#endif

/* WAC moved to below
#include <sys\stat.h>
*/

/* ALI: For compatibility */
#ifndef FILE_AREAS
#define compress(file)		compress_area(NULL, file)
#define uncompress(file)	uncompress_area(NULL, file)
#endif

#if defined(MSDOS) || defined(OS2) || defined(TOS) || defined(WIN32)
# ifndef GNUDOS
#include <sys\stat.h>
# else
#include <sys/stat.h>
# endif
#else
#include <sys/stat.h>
#endif
/* metanite64: the last else clause was added bc newer macOS versions weren't getting the include at all */
#ifndef O_BINARY	/* used for micros, no-op for others */
# define O_BINARY 0
#endif

#ifdef PREFIXES_IN_USE
#define FQN_NUMBUF 4
static char fqn_filename_buffer[FQN_NUMBUF][FQN_MAX_FILENAME];
#endif

#define BONESNAMELEN 20 /* long enough for "bon" + bonedunlvl + filecode + dlevel + ubirthday */

#if !defined(MFLOPPY) && !defined(VMS) && !defined(WIN32)
char bones[BONESNAMELEN] = "bonesnn.xxx";
char lock[PL_NSIZ+14] = "1lock"; /* long enough for uid+name+.99 */
#else
# if defined(MFLOPPY)
char bones[FILENAMELEN];           /* pathname of bones files */
char lock[FILENAMELEN];            /* pathname of level files */
# endif
# if defined(VMS)
char bones[BONESNAMELEN] = "bonesnn.xxx;1";
char lock[PL_NSIZ+17] = "1lock"; /* long enough for _uid+name+.99;1 */
# endif
# if defined(WIN32)
char bones[BONESNAMELEN] = "bonesnn.xxx";
char lock[PL_NSIZ+25];		/* long enough for username+-+name+.99 */
# endif
#endif

#if defined(UNIX) || defined(__BEOS__)
#define SAVESIZE	(PL_NSIZ + 13)	/* save/99999player.e */
#else
# ifdef VMS
#define SAVESIZE	(PL_NSIZ + 22)	/* [.save]<uid>player.e;1 */
# else
#  if defined(WIN32)
#define SAVESIZE	(PL_NSIZ + 40)	/* username-player.NetHack-saved-game */
#  else
#define SAVESIZE        FILENAMELEN        /* from macconf.h or pcconf.h */
#  endif
# endif
#endif

char SAVEF[SAVESIZE];	/* holds relative path of save file from playground */
#ifdef MICRO
char SAVEP[SAVESIZE];	/* holds path of directory for save file */
#endif

#ifdef HOLD_LOCKFILE_OPEN
struct level_ftrack {
int init;
int fd;					/* file descriptor for level file     */
int oflag;				/* open flags                         */
boolean nethack_thinks_it_is_open;	/* Does NetHack think it's open?       */
} lftrack;
# if defined(WIN32)
#include <share.h>
# endif
#endif /*HOLD_LOCKFILE_OPEN*/

#ifdef WIZARD
#define WIZKIT_MAX 128
static char wizkit[WIZKIT_MAX];
STATIC_DCL FILE *fopen_wizkit_file(void);
#endif

#ifdef AMIGA
extern char PATH[];	/* see sys/amiga/amidos.c */
extern char bbs_id[];
static int lockptr;
# ifdef __SASC_60
#include <proto/dos.h>
# endif

#include <libraries/dos.h>
extern void amii_set_text_font( char *, int );
#endif

#if defined(WIN32) || defined(MSDOS)
static int lockptr;
# ifdef MSDOS
#  define Delay(a) msleep(a)
# endif
# define Close close
# ifndef WIN_CE
#  define DeleteFile unlink
# endif
#endif

#ifdef MAC
# define unlink macunlink
#endif

#ifdef USER_SOUNDS
extern char *sounddir;
#endif

extern int n_dgns;		/* from dungeon.c */

STATIC_DCL char *set_bonesfile_name(char *,d_level*);
STATIC_DCL char *set_bonestemp_name(void);
#ifdef COMPRESS
STATIC_DCL void redirect(const char *,const char *,const char *,
	FILE *,BOOLEAN_P);
STATIC_DCL void docompress_file(const char *,const char *,BOOLEAN_P);
#endif
#ifndef FILE_AREAS
#ifndef USE_FCNTL 
STATIC_DCL char *make_lockname(const char *,char *);
#endif
#endif
STATIC_DCL FILE *fopen_config_file(const char *);
STATIC_DCL int get_uchars(FILE *,char *,char *,uchar *,BOOLEAN_P,int,const char *);
int parse_config_line(FILE *,char *,char *,char *);
#ifdef NOCWD_ASSUMPTIONS
STATIC_DCL void adjust_prefix(char *, int);
#endif
#ifdef SELF_RECOVER
STATIC_DCL boolean copy_bytes(int, int);
#endif
#ifdef HOLD_LOCKFILE_OPEN
STATIC_DCL int open_levelfile_exclusively(const char *, int, int);
#endif

/*
 * fname_encode()
 *
 *   Args:
 *	legal		zero-terminated list of acceptable file name characters
 *	quotechar	lead-in character used to quote illegal characters as hex digits
 *	s		string to encode
 *	callerbuf	buffer to house result
 *	bufsz		size of callerbuf
 *
 *   Notes:
 *	The hex digits 0-9 and A-F are always part of the legal set due to
 *	their use in the encoding scheme, even if not explicitly included in 'legal'.
 *
 *   Sample:
 *	The following call:
 *	    (void)fname_encode("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz",
 *				'%', "This is a % test!", buf, 512);
 *	results in this encoding:
 *	    "This%20is%20a%20%25%20test%21"
 */
char *
fname_encode(legal, quotechar, s, callerbuf, bufsz)
const char *legal;
char quotechar;
char *s, *callerbuf;
int bufsz;
{
	char *sp, *op;
	int cnt = 0;
	static char hexdigits[] = "0123456789ABCDEF";

	sp = s;
	op = callerbuf;
	*op = '\0';
	
	while (*sp) {
		/* Do we have room for one more character or encoding? */
		if ((bufsz - cnt) <= 4) return callerbuf;

		if (*sp == quotechar) {
			(void)sprintf(op, "%c%02X", quotechar, *sp);
			 op += 3;
			 cnt += 3;
		} else if ((index(legal, *sp) != 0) || (index(hexdigits, *sp) != 0)) {
			*op++ = *sp;
			*op = '\0';
			cnt++;
		} else {
			(void)sprintf(op,"%c%02X", quotechar, *sp);
			op += 3;
			cnt += 3;
		}
		sp++;
	}
	return callerbuf;
}

/*
 * fname_decode()
 *
 *   Args:
 *	quotechar	lead-in character used to quote illegal characters as hex digits
 *	s		string to decode
 *	callerbuf	buffer to house result
 *	bufsz		size of callerbuf
 */
char *
fname_decode(quotechar, s, callerbuf, bufsz)
char quotechar;
char *s, *callerbuf;
int bufsz;
{
	char *sp, *op;
	int k,calc,cnt = 0;
	static char hexdigits[] = "0123456789ABCDEF";

	sp = s;
	op = callerbuf;
	*op = '\0';
	calc = 0;

	while (*sp) {
		/* Do we have room for one more character? */
		if ((bufsz - cnt) <= 2) return callerbuf;
		if (*sp == quotechar) {
			sp++;
			for (k=0; k < 16; ++k) if (*sp == hexdigits[k]) break;
			if (k >= 16) return callerbuf;	/* impossible, so bail */
			calc = k << 4; 
			sp++;
			for (k=0; k < 16; ++k) if (*sp == hexdigits[k]) break;
			if (k >= 16) return callerbuf;	/* impossible, so bail */
			calc += k; 
			sp++;
			*op++ = calc;
			*op = '\0';
		} else {
			*op++ = *sp++;
			*op = '\0';
		}
		cnt++;
	}
	return callerbuf;
}

#ifndef PREFIXES_IN_USE
/*ARGSUSED*/
#endif
const char *
fqname(basename, whichprefix, buffnum)
const char *basename;
int whichprefix, buffnum;
{
#ifndef PREFIXES_IN_USE
	return basename;
#else
	if (!basename || whichprefix < 0 || whichprefix >= PREFIX_COUNT)
		return basename;
	if (!fqn_prefix[whichprefix])
		return basename;
	if (buffnum < 0 || buffnum >= FQN_NUMBUF) {
		impossible("Invalid fqn_filename_buffer specified: %d",
								buffnum);
		buffnum = 0;
	}
	if (strlen(fqn_prefix[whichprefix]) + strlen(basename) >=
						    FQN_MAX_FILENAME) {
		impossible("fqname too long: %s + %s", fqn_prefix[whichprefix],
						basename);
		return basename;	/* XXX */
	}
	strcpy(fqn_filename_buffer[buffnum], fqn_prefix[whichprefix]);
	return strcat(fqn_filename_buffer[buffnum], basename);
#endif
}

/* reasonbuf must be at least BUFSZ, supplied by caller */
/*ARGSUSED*/
int
validate_prefix_locations(reasonbuf)
char *reasonbuf;
{
#if defined(NOCWD_ASSUMPTIONS)
	FILE *fp;
	const char *filename;
	int prefcnt, failcount = 0;
	char panicbuf1[BUFSZ], panicbuf2[BUFSZ], *details;

	if (reasonbuf) reasonbuf[0] = '\0';
	for (prefcnt = 1; prefcnt < PREFIX_COUNT; prefcnt++) {
		/* don't test writing to configdir or datadir; they're readonly */
		if (prefcnt == CONFIGPREFIX || prefcnt == DATAPREFIX) continue;
		filename = fqname("validate", prefcnt, 3);
		if ((fp = fopen(filename, "w"))) {
			fclose(fp);
			(void) unlink(filename);
		} else {
			if (reasonbuf) {
				if (failcount) strcat(reasonbuf,", ");
				strcat(reasonbuf, fqn_prefix_names[prefcnt]);
			}
			/* the paniclog entry gets the value of errno as well */
			sprintf(panicbuf1,"Invalid %s", fqn_prefix_names[prefcnt]);
#if defined (NHSTDC) && !defined(NOTSTDC)
			if (!(details = strerror(errno)))
#endif
			details = "";
			sprintf(panicbuf2,"\"%s\", (%d) %s",
				fqn_prefix[prefcnt], errno, details);
			paniclog(panicbuf1, panicbuf2);
			failcount++;
		}	
	}
	if (failcount)
		return 0;
	else
#endif
	return 1;
}

/*
 * When file areas are in use, fopen_datafile_area is used instead.
 */

#ifndef FILE_AREA
/* fopen a file, with OS-dependent bells and whistles */
/* NOTE: a simpler version of this routine also exists in util/dlb_main.c */
FILE *
fopen_datafile(filename, mode, prefix)
const char *filename, *mode;
int prefix;
{
	FILE *fp;

	filename = fqname(filename, prefix, prefix == TROUBLEPREFIX ? 3 : 0);
#ifdef VMS	/* essential to have punctuation, to avoid logical names */
    {
	char tmp[BUFSIZ];

	if (!index(filename, '.') && !index(filename, ';'))
		filename = strcat(strcpy(tmp, filename), ";0");
	fp = fopen(filename, mode, "mbc=16");
    }
#else
	fp = fopen(filename, mode);
#endif
	return fp;
}
#endif	/* FILE_AREA */

/* ----------  BEGIN LEVEL FILE HANDLING ----------- */

#ifdef MFLOPPY
/* Set names for bones[] and lock[] */
void
set_lock_and_bones()
{
	if (!ramdisk) {
		strcpy(levels, permbones);
		strcpy(bones, permbones);
	}
	append_slash(permbones);
	append_slash(levels);
#ifdef AMIGA
	strncat(levels, bbs_id, PATHLEN);
#endif
	append_slash(bones);
	strcat(bones, "bonesnn.*");
	strcpy(lock, levels);
#ifndef AMIGA
	strcat(lock, alllevels);
#endif
	return;
}
#endif /* MFLOPPY */


/* Construct a file name for a level-type file, which is of the form
 * something.level (with any old level stripped off).
 * This assumes there is space on the end of 'file' to append
 * a two digit number.  This is true for 'level'
 * but be careful if you use it for other things -dgk
 */
void
set_levelfile_name(file, lev)
char *file;
int lev;
{
	char *tf;

	tf = rindex(file, '.');
	if (!tf) tf = eos(file);
	sprintf(tf, ".%d", lev);
#ifdef VMS
	strcat(tf, ";1");
#endif
	return;
}

int
create_levelfile(lev, errbuf)
int lev;
char errbuf[];
{
	int fd;
#ifndef FILE_AREAS
	const char *fq_lock;
#endif

	if (errbuf) *errbuf = '\0';
	set_levelfile_name(lock, lev);
#ifndef FILE_AREAS
	fq_lock = fqname(lock, LEVELPREFIX, 0);
#endif

#if defined(MICRO) || defined(WIN32)
	/* Use O_TRUNC to force the file to be shortened if it already
	 * exists and is currently longer.
	 */
# ifdef FILE_AREAS
	fd = open_area(FILE_AREA_LEVL, lock,
	  O_WRONLY |O_CREAT | O_TRUNC | O_BINARY, FCMASK);
# else
#  ifdef HOLD_LOCKFILE_OPEN
	if (lev == 0)
	    fd = open_levelfile_exclusively(fq_lock, lev,
			    O_WRONLY |O_CREAT | O_TRUNC | O_BINARY);
	else
#  endif
	fd = open(fq_lock, O_WRONLY |O_CREAT | O_TRUNC | O_BINARY, FCMASK);
# endif
#else	/* MICRO */
# ifdef FILE_AREAS
	fd = creat_area(FILE_AREA_LEVL, lock, FCMASK);
# else
# ifdef MAC
	fd = maccreat(fq_lock, LEVL_TYPE);
# else
	fd = creat(fq_lock, FCMASK);
# endif
# endif	/* FILE_AREAS */
#endif /* MICRO || WIN32 */

	if (fd >= 0)
	    level_info[lev].flags |= LFILE_EXISTS;
	else if (errbuf)        /* failure explanation */
	    sprintf(errbuf,
		    "Cannot create file \"%s\" for level %d (errno %d).",
		    lock, lev, errno);

	return fd;
}


int
open_levelfile(lev, errbuf)
int lev;
char errbuf[];
{
	int fd;
#ifndef FILE_AREAS
	const char *fq_lock;
#endif

	if (errbuf) *errbuf = '\0';
	set_levelfile_name(lock, lev);
#ifndef FILE_AREAS
	fq_lock = fqname(lock, LEVELPREFIX, 0);
#endif
#ifdef MFLOPPY
	/* If not currently accessible, swap it in. */
	if (level_info[lev].where != ACTIVE)
		swapin_file(lev);
#endif
#ifdef FILE_AREAS
	fd = open_area(FILE_AREA_LEVL, lock, O_RDONLY | O_BINARY, 0);
#else
# ifdef MAC
	fd = macopen(fq_lock, O_RDONLY | O_BINARY, LEVL_TYPE);
# else
#  ifdef HOLD_LOCKFILE_OPEN
	if (lev == 0)
		fd = open_levelfile_exclusively(fq_lock, lev, O_RDONLY | O_BINARY );
	else
#  endif
	fd = open(fq_lock, O_RDONLY | O_BINARY, 0);
# endif
#endif	/* FILE_AREAS */

	/* for failure, return an explanation that our caller can use;
	   settle for `lock' instead of `fq_lock' because the latter
	   might end up being too big for nethack's BUFSZ */
	if (fd < 0 && errbuf) {
	    sprintf(errbuf,
		    "Cannot open file \"%s\" for level %d (errno %d).",
		    lock, lev, errno);
	    pline("THIS IS A FATAL BUG. DO NOT TRY TO SAVE THE GAME, OR YOUR SAVEGAME FILE MAY BE CORRUPTED. On a public server the problem can sometimes be solved by using a staircase to return to a previously visited level. If nothing works, please contact the admins. --Amy");
	}

	return fd;
}


void
delete_levelfile(lev)
int lev;
{
	/*
	 * Level 0 might be created by port specific code that doesn't
	 * call create_levfile(), so always assume that it exists.
	 */
	if (lev == 0 || (level_info[lev].flags & LFILE_EXISTS)) {
		set_levelfile_name(lock, lev);
#ifdef FILE_AREAS
		(void) remove_area(FILE_AREA_LEVL, lock, 0);
#else
# ifdef HOLD_LOCKFILE_OPEN
		if (lev == 0) really_close();
# endif
		(void) unlink(fqname(lock, LEVELPREFIX, 0));
#endif
		level_info[lev].flags &= ~LFILE_EXISTS;
	}
}


void
clearlocks()
{
#ifdef HANGUPHANDLING
    if (program_state.preserve_locks)
        return;
#endif

#ifndef NO_SIGNAL
	(void) signal(SIGINT, SIG_IGN);
#if defined(UNIX) || defined(VMS)
	sethanguphandler((void (*)(int)) SIG_IGN);
#endif
#endif

	/* can't access maxledgerno() before dungeons are created -dlc */
	int x;
	for (x = (n_dgns ? maxledgerno() : 0); x >= 0; x--)
		delete_levelfile(x);	/* not all levels need be present */
#  ifdef WHEREIS_FILE
	delete_whereis();
#  endif
}

#ifdef HOLD_LOCKFILE_OPEN
STATIC_OVL int
open_levelfile_exclusively(name, lev, oflag)
const char *name;
int lev, oflag;
{
	int reslt, fd;
	if (!lftrack.init) {
		lftrack.init = 1;
		lftrack.fd = -1;
	}
	if (lftrack.fd >= 0) {
		/* check for compatible access */
		if (lftrack.oflag == oflag) {
			fd = lftrack.fd;
			reslt = lseek(fd, 0L, SEEK_SET);
			if (reslt == -1L)
			    panic("open_levelfile_exclusively: lseek failed %d", errno);
			lftrack.nethack_thinks_it_is_open = TRUE;
		} else {
			really_close();
			fd = sopen(name, oflag,SH_DENYRW, FCMASK);
			lftrack.fd = fd;
			lftrack.oflag = oflag;
			lftrack.nethack_thinks_it_is_open = TRUE;
		}
	} else {
			fd = sopen(name, oflag,SH_DENYRW, FCMASK);
			lftrack.fd = fd;
			lftrack.oflag = oflag;
			if (fd >= 0)
			    lftrack.nethack_thinks_it_is_open = TRUE;
	}
	return fd;
}

void
really_close()
{
	int fd = lftrack.fd;
	lftrack.nethack_thinks_it_is_open = FALSE;
	lftrack.fd = -1;
	lftrack.oflag = 0;
	(void)_close(fd);
	return;
}

int
close(fd)
int fd;
{
 	if (lftrack.fd == fd) {
		really_close();	/* close it, but reopen it to hold it */
		fd = open_levelfile(0, (char *)0);
		lftrack.nethack_thinks_it_is_open = FALSE;
		return 0;
	}
	return _close(fd);
}
#endif

#ifdef WHEREIS_FILE
void
touch_whereis()
{
  /* Write out our current level and branch to name.whereis
   *
   *      Could eventually bolt on all kinds of info, but this way
   *      at least something which wants to can scan for the games.
   *
   * For now this only works on Win32 and UNIX.  I'm too lazy
   * to sort out all the proper other-OS stuff.
   */

	/* certain nasty traps obscure the information - so we want to hide it from whereis too! --Amy */
	if (DisplayDoesNotGo || TheInfoIsFucked) {
		delete_whereis();
		return;
	}

  FILE* fp;
  char whereis_file[255];
  char whereis_work[255];

  sprintf(whereis_file,"%s",dump_format_str(WHEREIS_FILE));
  sprintf(whereis_work,
	  "player=%s%s%s%s:depth=%d:dnum=%d:dname=%s:turns=%ld:score=%ld:role=%s:race=%s:gender=%s:align=%s:amulet=0\n",
	  plalias[0] ? plalias : plname,
	  plalias[0] ? " (" : "",
	  plalias[0] ? plname : "",
	  plalias[0] ? ")" : "",
	  depth(&u.uz),
	  u.uz.dnum,
	  dungeons[u.uz.dnum].dname,
	  moves,
	  botl_score(),
	  urole.filecode,
	  urace.filecode,
	  genders[flags.female].filecode,
	  aligns[1-u.ualign.type].filecode);
  fp = fopen_datafile_area(FILE_AREA_VAR, whereis_file, "w", LEVELPREFIX);
  if (fp) {
#ifdef UNIX
    mode_t whereismode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;
    chmod(fqname(whereis_file, LEVELPREFIX, 2), whereismode);
#endif
    fwrite(whereis_work,strlen(whereis_work),1,fp);
    fclose(fp);
  }

}


/* Changed over to write out where the player last was when they
 * left the game; including possibly 'dead' :) */
void
delete_whereis()
{
    /*FILE* fp;*/
  char whereis_file[255];
  /*char whereis_work[255];*/
  sprintf(whereis_file,"%s",dump_format_str(WHEREIS_FILE));
  (void) unlink(fqname(whereis_file, LEVELPREFIX, 2));
  /*
  fp = fopen_datafile(whereis_file,"w",LEVELPREFIX);
  if (fp) {
    fwrite(whereis_work,strlen(whereis_work),1,fp);
    fclose(fp);
  }
  */
}
#endif /* WHEREIS_FILE */

/* ----------  END LEVEL FILE HANDLING ----------- */


/* ----------  BEGIN BONES FILE HANDLING ----------- */

/* set up "file" to be file name for retrieving bones, and return a
 * bonesid to be read/written in the bones file.
 */
STATIC_OVL char *
set_bonesfile_name(file, lev)
char *file;
d_level *lev;
{
	static char bonesid[75]; 
	s_level *sptr;
	char *dptr;

	/* We need more than 26 different IDs, and the yacc scanner is being stupid. If I try to use bison to generate
	 * an updated one, it's somehow lacking a lot of stuff because the dgn_comp.y simply doesn't have many of the
	 * functions present in dgn_yacc.c, what the hell? So I can't just make a second bones id to increase the amount
	 * of ones we can have. But, apparently we can just put the dungeon branch name in the file name instead! --Amy
	 * ... but then the bones level fails to link correctly :( */

	/* used to be: sprintf(bonesid, "%c.%s" */

	sprintf(bonesid, "%s.%s", /*dungeons[lev->dnum].boneid*/bonedunlvl(lev->dnum),
			In_quest(lev) ? urole.filecode : "0");
	dptr = eos(bonesid);
	if ((sptr = Is_special(lev)) != 0)
	    sprintf(dptr, ".%c", sptr->boneid);
	else
	    sprintf(dptr, ".%d", lev->dlevel);
	sprintf(file, "bon%s", bonesid); 
#ifdef BONES_POOL 
	sprintf(eos(file), ".%ld", (u.ubirthday % 10)); 
#endif
#ifdef VMS
	strcat(file, ";1");
#endif
	return(bonesid);
}

/* set up temporary file name for writing bones, to avoid another game's
 * trying to read from an uncompleted bones file.  we want an uncontentious
 * name, so use one in the namespace reserved for this game's level files.
 * (we are not reading or writing level files while writing bones files, so
 * the same array may be used instead of copying.)
 */
STATIC_OVL char *
set_bonestemp_name()
{
	char *tf;

	tf = rindex(lock, '.');
	if (!tf) tf = eos(lock);
	sprintf(tf, ".bn");
#ifdef VMS
	strcat(tf, ";1");
#endif
	return lock;
}

int
create_bonesfile(lev, bonesid, errbuf)
d_level *lev;
char **bonesid;
char errbuf[];
{
	const char *file;
	int fd;

	if (errbuf) *errbuf = '\0';
	*bonesid = set_bonesfile_name(bones, lev);
	file = set_bonestemp_name();
#ifndef FILE_AREAS
	file = fqname(file, BONESPREFIX, 0);
#endif

#if defined(MICRO) || defined(WIN32)
	/* Use O_TRUNC to force the file to be shortened if it already
	 * exists and is currently longer.
	 */
# ifdef FILE_AREAS
	fd = open_area(FILE_AREA_BONES, file,
	  O_WRONLY |O_CREAT | O_TRUNC | O_BINARY, FCMASK);
# else
	fd = open(file, O_WRONLY |O_CREAT | O_TRUNC | O_BINARY, FCMASK);
# endif
#else
# ifdef FILE_AREAS
	fd = creat_area(FILE_AREA_BONES, file, FCMASK);
# else
# ifdef MAC
	fd = maccreat(file, BONE_TYPE);
# else
	fd = creat(file, FCMASK);
# endif
# endif	/* FILE_AREAS */
#endif
	if (fd < 0 && errbuf) /* failure explanation */
	    sprintf(errbuf,
		    "Cannot create bones \"%s\", id %s (errno %d).",
		    lock, *bonesid, errno);

# if defined(VMS) && !defined(SECURE)
	/*
	   Re-protect bones file with world:read+write+execute+delete access.
	   umask() doesn't seem very reliable; also, vaxcrtl won't let us set
	   delete access without write access, which is what's really wanted.
	   Can't simply create it with the desired protection because creat
	   ANDs the mask with the user's default protection, which usually
	   denies some or all access to world.
	 */
#  ifndef FILE_AREAS
	(void) chmod(file, FCMASK | 007);  /* allow other users full access */
#  else
	(void) chmod_area(FILE_AREA_BONES, file, FCMASK | 007);
#  endif
# endif /* VMS && !SECURE */

	return fd;
}

#ifdef MFLOPPY
/* remove partial bonesfile in process of creation */
void
cancel_bonesfile()
{
	const char *tempname;

	tempname = set_bonestemp_name();
# ifdef FILE_AREAS
	(void) remove_area(FILE_AREA_BONES, tempname);
# else
	tempname = fqname(tempname, BONESPREFIX, 0);
	(void) unlink(tempname);
# endif
}
#endif /* MFLOPPY */

/* move completed bones file to proper name */
void
commit_bonesfile(lev)
d_level *lev;
{
	const char *fq_bones, *tempname;
	int ret;

	(void) set_bonesfile_name(bones, lev);
#ifndef FILE_AREAS
	fq_bones = fqname(bones, BONESPREFIX, 0);
#endif
	tempname = set_bonestemp_name();
#ifndef FILE_AREAS
	tempname = fqname(tempname, BONESPREFIX, 1);
#endif

#ifdef FILE_AREAS
	ret = rename_area(FILE_AREA_BONES, tempname, bones);
#else
# if (defined(SYSV) && !defined(SVR4)) || defined(GENIX)
	/* old SYSVs don't have rename.  Some SVR3's may, but since they
	 * also have link/unlink, it doesn't matter. :-)
	 */
	(void) unlink(fq_bones);
	ret = link(tempname, fq_bones);
	ret += unlink(tempname);
# else
	ret = rename(tempname, fq_bones);
# endif
#endif	/* FILE_AREAS */
#ifdef WIZARD
	if (wizard && ret != 0)
#ifdef FILE_AREAS
		pline("couldn't rename %s to %s.", tempname, bones);
#else
		pline("couldn't rename %s to %s.", tempname, fq_bones);
#endif
#endif
}


int
open_bonesfile(lev, bonesid)
d_level *lev;
char **bonesid;
{
	const char *fq_bones;
	int fd;

	*bonesid = set_bonesfile_name(bones, lev);
#ifdef FILE_AREAS
	uncompress_area(FILE_AREA_BONES, bones);  /* no effect if nonexistent */
	fd = open_area(FILE_AREA_BONES, bones, O_RDONLY | O_BINARY, 0);
#else
	fq_bones = fqname(bones, BONESPREFIX, 0);
	uncompress(fq_bones);	/* no effect if nonexistent */
# ifdef MAC
	fd = macopen(fq_bones, O_RDONLY | O_BINARY, BONE_TYPE);
# else
	fd = open(fq_bones, O_RDONLY | O_BINARY, 0);
# endif
#endif	/* FILE_AREAS */
	return fd;
}


int
delete_bonesfile(lev)
d_level *lev;
{
	(void) set_bonesfile_name(bones, lev);
#ifdef FILE_AREAS
	return !(remove_area(FILE_AREA_BONES, bones) < 0);
#else
	return !(unlink(fqname(bones, BONESPREFIX, 0)) < 0);
#endif
}


/* assume we're compressing the recently read or created bonesfile, so the
 * file name is already set properly */
void
compress_bonesfile()
{
#ifdef FILE_AREAS
	compress_area(FILE_AREA_BONES, bones);
#else
	compress(fqname(bones, BONESPREFIX, 0));
#endif
}

/* ----------  END BONES FILE HANDLING ----------- */


/* ----------  BEGIN SAVE FILE HANDLING ----------- */

/* set savefile name in OS-dependent manner from pre-existing plname,
 * avoiding troublesome characters */
void
set_savefile_name()
{
#if defined(WIN32)
	char fnamebuf[BUFSZ], encodedfnamebuf[BUFSZ];
#endif
#ifdef VMS
#ifndef FILE_AREAS
	sprintf(SAVEF, "[.save]%d%s", getuid(), plname);
	regularize(SAVEF+7);
#else
	sprintf(SAVEF, "%d%s", getuid(), plname);
	regularize(SAVEF);
#endif
	strcat(SAVEF, ";1");
#else
# if defined(MICRO)
	strcpy(SAVEF, SAVEP);
#  ifdef AMIGA
	strncat(SAVEF, bbs_id, PATHLEN);
#  endif
	{
		int i = strlen(SAVEP);
#  ifdef AMIGA
		/* plname has to share space with SAVEP and ".sav" */
		(void)strncat(SAVEF, plname, FILENAME - i - 4);
#  else
		(void)strncat(SAVEF, plname, 8);
#  endif
		regularize(SAVEF+i);
	}
	strcat(SAVEF, ".sav");
# else
#  ifndef FILE_AREAS
#  if defined(WIN32)
	/* Obtain the name of the logged on user and incorporate
	 * it into the name. */
	sprintf(fnamebuf, "%s-%s", get_username(0), plname);
	(void)fname_encode("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_-.",
				'%', fnamebuf, encodedfnamebuf, BUFSZ);
	sprintf(SAVEF, "%s.NetHack-saved-game", encodedfnamebuf);
#  else
	sprintf(SAVEF, "save/%d%s", (int)getuid(), plname);
	regularize(SAVEF+5);	/* avoid . or / in name */
#  endif /* WIN32 */
#  else
	sprintf(SAVEF, "%d%s", (int)getuid(), plname);
	regularize(SAVEF);      /* avoid . or / in name */
#  endif
# endif	/* MICRO */
#endif /* VMS   */
}

#ifdef INSURANCE
void
save_savefile_name(fd)
int fd;
{
	(void) write(fd, (void *) SAVEF, sizeof(SAVEF));
}
#endif


#if defined(WIZARD) && !defined(MICRO)
/* change pre-existing savefile name to indicate an error savefile */
void
set_error_savefile()
{
# ifdef VMS
      {
	char *semi_colon = rindex(SAVEF, ';');
	if (semi_colon) *semi_colon = '\0';
      }
	strcat(SAVEF, ".e;1");
# else
#  ifdef MAC
	strcat(SAVEF, "-e");
#  else
	strcat(SAVEF, ".e");
#  endif
# endif
}
#endif


/* create save file, overwriting one if it already exists */
int
create_savefile()
{
#ifndef FILE_AREAS
	const char *fq_save;
#endif
	int fd;

#ifdef FILE_AREAS
# ifdef MICRO
	fd = open_area(FILE_AREA_SAVE, SAVEF,
	  O_WRONLY | O_BINARY | O_CREAT | O_TRUNC, FCMASK);
# else
	fd = creat_area(FILE_AREA_SAVE, SAVEF, FCMASK);
# endif
#else	/* FILE_AREAS */
	fq_save = fqname(SAVEF, SAVEPREFIX, 0);
# if defined(MICRO) || defined(WIN32)
	fd = open(fq_save, O_WRONLY | O_BINARY | O_CREAT | O_TRUNC, FCMASK);
# else
#  ifdef MAC
	fd = maccreat(fq_save, SAVE_TYPE);
#  else
	fd = creat(fq_save, FCMASK);
#  endif
# endif /* MICRO */
#endif  /* FILE_AREAS */

#if defined(VMS) && !defined(SECURE)
	/*
	   Make sure the save file is owned by the current process.  That's
	   the default for non-privileged users, but for priv'd users the
	   file will be owned by the directory's owner instead of the user.
	 */
#  ifdef getuid	/*(see vmsunix.c)*/
#   undef getuid
#  endif
# ifdef FILE_AREAS
	(void) chown_area(FILE_AREA_SAVE, SAVEF, getuid(), getgid());
# else
	(void) chown(fq_save, getuid(), getgid());
# endif
#endif /* VMS && !SECURE */

	return fd;
}


/* open savefile for reading */
int
open_savefile()
{
	int fd;

#ifdef FILE_AREAS
	fd = open_area(FILE_AREA_SAVE, SAVEF, O_RDONLY | O_BINARY, 0);
#else
	const char *fq_save;
	fq_save = fqname(SAVEF, SAVEPREFIX, 0);
# ifdef MAC
	fd = macopen(fq_save, O_RDONLY | O_BINARY, SAVE_TYPE);
# else
	fd = open(fq_save, O_RDONLY | O_BINARY, 0);
# endif
#endif	/* FILE_AREAS */
	return fd;
}


/* delete savefile */
int
delete_savefile()
{
/*WAC OK...this is probably a contreversial addition.  It's an option tho*/
#ifdef KEEP_SAVE
/* Wizard mode already has prompt*/
	if (flags.keep_savefile && !wizard) {
# ifdef AMIGA /*WAC If Amiga is ever supported*/
		preserve_icon();
# endif
		return 1; /*Should this return 0?*/
	}
#endif

#ifdef FILE_AREAS
	(void) remove_area(FILE_AREA_SAVE, SAVEF);
#else
	(void) unlink(fqname(SAVEF, SAVEPREFIX, 0));
#endif
	return 0;	/* for restore_saved_game() (ex-xxxmain.c) test */
}


/* try to open up a save file and prepare to restore it */
int
restore_saved_game()
{
#ifndef FILE_AREAS
	const char *fq_save;
#endif
	int fd;

	set_savefile_name();
#ifdef MFLOPPY
	if (!saveDiskPrompt(1))
	    return -1;
#endif /* MFLOPPY */
#ifndef FILE_AREAS
	fq_save = fqname(SAVEF, SAVEPREFIX, 0);

	uncompress(fq_save);
#else
	uncompress_area(FILE_AREA_SAVE, SAVEF);
#endif
	if ((fd = open_savefile()) < 0) return fd;

#ifndef FILE_AREAS
	if (!uptodate(fd, fq_save)) {
#else
	if (!uptodate(fd, SAVEF)) {
#endif
	    (void) close(fd),  fd = -1;
	if (yn("Delete the old file?") == 'y') /* Damn you, sadistic programmers who delete stuff without asking! --Amy */
	    (void) delete_savefile();
	}
	return fd;
}

#if defined(UNIX) && defined(QT_GRAPHICS)
/*ARGSUSED*/
static char*
plname_from_file(filename)
const char* filename;
{
#ifdef STORE_PLNAME_IN_FILE
    int fd;
    char* result = 0;

    strcpy(SAVEF,filename);
#ifdef COMPRESS_EXTENSION
    SAVEF[strlen(SAVEF)-strlen(COMPRESS_EXTENSION)] = '\0';
#endif
    uncompress(SAVEF);
    if ((fd = open_savefile()) >= 0) {
	if (uptodate(fd, filename)) {
	    char tplname[PL_NSIZ];
	    mread(fd, (void *) tplname, PL_NSIZ);
	    result = strdup(tplname);
	}
	(void) close(fd);
    }
    compress(SAVEF);

    return result;
#else
# if defined(UNIX) && defined(QT_GRAPHICS)
    /* Name not stored in save file, so we have to extract it from
       the filename, which loses information
       (eg. "/", "_", and "." characters are lost. */
    int k;
    int uid;
    char name[64]; /* more than PL_NSIZ */
#ifdef COMPRESS_EXTENSION
#define EXTSTR COMPRESS_EXTENSION
#else
#define EXTSTR ""
#endif
    if ( sscanf( filename, "%*[^/]/%d%63[^.]" EXTSTR, &uid, name ) == 2 ) {
#undef EXTSTR
    /* "_" most likely means " ", which certainly looks nicer */
	for (k=0; name[k]; k++)
	    if ( name[k]=='_' )
		name[k]=' ';
	return strdup(name);
    } else
# endif
    {
	return 0;
    }
#endif
}
#endif /* defined(UNIX) && defined(QT_GRAPHICS) */

char**
get_saved_games()
{
#if defined(UNIX) && defined(QT_GRAPHICS)
    int myuid=getuid();
    struct dirent **namelist;
    int n = scandir("save", &namelist, 0, alphasort);;
    if ( n > 0 ) {
	int i,j=0;
	char** result = (char**)alloc((n+1)*sizeof(char*)); /* at most */
	for (i=0; i<n; i++) {
	    int uid;
	    char name[64]; /* more than PL_NSIZ */
	    if ( sscanf( namelist[i]->d_name, "%d%63s", &uid, name ) == 2 ) {
		if ( uid == myuid ) {
		    char filename[BUFSZ];
		    char* r;
		    sprintf(filename,"save/%d%s",uid,name);
		    r = plname_from_file(filename);
		    if ( r )
			result[j++] = r;
		}
	    }
	}
	result[j++] = 0;
	return result;
    } else
#endif
    {
	return 0;
    }
}

void
free_saved_games(saved)
char** saved;
{
    if ( saved ) {
	int i=0;
	while (saved[i]) free((void *)saved[i++]);
	free((void *)saved);
    }
}


/* ----------  END SAVE FILE HANDLING ----------- */


/* ----------  BEGIN FILE COMPRESSION HANDLING ----------- */

#ifdef COMPRESS

STATIC_OVL void
redirect(filearea, filename, mode, stream, uncomp)
const char *filearea, *filename, *mode;
FILE *stream;
boolean uncomp;
{
#ifndef FILE_AREAS
	if (freopen(filename, mode, stream) == (FILE *)0) {
#else
	if (freopen_area(filearea, filename, mode, stream) == (FILE *)0) {
#endif
		(void) fprintf(stderr, "redirect of %s for %scompress failed\n",
			filename, uncomp ? "un" : "");
		terminate(EXIT_FAILURE);
	}
}

/*
 * using system() is simpler, but opens up security holes and causes
 * problems on at least Interactive UNIX 3.0.1 (SVR3.2), where any
 * setuid is renounced by /bin/sh, so the files cannot be accessed.
 *
 * cf. child() in unixunix.c.
 */
STATIC_OVL void
docompress_file(filearea, filename, uncomp)
const char *filearea, *filename;
boolean uncomp;
{
	char cfn[80];
	FILE *cf;
	const char *args[10];
# ifdef COMPRESS_OPTIONS
	char opts[80];
# endif
	int i = 0;
	int f;
# ifdef TTY_GRAPHICS
	boolean istty = !strncmpi(windowprocs.name, "tty", 3);
# endif

	strcpy(cfn, filename);
# ifdef COMPRESS_EXTENSION
	strcat(cfn, COMPRESS_EXTENSION);
# endif
	/* when compressing, we know the file exists */
	if (uncomp) {
	    if ((cf = fopen_datafile_area(filearea, cfn, RDBMODE, FALSE)) ==
	      (FILE *)0)
		    return;
	    (void) fclose(cf);
	}

	args[0] = COMPRESS;
	if (uncomp) args[++i] = "-d";	/* uncompress */
# ifdef COMPRESS_OPTIONS
	{
	    /* we can't guarantee there's only one additional option, sigh */
	    char *opt;
	    boolean inword = FALSE;

	    strcpy(opts, COMPRESS_OPTIONS);
	    opt = opts;
	    while (*opt) {
		if ((*opt == ' ') || (*opt == '\t')) {
		    if (inword) {
			*opt = '\0';
			inword = FALSE;
		    }
		} else if (!inword) {
		    args[++i] = opt;
		    inword = TRUE;
		}
		opt++;
	    }
	}
# endif
	args[++i] = (char *)0;

# ifdef TTY_GRAPHICS
	/* If we don't do this and we are right after a y/n question *and*
	 * there is an error message from the compression, the 'y' or 'n' can
	 * end up being displayed after the error message.
	 */
	if (istty)
	    mark_synch();
# endif
	f = fork();
	if (f == 0) {	/* child */
# ifdef TTY_GRAPHICS
		/* any error messages from the compression must come out after
		 * the first line, because the more() to let the user read
		 * them will have to clear the first line.  This should be
		 * invisible if there are no error messages.
		 */
		if (istty)
		    raw_print("");
# endif
		/* run compressor without privileges, in case other programs
		 * have surprises along the line of gzip once taking filenames
		 * in GZIP.
		 */
		/* assume all compressors will compress stdin to stdout
		 * without explicit filenames.  this is true of at least
		 * compress and gzip, those mentioned in config.h.
		 */
		if (uncomp) {
			redirect(filearea, cfn, RDBMODE, stdin, uncomp);
			redirect(filearea, filename, WRBMODE, stdout, uncomp);
		} else {
			redirect(filearea, filename, RDBMODE, stdin, uncomp);
			redirect(filearea, cfn, WRBMODE, stdout, uncomp);
		}
		(void) setgid(getgid());
		(void) setuid(getuid());
		(void) execv(args[0], (char *const *) args);
		perror((char *)0);
		(void) fprintf(stderr, "Exec to %scompress %s failed.\n",
			uncomp ? "un" : "", filename);
		terminate(EXIT_FAILURE);
	} else if (f == -1) {
		perror((char *)0);
		pline("Fork to %scompress %s failed.",
			uncomp ? "un" : "", filename);
		return;
	}
# ifndef NO_SIGNAL
	(void) signal(SIGINT, SIG_IGN);
	(void) signal(SIGQUIT, SIG_IGN);
# endif
	(void) wait((int *)&i);
# ifndef NO_SIGNAL
	(void) signal(SIGINT, (SIG_RET_TYPE) done1);
# endif
# ifdef WIZARD
	if (wizard) (void) signal(SIGQUIT, SIG_DFL);
# endif
	if (i == 0) {
	    /* (un)compress succeeded: remove file left behind */
	    if (uncomp)
#ifndef FILE_AREAS
		(void) unlink(cfn);
#else
		(void) remove_area(filearea, cfn);
#endif
	    else
#ifndef FILE_AREAS
		(void) unlink(filename);
#else
		(void) remove_area(filearea, filename);
#endif
	} else {
	    /* (un)compress failed; remove the new, bad file */
	    if (uncomp) {
		raw_printf("Unable to uncompress %s", filename);
		(void) unlink(filename);
	    } else {
		/* no message needed for compress case; life will go on */
		(void) unlink(cfn);
	    }
#ifdef TTY_GRAPHICS
	    /* Give them a chance to read any error messages from the
	     * compression--these would go to stdout or stderr and would get
	     * overwritten only in tty mode.  It's still ugly, since the
	     * messages are being written on top of the screen, but at least
	     * the user can read them.
	     */
	    if (istty)
	    {
		clear_nhwindow(WIN_MESSAGE);
		more();
		/* No way to know if this is feasible */
		/* doredraw(); */
	    }
#endif
	}
}
#endif	/* COMPRESS */

/* compress file */
void
compress_area(filearea, filename)
const char *filearea, *filename;
{
#ifndef COMPRESS
#if defined(MAC_MPW) || defined(__MWERKS__)
# pragma unused(filename)
#endif
#else
	docompress_file(filearea, filename, FALSE);
#endif
}


/* uncompress file if it exists */
void
uncompress_area(filearea, filename)
const char *filearea, *filename;
{
#ifndef COMPRESS
#if defined(MAC_MPW) || defined(__MWERKS__)
# pragma unused(filename)
#endif
#else
	docompress_file(filearea, filename, TRUE);
#endif
}

/* ----------  END FILE COMPRESSION HANDLING ----------- */


/*
 * When file areas are in use, (un)lock_file_area are used instead.
 */

#ifndef FILE_AREAS

/* ----------  BEGIN FILE LOCKING HANDLING ----------- */

static int nesting = 0;

#if defined(NO_FILE_LINKS) || defined(USE_FCNTL) 	/* implies UNIX */
static int lockfd;	/* for lock_file() to pass to unlock_file() */
#endif
#ifdef USE_FCNTL
struct flock sflock; /* for unlocking, same as above */
#endif

#define HUP	if (!program_state.done_hup)

#ifndef USE_FCNTL
STATIC_OVL char *
make_lockname(filename, lockname)
const char *filename;
char *lockname;
{
#if defined(MAC_MPW) || defined(__MWERKS__)
# pragma unused(filename,lockname)
	return (char*)0;
#else
# if defined(UNIX) || defined(VMS) || defined(AMIGA) || defined(WIN32) || defined(MSDOS)
#  ifdef NO_FILE_LINKS
	strcpy(lockname, LOCKDIR);
	strcat(lockname, "/");
	strcat(lockname, filename);
#  else
	strcpy(lockname, filename);
#  endif
#  ifdef VMS
      {
	char *semi_colon = rindex(lockname, ';');
	if (semi_colon) *semi_colon = '\0';
      }
	strcat(lockname, ".lock;1");
#  else
	strcat(lockname, "_lock");
#  endif
	return lockname;
# else
	lockname[0] = '\0';
	return (char*)0;
# endif  /* UNIX || VMS || AMIGA || WIN32 || MSDOS */
#endif
}
#endif /* !USE_FCNTL */


/* lock a file */
boolean
lock_file(filename, whichprefix, retryct)
const char *filename;
int whichprefix;
int retryct;
{
#if defined(MAC_MPW) || defined(__MWERKS__)
# pragma unused(filename, retryct)
#endif
#ifndef USE_FCNTL
	char locknambuf[BUFSZ];
	const char *lockname;
#endif

	nesting++;
	if (nesting > 1) {
	    impossible("TRIED TO NEST LOCKS");
	    return TRUE;
	}

#ifndef USE_FCNTL
	lockname = make_lockname(filename, locknambuf);
#ifndef NO_FILE_LINKS	/* LOCKDIR should be subsumed by LOCKPREFIX */
	lockname = fqname(lockname, LOCKPREFIX, 2);
#endif
#endif
	filename = fqname(filename, whichprefix, 0);

#ifdef USE_FCNTL
	lockfd = open(filename,O_RDWR);
	if (lockfd == -1) {
		HUP raw_printf("Cannot open file %s. This is a program bug.",
			filename);
	}
	sflock.l_type = F_WRLCK;
	sflock.l_whence = SEEK_SET;
	sflock.l_start = 0;
	sflock.l_len = 0;
#endif

#if defined(UNIX) || defined(VMS)
# ifdef USE_FCNTL
	while (fcntl(lockfd,F_SETLK,&sflock) == -1) {
# else 
# ifdef NO_FILE_LINKS
	while ((lockfd = open(lockname, O_RDWR|O_CREAT|O_EXCL, 0666)) == -1) {
# else
	while (link(filename, lockname) == -1) {
# endif
# endif 

#ifdef USE_FCNTL
		if (retryct--) {
			HUP raw_printf(
				"Waiting for release of fcntl lock on %s. (%d retries left).",
				filename, retryct);
			sleep(1);
		} else {
		    HUP (void) raw_print("I give up.  Sorry.");
		    HUP raw_printf("Some other process has an unnatural grip on %s.",
					filename);
		    nesting--;
		    return FALSE;
		}
#else
	    register int errnosv = errno;

	    switch (errnosv) {	/* George Barbanis */
	    case EEXIST:
		if (retryct--) {
		    HUP raw_printf(
			    "Waiting for access to %s.  (%d retries left).",
			    filename, retryct);
# if defined(SYSV) || defined(ULTRIX) || defined(VMS)
		    (void)
# endif
			sleep(1);
		} else {
		    HUP (void) raw_print("I give up.  Sorry.");
		    HUP raw_printf("Perhaps there is an old %s around?",
					lockname);
		    nesting--;
		    return FALSE;
		}

		break;
	    case ENOENT:
		HUP raw_printf("Can't find file %s to lock!", filename);
		nesting--;
		return FALSE;
	    case EACCES:
		HUP raw_printf("No write permission to lock %s!", filename);
		nesting--;
		return FALSE;
# ifdef VMS			/* c__translate(vmsfiles.c) */
	    case EPERM:
		/* could be misleading, but usually right */
		HUP raw_printf("Can't lock %s due to directory protection.",
			       filename);
		nesting--;
		return FALSE;
# endif
	    default:
		HUP perror(lockname);
		HUP raw_printf(
			     "Cannot lock %s for unknown reason (%d).",
			       filename, errnosv);
		nesting--;
		return FALSE;
	    }
#endif /* USE_FCNTL */

	}
#endif  /* UNIX || VMS */

#if (defined(AMIGA) || defined(WIN32) || defined(MSDOS)) && !defined(USE_FCNTL)
# ifdef AMIGA
#define OPENFAILURE(fd) (!fd)
    lockptr = 0;
# else
#define OPENFAILURE(fd) (fd < 0)
    lockptr = -1;
# endif
    while (--retryct && OPENFAILURE(lockptr)) {
# if defined(WIN32) && !defined(WIN_CE)
	lockptr = sopen(lockname, O_RDWR|O_CREAT, SH_DENYRW, S_IWRITE);
# else
	(void)DeleteFile(lockname); /* in case dead process was here first */
#  ifdef AMIGA
	lockptr = Open(lockname,MODE_NEWFILE);
#  else
	lockptr = open(lockname, O_RDWR|O_CREAT|O_EXCL, S_IWRITE);
#  endif
# endif
	if (OPENFAILURE(lockptr)) {
	    raw_printf("Waiting for access to %s.  (%d retries left).",
			filename, retryct);
	    Delay(50);
	}
    }
    if (!retryct) {
	raw_printf("I give up.  Sorry.");
	nesting--;
	return FALSE;
    }
#endif /* AMIGA || WIN32 || MSDOS */
	return TRUE;
}


#ifdef VMS	/* for unlock_file, use the unlink() routine in vmsunix.c */
# ifdef unlink
#  undef unlink
# endif
# define unlink(foo) vms_unlink(foo)
#endif

/* unlock file, which must be currently locked by lock_file */
void
unlock_file(filename)
const char *filename;
{
#ifndef USE_FCNTL
	char locknambuf[BUFSZ];
	const char *lockname;
#endif

	if (nesting == 1) {
#ifdef USE_FCNTL
		sflock.l_type = F_UNLCK;
		if (fcntl(lockfd,F_SETLK,&sflock) == -1) {
			HUP raw_printf("Can't remove fcntl lock on %s.", filename);
			(void) close(lockfd);
		}
# else
		lockname = make_lockname(filename, locknambuf);
#ifndef NO_FILE_LINKS	/* LOCKDIR should be subsumed by LOCKPREFIX */
		lockname = fqname(lockname, LOCKPREFIX, 2);
#endif

#if defined(UNIX) || defined(VMS)
		if (unlink(lockname) < 0)
			HUP raw_printf("Can't unlink %s.", lockname);
# ifdef NO_FILE_LINKS
		(void) close(lockfd);
# endif

#endif  /* UNIX || VMS */

#if defined(AMIGA) || defined(WIN32) || defined(MSDOS)
		if (lockptr) Close(lockptr);
		DeleteFile(lockname);
		lockptr = 0;
#endif /* AMIGA || WIN32 || MSDOS */
#endif /* USE_FCNTL */
	}

	nesting--;
}

/* ----------  END FILE LOCKING HANDLING ----------- */

#endif	/* FILE_AREAS */

/* ----------  BEGIN CONFIG FILE HANDLING ----------- */

const char *configfile =
                        NH_CONFIG_FILE;
/* WAC This stuff is in filename.h now
#ifdef UNIX
			".nethackrc";
#else
# if defined(MAC) || defined(__BEOS__)
			"NetHack Defaults";
#  if defined(MSDOS) || defined(WIN32)
			"defaults.nh";
#  else
			"NetHack.cnf";
#  endif
# endif
#endif
*/


#ifdef MSDOS
/* conflict with speed-dial under windows
 * for XXX.cnf file so support of NetHack.cnf
 * is for backward compatibility only.
 * Preferred name (and first tried) is now defaults.nh but
 * the game will try the old name if there
 * is no defaults.nh.
 */
const char *backward_compat_configfile = "nethack.cnf"; 
#endif

#ifndef MFLOPPY
#define fopenp fopen
#endif

STATIC_OVL FILE *
fopen_config_file(filename)
const char *filename;
{
	FILE *fp;
#if defined(UNIX) || defined(VMS)
	char	tmp_config[BUFSZ];
	char *envp;
#endif

	/* "filename" is an environment variable, so it should hang around */
	/* if set, it is expected to be a full path name (if relevant) */
	if (filename) {
#ifdef UNIX
		if (access(filename, 4) == -1) {
			/* 4 is R_OK on newer systems */
			/* nasty sneaky attempt to read file through
			 * NetHack's setuid permissions -- this is the only
			 * place a file name may be wholly under the player's
			 * control
			 */
			raw_printf("Access to %s denied (%d).",
					filename, errno);
			wait_synch();
			/* fall through to standard names */
		} else
#endif
		if ((fp = fopenp(filename, "r")) != (FILE *)0) {
		    configfile = filename;
		    return(fp);
#if defined(UNIX) || defined(VMS)
		} else {
		    /* access() above probably caught most problems for UNIX */
		    raw_printf("Couldn't open requested config file %s (%d).",
					filename, errno);
		    wait_synch();
		    /* fall through to standard names */
#endif
		}
	}

#if defined(MICRO) || defined(MAC) || defined(__BEOS__) || defined(WIN32)
	if ((fp = fopenp(fqname(configfile, CONFIGPREFIX, 0), "r"))
								!= (FILE *)0)
		return(fp);
# ifdef MSDOS
	else if ((fp = fopenp(fqname(backward_compat_configfile,
					CONFIGPREFIX, 0), "r")) != (FILE *)0)
		return(fp);
# endif
#else
	/* constructed full path names don't need fqname() */
# ifdef VMS
	if ((fp = fopenp(fqname(NH_CONFIG_FILE, CONFIGPREFIX, 0), "r"))
								!= (FILE *)0) {
		configfile = NH_CONFIG_FILE;
		return(fp);
	}
	if ((fp = fopenp(NH_CONFIG_FILE2, "r")) != (FILE *)0) {
		configfile = index(NH_CONFIG_FILE2, ':');
		if (configfile)
			configfile++;
		else
			configfile = NH_CONFIG_FILE2;
		return(fp);
	}

	envp = nh_getenv("HOME");
	if (!envp)
		strcpy(tmp_config, NH_CONFIG_FILE3);
	else
		sprintf(tmp_config, "%s%s", envp, NH_CONFIG_FILE3);
	if ((fp = fopenp(tmp_config, "r")) != (FILE *)0)
		return(fp);
# else	/* should be only UNIX left */
	envp = nh_getenv("HOME");
	if (!envp)
		strcpy(tmp_config, configfile);
	else
		sprintf(tmp_config, "%s/%s", envp, configfile);
	if ((fp = fopenp(tmp_config, "r")) != (FILE *)0)
		return(fp);
# if defined(__APPLE__)
	/* try an alternative */
	if (envp) {
		sprintf(tmp_config, "%s/%s", envp, "Library/Preferences/NetHack Defaults");
		if ((fp = fopenp(tmp_config, "r")) != (FILE *)0)
			return(fp);
		sprintf(tmp_config, "%s/%s", envp, "Library/Preferences/NetHack Defaults.txt");
		if ((fp = fopenp(tmp_config, "r")) != (FILE *)0)
			return(fp);
	}
# endif
	if (errno != ENOENT) {
	    char *details;

	    /* e.g., problems when setuid NetHack can't search home
	     * directory restricted to user */

#if defined (NHSTDC) && !defined(NOTSTDC)
	    if ((details = strerror(errno)) == 0)
#endif
		details = "";
	    raw_printf("Couldn't open default config file %s %s(%d).",
			tmp_config, details, errno);
	    wait_synch();
	}
	else if (!strncmp(windowprocs.name, "proxy/", 6)) {
	    fp = fopenp("/etc/slashem/proxy.slashemrc", "r");
	    if (fp != (FILE *)0)
		return(fp);
	    else if (errno != ENOENT) {
		raw_printf("Couldn't open /etc/slashem/proxy.slashemrc (%d).",
				errno);
		wait_synch();
	    }
	}
# endif
#endif
	return (FILE *)0;

}


/*
 * Retrieve a list of integers from a file into a uchar array.
 *
 * NOTE: zeros are inserted unless modlist is TRUE, in which case the list
 *  location is unchanged.  Callers must handle zeros if modlist is FALSE.
 */
STATIC_OVL int
get_uchars(fp, buf, bufp, list, modlist, size, name)
    FILE *fp;		/* input file pointer */
    char *buf;		/* read buffer, must be of size BUFSZ */
    char *bufp;		/* current pointer */
    uchar *list;	/* return list */
    boolean modlist;	/* TRUE: list is being modified in place */
    int  size;		/* return list size */
    const char *name;		/* name of option for error message */
{
    unsigned int num = 0;
    int count = 0;
    boolean havenum = FALSE;

    while (1) {
	switch(*bufp) {
	    case ' ':  case '\0':
	    case '\t': case '\n':
		if (havenum) {
		    /* if modifying in place, don't insert zeros */
		    if (num || !modlist) list[count] = num;
		    count++;
		    num = 0;
		    havenum = FALSE;
		}
		if (count == size || !*bufp) return count;
		bufp++;
		break;

	    case '0': case '1': case '2': case '3':
	    case '4': case '5': case '6': case '7':
	    case '8': case '9':
		havenum = TRUE;
		num = num*10 + (*bufp-'0');
		bufp++;
		break;

	    case '\\':
		if (fp == (FILE *)0)
		    goto gi_error;
		do  {
		    if (!fgets(buf, BUFSZ, fp)) goto gi_error;
		} while (buf[0] == '#');
		bufp = buf;
		break;

	    default:
gi_error:
		raw_printf("Syntax error in %s", name);
		wait_synch();
		return count;
	}
    }
    /*NOTREACHED*/
}

#ifdef NOCWD_ASSUMPTIONS
STATIC_OVL void
adjust_prefix(bufp, prefixid)
char *bufp;
int prefixid;
{
	char *ptr;

	if (!bufp) return;
	/* Backward compatibility, ignore trailing ;n */ 
	if ((ptr = index(bufp, ';')) != 0) *ptr = '\0';
	if (strlen(bufp) > 0) {
		fqn_prefix[prefixid] = (char *)alloc(strlen(bufp)+2);
		strcpy(fqn_prefix[prefixid], bufp);
		append_slash(fqn_prefix[prefixid]);
	}
}
#endif

#define match_varname(INP,NAM,LEN) match_optname(INP, NAM, LEN, TRUE)

/*ARGSUSED*/
int
parse_config_line(fp, buf, tmp_ramdisk, tmp_levels)
FILE		*fp;
char		*buf;
char		*tmp_ramdisk;
char		*tmp_levels;
{
#if defined(MAC_MPW) || defined(__MWERKS__)
# pragma unused(tmp_ramdisk,tmp_levels)
#endif
	char		*bufp, *altp;
	uchar   translate[MAXPCHARS];
	int   len;

	if (*buf == '#')
		return 1;

	/* remove trailing whitespace */
	bufp = eos(buf);
	while (--bufp > buf && isspace((int)*bufp))
		continue;

	if (bufp <= buf)
		return 1;		/* skip all-blank lines */
	else
		*(bufp + 1) = '\0';	/* terminate line */

	/* find the '=' or ':' */
	bufp = index(buf, '=');
	altp = index(buf, ':');
	if (!bufp || (altp && altp < bufp)) bufp = altp;
	if (!bufp) return 0;

	/* skip  whitespace between '=' and value */
	do { ++bufp; } while (isspace((int)*bufp));

	/* Go through possible variables */
	/* some of these (at least LEVELS and SAVE) should now set the
	 * appropriate fqn_prefix[] rather than specialized variables
	 */
	if (match_varname(buf, "OPTIONS", 4)) {
		parseoptions(bufp, TRUE, TRUE);
		if (plname[0])		/* If a name was given */
			plnamesuffix();	/* set the character class */
	} else if (match_varname(buf, "TILESETS", 7)) {
		parsetileset(bufp);
#ifdef AUTOPICKUP_EXCEPTIONS
	} else if (match_varname(buf, "AUTOPICKUP_EXCEPTION", 5)) {
		add_autopickup_exception(bufp);
#endif
	} else if (match_varname(buf, "BINDINGS", 4)) {
		parsebindings(bufp);
	} else if (match_varname(buf, "AUTOCOMPLETE", 5)) {
		parseautocomplete(bufp, TRUE);
#ifdef NOCWD_ASSUMPTIONS
	} else if (match_varname(buf, "HACKDIR", 4)) {
		adjust_prefix(bufp, HACKPREFIX);
	} else if (match_varname(buf, "LEVELDIR", 4) ||
		   match_varname(buf, "LEVELS", 4)) {
		adjust_prefix(bufp, LEVELPREFIX);
	} else if (match_varname(buf, "SAVEDIR", 4)) {
		adjust_prefix(bufp, SAVEPREFIX);
	} else if (match_varname(buf, "BONESDIR", 5)) {
		adjust_prefix(bufp, BONESPREFIX);
	} else if (match_varname(buf, "DATADIR", 4)) {
		adjust_prefix(bufp, DATAPREFIX);
	} else if (match_varname(buf, "SCOREDIR", 4)) {
		adjust_prefix(bufp, SCOREPREFIX);
	} else if (match_varname(buf, "LOCKDIR", 4)) {
		adjust_prefix(bufp, LOCKPREFIX);
	} else if (match_varname(buf, "CONFIGDIR", 4)) {
		adjust_prefix(bufp, CONFIGPREFIX);
	} else if (match_varname(buf, "TROUBLEDIR", 4)) {
		adjust_prefix(bufp, TROUBLEPREFIX);
#else /*NOCWD_ASSUMPTIONS*/
# ifdef MICRO
	} else if (match_varname(buf, "HACKDIR", 4)) {
		(void) strncpy(hackdir, bufp, PATHLEN-1);
#  ifdef MFLOPPY
	} else if (match_varname(buf, "RAMDISK", 3)) {
				/* The following ifdef is NOT in the wrong
				 * place.  For now, we accept and silently
				 * ignore RAMDISK */
#   ifndef AMIGA
		(void) strncpy(tmp_ramdisk, bufp, PATHLEN-1);
#   endif
#  endif
	} else if (match_varname(buf, "LEVELS", 4)) {
		(void) strncpy(tmp_levels, bufp, PATHLEN-1);

	} else if (match_varname(buf, "SAVE", 4)) {
#  ifdef MFLOPPY
		extern	int saveprompt;
#  endif
		char *ptr;
		if ((ptr = index(bufp, ';')) != 0) {
			*ptr = '\0';
#  ifdef MFLOPPY
			if (*(ptr+1) == 'n' || *(ptr+1) == 'N') {
				saveprompt = FALSE;
			}
#  endif
		}
# ifdef	MFLOPPY
		else
		    saveprompt = flags.asksavedisk;
# endif

		(void) strncpy(SAVEP, bufp, SAVESIZE-1);
		append_slash(SAVEP);
# endif /* MICRO */
#endif /*NOCWD_ASSUMPTIONS*/

	} else if (match_varname(buf, "NAME", 4)) {
	    (void) strncpy(plname, bufp, PL_NSIZ-1);
	    plnamesuffix();
	} else if (match_varname(buf, "MSGTYPE", 7)) {
	    char pattern[256];
	    char msgtype[11];
	    if (sscanf(bufp, "%10s \"%255[^\"]\"", msgtype, pattern) == 2) {
		int typ = MSGTYP_NORMAL;
		if (!strcasecmp("norep", msgtype)) typ = MSGTYP_NOREP;
		else if (!strcasecmp("hide", msgtype)) typ = MSGTYP_NOSHOW;
		else if (!strcasecmp("noshow", msgtype)) typ = MSGTYP_NOSHOW;
		else if (!strcasecmp("more", msgtype)) typ = MSGTYP_STOP;
		else if (!strcasecmp("stop", msgtype)) typ = MSGTYP_STOP;
		if (typ != MSGTYP_NORMAL) {
		    msgpline_add(typ, pattern);
		}
	    }
	} else if (match_varname(buf, "ROLE", 4) ||
		   match_varname(buf, "CHARACTER", 4)) {
	    if ((len = str2role(bufp)) >= 0)
	    	flags.initrole = len;
	} else if (match_varname(buf, "DOGNAME", 3)) {
	    (void) strncpy(dogname, bufp, PL_PSIZ-1);
	} else if (match_varname(buf, "MONKEYNAME", 3)) {
	    (void) strncpy(monkeyname, bufp, PL_PSIZ-1);
	} else if (match_varname(buf, "PARROTNAME", 3)) {
	    (void) strncpy(parrotname, bufp, PL_PSIZ-1);
	} else if (match_varname(buf, "GIRLNAME", 3)) {
	    (void) strncpy(girlname, bufp, PL_PSIZ-1);
	} else if (match_varname(buf, "BOYNAME", 3)) {
	    (void) strncpy(boyname, bufp, PL_PSIZ-1);
	} else if (match_varname(buf, "RAVENNAME", 3)) {
	    (void) strncpy(ravenname, bufp, PL_PSIZ-1);
	} else if (match_varname(buf, "DRAGONNAME", 3)) {
	    (void) strncpy(dragonname, bufp, PL_PSIZ-1);
	} else if (match_varname(buf, "PLALIAS", 3)) {
	    (void) strncpy(plalias, bufp, PL_NSIZ-1);
	} else if (match_varname(buf, "CATNAME", 3)) {
	    (void) strncpy(catname, bufp, PL_PSIZ-1);
	} else if (match_varname(buf, "RATNAME", 3)) {
	    (void) strncpy(ratname, bufp, PL_PSIZ-1);
	} else if (match_varname(buf, "WOLFNAME", 3)) {
            (void) strncpy(wolfname, bufp, PL_PSIZ-1);
	} else if (match_varname(buf, "GHOULNAME", 3)) {
            (void) strncpy(ghoulname, bufp, PL_PSIZ-1);
#if 0
	} else if (match_varname(buf, "BATNAME", 3)) {
            (void) strncpy(batname, bufp, PL_PSIZ-1);
	} else if (match_varname(buf, "SNAKENAME", 3)) {
            (void) strncpy(batname, bufp, PL_PSIZ-1);
	} else if (match_varname(buf, "RATNAME", 3)) {
            (void) strncpy(batname, bufp, PL_PSIZ-1);
	} else if (match_varname(buf, "BADGERNAME", 3)) {
            (void) strncpy(batname, bufp, PL_PSIZ-1);
	} else if (match_varname(buf, "REDDRAGONNAME", 3)) {
            (void) strncpy(batname, bufp, PL_PSIZ-1);
	} else if (match_varname(buf, "WHITEDRAGONNAME", 3)) {
            (void) strncpy(batname, bufp, PL_PSIZ-1);
#endif

	} else if (match_varname(buf, "BOULDER", 3)) {
	    (void) get_uchars(fp, buf, bufp, &iflags.bouldersym, TRUE,
			      1, "BOULDER");
	} else if (match_varname(buf, "MENUCOLOR", 9) || match_varname(buf, "MENUCOLOUR", 10)) {
#ifdef MENU_COLOR
	   add_menu_coloring(bufp);
#endif
	} else if (match_varname(buf, "GRAPHICS", 4)) {
	    len = get_uchars(fp, buf, bufp, translate, FALSE,
			     MAXPCHARS, "GRAPHICS");
	    assign_graphics(translate, len, MAXPCHARS, 0);
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	} else if (match_varname(buf, "STATUSCOLOR", 11) || match_varname(buf, "STATUSCOLOUR", 12)) {
	    (void) parse_status_color_options(bufp);
#endif
	} else if (match_varname(buf, "DUNGEON", 4)) {
	    len = get_uchars(fp, buf, bufp, translate, FALSE,
			     MAXDCHARS, "DUNGEON");
	    assign_graphics(translate, len, MAXDCHARS, 0);
	} else if (match_varname(buf, "TRAPS", 4)) {
	    len = get_uchars(fp, buf, bufp, translate, FALSE,
			     MAXTCHARS, "TRAPS");
	    assign_graphics(translate, len, MAXTCHARS, MAXDCHARS);
	} else if (match_varname(buf, "EFFECTS", 4)) {
	    len = get_uchars(fp, buf, bufp, translate, FALSE,
			     MAXECHARS, "EFFECTS");
	    assign_graphics(translate, len, MAXECHARS, MAXDCHARS+MAXTCHARS);

	} else if (match_varname(buf, "OBJECTS", 3)) {
	    /* oc_syms[0] is the RANDOM object, unused */
	    (void) get_uchars(fp, buf, bufp, &(oc_syms[1]), TRUE,
					MAXOCLASSES-1, "OBJECTS");
	} else if (match_varname(buf, "MONSTERS", 3)) {
	    /* monsyms[0] is unused */
	    (void) get_uchars(fp, buf, bufp, &(monsyms[1]), TRUE,
					MAXMCLASSES-1, "MONSTERS");
	} else if (match_varname(buf, "WARNINGS", 5)) {
	    (void) get_uchars(fp, buf, bufp, translate, TRUE,
					WARNCOUNT, "WARNINGS");
	    assign_warnings(translate);
#ifdef WIZARD
	} else if (match_varname(buf, "WIZKIT", 6)) {
	    (void) strncpy(wizkit, bufp, WIZKIT_MAX-1);
#endif
#ifdef AMIGA
	} else if (match_varname(buf, "FONT", 4)) {
		char *t;

		if( t = strchr( buf+5, ':' ) )
		{
		    *t = 0;
		    amii_set_text_font( buf+5, atoi( t + 1 ) );
		    *t = ':';
		}
	} else if (match_varname(buf, "PATH", 4)) {
		(void) strncpy(PATH, bufp, PATHLEN-1);
	} else if (match_varname(buf, "DEPTH", 5)) {
		extern int amii_numcolors;
		int val = atoi( bufp );
		amii_numcolors = 1L << min( DEPTH, val );
	} else if (match_varname(buf, "DRIPENS", 7)) {
		int i, val;
		char *t;
		for (i = 0, t = strtok(bufp, ",/"); t != (char *)0;
				i < 20 && (t = strtok((char*)0, ",/")), ++i) {
			sscanf(t, "%d", &val );
			flags.amii_dripens[i] = val;
		}
	} else if (match_varname(buf, "SCREENMODE", 10 )) {
		extern long amii_scrnmode;
		if (!stricmp(bufp,"req"))
		    amii_scrnmode = 0xffffffff; /* Requester */
		else if( sscanf(bufp, "%x", &amii_scrnmode) != 1 )
		    amii_scrnmode = 0;
	} else if (match_varname(buf, "MSGPENS", 7)) {
		extern int amii_msgAPen, amii_msgBPen;
		char *t = strtok(bufp, ",/");
		if( t )
		{
		    sscanf(t, "%d", &amii_msgAPen);
		    if( t = strtok((char*)0, ",/") )
				sscanf(t, "%d", &amii_msgBPen);
		}
	} else if (match_varname(buf, "TEXTPENS", 8)) {
		extern int amii_textAPen, amii_textBPen;
		char *t = strtok(bufp, ",/");
		if( t )
		{
		    sscanf(t, "%d", &amii_textAPen);
		    if( t = strtok((char*)0, ",/") )
				sscanf(t, "%d", &amii_textBPen);
		}
	} else if (match_varname(buf, "MENUPENS", 8)) {
		extern int amii_menuAPen, amii_menuBPen;
		char *t = strtok(bufp, ",/");
		if( t )
		{
		    sscanf(t, "%d", &amii_menuAPen);
		    if( t = strtok((char*)0, ",/") )
				sscanf(t, "%d", &amii_menuBPen);
		}
	} else if (match_varname(buf, "STATUSPENS", 10)) {
		extern int amii_statAPen, amii_statBPen;
		char *t = strtok(bufp, ",/");
		if( t )
		{
		    sscanf(t, "%d", &amii_statAPen);
		    if( t = strtok((char*)0, ",/") )
				sscanf(t, "%d", &amii_statBPen);
		}
	} else if (match_varname(buf, "OTHERPENS", 9)) {
		extern int amii_otherAPen, amii_otherBPen;
		char *t = strtok(bufp, ",/");
		if( t )
		{
		    sscanf(t, "%d", &amii_otherAPen);
		    if( t = strtok((char*)0, ",/") )
				sscanf(t, "%d", &amii_otherBPen);
		}
	} else if (match_varname(buf, "PENS", 4)) {
		extern unsigned short amii_init_map[ AMII_MAXCOLORS ];
		int i;
		char *t;

		for (i = 0, t = strtok(bufp, ",/");
			i < AMII_MAXCOLORS && t != (char *)0;
			t = strtok((char *)0, ",/"), ++i)
		{
			sscanf(t, "%hx", &amii_init_map[i]);
		}
		amii_setpens( amii_numcolors = i );
	} else if (match_varname(buf, "FGPENS", 6)) {
		extern int foreg[ AMII_MAXCOLORS ];
		int i;
		char *t;

		for (i = 0, t = strtok(bufp, ",/");
			i < AMII_MAXCOLORS && t != (char *)0;
			t = strtok((char *)0, ",/"), ++i)
		{
			sscanf(t, "%d", &foreg[i]);
		}
	} else if (match_varname(buf, "BGPENS", 6)) {
		extern int backg[ AMII_MAXCOLORS ];
		int i;
		char *t;

		for (i = 0, t = strtok(bufp, ",/");
			i < AMII_MAXCOLORS && t != (char *)0;
			t = strtok((char *)0, ",/"), ++i)
		{
			sscanf(t, "%d", &backg[i]);
		}
#endif
#ifdef USER_SOUNDS
	} else if (match_varname(buf, "SOUNDDIR", 8)) {
		sounddir = (char *)strdup(bufp);
	} else if (match_varname(buf, "SOUND", 5)) {
		add_sound_mapping(bufp);
#endif
#ifdef QT_GRAPHICS
	/* These should move to wc_ options */
	} else if (match_varname(buf, "QT_TILEWIDTH", 12)) {
		extern char *qt_tilewidth;
		if (qt_tilewidth == NULL)	
			qt_tilewidth=(char *)strdup(bufp);
	} else if (match_varname(buf, "QT_TILEHEIGHT", 13)) {
		extern char *qt_tileheight;
		if (qt_tileheight == NULL)	
			qt_tileheight=(char *)strdup(bufp);
	} else if (match_varname(buf, "QT_FONTSIZE", 11)) {
		extern char *qt_fontsize;
		if (qt_fontsize == NULL)
			qt_fontsize=(char *)strdup(bufp);
	} else if (match_varname(buf, "QT_COMPACT", 10)) {
		extern int qt_compact_mode;
		qt_compact_mode = atoi(bufp);
#endif
#if defined(GL_GRAPHICS) || defined(SDL_GRAPHICS)
	} else if (match_varname(buf, "GL_OPTIONS", 10)) {
		Sdlgl_parse_options(bufp, TRUE, TRUE);
#endif
	} else
		return 0;
	return 1;
}

#ifdef USER_SOUNDS
boolean
can_read_file(filename)
const char *filename;
{
	return (access(filename, 4) == 0);
}
#endif /* USER_SOUNDS */

void
read_config_file(filename)
const char *filename;
{
#define tmp_levels	(char *)0
#define tmp_ramdisk	(char *)0

#if defined(MICRO) || defined(WIN32)
#undef tmp_levels
	char	tmp_levels[PATHLEN];
# ifdef MFLOPPY
#  ifndef AMIGA
#undef tmp_ramdisk
	char	tmp_ramdisk[PATHLEN];
#  endif
# endif
#endif
	char	buf[4*BUFSZ];
	FILE	*fp;
	int     i;
#ifdef PROXY_GRAPHICS
	int	found = FALSE;

	if (!(fp = fopen_config_file(filename)))
	    goto clnt_process;
	else
	    found = TRUE;
#else
	if (!(fp = fopen_config_file(filename))) goto post_process;
#endif

#if defined(MICRO) || defined(WIN32)
# ifdef MFLOPPY
#  ifndef AMIGA
	tmp_ramdisk[0] = 0;
#  endif
# endif
	tmp_levels[0] = 0;
#endif
	/* begin detection of duplicate configfile options */
	set_duplicate_opt_detection(1);

	while (fgets(buf, 4*BUFSZ, fp)) {
		if (!parse_config_line(fp, buf, tmp_ramdisk, tmp_levels)) {
			raw_printf("Bad option line:  \"%.50s\"", buf);
			wait_synch();
		}
	}
	(void) fclose(fp);
	
	/* turn off detection of duplicate configfile options */
	set_duplicate_opt_detection(0);

#ifdef PROXY_GRAPHICS
clnt_process:
	/*
	 * When acting as a proxy server, allow the client to provide
	 * its own config file which overrides values in our config file.
	 * Note: We don't want to warn of values being present in both
	 * files, but we do want to warn of duplicates within each file.
	 */
	if (!strncmp(windowprocs.name, "proxy/", 6) &&
		(fp = proxy_config_file_open())) {
	    found = TRUE;
	    set_duplicate_opt_detection(1);
	    while (fgets(buf, 4*BUFSZ, fp)) {
		if (match_varname(buf, "TILESETS", 7) ||
		   match_varname(buf, "HACKDIR", 4) ||
		   match_varname(buf, "LEVELDIR", 4) ||
		   match_varname(buf, "LEVELS", 4) ||
		   match_varname(buf, "SAVEDIR", 4) ||
		   match_varname(buf, "BONESDIR", 5) ||
		   match_varname(buf, "DATADIR", 4) ||
		   match_varname(buf, "SCOREDIR", 4) ||
		   match_varname(buf, "LOCKDIR", 4) ||
		   match_varname(buf, "CONFIGDIR", 4) ||
		   match_varname(buf, "TROUBLEDIR", 4) ||
		   match_varname(buf, "SOUNDDIR", 8) ||
		   match_varname(buf, "SOUND", 5)) {
		    /* Quietly ignore many commands. There's no sense in
		     * the client configuring these and some introduce
		     * potential security breachs.
		     */
		    continue;
		}
		if (!parse_config_line(fp, buf, tmp_ramdisk, tmp_levels)) {
		    pline("Bad option line:  \"%.50s\"", buf);
		    wait_synch();
		}
	    }
	    proxy_config_file_close(fp);
	    set_duplicate_opt_detection(0);
	}

	if (!found) goto post_process;
#endif

#if defined(MICRO) && !defined(NOCWD_ASSUMPTIONS)
	/* should be superseded by fqn_prefix[] */
# ifdef MFLOPPY
	strcpy(permbones, tmp_levels);
#  ifndef AMIGA
	if (tmp_ramdisk[0]) {
		strcpy(levels, tmp_ramdisk);
		if (strcmp(permbones, levels))		/* if not identical */
			ramdisk = TRUE;
	} else
#  endif /* AMIGA */
		strcpy(levels, tmp_levels);

	strcpy(bones, levels);
# endif /* MFLOPPY */
#endif /* MICRO */
post_process:
	if (!no_tilesets) {
		for(i = 0; strlen(def_tilesets[i].name); i++) {
			strcpy(tilesets[i].name, def_tilesets[i].name);
			strcpy(tilesets[i].file, def_tilesets[i].file);
			tilesets[i].flags = def_tilesets[i].flags;
		}
		no_tilesets = i;
	}
	if (tileset[0] != '\0') {
		unsigned int len = strlen(tileset);
		for(i = 0; i < no_tilesets; i++)
			if (len == strlen(tilesets[i].name) &&
			    !strncmpi(tilesets[i].name, tileset, len))
				break;
		if (i == no_tilesets) {
			pline("Tileset %s not defined.", tileset);
			tileset[0] = '\0';
		}
		else
			strcpy(tileset, tilesets[i].name);
	}
	return;
}

#ifdef WIZARD
STATIC_OVL FILE *
fopen_wizkit_file()
{
	FILE *fp;
#if defined(VMS) || defined(UNIX)
	char	tmp_wizkit[BUFSZ];
#endif
	char *envp;

	envp = nh_getenv("WIZKIT");
	if (envp && *envp) (void) strncpy(wizkit, envp, WIZKIT_MAX - 1);
	if (!wizkit[0]) return (FILE *)0;

#ifdef UNIX
	if (access(wizkit, 4) == -1) {
		/* 4 is R_OK on newer systems */
		/* nasty sneaky attempt to read file through
		 * NetHack's setuid permissions -- this is a
		 * place a file name may be wholly under the player's
		 * control
		 */
		raw_printf("Access to %s denied (%d).",
				wizkit, errno);
		wait_synch();
		/* fall through to standard names */
	} else
#endif
	if ((fp = fopenp(wizkit, "r")) != (FILE *)0) {
	    return(fp);
#if defined(UNIX) || defined(VMS)
	} else {
	    /* access() above probably caught most problems for UNIX */
	    raw_printf("Couldn't open requested config file %s (%d).",
				wizkit, errno);
	    wait_synch();
#endif
	}

#if defined(MICRO) || defined(MAC) || defined(__BEOS__) || defined(WIN32)
	if ((fp = fopenp(fqname(wizkit, CONFIGPREFIX, 0), "r"))
								!= (FILE *)0)
		return(fp);
#else
# ifdef VMS
	envp = nh_getenv("HOME");
	if (envp)
		sprintf(tmp_wizkit, "%s%s", envp, wizkit);
	else
		sprintf(tmp_wizkit, "%s%s", "sys$login:", wizkit);
	if ((fp = fopenp(tmp_wizkit, "r")) != (FILE *)0)
		return(fp);
# else	/* should be only UNIX left */
	envp = nh_getenv("HOME");
	if (envp)
		sprintf(tmp_wizkit, "%s/%s", envp, wizkit);
	else 	strcpy(tmp_wizkit, wizkit);
	if ((fp = fopenp(tmp_wizkit, "r")) != (FILE *)0)
		return(fp);
	else if (errno != ENOENT) {
		/* e.g., problems when setuid NetHack can't search home
		 * directory restricted to user */
		raw_printf("Couldn't open default wizkit file %s (%d).",
					tmp_wizkit, errno);
		wait_synch();
	}
# endif
#endif
	return (FILE *)0;
}

void
read_wizkit()
{
	FILE *fp;
	char *ep, buf[BUFSZ];
	struct obj *otmp;
	boolean bad_items = FALSE, skip = FALSE;

	if (!wizard || !(fp = fopen_wizkit_file())) return;

	while (fgets(buf, (int)(sizeof buf), fp)) {
	    ep = index(buf, '\n');
	    if (skip) {	/* in case previous line was too long */
		if (ep) skip = FALSE; /* found newline; next line is normal */
	    } else {
		if (!ep) skip = TRUE; /* newline missing; discard next fgets */
		else *ep = '\0';		/* remove newline */

		if (buf[0]) {
			otmp = readobjnam(buf, (struct obj *)0, FALSE, TRUE);
			if (otmp) {
			    if (otmp != &zeroobj)
				otmp = addinv(otmp);
			} else {
			    /* .60 limits output line width to 79 chars */
			    raw_printf("Bad wizkit item: \"%.60s\"", buf);
			    bad_items = TRUE;
			}
		}
	    }
	}
	if (bad_items)
	    wait_synch();
	(void) fclose(fp);
	return;
}

#endif /*WIZARD*/

/* ----------  END CONFIG FILE HANDLING ----------- */

/* ----------  BEGIN SCOREBOARD CREATION ----------- */

/* verify that we can write to the scoreboard file; if not, try to create one */
void
check_recordfile(dir)
const char *dir;
{
#if defined(MAC_MPW) || defined(__MWERKS__)
# pragma unused(dir)
#endif
	int fd;
#ifndef FILE_AREAS
	const char *fq_record;
#endif

#if defined(UNIX) || defined(VMS)
# ifdef FILE_AREAS
	fd = open_area(NH_RECORD_AREA, NH_RECORD, O_RDWR, 0);
# else
	fq_record = fqname(NH_RECORD, SCOREPREFIX, 0);
	fd = open(fq_record, O_RDWR, 0);
# endif

	if (fd >= 0) {
# ifdef VMS	/* must be stream-lf to use UPDATE_RECORD_IN_PLACE */
		if (!file_is_stmlf(fd)) {
		    raw_printf(
		  "Warning: scoreboard file %s is not in stream_lf format",
				fq_record);
		    wait_synch();
		}
# endif
	    (void) close(fd);   /* NH_RECORD is accessible */
#  ifdef FILE_AREAS
	} else if ((fd = open_area(NH_RECORD_AREA, NH_RECORD, O_CREAT|O_RDWR, FCMASK)) >= 0) {
#  else
	} else if ((fd = open(fq_record, O_CREAT|O_RDWR, FCMASK)) >= 0) {
#  endif
	    (void) close(fd);   /* NH_RECORD newly created */
# if defined(VMS) && !defined(SECURE)
	    /* Re-protect NH_RECORD with world:read+write+execute+delete access. */
#  ifdef FILE_AREAS
	    (void) chmod_area(NH_RECORD_AREA, NH_RECORD, FCMASK | 007);
#  else
	    (void) chmod(fq_record, FCMASK | 007);
#  endif
# endif /* VMS && !SECURE */
	} else {
# ifdef FILE_AREAS
	    raw_printf("Warning: cannot write scoreboard file %s", NH_RECORD);
# else
	    raw_printf("Warning: cannot write scoreboard file %s", fq_record);
# endif
	    wait_synch();
	}
#endif  /* !UNIX && !VMS */
#if defined(MICRO) || defined(WIN32)
	char tmp[PATHLEN];

# ifdef OS2_CODEVIEW   /* explicit path on opening for OS/2 */
	/* how does this work when there isn't an explicit path or fopenp
	 * for later access to the file via fopen_datafile? ? */
	(void) strncpy(tmp, dir, PATHLEN - 1);
	tmp[PATHLEN-1] = '\0';
	if ((strlen(tmp) + 1 + strlen(NH_RECORD)) < (PATHLEN - 1)) {
		append_slash(tmp);
		strcat(tmp, NH_RECORD);
	}
#  ifndef FILE_AREAS
	fq_record = tmp;
#  endif
# else
	strcpy(tmp, NH_RECORD);
#  ifndef FILE_AREAS
	fq_record = fqname(NH_RECORD, SCOREPREFIX, 0);
#  endif
# endif

# ifdef FILE_AREAS
	if ((fd = open_area(NH_RECORD_AREA, tmp, O_RDWR)) < 0) {
# else
	if ((fd = open(fq_record, O_RDWR)) < 0) {
# endif
	    /* try to create empty record */
# if defined(FILE_AREAS)
	    if ((fd = open_area(NH_RECORD_AREA, tmp, O_CREAT|O_RDWR,
	      S_IREAD|S_IWRITE)) < 0) {
# elif defined(AZTEC_C) || defined(_DCC) || (defined(__GNUC__) && defined(__AMIGA__))
	    /* Aztec doesn't use the third argument */
	    /* DICE doesn't like it */
	    if ((fd = open(fq_record, O_CREAT|O_RDWR)) < 0) {
# else
	    if ((fd = open(fq_record, O_CREAT|O_RDWR, S_IREAD|S_IWRITE)) < 0) {
# endif
	raw_printf("Warning: cannot write record %s", tmp);
		wait_synch();
		} else          /* create succeeded */
		(void) close(fd);
	} else		/* open succeeded */
	    (void) close(fd);
#else /* MICRO || WIN32*/

# ifdef MAC
	/* Create the record file, if necessary */
	fq_record = fqname(NH_RECORD, SCOREPREFIX, 0);
	fd = macopen (fq_record, O_RDWR | O_CREAT, LOGF_TYPE);
	if (fd != -1) macclose (fd);

	/* Create the logfile, if necessary */
	fq_record = fqname(LOGFILE, SCOREPREFIX, 0);
	fd = macopen (fq_record, O_RDWR | O_CREAT, LOGF_TYPE);
	if (fd != -1) macclose (fd);
# endif /* MAC */

#endif /* MICRO || WIN32*/
}

/* ----------  END SCOREBOARD CREATION ----------- */

/* ----------  BEGIN PANIC/IMPOSSIBLE LOG ----------- */

/*ARGSUSED*/
void
paniclog(type, reason)
const char *type;	/* panic, impossible, trickery */
const char *reason;	/* explanation */
{
#ifdef PANICLOG
	FILE *lfile;
	char buf[BUFSZ];

	if (!program_state.in_paniclog) {
		program_state.in_paniclog = 1;
		lfile = fopen_datafile_area(LOGAREA, PANICLOG, "a", TROUBLEPREFIX);
		if (lfile) {
		    (void) fprintf(lfile, "%s %08ld: %s %s\n",
				   version_string(buf), yyyymmdd((time_t)0L),
				   type, reason);
		    (void) fclose(lfile);
		}
		program_state.in_paniclog = 0;
	}
#endif /* PANICLOG */
	return;
}

/* ----------  END PANIC/IMPOSSIBLE LOG ----------- */

#ifdef SELF_RECOVER

/* ----------  BEGIN INTERNAL RECOVER ----------- */
boolean
recover_savefile()
{
	int gfd, lfd, sfd;
	int lev, savelev, hpid;
	xchar levc;
	struct version_info version_data;
	int processed[256];
	char savename[SAVESIZE], errbuf[BUFSZ];

	for (lev = 0; lev < 256; lev++)
		processed[lev] = 0;

	/* level 0 file contains:
	 *	pid of creating process (ignored here)
	 *	level number for current level of save file
	 *	name of save file nethack would have created
	 *	and game state
	 */
	gfd = open_levelfile(0, errbuf);
	if (gfd < 0) {
	    raw_printf("%s\n", errbuf);
	    return FALSE;
	}
	if (read(gfd, (void *) &hpid, sizeof hpid) != sizeof hpid) {
	    raw_printf(
"\nCheckpoint data incompletely written or subsequently clobbered. Recovery impossible.");
	    (void)close(gfd);
	    return FALSE;
	}
	if (read(gfd, (void *) &savelev, sizeof(savelev))
							!= sizeof(savelev)) {
	    raw_printf("\nCheckpointing was not in effect for %s -- recovery impossible.\n",
			lock);
	    (void)close(gfd);
	    return FALSE;
	}
	if ((read(gfd, (void *) savename, sizeof savename)
		!= sizeof savename) ||
	    (read(gfd, (void *) &version_data, sizeof version_data)
		!= sizeof version_data)) {
	    raw_printf("\nError reading %s -- can't recover.\n", lock);
	    (void)close(gfd);
	    return FALSE;
	}

	/* save file should contain:
	 *	version info
	 *	current level (including pets)
	 *	(non-level-based) game state
	 *	other levels
	 */
	set_savefile_name();
	sfd = create_savefile();
	if (sfd < 0) {
	    raw_printf("\nCannot recover savefile %s.\n", SAVEF);
	    (void)close(gfd);
	    return FALSE;
	}

	lfd = open_levelfile(savelev, errbuf);
	if (lfd < 0) {
	    raw_printf("\n%s\n", errbuf);
	    (void)close(gfd);
	    (void)close(sfd);
	    delete_savefile();
	    return FALSE;
	}

	if (write(sfd, (void *) &version_data, sizeof version_data)
		!= sizeof version_data) {
	    raw_printf("\nError writing %s; recovery failed.", SAVEF);
	    (void)close(gfd);
	    (void)close(sfd);
	    delete_savefile();
	    return FALSE;
	}

	if (!copy_bytes(lfd, sfd)) {
		(void) close(lfd);
		(void) close(sfd);
		delete_savefile();
		return FALSE;
	}
	(void)close(lfd);
	processed[savelev] = 1;

	if (!copy_bytes(gfd, sfd)) {
		(void) close(lfd);
		(void) close(sfd);
		delete_savefile();
		return FALSE;
	}
	(void)close(gfd);
	processed[0] = 1;

	for (lev = 1; lev < 256; lev++) {
		/* level numbers are kept in xchars in save.c, so the
		 * maximum level number (for the endlevel) must be < 256
		 */
		if (lev != savelev) {
			lfd = open_levelfile(lev, (char *)0);
			if (lfd >= 0) {
				/* any or all of these may not exist */
				levc = (xchar) lev;
				write(sfd, (void *) &levc, sizeof(levc));
				if (!copy_bytes(lfd, sfd)) {
					(void) close(lfd);
					(void) close(sfd);
					delete_savefile();
					return FALSE;
				}
				(void)close(lfd);
				processed[lev] = 1;
			}
		}
	}
	(void)close(sfd);

#ifdef HOLD_LOCKFILE_OPEN
	really_close();
#endif
	/*
	 * We have a successful savefile!
	 * Only now do we erase the level files.
	 */
	for (lev = 0; lev < 256; lev++) {
		if (processed[lev]) {
			const char *fq_lock;
			set_levelfile_name(lock, lev);
			fq_lock = fqname(lock, LEVELPREFIX, 3);
			(void) unlink(fq_lock);
		}
	}
	return TRUE;
}

boolean
copy_bytes(ifd, ofd)
int ifd, ofd;
{
	char buf[BUFSIZ];
	int nfrom, nto;

	do {
		nfrom = read(ifd, buf, BUFSIZ);
		nto = write(ofd, buf, nfrom);
		if (nto != nfrom) return FALSE;
	} while (nfrom == BUFSIZ);
	return TRUE;
}

/* ----------  END INTERNAL RECOVER ----------- */
#endif /*SELF_RECOVER*/

/*files.c*/
