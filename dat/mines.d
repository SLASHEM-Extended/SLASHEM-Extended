SPECIAL_LEVELS += minefill.lev minetn-1.lev minetn-2.lev minetn-3.lev minetn-4.lev minetn-5.lev minetn-6.lev minetn-7.lev minetn-8.lev minetn-9.lev minend-1.lev minend-2.lev minend-3.lev mineking.lev
dat/minefill.lev: dat/mines.des util/lev_comp
	( cd dat ; ../util/lev_comp mines.des )
dat/minetn-1.lev: dat/minefill.lev
dat/minetn-2.lev: dat/minefill.lev
dat/minetn-3.lev: dat/minefill.lev
dat/minetn-4.lev: dat/minefill.lev
dat/minetn-5.lev: dat/minefill.lev
dat/minetn-6.lev: dat/minefill.lev
dat/minetn-7.lev: dat/minefill.lev
dat/minetn-8.lev: dat/minefill.lev
dat/minetn-9.lev: dat/minefill.lev
dat/minend-1.lev: dat/minefill.lev
dat/minend-2.lev: dat/minefill.lev
dat/minend-3.lev: dat/minefill.lev
dat/mineking.lev: dat/minefill.lev
