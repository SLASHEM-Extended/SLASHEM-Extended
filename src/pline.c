/*	SCCS Id: @(#)pline.c	3.4	1999/11/28	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#define NEED_VARARGS /* Uses ... */	/* comment line for pre-compiled headers */
#include "hack.h"
#include "epri.h"
#ifdef WIZARD
#include "edog.h"
#endif

#ifdef OVLB

static boolean no_repeat = FALSE;

static char *FDECL(You_buf, (int));

/*VARARGS1*/
/* Note that these declarations rely on knowledge of the internals
 * of the variable argument handling stuff in "tradstdc.h"
 */
char * FDECL(replace, (const char *, const char *, const char *));

#if defined(USE_STDARG) || defined(USE_VARARGS)
static void FDECL(vpline, (const char *, va_list));

void
pline VA_DECL(const char *, line)
	VA_START(line);
	VA_INIT(line, char *);
	vpline(line, VA_ARGS);
	VA_END();
}

# ifdef USE_STDARG
static void
vpline(const char *line, va_list the_args) {
# else
static void
vpline(line, the_args) const char *line; va_list the_args; {
# endif

#else	/* USE_STDARG | USE_VARARG */

#define vpline pline

void
pline VA_DECL(const char *, line)
#endif	/* USE_STDARG | USE_VARARG */

	char pbuf[BUFSZ];
/* Do NOT use VA_START and VA_END in here... see above */

	if (!line || !*line) return;

	if ( (MemoryLoss || u.uprops[MEMORY_LOST].extrinsic || have_memorylossstone() ) && !program_state.in_impossible && !program_state.in_paniclog && !program_state.panicking && !program_state.gameover 

/* buildfix by EternalEye: sinfo.exiting only exists on win32 */
#if defined(WIN32)
&& !program_state.exiting
#endif

) line = "Warning: Low Local Memory. Freeing description strings.";

	if (index(line, '%')) {
	    Vsprintf(pbuf,line,VA_ARGS);
	    line = pbuf;
	}

        if ( (RotThirteen || u.uprops[ROT_THIRTEEN].extrinsic || have_rotthirteenstone() ) && (strlen(line)<(BUFSZ-5)) &&(!program_state.in_impossible) ) {

		line = replace(line,"a","N");
		line = replace(line,"b","O");
		line = replace(line,"c","P");
		line = replace(line,"d","Q");
		line = replace(line,"e","R");
		line = replace(line,"f","S");
		line = replace(line,"g","T");
		line = replace(line,"h","U");
		line = replace(line,"i","V");
		line = replace(line,"j","W");
		line = replace(line,"k","X");
		line = replace(line,"l","Y");
		line = replace(line,"m","Z");
		line = replace(line,"n","A");
		line = replace(line,"o","B");
		line = replace(line,"p","C");
		line = replace(line,"q","D");
		line = replace(line,"r","E");
		line = replace(line,"s","F");
		line = replace(line,"t","G");
		line = replace(line,"u","H");
		line = replace(line,"v","I");
		line = replace(line,"w","J");
		line = replace(line,"x","K");
		line = replace(line,"y","L");
		line = replace(line,"z","M");

	  }  else if ( (BigscriptEffect || u.uprops[BIGSCRIPT].extrinsic || have_bigscriptstone() ) && (strlen(line)<(BUFSZ-5)) &&(!program_state.in_impossible) ) {

		line = replace(line,"a","A");
		line = replace(line,"b","B");
		line = replace(line,"c","C");
		line = replace(line,"d","D");
		line = replace(line,"e","E");
		line = replace(line,"f","F");
		line = replace(line,"g","G");
		line = replace(line,"h","H");
		line = replace(line,"i","I");
		line = replace(line,"j","J");
		line = replace(line,"k","K");
		line = replace(line,"l","L");
		line = replace(line,"m","M");
		line = replace(line,"n","N");
		line = replace(line,"o","O");
		line = replace(line,"p","P");
		line = replace(line,"q","Q");
		line = replace(line,"r","R");
		line = replace(line,"s","S");
		line = replace(line,"t","T");
		line = replace(line,"u","U");
		line = replace(line,"v","V");
		line = replace(line,"w","W");
		line = replace(line,"x","X");
		line = replace(line,"y","Y");
		line = replace(line,"z","Z");

	  }


/*Intercept direct speach, inpossible() and very short or long Strings here*/
/* to cut down unnecesary calls to the now slower replace */
/* other checks like read must be done dynamically because */
/* they depent on position -CK */
/* right : Ye read "Here lies ..."
   wrong : You read "Here lies ..."
   wrong : Ye read "'er lies ..." */
        if( (Role_if(PM_PIRATE) || Role_if(PM_KORSAIR) ) &&(*line!='"')&&(strlen(line)<(BUFSZ-5))
             &&(!program_state.in_impossible)
             &&(strlen(line)>9)){
                /* >9: "You die ..." but not "It hits." */
		line = replace(line,"You","Ye");
		line = replace(line,"you","ye");
		line = replace(line,"His","'is");
		line = replace(line," his"," 'is");
		line = replace(line,"Her","'er");
		line = replace(line," her"," 'er");
		line = replace(line,"Are","Be");
		line = replace(line," are"," be");
		line = replace(line,"Is ","Be");
		line = replace(line," is "," be ");
		line = replace(line," is."," be.");
		line = replace(line," is,"," be,");
		if (Role_if(PM_KORSAIR)) { /* words beginning with a c will begin with a k for korsairs --Amy */
		line = replace(line,"C","K");
		line = replace(line," c"," k");
		line = replace(line,"(c","(k");
		}
		line = replace(line,"Is ","Be ");
		line = replace(line,"Of ","O' ");
		line = replace(line," of "," o' ");
		line = replace(line,"Of.","O'.");
		line = replace(line," of."," o'.");
		line = replace(line,"Of,","O',");
		line = replace(line," of,"," o',");
		line = replace(line," ear"," lug");
		line = replace(line,"Ear","Lug");
		line = replace(line,"eye","deadlight");
		line = replace(line,"Eye","Deadlight");
                /* If orkmid isn't contained, save some time -CK */
                if(strstr(line,"orkmid") )
                {
                 line = replace(line,"zorkmids ","doubloons ");
                 line = replace(line,"Zorkmids ","Doubloons ");
                 line = replace(line,"zorkmids.","doubloons.");
                 line = replace(line,"Zorkmids.","Doubloons.");
                 line = replace(line,"zorkmids,","doubloons,");
                 line = replace(line,"Zorkmids,","Doubloons,");
                 line = replace(line,"zorkmids)","doubloons)");
                 line = replace(line,"Zorkmids)","Doubloons)");
                 line = replace(line,"zorkmid ","doubloon ");
                 line = replace(line,"Zorkmid ","Doubloon ");
                 line = replace(line,"zorkmid.","doubloon.");
                 line = replace(line,"Zorkmid.","Doubloon.");
                 line = replace(line,"zorkmid,","doubloon,");
                 line = replace(line,"Zorkmid,","Doubloon,");
                 line = replace(line,"zorkmid)","doubloon)");
                 line = replace(line,"Zorkmid)","Doubloon)");
                } /* endif orkmid */
                /* If old coin isn't contained, save some time -CK */
                if(strstr(line,"old coin") )
                {
                 line = replace(line,"gold coins","pieces of eight");
                 line = replace(line,"Gold coins","Pieces of eight");
                 line = replace(line,"gold coin","piece of eight");
                 line = replace(line,"Gold coin","Piece of eight");
                }
                /* If old piece isn't contained, save some time -CK */
                if(strstr(line,"old piece") )
                {
                 line = replace(line,"gold pieces.","pieces of eight");
                 line = replace(line,"Gold pieces.","Pieces of eight");
                 line = replace(line,"gold pieces,","pieces of eight");
                 line = replace(line,"Gold pieces,","Pieces of eight");
                 line = replace(line,"gold pieces ","pieces of eight");
                 line = replace(line,"Gold pieces ","Pieces of eight");
                 line = replace(line,"gold piece.","piece of eight");
                 line = replace(line,"Gold piece.","Piece of eight");
                 line = replace(line,"gold piece,","piece of eight");
                 line = replace(line,"Gold piece,","Piece of eight");
                 line = replace(line,"gold piece ","piece of eight");
                 line = replace(line,"Gold piece ","Piece of eight");
                } /* endif old piece */
        }  /* endif role_if(PM_PIRATE),etc. */
	if (!iflags.window_inited) {
	    raw_print(line);
	    return;
	}
#ifndef MAC
	if (no_repeat && !strcmp(line, toplines))
	    return;
#endif /* MAC */
	if (vision_full_recalc) vision_recalc(0);
	if (u.ux) flush_screen(1);		/* %% */
	putstr(WIN_MESSAGE, 0, line);
}

/*VARARGS1*/
void
Norep VA_DECL(const char *, line)
	VA_START(line);
	VA_INIT(line, const char *);
	no_repeat = TRUE;
	vpline(line, VA_ARGS);
	no_repeat = FALSE;
	VA_END();
	return;
}

