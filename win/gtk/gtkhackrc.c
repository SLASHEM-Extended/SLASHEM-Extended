/* $Id: gtkhackrc.c,v 1.12 2004-01-17 12:30:01 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2003 */
/* NetHack may be freely redistributed.  See license for details. */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <glib.h>
#include "winGTK.h"
#include "prxyclnt.h"

/*
 * This is the version of the gtkhackrc file format only. It has no
 * relation to the version of gtkhack. Increase the minor version
 * number when the new file format can not be read by earlier versions
 * of gtkhack. Increase the major version number (and set minor to 0)
 * when the new version of gtkhack can not read earlier file formats.
 *
 * Note: adding new settings will not normally require changing the
 * file format version since gtkhack ignores (and preserves) settings
 * it does not recognize as long as they follow the supported syntax.
 */

#define GTKHACKRC_VER_MAJOR	0
#define GTKHACKRC_VER_MINOR	2

#define GTKHACKRC_VERSTR(major, minor)		#major "." #minor
#define GTKHACKRC_VERSTRING(major, minor)	GTKHACKRC_VERSTR(major, minor)
#define GTKHACKRC_VERSION	GTKHACKRC_VERSTRING(GTKHACKRC_VER_MAJOR,\
					GTKHACKRC_VER_MINOR)

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

extern int check_rc_version(GScanner *scanner, GtkHackRcValue *value);
extern void rc_window_position(GScanner *scanner, GtkHackRcVList *params,
  GtkHackRcValue *value);
extern void rc_window_size(GScanner *scanner, GtkHackRcVList *params,
  GtkHackRcValue *value);
extern void rc_map_font(GScanner *scanner, GtkHackRcValue *value);
extern void rc_map_clip_dist2(GScanner *scanner, GtkHackRcValue *value);
extern void rc_radar(GScanner *scanner, GtkHackRcValue *value);
#ifdef GTKHACK
extern void rc_connections(GScanner *scanner, GtkHackRcValue *value);
extern void rc_default_connection(GScanner *scanner, GtkHackRcValue *value);
#endif
#ifdef GTK_PROXY
extern void rc_proxy_cachedir(GScanner *scanner, GtkHackRcValue *value);
#endif
extern void rc_help_font(GScanner *scanner, GtkHackRcValue *value);

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
    GTKHACKRC_VARIABLE, "map.font", rc_map_font,
    GTKHACKRC_VARIABLE, "map.clip_dist2", rc_map_clip_dist2,
    GTKHACKRC_VARIABLE, "radar", rc_radar,
#ifdef GTKHACK
    GTKHACKRC_VARIABLE, "connections", rc_connections,
    GTKHACKRC_VARIABLE, "default_connection", rc_default_connection,
#endif
#ifdef GTK_PROXY
    GTKHACKRC_VARIABLE, "proxy.cachedir", rc_proxy_cachedir,
#endif
    GTKHACKRC_VARIABLE, "help.font", rc_help_font,
};

#define PARSE_ERROR_NONE		0
#define PARSE_ERROR_TOKEN		1	/* Expecting token ... */
#define PARSE_ERROR_RULE		2	/* Expecting rule ... */
#define PARSE_ERROR_RESOURCE		3	/* Not enough memory */
#define PARSE_ERROR_INCOMPATIBLE	4	/* Incompatible profile */

struct gtkhackrc {
    int error_type;
    union {
	GTokenType token;
	gchar *rule;
    } error;
    GtkHackRcValue *value;
    GtkHackRcValue *freelist;
    gchar *variable;			/* Only valid in settings handler */
#ifdef WIN32				/* Only valid in nh_write_gtkhackrc */
    HKEY key;
#else
    FILE *fp;
#endif
};

/* Preserved for writing back to file */
static GSList *unrecognized_settings = NULL;
#ifndef WIN32
static gboolean write_verspecific_file = FALSE;
#endif

static int
action_setting(GScanner *scanner, GtkHackRcValue *lhs, GtkHackRcValue *rhs);

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
    if (retval == PARSE_OK)
	retval = action_setting(scanner, lhs, GTKHACKRC(scanner)->value);
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

static GString *print_value_list(GtkHackRcVList *vl);

