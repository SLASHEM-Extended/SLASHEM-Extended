+------------------------------------------------------------------------+
|                              README.TXT                                |
+------------------------------------------------------------------------+
|                          SLASH'EM ver 0.0.6E4                          |
|               Super Lotsa Added Stuff Hack - Extended Magic            |
|                           A Nethack Variant                            |
| Courtesy of Warren Cheung (wac@intergate.bc.ca), Kevin Hugo and        |
|              the efforts and support of countless others               |
|                                                             (1997-2001)|
+------------------------------------------------------------------------+
| Nethack is Copyright 1985-2000                                         |
|            By Stichting Mathematisch Centrum and M. Stephenson.        |
| SLASH'EM and NetHack may be freely redistributed.                      |
| See license for details.                                               |
+------------------------------------------------------------------------+

+---------------------------------+
|Section 0.0 - What's on the Menu?|
+---------------------------------+
1.0 - News Briefs
        6.1 The Messy TODO/TOFIX List
        6.2 Today's Highlights
        6.3 Other Ideas
2.0 - For those in a Hurry...
	1.1 Quick Start
3.0 - What is the Nature of this Beast?
4.0 - Developer's Soapbox
5.0 - Theoretical System Requirements & Installation
        4.1 32-bit DOS Port
        4.2 Macintosh Port
        4.3 Also Available
6.0 - The Optional Things
7.0 - Who is Responsible & The Credits
        7.1 Contact
        7.2 Pillars of Strength
        7.3 Acknowledgements
8.0 - Other Resources

+----------------------------+
|Section 1.0 News Highlights |
+----------------------------+
===>1.1 Messy TODO/TOFIX list
-----------------------------
Please note that these are current ideas that may make no sense in their
current note form,  but *may* be implemented at a future date.

-monster players?
-trample with jump?
-push splat with boulder
-allow lifesaving if in high favor with god?
-pull dog through closed doors with leash
-Angband easy patch stuffs add?
-hypernutritious food?
-sleep
-force door - shoulder
-chatting == buy and sell?
-some way of determining weapon alignment for artys?
        -probing?
        -appraisal?
-bug? pack rats stealing large objs?
-wish for intrinsics
-make sleep resist quest nemeses?
-monster infight (eg. elves & orcs)
-mixed type monster groups
-id stones from stones on ring?
-'_' multirest?
-more accurate appraisal?
-shopkeepers cheating when shouldn't?
-engulfers picking up candle of eternal flame will put it out :(
-eating nutritious food exercise con?
-Chocobo's (for Darren Hebden ;B)
-fishing pole hitting toilets,  etc.  special cases ;B
-applying spellbooks ripping into scrolls
-pantheons - more elaborate pray/etc.?  Selectable?
-text parsing conversations?  Improved #chatting
-smell object trail?
-docs revamp/help system
-Allegro library port in progress.  Need to figure out how to draw tiles...
-mouse code started (pckeys.c,  video.c)
-vampire <==> vampire bat
-gnome - always slip through cracks? innate digging?  tame related creatures?
-gas spores killing - leaving corspses that can sprout gas spores?
-skilled at polearms - no close combat penalty?
-bloodblade - weapon that draws blood every X turns when wielded
-celebrity bones piles
-sewars/pipes access through sinks..
-arc lightning! (radial,  random dir)
-shatter weapon spell?
-pet integration...
-scroll of genocide -> ungenocide...
-tinning kits not charging?
-gas spore explosions are now poison gas (explode.c, monst.c, mon.c)
-sometimes able to choose aligns for roles that shouldn't allow align choose?
-Tourist - no techs?
-zap spells centered on player?
-succubi msgs?  Commented out code?
-monks -disarm?	
-eggs of winter wolf?!!
-SIGIL - damage
-veggies?  Grow?  Garden :B
-Hmm...timing out players turned into undead?
-anti teleport?
-Jedi, orc classes?
-do the race class thing?
-Use tech menu to invoke monster attacks
-incorrect random align chosen?
-shout "Spoon, evildoers!"
-drain life to actually drain life - uhitm, mhitm, mhitu, spell ?
-heat eye beams ;B  F-mage start tech?
-#helper
-do wax golems melting via fire?  Monsters special AD hit effects?
	-AD_FIRE attack?
-check unicorns catching glass
-glass golem glass launcher?
-D -poly weapons
-monks/martial arts - severe penalties for encumberance
-use percent-success in spell.c for chance/ combine with mods....
	-make spellcasting easier, but harder on power
	-wizards might need to get their spellcasting base fixed...
-meteor (use scroll of earth code)/armageddon
-fix monsters jumping through doors
-save ==> use the leve change paranoia stuff?
Just a few things I'd like to see.  Take the suggestions as you will.
Feel free to comment.

Have becoming Grand Master in martial arts give something extra.  As
far as I know it does not besides increasing unarmed damage.  Perhaps
the occaisional stunning blow at Master level, and at Grand Master,
perhaps the Touch of Death, usable about as often as one can invoke
artifacts, but only performable while bare-handed.  
(orlando de la cruz)
-Have Japanese versions of pancake be okonomiyaki, and club be bokken
(wooden practice sword).

-Have Offler become angry with you if you sacrifice a crocodile corpse
(he is the crocodile god, after all).
-unicorns ==> _horse corpse_ and horn? Then what about sacrificing unicorns?
	-how about unicorn corpse can be applied to obtain to get horse corpse 
	and horn
-Dopp and item generation - on existing items??  Or self generate?
-boots of kicking should bust down chests,  etc.
-weapons will "advance"?  Class change thing from FF?
-make race a separate field - get rid of all those is_foo()...
-make modifiers to movement keys do different strength attacks
-auto-enhance skills?  Prompt? 
-make it possible to finish the game - no "dead ends",  more choices.
	-eg. ignore quest - ironman mode ;) 
-invisible blindfold 
-spellbooks still need tiles (ochre)  and 3 others... should get 
-get tiles == description thing working again.  Monster Tiles :  Smaug, Thorin
-fix tamed water demon chat (??? How?)
-finish prayer upgrades
-detonate wands via spell - random aps in all dirs :)
-wish for attributes (str, luck, dex, etc.)
-use 'n' for multi-dig
-statuetraps activate multiple statues?
-stairs /w lots of items at base need "stairway here" message.
-more shop types?  Jewellers,  magic shop...
-elemental/demon/angel summoning - time-limited summoning? Use poly code?
-check for monster bullwhip and adjacent
-spells - summon lights
-Give small dmg bonus for wearing rings in H to H combat?
-cleanup -dropx, useupf
	-check for donning
	- foo_off, foo_gone, etc...
-check rehumanize
-generate animals as upset pets (that need to tamed?) ?
-priests for quest altars?
-implement magical armors (eg BDSM) for monsters
-one-eyed sam comes back?  Who takes care of blackmarket after he passes on?
-frozen steed easier to mount?
-food distrib?  Make corpses have variable timeouts rather than prob of corpse?
-limit # of Titans to 12?  (6 male, 6 female,  minus Prometheus and Atlas)
There appear to be many many Titans....
	-Cronus, Rhea
	-Iapetus and Themis, 
	-Oceanus and Tethys, 
	-Hyperion and Theia, 
	-Crius and Mnemosyne, 
	-Coeus and Phoebe
------------------------
	-TODO:finish god_gives_benefit code ;)
	-TODO-make ROPoly timed? charges?
	-TODO make all lightsaber code ifdef
	-FIXUP - Dark Elves - dual scimitars? Crossbow?
	-monster read scroll of taming vs pets ;)
	-Bug? pools of water kills DEATH?/riders
	-Tourist tech - detect stairwells
	-more endgames - journey to center of the earth 
		(end with throwing the amulet away à la Lord of the Rings)
		-kill a rider and take its place.
		-"Arcadia" instead of Gehemmon
-use obj->spe for spell level?
-tone down dmg for gun weapons? 
-C4, plastique
-woodchucks passing through walls(intentional),  xorn passing through trees! ;)
-Xorn tunnelling up/down?
-timeout genocide...monsters come back (explaining why people keep writing dem 
	scrolls ;) Angrier?
-"*" + "c" == doswapwep or twoweapon?
-artifact trident vs water
-monsters laying eggs in spare time?
-Hobbit "Teleportitis"
-instant_death, quick_death...??
-body armors - Kevlar(R), Twaron(R), Spectra Shield(R), Gold Flex(TM)
-dragon scale shields
-key redefinition
-use energy to absorb damage
-file of character stats at death/ingame
	-list inventory, stats
	-ethics(death only) genocides (death only)
-fainted == can't see 
-vampire bat needs to be a vampire - how are weres handled?
-god sends monadic deva - bug?
-map PMs, Onames to strings? - would involve finding all the PM refs
			-corpses, monster type, your monster type
			-unpoly mon & obj
-eaten corpses coming back with mucho HP
-weapons progression?  Different types of normal attacks? apply gems onto weapons?
	-apply gems == crush for effects?
		-DILITHIUM_CRYSTAL	Energy source in Star Trek
		-DIAMOND	-Invulnerable vs Undead
		-RUBY 		-good luck
		-JACINTH 	-luck in travelling, vs plague, ward vs fire
		-SAPPHIRE	-aids understanding problems, kills spiders
		-BLACK_OPAL	?  (Death)
		-EMERALD	?  (Nature? Regen? Heal?)
		-TURQUOISE	-aids horses in all ways, shatters when used.
		-CITRINE	?  -neutral attitude, rational thought. Cheerful
					(vs Mind attack?)
		-AQUAMARINE	?  (Water - Magical breathing?)
		-AMBER		vs disease
		-TOPAZ		vs evil spells
		-JET		soul object
		-OPAL		?
		-CHRYSOBERYL 	wards off foes (Warning?)
		-GARNET		?
		-AMETHYST 	prevent drunkenness, drugging
		-JASPER		vs venom
		-FLUORITE	?
		-OBSIDIAN	?
		-AGATE		-restful, safe sleep.
		-JADE	skill at music, musical instruments
	Color relations?
		Black	The Void, darkness, negation, blindness.
		Brown	The Earth, fertility, comfort, sleep.
		Blue	The Heavens, the sky, air, truth, spirituality, calmness.
		Clear	The Sun, luck, awareness, insight.
		Green	Venus, reproduction, sexuality, sight, resurrection, jealousy
		Orange	Mercury, heat, quickness, wisdom.
		Red	Mars, blood, war, fire, madness.
		Violet	Royalty, health, friendliness.
		White	The Moon, purity, virginity, hope, enigmas.
		Yellow	Secrecy, homeopathy, jaundice, diseases, cowardice.
-splinter attack
-races - vampires, liches (like elf but low char), demons...
-make all source files 8.3 so that SHORT_FILENAMES so no truncating filenames
-stackwep (e.g. marilith wield 6 daggers)

===>1.2 What's NEW
------------------
Bug - able to kick things out of an unseen pit?
Bug - mold from  molded corpse should no longer have the name of corpse?
Bug - obj_is_local problem?
Bug - flame spheres attack cause crash?
	-flame spheres and shopkeepers...
Bug - throwing or wielding silver while a were doesn't affect you / monsters?
Bug - not getting "you hear noises" when pet attacks but can see with telepathy?
Bug - graves in barracks?  No goodies when not randomly generated?
Bug - cockatrice lairs no longer have "goodies"?