/* work buffer for You(), &c and verbalize() */
static char *you_buf = 0;
static int you_buf_siz = 0;

static char *
You_buf(siz)
int siz;
{
	if (siz > you_buf_siz) {
		if (you_buf) free((genericptr_t) you_buf);
		you_buf_siz = siz + 10;
		you_buf = (char *) alloc((unsigned) you_buf_siz);
	}
	return you_buf;
}

void
free_youbuf()
{
	if (you_buf) free((genericptr_t) you_buf),  you_buf = (char *)0;
	you_buf_siz = 0;
}

/* `prefix' must be a string literal, not a pointer */
#define YouPrefix(pointer,prefix,text) \
 Strcpy((pointer = You_buf((int)(strlen(text) + sizeof prefix))), prefix)

#define YouMessage(pointer,prefix,text) \
 strcat((YouPrefix(pointer, prefix, text), pointer), text)

/*VARARGS1*/
void
You VA_DECL(const char *, line)
	char *tmp;
	VA_START(line);
	VA_INIT(line, const char *);
	vpline(YouMessage(tmp, "You ", line), VA_ARGS);
	VA_END();
}

/*VARARGS1*/
void
Your VA_DECL(const char *,line)
	char *tmp;
	VA_START(line);
	VA_INIT(line, const char *);
	vpline(YouMessage(tmp, "Your ", line), VA_ARGS);
	VA_END();
}

