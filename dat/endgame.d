SPECIAL_LEVELS += earth.lev air.lev fire.lev water.lev astral.lev
dat/earth.lev: dat/endgame.des util/lev_comp
	( cd dat ; ../util/lev_comp endgame.des )
dat/air.lev: dat/earth.lev
dat/fire.lev: dat/earth.lev
dat/water.lev: dat/earth.lev
dat/astral.lev: dat/earth.lev
