/* $Id: gtkhackrc.c,v 1.1 2003-04-17 23:15:53 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2003 */
/* NetHack may be freely redistributed.  See license for details. */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <glib.h>
#include "winGTK.h"

#define PARSE_OK		0
#define PARSE_FAIL		1
#define PARSE_ERROR		2

#ifndef SIZE
#define SIZE(array)		(sizeof(array)/sizeof(*(array)))
#endif

#define GTKHACKRC(scanner)	((struct gtkhackrc *)(scanner)->user_data)

typedef struct gtkhackrc_variable {	/* ident [.ident] ... */
    int n_idents;
    gchar **idents;
} GtkHackRcVariable;

typedef struct gtkhackrc_value GtkHackRcValue;

typedef struct gtkhackrc_valuelist {	/* [value [,value] ...] */
    int n_values;
    GtkHackRcValue **values;
} GtkHackRcVList;

typedef struct gtkhackrc_func {		/* var([value [,value] ...]) */
    GtkHackRcVariable var;
    GtkHackRcVList params;
} GtkHackRcFunc;

#define PARSE_VALUE_TYPE_NUMBER		0
#define PARSE_VALUE_TYPE_STRING		1
#define PARSE_VALUE_TYPE_VARIABLE	2
#define PARSE_VALUE_TYPE_FUNCTION	3
#define PARSE_VALUE_TYPE_RECORD		4
#define PARSE_VALUE_TYPE_VECTOR		5
#define PARSE_VALUE_TYPE_VALUE_LIST	6

#define PARSE_VALUE_TYPE_FREE		-1

struct gtkhackrc_value {
    int type;
    union {
	long number;
	gchar *string;
	GtkHackRcVariable var;
	GtkHackRcFunc func;
	GtkHackRcVList record;
	GtkHackRcVList vector;
	GtkHackRcVList vlist;
	GtkHackRcValue *next;	/* For free list */
    } u;
};

extern void rc_window_position(GScanner *scanner, GtkHackRcVList *params,
  GtkHackRcValue *value);
extern void rc_window_size(GScanner *scanner, GtkHackRcVList *params,
  GtkHackRcValue *value);
extern void rc_radar(GScanner *scanner, GtkHackRcValue *value);
extern void rc_connections(GScanner *scanner, GtkHackRcValue *value);
extern void rc_default_connection(GScanner *scanner, GtkHackRcValue *value);

#define GTKHACKRC_VARIABLE	0
#define GTKHACKRC_FUNCTION	1

typedef struct gtkhackrc_setting GtkHackRcSetting;

struct gtkhackrc_setting {
    int type;
    const char *name;
    union {
	void (*dummy)();		/* For initialization prototyping */
	void (*v)(GScanner *scanner, GtkHackRcValue *value);
	void (*f)(GScanner *scanner, GtkHackRcVList *params,
	  GtkHackRcValue *value);
    } handler;
} gtkhackrc_settings[] = {
    GTKHACKRC_FUNCTION, "window.position", rc_window_position,
    GTKHACKRC_FUNCTION, "window.size", rc_window_size,
#if 0
    GTKHACKRC_VARIABLE, "radar", rc_radar,
    GTKHACKRC_VARIABLE, "connections", rc_connections,
    GTKHACKRC_VARIABLE, "default_connection", rc_default_connection,
#endif
};

#define PARSE_ERROR_NONE	0
#define PARSE_ERROR_TOKEN	1	/* Expecting token ... */
#define PARSE_ERROR_RULE	2	/* Expecting rule ... */
#define PARSE_ERROR_RESOURCE	3	/* Not enough memory */

struct gtkhackrc {
    int error_type;
    union {
	GTokenType token;
	gchar *rule;
    } error;
    GtkHackRcValue *value;
    GtkHackRcValue *freelist;
    gchar *variable;			/* Only valid in settings handler */
};

GtkHackRcValue *new_value(GScanner *scanner, int type)
{
    GtkHackRcValue *v = GTKHACKRC(scanner)->freelist;
    if (v)
	GTKHACKRC(scanner)->freelist = v->u.next;
    else
	v = g_malloc(sizeof(*v));
    v->type = type;
    return v;
}

