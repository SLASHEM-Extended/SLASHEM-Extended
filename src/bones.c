/*	SCCS Id: @(#)bones.c	3.4	2003/09/06	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985,1993. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "lev.h"

extern char bones[];	/* from files.c */
#ifdef MFLOPPY
extern long bytes_counted;
#endif

STATIC_DCL boolean no_bones_level(d_level *);
STATIC_DCL void goodfruit(int);
STATIC_DCL void resetobjs(struct obj *,BOOLEAN_P);
STATIC_DCL void drop_upon_death(struct monst *, struct obj *);

STATIC_OVL boolean
no_bones_level(lev)
d_level *lev;
{
	extern d_level save_dlevel;		/* in do.c */
	s_level *sptr;

	if (ledger_no(&save_dlevel)) assign_level(lev, &save_dlevel);

	return (boolean)(((sptr = Is_special(lev)) != 0 && !sptr->boneid)
		|| !dungeons[lev->dnum].boneid
		   /* no bones on the last or multiway branch levels */
		   /* in any dungeon (level 1 isn't multiway).       */
		|| (Is_botlevel(lev) && In_V_tower(lev) ) || (Is_branchlev(lev) && lev->dlevel > 1 && !In_sokoban(lev) && !In_towndungeon(lev) )
		|| (lev->dlevel < 2 && In_V_tower(lev) )  /* no bones on 1st level */
		   /* no bones in the invocation level               */
		|| (In_gehennom(lev) && lev->dlevel == dunlevs_in_dungeon(lev) - 1)
		);
}

/* Call this function for each fruit object saved in the bones level: it marks
 * that particular type of fruit as existing (the marker is that that type's
 * ID is positive instead of negative).  This way, when we later save the
 * chain of fruit types, we know to only save the types that exist.
 */
STATIC_OVL void
goodfruit(id)
int id;
{
	register struct fruit *f;

	for(f=ffruit; f; f=f->nextf) {
		if(f->fid == -id) {
			f->fid = id;
			return;
		}
	}
}

STATIC_OVL void
resetobjs(ochain,restore)
struct obj *ochain;
boolean restore;
{
	struct obj *otmp;

	for (otmp = ochain; otmp; otmp = otmp->nobj) {

		/* prevent player from depositing a ton of stuff in a chest somewhere --Amy */
		if (Has_contents(otmp)) delete_contents(otmp);

		if (otmp->cobj)
		    resetobjs(otmp->cobj,restore);

		if (((otmp->otyp != CORPSE || otmp->corpsenm < SPECIAL_PM)
			&& otmp->otyp != STATUE)
			&& (!otmp->oartifact ||
			   (restore && (exist_artifact(otmp->otyp, ONAME(otmp))
					|| is_quest_artifact(otmp))))) {
			otmp->oartifact = 0;
			otmp->onamelth = 0;
			*ONAME(otmp) = '\0';
		} else if (otmp->oartifact && restore)
			artifact_exists(otmp,ONAME(otmp),TRUE);
		if (!restore) {
			/* do not zero out o_ids for ghost levels anymore */

			if(objects[otmp->otyp].oc_uses_known) otmp->known = 0;
			otmp->dknown = otmp->bknown = 0;
			otmp->rknown = 0;
			otmp->invlet = 0;
			otmp->no_charge = 0;
			otmp->was_thrown = 0;

			if (otmp->otyp == SLIME_MOLD) goodfruit(otmp->spe);
#ifdef MAIL
			else if (otmp->otyp == SCR_MAIL) otmp->spe = 1;
#endif
			else if (otmp->otyp == EGG) otmp->spe = 0;
			else if (otmp->otyp == TIN) {
			    /* make tins of unique monster's meat be empty */
			    if (otmp->corpsenm >= LOW_PM &&
				    (mons[otmp->corpsenm].geno & G_UNIQ))
				otmp->corpsenm = NON_PM;
			} else if (otmp->otyp == AMULET_OF_YENDOR) {
			    /* no longer the real Amulet */
			    otmp->otyp = FAKE_AMULET_OF_YENDOR;
			    curse(otmp);
			} else if (otmp->otyp == CANDELABRUM_OF_INVOCATION) {
			    if (otmp->lamplit)
				end_burn(otmp, TRUE);
			    otmp->otyp = WAX_CANDLE;
			    otmp->age = 50L;  /* assume used */
			    if (otmp->spe > 0)
				otmp->quan = (long)otmp->spe;
			    otmp->spe = 0;
			    otmp->owt = weight(otmp);
			    curse(otmp);
			} else if (otmp->otyp == BELL_OF_OPENING) {
			    otmp->otyp = BELL;
			    curse(otmp);
			} else if (otmp->otyp == SPE_BOOK_OF_THE_DEAD) {
			    otmp->otyp = SPE_BLANK_PAPER;
			    curse(otmp);
			} else if (otmp->otyp == WAN_WISHING || otmp->otyp == WAN_ACQUIREMENT) {
			    otmp->otyp = WAN_NOTHING;
			} else if (otmp->otyp == MAGIC_LAMP) {
			    otmp->otyp = TIN_OPENER;
			} else if (otmp->otyp == SCR_WISHING || otmp->otyp == SCR_ACQUIREMENT) {
			    otmp->otyp = SCR_AMNESIA;
			} else if (otmp->oartifact == ART_KEY_OF_LAW ||
				   otmp->oartifact == ART_KEY_OF_NEUTRALITY ||
				   otmp->oartifact == ART_KEY_OF_CHAOS ||
				   otmp->oartifact == ART_NIGHTHORN ||
				   otmp->oartifact == ART_EYE_OF_THE_BEHOLDER ||
				   otmp->oartifact == ART_GAUNTLET_KEY ||
				   otmp->oartifact == ART_HAND_OF_VECNA ||
				   otmp->oartifact == ART_THIEFBANE) {
			    /* Guaranteed artifacts become ordinary objects */
			    otmp->oartifact = 0;
			    otmp->onamelth = 0;
			    *ONAME(otmp) = '\0';
			}
			else if (is_lightsaber(otmp)){
				if (otmp->lamplit)
					end_burn(otmp, /*FALSE*/TRUE);
			}
			if (otmp) {

				if(!rn2(3)) curse(otmp);

				/* still blessed? Roll for a chance to make it uncursed. --Amy */
				if(!rn2(3) && otmp->blessed) unbless(otmp);

				/* degrade everything to reduce the # of free stuff the finder will get */
				if (rn2(2)) {
					if (otmp->spe > 2) otmp->spe /= 2;
					else if (otmp->spe > -20) otmp->spe--;
				}

			}
		}
	}
}

