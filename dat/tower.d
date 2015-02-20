SPECIAL_LEVELS += tower1.lev tower2.lev tower3.lev
dat/tower1.lev: dat/tower.des util/lev_comp
	( cd dat ; ../util/lev_comp tower.des )
dat/tower2.lev: dat/tower1.lev
dat/tower3.lev: dat/tower1.lev
