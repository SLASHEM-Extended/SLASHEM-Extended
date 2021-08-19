/*	SCCS Id: @(#)mkobj.c	3.4	2002/10/07	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "prop.h"


STATIC_DCL void mkbox_cnts(struct obj *);
STATIC_DCL void obj_timer_checks(struct obj *, XCHAR_P, XCHAR_P, int);
#ifdef OVL1
STATIC_DCL void container_weight(struct obj *);
STATIC_DCL struct obj *save_mtraits(struct obj *, struct monst *);
#ifdef WIZARD
STATIC_DCL const char *where_name(int);
STATIC_DCL void check_contained(struct obj *,const char *);
#endif
#endif /* OVL1 */

extern struct obj *thrownobj;		/* defined in dothrow.c */

/*#define DEBUG_EFFECTS*/	/* show some messages for debugging */

struct icp {
    int  iprob;		/* probability of an item type */
    char iclass;	/* item class */
};

/* faux artifacts to fool the player --Amy */
static const char * const fauxartinames[] = {

"Pus Spitter", "Excalibat", "Moonbite", "Steelgleam", "Spellstealer", "Ironward", "Crashbug Preventer", "Segfault Breath", "Server Operator", "Mindflayer's Best", "The Burninator", "Ascension Kit Part", "DYWYPI", "Identificator", "Frost Barrier", "Zombiekiller", "Litchbane", "Gnomerist", "Vorpal Element", "Silverswandir", "Electric Brand", "Mojo", "Mirrorblack", "Eluder", "Barrowtooth", "Icy Flame", "Spider's Tongue", "Thunderbringer", "White Death", "Coldfire", "Polar Sphere", "Mind Maw", "Hidden Potato", "Queeb Butt", "Total Nonsense", "Spewbrewer", "Lasercross", "Hotel Thirst", "Ultimate Artifact", "Anti-YASD Insurance", "Invulnerability Dummy", "Priced Bauble", "Hunter of Shiva", "Sharp Bender", "Itchalaquiaque", "Saintly Seduction", "Nymph Bait", "Magicbreath", "Team Nastytrap Medal", "Team Ant's Friend", "Hatefulpresident", "Snyder's Killer", "Universa Exit", "Marike's Farting Noise Generator", "Solvejg's Protector", "Josefine's Girly Fun", "Beeper Tag", "Larissa's Hugger", "Relic of AmyBSOD", "Slashem Slasher", "Deslexifier", "Soviet Russia Special", "Sexy Hexy", "Pinobot", "Kenault's Cantrip Generator", "The Deck of Wild Magic", "Baalzebub's Tormented Box", "Baalzebub's All-seeing Servant", "Boccob's Unfinished Works", "The Devil's Pentagram", "Mordekainen's Sneaking Eye", "Tenser's Alteration Manual", "The Pendulum of Orcus", "Mordekainen's Pocket Magician", "Benetar's Death Ray Experiment", "Valdarbon's Automatic Alchemist", "Demigods and their Uses", "Magic for the Layman", "Benetar's Mana Battery", "Benetar's Portable Plague", "Gamenlon's Summoning Manual", "The Lost Works of Kenault", "Planar Travel Made Easy", "The Wand Construction Kit", "The Clay Tablets of Antiquity", "Raal's Tormented Spirits", "Tenser's Mechanical Magician", "Boccob's Magical Mish-mash", "The Grail of Kenault", "Household Magic", "Tenser's Last Words", "Secrets of the Gnomish Wizards", "The Cunning Plan of Zog", "Immortality For Dummies", "Raistlin's Ready Ranger", "Mordenkainen's Mysterious Mind-Masher", "Famous Last Words", "Jor's Compendium of Strange Behaviour", "Agannazar's Antique Acorn", "Cathal's Corrupting Cymbal", "Pytar's Portable Pandemonium", "Finer Points of Munchkinism", "Agannazar's Altruistic Assassin", "Olive's Omnipotent Ostrich", "Cathal's Collapsible Crutch", "Nauglamir", "The Anchor of Space-Time", "Razorback", "Bladeturner", "Soulkeeper", "Belegennon", "Thalkettoth", "Holhenneth", "Colluin", "Holcolleth", "Colannon", "Cambeleg", "Cammithrim", "Paurhach", "Paurnimmen", "Pauraegen", "Paurnen", "Camlost", "Dal-i-thalion", "Angrist", "Narthanc", "Nimthanc", "Dethanc", "Belangil", "Calris", "Aranruth", "Glamourdring", "Aeglin", "Gurthang", "Zarcuthra", "Mormegil", "Gondricam", "Crisdurian", "Aglarang", "Ringil", "Anduril", "Anguirel", "Elvagil", "Forasgil", "Careth Asdriag", "Haradekket", "Gilettar", "Doomcaller", "Osondir", "Til-i-arc", "Aeglos", "Nimloth", "Lotharang", "Mundwine", "Barukkheled", "Avavir", "Grond", "Totila", "Thunderfist", "Bloodspike", "Firestar", "Taratol", "Nar-i-vagil", "Eriril", "Deathwreaker", "Turmil", "Belthronding", "Cubragol", "Glimdrir", "Lasher", "Bullseye", "Travak", "Limbslicer", "Orchast", "Naturebane", "Hanisbroner's Surprise", "Merlion Karc's Demonbane", "Narsil", "Lebohaum", "Mediator", "Hithlomir", "Skullcleaver", "Durandil", "Toris Mejistos", "Elessar", "Evenstar", "Balefire", "Stonebiter", "Heart's Blood", "Scale-piercer", "Garachoth", "Skycleaver", "Photic Illuminator", "Captain Nemo's Nautilus", "Le Loup Blanc", "The Eye of Ra", "The Pectoral of the Kheper", "War of the Worlds", "The Time Machine", "20000 Leagues Under the Sea", "Journey to the Centre of the Earth", "Treasure Island", "Les Miserables", "Hunchback of Notre Dame", "The Count of Monte Cristo", "The Three Musketeers", "The Invisible Man", "Around the World in 80 Days", "Wuthering Heights", "The Adventures of Sherlock Holmes", "The Hound of the Baskervilles", "The Return of Sherlock Holmes", "The Lost World", "The Sign of the Four", "A Christmas Carol", "Great Expectations", "A Tale of Two Cities", "Little Women", "Alice's Adventures in Wonderland", "Through the Looking Glass", "The Adventures of Huckleberry Finn", "The Adventures of Tom Sawyer", "A Connecticut Yankee in King Arthur's Court", "The Prince and the Pauper", "Life on the Mississippi", "On the Origin of Species", "Tenant of Wildfell Hall", "Sense and Sensibility", "Pride and Prejudice", "Northanger Abbey", "Persuasion", "Confessions of an English Opium Eater", "The System of the Heavens", "Lord Rosse's Telescope", "On Murder Considered as One of the Fine Arts", "The Arts of Cheating, Swindling and Murder", "The Man Who Would Be King", "The Mark of the Beast", "Captains Courageous", "The Hunting of the Snark", "The Picture of Dorian Gray", "The Turn of the Screw", "King Solomon's Mines", "Legend of Sleepy Hollow", "Crime and Punishment", "Last of the Mohicans", "From the Earth to the Moon", "Round the Moon", "War and Peace", "The Man in the Iron Mask", "The Jungle Book", "Principles of Political Economy", "The Narrative of Arthur Gordon Pym", "Tales of the Grotesque and Arabesque", "The Tomb and Other Tales", "Dagon and Other Macabre Tales", "The Dream-Quest of the Unknown Kadath", "Mysteries of the Depths of the Seas", "The Life and Adventures of Santa Claus", "Red Nail", "Deathspade", "Mindrend", "Rakescar", "The Chieftan", "The Humongous", "Maelstromwrath", "Gravenspine", "Umes Lament", "Felloak", "Knell Striker", "Rusthandle", "Stoutnail", "Bloodrise", "The Generals Tan Do Li Blabla", "Bonesob", "Steeldriver", "Rixots Keen", "Azurewrath", "Hellplague", "Kinemils Awl", "The Patriarch", "Gull", "Irices Shard", "The Dragon Chang", "Razortine", "Steelgoad", "Bane Ash", "The Iron Jang Bong", "Witherstring", "Rimeraven", "Piercerib", "Pullspite", "Wizendraw", "Hellclap", "Blastbark", "Leadcrow", "Ichorstring", "Hellcast", "Doomspittle", "Duskdeep", "Greyform", "Blinkbats Form", "The Centurion", "Twitchthroe", "Darkglow", "Venomsward", "Iceblink", "Boneflesh", "Pelta Lunata", "Stormguild", "Steelclash", "Chance Guards", "Frostburn", "Hotspur", "Tearhaunch", "Nightsmoke", "Nokozan Relic", "The Eye of Etlich", "The Mahim-Oak Curio", "Manald Heal", "Gorgethroat", "The Stone of Jordan", "Crissaegrim", "Force Field", "Mother of Universe", "The Ungodly Slayer of Hitpoints", "The Randomizer", "Creeping Death", "Anthrax", "Tranquilizer", "Ezerataneous", "Blood Omen", "Blizzard of Oz", "Spectral Avoider", "Heretic Saprophyte", "Nature Is Hell", "The Energy Reaver", "Glory of Suffering", "Da Grip", "Terminus Est", "World Champion", "Elysdeon", "Datura", "Talula", "Nativity in Black", "A Twist of Ugliness", "Ass Kickers", "Visionary", "Impact Splatter", "A Woman's Scorn", "Saint Ankh", "Mystery Ankh", "Shining Freedom", "Juarez", "Hide in Shadows", "Stalking Ambush", "Resident Evil", "Celestial Seasons", "Flame Visit", "Spread Needle", "Heaven's Punisher", "Holy Ray", "Enchanted Revenge", "Chain of Conductors", "Soul of Support", "Vagyu Darkness", "Spiritual Cleansing", "Crushing Divinity", "Sightseeker", "Gospel of Peace", "Word of Truth", "Eternal Sphere", "Tri-Emblem", "Star Guard", "Valkerie Guard", "Missing Score", "Rolling Thunder", "Conformer", "Death Penalty", "Army of Me", "Varista", "Orotiagito", "Reactor Fragment", "Energy Core", "Thermo Nuclear Chamber", "Crystalis", "Carved Perfection", "Gold Digger", "The Memory of Lady Mia", "Kaladgolg", "Scorched Trail", "Intercontinental", "Di Immortales", "Til Death do Us Part", "Wrath of Cardice", "Penetrators", "MP3 Player", "Coldkill", "Islestrike", "Warlord's Trust", "Spellsteel", "Suicide Branch", "Cairn Shard", "Drulan's Tongue", "The Fetid Sprinkler", "Sureshrill Frost", "Moonfall", "Baezils Vortex", "Earthshaker", "Coldsteal Eye", "Hexfire", "Riftslash", "The Atlantien", "Crainte Vomir", "Bing Sz Wang", "Cloudcrack", "Todesfaelle Flamme", "Kelpie Snare", "Hone Sundan", "Athena's Wrath", "Pierre Tombale Couant", "Husoldal Evo", "Grim's Burning Dead", "Chromatic Ire", "Skullcollector", "Skystrike", "Kuko Shakaku", "Endlesshail", "Langer Briser", "Buriza-Do Kyanon", "Vampiregaze", "String of Ears", "Gorerider", "Lavagout", "Venom Grip", "Visceratuant", "Guardian Angle", "Shaftstop", "Valkiry Wing", "Demon Machine", "Cliffkiller", "Razorswitch", "Meatscrape", "Pitblood Thirst", "Ondal's Wisdom", "Geronimo's Fury", "Charsi's Favor", "Doppleganger's Shadow", "Deathbit", "Warshrike", "Gutsiphon", "Stonerattle", "Marrowgrinder", "Steelshade", "Death's Web", "Gaia's Wrath", "Khalim's Vengance", "Angel's Song", "The Reedeemer", "Fleshbone", "Odium", "Blood Comet", "Bul-Kathos' Might", "Arioc's Needle", "Shadowdancer", "Indiego's Fancy", "Tyrael's Mercy", "Souldrain", "Runemaster", "Executioner's Justice", "Wallace's Tear", "Leviathan", "Qual'Kek's Enforcer", "Dawnbringer", "Wisp Projector", "Gargoyle's Bite", "Lacerator", "Mang Song's Lesson", "El Espiritu", "The Ensanguinator", "The Reaper's Toll", "Spiritkeeper", "Hellrack", "Alma Negra", "Darkforge Spawn", "Sankenkur's Resurrection", "The Archon Magus", "Widow maker", "Catgut", "Ghostflame", "Bling Bling", "Nebucaneezer's Storm", "Griffon's Eye", "Eaglewind", "Windhammer", "Thunderstroke", "Bloodmoon", "Cranebeak", "Iansang's Frenzy", "Warhound", "Gulletwound", "Headhunter's Glory", "Mordoc's marauder", "Talberd's Law", "Amodeus's Manipulator", "Darksoul", "The Black Adder", "Nature's Peace", "Seraph's Hymn", "Zakarum's Salvation", "Blood Rain", "Horizon's Tornado", "Wrath of Cain", "Siren's call", "Archangel's Deliverance", "Fuego Del Sol", "El Infierno", "Moonrend", "Larzuk's Champion", "Nightsummon", "Kang's Virtue", "Lifechoke", "Palo Grande", "Ghostleach", "Samual's Caretaker", "Hell's Whisper", "The TreeEnt", "Stormwillow", "Moonshadow", "Strongoak", "Demonweb", "Bloodraven's Charge", "Shadefalcon", "Robin's Yolk", "Glimmershred", "Wraithflight", "Lestron's Mark", "Banshee's Wail", "Medusa's Gaze", "Hadeshorn", "Rockstopper", "Crow Caw", "Skullder's Ire", "Toothrow", "Atma's Wail", "Black Hades", "Corpsemourn", "Que-hegan's Wisdom", "Moser's Blessed Circle", "Stormchaser", "Tiamat's Rebuke", "Gerke's Sanctuary", "Radimant's Sphere", "Gravepalm", "Hellmouth", "Infernostride", "Silkweave", "Wartraveler", "Razortail", "Gloomstrap", "Snowclash", "Thudergod's Vigor", "Lanceguard", "Squire's Cover", "Boneflame", "Steelpillar", "Nightwing's Veil", "Hightower's Watch", "Darkfear", "Ashrera's Wired Frame", "Rainbow Facet", "Ravenlore", "Boneshade", "Nethercrow", "Flamebellow", "Fathom", "Wolfhowl", "Gheed's Fortune", "The Vicar", "Halaberd's Reign", "Parkersor's Calm", "Frostwind", "Templar's Might", "Eschuta's temper", "Sandstorm Trek", "Marrowwalk", "Heaven's Light", "Merman's Speed", "Arachnid Mesh", "Sigurd's Staunch", "Carrion Wind", "Gillian's Brazier", "Drakeflame", "Dust Storm", "Skulltred", "Alma's Reflection", "Sacred Charge", "Saracen's Chance", "Highlord's Wrath", "Raven Frost", "Dwarf Star", "Atma's Scarab", "Mara's Kaleidoscope", "The Rising Sun", "The Cat's Eye", "Metalgrid", "Titan's Revenge", "Shakabra's Crux", "Lycander's Aim", "Shadow's Touch", "The Prowler", "Sarmichian Justice", "Annihilus", "The Harbinger", "Doomseer", "Apothecary's Tote", "Apocrypha", "Foci of Visjerei", "Aurora's Guard", "Herald of Zakarum", "Ancient Eye", "Xenos", "Sister of the Water", "Flanker Loop", "Bet Notice", "Mafia City", "Flanker Intro", "Pinball Ingame", "Mafia Water", "Pin of the 11B potatoes", "The Dictator of France", "Short Wave Hiding", "Lock Defendant", "Solved Puzzle", "The Torture Master", "Diablo 2's Eddy Wind", "DHG-Search Play Prize", "Brasilia's Rain Forrest", "Artifical Accomplishment", "Amusing Ivo's Fury", "Elnaz's Blood Rush", "Sizzle's Card Stand", "G-cheater's Fucking Rockstar", "Gigantic Double-deck Airplane", "Holland's World Master Team", "GTS Champion Elbow", "Lily's Puncturer", "Laura and her sisters and their force", "A.Ha's Revenant", "Shakira's Beautiful Dance", "Gerlindchen's Flower Pot", "Ruth's resouding laughter", "Jane's embrace-worthiness", "Steel Beetle's Victory", "Invisible Maker", "H.'s Giant Tank", "Course of Kastor-Transport", "Ludgera's Deception", "Upper Perfect Kunzite", "CD of Britney Spears", "Gettoblaster 2.0++", "Awake Sleep Tower Sign", "Iceforce", "The Boss of the Errors", "Teacher Police's Cheating", "Property Completion", "Urbaldi's Last Salvation", "Nest of the Black Raptors", "Ruth's Case Sign", "World's Burden", "Tyrant's Power", "Magma Shooting Bird", "Dracula's Wish", "Zephyros", "Course Point's Glacial Spike", "Reflection Cover", "Barkbite", "Error Biter", "Breakwound Impacter", "Bulldog Gaurd", "Dark Corner", "Computer Mouse", "The Mould Defiler", "Port Output", "Apocalypse", "Frostfoil", "Ascaron", "Laurefindelo Umbar", "Scaleron", "Cosomoco Pelecco", "Cosomoco Vaitando", "Aiglos", "Winter's Bite", "Tempest", "Scorchwind", "Withering Finger", "Oblivion's Enbrace", "Crushing Grip", "Cruel Spur", "Fold of Sorrow", "Crimson Steel", "Spineridge", "Diabolic Wrap", "Spider Strand", "Battlelock", "Threshold Insurmountable", "Diabolic Casing", "Astral Dream", "Enspelled Talisman", "Charm of Holding", "Broach of Eternal Life", "Vizier's Charm", "Circle of Life and Death", "Cryptic Glyph", "Scion of Numenor", "Diabolic Gifts", "Vampire's Immortality", "Star of Tar-Ancalime", "Vilisse", "Ancalquon Morramaloce", "Cosomot Valaraucatur", "Relics of the Third Age", "Rule of Eldarion", "Boromir's Gift", "Scourge of Fire", "Battlecry of Lammoth", "Darkness of Moria", "Sacred Bottle Spirit", "Clay Tone Band", "Measure-hit a corner", "Leo's secret hiding place", "Jutta flax's OHP", "Total Sun Darkness", "The Gravegraver", "The Secret Car", "Silver-cutting 1E-coin", "The Queue's Destruction", "Diablo II's Great Hatred", "Stalin's Secret Refuge", "Clockold Blackwhite TV", "A Green Prostitute", "Marc's Battlestandard", "TI-83 Pocket Calculator", "Plague Error Impacter", "Spleendrain", "Ametyhstmaster", "Irriant Darkness", "Cage Rattler", "Hobbes's Rectifier", "Elsa's Champion", "Corner to sport-resounded", "Punching Bag", "Lesson with DJ Leutkak", "Internal Clan Area", "Cristi's Greener Pin", "The Challenging Fence", "A Christmas Present", "Star Wars - Episode 2", "Gorbatschov's Talisman", "The Tarrif Cashier", "Door to the space 314", "Saddam Has Fallen", "The Toilet Brush's Shard", "Bull Mode", "Lock Entrance Cashier", "King of Porn", "Marianne's Measurer", "Red Lit Combat Arena", "Mars-resound's Disguise", "The Horse's Giant Engine", "Fear Forwards The Light", "Hiding Place of Urbaldi", "Inability of Reziarfg", "Kastor-Transport's Accellerotor", "Aee's Steel Sign", "Gunnhild's General Store", "Heaven For Everyone", "Braight Meteor", "Homicidal Tendancies", "Master Thief", "Drogman's Fortune", "Nebucaneezer's Reign", "Prick Rat Nest", "Nosferatu's Grasp", "Hibernal Wind", "Sentinal Angle", "Most Wanted", "Giant Air Elemental", "Volcano Walk", "Insidiarious", "Slowing Down Spider Net", "Granitegrid", "Large Feries", "Sigurd's Impact", "Carrion Beetle", "Bonusroomward", "Secret Order", "Larissa's Requiem", "Larissa's Repose", "Larissa's Marker", "Larissa's Stride", "Larissa's Decorum", "Larissa's Mourning", "Operating Charm", "Blood Rose", "Frost Orchid", "Stinging Vine", "Falling Thunder", "Marc's Sprint", "Larissa's Windsong", "Call of Odin", "Herald of the Dead Potatoes", "Censor of the Dead Potatoes", "Black Flame", "SpeeArrow", "Leon's Rage", "Invisible Bane", "Andariel's Bane", "Sarthan Fang", "Larissa's Embrace", "Unknown Quantity", "Harathnar", "Vigilante", "Grand Final", "Alphabet Soup", "Streetrace", "Great ski the jungle", "Buggards Greatness", "Grave Theft", "The Cheater", "Super Long Eddy Wind", "Weather Control", "Sky High Fire", "Prison Lattice", "Super Grandpa", "World Fall", "Dumoto", "Firecracker", "Impossible Dream", "Reduction Jet", "Upper World Killer", "Berrytown Disco", "Porsche Carrera GT", "Errland's Salvation", "Germany's Liberator", "Arrest Jet", "Cave Bear's Revenge", "Giant Pencil Sharpener", "Car Hostage", "End of a bridge", "God's Allmight", "The Annihilating Plague", "McDonalds Cheat", "Radio Wave", "Taxi Central", "Lower Level", "Sky Collapse", "Useless Rubber", "Nefarius's Sucking", "Phrozenkeep's Insanity", "Accelerator Button", "Money Printing Machine", "Blessed Aim Bug", "Great Boredom", "Greener Branch", "Reuniting of the Elements", "Error Generator", "Perma-red Traffic Light", "Beer Drinker's Trick", "Lit Room", "Buriza-Do Astron", "Duskhigh", "Water Course", "Andrials Fake", "Missing Light", "Statue Room", "Yet Another Defense Part", "Upper Convictor", "Difficult Start", "Double Order", "Start Absorber", "Still More Difficult End", "Multiple Character", "Attack Guard", "Phil's Heroism", "Cop Skilltab", "Upper Giant UFO", "Crash Bonus 1 Million", "Thornwalk", "Cold Animation", "Fire Spire", "Allied Agora", "High Calculation", "Preemptive Calculation", "False Tal Rasha's Tomb", "Sky Explosion", "Bonus Speed", "Giant Attic Spider", "Too bad!", "Course Reflector", "Super Spray Box 3.1", "Super Spray Box 3.BAR++", "Crusaders Justice", "Shining Light", "Cursed Begining", "Worldstone Collapse", "Wildheart", "The Onslaught", "Shard of Heaven", "Ancients Peak", "Elements Vortex", "The Worldstone Hold", "Zues' Holy Shards", "Gustforce", "Chieftans Shrine", "Immortal One", "Theifs Prowess", "Elements Bane", "Colossal Bones", "The Wounding Noose", "Phases of Time", "Riamens Savour", "Cold Maidens Woods", "The Spirit Within", "Cruel Prod", "The Jester Tear", "Enspirited Soul", "The Maidens Cursed Touch", "Ogre's Crossing", "The Tribute", "Enslaver", "Worldstone Shard", "Deaths Dancer", "Heart's Content", "Mind and Soul", "Rampage", "The Zodiac", "Enhancer", "The Elmentalist", "The Weathers", "Green Unweather", "Blue Unweather", "Plague Stealer", "Bomb off-throw", "Bomber of the Monsters", "Stopping Target Far Tube", "Not Ending Thunder", "Boat Torch Flare", "Iron Vortex", "The Top Secret Course", "Ethereal Fighter", "Greater Controversy Code", "Cursed Worth", "Great Potato Iron", "Flash Spirit", "Wide Level", "Dynamic Right Library", "Course Thief", "The Dictator Of CD Rome", "Mummy Loading", "Saddam Hussein's Bomb Notice", "Green Space", "Unclear Link Curve", "Legal Situation", "Hell-Light Source", "Dark Wonderer", "Trophy Trap", "Larger Poison New Facts", "Pins Without Soul", "The Amazon's Last Chance", "Poison Fighter's Alternation", "Really Impossible Crash Bug", "Altered Save Method", "Fast Reload", "Group Meeting", "Newest Terror Notice", "Shot Series", "Insane Contractor", "Gate Shot", "Gang Shot", "Post Shot", "Endless Wave", "Hotel Burner", "Outgoing Heap", "Cheating Contractor", "Giant Shooting Range", "ds will only run in Protect mode", "Stolen from Duke", "Only Newly Added", "Already Outcheated", "Swift Flight Body", "The All-maker", "The Vault Master", "Thunder Curse", "Call of Loki", "Stalingrad Liberator", "Cooling Shot", "Incinerating Shot", "Tschaeauwieauw!", "Tschiuwiu!", "Hollywood Holocaust", "Low Level PK", "Impossible to find", "Additional Learning Achievement", "Leutkak's Special", "Gorden's Frenzy", "Golem Bar", "First Siegecrafter", "Mexico's Secret Agency", "Guantanamo's Discovery", "Pseudo Grandpa", "Dwelling Case", "Cheater Stinger", "Marlboro Light", "End of the year", "Steel Runeword", "Copilot Wanted Level 6", "Nuke MiG", "Connections", "Perfect Set", "Urbaldi's New Invention", "2k Raid", "Upward Course", "Burning Spaceship", "Overcharge", "God's Annihilator", "Cop Activator", "747 Attack", "Demon Portal", "Horizons Alternative", "Undead Portal", "Gift of the Golden Gates", "Devil Warder", "The North Star", "Pack Caller", "Nighttime Birds", "Pandemonium", "Avalanche Strike", "Insight of the Ancients", "Beating Disk", "Repenter of Mage Corruption", "Serendipity", "On Most Surfaces", "Oakheart", "Gruntface", "Stand Up And Shout", "Me Smash", "Elemental Ignorance", "Blessing of the Dawn", "Angel's Grace", "Nupraptor's Madness", "Morte", "Passive Horror", "Nightwish", "Ancients' Epiphany", "Survival of the Fittest", "Don't Break the Oath", "White Tiger", "These Precious Illusions", "The Road of Good Intentions", "Beware the Heavens", "Tamara's Youth", "In My Cross Hairs", "Nerad's Attunuation", "Violence Has Arrived", "Diversity is Our Strength", "Scarlet's Walk", "A Thousand Kisses Deep", "Eclipse", "Marumana", "Little Earthquakes", "Bloody Ballerina", "Grade-A Nirvana", "Focus Shifter", "Typhoon", "Lead Us to War", "Never is a Promise", "Pillum of Marius", "Twinrova's Essence", "Dark Shadow", "Holy Avenger", "The Only Fantasy is Reality", "We've Come For Your Daughter", "One Hitpoint Wonder", "Pokey Pokey", "Paragons of Innocence", "Fear of the Dark", "Gold Moon Dragon", "The Mob Rules", "Midnight Madness", "Shadow Feet", "Rakashi's Tempest", "Murder God Lasher", "Gravesinger", "Bleed the Freak", "Blegh", "Zero Drive Illusion", "Smokestack", "Bless the Child", "Whore of the Abattoir", "Haven's Work", "Heirloom of the Shara Goddess", "All is Full of Love", "Stormsplitter", "Volcaetus", "Claimh Saolias", "Evanescence", "The King's Devotion", "Melufa", "Gimmie a Hand", "Vanquisher", "Silence Breaker", "Lich Hunter", "Euros", "Tomb Chill", "I Spit on Your Grave", "We Are Nine", "Persona", "Andrea's World", "Can I Play With Madness", "Hidden Place", "Akara's Thanks", "Skidbladnir", "The Devil's Orchid", "Curse Tips", "Azalin's Wish", "Hangman's Smile", "Morrenbane", "Infinite Fury", "Wizard Away Spray", "Planar Edge", "Terra's Enchantment", "Prox", "Ocean's Embrace", "Terminator", "The Last in Line", "Electric Funeral", "Turn Up the Night", "Silver Spark", "Angry Machines", "Tidal Master", "Longicolnis", "Spirit of Servents", "Powerslave", "D.N.R.", "From the Other Side", "Atheist Champion", "Horror Show", "Dusk", "van Gough's Gift", "Hail Storm", "Brotherhood", "Eat Your Entrails", "Straferiffic", "Poets and Madmen", "Immortal Corruptor", "Cvalda", "The Amnesty of Night", "Running Free", "Soul Hunter Medallion", "Reinventing the Steel", "Dehumanizer", "Sky Scorcher", "Cain's Support", "Wolfsbane Bloom", "Lobelia", "Biledriver", "Variant Reader's Codex", "Lost Orb of Phantastacoria", "The Colour of Magic", "The Light Fantastic", "Equal Rites", "Mort", "Sourcery", "Wyrd Sisters", "Pyramids", "Guards! Guards!", "Moving Pictures", "Reaper Man", "Witches Abroad", "Small Gods", "Lords and Ladies", "Men at Arms", "Soul Music", "Interesting Times", "Maskerade", "Feet of Clay", "Hogfather", "Jingo", "The Last Continent", "Carpe Jugulum", "The Fifth Elephant", "The Truth", "Thief of Time", "The Last Hero", "The Amazing Maurice and His Educated Rodents", "Night Watch", "The Wee Free Men", "Monstrous Regiment", "A Hat Full of Sky", "Going Postal", "Thud!", "Wintersmith", "Making Money", "Unseen Academicals", "I Shall Wear Midnight", "Snuff", "Raising Steam", "The Shepherd's Crown", "Restful Cage For Sorcerors", "Amaxoflandir",

};

#ifdef OVL1

/* STEPHEN WHITE'S NEW CODE */   
/* [Tom] tried to fix this back up a little... */
/* KMH, balance patch -- changed again */
const struct icp mkobjprobs[] = {
{129, WEAPON_CLASS},
{ 90, ARMOR_CLASS},
{160, FOOD_CLASS},
{ 70, TOOL_CLASS},
{ 30, GEM_CLASS},
{120, POTION_CLASS},
{120, SCROLL_CLASS},
{ 40, SPBOOK_CLASS},
{ 70, WAND_CLASS},
{ 20, RING_CLASS},
{ 15, AMULET_CLASS},
{122, COIN_CLASS},
{  1, VENOM_CLASS},
{ 10, ROCK_CLASS},
{  1, BALL_CLASS},
{  1, CHAIN_CLASS},
{  1, IMPLANT_CLASS}

/* KMH -- amulets now appear later in the game */
/*{ 0, AMULET_CLASS}*/
};

