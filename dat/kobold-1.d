SPECIAL_LEVELS += kobold-1.lev
dat/kobold-1.lev: dat/kobold-1.des util/lev_comp
	( cd dat ; ../util/lev_comp kobold-1.des )