/*VARARGS1*/
void
You_feel VA_DECL(const char *,line)
	char *tmp;
	VA_START(line);
	VA_INIT(line, const char *);
	vpline(YouMessage(tmp, "You feel ", line), VA_ARGS);
	VA_END();
}


/*VARARGS1*/
void
You_cant VA_DECL(const char *,line)
	char *tmp;
	VA_START(line);
	VA_INIT(line, const char *);
	vpline(YouMessage(tmp, "You can't ", line), VA_ARGS);
	VA_END();
}

/*VARARGS1*/
void
pline_The VA_DECL(const char *,line)
	char *tmp;
	VA_START(line);
	VA_INIT(line, const char *);
	vpline(YouMessage(tmp, "The ", line), VA_ARGS);
	VA_END();
}

/*VARARGS1*/
void
There VA_DECL(const char *,line)
	char *tmp;
	VA_START(line);
	VA_INIT(line, const char *);
	vpline(YouMessage(tmp, "There ", line), VA_ARGS);
	VA_END();
}

/*VARARGS1*/
void
You_hear VA_DECL(const char *,line)
	char *tmp;
	VA_START(line);
	VA_INIT(line, const char *);
	if (Underwater)
		YouPrefix(tmp, "You barely hear ", line);
	else if (u.usleep)
		YouPrefix(tmp, "You dream that you hear ", line);
	else
		YouPrefix(tmp, "You hear ", line);
	vpline(strcat(tmp, line), VA_ARGS);
	VA_END();
}

/*VARARGS1*/
void
verbalize VA_DECL(const char *,line)
	char *tmp;
	if (!flags.soundok) return;
	VA_START(line);
	VA_INIT(line, const char *);
	tmp = You_buf((int)strlen(line) + sizeof "\"\"");
	Strcpy(tmp, "\"");
	Strcat(tmp, line);
	Strcat(tmp, "\"");
	vpline(tmp, VA_ARGS);
	VA_END();
}

/*VARARGS1*/
/* Note that these declarations rely on knowledge of the internals
 * of the variable argument handling stuff in "tradstdc.h"
 */

