#include "MacTypes.r"
#include "BalloonTypes.r"

#include "date.h"
#include "patchlevel.h"

#ifdef TGT_API_MAC_CARBON
resource 'plst' (0, purgeable) {
};
#endif

type 'sLem' as 'STR ';
resource 'sLem' (0, purgeable) {
	"Slash'EM " VERSION_STRING
	" Copyright © Stichting Mathematisch Centrum, Amsterdam, 1985Ð2003"
};

resource 'vers' (1, purgeable) {
	VERSION_MAJOR, (VERSION_MINOR<<4) | PATCHLEVEL, beta, EDITLEVEL, verUS,
	VERSION_STRING,
	"Slash'EM " VERSION_STRING
};

resource 'vers' (2, purgeable) {
	VERSION_MAJOR, (VERSION_MINOR<<4) | PATCHLEVEL, beta, EDITLEVEL, verUS,
	VERSION_STRING,
	"for Slash'EM " VERSION_STRING " (www.slashem.org)"
};

resource 'SIZE' (-1) {
	reserved,
	acceptSuspendResumeEvents,
	reserved,
	canBackground,
	doesActivateOnFGSwitch,
	backgroundAndForeground,
	dontGetFrontClicks,
	ignoreAppDiedEvents,
	is32BitCompatible,
	notHighLevelEventAware,
	onlyLocalHLEvents,
	notStationeryAware,
	dontUseTextEditServices,
	reserved,
	reserved,
	reserved,
	3000 * 1024,	/* recommended */
	2000 * 1024		/* absolute minimum, to be determined */
};

/* Classic help balloon information */
resource 'hfdr' (-5696, purgeable) { 
     HelpMgrVersion, hmDefaultOptions, 0, 0, /* header information */ 
	    { HMSTRResItem { /* use 'STR ' resource 2000 */ 2000 } 
	} 
};

resource 'STR ' (2000, purgeable) { /* Help message for app icon */
   "Slash'EM\nThis is the famous Dungeons and Dragons*-like game ported to the Macintosh." 
};

resource 'STR '(-16396, purgeable) {	/* Will be copied to the saved file. */
	"Slash'EM"							/* See Inside Mac VI, page 9-21.	 */
};

read 'File' (1000,"cmdhelp")	":dat:cmdhelp";
read 'File' (1001,"help")		":dat:help";
read 'File' (1002,"hh")			":dat:hh";
read 'File' (1003,"history")	":dat:history";
read 'File' (1004,"license")	":dat:license";
read 'File' (1005,"MacHelp")	":sys:mac:MacHelp";
read 'File' (1006,"News")		":sys:mac:News";
read 'File' (1007,"opthelp")	":dat:opthelp";
read 'File' (1008,"wizhelp")	":dat:wizhelp";
read 'File' (1019,"data")		"::lib:data";
read 'File' (1020,"dungeon")	"::lib:dungeon";
read 'File' (1041,"options")	"::lib:options";
read 'File' (1043,"oracles")	"::lib:oracles";
read 'File' (1045,"quest.dat")	"::lib:quest.dat";
read 'File' (1046,"rumors")		"::lib:rumors";
