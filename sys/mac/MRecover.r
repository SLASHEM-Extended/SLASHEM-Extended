data 'ALRT' (128) {
	$"0028 0028 0082 017C 0080 0004"                      /* .(.(.Ç.|.Ä.. */
};

data 'ALRT' (129) {
	$"0028 0028 00C2 017C 0081 0004"                      /* .(.(.¬.|.Å.. */
};

resource 'DITL' (128, purgeable) {
	{	/* array DITLarray: 2 elements */
		/* [1] */
		{56, 110, 76, 230},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{10, 10, 42, 330},
		StaticText {
			disabled,
			"^0"
		}
	}
};

resource 'DITL' (129, purgeable) {
	{	/* array DITLarray: 2 elements */
		/* [1] */
		{120, 110, 140, 230},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{10, 10, 106, 330},
		StaticText {
			disabled,
			"1) Select \"Open...\" from the File menu.\n"
			"2) Select the game to recover from the d"
			"ialog.\n3) Wait for the result (OK or Sor"
			"ry).\n\nDo not attempt to recover a game t"
			"hat is \"in progress\" (i.e. still running"
			" in Slash'EM)!"
		}
	}
};

resource 'DITL' (256, purgeable) {
	{	/* array DITLarray: 1 elements */
		/* [1] */
		{7, 11, 17, 269},
		UserItem {
			disabled
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

resource 'MENU' (128) {
	128,
	textMenuProc,
	0x7FFFFFFB,
	enabled,
	apple,
	{	/* array: 3 elements */
		/* [1] */
		"About Recover...", noIcon, noKey, noMark, plain,
		/* [2] */
		"Help...", noIcon, noKey, noMark, plain,
		/* [3] */
		"-", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (129) {
	129,
	textMenuProc,
	0x7FFFFFF5,
	enabled,
	"File",
	{	/* array: 5 elements */
		/* [1] */
		"Open...", noIcon, "O", noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain,
		/* [3] */
		"Close DA", noIcon, "W", noMark, plain,
		/* [4] */
		"-", noIcon, noKey, noMark, plain,
		/* [5] */
		"Quit", noIcon, "Q", noMark, plain
	}
};

resource 'CURS' (128, locked, preload) {
	$"0000 03C0 02C0 03C0 0180 03C0 0C30 0810"
	$"1028 1048 1788 1008 0810 0C30 03C0",
	$"03C0 07E0 07E0 07E0 03C0 0FF0 1FF8 1FF8"
	$"3FFC 3FFC 3FFC 3FFC 1FF8 1FF8 0FF0 03C0",
	{9, 7}
};

resource 'CURS' (129, locked, preload) {
	$"0000 03C0 0340 03C0 0180 0180 0240 0460"
	$"0420 0460 05A0 0420 0460 0240 0180",
	$"03C0 07E0 07E0 07E0 03C0 03C0 07E0 0FF0"
	$"0FF0 0FF0 0FF0 0FF0 0FF0 07E0 03C0 0180",
	{9, 7}
};

resource 'CURS' (130, locked, preload) {
	$"0000 03C0 02C0 03C0 0180 0180 0240 03C0"
	$"0240 07C0 0640 03C0 0240 03C0 0180",
	$"03C0 07E0 07E0 07E0 03C0 03C0 07E0 07E0"
	$"07E0 0FE0 0FE0 07E0 07E0 07E0 03C0 0180",
	{9, 7}
};

resource 'CURS' (135, locked, preload) {
	$"0000 03C0 0340 03C0 0180 0180 0240 0660"
	$"0420 0660 05A0 0620 0460 0340 0180",
	$"03C0 07E0 07E0 07E0 03C0 03C0 07E0 0FF0"
	$"0FF0 0FF0 0FF0 0FF0 0FF0 07E0 03C0 0180",
	{9, 7}
};

resource 'CURS' (133, locked, preload) {
	$"0000 03C0 0340 03C0 0180 0180 0340 0620"
	$"0560 0520 0560 0520 0560 0240 0180",
	$"03C0 07E0 07E0 07E0 03C0 03C0 07E0 0FF0"
	$"0FF0 0FF0 0FF0 0FF0 0FF0 07E0 03C0 0180",
	{9, 7}
};

resource 'CURS' (134, locked, preload) {
	$"0000 03C0 02C0 03C0 0180 0180 0240 03C0"
	$"0240 03E0 0260 03C0 0240 03C0 0180",
	$"03C0 07E0 07E0 07E0 03C0 03C0 07E0 07E0"
	$"07E0 07F0 07F0 07E0 07E0 07E0 03C0 0180",
	{9, 7}
};

resource 'CURS' (132, locked, preload) {
	$"0000 03C0 02C0 03C0 0180 0380 0F90 0C10"
	$"1998 1998 1998 1998 0990 0C10 03C0",
	$"03C0 07E0 07E0 07E0 03C0 0FF0 1FF8 1FF8"
	$"3FFC 3FFC 3FFC 3FFC 1FF8 1FF8 0FF0 03C0",
	{9, 7}
};

resource 'CURS' (131, locked, preload) {
	$"0000 03C0 0340 03C0 0180 0180 03C0 0520"
	$"06A0 04A0 06A0 04A0 06A0 0340 0180",
	$"03C0 07E0 07E0 07E0 03C0 03C0 07E0 0FF0"
	$"0FF0 0FF0 0FF0 0FF0 0FF0 07E0 03C0 0180",
	{9, 7}
};

resource 'ICN#' (128) {
	{	/* array: 2 elements */
		/* [1] */
		$"0000 0000 001F 0000 007F C000 207F C000"
		$"383F B000 143F 7000 1238 F800 0937 7800"
		$"04FF 7800 027F B800 013F F800 619F F800"
		$"FFCC 30E0 FFE4 0130 EEF2 0290 E9F9 0550"
		$"F7E5 8AA0 EFE3 D540 4EE1 2280 01E0 C900"
		$"0FF0 8200 0FF1 2400 07C2 8A00 0005 5900"
		$"000A A480 0015 4240 0012 8160 0019 00D0"
		$"000E 0050 0000 0030",
		/* [2] */
		$"0FFF 0000 3FFF C000 7FFF F000 7FFF F800"
		$"FFFF FC00 FFFF FE00 FFFF FF00 FFFF FF80"
		$"FFFF FFC0 FFFF FFE0 FFFF FFF0 FFFF FFF8"
		$"FFFF FFFC FFFF FFFC FFFF FFFE FFFF FFFE"
		$"FFFF FFFF 7FFF FFFF 7FFF FFFF 3FFF FFFF"
		$"3FFF FFFF 1FFF FFFF 0FFF FFFF 07FF FFFF"
		$"03FF FFFF 01FF FFFF 00FF FFFF 007F FFFF"
		$"003F FFFE 001F FFFE 0007 FFFC 0001 FFF0"
	}
};

resource 'ics#' (128) {
	{	/* array: 2 elements */
		/* [1] */
		$"0680 0EC0 66E0 3760 1FC0 4FCC FE12 FF2A"
		$"E9D4 EC88 2910 32B0 0558 048C 0304",
		/* [2] */
		$"1FC0 7FF0 7FF8 FFFC FFFE FFFE FFFF FFFF"
		$"FFFF FFFF 7FFF 7FFF 3FFF 1FFE 0FFE 03F8"
	}
};

resource 'BNDL' (128) {
	'slRc',
	0,
	{	/* array TypeArray: 2 elements */
		/* [1] */
		'FREF',
		{	/* array IDArray: 1 elements */
			/* [1] */
			0, 128
		},
		/* [2] */
		'ICN#',
		{	/* array IDArray: 1 elements */
			/* [1] */
			0, 128
		}
	}
};

resource 'FREF' (128) {
	'APPL',
	0,
	""
};

resource 'icl4' (128) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 0000 00FF FFFF 0000 0000 0000 0000"
	$"0000 0000 0F11 1111 FF00 0000 0000 0000"
	$"00F0 0000 0F11 1111 1F00 0000 0000 0000"
	$"00FF F000 00F1 1111 1FFF 0000 0000 0000"
	$"000F DF00 00F1 1111 F11F 0000 0000 0000"
	$"000F DDF0 00F1 1FFF 1111 F000 0000 0000"
	$"0000 FDDF 00F1 F111 F111 F000 0000 0000"
	$"0000 0FDD FF11 1111 F111 F000 0000 0000"
	$"0000 00FD DF11 1111 1F11 F000 0000 0000"
	$"0000 000F DDF1 1111 1111 F000 0000 0000"
	$"0FF0 000F FDDF 11FF FF11 F000 0000 0000"
	$"F11F FFF1 1FDD FF00 00FF 0000 FFF0 0000"
	$"F111 1111 11FD DF00 0000 000F 44FF 0000"
	$"F11F 111F 111F DDF0 0000 00F4 F44F 0000"
	$"F11F 1FF1 11FF FDDF 0000 0F4F 4F4F 0000"
	$"F111 F111 11F0 0FD0 F000 F4F4 F4F0 0000"
	$"F11F 1111 11F0 00F0 0F0F 4F4F 4F00 0000"
	$"0FFF 111F 11F0 000F DDF4 44F4 F000 0000"
	$"000F FFF1 11F0 0000 FF44 F44F 0000 0000"
	$"000F 1111 111F 0000 F444 44F0 0000 0000"
	$"000F F111 111F 000F 44F4 4F00 0000 0000"
	$"0000 0FFF FFF0 00F4 F444 FDF0 0000 0000"
	$"0000 0000 0000 0F4F 4F4F FDDF 0000 0000"
	$"0000 0000 0000 F4F4 F4F0 0FDD F000 0000"
	$"0000 0000 000F 4F4F 4F00 00FD DF00 0000"
	$"0000 0000 000F 44F4 F000 000F D0F0 0000"
	$"0000 0000 000F F44F 0000 0000 F0DF 0000"
	$"0000 0000 0000 FFF0 0000 0000 0FDF 0000"
	$"0000 0000 0000 0000 0000 0000 00FF"
};

resource 'ics4' (128) {
	$"0000 0FFF F000 0000 0000 F11F 1F00 0000"
	$"0DD0 0F1F 11F0 0000 00DD 0F11 F1F0 0000"
	$"000D DF11 1F00 0000 0F00 DDFF FF00 FF00"
	$"F1FF FDD0 000F 44F0 F111 1FDD 00F4 F4F0"
	$"F11F 1F0D DF4F 4F00 FFFF 1F00 F444 F000"
	$"0000 F00F 444F 0000 0000 00F4 F4FD 0000"
	$"0000 0F4F 4F0D D000 0000 0F44 F000 DD00"
	$"0000 00FF 0000 0D"
};

resource 'SICN' (128) {
	{	/* array: 1 elements */
		/* [1] */
		$"0680 0EC0 66E0 3760 1FC0 4FCC FE12 FF2A"
		$"E9D4 EC88 2910 32B0 0558 048C 0304"
	}
};

data 'memB' (128, locked, preload) {
	$"0000 0004 0000 8000 0000 3000 0000 1000"            /* ......Ä...0..... */
	$"0000 4000"                                          /* ..@. */
};

data 'TMPL' (128, "memB") {
	$"0450 7269 7646 5752 4407 436C 6561 6E75"            /* .PrivFWRD.Cleanu */
	$"7044 5752 4407 5072 6565 6D70 7444 4C4E"            /* pDWRD.PreemptDLN */
	$"4707 5761 726E 696E 6744 4C4E 4705 4162"            /* G.WarningDLNG.Ab */
	$"6F72 7444 4C4E 4706 494F 2042 7566 444C"            /* ortDLNG.IO BufDL */
	$"4E47"                                               /* NG */
};

data 'DLOG' (256) {
	$"0028 0028 0040 0140 0004 0000 0000 0000"            /* .(.(.@.@........ */
	$"0000 0100 0850 726F 6772 6573 73"                   /* .....Progress */
};

