SPECIAL_LEVELS += exmall-1.lev exmall-2.lev exmall-3.lev exmall-4.lev exmall-5.lev exmall-6.lev
dat/exmall-1.lev: dat/exmall.des util/lev_comp
	( cd dat ; ../util/lev_comp exmall.des )
dat/exmall-2.lev: dat/exmall-1.lev
dat/exmall-3.lev: dat/exmall-1.lev
dat/exmall-4.lev: dat/exmall-1.lev
dat/exmall-5.lev: dat/exmall-1.lev
dat/exmall-6.lev: dat/exmall-1.lev
