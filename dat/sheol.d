SPECIAL_LEVELS += sheolfil.lev sheolmid.lev palace_f.lev palace_e.lev
dat/sheolfil.lev: dat/sheol.des util/lev_comp
	( cd dat ; ../util/lev_comp sheol.des )
dat/sheolmid.lev: dat/sheolfil.lev
dat/palace_f.lev: dat/sheolfil.lev
dat/palace_e.lev: dat/sheolfil.lev
