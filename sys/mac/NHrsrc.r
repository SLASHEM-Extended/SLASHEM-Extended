resource 'WIND' (128, "NULL") {
	{40, 40, 140, 162},
	documentProc,
	invisible,
	goAway,
	0x0,
	"Null Window !",
	staggerMainScreen
};

resource 'WIND' (129, "Message") {
	{264, 8, 330, 240},
	documentProc,
	invisible,
	noGoAway,
	0x0,
	"Message",
	staggerMainScreen
};

resource 'WIND' (130, "Status") {
	{302, 4, 338, 458},
	documentProc,
	invisible,
	noGoAway,
	0x0,
	"Status",
	staggerMainScreen
};

resource 'WIND' (131, "Dungeon") {
	{40, 40, 330, 494},
	documentProc,
	invisible,
	noGoAway,
	0x0,
	"Dungeon Map",
	staggerMainScreen
};

resource 'WIND' (132, "Menu") {
	{124, 218, 248, 394},
	documentProc,
	invisible,
	goAway,
	0x0,
	"",
	staggerMainScreen
};

resource 'WIND' (133, "Text") {
	{190, 6, 336, 306},
	documentProc,
	invisible,
	goAway,
	0x0,
	"Info",
	staggerMainScreen
};

resource 'WIND' (134, "Diagnostics") {
	{40, 40, 172, 460},
	documentProc,
	invisible,
	goAway,
	0x0,
	"Diagnostics",
	staggerMainScreen
};

resource 'WIND' (135, "TTY Window") {
	{48, 16, 322, 493},
	noGrowDocProc,
	invisible,
	noGoAway,
	0x0,
	"Dungeon Map",
	centerMainScreen
};

data 'TMPL' (128, "Nump") {
	$"0E4E 756D 6265 7220 6F66 206B 6579 734F"            /* .Number of keysO */
	$"434E 5405 2A2A 2A2A 2A4C 5354 4307 4B65"            /* CNT.*****LSTC.Ke */
	$"7963 6F64 6548 4259 5404 4368 6172 4348"            /* ycodeHBYT.CharCH */
	$"4152 052A 2A2A 2A2A 4C53 5445"                      /* AR.*****LSTE */
};

data 'TMPL' (129, "MNU#") {
	$"0B31 7374 206D 656E 7520 4944 4457 5244"            /* .1st menu IDDWRD */
	$"0A23 206F 6620 4D45 4E55 734F 434E 5405"            /* .# of MENUsOCNT. */
	$"2A2A 2A2A 2A4C 5354 4306 5265 7320 4944"            /* *****LSTC.Res ID */
	$"4457 5244 0852 6573 6572 7665 6446 5752"            /* DWRD.ReservedFWR */
	$"4405 2A2A 2A2A 2A4C 5354 45"                        /* D.*****LSTE */
};

data 'Nump' (128, "Unshifted") {
	$"000A 5979 5B6B 5C75 5668 576F 586C 5362"            /* ..Yy[k\uVhWoXlSb */
	$"546A 556E 522C"                                     /* TjUnR, */
};

data 'Nump' (129, "Shifted") {
	$"000A 5959 5B4B 5C55 5648 576F 584C 5342"            /* ..YY[K\UVHWoXLSB */
	$"544A 554E 522C"                                     /* TJUNR, */
};

data 'ALRT' (129, purgeable) {
	$"005A 006E 00EE 0192 0081 4444"                      /* .Z.n.Ó.í.ÅDD */
};

resource 'ALRT' (128) {
	{68, 68, 178, 469},
	128,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, silent,
		/* [2] */
		OK, visible, silent,
		/* [3] */
		OK, visible, silent,
		/* [4] */
		OK, visible, silent
	},
	alertPositionMainScreen
};

resource 'ALRT' (5000) {
	{74, 106, 211, 471},
	5000,
	{	/* array: 4 elements */
		/* [1] */
		OK, invisible, silent,
		/* [2] */
		OK, invisible, silent,
		/* [3] */
		OK, invisible, silent,
		/* [4] */
		OK, visible, silent
	},
	alertPositionMainScreen
};

resource 'ALRT' (5001) {
	{62, 78, 186, 434},
	5001,
	{	/* array: 4 elements */
		/* [1] */
		OK, invisible, silent,
		/* [2] */
		OK, invisible, silent,
		/* [3] */
		OK, invisible, silent,
		/* [4] */
		OK, visible, silent
	},
	alertPositionMainScreen
};

resource 'DITL' (129, purgeable) {
	{	/* array DITLarray: 3 elements */
		/* [1] */
		{120, 198, 138, 272},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{10, 70, 115, 272},
		StaticText {
			disabled,
			"Could not ^2 because ^0.  ^1"
		},
		/* [3] */
		{10, 20, 42, 52},
		Icon {
			disabled,
			0
		}
	}
};

resource 'DITL' (128) {
	{	/* array DITLarray: 2 elements */
		/* [1] */
		{80, 324, 100, 382},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{6, 56, 70, 380},
		StaticText {
			disabled,
			"^0"
		}
	}
};

resource 'DITL' (130) {
	{	/* array DITLarray: 6 elements */
		/* [1] */
		{106, 249, 126, 307},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{106, 165, 126, 223},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{10, 85, 58, 315},
		StaticText {
			disabled,
			"^0"
		},
		/* [4] */
		{68, 87, 84, 310},
		EditText {
			enabled,
			""
		},
		/* [5] */
		{10, 28, 42, 60},
		Icon {
			disabled,
			1
		},
		/* [6] */
		{100, 244, 132, 276},
		UserItem {
			disabled
		}
	}
};

resource 'DITL' (136) {
	{	/* array DITLarray: 8 elements */
		/* [1] */
		{99, 301, 119, 359},
		Button {
			enabled,
			"Yes"
		},
		/* [2] */
		{99, 231, 119, 289},
		Button {
			enabled,
			"No"
		},
		/* [3] */
		{99, 161, 119, 219},
		EditText {
			disabled,
			""
		},
		/* [4] */
		{99, 91, 119, 149},
		Button {
			enabled,
			"All"
		},
		/* [5] */
		{99, 21, 119, 79},
		Button {
			enabled,
			"Quit"
		},
		/* [6] */
		{11, 76, 84, 358},
		StaticText {
			disabled,
			"^0"
		},
		/* [7] */
		{12, 20, 44, 52},
		Icon {
			disabled,
			1
		},
		/* [8] */
		{94, 296, 126, 328},
		UserItem {
			disabled
		}
	}
};

resource 'DITL' (133) {
	{	/* array DITLarray: 5 elements */
		/* [1] */
		{99, 301, 119, 359},
		Button {
			enabled,
			"Yes"
		},
		/* [2] */
		{99, 231, 119, 289},
		Button {
			enabled,
			"No"
		},
		/* [3] */
		{11, 76, 84, 358},
		StaticText {
			disabled,
			"^0"
		},
		/* [4] */
		{12, 20, 44, 52},
		Icon {
			disabled,
			1
		},
		/* [5] */
		{93, 296, 125, 328},
		UserItem {
			disabled
		}
	}
};

resource 'DITL' (134) {
	{	/* array DITLarray: 6 elements */
		/* [1] */
		{99, 301, 119, 359},
		Button {
			enabled,
			"Yes"
		},
		/* [2] */
		{99, 231, 119, 289},
		Button {
			enabled,
			"No"
		},
		/* [3] */
		{99, 161, 119, 219},
		Button {
			enabled,
			"Quit"
		},
		/* [4] */
		{11, 76, 84, 358},
		StaticText {
			disabled,
			"^0"
		},
		/* [5] */
		{12, 20, 44, 52},
		Icon {
			disabled,
			1
		},
		/* [6] */
		{93, 296, 125, 328},
		UserItem {
			disabled
		}
	}
};

resource 'DITL' (135) {
	{	/* array DITLarray: 7 elements */
		/* [1] */
		{99, 301, 119, 359},
		Button {
			enabled,
			"Yes"
		},
		/* [2] */
		{99, 231, 119, 289},
		Button {
			enabled,
			"No"
		},
		/* [3] */
		{99, 161, 119, 219},
		Button {
			enabled,
			"All"
		},
		/* [4] */
		{99, 91, 119, 149},
		Button {
			enabled,
			"Quit"
		},
		/* [5] */
		{11, 76, 84, 358},
		StaticText {
			disabled,
			"^0"
		},
		/* [6] */
		{12, 20, 44, 52},
		Icon {
			disabled,
			1
		},
		/* [7] */
		{95, 297, 127, 329},
		UserItem {
			disabled
		}
	}
};

resource 'DITL' (137) {
	{	/* array DITLarray: 6 elements */
		/* [1] */
		{66, 77, 86, 135},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{66, 11, 86, 69},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{7, 52, 55, 212},
		StaticText {
			disabled,
			"^0"
		},
		/* [4] */
		{68, 152, 84, 204},
		EditText {
			enabled,
			"Edit Text"
		},
		/* [5] */
		{8, 10, 40, 42},
		Icon {
			disabled,
			1
		},
		/* [6] */
		{61, 73, 93, 105},
		UserItem {
			disabled
		}
	}
};

resource 'DITL' (5000) {
	{	/* array DITLarray: 3 elements */
		/* [1] */
		{103, 260, 123, 340},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{11, 81, 88, 352},
		StaticText {
			disabled,
			"^0^1"
		},
		/* [3] */
		{10, 24, 42, 56},
		Icon {
			disabled,
			1
		}
	}
};

resource 'DITL' (5001) {
	{	/* array DITLarray: 4 elements */
		/* [1] */
		{93, 270, 113, 330},
		Button {
			enabled,
			"No"
		},
		/* [2] */
		{93, 93, 113, 153},
		Button {
			enabled,
			"Yes"
		},
		/* [3] */
		{10, 79, 80, 339},
		StaticText {
			disabled,
			"^0"
		},
		/* [4] */
		{10, 22, 42, 54},
		Icon {
			disabled,
			0
		}
	}
};

resource 'DITL' (6000) {
	{	/* array DITLarray: 17 elements */
		/* [1] */
		{40, 274, 60, 354},
		Button {
			enabled,
			"Play"
		},
		/* [2] */
		{78, 274, 98, 354},
		Button {
			enabled,
			"Quit"
		},
		/* [3] */
		{36, 270, 64, 358},
		UserItem {
			disabled
		},
		/* [4] */
		{80, 72, 97, 122},
		UserItem {
			enabled
		},
		/* [5] */
		{80, 186, 97, 238},
		UserItem {
			enabled
		},
		/* [6] */
		{120, 72, 137, 122},
		UserItem {
			enabled
		},
		/* [7] */
		{120, 188, 137, 238},
		UserItem {
			enabled
		},
		/* [8] */
		{120, 318, 137, 353},
		UserItem {
			enabled
		},
		/* [9] */
		{41, 74, 57, 240},
		EditText {
			enabled,
			"Name"
		},
		/* [10] */
		{24, 20, 56, 52},
		Icon {
			disabled,
			1
		},
		/* [11] */
		{80, 36, 96, 72},
		StaticText {
			disabled,
			"Role:"
		},
		/* [12] */
		{80, 150, 96, 186},
		StaticText {
			disabled,
			"Race:"
		},
		/* [13] */
		{120, 20, 136, 72},
		StaticText {
			disabled,
			"Gender:"
		},
		/* [14] */
		{120, 150, 136, 188},
		StaticText {
			disabled,
			"Align:"
		},
		/* [15] */
		{120, 274, 136, 318},
		StaticText {
			disabled,
			"Mode:"
		},
		/* [16] */
		{19, 72, 36, 242},
		StaticText {
			disabled,
			"Who are you?"
		},
		/* [17] */
		{0, 0, 0, 0},
		HelpItem {
			enabled,
			HMScanhdlg {
				6000
			}
		}
	}
};

resource 'STR#' (128, "Misc. Strings", purgeable) {
	{	/* array StringArray: 1 elements */
		/* [1] */
		"Mac Slash'EM Help…/?"
	}
};

resource 'STR#' (131, "Keypress") {
	{	/* array StringArray: 13 elements */
		/* [1] */
		"•201 Control Keys",
		/* [2] */
		"•202 Punctuation",
		/* [3] */
		"•203 Brackets",
		/* [4] */
		"•204 a - m",
		/* [5] */
		"•205 n - z",
		/* [6] */
		"•206 A - M",
		/* [7] */
		"•207 N - Z",
		/* [8] */
		"•208 0 - 9",
		/* [9] */
		"•-",
		/* [10] */
		"\0x1B•escape",
		/* [11] */
		" •space",
		/* [12] */
		"\b•delete",
		/* [13] */
		"\n•return"
	}
};

resource 'STR#' (133, "Information") {
	{	/* array StringArray: 14 elements */
		/* [1] */
		"i",
		/* [2] */
		"I",
		/* [3] */
		"#a•djust",
		/* [4] */
		"•-",
		/* [5] */
		":",
		/* [6] */
		";",
		/* [7] */
		"/",
		/* [8] */
		"^",
		/* [9] */
		"•-",
		/* [10] */
		"C",
		/* [11] */
		"#n•ame",
		/* [12] */
		"\\",
		/* [13] */
		"•-",
		/* [14] */
		"#co•nduct"
	}
};

resource 'STR#' (134, "Equipment") {
	{	/* array StringArray: 15 elements */
		/* [1] */
		"•209 Current",
		/* [2] */
		"•-",
		/* [3] */
		"w",
		/* [4] */
		"t",
		/* [5] */
		"a",
		/* [6] */
		"•-",
		/* [7] */
		"W",
		/* [8] */
		"T",
		/* [9] */
		"A",
		/* [10] */
		"•-",
		/* [11] */
		"P",
		/* [12] */
		"R",
		/* [13] */
		"•-",
		/* [14] */
		"f",
		/* [15] */
		"Q"
	}
};

resource 'STR#' (136, "Magic") {
	{	/* array StringArray: 17 elements */
		/* [1] */
		"d",
		/* [2] */
		"D",
		/* [3] */
		",",
		/* [4] */
		"@",
		/* [5] */
		"•-",
		/* [6] */
		"e",
		/* [7] */
		"r",
		/* [8] */
		"q",
		/* [9] */
		"#d•ip",
		/* [10] */
		"•-",
		/* [11] */
		"+",
		/* [12] */
		"Z",
		/* [13] */
		"z",
		/* [14] */
		"#i•nvoke",
		/* [15] */
		"#ru•b",
		/* [16] */
		"•-",
		/* [17] */
		"#y•oupoly"
	}
};

resource 'STR#' (137, "Bits") {
	{	/* array StringArray: 16 elements */
		/* [1] */
		"s",
		/* [2] */
		"c",
		/* [3] */
		"o",
		/* [4] */
		"a",
		/* [5] */
		"\0x04•ctl-d",
		/* [6] */
		"•-",
		/* [7] */
		"#u•ntrap",
		/* [8] */
		"#f•orce",
		/* [9] */
		"#l•oot",
		/* [10] */
		"•-",
		/* [11] */
		"E",
		/* [12] */
		"#d•ip",
		/* [13] */
		"#s•it",
		/* [14] */
		"•-",
		/* [15] */
		"<",
		/* [16] */
		">"
	}
};

resource 'STR#' (201, "control keys") {
	{	/* array StringArray: 13 elements */
		/* [1] */
		"\0x02•ctl-b",
		/* [2] */
		"\r•ctl-j",
		/* [3] */
		"\0x0E•ctl-n",
		/* [4] */
		"\b•ctl-h",
		/* [5] */
		"\f•ctl-l",
		/* [6] */
		"\0x19•ctl-y",
		/* [7] */
		"\v•ctl-k",
		/* [8] */
		"\0x15•ctl-u",
		/* [9] */
		"•-",
		/* [10] */
		"\0x04•ctl-d",
		/* [11] */
		"\0x10•ctl-p",
		/* [12] */
		"\0x12•ctl-r",
		/* [13] */
		"\0x14•ctl-t"
	}
};

resource 'STR#' (202, "punctuation") {
	{	/* array StringArray: 16 elements */
		/* [1] */
		".",
		/* [2] */
		",",
		/* [3] */
		";",
		/* [4] */
		":",
		/* [5] */
		"!",
		/* [6] */
		"?",
		/* [7] */
		"+",
		/* [8] */
		"-",
		/* [9] */
		"=",
		/* [10] */
		"#",
		/* [11] */
		"$",
		/* [12] */
		"@",
		/* [13] */
		"&",
		/* [14] */
		"*",
		/* [15] */
		"~",
		/* [16] */
		"_"
	}
};

resource 'STR#' (203, "brackets") {
	{	/* array StringArray: 16 elements */
		/* [1] */
		"[",
		/* [2] */
		"]",
		/* [3] */
		"(",
		/* [4] */
		")",
		/* [5] */
		"{",
		/* [6] */
		"}",
		/* [7] */
		"<",
		/* [8] */
		">",
		/* [9] */
		"^",
		/* [10] */
		"`",
		/* [11] */
		"'",
		/* [12] */
		"\"",
		/* [13] */
		"\\",
		/* [14] */
		"/",
		/* [15] */
		"|",
		/* [16] */
		"%"
	}
};

resource 'STR#' (204, "a - m") {
	{	/* array StringArray: 13 elements */
		/* [1] */
		"a",
		/* [2] */
		"b",
		/* [3] */
		"c",
		/* [4] */
		"d",
		/* [5] */
		"e",
		/* [6] */
		"f",
		/* [7] */
		"g",
		/* [8] */
		"h",
		/* [9] */
		"i",
		/* [10] */
		"j",
		/* [11] */
		"k",
		/* [12] */
		"l",
		/* [13] */
		"m"
	}
};

resource 'STR#' (205, "n - z") {
	{	/* array StringArray: 13 elements */
		/* [1] */
		"n",
		/* [2] */
		"o",
		/* [3] */
		"p",
		/* [4] */
		"q",
		/* [5] */
		"r",
		/* [6] */
		"s",
		/* [7] */
		"t",
		/* [8] */
		"u",
		/* [9] */
		"v",
		/* [10] */
		"w",
		/* [11] */
		"x",
		/* [12] */
		"y",
		/* [13] */
		"z"
	}
};

resource 'STR#' (206, "A - M") {
	{	/* array StringArray: 13 elements */
		/* [1] */
		"A",
		/* [2] */
		"B",
		/* [3] */
		"C",
		/* [4] */
		"D",
		/* [5] */
		"E",
		/* [6] */
		"F",
		/* [7] */
		"G",
		/* [8] */
		"H",
		/* [9] */
		"I",
		/* [10] */
		"J",
		/* [11] */
		"K",
		/* [12] */
		"L",
		/* [13] */
		"M"
	}
};

resource 'STR#' (207, "N - Z") {
	{	/* array StringArray: 13 elements */
		/* [1] */
		"N",
		/* [2] */
		"O",
		/* [3] */
		"P",
		/* [4] */
		"Q",
		/* [5] */
		"R",
		/* [6] */
		"S",
		/* [7] */
		"T",
		/* [8] */
		"U",
		/* [9] */
		"V",
		/* [10] */
		"W",
		/* [11] */
		"X",
		/* [12] */
		"Y",
		/* [13] */
		"Z"
	}
};

resource 'STR#' (208, "0 - 9") {
	{	/* array StringArray: 10 elements */
		/* [1] */
		"0",
		/* [2] */
		"1",
		/* [3] */
		"2",
		/* [4] */
		"3",
		/* [5] */
		"4",
		/* [6] */
		"5",
		/* [7] */
		"6",
		/* [8] */
		"7",
		/* [9] */
		"8",
		/* [10] */
		"9"
	}
};

resource 'STR#' (200, "wizard") {
	{	/* array StringArray: 19 elements */
		/* [1] */
		"\0x18•ctl-x",
		/* [2] */
		"\0x05•ctl-e",
		/* [3] */
		"\0x06•ctl-f",
		/* [4] */
		"\0x07•ctl-g",
		/* [5] */
		"\t•ctl-i",
		/* [6] */
		"\0x0F•ctl-o",
		/* [7] */
		"\0x16•ctl-v",
		/* [8] */
		"\0x17•ctl-w",
		/* [9] */
		"•-",
		/* [10] */
		"\0x03•ctl-c",
		/* [11] */
		"\r•ctl-j",
		/* [12] */
		"\0x0E•ctl-n",
		/* [13] */
		"•-",
		/* [14] */
		"#li•ght sources",
		/* [15] */
		"#se•env",
		/* [16] */
		"#st•ats",
		/* [17] */
		"#ti•meout",
		/* [18] */
		"#vi•sion",
		/* [19] */
		"#wm•ode"
	}
};

