/*	SCCS Id: @(#)func_tab.h 3.4	1992/04/03	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef FUNC_TAB_H
#define FUNC_TAB_H

struct func_tab {
	char f_char;
	boolean	can_if_buried;
	int NDECL((*f_funct));
	const char *f_text;
};

struct ext_func_tab {
	const char *ef_txt, *ef_desc;
	int NDECL((*ef_funct));
	boolean	can_if_buried;
};

extern struct ext_func_tab extcmdlist[];

/*WAC for the menus */
struct menu_tab {
        char m_char;
	boolean	can_if_buried;
        int NDECL((*m_funct));
        const char *m_item;
        const char *m_text;
};

struct menu_list {
        const char *m_header;
        const char *m_parent;
        const struct menu_tab *m_menu;
};

#endif /* FUNC_TAB_H */
