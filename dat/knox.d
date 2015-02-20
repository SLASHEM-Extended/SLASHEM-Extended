SPECIAL_LEVELS += knox.lev
dat/knox.lev: dat/knox.des util/lev_comp
	( cd dat ; ../util/lev_comp knox.des )
