SPECIAL_LEVELS += grund-1.lev grund-2.lev grund-3.lev
dat/grund-1.lev: dat/grund.des util/lev_comp
	( cd dat ; ../util/lev_comp grund.des )
dat/grund-2.lev: dat/grund-1.lev
dat/grund-3.lev: dat/grund-1.lev
