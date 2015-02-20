SPECIAL_LEVELS += stor-1.lev
dat/stor-1.lev: dat/stor-1.des util/lev_comp
	( cd dat ; ../util/lev_comp stor-1.des )