static GString *print_variable(GtkHackRcVariable *var)
{
    int i, n;
    GString *str;
    n = var->n_idents;
    for(i = 0; i < var->n_idents; i++)
	n += strlen(var->idents[i]);
    str = g_string_sized_new(n - 1);
    for(i = 0; i < var->n_idents; i++) {
	if (i)
	    g_string_append_c(str, '.');
	g_string_append(str, var->idents[i]);
    }
    return str;
}

static GString *print_value(GtkHackRcValue *var)
{
    int i;
    GString *str, *vl;
    switch(var->type) {
	case PARSE_VALUE_TYPE_NUMBER:
	    str = g_string_new("");
	    g_string_printf(str, "%ld", var->u.number);
	    return str;
	case PARSE_VALUE_TYPE_STRING:
	    str = g_string_new("");
	    g_string_printf(str, "\"%s\"", g_strescape(var->u.string, ""));
	    return str;
	case PARSE_VALUE_TYPE_VARIABLE:
	    return print_variable(&var->u.var);
	case PARSE_VALUE_TYPE_FUNCTION:
	    str = print_variable(&var->u.func.var);
	    g_string_append_c(str, '(');
	    if (var->u.func.params.n_values) {
		vl = print_value_list(&var->u.func.params);
		g_string_append_len(str, vl->str, vl->len);
		g_string_free(vl, TRUE);
	    }
	    g_string_append_c(str, ')');
	    return str;
	case PARSE_VALUE_TYPE_RECORD:
	    str = g_string_new("{");
	    vl = print_value_list(&var->u.record);
	    g_string_append_len(str, vl->str, vl->len);
	    g_string_free(vl, TRUE);
	    g_string_append_c(str, '}');
	    return str;
	case PARSE_VALUE_TYPE_VECTOR:
	    str = g_string_new("[");
	    if (var->u.vector.n_values) {
		vl = print_value_list(&var->u.vector);
		g_string_append_len(str, vl->str, vl->len);
		g_string_free(vl, TRUE);
	    }
	    g_string_append_c(str, ']');
	    return str;
	case PARSE_VALUE_TYPE_VALUE_LIST:
	    return print_value_list(&var->u.vlist);
    }
}

static GString *print_value_list(GtkHackRcVList *vl)
{
    int i;
    GString *str, *v;
    str = print_value(vl->values[0]);
    for(i = 1; i < vl->n_values; i++) {
	g_string_append_c(str, ',');
	v = print_value(vl->values[i]);
	g_string_append_len(str, v->str, v->len);
	g_string_free(v, TRUE);
    }
    return str;
}

static int
action_setting(GScanner *scanner, GtkHackRcValue *lhs, GtkHackRcValue *rhs)
{
    int i, n, retval = 0;
    gchar *name;
    GString *str, *value;
    GtkHackRcVariable *var;
    {
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
		    gtkhackrc_settings[i].handler.v(scanner, rhs);
		else if (!strcmp(name, "version")) {
		    i = -1;
		    retval = check_rc_version(scanner, rhs);
		}
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
		      &lhs->u.func.params, rhs);
#ifdef DEBUG
		else
		    fprintf(stderr, "gtkhackrc: function %s ignored\n", name);
#endif
	    }
	    g_free(name);
	    if (i >= SIZE(gtkhackrc_settings)) {
		str = print_value(lhs);
		g_string_append(str, " = ");
		value = print_value(rhs);
		g_string_append_len(str, value->str, value->len);
		g_string_free(value, TRUE);
		unrecognized_settings =
		  g_slist_append(unrecognized_settings, str->str);
		g_string_free(str, FALSE);
	    }
	}
    }
    return retval;
}

static void clear_unrecognized()
{
    if (unrecognized_settings) {
	g_slist_foreach(unrecognized_settings, (GFunc)g_free, NULL);
	g_slist_free(unrecognized_settings);
	unrecognized_settings = NULL;
    }
}

/*
 * Return TRUE if okay to write file on exit; FALSE if we failed to read
 * file due to lack of resources.
 */

