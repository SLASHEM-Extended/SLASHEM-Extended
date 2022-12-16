/*	SCCS Id: @(#)shknam.c	3.4	2003/01/09	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/* shknam.c -- initialize a shop */

#include "hack.h"
#include "eshk.h"

#ifndef OVLB
extern const struct shclass shtypes[];

#else

STATIC_DCL void mkshobj_at(const struct shclass *,int,int,BOOLEAN_P);
STATIC_DCL void nameshk(struct monst *,const char * const *);
STATIC_DCL int  shkinit(const struct shclass *,struct mkroom *);

STATIC_DCL void stock_blkmar(const struct shclass *, struct mkroom *, int);

/* WAC init shk services */
static void init_shk_services(struct monst *);


static const char * const shkliquors[] = {
    /* Ukraine */
    "Njezjin", "Tsjernigof", "Ossipewsk", "Gorlowka",
    /* Belarus */
    "Gomel",
    /* N. Russia */
    "Konosja", "Weliki Oestjoeg", "Syktywkar", "Sablja",
    "Narodnaja", "Kyzyl",
    /* Silezie */
    "Walbrzych", "Swidnica", "Klodzko", "Raciborz", "Gliwice",
    "Brzeg", "Krnov", "Hradec Kralove",
    /* Schweiz */
    "Leuk", "Brig", "Brienz", "Thun", "Sarnen", "Burglen", "Elm",
    "Flims", "Vals", "Schuls", "Zum Loch",
    /* Mythology */
	"Aganippe", "Appias", "Bolbe", "Limnaee", "Pallas", "Tritonis",
	"Abarbarea", "Aegina", "Asopis", "Chalcis", "Cleone", "Combe",
	"Corcyra", "Euboea", "Gargaphia", "Harpina", "Ismene", "Nemea",
	"Ornea", "Peirene", "Salamis", "Sinope", "Tanagra", "Thebe",
	"Thespeia",

	"Schwiederloch", "Shol", "Pfahlhirsch", "Spendthrift",
	"Heartfont", "Chansina", "Delilah", "Marihuana", "Boeing",
	"Kovvelkov", "Jeler", "Hesser", "Rutte", "Meir", "Suchanek",
	"Jannter", "Erdinger", "Warsteiner", "Grolsch", "Jack Daniels",
	"Champagner", "Surilie", "Tamika", "Ga'er", "Tsedong",
	"Moonjava", "Wollfahrt", "Andrews", "Browning", "Craven",
	"Englefitch", "Gainsforth", "Hawkins", "Killingbeck", "Litton",
	"Nebbish", "Rawlins", "Shingleton", "Waldegrove", "Zed",
    0
};

static const char * const shkbooks[] = {
    /* Eire */
    "Skibbereen", "Kanturk", "Rath Luirc", "Ennistymon", "Lahinch",
    "Kinnegad", "Lugnaquillia", "Enniscorthy", "Gweebarra",
    "Kittamagh", "Nenagh", "Sneem", "Ballingeary", "Kilgarvan",
    "Cahersiveen", "Glenbeigh", "Kilmihil", "Kiltamagh",
    "Droichead Atha", "Inniscrone", "Clonegal", "Lisnaskea",
    "Culdaff", "Dunfanaghy", "Inishbofin", "Kesh",
    "Loughrea", "Croagh", "Maumakeogh", "Ballyjamesduff",
    /* Deep Ones */
	"Gilman", "Marsh", "Banks", "Waite", "Eliot", "Olmstead",

	"Benderaz", "Nagano", "Newbig", "Hayabusa", "Eckart",
	"Usanna", "Tresh", "Also Matte", "SIR", "Schroeder", "Shmeil",
	"Wellstruct", "Seasoon", "Zhangmu", "Drepung", "Petal",
	"Rokasel", "Ashton", "Callthorpe", "Crocker", "Epworth",
	"Gibbs", "Higgledy", "Kirkeby", "Loddington", "Norwood",
	"Robertson", "Skulker", "Waltham",
    0
};

static const char * const shkarmors[] = {
    /* Turquie */
    "Demirci", "Kalecik", "Boyabai", "Yildizeli", "Gaziantep",
    "Siirt", "Akhalataki", "Tirebolu", "Aksaray", "Ermenak",
    "Iskenderun", "Kadirli", "Siverek", "Pervari", "Malasgirt",
    "Bayburt", "Ayancik", "Zonguldak", "Balya", "Tefenni",
    "Artvin", "Kars", "Makharadze", "Malazgirt", "Midyat",
    "Birecik", "Kirikkale", "Alaca", "Polatli", "Nallihan",

	"Luna", "Nagi", "Nguyen", "Pecker", "Marid", "Warrington",
	"Koopa", "Pietra", "Runner", "Iris flax", "Muxfield", "Petry",
	"Erdogan", "Cunaj", "Istanbul", "Bosporus", "Hamzic", "Rikaze",
	"Azura", "Rhiannon", "Ceyra", "Aynesworth", "Carlyle",
	"Cunningham", "Esmund", "Glendale", "Hightower", "Kirkland",
	"Malingerer", "Nottingham", "Rochforth", "Snelling", "Welkins",
    0
};

static const char * const shkwands[] = {
    /* Wales */
    "Yr Wyddgrug", "Trallwng", "Mallwyd", "Pontarfynach",
    "Rhaeader", "Llandrindod", "Llanfair-ym-muallt",
    "Y-Fenni", "Maesteg", "Rhydaman", "Beddgelert",
    "Curig", "Llanrwst", "Llanerchymedd", "Caergybi",
    /* Scotland */
    "Nairn", "Turriff", "Inverurie", "Braemar", "Lochnagar",
    "Kerloch", "Beinn a Ghlo", "Drumnadrochit", "Morven",
    "Uist", "Storr", "Sgurr na Ciche", "Cannich", "Gairloch",
    "Kyleakin", "Dunvegan",

	"Walter", "Hall", "Hagen", "Rehe", "Pale", "Head", "Left",
	"Haxxor", "Bolgra", "Baeran", "Telpan", "Klaf", "Trump", "Rary",
	"Gallardo", "Peng", "Knat", "Glatz", "Jiangji", "Blaze",
	"Starla", "Baendre", "Babington", "Caxton", "Danvers", "Fackler",
	"Glover", "Hooks", "Knivetton", "Mahoney", "Olingsworth",
	"Russell", "Staverton", "Wexcombe",
    0
};

static const char * const shkrings[] = {
    /* Hollandse familienamen */
    "Feyfer", "Flugi", "Gheel", "Havic", "Haynin", "Hoboken",
    "Imbyze", "Juyn", "Kinsky", "Massis", "Matray", "Moy",
    "Olycan", "Sadelin", "Svaving", "Tapper", "Terwen", "Wirix",
    "Ypey",
    /* Skandinaviske navne */
    "Rastegaisa", "Varjag Njarga", "Kautekeino", "Abisko",
    "Enontekis", "Rovaniemi", "Avasaksa", "Haparanda",
    "Lulea", "Gellivare", "Oeloe", "Kajaani", "Fauske",

	"Sechs", "Saasch", "Schmitt", "Eigenlob", "Gynn", "Merilis",
	"Agnethasson", "Grahw", "Buffy", "Madrugada", "Ata", "Gaab",
	"Changdu", "Breanna", "Tranquilla", "Boerd", "Bartleby",
	"Chamberlain", "Derington", "Fenton", "Goldsmith", "Hornblower",
	"Lambert", "Marcheford", "Oxenbrigg", "Salter", "Styles",
	"Whippleton",
    0
};