static void
free_value(GScanner *scanner, GtkHackRcValue *value)
{
    int i;
    if (value) {
	switch(value->type)
	{
	    case PARSE_VALUE_TYPE_FREE:
	    case PARSE_VALUE_TYPE_NUMBER:
		break;
	    case PARSE_VALUE_TYPE_STRING:
		g_free(value->u.string);
		break;
	    case PARSE_VALUE_TYPE_VARIABLE:
		for(i = value->u.var.n_idents - 1; i >= 0; i--)
		    g_free(value->u.var.idents[i]);
		g_free(value->u.var.idents);
		break;
	    case PARSE_VALUE_TYPE_FUNCTION:
		for(i = value->u.func.var.n_idents - 1; i >= 0; i--)
		    g_free(value->u.func.var.idents[i]);
		g_free(value->u.func.var.idents);
		for(i = value->u.func.params.n_values - 1; i >= 0; i--)
		    free_value(scanner, value->u.func.params.values[i]);
		g_free(value->u.func.params.values);
		break;
	    case PARSE_VALUE_TYPE_RECORD:
	    case PARSE_VALUE_TYPE_VECTOR:
	    case PARSE_VALUE_TYPE_VALUE_LIST:
		for(i = value->u.vlist.n_values - 1; i >= 0; i--)
		    free_value(scanner, value->u.vlist.values[i]);
		g_free(value->u.vlist.values);
		break;
	}
	value->type = PARSE_VALUE_TYPE_FREE;
	value->u.next = GTKHACKRC(scanner)->freelist;
	GTKHACKRC(scanner)->freelist = value;
    }
}

/*
 * Syntax of a GtkHack profile:
 *
 * profile       : profile setting
 *               |
 *               ;
 *
 * setting       : lval = value
 *               ;
 *
 * lval          : variable
 *               | variable '(' ')'
 *               | variable '(' value_list ')'
 *               ;
 *
 * variable      : identifier
 *               | variable '.' identifier
 *               ;
 *
 * value         : number
 *               | string
 *               | lval
 *               | record
 *               | vector
 *               ;
 *
 * record        : '{' value_list '}'
 *               ;
 *
 * vector        : '[' value_list ']'
 *               | '[' ']'
 *               ;
 *
 * value_list    : value_list ',' value
 *               | value
 *               ;
 */

static int parse_setting(GScanner *scanner, int failok);
static int parse_lval(GScanner *scanner, int failok);
static int parse_variable(GScanner *scanner, int failok);
static int parse_value(GScanner *scanner, int failok);
static int parse_record(GScanner *scanner, int failok);
static int parse_vector(GScanner *scanner, int failok);
static int parse_value_list(GScanner *scanner, int failok);

static int
match(GScanner *scanner, GTokenType token)
{
    if (g_scanner_peek_next_token(scanner) == token) {
	g_scanner_get_next_token(scanner);
	return TRUE;
    } else
	return FALSE;
}

/*
 * Conventions of parse_ functions:
 *	- Must only return PARSE_FAIL if failok set
 *	- If PARSE_OK is returned, then value must be set accordingly
 *	- If PARSE_FAIL is returned, then the read pointer and value
 *	  must be unchanged.
 */