#ifdef WIN32
static int
nh_scan_gtkhackrc(GScanner *scanner)
{
    int i, retval;
    struct gtkhackrc *rc = GTKHACKRC(scanner);
    GtkHackRcValue *v;
    HKEY key;
    DWORD type, no_values, value_len, data_len, value_count, data_count;
    char *s, *value, *data;
    int minor;
    for(minor = GTKHACKRC_VER_MINOR; minor >= 0; minor--) {
	s = g_strdup_printf("Software\\NetHack\\GtkHack\\%d.%d",
	  GTKHACKRC_VER_MAJOR, minor);
        retval = RegOpenKeyEx(HKEY_CURRENT_USER, s, 0, KEY_READ, &key);
	g_free(s);
	if (retval == ERROR_SUCCESS)
	    break;
    }
    if (minor < 0)
	return TRUE;
    if (RegQueryInfoKey(key, NULL, NULL, NULL, NULL, NULL, NULL, &no_values,
	    &value_len, &data_len, NULL, NULL) != ERROR_SUCCESS) {
	RegCloseKey(key);
	return TRUE;
    }
    value = malloc(value_len + 1);
    data = malloc(data_len + 1);
    if (!value || !data) {
	free(value);
	free(data);
	RegCloseKey(key);
	return FALSE;
    }
    clear_unrecognized();
    for(i = no_values - 1; i >= 0; i--) {
	value_count = value_len + 1;
	data_count = data_len + 1;
	if (RegEnumValue(key, i, value, &value_count, NULL, &type, data,
		&data_count) == ERROR_SUCCESS && type == REG_SZ) {
	    scanner->input_name = value;
	    g_scanner_input_text(scanner, value, strlen(value));
	    retval = parse_lval(scanner, FALSE);
	    if (retval == PARSE_OK) {
		v = GTKHACKRC(scanner)->value;
		GTKHACKRC(scanner)->value = NULL;
		g_scanner_input_text(scanner, data, strlen(data));
		retval = parse_value(scanner, FALSE);
	    }
	    if (retval == PARSE_OK)
		retval = action_setting(scanner, v, GTKHACKRC(scanner)->value);
	    if (retval == PARSE_ERROR) {
		switch (rc->error_type)
		{
		    case PARSE_ERROR_TOKEN:
			g_scanner_get_next_token(scanner);
			g_scanner_unexp_token(scanner, rc->error.token, NULL,
			  NULL, NULL, NULL, TRUE);
			break;
		    case PARSE_ERROR_RULE:
			g_scanner_get_next_token(scanner);
			g_scanner_unexp_token(scanner,
			  scanner->token == G_TOKEN_SYMBOL ?
			    G_TOKEN_IDENTIFIER : G_TOKEN_SYMBOL,
			  rc->error.rule, rc->error.rule, NULL, NULL, TRUE);
			break;
		    case PARSE_ERROR_RESOURCE:
			g_scanner_error(scanner, "%s%s%s", "Resource failure",
			  " - ", "Read of GtkHack profile aborted");
			i = -1;
			break;
		}
	    }
	}
    }
    free(value);
    free(data);
    RegCloseKey(key);
    return retval != PARSE_ERROR || rc->error_type != PARSE_ERROR_RESOURCE;
}
#else	/* WIN32 */
static int
nh_scan_gtkhackrc(GScanner *scanner)
{
    int fd, retval;
    struct gtkhackrc *rc = GTKHACKRC(scanner);
    char *home, *file;
    home = getenv("HOME");
    if (!home)
	return FALSE;			/* Can't write file */
    file = malloc(strlen(home) + 13 + strlen(GTKHACKRC_VERSION));
    if (!file)
	return FALSE;
    sprintf(file, "%s/.gtkhackrc-" GTKHACKRC_VERSION, home);
    fd = open(file, O_RDONLY);
    if (fd < 0) {
	write_verspecific_file = FALSE;
	sprintf(file, "%s/.gtkhackrc", home);
	fd = open(file, O_RDONLY);
	if (fd < 0) {
	    free(file);
	    return TRUE;
	}
    }
    else
	write_verspecific_file = TRUE;
    scanner->input_name = file;
    g_scanner_input_file(scanner, fd);
    clear_unrecognized();
    while((retval = parse_setting(scanner, TRUE)) == PARSE_OK)
	;
    if (retval == PARSE_ERROR) {
	switch (rc->error_type)
	{
	    case PARSE_ERROR_TOKEN:
		g_scanner_get_next_token(scanner);
		g_scanner_unexp_token(scanner, rc->error.token, NULL, NULL,
		  NULL, "Read of GtkHack profile aborted", TRUE);
		break;
	    case PARSE_ERROR_RULE:
		g_scanner_get_next_token(scanner);
		g_scanner_unexp_token(scanner,
		  scanner->token == G_TOKEN_SYMBOL ? G_TOKEN_IDENTIFIER :
		    G_TOKEN_SYMBOL,
		  rc->error.rule, rc->error.rule, NULL,
		  "Read of GtkHack profile aborted", TRUE);
		break;
	    case PARSE_ERROR_RESOURCE:
		g_scanner_error(scanner, "%s%s%s", "Resource failure", " - ",
		  "Read of GtkHack profile aborted");
		break;
	    case PARSE_ERROR_INCOMPATIBLE:
		g_scanner_error(scanner, "%s%s%s", "Incompatible profile",
		  " - ", "Read of GtkHack profile aborted");
		break;
	    default:
		g_scanner_error(scanner, "%s%s%s", "Unknown error", " - ",
		  "Read of GtkHack profile aborted");
		break;
	}
    }
    close(fd);
    free(file);
    return retval != PARSE_ERROR || rc->error_type != PARSE_ERROR_RESOURCE;
}
#endif	/* WIN32 */

