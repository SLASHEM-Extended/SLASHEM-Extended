/* 	SCCS Id: @(#)school.c 	3.1 	94/26/03	*/
/* NetHack may be freely redistributed.  See license for details. */
/* Copyright 1994, Sebastian Klein */

#include "hack.h"
#include <stdio.h>

#ifdef SOUNDS
#ifdef OVL0

static char *pupil_msgs[] = {
	"Today no homework ... *please*",
	"six times nine is ... um ... uh ... ... forty-two",
	"you ... Strange word",	/* You're not expected to understand this ... */
	"Bugger off!",
	"*uck off!",
	"What are the newest news about the Super Information Highway?",
	"What do you want?",
	"Do the world a favour---jump out of the 20th story of the Uni-Center!",
	NULL
};

static char *baumert_msgs[] = {
	"No chance! Every day you'll get homework!",
	"Is it really true? Does really _everybody_ have the homework?",
	"That usage of the word 'goes' does harm to my ears!",
	NULL
};

static char *gier_msgs[] = {
	"Your attitude is really unacceptable!",
	"The \"Stigel-Brauerei\" was founded 1492. Well, in that year was that affair with that guy, Columbus, but that really isn't important.",
	"Why are you going? I'm only 20 minutes late!",
	NULL
};

static char *dickler_msgs[] = {
	"Where's your problem? I'll be happy to help you",
	"You didn't understand? Then let's begin again ... (*sigh*)",
	NULL
};

static char *overbeck_msgs[] = {
	"No homework yet? - This can be changed!",
	"Overbecks - das Ueberbier",
	NULL
};

static char *klomp_msgs[] = {
	"How about dehydrating carbonhydrates today?",
	"Back when I was a pupil, the following thing happened ...",
	"Back when I was studying chemistry, the following thing happened ...",
	NULL
};

static char *alers_msgs[] = {
	"... dann ist die Scheisse am dampfen",
	"NIKI forever!",
	"Pascal forever!",
	"Yes ... I know that everything is easier in C, but I simply love Pascal ...",
	NULL
};

static char *geis_haastert_msgs[] = {
	"You have Str:0 (at most), so bugger off!",
	"Do it - who cares about the odd broken bone?",
	"You are sick because you were running for 30 minutes? So run another hour!",
	"Shall I help you? (takes the whip)",
	NULL
};

static char *koerfgen_msgs[] = {
	"We'll do that diagonally. *grin* (wipes sweat off head)",
	"*grin*",
	"You know, (*grin*) we'll have to do something now! (*grin*)",
	NULL
};

static char *ennen_msgs[] = {
	"How about a pupil experiment - cut off your ears?",
	"Yet another pupil experiment: the consequences of KCN ingested.",
	NULL
};

static char *ellerhold_msgs[] = {
	"Don't expect to get away without homework!",
	"No homework in the holidays? You must have something to do, after all!",
	"The low level of you all is really beyond acception!",
	NULL
};

static char *hartstone_msgs[] = {
	"There was endless work in the supervision and administration of the farm ...",
	/* it's really a shame that I can't think of more messages for him */
	NULL
};

static char *generic_msgs[] = {
	"I expect you to do your homework _regularly_ and _carefully_!",
	"The level of work is really very low nowadays!",
	"In _our_ times pupils were real pupils and teachers were real teachers!",
	"Back when pupils where real pupils and teachers were real teachers, everything was better!",
	NULL
};

static struct teacher_msg_desc {
	char *teacher_name;
	char **messages;
} teacher_msgs[] = {
	{ "Mr. Baumert",	baumert_msgs },
	{ "Mr. Gier",		gier_msgs },
	{ "Mrs. Dickler",	dickler_msgs },
	{ "Mr. Overbeck",	overbeck_msgs },
	{ "Mr. Klomp",		klomp_msgs },
	{ "Mr. Alers",		alers_msgs },
	{ "Mr. Geis",		geis_haastert_msgs },
	{ "Mrs. Haastert",	geis_haastert_msgs },
	{ "Mr. Koerfgen",	koerfgen_msgs },
	{ "Mr. Ennen",		ennen_msgs },
	{ "Mr. Ellerhold",	ellerhold_msgs },
	{ "Mr. Hartstone",	hartstone_msgs },
	{ NULL,			generic_msgs }
};

static char *principal_msgs[] = {
	"What's up?",
	"I really feel sick - there are so many things to do!",
	"Help me, I faint!",
	"We'll do that in groups of one person!",
	NULL
};

boolean is_principal(struct monst *mon)
{
	return(mon->mnum == PM_PRINCIPAL);
}

boolean is_teacher(struct monst *mon)
{
	return(mon->mnum == PM_TEACHER);
}

boolean is_educator(struct monst *mon)
{
	return(is_teacher(mon) || is_principal(mon));
}

static void select_message(char **msgs)
{
	int i;

	i = 0;
	while (msgs[i] != NULL) i++;
	verbalize(msgs[rn2(i)]);

}

static void select_teacher_message(struct teacher_msg_desc *mmsgs,char *tname)
{
	int i;

	i = 0;
	while (mmsgs[i].teacher_name != NULL) {
		if (strcmp(mmsgs[i].teacher_name,tname) == 0) {
			select_message(mmsgs[i].messages);
			return;
		}
		i++;
	}
	select_message(mmsgs[i].messages);
}

void pupil_sound(struct monst *mon)
{
	select_message(pupil_msgs);
}

void teacher_sound(struct monst *mon)
{
	select_teacher_message(teacher_msgs,NAME(mon));
}

void principal_sound(struct monst *mon)
{
	select_message(principal_msgs);
}

#endif /* OVL0 */

#endif /* SOUNDS */

/* school.c */
