/* vim:set cin ft=c sw=4 sts=4 ts=8 et ai cino=Ls\:0t0(0 : -*- mode:c;fill-column:80;tab-width:8;c-basic-offset:4;indent-tabs-mode:nil;c-file-style:"k&r" -*-*/

#include "curses.h"
#include "hack.h"
#include "wincurs.h"
#include "cursinit.h"
#include "patchlevel.h"

#include <ctype.h>

/* Initialization and startup functions for curses interface */

/* Private declarations */

#define NETHACK_CURSES      1
#define SLASHEM_CURSES      2
#define UNNETHACK_CURSES    3
#define SPORKHACK_CURSES    4
#define GRUNTHACK_CURSES    5
#define DNETHACK_CURSES     6

static void set_window_position(int *, int *, int *, int *, int,
                                int *, int *, int *, int *, int,
                                int, int);
static void curses_character_selection(void);

/* array to save initial terminal colors for later restoration */

typedef struct nhrgb_type {
    short r;
    short g;
    short b;
} nhrgb;

struct rolestat {
    enum roletyp typ;
    int desc;
};

nhrgb orig_yellow;
nhrgb orig_white;
nhrgb orig_darkgray;
nhrgb orig_hired;
nhrgb orig_higreen;
nhrgb orig_hiyellow;
nhrgb orig_hiblue;
nhrgb orig_himagenta;
nhrgb orig_hicyan;
nhrgb orig_hiwhite;

/* Banners used for an optional ASCII splash screen */

#define NETHACK_SPLASH_A \
" _   _        _    _    _               _    "

#define NETHACK_SPLASH_B \
"| \\ | |      | |  | |  | |             | |   "

#define NETHACK_SPLASH_C \
"|  \\| |  ___ | |_ | |__| |  __ _   ___ | | __"

#define NETHACK_SPLASH_D \
"| . ` | / _ \\| __||  __  | / _` | / __|| |/ /"

#define NETHACK_SPLASH_E \
"| |\\  ||  __/| |_ | |  | || (_| || (__ |   < "

#define NETHACK_SPLASH_F \
"|_| \\_| \\___| \\__||_|  |_| \\__,_| \\___||_|\\_\\"

#define SLASHEM_SPLASH_A \
" _____  _              _     _  ______  __  __ "

#define SLASHEM_SPLASH_B \
" / ____|| |            | |   ( )|  ____||  \\/  |"

#define SLASHEM_SPLASH_C \
"| (___  | |  __ _  ___ | |__  \\|| |__   | \\  / |"

#define SLASHEM_SPLASH_D \
" \\___ \\ | | / _` |/ __|| '_ \\   |  __|  | |\\/| |"

#define SLASHEM_SPLASH_E \
" ____) || || (_| |\\__ \\| | | |  | |____ | |  | |"

#define SLASHEM_SPLASH_F \
"|_____/ |_| \\__,_||___/|_| |_|  |______||_|  |_|"

#define UNNETHACK_SPLASH_A \
" _    _         _   _        _    _    _               _"

#define UNNETHACK_SPLASH_B \
"| |  | |       | \\ | |      | |  | |  | |             | |"

#define UNNETHACK_SPLASH_C \
"| |  | | _ __  |  \\| |  ___ | |_ | |__| |  __ _   ___ | | __"

#define UNNETHACK_SPLASH_D \
"| |  | || '_ \\ | . ` | / _ \\| __||  __  | / _` | / __|| |/ /"

#define UNNETHACK_SPLASH_E \
"| |__| || | | || |\\  ||  __/| |_ | |  | || (_| || (__ |   <"

#define UNNETHACK_SPLASH_F \
" \\____/ |_| |_||_| \\_| \\___| \\__||_|  |_| \\__,_| \\___||_|\\_\\"

#define SPORKHACK_SPLASH_A \
"  _____                      _     _    _               _    "
#define SPORKHACK_SPLASH_B \
" / ____|                    | |   | |  | |             | |   "
#define SPORKHACK_SPLASH_C \
"| (___   _ __    ___   _ __ | | __| |__| |  __ _   ___ | | __"
#define SPORKHACK_SPLASH_D \
" \\___ \\ | '_ \\  / _ \\ | '__|| |/ /|  __  | / _` | / __|| |/ /"
#define SPORKHACK_SPLASH_E \
" ____) || |_) || (_) || |   |   < | |  | || (_| || (__ |   < "
#define SPORKHACK_SPLASH_F \
"|_____/ | .__/  \\___/ |_|   |_|\\_\\|_|  |_| \\__,_| \\___||_|\\_\\"
#define SPORKHACK_SPLASH_G \
"        | |                                                  "
#define SPORKHACK_SPLASH_H \
"        |_|                                                 "

#define GRUNTHACK_SPLASH_A \
" ______                      _    _    _               _    "
#define GRUNTHACK_SPLASH_B \
"/  ____)                    | |  | |  | |             | |   "
#define GRUNTHACK_SPLASH_C \
"| / ___  _ __  _   _  _ __  | |_ | |__| |  __ _   ___ | |  _"
#define GRUNTHACK_SPLASH_D \
"| | L  \\| '__)| | | || '_ \\ | __)|  __  | / _` | / __)| |/ /"
#define GRUNTHACK_SPLASH_E \
"| l__) || |   | |_| || | | || |_ | |  | || (_| || (__ |   < "
#define GRUNTHACK_SPLASH_F \
"\\______/|_|   \\___,_||_| |_| \\__)|_|  |_| \\__,_| \\___)|_|\\_\\"

#define DNETHACK_SPLASH_A \
"     _  _   _        _    _    _               _    "
#define DNETHACK_SPLASH_B \
"    | || \\ | |      | |  | |  | |             | |   "
#define DNETHACK_SPLASH_C \
"  __| ||  \\| |  ___ | |_ | |__| |  __ _   ___ | | __"
#define DNETHACK_SPLASH_D \
" / _` || . ` | / _ \\| __||  __  | / _` | / __|| |/ /"
#define DNETHACK_SPLASH_E \
"| (_| || |\\  ||  __/| |_ | |  | || (_| || (__ |   < "
#define DNETHACK_SPLASH_F \
" \\__,_||_| \\_| \\___| \\__||_|  |_| \\__,_| \\___||_|\\_\\"


/* win* is size and placement of window to change, x/y/w/h is baseline which can
   decrease depending on alignment of win* in orientation.
   Negative minh/minw: as much as possible, but at least as much as specified. */
