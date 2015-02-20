SPECIAL_LEVELS += dragons.lev
dat/dragons.lev: dat/dragons.des util/lev_comp
	( cd dat ; ../util/lev_comp dragons.des )