-ToAdd:  Control amount of eating with 'n'

-are meat-foo always delicious to any non hervivore? 
	(eg jackals,  undead, oozes?!)
-more rigorous check - make "has_blood" macro?
-should food be allowed to merge back in touchfood()?
-allow vampires to bite via #monster, turn into bats
-do the allegro botl split in the allegro code?
-undead should not be drinkable except vamps? 
	What about zombies, Frankenstein...
-sleep spell locking up - giving "sleepray hits newt?"
-Giantkiller - destroys boulders
-Thiefbane - extra damage based on stolen items
	-unpaid if shopkeeper
	-has your invlet if you wield...
-go through and do spfx for all the artis
-go through and do specials for (all) the monsters
-move the expanded tile detection to the txt library and use it to do
	on the fly interpolation when generating bitmap
-shops restock. Black market has at least 1 of every item
-can feel invisible objects when searching, blind
-remove encumber messges on startup
-fake object names for hallucinated objects (like monsters)
-mail daemon getting stuck? Too many monsters crowding?
-'%' command displays eaten corpses
	-need to track corpses known and eaten
	-give dopps a message to let them know they "remember" this polymorph
	-use this as a menu for polymorphing...
-implement gunblade
	-gunblade
		-two handed
		-gun aspect of a revolver (shorter range), 
			slightly weaker than a two handed sword
		-has own weapon skill
	-use apply to fire bullets AND slash (low bullet hit rate)
	-throw fires bullets alone
	-attack slashes alone
-reading invisible scrolls?
-"wizard need food badly" => different for vampires?
-turning into a werewolf in a shop -> shouldn't sell equip?
-back option for character creation TTY? - pretty tough, I think
-monsters should use "techniques" against you...pets using techniques
	-pet unicorns remove ailments, etc...
-Buy items via a menu - #chat with shopkeeper
-Limit Break based on HP loss? (see howling code)
-redo/add sound system using allegro
-split allegro into separate windowsystem after implementing use of GUI
-OR merge allegro to TTY / separate from DOS code
TODO: 
	-Do the Role/Race/Align/Gender Menusystem (WAC)
	-alleg_xputs should use const char (Strcpy then put up?)
*[In Progress] (Trying to) compile with Allegro WIP 3.9.32
	-anticipating using allegro on other systems
	-currently need to redo the font loading code (at least)
	-it appears to be segfaulting (randomly?)
-tofix: using the nethack config files.
-vampires shouldn't be allowed to become Flame Mages

ver 0.0.6E9 [?] [Released by ?]
-Actioned feature request 575146: Improvements to gypsy fortune messages.
-All situations where a monster flees now generate a message.
-Merged Sam Dennis's more intelligent autocomplete patch
-Merged Roderick Schertler's held status flag patch
-fixed bug 603350: Display layers: floating glyphs missing
-Actioned feature request 520961: Specifying lamplit flag in level files
	(Pasi Kallinen)

ver 0.0.6E8 [January 24/2003] [Released by Ali]
-fixed bug 233901: Gtk-Critical errors from gtk_main_quit.
-Proxy: Implement proxified GTK interface.
-Proxy: Add formal versioning of NhExt standards.
-Proxy: Add support for callback extensions.
-Merged Scott Bigham's blocked boulder patch
-Merged Sam Dennis's extended wizard-mode ^V command
-Merged Roderick Schertler's pickup thrown patch
-Merged Dylan O'Donnell's rumors of mail patch
-Merged Eric Wright's and Dylan O'Donnell's #vanquished patch
-Merged Aardvark Joe's record kept conducts patch
-Added compile time option to disable bones files
-Merged Malcolm Ryan's enhanced alchemy patch
-Merged Pasi Kallinen's dungeon growth patch
-Merged Nephi's grudge patch

ver 0.0.6E7 [December 6/2002] [Released by Ali]
-Lethe: Treacherous pets; Cthulhu revivification; monsters use of potions of
	amnesia & wands of cancellation; reflecting golems (Darshan Shaligram).
-Implemented gtkhack, a Gtk 2.0 external window interface for UNIX and win32.
-add support for multiple routes through the dungeon
-Lethe: Special rooms (Darshan Shaligram).
-Proxy: Switch to buffered I/O to remove packet length limitations.
-Proxy: Implement sub-protocol 0.

ver 0.0.6E6 [September 13/2002] [Released by Ali]
-Implemented "none" as a possible random place in level compiler.
-fixed bug 500833: Spellbook learn.
-fixed bug 456805: Baby dragons don't grow up.
-fixed bug 531179: Beheading when invulnerable.
-fixed bug 499439: Fireballs don't explode on hitting you.
-fixed bug 528174: having shopkeepers enchant daggers crash.
-fixed bug 499055: Bug in power-draining trap.
-refixed bug 459699: Oops. add_to_billobjs: obj is timed.
-applied patches 457587, 457592, 457593 & 457601: Building with Makefile.nt
-fixed bug 524758: #borrow on pets exercises dexterity.
-fixed bug 525461: Can't compile without UNPOLYPILE.
-fixed bug 479068: Bones can contain guaranteed artifacts.
-fixed bug 479067: Bones can contain aligned nemeses.
-Implemented NhExt sub-protocol 1.
-Proxy: Added support for callbacks.
-Proxy: Add display inventory callback.
-Proxy: Add dlb callbacks.
-Proxy: Add redraw callbacks.
-Proxy: Add status mode callback.
-Proxy: Add option callbacks.
-fixed bug 468049: Ronin should be removed.
-Implemented first cut at display layers.
-Merged vanilla 3.4.0 codebase.
-New lawful quest (nightmare) level (Tina Hall).
-OS/2: Added support for big tiles to X11 port (Pekka Rousu).
-OS/2: Enabled use of lex and yacc by default (Pekka Rousu).
-fixed bug 552677: "ask before hidding peaceful monsters".
-fixed bug 537214: /oDig at earth Es should anger them.
-Macintosh: added support for Meta key
-fixed bug 562517: Can't wish for vampire blood.
-fixed bug 561043: "<mon> is suddenly moving faster" mesg.
-fixed bug 558228: Liquid leap should anger peacefulls.
-fixed bug 558229: Bad input in techinque menu.
-fixed bug 566538: zero payment in money2mon!
-fixed bug 566564: Monk breaking the vegetarian conduct.
-fixed bug 557603: Vanishing items on display.
-fixed bug 567591: Shooting through closed door.
-fixed bug 567598: Firing more shells than exist.
-fixed bug 562550: Blank graves on priest quest.
-fixed bug 558230: Sokoban penalty for liquid leap.
-fixed bug 570738: Tame arch-lich summons hostiles.
-fixed bug 571086: C340-29: couldn't place lregion.
-fixed bug 570756: Firing assault rifles takes no time.
-Proxy: Add get player choices callback.
-Proxy: Add is valid selection callback.
-Proxy: Add quit game callback.
-Proxy: Add display score callback.
-Proxy: Add doset callback.
-Proxy: Add get extended commands callback.
-Proxy: Add map menu cmd callback.
-Proxy: Add get standard winid callback.
-Proxy: Add support for display layers.
-fixed bug 575250: Priest creation crash
-Proxy: Add support for tilesets.
-Actioned feature request 457998: Inconsistencies in monster polymorph
-fixed bug 580276: Stoning polyed monsters leaves statue.
-fixed bug 576543: Wands of draining should boing!
-fixed bug 582459: High Lycanthrope
-Added Lethe monsters (Darshan Shaligram).
-fixed bug 583448: No apply with healtstone.
-Added new SDL/GL windowing port.
-Added spooky sounds to nightmare level (Tina Hall).
-fixed bug 542027: Drained corpse gets moldy (Darshan Shaligram).
-Added potions of amnesia and Lethe water effects (Darshan Shaligram).
-Proxy: Add support for mapping between glyphs and tiles.
-Proxy: Add support for mapping between glyphs and symbols.
-fixed bug 570743: Plane of water full of spider webs.

ver 0.0.6E5F2 [November 30/2001] [Released by Ali]
-Implemented NhExt XDR (low level protocol for proxy window).
-Implemented support routines for NhExt sub-protocol 1.
-Upgraded GTK 2.0 interface to support Gtk+ version 1.3.9
-Actioned feature request 455676: Invulnerability & casting without power.
-Actioned feature request 469036: Thiefbane should drain levels.
-fixed bug 457682: Steal from black market - militia gen.
-Actioned feature request 476860: No wishing for guaranteed artifacts.
-Actioned feature request 476867: Indestructable aligned keys.
-fixed bug 470314: Sting and Orcrist should be lawful.

ver 0.0.6E5F1 [August 31/2001] [Released by Ali]
-New messages when engraving with wand of draining.
-Partially implemented first cut of proxy window interface support
-fixed bug 424479: Dipping into potions of gain level.
-Full support for Kevin Hugo's aligned keys.
-fixed bug 436639: Polyself in the Black Market.
-Actioned feature request 454655: Fuzzy object and shopkeepers.

ver 0.0.6E5 [May 3/2001] [Released by Warren]
-Basic support for Kevin Hugo's aligned keys
-Basic support for GTK 2.0 (currently as a seperate window port)
-fixed bug 232030: GTK: Can't start in character mode.
-fixed bug 417384: Shopkeeper services.
-fixed bug 225227: Vampires start with food rations.

ver 0.0.6E4F7 [February 28/2002] [Released by Ali]
-Improvements to database (Lars Huttar)
-fixed bug 488069: Magic lamps/candles can't be turned off.
-fixed bug 488589: Burned-out lamps cannot be turned off.
-fixed bug 490478: obj_is_local.
-fixed bug 490405: Eating merged food still buggy.
-fixed bug 460578: Macintosh window mode broken
-fixed bug 486268: holy spear of light doesn't burn player.
-fixed bug 480578: Hellfire/Plague don't work for monsters.
-fixed bug 493892: messages for lighting dynamite, inventory (lit) indicator.
-fixed bug 499368: Dynamite causes crash after detonation.
-fixed bug 504740: Serpent's Tongue gives bad message.
-fixed bug 491808: leashed hostile creatures.
-fixed bug 493896: Bullets not used up when fired upwards.
-fixed bug 520950: Level generator doesn't support scrawls.
-fixed bug 520412: Level generator doesn't support toilets.
-fixed bug 490400: Riders and revivification.
-fixed bug 516027: dlb utility program crashes on query.
-fixed bug 513831: (armed) tag doesn't work for grenades.
-fixed bug 519441: "pack shakes violently" does nothing.

