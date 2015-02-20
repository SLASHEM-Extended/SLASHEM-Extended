SPECIAL_LEVELS += mall-3.lev
dat/mall-3.lev: dat/mall-3.des util/lev_comp
	( cd dat ; ../util/lev_comp mall-3.des )