static void
set_window_position(int *winx, int *winy, int *winw, int *winh, int orientation,
                    int *x, int *y, int *w, int *h, int border_space,
                    int minh, int minw)
{
    *winw = *w;
    *winh = *h;

    /* Set window height/width */
    if (orientation == ALIGN_TOP || orientation == ALIGN_BOTTOM) {
        if (minh < 0) {
            *winh = (*h - ROWNO - border_space);
            if (-minh > *winh)
                *winh = -minh;
        } else
            *winh = minh;
        *h -= (*winh + border_space);
    } else {
        if (minw < 0) {
            *winw = (*w - COLNO - border_space);
            if (-minw > *winw)
                *winw = -minw;
        } else
            *winw = minw;
        *w -= (*winw + border_space);
    }

    *winx = *w + border_space + *x;
    *winy = *h + border_space + *y;

    /* Set window position */
    if (orientation != ALIGN_RIGHT) {
        *winx = *x;
        if (orientation == ALIGN_LEFT)
            *x += *winw + border_space;
    }
    if (orientation != ALIGN_BOTTOM) {
        *winy = *y;
        if (orientation == ALIGN_TOP)
            *y += *winh + border_space;
    }
}

/* Create the "main" nonvolitile windows used by nethack */

void
curses_create_main_windows()
{
    int min_message_height = 1;
    int message_orientation = 0;
    int status_orientation = 0;
    int border_space = 0;
    int hspace = term_cols - 80;
    boolean borders = FALSE;

    switch (iflags.wc2_windowborders) {
    case 1:                     /* On */
        borders = TRUE;
        break;
    case 2:                     /* Off */
        borders = FALSE;
        break;
    case 3:                     /* Auto */
        if ((term_cols > 81) && (term_rows > 25)) {
            borders = TRUE;
        }
        break;
    default:
        borders = FALSE;
    }


    if (borders) {
        border_space = 2;
        hspace -= border_space;
    }

    if ((term_cols - border_space) < COLNO) {
        min_message_height++;
    }

    /* Determine status window orientation */
    if (!iflags.wc_align_status || (iflags.wc_align_status == ALIGN_TOP)
        || (iflags.wc_align_status == ALIGN_BOTTOM)) {
        if (!iflags.wc_align_status) {
            iflags.wc_align_status = ALIGN_BOTTOM;
        }
        status_orientation = iflags.wc_align_status;
    } else {                    /* left or right alignment */

        /* Max space for player name and title horizontally */
        if ((hspace >= 26) && (term_rows >= 24)) {
            status_orientation = iflags.wc_align_status;
            hspace -= (26 + border_space);
        } else {
            status_orientation = ALIGN_BOTTOM;
        }
    }

    /* Determine message window orientation */
    if (!iflags.wc_align_message || (iflags.wc_align_message == ALIGN_TOP)
        || (iflags.wc_align_message == ALIGN_BOTTOM)) {
        if (!iflags.wc_align_message) {
            iflags.wc_align_message = ALIGN_TOP;
        }
        message_orientation = iflags.wc_align_message;
    } else {                    /* left or right alignment */

        if ((hspace - border_space) >= 25) {    /* Arbitrary */
            message_orientation = iflags.wc_align_message;
        } else {
            message_orientation = ALIGN_TOP;
        }
    }

    /* Figure out window positions and placements. Status and message area can be aligned
       based on configuration. The priority alignment-wise is: status > msgarea > game.
       Define everything as taking as much space as possible and shrink/move based on
       alignment positions. */
    int message_x = 0;
    int message_y = 0;
    int status_x = 0;
    int status_y = 0;
    int inv_x = 0;
    int inv_y = 0;
    int map_x = 0;
    int map_y = 0;

    int message_height = 0;
    int message_width = 0;
    int status_height = 0;
    int status_width = 0;
    int inv_height = 0;
    int inv_width = 0;
    int map_height = (term_rows - border_space);
    int map_width = (term_cols - border_space);

    boolean status_vertical = FALSE;
    boolean msg_vertical = FALSE;
    if (status_orientation == ALIGN_LEFT ||
        status_orientation == ALIGN_RIGHT)
        status_vertical = TRUE;
    if (message_orientation == ALIGN_LEFT ||
        message_orientation == ALIGN_RIGHT)
        msg_vertical = TRUE;

    int statusheight = 3;
    /*if (iflags.classic_status)
        statusheight = 2;*/

    /* Vertical windows have priority. Otherwise, priotity is:
       status > inv > msg */
    if (status_vertical)
        set_window_position(&status_x, &status_y, &status_width, &status_height,
                            status_orientation, &map_x, &map_y, &map_width, &map_height,
                            border_space, statusheight, 26);

    if (!restoring && flags.perm_invent && !(youmonst.data && InventoryDoesNotGo && !program_state.gameover)) {
        /* Take up all width unless msgbar is also vertical. */
        int width = -25;
        if (msg_vertical)
            width = 25;

        set_window_position(&inv_x, &inv_y, &inv_width, &inv_height,
                            ALIGN_RIGHT, &map_x, &map_y, &map_width, &map_height,
                            border_space, -1, width);
    }

    if (msg_vertical)
        set_window_position(&message_x, &message_y, &message_width, &message_height,
                            message_orientation, &map_x, &map_y, &map_width, &map_height,
                            border_space, -1, -25);

    /* Now draw horizontal windows */
    if (!status_vertical)
        set_window_position(&status_x, &status_y, &status_width, &status_height,
                            status_orientation, &map_x, &map_y, &map_width, &map_height,
                            border_space, statusheight, 26);

    if (!msg_vertical)
        set_window_position(&message_x, &message_y, &message_width, &message_height,
                            message_orientation, &map_x, &map_y, &map_width, &map_height,
                            border_space, -1, -25);

    if (map_width > COLNO)
        map_width = COLNO;

    if (map_height > ROWNO)
        map_height = ROWNO;

    if (curses_get_nhwin(STATUS_WIN)) {
        curses_del_nhwin(STATUS_WIN);
        curses_del_nhwin(MESSAGE_WIN);
        curses_del_nhwin(MAP_WIN);
        curses_del_nhwin(INV_WIN);

        clear();
    }

    curses_add_nhwin(STATUS_WIN, status_height, status_width, status_y,
                     status_x, status_orientation, borders);

    curses_add_nhwin(MESSAGE_WIN, message_height, message_width, message_y,
                     message_x, message_orientation, borders);

    if (!restoring && flags.perm_invent && !(youmonst.data && InventoryDoesNotGo && !program_state.gameover))
        curses_add_nhwin(INV_WIN, inv_height, inv_width, inv_y, inv_x,
                         ALIGN_RIGHT, borders);

    curses_add_nhwin(MAP_WIN, map_height, map_width, map_y, map_x, 0, borders);

    refresh();

    curses_refresh_nethack_windows();

    if (iflags.window_inited) {
        curses_update_stats();
        if (!restoring && flags.perm_invent && !(youmonst.data && InventoryDoesNotGo && !program_state.gameover))
            curses_update_inventory();
    } else {
        iflags.window_inited = TRUE;
    }
}


/* Initialize curses colors to colors used by NetHack */