static const char * const shkfoods[] = {
    /* Indonesia */
    "Djasinga", "Tjibarusa", "Tjiwidej", "Pengalengan",
    "Bandjar", "Parbalingga", "Bojolali", "Sarangan",
    "Ngebel", "Djombang", "Ardjawinangun", "Berbek",
    "Papar", "Baliga", "Tjisolok", "Siboga", "Banjoewangi",
    "Trenggalek", "Karangkobar", "Njalindoeng", "Pasawahan",
    "Pameunpeuk", "Patjitan", "Kediri", "Pemboeang", "Tringanoe",
    "Makin", "Tipor", "Semai", "Berhala", "Tegal", "Samoe",

	"Bort", "Plejan", "Raj", "Bruch", "Himal", "Manger", "Kelk",
	"Kraj", "Wurie", "Putin", "Dessert", "Suesska", "Vada",
	"Cranice", "Ice Bird", "Doener Kebap", "Shawarma", "McDonalds",
	"Burger King", "Kentucky Fried Chicken", "Mandy", "Le Crobag",
	"Linzhi", "Breezy", "Windsong", "Epart", "Beckingham", "Chester",
	"Digby", "Finch", "Goseborne", "Hornebolt", "Lancelot",
	"Massingbird", "Parsons", "Sapsucker", "Sweetchuck",
	"Whiteknuckle",
    0
};

static const char * const shkweapons[] = {
    /* Perigord */
    "Voulgezac", "Rouffiac", "Lerignac", "Touverac", "Guizengeard",
    "Melac", "Neuvicq", "Vanzac", "Picq", "Urignac", "Corignac",
    "Fleac", "Lonzac", "Vergt", "Queyssac", "Liorac", "Echourgnac",
    "Cazelon", "Eypau", "Carignan", "Monbazillac", "Jonzac",
    "Pons", "Jumilhac", "Fenouilledes", "Laguiolet", "Saujon",
    "Eymoutiers", "Eygurande", "Eauze", "Labouheyre",
    /* Orkney http://www.contrib.andrew.cmu.edu/~grm/orkney.html */
	"Crystie", "Marie", "Hector", "Inggagarth", "Cuthbert", "Marion",
	"Ninian", "Margret", "Edduard", "Alisoun", "Androw", "Christeane",
	"Henrie", "Sonneta", "Brandam", "Anne", "Malcolm", "Gelis", "Magnus",
	"Katherein", "Edwart", "Katherine", "Johnne", "Cristiane", "Troelius",
	"Helline", "Robertt", "Elspet", "Criste", "Alesoun", "Adam", "Helene",
	"Williame", "Margrete", "Gibboun", "Mariota", "Niniane", "Margarete",
	"Hugh", "Effe",
    /* GTA 3 */
    "Claude", "Catalina", "8-Ball", "Salvatore",
    /* Vice City */
    "Tommy", "Ken", "Vance", "Sonny", "Ricardo",
    /* San Andreas */
    "Carl", "Sean", "Melvin", "Wilson", /* don't want "Lance" twice, sorry Soviet --Amy */
    /* 4 */
    "Niko", "Dimitri", "Mikhail", "Vlad",
    /* Chinatown Wars */
    "Huang", "Wu",
    /* GTAV */
    "Michael", "Franklin", "Trevor", "Steve",
    "Weston",

	"Legel", "Deathstrike", "Arschegast", "Mashevka", "Valus",
	"Arm Chest Contractor", "Falcon", "Al Capone", "Schrack", "Mach",
	"March", "Shigatse", "Dharma", "Zennia", "Boast", "Billingham",
	"Chickenheart", "Donnett", "Fleetwilliam", "Grimsby", "Jay",
	"Lassard", "Michelgrove", "Peckham", "Saunderson", "Tackleberry",
	"Wickwillingham",
    0
};

static const char * const shktools[] = {
	/* Amy edit: allow all names, regardless of which operating system you're using, because randomness FTW */

    /* Spmi */
    "Ymla", "Eed-morra", "Cubask", "Nieb", "Bnowr Falr", "Telloc Cyaj",
    "Sperc", "Noskcirdneh", "Yawolloh", "Hyeghu", "Niskal", "Trahnil",
    "Htargcm", "Enrobwem", "Kachzi Rellim", "Regien", "Donmyar",
    "Yelpur", "Nosnehpets", "Stewe", "Renrut", "_Zlaw", "Nosalnef",
    "Rewuorb", "Rellenk", "Yad", "Cire Htims", "Y-crad", "Nenilukah",
    "Corsh", "Aned",
    "Erreip", "Nehpets", "Mron", "Snivek", "Lapu", "Kahztiy",
    "Lechaim", "Lexa", "Niod",
    "Nhoj-lee", "Evad\'kh", "Ettaw-noj", "Tsew-mot", "Ydna-s",
    "Yao-hang", "Tonbar", "Nivek", "Kivenhoug",
    "Falo", "Nosid-da\'r", "Ekim-p", "Rebrol-nek", "Noslo", "Yl-rednow",
    "Mured-oog", "Ivrajimsal",
    "Nivram", "Llardom", "Nedraawi-nav",
    "Lez-tneg", "Ytnu-haled", "Niknar", "AlliWar Wickson", "Oguhmk",

	"Karma", "Sound Impacts", "Wheel Corner", "Montoya", "Lebenich",
	"Strip-Mine", "be silent kert", "Being Moose", "Internet",
	"Ancient Regime", "Work Houses", "Pipe Cutting Equipment", "Parvus",
	"Gaellaei", "Tschulini", "Current Mountain", "Cuddle-Bundle",
	"Coffin Nail", "Gyantse", "Feather", "Zoe", "El Sartre", "Boddenham",
	"Clopton", "Duncombe", "Fleming", "Haddock", "Jameson", "Leventhorpe",
	"Milsent", "Pemberton", "Schtulman", "Thornton", "Williams",

	"Kaahb", "Badanoj", "Natas", "Sirhc", "Ntgnut", "Tnurgs",
	"Remirol", "Noeda", "QIF", "Livednam", "Maerdixul", "Yvi Nedlog",
	"Neercseulb", "Yton", "Ramysp", "Fle Cazorp", "Duam", "Selgnat",
	"Nairodsay", "Repinsgub", "Sipokoileh", "Retnuh Navihs", "Rohk",
	"Oirek", "Onnets", "Ypeed", "Lleremad", "Sia", "Dnnorle",
	"Lidnnerae", "Htereblle", "Redocdnilb", "Ryklebli", "Thgink Yerg",
	"Dexap", "Nogardlwarc", "Nacylg", "Nidrajsed Nouq",
	"Omed", "Xniragag", "Evitcepsortni",
	"Serolod", "Tehpmyn", "Irairam", "Ilol",
	"Reppohssarg", "Aronumrat", "Nnyl", "Ylime", "Emilsrous",
	"Gorflooc", "T0rm", "Suahnerri", "Grqerg", "Teuquob",
	"Trqs Dedne Tsalb", "Leksrol", "Terger", "Urirat", "Laverog",
	"Retniw", "Refrus Eutats", "Hagavvim Rey", "Eralfralos",
	"Aimhtyhrra", "Essiar", "Sbut", "Arym Yresim", "Eirimnele",
	"Tolasniw", "Aimyl", "Axxarhtoh", "Tcidsoa", "Redloheb", "Ycnuonna",
	"Ahtariz", "Tlov", "Sohtilixitirk", "Nasrekir", "Owthtiek",
	"Ledanagan", "Yoreel", "Garena", "Tsitonpyh", "Esoreci",
	"Ikustodam", "Elomacaug Gnimalf", "Qretep", "Blevap", "Tnepocard",
	"Plug Itna", "Oirdna", "Etinatem", "Lyrebyerg", "Nossllep",
	"Esulcer", "Anelam", "Tsaebknip", "Enamkcim", "Namkrop",
	"Goomorcim", "Rolam", "Kelrem", "Nomedcisum",
	"Ruohruetama", "Resuelibom", "Ieoa", "Aletaber",
	"Ecnalorcim", "Erifsysten", "Eribmu", "Iaf Tnc", "Ohor",
	"Allobec", "Noocyt Elppaenip",

    0
};

