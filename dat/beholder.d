SPECIAL_LEVELS += beholder.lev
dat/beholder.lev: dat/beholder.des util/lev_comp
	( cd dat ; ../util/lev_comp beholder.des )