#if defined(USE_STDARG) || defined(USE_VARARGS)
static void FDECL(vraw_printf,(const char *,va_list));

void
raw_printf VA_DECL(const char *, line)
	VA_START(line);
	VA_INIT(line, char *);
	vraw_printf(line, VA_ARGS);
	VA_END();
}

# ifdef USE_STDARG
static void
vraw_printf(const char *line, va_list the_args) {
# else
static void
vraw_printf(line, the_args) const char *line; va_list the_args; {
# endif

#else  /* USE_STDARG | USE_VARARG */

void
raw_printf VA_DECL(const char *, line)
#endif
/* Do NOT use VA_START and VA_END in here... see above */

	if(!index(line, '%'))
	    raw_print(line);
	else {
	    char pbuf[BUFSZ];
	    Vsprintf(pbuf,line,VA_ARGS);
	    raw_print(pbuf);
	}
}


/*VARARGS1*/
void
impossible VA_DECL(const char *, s)
	VA_START(s);
	VA_INIT(s, const char *);
	if (program_state.in_impossible)
		panic("impossible called impossible");
	program_state.in_impossible = 1;
	{
	    char pbuf[BUFSZ];
	    Vsprintf(pbuf,s,VA_ARGS);
	    paniclog("impossible", pbuf);
	}
	vpline(s,VA_ARGS);
	pline("Program in disorder. Please inform Amy (Bluescreenofdeath at nethackwiki) about this bug.");
	program_state.in_impossible = 0;
	VA_END();
}

const char *
align_str(alignment)
    aligntyp alignment;
{
    switch ((int)alignment) {
	case A_CHAOTIC: return "chaotic";
	case A_NEUTRAL: return "neutral";
	case A_LAWFUL:	return "lawful";
	case A_NONE:	return "unaligned";
    }
    return "unknown";
}

const char *
hybrid_str()
{
    char string[BUFSZ];
    *string = '\0';
	if (flags.hybridangbander) Sprintf(eos(string), "angbander ");
	if (flags.hybridaquarian) Sprintf(eos(string), "aquarian ");
	if (flags.hybridcurser) Sprintf(eos(string), "curser ");
	if (flags.hybridhaxor) Sprintf(eos(string), "haxor ");
	if (flags.hybridhomicider) Sprintf(eos(string), "homicider ");
	if (flags.hybridsuxxor) Sprintf(eos(string), "suxxor ");
	if (flags.hybridwarper) Sprintf(eos(string), "warper ");
	if (flags.hybridrandomizer) Sprintf(eos(string), "randomizer ");
	if (flags.hybridnullrace) Sprintf(eos(string), "null ");
	if (flags.hybridmazewalker) Sprintf(eos(string), "mazewalker ");
	if (flags.hybridsoviet) Sprintf(eos(string), "soviet ");
	if (flags.hybridxrace) Sprintf(eos(string), "x-race ");
	if (flags.hybridheretic) Sprintf(eos(string), "heretic ");
	if (flags.hybridsokosolver) Sprintf(eos(string), "sokosolver ");
	if (flags.hybridspecialist) Sprintf(eos(string), "specialist ");
	if (flags.hybridamerican) Sprintf(eos(string), "american ");
	if (flags.hybridminimalist) Sprintf(eos(string), "minimalist ");
	if (flags.hybridnastinator) Sprintf(eos(string), "nastinator ");

	if (!(flags.hybridangbander) && !(flags.hybridaquarian) && !(flags.hybridcurser) && !(flags.hybridhaxor) && !(flags.hybridhomicider) && !(flags.hybridsuxxor) && !(flags.hybridwarper) && !(flags.hybridrandomizer) && !(flags.hybridnullrace) && !(flags.hybridmazewalker) && !(flags.hybridsoviet) && !(flags.hybridxrace) && !(flags.hybridheretic) && !(flags.hybridsokosolver) && !(flags.hybridspecialist) && !(flags.hybridamerican) && !(flags.hybridminimalist) && !(flags.hybridnastinator)) Sprintf(eos(string), "none ");

    return string;
}

