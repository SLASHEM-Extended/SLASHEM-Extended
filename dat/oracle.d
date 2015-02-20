SPECIAL_LEVELS += oracle.lev
dat/oracle.lev: dat/oracle.des util/lev_comp
	( cd dat ; ../util/lev_comp oracle.des )
