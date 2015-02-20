SPECIAL_LEVELS += mall.lev
dat/mall.lev: dat/newmall.des util/lev_comp
	( cd dat ; ../util/lev_comp newmall.des )