const char *
hybrid_strcode()
{
    char string[BUFSZ];
    *string = '\0';
	if (flags.hybridangbander) Sprintf(eos(string), "Agb");
	if (flags.hybridaquarian) Sprintf(eos(string), "Aqu");
	if (flags.hybridcurser) Sprintf(eos(string), "Cur");
	if (flags.hybridhaxor) Sprintf(eos(string), "Hax");
	if (flags.hybridhomicider) Sprintf(eos(string), "Hom");
	if (flags.hybridsuxxor) Sprintf(eos(string), "Sux");
	if (flags.hybridwarper) Sprintf(eos(string), "War");
	if (flags.hybridrandomizer) Sprintf(eos(string), "Ran");
	if (flags.hybridnullrace) Sprintf(eos(string), "Nul");
	if (flags.hybridmazewalker) Sprintf(eos(string), "Maz");
	if (flags.hybridsoviet) Sprintf(eos(string), "Sov");
	if (flags.hybridxrace) Sprintf(eos(string), "Xra");
	if (flags.hybridheretic) Sprintf(eos(string), "Her");
	if (flags.hybridsokosolver) Sprintf(eos(string), "Sok");
	if (flags.hybridspecialist) Sprintf(eos(string), "Spe");
	if (flags.hybridamerican) Sprintf(eos(string), "Ame");
	if (flags.hybridminimalist) Sprintf(eos(string), "Min");
	if (flags.hybridnastinator) Sprintf(eos(string), "Nas");

	if (!(flags.hybridangbander) && !(flags.hybridaquarian) && !(flags.hybridcurser) && !(flags.hybridhaxor) && !(flags.hybridhomicider) && !(flags.hybridsuxxor) && !(flags.hybridwarper) && !(flags.hybridrandomizer) && !(flags.hybridnullrace) && !(flags.hybridmazewalker) && !(flags.hybridsoviet) && !(flags.hybridxrace) && !(flags.hybridheretic) && !(flags.hybridsokosolver) && !(flags.hybridspecialist) && !(flags.hybridamerican) && !(flags.hybridminimalist) && !(flags.hybridnastinator)) Sprintf(eos(string), "none");

    return string;
}

void
mstatusline(mtmp)
register struct monst *mtmp;
{
	aligntyp alignment;
	char info[BUFSZ], monnambuf[BUFSZ];

	if (mtmp->ispriest || mtmp->data == &mons[PM_ALIGNED_PRIEST]
				|| mtmp->data == &mons[PM_ANGEL])
		alignment = EPRI(mtmp)->shralign;
	else
		alignment = mtmp->data->maligntyp;
	alignment = (alignment > 0) ? A_LAWFUL :
		(alignment < 0) ? A_CHAOTIC :
		A_NEUTRAL;

	info[0] = 0;
	if (mtmp->mtame) {	  Strcat(info, ", tame");
#ifdef WIZARD
	    if (wizard) {
		Sprintf(eos(info), " (%d", mtmp->mtame);
		if (!mtmp->isminion)
		    Sprintf(eos(info), "; hungry %ld; apport %d; abuse %d",
			EDOG(mtmp)->hungrytime, EDOG(mtmp)->apport, EDOG(mtmp)->abuse);
		Strcat(info, ")");
	    }
#endif
	}
	else if (mtmp->mpeaceful) Strcat(info, ", peaceful");
	else if (mtmp->mtraitor)  Strcat(info, ", traitor");
	if (mtmp->meating)	  Strcat(info, ", eating");
	if (mtmp->mcan)		  Strcat(info, ", cancelled");
	if (mtmp->mconf)	  Strcat(info, ", confused");
	if (mtmp->mblinded || !mtmp->mcansee)
				  Strcat(info, ", blind");
	if (mtmp->mstun)	  Strcat(info, ", stunned");
	if (mtmp->msleeping)	  Strcat(info, ", asleep");
#if 0	/* unfortunately mfrozen covers temporary sleep and being busy
	   (donning armor, for instance) as well as paralysis */
	else if (mtmp->mfrozen)	  Strcat(info, ", paralyzed");
#else
	else if (mtmp->mfrozen || !mtmp->mcanmove)
				  Strcat(info, ", can't move");
#endif
				  /* [arbitrary reason why it isn't moving] */
	else if (mtmp->mstrategy & STRAT_WAITMASK)
				  Strcat(info, ", meditating");
	else if (mtmp->mflee) {	  Strcat(info, ", scared");
#ifdef WIZARD
	    if (wizard)		  Sprintf(eos(info), " (%d)", mtmp->mfleetim);
#endif
	}
	if (mtmp->mtrapped)	  Strcat(info, ", trapped");
	if (mtmp->mspeed)	  Strcat(info,
					mtmp->mspeed == MFAST ? ", fast" :
					mtmp->mspeed == MSLOW ? ", slow" :
					", ???? speed");
	if (mtmp->mundetected)	  Strcat(info, ", concealed");
	if (mtmp->minvis)	  Strcat(info, ", invisible");
	if (mtmp == u.ustuck)	  Strcat(info,
			(sticks(youmonst.data)) ? ", held by you" :
				u.uswallow ? (is_animal(u.ustuck->data) ?
				", swallowed you" :
				", engulfed you") :
				", holding you");
#ifdef STEED
	if (mtmp == u.usteed)	  Strcat(info, ", carrying you");
#endif
	if (mtmp->butthurt) Sprintf(eos(info), ", butthurt (%d)", mtmp->butthurt);

	/* avoid "Status of the invisible newt ..., invisible" */
	/* and unlike a normal mon_nam, use "saddled" even if it has a name */
	Strcpy(monnambuf, x_monnam(mtmp, ARTICLE_THE, (char *)0,
	    (SUPPRESS_IT|SUPPRESS_INVISIBLE), FALSE));

	pline("Status of %s (%s):  Level %d  HP %d(%d)  Pw %d(%d)  AC %d%s.",
		monnambuf,
		align_str(alignment),
		mtmp->m_lev,
		mtmp->mhp,
		mtmp->mhpmax,
		mtmp->m_en,
		mtmp->m_enmax,
		find_mac(mtmp),
		info);
}