ver 0.0.6E4F6 [November 30/2001] [Released by Ali]
-fixed bug 453246: Genetic engineers attacking monsters.
-fixed bug 458121: Vanishing corpses in ice boxes.
-fixed bug 458085: Zap wands of healing at steed.
-fixed bug 457238: Flame mage quest text mixup.
-fixed bug 458093: Free action bonus against grabs too high.
-fixed bug 456794: "doppelgangen".
-fixed bug 458336: Rings of sleeping and sinks.
-fixed bug 456335: Can't wish for bat from hell.
-fixed bug 454129: Wrong weight of piles of meatballs.
-fixed bug 455393: Confusing shk credit messages.
-fixed bug 456137: Pets and Sam's portal.
-fixed bug 456803: Riding through Sam's portal.
-fixed bug 457683: Crash: lost gas grenades.
-fixed bug 462512: Tight loop in blessed genocide menu.
-fixed bug 462513: GTK: getline crashes on close.
-fixed bug 459396: crash when entering Sam's portal.
-fixed bug 457864: enlightenment & versus stone.
-fixed bug 459954: slight missing wall in spiders.des map.
-fixed bug 455563: Shopkeeper refers to mattock.
-fixed bug 420942: two entrances to mines.
-fixed bug 452172: Penalty for drowning pets.
-fixed bug 452130: Odd message when charming shopkeeper.
-fixed bug 461443: Frankenstein is vegan.
-refixed bug 437609: No Caveman quest artifact?
-fixed bug 459778: Fuzzy and non-fuzzy object merging.
-fixed bug 458130: More spiders should be web-spinners.
-fixed bug 467477: HSoL #invoke message incomplete.
-fixed bug 468235: Upgrading wooden harps causes oddities.
-fixed bug 465183: black mold has two attk's and speed 0.
-fixed bug 460122: upgrade code blocks alchemy.
-fixed bug 459699: Oops. add_to_billobjs: obj is timed.
-fixed bug 450234: GTK: Taking n items out of a bag.
-fixed bug 459878: cut_worm: no segment at (45,14).
-fixed bug 465188: Deathsword's special attack impossible.
-fixed bug 469519: Cannibal checks for potential pets.
-fixed bug 469533: Luck bonuses of fedoras.
-fixed bug 462046: compile error in qt_win.cpp RH7.1, Qt2.3
-fixed bug 453628: You are warned of undead.
-fixed bug 469034: Thiefbane doesn't decapitate.
-fixed bug 469764: Great Dagger ignores drain resistance.
-fixed bug 450138: compilation errors in Winnt.
-fixed bug 465181: silver mace sdam should be d6+1.
-fixed bug 458625: Potion effects from sinks crash.
-fixed bug 453100: Papyrus spellbooks.
-fixed bug 475774: Dipping into potion of acid message.
-fixed bug 469282: vampire race to vampire chat problem.
-fixed bug 458134: character does't fall after a dash.
-fixed bug 482191: m_useup() messes up weight.
-fixed bug 480638: Doomblade uses bad messages.
-fixed bug 469033: Artifact missile launcher bonuses.
-fixed bug 469529: Throwing unacceptable food at monsters.
-fixed bug 482204: wielded/quivered food.
-fixed bug 482201: Eating merged partly eaten food.
-fixed bug 470817: Problem learning foodtype form tin.
-fixed bug 482196: weight() for partly eaten objects wrong.
-fixed bug 480547: Storm Whistle isn't intelligent.

ver 0.0.6E4F5 [August 31/2001] [Released by Ali]
-fixed bug 420517: Yellow dragon scale mail.
-fixed bug 420519: Unicorn horns, sliming, and bad message.
-fixed bug 421136: engraving with wands of draining.
-fixed bug 420947: remembered map ignores transparency.
-fixed bug 421733: Monster on monster attacks.
-fixed bug 422154: passwall makes you "etheral".
-fixed bug 422161: player's demon summoning may cause crash.
-fixed bug 420545: Steed galloping while asleep.
-fixed bug 424455: Tame ghouls and shopkeepers.
-fixed bug 422157: twoweapon status not saved?
-fixed bug 422163: passwall and "carrying too much..."
-fixed bug 422155: sigil of discharge kills multiple times.
-fixed bug 423715: Candle of Eternal Flame is buggy.
-fixed bug 422540: Succubi interaction while unconscious.
-fixed bug 420941: inventory won't scroll.
-fixed bug 425997: Exploding wands of healing.
-fixed bug 422880: Sleep ray kills monsters? Bug?
-fixed bug 422153: inaccessible spells after first 52.
-fixed bug 424394: Summon Undead causes crash.
-fixed bug 423686: Steed resisting and hilite_pet.
-fixed bug 430975: "the the Dark Lord".
-fixed bug 431283: Sleeping monsters can evade disarm tech.
-fixed bug 433019: Shopkeeper rustproofing and corrosion.
-fixed bug 433694: Holy Spear of Light blasts Yeoman.
-fixed bug 433695: Wrong weight of old corpses?
-fixed bug 436049: Rogues striking twice from behind.
-fixed bug 436047: Can't polymorph into dwarf or gnome.
-fixed bug 436055: Altar stained with doppelgangen blood.
-fixed bug 436056: Cavewomen and amulets of change.
-fixed bug 437609: No Caveman quest artifact?
-fixed bug 437840: "The curate turns into a curate!"
-fixed bug 422165: Slow screen update on vaporizing boulders.
-fixed bug 423217: "Silent" grenade blasts.
-fixed bug 440005: Call a wand of polymorph.
-fixed bug 440052: Making Sam angry.
-fixed bug 440159: Doppelgangers and save/restore.
-fixed bug 443827: New monsters and eating.
-Added debugging for bug 420942: Two entrances to mines.
-fixed bug 450316: Sting isn't listed as poisoned.
-fixed bug 436375: X11 Text windows too small.
-Put lex/yacc files in separate folder in mac build
-Direct bugs to SourceForge not hurtley in mac build
-Merge mac patches into SourceForge sources
-fixed bug 454954: Stealing iron balls.
-fixed bug 452244: Command undead causes crash.
-fixed bug 454800: a hang in sink-drinking.
-fixed bug 450229: Tame solars drop Sunsword.
-fixed bug 450119: Ice mage quest text mixup.
-fixed bug 450460: The plural of drow should be drow.
-fixed bug 452167: Rust from toilet water.
-fixed bug 455511: Finishing off scrolls on restore.
-fixed bug 455390: rogue quest message.
-Restore original NetHack copyright date in readme.txt
-Change strchr back to index in role.c
-Don't create 600-byte automatic in makedefs.c
-Support Mac build with Guidebook in doc directory
-fixed bug 453083: Vecna doesn't always leave his hand.
-fixed bug 455346: onbill: unpaid obj not on bill?
-fixed bug 456459: Weak steeds can jump into poly traps.
-fixed bug 425541: 'Blink' hobbit tech doesn't do anything.

ver 0.0.6E4F4mac0 [May 13/2001] [Released by Paul]
-built Mac 68K port
-fix bug: version garbled in Mac About box
-fix bug: crash selecting Help/Options/Macgraphics
-fix bug: Guidebook not displayed when selecting Help/Help/Guidebook.
          For now, only the first 10000 characters are displayed

ver 0.0.6E4F4 [May 2/2001] [Released by Ali]
-fixed bug 404825: wands of teleportation that explode/zapped at yourself
-Integrated Mac port
-Subverted setguid check in GTK+ version 1.2.9 and above
-fixed bug 405091: Contact poison corrodes greased gloves.
-fixed bug 409695: Bad fruit after polymorph.
-fixed bug 409699: Falling polymorphed objects cause crash.
-fixed bug 409906: No eucalyptus leaves from trees.
-fixed bug 409694: Polypiling instability.
-fixed bug 409984: Off-by-one error in coyote naming.
-fixed bug 410588: Caveman quest artifact non-existant.
-fixed bug 410583: Polymorphing spellbooks loose charges.
-fixed bug 410582: your foo seems less fuzzy.
-fixed bug 410402: Unpolymorph only remembers previous form.
-fixed bug 407857: Some wizard-mode commands don't work.
-fixed bug 409196: (DOS?) display bug with & command.
-fixed bug 409905: Drow don't recognize droven objects.
-fixed bug 405649: Messages about unseen pet's activities.
-fixed bug 411278: "Couldn't place lregion...".
-fixed bug 411282: arrow traps can't poison?
-fixed bug 411279: New demon lords won't wait.
-fixed bug 411340: Some racial intrinsics are missing.
-fixed bug 412250: Shopkeeper uncursing and Necromancers.
-fixed bug 412251: Shopkeeper charging and spellbooks.
-fixed bug 409198: underwater vampires.
-fixed bug 411277: strange charging messages.
-fixed bug 413366: Couldn't eat fortune cookie.
-fixed bug 413243: eating amulets of flying.
-Tweaked winnt makefile to make Win2K build easier for Ali.
-fixed bug 413537: Expensive camera malfunction.
-fixed bug 414101: Crash when asking for Help at prompt.
-Preliminary work on bug 405663: Port internal malloc to win32 platform.
-fixed bug 414291: GTK/win32 hide_privileges.
-fixed bug 227942: Crash when picking up a stack of objects.
-fixed bug 414674: Crash when walking over items.
-fixed bug 414673: Crash with exploding rings and wands.
-fixed bug 414152: W32 interface: directions.
-fixed bug 411280: Dragon Caves full of babies.
-fixed bug 415213: Yeenoghu touch of death.. bug?
-fixed bug 412289: Spellbook charging abuse.
-fixed bug 413966: Colours in map style ignored.
-fixed bug 418310: handling of vampire mage corpse.
-fixed bug 419866: Qt: Can't load "rip.xpm".
-fixed bug 419867: Gnome: Couldn't load required xpmFile.
-fixed bug 419918: X11: Map resize confusions.

ver 0.0.6E4F3 [February 28/2001] [Released by Ali]
-DOS VGA and Allegro videosystems now compile with REINCARNATION not defined
-fixed bug 127211: Pet vampires leave corpses partly eaten.
-fixed bug 127798: Eating a wraith corpse while polymorphed is buggy.
-fixed bug 128019: Player can eat corpses which are underwater.
-fixed bug 130386: Unused candles merge with partly used ones.
-fixed bug 130263: Polymorphing into a new version of yourself seems buggy.
-quick fix for bug 130857: Wrong location given for death.
-fixed bug 130957: Zapping wand of healing angers peaceful monsters.
-added FIXMEs for two bugs relating to wands of (extra) healing.
-fixed bug 131665: Wrong cause of death when cursed wands explode.
-added FIXME for bug relating to cause of death from magical explosions.
-fixed bug 132026: Transparent tiles: not updating when fountains dry up.
-fixed bug 128018: Frag grenades explode in water.
-fixed bug 132405: Increasing Intelligence.
-fixed bug 132521: #version lists the Qt windowing system twice.
-fixed bug 132163: Special properties of items show up while hallucinating.
-fixed bug 132033: GTK/win32: Esc not working.
-fixed bug 132029: Pseudo 3D tiles not suitable for menu images.
-fixed bug 132027: PgUp and PgDown not supported in scrolled menus.
-fixed bug 132818: Monster detection and hilite_pet.
-fixed bug 132819: Charged for wrong corpse in shop.
-fixed bug 132028: Unnecessary vertical spacing in GTK menus.
-fixed bug 133159: GTK: tile/description mapping is being lost.
-fixed bug 133666: Gdk-Critical errors.
-GTK: improved move & fight key handling.
-fixed bug 124233: Win: messages are drawn on top of each other.
-Integrated changes to cmdhelp from Mac port.
-Fixed a number of prototype problems.

ver 0.0.6E4F2 [December 30/2000] [Released by Ali]
-DOS: Allegro window port adapted to display the ground underneath walls
	(vidalleg.c)