void
curses_init_nhcolors()
{
#ifdef TEXTCOLOR
    if (has_colors()) {
        use_default_colors();
        init_pair(1, COLOR_BLACK, -1);
        init_pair(2, COLOR_RED, -1);
        init_pair(3, COLOR_GREEN, -1);
        init_pair(4, COLOR_YELLOW, -1);
        init_pair(5, COLOR_BLUE, -1);
        init_pair(6, COLOR_MAGENTA, -1);
        init_pair(7, COLOR_CYAN, -1);
        init_pair(8, -1, -1);

        {
            int i;

            int clr_remap[16] = {
                COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_YELLOW,
                COLOR_BLUE,
                COLOR_MAGENTA, COLOR_CYAN, -1, COLOR_WHITE,
                COLOR_RED + 8, COLOR_GREEN + 8, COLOR_YELLOW + 8,
                COLOR_BLUE + 8,
                COLOR_MAGENTA + 8, COLOR_CYAN + 8, COLOR_WHITE + 8
            };

            for (i = 0; i < (COLORS >= 16 ? 16 : 8); i++) {
                init_pair(17 + (i * 2) + 0, clr_remap[i], COLOR_RED);
                init_pair(17 + (i * 2) + 1, clr_remap[i], COLOR_BLUE);
                init_pair(17 + (i * 2) + 2, clr_remap[i], COLOR_YELLOW);
                init_pair(17 + (i * 2) + 3, clr_remap[i], COLOR_GREEN);
                init_pair(17 + (i * 2) + 4, clr_remap[i], COLOR_CYAN);
            }

            boolean hicolor = FALSE;
            if (COLORS >= 16)
                hicolor = TRUE;

            /* Work around the crazy definitions above for more background colors... */
            for (i = 0; i < (COLORS >= 16 ? 16 : 8); i++) {
                init_pair((hicolor ? 49 : 9) + i, clr_remap[i], COLOR_GREEN);
                init_pair((hicolor ? 65 : 33) + i, clr_remap[i], COLOR_YELLOW);
                init_pair((hicolor ? 81 : 41) + i, clr_remap[i], COLOR_MAGENTA);
                init_pair((hicolor ? 97 : 49) + i, clr_remap[i], COLOR_CYAN);
                init_pair((hicolor ? 113 : 57) + i, clr_remap[i], COLOR_WHITE);
            }
        }


        if (COLORS >= 16) {
            init_pair(9, COLOR_WHITE, -1);
            init_pair(10, COLOR_RED + 8, -1);
            init_pair(11, COLOR_GREEN + 8, -1);
            init_pair(12, COLOR_YELLOW + 8, -1);
            init_pair(13, COLOR_BLUE + 8, -1);
            init_pair(14, COLOR_MAGENTA + 8, -1);
            init_pair(15, COLOR_CYAN + 8, -1);
            init_pair(16, COLOR_WHITE + 8, -1);
        }

        if (can_change_color() && iflags.should_change_color) {
            /* Preserve initial terminal colors */
            color_content(COLOR_YELLOW, &orig_yellow.r, &orig_yellow.g,
                          &orig_yellow.b);
            color_content(COLOR_WHITE, &orig_white.r, &orig_white.g,
                          &orig_white.b);

            /* Set colors to appear as NetHack expects */
            init_color(COLOR_YELLOW, 500, 300, 0);
            init_color(COLOR_WHITE, 600, 600, 600);
            if (COLORS >= 16) {
                /* Preserve initial terminal colors */
                color_content(COLOR_RED + 8, &orig_hired.r,
                              &orig_hired.g, &orig_hired.b);
                color_content(COLOR_GREEN + 8, &orig_higreen.r,
                              &orig_higreen.g, &orig_higreen.b);
                color_content(COLOR_YELLOW + 8, &orig_hiyellow.r,
                              &orig_hiyellow.g, &orig_hiyellow.b);
                color_content(COLOR_BLUE + 8, &orig_hiblue.r,
                              &orig_hiblue.g, &orig_hiblue.b);
                color_content(COLOR_MAGENTA + 8, &orig_himagenta.r,
                              &orig_himagenta.g, &orig_himagenta.b);
                color_content(COLOR_CYAN + 8, &orig_hicyan.r,
                              &orig_hicyan.g, &orig_hicyan.b);
                color_content(COLOR_WHITE + 8, &orig_hiwhite.r,
                              &orig_hiwhite.g, &orig_hiwhite.b);

                /* Set colors to appear as NetHack expects */
                init_color(COLOR_RED + 8, 1000, 500, 0);
                init_color(COLOR_GREEN + 8, 0, 1000, 0);
                init_color(COLOR_YELLOW + 8, 1000, 1000, 0);
                init_color(COLOR_BLUE + 8, 0, 0, 1000);
                init_color(COLOR_MAGENTA + 8, 1000, 0, 1000);
                init_color(COLOR_CYAN + 8, 0, 1000, 1000);
                init_color(COLOR_WHITE + 8, 1000, 1000, 1000);
# ifdef USE_DARKGRAY
                if (COLORS > 16) {
                    color_content(CURSES_DARK_GRAY, &orig_darkgray.r,
                                  &orig_darkgray.g, &orig_darkgray.b);
                    init_color(CURSES_DARK_GRAY, 300, 300, 300);
                    /* just override black colorpair entry here */
                    init_pair(1, CURSES_DARK_GRAY, -1);
                }
# endif
            } else {
                /* Set flag to use bold for bright colors */
            }
        }
    }
#endif
}

/* Returns the index into roles[]/races[]/genders[]/aligns[]
   by permons (role/race) or by allow (gend/align).
   Returns -1 on failure. */
static int
curses_get_roleid_by_desc(enum roletyp typ, int desc)
{
    int i;

    switch (typ) {
    case CR_ROLE:
        for (i = 0; roles[i].name.m; i++)
            if (roles[i].malenum == desc ||
                roles[i].femalenum == desc)
                return i;
        break;
    case CR_RACE:
        for (i = 0; races[i].noun; i++)
            if (races[i].malenum == desc ||
                races[i].femalenum == desc)
                return i;
        break;
    case CR_GEND:
        for (i = 0; i < ROLE_GENDERS; i++)
            if (genders[i].allow == desc)
                return i;
        break;
    case CR_ALIGN:
        for (i = 0; i < ROLE_ALIGNS; i++)
            if (aligns[i].allow == desc)
                return i;
        break;
    default:
        break;
    }

    return -1;
}

/* Sets the selected rolestat to the given type. If by_id is
   TRUE, sets desc directly. If not, look up the relevant
   role/race/gender/alignment struct and set by permonst
   (role/race) or by allow (gend/align). */
static void
curses_set_rolestat(struct rolestat *selection, int let,
                    enum roletyp typ, int desc, boolean by_id)
{
    if (by_id)
        selection[let].desc = desc;
    else {
        int id;
        id = curses_get_roleid_by_desc(typ, desc);
        if (id == -1)
            return; /* invalid */

        selection[let].desc = id;
    }
 
    selection[let].typ = typ;
}

/* Returns TRUE if the rolestate is the given type. */
static boolean
curses_rolestat_is(struct rolestat *selection, int let, enum roletyp typ, int desc, boolean by_id)
{
    int id = desc;
    if (!by_id)
        id = curses_get_roleid_by_desc(typ, desc);
    if (selection[let].typ == typ && selection[let].desc == id)
        return TRUE;

    return FALSE;
}