void
mstatuslinebl(mtmp) /*for blessed stethoscope --Amy*/
register struct monst *mtmp;
{
	aligntyp alignment;
	char info[BUFSZ], monnambuf[BUFSZ];

	if (mtmp->ispriest || mtmp->data == &mons[PM_ALIGNED_PRIEST]
				|| mtmp->data == &mons[PM_ANGEL])
		alignment = EPRI(mtmp)->shralign;
	else
		alignment = mtmp->data->maligntyp;
	alignment = (alignment > 0) ? A_LAWFUL :
		(alignment < 0) ? A_CHAOTIC :
		A_NEUTRAL;

	info[0] = 0;
	if (mtmp->mtame) {	  Strcat(info, ", tame");
		Sprintf(eos(info), " (%d", mtmp->mtame);
		if (!mtmp->isminion)
		    Sprintf(eos(info), "; hungry %ld; apport %d",
			EDOG(mtmp)->hungrytime, EDOG(mtmp)->apport);
		Strcat(info, ")");
	}
	else if (mtmp->mpeaceful) Strcat(info, ", peaceful");
	else if (mtmp->mtraitor)  Strcat(info, ", traitor");
	if (mtmp->meating)	  Strcat(info, ", eating");
	if (mtmp->mcan)		  Strcat(info, ", cancelled");
	if (mtmp->mconf)	  Strcat(info, ", confused");
	if (mtmp->mblinded || !mtmp->mcansee)
				  Strcat(info, ", blind");
	if (mtmp->mstun)	  Strcat(info, ", stunned");
	if (mtmp->msleeping)	  Strcat(info, ", asleep");
#if 0	/* unfortunately mfrozen covers temporary sleep and being busy
	   (donning armor, for instance) as well as paralysis */
	else if (mtmp->mfrozen)	  Strcat(info, ", paralyzed");
#else
	else if (mtmp->mfrozen || !mtmp->mcanmove)
				  Strcat(info, ", can't move");
#endif
				  /* [arbitrary reason why it isn't moving] */
	else if (mtmp->mstrategy & STRAT_WAITMASK)
				  Strcat(info, ", meditating");
	else if (mtmp->mflee) {	  Strcat(info, ", scared");
	    Sprintf(eos(info), " (%d)", mtmp->mfleetim);
	}
	if (mtmp->mtrapped)	  Strcat(info, ", trapped");
	if (mtmp->mspeed)	  Strcat(info,
					mtmp->mspeed == MFAST ? ", fast" :
					mtmp->mspeed == MSLOW ? ", slow" :
					", ???? speed");
	if (mtmp->mundetected)	  Strcat(info, ", concealed");
	if (mtmp->minvis)	  Strcat(info, ", invisible");
	if (mtmp == u.ustuck)	  Strcat(info,
			(sticks(youmonst.data)) ? ", held by you" :
				u.uswallow ? (is_animal(u.ustuck->data) ?
				", swallowed you" :
				", engulfed you") :
				", holding you");
#ifdef STEED
	if (mtmp == u.usteed)	  Strcat(info, ", carrying you");
#endif

	/* avoid "Status of the invisible newt ..., invisible" */
	/* and unlike a normal mon_nam, use "saddled" even if it has a name */
	Strcpy(monnambuf, x_monnam(mtmp, ARTICLE_THE, (char *)0,
	    (SUPPRESS_IT|SUPPRESS_INVISIBLE), FALSE));

	pline("Status of %s (%s):  Level %d  HP %d(%d)  Pw %d(%d)  AC %d%s.",
		monnambuf,
		align_str(alignment),
		mtmp->m_lev,
		mtmp->mhp,
		mtmp->mhpmax,
		mtmp->m_en,
		mtmp->m_enmax,
		find_mac(mtmp),
		info);
}