static const char * const shklight[] = {
    /* Romania */
    "Zarnesti", "Slanic", "Nehoiasu", "Ludus", "Sighisoara", "Nisipitu",
    "Razboieni", "Bicaz", "Dorohoi", "Vaslui", "Fetesti", "Tirgu Neamt",
    "Babadag", "Zimnicea", "Zlatna", "Jiu", "Eforie", "Mamaia",
    /* Bulgaria */
    "Silistra", "Tulovo", "Panagyuritshte", "Smolyan", "Kirklareli",
    "Pernik", "Lom", "Haskovo", "Dobrinishte", "Varvara", "Oryahovo",
    "Troyan", "Lovech", "Sliven",
    /* Brazil */
    "Andre", "Daniel", "Tiago", "Alexandre", "Joao", 
    "Carlos", "Fabio", "Ricardo", "Gustavo", "Rafael",
    "Felipe", "Anderson", "Jorge", "Paulo", "Jose",
    "Rogerio", "Roberto", "Marcos", "Luis", "Luciano",

	"Habich", "Trong-Oul", "Dumont", "Dumas", "Vittel", "Hattrick",
	"Pekriah", "Innocentia", "Diagna", "Castor", "Ronaldo", "Ganden",
	"Jasmine", "Zora", "Burner", "Dobrindt", "Bramfield", "Compton",
	"Edgerton", "Fraidycat", "Hancock", "Jenner", "Lillyliver", "Moore",
	"Poltroon", "Scroggs", "Thursby", "Withinghall",
	"Dimaethor", "Hador", "Grond", "Hast", "Maeth",
	"Rom", "Belegur", "Camaen", "Elvedui", "Gwaedh",
	"Maen", "Maerphen", "Alagos", "Gaeralagos",
	"Faimben", "Fain", "Loss", "Cugu",
    0
};

static const char * const shkgeneral[] = {
    /* Suriname */
    "Hebiwerie", "Possogroenoe", "Asidonhopo", "Manlobbi",
    "Adjama", "Pakka Pakka", "Kabalebo", "Wonotobo",
    "Akalapi", "Sipaliwini",
    /* Greenland */
    "Annootok", "Upernavik", "Angmagssalik",
    /* N. Canada */
    "Aklavik", "Inuvik", "Tuktoyaktuk",
    "Chicoutimi", "Ouiatchouane", "Chibougamau",
    "Matagami", "Kipawa", "Kinojevis",
    "Abitibi", "Maganasipi",
    /* Iceland */
    "Akureyri", "Kopasker", "Budereyri", "Akranes", "Bordeyri",
    "Holmavik",
    /* Marathi */
	"Punita", "Shakuntala", "Richa", "Ninad", "Nikhil", "Sumeet",
	"Roshni", "Mohan", "Ranjeet", "Bipin", "Madhuri", "Abhijeet",
	"Purnima", "Radha", "Grishma", "Satish", "Aparna", "Reena",
	"Shrinivas", "Arjun", "Shekhar", "Deep", "Sameer", "Sunil", "Apoorva",

	"Miller", "Merkel", "Stone", "Bitchiya", "Pedas", "Garbage Houses",
	"Kun-Kwei", "Persh", "Levart", "Gunnhild", "Gardener", "Boker",
	"Tsurphu", "Luna", "Endurance", "Schiesskanista", "Lindner",
	"Bridgeman", "Cooke", "Eggerley", "Froggenhall", "Harricott",
	"Jones", "Limsley", "Morley", "Pratt", "Seymour", "Tweedy",
	"Worsley",
    0
};

static const char *shkblack[] = {
  "One-eyed Sam", "One-eyed Sam", "One-eyed Sam",
  "One-eyed Sam", "One-eyed Sam", "One-eyed Sam",
  "One-eyed Sam", "One-eyed Sam", "One-eyed Sam",
  "One-eyed Sam", "One-eyed Sam", "One-eyed Sam",
  "One-eyed Sam", "One-eyed Sam", "One-eyed Sam",
  "One-eyed Sam", "One-eyed Sam", "One-eyed Sam",
  "One-eyed Sam", "One-eyed Sam", "One-eyed Sam",
  "One-eyed Sam", "One-eyed Sam", "One-eyed Sam",
  "One-eyed Sam", "One-eyed Sam", "One-eyed Sam",
  "One-eyed Sam", "One-eyed Sam", "One-eyed Sam",
  "One-eyed Sam", "One-eyed Sam", "One-eyed Sam",
  0
};

/* STEPHEN WHITE'S NEW CODE */
static const char *shkpet[] = {
    "Silistra", "Tulovo", "Panagyuritshte", "Smolyan", "Kirklareli",
    "Pernik", "Lom", "Haskovo", "Dobrinishte", "Varvara", "Oryahovo",
    "Troyan", "Lovech", "Sliven",
    /* Bee scientific names */
    "Apis", "Mellifera", "Cerana", "Dorsata", "Andreniformis",
	"Florea", "Nearctica", "Nigrocincta",
    /* Ant */
    "Crematogaster", "Linepithema", "Humile", "Pheidole", "Nylanderia",
	"Pubens", "Camponotus", "Lasius", "Alienus",
	"Paratrechina", "Longicornis", "Solenopsis", "Invicta", "Tapinoma",
	"Melanocephalum", "Monomorium", "Minimum", "Tetramorium", "Caespitum",
	"Paraonis", "Dorymyrmex", "Brachymyrmex", "Technomyrmex", "Albipes", 
    /* Crab scientific names */
	"Potamon", "Ibericum", "Parathelphusa", "Convexa", "Johongarthia", "Lagostoma",
	"Gecarcoidea", "Natalis", 

	"War Mountain", "Oro", "Pure", "King", "Funo", "Cuntrina",
	"Tato", "Santana", "Dickmouth", "Mul", "White Logel", "Lhasa",
	"Melody", "Moonbeam", "Whoopee", "Sanuuicula", "Brocksby",
	"Cosworth", "Emerson", "Frye", "Harwood", "Kay", "Litchfield",
	"Mouse", "Rampston", "Shawe", "Vernon", "Yellowbelly",
	"Spiffy", "Bonko", "Binky", "Tubby", "Zippy", "Jumbo",
	"Mittens", "Chuckles", "Bam Bam", "Larry", "Curly",
	"Moe", "Zaff", "Punky",
    0
};