/* Returns TRUE if the given accelerator is available. */
static boolean
curses_rolestat_free(struct rolestat *selection, int let)
{
    return !!(selection[let].typ == CR_NONE);
}

/* Returns TRUE if the given accelerator is selected. */
static boolean
curses_rolestat_selected(struct rolestat *selection, int let)
{
    switch (selection[let].typ) {
    case CR_ROLE:
        return !!(flags.initrole == selection[let].desc);
    case CR_RACE:
        return !!(flags.initrace == selection[let].desc);
    case CR_GEND:
        return !!(flags.initgend == selection[let].desc);
    case CR_ALIGN:
        return !!(flags.initalign == selection[let].desc);
    default:
        break;
    }

    return FALSE;
}

/* Returns TRUE if the given accelerator is valid given
   the current role/race/gender/alignment selection. */
static boolean
curses_rolestat_valid(struct rolestat *selection, int let)
{
    switch (selection[let].typ) {
    case CR_ROLE:
        return ok_role(selection[let].desc, flags.initrace,
                       flags.initgend, flags.initalign);
    case CR_RACE:
        return ok_race(flags.initrole, selection[let].desc,
                       flags.initgend, flags.initalign);
    case CR_GEND:
        return ok_gend(flags.initrole, flags.initrace,
                       selection[let].desc, flags.initalign);
    case CR_ALIGN:
        return ok_align(flags.initrole, flags.initrace,
                       flags.initgend, selection[let].desc);
    default:
        break;
    }

    /* otherwise, return FALSE if *any* of our current
       selections are invalid. */
    if (ok_role(flags.initrole, flags.initrace,
                flags.initgend, flags.initalign) &&
        ok_race(flags.initrole, flags.initrace,
                flags.initgend, flags.initalign) &&
        ok_gend(flags.initrole, flags.initrace,
                flags.initgend, flags.initalign) &&
        ok_align(flags.initrole, flags.initrace,
                flags.initgend, flags.initalign))
        return TRUE;

    return FALSE;
}

/* Returns a string description for a role/race/gender/align
   by accelerator. */
static const char *
curses_rolestat_str(struct rolestat *selection, int let)
{
    int desc = selection[let].desc;

    switch (selection[let].typ) {
    case CR_ROLE:
        return roles[desc].name.m;
    case CR_RACE:
        return races[desc].noun;
    case CR_GEND:
        return genders[desc].adj;
    case CR_ALIGN:
        return aligns[desc].adj;
    case CR_SPECIAL:
        return "play!"; /* the "play" accelerator */
    default:
        return "???";
    }

}

/* Pick a random role/etc. If respect_config is TRUE, only randomize
   missing entries. */
static void
curses_random_role(boolean respect_config)
{
    if (!respect_config) {
        flags.initrole = ROLE_RANDOM;
        flags.initrace = ROLE_RANDOM;
        flags.initgend = ROLE_RANDOM;
        flags.initalign = ROLE_RANDOM;
    }

    /* Free up invalid choices. */
    if (!ok_align(flags.initrole, flags.initrace, flags.initgend,
                  flags.initalign))
        flags.initalign = ROLE_RANDOM;
    if (!ok_gend(flags.initrole, flags.initrace, flags.initgend,
                 flags.initalign))
        flags.initgend = ROLE_RANDOM;
    if (!ok_race(flags.initrole, flags.initrace, flags.initgend,
                 flags.initalign))
        flags.initrace = ROLE_RANDOM;
    if (!ok_role(flags.initrole, flags.initrace, flags.initgend,
                 flags.initalign))
        flags.initrole = ROLE_RANDOM;

    flags.initrole = pick_role(flags.initrole, flags.initrace,
                               flags.initgend, flags.initalign);
    flags.initrace = pick_race(flags.initrole, flags.initrace,
                               flags.initgend, flags.initalign);
    flags.initgend = pick_gend(flags.initrole, flags.initrace,
                               flags.initgend, flags.initalign);
    flags.initalign = pick_align(flags.initrole, flags.initrace,
                                 flags.initgend, flags.initalign);

    /* Don't let the role selector crash if this resulted in an invalid selection.
       This should never happen unless the game engine is doing something stupid... */
    if (flags.initrole < 0)
        flags.initrole = 0;
    if (flags.initrace < 0)
        flags.initrace = 0;
    if (flags.initgend < 0)
        flags.initgend = 0;
    if (flags.initalign < 0)
        flags.initalign = 0;
}

static int
role_accel(WINDOW *win, int y, int x, struct rolestat *selection,
           int let)
{
    attr_t attr = curses_color_attr(CLR_GRAY, 0);
    attr_t attrselected = curses_color_attr(CLR_WHITE, 0);
    attr_t attrinvalid = curses_color_attr(CLR_BROWN, 0);
    attr_t attrinvalid_selected = curses_color_attr(CLR_YELLOW, 0);
    boolean selected = curses_rolestat_selected(selection, let);
    boolean invalid = !curses_rolestat_valid(selection, let);
    char selch = '-';
    int ret = 0;
    if (selected) {
        attr = attrselected;
        selch = '+';
        if (invalid) {
            attr = attrinvalid_selected;
            selch = '!';
            ret = 1;
        }
    } else if (invalid)
        attr = attrinvalid;

    /* Create a temporary char so we can make the initial
       character always be in uppercase. */
    char str[BUFSZ];
    strcpy(str, curses_rolestat_str(selection, let));
    str[0] = toupper(str[0]);

    wattron(win, attr);
    mvwprintw(win, y, x, "%c %c %s", let, selch, str);
    wattroff(win, attr);
    return ret;
}