static int
parse_setting(GScanner *scanner, int failok)
{
    int i, n, retval;
    gchar *name;
    GtkHackRcValue *lhs;
    GtkHackRcVariable *var;
    retval = parse_lval(scanner, failok);
    lhs = GTKHACKRC(scanner)->value;
    GTKHACKRC(scanner)->value = NULL;
    if (retval != PARSE_OK) {
	free_value(scanner, lhs);
	return retval;
    }
    if (!match(scanner, G_TOKEN_EQUAL_SIGN)) {
	GTKHACKRC(scanner)->error_type = PARSE_ERROR_TOKEN;
	GTKHACKRC(scanner)->error.token = G_TOKEN_EQUAL_SIGN;
	free_value(scanner, lhs);
	return PARSE_ERROR;
    }
    retval = parse_value(scanner, FALSE);
    if (retval == PARSE_OK) {
	if (lhs->type == PARSE_VALUE_TYPE_VARIABLE)
	    var = &lhs->u.var;
	else
	    var = &lhs->u.func.var;
	n = var->n_idents;
	for(i = 0; i < var->n_idents; i++)
	    n += strlen(var->idents[i]);
	name = g_malloc(n);
	if (!name) {
	    GTKHACKRC(scanner)->error_type = PARSE_ERROR_RESOURCE;
	    retval = PARSE_ERROR;
	} else {
	    *name = '\0';
	    for(i = n = 0; i < var->n_idents; i++)
	    {
		if (i)
		    name[n++] = '.';
		strcpy(name + n, var->idents[i]);
		n += strlen(var->idents[i]);
	    }
	    GTKHACKRC(scanner)->variable = name;
	    if (lhs->type == PARSE_VALUE_TYPE_VARIABLE) {
		for(i = 0; i < SIZE(gtkhackrc_settings); i++)
		    if (gtkhackrc_settings[i].type == GTKHACKRC_VARIABLE &&
		      !strcmp(gtkhackrc_settings[i].name, name))
			break;
		if (i < SIZE(gtkhackrc_settings))
		    gtkhackrc_settings[i].handler.v(scanner,
		      GTKHACKRC(scanner)->value);
#ifdef DEBUG
		else
		    fprintf(stderr, "gtkhackrc: variable %s ignored\n", name);
#endif
	    } else {
		for(i = 0; i < SIZE(gtkhackrc_settings); i++)
		    if (gtkhackrc_settings[i].type == GTKHACKRC_FUNCTION &&
		      !strcmp(gtkhackrc_settings[i].name, name))
			break;
		if (i < SIZE(gtkhackrc_settings))
		    gtkhackrc_settings[i].handler.f(scanner,
		      &lhs->u.func.params, GTKHACKRC(scanner)->value);
#ifdef DEBUG
		else
		    fprintf(stderr, "gtkhackrc: function %s ignored\n", name);
#endif
	    }
	    GTKHACKRC(scanner)->variable = NULL;
	    g_free(name);
	}
    }
    free_value(scanner, lhs);
    free_value(scanner, GTKHACKRC(scanner)->value);
    GTKHACKRC(scanner)->value = NULL;
    return retval;
}

static int
parse_lval(GScanner *scanner, int failok)
{
    int retval;
    GtkHackRcValue *func;
    retval = parse_variable(scanner, failok);
    if (retval != PARSE_OK)
	return retval;
    if (match(scanner, G_TOKEN_LEFT_PAREN)) {
	func = GTKHACKRC(scanner)->value;
	GTKHACKRC(scanner)->value = NULL;
	func->type = PARSE_VALUE_TYPE_FUNCTION;
	func->u.func.var = func->u.var;
	retval = parse_value_list(scanner, TRUE);
	if (retval == PARSE_OK) {
	    func->u.func.params = GTKHACKRC(scanner)->value->u.vlist;
	    GTKHACKRC(scanner)->value->u.vlist.n_values = 0;
	    GTKHACKRC(scanner)->value->u.vlist.values = NULL;
	    free_value(scanner, GTKHACKRC(scanner)->value);
	} else if (retval == PARSE_FAIL) {
	    func->u.func.params.n_values = 0;
	    func->u.func.params.values = NULL;
	}
	GTKHACKRC(scanner)->value = func;
	if (retval != PARSE_ERROR)
	    if (!match(scanner, G_TOKEN_RIGHT_PAREN)) {
		GTKHACKRC(scanner)->error_type = PARSE_ERROR_TOKEN;
		GTKHACKRC(scanner)->error.token = G_TOKEN_RIGHT_PAREN;
		retval = PARSE_ERROR;
	    }
    }
    return retval;
}

