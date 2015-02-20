SPECIAL_LEVELS += kobold-2.lev
dat/kobold-2.lev: dat/kobold-2.des util/lev_comp
	( cd dat ; ../util/lev_comp kobold-2.des )