const struct icp boxiprobs[] = {
{ 99, WEAPON_CLASS},
{ 90, ARMOR_CLASS},
{ 20, GEM_CLASS},
{ 55, TOOL_CLASS},
{160, FOOD_CLASS},
{120, POTION_CLASS},
{120, SCROLL_CLASS},
{ 60, SPBOOK_CLASS},
{133, COIN_CLASS},
{ 65, WAND_CLASS},
{ 40, RING_CLASS},
{ 20, AMULET_CLASS},
{  1, VENOM_CLASS},
{ 10, ROCK_CLASS},
{  1, BALL_CLASS},
{  1, CHAIN_CLASS},
{  5, IMPLANT_CLASS}
};

const struct icp tchestprobs[] = {
{160, WEAPON_CLASS},
{150, ARMOR_CLASS},
{ 74, GEM_CLASS},
{ 70, TOOL_CLASS},
{ 90, FOOD_CLASS},
{ 90, POTION_CLASS},
{ 90, SCROLL_CLASS},
{ 60, SPBOOK_CLASS},
{ 53, COIN_CLASS},
{ 45, WAND_CLASS},
{ 55, RING_CLASS},
{ 45, AMULET_CLASS},
{  1, VENOM_CLASS},
{ 10, ROCK_CLASS},
{  1, BALL_CLASS},
{  1, CHAIN_CLASS},
{  5, IMPLANT_CLASS}
};

#ifdef REINCARNATION
const struct icp rogueprobs[] = {
{150, WEAPON_CLASS},
{130, ARMOR_CLASS},
{220, FOOD_CLASS},
{200, POTION_CLASS},
{200, SCROLL_CLASS},
{ 50, WAND_CLASS},
{ 50, RING_CLASS}
};
#endif

const struct icp hellprobs[] = {
{129, WEAPON_CLASS},
{ 80, ARMOR_CLASS},
{130, FOOD_CLASS},
{ 60, TOOL_CLASS},
{ 30, GEM_CLASS},
{ 90, POTION_CLASS},
{ 90, SCROLL_CLASS},
{ 50, WAND_CLASS},
{ 30, RING_CLASS},
{ 20, AMULET_CLASS},
{ 35, SPBOOK_CLASS},
{242, COIN_CLASS},
{  1, VENOM_CLASS},
{ 10, ROCK_CLASS},
{  1, BALL_CLASS},
{  1, CHAIN_CLASS},
{  1, IMPLANT_CLASS}
};

struct obj *
mkobj_at(let, x, y, artif, shopinit)
char let;
int x, y;
int artif;
boolean shopinit;
{
	struct obj *otmp;

	otmp = mkobj(let, artif, shopinit);
	if (!otmp) return (struct obj *)0; /* fail safe added by Amy */
	place_object(otmp, x, y);
	return(otmp);
}

struct obj *
mksobj_at(otyp, x, y, init, artif, shopinit)
int otyp, x, y;
boolean init;
int artif;
boolean shopinit;
{
	struct obj *otmp;

	otmp = mksobj(otyp, init, artif, shopinit);
	if (!otmp) return (struct obj *)0;
	place_object(otmp, x, y);
	return(otmp);
}

/* mkobj change by Amy: "artif" used to be a boolean and is now an int.
 * 0 = usually don't make item into an artifact, but retain a very low chance
 * 1 = normal chance of making the item into an artifact
 * 2 = absolutely never make the item into an artifact, and also don't make contents if it's a container
 */

struct obj *
mkobj(oclass, artif, shopinit)
char oclass;
int artif;
boolean shopinit;
{
	int tprob, i, j, prob = rnd(100000);

	int debugvar = 0;
	if (Is_rogue_level(&u.uz)) debugvar = 1;
	else if (Inhell) debugvar = 2;
	boolean wildcard = 0;
	if (oclass == WILDCARD_CLASS) { /* RANDOM_CLASS, but timebasedlowerchance is not in effect --Amy */
		oclass = RANDOM_CLASS;
		wildcard = TRUE;
	}

	register int levscalediff;

	if (oclass >= MAXOCLASSES) { /* failsafe --Amy */

		impossible("Error: mkobj() called with invalid object class %d", (int) oclass);
		oclass = RANDOM_CLASS;

	}

	if(oclass == RANDOM_CLASS) {
		const struct icp *iprobs =
#ifdef REINCARNATION
				    (Is_rogue_level(&u.uz)) ?
				    (const struct icp *)rogueprobs :
#endif
				    Inhell ? (const struct icp *)hellprobs :
				    (const struct icp *)mkobjprobs;

		for(tprob = rnd(1000);
		    (tprob -= iprobs->iprob) > 0;
		    iprobs++);
		oclass = iprobs->iclass;

		if ((u.veryobtainclass1 > 0) && (rn2(1000) < u.veryobtainclass1boost)) {
			oclass = u.veryobtainclass1;
		}
		else if ((u.veryobtainclass2 > 0) && (rn2(1000) < u.veryobtainclass2boost)) {
			oclass = u.veryobtainclass2;
		}
		else if ((u.veryobtainclass3 > 0) && (rn2(1000) < u.veryobtainclass3boost)) {
			oclass = u.veryobtainclass3;
		}

		if ((oclass == SCROLL_CLASS) && (rn2(100) < u.scrollspawnchance)) {
			oclass = COIN_CLASS;
		}
		if ((oclass == POTION_CLASS) && (rn2(100) < u.potionspawnchance)) {
			oclass = COIN_CLASS;
		}
		if ((oclass == SPBOOK_CLASS) && (rn2(100) < u.bookspawnchance)) {
			oclass = COIN_CLASS;
		}
		if ((oclass == WAND_CLASS) && (rn2(100) < u.wandspawnchance)) {
			oclass = COIN_CLASS;
		}
		if ((oclass == AMULET_CLASS) && (rn2(100) < u.amuletspawnchance)) {
			oclass = COIN_CLASS;
		}
		if ((oclass == IMPLANT_CLASS) && (rn2(100) < u.implantspawnchance)) {
			oclass = COIN_CLASS;
		}
		if ((oclass == RING_CLASS) && (rn2(100) < u.ringspawnchance)) {
			oclass = COIN_CLASS;
		}
		if ((oclass == WEAPON_CLASS) && (rn2(100) < u.weaponspawnchance)) {
			oclass = COIN_CLASS;
		}
		if ((oclass == ARMOR_CLASS) && (rn2(100) < u.armorspawnchance)) {
			oclass = COIN_CLASS;
		}
		if ((oclass == TOOL_CLASS) && (rn2(100) < u.toolspawnchance)) {
			oclass = COIN_CLASS;
		}
		if ((oclass == FOOD_CLASS) && (rn2(100) < u.foodspawnchance)) {
			oclass = COIN_CLASS;
		}

		if ((oclass == SCROLL_CLASS) && !wildcard && !timebasedlowerchance() && !timebasedlowerchance() ) {
			oclass = COIN_CLASS;
		}

		if ((oclass == WEAPON_CLASS) && !wildcard && !timebasedlowerchance() && !timebasedlowerchance() ) {
			oclass = COIN_CLASS;
		}

		if ((oclass == ARMOR_CLASS) && !wildcard && !timebasedlowerchance() && !timebasedlowerchance() ) {
			oclass = COIN_CLASS;
		}

		if ((oclass == TOOL_CLASS) && !wildcard && !timebasedlowerchance() && !timebasedlowerchance() ) {
			oclass = COIN_CLASS;
		}

		if ((oclass == POTION_CLASS) && !wildcard && !timebasedlowerchance() && !timebasedlowerchance() ) {
			oclass = COIN_CLASS;
		}

		if ((oclass == WAND_CLASS) && !wildcard && !timebasedlowerchance() && !timebasedlowerchance() ) {
			oclass = COIN_CLASS;
		}

		if ((oclass == AMULET_CLASS) && !wildcard && !timebasedlowerchance() ) {
			oclass = COIN_CLASS;
		}

		if ((oclass == SPBOOK_CLASS) && !wildcard && !timebasedlowerchance() && !timebasedlowerchance() && !timebasedlowerchance() ) {
			oclass = COIN_CLASS;
		}

		if ((oclass == FOOD_CLASS) && !wildcard && !timebasedlowerchance() && !timebasedlowerchance() && !timebasedlowerchance() ) {
			oclass = COIN_CLASS;
		}

		if ((oclass == GEM_CLASS) && !wildcard && !timebasedlowerchance() && (!timebasedlowerchance() || !timebasedlowerchance() ) ) {
			oclass = COIN_CLASS;
		}

		if ((oclass == RING_CLASS) && !wildcard && ( (!timebasedlowerchance() && !timebasedlowerchance() ) || (!timebasedlowerchance() && !timebasedlowerchance() ) ) ) {
			oclass = COIN_CLASS;
		}

		if (oclass >= MAXOCLASSES) {
			impossible("Error: mkobj() random class object returned invalid object class %d. Please tell Amy about this bug. Debug variable: %d", (int) oclass, debugvar);
			oclass = COIN_CLASS;

		}

	}

levscalereroll:

	i = bases[(int)oclass];
	while((prob -= objects[i].oc_prob) > 0) i++;

	if(objects[i].oc_class != oclass)
		panic("probtype error, oclass=%d i=%d", (int) oclass, i);
	if(!OBJ_NAME(objects[i]))
		panic("probtype no object name error, oclass=%d i=%d", (int) oclass, i);

	/* Levelscaler race: very unlikely to get "out of depth items" --Amy */
	if (islevelscaler && i != GOLD_PIECE && (objects[i].oc_minlvl > 1)) {

		levscalediff = level_difficulty();
		if (levscalediff < 1) levscalediff = 1; /* fail safe */

		/*pline("trying to make item %d with level %d (level difficulty is %d)", i, objects[i].oc_minlvl, levscalediff);*/

		while ((levscalediff >= 1) && (objects[i].oc_minlvl > levscalediff)) {
			if (!rn2(3)) {
				/*pline("didn't make the roll! (level difficulty was %d)", levscalediff);*/

				/* somehow there's a weird potential for infinite loops... let's prevent them --Amy */
				if (!rn2(50)) {
					i = GOLD_PIECE;
					goto levscalerollpast;
				}
				prob = rnd(100000);
				goto levscalereroll;
			}
			levscalediff += 1;
		}

	}
levscalerollpast:

	if (oclass == SPBOOK_CLASS) {

		if (u.spellbookbias1 >= 0 && (rnd(100) <= u.spellbookchance1) && (spell_skilltype(i) != u.spellbookbias1)) {

			int spattempts = 0;
			while (spattempts++ < 50000 && (spell_skilltype(i) != u.spellbookbias1)) {
				prob = rnd(100000);
				i = bases[(int)oclass];
				while((prob -= objects[i].oc_prob) > 0) i++;

				if(objects[i].oc_class != oclass)
					panic("probtype error, oclass=%d i=%d", (int) oclass, i);
				if(!OBJ_NAME(objects[i]))
					panic("probtype no object name error, oclass=%d i=%d", (int) oclass, i);
			}

		} else if (u.spellbookbias2 >= 0 && (rnd(100) <= u.spellbookchance2) && (spell_skilltype(i) != u.spellbookbias2)) {
			int spattempts = 0;
			while (spattempts++ < 50000 && (spell_skilltype(i) != u.spellbookbias2)) {
				prob = rnd(100000);
				i = bases[(int)oclass];
				while((prob -= objects[i].oc_prob) > 0) i++;

				if(objects[i].oc_class != oclass)
					panic("probtype error, oclass=%d i=%d", (int) oclass, i);
				if(!OBJ_NAME(objects[i]))
					panic("probtype no object name error, oclass=%d i=%d", (int) oclass, i);
			}

		} else if (u.spellbookbias3 >= 0 && (rnd(100) <= u.spellbookchance3) && (spell_skilltype(i) != u.spellbookbias3)) {
			int spattempts = 0;
			while (spattempts++ < 50000 && (spell_skilltype(i) != u.spellbookbias3)) {
				prob = rnd(100000);
				i = bases[(int)oclass];
				while((prob -= objects[i].oc_prob) > 0) i++;

				if(objects[i].oc_class != oclass)
					panic("probtype error, oclass=%d i=%d", (int) oclass, i);
				if(!OBJ_NAME(objects[i]))
					panic("probtype no object name error, oclass=%d i=%d", (int) oclass, i);
			}

		}

	}

	if (Role_if(PM_TOSSER) && oclass == WEAPON_CLASS) {

		if (!rn2(20) && objects[i].oc_skill != P_JAVELIN) {
			int spattempts = 0;
			while (spattempts++ < 50000 && (objects[i].oc_skill != P_JAVELIN)) {
				prob = rnd(100000);
				i = bases[(int)oclass];
				while((prob -= objects[i].oc_prob) > 0) i++;

				if(objects[i].oc_class != oclass)
					panic("probtype error, oclass=%d i=%d", (int) oclass, i);
				if(!OBJ_NAME(objects[i]))
					panic("probtype no object name error, oclass=%d i=%d", (int) oclass, i);
			}
		}

	}

	if (ismusablenumber(i) && (u.antimusablebias > rn2(100) ) ) {

		prob = rnd(100000);
		i = bases[(int)oclass];
		while((prob -= objects[i].oc_prob) > 0) i++;

		if(objects[i].oc_class != oclass)
			panic("probtype error, oclass=%d i=%d", (int) oclass, i);
		if(!OBJ_NAME(objects[i]))
			panic("probtype no object name error, oclass=%d i=%d", (int) oclass, i);
	}

	/* Gray stones are too common; have a chance to reroll them at least once --Amy */
	if (i >= RIGHT_MOUSE_BUTTON_STONE && i <= NASTY_STONE && rn2(5)) {

		prob = rnd(100000);
		i = bases[(int)oclass];
		while((prob -= objects[i].oc_prob) > 0) i++;

		if(objects[i].oc_class != oclass)
			panic("probtype error, oclass=%d i=%d", (int) oclass, i);
		if(!OBJ_NAME(objects[i]))
			panic("probtype no object name error, oclass=%d i=%d", (int) oclass, i);
	}

	if (i >= ELIF_S_JEWEL && i <= DORA_S_JEWEL && rn2(5)) {

		prob = rnd(100000);
		i = bases[(int)oclass];
		while((prob -= objects[i].oc_prob) > 0) i++;

		if(objects[i].oc_class != oclass)
			panic("probtype error, oclass=%d i=%d", (int) oclass, i);
		if(!OBJ_NAME(objects[i]))
			panic("probtype no object name error, oclass=%d i=%d", (int) oclass, i);
	}

	if (i == SCR_RAGNAROK && rn2(64)) {
		prob = rnd(100000);
		i = bases[(int)oclass];
		while((prob -= objects[i].oc_prob) > 0) i++;

		if(objects[i].oc_class != oclass)
			panic("probtype error, oclass=%d i=%d", (int) oclass, i);
		if(!OBJ_NAME(objects[i]))
			panic("probtype no object name error, oclass=%d i=%d", (int) oclass, i);

	}

	/* you get way too much useful armor... let's limit the amount by changing some into mundane ones --Amy */
	if (oclass == ARMOR_CLASS && !isvanillaarmor(i) && rn2(2)) {

		int armortries = 0;
armorreroll:
		prob = rnd(100000);
		i = bases[(int)oclass];
		while((prob -= objects[i].oc_prob) > 0) i++;

		if(objects[i].oc_class != oclass)
			panic("probtype error, oclass=%d i=%d", (int) oclass, i);
		if(!OBJ_NAME(objects[i]))
			panic("probtype no object name error, oclass=%d i=%d", (int) oclass, i);

		if (!isvanillaarmor(i) && (armortries < 5000)) {
			armortries++;
			goto armorreroll;
		}

	}

	/* the rnd value is 10000 here, this is not an error --Amy */

	if ((objects[u.veryobtainable].oc_class == oclass) && (rnd(10000) < u.veryobtainableboost) ) return(mksobj(u.veryobtainable, TRUE, artif, shopinit));

	else if ((objects[u.veryobtainable2].oc_class == oclass) && (rnd(10000) < u.veryobtainableboost2) ) return(mksobj(u.veryobtainable2, TRUE, artif, shopinit));

	else if ((objects[u.veryobtainable3].oc_class == oclass) && (rnd(10000) < u.veryobtainableboost3) ) return(mksobj(u.veryobtainable3, TRUE, artif, shopinit));

	else if ((objects[u.veryobtainable4].oc_class == oclass) && (rnd(10000) < u.veryobtainableboost4) ) return(mksobj(u.veryobtainable4, TRUE, artif, shopinit));

	else if ((objects[u.veryobtainable5].oc_class == oclass) && (rnd(10000) < u.veryobtainableboost5) ) return(mksobj(u.veryobtainable5, TRUE, artif, shopinit));

	else if ((objects[u.veryobtainable6].oc_class == oclass) && (rnd(10000) < u.veryobtainableboost6) ) return(mksobj(u.veryobtainable6, TRUE, artif, shopinit));

	else if ((objects[u.veryobtainable7].oc_class == oclass) && (rnd(10000) < u.veryobtainableboost7) ) return(mksobj(u.veryobtainable7, TRUE, artif, shopinit));

	else if ((objects[u.veryobtainable8].oc_class == oclass) && (rnd(10000) < u.veryobtainableboost8) ) return(mksobj(u.veryobtainable8, TRUE, artif, shopinit));

	else if ((objects[u.veryobtainable9].oc_class == oclass) && (rnd(10000) < u.veryobtainableboost9) ) return(mksobj(u.veryobtainable9, TRUE, artif, shopinit));

	else if ((objects[u.veryobtainable10].oc_class == oclass) && (rnd(10000) < u.veryobtainableboost10) ) return(mksobj(u.veryobtainable10, TRUE, artif, shopinit));

	else if ((objects[u.veryobtainable11].oc_class == oclass) && (rnd(10000) < u.veryobtainableboost11) ) return(mksobj(u.veryobtainable11, TRUE, artif, shopinit));

	else if ((objects[u.veryobtainable12].oc_class == oclass) && (rnd(10000) < u.veryobtainableboost12) ) return(mksobj(u.veryobtainable12, TRUE, artif, shopinit));

	else if ((objects[u.veryobtainable13].oc_class == oclass) && (rnd(10000) < u.veryobtainableboost13) ) return(mksobj(u.veryobtainable13, TRUE, artif, shopinit));

	else if ((objects[u.veryobtainable14].oc_class == oclass) && (rnd(10000) < u.veryobtainableboost14) ) return(mksobj(u.veryobtainable14, TRUE, artif, shopinit));

	else if ((objects[u.veryobtainable15].oc_class == oclass) && (rnd(10000) < u.veryobtainableboost15) ) return(mksobj(u.veryobtainable15, TRUE, artif, shopinit));

	else if ((objects[u.veryobtainable16].oc_class == oclass) && (rnd(10000) < u.veryobtainableboost16) ) return(mksobj(u.veryobtainable16, TRUE, artif, shopinit));

	else if ((objects[u.veryobtainable17].oc_class == oclass) && (rnd(10000) < u.veryobtainableboost17) ) return(mksobj(u.veryobtainable17, TRUE, artif, shopinit));

	else if ((objects[u.veryobtainable18].oc_class == oclass) && (rnd(10000) < u.veryobtainableboost18) ) return(mksobj(u.veryobtainable18, TRUE, artif, shopinit));

	else if ((objects[u.veryobtainable19].oc_class == oclass) && (rnd(10000) < u.veryobtainableboost19) ) return(mksobj(u.veryobtainable19, TRUE, artif, shopinit));

	else if ((objects[u.veryobtainable20].oc_class == oclass) && (rnd(10000) < u.veryobtainableboost20) ) return(mksobj(u.veryobtainable20, TRUE, artif, shopinit));

	else if (!rn2(100) && oclass == ARMOR_CLASS) {

		int freqhelmet = find_frequent_helmet();
		if (freqhelmet != -1 && freqhelmet < NUM_OBJECTS) {
			return(mksobj(freqhelmet, TRUE, artif, shopinit));
		} else {
			return(mksobj(i, TRUE, artif, shopinit));
		}

	}

	else if (!rn2(100) && issoviet && oclass == WAND_CLASS) {

		return(mksobj(WAN_DESLEXIFICATION, TRUE, artif, shopinit));

	}

	else return(mksobj(i, TRUE, artif, shopinit));
}

STATIC_OVL void
mkbox_cnts(box)
struct obj *box;
{
	register int n, minn = 0;
	register struct obj *otmp;

	box->cobj = (struct obj *) 0;

	switch (box->otyp) {
	case POTATO_BAG:

		otmp = mksobj(makemusableitem(), TRUE, TRUE, FALSE);
		if (otmp) {
			otmp->owt = weight(otmp);
			(void) add_to_container(box, otmp);
		}

		n = (ishaxor ? rnd(2) : rn2(2)); break;
		break;
	case MEDICAL_KIT:	n = (ishaxor ? 120 : 60);
				/* Initial inventory, no empty medical kits */
				if (moves <= 1 && !in_mklev) minn = 1;
				break;
	case TREASURE_CHEST:	n = (ishaxor ? rno(100) : rno(50)); break;
	case ICE_BOX_OF_HOLDING:
	case ICE_BOX_OF_WATERPROOFING:
	case DISPERSION_BOX:
	case ICE_BOX:		n = (ishaxor ? 40 : 20); break;
	case CHEST_OF_HOLDING:
	case NANO_CHEST:
	case CHEST:		n = (ishaxor ? rnd(10) : rnd(5)); break;
	case LARGE_BOX:		n = (ishaxor ? rnd(6) : rnd(3)); break;
	case LEAD_BOX:		n = (ishaxor ? rnd(6) : rnd(3)); break;
	case SACK:
	case HANDYBAG:
	case OILSKIN_SACK:
				/* initial inventory: sack starts out empty */
				if (moves <= 1 && !in_mklev) { n = 0; break; }
				/*else FALLTHRU*/
	case BAG_OF_HOLDING:	n = (ishaxor ? rnd(2) : rn2(2)); break;
	case LARGE_BOX_OF_DIGESTION:
	case ICE_BOX_OF_DIGESTION:
	case BAG_OF_DIGESTION:		/* makes sense, doesn't it ? */
	default:		n = 0; break;
	}

	for (n = rn1(n+1 - minn, minn); n > 0; n--) {
	    if (box->otyp == MEDICAL_KIT) {
		int supplies[] = { PHIAL, BANDAGE, PILL };
		if (!(otmp = mksobj(supplies[rn2(SIZE(supplies))], TRUE, TRUE, FALSE)))
		    continue;
		else
		    otmp->oinvis = otmp->oinvisreal = FALSE;
	    } else
	    if (box->otyp == ICE_BOX || box->otyp == DISPERSION_BOX || box->otyp == ICE_BOX_OF_HOLDING || box->otyp == ICE_BOX_OF_WATERPROOFING || box->otyp == ICE_BOX_OF_DIGESTION) {
		if (!timebasedlowerchance()) continue;
		if (!(otmp = mksobj(CORPSE, TRUE, TRUE, FALSE))) continue;
		/* Note: setting age to 0 is correct.  Age has a different
		 * from usual meaning for objects stored in ice boxes. -KAA
		 */
		otmp->age = 0L;
		otmp->icedobject = TRUE;
		if (otmp->timed) {
		    (void) stop_timer(ROT_CORPSE, (void *)otmp);
		    (void) stop_timer(MOLDY_CORPSE, (void *)otmp);
		    (void) stop_timer(REVIVE_MON, (void *)otmp);
		}
	    } else if (box->otyp == TREASURE_CHEST) {
		register int tprob;
		const struct icp *iprobs = tchestprobs;

		for (tprob = rnd(1000); (tprob -= iprobs->iprob) > 0; iprobs++)
		    ;
		if (!timebasedlowerchance()) continue;
		if (!(otmp = mkobj(iprobs->iclass, TRUE, FALSE))) continue;

		/* handle a couple of special cases */
		if (otmp->oclass == COIN_CLASS) {
		    /* 2.5 x level's usual amount; weight adjusted below */
		    otmp->quan = (long)(rnd(level_difficulty()+5) * rnd(10));

			if (Race_if(PM_VENTURE_CAPITALIST)) {	/* they get extra money, idea by deepy */

				if (rn2(2)) otmp->quan *= 2;
				if (!rn2(5)) otmp->quan *= 3;
				if (!rn2(20)) otmp->quan *= 5;
				if (!rn2(200)) otmp->quan *= 10;
				if (!rn2(1000)) otmp->quan *= 20;
				if (!rn2(5000)) otmp->quan *= 50;
				if (!rn2(25000)) otmp->quan *= 100;

			}

			if (uimplant && uimplant->oartifact == ART_SCROOGE_S_MONEY_MEMORY) {
				otmp->quan *= 2;
				if (powerfulimplants()) otmp->quan *= 2;
			}

			if (uarmh && uarmh->oartifact == ART_GOLD_STANDARD) otmp->quan *= 2;
			if (uarmg && uarmg->oartifact == ART_ROBBERY_GONE_RIGHT) otmp->quan *= 3;

		    otmp->owt = weight(otmp);
		}
	    } else {
		register int tprob;
		const struct icp *iprobs = boxiprobs;

		for (tprob = rnd(1000); (tprob -= iprobs->iprob) > 0; iprobs++)
		    ;
		if (!timebasedlowerchance()) continue;
		if (!(otmp = mkobj(iprobs->iclass, TRUE, FALSE))) continue;

		/* handle a couple of special cases */
		if (otmp->oclass == COIN_CLASS) {
		    /* 2.5 x level's usual amount; weight adjusted below */
		    otmp->quan = (long)(rnd(level_difficulty()+5) * rnd(10));

			if (Race_if(PM_VENTURE_CAPITALIST)) {	/* they get extra money, idea by deepy */

				if (rn2(2)) otmp->quan *= 2;
				if (!rn2(5)) otmp->quan *= 3;
				if (!rn2(20)) otmp->quan *= 5;
				if (!rn2(200)) otmp->quan *= 10;
				if (!rn2(1000)) otmp->quan *= 20;
				if (!rn2(5000)) otmp->quan *= 50;
				if (!rn2(25000)) otmp->quan *= 100;

			}

			if (uarmh && uarmh->oartifact == ART_GOLD_STANDARD) otmp->quan *= 2;
			if (uarmg && uarmg->oartifact == ART_ROBBERY_GONE_RIGHT) otmp->quan *= 3;

		    otmp->owt = weight(otmp);

		/* no reason not to allow stones... but in Soviet Russia, everything is a weird mix of special cases
		 * held together by special cases, and woe to you if you remove any of them because that will make the entire
		 * house of cards that is communism come down in a mighty crash. --Amy */
		} else while (issoviet && otmp->otyp == ROCK) {
		    otmp->otyp = rnd_class(DILITHIUM_CRYSTAL, FLINT);
		    if (otmp->quan > 2L) otmp->quan = 1L;
		    otmp->owt = weight(otmp);
		}
		if (box->otyp == BAG_OF_HOLDING || box->otyp == ICE_BOX_OF_HOLDING || box->otyp == CHEST_OF_HOLDING) {
		    if (Is_mbag(otmp)) {
			otmp->otyp = SACK;
			otmp->spe = 0;
			otmp->owt = weight(otmp);
		    } else while (otmp->otyp == WAN_CANCELLATION)
			    otmp->otyp = rnd_class(WAN_LIGHT, WAN_FIREBALL);
		}
	    }
	    (void) add_to_container(box, otmp);
	}
}

int
rndmonnum()	/* select a random, common monster type */
{
	register struct permonst *ptr;
	register int	i;

	/* Plan A: get a level-appropriate common monster */
	ptr = rndmonst();
	if (ptr) return(monsndx(ptr));

	/* Plan B: get any common monster */
	do {
	    i = rn1(SPECIAL_PM - LOW_PM, LOW_PM);
	    ptr = &mons[i];
	} while((ptr->geno & G_NOGEN) /*|| (!Inhell && (ptr->geno & G_HELL))*/);

	return(i);
}