/* Displays and handles the character selection window. */
static void
curses_character_selection(void)
{
    int let;
    int i, j, pass;

    /* First, set up valid choices. */
    struct rolestat selection[256];
    struct rolestat old_selection[256];
    memset(&selection, 0, sizeof selection);

    /* For scrollable, 1 means scrollable but w/o overlapping
       choices, 2+ means scrollable where choice meaning
       depends on current visible selection, in which it will
       hold the amount of letters dedicated for it. */
    int role_scrollable = 0;
    int race_scrollable = 0;
    boolean roles_done;
    boolean races_done;
    char rolestr[BUFSZ];

    /* Reserve qMFLNC */
    curses_set_rolestat(selection, '.', CR_SPECIAL, 0, TRUE);
    curses_set_rolestat(selection, 'q', CR_SPECIAL, 0, TRUE);
    curses_set_rolestat(selection, 'M', CR_GEND,
                        ROLE_MALE, FALSE);
    curses_set_rolestat(selection, 'F', CR_GEND,
                        ROLE_FEMALE, FALSE);
    curses_set_rolestat(selection, 'L', CR_ALIGN,
                        ROLE_LAWFUL, FALSE);
    curses_set_rolestat(selection, 'N', CR_ALIGN,
                        ROLE_NEUTRAL, FALSE);
    curses_set_rolestat(selection, 'C', CR_ALIGN,
                        ROLE_CHAOTIC, FALSE);

    /*
     * Do 4 passes when trying to assign roles/races to letters.
     * 1: Attempt first letter
     * 2: Attempt other letters in the race name
     * 3: Attempt any other free letter
     * 4: Assign leftover letters (lowercase for role, uppercase for race) and create a
     *    scrollable window part.
     */
    memcpy(&old_selection, &selection, sizeof selection);
    pass = 0;
    do {
        pass++;
        roles_done = TRUE;
        if (pass == 4) {
            /* Unreserve our failed unfinished race letter
               reservations */
            memcpy(&selection, &old_selection, sizeof selection);

            /* Reserve all (free) lowercase letters for roles
               and create a scrollable race selection. */
            for (let = 'a'; let <= 'z'; let++) {
                if (curses_rolestat_free(selection, let))
                    role_scrollable++;
            }

            break;
        }

        for (i = 0; roles[i].name.m; i++) {
            for (j = 0; roles[i].name.m[j]; j++) {
                /* Only attempt first letter in pass 1 */
                if (pass == 1 && j)
                    break;

                let = tolower(roles[i].name.m[j]);

                /* Maybe this race has been assigned already. */
                if (curses_rolestat_is(selection, let,
                                       CR_ROLE, i, TRUE))
                    break;

                /* Is the letter free? */
                if (curses_rolestat_free(selection, let)) {
                    curses_set_rolestat(selection, let,
                                        CR_ROLE, i, TRUE);
                    break;
                }

                /* Now try uppercase */
                let = toupper(let);

                if (curses_rolestat_is(selection, let,
                                       CR_ROLE, i, TRUE))
                    break;

                if (curses_rolestat_free(selection, let)) {
                    curses_set_rolestat(selection, let,
                                        CR_ROLE, i, TRUE);
                    break;
                }
            }

            /* For pass 3, try any letter. */
            if (pass == 3 &&
                !curses_rolestat_is(selection, let, CR_ROLE,
                                    i, TRUE)) {
                for (let = 'a'; let <= 'z'; let++) {
                    if (curses_rolestat_free(selection,
                                             let)) {
                        curses_set_rolestat(selection, let,
                                            CR_ROLE, i, TRUE);
                        break;
                    }
                }
            }

            /* If we failed to find a letter for any role in
               this pass, mark races as unfinished. */
            if (!curses_rolestat_is(selection, let, CR_ROLE,
                                    i, TRUE))
                roles_done = FALSE;
        }
    } while (!roles_done);

    /* Now do the same thing with races */
    memcpy(&old_selection, &selection, sizeof selection);
    pass = 0;
    do {
        pass++;
        races_done = TRUE;
        if (pass == 4) {
            memcpy(&selection, &old_selection, sizeof selection);

            for (let = 'A'; let <= 'Z'; let++) {
                if (curses_rolestat_free(selection, let))
                    race_scrollable++;
            }

            break;
        }

        for (i = 0; races[i].noun; i++) {
            for (j = 0; races[i].noun[j]; j++) {
                if (pass == 1 && j)
                    break;

                let = toupper(races[i].noun[j]);

                if (curses_rolestat_is(selection, let,
                                       CR_RACE, i, TRUE))
                    break;
 
                if (curses_rolestat_free(selection, let)) {
                    curses_set_rolestat(selection, let,
                                        CR_RACE, i, TRUE);
                    break;
                }

                let = tolower(let);

                if (curses_rolestat_is(selection, let,
                                       CR_RACE, i, TRUE))
                    break;

                if (curses_rolestat_free(selection, let)) {
                    curses_set_rolestat(selection, let,
                                        CR_RACE, i, TRUE);
                    break;
                }
            }

            if (pass == 3 &&
                !curses_rolestat_is(selection, let, CR_RACE,
                                    i, TRUE)) {
                for (let = 'A'; let <= 'Z'; let++) {
                    if (curses_rolestat_free(selection,
                                             let)) {
                        curses_set_rolestat(selection, let,
                                            CR_RACE, i, TRUE);
                        break;
                    }
                }
            }

            if (!curses_rolestat_is(selection, let, CR_RACE,
                                    i, TRUE))
                races_done = FALSE;
        }
    } while (!races_done);

    /* Done with letter assignments. Now figure out window
       dimensions and positioning of things. */
    int width = 0;
    int height = 14; /* fits gender/alignment/quit/play */

    int longest_role = 0;
    int longest_race = 0;
    int role_height = 0;
    int race_height = 0;
    int role_total = 0;
    int race_total = 0;
    int col2_start = 0;
    int col3_start = 0;
    int scroll_offset = 0;
    int any_invalid = 0;
    WINDOW *win, *bwin;
    int winx, winy;
    int x, y;

    for (i = 0; roles[i].name.m; i++)
        if (longest_role < strlen(roles[i].name.m))
            longest_role = strlen(roles[i].name.m);
    role_total = role_height = i;
    if (role_scrollable)
        role_height = role_scrollable;
    if (height < (role_height + 1))
        height = (role_height + 1); /* includes header */

    for (i = 0; races[i].noun; i++)
        if (longest_race < strlen(races[i].noun))
            longest_race = strlen(races[i].noun);
    race_total = race_height = i;
    if (race_scrollable)
        race_height = race_scrollable;
    if (height < (race_height + 1))
        height = (race_height + 1);

    if (role_scrollable && race_scrollable)
        height = min(role_scrollable + 1, race_scrollable + 1);

    height += 2; /* for the role/race/... descriptor */

    width += longest_role;
    width += longest_race;
    width += 8; /* 4*2 for accelerators ("a - ") */
    width += 4; /* 2*2 for spacing between columns */
    width += strlen("    Alignment"); /* 3rd column width */

    col2_start = longest_role + 6;
    col3_start = col2_start + longest_race + 6;

    /* If the terminal is unreasonably small, just bail out.
       (80x24 handles this fine, as do lower resolutions, up
       to a point). */
    if (width >= term_cols || term_rows < 16) {
        curses_raw_print("Terminal too small to fit the role"
                         "selection menu; picking randomly!");
        return;
    }

    /* If height exceeds term_rows, role, race or both needs
       to be made scrollable if it isn't already. */
    if (height + 2 > term_rows) {
        if ((role_height + 5) > term_rows) {
            role_height = (term_rows - 5);
            if (!role_scrollable)
                role_scrollable = 1;
        }
        if ((race_height + 5) > term_rows) {
            race_height = (term_rows - 5);
            if (!race_scrollable)
                race_scrollable = 1;
        }

        height = term_rows - 2;
    }

    memcpy(&old_selection, &selection, sizeof selection);
    while (TRUE) {
        any_invalid = 0;

        /* Create the window border as its seperate window */
        bwin = curses_create_window(width, height, -1);
        wrefresh(bwin);
        getbegyx(bwin, winy, winx);
        werase(bwin);
        delwin(bwin);
        win = newwin(height, width, winy + 1, winx + 1);

        /* 1st column (roles) */
        x = 0;
        y = 0;
        mvwaddstr(win, y++, x, "    Role");
        let = 'z';
        for (i = 0; i < role_total; i++) {
            /* Find letter for this role. */
            if (role_scrollable <= 1) {
                let = 'a';
                while (TRUE) {
                    if (curses_rolestat_is(selection, let,
                                           CR_ROLE, i, TRUE))
                        break;
                    if (let == 'z')
                        let = 'A';
                    else if (let == 'Z')
                        panic("No letter for role %d!", i);
                    else
                        let++;
                }
            } else {
                do {
                    if (let == 'z')
                        let = 'a';
                    else
                        let++;
                } while (!curses_rolestat_free(old_selection,
                                               let));

                /* (Temporarily) assign this role to this
                   letter. */
                curses_set_rolestat(selection, let, CR_ROLE,
                                    i, TRUE);
            }

            if (role_scrollable && scroll_offset > i)
                continue;

            any_invalid |= role_accel(win, y++, x,
                                      selection, let);

            if (y == height - 2)
                break;
        }

        /* 2st column (races) */
        x = col2_start;
        y = 0;
        mvwaddstr(win, y++, x, "    Race");
        let = 'Z';
        for (i = 0; i < race_total; i++) {
            if (race_scrollable <= 1) {
                let = 'A';
                while (TRUE) {
                    if (curses_rolestat_is(selection, let,
                                           CR_RACE, i, TRUE))
                        break;
                    if (let == 'Z')
                        let = 'a';
                    else if (let == 'z')
                        panic("No letter for race %d!", i);
                    else
                        let++;
                }
            } else {
                do {
                    if (let == 'Z')
                        let = 'A';
                    else
                        let++;
                } while (!curses_rolestat_free(old_selection,
                                               let));

                curses_set_rolestat(selection, let, CR_RACE,
                                    i, TRUE);
            }

            if (race_scrollable && scroll_offset > i)
                continue;

            any_invalid |= role_accel(win, y++, x,
                                      selection, let);

            if (y == height - 2)
                break;
        }

        /* Potentially show scroll info */
        int scroll_height = height - 3;
        int scroll_total = max(role_total, race_total);
        int curpage = (scroll_offset / scroll_height + 1);
        int lastpage = ((scroll_total - 1) / scroll_height + 1);
        boolean at_top = !!(curpage == 1);
        boolean at_bottom = !!(curpage == lastpage);
        if (lastpage > 1) {
            mvwprintw(win, height - 2, 0,
                      "%s (Page %d of %d) %s",
                      !at_top ? "<=" : "  ", curpage, lastpage,
                      !at_bottom ? "=>" : "  ");
        }

        /* 3rd column */
        x = col3_start;
        y = 0;
        mvwaddstr(win, y++, x, "    Gender");
        any_invalid |= role_accel(win, y++, x, selection, 'M');
        any_invalid |= role_accel(win, y++, x, selection, 'F');
        mvwaddstr(win, y++, x, "");
        mvwaddstr(win, y++, x, "    Alignment");
        any_invalid |= role_accel(win, y++, x, selection, 'L');
        any_invalid |= role_accel(win, y++, x, selection, 'N');
        any_invalid |= role_accel(win, y++, x, selection, 'C');
        mvwaddstr(win, y++, x, "");
        mvwaddstr(win, y++, x, "");
        mvwaddstr(win, y++, x, "q - quit");
        mvwaddstr(win, y++, x, "* - random");
        any_invalid |= role_accel(win, y++, x, selection, '.');

        /* Describe the current selection, or if it is
           invalid. */
        if (any_invalid)
            mvwaddstr(win, height - 1, 0,
                      "One or several choices are invalid.");
        else {
            sprintf(rolestr, "%s %s %s %s",
                    aligns[flags.initalign].adj,
                    genders[flags.initgend].adj,
                    races[flags.initrace].adj,
                    flags.initgend == ROLE_FEMALE &&
                    roles[flags.initrole].name.f ?
                    roles[flags.initrole].name.f :
                    roles[flags.initrole].name.m);
            mvwaddstr(win, height - 1, 0, rolestr);
        }

        /* Now do the input logic. */
        let = wgetch(win);
        if (let == 'q') {
            clearlocks();
            curses_bail(0);
            return;
        } else if ((let == '\r' || let == '\n' || let == '\0' ||
                    let == '.') && !any_invalid) {
            curses_destroy_win(win);
            return;
        }

        int typ = selection[let].typ;
        int desc = selection[let].desc;
        if (let == '*')
            curses_random_role(FALSE);
        else if (let == KEY_PPAGE || let == '<') {
            if (curpage > 1)
                scroll_offset -= scroll_height;
        } else if (let == KEY_NPAGE || let == '>') {
            if (curpage < lastpage)
                scroll_offset += scroll_height;
        } else {
            switch (typ) {
            case CR_ROLE:
                flags.initrole = desc;
                break;
            case CR_RACE:
                flags.initrace = desc;
                break;
            case CR_GEND:
                flags.initgend = desc;
                break;
            case CR_ALIGN:
                flags.initalign = desc;
                break;
            default:
                break;
            }
        }

        curses_destroy_win(win);
    }
}