/*
 * To add new shop types, all that is necessary is to edit the shtypes[] array.
 * See mkroom.h for the structure definition.  Typically, you'll have to lower
 * some or all of the probability fields in old entries to free up some
 * percentage for the new type.
 *
 * The placement type field is not yet used but will be in the near future.
 *
 * The iprobs array in each entry defines the probabilities for various kinds
 * of objects to be present in the given shop type.  You can associate with
 * each percentage either a generic object type (represented by one of the
 * *_CLASS macros) or a specific object (represented by an onames.h define).
 * In the latter case, prepend it with a unary minus so the code can know
 * (by testing the sign) whether to use mkobj() or mksobj().
 */
/* KMH -- Don't forget to update mkroom.h and lev_main.c when adding
 * a new shop type.
 */
const struct shclass shtypes[] = {
	{"general store", RANDOM_CLASS, 23,
	    D_SHOP, {{100, WILDCARD_CLASS}, {0, 0}, {0, 0}}, shkgeneral},
	{"used armor dealership", ARMOR_CLASS, 17,
	    D_SHOP, {{90, ARMOR_CLASS}, {10, WEAPON_CLASS}, {0, 0}},
	     shkarmors},
	{"second-hand bookstore", SCROLL_CLASS, 4, D_SHOP,
	    {{90, SCROLL_CLASS}, {10, SPBOOK_CLASS}, {0, 0}}, shkbooks},
	{"liquor emporium", POTION_CLASS, 4, D_SHOP,
	    {{100, POTION_CLASS}, {0, 0}, {0, 0}}, shkliquors},
	{"weapons outlet", WEAPON_CLASS, 17, D_SHOP, {
	    {80, WEAPON_CLASS}, {3, -PISTOL_BULLET}, {1, -ASSAULT_RIFLE_BULLET}, {3, -RIFLE_BULLET}, {2, -SMG_BULLET}, {1, -SILVER_PISTOL_BULLET},
	    {10, ARMOR_CLASS}, {0, 0}}, shkweapons},
	{"delicatessen", FOOD_CLASS, 4, D_SHOP,
	    {{83, FOOD_CLASS}, {5, -POT_FRUIT_JUICE}, {4, -POT_BOOZE},
	     {5, -POT_WATER}, {3, -ICE_BOX}}, shkfoods},
	{"jewelers", RING_CLASS, 3, D_SHOP,
	    {{85, RING_CLASS}, {9, GEM_CLASS}, {5, AMULET_CLASS}, {1, IMPLANT_CLASS}, {0, 0}},
	    shkrings},
	{"quality apparel and accessories", WAND_CLASS, 4, D_SHOP,
	    {{90, WAND_CLASS}, {5, -LEATHER_GLOVES}, {5, -ELVEN_CLOAK}, {0, 0}},
	     shkwands},
	{"hardware store", TOOL_CLASS, 3, D_SHOP,
	    {{100, TOOL_CLASS}, {0, 0}}, shktools},
	/* Actually shktools is ignored; the code specifically chooses a
	 * random implementor name (along with candle shops having
	 * random shopkeepers)
	 */
	/* STEPHEN WHITE'S NEW CODE */
	{"pet store", FOOD_CLASS, 4, D_SHOP, {
	    {67, -FIGURINE}, {5, -LEATHER_LEASH},{10, -TRIPE_RATION}, {5, -LEATHER_SADDLE},
	    {10, -TIN_WHISTLE}, {3, -MAGIC_WHISTLE}}, shkpet},
	/* Robin Johnson -- 4% taken from pet store */
	{"frozen food store", FOOD_CLASS, 4, D_SHOP,
		{{90, -ICE_BOX}, {10, -TIN},
		/* shopkeeper will pay for corpses, but they aren't generated */
		/* on the shop floor */
		{0, -CORPSE}, {0, 0}}, shkfoods},
	{"rare books", SPBOOK_CLASS, 4, D_SHOP,
	    {{90, SPBOOK_CLASS}, {10, SCROLL_CLASS}, {0, 0}}, shkbooks},
	{"fresh food store", FOOD_CLASS, 1, D_SHOP,
		{{90, -CORPSE}, {5, -TIN}, {5, FOOD_CLASS}, {0, 0}}, shkfoods},

	{"gun shop", WEAPON_CLASS, 1, D_SHOP, {
{16, -PISTOL}, {17, -SUBMACHINE_GUN}, {17, -HEAVY_MACHINE_GUN}, {16, -RIFLE}, {17, -ASSAULT_RIFLE}, {17, -SNIPER_RIFLE},
          {0, 0}}, shkweapons},

	{"banging shop", WEAPON_CLASS, 1, D_SHOP, {
 {24, -SHOTGUN}, {1, -SAWED_OFF_SHOTGUN}, {25, -AUTO_SHOTGUN}, {25, -ROCKET_LAUNCHER}, {25, -GRENADE_LAUNCHER},          {0, 0}}, shkweapons},

	{"ammo shop", WEAPON_CLASS, 1, D_SHOP, {
{3, -PISTOL_BULLET}, {3, -SMG_BULLET}, {3, -RIFLE_BULLET}, {3, -ASSAULT_RIFLE_BULLET}, {2, -SNIPER_BULLET}, {1, -MG_BULLET}, {15, -SILVER_PISTOL_BULLET}, {7, -SHOTGUN_SHELL}, {7, -AUTO_SHOTGUN_SHELL}, {13, -ROCKET}, {1, -FIVE_SEVEN_BULLET}, {14, -FRAG_GRENADE}, {14, -GAS_GRENADE}, {14, -STICK_OF_DYNAMITE} }, shkweapons},

	{"lightning store", TOOL_CLASS, 1, D_SHOP,
	    {{25, -WAX_CANDLE}, {35, -TALLOW_CANDLE}, {5, -TORCH}, {11, -BRASS_LANTERN},
	    {16, -OIL_LAMP}, {3, -MAGIC_LAMP}, {5, -MAGIC_CANDLE}}, shktools},

	{"one-stop mining shop", RANDOM_CLASS, 1, D_SHOP, {
	    {100, ROCK_CLASS}, {0, 0}}, shktools},

	{"rock-solid store", RANDOM_CLASS, 1, D_SHOP, {
	    {14, -LUCKSTONE}, {14, -HEALTHSTONE}, {14, -TOUCHSTONE}, {14, -LOADSTONE}, {14, -WHETSTONE}, {15, -FLINT}, {15, -ROCK}, {0, 0}}, shktools},

	{"weirdo shop", RANDOM_CLASS, 1, D_SHOP, {
	    {33, CHAIN_CLASS}, {33, BALL_CLASS}, {34, VENOM_CLASS}, {0, 0}}, shktools},

	{"zorkmid store", RANDOM_CLASS, 1,
	    D_SHOP, {{100, COIN_CLASS}, {0, 0}, {0, 0}}, shkgeneral},

	{"random store", RANDOM_CLASS, 0,
	    D_SHOP, {{100, COIN_CLASS}, {0, 0}, {0, 0}}, shkgeneral},

	/* Shops below this point are "unique".  That is they must all have a
	 * probability of zero.  They are only created via the special level
	 * loader.
	 */
	{"lighting store", TOOL_CLASS, 0, D_SHOP,
	    {{25, -WAX_CANDLE}, {35, -TALLOW_CANDLE}, {5, -TORCH}, {11, -BRASS_LANTERN},
	    {16, -OIL_LAMP}, {3, -MAGIC_LAMP}, {5, -MAGIC_CANDLE}}, shklight},
	{"black market", RANDOM_CLASS, 0, D_SHOP,
	   {{100, WILDCARD_CLASS}, {0, 0}, {0, 0}}, shkblack},
	{(char *)0, 0, 0, 0, {{0, 0}, {0, 0}, {0, 0}}, 0}
};

