SPECIAL_LEVELS += rats.lev
dat/rats.lev: dat/rats.des util/lev_comp
	( cd dat ; ../util/lev_comp rats.des )