/* Main menu. */
void
curses_choose_character(void)
{
    winid win;
    anything any;
    int n;
    menu_item *selected = 0;
    int quitcnt = 3;

    /* Prepare a random starting selection for 'y' or what 'n'
       begins with. TRUE because we want to respect player
       options here. */
    curses_random_role(TRUE);
    refresh();

    /* Create "main" menu. */
    win = curses_get_wid(NHW_MENU);
    curses_create_nhmenu(win);
    any.a_int = 1;
    curses_add_menu(win, NO_GLYPH, &any, 'y', 0, ATR_NONE,
                    "random start",
                    MENU_UNSELECTED);
    any.a_int = 2;
    curses_add_menu(win, NO_GLYPH, &any, 'n', 0, ATR_NONE,
                    "let me choose", MENU_UNSELECTED);
    any.a_int = 3;
#ifdef TUTORIAL_MODE
    curses_add_menu(win, NO_GLYPH, &any, 't', 0, ATR_NONE,
                    "tutorial mode", MENU_UNSELECTED);
    quitcnt = 4;
#endif
    any.a_int = quitcnt;
    curses_add_menu(win, NO_GLYPH, &any, 'q', 0, ATR_NONE,
                    "quit", MENU_UNSELECTED);
    curses_end_menu(win, "How do you want to start playing?");
    n = curses_display_nhmenu(win, PICK_ONE, &selected, TRUE);
    destroy_nhwindow(win);

    int result = 1;
    if (!selected) {
		result = 1;
    } else if (!(selected[0].item.a_int)) {
		result = 1;
    } else {
		result = selected[0].item.a_int;
    		free(selected);
    }
    selected = 0;
    refresh();

    /* Player choose to quit. */
    if (n != 1 || result == quitcnt) {
        clearlocks();
        curses_bail(0);
    }

    /* Other choices */
    switch (result) {
    case 1:
        return; /* we already picked a valid random choice. */
    case 2: /* let the player choose */
        curses_character_selection();
        return;
#ifdef TUTORIAL_MODE
    case 3:
        win = curses_get_wid(NHW_MENU);
        curses_create_nhmenu(win);
        any.a_int = 1;
        curses_add_menu(win, NO_GLYPH, &any, 'v', 0, ATR_NONE,
                        "lawful female dwarf Valkyrie (uses melee and thrown weapons)",
                        MENU_UNSELECTED);
        any.a_int = 2;
        curses_add_menu(win, NO_GLYPH, &any, 'w', 0, ATR_NONE,
                        "chaotic male elf Wizard (relies mostly on spells)",
                        MENU_UNSELECTED);
        any.a_int = 3;
        curses_add_menu(win, NO_GLYPH, &any, 'R', 0, ATR_NONE,
                        "neutral female human Ranger (good with ranged combat)",
                        MENU_UNSELECTED);
        any.a_int = 4;
        curses_add_menu(win, NO_GLYPH, &any, 'q', 0, ATR_NONE,
                        "quit", MENU_UNSELECTED);
        curses_end_menu(win, "What character do you want to try?");
        n = curses_display_nhmenu(win, PICK_ONE, &selected, TRUE);
        destroy_nhwindow(win);
        if (n != 1 || selected[0].item.a_int == 4) {
            clearlocks();
            curses_bail(0);
        }
        switch (selected[0].item.a_int) {
        case 1:
            flags.initrole = str2role("Valkyrie");
            flags.initrace = str2race("dwarf");
            flags.initgend = str2gend("female");
            flags.initalign = str2align("lawful");
            break;
        case 2:
            flags.initrole = str2role("Wizard");
            flags.initrace = str2race("elf");
            flags.initgend = str2gend("male");
            flags.initalign = str2align("chaotic");
            break;
        case 3:
            flags.initrole = str2role("Ranger");
            flags.initrace = str2race("human");
            flags.initgend = str2gend("female");
            flags.initalign = str2align("neutral");
            break;
        default:
            panic("Impossible menu selection");
            break;
        }
        free((void *) selected);
        selected = 0;
        flags.tutorial = 1;
        return;
#endif
    default:
        panic("Impossible menu selection");
        /* NOTREACHED */ return;
    }
}