int
usefulitem() /* select the ID number of an item that may be useful for the player --Amy */
{

	switch (rnd(152)) {

		case 1:
			return ATHAME;
		case 2:
			return WORM_TOOTH;
		case 3:
			return AMULET_OF_LIFE_SAVING;
		case 4:
			return AMULET_VERSUS_STONE;
		case 5:
			return ICE_BOX;
		case 6:
			return MAGIC_WHISTLE;
		case 7:
		case 8:
			return FROST_HORN;
		case 9:
		case 10:
			return FIRE_HORN;
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			return UNICORN_HORN;
		case 16:
			return STETHOSCOPE;
		case 17:
			return TINNING_KIT;
		case 18:
			return CAN_OF_GREASE;
		case 19:
			return MAGIC_MARKER;
		case 20:
		case 21:
		case 22:
		case 23:
		case 24:
		case 25:
		case 26:
		case 27:
		case 28:
		case 29:
			return CHEMISTRY_SET;
		case 30:
			return HUGE_CHUNK_OF_MEAT;
		case 31:
			return KELP_FROND;
		case 32:
		case 33:
			return EUCALYPTUS_LEAF;
		case 34:
		case 35:
			return LUMP_OF_ROYAL_JELLY;
		case 36:
		case 37:
			return HACKER_S_FOOD;
		case 38:
		case 39:
			return POT_RESTORE_ABILITY;
		case 40:
		case 41:
		case 42:
		case 43:
		case 44:
			return POT_GAIN_ENERGY;
		case 45:
		case 46:
		case 47:
			return POT_GAIN_HEALTH;
		case 48:
		case 49:
		case 50:
		case 51:
		case 52:
			return POT_FULL_HEALING;
		case 53:
		case 54:
		case 55:
		case 56:
		case 57:
			return POT_GAIN_ABILITY;
		case 58:
		case 59:
		case 60:
		case 61:
		case 62:
			return POT_GAIN_LEVEL;
		case 63:
		case 64:
		case 65:
			return POT_WATER;
		case 66:
		case 67:
			return SCR_CREATE_FAMILIAR;
		case 68:
			return SCR_TAMING;
		case 69:
			return SCR_INVENTORY_ID;
		case 70:
		case 71:
		case 72:
		case 73:
		case 74:
			return SCR_GAIN_MANA;
		case 75:
		case 76:
		case 77:
			return SCR_ENCHANT_WEAPON;
		case 78:
		case 79:
			return SCR_ENCHANT_ARMOR;
		case 80:
		case 81:
		case 82:
		case 83:
		case 84:
			return SCR_RANDOM_ENCHANTMENT;
		case 85:
		case 86:
			return SCR_REMOVE_CURSE;
		case 87:
		case 88:
		case 89:
		case 90:
		case 91:
			return SCR_TELEPORTATION;
		case 92:
		case 93:
			return SCR_TELE_LEVEL;
		case 94:
			return SCR_CURE;
		case 95:
			return SCR_TRAP_DISARMING;
		case 96:
		case 97:
		case 98:
			return SCR_CHARGING;
		case 99:
		case 100:
			return SCR_GENOCIDE;
		case 101:
		case 102:
			return SCR_TRAP_DETECTION;
		case 103:
			return SCR_ACQUIREMENT;
		case 104:
			return SCR_CONSECRATION;
		case 105:
			return SCR_ENTHRONIZATION;
		case 106:
			return SPE_CURE_HALLUCINATION;
		case 107:
			return SPE_EXTRA_HEALING;
		case 108:
			return SPE_FULL_HEALING;
		case 109:
			return SPE_RESTORE_ABILITY;
		case 110:
			return SPE_CREATE_FAMILIAR;
		case 111:
			return SPE_IDENTIFY;
		case 112:
		case 113:
		case 114:
		case 115:
		case 116:
			return SPE_CHEMISTRY;
		case 117:
			return SPE_REMOVE_CURSE;
		case 118:
			return SPE_LEVITATION;
		case 119:
		case 120:
			return SPE_REPAIR_ARMOR;
		case 121:
			return SPE_MAGIC_MISSILE;
		case 122:
			return SPE_FORCE_BOLT;
		case 123:
			return WAN_MANA;
		case 124:
			return WAN_GAIN_LEVEL;
		case 125:
		case 126:
		case 127:
			return WAN_HEALING;
		case 128:
			return WAN_IDENTIFY;
		case 129:
			return WAN_REMOVE_CURSE;
		case 130:
			return WAN_TRAP_DISARMING;
		case 131:
			return WAN_ENTRAPPING;
		case 132:
			return WAN_INCREASE_MAX_HITPOINTS;
		case 133:
			return WAN_CREATE_FAMILIAR;
		case 134:
		case 135:
			return WAN_TELEPORTATION;
		case 136:
			return WAN_EXTRA_HEALING;
		case 137:
			return WAN_FULL_HEALING;
		case 138:
			return WAN_ACQUIREMENT;
		case 139:
			return WAN_CHARGING;
		case 140:
		case 141:
			return WAN_MAGIC_MISSILE;
		case 142:
			return WAN_FIRE;
		case 143:
			return WAN_SLEEP;
		case 144:
			return WAN_DEATH;
		case 145:
		case 146:
			return WAN_COLD;
		case 147:
			return WAN_LIGHTNING;
		case 148:
			return WAN_ACID;
		case 149:
			return WAN_SOLAR_BEAM;
		case 150:
			return SPE_REPAIR_WEAPON;
		case 151:
			return SCR_MAKE_PENTAGRAM;
		case 152:
			return WAN_RESTORATION;
		default: /* fail safe */
			return POT_FULL_HEALING;
	}

}

int
nastymusableitem() /* select the ID number of an item that the monsters may use against you --Amy */
{

	switch (rnd(208)) {

		case 1:
		case 2:
		case 3:
			return WAN_CREATE_MONSTER;
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
			return SCR_CREATE_MONSTER;
		case 11:
		case 12:
		case 13:
			return SCR_CREATE_VICTIM;
		case 14:
		case 15:
			return WAN_CREATE_HORDE;
		case 16:
		case 17:
		case 18:
			return RIN_TIMELY_BACKUP;
		case 19:
		case 20:
		case 21:
			return SCR_ROOT_PASSWORD_DETECTION;
		case 22:
		case 23:
		case 24:
		case 25:
		case 26:
		case 27:
		case 28:
			return SCR_SUMMON_UNDEAD;
		case 29:
		case 30:
		case 31:
			return WAN_SUMMON_UNDEAD;
		case 32:
			return SCR_WARPING;
		case 33:
			return WAN_REDUCE_MAX_HITPOINTS;
		case 34:
			return WAN_INCREASE_MAX_HITPOINTS;
		case 35:
		case 36:
			return POT_AMNESIA;
		case 37:
			return POT_CYANIDE;
		case 38:
			return POT_RADIUM;
		case 39:
		case 40:
		case 41:
		case 42:
			return SCR_TRAP_CREATION;
		case 43:
		case 44:
			return SCR_CREATE_TRAP;
		case 45:
		case 46:
			return WAN_TRAP_CREATION;
		case 47:
			return SCR_FLOOD;
		case 48:
			return SCR_LAVA;
		case 49:
			return SCR_GROWTH;
		case 50:
			return SCR_BARRHING;
		case 51:
			return SCR_LOCKOUT;
		case 52:
			return (rn2(10) ? WAN_TELEPORTATION : WAN_BANISHMENT);
		case 53:
		case 54:
		case 55:
		case 56:
		case 57:
		case 58:
		case 59:
			return POT_HALLUCINATION;
		case 60:
		case 61:
		case 62:
			return POT_ICE;
		case 63:
		case 64:
		case 65:
		case 66:
		case 67:
		case 68:
		case 69:
			return POT_STUNNING;
		case 70:
		case 71:
		case 72:
		case 73:
		case 74:
		case 75:
		case 76:
			return POT_NUMBNESS;
		case 77:
			return POT_URINE;
		case 78:
			return POT_CANCELLATION;
		case 79:
			return POT_SLIME;
		case 80:
		case 81:
		case 82:
		case 83:
		case 84:
		case 85:
			return SCR_BAD_EFFECT;
		case 86:
			return WAN_BAD_EFFECT;
		case 87:
		case 88:
		case 89:
		case 90:
		case 91:
		case 92:
		case 93:
			return POT_FIRE;
		case 94:
			return WAN_GAIN_LEVEL;
		case 95:
			return WAN_MUTATION;
		case 96:
			return WAN_CLONE_MONSTER;
		case 97:
			return SCR_DESTROY_ARMOR;
		case 98:
			return SCR_DESTROY_WEAPON;
		case 99:
			return SCR_STONING;
		case 100:
			return SCR_AMNESIA;
		case 101:
		case 102:
			return BAG_OF_TRICKS;
		case 103:
			return WAN_STONING;
		case 104:
			return WAN_DISINTEGRATION;
		case 105:
			return WAN_PARALYSIS;
		case 106:
			return WAN_CURSE_ITEMS;
		case 107:
			return WAN_AMNESIA;
		case 108:
			return WAN_BAD_LUCK;
		case 109:
			return WAN_REMOVE_RESISTANCE;
		case 110:
			return WAN_CORROSION;
		case 111:
			return WAN_STARVATION;
		case 112:
			return WAN_CONFUSION;
		case 113:
			return WAN_SLIMING;
		case 114:
			return WAN_LYCANTHROPY;
		case 115:
			return WAN_FUMBLING;
		case 116:
			return WAN_PUNISHMENT;
		case 117:
		case 118:
			return SCR_PUNISHMENT;
		case 119:
		case 120:
		case 121:
		case 122:
		case 123:
			return SCR_SUMMON_BOSS;
		case 124:
		case 125:
		case 126:
		case 127:
		case 128:
		case 129:
		case 130:
			return SCR_WOUNDS;
		case 131:
		case 132:
			return SCR_BULLSHIT;
		case 133:
			return SCR_CHAOS_TERRAIN;
		case 134:
			return SCR_NASTINESS;
		case 135:
			return SCR_DEMONOLOGY;
		case 136:
			return SCR_ELEMENTALISM;
		case 137:
			return SCR_GIRLINESS;
		case 138:
			return WAN_SUMMON_SEXY_GIRL;
		case 139:
		case 140:
		case 141:
		case 142:
		case 143:
			return SCR_GROUP_SUMMONING;
		case 144:
		case 145:
		case 146:
			return WAN_STUN_MONSTER;
		case 147:
			return SCR_SUMMON_GHOST;
		case 148:
			return SCR_MEGALOAD;
		case 149:
			return SCR_ENRAGE;
		case 150:
			return WAN_TIDAL_WAVE;
		case 151:
			return SCR_ANTIMATTER;
		case 152:
		case 153:
			return SCR_SUMMON_ELM;
		case 154:
			return WAN_SUMMON_ELM;
		case 155:
			return WAN_DRAIN_MANA;
		case 156:
			return WAN_FINGER_BENDING;
		case 157:
		case 158:
		case 159:
			return SCR_IMMOBILITY;
		case 160:
			return WAN_IMMOBILITY;
		case 161:
			return SCR_FLOODING;
		case 162:
		case 163:
		case 164:
			return SCR_EGOISM;
		case 165:
			return WAN_EGOISM;
		case 166:
			return SCR_SIN;
		case 167:
			return WAN_SIN;
		case 168:
			return WAN_INERTIA;
		case 169:
			return WAN_TIME;
		case 170:
			return WAN_LEVITATION;
		case 171:
			return SCR_VILENESS;
		case 172:
		case 173:
		case 174:
		case 175:
		case 176:
		case 177:
		case 178:
			return POT_DIMNESS;
		case 179:
			return SCR_OFFLEVEL_ITEM;
		case 180:
			return SCR_NASTY_CURSE;
		case 181:
			return WAN_FLEECY_TERRAIN;
		case 182:
			return WAN_DISENCHANTMENT;
		case 183:
			return WAN_CONTAMINATION;
		case 184:
			return WAN_TREMBLING;
		case 185:
			return SCR_GRAVE;
		case 186:
			return SCR_TUNNELS;
		case 187:
			return SCR_FARMING;
		case 188:
			return SCR_MOUNTAINS;
		case 189:
			return SCR_DIVING;
		case 190:
			return SCR_CRYSTALLIZATION;
		case 191:
			return SCR_MOORLAND;
		case 192:
			return SCR_URINE;
		case 193:
			return SCR_QUICKSAND;
		case 194:
			return SCR_STYX;
		case 195:
			return SCR_SNOW;
		case 196:
			return SCR_ASH;
		case 197:
			return SCR_SAND;
		case 198:
			return SCR_PAVING;
		case 199:
			return SCR_HIGHWAY;
		case 200:
			return SCR_GRASSLAND;
		case 201:
			return SCR_NETHER;
		case 202:
			return SCR_STALACTITE;
		case 203:
			return SCR_CRYPT;
		case 204:
			return SCR_BUBBLE_BOBBLE;
		case 205:
			return SCR_RAIN;
		case 206:
			return WAN_CHAOS_TERRAIN;
		case 207:
			return SCR_EVIL_VARIANT;
		case 208:
			return WAN_STAT_REDUCTION;
		default: /* fail safe */
			return WAN_CREATE_HORDE;
	}

}

int
makemusableitem() /* select the ID number of a musable item --Amy */
{
	switch (rnd(319)) {

		case 1: return WAN_SLEEP;
		case 2: return WAN_FIREBALL;
		case 3: return WAN_FIRE;
		case 4: return WAN_COLD;
		case 5: return WAN_LIGHTNING;
		case 6: return WAN_MAGIC_MISSILE;
		case 7: return WAN_STRIKING;
		case 8: return SCR_FIRE;
		case 9: return POT_PARALYSIS;
		case 10: return POT_BLINDNESS;
		case 11: return POT_CONFUSION;
		case 12: return POT_SLEEPING;
		case 13: return POT_ACID;
		case 14: return FROST_HORN;
		case 15: return FIRE_HORN;
		case 16: return WAN_DRAINING;
		case 17: return SCR_EARTH;
		case 18: return POT_AMNESIA;
		case 19: return WAN_CANCELLATION;
		case 20: return POT_CYANIDE;
		case 21: return POT_RADIUM;
		case 22: return WAN_ACID;
		case 23: return SCR_TRAP_CREATION;
		case 24: return WAN_TRAP_CREATION;
		case 25: return SCR_FLOOD;
		case 26: return SCR_LAVA;
		case 27: return SCR_GROWTH;
		case 28: return SCR_ICE;
		case 29: return SCR_CLOUDS;
		case 30: return SCR_BARRHING;
		case 31: return WAN_SOLAR_BEAM;
		case 32: return SCR_LOCKOUT;
		case 33: return (rn2(10) ? WAN_TELEPORTATION : WAN_BANISHMENT);
		case 34: return POT_HALLUCINATION;
		case 35: return POT_NUMBNESS;
		case 36: return POT_ICE;
		case 37: return POT_STUNNING;
		case 38: return SCR_BAD_EFFECT;
		case 39: return WAN_BAD_EFFECT;
		case 40: return POT_FIRE;
		case 41: return WAN_SLOW_MONSTER;
		case 42: return WAN_FEAR;
		case 43: return POT_FEAR;
		case 44: return SCR_DESTROY_ARMOR;
		case 45: return SCR_STONING;
		case 46: return POT_URINE;
		case 47: return POT_SLIME;
		case 48: return POT_CANCELLATION;
		case 49: return WAN_STONING;
		case 50: return WAN_DISINTEGRATION;
		case 51: return WAN_PARALYSIS;
		case 52: return WAN_CURSE_ITEMS;
		case 53: return WAN_AMNESIA;
		case 54: return WAN_BAD_LUCK;
		case 55: return WAN_REMOVE_RESISTANCE;
		case 56: return WAN_CORROSION;
		case 57: return WAN_FUMBLING;
		case 58: return WAN_STARVATION;
		case 59: return WAN_PUNISHMENT;
		case 60: return SCR_PUNISHMENT;
		case 61: return WAN_MAKE_VISIBLE;
		case 62: return WAN_REDUCE_MAX_HITPOINTS;
		case 63: return WAN_CONFUSION;
		case 64: return WAN_SLIMING;
		case 65: return WAN_LYCANTHROPY;
		case 66: return SCR_CHAOS_TERRAIN;
		case 67: return SCR_WOUNDS;
		case 68: return SCR_BULLSHIT;
		case 69: return SCR_AMNESIA;
		case 70: return WAN_SUMMON_SEXY_GIRL;
		case 71: return SCR_DEMONOLOGY;
		case 72: return SCR_NASTINESS;
		case 73: return SCR_GIRLINESS;
		case 74: return SCR_ELEMENTALISM;
		case 75: return TEMPEST_HORN;
		case 76: return WAN_POISON;
		case 77: return SCR_DESTROY_WEAPON;
		case 78: return WAN_DISINTEGRATION_BEAM;
		case 79: return WAN_CHROMATIC_BEAM;
		case 80: return WAN_STUN_MONSTER;
		case 81: return SCR_MEGALOAD;
		case 82: return SCR_ENRAGE;
		case 83: return WAN_TIDAL_WAVE;
		case 84: return SCR_ANTIMATTER;
		case 85: return WAN_DRAIN_MANA;
		case 86: return WAN_FINGER_BENDING;
		case 87: return SCR_IMMOBILITY;
		case 88: return WAN_IMMOBILITY;
		case 89: return SCR_FLOODING;
		case 90: return SCR_EGOISM;
		case 91: return WAN_EGOISM;
		case 92: return SCR_RUMOR;
		case 93: return SCR_MESSAGE;
		case 94: return SCR_SIN;
		case 95: return WAN_SIN;
		case 96: return WAN_INERTIA;
		case 97: return WAN_TIME;
		case 98: return WAN_LEVITATION;
		case 99: return WAN_PSYBEAM;
		case 100: return WAN_HYPER_BEAM;
		case 101: return WAN_STRIKING;
		case 102: return POT_ACID;
		case 103: return POT_CONFUSION;
		case 104: return POT_BLINDNESS;
		case 105: return POT_SLEEPING;
		case 106: return POT_PARALYSIS;
		case 107: return WAN_MAGIC_MISSILE;
		case 108: return WAN_SLEEP;
		case 109: return WAN_FIRE;
		case 110: return WAN_COLD;
		case 111: return WAN_STRIKING;
		case 112: return POT_ACID;
		case 113: return POT_CONFUSION;
		case 114: return POT_BLINDNESS;
		case 115: return POT_SLEEPING;
		case 116: return POT_PARALYSIS;
		case 117: return WAN_MAGIC_MISSILE;
		case 118: return WAN_SLEEP;
		case 119: return WAN_FIRE;
		case 120: return WAN_COLD;
		case 121: return WAN_STRIKING;
		case 122: return POT_ACID;
		case 123: return POT_CONFUSION;
		case 124: return POT_BLINDNESS;
		case 125: return POT_SLEEPING;
		case 126: return POT_PARALYSIS;
		case 127: return WAN_MAGIC_MISSILE;
		case 128: return WAN_SLEEP;
		case 129: return WAN_FIRE;
		case 130: return WAN_COLD;
		case 131: return WAN_STRIKING;
		case 132: return POT_ACID;
		case 133: return POT_CONFUSION;
		case 134: return POT_BLINDNESS;
		case 135: return POT_SLEEPING;
		case 136: return POT_PARALYSIS;
		case 137: return WAN_MAGIC_MISSILE;
		case 138: return WAN_SLEEP;
		case 139: return WAN_FIRE;
		case 140: return WAN_COLD;
		case 141: return WAN_STRIKING;
		case 142: return POT_ACID;
		case 143: return POT_CONFUSION;
		case 144: return POT_BLINDNESS;
		case 145: return POT_SLEEPING;
		case 146: return POT_PARALYSIS;
		case 147: return WAN_MAGIC_MISSILE;
		case 148: return WAN_SLEEP;
		case 149: return WAN_FIRE;
		case 150: return WAN_COLD;
		case 151: return WAN_LIGHTNING;
		case 152: return WAN_LIGHTNING;
		case 153: return WAN_LIGHTNING;
		case 154: return WAN_LIGHTNING;
		case 155: return WAN_LIGHTNING;
		case 156: return POT_HALLUCINATION;
		case 157: return POT_NUMBNESS;
		case 158: return POT_ICE;
		case 159: return POT_STUNNING;
		case 160: return SCR_BAD_EFFECT;
		case 161: return POT_FIRE;
		case 162: return POT_FEAR;
		case 163: return POT_HALLUCINATION;
		case 164: return POT_NUMBNESS;
		case 165: return POT_ICE;
		case 166: return POT_STUNNING;
		case 167: return SCR_BAD_EFFECT;
		case 168: return POT_FIRE;
		case 169: return POT_FEAR;
		case 170: return POT_HALLUCINATION;
		case 171: return POT_NUMBNESS;
		case 172: return POT_ICE;
		case 173: return POT_STUNNING;
		case 174: return SCR_BAD_EFFECT;
		case 175: return POT_FIRE;
		case 176: return POT_FEAR;
		case 177: return POT_HALLUCINATION;
		case 178: return POT_NUMBNESS;
		case 179: return POT_ICE;
		case 180: return POT_STUNNING;
		case 181: return SCR_BAD_EFFECT;
		case 182: return POT_FIRE;
		case 183: return POT_FEAR;
		case 184: return POT_HALLUCINATION;
		case 185: return POT_NUMBNESS;
		case 186: return POT_ICE;
		case 187: return POT_STUNNING;
		case 188: return SCR_BAD_EFFECT;
		case 189: return POT_FIRE;
		case 190: return POT_FEAR;
		case 191: return WAN_STRIKING;
		case 192: return POT_ACID;
		case 193: return POT_CONFUSION;
		case 194: return POT_BLINDNESS;
		case 195: return POT_SLEEPING;
		case 196: return POT_PARALYSIS;
		case 197: return WAN_MAGIC_MISSILE;
		case 198: return WAN_SLEEP;
		case 199: return WAN_FIRE;
		case 200: return WAN_COLD;
		case 201: return WAN_LIGHTNING;
		case 202: return WAN_INFERNO;
		case 203: return WAN_ICE_BEAM;
		case 204: return WAN_THUNDER;
		case 205: return WAN_SLUDGE;
		case 206: return WAN_TOXIC;
		case 207: return WAN_NETHER_BEAM;
		case 208: return WAN_AURORA_BEAM;
		case 209: return WAN_GRAVITY_BEAM;
		case 210: return WAN_CHLOROFORM;
		case 211: return WAN_DREAM_EATER;
		case 212: return WAN_BUBBLEBEAM;
		case 213: return WAN_GOOD_NIGHT;
		case 214: return SCR_VILENESS;
		case 215: return POT_DIMNESS;
		case 216: return POT_DIMNESS;
		case 217: return POT_DIMNESS;
		case 218: return POT_DIMNESS;
		case 219: return POT_DIMNESS;
		case 220: return POT_DIMNESS;
		case 221: return POT_DIMNESS;
		case 222: return SCR_OFFLEVEL_ITEM;
		case 223: return POT_SANITY;
		case 224: return POT_SANITY;
		case 225: return POT_SANITY;
		case 226: return POT_SANITY;
		case 227: return SCR_BAD_EQUIPMENT;
		case 228: return WAN_INSANITY;
		case 229: return WAN_BAD_EQUIPMENT;
		case 230: return SCR_NASTY_CURSE;
		case 231: return SCR_HYBRIDIZATION;
		case 232: return WAN_DISENCHANTMENT;
		case 233: return WAN_TREMBLING;
		case 234: return WAN_CONTAMINATION;
		case 235: return SCR_GRAVE;
		case 236: return SCR_TUNNELS;
		case 237: return SCR_FARMING;
		case 238: return SCR_MOUNTAINS;
		case 239: return SCR_DIVING;
		case 240: return SCR_CRYSTALLIZATION;
		case 241: return SCR_MOORLAND;
		case 242: return SCR_URINE;
		case 243: return SCR_QUICKSAND;
		case 244: return SCR_STYX;
		case 245: return SCR_SNOW;
		case 246: return SCR_ASH;
		case 247: return SCR_SAND;
		case 248: return SCR_PAVING;
		case 249: return SCR_HIGHWAY;
		case 250: return SCR_GRASSLAND;
		case 251: return SCR_NETHER;
		case 252: return SCR_STALACTITE;
		case 253: return SCR_CRYPT;
		case 254: return SCR_BUBBLE_BOBBLE;
		case 255: return SCR_RAIN;
		case 256: return WAN_CHAOS_TERRAIN;
		case 257: return WAN_FLEECY_TERRAIN;
		case 258: return ETHER_HORN;
		case 259: return SHADOW_HORN;
		case 260: return CHROME_HORN;
		case 261: return SCR_ILLUSION;
		case 262: return SCR_EVIL_VARIANT;
		case 263: return SCR_FEMINISM;
		case 264: return WAN_BLEEDING;
		case 265: return WAN_UNDRESSING;
		case 266: return WAN_STAT_REDUCTION;
		case 267: return SCR_VISIBLE_ITEM;
		case 268: return WAN_DEATH;
		case 269: return SCR_TELEPORTATION;
		case 270: return POT_HEALING;
		case 271: return POT_EXTRA_HEALING;
		case 272: return WAN_DIGGING;
		case 273: return WAN_CREATE_MONSTER;
		case 274: return SCR_CREATE_MONSTER;
		case 275: return WAN_TELEPORTATION;
		case 276: return BUGLE;
		case 277: return UNICORN_HORN;
		case 278: return POT_FULL_HEALING;
		case 279: return SCR_SUMMON_UNDEAD;
		case 280: return WAN_HEALING;
		case 281: return WAN_EXTRA_HEALING;
		case 282: return WAN_CREATE_HORDE;
		case 283: return POT_VAMPIRE_BLOOD;
		case 284: return WAN_FULL_HEALING;
		case 285: return SCR_TELE_LEVEL;
		case 286: return SCR_ROOT_PASSWORD_DETECTION;
		case 287: return RIN_TIMELY_BACKUP;
		case 288: return WAN_SUMMON_UNDEAD;
		case 289: return SCR_HEALING;
		case 290: return SCR_WARPING;
		case 291: return BAG_OF_TRICKS;
		case 292: return WAN_TELE_LEVEL;
		case 293: return SCR_SUMMON_BOSS;
		case 294: return POT_CURE_WOUNDS;
		case 295: return POT_CURE_SERIOUS_WOUNDS;
		case 296: return POT_CURE_CRITICAL_WOUNDS;
		case 297: return SCR_POWER_HEALING;
		case 298: return SCR_CREATE_VICTIM;
		case 299: return SCR_GROUP_SUMMONING;
		case 300: return SCR_SUMMON_GHOST;
		case 301: return SCR_SUMMON_ELM;
		case 302: return WAN_SUMMON_ELM;
		case 303: return SCR_RELOCATION;
		case 304: return SCR_EXTRA_HEALING;
		case 305: return POT_BLOOD;
		case 306: return POT_GAIN_LEVEL;
		case 307: return WAN_MAKE_INVISIBLE;
		case 308: return POT_INVISIBILITY;
		case 309: return WAN_POLYMORPH;
		case 310: return POT_SPEED;
		case 311: return WAN_SPEED_MONSTER;
		case 312: return BULLWHIP;
		case 313: return POT_POLYMORPH;
		case 314: return WAN_CLONE_MONSTER;
		case 315: return WAN_HASTE_MONSTER;
		case 316: return POT_MUTATION;
		case 317: return WAN_MUTATION;
		case 318: return WAN_GAIN_LEVEL;
		case 319: return WAN_INCREASE_MAX_HITPOINTS;
		default: return WAN_MAGIC_MISSILE;

	}

	return WAN_MAGIC_MISSILE; /* to appease the compiler */

}