void
ustatusline()
{
	char info[BUFSZ];

	info[0] = '\0';
	if (Sick) {
		Strcat(info, ", dying from");
		if (u.usick_type & SICK_VOMITABLE)
			Strcat(info, " food poisoning");
		if (u.usick_type & SICK_NONVOMITABLE) {
			if (u.usick_type & SICK_VOMITABLE)
				Strcat(info, " and");
			Strcat(info, " illness");
		}
	}
	if (Stoned)		Strcat(info, ", solidifying");
	if (Slimed)		Strcat(info, ", becoming slimy");
	if (Strangled)		Strcat(info, ", being strangled");
	if (Vomiting)		Strcat(info, ", nauseated"); /* !"nauseous" */
	if (Confusion)		Strcat(info, ", confused");
	if (Blind) {
	    Strcat(info, ", blind");
	    if (u.ucreamed) {
		if ((long)u.ucreamed < Blinded || Blindfolded
						|| !haseyes(youmonst.data))
		    Strcat(info, ", cover");
		Strcat(info, "ed by sticky goop");
	    }	/* note: "goop" == "glop"; variation is intentional */
	}
	if (Stunned)		Strcat(info, ", stunned");
	if (Numbed)		Strcat(info, ", numbed");
	if (Feared)		Strcat(info, ", stricken with fear");
	if (Frozen)		Strcat(info, ", frozen solid");
	if (Burned)		Strcat(info, ", burned");
#ifdef STEED
	if (!u.usteed)
#endif
	if (Wounded_legs) {
	    const char *what = body_part(LEG);
	    if ((Wounded_legs & BOTH_SIDES) == BOTH_SIDES)
		what = makeplural(what);
				Sprintf(eos(info), ", injured %s", what);
	}
	if (IsGlib)		Sprintf(eos(info), ", slippery %s",
					makeplural(body_part(HAND)));
	if (u.utrap)		Strcat(info, ", trapped");
	if (Fast)		Strcat(info, Very_fast ?
						", very fast" : ", fast");
	if (u.uundetected)	Strcat(info, ", concealed");
	if (Invis)		Strcat(info, ", invisible");
	if (u.ustuck) {
	    if (sticks(youmonst.data))
		Strcat(info, ", holding ");
	    else
		Strcat(info, ", held by ");
	    Strcat(info, mon_nam(u.ustuck));
	}

	pline("Status of %s (%s%s):  Level %d  HP %d(%d)  Pw %d(%d)  AC %d%s.",
		plname,
		    (u.ualign.record >= 20) ? "piously " :
		    (u.ualign.record > 13) ? "devoutly " :
		    (u.ualign.record > 8) ? "fervently " :
		    (u.ualign.record > 3) ? "stridently " :
		    (u.ualign.record == 3) ? "" :
		    (u.ualign.record >= 1) ? "haltingly " :
		    (u.ualign.record == 0) ? "nominally " :
					    "insufficiently ",
		align_str(u.ualign.type),
		Upolyd ? mons[u.umonnum].mlevel : u.ulevel,
		Upolyd ? u.mh : u.uhp,
		Upolyd ? u.mhmax : u.uhpmax,
		u.uen,
		u.uenmax,
		u.uac,
		info);
}

