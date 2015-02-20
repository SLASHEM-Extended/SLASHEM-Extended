SPECIAL_LEVELS += compu.lev
dat/compu.lev: dat/compu.des util/lev_comp
	( cd dat ; ../util/lev_comp compu.des )
