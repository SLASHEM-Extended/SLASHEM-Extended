SPECIAL_LEVELS += spiders.lev
dat/spiders.lev: dat/spiders.des util/lev_comp
	( cd dat ; ../util/lev_comp spiders.des )
