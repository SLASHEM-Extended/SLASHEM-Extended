#include "date.h"
#include "patchlevel.h"

type 'slRc' as 'STR ';
resource 'slRc' (0, purgeable) {
	"Recover " VERSION_STRING " Copyright © David Hairston 1993"
};

resource 'vers' (1) {
	VERSION_MAJOR, (VERSION_MINOR<<4) | PATCHLEVEL, beta, EDITLEVEL, verUS,
	VERSION_STRING,
	"Recover " VERSION_STRING
};

resource 'vers' (2) {
	VERSION_MAJOR, (VERSION_MINOR<<4) | PATCHLEVEL, beta, EDITLEVEL, verUS,
	VERSION_STRING,
	"for Slash'EM " VERSION_STRING " (www.slashem.org)"
};
