SPECIAL_LEVELS += intpla-1.lev intpla-2.lev intpla-3.lev intpla-4.lev intpla-5.lev
dat/intpla-1.lev: dat/intpla.des util/lev_comp
	( cd dat ; ../util/lev_comp intpla.des )
dat/intpla-2.lev: dat/intpla-1.lev
dat/intpla-3.lev: dat/intpla-1.lev
dat/intpla-4.lev: dat/intpla-1.lev
dat/intpla-5.lev: dat/intpla-1.lev
