/*	SCCS Id: @(#)quest.c	3.4	2000/05/05	*/
/*	Copyright 1991, M. Stephenson		  */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

/*  quest dungeon branch routines. */

#include "quest.h"
#include "qtext.h"

#define Not_firsttime	(on_level(&u.uz0, &u.uz))
#define Qstat(x)	(quest_status.x)

STATIC_DCL void on_start(void);
STATIC_DCL void on_locate(void);
STATIC_DCL void on_goal(void);
STATIC_DCL boolean not_capable(void);
STATIC_DCL int is_pure(BOOLEAN_P);
STATIC_DCL void expulsion(BOOLEAN_P);
STATIC_DCL void chat_with_leader(void);
STATIC_DCL void chat_with_nemesis(void);
STATIC_DCL void chat_with_guardian(void);
STATIC_DCL void prisoner_speaks(struct monst *);


STATIC_OVL void
on_start()
{
  if(!Qstat(first_start)) {
    qt_pager(QT_FIRSTTIME);
    Qstat(first_start) = TRUE;
  } else if((u.uz0.dnum != u.uz.dnum) || (u.uz0.dlevel < u.uz.dlevel)) {
    if(Qstat(not_ready) <= 2) qt_pager(QT_NEXTTIME);
    else	qt_pager(QT_OTHERTIME);
  }
}

STATIC_OVL void
on_locate()
{
  if(!Qstat(first_locate)) {
    qt_pager(QT_FIRSTLOCATE);
    Qstat(first_locate) = TRUE;
  } else if(u.uz0.dlevel < u.uz.dlevel && !Qstat(killed_nemesis))
	qt_pager(QT_NEXTLOCATE);
}

STATIC_OVL void
on_goal()
{
  if (Qstat(killed_nemesis)) {
    return;
  } else if (!Qstat(made_goal)) {
    qt_pager(QT_FIRSTGOAL);
    Qstat(made_goal) = 1;
  } else {
    qt_pager(QT_NEXTGOAL);
    if(Qstat(made_goal) < 7) Qstat(made_goal)++;
  }
}

void
onquest()
{
	if(u.uevent.qcompleted || Not_firsttime) return;
	if(!Is_special(&u.uz)) return;

	if(Is_qstart(&u.uz)) on_start();
	else if(Is_qlocate(&u.uz) && u.uz.dlevel > u.uz0.dlevel) on_locate();
	else if(Is_nemesis(&u.uz)) on_goal();
	return;
}

void
nemdead()
{
	if(!Qstat(killed_nemesis)) {
	    Qstat(killed_nemesis) = TRUE;
	    qt_pager(QT_KILLEDNEM);

		if (!Race_if(PM_BABYLONIAN)) {
			u.weapon_slots += 1;
			pline("As a bonus, you gain an additional skill slot!");
		}

	    /* quest-specific artifacts of all roles can randomly generate now; note that this doesn't mean the actual QAs,
	     * but rather special artifacts that are placed somewhere on a specific quest --Amy */
	    de_energise_artifact(ART_PROCOMFORT_SUPER);

	    if (achieveX.completed_rivalquest) {
		int quartinum = ART_ORB_OF_DETECTION;
		while (quartinum <= ART_PENUMBRAL_LASSO) {
			de_energise_artifact(quartinum);
			quartinum++;
		}
	    }

	    if (Role_if(PM_PRACTICANT) || isbeacher) {
		You("managed to overthrow %s's reign of terror!!!", noroelaname());
		if (u.practicantcash) {
			You("rummage through %s's possessions and find the box in which she kept the money she stole from you. The bitch spent some of it, but you take the remaining %d zorkmids and pocket them.", noroelaname(), u.practicantcash);
			u.ugold += u.practicantcash;
			u.practicantcash = 0;
		}
	    }
	}
}

void
artitouch()
{
	if(!Qstat(touched_artifact)) {
	    Qstat(touched_artifact) = TRUE;
	    qt_pager(QT_GOTIT);

		if (!Race_if(PM_BABYLONIAN)) {
			u.weapon_slots += 1;
			pline("As a bonus, you gain an additional skill slot!");
		}

	    exercise(A_WIS, TRUE);
	}
}

/* external hook for do.c (level change check) */
boolean
ok_to_quest()
{
	return((boolean)((Qstat(got_quest) || Qstat(got_thanks)))
			&& (is_pure(FALSE) > 0));
}