STATIC_OVL void
drop_upon_death(mtmp, cont)
struct monst *mtmp;
struct obj *cont;
{
	struct obj *otmp;

	uswapwep = 0; /* ensure curse() won't cause swapwep to drop twice */
	while ((otmp = invent) != 0) {
		obj_extract_self(otmp);
		obj_no_longer_held(otmp);

		otmp->owornmask = 0;
		/* lamps don't go out when dropped */
		if ((cont || artifact_light(otmp)) && obj_is_burning(otmp))
		    end_burn(otmp, TRUE);	/* smother in statue */

		if(otmp->otyp == SLIME_MOLD) goodfruit(otmp->spe);

		/* Getting a bag filled with 10 pages of crap is incredibly imbalanced. --Amy */
		if (Has_contents(otmp)) delete_contents(otmp);
		/* At least now the late player will have to keep their stuff out in the open,
		 * which makes the items likely to be cursed. See below for an additional change... */

		if(rn2(5)) curse(otmp);

		/* still blessed? Roll for a chance to make it uncursed. --Amy */
		if(rn2(5) && otmp->blessed) unbless(otmp);

		if (otmp && rn2(2)) delobj(otmp); /* prevent bones finders from getting everything --Amy */
		else if (mtmp)
			(void) add_to_minv(mtmp, otmp);
		else if (cont)
			(void) add_to_container(cont, otmp);
		else
			place_object(otmp, u.ux, u.uy);
	}
#ifndef GOLDOBJ
	if(u.ugold) {
		long ugold = u.ugold;
		if (mtmp) mtmp->mgold = ugold;
		else if (cont) (void) add_to_container(cont, mkgoldobj(ugold));
		else (void)mkgold(ugold, u.ux, u.uy);
		u.ugold = ugold;	/* undo mkgoldobj()'s removal */
	}
#endif
	if (cont) cont->owt = weight(cont);
}

