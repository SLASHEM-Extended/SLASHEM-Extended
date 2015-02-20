SPECIAL_LEVELS += mall-1.lev
dat/mall-1.lev: dat/mall-1.des util/lev_comp
	( cd dat ; ../util/lev_comp mall-1.des )
