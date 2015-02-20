SPECIAL_LEVELS += Stu-strt.lev Stu-fila.lev Stu-loca.lev Stu-filb.lev Stu-goal.lev
dat/Stu-strt.lev: dat/Stunt.des util/lev_comp
	( cd dat ; ../util/lev_comp Stunt.des )
dat/Stu-fila.lev: dat/Stu-strt.lev
dat/Stu-loca.lev: dat/Stu-strt.lev
dat/Stu-filb.lev: dat/Stu-strt.lev
dat/Stu-goal.lev: dat/Stu-strt.lev