resource 'STR#' (209, "current") {
	{	/* array StringArray: 9 elements */
		/* [1] */
		")",
		/* [2] */
		"[",
		/* [3] */
		"=",
		/* [4] */
		"\"",
		/* [5] */
		"(",
		/* [6] */
		"$",
		/* [7] */
		"+",
		/* [8] */
		"•-",
		/* [9] */
		"*"
	}
};

resource 'STR#' (132, "Help") {
	{	/* array StringArray: 8 elements */
		/* [1] */
		"O",
		/* [2] */
		"•-",
		/* [3] */
		"?",
		/* [4] */
		"&",
		/* [5] */
		"•-",
		/* [6] */
		"v",
		/* [7] */
		"V",
		/* [8] */
		"#v•ersion"
	}
};

resource 'STR#' (135, "Action") {
	{	/* array StringArray: 20 elements */
		/* [1] */
		".",
		/* [2] */
		"•-",
		/* [3] */
		"\0x14• ctl-t",
		/* [4] */
		"#j•ump",
		/* [5] */
		"#m•onster",
		/* [6] */
		"#w•ipe",
		/* [7] */
		"•-",
		/* [8] */
		"a",
		/* [9] */
		"p",
		/* [10] */
		"\0x04•ctl-d",
		/* [11] */
		"#ch•at",
		/* [12] */
		"#o•ffer",
		/* [13] */
		"#p•ray",
		/* [14] */
		"#tu•rn",
		/* [15] */
		"•-",
		/* [16] */
		"#b•orrow",
		/* [17] */
		"#te•chnique",
		/* [18] */
		"x",
		/* [19] */
		"#2•weapon",
		/* [20] */
		"#en•hance"
	}
};

resource 'ics4' (1000) {
	$"0000 0000 0000 0000 0000 000F FF00 0000"
	$"00FF 00F1 01F0 0000 00FB F00F 101F 0000"
	$"000F BF0F 0101 F000 0000 FBFF 1010 F000"
	$"00F0 0FBF FFF1 F000 0F0F FFFB F00F 0000"
	$"0F10 10FF BF00 0000 0F01 01F0 FBF0 0000"
	$"00F0 10F0 0FBF 0000 000F 010F 00FB F000"
	$"0000 FFF0 000F BF00 0000 0000 0000 FBF0"
	$"0000 0000 0000 0FBF 0000 0000 0000 00FF"
};

resource 'ics4' (1001) {
	$"0FFF FFFF FFFF 0000 0F0C 0C0C 0C0F F000"
	$"0FC0 C0C0 C0CF CF00 0F0C 0C0F FF0F FFF0"
	$"0FCF F0CF 10F0 C0F0 0F0F BF0F 010F 0CF0"
	$"0FC0 FBFF 101F C0F0 0F0C 0FBF FFFF 0CF0"
	$"0FFF FFFB F0C0 C0F0 0FF1 01FF BF0C 0CF0"
	$"0FF0 10F0 FBF0 C0F0 0F0F 01FC 0FBF 0CF0"
	$"0FC0 FFF0 C0FB F0F0 0F0C 0C0C 0C0F BFF0"
	$"0FC0 C0C0 C0C0 FBF0 0FFF FFFF FFFF FFF0"
};

resource 'ics4' (1002) {
	$"0000 0000 0000 0000 0FFF FFFF FFFF FF00"
	$"0FCC CCFF FCCC CFF0 0FCF FCF1 0FCC CFF0"
	$"0FCF BFF0 10FC CFF0 0FCC FBF1 01FC CFF0"
	$"0FFF FFBF FFFC CFF0 0FF1 01FB FCCC CFF0"
	$"0FF0 10FF BFCC CFF0 0FCF 01FC FBFC CFF0"
	$"0FCC FFFC CFBF CFF0 0FCC CCCC CCFB FFF0"
	$"0FCC CCCC CCCF BFF0 0FCC CCCC CCCC FFF0"
	$"0FFF FFFF FFFF FFF0 00FF FFFF FFFF FFF0"
};

resource 'ics4' (1003, purgeable) {
	$"0FFF FFFF FFFF 0000 0F0C 0C0C 0C0F F000"
	$"0FCF FFC0 FFFF CF00 0F0C 0C0F FF0F FFF0"
	$"0FCF F0CF 10F0 C0F0 0F0F BF0F 010F 0CF0"
	$"0FC0 FBFF 101F C0F0 0F0C 0FBF FFFF 0CF0"
	$"0FFF FFFB F0C0 C0F0 0FF1 01FF BF0F FCF0"
	$"0FF0 10F0 FBF0 C0F0 0F0F 01FC 0FBF 0CF0"
	$"0FC0 FFF0 C0FB F0F0 0F0F 0C0F FF0F BFF0"
	$"0FC0 C0C0 C0C0 FBF0 0FFF FFFF FFFF FFF0"
};

resource 'ics4' (1004, purgeable) {
	$"0FFF FFFF FFFF 0000 0F0C 0C0F 0C0F F000"
	$"0FCF C0FB F0CF CF00 0FF1 FCFB FCFF FFF0"
	$"0FF0 1FFB FF10 F0F0 0FF1 01FB F101 FCF0"
	$"0FF0 1FFB FF10 F0F0 0FF1 FCFB FCF1 FCF0"
	$"0FCF C0FB F0CF C0F0 0F0C 0CFB FC0C 0CF0"
	$"0FC0 F0FB F0F0 C0F0 0F0C 0FFB FF0C 0CF0"
	$"0FC0 C0FB F0C0 C0F0 0F0C 0FFB FF0C 0CF0"
	$"0FC0 F0FB F0F0 C0F0 0FFF FFFF FFFF FFF0"
};

resource 'ics#' (1000) {
	{	/* array: 2 elements */
		/* [1] */
		$"0000 01C0 33E0 29F0 15F8 0BF8 25F8 7E90"
		$"7F40 7EA0 3E50 1F28 0E14 000A 0005 0003",
		/* [2] */
		$"01C0 73E0 7FF0 7FF8 3FFC 3FFC 7FFC FFF8"
		$"FFF0 FFF0 7FF8 3FFC 1F3E 0E1F 000F 0007"
	}
};

resource 'ics#' (1001) {
	{	/* array: 2 elements */
		/* [1] */
		$"7FF0 4018 4014 41DE 59E2 55F2 4BF2 45F2"
		$"7E82 7F42 7EA2 5E52 4E2A 4016 400A 7FFE",
		/* [2] */
		$"7FF0 7FF8 7FFC 7FFE 7FFE 7FFE 7FFE 7FFE"
		$"7FFE 7FFE 7FFE 7FFE 7FFE 7FFE 7FFE 7FFE"
	}
};

resource 'ics#' (1002) {
	{	/* array: 2 elements */
		/* [1] */
		$"0000 7FFC 4386 5BC6 5FE6 4FE6 7FE6 7F86"
		$"7FC6 5EE6 4E76 403E 401E 400E 7FFE 3FFE",
		/* [2] */
		$"0000 7FFC 7FFE 7FFE 7FFE 7FFE 7FFE 7FFE"
		$"7FFE 7FFE 7FFE 7FFE 7FFE 7FFE 7FFE 3FFE"
	}
};

resource 'ics#' (1003, purgeable) {
	{	/* array: 2 elements */
		/* [1] */
		$"7FF0 4018 5CF4 41DE 59E2 5DF2 4FF2 47F2"
		$"7F82 7FDA 7EE2 5E72 4E3A 51DE 400E 7FFE",
		/* [2] */
		$"7FF0 7FF8 7FFC 7FFE 7FFE 7FFE 7FFE 7FFE"
		$"7FFE 7FFE 7FFE 7FFE 7FFE 7FFE 7FFE 7FFE"
	}
};

resource 'ics#' (1004, purgeable) {
	{	/* array: 2 elements */
		/* [1] */
		$"7FF0 4018 5394 7ABE 7EFA 7EFA 7EFA 7ABA"
		$"5292 4282 4AA2 46C2 4282 46C2 4AA2 7FFE",
		/* [2] */
		$"7FF0 7FF8 7FFC 7FFE 7FFE 7FFE 7FFE 7FFE"
		$"7FFE 7FFE 7FFE 7FFE 7FFE 7FFE 7FFE 7FFE"
	}
};

resource 'ICN#' (1000) {
	{	/* array: 2 elements */
		/* [1] */
		$"0000 0000 000F 8000 003F E000 103F F000"
		$"1C1F F800 0A1F F800 091F FC00 049F FC00"
		$"027F FC00 013F FC00 009F FC00 30CF FC00"
		$"7FE6 1800 7FF2 0000 7FF9 0000 7FFC 8000"
		$"7FF2 4000 7FF1 2000 3FF0 9000 3FF0 4800"
		$"1FF8 2400 0FF8 1200 03E0 0900 0000 0480"
		$"0000 0240 0000 0120 0000 0090 0000 0048"
		$"0000 0024 0000 0012 0000 000A 0000 0004",
		/* [2] */
		$"000F 8000 003F E000 107F F000 3C7F F800"
		$"3E3F FC00 1F3F FC00 1FBF FE00 0FFF FE00"
		$"07FF FE00 03FF FE00 31FF FE00 7FFF FE00"
		$"FFFF FC00 FFFF 1800 FFFF 8000 FFFF C000"
		$"FFFF E000 FFFB F000 7FF9 F800 7FF8 FC00"
		$"3FFC 7E00 1FFC 3F00 0FF8 1F80 03E0 0FC0"
		$"0000 07E0 0000 03F0 0000 01F8 0000 00FC"
		$"0000 007E 0000 003F 0000 001F 0000 000E"
	}
};

resource 'ICN#' (1001) {
	{	/* array: 2 elements */
		/* [1] */
		$"1FFF FE00 1000 0300 1000 0280 1000 0240"
		$"1000 0220 1000 0210 1003 E208 1007 FBF8"
		$"1187 FC08 1143 FC08 1123 FE08 1093 FE08"
		$"104F FE08 1027 FE08 1013 FE08 1319 8C08"
		$"17FC 8008 17FE 4008 17FF 2008 17FC 9008"
		$"17FC 4808 13FC 2408 13FE 1208 11FE 0908"
		$"107C 0488 1000 0248 1000 0128 1000 0098"
		$"1000 0048 1000 0028 1000 0018 1FFF FFF8",
		/* [2] */
		$"1FFF FE00 1FFF FF00 1FFF FF80 1FFF FFC0"
		$"1FFF FFE0 1FFF FFF0 1FFF FFF8 1FFF FFF8"
		$"1FFF FFF8 1FFF FFF8 1FFF FFF8 1FFF FFF8"
		$"1FFF FFF8 1FFF FFF8 1FFF FFF8 1FFF FFF8"
		$"1FFF FFF8 1FFF FFF8 1FFF FFF8 1FFF FFF8"
		$"1FFF FFF8 1FFF FFF8 1FFF FFF8 1FFF FFF8"
		$"1FFF FFF8 1FFF FFF8 1FFF FFF8 1FFF FFF8"
		$"1FFF FFF8 1FFF FFF8 1FFF FFF8 1FFF FFF8"
	}
};

resource 'ICN#' (1002) {
	{	/* array: 2 elements */
		/* [1] */
		$"0000 0000 0000 0000 3FFF FFF0 2000 0018"
		$"2007 C018 200F F018 230F F818 2287 F818"
		$"2247 FC18 2127 FC18 209F FC18 204F FC18"
		$"2027 FC18 2633 1818 2FF9 0018 2FFC 8018"
		$"2FFE 4018 2FF9 2018 2FF8 9018 27F8 4818"
		$"27FC 2418 23FC 1218 20F8 0918 2000 0498"
		$"2000 0258 2000 0138 2000 0098 2000 0058"
		$"2000 0038 3FFF FFF8 1FFF FFF8",
		/* [2] */
		$"0000 0000 0000 0000 3FFF FFF0 3FFF FFF8"
		$"3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8"
		$"3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8"
		$"3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8"
		$"3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8"
		$"3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8"
		$"3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8"
		$"3FFF FFF8 3FFF FFF8 1FFF FFF8"
	}
};

resource 'ICN#' (1003, purgeable) {
	{	/* array: 2 elements */
		/* [1] */
		$"1FFF FE00 1000 0300 1000 0280 173D F240"
		$"1000 0220 1000 0210 17BB E208 1007 FBF8"
		$"1187 FC08 177F FF88 1123 FE08 1093 FE08"
		$"17CF FFC8 1027 FE08 1013 FE08 1319 8CC8"
		$"17FC 8008 17FE 4008 17FF 3BC8 17FC 9008"
		$"17FC 4808 13FF E768 13FE 1208 11FE 0908"
		$"147D DC88 1000 0248 1000 0128 17CF E098"
		$"1000 0048 1000 0028 1000 0018 1FFF FFF8",
		/* [2] */
		$"1FFF FE00 1FFF FF00 1FFF FF80 1FFF FFC0"
		$"1FFF FFE0 1FFF FFF0 1FFF FFF8 1FFF FFF8"
		$"1FFF FFF8 1FFF FFF8 1FFF FFF8 1FFF FFF8"
		$"1FFF FFF8 1FFF FFF8 1FFF FFF8 1FFF FFF8"
		$"1FFF FFF8 1FFF FFF8 1FFF FFF8 1FFF FFF8"
		$"1FFF FFF8 1FFF FFF8 1FFF FFF8 1FFF FFF8"
		$"1FFF FFF8 1FFF FFF8 1FFF FFF8 1FFF FFF8"
		$"1FFF FFF8 1FFF FFF8 1FFF FFF8 1FFF FFF8"
	}
};

resource 'ICN#' (1004, purgeable) {
	{	/* array: 2 elements */
		/* [1] */
		$"1FFF FE00 1000 0300 1101 0280 1302 8240"
		$"1382 8220 1782 8210 17C2 87F8 17F2 9FC8"
		$"1FFE FFE8 1FFE FFE8 1FFE FFE8 1FFE FFE8"
		$"1FFE FFE8 17F2 9FC8 17C2 87C8 1782 83C8"
		$"1382 8388 133A B988 117A BD08 107E FC08"
		$"107E FC08 101E F008 100E E008 1006 C008"
		$"100E E008 101E F008 107E FC08 107E FC08"
		$"107A BC08 103A B808 1002 8008 1FFF FFF8",
		/* [2] */
		$"1FFF FE00 1FFF FF00 1FFF FF80 1FFF FFC0"
		$"1FFF FFE0 1FFF FFF0 1FFF FFF8 1FFF FFF8"
		$"1FFF FFF8 1FFF FFF8 1FFF FFF8 1FFF FFF8"
		$"1FFF FFF8 1FFF FFF8 1FFF FFF8 1FFF FFF8"
		$"1FFF FFF8 1FFF FFF8 1FFF FFF8 1FFF FFF8"
		$"1FFF FFF8 1FFF FFF8 1FFF FFF8 1FFF FFF8"
		$"1FFF FFF8 1FFF FFF8 1FFF FFF8 1FFF FFF8"
		$"1FFF FFF8 1FFF FFF8 1FFF FFF8 1FFF FFF8"
	}
};

resource 'icl4' (1000) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 FFFF F000 0000 0000 0000"
	$"0000 0000 00FF 1010 1FF0 0000 0000 0000"
	$"000F 0000 00F1 0101 010F 0000 0000 0000"
	$"000F FF00 000F 1010 1010 F000 0000 0000"
	$"0000 FBF0 000F 0101 0101 F000 0000 0000"
	$"0000 FBBF 000F 1010 1010 1F00 0000 0000"
	$"0000 0FBB F00F 0101 0101 0F00 0000 0000"
	$"0000 00FB BFFF 1010 1010 1F00 0000 0000"
	$"0000 000F BBF1 0101 0101 0F00 0000 0000"
	$"0000 0000 FBBF 1010 1010 1F00 0000 0000"
	$"00FF 0000 FFBB F1FF FFF1 0F00 0000 0000"
	$"0F10 FFFF 10FB BFF0 000F F000 0000 0000"
	$"0F01 0101 010F BBF0 0000 0000 0000 0000"
	$"0F10 1010 1010 FBBF 0000 0000 0000 0000"
	$"0F01 0101 0101 FFBB F000 0000 0000 0000"
	$"0F10 1010 101F 00FB BF00 0000 0000 0000"
	$"0F01 0101 010F 000F BBF0 0000 0000 0000"
	$"00F0 1010 101F 0000 FBBF 0000 0000 0000"
	$"00F1 0101 010F 0000 0FBB F000 0000 0000"
	$"000F 1010 1010 F000 00FB BF00 0000 0000"
	$"0000 FF01 010F F000 000F BBF0 0000 0000"
	$"0000 00FF FFF0 0000 0000 FBBF 0000 0000"
	$"0000 0000 0000 0000 0000 0FBB F000 0000"
	$"0000 0000 0000 0000 0000 00FB BF00 0000"
	$"0000 0000 0000 0000 0000 000F BBF0 0000"
	$"0000 0000 0000 0000 0000 0000 FBBF 0000"
	$"0000 0000 0000 0000 0000 0000 0FBB F000"
	$"0000 0000 0000 0000 0000 0000 00FB BF00"
	$"0000 0000 0000 0000 0000 0000 000F BBF0"
	$"0000 0000 0000 0000 0000 0000 0000 FBF0"
	$"0000 0000 0000 0000 0000 0000 0000 0F"
};

resource 'icl4' (1001) {
	$"000F FFFF FFFF FFFF FFFF FFF0 0000 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0CFF 0000 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0FC F000 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0CFC CF00 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0FC CCF0 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0CFC CCCF 0000"
	$"000F C0C0 C0C0 C0FF FFF0 C0FC CCCC F000"
	$"000F 0C0C 0C0C 0F01 010F FCFF FFFF F000"
	$"000F C0CF F0C0 CF10 1010 FFC0 C0C0 F000"
	$"000F 0C0F BF0C 0CF1 0101 0F0C 0C0C F000"
	$"000F C0CF BBF0 C0F0 1010 10F0 C0C0 F000"
	$"000F 0C0C FBBF 0CF1 0101 01FC 0C0C F000"
	$"000F C0C0 CFBB FFF0 1010 10F0 C0C0 F000"
	$"000F 0C0C 0CFB BF01 0101 01FC 0C0C F000"
	$"000F C0C0 C0CF BBF0 FFFF 10F0 C0C0 F000"
	$"000F 0CFF 0C0F FBBF FC0C FF0C 0C0C F000"
	$"000F CF10 FFFF 1FBB F0C0 C0C0 C0C0 F000"
	$"000F 0F01 0101 01FB BF0C 0C0C 0C0C F000"
	$"000F CF10 1010 1FFF BBF0 C0C0 C0C0 F000"
	$"000F 0F01 0101 0F0C FBBF 0C0C 0C0C F000"
	$"000F CF10 1010 1FC0 CFBB F0C0 C0C0 F000"
	$"000F 0CF1 0101 0F0C 0CFB BF0C 0C0C F000"
	$"000F C0F0 1010 10F0 C0CF BBF0 C0C0 F000"
	$"000F 0C0F F101 01FC 0C0C FBBF 0C0C F000"
	$"000F C0C0 CFFF FFC0 C0C0 CFBB F0C0 F000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0CFB BF0C F000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0CF BBF0 F000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0C0C FBBF F000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0C0 CFBB F000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0C0C 0CFB F000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0C0 C0CF F000"
	$"000F FFFF FFFF FFFF FFFF FFFF FFFF F0"
};