/* check whether bones are feasible */
boolean
can_make_bones()
{
	register struct trap *ttmp;

#ifdef NO_BONES
	return FALSE;
#endif

	if (ledger_no(&u.uz) <= 0 || ledger_no(&u.uz) > maxledgerno())
	    return FALSE;
	if (no_bones_level(&u.uz))
	    return FALSE;		/* no bones for specific levels */
	if (u.uswallow) {
	    return FALSE;		/* no bones when swallowed */
	}
	if (!Is_branchlev(&u.uz)) {
	    /* no bones on non-branches with portals */
	    for(ttmp = ftrap; ttmp; ttmp = ttmp->ntrap)
		if (ttmp->ttyp == MAGIC_PORTAL) return FALSE;
	}

	/* Several variant authors have experimented with bones probabilities */
	/* KMH -- Restored to NetHack's chances, to limit abuse and for fairness */
	/* to both low-level and high-level characters */
	if(depth(&u.uz) <= 0 ||		/* bulletproofing for endgame */
	   (!rn2(1 + (depth(&u.uz)>>2)) && !(BonesLevelChange || u.uprops[BONES_CHANGE].extrinsic || have_bonestone())	/* fewer ghosts on low levels */
#ifdef WIZARD
		&& !wizard
#endif
		)) return FALSE;

	/* don't let multiple restarts generate multiple copies of objects
	 * in bones files */
	if (discover) return FALSE;
	return TRUE;
}

char *
bonedunlvl(dnum)
xchar dnum;
{
	static char buf[BUFSZ];

	strcpy(buf, "XXX"); /* placeholder */

	if (!strcmp(dungeons[u.uz.dnum].dname, "Angmar"))
		strcpy(buf, "ANG");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Hell's Bathroom"))
		strcpy(buf, "BAT");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Bell Caves"))
		strcpy(buf, "BEL");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Dead Grounds"))
		strcpy(buf, "DEA");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Deep Mines"))
		strcpy(buf, "DEE");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Digdug Challenge"))
		strcpy(buf, "DIG");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "The Dungeons of Doom"))
		strcpy(buf, "DOO");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Emyn Luin"))
		strcpy(buf, "EMY");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Forging Chamber"))
		strcpy(buf, "FOR");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Gamma Caves"))
		strcpy(buf, "GAM");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Gehennom"))
		strcpy(buf, "GEH");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "The Giant Caverns"))
		strcpy(buf, "GIA");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "The Gnomish Mines"))
		strcpy(buf, "GNO");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Grue Challenge"))
		strcpy(buf, "GRE");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Grund's Stronghold"))
		strcpy(buf, "GRU");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Illusory Castle"))
		strcpy(buf, "ILL");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Joust Challenge"))
		strcpy(buf, "JOU");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Fort Ludios"))
		strcpy(buf, "KNX");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Frankenstein's Lab"))
		strcpy(buf, "LAB");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Mainframe"))
		strcpy(buf, "MAI");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Minotaur Maze"))
		strcpy(buf, "MIN");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "The Temple of Moloch"))
		strcpy(buf, "MOL");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Nether Realm"))
		strcpy(buf, "NET");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Ordered Chaos"))
		strcpy(buf, "ORD");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Pacman Challenge"))
		strcpy(buf, "PAC");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Pool Challenge"))
		strcpy(buf, "POO");
	else if (In_quest(&u.uz))
		strcpy(buf, "QUE");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Resting Zone A"))
		strcpy(buf, "RAZ");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Resting Zone GA"))
		strcpy(buf, "RGA");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Resting Zone GB"))
		strcpy(buf, "RGB");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Resting Zone GC"))
		strcpy(buf, "RGC");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Resting Zone GD"))
		strcpy(buf, "RGD");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Resting Zone GE"))
		strcpy(buf, "RGE");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Resting Zone S"))
		strcpy(buf, "RSZ");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Resting Zone TA"))
		strcpy(buf, "RTA");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Resting Zone TB"))
		strcpy(buf, "RTB");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Resting Zone TC"))
		strcpy(buf, "RTC");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Resting Zone TD"))
		strcpy(buf, "RTD");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Resting Zone TE"))
		strcpy(buf, "RTE");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Resting Zone TF"))
		strcpy(buf, "RTF");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Resting Zone TG"))
		strcpy(buf, "RTG");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Resting Zone TH"))
		strcpy(buf, "RTH");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Resting Zone TI"))
		strcpy(buf, "RTI");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Resting Zone TJ"))
		strcpy(buf, "RTJ");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "One-eyed Sam's Market"))
		strcpy(buf, "SAM");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "The Subquest"))
		strcpy(buf, "SBQ");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "The Sunless Sea"))
		strcpy(buf, "SEA");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Sewer Plant"))
		strcpy(buf, "SEW");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Sheol"))
		strcpy(buf, "SHL");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Sokoban"))
		strcpy(buf, "SOK");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Space Base"))
		strcpy(buf, "SPC");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "The Spider Caves"))
		strcpy(buf, "SPI");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Swimming Pool"))
		strcpy(buf, "SWI");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "The Lost Tomb"))
		strcpy(buf, "TOM");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Town"))
		strcpy(buf, "TOW");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Vlad's Tower"))
		strcpy(buf, "VLA");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Void"))
		strcpy(buf, "VOI");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "The Wyrm Caves"))
		strcpy(buf, "WYR");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Yendorian Tower"))
		strcpy(buf, "YEN");
	else impossible("Error! No bones level identifier found. Please notify Amy about this bug and tell her on which level it occurred.");

	return buf;

}