int
makegreatitem() /* select the ID number of an item that might be very useful for you --Amy */
{

	switch (rnd(177)) {
		case 1:
			return TOOTH_OF_AN_ALGOLIAN_SUNTIGER;
		case 2:
			return DWARVISH_BATTLE_AXE;
		case 3:
			return ELECTRIC_SWORD;
		case 4:
			return BASTERD_SWORD;
		case 5:
			return HUGE_CLUB;
		case 6:
			return LOG;
		case 7:
			return INSECT_SQUASHER;
		case 8:
			return BOAT_OAR;
		case 9:
			return REINFORCED_MACE;
		case 10:
			return DEVIL_STAR;
		case 11:
			return OBSID;
		case 12:
			return SLEDGE_HAMMER;
		case 13:
			return MALLET;
		case 14:
			return BATTLE_STAFF;
		case 15:
			return TORPEDO;
		case 16:
			return MANCATCHER;
		case 17:
			return HYDRA_BOW;
		case 18:
			return CATAPULT;
		case 19:
			return BFG;
		case 20:
			return DEMON_CROSSBOW;
		case 21:
			return BODYGLOVE;
		case 22:
			return KYRT_SHIRT;
		case 23:
			return FULL_PLATE_MAIL;
		case 24:
			return ROBE_OF_MAGIC_RESISTANCE;
		case 25:
			return GRAY_DRAGON_SCALE_MAIL;
		case 26:
			return SILVER_DRAGON_SCALE_MAIL;
		case 27:
			return MERCURIAL_DRAGON_SCALE_MAIL;
		case 28:
			return DEEP_DRAGON_SCALE_MAIL;
		case 29:
			return BLACK_DRAGON_SCALE_MAIL;
		case 30:
			return GOLDEN_DRAGON_SCALE_MAIL;
		case 31:
			return STONE_DRAGON_SCALE_MAIL;
		case 32:
			return SKY_DRAGON_SCALE_MAIL;
		case 33:
			return CLOAK_OF_MAGIC_RESISTANCE;
		case 34:
			return CLOAK_OF_DRAIN_RESISTANCE;
		case 35:
			return CLOAK_OF_REFLECTION;
		case 36:
			return WING_CLOAK;
		case 37:
			return CLOAK_OF_STABILITY;
		case 38:
			return COCLOAK;
		case 39:
			return CLOAK_OF_SHIFTING;
		case 40:
			return SENSOR_CLOAK;
		case 41:
			return CLOAK_OF_SPEED;
		case 42:
			return OPERATION_CLOAK;
		case 43:
			return CLOAK_OF_FREEDOM;
		case 44:
			return CLOAK_OF_CURSE_CATCHING;
		case 45:
			return HELM_OF_DRAIN_RESISTANCE;
		case 46:
			return HELM_OF_SPEED;
		case 47:
			return HELM_OF_DETOXIFICATION;
		case 48:
			return HELM_OF_TELEPATHY;
		case 49:
			return ORIHALCYON_GAUNTLETS;
		case 50:
			return GAUNTLETS_OF_REFLECTION;
		case 51:
			return GAUNTLETS_OF_FREE_ACTION;
		case 52:
			return SHIELD_OF_MOBILITY;
		case 53:
			return GRAY_DRAGON_SCALE_SHIELD;
		case 54:
			return SILVER_DRAGON_SCALE_SHIELD;
		case 55:
			return MERCURIAL_DRAGON_SCALE_SHIELD;
		case 56:
			return DEEP_DRAGON_SCALE_SHIELD;
		case 57:
			return BLACK_DRAGON_SCALE_SHIELD;
		case 58:
			return GOLDEN_DRAGON_SCALE_SHIELD;
		case 59:
			return STONE_DRAGON_SCALE_SHIELD;
		case 60:
			return SKY_DRAGON_SCALE_SHIELD;
		case 61:
			return SPEED_BOOTS;
		case 62:
			return FLYING_BOOTS;
		case 63:
			return BOOTS_OF_FREEDOM;
		case 64:
			return BOOTS_OF_TOTAL_STABILITY;
		case 65:
			return RIN_DRAIN_RESISTANCE;
		case 66:
			return RIN_MAGIC_RESISTANCE;
		case 67:
			return RIN_MATERIAL_STABILITY;
		case 68:
			return RIN_SICKNESS_RESISTANCE;
		case 69:
			return RIN_ALACRITY;
		case 70:
			return RIN_FREE_ACTION;
		case 71:
			return RIN_POLYMORPH_CONTROL;
		case 72:
			return AMULET_OF_DRAIN_RESISTANCE;
		case 73:
			return AMULET_OF_ESP;
		case 74:
			return AMULET_OF_FLYING;
		case 75:
			return AMULET_OF_LIFE_SAVING;
		case 76:
			return AMULET_OF_REFLECTION;
		case 77:
			return AMULET_OF_DEPETRIFY;
		case 78:
			return AMULET_OF_MAGIC_RESISTANCE;
		case 79:
			return AMULET_OF_SICKNESS_RESISTANCE;
		case 80:
			return AMULET_OF_DISINTEGRATION_RESIS;
		case 81:
			return AMULET_OF_SPEED;
		case 82:
			return AMULET_OF_POLYMORPH_CONTROL;
		case 83:
			return AMULET_OF_PRISM;
		case 84:
			return AMULET_OF_WARP_DIMENSION;
		case 85:
			return AMULET_VERSUS_CURSES;
		case 86:
			return CHEST_OF_HOLDING;
		case 87:
			return ICE_BOX_OF_HOLDING;
		case 88:
			return ICE_BOX_OF_WATERPROOFING;
		case 89:
			return BAG_OF_HOLDING;
		case 90:
			return MAGIC_CANDLE;
		case 91:
			return MAGIC_WHISTLE;
		case 92:
			return MAGIC_HARP;
		case 93:
			return RED_DOUBLE_LIGHTSABER;
		case 94:
			return MAGIC_MARKER;
		case 95:
			return SWITCHER;
		case 96:
			return GOD_O_METER;
		case 97:
			return CHEMISTRY_SET;
		case 98:
			return POT_GAIN_HEALTH;
		case 99:
			return POT_FULL_HEALING;
		case 100:
			return POT_GAIN_LEVEL;
		case 101:
			return POT_INVULNERABILITY;
		case 102:
			return POT_RECOVERY;
		case 103:
			return POT_HEROISM;
		case 104:
			return SCR_CREATE_FAMILIAR;
		case 105:
			return SCR_SECURE_IDENTIFY;
		case 106:
			return SCR_INVENTORY_ID;
		case 107:
			return SCR_ARMOR_SPECIALIZATION;
		case 108:
			return SCR_ARTIFACT_CREATION;
		case 109:
			return SCR_GENOCIDE;
		case 110:
			return SCR_ACQUIREMENT;
		case 111:
			return SCR_REVERSE_IDENTIFY;
		case 112:
			return SCR_ALTER_REALITY;
		case 113:
			return SCR_ERASURE;
		case 114:
			return SCR_POWER_HEALING;
		case 115:
			return SCR_SKILL_UP;
		case 116:
			return SCR_CREATE_ALTAR;
		case 117:
			return SCR_WORLD_FALL;
		case 118:
			return SCR_RESURRECTION;
		case 119:
			return SCR_CONSECRATION;
		case 120:
			return SCR_REPAIR_ITEM;
		case 121:
			return SCR_ITEM_GENOCIDE;
		case 122:
			return SPE_TIME;
		case 123:
			return SPE_DISINTEGRATION;
		case 124:
			return SPE_DISINTEGRATION_BEAM;
		case 125:
			return SPE_FIXING;
		case 126:
			return SPE_MAP_LEVEL;
		case 127:
			return SPE_INERTIA;
		case 128:
			return SPE_REMOVE_CURSE;
		case 129:
			return SPE_TIME_STOP;
		case 130:
			return SPE_LEVELPORT;
		case 131:
			return SPE_PARALYSIS;
		case 132:
			return SPE_PETRIFY;
		case 133:
			return SPE_GRAVITY_BEAM;
		case 134:
			return SPE_GOOD_NIGHT;
		case 135:
			return SPE_INFERNO;
		case 136:
			return SPE_ICE_BEAM;
		case 137:
			return SPE_THUNDER;
		case 138:
			return SPE_SLUDGE;
		case 139:
			return SPE_TOXIC;
		case 140:
			return SPE_NETHER_BEAM;
		case 141:
			return SPE_AURORA_BEAM;
		case 142:
			return SPE_CHLOROFORM;
		case 143:
			return SPE_FINGER_OF_DEATH;
		case 144:
			return WAN_GAIN_LEVEL;
		case 145:
			return WAN_TIME_STOP;
		case 146:
			return WAN_TELE_LEVEL;
		case 147:
			return WAN_GENOCIDE;
		case 148:
			return WAN_ENTRAPPING;
		case 149:
			return WAN_PARALYSIS;
		case 150:
			return WAN_DISINTEGRATION;
		case 151:
			return WAN_STONING;
		case 152:
			return WAN_INCREASE_MAX_HITPOINTS;
		case 153:
			return WAN_CREATE_FAMILIAR;
		case 154:
			return WAN_DISINTEGRATION_BEAM;
		case 155:
			return WAN_ACQUIREMENT;
		case 156:
			return WAN_CHARGING;
		case 157:
			return WAN_GRAVITY_BEAM;
		case 158:
			return WAN_GOOD_NIGHT;
		case 159:
			return WAN_INFERNO;
		case 160:
			return WAN_ICE_BEAM;
		case 161:
			return WAN_THUNDER;
		case 162:
			return WAN_SLUDGE;
		case 163:
			return WAN_TOXIC;
		case 164:
			return WAN_NETHER_BEAM;
		case 165:
			return WAN_AURORA_BEAM;
		case 166:
			return WAN_DEATH;
		case 167:
			return CHARGER;
		case 168:
			return MATERIAL_KIT;
		case 169:
			return INTELLIGENCE_PACK;
		case 170:
			return SYMBIOTE;
		case 171:
			return SCR_SUPERIOR_MATERIAL;
		case 172:
			return SCR_GREATER_ENCHANT_WEAPON;
		case 173:
			return SCR_GREATER_ENCHANT_ARMOR;
		case 174:
			return SCR_POWER_CHARGING;
		case 175:
			return BITCHER;
		case 176:
			return POT_TECH_LEVEL_UP;
		case 177:
			return SCR_BRANCH_TELEPORT;
		default: /* fail safe */
			return SCR_IDENTIFY;

	}

	/* appease the compiler --Amy */
	return SCR_IDENTIFY;

}

/*
 * Split obj so that it gets size gets reduced by num. The quantity num is
 * put in the object structure delivered by this call.  The returned object
 * has its wornmask cleared and is positioned just following the original
 * in the nobj chain (and nexthere chain when on the floor).
 */
struct obj *
splitobj(obj, num)
struct obj *obj;
long num;
{
	struct obj *otmp;

	if (obj->cobj || num <= 0L || obj->quan <= num)
	    panic("splitobj");	/* can't split containers */
	otmp = newobj(obj->oxlth + obj->onamelth);
	*otmp = *obj;		/* copies whole structure */
	otmp->o_id = flags.ident++;
	if (!otmp->o_id) otmp->o_id = flags.ident++;	/* ident overflowed */
	otmp->timed = 0;	/* not timed, yet */
	otmp->lamplit = 0;	/* ditto */
	otmp->owornmask = 0L;	/* new object isn't worn */
	obj->quan -= num;
	obj->owt = weight(obj);
	otmp->quan = num;
	otmp->owt = weight(otmp);	/* -= obj->owt ? */
	obj->nobj = otmp;
	/* Only set nexthere when on the floor, nexthere is also used */
	/* as a back pointer to the container object when contained. */
	if (obj->where == OBJ_FLOOR)
	    obj->nexthere = otmp;
	if (obj->oxlth)
	    (void)memcpy((void *)otmp->oextra, (void *)obj->oextra,
			obj->oxlth);
	if (obj->onamelth)
	    (void)strncpy(ONAME(otmp), ONAME(obj), (int)obj->onamelth);
	if (obj->unpaid) splitbill(obj,otmp);
	if (obj->timed) obj_split_timers(obj, otmp);
	if (obj_sheds_light(obj)) obj_split_light_source(obj, otmp);
	return otmp;
}

/*
 * Insert otmp right after obj in whatever chain(s) it is on.  Then extract
 * obj from the chain(s).  This function does a literal swap.  It is up to
 * the caller to provide a valid context for the swap.  When done, obj will
 * still exist, but not on any chain.
 *
 * Note:  Don't use use obj_extract_self() -- we are doing an in-place swap,
 * not actually moving something.
 */
void
replace_object(obj, otmp)
struct obj *obj;
struct obj *otmp;
{
    otmp->where = obj->where;
    switch (obj->where) {
    case OBJ_FREE:
	/* do nothing */
	break;
    case OBJ_INVENT:
	otmp->nobj = obj->nobj;
	obj->nobj = otmp;
	extract_nobj(obj, &invent);
	break;
    case OBJ_CONTAINED:
	otmp->nobj = obj->nobj;
	otmp->ocontainer = obj->ocontainer;
	obj->nobj = otmp;
	extract_nobj(obj, &obj->ocontainer->cobj);
	break;
    case OBJ_MINVENT:
	otmp->nobj = obj->nobj;
	otmp->ocarry =  obj->ocarry;
	obj->nobj = otmp;
	extract_nobj(obj, &obj->ocarry->minvent);
	break;
    case OBJ_FLOOR:
	otmp->nobj = obj->nobj;
	otmp->nexthere = obj->nexthere;
	otmp->ox = obj->ox;
	otmp->oy = obj->oy;
	obj->nobj = otmp;
	obj->nexthere = otmp;
	extract_nobj(obj, &fobj);
	extract_nexthere(obj, &level.objects[obj->ox][obj->oy]);
	break;
    case OBJ_MIGRATING:
	otmp->nobj = obj->nobj;
	obj->nobj = otmp;
	extract_nobj(obj, &migrating_objs);
	break;
    case OBJ_BURIED:
	otmp->nobj = obj->nobj;
	obj->nobj = otmp;
	extract_nobj(obj, &level.buriedobjlist);
	break;
    case OBJ_ONBILL:
	otmp->nobj = obj->nobj;
	obj->nobj = otmp;
	extract_nobj(obj, &billobjs);
	break;
    default:
	panic("replace_object: obj position");
	break;
    }
}

/*
 * Create a dummy duplicate to put on shop bill.  The duplicate exists
 * only in the billobjs chain.  This function is used when a shop object
 * is being altered, and a copy of the original is needed for billing
 * purposes.  For example, when eating, where an interruption will yield
 * an object which is different from what it started out as; the "I x"
 * command needs to display the original object.
 *
 * The caller is responsible for checking otmp->unpaid and
 * costly_spot(u.ux, u.uy).  This function will make otmp no charge.
 *
 * Note that check_unpaid_usage() should be used instead for partial
 * usage of an object.
 */
void
bill_dummy_object(otmp)
register struct obj *otmp;
{
	register struct obj *dummy, *obj;

	if (otmp->unpaid)
	    subfrombill(otmp, shop_keeper(*u.ushops));
	dummy = newobj(otmp->oxlth + otmp->onamelth);
	*dummy = *otmp;
	if (Has_contents(otmp)) {
	    for(obj = otmp->cobj; obj; obj = obj->nobj)
		bill_dummy_object(obj);
	    dummy->cobj = NULL;
	}
	dummy->where = OBJ_FREE;
	dummy->o_id = flags.ident++;
	if (!dummy->o_id) dummy->o_id = flags.ident++;	/* ident overflowed */
	dummy->timed = 0;
	if (otmp->oxlth)
	    (void)memcpy((void *)dummy->oextra,
			(void *)otmp->oextra, otmp->oxlth);
	if (otmp->onamelth)
	    (void)strncpy(ONAME(dummy), ONAME(otmp), (int)otmp->onamelth);
	if (Is_candle(dummy)) dummy->lamplit = 0;
	addtobill(dummy, FALSE, TRUE, TRUE);
	if (otmp->where != OBJ_INVENT)
	    otmp->no_charge = 1;
	otmp->unpaid = 0;
	return;
}

#endif /* OVL1 */
#ifdef OVLB

static const char dknowns[] = {
		WAND_CLASS, RING_CLASS, POTION_CLASS, SCROLL_CLASS, AMULET_CLASS, IMPLANT_CLASS,
		GEM_CLASS, SPBOOK_CLASS, WEAPON_CLASS, TOOL_CLASS, 0
};