STATIC_OVL boolean
not_capable()
{
	return((boolean)(u.ulevel < MIN_QUEST_LEVEL));
}

STATIC_OVL int
is_pure(talk)
boolean talk;
{
    int purity;
    aligntyp original_alignment = u.ualignbase[A_ORIGINAL];

#ifdef WIZARD
    if (wizard && talk) {
	if (u.ualign.type != original_alignment) {
	    You("are currently %s instead of %s.",
		align_str(u.ualign.type), align_str(original_alignment));
	} else if (u.ualignbase[A_CURRENT] != original_alignment) {
	    You("have converted.");
	} else if (u.ualign.record < MIN_QUEST_ALIGN) {
	    You("are currently %d and require %d.",
		u.ualign.record, MIN_QUEST_ALIGN);
	    if (yn_function("adjust?", (char *)0, 'y') == 'y')
		u.ualign.record = MIN_QUEST_ALIGN;
	}
    }
#endif
    purity = (u.ualign.record >= MIN_QUEST_ALIGN &&
	      u.ualign.type == original_alignment &&
	      u.ualignbase[A_CURRENT] == original_alignment) ?  1 :
	     (u.ualignbase[A_CURRENT] != original_alignment) ? -1 : 0;
    return purity;
}

/*
 * Expell the player to the stairs on the parent of the quest dungeon.
 *
 * This assumes that the hero is currently _in_ the quest dungeon and that
 * there is a single branch to and from it.
 */
STATIC_OVL void
expulsion(seal)
boolean seal;
{
    branch *br;
    d_level *dest;
    struct trap *t;
    int portal_flag;

    br = dungeon_branch("The Quest");
    dest = (br->end1.dnum == u.uz.dnum) ? &br->end2 : &br->end1;
    portal_flag = u.uevent.qexpelled ? 0 :	/* returned via artifact? */
		  !seal ? 1 : -1;
    schedule_goto(dest, FALSE, FALSE, portal_flag, (char *)0, (char *)0);
    if (seal) {	/* remove the portal to the quest - sealing it off */
	int reexpelled = u.uevent.qexpelled;
	u.uevent.qexpelled = 1;
	/* Delete the near portal now; the far (main dungeon side)
	   portal will be deleted as part of arrival on that level.
	   If monster movement is in progress, any who haven't moved
	   yet will now miss out on a chance to wander through it... */
	for (t = ftrap; t; t = t->ntrap)
	    if (t->ttyp == MAGIC_PORTAL) break;
	if (t) deltrap(t);	/* (display might be briefly out of sync) */
	else if (!reexpelled) impossible("quest portal already gone?");
    }
}

/* Either you've returned to quest leader while carrying the quest
   artifact or you've just thrown it to/at him or her.  If quest
   completion text hasn't been given yet, give it now.  Otherwise
   give another message about the character keeping the artifact
   and using the magic portal to return to the dungeon. */
void
finish_quest(obj)
struct obj *obj;	/* quest artifact; possibly null if carrying Amulet */
{
	struct obj *otmp;

	if (u.uhave.amulet) {	/* unlikely but not impossible */
	    qt_pager(QT_HASAMULET);
	    /* leader IDs the real amulet but ignores any fakes */
	    if ((otmp = carrying(AMULET_OF_YENDOR)) != 0)
		fully_identify_obj(otmp);
		makeknown(otmp->otyp);

	    if (!Qstat(amulet_bonus)) {
		Qstat(amulet_bonus) = TRUE;
		u.weapon_slots += 2;
		pline("As a reward, you gain 2 extra skill slots!");
	    }
	    if (u.amuletcompletelyimbued && !Qstat(amulet_bonus2)) {
		Qstat(amulet_bonus2) = TRUE;
		u.weapon_slots += 3;
		pline("As an additional reward, you gain 3 extra skill slots!");
	    }

	} else {
	    qt_pager(!Qstat(got_thanks) ? QT_OFFEREDIT : QT_OFFEREDIT2);
	    /* should have obtained bell during quest;
	       if not, suggest returning for it now */
	    if ((otmp = carrying(BELL_OF_OPENING)) == 0)
		/*com_pager(5)*/qt_pager(QT_SILVERBELL); /* have individual messages for certain classes --Amy */
	}