#if 0
/* validate shop probabilities; otherwise incorrect local changes could
   end up provoking infinite loops or wild subscripts fetching garbage */
void
init_shop_selection()
{
	register int i, j, item_prob, shop_prob;

	for (shop_prob = 0, i = 0; i < SIZE(shtypes); i++) {
		shop_prob += shtypes[i].prob;
		for (item_prob = 0, j = 0; j < SIZE(shtypes[0].iprobs); j++)
			item_prob += shtypes[i].iprobs[j].iprob;
		if (item_prob != 100)
			panic("item probabilities total to %d for %s shops!",
				item_prob, shtypes[i].name);
	}
	if (shop_prob != 100)
		panic("shop probabilities total to %d!", shop_prob);
}
#endif /*0*/

STATIC_OVL void
mkshobj_at(shp, sx, sy, artif)
/* make an object of the appropriate type for a shop square */
const struct shclass *shp;
int sx, sy;
boolean artif;
{
	struct monst *mtmp;
	int atype;
	struct permonst *ptr;

	/* Amy edit: since the maximum dungeon level is much higher, we need to adjust this calculation */
	if (rn2(250) < depth(&u.uz) &&
		!MON_AT(sx, sy) && (ptr = mkclass(S_MIMIC,0)) &&
		(mtmp = makemon(ptr,sx,sy,NO_MM_FLAGS)) != 0) {
	    /* note: makemon will set the mimic symbol to a shop item */
	    if (rn2(10) >= depth(&u.uz)) {
		mtmp->m_ap_type = M_AP_OBJECT;
		mtmp->mappearance = STRANGE_OBJECT;
	    }
	} else {

	    if (timebasedlowerchance() && (rnd(100) > u.shopitemreduction) ) {

		    atype = get_shop_item(shp - shtypes);
		    if (atype < 0)
			(void) mksobj_at(-atype, sx, sy, TRUE, artif, TRUE);
		    else
			(void) mkobj_at(atype, sx, sy, artif, TRUE);

	    }
	}

	if (ishaxor && (rn2(250) >= depth(&u.uz)) ) {

	    if (timebasedlowerchance() && (rnd(100) > u.shopitemreduction) ) {

		    atype = get_shop_item(shp - shtypes);
		    if (atype < 0)
			(void) mksobj_at(-atype, sx, sy, TRUE, artif, TRUE);
		    else
			(void) mkobj_at(atype, sx, sy, artif, TRUE);

	    }
	}

}

/* extract a shopkeeper name for the given shop type */
STATIC_OVL void
nameshk(shk, nlp)
struct monst *shk;
const char * const *nlp;
{
	int i, trycnt, names_avail;
	const char *shname = 0;
	struct monst *mtmp;
	int name_wanted;
	s_level *sptr;

	if (nlp == shklight && In_mines(&u.uz)
		&& (sptr = Is_special(&u.uz)) != 0 && sptr->flags.town) {
	    /* special-case minetown lighting shk */
	    shname = "Izchak";
	    shk->female = FALSE;

		/* Amy edit: Izchak is ultra special :D */

		if (rn2(25)) {
			shk->noegodesc = shk->noegodisplay = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_wallwalk = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_disenchant = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_rust = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_corrosion = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_decay = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_flamer = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_blasphemer = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_inverter = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_debtor = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_dropper = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_wither = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_grab = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_slows = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_vampire = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_wrap = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_disease = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_slime = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_engrave = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_dark = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_luck = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_arcane = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_clerical = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_armorer = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_tank = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_speedster = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_racer = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_blaster = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_gator = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_reflecting = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_farter = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_watersplasher = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_cancellator = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_shredder = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_incrementor = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_mirrorimage = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_curser = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_cullen = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_webber = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_itemporter = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_abomination = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_flickerer = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_hitter = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_piercer = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_lifesaver = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_shader = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_amnesiac = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_trapmaster = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_mastercaster = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_sinner = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_weeper = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_destructor = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_trembler = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_worldender = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_damager = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_antitype = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_statdamager = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_nastycurser = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_sanitizer = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_rogue = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_champion = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_boss = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_atomizer = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_perfumespreader = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_converter = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_alladrainer = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_stabilizer = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_spoilerproofer = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_metalmafioso = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_deepstatemember = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_wouwouer = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_badowner = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_bleeder = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_shanker = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_terrorizer = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_feminizer = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_levitator = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_illusionator = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_stealer = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_stoner = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_maecke = TRUE;
		}
		if (!rn2(20)) {
			shk->isegotype = 1;
			shk->egotype_amberite = TRUE;
		}
		/* Izchak always has the laserpwnzor and datadeleter egotypes --Amy */
		shk->isegotype = 1;
		shk->egotype_laserpwnzor = TRUE;
		shk->egotype_datadeleter = TRUE;

	} else if (nlp == shkblack) {
	    /* special-case black marketeer */
	    shname = "One-eyed Sam";
	    shk->female = shk->data->mflags2 & M2_MALE ? FALSE : TRUE;

		/* Amy edit: Sam is even more special! */

		shk->noegodesc = shk->noegodisplay = TRUE;
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_wallwalk = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_disenchant = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_rust = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_corrosion = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_decay = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_flamer = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_blasphemer = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_inverter = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_debtor = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_dropper = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_wither = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_grab = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_slows = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_vampire = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_wrap = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_disease = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_slime = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_engrave = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_dark = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_luck = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_arcane = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_clerical = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_armorer = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_tank = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_speedster = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_racer = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_blaster = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_gator = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_reflecting = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_farter = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_watersplasher = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_cancellator = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_shredder = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_incrementor = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_mirrorimage = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_curser = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_cullen = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_webber = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_itemporter = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_abomination = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_flickerer = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_hitter = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_piercer = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_lifesaver = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_shader = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_amnesiac = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_trapmaster = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_mastercaster = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_sinner = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_weeper = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_destructor = TRUE;
		}
		if (!rn2(1000)) {
			shk->isegotype = 1;
			shk->egotype_datadeleter = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_trembler = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_worldender = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_damager = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_antitype = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_statdamager = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_nastycurser = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_sanitizer = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_rogue = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_champion = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_boss = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_atomizer = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_perfumespreader = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_converter = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_alladrainer = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_stabilizer = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_spoilerproofer = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_metalmafioso = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_deepstatemember = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_wouwouer = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_laserpwnzor = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_badowner = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_bleeder = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_shanker = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_terrorizer = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_feminizer = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_levitator = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_illusionator = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_stealer = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_stoner = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_maecke = TRUE;
		}
		if (!rn2(10)) {
			shk->isegotype = 1;
			shk->egotype_amberite = TRUE;
		}

	} else {
	    /* We want variation from game to game, without needing the save
	       and restore support which would be necessary for randomization;
	       try not to make too many assumptions about time_t's internals;
	       use ledger_no rather than depth to keep mine town distinct. */
	    int nseed = /*(int)((long)u.ubirthday / 257L);*/rnd(257);

	    name_wanted = ledger_no(&u.uz) + (nseed % 13) - (nseed % 5);
	    if (name_wanted < 0) name_wanted += (13 + 5);
	    shk->female = name_wanted & 1;

	    for (names_avail = 0; nlp[names_avail]; names_avail++)
		continue;

	    for (trycnt = 0; trycnt < 50; trycnt++) {
		if (nlp == shktools) {
		    shname = shktools[rn2(names_avail)];
		    shk->female = (*shname == '_');
		    if (shk->female) shname++;
		} else if (name_wanted < names_avail) {
		    shname = nlp[name_wanted];
		} else if ((i = rn2(names_avail)) != 0) {
		    shname = nlp[i - 1];
		} else if (nlp != shkgeneral) {
		    nlp = shkgeneral;	/* try general names */
		    for (names_avail = 0; nlp[names_avail]; names_avail++)
			continue;
		    continue;		/* next `trycnt' iteration */
		} else {
		    shname = shk->female ? "Lucrezia" : "Dirk";
		}

		/* is name already in use on this level? */
		for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		    if (DEADMONSTER(mtmp) || (mtmp == shk) || !mtmp->isshk) continue;
		    if (strcmp(ESHK(mtmp)->shknam, shname)) continue;
		    break;
		}
		if (!mtmp) break;	/* new name */
	    }
	}
	(void) strncpy(ESHK(shk)->shknam, shname, PL_NSIZ);
	ESHK(shk)->shknam[PL_NSIZ-1] = 0;
}