/* save bones and possessions of a deceased adventurer */
void
savebones(corpse)
struct obj *corpse;
{
	int fd, x, y;
	struct trap *ttmp;
	struct monst *mtmp;
	struct permonst *mptr;
	struct fruit *f;
	char c, *bonesid;
	char whynot[BUFSZ];

	/* caller has already checked `can_make_bones()' */

	clear_bypasses();
	fd = open_bonesfile(&u.uz, &bonesid);
	if (fd >= 0) {
		(void) close(fd);
		compress_bonesfile();
#ifdef WIZARD
		if (wizard) {
		    if (yn("Bones file already exists.  Replace it?") == 'y') {
			if (delete_bonesfile(&u.uz)) goto make_bones;
			else pline("Cannot unlink old bones.");
		    }
		}
#endif
		return;
	}

#ifdef WIZARD
 make_bones:
#endif
	unleash_all();

	/* in case these characters are not in their home bases */
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    mptr = mtmp->data;
	    if (mtmp->iswiz || mptr == &mons[PM_MEDUSA] ||
		    (mptr->msound == MS_NEMESIS && mtmp->mnum >= PM_LORD_CARNARVON && mtmp->mnum <= PM_UPPER_BULL) || mptr->msound == MS_LEADER ||
		    mptr == &mons[PM_VLAD_THE_IMPALER] ||
		    mptr == &mons[PM_NIGHTMARE] ||
		    mptr == &mons[PM_BEHOLDER] || mptr == &mons[PM_VECNA] ||
		    mptr == &mons[PM_CTHULHU]) {
		/* Since these monsters may be carrying indestructible 
		 * artifacts, free inventory specifically here to avoid
		 * the indestructible sanity check in discard_minvent.
		 * Similar considerations cause the necessity to avoid
		 * calling delete_contents on containers which are
		 * directly in a monster's inventory (indestructable
		 * objects would be dropped on the floor).
		 */
		struct obj *otmp, *curr;
	    	while ((otmp = mtmp->minvent) != 0) {
		    while (Has_contents(otmp)) {
			while (Has_contents(otmp->cobj))
			    delete_contents(otmp->cobj);
			curr = otmp->cobj;
			obj_extract_self(curr);
			obfree(curr, (struct obj *)0);
		    }
		    obj_extract_self(otmp);
		    obfree(otmp, (struct obj *)0);
		}
		mongone(mtmp);
	    }
	}
	if (u.usteed) dismount_steed(DISMOUNT_BONES);
	dmonsfree();		/* discard dead or gone monsters */

	/* mark all fruits as nonexistent; when we come to them we'll mark
	 * them as existing (using goodfruit())
	 */
	for(f=ffruit; f; f=f->nextf) f->fid = -f->fid;

	/* check iron balls separately--maybe they're not carrying it */
	if (uball) uball->owornmask = uchain->owornmask = 0;

	/* dispose of your possessions, usually cursed */
	if (u.ugrave_arise == (NON_PM - 1)) {
		struct obj *otmp;

		/* embed your possessions in your statue */
		otmp = mk_named_object(STATUE, &mons[u.umonnum], u.ux, u.uy, playeraliasname);

		drop_upon_death((struct monst *)0, otmp);
		if (!otmp) return;	/* couldn't make statue */
		mtmp = (struct monst *)0;
	} else if (Frozen) { /* no player ghost if the player exploded into tiny ice cubes --Amy */

		/* drop everything */
		drop_upon_death((struct monst *)0, (struct obj *)0);
		mtmp = (struct monst *)0;

	} else if (u.ugrave_arise < LOW_PM) {
		/* trick makemon() into allowing monster creation
		 * on your location
		 */
		in_mklev = TRUE;

		/* Come on, the ghost is just too weak. Let's try to make DCSS-styled player "ghosts". --Amy */

		if (!rn2(5)) mtmp = makemon(&mons[PM_GHOST], u.ux, u.uy, MM_NONAME);

		else if (u.ulevel >= 10 && !rn2(5)) { /* dangerous undead version --Amy */

		if (flags.female && urole.undeadfemalenum != NON_PM) mtmp = makemon(&mons[urole.undeadfemalenum], u.ux, u.uy, MM_NONAME);
		else mtmp = makemon(&mons[urole.undeadmalenum], u.ux, u.uy, MM_NONAME);

		}

		else if (flags.female && urole.femalenum != NON_PM) mtmp = makemon(&mons[urole.femalenum], u.ux, u.uy, MM_NONAME);
		else mtmp = makemon(&mons[urole.malenum], u.ux, u.uy, MM_NONAME);


		in_mklev = FALSE;

		if (!mtmp) {
		/* drop everything */
			drop_upon_death((struct monst *)0, (struct obj *)0);
			return;
		}

		mtmp = christen_monst(mtmp, playeraliasname);

		drop_upon_death(mtmp, (struct obj *)0);
		m_dowear(mtmp, TRUE);

		if (corpse)
			(void) obj_attach_mid(corpse, mtmp->m_id); 
	} else {
		/* give your possessions to the monster you become */
		in_mklev = TRUE;
		mtmp = makemon(&mons[u.ugrave_arise], u.ux, u.uy, NO_MM_FLAGS);
		in_mklev = FALSE;
		if (!mtmp) {
			drop_upon_death((struct monst *)0, (struct obj *)0);
			return;
		}
		mtmp = christen_monst(mtmp, playeraliasname);
		newsym(u.ux, u.uy);
		Your("body rises from the dead as %s...",
			an(mons[u.ugrave_arise].mname));
		display_nhwindow(WIN_MESSAGE, FALSE);
		drop_upon_death(mtmp, (struct obj *)0);
		m_dowear(mtmp, TRUE);
	}
	if (mtmp) {
		mtmp->m_lev = (u.ulevel ? u.ulevel : 1);
		mtmp->mhp = mtmp->mhpmax = u.uhpmax;
		mtmp->female = flags.female;
		mtmp->msleeping = 1;
#ifdef LIVELOG_BONES_KILLER 
		strcpy(mtmp->former_rank, rank()); 
#endif 
	}
	for(mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		resetobjs(mtmp->minvent,FALSE);
		/* do not zero out m_ids for bones levels any more */
		mtmp->mlstmv = 0L;
		if(mtmp->mtame) mtmp->mtame = mtmp->mpeaceful = 0;
	}
	for(ttmp = ftrap; ttmp; ttmp = ttmp->ntrap) {
		ttmp->madeby_u = 0;
		ttmp->tseen = (!ttmp->hiddentrap && (ttmp->ttyp == HOLE));
	}
	resetobjs(fobj,FALSE);
	resetobjs(level.buriedobjlist, FALSE);

	/* Hero is no longer on the map. */
	u.ux = u.uy = 0;

	/* Clear all memory from the level. */
	for(x=0; x<COLNO; x++) for(y=0; y<ROWNO; y++) {
	    levl[x][y].seenv = 0;
	    levl[x][y].waslit = 0;
	    clear_memory_glyph(x, y, S_stone);
	}

	fd = create_bonesfile(&u.uz, &bonesid, whynot);
	if(fd < 0) {
#ifdef WIZARD
		if(wizard)
			pline("%s", whynot);
#endif
		/* bones file creation problems are silent to the player.
		 * Keep it that way, but place a clue into the paniclog.
		 */
		paniclog("savebones", whynot);
		return;
	}
	c = (char) (strlen(bonesid) + 1);

