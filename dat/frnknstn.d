SPECIAL_LEVELS += frnknstn.lev
dat/frnknstn.lev: dat/frnknstn.des util/lev_comp
	( cd dat ; ../util/lev_comp frnknstn.des )
