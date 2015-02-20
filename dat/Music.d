SPECIAL_LEVELS += Mus-strt.lev Mus-loca.lev Mus-goal.lev Mus-fila.lev Mus-filb.lev
dat/Mus-strt.lev: dat/Music.des util/lev_comp
	( cd dat ; ../util/lev_comp Music.des )
dat/Mus-loca.lev: dat/Mus-strt.lev
dat/Mus-goal.lev: dat/Mus-strt.lev
dat/Mus-fila.lev: dat/Mus-strt.lev
dat/Mus-filb.lev: dat/Mus-strt.lev
