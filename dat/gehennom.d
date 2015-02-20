SPECIAL_LEVELS += valley.lev juiblex.lev orcus.lev asmodeus.lev baalz.lev yeenoghu.lev geryon.lev demogorg.lev dispater.lev sanctum.lev school-1.lev school-2.lev school-3.lev school-4.lev school-5.lev schoolX.lev
dat/valley.lev: dat/gehennom.des util/lev_comp
	( cd dat ; ../util/lev_comp gehennom.des )
dat/juiblex.lev: dat/valley.lev
dat/orcus.lev: dat/valley.lev
dat/asmodeus.lev: dat/valley.lev
dat/baalz.lev: dat/valley.lev
dat/yeenoghu.lev: dat/valley.lev
dat/geryon.lev: dat/valley.lev
dat/demogorg.lev: dat/valley.lev
dat/dispater.lev: dat/valley.lev
dat/sanctum.lev: dat/valley.lev
dat/school-1.lev: dat/valley.lev
dat/school-2.lev: dat/valley.lev
dat/school-3.lev: dat/valley.lev
dat/school-4.lev: dat/valley.lev
dat/school-5.lev: dat/valley.lev
dat/schoolX.lev: dat/valley.lev
