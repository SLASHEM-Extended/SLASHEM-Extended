SPECIAL_LEVELS += blkmar.lev
dat/blkmar.lev: dat/blkmar.des util/lev_comp
	( cd dat ; ../util/lev_comp blkmar.des )