struct obj *
mksobj(otyp, init, artif, shopinit)
int otyp;
boolean init;
int artif;
boolean shopinit;
{
	int mndx, tryct;
	struct obj *otmp;

	if (otyp >= NUM_OBJECTS) {
		impossible("Error: mksobj() called with invalid object type %d! Debug info for Amy: %d, %d, %d", otyp, (int) init, artif, (int) shopinit);
		otyp = GOLD_PIECE;
	}

	if (objects[otyp].oc_class >= MAXOCLASSES) {
		impossible("Error: mksobj() called with invalid object class! Debug info for Amy: %d, %d, %d, %d", otyp, (int) init, artif, (int) shopinit);
		otyp = GOLD_PIECE;
	}

	if (otyp == u.unobtainable && !(u.riderhack && otyp == CORPSE) && !(issokosolver && otyp == BOULDER)) {
		otyp = GOLD_PIECE;
	}

	if (otyp == u.unobtainable2 && !(u.riderhack && otyp == CORPSE) && !(issokosolver && otyp == BOULDER)) {
		otyp = GOLD_PIECE;
	}

	if (otyp == u.unobtainable3 && !(u.riderhack && otyp == CORPSE) && !(issokosolver && otyp == BOULDER)) {
		otyp = GOLD_PIECE;
	}

	if (otyp == u.unobtainable4 && !(u.riderhack && otyp == CORPSE) && !(issokosolver && otyp == BOULDER)) {
		otyp = GOLD_PIECE;
	}

	if (otyp == u.unobtainable5 && !(u.riderhack && otyp == CORPSE) && !(issokosolver && otyp == BOULDER)) {
		otyp = GOLD_PIECE;
	}

	if (otyp == u.unobtainable6 && !(u.riderhack && otyp == CORPSE) && !(issokosolver && otyp == BOULDER)) {
		otyp = GOLD_PIECE;
	}

	if (otyp == u.unobtainable7 && !(u.riderhack && otyp == CORPSE) && !(issokosolver && otyp == BOULDER)) {
		otyp = GOLD_PIECE;
	}

	if (otyp == u.unobtainable8 && !(u.riderhack && otyp == CORPSE) && !(issokosolver && otyp == BOULDER)) {
		otyp = GOLD_PIECE;
	}

	if (otyp == u.unobtainable9 && !(u.riderhack && otyp == CORPSE) && !(issokosolver && otyp == BOULDER)) {
		otyp = GOLD_PIECE;
	}

	if (otyp == u.unobtainable10 && !(u.riderhack && otyp == CORPSE) && !(issokosolver && otyp == BOULDER)) {
		otyp = GOLD_PIECE;
	}

	if (otyp == u.unobtainablegeno && !(u.riderhack && otyp == CORPSE)) { /* your fault for genociding them. No sokosolver specialties! --Amy */
		otyp = GOLD_PIECE;
	}

	if (u.minimalism && isminimalist && rn2(u.minimalism) && !(issokosolver && otyp == BOULDER)) {
		if (otyp != GOLD_PIECE && otyp != STRANGE_OBJECT && otyp != AMULET_OF_YENDOR && otyp != CANDELABRUM_OF_INVOCATION
#ifdef MAIL
	&& otyp != SCR_MAIL
#endif
 && otyp != BELL_OF_OPENING && otyp != SPE_BOOK_OF_THE_DEAD)
		otyp = GOLD_PIECE;
	}

	char let = objects[otyp].oc_class;
	if (objects[otyp].oc_class >= MAXOCLASSES) {
		impossible("Error! mkobj oc_class invalid. Debug info for Amy: %d, %d, %d, %d, let %c", otyp, (int) init, artif, (int) shopinit, let);
	}

	otmp = newobj(0);
	*otmp = zeroobj;
	otmp->age = monstermoves;
	otmp->o_id = flags.ident++;
	if (!otmp->o_id) otmp->o_id = flags.ident++;	/* ident overflowed */
	otmp->quan = 1L;
	otmp->oclass = let;
	otmp->otyp = otyp;
	otmp->where = OBJ_FREE;
	otmp->dknown = index(dknowns, let) ? 0 : 1;
 	otmp->selfmade = FALSE;
	otmp->oinvis = 0;
	otmp->oinvisreal = 0;
	otmp->petmarked = 0;
	otmp->obrittle = 0;
	otmp->obrittle2 = 0;
	otmp->olocked = FALSE; /* ->recharged */
	otmp->altmode = WP_MODE_AUTO;
	otmp->fakeartifact = 0;
	otmp->enchantment = 0;
	otmp->finalcancel = 0;
	otmp->nemtrident = 0;
	otmp->objwassold = (shopinit ? 1 : 0);
	otmp->icedobject = 0;

	/* each aspect that can be identified has a 4 in 5 chance of being possible to reveal by using the ID spell --Amy */
	otmp->ident_bst = rn2(5);
	otmp->ident_knw = rn2(5);
	otmp->ident_dkn = rn2(5);
	otmp->ident_bkn = rn2(5);
	otmp->ident_rkn = rn2(5);

	otmp->stackmarked = FALSE;
	otmp->shirtmessage = rnd(1000000);
	/* for use in read.c (shirts only) - since items can be polymorphed, set it for everything, including non-shirts */

	if ((otmp->otyp >= ELVEN_SHIELD && otmp->otyp <= ORCISH_SHIELD) ||
			otmp->otyp == SHIELD_OF_REFLECTION)
		otmp->dknown = 0;
	if (!objects[otmp->otyp].oc_uses_known)
		otmp->known = 1;
	otmp->oinvis = !always_visible(otmp) && \
		/*(otmp->otyp != BOULDER || !In_sokoban(&u.uz)) && */ !rn2(1250);
	if (otmp->otyp == BOULDER && In_sokoban(&u.uz) && !((moves + u.monstertimefinish) % 1317) ) otmp->oinvis = 1;

	/* 1 in 10000 random boulders are Wendyhole; use onameX to make sure it can generate several times --Amy */
	if (otmp->otyp == BOULDER && !rn2(10000) && !otmp->oartifact) {
		otmp = onameX(otmp, artiname(ART_WENDYHOLE));
	}

	if (otmp->oinvis && !rn2(!(u.monstertimefinish % 13333) ? 3 : !(u.monstertimefinish % 1333) ? 10 : !(u.monstertimefinish % 133) ? 30 : 100) ) otmp->oinvisreal = 1;

	if (init && (let >= MAXOCLASSES)) {
		impossible("Error: mksobj() let value is invalid! Debug info for Amy: %d, %d, %d, %d, let %c", otyp, (int) init, artif, (int) shopinit, let);
	}

	if (init) switch (let) {
/* -----------============STEPHEN WHITE'S NEW CODE============----------- */                   
	case WEAPON_CLASS:
		/* KMH, balance patch -- new macros */
		otmp->quan = is_multigen(otmp) ? (long) rn1(12,12) : 1L;
		if (otmp->otyp == BULLET) otmp->quan += rnd(50);
		if (otmp->otyp == LEAD_BULLET) otmp->quan += rnd(50);
		if (otmp->otyp == SILVER_BULLET) otmp->quan += rnd(75);
		if (otmp->otyp == ANTIMATTER_BULLET) otmp->quan += rnd(75);
		if (otmp->otyp == BLASTER_BOLT) otmp->quan += rnd(30);
		if (otmp->otyp == HEAVY_BLASTER_BOLT) otmp->quan += rnd(40);
		if (otmp->otyp == LASER_BEAM) otmp->quan += rnd(50);
		if (otmp->otyp == SHOTGUN_SHELL) otmp->quan += rnd(20);
		if (otmp->otyp == LEAD_SHOT) otmp->quan += rnd(20);
		if (otmp->otyp == ROCKET) otmp->quan += rnd(5);
		if (otmp->otyp == CROSSBOW_BOLT) otmp->quan += rnd(25);
		if (otmp->otyp == DROVEN_BOLT) otmp->quan += rnd(75);
		if (otmp->otyp == DROVEN_ARROW) otmp->quan += rnd(75);
		if (otmp->otyp == INFERIOR_BOLT) otmp->quan += rnd(55);
		if (otmp->otyp == KOKKEN || otmp->otyp == MINERAL_BOLT || otmp->otyp == POISON_BOLT || otmp->otyp == HEAVY_CROSSBOW_BOLT || otmp->otyp == ETHER_BOLT || otmp->otyp == MATERIAL_BOLT || otmp->otyp == FLEECE_BOLT || otmp->otyp == PIN_BOLT) otmp->quan += rnd(50);
		if (otmp->otyp == PAPER_ARROW || otmp->otyp == FORBIDDEN_ARROW || otmp->otyp == GOLDEN_ARROW || otmp->otyp == ANCIENT_ARROW || otmp->otyp == BRONZE_ARROW) otmp->quan += rnd(15);
		if (otmp->otyp == YA) otmp->quan += rnd(20);
		if (otmp->otyp == FLIMSY_DART) otmp->quan += rnd(30);
		if (otmp->otyp == SAND_DART) otmp->quan += rnd(50);
		if (otmp->otyp == INKA_STINGER || otmp->otyp == SPIKE) otmp->quan += rnd(30);
		if (otmp->otyp == SHURIKEN) otmp->quan += rnd(100);
		if (otmp->otyp == MYSTERY_SHURIKEN) otmp->quan += rnd(100);
		if (otmp->otyp == TAR_STAR) otmp->quan += rnd(100);
		if (otmp->otyp == NANO_SHURIKEN) otmp->quan += rnd(100);
		if (otmp->otyp == CUBIC_STAR) otmp->quan += rnd(100);
		if (otmp->otyp == WINDMILL_BLADE) otmp->quan += rnd(100);
		if (otmp->otyp == JUMPING_FLAMER) otmp->quan += rnd(100);
		if (otmp->otyp == NEEDLE) otmp->quan += rnd(100);
		if (otmp->otyp == SOFT_STAR) otmp->quan += rnd(200);
		if (otmp->otyp == CALTROP) otmp->quan += rnd(400);
		if (otmp->otyp == BFG_AMMO) {
			otmp->quan = 40;
			if (!rn2(5)) otmp->quan *= (1 + rnd(4));
		}
		if (is_lower_multigen(otmp) && !is_multigen(otmp) && !issoviet && !rn2(50) ) otmp->quan += rn2(6);
		if (is_lower_multigen(otmp) && !is_multigen(otmp) && !issoviet && !rn2(10) ) otmp->quan += rn2(2);

		/* occasionally make bigger stacks of stackable weapons --Amy */
		if (!issoviet && !rn2(is_multigen(otmp) ? 20 : is_lower_multigen(otmp) ? 100 : 1000) ) {
			otmp->quan += rnd(otmp->quan);
			if (!rn2(50)) otmp->quan += rnz(otmp->quan + 3);
			if (!rn2(50)) otmp->quan += rnz( rnd( (otmp->quan * 2) + 3) );
		}

		if (Race_if(PM_JAVA) && (otmp->otyp == JAVELIN || otmp->otyp == ASBESTOS_JAVELIN || otmp->otyp == STACK_JAVELIN || otmp->otyp == SPIRIT_THROWER || otmp->otyp == COURSE_JAVELIN || otmp->otyp == BRICK_MISSILE || otmp->otyp == TORPEDO || otmp->otyp == HOMING_TORPEDO)) {
			otmp->quan += rnd(5);
		}
		if (Role_if(PM_TOSSER) && (otmp->otyp == JAVELIN || otmp->otyp == ASBESTOS_JAVELIN || otmp->otyp == STACK_JAVELIN || otmp->otyp == SPIRIT_THROWER || otmp->otyp == COURSE_JAVELIN || otmp->otyp == BRICK_MISSILE || otmp->otyp == TORPEDO || otmp->otyp == HOMING_TORPEDO)) {
			otmp->quan += rnd(5);
		}

		if (uarmc && uarmc->oartifact == ART_ARABELLA_S_WEAPON_STORAGE && (is_ammo(otmp) || is_missile(otmp) || is_grenade(otmp)) ) otmp->quan *= 2;

		/* In Soviet Russia, it's completely unthinkable to find a stack of more than the standard rn1(12,12) arrows.
		 * Or darts or whatever other ranged weapon it is. Because it's not allowed to luck into a huge stack of
		 * 100 +8 arrows or something. The standard amount must be sufficient for everyone in communism! --Amy */

		if(!rn2(ishaxor ? 3 : 6)) {
			otmp->spe = rne(Race_if(PM_LISTENER) ? 3 : 2);
			if (rn2(2)) otmp->blessed = rn2(2);
			 else	blessorcurse_on_creation(otmp, 3);
		} else if(!rn2(ishaxor ? 4 : 8)) {
			if (rn2(10)) curse_on_creation(otmp);
			 else	blessorcurse_on_creation(otmp, 3);
			otmp->spe = -rne(Race_if(PM_LISTENER) ? 3 : 2);
		} else	blessorcurse_on_creation(otmp, 10);

		/* lategame (Gehennom and beyond) should spawn highly enchanted gear slightly more often --Amy
		 * idea from Nethack Fourk but implementation is my own */
		if (otmp->spe && !rn2(6) && In_lategame(&u.uz)) {
			if (otmp->spe > 0) otmp->spe += rne(Race_if(PM_LISTENER) ? 3 : 2);
			else otmp->spe -= rne(Race_if(PM_LISTENER) ? 3 : 2);
		}

		if ((Race_if(PM_LISTENER) || RngeListening) && !Hallucination && (rnd(30) > ACURR(A_INT)) && (abs(otmp->spe) > 3 || (abs(otmp->spe) == 3 && rn2(2) ) || (abs(otmp->spe) == 2 && !rn2(3) )|| (abs(otmp->spe) == 1 && !rn2(5) ) ) ) pline("Precognition: made object with enchantment %d", abs(otmp->spe));

		if (is_poisonable(otmp) && !rn2(100)) {
			otmp->opoisoned = 1;
			if (!rn2(100)) otmp->superpoison = 1;
		}
		if (!rn2(400)) otmp->oerodeproof = 1;
		if (!rn2(400)) {
			if (!rn2(3)) otmp->oeroded = rnd(3);
			if (!rn2(3)) otmp->oeroded2 = rnd(3);
		}
		if (!rn2(200)) otmp->greased = rn2(5) ? 1 : rn2(3) ? 2 : 3;

		if (artif && (artif != 2) && !rn2(Race_if(PM_LISTENER) ? 40 : 20)) {
		    otmp = mk_artifact(otmp, (aligntyp)A_NONE, TRUE);
			if ((Race_if(PM_LISTENER) || RngeListening) && !Hallucination && (rnd(30) > ACURR(A_INT))) pline("Precognition: made artifact");
		}
		else if ((artif || otmp->spe) && !rn2((abs(otmp->spe) > 9) ? 2 : (abs(otmp->spe) > 7) ? 3 : (abs(otmp->spe) > 5) ? 4 : (abs(otmp->spe) > 4) ? 5 : (abs(otmp->spe) > 3) ? 6 : (abs(otmp->spe) > 2) ? 7 : (abs(otmp->spe) > 1) ? 8 : (abs(otmp->spe) > 0) ? 10 : 50)) {
		    otmp = oname(otmp, !rn2(20) ? generate_garbage_string() : fauxartinames[rn2(SIZE(fauxartinames))] );
			otmp->fakeartifact = 1;
			if (artif != 2) u.fakeartifacts++;
			}
		if (otmp->otyp == STICK_OF_DYNAMITE) {
			otmp->age = (otmp->cursed ? rn2(15) + 2 : 
					(otmp->blessed ? 15 : rn2(10) + 10));
		}
		break;
	case FOOD_CLASS:
	    otmp->odrained = 0;
	    otmp->oeaten = 0;
	    switch(otmp->otyp) {
	    case CORPSE:
		/* possibly overridden by mkcorpstat() */
		tryct = 50;
		do otmp->corpsenm = undead_to_corpse(rndmonnum());
		while ((mvitals[otmp->corpsenm].mvflags & G_NOCORPSE) && (--tryct > 0));
		if (tryct == 0) {
		/* perhaps rndmonnum() only wants to make G_NOCORPSE monsters on
		   this level; let's create an adventurer's corpse instead, then */
			otmp->corpsenm = PM_HUMAN;
		}
		/* timer set below */
		blessorcurse_on_creation(otmp, 8);
		break;
	    case EGG:
		otmp->corpsenm = NON_PM;	/* generic egg */
		if (rn2(3)) for (tryct = 500; tryct > 0; --tryct) { /* Uncommon egg types may be possible. --Amy */
		    mndx = (rn2(100) ? can_be_hatched(rndmonnum()) : rndmonnum() );
		    if (mndx != NON_PM && !dead_species(mndx, TRUE)) {
			otmp->corpsenm = mndx;		/* typed egg */
			attach_egg_hatch_timeout(otmp);
			break;
		    }
		}
		if (!rn2(200)) otmp->spe = 2;
		blessorcurse_on_creation(otmp, 8);
		break;
	    case TIN:
		otmp->corpsenm = NON_PM;	/* empty (so far) */
		if (!rn2(10)) /* more rare now --Amy */
		    otmp->spe = rnd(4);		/* spinach, beans, peaches or fish */
		else for (tryct = 200; tryct > 0; --tryct) {
		    mndx = undead_to_corpse(rndmonnum());
		    if (mons[mndx].cnutrit &&
			    !(mvitals[mndx].mvflags & G_NOCORPSE)) {
			otmp->corpsenm = mndx;
			break;
		    }
		}
		blessorcurse_on_creation(otmp, 10);
		break;
	    case SLIME_MOLD:
		otmp->spe = current_fruit;
		otmp->quan = (long) rnd(15);
		blessorcurse_on_creation(otmp, 10);
		break;
	    case APPLE:
	    case CARROT:
	    case PEAR:
	    case CHERRY:
	    case ASIAN_PEAR:
	    case BANANA:
	    case LEMON:
	    case ORANGE:
	    case MELON:
	    case KELP_FROND:
		otmp->quan = (long) rnd(5);
		blessorcurse_on_creation(otmp, 10);
		break;
	    }
		blessorcurse_on_creation(otmp, 10);
		if (!rn2(4000)) otmp->oerodeproof = 1;
		if (!rn2(4000)) {
			if (!rn2(3)) otmp->oeroded = rnd(3);
			if (!rn2(3)) otmp->oeroded2 = rnd(3);
		}
		if (!rn2(2000)) otmp->greased = rn2(5) ? 1 : rn2(3) ? 2 : 3;
		if (artif && (artif != 2) && !rn2(Race_if(PM_LISTENER) ? 400 : 200)) {
		    otmp = mk_artifact(otmp, (aligntyp)A_NONE, TRUE);
			if ((Race_if(PM_LISTENER) || RngeListening) && !Hallucination && (rnd(30) > ACURR(A_INT))) pline("Precognition: made artifact");
		}
		else if (artif && !rn2(500)) {
		    otmp = oname(otmp, !rn2(20) ? generate_garbage_string() : fauxartinames[rn2(SIZE(fauxartinames))] );
			otmp->fakeartifact = 1;
			if (artif != 2) u.fakeartifacts++;
		}
		 break;
	    /* no longer fall into next case --Amy */

/* -----------============STEPHEN WHITE'S NEW CODE============----------- */
	case GEM_CLASS:

		if (otmp->otyp == WONDER_STONE) otmp->oinvis = 1;

		/* KMH, balance patch -- healthstone replaces rotting/health */
		if (otmp->otyp == LOADSTONE || otmp->otyp == HEALTHSTONE || otmp->otyp == MANASTONE || otmp->otyp == SLEEPSTONE || otmp->otyp == LOADBOULDER || otmp->otyp == STARLIGHTSTONE) {
			curse_on_creation(otmp);
			if (!rn2(6)) otmp->quan = rn2(5) ? 2L : rn2(4) ? rn1(5,5) : rn1(10,10);
			else otmp->quan = 1L;

			if (!rn2(1000)) otmp->oerodeproof = 1;
			if (!rn2(1000)) {
				if (!rn2(3)) otmp->oeroded = rnd(3);
				if (!rn2(3)) otmp->oeroded2 = rnd(3);
			}
			if (!rn2(500)) otmp->greased = rn2(5) ? 1 : rn2(3) ? 2 : 3;

			if (artif && (artif != 2) && !rn2(Race_if(PM_LISTENER) ? 100 : 50)) {
			    otmp = mk_artifact(otmp, (aligntyp)A_NONE, TRUE);
				if ((Race_if(PM_LISTENER) || RngeListening) && !Hallucination && (rnd(30) > ACURR(A_INT))) pline("Precognition: made artifact");
			}
			else if ((artif || otmp->spe) && !rn2((abs(otmp->spe) > 9) ? 2 : (abs(otmp->spe) > 7) ? 3 : (abs(otmp->spe) > 5) ? 4 : (abs(otmp->spe) > 4) ? 5 : (abs(otmp->spe) > 3) ? 6 : (abs(otmp->spe) > 2) ? 7 : (abs(otmp->spe) > 1) ? 8 : (abs(otmp->spe) > 0) ? 10 : 150)) {
			    otmp = oname(otmp, !rn2(20) ? generate_garbage_string() : fauxartinames[rn2(SIZE(fauxartinames))] );
				otmp->fakeartifact = 1;
				if (artif != 2) u.fakeartifacts++;
			}

			break;
		}
		else if (otmp->otyp == ROCK) otmp->quan = (long) rn1(6,6);
	/* Finding single flint stones is just useless. Let sling users have some fun! --Amy */
		else if (otmp->otyp == FLINT && rn2(2) ) {
			otmp->quan = (long) rn1(5,5);
			if (!rn2(10)) otmp->quan *= 2;
		}
		else if (otmp->otyp == SMALL_PIECE_OF_UNREFINED_MITHR && rn2(2) ) {
			otmp->quan = (long) rn1(6,6);
			if (!rn2(10)) otmp->quan *= 2;
		}
		else if (otmp->otyp == SLING_AMMO && rn2(2) ) {
			otmp->quan = (long) rn1(7,7);
			if (!rn2(10)) otmp->quan *= 2;
		}
		else if (otmp->otyp == LEAD_CLUMP && rn2(2) ) {
			otmp->quan = (long) rn1(6,6);
			if (!rn2(10)) otmp->quan *= 2;
		}
		else if (otmp->otyp == BONE_FRAGMENT && rn2(2) ) {
			otmp->quan = (long) rn1(8,8);
			if (!rn2(10)) otmp->quan *= 2;
		}
		else if (otmp->otyp == SILVER_SLINGSTONE && rn2(2) ) {
			otmp->quan = (long) rn1(10,10);
			if (!rn2(10)) otmp->quan *= 2;
		}
		else if (otmp->otyp == CONUNDRUM_NUGGET && rn2(2) ) {
			otmp->quan = (long) rn1(9,9);
			if (!rn2(10)) otmp->quan *= 2;
		}

	/* it's too easy to tell flint etc. apart from dangerous gray stones, so let's allow the latter to spawn
	 * in bigger stacks occasionally just to troll players who expect stacks to always be flint. --Amy */
		else if (!rn2(6)) otmp->quan = rn2(5) ? 2L : rn2(4) ? rn1(5,5) : rn1(10,10);
		else otmp->quan = 1L;

		/* random chance of getting a bigger stack --Amy */
		if (!issoviet && !rn2(50) ) otmp->quan += rn2(6);
		if (!issoviet && !rn2(10) ) otmp->quan += rn2(2);
		if (!issoviet && !rn2(20) ) {
			otmp->quan += rnd(otmp->quan);
			if (!rn2(50)) otmp->quan += rnz(otmp->quan + 3);
			if (!rn2(50)) otmp->quan += rnz( rnd( (otmp->quan * 2) + 3) );
		}

		if(!rn2(ishaxor ? 4 : 8)) {
			otmp->spe = rne(Race_if(PM_LISTENER) ? 3 : 2);
			if (rn2(2)) otmp->blessed = rn2(2);
			 else	blessorcurse_on_creation(otmp, 3);
		} else if(!rn2(ishaxor ? 5 : 10)) {
			if (rn2(10)) curse_on_creation(otmp);
			 else	blessorcurse_on_creation(otmp, 3);
			otmp->spe = -rne(Race_if(PM_LISTENER) ? 3 : 2);
		} else	blessorcurse_on_creation(otmp, 10);

		if (otmp->spe && !rn2(6) && In_lategame(&u.uz)) {
			if (otmp->spe > 0) otmp->spe += rne(Race_if(PM_LISTENER) ? 3 : 2);
			else otmp->spe -= rne(Race_if(PM_LISTENER) ? 3 : 2);
		}

		if (!rn2(1000)) otmp->oerodeproof = 1;
		if (!rn2(1000)) {
			if (!rn2(3)) otmp->oeroded = rnd(3);
			if (!rn2(3)) otmp->oeroded2 = rnd(3);
		}
		if (!rn2(500)) otmp->greased = rn2(5) ? 1 : rn2(3) ? 2 : 3;

		if (artif && (artif != 2) && !rn2(Race_if(PM_LISTENER) ? 100 : 50)) {
		    otmp = mk_artifact(otmp, (aligntyp)A_NONE, TRUE);
			if ((Race_if(PM_LISTENER) || RngeListening) && !Hallucination && (rnd(30) > ACURR(A_INT))) pline("Precognition: made artifact");
		}
		else if ((artif || otmp->spe) && !rn2((abs(otmp->spe) > 9) ? 2 : (abs(otmp->spe) > 7) ? 3 : (abs(otmp->spe) > 5) ? 4 : (abs(otmp->spe) > 4) ? 5 : (abs(otmp->spe) > 3) ? 6 : (abs(otmp->spe) > 2) ? 7 : (abs(otmp->spe) > 1) ? 8 : (abs(otmp->spe) > 0) ? 10 : 150)) {
		    otmp = oname(otmp, !rn2(20) ? generate_garbage_string() : fauxartinames[rn2(SIZE(fauxartinames))] );
			otmp->fakeartifact = 1;
			if (artif != 2) u.fakeartifacts++;
		}

	/* "Disable enchantable rocks. The racial + stregnth + sling enchantment bonus is more than enough." In Soviet Russia, people never use slings anyway so they never noticed that those retarded things never do enough damage. They also completely disregard the fact that higher enchantment means a lower chance for the projectiles to disappear too. Guess they never used a stack of blessed +7 rocks in their life, otherwise they'd know the benefits of having them. --Amy */
		if (issoviet) otmp->spe = 0;

		if ((Race_if(PM_LISTENER) || RngeListening) && !Hallucination && (rnd(30) > ACURR(A_INT)) && (abs(otmp->spe) > 3 || (abs(otmp->spe) == 3 && rn2(2) ) || (abs(otmp->spe) == 2 && !rn2(3) )|| (abs(otmp->spe) == 1 && !rn2(5) ) ) ) pline("Precognition: made object with enchantment %d", abs(otmp->spe));

		break;
/* -----------============STEPHEN WHITE'S NEW CODE============----------- */
	case TOOL_CLASS:
	    switch(otmp->otyp) {
		case TALLOW_CANDLE:
		case WAX_CANDLE:
		case JAPAN_WAX_CANDLE:
		case OIL_CANDLE:
		case UNSPECIFIED_CANDLE:
		case SPECIFIC_CANDLE:
		case __CANDLE:
		case NATURAL_CANDLE:
		case UNAFFECTED_CANDLE:
		case GENERAL_CANDLE:
			otmp->spe = 1;
					otmp->age = 2000L; /* used to depend on cost but that's BS --Amy */

					if (!rn2(20)) {
						otmp->age += (rnd(10) * 500);
					} else if (!rn2(20)) {
						otmp->age -= (rnd(9) * 200);
					}

					if (ishaxor) otmp->age *= 2;
					otmp->lamplit = 0;
					otmp->quan = 1L +
					      (long)(rn2(2) ? rn2(7) : 0);
					blessorcurse_on_creation(otmp, 5);
					break;
	        case TORCH:	        otmp->spe = 0;
					otmp->age = (long) rn1(300,600);

					if (!rn2(20)) {
						otmp->age += rnz(1000);
					} else if (!rn2(20)) {
						otmp->age = rnz(600);
					}

					if (ishaxor) otmp->age *= 2;
					otmp->lamplit = 0;
					otmp->quan = rnd(3);
					blessorcurse_on_creation(otmp, 5);
					break;
		case BRASS_LANTERN:
		case OIL_LAMP:
			otmp->recharged = 0;
			if(!rn2(5)) otmp->recharged = rnd(7);
			otmp->spe = 1;
					otmp->age = (long) rn1(500,1000);
					if (!rn2(20)) {
						otmp->age += rnz(2000);
					} else if (!rn2(20)) {
						otmp->age = rnz(1000);
					}
					if (ishaxor) otmp->age *= 2;
					otmp->lamplit = 0;
					blessorcurse_on_creation(otmp, 5);
					break;
		case MAGIC_CANDLE:                
		case MAGIC_LAMP:
			otmp->spe = 1;
					otmp->lamplit = 0;
					blessorcurse_on_creation(otmp, 2);
					break;
		case RED_DOUBLE_LIGHTSABER:
		case LASERDENT:
		case LASERXBOW:
		case SITH_STAFF:
		case LASER_POLE:
		case WHITE_DOUBLE_LIGHTSABER:
					otmp->altmode = FALSE;
		case STARWARS_MACE:
		case LASER_SWORD:
		case BEAMSWORD:
		case GREEN_LIGHTSABER:
		case ORANGE_LIGHTSABER:
		case BLACK_LIGHTSABER:
		case BLUE_LIGHTSABER:
		case MYSTERY_LIGHTSABER:
		case VIOLET_LIGHTSABER:
		case WHITE_LIGHTSABER:
		case YELLOW_LIGHTSABER:
		case RED_LIGHTSABER:
		case LASER_SWATTER:
		case NANO_HAMMER:
		case LIGHTWHIP:
		case ELECTRIC_CIGARETTE:
			otmp->recharged = 0;
			if(!rn2(5)) otmp->recharged = rnd(7);
			otmp->lamplit = 0;
			otmp->age = (long) rn1(500,1000);
					if (!rn2(20)) {
						otmp->age += rnz(1000);
					} else if (!rn2(20)) {
						otmp->age = rnz(500);
					}

			if (otmp->otyp == BLACK_LIGHTSABER) otmp->age += rnz(5000);

			if (ishaxor) otmp->age *= 2;

			if(!rn2(ishaxor ? 3 : 6)) {
				otmp->spe = rne(Race_if(PM_LISTENER) ? 3 : 2);
				if (rn2(2)) otmp->blessed = rn2(2);
				 else	blessorcurse_on_creation(otmp, 3);
			} else if(!rn2(ishaxor ? 4 : 8)) {
				if (rn2(10)) curse_on_creation(otmp);
				 else	blessorcurse_on_creation(otmp, 3);
				otmp->spe = -rne(Race_if(PM_LISTENER) ? 3 : 2);
			} else	blessorcurse_on_creation(otmp, 10);

			if (otmp->spe && !rn2(6) && In_lategame(&u.uz)) {
				if (otmp->spe > 0) otmp->spe += rne(Race_if(PM_LISTENER) ? 3 : 2);
				else otmp->spe -= rne(Race_if(PM_LISTENER) ? 3 : 2);
			}

			if ((Race_if(PM_LISTENER) || RngeListening) && !Hallucination && (rnd(30) > ACURR(A_INT)) && (abs(otmp->spe) > 3 || (abs(otmp->spe) == 3 && rn2(2) ) || (abs(otmp->spe) == 2 && !rn2(3) )|| (abs(otmp->spe) == 1 && !rn2(5) ) ) ) pline("Precognition: made object with enchantment %d", abs(otmp->spe));

		break;
		case TREASURE_CHEST:
			otmp->olocked = TRUE;
			otmp->otrapped = rn2(5);
			if (artif != 2) mkbox_cnts(otmp);
			blessorcurse_on_creation(otmp, 8);
		break;

		case CHEST:
		case NANO_CHEST:
		case CHEST_OF_HOLDING:
		case LARGE_BOX:
		case LEAD_BOX:
			otmp->olocked = !!(rn2(5));
					otmp->otrapped = !(rn2(10));
		case ICE_BOX:
		case DISPERSION_BOX:
		case ICE_BOX_OF_HOLDING:
		case ICE_BOX_OF_WATERPROOFING:
		case SACK:
		case OILSKIN_SACK:
		case POTATO_BAG:
		case HANDYBAG:
		case BAG_OF_HOLDING:
		case MEDICAL_KIT:
			if (artif != 2) mkbox_cnts(otmp);
		blessorcurse_on_creation(otmp, 8);
					break;
		case EXPENSIVE_CAMERA:
		case FELT_TIP_MARKER:
			otmp->recharged = 0;
			if(!rn2(5)) otmp->recharged = rnd(7);
			otmp->spe = rnd(ishaxor ? 120 : 100);
			otmp->spe -= rn2(3);
			blessorcurse_on_creation(otmp, 5);
					break;
		case TINNING_KIT:
		case BINNING_KIT:
			otmp->recharged = 0;
			if(!rn2(5)) otmp->recharged = rnd(7);
			otmp->spe = rnd(ishaxor ? 120 : 100);
			if (rn2(10)) otmp->spe = rnd(ishaxor ? 100 : 50);
			if (!rn2(3)) otmp->spe = rnd(ishaxor ? 60 : 30);
			if (!rn2(5)) otmp->spe = rnd(ishaxor ? 40 : 20);
			otmp->spe -= rn2(3);
			blessorcurse_on_creation(otmp, 5);
					break;
			break;
		case MAGIC_MARKER:
			otmp->recharged = 0;
			if(!rn2(2)) otmp->recharged = 1;
			otmp->spe = rnd(ishaxor ? 120 : 100);
			otmp->spe -= rn2(3);
			blessorcurse_on_creation(otmp, 5);
					break;
		case CAN_OF_GREASE:
		case LUBRICANT_CAN:
			otmp->recharged = 0;
			if(!rn2(5)) otmp->recharged = rnd(7);
			otmp->spe = rnd(ishaxor ? 70 : 35);
			otmp->spe -= rn2(3);
			blessorcurse_on_creation(otmp, 10);
					break;
		/* KMH, balance patch -- removed to prevent abuse
		case ORB_OF_DESTRUCTION:blessorcurse_on_creation(otmp, 2);
					break;
		case ORB_OF_CHARGING:   otmp->spe = rnd(10) + 5;
					blessorcurse_on_creation(otmp, 2);
					break;
		case ORB_OF_ENCHANTMENT:otmp->spe = rnd(3) + 1;
					blessorcurse_on_creation(otmp, 2);
					break;*/
		case CRYSTAL_BALL:
			otmp->recharged = 0;
			if(!rn2(5)) otmp->recharged = rnd(7);
			otmp->spe = rn1(10,3);
			otmp->spe -= rn2(3);
			blessorcurse_on_creation(otmp, 2);
					break;
		case HORN_OF_PLENTY:
		case BAG_OF_TRICKS:
			otmp->recharged = 0;
			if(!rn2(5)) otmp->recharged = rnd(7);
			otmp->spe = rnd(30);
			otmp->spe -= rn2(3);
			blessorcurse_on_creation(otmp, 5);
					break;
 		case CHEMISTRY_SET:
			otmp->recharged = 0;
			if(!rn2(5)) otmp->recharged = rnd(7);
			otmp->spe = rnd(ishaxor ? 80 : 40);
			otmp->spe -= rn2(3);
			blessorcurse_on_creation(otmp,4);
 					break;
		case FIGURINE:	{	int tryct2 = 0;
					do
					    otmp->corpsenm = rndmonnum();
					while(is_human(&mons[otmp->corpsenm]) && !rn2(5)
						&& tryct2++ < 30); /* come on, human figurines should be possible! --Amy */
					blessorcurse_on_creation(otmp, 4);
					break;
				}
		case ENERGY_SAP:
			{
				int tryct2 = 0;
				do otmp->corpsenm = rndmonnum();
				while(is_human(&mons[otmp->corpsenm]) && !rn2(5) && tryct2++ < 30);
				blessorcurse_on_creation(otmp, 4);
				break;
			}
		case BELL_OF_OPENING:
			otmp->spe = (ishaxor ? 60 : 30);
					break;
		case MAGIC_FLUTE:
		case MAGIC_HARP:
		case FROST_HORN:
		case TEMPEST_HORN:
		case ETHER_HORN:
		case SHADOW_HORN:
		case CHROME_HORN:
		case FIRE_HORN:
		case DRUM_OF_EARTHQUAKE:
		/* KMH, balance patch -- removed
		case PAN_PIPE_OF_SUMMONING:
		case PAN_PIPE_OF_THE_SEWERS: */
			otmp->recharged = 0;
			if(!rn2(5)) otmp->recharged = rnd(7);
			otmp->spe = rnd(ishaxor ? 30 : 15);
			otmp->spe -= rn2(3);
			blessorcurse_on_creation(otmp, 10);
					break;
		default: /* all the other tools --Amy */
		if(!rn2(ishaxor ? 3 : 6)) {
			otmp->spe = rne(Race_if(PM_LISTENER) ? 3 : 2);
			if (rn2(2)) otmp->blessed = rn2(2);
			 else	blessorcurse_on_creation(otmp, 3);
		} else if(!rn2(ishaxor ? 4 : 8)) {
			if (rn2(10)) curse_on_creation(otmp);
			 else	blessorcurse_on_creation(otmp, 3);
			otmp->spe = -rne(Race_if(PM_LISTENER) ? 3 : 2);
		} else	blessorcurse_on_creation(otmp, 10);

		if (otmp->spe && !rn2(6) && In_lategame(&u.uz)) {
			if (otmp->spe > 0) otmp->spe += rne(Race_if(PM_LISTENER) ? 3 : 2);
			else otmp->spe -= rne(Race_if(PM_LISTENER) ? 3 : 2);
		}

		if ((Race_if(PM_LISTENER) || RngeListening) && !Hallucination && (rnd(30) > ACURR(A_INT)) && (abs(otmp->spe) > 3 || (abs(otmp->spe) == 3 && rn2(2) ) || (abs(otmp->spe) == 2 && !rn2(3) )|| (abs(otmp->spe) == 1 && !rn2(5) ) ) ) pline("Precognition: made object with enchantment %d", abs(otmp->spe));

		break;
	    }

		if (!rn2(800)) otmp->oerodeproof = 1;
		if (!rn2(800)) {
			if (!rn2(3)) otmp->oeroded = rnd(3);
			if (!rn2(3)) otmp->oeroded2 = rnd(3);
		}
		if (!rn2(400)) otmp->greased = rn2(5) ? 1 : rn2(3) ? 2 : 3;

		if (artif && (artif != 2) && !rn2(Race_if(PM_LISTENER) ? 80 : 40)) {
		    otmp = mk_artifact(otmp, (aligntyp)A_NONE, TRUE);
			if ((Race_if(PM_LISTENER) || RngeListening) && !Hallucination && (rnd(30) > ACURR(A_INT))) pline("Precognition: made artifact");
		}
		else if ((artif && !rn2(100)) || (otmp->spe && is_weptool(otmp) && !rn2((abs(otmp->spe) > 9) ? 2 : (abs(otmp->spe) > 7) ? 3 : (abs(otmp->spe) > 5) ? 4 : (abs(otmp->spe) > 4) ? 5 : (abs(otmp->spe) > 3) ? 6 : (abs(otmp->spe) > 2) ? 7 : (abs(otmp->spe) > 1) ? 8 : (abs(otmp->spe) > 0) ? 10 : 20)) ) {
		    otmp = oname(otmp, !rn2(20) ? generate_garbage_string() : fauxartinames[rn2(SIZE(fauxartinames))] );
			otmp->fakeartifact = 1;
			if (artif != 2) u.fakeartifacts++;
		}

	    break;
	case AMULET_CLASS:
		if (otmp->otyp == AMULET_OF_YENDOR) flags.made_amulet = TRUE;
		if(rn2(10) && (otmp->otyp == AMULET_OF_STRANGULATION ||
		   otmp->otyp == AMULET_OF_CHANGE ||
		   otmp->otyp == AMULET_OF_FUMBLING ||
		   otmp->otyp == AMULET_OF_HUNGER ||
		   otmp->otyp == AMULET_OF_BLINDNESS ||
		   otmp->otyp == AMULET_OF_RESTFUL_SLEEP)) {
			curse_on_creation(otmp);
		} else {
			blessorcurse_on_creation(otmp, 10);
		}
		if (!rn2(1200)) otmp->oerodeproof = 1;
		if (!rn2(1200)) {
			if (!rn2(3)) otmp->oeroded = rnd(3);
			if (!rn2(3)) otmp->oeroded2 = rnd(3);
		}
		if (!rn2(600)) otmp->greased = rn2(5) ? 1 : rn2(3) ? 2 : 3;

		if (artif && (artif != 2) && !rn2(Race_if(PM_LISTENER) ? 120 : 60)) {
		    otmp = mk_artifact(otmp, (aligntyp)A_NONE, TRUE);
			if ((Race_if(PM_LISTENER) || RngeListening) && !Hallucination && (rnd(30) > ACURR(A_INT))) pline("Precognition: made artifact");
		}
		else if (artif && !rn2(140)) {
		    otmp = oname(otmp, !rn2(20) ? generate_garbage_string() : fauxartinames[rn2(SIZE(fauxartinames))] );
			otmp->fakeartifact = 1;
			if (artif != 2) u.fakeartifacts++;
		}

		break;
	case IMPLANT_CLASS:

		if(objects[otmp->otyp].oc_charged) {
		    blessorcurse_on_creation(otmp, 5);
		    if(rn2(10)) {
			if(rn2(10) && bcsign(otmp))
			    otmp->spe = bcsign(otmp) * rne(Race_if(PM_LISTENER) ? 3 : 2);
			else otmp->spe = rn2(2) ? rne(Race_if(PM_LISTENER) ? 3 : 2) : -rne(Race_if(PM_LISTENER) ? 3 : 2);
		    }
		    /* make useless +0 implants much less common */
		    if (otmp->spe == 0) {
/*                     otmp->spe = rn2(4) - rn2(3); */
		       /* wow! +8! */
		       if (rn2(3)) otmp->spe = rne(2)+1;
		       else otmp->spe = -(rne(2)+1);
		    }
			if ((Race_if(PM_LISTENER) || RngeListening) && !Hallucination && (rnd(30) > ACURR(A_INT)) && (abs(otmp->spe) > 3 || (abs(otmp->spe) == 3 && rn2(2) ) || (abs(otmp->spe) == 2 && !rn2(3) )|| (abs(otmp->spe) == 1 && !rn2(5) ) ) ) pline("Precognition: made object with enchantment %d", abs(otmp->spe));

		    /* negative implants are usually cursed */
		    if (otmp->spe < 0 && rn2(5)) curse_on_creation(otmp);
		} else blessorcurse_on_creation(otmp, 5);

		if (!rn2(1200)) otmp->oerodeproof = 1;
		if (!rn2(1200)) {
			if (!rn2(3)) otmp->oeroded = rnd(3);
			if (!rn2(3)) otmp->oeroded2 = rnd(3);
		}
		if (!rn2(600)) otmp->greased = rn2(5) ? 1 : rn2(3) ? 2 : 3;

		if (artif && (artif != 2) && !rn2(Race_if(PM_LISTENER) ? 120 : 60)) {
		    otmp = mk_artifact(otmp, (aligntyp)A_NONE, TRUE);
			if ((Race_if(PM_LISTENER) || RngeListening) && !Hallucination && (rnd(30) > ACURR(A_INT))) pline("Precognition: made artifact");
		}
		else if (artif && !rn2(140)) {
		    otmp = oname(otmp, !rn2(20) ? generate_garbage_string() : fauxartinames[rn2(SIZE(fauxartinames))] );
			otmp->fakeartifact = 1;
			if (artif != 2) u.fakeartifacts++;
		}

		break;
	case VENOM_CLASS:
		blessorcurse_on_creation(otmp, 10);

		if (!rn2(400)) otmp->oerodeproof = 1;
		if (!rn2(400)) {
			if (!rn2(3)) otmp->oeroded = rnd(3);
			if (!rn2(3)) otmp->oeroded2 = rnd(3);
		}
		if (!rn2(200)) otmp->greased = rn2(5) ? 1 : rn2(3) ? 2 : 3;

		if (artif && (artif != 2) && !rn2(Race_if(PM_LISTENER) ? 40 : 20)) {
		    otmp = mk_artifact(otmp, (aligntyp)A_NONE, TRUE);
			if ((Race_if(PM_LISTENER) || RngeListening) && !Hallucination && (rnd(30) > ACURR(A_INT))) pline("Precognition: made artifact");
		}
		else if (artif && !rn2(50)) {
		    otmp = oname(otmp, !rn2(20) ? generate_garbage_string() : fauxartinames[rn2(SIZE(fauxartinames))] );
			otmp->fakeartifact = 1;
			if (artif != 2) u.fakeartifacts++;
		}
		break;
	case CHAIN_CLASS:
	case BALL_CLASS:

		switch (otmp->otyp) {
			case HEAVY_LASER_BALL:
			case LASER_CHAIN:
				otmp->recharged = 0;
				if(!rn2(5)) otmp->recharged = rnd(7);
				otmp->lamplit = 0;
				otmp->age = (long) rn1(500,1000);
						if (!rn2(20)) {
							otmp->age += rnz(1000);
						} else if (!rn2(20)) {
							otmp->age = rnz(500);
						}

				if (otmp->otyp == BLACK_LIGHTSABER) otmp->age += rnz(5000);

				if (ishaxor) otmp->age *= 2;

				break;
		}

			if ((Race_if(PM_LISTENER) || RngeListening) && !Hallucination && (rnd(30) > ACURR(A_INT)) && (abs(otmp->spe) > 3 || (abs(otmp->spe) == 3 && rn2(2) ) || (abs(otmp->spe) == 2 && !rn2(3) )|| (abs(otmp->spe) == 1 && !rn2(5) ) ) ) pline("Precognition: made object with enchantment %d", abs(otmp->spe));


		if(!rn2(ishaxor ? 3 : 6)) {
			otmp->spe = rne(Race_if(PM_LISTENER) ? 3 : 2);
			if (rn2(2)) otmp->blessed = rn2(2);
			 else	blessorcurse_on_creation(otmp, 3);
		} else if(!rn2(ishaxor ? 4 : 8)) {
			if (rn2(10)) curse_on_creation(otmp);
			 else	blessorcurse_on_creation(otmp, 3);
			otmp->spe = -rne(Race_if(PM_LISTENER) ? 3 : 2);
		} else	blessorcurse_on_creation(otmp, 10);

		if (otmp->spe && !rn2(6) && In_lategame(&u.uz)) {
			if (otmp->spe > 0) otmp->spe += rne(Race_if(PM_LISTENER) ? 3 : 2);
			else otmp->spe -= rne(Race_if(PM_LISTENER) ? 3 : 2);
		}

		if (issoviet) otmp->spe = 0;

		if ((Race_if(PM_LISTENER) || RngeListening) && !Hallucination && (rnd(30) > ACURR(A_INT)) && (abs(otmp->spe) > 3 || (abs(otmp->spe) == 3 && rn2(2) ) || (abs(otmp->spe) == 2 && !rn2(3) )|| (abs(otmp->spe) == 1 && !rn2(5) ) ) ) pline("Precognition: made object with enchantment %d", abs(otmp->spe));

		if (!rn2(1000)) otmp->oerodeproof = 1;
		if (!rn2(1000)) {
			if (!rn2(3)) otmp->oeroded = rnd(3);
			if (!rn2(3)) otmp->oeroded2 = rnd(3);
		}
		if (!rn2(500)) otmp->greased = rn2(5) ? 1 : rn2(3) ? 2 : 3;

		if (artif && (artif != 2) && !rn2(Race_if(PM_LISTENER) ? 100 : 50)) {
		    otmp = mk_artifact(otmp, (aligntyp)A_NONE, TRUE);
			if ((Race_if(PM_LISTENER) || RngeListening) && !Hallucination && (rnd(30) > ACURR(A_INT))) pline("Precognition: made artifact");
		}
		else if ((artif || otmp->spe) && !rn2((abs(otmp->spe) > 9) ? 2 : (abs(otmp->spe) > 7) ? 3 : (abs(otmp->spe) > 5) ? 4 : (abs(otmp->spe) > 4) ? 5 : (abs(otmp->spe) > 3) ? 6 : (abs(otmp->spe) > 2) ? 7 : (abs(otmp->spe) > 1) ? 8 : (abs(otmp->spe) > 0) ? 10 : 150)) {
		    otmp = oname(otmp, !rn2(20) ? generate_garbage_string() : fauxartinames[rn2(SIZE(fauxartinames))] );
			otmp->fakeartifact = 1;
			if (artif != 2) u.fakeartifacts++;
		}

		break;
	case POTION_CLASS:
		if (otmp->otyp == POT_OIL)
		    otmp->age = MAX_OIL_IN_FLASK;	/* amount of oil */
		/* fall through */
	case SCROLL_CLASS:
#ifdef MAIL
		if (otmp->otyp != SCR_MAIL)
#endif
			blessorcurse_on_creation(otmp, 4);

		if (!rn2(320)) otmp->oerodeproof = 1;
		if (!rn2(320)) {
			if (!rn2(3)) otmp->oeroded = rnd(3);
			if (!rn2(3)) otmp->oeroded2 = rnd(3);
		}
		if (!rn2(160)) otmp->greased = rn2(5) ? 1 : rn2(3) ? 2 : 3;

		if (artif && (artif != 2) && !rn2(Race_if(PM_LISTENER) ? 320 : 160)) {
		    otmp = mk_artifact(otmp, (aligntyp)A_NONE, TRUE);
			if ((Race_if(PM_LISTENER) || RngeListening) && !Hallucination && (rnd(30) > ACURR(A_INT))) pline("Precognition: made artifact");
		}
		else if (artif && !rn2(400)) {
		    otmp = oname(otmp, !rn2(20) ? generate_garbage_string() : fauxartinames[rn2(SIZE(fauxartinames))] );
			otmp->fakeartifact = 1;
			if (artif != 2) u.fakeartifacts++;
		}
		if (!rn2(3) && !(otmp->otyp == SCR_HEALING || otmp->otyp == SCR_EXTRA_HEALING || otmp->otyp == SCR_HEAL_OTHER || otmp->otyp == SCR_STANDARD_ID || otmp->otyp == SCR_MANA || otmp->otyp == SCR_GREATER_MANA_RESTORATION
#ifdef MAIL
		|| otmp->otyp == SCR_MAIL
#endif
		|| otmp->otyp == SCR_CURE || otmp->otyp == SCR_PHASE_DOOR) ) otmp->finalcancel = TRUE;

		break;
	case SPBOOK_CLASS:
		/* WAC charged books are easier to read */
		if (otmp->otyp != SPE_BOOK_OF_THE_DEAD) {
			if (!rn2(3)) otmp->spe = rnd(ishaxor ? 10 : 5);
			else if (rn2(5)) otmp->spe = rnd(ishaxor ? 6 : 3);
			else otmp->spe = rnd(ishaxor ? 4 : 2);
			if (!rn2(10)) otmp->spe -= rnd(4);
		}
		otmp->recharged = 0;
		if(!rn2(3)) otmp->recharged = rnd(7);
		blessorcurse_on_creation(otmp, 17);

		if (!rn2(200)) otmp->oerodeproof = 1;
		if (!rn2(200)) {
			if (!rn2(3)) otmp->oeroded = rnd(3);
			if (!rn2(3)) otmp->oeroded2 = rnd(3);
		}
		if (!rn2(100)) otmp->greased = rn2(5) ? 1 : rn2(3) ? 2 : 3;

		if (artif && (artif != 2) && !rn2(Race_if(PM_LISTENER) ? 200 : 100)) {
		    otmp = mk_artifact(otmp, (aligntyp)A_NONE, TRUE);
			if ((Race_if(PM_LISTENER) || RngeListening) && !Hallucination && (rnd(30) > ACURR(A_INT))) pline("Precognition: made artifact");
		}
		else if (artif && !rn2(250)) {
		    otmp = oname(otmp, !rn2(20) ? generate_garbage_string() : fauxartinames[rn2(SIZE(fauxartinames))] );
			otmp->fakeartifact = 1;
			if (artif != 2) u.fakeartifacts++;
		}
		if (!rn2(4)) otmp->finalcancel = TRUE;

		break;
/* -----------============STEPHEN WHITE'S NEW CODE============----------- */           
	case ARMOR_CLASS:
		if(rn2(10) && (otmp->otyp == FUMBLE_BOOTS ||
		   otmp->otyp == LEVITATION_BOOTS ||
		   otmp->otyp == ZIPPER_BOOTS ||
		   otmp->otyp == FIRE_BOOTS ||
		   otmp->otyp == BOOTS_OF_MOLASSES ||
		   otmp->otyp == HIPPIE_HEELS ||
		   otmp->otyp == CLOAK_OF_CONFUSION ||
		   otmp->otyp == CLOAK_OF_FUMBLING ||
		   otmp->otyp == CLOAK_OF_DIMNESS ||
		   otmp->otyp == CLOAK_OF_TRANSFORMATION ||
		   otmp->otyp == CLOAK_OF_AGGRAVATION ||
		   otmp->otyp == CLOAK_OF_CONFLICT ||
		   otmp->otyp == CLOAK_OF_STAT_LOCK ||
		   otmp->otyp == HELM_OF_OPPOSITE_ALIGNMENT ||
		   otmp->otyp == HELM_OF_FEAR ||
		   otmp->otyp == BOOGEYMAN_HELMET ||
		   otmp->otyp == HELM_OF_SENSORY_DEPRIVATION ||
		   otmp->otyp == HELM_OF_HUNGER ||
		   otmp->otyp == HELM_OF_STORMS ||
		   otmp->otyp == HELM_OF_TELEPORTATION ||
		   otmp->otyp == HELM_OF_DETECT_MONSTERS ||
		   otmp->otyp == GAUNTLETS_OF_FUMBLING ||
		   otmp->otyp == GAUNTLETS_OF_PANIC ||
		   otmp->otyp == GAUNTLETS_OF_SLOWING ||
		   otmp->otyp == ROBE_OF_WEAKNESS ||
		   otmp->otyp == OILSKIN_GLOVES ||
		   !rn2(ishaxor ? 5 : 11))) {
			if (rn2(10)) curse_on_creation(otmp);
			 else	blessorcurse_on_creation(otmp, 3);
			otmp->spe = -rne(Race_if(PM_LISTENER) ? 3 : 2);
		} else if(!rn2(ishaxor ? 5 : 8)) {
			if (rn2(2)) otmp->blessed = rn2(2);
			 else	blessorcurse_on_creation(otmp, 3);
			otmp->spe = rne(Race_if(PM_LISTENER) ? 3 : 2);
		} else	blessorcurse_on_creation(otmp, 10);

		if (otmp->spe && !rn2(6) && In_lategame(&u.uz)) {
			if (otmp->spe > 0) otmp->spe += rne(Race_if(PM_LISTENER) ? 3 : 2);
			else otmp->spe -= rne(Race_if(PM_LISTENER) ? 3 : 2);
		}

		if ((Race_if(PM_LISTENER) || RngeListening) && !Hallucination && (rnd(30) > ACURR(A_INT)) && (abs(otmp->spe) > 3 || (abs(otmp->spe) == 3 && rn2(2) ) || (abs(otmp->spe) == 2 && !rn2(3) )|| (abs(otmp->spe) == 1 && !rn2(5) ) ) ) pline("Precognition: made object with enchantment %d", abs(otmp->spe));

		if (!rn2(80)) otmp->oerodeproof = 1;
		if (!rn2(80)) {
			if (!rn2(3)) otmp->oeroded = rnd(3);
			if (!rn2(3)) otmp->oeroded2 = rnd(3);
		}
		if (!rn2(40)) otmp->greased = rn2(5) ? 1 : rn2(3) ? 2 : 3;

		if (artif && (artif != 2) && !rn2(Race_if(PM_LISTENER) ? 80 : 40)) {
		    otmp = mk_artifact(otmp, (aligntyp)A_NONE, TRUE);
			if ((Race_if(PM_LISTENER) || RngeListening) && !Hallucination && (rnd(30) > ACURR(A_INT))) pline("Precognition: made artifact");
		}
		else if ((artif || otmp->spe) && !rn2((abs(otmp->spe) > 9) ? 2 : (abs(otmp->spe) > 7) ? 3 : (abs(otmp->spe) > 5) ? 4 : (abs(otmp->spe) > 4) ? 5 : (abs(otmp->spe) > 3) ? 6 : (abs(otmp->spe) > 2) ? 7 : (abs(otmp->spe) > 1) ? 8 : (abs(otmp->spe) > 0) ? 10 : 100)) {
		    otmp = oname(otmp, !rn2(20) ? generate_garbage_string() : fauxartinames[rn2(SIZE(fauxartinames))] );
			otmp->fakeartifact = 1;
			if (artif != 2) u.fakeartifacts++;
		}
		/* simulate lacquered armor for samurai */
		if ( ( Role_if(PM_SAMURAI) && otmp->otyp == SPLINT_MAIL) || (Role_if(PM_ERDRICK) && otmp->otyp == HELMET) ) {
#ifdef UNIXPC
			/* optimizer bitfield bug */
			otmp->oerodeproof = 1;
			otmp->rknown = 1;
#else
			otmp->oerodeproof = otmp->rknown = 1;
#endif
		}

		if (!rn2(100) || (uarmh && uarmh->oartifact == ART_WHY_NOT_DO_THE_REAL_THING && !rn2(20)) ||
		( (is_shirt(otmp) || otmp->otyp == WHISPERING_HELMET || otmp->otyp == MOMHAT || otmp->otyp == CAPTCHA_HELM || otmp->otyp == QUIZ_HELM || otmp->otyp == LONG_POINTY_HEELS || otmp->otyp == METER_GAUNTLETS || otmp->otyp == WEIGHTING_GAUNTLETS || otmp->otyp == BIGSCRIPT_HELM ) && !rn2(15) ) ||
		( (otmp->otyp == WEDGE_SANDALS || otmp->otyp == REPEATABLE_BOOTS || otmp->otyp == DANCING_SHOES || otmp->otyp == ANGER_HELM || otmp->otyp == SCRIPTED_HELMET || otmp->otyp == POKELIE_GLOVES || otmp->otyp == ASS_KICKER_BOOTS || otmp->otyp == SWEET_MOCASSINS || otmp->otyp == SOFT_SNEAKERS ) && !rn2(10) ) ||
		( (otmp->otyp == CLOAK_OF_UNSPELLING || otmp->otyp == ANTI_CASTER_CLOAK || otmp->otyp == HEAVY_STATUS_CLOAK || otmp->otyp == CLOAK_OF_LUCK_NEGATION || otmp->otyp == YELLOW_SPELL_CLOAK || otmp->otyp == VULNERABILITY_CLOAK || otmp->otyp == CLOAK_OF_INVENTORYLESSNESS || otmp->otyp == HELM_OF_LOSE_IDENTIFICATION || otmp->otyp == HELM_OF_OBSCURED_DISPLAY || otmp->otyp == COVETED_BOOTS || otmp->otyp == LIGHTLESS_BOOTS || otmp->otyp == SELF_WILLED_HEELS || otmp->otyp == SOIL_CLINGING_BOOTS || otmp->otyp == PERMANENTLY_BLACK_SHOES || otmp->otyp == AUTOSCOOTER_HEELS || otmp->otyp == FORCEFUL_BOOTS || otmp->otyp == BUM_BUM_BOOTS || otmp->otyp == SADISTIC_BOOTS || otmp->otyp == FLUCKERING_BOOTS || otmp->otyp == SPRAP_BOOTS || otmp->otyp == FAILED_ATTEMPT_BOOTS || otmp->otyp == BRIGHT_CYAN_BEAUTIES || otmp->otyp == SIA_BOOTS || otmp->otyp == PLATINUM_SPELL_BOOTS || otmp->otyp == INVIS_WALKERS || otmp->otyp == NOISY_BOOTS || otmp->otyp == MORK_BOOTS || otmp->otyp == HELM_OF_THIRST || otmp->otyp == CLOCKLOAK || otmp->otyp == KILLER_HEELS || otmp->otyp == VIOLET_BEAUTY_HEELS || otmp->otyp == CHECKER_BOOTS || otmp->otyp == ELVIS_SHOES || otmp->otyp == HELM_OF_AMNESIA || otmp->otyp == BLACKY_HELMET || otmp->otyp == BLOODSUCKING_SHOES || otmp->otyp == ANTI_DRINKER_HELMET || otmp->otyp == NOKEDEX_CLOAK || otmp->otyp == CYPHER_HELM || otmp->otyp == DIZZY_HELMET || otmp->otyp == MUTING_HELM || otmp->otyp == ULCH_HELMET || otmp->otyp == GAUNTLETS_OF_BAD_CASTING || otmp->otyp == MARY_SUE_GLOVES || otmp->otyp == DIMMER_HELMET || otmp->otyp == BORDERLESS_HELMET || otmp->otyp == BROWN_SHIT_CLOAK || otmp->otyp == ANTICALL_CLOAK || otmp->otyp == YELLOW_WING || otmp->otyp == ELESDE_CLOAK || otmp->otyp == CLOAK_OF_GRAVATION || otmp->otyp == ALLPICKING_GLOVES || otmp->otyp == SILVER_SPELL_GLOVES || otmp->otyp == SPELLDRAIN_GLOVES || otmp->otyp == CLOAK_OF_NONFOLLOWING || otmp->otyp == SPELL_RETAIN_CLOAK || otmp->otyp == CLOAK_OF_FAST_DECAY || otmp->otyp == ORANGE_SPELL_GLOVES || otmp->otyp == MOJIBAKE_GLOVES || otmp->otyp == DIXPLOSION_GLOVES || otmp->otyp == LEFT_APPENDAGE_GLOVES || otmp->otyp == HELMET_OF_ANTI_SEARCHING || otmp->otyp == RARE_HELMET || otmp->otyp == HEAVY_GRABBING_GLOVES || otmp->otyp == AIRSTEP_BOOTS || otmp->otyp == BOOTS_OF_INTERRUPTION || otmp->otyp == BLACK_SPELL_GAUNTLETS || otmp->otyp == HIGH_HEELED_SKIERS || otmp->otyp == HIGH_SCORING_HEELS || otmp->otyp == HELM_OF_STARVATION || otmp->otyp == QUAFFER_HELMET || otmp->otyp == PSEUDO_TELEPORTER_CLOAK || otmp->otyp == SNARENET_CLOAK || otmp->otyp == PINK_SPELL_CLOAK || otmp->otyp == QUAVERSAL_HELMET || otmp->otyp == HELM_OF_SHUFFLING || otmp->otyp == GOLDSPELL_HELMET || otmp->otyp == AIRHEAD_CAP || otmp->otyp == FALLOUT_HELMET || otmp->otyp == IDENTIFY_CURD_HELMET || otmp->otyp == BAEAEAEP_SPY_HELMET || otmp->otyp == CRUNCHER_HELMET || otmp->otyp == DISTORTED_GRIMACE || otmp->otyp == ELM_ET || otmp->otyp == SANEMAKER_HELMET || otmp->otyp == GRAYOUT_CLOAK || otmp->otyp == TRON_BOOTS || otmp->otyp == RED_SPELL_HEELS || otmp->otyp == DESTRUCTIVE_HEELS || otmp->otyp == CARTRIDGE_OF_HAVING_A_HORROR || otmp->otyp == SOUND_EFFECT_HELMET || otmp->otyp == INCORRECTLY_ADJUSTED_HELMET || otmp->otyp == CLOAK_OF_RESPAWNING || otmp->otyp == HELM_OF_BAD_ALIGNMENT || otmp->otyp == SOUNDPROOF_HELMET || otmp->otyp == HELM_OF_COUNTER_ROTATION || otmp->otyp == DELIGHT_HELMET || otmp->otyp == OUT_OF_MEMORY_HELMET || otmp->otyp == FUCKUP_MELEE_GAUNTLETS || otmp->otyp == INFOLESS_HELMET || otmp->otyp == BLUE_SPELL_HELMET || otmp->otyp == MORE_HELMET || otmp->otyp == MESSAGE_FILTER_HELMET || otmp->otyp == FLICKER_VISOR || otmp->otyp == STORMY_CLOAK || otmp->otyp == CLOAK_OF_WRONG_ANNOUNCEMENT || otmp->otyp == BATTERY_CLOAK || otmp->otyp == NAYLIGHT_CLOAK || otmp->otyp == UNDERLAYER_CLOAK || otmp->otyp == EVENCORE_CLOAK || otmp->otyp == SCALER_MITTENS || otmp->otyp == GLOVES_OF_ENERGY_DRAINING || otmp->otyp == MENU_NOSE_GLOVES || otmp->otyp == UNWIELDY_GLOVES || otmp->otyp == ELONGATION_CLOAK || otmp->otyp == CYAN_SPELL_CLOAK || otmp->otyp == BANKING_GLOVES || otmp->otyp == DIFFICULT_GLOVES || otmp->otyp == CHAOS_GLOVES || otmp->otyp == COMPETITION_BOOTS || otmp->otyp == QUASIMODULAR_BOOTS || otmp->otyp == SINFUL_HEELS || otmp->otyp == LEVELING_GLOVES || otmp->otyp == GIMP_CLOAK || otmp->otyp == UNFAIR_ATTACK_CLOAK || otmp->otyp == CLOAK_OF_BAD_PART || otmp->otyp == ADOM_CLOAK || otmp->otyp == EMPTY_LINE_HELMET || otmp->otyp == GREEN_SPELL_HELMET || otmp->otyp == EGOIST_CLOAK || otmp->otyp == CHATBOX_CLOAK || otmp->otyp == HERETIC_CLOAK || otmp->otyp == EERIE_CLOAK || otmp->otyp == CLOAK_OF_BAD_TRAPPING || otmp->otyp == CLOAK_OF_NAKEDNESS || otmp->otyp == GAUNTLETS_OF_REVERSE_ENCHANTME || otmp->otyp == CLOAK_OF_TIME || otmp->otyp == SPAWN_CLOAK || otmp->otyp == CONFUSING_GLOVES || otmp->otyp == DISENCHANTING_BOOTS || otmp->otyp == LIMITATION_BOOTS || otmp->otyp == UNDROPPABLE_GLOVES || otmp->otyp == GAUNTLETS_OF_MISSING_INFORMATI || otmp->otyp == GAUNTLETS_OF_TRAP_CREATION || otmp->otyp == GAUNTLETS_OF_STEALING || otmp->otyp == CLOAK_OF_SUDDEN_ATTACK || otmp->otyp == TRUMP_COAT || otmp->otyp == GREYOUT_CLOAK || otmp->otyp == WHITE_SPELL_CLOAK || otmp->otyp == GAUNTLETS_OF_MISFIRING || otmp->otyp == SADO_MASO_GLOVES || otmp->otyp == FEMININE_PUMPS || otmp->otyp == LEATHER_PEEP_TOES || otmp->otyp == COMBAT_STILETTOS || otmp->otyp == LADY_BOOTS || otmp->otyp == ITALIAN_HEELS || otmp->otyp == STILETTO_SANDALS || otmp->otyp == AUTODESTRUCT_DE_VICE_BOOTS || otmp->otyp == SPEEDBUG_BOOTS || otmp->otyp == DISCONNECTED_BOOTS || otmp->otyp == BOSS_BOOTS || otmp->otyp == PET_STOMPING_PLATFORM_BOOTS || otmp->otyp == DEMENTIA_BOOTS || otmp->otyp == BOOTS_OF_FAINTING || otmp->otyp == DIFFICULT_BOOTS || otmp->otyp == BOOTS_OF_WEAKNESS || otmp->otyp == GRIDBUG_CONDUCT_BOOTS || otmp->otyp == STAIRWELL_STOMPING_BOOTS ) && !rn2(5) ) ||
		(otmp->otyp == HIPPIE_HEELS || otmp->otyp == SENTIENT_HIGH_HEELED_SHOES) ) {

			if (!rn2(3)) otmp->enchantment = randenchantment();

		}

		/* certain base items are characterized by the fact that they always have an enchantment and are otherwise plain --Amy */
		if (otmp->otyp == SPECIAL_CLOAK || otmp->otyp == CHANTER_SHIRT || otmp->otyp == MAGIC_DRAGON_SCALE_MAIL || otmp->otyp == MAGIC_DRAGON_SCALES || otmp->otyp == MAGIC_DRAGON_SCALE_SHIELD || otmp->otyp == SPECIAL_SHIELD || otmp->otyp == SPECIAL_LEATHER_ARMOR || otmp->otyp == MAGE_PLATE_MAIL || otmp->otyp == ROBE_OF_SPECIALTY || otmp->otyp == WONDER_HELMET || otmp->otyp == ARCANE_GAUNTLETS || otmp->otyp == SKY_HIGH_HEELS) {
			otmp->enchantment = randenchantment();
		}


		break;
/* -----------============STEPHEN WHITE'S NEW CODE============----------- */           
	case WAND_CLASS:
		if(otmp->otyp == WAN_WISHING) {
			if (rn2(50)) otmp->spe = rnd(ishaxor ? 3 : 2);
			else otmp->spe = rnd(ishaxor ? 4 : 3); /* if you get really lucky... --Amy */
			if (Is_stronghold(&u.uz)) otmp->oinvis = 1;
			otmp->recharged = 0;
			if(!rn2(2)) otmp->recharged = 1;
		} else if(otmp->otyp == WAN_GENOCIDE) {
			if (rn2(20)) otmp->spe = rnd(ishaxor ? 4 : 3);
			else otmp->spe = rnd(ishaxor ? 6 : 4); /* sometimes you may get lucky --Amy */
			otmp->recharged = 0;
			if(!rn2(3)) otmp->recharged = rnd(7);
		} else if(otmp->otyp == WAN_ACQUIREMENT) {
			if (rn2(20)) otmp->spe = rnd(ishaxor ? 6 : 5);
			else otmp->spe = rnd(ishaxor ? 10 : 8); /* sometimes you may get lucky --Amy */
			otmp->recharged = 0;
			if(!rn2(3)) otmp->recharged = 1;
		} else if(otmp->otyp == WAN_GAIN_LEVEL) {
			if (rn2(20)) otmp->spe = rnd(ishaxor ? 5 : 4);
			else otmp->spe = rnd(ishaxor ? 6 : 5); /* sometimes you may get lucky --Amy */
			otmp->recharged = 0;
			if(!rn2(2)) otmp->recharged = rnd(7);
		} else if(otmp->otyp == WAN_VENOM_SCATTERING) {
			if (rn2(20)) otmp->spe = rnd(ishaxor ? 40 : 20);
			else otmp->spe = rnd(ishaxor ? 50 : 25); /* sometimes you may get lucky --Amy */
			otmp->recharged = 0;
			if(!rn2(10)) otmp->recharged = rnd(7);
		} else if(otmp->otyp == WAN_INCREASE_MAX_HITPOINTS) {
			if (rn2(20)) otmp->spe = rnd(ishaxor ? 6 : 5);
			else otmp->spe = rnd(ishaxor ? 7 : 6); /* sometimes you may get lucky --Amy */
			otmp->recharged = 0;
			if(!rn2(2)) otmp->recharged = rnd(7);
		} else if(otmp->otyp == WAN_PARALYSIS || otmp->otyp == WAN_DISINTEGRATION || otmp->otyp == WAN_DISINTEGRATION_BEAM || otmp->otyp == WAN_STONING || otmp->otyp == WAN_INERTIA || otmp->otyp == WAN_TIME || otmp->otyp == WAN_IDENTIFY || otmp->otyp == WAN_REMOVE_CURSE || otmp->otyp == WAN_TIME_STOP || otmp->otyp == WAN_TELE_LEVEL || otmp->otyp == WAN_ENTRAPPING || otmp->otyp == WAN_MAGIC_MAPPING || otmp->otyp == WAN_CREATE_FAMILIAR) {
			if (rn2(20)) otmp->spe = rnd(ishaxor ? 6 : 5);
			else otmp->spe = rnd(ishaxor ? 10 : 8); /* sometimes you may get lucky --Amy */
			otmp->recharged = 0;
			if(!rn2(10)) otmp->recharged = rnd(7);
		} else if(otmp->otyp == WAN_ENLIGHTENMENT || otmp->otyp == WAN_TRAP_DISARMING || otmp->otyp == WAN_CANCELLATION || otmp->otyp == WAN_POLYMORPH || otmp->otyp == WAN_MUTATION || otmp->otyp == WAN_CHARGING || otmp->otyp == WAN_BAD_EQUIPMENT) {
			if (rn2(20)) otmp->spe = rnd(ishaxor ? 10 : 7);
			else otmp->spe = rnd(ishaxor ? 15 : 10); /* sometimes you may get lucky --Amy */
			otmp->recharged = 0;
			if(!rn2(10)) otmp->recharged = rnd(7);

		} else if (rn2(10) && !ishaxor) otmp->spe = rnd( (objects[otmp->otyp].oc_dir == NODIR) ? 20 : (objects[otmp->otyp].oc_dir == IMMEDIATE) ? 12 : 10);
			else otmp->spe = rnd( (objects[otmp->otyp].oc_dir == NODIR) ? 40 : (objects[otmp->otyp].oc_dir == IMMEDIATE) ? 24 : 20); /* low chance for extra charges --Amy */
		if (!rn2(3) && !issoviet) otmp->spe -= rnd(3); /* allow random useless wands to spawn --Amy */
		/* "Remove chance of non-charged wands spawning." In Soviet Russia, players don't realize that a lower chance of finding useful stuff makes the game more interesting. The pre-cancelled wands are actually there for a reason: that way, players trying to zap-identify can randomly vaporize their wands if they happen to be cancelled, making identification more useful! --Amy */

		blessorcurse_on_creation(otmp, 17);
		if (otmp->otyp != WAN_WISHING && otmp->otyp != WAN_ACQUIREMENT && otmp->otyp != WAN_GENOCIDE && otmp->otyp != WAN_GAIN_LEVEL && otmp->otyp != WAN_INCREASE_MAX_HITPOINTS) otmp->recharged = 0; /* used to control recharging */
		if (!rn2(10)) otmp->recharged = rnd(7); /* allow recharged wands to spawn --Amy */

		if (!rn2(3200)) otmp->oerodeproof = 1;
		if (!rn2(3200)) {
			if (!rn2(3)) otmp->oeroded = rnd(3);
			if (!rn2(3)) otmp->oeroded2 = rnd(3);
		}
		if (!rn2(1600)) otmp->greased = rn2(5) ? 1 : rn2(3) ? 2 : 3;

		if (artif && (artif != 2) && !rn2(Race_if(PM_LISTENER) ? 320 : 160)) {
		    otmp = mk_artifact(otmp, (aligntyp)A_NONE, TRUE);
			if ((Race_if(PM_LISTENER) || RngeListening) && !Hallucination && (rnd(30) > ACURR(A_INT))) pline("Precognition: made artifact");
		}
		else if (artif && !rn2(500)) {
		    otmp = oname(otmp, !rn2(20) ? generate_garbage_string() : fauxartinames[rn2(SIZE(fauxartinames))] );
			otmp->fakeartifact = 1;
			if (artif != 2) u.fakeartifacts++;
		}

		break;
	case RING_CLASS:
		if(objects[otmp->otyp].oc_charged) {
		    blessorcurse_on_creation(otmp, 3);
		    if(rn2(10)) {
			if(rn2(10) && bcsign(otmp))
			    otmp->spe = bcsign(otmp) * rne(Race_if(PM_LISTENER) ? 3 : 2);
			else otmp->spe = rn2(2) ? rne(Race_if(PM_LISTENER) ? 3 : 2) : -rne(Race_if(PM_LISTENER) ? 3 : 2);
		    }
		    /* make useless +0 rings much less common */
		    if (otmp->spe == 0) {
/*                     otmp->spe = rn2(4) - rn2(3); */
		       /* wow! +8! */
		       if (rn2(3)) otmp->spe = rne(2)+1;
		       else otmp->spe = -(rne(2)+1);
		    }
			if ((Race_if(PM_LISTENER) || RngeListening) && !Hallucination && (rnd(30) > ACURR(A_INT)) && (abs(otmp->spe) > 3 || (abs(otmp->spe) == 3 && rn2(2) ) || (abs(otmp->spe) == 2 && !rn2(3) )|| (abs(otmp->spe) == 1 && !rn2(5) ) ) ) pline("Precognition: made object with enchantment %d", abs(otmp->spe));

		    /* negative rings are usually cursed */
		    if (otmp->spe < 0 && rn2(5)) curse_on_creation(otmp);
		} else if(rn2(10) && (otmp->otyp == RIN_TELEPORTATION ||
			  otmp->otyp == RIN_POLYMORPH ||
			  otmp->otyp == RIN_AGGRAVATE_MONSTER ||
			  otmp->otyp == RIN_SLEEPING ||
			  otmp->otyp == RIN_DISARMING ||
			  otmp->otyp == RIN_NUMBNESS ||
			  otmp->otyp == RIN_CURSE ||
			  otmp->otyp == RIN_DIMNESS ||
			  otmp->otyp == RIN_HALLUCINATION ||
			  otmp->otyp == RIN_HUNGER || !rn2(9))) {
			curse_on_creation(otmp);
		}

		if (!rn2(1000)) otmp->oerodeproof = 1;
		if (!rn2(1000)) {
			if (!rn2(3)) otmp->oeroded = rnd(3);
			if (!rn2(3)) otmp->oeroded2 = rnd(3);
		}
		if (!rn2(500)) otmp->greased = rn2(5) ? 1 : rn2(3) ? 2 : 3;

		if (artif && (artif != 2) && !rn2(Race_if(PM_LISTENER) ? 100 : 50)) {
		    otmp = mk_artifact(otmp, (aligntyp)A_NONE, TRUE);
			if ((Race_if(PM_LISTENER) || RngeListening) && !Hallucination && (rnd(30) > ACURR(A_INT))) pline("Precognition: made artifact");
		}
		else if (artif && !rn2(125)) {
		    otmp = oname(otmp, !rn2(20) ? generate_garbage_string() : fauxartinames[rn2(SIZE(fauxartinames))] );
			otmp->fakeartifact = 1;
			if (artif != 2) u.fakeartifacts++;
		}

		break;
	case ROCK_CLASS:
		switch (otmp->otyp) {
		    case STATUE:
			/* possibly overridden by mkcorpstat() */
			otmp->corpsenm = rndmonnum();
			if ( (!verysmall(&mons[otmp->corpsenm]) || !rn2(10) ) && timebasedlowerchance() && (artif != 2) &&
				rn2(level_difficulty()/2 + 10) > 10)
			    (void) add_to_container(otmp,
						    mkobj(SPBOOK_CLASS,FALSE, FALSE));
		}
	      blessorcurse_on_creation(otmp, 7);

		if (!rn2(4000)) otmp->oerodeproof = 1;
		if (!rn2(4000)) {
			if (!rn2(3)) otmp->oeroded = rnd(3);
			if (!rn2(3)) otmp->oeroded2 = rnd(3);
		}
		if (!rn2(2000)) otmp->greased = rn2(5) ? 1 : rn2(3) ? 2 : 3;

		if (artif && (artif != 2) && !rn2(Race_if(PM_LISTENER) ? 400 : 200)) {
		    otmp = mk_artifact(otmp, (aligntyp)A_NONE, TRUE);
			if ((Race_if(PM_LISTENER) || RngeListening) && !Hallucination && (rnd(30) > ACURR(A_INT))) pline("Precognition: made artifact");
		}
		else if (artif && !rn2(500)) {
		    otmp = oname(otmp, !rn2(20) ? generate_garbage_string() : fauxartinames[rn2(SIZE(fauxartinames))] );
			otmp->fakeartifact = 1;
			if (artif != 2) u.fakeartifacts++;
		}

		break;
	case COIN_CLASS:
		break;	/* do nothing */
	default:
		impossible("impossible mkobj %ld, sym '%c'. Debug info for Amy: %d, %d, %d, %d, let %c", otmp->otyp, objects[otmp->otyp].oc_class, otyp, (int) init, artif, (int) shopinit, let);

		return (struct obj *)0;
	}

	if (iserosator && otmp && otmp->oclass != COIN_CLASS) {
		if (!rn2(3)) otmp->oeroded = rnd(3);
		else if (!rn2(2)) otmp->oeroded2 = rnd(3);
		else {
			otmp->oeroded = rnd(3);
			otmp->oeroded2 = rnd(3);
		}
		if (!rn2(100)) otmp->oeroded = rn2(4);
		if (!rn2(100)) otmp->oeroded2 = rn2(4);
	}

	/* nice patch idea by Amy - sometimes have items start out with partial identification */

	if (otmp && otmp->oclass != COIN_CLASS && !rn2(1000 - (u.ulevel * (ishaxor ? 20 : 10) ) )) otmp->rknown = TRUE;
	if (otmp && otmp->oclass != COIN_CLASS && !rn2(1000 - (u.ulevel * (ishaxor ? 20 : 10) ) )) otmp->known = TRUE;
	if (otmp && otmp->oclass != COIN_CLASS && !rn2(1000 - (u.ulevel * (ishaxor ? 20 : 10) ) )) otmp->dknown = TRUE;
	if (otmp && otmp->oclass != COIN_CLASS && !rn2(1000 - (u.ulevel * (ishaxor ? 20 : 10) ) )) otmp->bknown = TRUE;
	if (otmp && otmp->oclass != COIN_CLASS && (u.bucskill > rn2(300)) ) otmp->bknown = TRUE;
	if (otmp && otmp->oclass != COIN_CLASS && weapon_type(otmp) == P_NONE && (u.enchantrecskill > rn2(300)) ) otmp->known = TRUE;
	if (otmp && otmp->oclass != COIN_CLASS && weapon_type(otmp) != P_NONE && (u.weapchantrecskill > rn2(300)) ) otmp->known = TRUE;

	if (otmp && otmp->oclass != COIN_CLASS && (otmp->otyp == TIN || otmp->otyp == EGG) && !rn2(100 - (u.ulevel * (ishaxor ? 2 : 1) ) )) otmp->known = TRUE;

	/* For a curser, every item has an 80% chance of being cursed if it would have been noncursed. --Amy */
	if (iscurser && otmp && otmp->oclass != COIN_CLASS && rn2(5)) curse_on_creation(otmp);
	if (u.genericcursechance && otmp && otmp->oclass != COIN_CLASS && (u.genericcursechance >= rnd(100)) ) curse_on_creation(otmp);

	if (otyp == u.alwayscurseditem) curse_on_creation(otmp);
	if (otyp == u.alwayscurseditem2) curse_on_creation(otmp);
	if (otyp == u.alwayscurseditem3) curse_on_creation(otmp);
	if (otyp == u.alwayscurseditem4) curse_on_creation(otmp);
	if (otyp == u.alwayscurseditem5) curse_on_creation(otmp);
	if (otyp == u.alwayscurseditem6) curse_on_creation(otmp);
	if (otyp == u.alwayscurseditem7) curse_on_creation(otmp);
	if (otyp == u.alwayscurseditem8) curse_on_creation(otmp);
	if (otyp == u.alwayscurseditem9) curse_on_creation(otmp);
	if (otyp == u.alwayscurseditem10) curse_on_creation(otmp);

	/* Some things must get done (timers) even if init = 0 */
	switch (otmp->otyp) {
	    case CORPSE:
		start_corpse_timeout(otmp);
		break;
	}

	/* unique objects may have an associated artifact entry */
	if (objects[otyp].oc_unique && !otmp->oartifact)
	    otmp = mk_artifact(otmp, (aligntyp)A_NONE, FALSE);
	otmp->owt = weight(otmp);
	return(otmp);
}