resource 'icl4' (1002) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"00FF FFFF FFFF FFFF FFFF FFFF FFFF 0000"
	$"00FC CCCC CCCC CCCC CCCC CCCC CCCF F000"
	$"00FC CCCC CCCC CFFF FFCC CCCC CCCF F000"
	$"00FC CCCC CCCC F101 01FF CCCC CCCF F000"
	$"00FC CCFF CCCC F010 1010 FCCC CCCF F000"
	$"00FC CCFB FCCC CF01 0101 FCCC CCCF F000"
	$"00FC CCFB BFCC CF10 1010 1FCC CCCF F000"
	$"00FC CCCF BBFC CF01 0101 0FCC CCCF F000"
	$"00FC CCCC FBBF F010 1010 1FCC CCCF F000"
	$"00FC CCCC CFBB F101 0101 0FCC CCCF F000"
	$"00FC CCCC CCFB BF10 FFF0 1FCC CCCF F000"
	$"00FC CFFC CCFF BBFF CCCF FCCC CCCF F000"
	$"00FC F01F FF10 FBBF CCCC CCCC CCCF F000"
	$"00FC F101 0101 0FBB FCCC CCCC CCCF F000"
	$"00FC F010 1010 1FFB BFCC CCCC CCCF F000"
	$"00FC F101 0101 FCCF BBFC CCCC CCCF F000"
	$"00FC F010 1010 FCCC FBBF CCCC CCCF F000"
	$"00FC CF01 0101 FCCC CFBB FCCC CCCF F000"
	$"00FC CF10 1010 1FCC CCFB BFCC CCCF F000"
	$"00FC CCFF 0101 0FCC CCCF BBFC CCCF F000"
	$"00FC CCCC FFFF FCCC CCCC FBBF CCCF F000"
	$"00FC CCCC CCCC CCCC CCCC CFBB FCCF F000"
	$"00FC CCCC CCCC CCCC CCCC CCFB BFCF F000"
	$"00FC CCCC CCCC CCCC CCCC CCCF BBFF F000"
	$"00FC CCCC CCCC CCCC CCCC CCCC FBBF F000"
	$"00FC CCCC CCCC CCCC CCCC CCCC CFBF F000"
	$"00FC CCCC CCCC CCCC CCCC CCCC CCFF F000"
	$"00FF FFFF FFFF FFFF FFFF FFFF FFFF F000"
	$"000F FFFF FFFF FFFF FFFF FFFF FFFF F0"
};

resource 'icl4' (1003, purgeable) {
	$"000F FFFF FFFF FFFF FFFF FFF0 0000 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0CFF 0000 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0FC F000 0000"
	$"000F 0FFF 0CFF FF0F FFFF 0CFC CF00 0000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0FC CCF0 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0CFC CCCF 0000"
	$"000F CFFF F0FF F0FF FFF0 C0FC CCCC F000"
	$"000F 0C0C 0C0C 0F01 010F FCFF FFFF F000"
	$"000F C0CF F0C0 CF10 1010 1FC0 C0C0 F000"
	$"000F 0FFF BFFF FFF1 0101 0FFF FC0C F000"
	$"000F C0CF BBF0 C0F0 1010 10F0 C0C0 F000"
	$"000F 0C0C FBBF 0CF1 0101 01FC 0C0C F000"
	$"000F CFFF FFBB FF10 1010 10FF FFC0 F000"
	$"000F 0C0C 0CFB BF01 0101 01FC 0C0C F000"
	$"000F C0C0 C0CF BBF0 1FFF 10F0 C0C0 F000"
	$"000F 0CFF 0C0F FBBF FC0C FF0C FF0C F000"
	$"000F CF10 FFF0 1FBB F0C0 C0C0 C0C0 F000"
	$"000F 0F01 0101 01FB BF0C 0C0C 0C0C F000"
	$"000F CF10 1010 10FF BBFF F0FF FFC0 F000"
	$"000F 0F01 0101 0F0C FBBF 0C0C 0C0C F000"
	$"000F CF10 1010 1FC0 CFBB F0C0 C0C0 F000"
	$"000F 0CF1 0101 0FFF FFFB BFFF 0FFC F000"
	$"000F C0F0 1010 10F0 C0CF BBF0 C0C0 F000"
	$"000F 0C0F F101 01FC 0C0C FBBF 0C0C F000"
	$"000F CFC0 CFFF FFCF FFCF FFBB F0C0 F000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0CFB BF0C F000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0CF BBF0 F000"
	$"000F 0FFF FF0C FFFF FFFC 0C0C FBBF F000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0C0 CFBB F000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0C0C 0CFB F000"
	$"000F C0C0 C0C0 C0C0 C0C0 C0C0 C0CF F000"
	$"000F FFFF FFFF FFFF FFFF FFFF FFFF F0"
};

resource 'icl4' (1004, purgeable) {
	$"000F FFFF FFFF FFFF FFFF FFF0 0000 0000"
	$"000F 0C0C 0C0C 0C0C 0C0C 0CFF 0000 0000"
	$"000F C0CF C0C0 C0CF C0C0 C0FC F000 0000"
	$"000F 0CFF 0C0C 0CFB FC0C 0CFC CF00 0000"
	$"000F C0F0 F0C0 C0FB F0C0 C0FC CCF0 0000"
	$"000F 0F01 FC0C 0CFB FC0C 0CFC CCCF 0000"
	$"000F CF10 1FC0 C0FB F0C0 CFFF FFFF F000"
	$"000F 0F01 01FF 0CFB FC0F F101 0F0C F000"
	$"000F F010 1010 FFFB FFF0 1010 10F0 F000"
	$"000F F101 0101 01FB F101 0101 01FC F000"
	$"000F F010 1010 10FB F010 1010 10F0 F000"
	$"000F F101 0101 01FB F101 0101 01FC F000"
	$"000F F010 1010 FFFB FFF0 1010 10F0 F000"
	$"000F 0F01 01FF 0CFB FC0F F101 0F0C F000"
	$"000F CF10 1FC0 C0FB F0C0 CF10 1FC0 F000"
	$"000F 0F01 FC0C 0CFB FC0C 0CF1 0F0C F000"
	$"000F C0F0 F0C0 C0FB F0C0 C0F0 F0C0 F000"
	$"000F 0CFF 0CFF FCFB FCFF FC0F FC0C F000"
	$"000F C0CF CFF0 F0FB F0F0 FFCF C0C0 F000"
	$"000F 0C0C 0F00 1FFB FF00 1F0C 0C0C F000"
	$"000F C0C0 CFF0 00FB F000 FFC0 C0C0 F000"
	$"000F 0C0C 0C0F 00FB F01F 0C0C 0C0C F000"
	$"000F C0C0 C0C0 F0FB F0F0 C0C0 C0C0 F000"
	$"000F 0C0C 0C0C 0FFB FF0C 0C0C 0C0C F000"
	$"000F C0C0 C0C0 F0FB F0F0 C0C0 C0C0 F000"
	$"000F 0C0C 0C0F 00FB F01F 0C0C 0C0C F000"
	$"000F C0C0 CFF0 00FB F000 FFC0 C0C0 F000"
	$"000F 0C0C 0F00 1FFB FF00 1F0C 0C0C F000"
	$"000F C0C0 CFF0 F0FB F0F0 FFC0 C0C0 F000"
	$"000F 0C0C 0CFF FCFB FCFF FC0C 0C0C F000"
	$"000F C0C0 C0C0 C0FB F0C0 C0C0 C0C0 F000"
	$"000F FFFF FFFF FFFF FFFF FFFF FFFF F0"
};

resource 'DLOG' (136, "ynNaq") {
	{66, 92, 195, 471},
	movableDBoxProc,
	invisible,
	noGoAway,
	0x0,
	136,
	"",
	alertPositionParentWindowScreen
};

resource 'DLOG' (135, "ynaq") {
	{66, 92, 195, 471},
	movableDBoxProc,
	invisible,
	noGoAway,
	0x0,
	135,
	"",
	alertPositionParentWindowScreen
};

resource 'DLOG' (134, "ynq") {
	{66, 92, 195, 471},
	movableDBoxProc,
	invisible,
	noGoAway,
	0x0,
	134,
	"",
	alertPositionParentWindowScreen
};

resource 'DLOG' (133, "yn") {
	{66, 92, 195, 471},
	movableDBoxProc,
	invisible,
	noGoAway,
	0x0,
	133,
	"",
	alertPositionParentWindowScreen
};

resource 'DLOG' (137, "Small Getline") {
	{188, 154, 283, 371},
	movableDBoxProc,
	invisible,
	noGoAway,
	0x0,
	137,
	"",
	centerParentWindowScreen
};

resource 'DLOG' (130, "Prompt") {
	{90, 94, 232, 423},
	movableDBoxProc,
	invisible,
	noGoAway,
	0x0,
	130,
	"",
	alertPositionParentWindowScreen
};

resource 'DLOG' (6000, "askname") {
	{66, 46, 226, 434},
	movableDBoxProc,
	visible,
	noGoAway,
	0x0,
	6000,
	"Select a Character",
	alertPositionMainScreen
};

resource 'FONT' (19337, purgeable) {
	doExpandFont,
	proportionalFont,
	blackFont,
	notSynthetic,
	nofctbRsrc,
	oneBit,
	noCharWidthTable,
	noImageHeightTable,
	0,
	254,
	6,
	0,
	-9,
	6,
	11,
	9,
	2,
	0,
	63,
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 00D8 0000 0000 0000 1441 0000 0000"
	$"0029 4000 0030 0000 1A00 0002 1840 056B"
	$"5889 4007 E039 4420 8040 0495 C070 1C01"
	$"C214 0015 0014 208F C0FF F001 F800 0000"
	$"7380 2000 0000 0000 2000 000E 6001 0010"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0070"
	$"0050 8802 1804 0102 80A2 0004 0208 8000"
	$"0060 0041 2213 6C00 0002 2880 0001 F889"
	$"4005 A029 4420 8040 0495 4050 1401 4214"
	$"0015 0014 208F C0FF F00D 0800 0000 2440"
	$"5002 F3C0 0004 2003 0009 9006 D539 EC4C"
	$"2000 05CB 9C2F BBEE 7000 0E73 BCEF 7FEE"
	$"8F0C 6118 BBCE F3BF 18C6 31FF 1A02 0400"
	$"1018 2130 C000 0000 0200 0000 00A9 AEF9"
	$"14A1 240A 50AD 5CEF 81EA 5114 453A 2094"
	$"7C42 4425 A2EC 4002 4920 056B 5889 4005"
	$"A029 4420 8040 0495 4050 1401 4214 0015"
	$"0014 208F C0FF F013 003E 0036 745C 4005"
	$"FC20 121A 2004 8009 20F4 DFD6 B292 A900"
	$"063C 6268 4031 8820 918C 6318 C211 8A0C"
	$"A1BC C631 8C49 18C5 510D 0D01 0400 1020"
	$"2010 4000 0000 0200 0000 00AA D1F8 0000"
	$"1800 0000 2318 0280 0000 0046 2482 A240"
	$"0000 3332 0002 AA00 0001 F889 4005 A029"
	$"4420 8040 0495 4050 1401 4214 0015 0014"
	$"208F C0FF F023 0012 0009 AC62 203D FC3F"
	$"912A 240C B839 4095 8552 D421 7100 0A68"
	$"42AF 7831 8D5F 41EC 6308 C210 8A0D 215A"
	$"C631 8C09 18C4 9114 8800 7F9C F773 FD32"
	$"7D67 79F6 7FC6 358C 7EA8 1053 9EF7 BDCE"
	$"7392 6318 3A8E 73A3 18C6 2EE7 E2E7 CE8D"
	$"AB32 4001 5560 056B 58F9 41FD BFE9 7C20"
	$"8040 04F5 7FDF F7FF 7F95 F815 F815 F88F"
	$"C0FF F765 07C8 FC88 AFE2 EDCB FC20 7CC8"
	$"2073 7C29 FE97 8FB9 4821 AFCF 8AA8 8D20"
	$"C44E 8880 22AF FD08 FBD3 FA0E 2119 C7D1"
	$"F389 18C4 8A24 8800 8C63 18A4 6334 6B98"
	$"C639 8246 3554 4504 1054 6318 C631 8C52"
	$"7FFF 57D1 8C63 18C6 3581 3C48 D18E 66EC"
	$"8FFE 1969 9001 FF8F 7E31 B18F C7FF FFFF"
	$"FF97 330C 330C 207E 07FF 27FE 27FF FFFF"
	$"F8A5 0285 1488 AC63 1ACB FC3F 9128 3F88"
	$"7C40 0E90 8515 B521 2100 1329 03F0 C491"
	$"785F 44F4 6308 C211 8A8D 2118 C611 8849"
	$"18D4 8444 4800 8C61 1FA4 633C 6B18 C630"
	$"7246 3524 48A8 1077 E318 C61F FFD2 6318"
	$"7E91 8C63 18C6 3487 E448 D18C 6201 0804"
	$"21F2 4D6B 58F9 4BFD BDF8 3C40 0240 04F5"
	$"F7FF 77FF 7F81 F941 FD15 F84F FFFC 08A3"
	$"0289 1488 7463 1DD3 FC20 1218 2070 7F40"
	$"0E90 0056 B212 0100 122A 2228 C491 0820"
	$"8084 6318 C211 8A8C A118 C611 8C49 156D"
	$"4484 4800 9C61 1824 6332 6B18 C630 0A4D"
	$"5554 50A8 1124 2739 CE10 8412 6318 5291"
	$"8C67 38C6 3589 2E49 D19C 63FF 1804 4072"
	$"4801 F889 4A25 A500 0040 0240 0494 1401"
	$"5401 4000 2140 2514 204F FFFC 08A3 0293"
	$"1488 2455 1053 FC3F 8008 2480 3880 0090"
	$"803A 4D0C 0010 61CF DC27 388E 7500 0474"
	$"7CEF 7E0E 8F74 7F18 BA0E 8B88 E246 24FE"
	$"38FC 6F9E F7A3 E331 6B17 79F0 F1B4 8A8B"
	$"FEA8 0E23 DAD6 B5EF 7BD2 631F DEEE 739A"
	$"D7B9 CEF1 2446 CE6C 6200 E000 7069 956B"
	$"5889 4A25 A500 0040 0240 0494 1401 5401"
	$"4000 2140 2514 204F FFFC 076D 02BE E748"
	$"73B6 E03D FC20 7FF9 2000 0080 00F0 0010"
	$"0000 0010 2000 0000 0000 0100 0000 0000"
	$"0000 0000 0000 0001 0000 0000 0000 2000"
	$"0000 0000 4020 0000 4020 0000 0000 4070"
	$"0270 0000 0080 0000 0000 0000 0000 0080"
	$"0400 0640 0000 0000 0000 0000 0001 F889"
	$"4A25 A500 0040 0240 0494 1401 5401 4000"
	$"2140 2514 204F FFFC 0020 0000 0400 0000"
	$"0021 FFE0 0009 2000 0000 0000 0000 0000"
	$"0020 0000 0000 0000 0200 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0003 80C0 0000 4020 0000 0003 8000 0400"
	$"0000 0100 0000 0000 0000 0000 0700 0000"
	$"0180 0000 0000 0000 0000 056B 5889 4A27"
	$"E700 0040 0240 0494 1C01 DC01 C000 2140"
	$"2514 204F FFFC 0000 0000 0400 0000 0040"
	$"0000 0008 C000 0000 0000",
	{	/* array: 257 elements */
		/* [1] */
		0,
		/* [2] */
		0,
		/* [3] */
		0,
		/* [4] */
		0,
		/* [5] */
		0,
		/* [6] */
		0,
		/* [7] */
		0,
		/* [8] */
		0,
		/* [9] */
		0,
		/* [10] */
		0,
		/* [11] */
		0,
		/* [12] */
		0,
		/* [13] */
		0,
		/* [14] */
		0,
		/* [15] */
		0,
		/* [16] */
		0,
		/* [17] */
		0,
		/* [18] */
		0,
		/* [19] */
		0,
		/* [20] */
		0,
		/* [21] */
		0,
		/* [22] */
		0,
		/* [23] */
		0,
		/* [24] */
		0,
		/* [25] */
		0,
		/* [26] */
		0,
		/* [27] */
		0,
		/* [28] */
		0,
		/* [29] */
		0,
		/* [30] */
		0,
		/* [31] */
		0,
		/* [32] */
		0,
		/* [33] */
		0,
		/* [34] */
		0,
		/* [35] */
		1,
		/* [36] */
		4,
		/* [37] */
		9,
		/* [38] */
		14,
		/* [39] */
		19,
		/* [40] */
		24,
		/* [41] */
		26,
		/* [42] */
		29,
		/* [43] */
		32,
		/* [44] */
		37,
		/* [45] */
		42,
		/* [46] */
		44,
		/* [47] */
		49,
		/* [48] */
		50,
		/* [49] */
		54,
		/* [50] */
		59,
		/* [51] */
		61,
		/* [52] */
		66,
		/* [53] */
		71,
		/* [54] */
		76,
		/* [55] */
		81,
		/* [56] */
		86,
		/* [57] */
		91,
		/* [58] */
		96,
		/* [59] */
		101,
		/* [60] */
		102,
		/* [61] */
		104,
		/* [62] */
		107,
		/* [63] */
		112,
		/* [64] */
		115,
		/* [65] */
		120,
		/* [66] */
		125,
		/* [67] */
		130,
		/* [68] */
		135,
		/* [69] */
		140,
		/* [70] */
		145,
		/* [71] */
		150,
		/* [72] */
		155,
		/* [73] */
		160,
		/* [74] */
		165,
		/* [75] */
		168,
		/* [76] */
		173,
		/* [77] */
		178,
		/* [78] */
		183,
		/* [79] */
		188,
		/* [80] */
		193,
		/* [81] */
		198,
		/* [82] */
		203,
		/* [83] */
		208,
		/* [84] */
		213,
		/* [85] */
		218,
		/* [86] */
		223,
		/* [87] */
		228,
		/* [88] */
		233,
		/* [89] */
		238,
		/* [90] */
		243,
		/* [91] */
		248,
		/* [92] */
		253,
		/* [93] */
		255,
		/* [94] */
		259,
		/* [95] */
		261,
		/* [96] */
		264,
		/* [97] */
		270,
		/* [98] */
		272,
		/* [99] */
		277,
		/* [100] */
		282,
		/* [101] */
		287,
		/* [102] */
		292,
		/* [103] */
		297,
		/* [104] */
		301,
		/* [105] */
		306,
		/* [106] */
		311,
		/* [107] */
		312,
		/* [108] */
		315,
		/* [109] */
		320,
		/* [110] */
		322,
		/* [111] */
		327,
		/* [112] */
		332,
		/* [113] */
		337,
		/* [114] */
		342,
		/* [115] */
		347,
		/* [116] */
		352,
		/* [117] */
		357,
		/* [118] */
		361,
		/* [119] */
		366,
		/* [120] */
		371,
		/* [121] */
		376,
		/* [122] */
		381,
		/* [123] */
		386,
		/* [124] */
		391,
		/* [125] */
		394,
		/* [126] */
		395,
		/* [127] */
		398,
		/* [128] */
		403,
		/* [129] */
		403,
		/* [130] */
		408,
		/* [131] */
		413,
		/* [132] */
		418,
		/* [133] */
		423,
		/* [134] */
		428,
		/* [135] */
		433,
		/* [136] */
		438,
		/* [137] */
		443,
		/* [138] */
		448,
		/* [139] */
		453,
		/* [140] */
		458,
		/* [141] */
		461,
		/* [142] */
		464,
		/* [143] */
		466,
		/* [144] */
		471,
		/* [145] */
		476,
		/* [146] */
		481,
		/* [147] */
		486,
		/* [148] */
		491,
		/* [149] */
		496,
		/* [150] */
		501,
		/* [151] */
		506,
		/* [152] */
		511,
		/* [153] */
		516,
		/* [154] */
		521,
		/* [155] */
		526,
		/* [156] */
		531,
		/* [157] */
		536,
		/* [158] */
		541,
		/* [159] */
		546,
		/* [160] */
		551,
		/* [161] */
		556,
		/* [162] */
		561,
		/* [163] */
		563,
		/* [164] */
		568,
		/* [165] */
		573,
		/* [166] */
		578,
		/* [167] */
		583,
		/* [168] */
		587,
		/* [169] */
		591,
		/* [170] */
		596,
		/* [171] */
		601,
		/* [172] */
		606,
		/* [173] */
		612,
		/* [174] */
		618,
		/* [175] */
		619,
		/* [176] */
		624,
		/* [177] */
		629,
		/* [178] */
		634,
		/* [179] */
		639,
		/* [180] */
		644,
		/* [181] */
		645,
		/* [182] */
		649,
		/* [183] */
		653,
		/* [184] */
		658,
		/* [185] */
		663,
		/* [186] */
		667,
		/* [187] */
		672,
		/* [188] */
		675,
		/* [189] */
		680,
		/* [190] */
		685,
		/* [191] */
		690,
		/* [192] */
		694,
		/* [193] */
		698,
		/* [194] */
		701,
		/* [195] */
		707,
		/* [196] */
		713,
		/* [197] */
		716,
		/* [198] */
		722,
		/* [199] */
		728,
		/* [200] */
		731,
		/* [201] */
		735,
		/* [202] */
		739,
		/* [203] */
		743,
		/* [204] */
		749,
		/* [205] */
		755,
		/* [206] */
		759,
		/* [207] */
		765,
		/* [208] */
		771,
		/* [209] */
		777,
		/* [210] */
		783,
		/* [211] */
		789,
		/* [212] */
		795,
		/* [213] */
		799,
		/* [214] */
		802,
		/* [215] */
		805,
		/* [216] */
		809,
		/* [217] */
		815,
		/* [218] */
		821,
		/* [219] */
		825,
		/* [220] */
		828,
		/* [221] */
		834,
		/* [222] */
		840,
		/* [223] */
		843,
		/* [224] */
		846,
		/* [225] */
		852,
		/* [226] */
		858,
		/* [227] */
		863,
		/* [228] */
		869,
		/* [229] */
		874,
		/* [230] */
		879,
		/* [231] */
		885,
		/* [232] */
		890,
		/* [233] */
		896,
		/* [234] */
		901,
		/* [235] */
		906,
		/* [236] */
		911,
		/* [237] */
		916,
		/* [238] */
		921,
		/* [239] */
		927,
		/* [240] */
		933,
		/* [241] */
		939,
		/* [242] */
		945,
		/* [243] */
		950,
		/* [244] */
		953,
		/* [245] */
		956,
		/* [246] */
		959,
		/* [247] */
		963,
		/* [248] */
		968,
		/* [249] */
		973,
		/* [250] */
		977,
		/* [251] */
		982,
		/* [252] */
		983,
		/* [253] */
		988,
		/* [254] */
		992,
		/* [255] */
		996,
		/* [256] */
		999,
		/* [257] */
		1005
	},
	{	/* array: 257 elements */
		/* [1] */
		6,
		/* [2] */
		6,
		/* [3] */
		6,
		/* [4] */
		6,
		/* [5] */
		6,
		/* [6] */
		6,
		/* [7] */
		6,
		/* [8] */
		6,
		/* [9] */
		6,
		/* [10] */
		6,
		/* [11] */
		6,
		/* [12] */
		6,
		/* [13] */
		6,
		/* [14] */
		6,
		/* [15] */
		6,
		/* [16] */
		6,
		/* [17] */
		6,
		/* [18] */
		6,
		/* [19] */
		6,
		/* [20] */
		6,
		/* [21] */
		6,
		/* [22] */
		6,
		/* [23] */
		6,
		/* [24] */
		6,
		/* [25] */
		6,
		/* [26] */
		6,
		/* [27] */
		6,
		/* [28] */
		6,
		/* [29] */
		6,
		/* [30] */
		6,
		/* [31] */
		6,
		/* [32] */
		6,
		/* [33] */
		1542,
		/* [34] */
		518,
		/* [35] */
		262,
		/* [36] */
		6,
		/* [37] */
		6,
		/* [38] */
		6,
		/* [39] */
		6,
		/* [40] */
		518,
		/* [41] */
		262,
		/* [42] */
		518,
		/* [43] */
		6,
		/* [44] */
		6,
		/* [45] */
		262,
		/* [46] */
		6,
		/* [47] */
		518,
		/* [48] */
		262,
		/* [49] */
		6,
		/* [50] */
		518,
		/* [51] */
		6,
		/* [52] */
		6,
		/* [53] */
		6,
		/* [54] */
		6,
		/* [55] */
		6,
		/* [56] */
		6,
		/* [57] */
		6,
		/* [58] */
		6,
		/* [59] */
		518,
		/* [60] */
		262,
		/* [61] */
		262,
		/* [62] */
		6,
		/* [63] */
		262,
		/* [64] */
		6,
		/* [65] */
		6,
		/* [66] */
		6,
		/* [67] */
		6,
		/* [68] */
		6,
		/* [69] */
		6,
		/* [70] */
		6,
		/* [71] */
		6,
		/* [72] */
		6,
		/* [73] */
		6,
		/* [74] */
		262,
		/* [75] */
		6,
		/* [76] */
		6,
		/* [77] */
		6,
		/* [78] */
		6,
		/* [79] */
		6,
		/* [80] */
		6,
		/* [81] */
		6,
		/* [82] */
		6,
		/* [83] */
		6,
		/* [84] */
		6,
		/* [85] */
		6,
		/* [86] */
		6,
		/* [87] */
		6,
		/* [88] */
		6,
		/* [89] */
		6,
		/* [90] */
		6,
		/* [91] */
		6,
		/* [92] */
		518,
		/* [93] */
		262,
		/* [94] */
		518,
		/* [95] */
		262,
		/* [96] */
		6,
		/* [97] */
		262,
		/* [98] */
		6,
		/* [99] */
		6,
		/* [100] */
		6,
		/* [101] */
		6,
		/* [102] */
		6,
		/* [103] */
		262,
		/* [104] */
		6,
		/* [105] */
		6,
		/* [106] */
		518,
		/* [107] */
		6,
		/* [108] */
		6,
		/* [109] */
		262,
		/* [110] */
		6,
		/* [111] */
		6,
		/* [112] */
		6,
		/* [113] */
		6,
		/* [114] */
		6,
		/* [115] */
		6,
		/* [116] */
		6,
		/* [117] */
		262,
		/* [118] */
		6,
		/* [119] */
		6,
		/* [120] */
		6,
		/* [121] */
		6,
		/* [122] */
		6,
		/* [123] */
		6,
		/* [124] */
		518,
		/* [125] */
		518,
		/* [126] */
		262,
		/* [127] */
		6,
		/* [128] */
		0,
		/* [129] */
		6,
		/* [130] */
		6,
		/* [131] */
		6,
		/* [132] */
		6,
		/* [133] */
		6,
		/* [134] */
		6,
		/* [135] */
		6,
		/* [136] */
		6,
		/* [137] */
		6,
		/* [138] */
		6,
		/* [139] */
		6,
		/* [140] */
		262,
		/* [141] */
		262,
		/* [142] */
		262,
		/* [143] */
		6,
		/* [144] */
		6,
		/* [145] */
		6,
		/* [146] */
		6,
		/* [147] */
		6,
		/* [148] */
		6,
		/* [149] */
		6,
		/* [150] */
		6,
		/* [151] */
		6,
		/* [152] */
		6,
		/* [153] */
		6,
		/* [154] */
		6,
		/* [155] */
		6,
		/* [156] */
		6,
		/* [157] */
		6,
		/* [158] */
		6,
		/* [159] */
		6,
		/* [160] */
		6,
		/* [161] */
		6,
		/* [162] */
		518,
		/* [163] */
		6,
		/* [164] */
		6,
		/* [165] */
		6,
		/* [166] */
		6,
		/* [167] */
		262,
		/* [168] */
		262,
		/* [169] */
		6,
		/* [170] */
		6,
		/* [171] */
		6,
		/* [172] */
		6,
		/* [173] */
		6,
		/* [174] */
		774,
		/* [175] */
		6,
		/* [176] */
		6,
		/* [177] */
		6,
		/* [178] */
		6,
		/* [179] */
		6,
		/* [180] */
		774,
		/* [181] */
		6,
		/* [182] */
		6,
		/* [183] */
		6,
		/* [184] */
		6,
		/* [185] */
		6,
		/* [186] */
		6,
		/* [187] */
		518,
		/* [188] */
		6,
		/* [189] */
		6,
		/* [190] */
		6,
		/* [191] */
		6,
		/* [192] */
		6,
		/* [193] */
		774,
		/* [194] */
		6,
		/* [195] */
		6,
		/* [196] */
		774,
		/* [197] */
		6,
		/* [198] */
		6,
		/* [199] */
		774,
		/* [200] */
		518,
		/* [201] */
		518,
		/* [202] */
		518,
		/* [203] */
		6,
		/* [204] */
		6,
		/* [205] */
		518,
		/* [206] */
		6,
		/* [207] */
		6,
		/* [208] */
		6,
		/* [209] */
		6,
		/* [210] */
		6,
		/* [211] */
		6,
		/* [212] */
		518,
		/* [213] */
		774,
		/* [214] */
		774,
		/* [215] */
		518,
		/* [216] */
		6,
		/* [217] */
		6,
		/* [218] */
		6,
		/* [219] */
		774,
		/* [220] */
		6,
		/* [221] */
		6,
		/* [222] */
		6,
		/* [223] */
		774,
		/* [224] */
		6,
		/* [225] */
		6,
		/* [226] */
		6,
		/* [227] */
		6,
		/* [228] */
		6,
		/* [229] */
		6,
		/* [230] */
		6,
		/* [231] */
		6,
		/* [232] */
		6,
		/* [233] */
		6,
		/* [234] */
		6,
		/* [235] */
		6,
		/* [236] */
		6,
		/* [237] */
		6,
		/* [238] */
		6,
		/* [239] */
		6,
		/* [240] */
		6,
		/* [241] */
		6,
		/* [242] */
		6,
		/* [243] */
		262,
		/* [244] */
		262,
		/* [245] */
		774,
		/* [246] */
		6,
		/* [247] */
		6,
		/* [248] */
		6,
		/* [249] */
		262,
		/* [250] */
		6,
		/* [251] */
		518,
		/* [252] */
		6,
		/* [253] */
		6,
		/* [254] */
		6,
		/* [255] */
		262,
		/* [256] */
		6,
		/* [257] */
		-1
	},
	{	/* array: 0 elements */
	},
	{	/* array: 0 elements */
	}
};

