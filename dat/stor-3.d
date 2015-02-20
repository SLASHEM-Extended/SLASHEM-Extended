SPECIAL_LEVELS += stor-3.lev
dat/stor-3.lev: dat/stor-3.des util/lev_comp
	( cd dat ; ../util/lev_comp stor-3.des )