-Win32: binaries now compiled with the Oct/2000 build of Win GTK libs
-fixed bug 122396: Bottom line slow to refresh.
-fixed bug 122397: "Eating" an inedible corple leaves it partly eaten.
-fixed bug 122966: Dwarf's rage technique should not heal as well.
-fixed bug 122999: Can't wish for The Key of Chaos.
-fixed bug 121596: GTK widget options reverting after "more options".
-fixed bug 123287: AllegroHack based tileset is out of date.
-fixed bug 124048: Antimagic traps not displayed when monster triggers them.
-fixed bug 124049: Missing "a" in magical explosion message.
-fixed bug 121723: Dialog boxes appear at the mouse pointer.
-fixed bug 123175: Vampire over-indulgement crashed game.
-fixed bug 124232: windows GTK port unable to load savefiles beyond level 1.
-vampires no longer allowed to be flame magi.
-fixed bug 123468: Inconsistent messages engraved on headstones
-fixed bug 123387: slashem-0.0.6E4F1-1 can't locate tiles for X.
-fixed bug 124064: gtk/win32 port can't display non-ASCII chars.
-fixed bug 124069: gtk/win32: Inventory screen non-closeable.
-fixed bug 123915: Bags of holding can be identified by weight.
-fixed bug 124967: Pick axe ceiling.
-fixed bug 124070: GTK port doesn't support variable width map fonts.
-fixed bug 124168: Win: Repaint after mode change.
-extended SHOPTYPE wizard mode control.
-fixed bug 123490: Drained lichen corpses?
-fixed bug 125230: Wrong weight displayed for gold in bag of holding.
-fixed bug 125472: slashem-0.0.6E4F1-1 source RPM fails on RedHat 7.
-extended heap monitoring under LINUX
-plugged some leaks for bug 124143: Win Version - Slowdown/Hangup/Memory leaks?
-fixed bug 126097: Misleading "You can't polymorph at will yet" message.
-fixed bug 123491: Extra messages when lycanthropes eat wolfsbane.
-fixed bug 124968: Vampires, helms of opposite alignment, and prayer.
-fixed bug 117420: Gauntlets of Power not functioning properly.
-fixed bug 121595: REINCARNATION defined but not supported.
-fixed bug 124920: GTK: inv_weight slow update.
-fixed bug 125228: Priest: attack spells should be at least skilled.
-fixed bug 126037: Manpage is outdated.
-fixed bug 115064: WINNT: RNG seed problems.
-fixed bug 113620: hilite_pet uses colour even if colour is off.

ver 0.0.6E4F1 [November 10/2000] [Released by Ali]
-GTK: status window stats columns should never be too small
-GTK: deactivated the unimplemented Fight command
-GTK: made the menus able to "tear-off"
-GTK: move menu works with num_pad TRUE
-GTK: message window now word wraps
-GTK: mesage window takes up all available horizontal space minus that of status bar
-GTK: map window takes up all available vertical space minus status bar/message bar
	space
-GTK: MP/HP bars are initialized to black at startup
-GTK: Default map size is now 50% of screen rather than 66%
-GTK: Screen automatically centers
-GTK: Scrollbars now disappear when map screen is large enough
-GTK: Added two more common monospaced Windows Fonts for the map
-GTK: Replaced NH_WIDTH and NH_HEIGHT in gtkmap.c with ROWNO, COLNO
-fixed bug 113053: gnome windowing port now supported
-fixed gnome multishot bonus (dothrow.c) and show_damage ability (hack.c)
-removed unnecessary "uncursed" tags for necromancer role (to match priest)
-GTK: Added graphical glyphs to the menus, switched in winGTK.h
-GTK: extended commands via menus work again.
-upgrading enhanced arrow types (elven, ya, dark elven) arrows results in 
	an arrow, not a bow (potion.c)
-saving/restoring now only enables the [...] effect in tty
-berserk technique now takes time to "recharge" like most other techs
-archeologists can become skilled at matter spells (was basic)
-fixed bug 115271: nhwin.a should be deleted by make clean
-fixed bug 115347: Guidebook incorrect re. reading engravings and conduct.
-fixed bug 113621: GTK port now supports 3D tileset.
-fixed bug 113813: Update tileset.
-fixed bug 115554: Primal roar technique can cause crash.
-fixed bug 115292: Better grammar in draining blood message (vampires).
-fixed bug 115380: Avoid critical GTK errors from status bars.
-fixed bug 114401: GTK port is no longer dependant on TTY port.
-fixed bug 115301: Discovery list now has a scrollbar and menu images.
-fixed bug 115348: Dwarf thieves "seduce" players of same gender.
-fixed bug 115656: Player can start with ring of sleeping.
-fixed bug 115471: Shopkeepers mistakenly think items are fully identified.
-fixed bug 115837: Double "your" in messages.
-fixed bug 115475: Eating identified tins should add to eaten memories.
-fixed bug 115657: Encumber messages during initialization.
-Win32 GTK port uses the new tilesets
-fixed bug 113618: Can now specify tile sets in configuration file.
-fixed bug 116500: #monster no longer panics if lycanthrope in normal form.
-Allegro DOS port verified to use the new tilesets
-Allegro: scroll_lim read properly from config file
-fixed default tileset declaration in decl.c
-fixed bug 115821: Molds grow from corpses underwater.
-fixed bug 115598: Gtk-WARNING if you kill the "more options" window.
-fixed bug 115765: Inappropriate messages when corpses go moldy.
-fixed bug 115297: Vampires can feed off monsters without penalty
-Vampire feeding revamp:
 ATTACKS
	-biting living monsters gets appropriate penalties/benefits
	-don't bite monster if it would lead to death
	-drain lifeblood (6 nutrition points per HP drained)
	-does not count against conduct
 EATING
 	-now multi-turn occupation
	-corpses marked as (partly) drained
	-can continue draining if blood hasn't coagulated
	-can't drain partly eaten corpses
	-Vampire race gets 20% chance of cpostfx()
	-Vampires are exempt for cannibalism penalties.
-fixed bug 116403: Pets can follow you into the Black Market.
-fixed bug 116356: Lighting one of a stack of candles is buggy.
-fixed bug 116587: Molds can have HP above their max HP.
-fixed bug 117371: Wands of create horde should auto-identify.
-fixed bug 116266: Grenades carried by monsters set HP to 2d5.
-fixed bug 117480: Amulets versus stone last forever.
-fixed bug 115210: Kiii technique can be used again immediately.
-fixed bug 119758: Various interactions of moldy corpses and ice.
-fixed bug 115764: Attempting to eat inedible corpses should not affect conduct.
-fixed bug 117663: Weight of partly eaten corpses wrongly displayed.
-fixed bug 117483: Lycanthrope's berserk behaviour doesn't work.
-fixed bug 115779: Bottom line doesn't show all information.
-fixed bug 114768: GTK character mode doesn't support IBMgraphics.
-fixed bug 121216: Crashes when kicking a stack of objects.
-fixed bug 116922: Bug with vampire race and self-polymorph.
-fixed bug 115780: Missing messages when grenades explode.
-fixed bug 115908: Spurious "the" in messages about invisible mplayers.
-fixed bug 119480: Error: Tech already known (berserk).
-fixed bug 121719: Options in defaults.nh are not supported?
-fixed bug 115822: Elves are peaceful to drows.
-fixed bug 119856: GTK port takes a turn to set options.
-fixed bug 114646: Role-selection broken.
-fixed bug 122139: GTK interface doesn't support FILE_AREAS.

ver 0.0.6E4F0 [September 15/2000]
-changed some checks in tech.c to use DEADMONSTER()
-updated Guidebook
-fixed bug 112478: game crashes whenever the Amulet of Yendor is referred to
-fixed bug 112489: #twoweapon while no secondary weapon seg. faults
-fixed bug 112520: game crashes when beams reflect diagonally
-the ')' command now prints varying messages depending on twoweap status
	and mentions empty hands if needed. (invent.c)
-Monks starting with the sleep spell no longer get warnings on startup (u_init.c)
-the "use #quit to quit message" only occurs if you comment out suppress_alert
-time before next use shows up in wizard mode tech menu (tech.c)
-added the use of technique limit breaks.  When HP drops below 10%,
 there is a random chance each turn that you are in LIMIT mode. In limit mode,
 all your known techniques are useable (even the ones usually Not Prepared)
 (tech.c)
-fixed bug 112553: GTK interface failed to compile
-fixed bug 112554: UNIX/VMS default config file was wrong
-fixed bug 112566: Qt port 3.3.1 integration was broken
-fixed bug 112616: Dazzle in a direction with no adjacent monster crashed
-fixed bug 112545: Gnomish mines generated without stuff
-Removed X11 dependencies from GTK port (switched in winGTK.h)
-fixed bug 113031: Qt port did not support showweight option
-fixed bug 112789: GTK port now supports perm_invent option
-fixed bug 113396: X11 port was crashing when displaying rip.xpm
-fixed unlogged bug: X11 port wouldn't compile if USE_XPM wasn't defined
-fixed bug 113523: Couldn't compile if UNPOLYPILE not defined
-fixed bug 112108: can now select between human and hobbit archeologists
-fixed bug 112614: rage eruption no longer drains energy
-fixed bug 112616: dazzle no longer causes segfault, has limited range
-integrated changes from Allegrohack 1.5
-fixed bug 113366: GTK: Checking your scores before starting a game panics
-fixed bug 113947: can't compile without #define FIREARMS
-alpha compile of win32 with gtk windowsystem.
	-prompts for name (gtk.c) 
	[possibly change to be more general than just WIN32?]
	-win32 port uses different fonts (but can't draw walls properly :P)
-GTK: barebones error checking before destroying window widgets (gtkmenu.c)
-GTK: directional dialog works with number_pad true
-GTK: hitting 'enter' while in a yn dialog chooses default choice

ver 0.0.6E3F1 [August 21/2000] [Released by Ali]
-fixed some bugs in the zap/cast code for magic missile, fire, cold, fireball, etc.
	-tmp_at was being called incorrectly (new tmp_at handles nested tmp_at calls)
	(zap.c, explode.c, display.c)
-changed all the file names in global.h to be prefixed with NH_
	(DATAFILE, HELP, RECORD, ORACLE, RUMOUR, etc.)
-changed #define FILENAME (macconf.h, pcconf.h) to FILENAMELEN since FILENAME 
	is #defined by dir.h, used by Allegro WIP
-fixed dereferencing NULL pointer when monster throwing items
	(mthrowu.c)
-did the uppercase for conflicting races - TODO: Reimplement F. Theoderson's 
	selection system or something similar while maintaining current 
	capabilities (wintty.c)
-fixed status bar in Allegro modes when screen Height was not 480.  Initial 
 status bar was drawn correctly, but updates were incorrectly printed.
	(wintty.c)
-cleaned up the DOS makefile to allow easy building of TTY+VGA+Allegro binary
	(makefile.gcc)

ver 0.0.6E3F0 [August 12/2000]
-Nethack 3.3.1 Integration Preliminary done
	-flag.h - remove possibly unused vars initrole, etc...
	-changed position of TREE ...
	-eating tripe rations may have problems
	-uwep_skill_type returns P_TWOWEAPON for u.twoweap
	-increase penalties for two weapon combat
	-possible light source not removed problem in bhit() (zap.c)
	-*.rej files left over from incomplete merging of the diffs
		(i.e. the non-trivial changes)
		-notably all the system/window ports I can't compile myself
-technique power surge == rage for energy
	-tempered rage technique to give less HP bonus
	-given to flame mages, necromancers, ice mages,  wizards and monks
-monk technique - blitz, ground slam, pummel
	-can chain together various techniques to do more damage
	(tech.h, tech.c)
-vampire technique - dazzle
	-vision based freezing of an enemy (depends on relative level of tech
		and enemy) (tech.c)
-should be able to #twoweapon in polymorphed forms (wield.c)
	-can now get 6 attacks if #twoweapon and a marilith
-new ghoul, ghast, larva 32x32 tiles from Andrew Walker
-can now take screenshots in 8-bit color mode (vidalleg.c)
-you can now force 3d tile drawing (so you can resize the tiles to 32x32 and still
	draw in 3d mode) (alginit.h)
-substitute tiles are now resized correctly for 3D mode
	(no more giant female priests) (alginit.h)
-reduced memory consumption for the minimap ;) (alginit.h)
	(80x24 pixels, not tiles :)
