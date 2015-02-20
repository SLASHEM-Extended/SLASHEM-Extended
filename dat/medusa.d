SPECIAL_LEVELS += medusa-1.lev medusa-2.lev medusa-3.lev medusa-4.lev medusa-5.lev medusa-6.lev medusa-7.lev
dat/medusa-1.lev: dat/medusa.des util/lev_comp
	( cd dat ; ../util/lev_comp medusa.des )
dat/medusa-2.lev: dat/medusa-1.lev
dat/medusa-3.lev: dat/medusa-1.lev
dat/medusa-4.lev: dat/medusa-1.lev
dat/medusa-5.lev: dat/medusa-1.lev
dat/medusa-6.lev: dat/medusa-1.lev
dat/medusa-7.lev: dat/medusa-1.lev
