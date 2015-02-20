SPECIAL_LEVELS += nymph.lev
dat/nymph.lev: dat/nymph.des util/lev_comp
	( cd dat ; ../util/lev_comp nymph.des )
