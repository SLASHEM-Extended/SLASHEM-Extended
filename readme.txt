+------------------------------------------------------------------------+
|                              README.TXT                                |
+------------------------------------------------------------------------+
|                          SLASH'EM ver 0.0.8                            |
|               Super Lotsa Added Stuff Hack - Extended Magic            |
|                           A Nethack Variant                            |
| Courtesy of the Slash'EM development team with the efforts and support |
| of countless others, especially the slashem-devel mailing list.        |
|                                                             (1997-2005)|
+------------------------------------------------------------------------+
| Nethack is Copyright 1985-2003                                         |
|            By Stichting Mathematisch Centrum and M. Stephenson.        |
| SLASH'EM and NetHack may be freely redistributed.                      |
| See license in the dat directory for details.                          |
+------------------------------------------------------------------------+

+-------+
| Index |
+-------+

1 - News
	1.1 Status
	1.2 Changes
2 - Getting started
	2.1 Using official binaries
	2.2 Playing the game
	2.3 Building it yourself
3 - What is the Nature of this Beast?
4 - Who is Responsible & The Credits
	4.1 The dev-team
	4.2 Acknowledgements
5 - Further reading

+-----------------+
| Section 1: News |
+-----------------+

1.1 Status
----------

The alpha development stage of version 0.0.8 of Slash'EM began in May 2004.

If you find any bugs then please submit a report via the offical bug page:

http://sourceforge.net/bugs/?group_id=9746

If you are interested in the development of Slash'EM, please also join
the slashem-devel mailing list so that you can take part in discussions
and give us any feedback.

If you'd just like to talk about Slash'EM with other players or chat to
the developers, then please join the slashem-discuss mailing list.

Finally, all users are encouraged to join the slashem-announce list,
which is very low traffic but will keep you up to date on new versions.

Slash'EM mailing list page (including archives):

http://sourceforge.net/mail/?group_id=9746

All the above, and more, are available via our home page:

http://www.slashem.org/

1.2 Changes
-----------

ver 0.0.8E0F1 [?] [Released by ?]
-fixed bug 1231549: GNU autoconf vpath support is broken
-fixed bug 1231579: Can't specify wizard with GNU autoconf
-fixed bug 1232707: engraving with lightsabers
-fixed bug 1239041: engrave dialog doesn't show lightsabers
-fixed bug 1231766: Food shown in Medical Kits
-fixed bug 1238060: grammatical error when burning through a door 
		    (Benjamin Schieder)
-fixed bug 1104713: Pet and Frozen food stores can charge
-fixed bug 988827: Monster wearing CoI left invisible corpse
-fixed bug 934078: Mold "wiggle" out of bag
-fixed bug 933587: One-Eyed Sam referred to as "it"
-fixed bug 1240794: Pets don't grow up properly
-fixed bug 1240799: Steed ridden onto poly trap polymorphs permanently
-fixed bug 1243904: chameleon as statue trap
-fixed bug 1223226: Some creatures are missing horns
-fixed bug 1223222: Female Bulls??
-fixed bug 1244470: shopkeeper don't have shotgun
Autoconf improvements:
	-support for data librarian,
	-fix default values in help strings,
	-more portable use of test and sed,
	-drop useless borg and shout features,
	-recognize mingw32 platform for msys,
	-support aclocal 1.7 (needed for msys development),
	-basic support for Microsoft Window's native graphics,
	-better determination of default windowing system,
	-improved detection of libpng/zlib,
	-support "make depend".
-fixed bug 215062: no black color
		   Extends videocolors option, see Guidebook for details.
-Macintosh: fixed calculation of version for About Box
-fixed bug 1258354: msys autoconf fails to detect strncmpi
-fixed bug 1258355: autoconf Makefile.src assumes UNIX
-fixed bug 1258357: configure doesn't set TTY_GRAPHICS under mingw32
-fixed bug 1258358: No autoconf support for key handlers
-fixed bug 1244468: different spitting venom when polymorph
-fixed bug 1179557: Found egg marked (laid by you)
-fixed bug 1206099: Torches are not extinguished with rust traps
-fixed bug 1294907: 'Raise Zombies' technique cannot raise dwarf zombies! (Xeno)
-fixed bug 1355972: Monsters Don't gain Drain resistance due to worn items!
		    (Xeno)
-inserted missing #ifdef resulting in compilation of pure ansi tty version for DOS failure.
-fixed bug 1461660: Misaligned door

See history.txt for changes made in previous versions.

+----------------------------+
| Section 2: Getting started |
+----------------------------+

2.1 Using official binaries
---------------------------

The Slash'EM development team make offical binaries available for a few
platforms. There are other, unofficial, binaries available as well which
will come with their own instructions.

2.1.1 DOS
---------

Support for this platform is sporadic at best. You may find you have to
use an unofficial binary. When an official binary is provided:

o Decompress the zip archive into its own directory.
o Use a text editor to review defaults.nh and make any changes needed.
o If you chose to keep the fancy graphical environment (the allegro
  interface) by leaving video set to autodetect, then review alleg.cnf
  as well (this file is ignored in VGA tile mode).
o Run the slashem executable.

2.1.2 MAC
---------

The MAC binary comes with its own documentation which you should consult.

2.1.3 Linux RPMs
----------------

Perform the following steps:

