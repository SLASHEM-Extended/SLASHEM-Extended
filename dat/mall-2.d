SPECIAL_LEVELS += mall-2.lev
dat/mall-2.lev: dat/mall-2.des util/lev_comp
	( cd dat ; ../util/lev_comp mall-2.des )