static int
parse_variable(GScanner *scanner, int failok)
{
    int n, matched;
    gchar **new;
    GtkHackRcValue *var;
    var = new_value(scanner, PARSE_VALUE_TYPE_VARIABLE);
    n = 0;
    var->u.var.idents = NULL;
    do {
	matched = match(scanner, G_TOKEN_IDENTIFIER);
	if (matched) {
	    new = g_realloc(var->u.var.idents, ++n * sizeof(*new));
	    if (!new) {
		free_value(scanner, var);
		GTKHACKRC(scanner)->error_type = PARSE_ERROR_RESOURCE;
		return PARSE_ERROR;
	    }
	    var->u.var.idents = new;
	    var->u.var.idents[n - 1] =
	      g_strdup(g_scanner_cur_value(scanner).v_identifier);
	    if (!var->u.var.idents[n - 1]) {
		GTKHACKRC(scanner)->error_type = PARSE_ERROR_RESOURCE;
		return PARSE_ERROR;
	    }
	}
    } while (matched && match(scanner, '.'));
    var->u.var.n_idents = n;
    if (!matched)
	free_value(scanner, var);
    else
	GTKHACKRC(scanner)->value = var;
    if (!failok && !matched) {
	GTKHACKRC(scanner)->error_type = PARSE_ERROR_TOKEN;
	GTKHACKRC(scanner)->error.token = G_TOKEN_IDENTIFIER;
	return PARSE_ERROR;
    }
    else
	return matched ? PARSE_OK : PARSE_FAIL;
}

static int
parse_value(GScanner *scanner, int failok)
{
    int retval;
    GtkHackRcValue *var;
    retval = match(scanner, G_TOKEN_INT) ? PARSE_OK : PARSE_FAIL;
    if (retval == PARSE_OK) {
	var = new_value(scanner, PARSE_VALUE_TYPE_NUMBER);
	var->u.number = g_scanner_cur_value(scanner).v_int;
    } else {
	retval = match(scanner, G_TOKEN_STRING) ? PARSE_OK : PARSE_FAIL;
	if (retval == PARSE_OK) {
	    var = new_value(scanner, PARSE_VALUE_TYPE_STRING);
	    var->u.string = g_strdup(g_scanner_cur_value(scanner).v_string);
	}
    }
    if (retval == PARSE_OK)
	GTKHACKRC(scanner)->value = var;
    else
	retval = parse_lval(scanner, TRUE);
    if (retval == PARSE_FAIL)
	retval = parse_record(scanner, TRUE);
    if (retval == PARSE_FAIL)
	retval = parse_vector(scanner, TRUE);
    if (!failok && retval == PARSE_FAIL) {
	GTKHACKRC(scanner)->error_type = PARSE_ERROR_RULE;
	GTKHACKRC(scanner)->error.rule = "value";
	retval = PARSE_ERROR;
    }
    return retval;
}

static int
parse_record(GScanner *scanner, int failok)
{
    int retval;
    if (match(scanner, G_TOKEN_LEFT_CURLY)) {
	retval = parse_value_list(scanner, FALSE);
	if (retval == PARSE_OK) {
	    GTKHACKRC(scanner)->value->type = PARSE_VALUE_TYPE_RECORD;
	    if (!match(scanner, G_TOKEN_RIGHT_CURLY)) {
		GTKHACKRC(scanner)->error_type = PARSE_ERROR_TOKEN;
		GTKHACKRC(scanner)->error.token = G_TOKEN_RIGHT_CURLY;
		retval = PARSE_ERROR;
	    }
	}
    } else if (!failok) {
	GTKHACKRC(scanner)->error_type = PARSE_ERROR_TOKEN;
	GTKHACKRC(scanner)->error.token = G_TOKEN_LEFT_CURLY;
	retval = PARSE_ERROR;
    }
    else
	retval = PARSE_FAIL;
    return retval;
}

