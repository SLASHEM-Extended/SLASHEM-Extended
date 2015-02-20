SPECIAL_LEVELS += guild.lev
dat/guild.lev: dat/guild.des util/lev_comp
	( cd dat ; ../util/lev_comp guild.des )
