#include "Types.r"
#include "SysTypes.r"
#include "BalloonTypes.r"	    /* Mac resource type definitions */
#include "Files.r"				/* File resources */

#include "NHsound.r"
#include "NHrsrc.r"

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
	3000 * 1024,		/* recommended */
	2000 * 1024		/* absolute minimum, to be determined */
};

/* System 7 help balloon information */
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


/* Mac error decodes : 2000 - err# for common errors. Add your favorites below */

resource 'STR ' (2034) { "the disk is full"  };
resource 'STR ' (2036) { "there was an I/O error"  };
resource 'STR ' (2043) { "a file is missing"  };
resource 'STR ' (2044) { "the disk is write-protected"  };
resource 'STR ' (2047) { "the file is busy"  };
resource 'STR ' (2049) { "the file is already open"  };
resource 'STR ' (2108) { "there is not enough memory"  };
resource 'STR ' (2192) { "a resource is missing"  };