static int
parse_vector(GScanner *scanner, int failok)
{
    int retval;
    if (match(scanner, G_TOKEN_LEFT_BRACE)) {
	retval = parse_value_list(scanner, TRUE);
	if (retval != PARSE_ERROR)
	    if (match(scanner, G_TOKEN_RIGHT_BRACE)) {
		if (retval == PARSE_OK)
		    GTKHACKRC(scanner)->value->type = PARSE_VALUE_TYPE_VECTOR;
		else {
		    GTKHACKRC(scanner)->value =
		      new_value(scanner, PARSE_VALUE_TYPE_VECTOR);
		    GTKHACKRC(scanner)->value->u.vector.n_values = 0;
		    GTKHACKRC(scanner)->value->u.vector.values = NULL;
		}
		retval = PARSE_OK;
	    } else {
		GTKHACKRC(scanner)->error_type = PARSE_ERROR_TOKEN;
		GTKHACKRC(scanner)->error.token = G_TOKEN_RIGHT_BRACE;
		retval = PARSE_ERROR;
	    }
    } else if (!failok) {
	GTKHACKRC(scanner)->error_type = PARSE_ERROR_TOKEN;
	GTKHACKRC(scanner)->error.token = G_TOKEN_LEFT_BRACE;
	retval = PARSE_ERROR;
    }
    else
	retval = PARSE_FAIL;
    return retval;
}

static int
parse_value_list(GScanner *scanner, int failok)
{
    int n, retval;
    GtkHackRcValue *vl;
    GtkHackRcValue **new;
    vl = new_value(scanner, PARSE_VALUE_TYPE_VALUE_LIST);
    n = 0;
    vl->u.vlist.values = NULL;
    do {
	retval = parse_value(scanner, failok);
	if (retval == PARSE_OK) {
	    new = g_realloc(vl->u.vlist.values, ++n * sizeof(*new));
	    if (!new) {
		free_value(scanner, vl);
		GTKHACKRC(scanner)->error_type = PARSE_ERROR_RESOURCE;
		return PARSE_ERROR;
	    }
	    vl->u.vlist.values = new;
	    vl->u.vlist.values[n - 1] = GTKHACKRC(scanner)->value;
	    GTKHACKRC(scanner)->value = NULL;
	    failok = FALSE;
	}
    } while (retval == PARSE_OK && match(scanner, ','));
    vl->u.vlist.n_values = n;
    if (retval != PARSE_OK)
	free_value(scanner, vl);
    else
	GTKHACKRC(scanner)->value = vl;
    if (!failok && retval == PARSE_FAIL) {
	GTKHACKRC(scanner)->error_type = PARSE_ERROR_TOKEN;
	GTKHACKRC(scanner)->error.token = G_TOKEN_IDENTIFIER;
	retval = PARSE_ERROR;
    }
    return retval;
}

int
nh_read_gtkhackrc(void)
{
    int fd, retval;
    char *home, *file;
    struct gtkhackrc rc;
    GScanner *scanner;
    GtkHackRcValue *v, *vn;
    home = getenv("HOME");
    if (!home)
	return -1;
    file = malloc(strlen(home) + 12);
    if (!file)
	return -1;
    sprintf(file, "%s/.gtkhackrc", home);
    fd = open(file, O_RDONLY);
    if (fd < 0) {
	free(file);
	return -1;
    }
    scanner = g_scanner_new(NULL);
    if (!scanner) {
	close(fd);
	free(file);
	return -1;
    }
    rc.error_type = PARSE_ERROR_NONE;
    rc.value = NULL;
    rc.freelist = NULL;
    scanner->user_data = (gpointer)&rc;
    scanner->config->scan_float = FALSE;	/* Don't treat '.' as special */
    scanner->input_name = file;
    g_scanner_input_file(scanner, fd);
    while((retval = parse_setting(scanner, TRUE)) == PARSE_OK)
	;
    if (retval == PARSE_ERROR) {
	switch (rc.error_type)
	{
	    case PARSE_ERROR_TOKEN:
		g_scanner_get_next_token(scanner);
		g_scanner_unexp_token(scanner, rc.error.token, NULL, NULL,
		  NULL, "Read of GtkHack profile aborted", TRUE);
		break;
	    case PARSE_ERROR_RULE:
		g_scanner_get_next_token(scanner);
		g_scanner_unexp_token(scanner,
		  scanner->token == G_TOKEN_SYMBOL ? G_TOKEN_IDENTIFIER :
		    G_TOKEN_SYMBOL,
		  rc.error.rule, rc.error.rule, NULL,
		  "Read of GtkHack profile aborted", TRUE);
		break;
	    case PARSE_ERROR_RESOURCE:
		g_scanner_error(scanner, "%s%s%s", "Resource failure", " - ",
		  "Read of GtkHack profile aborted");
		break;
	}
    }
    free_value(scanner, rc.value);
    for(v = rc.freelist; v; ) {
	vn = v->u.next;
	g_free(v);
	v = vn;
    }
    g_scanner_destroy(scanner);
    close(fd);
    free(file);
    /*
     * Don't update the file on exit if we failed to read it correctly
     * due to a (presumably temporary) resource failure.
     */
    if (retval != PARSE_ERROR || rc.error_type != PARSE_ERROR_RESOURCE)
	atexit(nh_write_gtkhackrc);
    return 0;
}