-preliminary support for 2xSaI - thanks to Kreed for source code
-prevented some unnecessary screen mode changes in the Allegro code
-silly bad merging with werewolf summoning brethren code (polyself.c)
-fixed a bug with help of someone whose e-mail message got lost
	-raising zombies no longer causes odd domination messages (tech.c)
-some modifications that enable better graphics mode switching
-Integrated AllegroHack 1.4 tiles and code
-hopefully fixed incorrectly crediting player for monster thrown grenades
	(explode.c)
-some types of partially identified objects weren't recognized as not fully 
	identified by shopkeepers (shk.c)
-cancellation of mind flayers no longer disables their brain eating (mhitu.c)
-quaffing potions of gain energy no longer allows current energy to go above max
-DOS binaries now building with DJGPP 2.03
-Some modifications courtesy of JRNieder
	-additions/rearrange rumours.tru/fal
	-cleaned up commenting in pcvideo.h, makedefs.c
	-added comment when building allegfx.h (file is obsolete) (txt2bmp.c)
	-cleaned up makefile, made it easier to config (makefile.gcc)
-moved status bar manipulations out of botl.c and into vidalleg.c, wintty.c
-removed exclamation point in "you are celibate"
-when you are a vampire and you get hit by a polymorph beam you become a vampire
	bat (polyself.c)
-sanity checks for repeat_hit, no repeat_hit chain ever goes above 10.
	(cmd.c)
-lose berserk status when you retract your claws (tech.c)
-can no longer purify oneself with sprigs of wolfbane when a werewolf
	-causes hp loss, etc but you are still a werewolf
-updated credits with Jonathan Nieder (for icon in win32 exe and more I don't 
					remember :)
-basic 8bit color mode support
	-known issues - fade out on death doesn't work properly
	-various color blending problems
-all the files in the tiles directory are now 8-bit
-updated rog_fem.bmp in tiles/subs from allegrohack 1.31
-fixed vampire bite attacks drinking from golems, undead
	(drinking from other vampires is still ok)
	(eat.c, uhitm.c, mhitm.c, mhitu.c)
-monsters attacking other monsters only get less hungry during drain life attack
	only if attacker is a vampire, defender has blood
	(mhitm.c)

ver 0.0.6E2F1 [July 2/2000]
-zap.c mtmp->mtame dereference without check fixed (courtesy of r.r.g.n)
-merged in allegrohack 1.3 tiles 
	-(obj-alg.txt, oth-alg.txt, mon-alg.txt, makefile.gcc)
	-merge_colormap now does a fuzzy merge if there are too many colors
		-checks for the "closest" color
			-reduce the max of the pixel value differences
			-then minimize total of pixel valu differences
	*cmap remap the explosions
-magic candles are now the same as regular candles in 32x32 tilemode 
	(objse.txt)
-the slam??.bmp files are now 8bpp .bmp files as they use less than 256 colors
	(much smaller files!) (txt2bmp.c)
-vampires have 1/5 chance of having a corpse effect (gaining intrinsic, etc.)
	when drinking (eat.c)
-vampires who drink are no longer vegetarian, vegan (eat.c)
-auto checks for slam3d, slam32 and slam16 tiles
-nh2k tiles merged in (mon-2kmi.txt, etc.)
-3D tile support
	-xputg places the tiles properly
	-clipping, minimap, cursor work modified to work properly
		-optimized xputg 
	-overview mode not available with 3D tiles
	-walls that cover tiles are now drawn semi-transparent
	-fadeout on death centers properly (approximately)
	-swallowing works properly
	-explosion centered properly
	-uspellprotection works properly
	-spell_aura works properly
	-shieldeffect works properly
	*Optimize tile_subscreen to actual size
-allegrohack should work under pure dos (alginit.h, vidalleg.c)
	-fixed dereferencing null pointers in the init, xputg functions fixed
-fixed bug where slashem wouldn't load extremely large tiles due to progress
	bar being too big - progress bar is now completely screen size independent
	(alginit.h, vidalleg.c)
-started work on bmp2txt,  but that's probably going to be pretty hard
	-easier to use a bmp2gif program,  then use gif2txt
-merged in chnages from allegrohack 1.31 (minimap+hud combo mode) 
-idx2bmp.exe generates a "large bmp" from an index of smaller ones (idx2bmp.c)
-use textout without stretching to speed up traditional tile mode (vidalleg.c)
	-support "traditional mode"
	-redraws properly
	-seperate subscreen for tiles and text (alginit.h, vidalleg.c)	
-progress bar is now a fixed size (alleginit.h, vidalleg.c)
-alleg.cnf now supports specifying tile files, tile_width and tile_height
	(alleginit.h)
-vidalleg.c now supports (hopefully) any size tile (reasonably speaking)
-tile2bmp is now txt2bmp (since that is more accurate)
	-Now processes backgrounds properly on dragons and some other tiles 
-can now build a '32tile.bmp' that holds all the tiles simultaneously
-changed alleg_traditional(boolean) to the more informative alleg_screenshot()
-tile2bmp.exe creates the index, allegfx.h and *.bmp
	-select behaviour using -i (index), -h (header), -f (files)
	-no need to redirect output - all relevent output goes to files directly
-fixed tile2bmp.o not building automatically (depends on portio.h)
	(msdos/makefile.gcc)

ver 0.0.6E1F4 [June 21/2000] [Released by Warren]
-default (DOS) makefiles are set up to now build TTY-Allegro
-add in AllegroHack support (ifdef ALLEG_FX)
	-detect function now detects whether it is possible to enable Allegro
	-pcvideo.h contains prototypes
	-can now compile a binary that can use tiles VGA, tiles allegro and tty
	-video:autodetect and video:alleg enable allegro
	-video:vga enables the old tiles
	-no video:foo gives the tty interface
	-To compile ALLEGRO, enable it in the makefile and pcconf.h
	-basic support appears to be working after some minor alterations
		-src (botl.c, display.c, end.c, explode.c, spell.c, makefile.gcc)
		-msdos (alfuncs.h, alginit.h, alnames.h, loadfont.h, pckeys.c, 
			tile2bmp.c, vidalleg.c)
	-botl.c - why split the botl here? can it be done separately?
	-Need tiles in \sys\msdos\tiles as well as autogenerated tiles to create
		distrib
	-tile2bmp.c changed work with new tile code
		-uses magtile and txtmerge
		-all moving, etc handled in makefile
	-the following should no longer cause SEGFAULTS 
		(which drove me up a wall hunting down)
	 if not present
		-must have anethack.fnt in the gamedir (fixed)
		-must have tiles directory
		-must have all the monster, object, other tiles in the tiles directory
		-must have upt-to-date index in the tiles directory
		-must have other misc tiles (credits, etc) in the tiles directory
-mind flayer pet attacking you now becomes less hungry (mhitu.c)
-player polymorphed as koala can only eat eucalyptus leaves (eat.c)
-fixed bug where you don't use your kick attack on monsters when attacking 
	without a weapon (uhitm.c)
-vampires as player class
	-level drain== eating (uhitm.c, mhitm.c, mhitu.c)
	-drinking blood from corpses
		-need to eat the corpse within 5 monstermoves of the kill
		-takes one turn
		-you get 1/5 normal nutrition, but you can still eat the rest
		 if you poly into normal carnivore
		-same for pets (dog.c, dogmove.c)
	-bonus strength, charisma, dexterity, constitution, 
	-no bonus to intelligence or wisdom
	-high max strength, charisma, dexterity, constitution,  
	 normal max intelligence and wisdom
	-lower starting hit points,  but raises quickly
	-starts off with a -5 alignment penalty, -1 luck penalty
		(from original values)
	-is_vampire() macro now used (catches vampire bats)
		-not having a reflection (apply.c)
		-creating vampires (end.c)
		-take extra damage from stakes (mhitm.c, mhitu.c, uhitm.c)
		-afraid of altars (monmove.c)
-ghouls and ghast are carnivores 
	-when you poly into them,  you still get hungry 	
		(side effect of being carnivorous)
	-only eat old corpses - synchronized with dogfood (dog.c, eat.c)
-fixed possible bug in dogfood() for cannibilism (eating same letter class)
	-EDOG struct was used before checking has_edog
-not-so-preliminary work done on WinNT with GTK (src/winnt/makefile.gcc, tiletext.c)
	-I've gotten the tile creation code working, all that remains is getting the
		gtk window-port code working
	-seems that some X11 dependencies remain :P

ver 0.0.6E1F3 [June 13/2000] [Released by Warren]
-NH004 Fixed panic caused by monster summons failing. 
-fixed some corpses randomly reviving (mkobj.c)
-fixed spelling error - "bloodcurdling roar" (tech.c)
-toilets seen by ':' look command (invent.c)
-reworked the cost of #youpoly for the flame/ice mage
	(minimum level, energy)
	-no special armour:
		adult form  level 13 and 15 energy
		baby form   level 6 and 10 energy
	-scales
		adult form  level 6 and 10 energy
		baby form   level 6 and no energy
	-scale mail
		adult form  level 6 and no energy
		baby form   any level and no energy
-fixed bug in #youpoly where you broke out of your armour when you should have
	merged with
	(polyself.c)
-WIN32:  icon is now embedded into the exe
	makefile, slashem.ico in sys/share, slashem.rc
	Courtesty of Jonathan Nieder
-fixed bug where projectiles that are their own launcher 
	(spears, daggers) had incorrect range and rate of fire
	(dothrow.c)
	
ver 0.0.6E1F2 [April 23/2000] [Released by Ali]
-SE065 Fix broken FILE_AREAS support in GTK interface (Fred Richardson). 
-SE066 Put tile.o into a library for ease of configuration. 
-SE067 GTK niggles: yn_function confusion, showweight option, slower animations, setuid
-operation, gender buttons, !color support, segmentation faults, spurious gtk errors, score
	option, ... 
-SE068 Fix broken tinning code (Maciej Kalisiak). 
-SE069 Honour horsename option. 
-SE070 The adjective for the hobbit race is hobbit and not bobbit! 
-SE071 Fix database entries for Shelob and Stormbringer. 
-SE072 Provide Slash'EM specific example slashem.rc for X11 interface. 
-SE073 Don't give miss messages for weapons if you've chosen not to attack with them. 

