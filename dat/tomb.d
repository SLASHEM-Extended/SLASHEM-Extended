SPECIAL_LEVELS += tomb.lev
dat/tomb.lev: dat/tomb.des util/lev_comp
	( cd dat ; ../util/lev_comp tomb.des )
