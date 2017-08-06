/*	SCCS Id: @(#)func_tab.h 3.4	1992/04/03	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef FUNC_TAB_H
#define FUNC_TAB_H

struct ext_func_tab {
	const char *ef_txt, *ef_desc;
	int (*ef_funct)(void);
	boolean	can_if_buried;
	boolean autocomplete;

	/* for interrupted repeats; e.g.,
	 * "searching" => "You stop searching." */
	const char *f_text;
};

/* Tells what extended command to do on a command key press */
struct key_tab {
	struct ext_func_tab * bind_cmd;
};

/* Linked list of bindings; see "crappy hack" in cmd.c */
struct binding_list_tab {
	char key;       /* ascii code for key */
	char* extcmd;   /* extended command key is going to be rebound to */
	struct binding_list_tab * next;
};

extern struct ext_func_tab extcmdlist[];

/*WAC for the menus */
struct menu_tab {
        char m_char;
	boolean	can_if_buried;
        int (*m_funct)(void);
        const char *m_item;
        const char *m_text;
};

struct menu_list {
        const char *m_header;
        const char *m_parent;
        const struct menu_tab *m_menu;
};

#endif /* FUNC_TAB_H */