/* Prompt user for character race, role, alignment, or gender */

int
curses_character_dialog(const char **choices, const char *prompt)
{
    int count, count2, ret, curletter;
    char used_letters[52];
    anything identifier;
    menu_item *selected = NULL;
    winid wid = curses_get_wid(NHW_MENU);

    identifier.a_void = 0;
    curses_start_menu(wid);

    for (count = 0; choices[count]; count++) {
        curletter = tolower(choices[count][0]);
        for (count2 = 0; count2 < count; count2++) {
            if (curletter == used_letters[count2]) {
                curletter = toupper(curletter);
            }
        }

        identifier.a_int = (count + 1); /* Must be non-zero */
        curses_add_menu(wid, NO_GLYPH, &identifier, curletter, 0,
                        A_NORMAL, choices[count], FALSE);
        used_letters[count] = curletter;
    }

    /* Random Selection */
    identifier.a_int = ROLE_RANDOM;
    curses_add_menu(wid, NO_GLYPH, &identifier, '*', 0, A_NORMAL, "Random",
                    FALSE);

    /* Quit prompt */
    identifier.a_int = ROLE_NONE;
    curses_add_menu(wid, NO_GLYPH, &identifier, 'q', 0, A_NORMAL, "Quit",
                    FALSE);
    curses_end_menu(wid, prompt);
    ret = curses_select_menu(wid, PICK_ONE, &selected);
    if (ret == 1) {
        ret = (selected->item.a_int);
    } else {                    /* Cancelled selection */

        ret = ROLE_NONE;
    }

    if (ret > 0) {
        ret--;
    }

    free(selected);
    return ret;
}


/* Initialize and display options appropriately */

void
curses_init_options()
{
    set_wc_option_mod_status(WC_ALIGN_MESSAGE | WC_ALIGN_STATUS | WC_COLOR |
                             WC_HILITE_PET | WC_POPUP_DIALOG, SET_IN_GAME);

    set_wc2_option_mod_status(WC2_GUICOLOR, SET_IN_GAME);

    /* Remove a few options that are irrelevant to this windowport */
    set_option_mod_status("DECgraphics", SET_IN_FILE);
    set_option_mod_status("eight_bit_tty", SET_IN_FILE);

    /* Add those that are */
    set_option_mod_status("classic_status", SET_IN_GAME);

    /* Make sure that DECgraphics is not set to true via the config
       file, as this will cause display issues.  We can't disable it in
       options.c in case the game is compiled with both tty and curses. */
    if (iflags.DECgraphics) {
        switch_graphics(CURS_GRAPHICS);
    }
#ifdef PDCURSES
    /* PDCurses for SDL, win32 and OS/2 has the ability to set the
       terminal size programatically.  If the user does not specify a
       size in the config file, we will set it to a nice big 110x32 to
       take advantage of some of the nice features of this windowport. */
    if (iflags.wc2_term_cols == 0) {
        iflags.wc2_term_cols = 110;
    }

    if (iflags.wc2_term_rows == 0) {
        iflags.wc2_term_rows = 32;
    }

    resize_term(iflags.wc2_term_rows, iflags.wc2_term_cols);
    getmaxyx(base_term, term_rows, term_cols);

    /* This is needed for an odd bug with PDCurses-SDL */
    switch_graphics(ASCII_GRAPHICS);
    if (iflags.IBMgraphics) {
        switch_graphics(IBM_GRAPHICS);
    } else if (iflags.cursesgraphics) {
        switch_graphics(CURS_GRAPHICS);
    } else {
        switch_graphics(ASCII_GRAPHICS);
    }
#endif /* PDCURSES */
    if (!iflags.wc2_windowborders) {
        iflags.wc2_windowborders = 3;   /* Set to auto if not specified */
    }

    if (!iflags.wc2_petattr) {
        iflags.wc2_petattr = A_REVERSE;
    } else {                    /* Pet attribute specified, so hilite_pet should be true */

        iflags.hilite_pet = TRUE;
    }

#ifdef NCURSES_MOUSE_VERSION
    if (iflags.wc_mouse_support) {
        mousemask(BUTTON1_CLICKED, NULL);
    }
#endif
}


/* Display an ASCII splash screen if the splash_screen option
   is set. Returns how much area that ended up taking.
   If count_only is set to TRUE, only count how much it would
   take up. */