#ifdef MFLOPPY  /* check whether there is room */
	if (iflags.checkspace) {
	    savelev(fd, ledger_no(&u.uz), COUNT_SAVE);
	    /* savelev() initializes bytes_counted to 0, so it must come
	     * first here even though it does not in the real save.  the
	     * resulting extra bflush() at the end of savelev() may increase
	     * bytes_counted by a couple over what the real usage will be.
	     *
	     * note it is safe to call store_version() here only because
	     * bufon() is null for ZEROCOMP, which MFLOPPY uses -- otherwise
	     * this code would have to know the size of the version
	     * information itself.
	     */
	    store_version(fd);
	    bwrite(fd, (void *) &c, sizeof c);
	    bwrite(fd, (void *) bonesid, (unsigned) c);	/* DD.nnn */
	    savefruitchn(fd, COUNT_SAVE);
	    bflush(fd);
	    if (bytes_counted > freediskspace(bones)) { /* not enough room */
# ifdef WIZARD
		if (wizard)
			pline("Insufficient space to create bones file.");
# endif
		(void) close(fd);
		cancel_bonesfile();
		return;
	    }
	    co_false();	/* make sure stuff before savelev() gets written */
	}
#endif /* MFLOPPY */

	store_version(fd);
	bwrite(fd, (void *) &c, sizeof c);
	bwrite(fd, (void *) bonesid, (unsigned) c);	/* DD.nnn */
	savefruitchn(fd, WRITE_SAVE | FREE_SAVE);
	update_mlstmv();	/* update monsters for eventual restoration */
	savelev(fd, ledger_no(&u.uz), WRITE_SAVE | FREE_SAVE);
	bclose(fd);
	commit_bonesfile(&u.uz);
	compress_bonesfile();
}