/*
 * Start a corpse decay or revive timer.
 * This takes the age of the corpse into consideration as of 3.4.0.
 */
void
start_corpse_timeout(body)
	struct obj *body;
{
	struct monst *mtmp;
	long when; 		/* rot away when this old */
	long corpse_age;	/* age of corpse          */
	int rot_adjust;
	short action;

#define TAINT_AGE (50L)		/* age when corpses go bad */
#define TROLL_REVIVE_CHANCE 37	/* 1/37 chance for 50 turns ~ 75% chance */
#define TROLL_REVIVE_LATE_CHANCE 300	/* 1/300 chance for 200 turns ~ 49% chance */
#define MOLD_REVIVE_CHANCE 23	/*  1/23 chance for 50 turns ~ 90% chance */
#define MOLDY_CHANCE 900	/*  1/290 chance for 200 turns ~ 50% chance, but edited by Amy to be much more rare, actual chance about 20% now */
#define ROT_AGE (250L)		/* age when corpses rot away */

	/* lizards and lichen don't rot or revive */
	if ( nocorpsedecay(&mons[body->corpsenm]) ) return;

	action = ROT_CORPSE;		/* default action: rot away */
	rot_adjust = in_mklev ? 25 : 10;	/* give some variation */
	corpse_age = monstermoves - body->age;
	if (corpse_age > ROT_AGE)
		when = rot_adjust;
	else
		when = ROT_AGE - corpse_age;
	when += (long)(rnz(rot_adjust) - rot_adjust);