resource 'FONT' (19340) {
	doExpandFont,
	proportionalFont,
	blackFont,
	notSynthetic,
	nofctbRsrc,
	oneBit,
	noCharWidthTable,
	noImageHeightTable,
	0,
	255,
	7,
	0,
	-3,
	7,
	15,
	12,
	3,
	0,
	66,
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 3800 0000"
	$"0000 0000 0000 0000 0006 8000 0000 0000"
	$"55AB 5622 5001 F80E 5108 1002 0008 8A70"
	$"0700 7001 C105 0000 A400 0A08 10FE 03FF"
	$"F800 0000 0000 0000 0000 0000 0000 2000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0005"
	$"4440 0000 0000 0A50 0000 0C00 000B 0000"
	$"0020 8000 0001 FE22 5001 680A 5108 1002"
	$"0008 8A50 0500 5001 4105 0000 A400 0A08"
	$"10FE 03FF F800 0000 0000 0000 0000 03C0"
	$"0000 2000 000A 6000 0010 000C 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 7070 0000 0000 0000"
	$"0000 0000 0000 0000 0000 1C00 0000 0006"
	$"0000 0000 4480 0000 0000 0000 0000 1000"
	$"0000 0000 0020 8000 55AB 5622 5001 680A"
	$"5108 1002 0008 8A50 0500 5001 4105 0000"
	$"A400 0A08 10FE 03FF F800 0000 0000 3800"
	$"1800 F420 0000 2000 000D 9000 D539 2C52"
	$"2000 02E5 CE17 CDF7 3800 01C0 3BCE F7FE"
	$"E8F0 C611 8BBC EF3B F18C 631F C812 0081"
	$"0004 0608 4C00 0000 0000 0000 0000 2A6B"
	$"8022 0089 0100 80A7 3BE0 7B00 8220 0E88"
	$"0D1F 1048 84D8 BDC4 0022 8A00 0001 FE22"
	$"5001 680A 5108 1002 0008 8A50 0500 5001"
	$"4105 0000 A400 0A08 10FE 03FF F80E F83E"
	$"0000 11CE 2001 FC20 0020 2003 0039 207D"
	$"DFD6 B252 A800 031E 3134 1018 C408 2220"
	$"4631 8C21 18A0 CA1B 8C63 18C4 918C 6310"
	$"C815 0041 0004 0808 4C30 0000 0000 8000"
	$"0000 2AB4 5445 2849 0294 4B58 C600 A494"
	$"4511 5188 10A8 9091 0968 C624 0024 9200"
	$"55AB 5622 5001 680A 5108 1002 0008 8A50"
	$"0500 5001 4105 0000 A400 0A08 10FE 03FF"
	$"F811 4822 001B 9231 2005 FC20 044C 2004"
	$"8029 4F44 D552 D261 7100 0514 2154 2018"
	$"C410 102F 4631 8C21 08A0 D215 CC63 18C0"
	$"918C 6310 C418 8001 0004 0808 0410 0000"
	$"0000 8000 0000 2A04 4000 0006 0000 0008"
	$"C600 A000 0000 1189 11F8 9000 000C C620"
	$"0008 2000 0001 FE22 5001 680A 5108 1002"
	$"0008 8A50 0500 5001 4105 0000 A400 0A08"
	$"10FE 03FF FB71 4010 0024 3A31 1005 FC3F"
	$"2292 2404 8029 FF44 8531 D421 7100 0534"
	$"2697 BC27 46A7 C850 C7D0 8C21 08A0 E211"
	$"AC63 18C0 918C 5511 4410 001F E73D DCFF"
	$"4C9F 79DE 7D9D D18D 631F AA04 22E7 BDEF"
	$"739C E498 C60E A39C E8C6 318B 9048 B9F3"
	$"A36A C624 0016 4A5A 55AB 563E 507F 6FFA"
	$"5F08 1002 0008 FA5F FDFF DFFF 7FE5 3F80"
	$"A7F8 0A7F 10FE 03FF FC92 47C8 FC44 4631"
	$"781D FC20 2152 204B 3820 0F44 8F98 8821"
	$"AFC0 0954 41F8 6248 C6C0 0496 FE30 8FBD"
	$"3FA0 C211 9C7D 1F38 918C 48A2 4210 0023"
	$"18C6 2918 CD1A C631 8E62 918D 5511 4904"
	$"2318 C631 8C63 1498 C7D5 FC63 18C6 318D"
	$"79FF 1234 6399 BDC4 FFE9 9AA5 0001 FFE3"
	$"DF8C 6C63 F1FF FFFF FFFF 8BC6 3060 C60C"
	$"101F C07F FC47 FF88 FFFF FFFF FC92 4285"
	$"2444 47F1 8EEB FC3F FA30 3FB0 7C20 0F44"
	$"8515 5421 210F 8994 8110 6248 BC27 C896"
	$"C630 8C21 18A0 E211 8C61 1884 918C 5444"
	$"4210 0023 1847 E918 CE1A C631 8C5C 918D"
	$"4912 2A04 23F8 C631 87FF F49F FE17 A463"
	$"18C6 318D 1048 1234 6318 8008 8041 2B42"
	$"D5AB 563E 52FF 6F7E 0F10 0022 0008 FA7E"
	$"FFFB DFFF 7FE0 3F94 07FD 0A7F 08FF FFFC"
	$"0371 4289 2444 4631 8D6B FC20 2411 2048"
	$"7E20 0F44 0015 B2A1 0100 1115 0110 6248"
	$"8410 1017 C630 8C21 18A8 D211 8C61 1884"
	$"918D 6248 4110 0023 1846 0918 CD1A C631"
	$"8C02 918D 4914 2A04 2308 C631 8421 0498"
	$"C614 A463 18C6 318D 1249 1234 6318 FFF0"
	$"8042 7F42 8001 FE22 5289 6940 0010 0022"
	$"0008 8A02 800A 5001 4000 0414 0045 0A08"
	$"08FF FFFC 0011 4291 2444 462A 8EEB FC3F"
	$"20F1 24B0 7DA0 0F44 8056 B112 0010 5116"
	$"1114 6248 8A88 2090 4631 8C21 18A8 CA11"
	$"8C61 18C4 9156 E248 4110 0027 18C6 2918"
	$"CC9A C631 8C22 9355 5518 2A04 6719 CE73"
	$"8C61 1498 C615 A463 19CE 318D 524B 9274"
	$"6718 8011 0004 0AA5 55AB 5622 5289 6940"
	$"0010 0022 0008 8A02 800A 5001 4000 0414"
	$"0045 0A08 08FF FFFC 0011 42A3 24C4 3A2A"
	$"882B FC20 0710 C000 3860 0044 803A 4E92"
	$"0010 60E7 EE13 9C47 3280 008F 47CE F7E0"
	$"E8F7 47F1 8BA0 E8B8 8E24 624F C090 7F1B"
	$"E73D C8F8 CC5A C5DE 7C1C 6D22 A2FF AA03"
	$"9AE6 B5AD 739E E498 C7EE BB9C E6B5 EE73"
	$"BC49 11B3 9B18 800E 000F 0A5A 0001 FE22"
	$"5289 6940 0010 0022 0008 8A02 800A 5001"
	$"4000 0414 0045 0A08 08FF FFFC 0016 42BE"
	$"C742 11DB 701D FFE0 F810 0000 0020 007D"
	$"0010 000C 0010 2000 0000 0000 0080 0000"
	$"0000 0000 0000 0000 0000 4000 0000 0000"
	$"70F0 0000 0000 0010 0800 0010 0800 0000"
	$"0010 1C01 0000 0000 2000 0000 0000 0000"
	$"0000 2001 0001 1000 0000 0000 0000 0000"
	$"55AB 5622 5289 6940 0010 0022 0008 8A02"
	$"800A 5001 4000 0414 0045 0A08 08FF FFFC"
	$"0010 0000 0400 1000 0011 F800 0010 0000"
	$"0000 0000 0000 0000 0020 0000 0000 0000"
	$"0100 0000 0000 0000 0000 0000 0000 2000"
	$"0000 0000 0000 0000 0000 0110 0800 0010"
	$"0800 0000 0110 0001 0000 0000 2000 0000"
	$"0000 0000 0000 2000 0001 E000 0000 0000"
	$"0000 0000 0001 FE22 5289 6940 0010 0022"
	$"0008 8A02 800A 5001 4000 0414 0045 0A08"
	$"08FF FFFC 0000 0000 0400 3800 0010 0000"
	$"0010 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 00E0"
	$"3000 0000 0000 0000 00E0 0002 0000 0000"
	$"4000 0000 0000 0000 0001 C000 0000 0000"
	$"0000 0000 0000 0000 55AB 5622 5289 F9C0"
	$"0010 0022 0008 8A03 800E 7001 C000 0414"
	$"0045 0A08 08FF FFFC 0000 0000 0000 0000"
	$"0000 0000 0010 0000 0000 0000",
	{	/* array: 258 elements */
		/* [1] */
		0,
		/* [2] */
		0,
		/* [3] */
		0,
		/* [4] */
		0,
		/* [5] */
		0,
		/* [6] */
		0,
		/* [7] */
		0,
		/* [8] */
		0,
		/* [9] */
		0,
		/* [10] */
		0,
		/* [11] */
		0,
		/* [12] */
		0,
		/* [13] */
		0,
		/* [14] */
		0,
		/* [15] */
		0,
		/* [16] */
		0,
		/* [17] */
		0,
		/* [18] */
		0,
		/* [19] */
		0,
		/* [20] */
		0,
		/* [21] */
		0,
		/* [22] */
		0,
		/* [23] */
		0,
		/* [24] */
		0,
		/* [25] */
		0,
		/* [26] */
		0,
		/* [27] */
		0,
		/* [28] */
		0,
		/* [29] */
		0,
		/* [30] */
		0,
		/* [31] */
		0,
		/* [32] */
		0,
		/* [33] */
		0,
		/* [34] */
		0,
		/* [35] */
		1,
		/* [36] */
		4,
		/* [37] */
		9,
		/* [38] */
		14,
		/* [39] */
		19,
		/* [40] */
		25,
		/* [41] */
		26,
		/* [42] */
		29,
		/* [43] */
		32,
		/* [44] */
		37,
		/* [45] */
		42,
		/* [46] */
		44,
		/* [47] */
		49,
		/* [48] */
		50,
		/* [49] */
		55,
		/* [50] */
		60,
		/* [51] */
		62,
		/* [52] */
		67,
		/* [53] */
		72,
		/* [54] */
		77,
		/* [55] */
		82,
		/* [56] */
		87,
		/* [57] */
		92,
		/* [58] */
		97,
		/* [59] */
		102,
		/* [60] */
		103,
		/* [61] */
		105,
		/* [62] */
		109,
		/* [63] */
		114,
		/* [64] */
		118,
		/* [65] */
		123,
		/* [66] */
		129,
		/* [67] */
		134,
		/* [68] */
		139,
		/* [69] */
		144,
		/* [70] */
		149,
		/* [71] */
		154,
		/* [72] */
		159,
		/* [73] */
		164,
		/* [74] */
		169,
		/* [75] */
		172,
		/* [76] */
		177,
		/* [77] */
		182,
		/* [78] */
		187,
		/* [79] */
		192,
		/* [80] */
		197,
		/* [81] */
		202,
		/* [82] */
		207,
		/* [83] */
		212,
		/* [84] */
		217,
		/* [85] */
		222,
		/* [86] */
		227,
		/* [87] */
		232,
		/* [88] */
		237,
		/* [89] */
		242,
		/* [90] */
		247,
		/* [91] */
		252,
		/* [92] */
		257,
		/* [93] */
		260,
		/* [94] */
		265,
		/* [95] */
		268,
		/* [96] */
		273,
		/* [97] */
		280,
		/* [98] */
		282,
		/* [99] */
		287,
		/* [100] */
		292,
		/* [101] */
		297,
		/* [102] */
		302,
		/* [103] */
		307,
		/* [104] */
		311,
		/* [105] */
		316,
		/* [106] */
		321,
		/* [107] */
		322,
		/* [108] */
		325,
		/* [109] */
		330,
		/* [110] */
		332,
		/* [111] */
		337,
		/* [112] */
		342,
		/* [113] */
		347,
		/* [114] */
		352,
		/* [115] */
		357,
		/* [116] */
		362,
		/* [117] */
		367,
		/* [118] */
		371,
		/* [119] */
		376,
		/* [120] */
		381,
		/* [121] */
		386,
		/* [122] */
		391,
		/* [123] */
		396,
		/* [124] */
		401,
		/* [125] */
		404,
		/* [126] */
		405,
		/* [127] */
		408,
		/* [128] */
		413,
		/* [129] */
		413,
		/* [130] */
		418,
		/* [131] */
		423,
		/* [132] */
		428,
		/* [133] */
		433,
		/* [134] */
		438,
		/* [135] */
		443,
		/* [136] */
		448,
		/* [137] */
		453,
		/* [138] */
		458,
		/* [139] */
		463,
		/* [140] */
		468,
		/* [141] */
		471,
		/* [142] */
		474,
		/* [143] */
		476,
		/* [144] */
		481,
		/* [145] */
		486,
		/* [146] */
		491,
		/* [147] */
		496,
		/* [148] */
		501,
		/* [149] */
		506,
		/* [150] */
		511,
		/* [151] */
		516,
		/* [152] */
		521,
		/* [153] */
		526,
		/* [154] */
		531,
		/* [155] */
		536,
		/* [156] */
		541,
		/* [157] */
		546,
		/* [158] */
		551,
		/* [159] */
		556,
		/* [160] */
		561,
		/* [161] */
		566,
		/* [162] */
		571,
		/* [163] */
		573,
		/* [164] */
		578,
		/* [165] */
		583,
		/* [166] */
		588,
		/* [167] */
		593,
		/* [168] */
		598,
		/* [169] */
		603,
		/* [170] */
		608,
		/* [171] */
		613,
		/* [172] */
		618,
		/* [173] */
		624,
		/* [174] */
		630,
		/* [175] */
		631,
		/* [176] */
		636,
		/* [177] */
		641,
		/* [178] */
		648,
		/* [179] */
		655,
		/* [180] */
		662,
		/* [181] */
		663,
		/* [182] */
		667,
		/* [183] */
		671,
		/* [184] */
		676,
		/* [185] */
		681,
		/* [186] */
		685,
		/* [187] */
		690,
		/* [188] */
		693,
		/* [189] */
		698,
		/* [190] */
		703,
		/* [191] */
		708,
		/* [192] */
		712,
		/* [193] */
		716,
		/* [194] */
		720,
		/* [195] */
		727,
		/* [196] */
		734,
		/* [197] */
		738,
		/* [198] */
		745,
		/* [199] */
		752,
		/* [200] */
		756,
		/* [201] */
		761,
		/* [202] */
		766,
		/* [203] */
		771,
		/* [204] */
		778,
		/* [205] */
		785,
		/* [206] */
		790,
		/* [207] */
		797,
		/* [208] */
		804,
		/* [209] */
		811,
		/* [210] */
		818,
		/* [211] */
		825,
		/* [212] */
		832,
		/* [213] */
		837,
		/* [214] */
		841,
		/* [215] */
		845,
		/* [216] */
		850,
		/* [217] */
		857,
		/* [218] */
		864,
		/* [219] */
		868,
		/* [220] */
		872,
		/* [221] */
		879,
		/* [222] */
		886,
		/* [223] */
		890,
		/* [224] */
		894,
		/* [225] */
		901,
		/* [226] */
		907,
		/* [227] */
		912,
		/* [228] */
		917,
		/* [229] */
		922,
		/* [230] */
		927,
		/* [231] */
		933,
		/* [232] */
		938,
		/* [233] */
		945,
		/* [234] */
		950,
		/* [235] */
		955,
		/* [236] */
		960,
		/* [237] */
		965,
		/* [238] */
		970,
		/* [239] */
		975,
		/* [240] */
		981,
		/* [241] */
		987,
		/* [242] */
		992,
		/* [243] */
		997,
		/* [244] */
		1000,
		/* [245] */
		1003,
		/* [246] */
		1007,
		/* [247] */
		1011,
		/* [248] */
		1016,
		/* [249] */
		1021,
		/* [250] */
		1025,
		/* [251] */
		1030,
		/* [252] */
		1031,
		/* [253] */
		1036,
		/* [254] */
		1040,
		/* [255] */
		1044,
		/* [256] */
		1048,
		/* [257] */
		1048,
		/* [258] */
		1055
	},
	{	/* array: 258 elements */
		/* [1] */
		7,
		/* [2] */
		7,
		/* [3] */
		7,
		/* [4] */
		7,
		/* [5] */
		7,
		/* [6] */
		7,
		/* [7] */
		7,
		/* [8] */
		7,
		/* [9] */
		7,
		/* [10] */
		7,
		/* [11] */
		7,
		/* [12] */
		7,
		/* [13] */
		7,
		/* [14] */
		7,
		/* [15] */
		7,
		/* [16] */
		7,
		/* [17] */
		7,
		/* [18] */
		7,
		/* [19] */
		7,
		/* [20] */
		7,
		/* [21] */
		7,
		/* [22] */
		7,
		/* [23] */
		7,
		/* [24] */
		7,
		/* [25] */
		7,
		/* [26] */
		7,
		/* [27] */
		7,
		/* [28] */
		7,
		/* [29] */
		7,
		/* [30] */
		7,
		/* [31] */
		7,
		/* [32] */
		7,
		/* [33] */
		7,
		/* [34] */
		775,
		/* [35] */
		519,
		/* [36] */
		263,
		/* [37] */
		263,
		/* [38] */
		263,
		/* [39] */
		7,
		/* [40] */
		775,
		/* [41] */
		519,
		/* [42] */
		519,
		/* [43] */
		263,
		/* [44] */
		263,
		/* [45] */
		519,
		/* [46] */
		263,
		/* [47] */
		775,
		/* [48] */
		263,
		/* [49] */
		263,
		/* [50] */
		519,
		/* [51] */
		263,
		/* [52] */
		263,
		/* [53] */
		263,
		/* [54] */
		263,
		/* [55] */
		263,
		/* [56] */
		263,
		/* [57] */
		263,
		/* [58] */
		263,
		/* [59] */
		775,
		/* [60] */
		519,
		/* [61] */
		263,
		/* [62] */
		263,
		/* [63] */
		519,
		/* [64] */
		263,
		/* [65] */
		7,
		/* [66] */
		263,
		/* [67] */
		263,
		/* [68] */
		263,
		/* [69] */
		263,
		/* [70] */
		263,
		/* [71] */
		263,
		/* [72] */
		263,
		/* [73] */
		263,
		/* [74] */
		519,
		/* [75] */
		263,
		/* [76] */
		263,
		/* [77] */
		263,
		/* [78] */
		263,
		/* [79] */
		263,
		/* [80] */
		263,
		/* [81] */
		263,
		/* [82] */
		263,
		/* [83] */
		263,
		/* [84] */
		263,
		/* [85] */
		263,
		/* [86] */
		263,
		/* [87] */
		263,
		/* [88] */
		263,
		/* [89] */
		263,
		/* [90] */
		263,
		/* [91] */
		263,
		/* [92] */
		519,
		/* [93] */
		263,
		/* [94] */
		519,
		/* [95] */
		263,
		/* [96] */
		7,
		/* [97] */
		775,
		/* [98] */
		263,
		/* [99] */
		263,
		/* [100] */
		263,
		/* [101] */
		263,
		/* [102] */
		263,
		/* [103] */
		519,
		/* [104] */
		263,
		/* [105] */
		263,
		/* [106] */
		775,
		/* [107] */
		263,
		/* [108] */
		263,
		/* [109] */
		519,
		/* [110] */
		263,
		/* [111] */
		263,
		/* [112] */
		263,
		/* [113] */
		263,
		/* [114] */
		263,
		/* [115] */
		263,
		/* [116] */
		263,
		/* [117] */
		263,
		/* [118] */
		263,
		/* [119] */
		263,
		/* [120] */
		263,
		/* [121] */
		263,
		/* [122] */
		263,
		/* [123] */
		263,
		/* [124] */
		519,
		/* [125] */
		775,
		/* [126] */
		519,
		/* [127] */
		263,
		/* [128] */
		0,
		/* [129] */
		263,
		/* [130] */
		263,
		/* [131] */
		263,
		/* [132] */
		263,
		/* [133] */
		263,
		/* [134] */
		263,
		/* [135] */
		263,
		/* [136] */
		263,
		/* [137] */
		263,
		/* [138] */
		263,
		/* [139] */
		263,
		/* [140] */
		519,
		/* [141] */
		519,
		/* [142] */
		519,
		/* [143] */
		263,
		/* [144] */
		263,
		/* [145] */
		263,
		/* [146] */
		263,
		/* [147] */
		263,
		/* [148] */
		263,
		/* [149] */
		263,
		/* [150] */
		263,
		/* [151] */
		263,
		/* [152] */
		263,
		/* [153] */
		263,
		/* [154] */
		263,
		/* [155] */
		263,
		/* [156] */
		263,
		/* [157] */
		263,
		/* [158] */
		263,
		/* [159] */
		263,
		/* [160] */
		263,
		/* [161] */
		263,
		/* [162] */
		775,
		/* [163] */
		263,
		/* [164] */
		263,
		/* [165] */
		263,
		/* [166] */
		263,
		/* [167] */
		263,
		/* [168] */
		263,
		/* [169] */
		263,
		/* [170] */
		263,
		/* [171] */
		263,
		/* [172] */
		263,
		/* [173] */
		263,
		/* [174] */
		775,
		/* [175] */
		263,
		/* [176] */
		263,
		/* [177] */
		7,
		/* [178] */
		7,
		/* [179] */
		7,
		/* [180] */
		775,
		/* [181] */
		7,
		/* [182] */
		7,
		/* [183] */
		7,
		/* [184] */
		7,
		/* [185] */
		7,
		/* [186] */
		7,
		/* [187] */
		519,
		/* [188] */
		7,
		/* [189] */
		7,
		/* [190] */
		7,
		/* [191] */
		7,
		/* [192] */
		7,
		/* [193] */
		775,
		/* [194] */
		7,
		/* [195] */
		7,
		/* [196] */
		775,
		/* [197] */
		7,
		/* [198] */
		7,
		/* [199] */
		775,
		/* [200] */
		519,
		/* [201] */
		519,
		/* [202] */
		519,
		/* [203] */
		7,
		/* [204] */
		7,
		/* [205] */
		519,
		/* [206] */
		7,
		/* [207] */
		7,
		/* [208] */
		7,
		/* [209] */
		7,
		/* [210] */
		7,
		/* [211] */
		7,
		/* [212] */
		519,
		/* [213] */
		775,
		/* [214] */
		775,
		/* [215] */
		519,
		/* [216] */
		7,
		/* [217] */
		7,
		/* [218] */
		7,
		/* [219] */
		775,
		/* [220] */
		7,
		/* [221] */
		7,
		/* [222] */
		7,
		/* [223] */
		775,
		/* [224] */
		7,
		/* [225] */
		263,
		/* [226] */
		263,
		/* [227] */
		263,
		/* [228] */
		263,
		/* [229] */
		263,
		/* [230] */
		263,
		/* [231] */
		263,
		/* [232] */
		7,
		/* [233] */
		263,
		/* [234] */
		263,
		/* [235] */
		263,
		/* [236] */
		263,
		/* [237] */
		263,
		/* [238] */
		263,
		/* [239] */
		263,
		/* [240] */
		263,
		/* [241] */
		263,
		/* [242] */
		263,
		/* [243] */
		519,
		/* [244] */
		519,
		/* [245] */
		775,
		/* [246] */
		7,
		/* [247] */
		263,
		/* [248] */
		263,
		/* [249] */
		263,
		/* [250] */
		263,
		/* [251] */
		775,
		/* [252] */
		263,
		/* [253] */
		7,
		/* [254] */
		7,
		/* [255] */
		263,
		/* [256] */
		1,
		/* [257] */
		7,
		/* [258] */
		-1
	},
	{	/* array: 0 elements */
	},
	{	/* array: 0 elements */
	}
};