ver 0.0.6E1F1 [April 23/2000] [Released by Warren]
-fixed a minor bug in tile thinning (DOS port affected only AFAIK)
	-a similar bug in the tile magnifying code is also present, but has no 
	 effect since currently tile width and height are the same
	 (magtile.c, thintile.c)
-now uses nhshare, nhushare for shared and unshared files (makefile, dlb.h)
-DOS now uses nhshare and nhushare instead of nhdat
-fixed a minor bug - if a monster is drained below zero energy, YOU lose max energy
	rather than the monster (trap.c)
-fixed a crash bug when monsters get power drained 
	-some RNG can't do RND(0)
	(uhitm.c, mhitm.c, trap.c)
-dipping a potion of acid while flying should no longer cause a crash
	-tried to useup() the potion twice (potion.c)

ver 0.0.6E1 [April 13/2000] [Released by Ali]
-SE040: Fix X11 interface Slash'EM/3.3 merge problems
-SE041: Use new format (3.3) record & logfile entries and understand old format
	(0.0.5) entries
-SE042: Fix bad error message when eggs hatch from pack
-SE043: No longer crashes when throwing darts, daggers etc. when not wielding
	a weapon
-SE044: New Qt interface (v2.0.0) from NetHack 3.3.0 
-NH002:	Add dialog on closing Qt interface main window
-SE045: Add GTK interface from JNetHack v1.1.4
-SE046: eraseoldlocks() now supports file areas
-SE047: Tiles can be displayed by the Qt interface at up to 64x64 pixels
-SE048: Add support for 256-colour tile sets
-NH003: Fixes panic/crash if polymorphed objects are placed in a container or
	are carried by a monster
-GTK002:Allocate read-only (shareable) colours rather than writeable ones
-SE049: Add support for Mitsuhiro Itakura's 32x32 tile set
-SE050: Installer can now choose which tile sets to build simply from the top
	level Makefile
-SE051:	Add support for tile sets other than 16x16 to Qt interface
-SE052:	Remove ROLE_PATCH configuration switch and fix duplicate entries for
	role, align and gender in the options list
-SE053:	Add GTK interface to the list of supported windowing systems produced
	by the #version command
-SE054:	Remove inappropriate references to "NetHack" rather than "Slash'EM"
-SE055:	Remove redundant include/Window.h and include/WindowP.h
-SE056:	Add ability to wish for visible objects
-SE057:	Upgrade GTK interface to version 1.1.5
-GTK003:Honour menu mapping options (eg., menu_select_all)
-SE058:	Seperate YEOMAN and STEED configuration switches
-SE059:	Don't compile redundant quests: Dopp, Dwarf and Elf and restore hobbits
-SE060:	Bring guidebook up to date
-SE061:	Upgrade GTK player selection for 3.3 codebase
-SE062:	Fix a number of niggles: nested comments, README.33, Qt in #version,
	obsolete pet name options, duplicate gender option, glyph confusion
-SE063:	Add Slash'EM specific commands to GTK menus
-SE064:	Remove redundant WEAPON_SKILLS configuration switch

ver 0.0.6E0 [January 15/2000]
-fixed a fairly significant saving bug.  (restore.c)
-Mingw32 compiled version.   No mail capabilities.
-incorporated SE033, SE034, SE035, SE039
-cleared up some molding problems (theoretically) (do.c)
-stores will always have at least one service (shknam.c)
-the "HD" of your assumed lycanthropic form depends on your experience level
	(botl.c, polyself.c)
-you now suffer silver damage, etc. if your race is werewolf in human form
	and also when you get infected (but still in human form)
	(u_init.c, polyself.c, restore.c, mhitu.c, were.c)
-reimplemented Blink (allmain.c)
-kicking green mold no longer causes damage to weapon (dokick.c)
-redid the technique code - rage eruption no longer causes massive HP loss when
	you have speed ;) (tech.c, timeout.c)
-fixed up odd race/role combinations (eg. elven necromancers) (role.c)
-fixed bug involving drinking potions of (foo) healing when at max HP (potion.c)
-mindless monsters aren't afraid of attacking higher level opponents (monmove.c)

ver 0.0.5E9 [January 6/2000]
-fixed a bug involving Drows, Doppelgangers gaining levels
-Remove obsolete references to uhpbase, uenbase
	-removed extra energy bonuses from classes and races!
-elves are now lawful/neutral, drows are chaotic
	-drows start with dark elven bows, arrows
	-most elven equipment is now wooden
-fixed skills for casting spells (no more crashing as well)
-changed charging of spellbooks so that they crumble rather than explode
-incorporated the new wintty.c code from Finn Theoderson
	-modified to use it also for race selection
-statues no longer rot
-wizard mode identify and mapping should work as expected
-Yeomen don't start with a saddle (it's already on their pony)
-reenabled KEEP_SAVE and SCORE_ON_BOTL
-fixed up sokoban level names in dungeon.def.  Had to increase LEV_LIMIT from 50 
	(made it 99) since there were too many special levels!
-fixed up dungeon.def with new sokoban
-undead slayer is now before valkyrie on role selection list
-fixed "invisible invisible" objects
-fixed doubled options - pushweapon, role, gender, align, autoquiver, pickup_burden
	-pickup_burden should work like it does in vanilla

ver 0.0.5E8 [December 20/1999] [NH330 integration version!]
-hobbits should now be races... 
-fixup quest artys - artilist.h
-change 
	/* #define foo /* remark */ 
	to 
	/* #define foo */ /* remark */

-oddity - u.umonnum is set to your ROLE not your race.  Which makes checks to 
	youmonst.data (which is set to &mons[u.umonnum] when !Upolyd) give odd
	results when checking for whether you are a were (or an elf, a dwarf...)
	-should make u.umonnum = your race monster number (fixup affected code)

-made race selection do the uppercase lowercase thing to resolve clashes
	(doppelgangers vs dwarves)
-doppelgangers have been added
	-fixed doppelgangers changing back to normal form
-amulets of unchanging should no longer polymorph
-moved around M2_WERE to allow for lycanthropes
-weapon enchantment helps in #forcing chests
-Mantle of knowledge disabled...
-termcap.h is now tcap.h
-fixed difficulty in generating vampires with opera cloaks under WinNT
-fixed explosion bug
-fixed minor problem with file areas

ver 0.0.5E7 [November 30/1999]
-setting twoweapon without a secondary weapon while having greasy hands caused a 
	crash - fixed :)  (wield.c)
-polymorph untameable - then make tame?
-use mfnpos to set allow_m for mon to mon attks? (only doable when you're not nearby)

-should now be able to restore while mounted on a steed. (restore.c)
-monsters now have energy:
	-mindless monsters don't have energy
	-monsters regenerate energy (dependant on level of monster)	
	-pets/others gain power along with HP
	-Pw shows up on probing
	-monsters use up energy when casting spells
	-affected by drain energy attacks, by magic traps and antimagic traps
-wishing for grenade, gun, firearm now gives out a random weapon of that class
-assault rifle now has 3 fire modes - burst, auto, single shot. Burst is 1/3 normal.
	-apply the rifle to change modes
-added stick of dynamite.  Has ~15 turn fuse.  Extremely strong explosion.
-There are now messages "Your foo blocks foo's attack", etc. for monster vs monster
  you vs monster, monster vs you (when verbose option is TRUE)
	(mhitu.c, uhitm.c, dokick.c, mhitm.c)
-You get the disarm tech when your ability in a combat skill reaches Skilled. 
	(weapon.c, tech.h, tech.c)
-monks get Chi Healing
	-converts chi (power) to health for a short period of time 
		(tech.c, tech.h, allmain.c, timeout.c)
-most "error messages" for techs no longer cost a turn 
	(unless information is given) (tech.c)
-Most spellcasters (except Necromancers) have a new tech - draw energy
	-gets energy from surroundings
	(tech.c, tech.h)
-Necromancers also detect bless/curse status of objects (invent.c)
-throwing a single item should no longer cause a crash (invent.c)
-incorporated variant of SE023.txt (being able to throw welded objects)
        (dothrow.c)
-now prompts if you only want to light a single candle rather than assuming you 
	want them all lit if several candles are merged (apply.c)
-fixed buglet involving dragged down stairs by an iron ball and having 
	quivered objects follow you 
-invoking Sunsword, Holy spear of light will burn nearby undead and demons 
	-causes 50% monster HP damage, no outright killing though.
	(artifact.c)
-stairs are bright white (to stand out),  unlit corridors are darker 
	(differentiate b/w lit/seen and unlit corridors)
	(Kelly Bailey)
-definitions from Dave and Edward Walker
-a little bit of randomness in the movement code
-prevent generation of multiple arkenstones (artifact.c)
-fixed problem with generating gold on Yeoman Quest Home (sp_lev.c)
-Kelly Bailey found out why the hitpoints weren't always being updated properly!
-molding.  Molds will tend to revive like trolls. ~50% chance of normal corpses 
	becoming "moldy" after a while.  Hopefully, this will act as "emergency 
	food" for beginning adventureres,  although most fungi cause 
	hallucination.  Could generate more shriekers == more purple worms.
	Let me know if resurrecting molds becomes perilous to adventurer's health
-removed debugging code in msdos.c
-ethic:  chastity.  Invalidated by encounters with *ubbi.
-Added an X11 workaround (SE022)
-Added Ali's workaround/ESC callable wizard menu,  fixed some obscure bugs
	    1. If a lycanthrope who is currently eviscerating (if
               that's a word!) attempts to apply a pick axe, which
               also happens to be his secondary weapons, then
               he will be allowed to dig even though he can't
               retract his claws. A segmentation fault may then follow.
            2. Dipping Sunsword into a potion of uncursed water will
               cause it to be used up while dipping a long sword will
               not.
            3. If you're mounted on a frozen stead while levitating
               or flying, time stands still for it unless you get off.
               Thus it will never respond.
-autoquiver looks for spears now 
	-rocks and like are not valid unless you have a sling
-dice rolling counted separately for each weapon in 2 weapon combat
	-all known problems with 2weapon offhand artifacts have been dealt with 
		AFAIK
	-PLEASE let me know if there are any more issues to deal with ;) Warning
		has been removed
-throwing grenades/bullets up/down results in regular throwing, not firing the gun,
	arming the grenade, etc...
-Fixed game getting caught in an infinite loop when wands wands of digging are 
	caused to explode by wands of lightning AND you fall down a level.
	Game kept trying to save the same light again and again and...
	(light.c)
-Monks get a new Tech Chi Strike - for a couple turns,  you get extra hand to hand 
	dmg (this is dependant on how much power you have left)
-vampires now get opera cloaks ;)
-grenades now have a small timeout before exploding
	-partial implementation of remote explosions
		(drop grenade, leave level, come back later)
		-Let me know if odd messages happen when you come back
	-apply arms a grenade
	-can't put armed grenades in containers
	-grenades armed when thrown
	-cursed may or may not arm, have more varied timeout
	-blessed exactly 3 turns 
	-uncursed slight variation (+-1)
	-direct hit == explosion,  otherwise timer takes over	
	-arming an unpaid grenade makes it yours (you're billed for it)
-added grenade launcher
	-fairly inaccurate (-3 to hit),  slow to reload (-3 penalty)
	-fires grenades
-One-Eyed Sam will get all those reinforcements he calls for... (shk.c)
-can no longer level teleport when riding a steed a steed that's carrying the amulet
	of yendor. Pretty sneaky trick ;) (teleport.c)
