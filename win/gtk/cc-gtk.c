/* $Id: cc-gtk.c,v 1.2 2003-12-23 23:28:46 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2002 */
/* NetHack may be freely redistributed.  See license for details. */

/*
 * This program provides a means of using pkg-config to determine how to
 * compile and link against Gtk+ 2.0. Using this as a front-end to the
 * compiler avoids the limitations of cmd.exe.
 */

#include <stdio.h>
#include <string.h>
#ifdef WIN32
#include <stdlib.h>
#include <process.h>
#include <io.h>
#include <fcntl.h>
#include <direct.h>
#endif
#include "cc-gtk.h"

#define M_COMPILE	'c'
#define M_LINK		'l'

#ifndef WIN32
extern char **environ;	/* MS-Windows defines this in stdlib.h */
#endif

#ifdef WIN32
#define pipe(fds)	_pipe(fds, 1024, _O_BINARY)
#endif

int mode = M_COMPILE;
int verbose = 0;

static void
usage()
{
    fprintf(stderr,"%s", "Usage: cc-gtk [-v] [-c|-l] cc [cc options] ...\n");
    exit(1);
}

static int
parse_cmdline(cmd, argc, argv)
char *cmd;
int *argc;
char ***argv;
{
    int i, j;
    int was_space = 1;
    *argc = 0;
    for(i = 0; cmd[i]; i++) {
	if (was_space && !isspace(cmd[i]))
	    (*argc)++;
	was_space = isspace(cmd[i]);
    }
    *argv = (char **)malloc((*argc + 1) * sizeof(char *));
    if (!*argv)
	return 0;
    *argc = 0;
    was_space = 1;
    for(i = j = 0; ; i++) {
	if (!cmd[i] || isspace(cmd[i])) {
	    if (was_space)
		j = i;
	    else {
		(*argv)[*argc] = (char *)malloc(i - j + 1);
		if (!(*argv)[*argc]) {
		    free(*argv);
		    return 0;
		}
		memcpy((*argv)[*argc], cmd + j, i - j);
		(*argv)[(*argc)++][i - j] = '\0';
		j = i;
		was_space = 1;
	    }
	    if (!cmd[i])
		break;
	} else if (was_space) {
	    was_space = 0;
	    j = i;
	}
    }
    (*argv)[*argc] = (char *)0;
    return 1;
}

static int
str_cmpni(s1, s2, n)
char *s1, *s2;
int n;
{
    int i, r;
    for(i = 0; i < n; i++) {
	r = (s1[i] | 0x20) - (s2[i] | 0x20);
	if (r)
	    return r;
    }
    return 0;
}

static int
env_init(enc, env)
int *enc;
char ***env;
{
    int i;
    for(i = 0; environ[i]; i++)
	;
    *enc = i;
    *env = (char **)malloc((i + 1) * sizeof(char *));
    if (!*env)
	return 0;
    for(i = 0; environ[i]; i++)
	(*env)[i] = environ[i];
    (*env)[i] = (char *)0;
    return 1;
}

static char *
env_get(enc, env, var)
int enc;
char **env;
char *var;
{
    int i, n = strlen(var);
    for(i = 0; i < enc; i++)
	if (!str_cmpni(env[i], var, n) && env[i][n]=='=')
	    return env[i];
    return (char *)0;
}

static int
env_put(enc, env, val)
int *enc;
char ***env;
char *val;
{
    int i, n;
    char *s;
    char **new;
    s = strchr(val, '=');
    if (!s)
	return 0;
    n = s - val;
    for(i = 0; i < *enc; i++)
	if (!str_cmpni((*env)[i], val, n) && (*env)[i][n] == '=') {
	    (*env)[i] = val;
	    return 1;
	}
    new = (char **)realloc(*env, (*enc + 2) * sizeof(char *));
    if (!new)
	return 0;
    *env = new;
    (*env)[(*enc)++] = val;
    (*env)[*enc] = (char *)0;
    return 1;
}