STATIC_OVL int
shkinit(shp, sroom)	/* create a new shopkeeper in the given room */
const struct shclass	*shp;
struct mkroom	*sroom;
{
	register int sh, sx, sy;
	struct monst *shk;
	long shkmoney; /* Temporary placeholder for Shopkeeper's initial capital */

	/* place the shopkeeper in the given room */
	sh = sroom->fdoor;
	sx = doors[sh].x;
	sy = doors[sh].y;

	/* check that the shopkeeper placement is sane */
	if(sroom->irregular) {
	    int rmno = (sroom - rooms) + ROOMOFFSET;
	    if (isok(sx-1,sy) && !levl[sx-1][sy].edge &&
		(int) levl[sx-1][sy].roomno == rmno) sx--;
	    else if (isok(sx+1,sy) && !levl[sx+1][sy].edge &&
		(int) levl[sx+1][sy].roomno == rmno) sx++;
	    else if (isok(sx,sy-1) && !levl[sx][sy-1].edge &&
		(int) levl[sx][sy-1].roomno == rmno) sy--;
	    else if (isok(sx,sy+1) && !levl[sx][sy+1].edge &&
		(int) levl[sx][sy+1].roomno == rmno) sx++;
	    else goto shk_failed;
	}
	else if(sx == sroom->lx-1) sx++;
	else if(sx == sroom->hx+1) sx--;
	else if(sy == sroom->ly-1) sy++;
	else if(sy == sroom->hy+1) sy--; else {
	shk_failed:
#ifdef DEBUG
# ifdef WIZARD
	    /* Said to happen sometimes, but I have never seen it. */
	    /* Supposedly fixed by fdoor change in mklev.c */
	    if(wizard) {
		register int j = sroom->doorct;

		pline("Where is shopdoor?");
		pline("Room at (%d,%d),(%d,%d).",
		      sroom->lx, sroom->ly, sroom->hx, sroom->hy);
		pline("doormax=%d doorct=%d fdoor=%d",
		      doorindex, sroom->doorct, sh);
		while(j--) {
		    pline("door [%d,%d]", doors[sh].x, doors[sh].y);
		    sh++;
		}
		display_nhwindow(WIN_MESSAGE, FALSE);
	    }
# endif
#endif
	    return(-1);
	}

	if(MON_AT(sx, sy)) (void) rloc(m_at(sx, sy), FALSE); /* insurance */

	/* now initialize the shopkeeper monster structure */
	  
	shk = 0;
	if (Is_blackmarket(&u.uz)) {
	  shk = makemon(&mons[PM_BLACK_MARKETEER], sx, sy, NO_MM_FLAGS);
	}
	/* deepy wants shopkeepers to start out hostile to a venture capitalist --Amy */
	if (!shk) {
	  if(!(shk = makemon(&mons[((level_difficulty() + u.pompejipermanentrecord) > 39) ? PM_ELITE_SHOPKEEPER : ((level_difficulty() + u.pompejipermanentrecord) > 19) ? PM_MASTER_SHOPKEEPER : PM_SHOPKEEPER], sx, sy, Race_if(PM_VENTURE_CAPITALIST) ? MM_ANGRY : NO_MM_FLAGS)))
		return(-1);
	}        
  
	shk->isshk = shk->mpeaceful = 1;
	if (Race_if(PM_VENTURE_CAPITALIST)) shk->mpeaceful = 0;
	set_malign(shk);
	shk->msleeping = 0;
	/*shk->mtrapseen = ~0;*/	/* we know all the traps already */
	ESHK(shk)->shoproom = (sroom - rooms) + ROOMOFFSET;
	sroom->resident = shk;
	ESHK(shk)->shoptype = sroom->rtype;
	assign_level(&(ESHK(shk)->shoplevel), &u.uz);
	ESHK(shk)->creditlimit = (5000 + (depth(&u.uz) * 1000) );
	ESHK(shk)->totalcredit = 0;
	ESHK(shk)->shd.x = doors[sh].x;
	ESHK(shk)->shd.y = doors[sh].y;
	ESHK(shk)->shk.x = sx;
	ESHK(shk)->shk.y = sy;
	ESHK(shk)->robbed = 0L;
	ESHK(shk)->credit = 0L;
	ESHK(shk)->debit = 0L;
	ESHK(shk)->loan = 0L;
	ESHK(shk)->visitct = 0;
	ESHK(shk)->following = 0;
	ESHK(shk)->billct = 0;
	/* WAC init services */
	init_shk_services(shk);