resource 'FONT' (19465, "PSHackFont 9", purgeable) {
	doExpandFont,
	proportionalFont,
	blackFont,
	notSynthetic,
	nofctbRsrc,
	oneBit,
	noCharWidthTable,
	noImageHeightTable,
	0,
	239,
	10,
	0,
	-10,
	10,
	12,
	10,
	2,
	0,
	52,
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0003 804D 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"001F 81C0 0000 0000 0000 701C 0700 7000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0054 4096 5280 0001"
	$"8000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0002 A000 0000 0016 8140 0000 0000"
	$"8000 5014 0500 5000 0000 0000 0000 0000"
	$"0510 00C0 C000 0080 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0001"
	$"C600 0000 0000 0000 0000 0000 0000 0003"
	$"8003 8000 0009 0816 4091 0190 5244 0524"
	$"404C 0038 0001 E1E0 0000 0000 0000 0000"
	$"0016 814E 0000 000D 0000 5014 0500 5000"
	$"0155 4000 000F 3C00 DFB9 FD25 2500 00B9"
	$"77C5 F37D CE00 018E 13CE E7FB A30C 620E"
	$"5DE7 79DF 8C60 C63F 4280 8200 40C2 1318"
	$"0000 0004 0000 0005 4D21 1DF9 7450 9569"
	$"810A A66D A42A 9A42 A9F2 4344 0F72 1217"
	$"8B40 3E76 A000 8833 FF96 F951 0000 A009"
	$"0000 5794 0500 5000 0296 4022 401F C3F8"
	$"D556 4946 1210 0147 888D 0406 3104 1251"
	$"1231 9444 630C A31E 6318 C624 8C60 C623"
	$"2340 4200 4102 0108 0000 0004 0000 0005"
	$"5621 2319 8C40 0000 0000 0000 0000 0000"
	$"0052 E441 D58D CCCA DC04 5088 0424 5489"
	$"2976 CF60 8034 0011 0480 5CF6 0581 D800"
	$"02AA 8850 0F9F C308 8FD2 5082 1F90 0145"
	$"0915 E80A 31AB E866 AA30 8C44 230D 22AD"
	$"6318 C604 8C55 2945 2200 1B99 DB9F B72F"
	$"6E6E 7B7F 3183 19F5 4072 A115 8C5D DDDD"
	$"D999 9BF6 E666 6699 994D 543B F58D 2D0A"
	$"A83E 509A A442 FC94 8946 EC60 BE48 A013"
	$"B884 CDC3 FCFF 0B60 0000 008A 305F C308"
	$"8539 B952 127C F245 13A4 1F09 CF10 04AA"
	$"ABD0 8F74 FF0E 224D 63E8 F9C4 8A95 1089"
	$"1200 0666 6526 534C 9999 9C85 2A54 A928"
	$"2072 A1D5 8C66 6666 6666 6552 9999 9999"
	$"9941 4E47 F5B5 CD0A 8808 FCA8 1F81 24A4"
	$"4976 AFE0 C959 3FA1 4549 7D7F 0DC3 DC90"
	$"0000 3F06 30FF C308 8014 6622 1510 0245"
	$"207E 1892 210B E92A FE30 8C44 638D 220C"
	$"E308 D024 8A8A 2891 1200 1E62 7D26 538C"
	$"9999 9865 2A54 4945 40FF E113 8C66 6666"
	$"63FF FD52 9999 9999 9901 543B EF8D 2CC8"
	$"003E 90CA A442 FCA4 8916 A811 4F6A 22A1"
	$"3952 0501 DD7E 54FF FC00 008A 307F C308"
	$"0054 A652 1010 0445 4445 1892 2204 1027"
	$"4631 9444 638C A20C E30A CA24 890A 4491"
	$"0A00 2666 6126 534C 9999 9815 2428 A985"
	$"408C 6313 8C66 6666 6622 2152 9999 9999"
	$"9900 E485 C58A 1210 0011 1088 0424 24A5"
	$"2916 A80A 484A 6241 4229 0500 5500 5480"
	$"0000 0852 2F9F C308 8039 1989 2001 0C39"
	$"FB84 E711 CCA0 0110 47CE E7C3 A374 7E0C"
	$"5D07 45C4 710A 449F 0A3F 1F99 D91E 532C"
	$"996E 78E2 E429 17F5 408C 5DF1 739D DDDD"
	$"D9D9 9D52 9666 6677 7700 4F44 05B1 E1E0"
	$"0001 1F72 BFFF 275B E916 B81B 3EB1 A041"
	$"03E4 8700 5500 53E0 0000 0021 E89F C3F8"
	$"0010 0000 C001 0000 0000 0000 0020 000E"
	$"0000 0000 0000 0000 0001 0000 0000 0001"
	$"8600 0000 0004 0200 0008 1000 0000 0103"
	$"8000 0800 0000 0000 0800 0000 0000 0000"
	$"0000 0038 0000 0000 0000 0000 0000 0400"
	$"001F 8000 0000 0001 0000 0000 5700 7000"
	$"0000 0000 201F FE00 0000 0000 0002 0000"
	$"0000 0000 0040 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0018 0C00"
	$"0008 1000 0000 0600 0000 1000 0000 0000"
	$"1000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0400 0000 0000 0000 0002"
	$"0000 0000 7000 0000 0000 0001 C000 0000",
	{	/* array: 242 elements */
		/* [1] */
		0,
		/* [2] */
		0,
		/* [3] */
		0,
		/* [4] */
		0,
		/* [5] */
		0,
		/* [6] */
		0,
		/* [7] */
		0,
		/* [8] */
		0,
		/* [9] */
		0,
		/* [10] */
		0,
		/* [11] */
		0,
		/* [12] */
		0,
		/* [13] */
		0,
		/* [14] */
		0,
		/* [15] */
		0,
		/* [16] */
		0,
		/* [17] */
		0,
		/* [18] */
		0,
		/* [19] */
		0,
		/* [20] */
		0,
		/* [21] */
		0,
		/* [22] */
		0,
		/* [23] */
		0,
		/* [24] */
		0,
		/* [25] */
		0,
		/* [26] */
		0,
		/* [27] */
		0,
		/* [28] */
		0,
		/* [29] */
		0,
		/* [30] */
		0,
		/* [31] */
		0,
		/* [32] */
		0,
		/* [33] */
		0,
		/* [34] */
		0,
		/* [35] */
		1,
		/* [36] */
		4,
		/* [37] */
		9,
		/* [38] */
		14,
		/* [39] */
		22,
		/* [40] */
		29,
		/* [41] */
		30,
		/* [42] */
		33,
		/* [43] */
		36,
		/* [44] */
		41,
		/* [45] */
		46,
		/* [46] */
		48,
		/* [47] */
		52,
		/* [48] */
		53,
		/* [49] */
		57,
		/* [50] */
		62,
		/* [51] */
		64,
		/* [52] */
		69,
		/* [53] */
		74,
		/* [54] */
		79,
		/* [55] */
		84,
		/* [56] */
		89,
		/* [57] */
		94,
		/* [58] */
		99,
		/* [59] */
		104,
		/* [60] */
		105,
		/* [61] */
		107,
		/* [62] */
		110,
		/* [63] */
		115,
		/* [64] */
		118,
		/* [65] */
		122,
		/* [66] */
		129,
		/* [67] */
		134,
		/* [68] */
		139,
		/* [69] */
		144,
		/* [70] */
		149,
		/* [71] */
		153,
		/* [72] */
		157,
		/* [73] */
		162,
		/* [74] */
		167,
		/* [75] */
		168,
		/* [76] */
		173,
		/* [77] */
		178,
		/* [78] */
		182,
		/* [79] */
		189,
		/* [80] */
		194,
		/* [81] */
		199,
		/* [82] */
		204,
		/* [83] */
		209,
		/* [84] */
		214,
		/* [85] */
		219,
		/* [86] */
		224,
		/* [87] */
		229,
		/* [88] */
		234,
		/* [89] */
		241,
		/* [90] */
		246,
		/* [91] */
		251,
		/* [92] */
		255,
		/* [93] */
		257,
		/* [94] */
		261,
		/* [95] */
		263,
		/* [96] */
		266,
		/* [97] */
		272,
		/* [98] */
		274,
		/* [99] */
		278,
		/* [100] */
		282,
		/* [101] */
		286,
		/* [102] */
		290,
		/* [103] */
		294,
		/* [104] */
		298,
		/* [105] */
		302,
		/* [106] */
		306,
		/* [107] */
		308,
		/* [108] */
		311,
		/* [109] */
		315,
		/* [110] */
		317,
		/* [111] */
		324,
		/* [112] */
		328,
		/* [113] */
		332,
		/* [114] */
		336,
		/* [115] */
		340,
		/* [116] */
		344,
		/* [117] */
		348,
		/* [118] */
		351,
		/* [119] */
		355,
		/* [120] */
		360,
		/* [121] */
		367,
		/* [122] */
		372,
		/* [123] */
		376,
		/* [124] */
		380,
		/* [125] */
		383,
		/* [126] */
		384,
		/* [127] */
		387,
		/* [128] */
		392,
		/* [129] */
		392,
		/* [130] */
		397,
		/* [131] */
		402,
		/* [132] */
		407,
		/* [133] */
		411,
		/* [134] */
		416,
		/* [135] */
		421,
		/* [136] */
		426,
		/* [137] */
		430,
		/* [138] */
		434,
		/* [139] */
		438,
		/* [140] */
		442,
		/* [141] */
		446,
		/* [142] */
		450,
		/* [143] */
		454,
		/* [144] */
		458,
		/* [145] */
		462,
		/* [146] */
		466,
		/* [147] */
		470,
		/* [148] */
		472,
		/* [149] */
		474,
		/* [150] */
		477,
		/* [151] */
		480,
		/* [152] */
		484,
		/* [153] */
		488,
		/* [154] */
		492,
		/* [155] */
		496,
		/* [156] */
		500,
		/* [157] */
		504,
		/* [158] */
		508,
		/* [159] */
		512,
		/* [160] */
		516,
		/* [161] */
		520,
		/* [162] */
		523,
		/* [163] */
		527,
		/* [164] */
		532,
		/* [165] */
		537,
		/* [166] */
		542,
		/* [167] */
		547,
		/* [168] */
		552,
		/* [169] */
		557,
		/* [170] */
		565,
		/* [171] */
		573,
		/* [172] */
		581,
		/* [173] */
		583,
		/* [174] */
		586,
		/* [175] */
		591,
		/* [176] */
		599,
		/* [177] */
		606,
		/* [178] */
		611,
		/* [179] */
		616,
		/* [180] */
		620,
		/* [181] */
		624,
		/* [182] */
		629,
		/* [183] */
		634,
		/* [184] */
		638,
		/* [185] */
		643,
		/* [186] */
		649,
		/* [187] */
		654,
		/* [188] */
		657,
		/* [189] */
		661,
		/* [190] */
		666,
		/* [191] */
		673,
		/* [192] */
		680,
		/* [193] */
		686,
		/* [194] */
		690,
		/* [195] */
		691,
		/* [196] */
		695,
		/* [197] */
		702,
		/* [198] */
		705,
		/* [199] */
		710,
		/* [200] */
		715,
		/* [201] */
		721,
		/* [202] */
		725,
		/* [203] */
		729,
		/* [204] */
		735,
		/* [205] */
		741,
		/* [206] */
		745,
		/* [207] */
		751,
		/* [208] */
		757,
		/* [209] */
		764,
		/* [210] */
		768,
		/* [211] */
		774,
		/* [212] */
		778,
		/* [213] */
		782,
		/* [214] */
		784,
		/* [215] */
		786,
		/* [216] */
		791,
		/* [217] */
		798,
		/* [218] */
		803,
		/* [219] */
		811,
		/* [220] */
		811,
		/* [221] */
		811,
		/* [222] */
		811,
		/* [223] */
		811,
		/* [224] */
		811,
		/* [225] */
		811,
		/* [226] */
		811,
		/* [227] */
		811,
		/* [228] */
		811,
		/* [229] */
		811,
		/* [230] */
		811,
		/* [231] */
		811,
		/* [232] */
		811,
		/* [233] */
		811,
		/* [234] */
		811,
		/* [235] */
		811,
		/* [236] */
		811,
		/* [237] */
		811,
		/* [238] */
		811,
		/* [239] */
		811,
		/* [240] */
		817,
		/* [241] */
		823,
		/* [242] */
		830
	},
	{	/* array: 242 elements */
		/* [1] */
		0,
		/* [2] */
		0,
		/* [3] */
		0,
		/* [4] */
		0,
		/* [5] */
		0,
		/* [6] */
		0,
		/* [7] */
		-1,
		/* [8] */
		-1,
		/* [9] */
		-1,
		/* [10] */
		1542,
		/* [11] */
		-1,
		/* [12] */
		-1,
		/* [13] */
		-1,
		/* [14] */
		0,
		/* [15] */
		-1,
		/* [16] */
		-1,
		/* [17] */
		-1,
		/* [18] */
		-1,
		/* [19] */
		-1,
		/* [20] */
		-1,
		/* [21] */
		-1,
		/* [22] */
		-1,
		/* [23] */
		-1,
		/* [24] */
		-1,
		/* [25] */
		-1,
		/* [26] */
		-1,
		/* [27] */
		-1,
		/* [28] */
		-1,
		/* [29] */
		7,
		/* [30] */
		-1,
		/* [31] */
		-1,
		/* [32] */
		-1,
		/* [33] */
		771,
		/* [34] */
		259,
		/* [35] */
		261,
		/* [36] */
		263,
		/* [37] */
		6,
		/* [38] */
		9,
		/* [39] */
		8,
		/* [40] */
		259,
		/* [41] */
		4,
		/* [42] */
		4,
		/* [43] */
		263,
		/* [44] */
		6,
		/* [45] */
		260,
		/* [46] */
		5,
		/* [47] */
		259,
		/* [48] */
		262,
		/* [49] */
		6,
		/* [50] */
		262,
		/* [51] */
		6,
		/* [52] */
		6,
		/* [53] */
		6,
		/* [54] */
		6,
		/* [55] */
		6,
		/* [56] */
		6,
		/* [57] */
		6,
		/* [58] */
		6,
		/* [59] */
		516,
		/* [60] */
		260,
		/* [61] */
		261,
		/* [62] */
		6,
		/* [63] */
		261,
		/* [64] */
		262,
		/* [65] */
		8,
		/* [66] */
		263,
		/* [67] */
		6,
		/* [68] */
		6,
		/* [69] */
		6,
		/* [70] */
		5,
		/* [71] */
		5,
		/* [72] */
		6,
		/* [73] */
		6,
		/* [74] */
		259,
		/* [75] */
		6,
		/* [76] */
		6,
		/* [77] */
		5,
		/* [78] */
		8,
		/* [79] */
		6,
		/* [80] */
		6,
		/* [81] */
		6,
		/* [82] */
		6,
		/* [83] */
		6,
		/* [84] */
		6,
		/* [85] */
		6,
		/* [86] */
		6,
		/* [87] */
		6,
		/* [88] */
		8,
		/* [89] */
		6,
		/* [90] */
		6,
		/* [91] */
		5,
		/* [92] */
		260,
		/* [93] */
		262,
		/* [94] */
		260,
		/* [95] */
		4,
		/* [96] */
		6,
		/* [97] */
		3,
		/* [98] */
		5,
		/* [99] */
		5,
		/* [100] */
		5,
		/* [101] */
		5,
		/* [102] */
		5,
		/* [103] */
		4,
		/* [104] */
		5,
		/* [105] */
		5,
		/* [106] */
		3,
		/* [107] */
		4,
		/* [108] */
		5,
		/* [109] */
		3,
		/* [110] */
		8,
		/* [111] */
		5,
		/* [112] */
		5,
		/* [113] */
		5,
		/* [114] */
		5,
		/* [115] */
		5,
		/* [116] */
		5,
		/* [117] */
		4,
		/* [118] */
		5,
		/* [119] */
		6,
		/* [120] */
		8,
		/* [121] */
		6,
		/* [122] */
		6,
		/* [123] */
		5,
		/* [124] */
		4,
		/* [125] */
		2,
		/* [126] */
		4,
		/* [127] */
		6,
		/* [128] */
		0,
		/* [129] */
		6,
		/* [130] */
		263,
		/* [131] */
		6,
		/* [132] */
		5,
		/* [133] */
		6,
		/* [134] */
		6,
		/* [135] */
		6,
		/* [136] */
		5,
		/* [137] */
		5,
		/* [138] */
		5,
		/* [139] */
		5,
		/* [140] */
		5,
		/* [141] */
		5,
		/* [142] */
		5,
		/* [143] */
		5,
		/* [144] */
		5,
		/* [145] */
		5,
		/* [146] */
		5,
		/* [147] */
		3,
		/* [148] */
		3,
		/* [149] */
		4,
		/* [150] */
		4,
		/* [151] */
		5,
		/* [152] */
		5,
		/* [153] */
		5,
		/* [154] */
		5,
		/* [155] */
		5,
		/* [156] */
		5,
		/* [157] */
		5,
		/* [158] */
		5,
		/* [159] */
		5,
		/* [160] */
		5,
		/* [161] */
		4,
		/* [162] */
		5,
		/* [163] */
		6,
		/* [164] */
		6,
		/* [165] */
		6,
		/* [166] */
		6,
		/* [167] */
		6,
		/* [168] */
		6,
		/* [169] */
		9,
		/* [170] */
		9,
		/* [171] */
		9,
		/* [172] */
		260,
		/* [173] */
		4,
		/* [174] */
		6,
		/* [175] */
		9,
		/* [176] */
		7,
		/* [177] */
		266,
		/* [178] */
		6,
		/* [179] */
		5,
		/* [180] */
		5,
		/* [181] */
		6,
		/* [182] */
		6,
		/* [183] */
		5,
		/* [184] */
		6,
		/* [185] */
		7,
		/* [186] */
		776,
		/* [187] */
		517,
		/* [188] */
		261,
		/* [189] */
		5,
		/* [190] */
		8,
		/* [191] */
		265,
		/* [192] */
		6,
		/* [193] */
		262,
		/* [194] */
		259,
		/* [195] */
		5,
		/* [196] */
		8,
		/* [197] */
		261,
		/* [198] */
		6,
		/* [199] */
		263,
		/* [200] */
		8,
		/* [201] */
		8,
		/* [202] */
		264,
		/* [203] */
		6,
		/* [204] */
		7,
		/* [205] */
		775,
		/* [206] */
		6,
		/* [207] */
		9,
		/* [208] */
		8,
		/* [209] */
		5,
		/* [210] */
		7,
		/* [211] */
		263,
		/* [212] */
		519,
		/* [213] */
		260,
		/* [214] */
		260,
		/* [215] */
		6,
		/* [216] */
		8,
		/* [217] */
		6,
		/* [218] */
		9,
		/* [219] */
		-1,
		/* [220] */
		-1,
		/* [221] */
		-1,
		/* [222] */
		-1,
		/* [223] */
		-1,
		/* [224] */
		-1,
		/* [225] */
		-1,
		/* [226] */
		-1,
		/* [227] */
		-1,
		/* [228] */
		-1,
		/* [229] */
		-1,
		/* [230] */
		-1,
		/* [231] */
		-1,
		/* [232] */
		-1,
		/* [233] */
		-1,
		/* [234] */
		-1,
		/* [235] */
		-1,
		/* [236] */
		-1,
		/* [237] */
		-1,
		/* [238] */
		-1,
		/* [239] */
		6,
		/* [240] */
		6,
		/* [241] */
		7,
		/* [242] */
		0
	},
	{	/* array: 0 elements */
	},
	{	/* array: 0 elements */
	}
};

