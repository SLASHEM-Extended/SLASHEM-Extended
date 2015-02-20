SPECIAL_LEVELS += forge.lev
dat/forge.lev: dat/forge.des util/lev_comp
	( cd dat ; ../util/lev_comp forge.des )
