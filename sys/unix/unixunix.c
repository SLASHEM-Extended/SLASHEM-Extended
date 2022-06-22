/*	SCCS Id: @(#)unixunix.c	3.4	1994/11/07	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/* This file collects some Unix dependencies */

#include "hack.h"	/* mainly for index() which depends on BSD */

#include <errno.h>
#include <sys/stat.h>
#include <limits.h>
#if defined(NO_FILE_LINKS) || defined(SUNOS4) || defined(POSIX_TYPES)
#include <fcntl.h>
#endif
#include <signal.h>

#ifdef _M_UNIX
extern void sco_mapon(void);
extern void sco_mapoff(void);
#endif
#ifdef __linux__
extern void linux_mapon(void);
extern void linux_mapoff(void);
#endif

#ifdef LINUX
extern void linux_mapon(void);
extern void linux_mapoff(void);
#endif

static int veryold(int);
static int eraseoldlocks(void);

#ifndef NHSTDC
extern int errno;
#endif

extern int restore_savefile(char *, const char *);

static struct stat buf;

/* see whether we should throw away this xlock file */
static int
veryold(fd)
int fd;
{
	time_t date;

	if(fstat(fd, &buf)) return(0);			/* cannot get status */
#ifndef INSURANCE
	if(buf.st_size != sizeof(int)) return(0);	/* not an xlock file */
#endif
#if defined(BSD) && !defined(POSIX_TYPES)
	(void) time((long *)(&date));
#else
	(void) time(&date);
#endif
	if(date - buf.st_mtime < 3L*24L*60L*60L) {	/* recent */
		int lockedpid;	/* should be the same size as hackpid */

		if(read(fd, (void *)&lockedpid, sizeof(lockedpid)) !=
			sizeof(lockedpid))
			/* strange ... */
			return(0);

		/* From: Rick Adams <seismo!rick> */
		/* This will work on 4.1cbsd, 4.2bsd and system 3? & 5. */
		/* It will do nothing on V7 or 4.1bsd. */
#ifndef NETWORK
		/* It will do a VERY BAD THING if the playground is shared
		   by more than one machine! -pem */
		if(!(kill(lockedpid, 0) == -1 && errno == ESRCH))
#endif
			return(0);
	}
	(void) close(fd);
	return(1);
}

static int
eraseoldlocks()
{
	register int i;
	program_state.preserve_locks = 0; /* not required but shows intent */

	/* cannot use maxledgerno() here, because we need to find a lock name
	 * before starting everything (including the dungeon initialization
	 * that sets astral_level, needed for maxledgerno()) up
	 */
	for(i = 1; i <= MAXDUNGEON*MAXLEVEL + 1; i++) {
		/* try to remove all */
		set_levelfile_name(lock, i);
#ifdef FILE_AREAS
		(void) remove_area(FILE_AREA_LEVL, lock);
#else
		(void) unlink(fqname(lock, LEVELPREFIX, 0));
#endif
	}
	set_levelfile_name(lock, 0);
#ifdef FILE_AREAS
	if (remove_area(FILE_AREA_LEVL, lock))
#else
	if (unlink(fqname(lock, LEVELPREFIX, 0)))
#endif
		return(0);				/* cannot remove it */
	return(1);					/* success! */
}