-TTY colormode - walls of gnomish mines, knox are brown, hell is red.
-code will compile again with FIREARMS commented out - I have to stop breaking this!
-DOS, VGA mode - now has a little "heart petmark" seen in X11
		-set hilite_pet to see it ;)
		-and now I know what a pain it is to program video adapters ;)
-tech:  
	Blink for Hobbits
		freezes all monsters for a short period of time.
	Rage Eruption for Dwarves - 
		You gain hp and maxhp,  but it all bleeds off one pt/turn at a time
-Finn Thoederson - fixed up monsters coming out of toilets
-Karl Gerrison - fixed some prototyping/type buglets.  Win32 compile!
-fixed mistake involving putting some but not all of a wielded/quiver/secondary 
	object into a container.  (Eg.  putting 5 out of 10 cursed wielded arrows
	into a bag results in two entries of 5 cursed arrows -  both wielded!)
	(pickup.c)
-lightsaber "melting through" door now has correct finishing statement 
	(lock.c)
	(thanks marvin!)
-someone on the newsgroup (lost your name!) mentioned that a period is missing from
	cancelled spitters...
	(dothrow.c)
-fixed buglet that was miscalculating strength of an object when prying open doors,
	causing crashes (lock.c)
-fixed possible bugs with monsters firing slow firearms 
	(shotgun, rocket launcher, sniper rifle)
	(mthrowu.c)
-monsters will toss grenades (weapon.c)

ver 0.0.5E6 [October 25/1999]
-all polymorphed objects will now revert unless "fixed"
-fixed bug where you got many more objects after polypiling
-can now wish for Ogresmasher (I'm not really sure if you'd ever WANT to, but 
	that's another question entirely ;) (objnam.c)
-the foobane weapons will cancel with a dieroll of 2 or 3 (artifact.c) [Adam Clarke]
	includes:  Orcrist, Elfrist, Ogresmasher, Werebane, Dragonbane, Thiefbane
		Demonbane, Trollsbane, Giantkiller
-Ralf Engels - added more special cases of extra dmg  (weapon.c)
	-polearm vs flyer
	-cutting worms
	-piercing blobs
	-slashing jellies
-can use counts in the throw command object prompt to control # of projectiles thrown,
	can use count directly on the fire command to do the same
	-ie 
		"n 1 f" will fire 1 arrow
		"t 3 a" will attempt to fire 3 of item a
		"3 t" will still go into 3 consecutive throw prompts 
			(should this change to act the same as fire?)
-gas, frag grenades added
-Added (Thanks J. Ali Harlow ;)
    SE007: Window resize code fixed 
    SE008: Linux colour problems solved 
    SE009: Incompatability between X11 port and GNU libc removed 
    SE010: Fix monsters' multishot ability bug 
    SE013: Now compiles with YEOMAN commented out 
    SE014: Better error message if non-XPM tile file cannot be opened 
    SE015: X11 port without tiles no longer causes divide by zero -finished removing remaining spell skill errors on startup by giving Dark Elves
	specified spellbooks.
-master mindflayer, mindflayer recognized as alternate spelling of mind flayer
-added auto shotgun, assault rifle, sniper rifle
	-these,  like most of the specialty weapons, are VERY expensive
	-need to be balanced, most probably
-added rocket launcher
	-okay,  this thing weighs a TON (750) .  Plus rockets (200).
	-Big ouches for getting hit.  (d45/d60).  
	-Rockets explode for 6d6 dmg
	-captains might carry
-added heavy machine gun.  Monsters should use guns properly.
	-Monsters were throwing (?!) bullets even though message said "fires"
	-Heavy machine gun has -4 to hit,  but eats ammo like a house on fire ;)
	-Higher ranking soldiers may carry the heavy machine gun
	-bimanual.  Weighs a bit less than a chest (500)
-Qt compilation instructions included,  minor bugfixes (Ralf Engels)

ver 0.0.5E5 [October 18/1999] 
-shopkeep code - Black market has all shopkeep services
-fixed nasty little sacrifice blessing error 
	-thanks to Bruce Cox,  who somehow hasn't been mentioned in this file yet?!
	==>BTW, if you've ever submitted something and aren't mentioned here, bug me!
-shocking/freezing/flaming spheres are now M2_NOPOLY
-ring of polymorph control will resist polymorph
-fixed Elf starting spellbooks
-kicking objects at monsters should no longer cause seg faults
-should compile with FIREARMS commented out
-fixed tilemap.c exclusion code 
-fixed monster multiwep firing code
-frozen steeds really won't move anymore!

ver 0.0.5E4 [August 14/1999]
-patches for linux
-will stop twoweapon if you wield a bimanual tool
-reworded some of the twoweapon stuff
-fixed bugs with untrapping rust traps
-untrapping fire traps with water into potions of oil. Oil will explode.
-updated makefile.nt - winnt should build properly ;)
-can no longer force without weapon
-rust trap disarming into fountain should work properly
-option menu_on_esc - show menu on hitting esc?
-all liches are now g_nocorpse
-can now untrap rust traps to fountains
-fixes to wintty.c, winmap.c
-confused steed will move randomly, frozen/sleeping steed won't move 
	(when you're riding)
	(hack.c)
-wallet of perseus no longer #name-able

ver 0.0.5E3 [July 27/1999]
-removed some floating point math operations (spell.c,  mhitu.c)
	-monster with weapon missing
	-failing to cast a spell so Blackmarket has all services
-fixed drinking off floor - used to be you could only drink off the floor 
	when floating above it!
-doc/guidebook.mn fixed typo slash'emoptions -> slashemoptions
-util/levcomp.l now has a %n of 1000 
-sys/unix/makefile.dat now compiles the hobbit.des
-objects.txt has been cleaned up ;)
-can no longer light potions of oil underwater
-golems no longer regenerate.  Now have 2x the HP to compensate. 
	(monmove.c, makemon.c)

ver 0.0.5E2 [July 18/1999]
-OOOPS!  Forgot to remove the debugging code that put lightsabers in all the 
	humanoids' inventories (for a while,  monsters wouldn't pick up lightsabers)
-shotgun, rifle added. Shotguns take shotgun shells, are short ranged, 
	do lots of damage, +3 tohit.  Rifles have longer range, +1 tohit.
-zruty has been moved to yeti (green)
-yellow jelly is now yellow
-Gnome technique typo fix - nible to nimble
-only quest artifacts are now blast for "wrong class"
-shopkeepers premium identify now fully identifies, basic only partial id (name only).

ver 0.0.5E1 [July 16/1999]
-#2weapon (ie #2) extended command (same as twoweapon, but quicker to type)
-showweight only does status line weight display
	, invweight does inventory/pickup weight display
-monsters that wield weapons now pick up weptools (muse.c)
-black mold no longer moves (monst.c)
-lit objects thrown down hallways have short-range light effects 
	(zap.c)
-Arkenstone luck working properly (artilist.h)
-monsters striking with non-weapons do piddle damage (1-2) (mhitm.c, mhitu.c)
-you know contents of tins you make yourself (eat.c)
-starving herbivores will no longer eat their own kind (dog.c)
-lightsaber code reworked
	-burn engraving only works with lightsaber on.
	-bashing messages
	-no more Force flowing business when wielding
	-lightsabers are now green,red, blue
	-apply to turn them on
	-deactivate when they hit the floor
	-deactivate after a turn if unwielded
	-lightsabers are a separate skill
	-damage has been increased - they all have the same average damage
	- -3 to hit,  one-handed 
	-charge lightsabers via scrolls of charging
	-red double lightsabers  -4 tohit,  double the dice damage of red lightsabers
		-can also be used in single blade mode 
			(same damage as red lightsaber, but tohit still -4)
	-cursed lightsabers may randomly deactivate,  harder to activate
	-can force chests with lightsaber (but you might end up cutting the chest 
		to pieces by accident ;)
	-#force command to open doors, walls
	-monsters wield and use lightsabers
-#force allows bashing/prying open doors with weapons (use . to force chests)
	-pickax will dig normally through walls/doors 
		(but can bash chests with #force)
	-axes can chop through doors
-minor lighting code cleanup (light.c)

ver 0.0.5E0 [July 07/1999]
-fixed minor pager printing bug -- Myers (pager.c)
-you no longer learn the contents of tins you discard (eat.c)
-sleeping unicorns don't catch gems (dothrow.c)
-spheres are now sleep-resistant (monster.c)

===>1.3 Other Ideas
-------------------
More things.   These are probably long-term goals...

-make all of slash'em ifdef! Probably not until Nethack 4.0 comes out ;B
-christmas! New Quests?
-trap doors lead to same place?
-Different sized levels (for custom levs)
-more quests (randomized - wipe out the xxx, save the xxx, etc.)
-scoring bonuses for /w pets based on # turns?
-OPTIONAL haggling
-pool of water, psi bolt, destroy armour (directable <g>)
        confusion, curse items
-Cleric:  insects/snake summon?, blindness, wounding, hold
-Drain life spell for Necromancer?
-more holidays, special days? (Thanksgiving, Christmas)
-level generator problem of 2 special levels on same level one of them
                unavailable - in the .des file?
                -due to .des file - overlapping branches
-"you caitliff"  message from knights final blow
                try knights vs mimics
-change dungeon map size (global.h ROWNO, COLNO)
-new map types?
-arena
-library
-polymorph abusive?
-training hall
-bar? hiring "pets"
-food with different timers in same slot?
-holy hand grenade of antioch?
-pets getting intrinsics/stun from corpses?
-"i see nobody there" when chatting + blind
        -should still be able to chat?
-various different types of lycanthropes?
        (u_init.c)
-do splash directions for oil
-burning fires
-eating...stop...attack gelat cube...frozen ... finally finished msg?
-score list:  average score,  by player,  total score, etc., by class
        (topten.c)
-not averaging age of food on pickup? (invent.c)
-various were types for Lycanthrope?
-shopkeepers blocking mons w/ pickaxes
        -defending players in shops
-watchmen defending players in towns?
-martial artists being able to catch projectiles at Expert skill?
        shield deflect shots?
-shopkeepers repair pits?
-druid, illusion
-gnome animal affinity, go through cracks?

from balance.txt
~~~~~~~~~~~~~~~~
The following will probably not be implemented until the command patch.

Each role will have designated first and second artifact gifts, and they
will be as easy to get as in NetHack.  The first gift should be a weak
artifact weapon or a non-weapon; the second should be a strong artifact
weapon.

# A different strategy could be used to make it harder to get a
  strong artifact.  Instead of altering the algorithm used for
  wishing and sacrificing in NetHack, more weak artifacts could be
  placed in well-defined places, making it harder to get an
  artifact later in the game.

# Spellcasting classes should get at least basic skill in all of the
  spell skills.

I should examine the newer monsters more carefully and modify or
eliminate the unbalancing ones.
# Some of the newer creatures should be assigned to the appropriate
  special level (e.g. rot worms only in the Wyrm Caverns), or to Gehennom.

# Introduce a disenchantment attack (affects your armor) and defense
  (affects your weapon).  This would be prevented by cancelling the monster
  or wearing a high cancellation factor, but not by magic resistance.
  Available only to monsters generated only in Gehennom, so it wouldn't
  appear until very late in the game.

# Acid attacks should have a chance of removing erosionproofing.

# Eliminate the "plus required to hit" and "hits as if plus" for various
  monsters.  That's already covered by other factors (AC, hit dice,
  damage ranges).