	if (!Qstat(got_thanks)) {
		Qstat(got_thanks) = TRUE; /* filthy hangup cheater */
		u.weapon_slots += 2;
		pline("As a reward, you gain 2 extra skill slots!");
	}

	Qstat(got_thanks) = TRUE;

	if (obj) {
	    u.uevent.qcompleted = 1;	/* you did it! */
	    /* behave as if leader imparts sufficient info about the
	       quest artifact */
	    fully_identify_obj(obj);
	    makeknown(obj->otyp);
	    update_inventory();
	}
}

STATIC_OVL void
chat_with_leader()
{
	register struct monst *mon;

/*	Rule 0:	Cheater checks.					*/
	if(u.uhave.questart && !Qstat(met_nemesis))
	    Qstat(cheater) = TRUE;

/*	It is possible for you to get the amulet without completing
 *	the quest.  If so, try to induce the player to quest.
 */
	if(Qstat(got_thanks)) {
/*	Rule 1:	You've gone back with/without the amulet.	*/
	    if(u.uhave.amulet)	finish_quest((struct obj *)0);

/*	Rule 2:	You've gone back before going for the amulet.	*/
	    else		qt_pager(QT_POSTHANKS);
	}

/*	Rule 3: You've got the artifact and are back to return it. */
	  else if(u.uhave.questart) {
	    struct obj *otmp;

	    for (otmp = invent; otmp; otmp = otmp->nobj)
		if (is_quest_artifact(otmp)) break;

	    finish_quest(otmp);

/*	Rule 4: You haven't got the artifact yet.	*/
	} else if(Qstat(got_quest)) {
	    qt_pager(rn1(10, QT_ENCOURAGE));

/*	Rule 5: You aren't yet acceptable - or are you? */
	} else {
	  if(!Qstat(met_leader)) {
	    qt_pager(QT_FIRSTLEADER);
	    Qstat(met_leader) = TRUE;
	    Qstat(not_ready) = 0;
	  } else if (Qstat(not_ready) < MAX_QUEST_TRIES - 1) qt_pager(QT_NEXTLEADER);
		else qt_pager(QT_OTHERLEADER);
	  /* the quest leader might have passed through the portal into
	     the regular dungeon; none of the remaining make sense there */
	  if (!on_level(&u.uz, &qstart_level)) return;

	  if(not_capable()) {
	    qt_pager(QT_BADLEVEL);
	    exercise(A_WIS, TRUE);
	    expulsion(FALSE);
	  } else if(is_pure(TRUE) < 0) {
	    /*com_pager(QT_BANISHED);*/
	    qt_pager(QT_HERETIC); /* gotta have realistic dialogue for all quest leaders! --Amy */

		/* turn them all hostile --Amy */

		for (mon = fmon; mon; mon = mon->nmon)
		if (!DEADMONSTER(mon) && (mon->data->msound == MS_GUARDIAN || mon->data->msound == MS_LEADER) && mon->mpeaceful) {
		    mon->mpeaceful = 0;
		}
		Qstat(pissed_off) = TRUE;

	    /* expulsion(TRUE); */
	    /* expulsion(FALSE); */ /* prevent infinite loops for converted heroes */
	  } else if(is_pure(TRUE) == 0) {
	    qt_pager(QT_BADALIGN);
	    if(Qstat(not_ready) == MAX_QUEST_TRIES) {
	      qt_pager(QT_LASTLEADER);
	      /* expulsion(TRUE); */
	      expulsion(FALSE);
	    } else {
	      Qstat(not_ready)++;
	      exercise(A_WIS, TRUE);
	      expulsion(FALSE);
	    }
	  } else {	/* You are worthy! */
	    qt_pager(QT_ASSIGNQUEST);
	    exercise(A_WIS, TRUE);
	    Qstat(got_quest) = TRUE;
	  }
	}
}

void
leader_speaks(mtmp)
	register struct monst *mtmp;
{
	/* maybe you attacked leader? */
	if(!mtmp->mpeaceful) {
		Qstat(pissed_off) = TRUE;
		mtmp->mstrategy &= ~STRAT_WAITMASK;	/* end the inaction */
	}
	/* the quest leader might have passed through the portal into the
	   regular dungeon; if so, mustn't perform "backwards expulsion" */
	if (!on_level(&u.uz, &qstart_level)) return;

	if(Qstat(pissed_off)) {
	  qt_pager(QT_LASTLEADER); /* remove expulsion to prevent infinite loops --Amy */
	  /* expulsion(TRUE); */ /* since the quest cannot be sealed anymore to prevent the game from */
	  /* expulsion(FALSE); */ /* becoming unwinnable - whoever had that stupid idea anyway?! */
	} else chat_with_leader();
}