resource 'FONT' (19468, "PSHackFont", purgeable) {
	doExpandFont,
	proportionalFont,
	blackFont,
	notSynthetic,
	nofctbRsrc,
	oneBit,
	noCharWidthTable,
	noImageHeightTable,
	0,
	239,
	15,
	0,
	-12,
	15,
	15,
	12,
	3,
	1,
	62,
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0070 0132 0000"
	$"0000 0600 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0001 5400 0000 0000"
	$"1F80 E000 0000 0000 0000 0000 000E 0038"
	$"0000 0000 0000 0000 0000 0005 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 2888 024C 9240 0000 0900"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 1680 A000"
	$"0000 0000 0000 0E7C E00A 0028 0000 0000"
	$"0000 0000 F000 0005 0004 0000 3000 0004"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0007 0C00"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0700 0088 0000 0004 2100 D900 8820 1906"
	$"8904 0344 8201 0000 0000 0000 0000 0000"
	$"0001 5400 0000 0000 16FC A000 0000 000C"
	$"0000 0A44 A00A 0028 0000 094A 8000 0301"
	$"FBC0 0005 D14E 7E61 4920 0004 C4EF C17E"
	$"77EF 3C00 00F0 008F 9EF3 FF7A 1830 C20E"
	$"2F7C F7CF 7F86 0C06 31FD 0500 4080 0203"
	$"0413 0C00 0000 0002 0000 0000 0A99 1071"
	$"EFE2 F428 1295 6601 0452 A6DB 108A 5588"
	$"452B B00C 7803 CE00 0074 5A00 7E7A 0000"
	$"1100 CFFF 96C4 A780 0004 8008 8000 0A5C"
	$"A00A 0028 0000 124C 8004 4601 FC2F F005"
	$"D7F5 9291 48C0 0009 2D10 8340 8030 C200"
	$"0109 E088 618A 1086 1831 431E 30C3 0C30"
	$"8886 0C06 310C 8680 2080 0204 0401 0400"
	$"0000 0002 0000 0000 0AA6 1022 1863 0C20"
	$"0000 0000 0000 0000 0000 0000 0001 4912"
	$"8405 511E 1E26 E000 A085 5400 0A00 2451"
	$"16F4 A840 0000 0011 0020 0A50 A00A 0028"
	$"0000 1295 0040 0C01 FC28 1005 8294 94A0"
	$"87F2 000A 1611 0541 0050 C204 110A 1148"
	$"6086 1082 1832 42AD 30C3 0C30 0886 0C45"
	$"5114 8400 0EF3 9EEE 7DB7 16CD 6779 F777"
	$"4631 18C7 EA80 2852 0853 0C2E 739C E739"
	$"CE73 A52B 39CE 73A3 18C5 4B90 81E9 5121"
	$"2125 4000 A08C 0084 9F44 1211 1694 B02E"
	$"E3A4 8011 0020 0A50 A7FA 0028 EE00 0000"
	$"10A4 5FF5 FC28 1005 8FCC 6840 84C2 0012"
	$"1413 097D F04F 4348 0814 E94F A086 1082"
	$"1834 424D 30C3 0C30 0885 1249 4A14 4400"
	$"118C 6314 8E53 2733 98C6 388A 4631 18C4"
	$"2A80 2852 0853 0C31 8C63 18C6 318C 652C"
	$"C631 8C63 18C5 3550 7BF9 525C CCA4 4011"
	$"2095 5488 4444 7111 7697 B031 1444 8023"
	$"B250 0A50 A41B 80EF 1100 0000 0114 7FFD"
	$"FC28 1005 8506 10A0 852F 9F12 1420 9103"
	$"0890 C213 E425 2A28 6087 DE9F F838 420C"
	$"B0FD 0FCF 0885 12A8 8A24 4400 0F8C 2314"
	$"8C53 4623 18C6 3082 462A A544 5240 448A"
	$"0F4B 0C2F 7BDE F7C2 318C 6528 C631 8C63"
	$"18C4 0538 87F9 5152 D080 01F9 FCA4 03F0"
	$"3F44 9091 4696 302F 14C8 FC21 4C50 9BD3"
	$"BF78 FF83 1100 0000 7E0C 7FF9 FC28 1005"
	$"8005 2D14 8402 0022 1440 6103 0890 BE20"
	$"0245 2A28 6086 1086 1834 420C B0C1 0D00"
	$"8884 A2A9 4444 2400 118C 23F4 8C53 8623"
	$"18C6 3072 454A A244 8A80 448A 084B 0C31"
	$"8C63 18C3 FFFF E528 C631 8C63 18C4 0510"
	$"87F5 50DC D080 0022 20C5 5488 4445 1111"
	$"7697 E851 F550 8641 0089 28D2 094B C1EF"
	$"1FFF E000 020C 43F9 FC28 1005 8005 5308"
	$"8402 0022 1480 7F83 0910 8210 0444 F7F8"
	$"6086 1086 1C32 420C 70C1 0C80 8884 A111"
	$"4444 2400 118C 2304 8C53 4623 18C6 300A"
	$"4544 4545 0A80 FFFE 0847 0C31 8C63 18C2"
	$"1084 2528 C631 8C63 18C4 0510 7BF3 D0D2"
	$"CC80 0042 2084 0084 8445 1211 1694 0851"
	$"0651 8541 328A 4FF3 F94A 7F29 1000 0000"
	$"1114 461F FC28 1005 0015 9314 4800 0041"
	$"2508 8143 0910 840B E802 0418 618A 1086"
	$"1C31 420C 70C1 2C50 8884 4112 2484 1400"
	$"118C 6314 8C53 2623 18C6 308A 4C84 48C6"
	$"0A80 8306 1847 0C31 8C63 18C6 318C 6528"
	$"C631 8C67 39CC 0551 05E1 5121 2100 01FC"
	$"2185 5400 0445 2451 169C 0491 1451 8081"
	$"4D05 2000 014A 0029 1100 0000 00A4 4801"
	$"FC28 1005 800E 8CE2 4800 20C0 C7F7 013C"
	$"F10F 3944 1041 C41F 9EF3 F07A 1BD0 FE0C"
	$"2F40 F42F 0878 4112 24FC 147F 8FF3 9EE4"
	$"7C53 1623 1779 F071 B484 48BF EA80 8305"
	$"EFC2 F3CF 7BDE F7B9 CE73 A528 B9CE 739A"
	$"D6B4 03BE 8401 761E 1E00 0084 3E78 03FF"
	$"E47A CFD1 1680 0CCE EB8E 8081 01FC 9000"
	$"014A 0028 EE00 0000 0043 C001 FC2F F005"
	$"0004 0000 3000 2000 0000 0000 0000 0040"
	$"0000 0000 0000 0000 0000 0000 0000 0800"
	$"0000 0000 0006 0C00 0000 0000 0802 0000"
	$"0040 2000 0000 0004 0700 0000 4000 0000"
	$"0000 0010 0000 0000 0000 0000 0000 0100"
	$"7800 0000 0000 0000 0001 5400 0040 0000"
	$"1680 0000 0000 0001 0000 0000 014A 0028"
	$"0000 0000 0000 4000 07E0 0005 0000 0000"
	$"0000 4000 0000 0000 0000 0080 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 8802 0000 0040 2000"
	$"0000 0044 0000 0000 2000 0000 0000 0020"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0080 0000 1680 0000"
	$"0000 0001 0000 0000 01CA 0028 0000 0000"
	$"0004 4000 0000 0005 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 700C 0000 0000 0000 0000 0038"
	$"0000 0000 4000 0000 0000 0000 0000 0000"
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0001 5400 0000 0000 1F80 0000 0000 0002"
	$"0000 0000 000E 0038 0000 0000 0003 8000"
	$"0000 0005",
	{	/* array: 242 elements */
		/* [1] */
		0,
		/* [2] */
		0,
		/* [3] */
		0,
		/* [4] */
		0,
		/* [5] */
		0,
		/* [6] */
		0,
		/* [7] */
		0,
		/* [8] */
		0,
		/* [9] */
		0,
		/* [10] */
		0,
		/* [11] */
		0,
		/* [12] */
		0,
		/* [13] */
		0,
		/* [14] */
		0,
		/* [15] */
		0,
		/* [16] */
		0,
		/* [17] */
		0,
		/* [18] */
		0,
		/* [19] */
		0,
		/* [20] */
		0,
		/* [21] */
		0,
		/* [22] */
		0,
		/* [23] */
		0,
		/* [24] */
		0,
		/* [25] */
		0,
		/* [26] */
		0,
		/* [27] */
		0,
		/* [28] */
		0,
		/* [29] */
		0,
		/* [30] */
		0,
		/* [31] */
		0,
		/* [32] */
		0,
		/* [33] */
		0,
		/* [34] */
		0,
		/* [35] */
		1,
		/* [36] */
		4,
		/* [37] */
		11,
		/* [38] */
		16,
		/* [39] */
		23,
		/* [40] */
		31,
		/* [41] */
		32,
		/* [42] */
		35,
		/* [43] */
		38,
		/* [44] */
		44,
		/* [45] */
		49,
		/* [46] */
		51,
		/* [47] */
		56,
		/* [48] */
		57,
		/* [49] */
		62,
		/* [50] */
		68,
		/* [51] */
		70,
		/* [52] */
		76,
		/* [53] */
		82,
		/* [54] */
		89,
		/* [55] */
		95,
		/* [56] */
		101,
		/* [57] */
		107,
		/* [58] */
		113,
		/* [59] */
		119,
		/* [60] */
		120,
		/* [61] */
		122,
		/* [62] */
		126,
		/* [63] */
		131,
		/* [64] */
		135,
		/* [65] */
		141,
		/* [66] */
		149,
		/* [67] */
		156,
		/* [68] */
		162,
		/* [69] */
		168,
		/* [70] */
		174,
		/* [71] */
		179,
		/* [72] */
		184,
		/* [73] */
		190,
		/* [74] */
		196,
		/* [75] */
		197,
		/* [76] */
		203,
		/* [77] */
		209,
		/* [78] */
		214,
		/* [79] */
		221,
		/* [80] */
		227,
		/* [81] */
		233,
		/* [82] */
		239,
		/* [83] */
		245,
		/* [84] */
		251,
		/* [85] */
		257,
		/* [86] */
		264,
		/* [87] */
		270,
		/* [88] */
		277,
		/* [89] */
		286,
		/* [90] */
		291,
		/* [91] */
		296,
		/* [92] */
		301,
		/* [93] */
		303,
		/* [94] */
		308,
		/* [95] */
		310,
		/* [96] */
		313,
		/* [97] */
		321,
		/* [98] */
		323,
		/* [99] */
		328,
		/* [100] */
		333,
		/* [101] */
		338,
		/* [102] */
		343,
		/* [103] */
		348,
		/* [104] */
		352,
		/* [105] */
		357,
		/* [106] */
		362,
		/* [107] */
		364,
		/* [108] */
		367,
		/* [109] */
		372,
		/* [110] */
		374,
		/* [111] */
		383,
		/* [112] */
		388,
		/* [113] */
		393,
		/* [114] */
		398,
		/* [115] */
		403,
		/* [116] */
		408,
		/* [117] */
		413,
		/* [118] */
		417,
		/* [119] */
		422,
		/* [120] */
		427,
		/* [121] */
		436,
		/* [122] */
		441,
		/* [123] */
		446,
		/* [124] */
		451,
		/* [125] */
		454,
		/* [126] */
		455,
		/* [127] */
		458,
		/* [128] */
		464,
		/* [129] */
		464,
		/* [130] */
		471,
		/* [131] */
		478,
		/* [132] */
		484,
		/* [133] */
		489,
		/* [134] */
		495,
		/* [135] */
		501,
		/* [136] */
		507,
		/* [137] */
		512,
		/* [138] */
		517,
		/* [139] */
		522,
		/* [140] */
		527,
		/* [141] */
		532,
		/* [142] */
		537,
		/* [143] */
		542,
		/* [144] */
		547,
		/* [145] */
		552,
		/* [146] */
		557,
		/* [147] */
		562,
		/* [148] */
		564,
		/* [149] */
		566,
		/* [150] */
		569,
		/* [151] */
		572,
		/* [152] */
		577,
		/* [153] */
		582,
		/* [154] */
		587,
		/* [155] */
		592,
		/* [156] */
		597,
		/* [157] */
		602,
		/* [158] */
		607,
		/* [159] */
		612,
		/* [160] */
		617,
		/* [161] */
		622,
		/* [162] */
		625,
		/* [163] */
		629,
		/* [164] */
		634,
		/* [165] */
		640,
		/* [166] */
		646,
		/* [167] */
		652,
		/* [168] */
		658,
		/* [169] */
		665,
		/* [170] */
		673,
		/* [171] */
		681,
		/* [172] */
		690,
		/* [173] */
		692,
		/* [174] */
		695,
		/* [175] */
		701,
		/* [176] */
		711,
		/* [177] */
		719,
		/* [178] */
		726,
		/* [179] */
		731,
		/* [180] */
		735,
		/* [181] */
		739,
		/* [182] */
		744,
		/* [183] */
		751,
		/* [184] */
		756,
		/* [185] */
		762,
		/* [186] */
		769,
		/* [187] */
		774,
		/* [188] */
		777,
		/* [189] */
		782,
		/* [190] */
		787,
		/* [191] */
		795,
		/* [192] */
		804,
		/* [193] */
		811,
		/* [194] */
		816,
		/* [195] */
		817,
		/* [196] */
		822,
		/* [197] */
		830,
		/* [198] */
		833,
		/* [199] */
		839,
		/* [200] */
		846,
		/* [201] */
		852,
		/* [202] */
		857,
		/* [203] */
		862,
		/* [204] */
		869,
		/* [205] */
		876,
		/* [206] */
		881,
		/* [207] */
		888,
		/* [208] */
		895,
		/* [209] */
		904,
		/* [210] */
		908,
		/* [211] */
		915,
		/* [212] */
		920,
		/* [213] */
		925,
		/* [214] */
		927,
		/* [215] */
		929,
		/* [216] */
		934,
		/* [217] */
		941,
		/* [218] */
		946,
		/* [219] */
		959,
		/* [220] */
		959,
		/* [221] */
		959,
		/* [222] */
		959,
		/* [223] */
		959,
		/* [224] */
		959,
		/* [225] */
		959,
		/* [226] */
		959,
		/* [227] */
		959,
		/* [228] */
		959,
		/* [229] */
		959,
		/* [230] */
		959,
		/* [231] */
		959,
		/* [232] */
		959,
		/* [233] */
		959,
		/* [234] */
		959,
		/* [235] */
		959,
		/* [236] */
		959,
		/* [237] */
		959,
		/* [238] */
		959,
		/* [239] */
		959,
		/* [240] */
		965,
		/* [241] */
		971,
		/* [242] */
		981
	},
	{	/* array: 242 elements */
		/* [1] */
		0,
		/* [2] */
		-1,
		/* [3] */
		0,
		/* [4] */
		0,
		/* [5] */
		0,
		/* [6] */
		0,
		/* [7] */
		-1,
		/* [8] */
		-1,
		/* [9] */
		-1,
		/* [10] */
		2056,
		/* [11] */
		-1,
		/* [12] */
		-1,
		/* [13] */
		-1,
		/* [14] */
		0,
		/* [15] */
		-1,
		/* [16] */
		-1,
		/* [17] */
		-1,
		/* [18] */
		-1,
		/* [19] */
		-1,
		/* [20] */
		-1,
		/* [21] */
		-1,
		/* [22] */
		-1,
		/* [23] */
		-1,
		/* [24] */
		-1,
		/* [25] */
		-1,
		/* [26] */
		-1,
		/* [27] */
		-1,
		/* [28] */
		-1,
		/* [29] */
		-1,
		/* [30] */
		-1,
		/* [31] */
		-1,
		/* [32] */
		-1,
		/* [33] */
		1028,
		/* [34] */
		259,
		/* [35] */
		261,
		/* [36] */
		265,
		/* [37] */
		263,
		/* [38] */
		265,
		/* [39] */
		9,
		/* [40] */
		259,
		/* [41] */
		261,
		/* [42] */
		261,
		/* [43] */
		264,
		/* [44] */
		263,
		/* [45] */
		260,
		/* [46] */
		263,
		/* [47] */
		259,
		/* [48] */
		263,
		/* [49] */
		264,
		/* [50] */
		776,
		/* [51] */
		264,
		/* [52] */
		264,
		/* [53] */
		264,
		/* [54] */
		264,
		/* [55] */
		264,
		/* [56] */
		264,
		/* [57] */
		264,
		/* [58] */
		264,
		/* [59] */
		259,
		/* [60] */
		260,
		/* [61] */
		262,
		/* [62] */
		263,
		/* [63] */
		262,
		/* [64] */
		264,
		/* [65] */
		266,
		/* [66] */
		265,
		/* [67] */
		264,
		/* [68] */
		264,
		/* [69] */
		264,
		/* [70] */
		263,
		/* [71] */
		263,
		/* [72] */
		264,
		/* [73] */
		264,
		/* [74] */
		517,
		/* [75] */
		7,
		/* [76] */
		264,
		/* [77] */
		263,
		/* [78] */
		265,
		/* [79] */
		264,
		/* [80] */
		264,
		/* [81] */
		264,
		/* [82] */
		264,
		/* [83] */
		264,
		/* [84] */
		264,
		/* [85] */
		8,
		/* [86] */
		264,
		/* [87] */
		265,
		/* [88] */
		267,
		/* [89] */
		263,
		/* [90] */
		263,
		/* [91] */
		263,
		/* [92] */
		260,
		/* [93] */
		263,
		/* [94] */
		260,
		/* [95] */
		260,
		/* [96] */
		8,
		/* [97] */
		260,
		/* [98] */
		263,
		/* [99] */
		263,
		/* [100] */
		263,
		/* [101] */
		263,
		/* [102] */
		263,
		/* [103] */
		262,
		/* [104] */
		263,
		/* [105] */
		263,
		/* [106] */
		261,
		/* [107] */
		262,
		/* [108] */
		263,
		/* [109] */
		261,
		/* [110] */
		267,
		/* [111] */
		263,
		/* [112] */
		263,
		/* [113] */
		263,
		/* [114] */
		263,
		/* [115] */
		262,
		/* [116] */
		263,
		/* [117] */
		262,
		/* [118] */
		263,
		/* [119] */
		263,
		/* [120] */
		267,
		/* [121] */
		263,
		/* [122] */
		263,
		/* [123] */
		263,
		/* [124] */
		261,
		/* [125] */
		259,
		/* [126] */
		261,
		/* [127] */
		264,
		/* [128] */
		0,
		/* [129] */
		265,
		/* [130] */
		265,
		/* [131] */
		264,
		/* [132] */
		263,
		/* [133] */
		264,
		/* [134] */
		264,
		/* [135] */
		264,
		/* [136] */
		263,
		/* [137] */
		263,
		/* [138] */
		263,
		/* [139] */
		263,
		/* [140] */
		263,
		/* [141] */
		263,
		/* [142] */
		263,
		/* [143] */
		263,
		/* [144] */
		263,
		/* [145] */
		263,
		/* [146] */
		263,
		/* [147] */
		517,
		/* [148] */
		261,
		/* [149] */
		261,
		/* [150] */
		261,
		/* [151] */
		263,
		/* [152] */
		263,
		/* [153] */
		263,
		/* [154] */
		263,
		/* [155] */
		263,
		/* [156] */
		263,
		/* [157] */
		263,
		/* [158] */
		263,
		/* [159] */
		263,
		/* [160] */
		263,
		/* [161] */
		261,
		/* [162] */
		262,
		/* [163] */
		263,
		/* [164] */
		264,
		/* [165] */
		264,
		/* [166] */
		264,
		/* [167] */
		264,
		/* [168] */
		265,
		/* [169] */
		266,
		/* [170] */
		266,
		/* [171] */
		267,
		/* [172] */
		260,
		/* [173] */
		261,
		/* [174] */
		264,
		/* [175] */
		268,
		/* [176] */
		8,
		/* [177] */
		11,
		/* [178] */
		263,
		/* [179] */
		262,
		/* [180] */
		262,
		/* [181] */
		263,
		/* [182] */
		265,
		/* [183] */
		263,
		/* [184] */
		264,
		/* [185] */
		265,
		/* [186] */
		522,
		/* [187] */
		261,
		/* [188] */
		6,
		/* [189] */
		6,
		/* [190] */
		266,
		/* [191] */
		267,
		/* [192] */
		7,
		/* [193] */
		264,
		/* [194] */
		516,
		/* [195] */
		263,
		/* [196] */
		266,
		/* [197] */
		261,
		/* [198] */
		264,
		/* [199] */
		265,
		/* [200] */
		264,
		/* [201] */
		264,
		/* [202] */
		522,
		/* [203] */
		8,
		/* [204] */
		265,
		/* [205] */
		521,
		/* [206] */
		264,
		/* [207] */
		780,
		/* [208] */
		267,
		/* [209] */
		262,
		/* [210] */
		265,
		/* [211] */
		263,
		/* [212] */
		263,
		/* [213] */
		260,
		/* [214] */
		260,
		/* [215] */
		263,
		/* [216] */
		265,
		/* [217] */
		264,
		/* [218] */
		271,
		/* [219] */
		-1,
		/* [220] */
		-1,
		/* [221] */
		-1,
		/* [222] */
		-1,
		/* [223] */
		-1,
		/* [224] */
		-1,
		/* [225] */
		-1,
		/* [226] */
		-1,
		/* [227] */
		-1,
		/* [228] */
		-1,
		/* [229] */
		-1,
		/* [230] */
		-1,
		/* [231] */
		-1,
		/* [232] */
		-1,
		/* [233] */
		-1,
		/* [234] */
		-1,
		/* [235] */
		-1,
		/* [236] */
		-1,
		/* [237] */
		-1,
		/* [238] */
		-1,
		/* [239] */
		521,
		/* [240] */
		521,
		/* [241] */
		10,
		/* [242] */
		-18725
	},
	{	/* array: 0 elements */
	},
	{	/* array: 0 elements */
	}
};