int
getbones()
{
	register int fd;
	register int ok;
	char c, *bonesid, oldbonesid[50];

#ifdef NO_BONES
	return(0);
#endif

	if(discover)		/* save bones files for real games */
		return(0);

	/* wizard check added by GAN 02/05/87 */
	if(rn2(3) && !(BonesLevelChange || u.uprops[BONES_CHANGE].extrinsic || have_bonestone())	/* only once in three times do we find bones */

#ifdef WIZARD
		&& !wizard
#endif
		) return(0);
	/* if you triggered a bones trap, bad luck - you can now get bones even if you disabled them :P --Amy */
	if (!iflags.bones && !(BonesLevelChange || u.uprops[BONES_CHANGE].extrinsic || have_bonestone()) ) return(0);
	if(no_bones_level(&u.uz)) return(0);
	fd = open_bonesfile(&u.uz, &bonesid);
	if (fd < 0) return(0);

	if ((ok = uptodate(fd, bones)) == 0) {
#ifdef WIZARD
	    if (!wizard)
#endif
		pline("Discarding unuseable bones; no need to panic...");
	} else {
#ifdef WIZARD
		if(wizard)  {
			if(yn("Get bones?") == 'n') {
				(void) close(fd);
				compress_bonesfile();
				return(0);
			}
		}
#endif
		mread(fd, (void *) &c, sizeof c);	/* length incl. '\0' */
		mread(fd, (void *) oldbonesid, (unsigned) c); /* DD.nnn */
		if (strcmp(bonesid, oldbonesid) != 0) {
			char errbuf[BUFSZ];

			sprintf(errbuf, "This is bones level '%s', not '%s'!",
				oldbonesid, bonesid);
#ifdef WIZARD
			if (wizard) {
				pline("%s", errbuf);
				ok = FALSE;	/* won't die of trickery */
			}
#endif
			trickery(errbuf);
		} else {
			register struct monst *mtmp;

			getlev(fd, 0, 0, TRUE);

			/* Note that getlev() now keeps tabs on unique
			 * monsters such as demon lords, and tracks the
			 * birth counts of all species just as makemon()
			 * does.  If a bones monster is extinct or has been
			 * subject to genocide, their mhpmax will be
			 * set to the magic DEFUNCT_MONSTER cookie value.
			 */
			for(mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
			    if (mtmp->mhpmax == DEFUNCT_MONSTER) {
#if defined(DEBUG) && defined(WIZARD)
				if (wizard)
				    pline("Removing defunct monster %s from bones.",
					mtmp->data->mname);
#endif
				mongone(mtmp);
			    } else
				/* to correctly reset named artifacts on the level */
				resetobjs(mtmp->minvent,TRUE);
			}
			resetobjs(fobj,TRUE);
			resetobjs(level.buriedobjlist,TRUE);
		}
	}
	(void) close(fd);

#ifdef WIZARD
	if(wizard) {
		if(yn("Unlink bones?") == 'n') {
			compress_bonesfile();
			return(ok);
		}
	}
#endif

	if (!rn2(10)) { /* rarely don't delete bones --Amy */
		compress_bonesfile();
		return(ok);
	}

	if (!delete_bonesfile(&u.uz)) {
		/* When N games try to simultaneously restore the same
		 * bones file, N-1 of them will fail to delete it
		 * (the first N-1 under AmigaDOS, the last N-1 under UNIX).
		 * So no point in a mysterious message for a normal event
		 * -- just generate a new level for those N-1 games.
		 */
		/* pline("Cannot unlink bones."); */
		return(0);
	}
	return(ok);
}

/*bones.c*/
