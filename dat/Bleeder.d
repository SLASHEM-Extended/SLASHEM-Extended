SPECIAL_LEVELS += Ble-strt.lev Ble-loca.lev Ble-goal.lev Ble-fila.lev Ble-filb.lev
dat/Ble-strt.lev: dat/Bleeder.des util/lev_comp
	( cd dat ; ../util/lev_comp Bleeder.des )
dat/Ble-loca.lev: dat/Ble-strt.lev
dat/Ble-goal.lev: dat/Ble-strt.lev
dat/Ble-fila.lev: dat/Ble-strt.lev
dat/Ble-filb.lev: dat/Ble-strt.lev