void
self_invis_message()
{
	if(Role_if(PM_PIRATE) || Role_if(PM_KORSAIR) ){
	pline("%s %s.",
	    Hallucination ? "Arr, Matey!  Ye" : "Avast!  All of a sudden, ye",
	    See_invisible ? "can see right through yerself" :
		"can't see yerself");
	}
	else{
	pline("%s %s.",
	    Hallucination ? "Far out, man!  You" : "Gee!  All of a sudden, you",
	    See_invisible ? "can see right through yourself" :
		"can't see yourself");
	}
}

/* replace() from CK */

/*Consecutive calls to replace would result in strings copied onto itselves*/
/*So we alternate between two buffers*/
char            replace_buffer[BUFSZ*2]; /* two buffers */
unsigned int    flipflop=0;              /* which one ? */
/* flipflop must be unsigned int (not int or boolean) to act as senitel */
/* If it wrote beyond the end of buffer, flipflop is >1 ... */
/* ... then you can panic */

char *replace(st, orig, repl)
const char *st, *orig, *repl;
{
        char *buffer;
	char *ch;
        char *tmp;
        int i;  

        /* Direct speach ? (cheapest test, and only once ) */
        /*"Thou art doomed, scapegrace!" */
        /*"Who do you think you are, War?" */
        /*"Hello Dudley. Welcome to Delphi."*/
        if( (*st)== '"' ) return st;

        /* at most 20 times we replace the word to prevent infinite loops */
        i=100;
REPEAT:
        /*Most calls won't match, so do the match first.*/
	if (!(ch = strstr(st, orig)))
		return st;

        /* You read "eyelbereth" */
        if( (tmp = strstr(st,"read")) && (tmp<ch) ) return st;

        /* A cursed rusty iron chain named Eye Pod */
        if( (tmp = strstr(st,"named")) && (tmp<ch) ) return st;

        /* A raven called Hugin - Eye of Odin */
        if( (tmp = strstr(st,"called")) && (tmp<ch) ) return st;

        /* A tiger eye ring (that is called tiger eye in inventory) */
        if( !strcmp(orig,"eye") && strstr(st,"tiger") ) return st;

        /* The Eye of the Aethiopica, The Eyes of the Overworld */
        if( !strcmp(orig,"Eye") && (
            strstr(ch,"Aethiopica") ||
            strstr(ch,"Overworld")
          )) return st;

	/* note by Amy - I think the Deadlight of the Aethiopica sounds funny... */

        /* Check if it will fit into the buffer */
        /* 2 is enough, but 5 is safer */
        /* Should be rare enough to come late */
        if( ( strlen(st)+strlen(repl)-strlen(orig)+5 )>BUFSZ ) return st;

/*Quote beginning in the middle of the string */
/*The voice of Moloch booms out "So, mortal!  You dare desecrate my High Temple!"*/
/* voice o' Moloch, but not Ye dare */
/* rare enough, to come last */
        if( (tmp=strstr(st,"\"")) && (tmp<ch) ) return st;

        /* Don't convert disorder messages into pirate slang ! */
        /* Nested calls of impossible() call panic(). */
        if(program_state.in_impossible) return st;

        /* get a buffer */
        buffer = &replace_buffer[(flipflop^=1)?BUFSZ:0];

        /* If it is the same buffer, something went wrong. */
        /* This may happen if you work with two strings at the same time */
        /* and make intersecting calls to replace */
        if(buffer==st)
        {
         /* Assert a disorder if in wizard mode */
         /* otherwise do it silently */
         if(wizard)
           impossible("Intersecting calls to replace() in pline.c !");
         return st;
        }

        strncpy(buffer, st, ch-st);  
	buffer[ch-st] = 0;
        sprintf(buffer+(ch-st), "%s%s", repl, ch+strlen(orig));

        /* we don't know how much data was destroyed, so assume the worst */
        if(flipflop>1)
          panic("Memory leak in replace() !");

        st=buffer;
        if(i--) goto REPEAT;
        return st;
}

#endif /* OVLB */
/*pline.c*/