data 'FOND' (151, "HackFont", preload) {
	$"00F0 0097 0000 0000 0000 0000 0000 0000"            /* ..ó............ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0001 0001 0009 0000 4B89 000C 0000"            /* .......∆..Kâ.... */
	$"4B8C"                                               /* Kå */
};

resource 'FOND' (152, "PSHackFont", preload) {
	proportionalFont,
	dontUseFractWidthTable,
	useIntegerExtra,
	useFractEnable,
	canAdjustCharSpacing,
	noCharWidthTable,
	noImageHeightTable,
	152,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	1,
	{	/* array FontEntries: 2 elements */
		/* [1] */
		9, plain, 14345,
		/* [2] */
		12, plain, 14348
	},
	{	/* array: 0 elements */
	},
	{	/* array: 0 elements */
	},
	{	/* array: 0 elements */
	},
	{	/* array: 0 elements */
	}
};

resource 'CURS' (512, purgeable) {
	$"0000 0000 0FF0 0810 0810 0810 0810 0810"
	$"0810 0810 0810 0810 0FF0",
	$"0000 0FF0 1FF8 1818 1818 1818 1818 1818"
	$"1818 1818 1818 1818 1FF8 0FF0",
	{7, 7}
};

resource 'CURS' (515, purgeable) {
	$"0100 0280 0440 0EE0 0280 0280 0380",
	$"0100 0380 07C0 0FE0 0380 0380 0380",
	{6, 7}
};

resource 'CURS' (516, purgeable) {
	$"001F 0009 0009 0017 0029 0010",
	$"001F 000F 000F 001F 0039 0010",
	{4, 11}
};

resource 'CURS' (517, purgeable) {
	$"0000 0000 0000 0000 0008 000C 007A 0041"
	$"007A 000C 0008",
	$"0000 0000 0000 0000 0008 000C 007E 007F"
	$"007E 000C 0008",
	{7, 9}
};

resource 'CURS' (519, purgeable) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 01C0 0140 0140 0770 0220 0140 0080",
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 01C0 01C0 01C0 07F0 03E0 01C0 0080",
	{9, 8}
};

resource 'CURS' (513, purgeable) {
	$"0000 0000 0000 0000 0000 1000 3000 5E00"
	$"8200 5E00 3000 10",
	$"0000 0000 0000 0000 0000 1000 3000 7E00"
	$"FE00 7E00 3000 10",
	{8, 6}
};

resource 'CURS' (518, purgeable) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0010 0029 0017 0009 0009 001F",
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0010 0039 001F 000F 000F 001F",
	{11, 11}
};

resource 'CURS' (520, purgeable) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0800 9400 E800 9000 9000 F8",
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 0800 9C00 F800 F000 F000 F8",
	{11, 4}
};

resource 'CURS' (514, purgeable) {
	$"F800 9000 9000 E800 9400 08",
	$"F800 F000 F000 F800 9C00 08",
	{4, 4}
};

data 'slEm' (0, purgeable) {
	$"394E 6574 4861 636B 2076 332E 312E 2049"            /* 9NetHack v3.1. I */
	$"636F 6E73 2062 7920 446F 6E61 6C64 204C"            /* cons by Donald L */
	$"2E20 4E61 7368 202D 2044 2E4E 6173 6840"            /* . Nash - D.Nash@ */
	$"7574 6578 6173 2E65 6475"                           /* utexas.edu */
};

resource 'FREF' (128, purgeable) {
	'APPL',
	0,
	""
};

resource 'FREF' (129, purgeable) {
	'SAVE',
	1,
	""
};

resource 'FREF' (130, purgeable) {
	'PREF',
	2,
	""
};

resource 'FREF' (131, purgeable) {
	'TEXT',
	3,
	""
};

resource 'FREF' (132, purgeable) {
	'BONE',
	4,
	""
};

resource 'BNDL' (128, purgeable) {
	'slEm',
	0,
	{	/* array TypeArray: 2 elements */
		/* [1] */
		'FREF',
		{	/* array IDArray: 5 elements */
			/* [1] */
			0, 128,
			/* [2] */
			1, 129,
			/* [3] */
			2, 130,
			/* [4] */
			3, 131,
			/* [5] */
			4, 132
		},
		/* [2] */
		'ICN#',
		{	/* array IDArray: 5 elements */
			/* [1] */
			0, 1000,
			/* [2] */
			1, 1001,
			/* [3] */
			2, 1002,
			/* [4] */
			3, 1003,
			/* [5] */
			4, 1004
		}
	}
};

