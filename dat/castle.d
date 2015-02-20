SPECIAL_LEVELS += castle-1.lev castle-2.lev castle-3.lev castle-4.lev castle-5.lev castle-6.lev castle-7.lev
dat/castle-1.lev: dat/castle.des util/lev_comp
	( cd dat ; ../util/lev_comp castle.des )
dat/castle-2.lev: dat/castle-1.lev
dat/castle-3.lev: dat/castle-1.lev
dat/castle-4.lev: dat/castle-1.lev
dat/castle-5.lev: dat/castle-1.lev
dat/castle-6.lev: dat/castle-1.lev
dat/castle-7.lev: dat/castle-1.lev