Pets.  Some have gone just too far.
? Is there a compelling reason for having more than one pet name
  option (e.g., dogname, catname, badgername)?

# Bullwhips may get a radius of 2.

# Silver wallets should only carry money and unlocking tools.  No
  changes are proposed for other containers.

Mummy wrappings, potions of see invisible, and probably some other
objects should not be allowed to be invisible objects.

A blessed potion of full healing should restore lost experience levels.

Many additions which I do not intend to discuss here, since they do
not relate to game balance.  I am not considering any more new items
or monsters for this patch.

+--------------------------------+
|Section 2.0 For Those in a Hurry|
+--------------------------------+
===>2.1 Quick Start
-------------------
                Decompress the archive into its own directory.  Run the
        SLASH'EM executable.

                Once in the game,  type in the name for your alter ego.
        You will then be prompted to select a character class.  Select a
        gender and alignment, if necessary.  Once you're on the dungeon map,
        use the number pad to move around (or the standard roguelike keys
        [yuhjklbn], if your turn off number_pad) Move into monsters to
        attack them.  Help is the '?' key, inventory is 'i'.

                Try out the new menu system by hitting the ` key.

+-------------------------------------------------+
|Section 3.0     What is the Nature of this Beast?|
+-------------------------------------------------+
                THE PROBLEM:  The AMULET OF YENDOR has been stolen.  Not only
        that but it appears that the Wizard of Yendor (not a nice person),
        who took the amulet,  is hiding in the Dungeons of Doom (not a
        friendly place).
                The SOLUTION:  Well,  armies have been suggested.
        Maybe hiring a superhero or two.  Unfortunately, it seems that it
        is more economical to offer a reward and let some poor adventurer
        with dreams of glory go and get it.  Guess who's got the enviable
        job of saving the day...

                Super Lotsa Added Stuff Hack - Extended Magic (SLASH'EM)
        is a role-playing game where you control a single character.  The
        interface and gameplay are similar in style to Rogue,  ADOM, Angband
        and, of course,  Nethack.  You control the actions through the
        keyboard and view the world from an overhead perspective.

+-------------------------------+
|Section 4.0 Developer's Soapbox|
+-------------------------------+
                SLASH'EM is an unfinished product.  That means that there 
        are probably more than a handful of bugs and mistakes sprinkled
        throughout the game.  That also means that I'm VERY receptive to
        reports of bugs,  from game-crashing ones to spelling mistakes.
                And the future of this behemoth?  Well - think of it this
        way.  Think of 1.0.0 as the 100% mark,  the final release for this
        incarnation of SLASH'EM  and 0.0.0 as 0% - before anything was
        started.  The current version number should indicate therefore
        the approximate progress towards the magic 1.0.0 release.  Hope
        everyone's still around when I get there - and I really do hope
        to get there ;B

+----------------------------------------------------------+
|Section 5.0 Theoretical System Requirements & Installation|
+----------------------------------------------------------+
===>5.1 DOS PORT
----------------
IBM PC 386SX or greater
DOS or compatible OS
Minimum 3 Megabytes of DPMI memory - RAM or virtual memory
        (CWSDPMI is loaded if a DPMI provider is needed)
Minimum 2 Megabytes of Hard Drive Space (More for Save files)
For Sound: PC Speaker
For Tiles: VGA (640x480x16)

        To start a game of SLASH'EM,  go to the directory you unzipped it
        into and type

                SLASHEM

        To get the VGA tiles in DOS,  you need to edit the slashem.cnf file.
        Find the line that says

                #OPTIONS=video:autodetect

        and change it to

                OPTIONS=video:autodetect

        Unlike your run-of-the-mill windows program,  SLASH'EM is a good
        little program and keeps all its files in its directory.  Just
        delete the directory you unpacked it in and -presto- it's really
        gone :)


==>5.2 Macintosh Port
---------------------
Macintosh with 68020 or later, or Power Macintosh (Untested on 68000)
System 7.0 or later
3 Mb (68k) or 4 Mb (PowerPC) RAM
6 Mb disk space

        After downloading the package, it should be un-BinHexed and then
        unstuffed.  Most web and FTP clients will handle this process
        automatically, or you can use the shareware program "Stuffit Expander".
        The directory structure and method of game play are very similar to NetHack.

===>5.3 Also Available
----------------------
Linux port (tty and Qt)
OS/2 EMX port (tty)


+-------------------------------+
|Section 6.0 The Optional Things|
+-------------------------------+
-There are a couple of new options that can be set in the 'O'ptions menu
        (or in the config file)

showdmg        
  shows dmg done onto monsters (sometimes) and dmg recieved (default FALSE)

keep_savefile
  allows you to not delete the savefile. (please note that this could detract 
  from your enjoyment of the game,  and is currently unstable) 
  (default FALSE)

pickup_burden
  Set encumberence level to begin prompting when picking up objects 
  (defualt MODERATE)

role
  Your starting role (e.g., role:Barbarian, role:Elf).  Although
  you can specify just the first letter(s), it will choose only
  the first role it finds that matches; thus, it is recommended
  that you spell out as much of the role name as possible.  You
  can also still denote your role by appending it to the "name"
  option (e.g., name:Vic-V), but the "role" option will take
  precedence.

gender
  Your starting gender (gender:male or gender:female).
  You may specify just the first letter.  Although you can
  still denote your gender using the "male" and "female"
  options, the "gender" option will take precedence.

align
  Your starting alignment (align:lawful, align:neutral, or
  align:chaotic).  You may specify just the first letter.

autoquiver
  If you use the (f)ire command when the quiver is empty, and
  this option is set, a suitable weapon will be placed in your
  quiver.  You must still remember to wield your bow if you plan
  to fire arrows (or other launcher/ammo combination).  You may
  still use the (Q)uiver command manually, if desired.
  (4E3)
  Defaults to true.  (4E4)

pushweapon
  If true, then using the (w)ield command will push your old
  main weapon (if there is one) into the secondary weapon slot.
  (4E2)

# The order of the symbols is:
# solid rock, vertical wall,horizontal wall, upper left corner,
# upper right corner, lower left corner, lower right corner, cross wall,
# upward  T wall, downward T wall, leftward T wall, rightward T wall,
# no door, vertical open door, horizontal open door, vertical closed door,
# horizontal closed door, IRON BARS, TREE,
# floor of a room, dark corridor, lit corridor,
# stairs up, stairs down, ladder up, ladder down, altar, headstone, throne,
# kitchen sink, toilet, fountain, pool or moat, ice, lava,
# vertical lowered drawbridge, horizontal lowered drawbridge,
# vertical raised drawbridge, horizontal raised drawbridge, air, cloud,
# under water.
DUNGEON=032 186 205 201 187 200 188 206 202 203 185 204 \
        250 196 179 179 196 035 084 \
        250 176 177 030 031 \
        024 025 240 239 234 236 \
        235 244 247 249 247  \
        186 205 186 205 032 035 247

+-------------------------------------------+
|Section 7.0 Who's Responsible & The Credits|
+-------------------------------------------+
===>7.1 Contact
---------------
        Have a Question?  A nasty bug got you down?  The following people
        should be able to lend you a helping hand.

        * Primary Maintainer:  Warren Cheung (wac@intergate.bc.ca)
                -If it's broken,  it's probably his fault ;B
                -I compile the Win32 and DOS binaries.

        * Macintosh (and many other things): None at present
                Paul Hurtley (paul@ilium.demon.co.uk) has compiled recent versions
                available at his SLASH'EM for Mac-PPC website 

                http://www.ilium.demon.co.uk/Slash-EM/index.html

        * Linux, Qt Ports: None officially at present
		J. Ali Harlow (ali@avrc.city.ac.uk) has recast my releases in the 
		style of vanilla nethack,  and has tips for compiling SLASH'EM 
		under Linux (which may also apply for other Unix boxes)		
        
        	http://avrc.city.ac.uk/nethack/slashem.html

        * Nethack DevTeam: nethack-bugs@linc.cis.upenn.edu
                -they are in charge of Nethack (currently 3.3.1) and have NO
                 association with the SLASH'EM technical support process.
                 Please do not send them mail about SLASH'EM unless it also
		 pertains to Nethack.
                 
                 http://www.nethack.org/

===>7.2 Pillars of Strength
---------------------------
Thanks to My Mom.

Anthony Tang, Valerie Wong, Bibiana Wong, Brian Chan, Sylvia Chan,
Tania Bangayan, Sylvia Chen,  Rachel Chu, Huijin Kong,  Sandy Mak,
Francesca Ranada, Christine Tam, Joel Wagler, Geoffrey Wan, Geoffrey Cowan,
and Katrin Veiel.

===>7.3 Acknowledgements
------------------------
        Thanks to Kevin Hugo (hugo@cae.wisc.edu),  the previous Macintosh
        maintainer and the author of many changes,  who has had to move on to 
        other projects.
        
        Thanks to Dirk Schönberger (shoenberger@signsoft.com),  who worked on
        the Qt and Linux ports.

        Thanks to Ben Olmstead for mirroring the homepage.

        Thanks for the support, bug reports and suggestions of 
        (in no particular order):
        
        Pat Rankin,  Linley Henzell,  0ystein Kerstad,
        Jim Miller, pi, Steven Uy, Alex Critchfield,  Mike Balcom,
        Sascha Wostmann, Drew Curtis, Daniel Krause, E. P. Dinklewhopper,
        Tim Jordan, Lukas Geyer, Kresimir Kukulj, Brent Oster, Travis McKay,
        Jared Withington, Philippe-Antoine Menard, Ain Uustare, Doug Yanega,
        David Tang, Lief Clennon, Robert Herman, Petra Prinz,
        Aki Lumme, Nick Number,  Robin Johnson, Heiko Niemeier, Chris Bevan,
        David Damerell, Kevin Hugo, Daniel Mayer, Ben Olmstead, Tom Friedetzky,
        Kalle Niemitalo, Sabine Fischer, Christian Bressler, Jake Kesinger
        Chris Subich, Paul Sonier, Peter Makholm, Matthew Ryan, J. Ali Harlow,
        George Stephens, Bruce Cox, Edward Walker, Karl Garrisson,
        Finn Theoderson, Kelly Bailey, Seth Friedman, Jonathan Nieder,
        Eva R. Myers.

+---------------------------+
|Section 8.0 Other Resources|
+---------------------------+
Author:  Warren 'WACko' Cheung (wac@intergate.bc.ca)
Homepage:
        http://wac.cjb.net/

        which currently point to 
        
        http://www.intergate.bc.ca/personal/wac/

SLASHEM Homepage:
        http://www.slashem.org/

        which currently point to 
        
        http://slashem.sourceforge.net/