  shkmoney = 1000L + 30L*(long)rnd(100);	/* initial capital */
  						/* [CWC] Lets not create the money yet until we see if the
  							 shk is a black marketeer, else we'll have to create
  						   another money object, if GOLDOBJ is defined */

	if (shp->shknms == shkrings)
	    (void) mongets(shk, TOUCHSTONE);
	nameshk(shk, shp->shknms);

	if (Is_blackmarket(&u.uz))
    shkmoney = 7*shkmoney + rn2(3*shkmoney);

#ifndef GOLDOBJ
	shk->mgold = shkmoney;	
#else
  mkmonmoney(shk, shkmoney);
#endif

	if (Is_blackmarket(&u.uz)) {
	  register struct obj *otmp;
/* make sure black marketeer can wield Thiefbane */
	  shk->data->maligntyp = -1;
/* black marketeer's equipment */
	  otmp = mksobj(LONG_SWORD, FALSE, FALSE, FALSE);
	  if (otmp) {
		  otmp = oname(otmp, artiname(ART_THIEFBANE));
		  mpickobj(shk, otmp, TRUE);
		  if (otmp->spe < 5) otmp->spe += rnd(5);
	  }
	  otmp = mksobj(SHIELD_OF_REFLECTION, FALSE, FALSE, FALSE);
	  if (otmp) {
		  mpickobj(shk, otmp, TRUE);
		  if (otmp->spe < 5) otmp->spe += rnd(5);
	  }
	  otmp = mksobj(GRAY_DRAGON_SCALE_MAIL, FALSE, FALSE, FALSE);
	  if (otmp) {
		  mpickobj(shk, otmp, TRUE);
		  if (otmp->spe < 5) otmp->spe += rnd(5);
	  }
	  otmp = mksobj(SPEED_BOOTS, FALSE, FALSE, FALSE);
	  if (otmp) {
		  mpickobj(shk, otmp, TRUE);
		  if (otmp->spe < 5) otmp->spe += rnd(5);
	  }
	  otmp = mksobj(AMULET_OF_LIFE_SAVING, FALSE, FALSE, FALSE);
	  if (otmp) {
		  mpickobj(shk, otmp, TRUE);
	  }
/* wear armor and amulet */
	  m_dowear(shk, TRUE);
	  otmp = mksobj(SKELETON_KEY, FALSE, FALSE, FALSE);
	  if (otmp) {
		  mpickobj(shk, otmp, TRUE);
	  }
	}

	if (Race_if(PM_VENTURE_CAPITALIST)) hot_pursuit(shk);

	return(sh);
}

/* stock a newly-created room with objects */
void
stock_room(shp_indx, sroom)
int shp_indx;
register struct mkroom *sroom;
{
    /*
     * Someday soon we'll dispatch on the shdist field of shclass to do
     * different placements in this routine. Currently it only supports
     * shop-style placement (all squares except a row nearest the first
     * door get objects).
     */
    register int sx, sy, sh;
    char buf[BUFSZ];
    int rmno = (sroom - rooms) + ROOMOFFSET;
    const struct shclass *shp = &shtypes[shp_indx];

    /* first, try to place a shopkeeper in the room */
    if ((sh = shkinit(shp, sroom)) < 0)
	return;

    /* make sure no doorways without doors, and no */
    /* trapped doors, in shops.			   */
    sx = doors[sroom->fdoor].x;
    sy = doors[sroom->fdoor].y;

    if(levl[sx][sy].doormask == D_NODOOR) {
	    levl[sx][sy].doormask = D_ISOPEN;
	    newsym(sx,sy);
    }
    if(levl[sx][sy].typ == SDOOR) {
	    cvt_sdoor_to_door(&levl[sx][sy]);	/* .typ = DOOR */
	    newsym(sx,sy);
    }
    if(levl[sx][sy].doormask & D_TRAPPED)
	    levl[sx][sy].doormask = D_LOCKED;

    if(levl[sx][sy].doormask == D_LOCKED) {
	    register int m = sx, n = sy;

	    if(inside_shop(sx+1,sy)) m--;
	    else if(inside_shop(sx-1,sy)) m++;
	    if(inside_shop(sx,sy+1)) n--;
	    else if(inside_shop(sx,sy-1)) n++;
	    sprintf(buf, "Closed for inventory");
	    make_engr_at(m, n, buf, 0L, DUST);
    }

    if (Is_blackmarket(&u.uz)) {
      stock_blkmar(shp, sroom, sh);
      level.flags.has_shop = TRUE;
      return;
    }

    for(sx = sroom->lx; sx <= sroom->hx; sx++)
	for(sy = sroom->ly; sy <= sroom->hy; sy++) {
	    if(sroom->irregular) {
		if (levl[sx][sy].edge || (int) levl[sx][sy].roomno != rmno ||
		   distmin(sx, sy, doors[sh].x, doors[sh].y) <= 1)
		    continue;
	    } else if((sx == sroom->lx && doors[sh].x == sx-1) ||
		      (sx == sroom->hx && doors[sh].x == sx+1) ||
		      (sy == sroom->ly && doors[sh].y == sy-1) ||
		      (sy == sroom->hy && doors[sh].y == sy+1)) continue;
	    if (rn2(10)) {

			/* shops that stock very specific items shouldn't have hundreds of artifacts... --Amy */
			if (sroom->rtype == GUNSHOP || sroom->rtype == BANGSHOP || sroom->rtype == AMMOSHOP) 
				mkshobj_at(shp, sx, sy, FALSE);
			else mkshobj_at(shp, sx, sy, TRUE);
	    }
	}

    /*
     * Special monster placements (if any) should go here: that way,
     * monsters will sit on top of objects and not the other way around.
     */

    level.flags.has_shop = TRUE;
}