int
nh_read_gtkhackrc(void)
{
    int retval;
    struct gtkhackrc rc;
    GScanner *scanner;
    GtkHackRcValue *v, *vn;
    scanner = g_scanner_new(NULL);
    if (!scanner)
	return -1;
    rc.error_type = PARSE_ERROR_NONE;
    rc.value = NULL;
    rc.freelist = NULL;
    scanner->user_data = (gpointer)&rc;
    scanner->config->scan_float = FALSE;	/* Don't treat '.' as special */
    /*
     * Don't update the file on exit if we failed to read it correctly
     * due to a (presumably temporary) resource failure.
     */
    if (nh_scan_gtkhackrc(scanner))
	atexit(nh_write_gtkhackrc);
    free_value(scanner, rc.value);
    for(v = rc.freelist; v; ) {
	vn = v->u.next;
	g_free(v);
	v = vn;
    }
    g_scanner_destroy(scanner);
    return 0;
}

void
nh_gtkhackrc_store(struct gtkhackrc *rc, const char *fmt, ...)
{
#ifdef WIN32
    gchar *value, *data, *s;
#endif
    va_list args;
    va_start(args, fmt);
#ifdef WIN32
    value = g_strdup_vprintf(fmt, args);
    s = strchr(value, '=');
    if (s) {
	for(data = s + 1; *data == ' '; data++)
	    ;
	for(s--; *s == ' '; s--)
	    ;
	s[1] = '\0';
	RegSetValueEx(rc->key, value, 0, REG_SZ, data, strlen(data) + 1);
    }
    free(value);
#else
    vfprintf(rc->fp, fmt, args);
    putc('\n', rc->fp);
#endif
    va_end(args);
}

void
nh_write_gtkhackrc(void)
{
    struct gtkhackrc rc;
    GSList *list;
#ifdef WIN32
    RegDeleteKey(HKEY_CURRENT_USER,
      "Software\\NetHack\\GtkHack\\" GTKHACKRC_VERSION);
    RegCreateKeyEx(HKEY_CURRENT_USER,
      "Software\\NetHack\\GtkHack\\" GTKHACKRC_VERSION,
      0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &rc.key, NULL);
#else
    char *home, *file;
    home = getenv("HOME");
    if (!home)
	return;
    if (write_verspecific_file)
	file = malloc(strlen(home) + 13 + strlen(GTKHACKRC_VERSION));
    else
	file = malloc(strlen(home) + 12);
    if (!file)
	return;
    if (write_verspecific_file)
	sprintf(file, "%s/.gtkhackrc-" GTKHACKRC_VERSION, home);
    else
	sprintf(file, "%s/.gtkhackrc", home);
    rc.fp = fopen(file, "w");
    if (!rc.fp) {
	perror(file);
	free(file);
	return;
    }
    fprintf(rc.fp,
      "# This file is generated by the Gtk interface of NetHack & friends.\n"
      "# Do not edit.\n"
      "\n"
      "version = \"" GTKHACKRC_VERSION "\"\n");
#endif
    nh_session_save(&rc);
    GTK_preferences_save(&rc);
#ifdef GTKHACK
    GTK_connection_save(&rc);
#endif
#ifdef GTK_PROXY
    nh_proxy_cache_save(&rc);
#endif
    nh_help_save(&rc);
    if (unrecognized_settings)
	for(list = unrecognized_settings; list; list = list->next)
	    nh_gtkhackrc_store(&rc, "%s", (gchar *)list->data);
#ifdef WIN32
    RegCloseKey(rc.key);
#else
    fclose(rc.fp);
    free(file);
#endif
}

