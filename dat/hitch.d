SPECIAL_LEVELS += hitch.lev
dat/hitch.lev: dat/hitch.des util/lev_comp
	( cd dat ; ../util/lev_comp hitch.des )