int
curses_display_splash_window(boolean count_only)
{
    int x_start = 0;
    int y_start = 0;
    int which_variant = NETHACK_CURSES; /* Default to NetHack */
    boolean splash = TRUE;

    if ((term_cols < 70) || (term_rows < 20))
        splash = FALSE; /* No room for splash screen */

#ifdef DEF_GAME_NAME
    if (strcmp(DEF_GAME_NAME, "SlashEM") == 0) {
        which_variant = SLASHEM_CURSES;
    }
#endif

#ifdef GAME_SHORT_NAME
    if (strcmp(GAME_SHORT_NAME, "UNH") == 0) {
        which_variant = UNNETHACK_CURSES;
    }
#endif

    if (strncmp("SporkHack", COPYRIGHT_BANNER_A, 9) == 0) {
        which_variant = SPORKHACK_CURSES;
    }

    if (strncmp("GruntHack", COPYRIGHT_BANNER_A, 9) == 0) {
        which_variant = GRUNTHACK_CURSES;
    }

    if (strncmp("dNethack", COPYRIGHT_BANNER_A, 8) == 0) {
        which_variant = DNETHACK_CURSES;
    }

#define MVADDSTR(y, x, str) if (!count_only) mvaddstr(y, x, str); y++;

    curses_toggle_color_attr(stdscr, CLR_WHITE, A_NORMAL, ON);
    if (splash) {
        switch (which_variant) {
        case NETHACK_CURSES:
            MVADDSTR(y_start, x_start, NETHACK_SPLASH_A);
            MVADDSTR(y_start, x_start, NETHACK_SPLASH_B);
            MVADDSTR(y_start, x_start, NETHACK_SPLASH_C);
            MVADDSTR(y_start, x_start, NETHACK_SPLASH_D);
            MVADDSTR(y_start, x_start, NETHACK_SPLASH_E);
            MVADDSTR(y_start, x_start, NETHACK_SPLASH_F);
            break;
        case SLASHEM_CURSES:
            MVADDSTR(y_start, x_start, SLASHEM_SPLASH_A);
            MVADDSTR(y_start, x_start, SLASHEM_SPLASH_B);
            MVADDSTR(y_start, x_start, SLASHEM_SPLASH_C);
            MVADDSTR(y_start, x_start, SLASHEM_SPLASH_D);
            MVADDSTR(y_start, x_start, SLASHEM_SPLASH_E);
            MVADDSTR(y_start, x_start, SLASHEM_SPLASH_F);
            break;
        case UNNETHACK_CURSES:
            MVADDSTR(y_start, x_start, UNNETHACK_SPLASH_A);
            MVADDSTR(y_start, x_start, UNNETHACK_SPLASH_B);
            MVADDSTR(y_start, x_start, UNNETHACK_SPLASH_C);
            MVADDSTR(y_start, x_start, UNNETHACK_SPLASH_D);
            MVADDSTR(y_start, x_start, UNNETHACK_SPLASH_E);
            MVADDSTR(y_start, x_start, UNNETHACK_SPLASH_F);
            break;
        case SPORKHACK_CURSES:
            MVADDSTR(y_start, x_start, SPORKHACK_SPLASH_A);
            MVADDSTR(y_start, x_start, SPORKHACK_SPLASH_B);
            MVADDSTR(y_start, x_start, SPORKHACK_SPLASH_C);
            MVADDSTR(y_start, x_start, SPORKHACK_SPLASH_D);
            MVADDSTR(y_start, x_start, SPORKHACK_SPLASH_E);
            MVADDSTR(y_start, x_start, SPORKHACK_SPLASH_F);
            MVADDSTR(y_start, x_start, SPORKHACK_SPLASH_G);
            MVADDSTR(y_start, x_start, SPORKHACK_SPLASH_H);
            break;
        case GRUNTHACK_CURSES:
            MVADDSTR(y_start, x_start, GRUNTHACK_SPLASH_A);
            MVADDSTR(y_start, x_start, GRUNTHACK_SPLASH_B);
            MVADDSTR(y_start, x_start, GRUNTHACK_SPLASH_C);
            MVADDSTR(y_start, x_start, GRUNTHACK_SPLASH_D);
            MVADDSTR(y_start, x_start, GRUNTHACK_SPLASH_E);
            MVADDSTR(y_start, x_start, GRUNTHACK_SPLASH_F);
            break;
        case DNETHACK_CURSES:
            MVADDSTR(y_start, x_start, DNETHACK_SPLASH_A);
            MVADDSTR(y_start, x_start, DNETHACK_SPLASH_B);
            MVADDSTR(y_start, x_start, DNETHACK_SPLASH_C);
            MVADDSTR(y_start, x_start, DNETHACK_SPLASH_D);
            MVADDSTR(y_start, x_start, DNETHACK_SPLASH_E);
            MVADDSTR(y_start, x_start, DNETHACK_SPLASH_F);
            break;
        default:
            impossible("which_variant number %d out of range", which_variant);
        }
        y_start++;
    }

    curses_toggle_color_attr(stdscr, CLR_WHITE, A_NORMAL, OFF);

#ifdef COPYRIGHT_BANNER_A
    MVADDSTR(y_start, x_start, COPYRIGHT_BANNER_A);
#endif

#ifdef COPYRIGHT_BANNER_B
    MVADDSTR(y_start, x_start, COPYRIGHT_BANNER_B);
#endif

#ifdef COPYRIGHT_BANNER_C
    MVADDSTR(y_start, x_start, COPYRIGHT_BANNER_C);
#endif

#ifdef COPYRIGHT_BANNER_D       /* Just in case */
    MVADDSTR(y_start, x_start, COPYRIGHT_BANNER_D);
#endif
    if (!count_only)
        refresh();

#undef MVADDSTR
    return y_start;
}


/* Resore colors and cursor state before exiting */

void
curses_cleanup()
{
#ifdef TEXTCOLOR
    if (has_colors() && can_change_color()) {
        init_color(COLOR_YELLOW, orig_yellow.r, orig_yellow.g, orig_yellow.b);
        init_color(COLOR_WHITE, orig_white.r, orig_white.g, orig_white.b);

        if (COLORS >= 16) {
            init_color(COLOR_RED + 8, orig_hired.r, orig_hired.g, orig_hired.b);
            init_color(COLOR_GREEN + 8, orig_higreen.r, orig_higreen.g,
                       orig_higreen.b);
            init_color(COLOR_YELLOW + 8, orig_hiyellow.r,
                       orig_hiyellow.g, orig_hiyellow.b);
            init_color(COLOR_BLUE + 8, orig_hiblue.r, orig_hiblue.g,
                       orig_hiblue.b);
            init_color(COLOR_MAGENTA + 8, orig_himagenta.r,
                       orig_himagenta.g, orig_himagenta.b);
            init_color(COLOR_CYAN + 8, orig_hicyan.r, orig_hicyan.g,
                       orig_hicyan.b);
            init_color(COLOR_WHITE + 8, orig_hiwhite.r, orig_hiwhite.g,
                       orig_hiwhite.b);
# ifdef USE_DARKGRAY
            if (COLORS > 16) {
                init_color(CURSES_DARK_GRAY, orig_darkgray.r,
                           orig_darkgray.g, orig_darkgray.b);
            }
# endif
        }
    }
#endif
}