void
nh_write_gtkhackrc(void)
{
    FILE *fp;
    char *home, *file;
    home = getenv("HOME");
    if (!home)
	return;
    file = malloc(strlen(home) + 12);
    if (!file)
	return;
    sprintf(file, "%s/.gtkhackrc", home);
    fp = fopen(file, "w");
    if (!fp) {
	perror(file);
	free(file);
	return;
    }
    fprintf(fp,
      "# This file is generated by the Gtk interface of NetHack & friends.\n"
      "# Do not edit.\n"
      "\n"
      "version = \"0.1\"\n");
    nh_session_save(fp);
    fclose(fp);
    free(file);
}

/* Helper functions for handlers to check the value being set and, for
 * functions, the parameters.
 */

int gtkhackrc_check_no(GScanner *scanner, GtkHackRcVList *params,
  const char *names, int n)
{
    if (params->n_values > n) {
	g_scanner_warn(scanner, "too many %s to function `%s'", names,
	  GTKHACKRC(scanner)->variable);
	return FALSE;
    } else if (params->n_values < n) {
	g_scanner_warn(scanner, "too few %s to function `%s'", names,
	  GTKHACKRC(scanner)->variable);
	return FALSE;
    } else
	return TRUE;
}

int gtkhackrc_check_type(GScanner *scanner, GtkHackRcValue *value,
  const char *name, int type)
{
    if (value->type != type) {
	g_scanner_warn(scanner, "incompatible type for %s of `%s'", name,
	  GTKHACKRC(scanner)->variable);
	return FALSE;
    } else
	return TRUE;
}

void rc_window_position(GScanner *scanner, GtkHackRcVList *params,
  GtkHackRcValue *value)
{
    if (!gtkhackrc_check_no(scanner, params, "arguments", 1) ||
      !gtkhackrc_check_type(scanner, params->values[0], "argument 1",
        PARSE_VALUE_TYPE_STRING) ||
      !gtkhackrc_check_type(scanner, value, "value", PARSE_VALUE_TYPE_RECORD) ||
      !gtkhackrc_check_no(scanner, &value->u.record, "fields in value", 2) ||
      !gtkhackrc_check_type(scanner, value->u.record.values[0], "x- value",
        PARSE_VALUE_TYPE_NUMBER) ||
      !gtkhackrc_check_type(scanner, value->u.record.values[1], "y- value",
        PARSE_VALUE_TYPE_NUMBER))
	return;
    nh_session_set_geometry(params->values[0]->u.string,
      value->u.record.values[0]->u.number, value->u.record.values[1]->u.number,
      -1, -1);
}

void rc_window_size(GScanner *scanner, GtkHackRcVList *params,
  GtkHackRcValue *value)
{
    if (!gtkhackrc_check_no(scanner, params, "arguments", 1) ||
      !gtkhackrc_check_type(scanner, params->values[0], "argument 1",
        PARSE_VALUE_TYPE_STRING) ||
      !gtkhackrc_check_type(scanner, value, "value", PARSE_VALUE_TYPE_RECORD) ||
      !gtkhackrc_check_no(scanner, &value->u.record, "fields in value", 2) ||
      !gtkhackrc_check_type(scanner, value->u.record.values[0], "width",
        PARSE_VALUE_TYPE_NUMBER) ||
      !gtkhackrc_check_type(scanner, value->u.record.values[1], "height",
        PARSE_VALUE_TYPE_NUMBER))
	return;
    nh_session_set_geometry(params->values[0]->u.string, -1, -1,
      value->u.record.values[0]->u.number, value->u.record.values[1]->u.number);
}