resource 'MENU' (130) {
	130,
	textMenuProc,
	0x7FFFFFFD,
	enabled,
	"Edit",
	{	/* array: 6 elements */
		/* [1] */
		"Undo", noIcon, "Z", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"Cut", noIcon, "X", noMark, plain,
		/* [4] */
		"Copy", noIcon, "C", noMark, plain,
		/* [5] */
		"Paste", noIcon, "V", noMark, plain,
		/* [6] */
		"Clear", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (131) {
	131,
	textMenuProc,
	0x7FFFFEFF,
	enabled,
	"Keypress",
	{	/* array: 13 elements */
		/* [1] */
		"Control Keys", noIcon, hierarchicalMenu, "…", plain,
		/* [2] */
		"Punctuation", noIcon, hierarchicalMenu, " ", plain,
		/* [3] */
		"Brackets", noIcon, hierarchicalMenu, "À", plain,
		/* [4] */
		"a - m", noIcon, hierarchicalMenu, "Ã", plain,
		/* [5] */
		"n - z", noIcon, hierarchicalMenu, "Õ", plain,
		/* [6] */
		"A - M", noIcon, hierarchicalMenu, "Œ", plain,
		/* [7] */
		"N - Z", noIcon, hierarchicalMenu, "œ", plain,
		/* [8] */
		"0 - 9", noIcon, hierarchicalMenu, "–", plain,
		/* [9] */
		"-", noIcon, noKey, noMark, plain,
		/* [10] */
		"(escape)", noIcon, noKey, noMark, plain,
		/* [11] */
		"(space)", noIcon, noKey, noMark, plain,
		/* [12] */
		"(delete)", noIcon, noKey, noMark, plain,
		/* [13] */
		"(return)", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (132) {
	132,
	textMenuProc,
	0x7FFFFFED,
	enabled,
	"Help",
	{	/* array: 8 elements */
		/* [1] */
		"Options", noIcon, noKey, "O", plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"Help", noIcon, noKey, "?", plain,
		/* [4] */
		"Describe Key", noIcon, noKey, "&", plain,
		/* [5] */
		"-", noIcon, noKey, noMark, plain,
		/* [6] */
		"Version", noIcon, noKey, "v", plain,
		/* [7] */
		"History", noIcon, noKey, "V", plain,
		/* [8] */
		"Version Features", noIcon, noKey, "#", plain
	}
};

resource 'MENU' (128) {
	128,
	textMenuProc,
	0x7FFFFFFD,
	enabled,
	apple,
	{	/* array: 2 elements */
		/* [1] */
		"About Slash'EM…", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (133) {
	133,
	textMenuProc,
	0x7FFFEEF7,
	enabled,
	"Information",
	{	/* array: 14 elements */
		/* [1] */
		"Inventory All", noIcon, noKey, "i", plain,
		/* [2] */
		"Inventory Select", noIcon, noKey, "I", plain,
		/* [3] */
		"Adjust", noIcon, noKey, "#", plain,
		/* [4] */
		"-", noIcon, noKey, noMark, plain,
		/* [5] */
		"Look Down", noIcon, noKey, ":", plain,
		/* [6] */
		"Describe One", noIcon, noKey, ";", plain,
		/* [7] */
		"Describe Many", noIcon, noKey, "/", plain,
		/* [8] */
		"Describe Trap", noIcon, noKey, "^", plain,
		/* [9] */
		"-", noIcon, noKey, noMark, plain,
		/* [10] */
		"Call Monster", noIcon, noKey, "C", plain,
		/* [11] */
		"Name Object", noIcon, noKey, "#", plain,
		/* [12] */
		"Discoveries", noIcon, noKey, "\\", plain,
		/* [13] */
		"-", noIcon, noKey, noMark, plain,
		/* [14] */
		"Show Conduct", noIcon, noKey, "#", plain
	}
};

resource 'MENU' (134) {
	134,
	textMenuProc,
	0x7FFFEDDD,
	enabled,
	"Equipment",
	{	/* array: 15 elements */
		/* [1] */
		"Current", noIcon, hierarchicalMenu, "—", plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"Wield", noIcon, noKey, "w", plain,
		/* [4] */
		"Throw", noIcon, noKey, "t", plain,
		/* [5] */
		"Apply", noIcon, noKey, "a", plain,
		/* [6] */
		"-", noIcon, noKey, noMark, plain,
		/* [7] */
		"Wear Armor", noIcon, noKey, "W", plain,
		/* [8] */
		"Take Off", noIcon, noKey, "T", plain,
		/* [9] */
		"Ask Remove", noIcon, noKey, "A", plain,
		/* [10] */
		"-", noIcon, noKey, noMark, plain,
		/* [11] */
		"Put On", noIcon, noKey, "P", plain,
		/* [12] */
		"Remove", noIcon, noKey, "R", plain,
		/* [13] */
		"-", noIcon, noKey, noMark, plain,
		/* [14] */
		"Fire", noIcon, noKey, "f", plain,
		/* [15] */
		"Ready Quiver", noIcon, noKey, "Q", plain
	}
};

resource 'MENU' (136) {
	136,
	textMenuProc,
	0x7FFF7DEF,
	enabled,
	"Magic",
	{	/* array: 17 elements */
		/* [1] */
		"Drop Item", noIcon, noKey, "d", plain,
		/* [2] */
		"Drop Select", noIcon, noKey, "D", plain,
		/* [3] */
		"Pickup", noIcon, noKey, ",", plain,
		/* [4] */
		"Toggle pickup", noIcon, noKey, "@", plain,
		/* [5] */
		"-", noIcon, noKey, noMark, plain,
		/* [6] */
		"Eat", noIcon, noKey, "e", plain,
		/* [7] */
		"Read", noIcon, noKey, "r", plain,
		/* [8] */
		"Quaff", noIcon, noKey, "q", plain,
		/* [9] */
		"Dip", noIcon, noKey, "#", plain,
		/* [10] */
		"-", noIcon, noKey, noMark, plain,
		/* [11] */
		"List Spells", noIcon, noKey, "+", plain,
		/* [12] */
		"Cast Spell", noIcon, noKey, "Z", plain,
		/* [13] */
		"Zap", noIcon, noKey, "z", plain,
		/* [14] */
		"Invoke", noIcon, noKey, "#", plain,
		/* [15] */
		"Rub", noIcon, noKey, "#", plain,
		/* [16] */
		"-", noIcon, noKey, noMark, plain,
		/* [17] */
		"Polymorph", noIcon, noKey, "#", plain
	}
};

resource 'MENU' (137) {
	137,
	textMenuProc,
	0x7FFFDDDF,
	enabled,
	"Bits",
	{	/* array: 16 elements */
		/* [1] */
		"Search", noIcon, noKey, "s", plain,
		/* [2] */
		"Close Door", noIcon, noKey, "c", plain,
		/* [3] */
		"Open Door", noIcon, noKey, "o", plain,
		/* [4] */
		"Apply", noIcon, noKey, "a", plain,
		/* [5] */
		"Kick", noIcon, "D", "\0x11", plain,
		/* [6] */
		"-", noIcon, noKey, noMark, plain,
		/* [7] */
		"Untrap", noIcon, noKey, "#", plain,
		/* [8] */
		"Force", noIcon, noKey, "#", plain,
		/* [9] */
		"Loot", noIcon, noKey, "#", plain,
		/* [10] */
		"-", noIcon, noKey, noMark, plain,
		/* [11] */
		"Engrave", noIcon, noKey, "E", plain,
		/* [12] */
		"Dip", noIcon, noKey, "#", plain,
		/* [13] */
		"Sit", noIcon, noKey, "#", plain,
		/* [14] */
		"-", noIcon, noKey, "#", 2,
		/* [15] */
		"Climp Up", noIcon, noKey, "<", plain,
		/* [16] */
		"Climb Down", noIcon, noKey, ">", plain
	}
};

resource 'MENU' (202) {
	202,
	textMenuProc,
	allEnabled,
	enabled,
	"punctuation",
	{	/* array: 16 elements */
		/* [1] */
		" .", noIcon, noKey, noMark, plain,
		/* [2] */
		" ,", noIcon, noKey, noMark, plain,
		/* [3] */
		" ;", noIcon, noKey, noMark, plain,
		/* [4] */
		" :", noIcon, noKey, noMark, plain,
		/* [5] */
		" !", noIcon, noKey, noMark, plain,
		/* [6] */
		" ?", noIcon, noKey, noMark, plain,
		/* [7] */
		" +", noIcon, noKey, noMark, plain,
		/* [8] */
		" -", noIcon, noKey, noMark, plain,
		/* [9] */
		" =", noIcon, noKey, noMark, plain,
		/* [10] */
		" #", noIcon, noKey, noMark, plain,
		/* [11] */
		" $", noIcon, noKey, noMark, plain,
		/* [12] */
		" @", noIcon, noKey, noMark, plain,
		/* [13] */
		" &", noIcon, noKey, noMark, plain,
		/* [14] */
		" *", noIcon, noKey, noMark, plain,
		/* [15] */
		" ~", noIcon, noKey, noMark, plain,
		/* [16] */
		" _", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (203) {
	203,
	textMenuProc,
	allEnabled,
	enabled,
	"brackets",
	{	/* array: 16 elements */
		/* [1] */
		"[", noIcon, noKey, noMark, plain,
		/* [2] */
		"]", noIcon, noKey, noMark, plain,
		/* [3] */
		"(", noIcon, noKey, noMark, plain,
		/* [4] */
		")", noIcon, noKey, noMark, plain,
		/* [5] */
		"{", noIcon, noKey, noMark, plain,
		/* [6] */
		"}", noIcon, noKey, noMark, plain,
		/* [7] */
		"<", noIcon, noKey, noMark, plain,
		/* [8] */
		">", noIcon, noKey, noMark, plain,
		/* [9] */
		"^", noIcon, noKey, noMark, plain,
		/* [10] */
		"`", noIcon, noKey, noMark, plain,
		/* [11] */
		"'", noIcon, noKey, noMark, plain,
		/* [12] */
		"\"", noIcon, noKey, noMark, plain,
		/* [13] */
		"\\", noIcon, noKey, noMark, plain,
		/* [14] */
		"/", noIcon, noKey, noMark, plain,
		/* [15] */
		"|", noIcon, noKey, noMark, plain,
		/* [16] */
		"%", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (204) {
	204,
	textMenuProc,
	allEnabled,
	enabled,
	"a - m",
	{	/* array: 13 elements */
		/* [1] */
		"a", noIcon, noKey, noMark, plain,
		/* [2] */
		"b", noIcon, noKey, noMark, plain,
		/* [3] */
		"c", noIcon, noKey, noMark, plain,
		/* [4] */
		"d", noIcon, noKey, noMark, plain,
		/* [5] */
		"e", noIcon, noKey, noMark, plain,
		/* [6] */
		"f", noIcon, noKey, noMark, plain,
		/* [7] */
		"g", noIcon, noKey, noMark, plain,
		/* [8] */
		"h", noIcon, noKey, noMark, plain,
		/* [9] */
		"i", noIcon, noKey, noMark, plain,
		/* [10] */
		"j", noIcon, noKey, noMark, plain,
		/* [11] */
		"k", noIcon, noKey, noMark, plain,
		/* [12] */
		"l", noIcon, noKey, noMark, plain,
		/* [13] */
		"m", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (205) {
	205,
	textMenuProc,
	allEnabled,
	enabled,
	"n - z",
	{	/* array: 13 elements */
		/* [1] */
		"n", noIcon, noKey, noMark, plain,
		/* [2] */
		"o", noIcon, noKey, noMark, plain,
		/* [3] */
		"p", noIcon, noKey, noMark, plain,
		/* [4] */
		"q", noIcon, noKey, noMark, plain,
		/* [5] */
		"r", noIcon, noKey, noMark, plain,
		/* [6] */
		"s", noIcon, noKey, noMark, plain,
		/* [7] */
		"t", noIcon, noKey, noMark, plain,
		/* [8] */
		"u", noIcon, noKey, noMark, plain,
		/* [9] */
		"v", noIcon, noKey, noMark, plain,
		/* [10] */
		"w", noIcon, noKey, noMark, plain,
		/* [11] */
		"x", noIcon, noKey, noMark, plain,
		/* [12] */
		"y", noIcon, noKey, noMark, plain,
		/* [13] */
		"z", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (206) {
	206,
	textMenuProc,
	allEnabled,
	enabled,
	"A - M",
	{	/* array: 13 elements */
		/* [1] */
		"A", noIcon, noKey, noMark, plain,
		/* [2] */
		"B", noIcon, noKey, noMark, plain,
		/* [3] */
		"C", noIcon, noKey, noMark, plain,
		/* [4] */
		"D", noIcon, noKey, noMark, plain,
		/* [5] */
		"E", noIcon, noKey, noMark, plain,
		/* [6] */
		"F", noIcon, noKey, noMark, plain,
		/* [7] */
		"G", noIcon, noKey, noMark, plain,
		/* [8] */
		"H", noIcon, noKey, noMark, plain,
		/* [9] */
		"I", noIcon, noKey, noMark, plain,
		/* [10] */
		"J", noIcon, noKey, noMark, plain,
		/* [11] */
		"K", noIcon, noKey, noMark, plain,
		/* [12] */
		"L", noIcon, noKey, noMark, plain,
		/* [13] */
		"M", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (207) {
	207,
	textMenuProc,
	allEnabled,
	enabled,
	"N - Z",
	{	/* array: 13 elements */
		/* [1] */
		"N", noIcon, noKey, noMark, plain,
		/* [2] */
		"O", noIcon, noKey, noMark, plain,
		/* [3] */
		"P", noIcon, noKey, noMark, plain,
		/* [4] */
		"Q", noIcon, noKey, noMark, plain,
		/* [5] */
		"R", noIcon, noKey, noMark, plain,
		/* [6] */
		"S", noIcon, noKey, noMark, plain,
		/* [7] */
		"T", noIcon, noKey, noMark, plain,
		/* [8] */
		"U", noIcon, noKey, noMark, plain,
		/* [9] */
		"V", noIcon, noKey, noMark, plain,
		/* [10] */
		"W", noIcon, noKey, noMark, plain,
		/* [11] */
		"X", noIcon, noKey, noMark, plain,
		/* [12] */
		"Y", noIcon, noKey, noMark, plain,
		/* [13] */
		"Z", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (201) {
	201,
	textMenuProc,
	0x7FFFFEFF,
	enabled,
	"control keys",
	{	/* array: 13 elements */
		/* [1] */
		"b", noIcon, "1", noMark, plain,
		/* [2] */
		"j", noIcon, "2", noMark, plain,
		/* [3] */
		"n", noIcon, "3", noMark, plain,
		/* [4] */
		"h", noIcon, "4", noMark, plain,
		/* [5] */
		"l", noIcon, "6", noMark, plain,
		/* [6] */
		"y", noIcon, "7", noMark, plain,
		/* [7] */
		"k", noIcon, "8", noMark, plain,
		/* [8] */
		"u", noIcon, "9", noMark, plain,
		/* [9] */
		"-", noIcon, noKey, noMark, plain,
		/* [10] */
		"d", noIcon, "D", noMark, plain,
		/* [11] */
		"p", noIcon, "P", noMark, plain,
		/* [12] */
		"r", noIcon, "R", noMark, plain,
		/* [13] */
		"t", noIcon, "T", noMark, plain
	}
};

resource 'MENU' (208) {
	208,
	textMenuProc,
	allEnabled,
	enabled,
	"0 - 9",
	{	/* array: 10 elements */
		/* [1] */
		"0", noIcon, noKey, noMark, plain,
		/* [2] */
		"1", noIcon, noKey, noMark, plain,
		/* [3] */
		"2", noIcon, noKey, noMark, plain,
		/* [4] */
		"3", noIcon, noKey, noMark, plain,
		/* [5] */
		"4", noIcon, noKey, noMark, plain,
		/* [6] */
		"5", noIcon, noKey, noMark, plain,
		/* [7] */
		"6", noIcon, noKey, noMark, plain,
		/* [8] */
		"7", noIcon, noKey, noMark, plain,
		/* [9] */
		"8", noIcon, noKey, noMark, plain,
		/* [10] */
		"9", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (200) {
	200,
	textMenuProc,
	0x7FFFEEFF,
	enabled,
	"wizard",
	{	/* array: 19 elements */
		/* [1] */
		"Attributes", noIcon, noKey, "x", plain,
		/* [2] */
		"Detect Unseen", noIcon, noKey, "e", plain,
		/* [3] */
		"Map Floor", noIcon, noKey, "f", plain,
		/* [4] */
		"Generate Monster", noIcon, noKey, "g", plain,
		/* [5] */
		"Identify", noIcon, noKey, "i", plain,
		/* [6] */
		"Show Levels", noIcon, noKey, "o", plain,
		/* [7] */
		"Level Teleport", noIcon, noKey, "v", plain,
		/* [8] */
		"Wish", noIcon, noKey, "w", plain,
		/* [9] */
		"-", noIcon, noKey, "c", plain,
		/* [10] */
		"Raise Armor Class", noIcon, noKey, "c", plain,
		/* [11] */
		"Jump Level", noIcon, noKey, "j", plain,
		/* [12] */
		"Toggle Invulnerable", noIcon, noKey, "n", plain,
		/* [13] */
		"-", noIcon, noKey, noMark, plain,
		/* [14] */
		"Show Lights", noIcon, noKey, "#", plain,
		/* [15] */
		"Show Seen", noIcon, noKey, "#", plain,
		/* [16] */
		"Show Memory", noIcon, noKey, "#", plain,
		/* [17] */
		"Show Timeout", noIcon, noKey, "#", plain,
		/* [18] */
		"Show Vision", noIcon, noKey, "#", plain,
		/* [19] */
		"Show Walls", noIcon, noKey, "#", plain
	}
};

resource 'MENU' (209) {
	209,
	textMenuProc,
	0x7FFFFF7F,
	enabled,
	"current",
	{	/* array: 9 elements */
		/* [1] */
		"Weapon", noIcon, noKey, ")", plain,
		/* [2] */
		"Armor", noIcon, noKey, "[", plain,
		/* [3] */
		"Rings", noIcon, noKey, "=", plain,
		/* [4] */
		"Amulet", noIcon, noKey, "\"", plain,
		/* [5] */
		"Tools", noIcon, noKey, "(", plain,
		/* [6] */
		"Gold", noIcon, noKey, "$", plain,
		/* [7] */
		"Spells", noIcon, noKey, "+", plain,
		/* [8] */
		"-", noIcon, noKey, noMark, plain,
		/* [9] */
		"In Use", noIcon, noKey, "*", plain
	}
};

resource 'MENU' (135) {
	135,
	textMenuProc,
	0x7FFFBFBD,
	enabled,
	"Action",
	{	/* array: 20 elements */
		/* [1] */
		"Wait", noIcon, noKey, ".", plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"Teleport", noIcon, "T", "\0x11", plain,
		/* [4] */
		"Jump", noIcon, noKey, "#", plain,
		/* [5] */
		"Monster", noIcon, noKey, "#", plain,
		/* [6] */
		"Wipe", noIcon, noKey, "#", plain,
		/* [7] */
		"-", noIcon, noKey, noMark, plain,
		/* [8] */
		"Apply", noIcon, noKey, "a", plain,
		/* [9] */
		"Pay", noIcon, noKey, "p", plain,
		/* [10] */
		"Kick", noIcon, "D", "\0x11", plain,
		/* [11] */
		"Chat", noIcon, noKey, "#", plain,
		/* [12] */
		"Offer", noIcon, noKey, "#", plain,
		/* [13] */
		"Pray", noIcon, noKey, "#", plain,
		/* [14] */
		"Turn", noIcon, noKey, "#", plain,
		/* [15] */
		"-", noIcon, noKey, noMark, plain,
		/* [16] */
		"Borrow", noIcon, noKey, "#", plain,
		/* [17] */
		"Perform Technique", noIcon, noKey, "#", plain,
		/* [18] */
		"Swap Weapons", noIcon, noKey, "x", plain,
		/* [19] */
		"Use Two Weapons", noIcon, noKey, "#", plain,
		/* [20] */
		"Enhance Ability", noIcon, noKey, "#", plain
	}
};

resource 'MENU' (129) {
	129,
	textMenuProc,
	0x7FFFFEA7,
	enabled,
	"File",
	{	/* array: 10 elements */
		/* [1] */
		"Redraw Map", noIcon, "R", "\0x11", plain,
		/* [2] */
		"Previous Msg", noIcon, "P", "\0x11", plain,
		/* [3] */
		"Reposition", noIcon, "N", noMark, plain,
		/* [4] */
		"-", noIcon, noKey, noMark, plain,
		/* [5] */
		"Regular", noIcon, hierarchicalMenu, "»", plain,
		/* [6] */
		"Enter Explore", noIcon, noKey, "X", plain,
		/* [7] */
		"-", noIcon, noKey, noMark, plain,
		/* [8] */
		"Save", noIcon, "S", "S", plain,
		/* [9] */
		"-", noIcon, noKey, noMark, plain,
		/* [10] */
		"Quit", noIcon, "Q", "Q", plain
	}
};

data 'MNU#' (128, "menubar", locked, preload) {
	$"0080 000A 0080 0000 0081 0000 0082 0000"            /* .Ä...Ä...Å...Ç.. */
	$"0083 0000 0084 0000 0085 0000 0086 0000"            /* .É...Ñ...Ö...Ü.. */
	$"0087 0000 0088 0000 0089 0000"                      /* .á...à...â.. */
};

data 'MNU#' (129, "submenu", locked, preload) {
	$"00C8 000A 00C8 0000 00C9 0000 00CA 0000"            /* .»...»...…... .. */
	$"00CB 0000 00CC 0000 00CD 0000 00CE 0000"            /* .À...Ã...Õ...Œ.. */
	$"00CF 0000 00D0 0000 00D1 0000"                      /* .œ...–...—.. */
};

resource 'vers' (1, purgeable) {
	0x0,
	0x6,
	beta,
	0x4,
	0,
	"0.0.6e4f8",
	"Slash'EM 0.0.6e4f8 (2002/02)\nhttp://slashem.cjb.net/"
};

resource 'vers' (2, purgeable) {
	0x0,
	0x6,
	beta,
	0x4,
	0,
	"0.0.6e4f8",
	"Slash'EM 0.0.6e4f8 (2002/02)"
};

resource 'STR ' (128, "Pref File Name") {
	"Slash'EM Preferences"
};