void
getlock()
{
	register int i = 0, fd, c;
#ifndef FILE_AREAS
	const char *fq_lock;
#endif

#ifdef TTY_GRAPHICS
	/* idea from rpick%ucqais@uccba.uc.edu
	 * prevent automated rerolling of characters
	 * test input (fd0) so that tee'ing output to get a screen dump still
	 * works
	 * also incidentally prevents development of any hack-o-matic programs
	 */
	/* added check for window-system type -dlc */
	if (!strcmp(windowprocs.name, "tty"))
	    if (!isatty(0))
		error("You must play from a terminal.");
#endif

	/* we ignore QUIT and INT at this point */
#ifndef FILE_AREAS
	if (!lock_file(HLOCK, LOCKPREFIX, 10)) {
#else
	if (!lock_file_area(HLOCK_AREA, HLOCK, 10)) {
#endif
		wait_synch();
		error("%s", "");
	}

	regularize(lock);
	set_levelfile_name(lock, 0);

	if(locknum) {
		if(locknum > 25) locknum = 25;

		do {
			lock[0] = 'a' + i++;

#ifndef FILE_AREAS
			fq_lock = fqname(lock, LEVELPREFIX, 0);
			if((fd = open(fq_lock, 0, 0)) == -1) {
#else
			if((fd = open_area(FILE_AREA_LEVL, lock, 0, 0)) == -1) {
#endif
			    if(errno == ENOENT) goto gotlock; /* no such file */
#ifndef FILE_AREAS
			    perror(fq_lock);
			    unlock_file(HLOCK);
			    error("Cannot open %s", fq_lock);
#else
			    perror(lock);
			    unlock_file_area(HLOCK_AREA, HLOCK);
			    error("Cannot open %s", lock);
#endif
			}

			if(veryold(fd) /* closes fd if true */
							&& eraseoldlocks())
				goto gotlock;
			(void) close(fd);
		} while(i < locknum);

		unlock_file_area(HLOCK_AREA, HLOCK);
		error("Too many hacks running now.");
	} else {
#ifndef FILE_AREAS
		fq_lock = fqname(lock, LEVELPREFIX, 0);
		if((fd = open(fq_lock, 0, 0)) == -1) {
#else
		if((fd = open_area(FILE_AREA_LEVL, lock, 0, 0)) == -1) {
#endif
			if(errno == ENOENT) goto gotlock;    /* no such file */
#ifndef FILE_AREAS
			perror(fq_lock);
			unlock_file(HLOCK);
			error("Cannot open %s", fq_lock);
#else
			perror(lock);
			unlock_file_area(HLOCK_AREA, HLOCK);
			error("Cannot open %s", lock);
#endif
		}

		if(veryold(fd) /* closes fd if true */ && eraseoldlocks())
			goto gotlock;
		(void) close(fd);

		if(iflags.window_inited) {
		    c = yn_function("There is already a game in progress under your name.  Destroy old game [y], Recover it [r], Cancel [n] ?", "ynr", 'n');
		} else {
		    (void) printf("\nThere is already a game in progress under your name. Do what?\n");
		    (void) printf("\n  y - Destroy old game?");
		    (void) printf("\n  r - Try to recover it?");
		    (void) printf("\n  n - Cancel");
		    (void) printf("\n\n  => ");
		    (void) fflush(stdout);
		    do {
			c = getchar();
		    } while (!index("rRyYnN", c) && c != -1);
		    (void) printf("\e[7A"); /* cursor up 7 */
		    (void) printf("\e[J"); /* clear from cursor down */
		}
		if (c == 'r' || c == 'R') {
#ifdef FILE_AREAS
		    if (restore_savefile(lock, FILE_AREA_LEVL) == 0) {
#else
		    if (restore_savefile(lock, fqn_prefix[SAVEPREFIX]) == 0) {
#endif
			const char *msg = "Automatical recovery of save file successful! "
			    "Press any key to continue ...\n";
			fflush(stdout);
			if (iflags.window_inited) {
			    pline("%s", msg);
			} else {
			    printf("\n\n%s", msg);
			    fflush(stdout);
			    c = getchar();
			}
			goto gotlock;
		    } else {
			unlock_file_area(HLOCK_AREA, HLOCK);

			if (iflags.window_inited) {
			    pline("Recovery failed. In some cases this problem can disappear if you wait a few minutes and try again, but more often than not this error needs manual intervention. In the case of doubt, please contact the server administrator.");
			} else {
			    (void) printf("\nRecovery failed.  In some cases this problem can disappear if you wait");
			    (void) printf("\na few minutes and try again, but often this error needs manual intervention.");
			    (void) printf("\nIn the case of doubt, please contact the server administrator.");
			    (void) fflush(stdout);
			    c = getchar();
			}

			error("Recovery failed. Terminating now. Please contact the server admin if the problem persists.");
		    }
		} else if (c == 'y' || c == 'Y') {

			if(iflags.window_inited) {
				c = yn_function("ARE YOU SURE??? Confirm with 'j' that you really want to delete your game!", "jn", 'n');
			} else {
			    (void) printf("\nARE YOU SURE??? Hit 'j' to confirm that you really want to delete your game!\n");
			    (void) printf("\n  j - Really delete your game (THIS IS PERMANENT)");
			    (void) printf("\n  n - Cancel");
			    (void) printf("\n  ");
			    (void) printf("\n\n  => ");
			    (void) fflush(stdout);
			    do {
				c = getchar();
			    } while (!index("jJnN", c) && c != -1);
			    (void) printf("\e[7A"); /* cursor up 7 */
			    (void) printf("\e[J"); /* clear from cursor down */

			}
			if (c == 'j' || c == 'J') {

				/* another layer of security because j and n are very close to each other --Amy
				 * (no, y and n aren't close to each other, y is of course to the left of x, i.e. far away :P) */
				if(iflags.window_inited) {
					c = yn_function("ARE YOU *REALLY* SURE??? Press 'p' to delete your game", "np", 'n');
				} else {
				    (void) printf("\nARE YOU *REALLY* SURE??? Press 'p' to delete your game\n");
				    (void) printf("\n  p - REALLY delete your game (THIS IS PERMANENT)");
				    (void) printf("\n  n - Cancel");
				    (void) printf("\n  ");
				    (void) printf("\n\n  => ");
				    (void) fflush(stdout);
				    do {
					c = getchar();
				    } while (!index("jJnN", c) && c != -1);
				    (void) printf("\e[7A"); /* cursor up 7 */
				    (void) printf("\e[J"); /* clear from cursor down */
				}

				if (c == 'p' || c == 'P') {

					if(eraseoldlocks())
						goto gotlock;
					else {
						unlock_file_area(HLOCK_AREA, HLOCK);
						error("Couldn't destroy old game.");
					}
				}
			} else {
			unlock_file_area(HLOCK_AREA, HLOCK);
			error("%s", "");
			}
		} else {
			unlock_file_area(HLOCK_AREA, HLOCK);
			error("%s", "");
		}
	}

gotlock:
#ifndef FILE_AREAS
	fd = creat(fq_lock, FCMASK);
#else
	fd = creat_area(FILE_AREA_LEVL, lock, FCMASK);
#endif
	unlock_file_area(HLOCK_AREA, HLOCK);
	if(fd == -1) {
#ifndef FILE_AREAS
		error("cannot creat lock file (%s).", fq_lock);
#else
		error("cannot creat lock file (%s in %s).", lock,
		  FILE_AREA_LEVL);
#endif
	} else {
		if(write(fd, (void *) &hackpid, sizeof(hackpid))
		    != sizeof(hackpid)){
#ifndef FILE_AREAS
			error("cannot write lock (%s)", fq_lock);
#else
			error("cannot write lock (%s in %s)", lock,
			  FILE_AREA_LEVL);
#endif
		}
		if(close(fd) == -1) {
#ifndef FILE_AREAS
			error("cannot close lock (%s)", fq_lock);
#else
			error("cannot close lock (%s in %s)", lock,
			  FILE_AREA_LEVL);
#endif
		}
	}
}

void
regularize(s)	/* normalize file name - we don't like .'s, /'s, spaces */
register char *s;
{
	register char *lp;

	while((lp=index(s, '.')) || (lp=index(s, '/')) || (lp=index(s,' ')))
		*lp = '_';
#if defined(SYSV) && !defined(AIX_31) && !defined(SVR4) && !defined(LINUX) && !defined(__APPLE__)
	/* avoid problems with 14 character file name limit */
# ifdef COMPRESS
	/* leave room for .e from error and .Z from compress appended to
	 * save files */
	{
#  ifdef COMPRESS_EXTENSION
	    int i = 12 - strlen(COMPRESS_EXTENSION);
#  else
	    int i = 10;		/* should never happen... */
#  endif
	    if(strlen(s) > i)
		s[i] = '\0';
	}
# else
	if(strlen(s) > 11)
		/* leave room for .nn appended to level files */
		s[11] = '\0';
# endif
#endif
}

#if defined(TIMED_DELAY) && !defined(msleep) && defined(SYSV)
#include <poll.h>

void
msleep(msec)
unsigned msec;				/* milliseconds */
{
	struct pollfd unused;
	int msecs = msec;		/* poll API is signed */

	if (msecs < 0) msecs = 0;	/* avoid infinite sleep */
	(void) poll(&unused, (unsigned long)0, msecs);
}
#endif /* TIMED_DELAY for SYSV */

#ifndef PUBLIC_SERVER
int
dosh()
{
	register char *str;
	if(child(0)) {
		if((str = getenv("SHELL")) != (char*)0)
			(void) execl(str, str, (char *)0);
		else
			(void) execl("/bin/sh", "sh", (char *)0);
		raw_print("sh: cannot execute.");
		exit(EXIT_FAILURE);
	}
	return 0;
}
#endif /* PUBLIC_SERVER */

#if defined(SHELL) || defined(DEF_PAGER) || defined(DEF_MAILREADER)
int
child(wt)
int wt;
{
	register int f;
	suspend_nhwindows((char *)0);	/* also calls end_screen() */
#ifdef _M_UNIX
	sco_mapon();
#endif
#ifdef LINUX
	linux_mapon();
#endif
#ifdef __linux__
	linux_mapon();
#endif
	if((f = fork()) == 0){		/* child */
		(void) setgid(getgid());
		(void) setuid(getuid());
#ifdef CHDIR
		(void) chdir(getenv("HOME"));
#endif
		return(1);
	}
	if(f == -1) {	/* cannot fork */
		pline("Fork failed.  Try again.");
		return(0);
	}
	/* fork succeeded; wait for child to exit */
	(void) signal(SIGINT,SIG_IGN);
	(void) signal(SIGQUIT,SIG_IGN);
	(void) wait( (int *) 0);
#ifdef _M_UNIX
	sco_mapoff();
#endif
#ifdef __linux__
	linux_mapoff();
#endif
#ifdef LINUX
	linux_mapoff();
#endif
	(void) signal(SIGINT, (SIG_RET_TYPE) done1);
#ifdef WIZARD
	if(wizard) (void) signal(SIGQUIT,SIG_DFL);
#endif
	if(wt) {
		raw_print("");
		wait_synch();
	}
	resume_nhwindows();
	return(0);
}
#endif

#ifdef FILE_AREAS

/*
 * Unix file areas are directories with trailing slashes.
 */

char *make_file_name(filearea, filename)
const char *filearea, *filename;
{
	char *buf;
	int lenarea;
	if (filearea && filename[0]!='/')
	{
		lenarea = strlen(filearea);
		buf = (char *)alloc(lenarea+strlen(filename)+1);
		strcpy(buf, filearea);
		strcpy(buf+lenarea, filename);
	}
	else
	{
		buf = (char *)alloc(strlen(filename)+1);
		strcpy(buf, filename);
	}
	return buf;
}

FILE *
fopen_datafile_area(filearea, filename, mode, use_scoreprefix)
const char *filearea, *filename, *mode;
boolean use_scoreprefix;
{
	FILE *fp;
	char *buf;
	buf = make_file_name(filearea, filename);
	fp = fopen(buf, mode);
	return fp;
}

int
chmod_area(filearea, filename, mode)
const char *filearea, *filename;
int mode;
{
	int retval;
	char *buf;
	buf = make_file_name(filearea, filename);
	retval = chmod(buf, mode);
	free(buf);
	return retval;
}

int
open_area(filearea, filename, flags, mode)
const char *filearea, *filename;
int flags, mode;
{
	int fd;
	char *buf;
	buf = make_file_name(filearea, filename);
	fd = open(buf, flags, mode);
	free(buf);
	return fd;
}

int
creat_area(filearea, filename, mode)
const char *filearea, *filename;
int mode;
{
	int fd;
	char *buf;
	buf = make_file_name(filearea, filename);
	fd = creat(buf, mode);
	free(buf);
	return fd;
}

int
rename_area(filearea, oldfilename, newfilename)
const char *filearea, *oldfilename, *newfilename;
{
	int retval;
	char *oldpath,*newpath;
	int lenarea;
	oldpath = make_file_name(filearea, oldfilename);
	newpath = make_file_name(filearea, newfilename);
	retval = rename(oldpath, newpath);
	free(oldpath);
	free(newpath);
	return retval;
}

int
remove_area(filearea, filename)
const char *filearea, *filename;
{
	int retval;
	char *buf;
	buf = make_file_name(filearea, filename);
	retval = remove(buf);
	free(buf);
	return retval;
}

FILE *
freopen_area(filearea, filename, mode, stream)
const char *filearea, *filename, *mode;
FILE *stream;
{
	FILE *fp;
	char *buf;
	buf = make_file_name(filearea, filename);
	fp = freopen(buf, mode, stream);
	free(buf);
	return fp;
}

/* ----------  BEGIN FILE LOCKING HANDLING ----------- */

/*
 * ALI
 *
 * We assume that filenames are unique and so locks
 * don't need to take the filearea into account. Since
 * these locks are only used for RECORD, LOGFILE and HLOCK,
 * this assumption is currently valid.
 */

static int nesting = 0;

static int lockfd=-1;   /* for lock_file_area() to pass to unlock_file_area() */

#define HUP     if (!program_state.done_hup)

static char *
make_lockname(filename, lockname)
const char *filename;
char *lockname;
{
# ifdef NO_FILE_LINKS
	strcpy(lockname, LOCKDIR);
	if (LOCKDIR[sizeof(LOCKDIR)-2]!='/')
		strcat(lockname, "/");
	strcat(lockname, filename);
# else
# ifdef FILE_AREAS
	strcpy(lockname, FILE_AREA_VAR);
	strcat(lockname, filename);
# else
	strcpy(lockname, filename);
# endif
# endif	/* NO_FILE_LINKS */
	strcat(lockname, "_lock");
	return lockname;
}

static boolean
assert_lock(filename, lockname)
const char *filename, *lockname;
{
# ifdef NO_FILE_LINKS
    return (lockfd = open(lockname, O_RDWR|O_CREAT|O_EXCL, 0666)) != -1;
# else
    if (link(filename, lockname) == -1)
	if (errno == EXDEV)
	    return (lockfd = open(lockname, O_RDWR|O_CREAT|O_EXCL, 0666)) != -1;
	else
	    return FALSE;
    else
    {
	lockfd=-1;
	return TRUE;
    }
# endif
}

/*
 * lock a file
 *
 * Note: The area says where the file is, not where the lock is.
 */

boolean
lock_file_area(filearea, filename, retryct)
const char *filearea, *filename;
int retryct;
{
	int retval=TRUE;
	char *lockname, locknambuf[BUFSZ];
	char *buf;

	nesting++;
	if (nesting > 1) {
	    impossible("TRIED TO NEST LOCKS");
	    return TRUE;
	}

	buf = make_file_name(filearea, filename);
	lockname = make_lockname(filename, locknambuf);

	while (retval && !assert_lock(buf, lockname)) {
	    register int errnosv = errno;

	    switch (errnosv) {  /* George Barbanis */
	    case EEXIST:
		if (retryct--) {
		    HUP raw_printf(
			    "Waiting for access to %s.  (%d retries left).",
			    filename, retryct);
# if defined(SYSV) || defined(ULTRIX)
		    (void)
# endif
			sleep(1);
		} else {
		    HUP (void) raw_print("I give up.  Sorry.");
		    HUP raw_printf("Perhaps there is an old %s around?",
					lockname);
		    nesting--;
		    retval = FALSE;
		}

		break;
	    case ENOENT:
		HUP raw_printf("Can't find file %s to lock!", filename);
		nesting--;
		retval = FALSE;
	    case EACCES:
		HUP raw_printf("No write permission to lock %s!", filename);
		nesting--;
		retval = FALSE;
	    default:
		HUP perror(lockname);
		HUP raw_printf(
			     "Cannot lock %s for unknown reason (%d).",
			       filename, errnosv);
		nesting--;
		retval = FALSE;
	    }

	}

	free(buf);
	return retval;
}

/* unlock file, which must be currently locked by lock_file_area */
void
unlock_file_area(filearea, filename)
const char *filearea, *filename;
{
	char *lockname, locknambuf[BUFSZ];

	if (nesting == 1) {
		lockname = make_lockname(filename, locknambuf);

		if (unlink(lockname) < 0)
			HUP raw_printf("Can't unlink %s.", lockname);
		if (lockfd!=-1)
		{
			(void) close(lockfd);
			lockfd=-1;
		}
	}

	nesting--;
}

/* ----------  END FILE LOCKING HANDLING ----------- */

#endif	/* FILE_AREAS */

#ifdef GETRES_SUPPORT

extern int nh_getresuid(uid_t *, uid_t *, uid_t *);
extern uid_t nh_getuid(void);
extern uid_t nh_geteuid(void);
extern int nh_getresgid(gid_t *, gid_t *, gid_t *);
extern gid_t nh_getgid(void);
extern gid_t nh_getegid(void);

int
(getresuid)(ruid, euid, suid)
uid_t *ruid, *euid, *suid;
{
    return nh_getresuid(ruid, euid, suid);
}

uid_t
(getuid)()
{
    return nh_getuid();
}

uid_t
(geteuid)()
{
    return nh_geteuid();
}

int
(getresgid)(rgid, egid, sgid)
gid_t *rgid, *egid, *sgid;
{
    return nh_getresgid(rgid, egid, sgid);
}

gid_t
(getgid)()
{
    return nh_getgid();
}

gid_t
(getegid)()
{
    return nh_getegid();
}

#endif	/* GETRES_SUPPORT */