STATIC_OVL void
chat_with_nemesis()
{
/*	The nemesis will do most of the talking, but... */
	qt_pager(rn1(10, QT_DISCOURAGE));
	if(!Qstat(met_nemesis)) Qstat(met_nemesis++);
}

void
nemesis_speaks()
{
	if(!Qstat(in_battle)) {
	  if(u.uhave.questart) qt_pager(QT_NEMWANTSIT);
	  else if(Qstat(made_goal) == 1 || !Qstat(met_nemesis)) 
	      qt_pager(QT_FIRSTNEMESIS);
	  else if(Qstat(made_goal) < 4) qt_pager(QT_NEXTNEMESIS);
	  else if(Qstat(made_goal) < 7) qt_pager(QT_OTHERNEMESIS);
	  else if(!rn2(5))	qt_pager(rn1(10, QT_DISCOURAGE));
	  if(Qstat(made_goal) < 7) Qstat(made_goal)++;
	  Qstat(met_nemesis) = TRUE;
	} else /* he will spit out random maledictions */
	  if(!rn2(5))	qt_pager(rn1(10, QT_DISCOURAGE));
}

STATIC_OVL void
chat_with_guardian()
{
/*	These guys/gals really don't have much to say... */
	if (u.uhave.questart && Qstat(killed_nemesis))
	    qt_pager(rn1(5, QT_GUARDTALK2));
	else
	    qt_pager(rn1(5, QT_GUARDTALK));
}

STATIC_OVL void
prisoner_speaks (mtmp)
	register struct monst *mtmp;
{
	if ((mtmp->data == &mons[PM_PRISONER] || mtmp->data == &mons[PM_CASTLE_PRISONER] || mtmp->data == &mons[PM_YLVA_PRISONER] || mtmp->data == &mons[PM_YOUR_BROTHER] || mtmp->data == &mons[PM_YOUR_SISTER] || mtmp->data == &mons[PM_OCCASIONAL_FRIEND] || mtmp->data == &mons[PM_GIRL_OUTSIDE_GANG] || mtmp->data == &mons[PM_ARCANE_PRISONER] || mtmp->data == &mons[PM_GRAVITY_STRIKER] || mtmp->data == &mons[PM_POEZ_PRESIDENT] || mtmp->data == &mons[PM_IDIOT_HOME_PRINCESS]) &&
			(mtmp->mstrategy & STRAT_WAITMASK)) {
	    /* Awaken the prisoner */
	    if (canseemon(mtmp))
	    	pline("%s speaks:", Monnam(mtmp));
	    verbalize("I'm finally free!");
	    mtmp->mstrategy &= ~STRAT_WAITMASK;
	    mtmp->mpeaceful = 1;

	    /* Your god is happy... */
	    if (Race_if(PM_HUMANOID_DEVIL)) devil_misbehavior();
	    else adjalign(3);

		/* ...But the guards are not */
	    (void) angry_guards(FALSE);
	}
	return;
}

void
quest_chat(mtmp)
	register struct monst *mtmp;
{
    if (mtmp->m_id == Qstat(leader_m_id)) {
	chat_with_leader();
	return;
    }
    switch(mtmp->data->msound) {
	    case MS_NEMESIS:	chat_with_nemesis(); break;
	    case MS_GUARDIAN:	chat_with_guardian(); break;
	    default:	impossible("quest_chat: Unknown quest character %s.",
				   mon_nam(mtmp));
	}
}

void
quest_talk(mtmp)
	register struct monst *mtmp;
{
    if (mtmp->m_id == Qstat(leader_m_id)) {
	leader_speaks(mtmp);
	return;
    }
    switch(mtmp->data->msound) {
	    case MS_NEMESIS:	nemesis_speaks(); break;
	    case MS_DJINNI:	prisoner_speaks(mtmp); break;
	    default:		break;
	}
}

void
quest_stat_check(mtmp)
	struct monst *mtmp;
{
    if(mtmp->data->msound == MS_NEMESIS)
	Qstat(in_battle) = (mtmp->mcanmove && !mtmp->msleeping &&
			    monnear(mtmp, u.ux, u.uy));
}

/*quest.c*/