	if (is_rider(&mons[body->corpsenm])) {
		/*
		 * Riders always revive.  They have a 1/3 chance per turn
		 * of reviving after 12 turns.  Always revive by 500.
		 */
		action = REVIVE_MON;
		for (when = 12L; when < 500L; when++)
		    if (!rn2(3)) break;

	} else if ((mons[body->corpsenm].mlet == S_TROLL && !body->norevive) || is_deadlysin(&mons[body->corpsenm]) )  {
		long age;
		for (age = TAINT_AGE + 1; age <= ROT_AGE; age++)
		    if (!rn2(TROLL_REVIVE_LATE_CHANCE) && !(uwep && uwep->oartifact == ART_ZOMBIEBANE) ) {	/* troll revives */
			action = REVIVE_MON;
			when = age;
			break;
		    }
		for (age = 2; age <= TAINT_AGE; age++)
		    if (!rn2(TROLL_REVIVE_CHANCE) && !(uwep && uwep->oartifact == ART_ZOMBIEBANE)) {	/* troll revives */
			action = REVIVE_MON;
			when = age;
			break;
		    }

		if (u.uprops[STARVATION_EFFECT].extrinsic || StarvationEffect || (uarmc && uarmc->oartifact == ART_FEMMY_FATALE) || have_starvationstone() ) {
			if (rn2(10) && !(uwep && uwep->oartifact == ART_ZOMBIEBANE)) {
				action = REVIVE_MON;
				when = 1;
			} else {
				action = ROT_CORPSE;
				when = 1;
			}
		}

	} else if (mons[body->corpsenm].mlet == S_FUNGUS || is_reviver(&mons[body->corpsenm]) ||
		((((mtmp = get_mtraits(body, FALSE)) != (struct monst *)0) ) && mtmp->egotype_troll)
		 ) {
		/* Fungi come back with a vengeance - if you don't eat it or
		 * destroy it,  any live cells will quickly use the dead ones
		 * as food and come back.
		 */
		long age;

		for (age = 2; age <= TAINT_AGE; age++)
		    if ((mons[body->corpsenm].mlet == S_FUNGUS) && !rn2(MOLD_REVIVE_CHANCE) && !(uwep && uwep->oartifact == ART_ZOMBIEBANE)) {    /* mold revives */
			action = REVIVE_MON;
			when = age;
			break;
		    } else if (is_reviver(&mons[body->corpsenm]) && !(uwep && uwep->oartifact == ART_ZOMBIEBANE) && !rn2(MOLD_REVIVE_CHANCE)) {
			action = REVIVE_MON;
			when = age;
			break;
		    } else if (((((mtmp = get_mtraits(body, FALSE)) != (struct monst *)0) ) && mtmp->egotype_troll && !(uwep && uwep->oartifact == ART_ZOMBIEBANE)) && !rn2(MOLD_REVIVE_CHANCE)) {
			action = REVIVE_MON;
			when = age;
			break;
		    }

		if (u.uprops[STARVATION_EFFECT].extrinsic || StarvationEffect || (uarmc && uarmc->oartifact == ART_FEMMY_FATALE) || have_starvationstone() ) {
			if (rn2(10) && !(uwep && uwep->oartifact == ART_ZOMBIEBANE)) {
				action = REVIVE_MON;
				when = 1;
			} else {
				action = ROT_CORPSE;
				when = 1;
			}
		}

	} 
	
	if (action == ROT_CORPSE && !acidic(&mons[body->corpsenm])) {
		/* Corpses get moldy
		 */
		long age;
		for (age = TAINT_AGE + 1; age <= ROT_AGE; age++)
		    if (!rn2(MOLDY_CHANCE)) {    /* "revives" as a random s_fungus */
			action = ((!rn2(100) && !(uwep && uwep->oartifact == ART_ZOMBIEBANE)) ? REVIVE_MON : MOLDY_CORPSE); /* or rarely the monster revives itself --Amy */
			when = age;
			break;
		    }
	}
	
	if (body->norevive) body->norevive = 0;
	(void) start_timer(when, TIMER_OBJECT, action, (void *)body);
}

void
bless(otmp)
register struct obj *otmp;
{
#ifdef GOLDOBJ
	if (otmp->oclass == COIN_CLASS) return;
#endif
	if ((otmp->morgcurse || otmp->evilcurse || otmp->bbrcurse) && !rn2(100) ) {
		otmp->morgcurse = otmp->evilcurse = otmp->bbrcurse = otmp->prmcurse = otmp->hvycurse = otmp->cursed = otmp->stckcurse = 0;
	}
	else if (otmp->prmcurse && !(otmp->morgcurse || otmp->evilcurse || otmp->bbrcurse) && !rn2(10) ) {
		otmp->morgcurse = otmp->evilcurse = otmp->bbrcurse = otmp->prmcurse = otmp->hvycurse = otmp->cursed = otmp->stckcurse = 0;
	}
	else if (!(otmp->prmcurse) && !(otmp->morgcurse || otmp->evilcurse || otmp->bbrcurse) && otmp->hvycurse && !rn2(3) ) {
		otmp->morgcurse = otmp->evilcurse = otmp->bbrcurse = otmp->prmcurse = otmp->hvycurse = otmp->cursed = otmp->stckcurse = 0;
	}
	else if (!(otmp->prmcurse) && !(otmp->hvycurse) && !(otmp->morgcurse || otmp->evilcurse || otmp->bbrcurse) ) otmp->morgcurse = otmp->evilcurse = otmp->bbrcurse = otmp->prmcurse = otmp->hvycurse = otmp->cursed = otmp->stckcurse = 0;

	if (otmp->cursed == 0) {
	otmp->blessed = 1;
	if (carried(otmp) && confers_luck(otmp))
	    set_moreluck();
	if (otmp->otyp == HEALTHSTONE)
	    recalc_health();
	if (otmp->otyp == BAG_OF_HOLDING || otmp->otyp == ICE_BOX_OF_HOLDING || otmp->otyp == CHEST_OF_HOLDING)
	    otmp->owt = weight(otmp);
	if (otmp->otyp == FIGURINE && otmp->timed)
	    (void) stop_timer(FIG_TRANSFORM, (void *) otmp);
	}
	return;
}

void
unbless(otmp)
register struct obj *otmp;
{
	otmp->blessed = 0;
	if (carried(otmp) && confers_luck(otmp))
	    set_moreluck();
	if (otmp->otyp == HEALTHSTONE)
	    recalc_health();
	if (otmp->otyp == BAG_OF_HOLDING || otmp->otyp == ICE_BOX_OF_HOLDING || otmp->otyp == CHEST_OF_HOLDING)
	    otmp->owt = weight(otmp);
	if (otmp->otyp == FIGURINE && otmp->timed)
	    (void) stop_timer(FIG_TRANSFORM, (void *) otmp);
	return;
}

void
curse(otmp)
register struct obj *otmp;
{
#ifdef GOLDOBJ
	if (otmp->oclass == COIN_CLASS) return;
#endif
	otmp->blessed = 0;

	if (!rn2(isfriday ? 50 : 100)) otmp->stckcurse = 1;

	if (u.stickycursechance && (u.stickycursechance >= rnd(100)) ) otmp->stckcurse = 1;
	if (youmonst.data && Role_if(PM_CELLAR_CHILD) && !rn2(10)) otmp->stckcurse = 1;

	if (otmp->cursed) {
		if (!otmp->hvycurse && !rn2(5)) otmp->hvycurse = 1;
		else if (otmp->hvycurse && !otmp->prmcurse && !rn2(25)) otmp->prmcurse = 1;
		else if (otmp->prmcurse && !rn2(250)) {
			if (!rn2(3)) otmp->morgcurse = 1;
			else if (!rn2(2)) otmp->evilcurse = 1;
			else otmp->bbrcurse = 1;
		}
	} else {
		otmp->cursed = 1;
		if (!otmp->hvycurse && !otmp->prmcurse && !(otmp->morgcurse || otmp->evilcurse || otmp->bbrcurse) && !rn2(isfriday ? 20 : 35)) otmp->hvycurse = 1;
		if (otmp->hvycurse && !otmp->prmcurse && !(otmp->morgcurse || otmp->evilcurse || otmp->bbrcurse) && !rn2(isfriday ? 100 : 225)) otmp->prmcurse = 1;
		if (otmp->prmcurse && !rn2(isfriday ? 1000 : 6255)) {
			if (!rn2(3)) otmp->morgcurse = 1;
			else if (!rn2(2)) otmp->evilcurse = 1;
			else otmp->bbrcurse = 1;
		}
	}

	/* welded two-handed weapon interferes with some armor removal */
	if (otmp == uwep && bimanual(uwep)) reset_remarm();
	/* rules at top of wield.c state that twoweapon cannot be done
	   with cursed alternate weapon */
	if (otmp == uswapwep && u.twoweap && (PlayerCannotUseSkills || (P_SKILL(P_TWO_WEAPON_COMBAT) < P_EXPERT) ) )
	    drop_uswapwep();
	/* some cursed items need immediate updating */
	if (carried(otmp) && confers_luck(otmp))
	    set_moreluck();
	if (otmp->otyp == HEALTHSTONE)
	    recalc_health();
	if (otmp->otyp == BAG_OF_HOLDING || otmp->otyp == ICE_BOX_OF_HOLDING || otmp->otyp == CHEST_OF_HOLDING)
	    otmp->owt = weight(otmp);
	if (otmp->otyp == FIGURINE) {
		if (otmp->corpsenm != NON_PM
		    && !dead_species(otmp->corpsenm,TRUE)
		    && (carried(otmp) || mcarried(otmp)))
			attach_fig_transform_timeout(otmp);
	}
	return;
}

/* Special function for objects that get cursed on creation --Amy */
void
curse_on_creation(otmp)
register struct obj *otmp;
{
#ifdef GOLDOBJ
	if (otmp->oclass == COIN_CLASS) return;
#endif
	otmp->blessed = 0;

	if (!rn2(isfriday ? 50 : 100)) otmp->stckcurse = 1;

	if (u.stickycursechance && (u.stickycursechance >= rnd(100)) ) otmp->stckcurse = 1;
	if (youmonst.data && Role_if(PM_CELLAR_CHILD) && !rn2(10)) otmp->stckcurse = 1;

	if (otmp->cursed) { /* should not happen, but oh well */
		if (!otmp->hvycurse && !rn2(5)) otmp->hvycurse = 1;
		else if (otmp->hvycurse && !otmp->prmcurse && !rn2(25)) otmp->prmcurse = 1;
		else if (otmp->prmcurse && !rn2(250)) {
			if (!rn2(3)) otmp->morgcurse = 1;
			else if (!rn2(2)) otmp->evilcurse = 1;
			else otmp->bbrcurse = 1;
		}
	} else {
		otmp->cursed = 1;

		if (u.heavycursechance && (u.heavycursechance >= rnd(100)) ) otmp->hvycurse = 1;

		if (!u.dungeongrowthhack) {

			if (!rn2(10) && uarmc && itemhasappearance(uarmc, APP_CURSED_CALLED_CLOAK)) otmp->hvycurse = 1;

		}

		if (!otmp->hvycurse && !otmp->prmcurse && !(otmp->morgcurse || otmp->evilcurse || otmp->bbrcurse) && !rn2(Role_if(PM_CAMPERSTRIKER) ? 5 : isfriday ? 15 : 25)) otmp->hvycurse = 1;

		if (otmp->hvycurse && u.primecursechance && (u.primecursechance >= rnd(100)) ) otmp->prmcurse = 1;

		if (otmp->hvycurse && !otmp->prmcurse && !(otmp->morgcurse || otmp->evilcurse || otmp->bbrcurse) && !rn2(isfriday ? 50 : 125)) otmp->prmcurse = 1;
		if (otmp->prmcurse && !rn2(isfriday ? 500 : 1255)) {
			if (!rn2(3)) otmp->morgcurse = 1;
			else if (!rn2(2)) otmp->evilcurse = 1;
			else otmp->bbrcurse = 1;
		}
	}

	/* welded two-handed weapon interferes with some armor removal */
	if (otmp == uwep && bimanual(uwep)) reset_remarm();
	/* rules at top of wield.c state that twoweapon cannot be done
	   with cursed alternate weapon */
	if (otmp == uswapwep && u.twoweap && (PlayerCannotUseSkills || (P_SKILL(P_TWO_WEAPON_COMBAT) < P_EXPERT) ) )
	    drop_uswapwep();
	/* some cursed items need immediate updating */
	if (carried(otmp) && confers_luck(otmp))
	    set_moreluck();
	if (otmp->otyp == HEALTHSTONE)
	    recalc_health();
	if (otmp->otyp == BAG_OF_HOLDING || otmp->otyp == ICE_BOX_OF_HOLDING || otmp->otyp == CHEST_OF_HOLDING)
	    otmp->owt = weight(otmp);
	if (otmp->otyp == FIGURINE) {
		if (otmp->corpsenm != NON_PM
		    && !dead_species(otmp->corpsenm,TRUE)
		    && (carried(otmp) || mcarried(otmp)))
			attach_fig_transform_timeout(otmp);
	}
	return;
}

void
uncurse(otmp, guaranteed)
register struct obj *otmp;
boolean guaranteed; /* can it work even when you have permacurse nastytrap effect? (can still fail if heavily cursed --Amy */
{
	if (!guaranteed && (PermacurseEffect || u.uprops[PERMACURSE_EFFECT].extrinsic || have_permacursestone()) ) return;

	if ((otmp->morgcurse || otmp->evilcurse || otmp->bbrcurse) && !rn2(100) ) {
		otmp->morgcurse = otmp->evilcurse = otmp->bbrcurse = otmp->prmcurse = otmp->hvycurse = otmp->cursed = otmp->stckcurse = 0;
	}
	else if (otmp->prmcurse && !(otmp->morgcurse || otmp->evilcurse || otmp->bbrcurse) && !rn2(10) ) {
		otmp->morgcurse = otmp->evilcurse = otmp->bbrcurse = otmp->prmcurse = otmp->hvycurse = otmp->cursed = otmp->stckcurse = 0;
	}
	else if (!(otmp->prmcurse) && !(otmp->morgcurse || otmp->evilcurse || otmp->bbrcurse) && otmp->hvycurse && !rn2(3) ) {
		otmp->morgcurse = otmp->evilcurse = otmp->bbrcurse = otmp->prmcurse = otmp->hvycurse = otmp->cursed = otmp->stckcurse = 0;
	}
	else if (!(otmp->prmcurse) && !otmp->hvycurse && !(otmp->morgcurse || otmp->evilcurse || otmp->bbrcurse) ) otmp->morgcurse = otmp->evilcurse = otmp->bbrcurse = otmp->prmcurse = otmp->hvycurse = otmp->cursed = otmp->stckcurse = 0;
	if (carried(otmp) && confers_luck(otmp))
	    set_moreluck();
	/* KMH, balance patch -- healthstones affect healing */
	if (otmp->otyp == HEALTHSTONE)
	    recalc_health();
	if (otmp->otyp == BAG_OF_HOLDING || otmp->otyp == ICE_BOX_OF_HOLDING || otmp->otyp == CHEST_OF_HOLDING)
	    otmp->owt = weight(otmp);
}

#endif /* OVLB */
#ifdef OVL1

void
blessorcurse(otmp, chance)
register struct obj *otmp;
register int chance;
{
	if(otmp->blessed || otmp->cursed) return;

	if(!rn2(chance)) {
	    if(!rn2(isfriday ? 2 : 3)) {
		curse(otmp);
	    } else {
		bless(otmp);
	    }
	} else if (Role_if(PM_CAMPERSTRIKER) && !rn2(chance)) curse(otmp);

	return;
}

void
blessorcurse_on_creation(otmp, chance)
register struct obj *otmp;
register int chance;
{
	if(otmp->blessed || otmp->cursed) return;

	if(!rn2(chance)) {
	    if(!rn2(isfriday ? 2 : 3)) {
		curse_on_creation(otmp);
	    } else if (!u.dungeongrowthhack) {
			if (!rn2(5) && uarmc && itemhasappearance(uarmc, APP_CURSED_CALLED_CLOAK)) {
				curse_on_creation(otmp);
			} else {
				bless(otmp);
			}
	    } else {
		bless(otmp);
	    }
	} else if (Role_if(PM_CAMPERSTRIKER) && !rn2(chance)) curse_on_creation(otmp);

	return;
}

#endif /* OVL1 */
#ifdef OVLB

int
bcsign(otmp)
register struct obj *otmp;
{
	return(!!otmp->blessed - !!otmp->cursed);
}

#endif /* OVLB */
#ifdef OVL0

/*
 *  Calculate the weight of the given object.  This will recursively follow
 *  and calculate the weight of any containers.
 *
 *  Note:  It is possible to end up with an incorrect weight if some part
 *	   of the code messes with a contained object and doesn't update the
 *	   container's weight.
 */
int
weight(obj)
register struct obj *obj;
{
	int wt = objects[obj->otyp].oc_weight;

	if (obj->otyp == LARGE_BOX && obj->spe == 1) /* Schroedinger's Cat */
		wt += mons[PM_HOUSECAT].cwt;
	if (Is_container(obj) || obj->otyp == STATUE) {
		struct obj *contents;
		register int cwt = 0;

		if (obj->otyp == STATUE && obj->corpsenm >= LOW_PM)
		    wt = (int)obj->quan *
			 ((int)mons[obj->corpsenm].cwt * 3 / 2);

		for(contents=obj->cobj; contents; contents=contents->nobj)
			cwt += weight(contents);
		/* KMH -- support artifact BoH (including the Wallet of Perseus)
		 *
		 *  The weight of bags of holding is calculated as the weight
		 *  of the bag plus the weight of the bag's contents modified
		 *  as follows:
		 *
		 *      Bag status    Ordinary    Artifact
		 *      ----------    --------    --------
		 *      cursed          2x           4x
		 *      blessed       (x + 3)/4   (x + 5)/6
		 *      otherwise     (x + 1)/2   (x + 2)/3
		 *
		 *  The macro DELTA_CWT in pickup.c also implements these
		 *  weight equations.
		 *
		 *  Note:  The above checks are performed in the given order.
		 *	   this means that if an object is both blessed and
		 *	   cursed (not supposed to happen), it will be treated
		 *	   as cursed.
		 */
#define CEILDIV(x,y)	(((x)+(y)-1)/(y))	/* ceil(x/y) */
		if (obj->otyp == BAG_OF_HOLDING || obj->otyp == ICE_BOX_OF_HOLDING || obj->otyp == CHEST_OF_HOLDING)
			cwt = obj->cursed ? (cwt * (obj->oartifact ? 4 : 2)) :
				CEILDIV(cwt, (obj->oartifact ? 3 : 2) * (obj->blessed ? 2 : 1));
		if (obj->otyp == HANDYBAG) cwt = (flags.female ? (cwt * 4 / 5) : cwt );
#undef CEILDIV
		return wt + cwt;
	}
	if (obj->otyp == CORPSE && obj->corpsenm >= LOW_PM) {
		long long_wt = (int)obj->quan * mons[obj->corpsenm].cwt;

		wt = (long_wt > LARGEST_INT) ? LARGEST_INT : (int)long_wt;
		if (obj->oeaten) wt = eaten_stat(wt, obj);
		return wt;
	} else if (obj->oclass == FOOD_CLASS && obj->oeaten) {
		return eaten_stat((int)obj->quan * wt, obj);
	} else if (obj->oclass == COIN_CLASS)
		return (int)((obj->quan + 50L) / /*100*/10000L); /* gold weight fix --Amy */
	else if (obj->otyp == HEAVY_IRON_BALL && obj->owt != 0)
		return((int)(obj->owt));	/* kludge for "very" heavy iron ball */
	return(wt ? wt*(int)obj->quan : ((int)obj->quan + 1)>>5);
}

static int treefruits[] = {APPLE,ORANGE,PEAR,BANANA,EUCALYPTUS_LEAF,LEMON,CHERRY};
static int treefruitsX[] = {APPLE,ORANGE,PEAR,BANANA,EUCALYPTUS_LEAF,CHERRY,ASIAN_PEAR,LEMON,MELON,SLIME_MOLD,LUMP_OF_ROYAL_JELLY,WATERMELON,WHITE_PEACH,MAGIC_BANANA};
/* rare fruits added by Amy, of course they don't grow in Soviet Russia, it's too cold there because the type of ice block
 * brings winter - he's like "Väterchen Frost" :-P */

struct obj *
rnd_treefruit_at(x,y)
int x, y;
{
	if (rn2(200) || issoviet) return mksobj_at(treefruits[rn2(SIZE(treefruits))], x, y, TRUE, FALSE, FALSE);
	else return mksobj_at(treefruitsX[rn2(SIZE(treefruitsX))], x, y, TRUE, FALSE, FALSE);
}
#endif /* OVL0 */
#ifdef OVLB

struct obj *
mkgold(amount, x, y)
long amount;
int x, y;
{
    register struct obj *gold = g_at(x,y);

    if (amount <= 0L) {
	amount = (long)(1 + rnd(level_difficulty()+2) * rnd(10));
	if (!rn2(2)) amount += (long)(1 + rnd(level_difficulty()+2) * rnd(10));

	if (Race_if(PM_VENTURE_CAPITALIST)) {	/* they get extra money, idea by deepy */

		if (rn2(2)) amount *= 2;
		if (!rn2(5)) amount *= 3;
		if (!rn2(20)) amount *= 5;
		if (!rn2(200)) amount *= 10;
		if (!rn2(1000)) amount *= 20;
		if (!rn2(5000)) amount *= 50;
		if (!rn2(25000)) amount *= 100;
	}

	if (uarmh && uarmh->oartifact == ART_GOLD_STANDARD) amount *= 2;
	if (uarmg && uarmg->oartifact == ART_ROBBERY_GONE_RIGHT) amount *= 3;

	if (RngeWealth) amount *= (1 + rnd(2)); /* 2.5 times as much gold on average --Amy */
	if (uarmf && uarmf->oartifact == ART_PRACTICLASSY) amount *= 2;
    }

    if (gold) {
	gold->quan += amount;
    } else {
	gold = mksobj_at(GOLD_PIECE, x, y, TRUE, FALSE, FALSE);
	gold->quan = amount;
    }
    gold->owt = weight(gold);
    return (gold);
}

#endif /* OVLB */
#ifdef OVL1

/* return TRUE if the corpse has special timing */
/* special timing is a timing that is not rotting or molding */

#define special_corpse(num)  (((num) == PM_KATNISS)		\
				|| (nocorpsedecay(&mons[num]))	\
				|| (is_rider(&mons[num]))	\
				|| (is_deadlysin(&mons[num]))	\
				|| (is_reviver(&mons[num]))	\
				|| (mons[num].mlet == S_FUNGUS) \
				|| (mons[num].mlet == S_TROLL))

/*
 * OEXTRA note: Passing mtmp causes mtraits to be saved
 * even if ptr passed as well, but ptr is always used for
 * the corpse type (corpsenm). That allows the corpse type
 * to be different from the original monster,
 *	i.e.  vampire -> human corpse
 * yet still allow restoration of the original monster upon
 * resurrection.
 */
struct obj *
mkcorpstat(objtype, mtmp, ptr, x, y, init)
int objtype;	/* CORPSE or STATUE */
struct monst *mtmp;
struct permonst *ptr;
int x, y;
boolean init;
{
	register struct obj *otmp;

	/* hack by Amy: can't easily dispose of riders or sins by genociding corpses! */
	if (mtmp && (is_rider(mtmp->data) || is_deadlysin(mtmp->data)) && objtype == CORPSE) u.riderhack = TRUE;

	if (objtype != CORPSE && objtype != STATUE)
	    impossible("making corpstat type %d", objtype);
	if (x == 0 && y == 0) {		/* special case - random placement */
		otmp = mksobj(objtype, init, FALSE, FALSE);
		if (otmp) rloco(otmp);
	} else
		otmp = mksobj_at(objtype, x, y, init, FALSE, FALSE);

	u.riderhack = FALSE;

	if (otmp) {
	    if (mtmp) {
		struct obj *otmp2;

		if (!ptr) ptr = mtmp->data;
		/* save_mtraits frees original data pointed to by otmp */
		otmp2 = save_mtraits(otmp, mtmp);
		if (otmp2) otmp = otmp2;
	    }
	    /* use the corpse or statue produced by mksobj() as-is
	       unless `ptr' is non-null */
	    if (ptr) {
		int old_corpsenm = otmp->corpsenm;

		otmp->corpsenm = monsndx(ptr);
		otmp->owt = weight(otmp);
		if (otmp->otyp == CORPSE &&
			(special_corpse(old_corpsenm) || (mtmp && mtmp->egotype_troll) ||
				special_corpse(otmp->corpsenm))) {
		    obj_stop_timers(otmp);
		    start_corpse_timeout(otmp);
		}
	    }
	}
	return(otmp);
}

