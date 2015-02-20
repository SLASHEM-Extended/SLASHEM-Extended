SPECIAL_LEVELS += nightmar.lev
dat/nightmar.lev: dat/nightmar.des util/lev_comp
	( cd dat ; ../util/lev_comp nightmar.des )