/* Helper functions for handlers to check the value being set and, for
 * functions, the parameters.
 */

int gtkhackrc_check_no(GScanner *scanner, GtkHackRcVList *params,
  const char *names, int mn, int mx)
{
    if (params->n_values > mx) {
	g_scanner_warn(scanner, "too many %s to function `%s'", names,
	  GTKHACKRC(scanner)->variable);
	return FALSE;
    } else if (params->n_values < mn) {
	g_scanner_warn(scanner, "too few %s to function `%s'", names,
	  GTKHACKRC(scanner)->variable);
	return FALSE;
    } else
	return TRUE;
}

int gtkhackrc_check_type(GScanner *scanner, GtkHackRcValue *value,
  const char *name, int type)
{
    char *types[]={"number", "string", "variable", "function", "record",
      "vector", "value list"};
    if (value->type != type) {
	g_scanner_warn(scanner, "incompatible type for %s of `%s'", name,
	  GTKHACKRC(scanner)->variable);
	g_scanner_warn(scanner, "(expected %s, got %s)", types[type],
	  value->type >= 0 && value->type < SIZE(types) ? types[value->type] :
	  "value of unknown type");
	return FALSE;
    } else
	return TRUE;
}

int check_rc_version(GScanner *scanner, GtkHackRcValue *value)
{
    int major, minor;
    if (!gtkhackrc_check_type(scanner, value, "value",
      PARSE_VALUE_TYPE_STRING)) {
	GTKHACKRC(scanner)->error_type = PARSE_ERROR_INCOMPATIBLE;
#ifndef WIN32
	write_verspecific_file = TRUE;
#endif
	return PARSE_ERROR;
    }
    if (sscanf(value->u.string, "%d.%d", &major, &minor) != 2) {
	GTKHACKRC(scanner)->error_type = PARSE_ERROR_INCOMPATIBLE;
#ifndef WIN32
	write_verspecific_file = TRUE;
#endif
	return PARSE_ERROR;
    }
    if (major != GTKHACKRC_VER_MAJOR || minor > GTKHACKRC_VER_MINOR) {
	GTKHACKRC(scanner)->error_type = PARSE_ERROR_INCOMPATIBLE;
#ifndef WIN32
	if (major >= GTKHACKRC_VER_MAJOR)
	    write_verspecific_file = TRUE;
#endif
	return PARSE_ERROR;
    }
    return 0;
}

