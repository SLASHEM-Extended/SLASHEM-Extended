SPECIAL_LEVELS += wizard1.lev wizard2.lev wizard3.lev fakewiz1.lev fakewiz2.lev
dat/wizard1.lev: dat/yendor.des util/lev_comp
	( cd dat ; ../util/lev_comp yendor.des )
dat/wizard2.lev: dat/wizard1.lev
dat/wizard3.lev: dat/wizard1.lev
dat/fakewiz1.lev: dat/wizard1.lev
dat/fakewiz2.lev: dat/wizard1.lev
