SPECIAL_LEVELS += hellma-1.lev hellma-2.lev hellma-3.lev hellma-4.lev hellma-5.lev hellma-6.lev
dat/hellma-1.lev: dat/hellma.des util/lev_comp
	( cd dat ; ../util/lev_comp hellma.des )
dat/hellma-2.lev: dat/hellma-1.lev
dat/hellma-3.lev: dat/hellma-1.lev
dat/hellma-4.lev: dat/hellma-1.lev
dat/hellma-5.lev: dat/hellma-1.lev
dat/hellma-6.lev: dat/hellma-1.lev