void rc_window_position(GScanner *scanner, GtkHackRcVList *params,
  GtkHackRcValue *value)
{
    if (!gtkhackrc_check_no(scanner, params, "arguments", 1, 1) ||
      !gtkhackrc_check_type(scanner, params->values[0], "argument 1",
        PARSE_VALUE_TYPE_STRING) ||
      !gtkhackrc_check_type(scanner, value, "value", PARSE_VALUE_TYPE_RECORD) ||
      !gtkhackrc_check_no(scanner, &value->u.record, "fields in value", 2, 2) ||
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
    if (!gtkhackrc_check_no(scanner, params, "arguments", 1, 1) ||
      !gtkhackrc_check_type(scanner, params->values[0], "argument 1",
        PARSE_VALUE_TYPE_STRING) ||
      !gtkhackrc_check_type(scanner, value, "value", PARSE_VALUE_TYPE_RECORD) ||
      !gtkhackrc_check_no(scanner, &value->u.record, "fields in value", 2, 2) ||
      !gtkhackrc_check_type(scanner, value->u.record.values[0], "width",
        PARSE_VALUE_TYPE_NUMBER) ||
      !gtkhackrc_check_type(scanner, value->u.record.values[1], "height",
        PARSE_VALUE_TYPE_NUMBER))
	return;
    nh_session_set_geometry(params->values[0]->u.string, -1, -1,
      value->u.record.values[0]->u.number, value->u.record.values[1]->u.number);
}

void rc_map_font(GScanner *scanner, GtkHackRcValue *value)
{
    if (!gtkhackrc_check_type(scanner, value, "value", PARSE_VALUE_TYPE_STRING))
	return;
    nh_set_map_font(value->u.string);
}

void rc_map_clip_dist2(GScanner *scanner, GtkHackRcValue *value)
{
    if (!gtkhackrc_check_type(scanner, value, "value", PARSE_VALUE_TYPE_NUMBER))
	return;
    map_clip_dist2 = value->u.number;
    nh_map_check_visibility();
}

void rc_radar(GScanner *scanner, GtkHackRcValue *value)
{
    if (!gtkhackrc_check_type(scanner, value, "value", PARSE_VALUE_TYPE_NUMBER))
	return;
    nh_radar_set_use(value->u.number);
}

#ifdef GTKHACK
void rc_connections(GScanner *scanner, GtkHackRcValue *value)
{
    int i, j, n, nflags;
    unsigned long flags;
    GtkHackRcValue *con, *flag;
    if (!gtkhackrc_check_type(scanner, value, "value", PARSE_VALUE_TYPE_VECTOR))
	return;
    n = value->u.vector.n_values;
    for(i = 0; i < n; i++) {
	con = value->u.vector.values[i];
	if (gtkhackrc_check_type(scanner, con, "connection",
	    PARSE_VALUE_TYPE_RECORD) &&
	  gtkhackrc_check_no(scanner, &con->u.record, "fields in connection",
	    3, 4) &&
	  gtkhackrc_check_type(scanner, con->u.record.values[0], "name",
	    PARSE_VALUE_TYPE_STRING) &&
	  gtkhackrc_check_type(scanner, con->u.record.values[1], "scheme",
	    PARSE_VALUE_TYPE_STRING) &&
	  gtkhackrc_check_type(scanner, con->u.record.values[2], "address",
	    PARSE_VALUE_TYPE_STRING)) {
	    if (con->u.record.n_values < 4 || gtkhackrc_check_type(scanner,
	      con->u.record.values[3], "flags", PARSE_VALUE_TYPE_VECTOR)) {
		flags = 0;
		if (con->u.record.n_values >= 4) {
		    nflags = con->u.record.values[3]->u.vector.n_values;
		    for(j = 0; j < nflags; j++) {
			flag = con->u.record.values[3]->u.vector.values[j];
			if (gtkhackrc_check_type(scanner, flag, "flag",
			  PARSE_VALUE_TYPE_STRING)) {
			    if (!strcmp(flag->u.string, "synchronous"))
				flags |= PROXY_CLNT_SYNCHRONOUS;
			    else if (!strcmp(flag->u.string, "logged"))
				flags |= PROXY_CLNT_LOGGED;
			}
		    }
		}
		GTK_connection_add(con->u.record.values[0]->u.string,
		  con->u.record.values[1]->u.string,
		  con->u.record.values[2]->u.string, flags);
	    }
	}
    }
}

void rc_default_connection(GScanner *scanner, GtkHackRcValue *value)
{
    if (!gtkhackrc_check_type(scanner, value, "value", PARSE_VALUE_TYPE_STRING))
	return;
    GTK_connection_set_default(value->u.string);
}
#endif

#ifdef GTK_PROXY
void rc_proxy_cachedir(GScanner *scanner, GtkHackRcValue *value)
{
    if (!gtkhackrc_check_type(scanner, value, "value", PARSE_VALUE_TYPE_STRING))
	return;
    nh_proxy_cache_set_dir(value->u.string);
}
#endif

void rc_help_font(GScanner *scanner, GtkHackRcValue *value)
{
    if (!gtkhackrc_check_type(scanner, value, "value", PARSE_VALUE_TYPE_STRING))
	return;
    nh_set_help_font(value->u.string);
}