/* stock a newly-created black market with objects */
static void
stock_blkmar(shp, sroom, sh)
const struct shclass *shp;
register struct mkroom *sroom;
register int sh;
{
    /*
     * Someday soon we'll dispatch on the shdist field of shclass to do
     * different placements in this routine. Currently it only supports
     * shop-style placement (all squares except a row nearest the first
     * door get objects).
     */
    /* [max] removed register int cl,  char buf[bufsz] */
    int i, sx, sy, first = 0, next = 0, total, partial, typ;
    struct obj *otmp;
    int blkmar_gen[NUM_OBJECTS+2];
    int *clp, *lastclp;
    int goodcl[12];

    goodcl[ 0] = WEAPON_CLASS;
    goodcl[ 1] = ARMOR_CLASS;
    goodcl[ 2] = RING_CLASS;
    goodcl[ 3] = AMULET_CLASS;
    goodcl[ 4] = TOOL_CLASS;
    goodcl[ 5] = FOOD_CLASS;
    goodcl[ 6] = POTION_CLASS;
    goodcl[ 7] = SCROLL_CLASS;
    goodcl[ 8] = SPBOOK_CLASS;
    goodcl[ 9] = WAND_CLASS;
    goodcl[10] = GEM_CLASS;
    goodcl[11] = 0;

    for (i=0; i < NUM_OBJECTS; i++) {
      blkmar_gen[i] = 0;
    }

    total = 0;
    for (clp=goodcl; *clp!=0; clp++)  {
      lastclp = clp;
      first = bases[*clp];
/* this assumes that luckstone & loadstone comes just after the gems */
      next = (*clp==GEM_CLASS) ? (FLINT+1) : bases[(*clp)+1];
      total += next-first;
    }
    if (total==0)  return;

    if (sroom->hx-sroom->lx<2)  return;
    clp = goodcl-1;
    partial = 0;
    for(sx = sroom->lx+1; sx <= sroom->hx; sx++) {
      if (sx==sroom->lx+1 ||
	  ((sx-sroom->lx-2)*total)/(sroom->hx-sroom->lx-1)>partial) {
	clp++;
	if (clp>lastclp)  clp = lastclp;
	first = bases[*clp];
	next = (*clp==GEM_CLASS) ? (FLINT+1) : bases[(*clp)+1];
	partial += next-first;
      }

      for(sy = sroom->ly; sy <= sroom->hy; sy++) {
	if((sx == sroom->lx && doors[sh].x == sx-1) ||
	   (sx == sroom->hx && doors[sh].x == sx+1) ||
	   (sy == sroom->ly && doors[sh].y == sy-1) ||
	   (sy == sroom->hy && doors[sh].y == sy+1) || (rn2(3)))
	  continue;

	for (i=0; i<50; i++) {
	  typ = rn2(next-first) + first;

/* forbidden objects  */
	  if (typ==AMULET_OF_YENDOR || typ==CANDELABRUM_OF_INVOCATION ||
	      typ==BELL_OF_OPENING  || typ==SPE_BOOK_OF_THE_DEAD ||
	      objects[typ].oc_nowish || typ==0)
	    continue;

	  otmp = mkobj_at(WILDCARD_CLASS,sx,sy,TRUE, TRUE);
/* generate multiple copies with decreasing probabilities */
/*        if (rn2(blkmar_gen[typ]+1) && i<49)  continue; */

/*        otmp = mksobj_at(typ, sx, sy, TRUE, TRUE, TRUE);
	  blkmar_gen[typ]++;*/

	if (!otmp) break;

/* prevent wishing abuse */
	  if (typ==WAN_WISHING || typ==WAN_ACQUIREMENT) {
	    otmp->spe = 0;
	    otmp->recharged = 1;
	  }
	  if (typ==MAGIC_LAMP) {
	    otmp->spe = 0;
	  }
	  if (typ==SCR_WISHING || typ==SCR_ACQUIREMENT || typ==SCR_ENTHRONIZATION || typ==SCR_MAKE_PENTAGRAM || typ==SCR_FOUNTAIN_BUILDING || typ==SCR_SINKING || typ==SCR_WC) {
	    typ = SCR_BLANK_PAPER;
	  }

	  break;
	}
	
      }
    }

    /*
     * Special monster placements (if any) should go here: that way,
     * monsters will sit on top of objects and not the other way around.
     */
}


static void
init_shk_services(shk)
struct monst *shk;
{
	ESHK(shk)->services = 0L;

	/* KMH, balance patch 2 -- Increase probability of shopkeeper services.
	 * Requested by Dave <mitch45678@aol.com>
	 */
	if (Is_blackmarket(&u.uz)) {
		ESHK(shk)->services = 
		    SHK_ID_BASIC|SHK_ID_PREMIUM|SHK_UNCURSE|SHK_BLESS|SHK_APPRAISE|
		    SHK_SPECIAL_A|SHK_SPECIAL_B|SHK_SPECIAL_C|SHK_CREDITSRV;
		return;
	}

	/* Guarantee some form of identification
	 * 1/3 		both Basic and Premium ID
	 * 2/15 	Premium ID only
	 * 8/15 	Basic ID only
	 */
	if (!rn2(20)) ESHK(shk)->services |= (SHK_ID_BASIC|SHK_ID_PREMIUM);
	else if (!rn2(10)) ESHK(shk)->services |= SHK_ID_BASIC;
	else if (!rn2(10)) ESHK(shk)->services |= SHK_ID_PREMIUM;

	if (!rn2(10)) ESHK(shk)->services |= SHK_UNCURSE;
	if (!rn2(20)) ESHK(shk)->services |= SHK_BLESS;

	if (!rn2(2) && shk_class_match(WEAPON_CLASS, shk))
		ESHK(shk)->services |= SHK_APPRAISE;

	if ((shk_class_match(WEAPON_CLASS, shk) == SHK_MATCH) ||
	(shk_class_match(ARMOR_CLASS, shk) == SHK_MATCH) ||
	(shk_class_match(WAND_CLASS, shk) == SHK_MATCH) ||
	(shk_class_match(TOOL_CLASS, shk) == SHK_MATCH) ||
	(shk_class_match(SPBOOK_CLASS, shk) == SHK_MATCH) ||
	(shk_class_match(RING_CLASS, shk) == SHK_MATCH)) {
		if (!rn2(3/*5*/)) ESHK(shk)->services |= SHK_SPECIAL_A;
		if (!rn2(3/*5*/)) ESHK(shk)->services |= SHK_SPECIAL_B;
	}
	if (!rn2(3/*5*/) && (shk_class_match(WEAPON_CLASS, shk) == SHK_MATCH))
	 ESHK(shk)->services |= SHK_SPECIAL_C;

	/* establishing credit is always possible --Amy */
	ESHK(shk)->services |= SHK_CREDITSRV;

	return;
}

#endif /* OVLB */
#ifdef OVL0

/* does shkp's shop stock this item type? */
boolean
saleable(shkp, obj)
struct monst *shkp;
struct obj *obj;
{
    int i, shp_indx = ESHK(shkp)->shoptype - SHOPBASE;
    const struct shclass *shp = &shtypes[shp_indx];

    if (shp->symb == RANDOM_CLASS || (uarmf && uarmf->oartifact == ART_KRISTIN_S_NOBILITY) ) return TRUE;
    else for (i = 0; i < SIZE(shtypes[0].iprobs) && shp->iprobs[i].iprob; i++)
		if (shp->iprobs[i].itype < 0 ?
			shp->iprobs[i].itype == - obj->otyp :
			shp->iprobs[i].itype == obj->oclass) return TRUE;
    /* not found */
    return FALSE;
}

/* positive value: class; negative value: specific object type */
int
get_shop_item(type)
int type;
{
	const struct shclass *shp = shtypes+type;
	register int i,j;

	/* select an appropriate object type at random */
	for(j = rnd(100), i = 0; (j -= shp->iprobs[i].iprob) > 0; i++)
		continue;

	return shp->iprobs[i].itype;
}

#endif /* OVL0 */

/*shknam.c*/