static int
pkg_config(pcargc, pcargv)
int *pcargc;
char ***pcargv;
{
    int i, pid, argc;
    char *cmd = mode == M_COMPILE ? GTKCFLAGS : GTKLIBS;
    char **argv;
    char buf[128];
    int config_bytes = 0;
    char *config;
    int p[2];
    int status;
    int enc;
    char **env;
    char *s, *pcp, *path;
    char sep;
    if (!env_init(&enc, &env))
	return 0;
    s = env_get(enc, env, "PKG_CONFIG_PATH");
    if (s)
	sep = ';';
    else {
	s = "PKG_CONFIG_PATH";
	sep = '=';
    }
    pcp = (char *)malloc(strlen(s) + strlen(PKG_CONFIG_PATH) + 2);
    sprintf(pcp, "%s%c%s", s, sep, PKG_CONFIG_PATH);
    /* Re-write PKG_CONFIG_PATH to use backslashes so that
     * pkg-config's prefix re-writing will work.
     */
    for(i = 0; pcp[i]; i++)
	if (pcp[i] == '/')
	    pcp[i] = '\\';
    env_put(&enc, &env, pcp);
    s = env_get(enc, env, "PATH");
    if (s)
	sep = ';';
    else {
	s = "PATH";
	sep = '=';
    }
    path = (char *)malloc(strlen(s) + strlen(PKG_CONFIG_DLL) + 2);
    sprintf(path, "%s%c%s", s, sep, PKG_CONFIG_DLL);
    env_put(&enc, &env, path);
    if (!parse_cmdline(cmd, &argc, &argv))
	return 0;
    if (verbose) {
	for(i = 0; i < argc; i++) {
	    if (i)
		putc(' ', stderr);
	    fputs(argv[i], stderr);
	}
	putc('\n', stderr);
    }
    if (pipe(p))
	return 0;
    i = dup(1);
    dup2(p[1], 1);
#ifdef WIN32
    pid = spawnvpe(P_NOWAIT, argv[0], argv, env);
    if (pid < 0) {
	perror(argv[0]);
	return 0;
    }
#else
    pid = fork();
    if (pid < 0) {
	perror(argv[0]);
	return 0;
    } else if (!pid) {
	execvpe(argv[0], argv, env);
	perror(argv[0]);
	exit(1);
    }
#endif
    close(p[1]);
    dup2(i, 1);
    close(i);
    free(env);
    free(pcp);
    free(path);
    while((i = read(p[0], buf, sizeof(buf))) > 0) {
	if (config_bytes)
	    config = (char *)realloc(config, config_bytes + i);
	else
	    config = (char *)malloc(i);
	memcpy(config + config_bytes, buf, i);
	config_bytes += i;
    }
    if (!config_bytes) {
	fprintf(stderr, "%s: No output\n", argv[0]);
	return 0;
    }
    if (config[config_bytes - 1] != '\n') {
	fprintf(stderr, "%s: Missing newline (%*.*s)\n", argv[0], config_bytes,
		config_bytes, config);
    }
    for(i = 0; i < argc; i++)
	free(argv[i]);
#ifdef WIN32
    if (config_bytes > 1 && config[config_bytes - 2] == '\r')
	config[config_bytes - 2] = '\0';
    else
#endif
    config[config_bytes - 1] = '\0';
    close(p[0]);
#ifdef WIN32
    (void)cwait(&status, pid, WAIT_CHILD);
    if (status)
	exit(1);
#else
    (void)waitpid(pid, &status, 0);
    if (!WIFEXITED(status) || WEXITSTATUS(status))
	exit(1);
#endif
    if (!parse_cmdline(config, pcargc, pcargv))
	return 0;
    free(config);
    return 1;
}

int
main(argc, argv)
int argc;
char **argv;
{
    int i, n = 1;
    int pcargc, nargc;
    char **pcargv, **nargv;
    while (n < argc && argv[n][0] == '-') {
	if (!argv[n][1]) {
	    n++;
	    break;
	} else if (!argv[n][2])
	    switch(argv[n][1]) {
		case 'v':
		    verbose = 1;
		    break;
		case 'c':
		    mode = M_COMPILE;
		    break;
		case 'l':
		    mode = M_LINK;
		    break;
		default:
		    usage();
	    }
	else
	    usage();
	n++;
    }
    if (n == argc)
	usage();
    pkg_config(&pcargc, &pcargv);
    nargc = argc - n + pcargc;
    nargv = (char **)malloc((nargc + 1) * sizeof(char *));
    if (!nargv) {
	fprintf(stderr,"%s", "cc-gtk: Not enough memory\n");
	exit(1);
    }
    if (mode == M_COMPILE) {
	/* Insert output of pkg-config between compile cmd and its arguments */
	nargv[0] = argv[n];
	for(i = 0; i < pcargc; i++)
	    nargv[i + 1] = pcargv[i];
	for(i = 1; i < argc - n; i++)
	    nargv[i + pcargc] = argv[n + i];
    } else {
	/* Append output of pkg-config after all arguments to link cmd */
	for(i = 0; i < argc - n; i++)
	    nargv[i] = argv[n + i];
	for(i = 0; i < pcargc; i++)
	    nargv[i + argc - n] = pcargv[i];
    }
    nargv[nargc] = (char *)0;
    for(i = 0; i < nargc; i++) {
	if (i)
	    putc(' ', stderr);
	fprintf(stderr, "%s", nargv[i]);
    }
    putc('\n', stderr);
#ifdef WIN32
    i = spawnvp(P_WAIT, nargv[0], nargv);
    if (i < 0) {
	perror(nargv[0]);
	exit(1);
    }
    exit(i);
#else
    execvp(nargv[0], nargv);
    perror(nargv[0]);
    exit(1);
#endif
}
