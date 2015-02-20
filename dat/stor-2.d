SPECIAL_LEVELS += stor-2.lev
dat/stor-2.lev: dat/stor-2.des util/lev_comp
	( cd dat ; ../util/lev_comp stor-2.des )