/*
 * Attach a monster id to an object, to provide
 * a lasting association between the two.
 */
struct obj *
obj_attach_mid(obj, mid)
struct obj *obj;
unsigned mid;
{
    struct obj *otmp;
    int lth, namelth;

    if (!mid || !obj) return (struct obj *)0;
    lth = sizeof(mid);
    namelth = obj->onamelth ? strlen(ONAME(obj)) + 1 : 0;
    if (namelth) 
	otmp = realloc_obj(obj, lth, (void *) &mid, namelth, ONAME(obj));
    else {
	otmp = obj;
	otmp->oxlth = sizeof(mid);
	(void) memcpy((void *)otmp->oextra, (void *)&mid,
								sizeof(mid));
    }
    if (otmp && otmp->oxlth) otmp->oattached = OATTACHED_M_ID;	/* mark it */
    return otmp;
}

static struct obj *
save_mtraits(obj, mtmp)
struct obj *obj;
struct monst *mtmp;
{
	struct obj *otmp;
	int lth, namelth;

	lth = sizeof(struct monst) + mtmp->mxlth + mtmp->mnamelth;
	namelth = obj->onamelth ? strlen(ONAME(obj)) + 1 : 0;
	otmp = realloc_obj(obj, lth, (void *) mtmp, namelth, ONAME(obj));
	if (otmp && otmp->oxlth) {
		struct monst *mtmp2 = (struct monst *)otmp->oextra;
		if (mtmp->data) mtmp2->mnum = monsndx(mtmp->data);
		/* invalidate pointers */
		/* m_id is needed to know if this is a revived quest leader */
		/* but m_id must be cleared when loading bones */
		mtmp2->nmon     = (struct monst *)0;
		mtmp2->data     = (struct permonst *)0;
		mtmp2->minvent  = (struct obj *)0;
		otmp->oattached = OATTACHED_MONST;	/* mark it */
	}
	return otmp;
}

/* returns a pointer to a new monst structure based on
 * the one contained within the obj.
 */
struct monst *
get_mtraits(obj, copyof)
struct obj *obj;
boolean copyof;
{
	struct monst *mtmp = (struct monst *)0;
	struct monst *mnew = (struct monst *)0;

	if (obj->oxlth && obj->oattached == OATTACHED_MONST)
		mtmp = (struct monst *)obj->oextra;
	if (mtmp) {
	    if (copyof) {
		int lth = mtmp->mxlth + mtmp->mnamelth;
		mnew = newmonst(lth);
		lth += sizeof(struct monst);
		(void) memcpy((void *)mnew,
				(void *)mtmp, lth);
	    } else {
	      /* Never insert this returned pointer into mon chains! */
	    	mnew = mtmp;
	    }
}
	return mnew;
}

#endif /* OVL1 */
#ifdef OVLB

/* make an object named after someone listed in the scoreboard file */
struct obj *
mk_tt_object(objtype, x, y)
int objtype; /* CORPSE or STATUE */
register int x, y;
{
	register struct obj *otmp, *otmp2;
	boolean initialize_it;

	/* player statues never contain books */
	initialize_it = (objtype != STATUE);
	if ((otmp = mksobj_at(objtype, x, y, initialize_it, FALSE, FALSE)) != 0) {
	    /* tt_oname will return null if the scoreboard is empty */
	    if ((otmp2 = tt_oname(otmp)) != 0) otmp = otmp2;
	}
	return(otmp);
}

/* make a new corpse or statue, uninitialized if a statue (i.e. no books) */
struct obj *
mk_named_object(objtype, ptr, x, y, nm)
int objtype;	/* CORPSE or STATUE */
struct permonst *ptr;
int x, y;
const char *nm;
{
	struct obj *otmp;

	otmp = mkcorpstat(objtype, (struct monst *)0, ptr,
				x, y, (boolean)(objtype != STATUE));
	if (nm)
		otmp = oname(otmp, nm);
	return(otmp);
}

/* Amy grepping target: "materialeffect" */
boolean
is_flammable(otmp)
register struct obj *otmp;
{
	int otyp = otmp->otyp;
	int omat = objects[otyp].oc_material;

	if (uamul && uamul->oartifact == ART_AUTOMATICALLY_METAL) {
		return (!(is_metallic(otmp)));
	}

	if (objects[otyp].oc_oprop == FIRE_RES || otyp == WAN_FIRE)
		return FALSE;

	if (objects[otyp].oc_material == MT_PLASTIC) return TRUE;
	if (objects[otyp].oc_material == MT_VIVA) return TRUE;
	if (objects[otyp].oc_material == MT_SILK) return TRUE;
	if (objects[otyp].oc_material == MT_ARCANIUM) return TRUE;
	if (objects[otyp].oc_material == MT_POURPOOR) return TRUE;
	if (objects[otyp].oc_material == MT_ETHER) return TRUE;
	if (objects[otyp].oc_material == MT_BRICK) return TRUE;
	if (objects[otyp].oc_material == MT_NANOMACHINE) return TRUE;
	if (objects[otyp].oc_material == MT_FOAM) return TRUE;
	if (objects[otyp].oc_material == MT_TINSEL) return TRUE;
	if (objects[otyp].oc_material == MT_AMBER) return TRUE;
	if (objects[otyp].oc_material == MT_DRAGON_HIDE && evilfriday) return TRUE;
	if (objects[otyp].oc_material == MT_COMPOST && evilfriday) return TRUE;
	if (objects[otyp].oc_material == MT_INKA && evilfriday) return TRUE;
	if (objects[otyp].oc_material == MT_SHADOWSTUFF && evilfriday) return TRUE;
	if (objects[otyp].oc_material == MT_OBSIDIAN && evilfriday) return TRUE;
	if (objects[otyp].oc_material == MT_LEAD && evilfriday) return TRUE;
	if (objects[otyp].oc_material == MT_CHROME && evilfriday) return TRUE;
	if (objects[otyp].oc_material == MT_CERAMIC && evilfriday) return TRUE;
	if (objects[otyp].oc_material == MT_CELESTIUM && evilfriday) return TRUE;
	if (objects[otyp].oc_material == MT_PWN_BUBBLE && evilfriday) return TRUE;

	if (omat <= MT_WOOD && omat != MT_LIQUID) return TRUE;
	return FALSE;

}

boolean
is_rottable(otmp)
register struct obj *otmp;
{
	int otyp = otmp->otyp;
	int omat = objects[otyp].oc_material;

	if (uamul && uamul->oartifact == ART_AUTOMATICALLY_METAL) {
		return (!(is_metallic(otmp)));
	}

	if (objects[otyp].oc_material == MT_INKA) return TRUE;
	if (objects[otyp].oc_material == MT_ARCANIUM) return TRUE;
	if (objects[otyp].oc_material == MT_SECREE) return TRUE;
	if (objects[otyp].oc_material == MT_COMPOST) return TRUE;
	if (objects[otyp].oc_material == MT_BRICK) return TRUE;
	if (objects[otyp].oc_material == MT_SAND) return TRUE;
	if (objects[otyp].oc_material == MT_NANOMACHINE) return TRUE;
	if (objects[otyp].oc_material == MT_ANTIDOTIUM) return TRUE;
	if (objects[otyp].oc_material == MT_FOAM) return TRUE;
	if (objects[otyp].oc_material == MT_SCRAP) return TRUE;
	if (objects[otyp].oc_material == MT_AMBER) return TRUE;
	if (objects[otyp].oc_material == MT_BONE && evilfriday) return TRUE;
	if (objects[otyp].oc_material == MT_DRAGON_HIDE && evilfriday) return TRUE;
	if (objects[otyp].oc_material == MT_SHADOWSTUFF && evilfriday) return TRUE;
	if (objects[otyp].oc_material == MT_CELESTIUM && evilfriday) return TRUE;

	if (omat <= MT_WOOD && omat != MT_LIQUID) return TRUE;
	return FALSE;
}

boolean
is_rustprone(otmp)
register struct obj *otmp;
{
	int otyp = otmp->otyp;

	if (SpellColorMetal) {
		return (!(is_metallic(otmp)));
	}

	if (objects[otyp].oc_material == MT_IRON) return TRUE;
	if (objects[otyp].oc_material == MT_SCRAP) return TRUE;
	if (objects[otyp].oc_material == MT_ALLOY) return TRUE;
	if (objects[otyp].oc_material == MT_INKA) return TRUE;
	if (objects[otyp].oc_material == MT_ARCANIUM) return TRUE;
	if (objects[otyp].oc_material == MT_POURPOOR) return TRUE;
	if (objects[otyp].oc_material == MT_ETERNIUM) return TRUE;
	if (objects[otyp].oc_material == MT_ETHER) return TRUE;
	if (objects[otyp].oc_material == MT_BRICK) return TRUE;
	if (objects[otyp].oc_material == MT_TINSEL) return TRUE;
	if (objects[otyp].oc_material == MT_SAND && evilfriday) return TRUE;
	if (objects[otyp].oc_material == MT_NANOMACHINE && evilfriday) return TRUE;

	return FALSE;
}

boolean
is_corrodeable(otmp)
register struct obj *otmp;
{
	int otyp = otmp->otyp;

	if (uamul && uamul->oartifact == ART_AUTOMATICALLY_METAL) {
		return (!(is_metallic(otmp)));
	}

	if (objects[otyp].oc_material == MT_COPPER) return TRUE;
	if (objects[otyp].oc_material == MT_IRON) return TRUE;
	if (objects[otyp].oc_material == MT_VIVA) return TRUE;
	if (objects[otyp].oc_material == MT_TAR) return TRUE;
	if (objects[otyp].oc_material == MT_ARCANIUM) return TRUE;
	if (objects[otyp].oc_material == MT_SECREE) return TRUE;
	if (objects[otyp].oc_material == MT_POURPOOR) return TRUE;
	if (objects[otyp].oc_material == MT_ETERNIUM) return TRUE;
	if (objects[otyp].oc_material == MT_BRICK) return TRUE;
	if (objects[otyp].oc_material == MT_CHROME) return TRUE;
	if (objects[otyp].oc_material == MT_FOAM) return TRUE;
	if (objects[otyp].oc_material == MT_SCRAP) return TRUE;
	if (objects[otyp].oc_material == MT_ALLOY) return TRUE;
	if (objects[otyp].oc_material == MT_TINSEL) return TRUE;
	if (objects[otyp].oc_material == MT_LEAD && evilfriday) return TRUE;
	if (objects[otyp].oc_material == MT_METAL && evilfriday) return TRUE;
	if (objects[otyp].oc_material == MT_SILVER && evilfriday) return TRUE;
	if (objects[otyp].oc_material == MT_PLATINUM && evilfriday) return TRUE;
	if (objects[otyp].oc_material == MT_SAND && evilfriday) return TRUE;
	if (objects[otyp].oc_material == MT_OBSIDIAN && evilfriday) return TRUE;
	if (objects[otyp].oc_material == MT_NANOMACHINE && evilfriday) return TRUE;
	if (objects[otyp].oc_material == MT_CONUNDRUM && evilfriday) return TRUE;
	if (objects[otyp].oc_material == MT_AMBER && evilfriday) return TRUE;

	return FALSE;
}

#endif /* OVLB */
#ifdef OVL1

/*
 * These routines maintain the single-linked lists headed in level.objects[][]
 * and threaded through the nexthere fields in the object-instance structure.
 */

/* put the object at the given location */
void
place_object(otmp, x, y)
register struct obj *otmp;
int x, y;
{
    register struct obj *otmp2 = level.objects[x][y];

    if (otmp->where != OBJ_FREE)
	panic("place_object: obj not free");

    obj_no_longer_held(otmp);
    if (otmp->otyp == BOULDER) block_point(x,y);	/* vision */

    /* obj goes under boulders */
    if (otmp2 && (otmp2->otyp == BOULDER)) {
	otmp->nexthere = otmp2->nexthere;
	otmp2->nexthere = otmp;
    } else {
	otmp->nexthere = otmp2;
	level.objects[x][y] = otmp;
    }

    /* set the new object's location */
    otmp->ox = x;
    otmp->oy = y;

    otmp->where = OBJ_FLOOR;

    /* add to floor chain */
    otmp->nobj = fobj;
    fobj = otmp;
    if (otmp->timed) obj_timer_checks(otmp, x, y, 0);
}

#define ON_ICE(a) ((a)->recharged)
#define ROT_ICE_ADJUSTMENT 2	/* rotting on ice takes 2 times as long */

/* If ice was affecting any objects correct that now
 * Also used for starting ice effects too. [zap.c]
 */
void
obj_ice_effects(x, y, do_buried)
int x, y;
boolean do_buried;
{
	struct obj *otmp;

	for (otmp = level.objects[x][y]; otmp; otmp = otmp->nexthere) {
		if (otmp->timed) obj_timer_checks(otmp, x, y, 0);
	}
	if (do_buried) {
	    for (otmp = level.buriedobjlist; otmp; otmp = otmp->nobj) {
 		if (otmp->ox == x && otmp->oy == y) {
			if (otmp->timed) obj_timer_checks(otmp, x, y, 0);
		}
	    }
	}
}

/*
 * Returns an obj->age for a corpse object on ice, that would be the
 * actual obj->age if the corpse had just been lifted from the ice.
 * This is useful when just using obj->age in a check or calculation because
 * rot timers pertaining to the object don't have to be stopped and
 * restarted etc.
 */
long
peek_at_iced_corpse_age(otmp)
struct obj *otmp;
{
    long age, retval = otmp->age;
    
    if (otmp->otyp == CORPSE && ON_ICE(otmp)) {
	/* Adjust the age; must be same as obj_timer_checks() for off ice*/
	age = monstermoves - otmp->age;
	retval = otmp->age + (age / ROT_ICE_ADJUSTMENT);
#ifdef DEBUG_EFFECTS
	pline_The("%s age has ice modifications:otmp->age = %ld, returning %ld.",
		s_suffix(doname(otmp)),otmp->age, retval);
	pline("Effective age of corpse: %ld.",
		monstermoves - retval);
#endif
    }
    return retval;
}

STATIC_OVL void
obj_timer_checks(otmp, x, y, force)
struct obj *otmp;
xchar x, y;
int force;	/* 0 = no force so do checks, <0 = force off, >0 force on */
{
    long tleft = 0L;
    short action = ROT_CORPSE;
    boolean restart_timer = FALSE;
    boolean on_floor = (otmp->where == OBJ_FLOOR);
    boolean buried = (otmp->where == OBJ_BURIED);

    /* Check for corpses just placed on or in ice */
    if (otmp->otyp == CORPSE && (on_floor || buried) && is_ice(x,y)) {
	tleft = stop_timer(action, (void *)otmp);
	if (tleft == 0L) {
	    action = MOLDY_CORPSE;
	    tleft = stop_timer(action, (void *)otmp);
	    if (tleft == 0L) {
		action = REVIVE_MON;
		tleft = stop_timer(action, (void *)otmp);
	} 
	} 
	if (tleft != 0L) {
	    long age;
	    
	    tleft = tleft - monstermoves;
	    /* mark the corpse as being on ice */
	    ON_ICE(otmp) = 1;
#ifdef DEBUG_EFFECTS
	    pline("%s is now on ice at %d,%d.", The(xname(otmp)),x,y);
#endif
	    /* Adjust the time remaining */
	    tleft *= ROT_ICE_ADJUSTMENT;
	    restart_timer = TRUE;
	    /* Adjust the age; must be same as in obj_ice_age() */
	    age = monstermoves - otmp->age;
	    otmp->age = monstermoves - (age * ROT_ICE_ADJUSTMENT);
	}
    }
    /* Check for corpses coming off ice */
    else if ((force < 0) ||
	     (otmp->otyp == CORPSE && ON_ICE(otmp) &&
	     ((on_floor && !is_ice(x,y)) || !on_floor))) {
	tleft = stop_timer(action, (void *)otmp);
	if (tleft == 0L) {
	    action = MOLDY_CORPSE;
	    tleft = stop_timer(action, (void *)otmp);
	    if (tleft == 0L) {
		action = REVIVE_MON;
		tleft = stop_timer(action, (void *)otmp);
	}
	}
	if (tleft != 0L) {
		long age;

		tleft = tleft - monstermoves;
		ON_ICE(otmp) = 0;
#ifdef DEBUG_EFFECTS
	    	pline("%s is no longer on ice at %d,%d.", The(xname(otmp)),x,y);
#endif
		/* Adjust the remaining time */
		tleft /= ROT_ICE_ADJUSTMENT;
		restart_timer = TRUE;
		/* Adjust the age */
		age = monstermoves - otmp->age;
		otmp->age = otmp->age + (age / ROT_ICE_ADJUSTMENT);
	}
    }
    /* now re-start the timer with the appropriate modifications */ 
    if (restart_timer)
	(void) start_timer(tleft, TIMER_OBJECT, action, (void *)otmp);
}

#undef ON_ICE
#undef ROT_ICE_ADJUSTMENT

void
remove_object(otmp)
register struct obj *otmp;
{
    xchar x = otmp->ox;
    xchar y = otmp->oy;

    if (otmp->where != OBJ_FLOOR)
	panic("remove_object: obj not on floor");
    if (otmp->otyp == BOULDER) unblock_point(x,y); /* vision */
    extract_nexthere(otmp, &level.objects[x][y]);
    extract_nobj(otmp, &fobj);
    if (otmp->timed) obj_timer_checks(otmp,x,y,0);
}

/* throw away all of a monster's inventory */
void
discard_minvent(mtmp)
struct monst *mtmp;
{
    struct obj *otmp, *curr;

    while (mtmp->minvent) {
	/* Move all contained objects out into the monster's main inventory
	 * so that we can easily check that every object (whether contained
	 * or not) does not evade destruction.
	 */
	while (Has_contents((otmp = mtmp->minvent))) {
	    curr = otmp->cobj;
	    obj_extract_self(curr);
	    (void) add_to_minv(mtmp, curr);
	}
	obj_extract_self(otmp);
	if (evades_destruction(otmp)) {
	    impossible("%s discarded from %s inventory",
	      obj_typename(otmp->otyp), s_suffix(mon_nam(mtmp)));
	    place_object(otmp, mtmp->mx, mtmp->my);
	    continue;
	}
	obfree(otmp, (struct obj *)0);	/* dealloc_obj() isn't sufficient */
    }
}

/*
 * Free obj from whatever list it is on in preperation of deleting it or
 * moving it elsewhere.  This will perform all high-level consequences
 * involved with removing the item.  E.g. if the object is in the hero's
 * inventory and confers heat resistance, the hero will lose it.
 *
 * Object positions:
 *	OBJ_FREE	not on any list
 *	OBJ_FLOOR	fobj, level.locations[][] chains (use remove_object)
 *	OBJ_CONTAINED	cobj chain of container object
 *	OBJ_INVENT	hero's invent chain (use freeinv)
 *	OBJ_MINVENT	monster's invent chain
 *	OBJ_MIGRATING	migrating chain
 *	OBJ_BURIED	level.buriedobjs chain
 *	OBJ_ONBILL	on billobjs chain
 */
void
obj_extract_self(obj)
    struct obj *obj;
{
    switch (obj->where) {
	case OBJ_FREE:
	    break;
	case OBJ_FLOOR:
	    remove_object(obj);
	    break;
	case OBJ_CONTAINED:
	    extract_nobj(obj, &obj->ocontainer->cobj);
	    container_weight(obj->ocontainer);
	    break;
	case OBJ_INVENT:
	    freeinv(obj);
	    break;
	case OBJ_MINVENT:
	    extract_nobj(obj, &obj->ocarry->minvent);
	    break;
	case OBJ_MIGRATING:
	    extract_nobj(obj, &migrating_objs);
	    break;
	case OBJ_BURIED:
	    extract_nobj(obj, &level.buriedobjlist);
	    break;
	case OBJ_ONBILL:
	    extract_nobj(obj, &billobjs);
	    break;
	default:
	    panic("obj_extract_self");
	    break;
    }
}

/* Extract a contained indestructable object (if one exists) and return it */
struct obj *
container_extract_indestructable(struct obj *obj)
{
    struct obj *otmp = obj->cobj, *indestructable = (struct obj *)0;
    while (!indestructable && otmp) {
	if (Has_contents(otmp))
	    indestructable = container_extract_indestructable(otmp);
	if (!indestructable && evades_destruction(otmp)) {
	    indestructable = otmp;
	    obj_extract_self(indestructable);
	}
	otmp = otmp->nobj;
    }
    return indestructable;
}

/* Extract the given object from the chain, following nobj chain. */
void
extract_nobj(obj, head_ptr)
    struct obj *obj, **head_ptr;
{
    struct obj *curr, *prev;

    curr = *head_ptr;
    for (prev = (struct obj *) 0; curr; prev = curr, curr = curr->nobj) {
	if (curr == obj) {
	    if (prev)
		prev->nobj = curr->nobj;
	    else
		*head_ptr = curr->nobj;
	    break;
	}
    }
    if (!curr) panic("extract_nobj: object lost");
    obj->where = OBJ_FREE;
}


/*
 * Extract the given object from the chain, following nexthere chain.
 *
 * This does not set obj->where, this function is expected to be called
 * in tandem with extract_nobj, which does set it.
 */
void
extract_nexthere(obj, head_ptr)
    struct obj *obj, **head_ptr;
{
    struct obj *curr, *prev;

    curr = *head_ptr;
    for (prev = (struct obj *) 0; curr; prev = curr, curr = curr->nexthere) {
	if (curr == obj) {
	    if (prev)
		prev->nexthere = curr->nexthere;
	    else
		*head_ptr = curr->nexthere;
	    break;
	}
    }
    if (!curr) panic("extract_nexthere: object lost");
}


/*
 * Add obj to mon's inventory.  If obj is able to merge with something already
 * in the inventory, then the passed obj is deleted and 1 is returned.
 * Otherwise 0 is returned.
 */
int
add_to_minv(mon, obj)
    struct monst *mon;
    struct obj *obj;
{
    struct obj *otmp;

    if (obj->where != OBJ_FREE)
	panic("add_to_minv: obj not free");

    /* merge if possible */
    for (otmp = mon->minvent; otmp; otmp = otmp->nobj)
	if (merged(&otmp, &obj))
	    return 1;	/* obj merged and then free'd */
    /* else insert; don't bother forcing it to end of chain */
    obj->where = OBJ_MINVENT;
    obj->ocarry = mon;
    obj->nobj = mon->minvent;
    mon->minvent = obj;
    return 0;	/* obj on mon's inventory chain */
}

/*
 * Add obj to container, make sure obj is "free".  Returns (merged) obj.
 * The input obj may be deleted in the process.
 */
struct obj *
add_to_container(container, obj)
    struct obj *container, *obj;
{
    struct obj *otmp;

    if (obj->where != OBJ_FREE)
	panic("add_to_container: obj not free");
    if (container->where != OBJ_INVENT && container->where != OBJ_MINVENT)
	obj_no_longer_held(obj);

    /* merge if possible */
    for (otmp = container->cobj; otmp; otmp = otmp->nobj)
	if (merged(&otmp, &obj)) return (otmp);

    obj->mstartinvent = 0;
    obj->mstartinventB = 0;
    obj->mstartinventC = 0;
    obj->mstartinventD = 0;
    obj->where = OBJ_CONTAINED;
    obj->ocontainer = container;
    obj->nobj = container->cobj;
    container->cobj = obj;
    return (obj);
}

void
add_to_migration(obj)
    struct obj *obj;
{
    if (obj->where != OBJ_FREE)
	panic("add_to_migration: obj not free");

    obj->where = OBJ_MIGRATING;
    obj->nobj = migrating_objs;
    migrating_objs = obj;
}

void
add_to_buried(obj)
    struct obj *obj;
{
    if (obj->where != OBJ_FREE)
	panic("add_to_buried: obj not free");

    obj->where = OBJ_BURIED;
    obj->nobj = level.buriedobjlist;
    level.buriedobjlist = obj;
}

/* Recalculate the weight of this container and all of _its_ containers. */
STATIC_OVL void
container_weight(container)
    struct obj *container;
{
    container->owt = weight(container);
    if (container->where == OBJ_CONTAINED)
	container_weight(container->ocontainer);
/*
    else if (container->where == OBJ_INVENT)
	recalculate load delay here ???
*/
}

/*
 * Deallocate the object.  _All_ objects should be run through here for
 * them to be deallocated.
 */
void
dealloc_obj(obj)
    struct obj *obj;
{
    if (obj->where != OBJ_FREE)
	panic("dealloc_obj: obj not free");

    /* free up any timers attached to the object */
    if (obj->timed)
	obj_stop_timers(obj);

    /*
     * Free up any light sources attached to the object.
     *
     * We may want to just call del_light_source() without any
     * checks (requires a code change there).  Otherwise this
     * list must track all objects that can have a light source
     * attached to it (and also requires lamplit to be set).
     */
    if (obj_sheds_light(obj))
	del_light_source(LS_OBJECT, (void *) obj);

    if (obj == thrownobj) thrownobj = (struct obj*)0;

    free((void *) obj);
}

#if defined(OBJ_SANITY) || defined(WIZARD)
# ifdef WIZARD
# define msgprefix	""
# else
# define msgprefix	"BUG (please report): "
# endif

/* Check all object lists for consistency. */
void
obj_sanity_check()
{
    int x, y;
    struct obj *obj;
    struct monst *mon;
    const char *mesg;
    char obj_address[20], mon_address[20];  /* room for formatted pointers */

    mesg = "fobj sanity";
    for (obj = fobj; obj; obj = obj->nobj) {
	if (obj->where != OBJ_FLOOR) {
	    pline("%s%s obj %s %s@(%d,%d): %s\n", msgprefix, mesg,
		fmt_ptr((void *)obj, obj_address),
		where_name(obj->where),
		obj->ox, obj->oy, doname(obj));
	}
	check_contained(obj, mesg);
    }

    mesg = "location sanity";
    for (x = 0; x < COLNO; x++)
	for (y = 0; y < ROWNO; y++)
	    for (obj = level.objects[x][y]; obj; obj = obj->nexthere)
		if (obj->where != OBJ_FLOOR) {
		    pline("%s%s obj %s %s@(%d,%d): %s\n", msgprefix, mesg,
			fmt_ptr((void *)obj, obj_address),
			where_name(obj->where),
			obj->ox, obj->oy, doname(obj));
		}

    mesg = "invent sanity";
    for (obj = invent; obj; obj = obj->nobj) {
	if (obj->where != OBJ_INVENT) {
	    pline("%s%s obj %s %s: %s\n", msgprefix, mesg,
		fmt_ptr((void *)obj, obj_address),
		where_name(obj->where), doname(obj));
	}
	check_contained(obj, mesg);
    }

    mesg = "migrating sanity";
    for (obj = migrating_objs; obj; obj = obj->nobj) {
	if (obj->where != OBJ_MIGRATING) {
	    pline("%s%s obj %s %s: %s\n", msgprefix, mesg,
		fmt_ptr((void *)obj, obj_address),
		where_name(obj->where), doname(obj));
	}
	check_contained(obj, mesg);
    }

    mesg = "buried sanity";
    for (obj = level.buriedobjlist; obj; obj = obj->nobj) {
	if (obj->where != OBJ_BURIED) {
	    pline("%s%s obj %s %s: %s\n", msgprefix, mesg,
		fmt_ptr((void *)obj, obj_address),
		where_name(obj->where), doname(obj));
	}
	check_contained(obj, mesg);
    }

    mesg = "bill sanity";
    for (obj = billobjs; obj; obj = obj->nobj) {
	if (obj->where != OBJ_ONBILL) {
	    pline("%s%s obj %s %s: %s\n", msgprefix, mesg,
		fmt_ptr((void *)obj, obj_address),
		where_name(obj->where), doname(obj));
	}
	/* shouldn't be a full container on the bill */
	if (obj->cobj) {
	    pline("%s%s obj %s contains %s! %s\n", msgprefix, mesg,
		fmt_ptr((void *)obj, obj_address),
		something, doname(obj));
	}
    }

    mesg = "minvent sanity";
    for (mon = fmon; mon; mon = mon->nmon)
	for (obj = mon->minvent; obj; obj = obj->nobj) {
	    if (obj->where != OBJ_MINVENT) {
		pline("%s%s obj %s %s: %s\n", msgprefix, mesg,
			fmt_ptr((void *)obj, obj_address),
			where_name(obj->where), doname(obj));
	    }
	    if (obj->ocarry != mon) {
		pline("%s%s obj %s (%s) not held by mon %s (%s)\n", msgprefix, mesg,
			fmt_ptr((void *)obj, obj_address),
			doname(obj),
			fmt_ptr((void *)mon, mon_address),
			mon_nam(mon));
	    }
	    check_contained(obj, mesg);
	}
}

/* This must stay consistent with the defines in obj.h. */
static const char *obj_state_names[NOBJ_STATES] = {
	"free",		"floor",	"contained",	"invent",
	"minvent",	"migrating",	"buried",	"onbill"
};

STATIC_OVL const char *
where_name(where)
    int where;
{
    return (where<0 || where>=NOBJ_STATES) ? "unknown" : obj_state_names[where];
}

/* obj sanity check: check objs contained by container */
STATIC_OVL void
check_contained(container, mesg)
    struct obj *container;
    const char *mesg;
{
    struct obj *obj;
    char obj1_address[20], obj2_address[20];

    for (obj = container->cobj; obj; obj = obj->nobj) {
	if (obj->where != OBJ_CONTAINED)
	    pline("%scontained %s obj %s: %s\n", msgprefix, mesg,
		fmt_ptr((void *)obj, obj1_address),
		where_name(obj->where));
	else if (obj->ocontainer != container)
	    pline("%s%s obj %s not in container %s\n", msgprefix, mesg,
		fmt_ptr((void *)obj, obj1_address),
		fmt_ptr((void *)container, obj2_address));
    }
}
#endif /* OBJ_SANITY || WIZARD */

#endif /* OVL1 */

/*mkobj.c*/