o Install the package using rpm -U <package file>
o Read the RPM specific documentation in README.RPM
o In case of difficulty, consult Ali's linux page:

  http://www.juiblex.co.uk/nethack/slashem/linux.html

2.1.4 MS-Windows
----------------

Perform the following steps:

o Download the core package and install it. This will allow you to play
  Slash'EM using the native gui (common controls) windowing interface.
o If you would prefer to use the console (tty) windowing interface, then
  you need to edit the defaults.nh file and set the windowype option to
  tty by uncommenting the relevant line.
o If you would prefer to use GTK, an alternative graphical windowing
  interface, then you need to download and install the GTK add-on package
  as well. You will also need a copy of the gtk-win runtime environment
  version 2.2.1 or above. This is available from http://www.dropline.net/gtk
o Use a text editor to review either or both copies of defaults.nh and
  make any changes needed.
o If you chose to install the GTK add-on interface, then review gtkrc
  as well.
o Use the start menu or desktop icons to play Slash'EM under your
  preferred windowing interface.

2.2 Playing the game
--------------------

Once in the game, type in the name for your alter ego.  You will then
be prompted to select your character's role and race.  Select a gender
and alignment, if necessary.  Once you're on the dungeon map, use the
number pad to move around (or the standard roguelike keys [yuhjklbn],
if your turn off number_pad).  Move into monsters to attack them.
Help is the '?' key, inventory is 'i'.

Try out the menu system by hitting the ` key.

2.3 Building it yourself
------------------------

If you don't already have a copy of the source code for Slash'EM then
get the source tarball via our homepage:

http://www.slashem.org/

Then read the installation instructions specific to your platform,
as follows:

	Amiga		sys/amiga/Install.ami
	Atari		sys/atari/Install.tos
	MAC		sys/mac/Install.mpw
	MS-DOS		sys/msdos/Install.dos
	OS/2		sys/os2/Install.os2
	UNIX		sys/unix/Install.unx
	VMS		sys/vms/Install.vms
	MS-WinCE	sys/wince/Install.ce
	MS-Win32	sys/winnt/Install.gcc

+----------------------------------------------+
| Section 3: What is the Nature of this Beast? |
+----------------------------------------------+

THE PROBLEM:  The AMULET OF YENDOR has been stolen.  Not only that but
it appears that the Wizard of Yendor (not a nice person), who took the
amulet, is hiding in the Dungeons of Doom (not a friendly place).

The SOLUTION:  Well, armies have been suggested.  Maybe hiring a superhero
or two.  Unfortunately, it seems that it is more economical to offer a
reward and let some poor adventurer with dreams of glory go and get it.
Guess who's got the enviable job of saving the day...

Super Lotsa Added Stuff Hack - Extended Magic (SLASH'EM) is a role-playing
game where you control a single character.  The interface and gameplay
are similar in style to Rogue, ADOM, Angband and, of course, Nethack.
You control the actions through the keyboard and view the world from an
overhead perspective.

+--------------------------------------------+
| Section 4: Who's Responsible & The Credits |
+--------------------------------------------+

4.1 The dev-team
----------------

o Project administrators: Warren Cheung and J. Ali Harlow
o Developers: Pekka Rousu and Clive Crous
o Mac port: Paul Hurtley

4.2 Acknowledgements
--------------------

Thanks to Andrew Apted, who contributed the glHack interface.

Thanks to Kevin Hugo (hugo@cae.wisc.edu), the previous Macintosh
maintainer and the author of many changes, who has had to move on to
other projects.

Thanks to Dirk Schönberger (shoenberger@signsoft.com), who worked on
the Qt and Linux ports.

Thanks to Ben Olmstead for mirroring the homepage.

Thanks for the support, bug reports and suggestions of
(in no particular order):

	Pat Rankin, Linley Henzell, 0ystein Kerstad, Jim Miller, pi,
	Steven Uy, Alex Critchfield, Mike Balcom, Sascha Wostmann,
	Drew Curtis, Daniel Krause, E. P. Dinklewhopper, Tim Jordan,
	Lukas Geyer, Kresimir Kukulj, Brent Oster, Travis McKay,
	Jared Withington, Philippe-Antoine Menard, Ain Uustare,
	Doug Yanega, David Tang, Lief Clennon, Robert Herman, Petra Prinz,
	Aki Lumme, Nick Number, Robin Johnson, Heiko Niemeier, Chris Bevan,
	David Damerell, Kevin Hugo, Daniel Mayer, Ben Olmstead, Tom Friedetzky,
	Kalle Niemitalo, Sabine Fischer, Christian Bressler, Jake Kesinger,
	Chris Subich, Paul Sonier, Peter Makholm, Matthew Ryan, J. Ali Harlow,
	George Stephens, Bruce Cox, Edward Walker, Karl Garrisson,
	Finn Theoderson, Kelly Bailey, Seth Friedman, Jonathan Nieder,
	Eva R. Myers, Darshan Shaligram, Tina Hall, Christian Cooper.

+----------------------------+
| Section 5: Further Reading |
+----------------------------+

o Slash'EM Homepage: http://www.slashem.org/
o Ali's Slash'EM page: http://www.juiblex.co.uk/nethack/slashem.html
o Eva's spoiler page: http://www.statslab.cam.ac.uk/~eva/slashem/index.html
