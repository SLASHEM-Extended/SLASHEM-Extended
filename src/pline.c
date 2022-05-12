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

static char *You_buf(int);

#if defined(DUMP_LOG)
char msgs[DUMPMSGS][BUFSZ];
int msgs_count[DUMPMSGS];
int lastmsg = -1;
#endif

void
msgpline_add(typ, pattern)
     int typ;
     char *pattern;
{
    struct _plinemsg *tmp = (struct _plinemsg *) alloc(sizeof(struct _plinemsg));
    if (!tmp) return;
    tmp->msgtype = typ;
    tmp->pattern = strdup(pattern);
    tmp->next = pline_msg;
    pline_msg = tmp;
}

void
msgpline_free()
{
    struct _plinemsg *tmp = pline_msg;
    struct _plinemsg *tmp2;
    while (tmp) {
	free(tmp->pattern);
	tmp2 = tmp;
	tmp = tmp->next;
	free(tmp2);
    }
    pline_msg = NULL;
}

int
msgpline_type(msg)
     char *msg;
{
    struct _plinemsg *tmp = pline_msg;
    while (tmp) {
	if (pmatch(tmp->pattern, msg)) return tmp->msgtype;
	tmp = tmp->next;
    }
    return MSGTYP_NORMAL;
}

/*VARARGS1*/
/* Note that these declarations rely on knowledge of the internals
 * of the variable argument handling stuff in "tradstdc.h"
 */
char * replace(const char *, const char *, const char *, BOOLEAN_P);

#if defined(USE_STDARG) || defined(USE_VARARGS)
static void vpline(const char *, va_list);

void
pline VA_DECL(const char *, line)
	VA_START(line);
	VA_INIT(line, char *);
	vpline(line, VA_ARGS);
	VA_END();
}

char prevmsg[BUFSZ];

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
	int typ;
/* Do NOT use VA_START and VA_END in here... see above */

	if (!line || !*line) return;

	if (program_state.done_hup)
		return;

	if (PlayerHearsMessages && !program_state.in_impossible && !program_state.in_paniclog && !program_state.panicking && !program_state.gameover && rn2(3) && !u.captchahack

#if defined(WIN32)
&& !program_state.exiting
#endif

) line = fauxmessage();

	if (SpellColorRed && !rn2(10) && !program_state.in_impossible && !program_state.in_paniclog && !program_state.panicking && !program_state.gameover && !u.captchahack

#if defined(WIN32)
&& !program_state.exiting
#endif

) line = generate_garbage_string();

	if (LLMMessages && !program_state.in_impossible && !program_state.in_paniclog && !program_state.panicking && !program_state.gameover && !u.captchahack

/* buildfix by EternalEye: sinfo.exiting only exists on win32 */
#if defined(WIN32)
&& !program_state.exiting
#endif

) line = "Warning: Low Local Memory. Freeing description strings.";

	if (MessagesSuppressed && !program_state.in_impossible && !program_state.in_paniclog && !program_state.panicking && !program_state.gameover && !u.captchahack
#if defined(WIN32)
&& !program_state.exiting
#endif
) line = " ";

	if (index(line, '%')) {
	    vsnprintf(pbuf,sizeof pbuf,line,VA_ARGS);
	    line = pbuf;
	}

        if (RotThirteenCipher && (strlen(line)<(BUFSZ-5)) &&(!program_state.in_impossible) ) {

		line = replace(line,"a","N", FALSE);
		line = replace(line,"b","O", FALSE);
		line = replace(line,"c","P", FALSE);
		line = replace(line,"d","Q", FALSE);
		line = replace(line,"e","R", FALSE);
		line = replace(line,"f","S", FALSE);
		line = replace(line,"g","T", FALSE);
		line = replace(line,"h","U", FALSE);
		line = replace(line,"i","V", FALSE);
		line = replace(line,"j","W", FALSE);
		line = replace(line,"k","X", FALSE);
		line = replace(line,"l","Y", FALSE);
		line = replace(line,"m","Z", FALSE);
		line = replace(line,"n","A", FALSE);
		line = replace(line,"o","B", FALSE);
		line = replace(line,"p","C", FALSE);
		line = replace(line,"q","D", FALSE);
		line = replace(line,"r","E", FALSE);
		line = replace(line,"s","F", FALSE);
		line = replace(line,"t","G", FALSE);
		line = replace(line,"u","H", FALSE);
		line = replace(line,"v","I", FALSE);
		line = replace(line,"w","J", FALSE);
		line = replace(line,"x","K", FALSE);
		line = replace(line,"y","L", FALSE);
		line = replace(line,"z","M", FALSE);

	  }  else if (YouHaveBigscript && (strlen(line)<(BUFSZ-5)) &&(!program_state.in_impossible) ) {

		line = replace(line,"a","A", FALSE);
		line = replace(line,"b","B", FALSE);
		line = replace(line,"c","C", FALSE);
		line = replace(line,"d","D", FALSE);
		line = replace(line,"e","E", FALSE);
		line = replace(line,"f","F", FALSE);
		line = replace(line,"g","G", FALSE);
		line = replace(line,"h","H", FALSE);
		line = replace(line,"i","I", FALSE);
		line = replace(line,"j","J", FALSE);
		line = replace(line,"k","K", FALSE);
		line = replace(line,"l","L", FALSE);
		line = replace(line,"m","M", FALSE);
		line = replace(line,"n","N", FALSE);
		line = replace(line,"o","O", FALSE);
		line = replace(line,"p","P", FALSE);
		line = replace(line,"q","Q", FALSE);
		line = replace(line,"r","R", FALSE);
		line = replace(line,"s","S", FALSE);
		line = replace(line,"t","T", FALSE);
		line = replace(line,"u","U", FALSE);
		line = replace(line,"v","V", FALSE);
		line = replace(line,"w","W", FALSE);
		line = replace(line,"x","X", FALSE);
		line = replace(line,"y","Y", FALSE);
		line = replace(line,"z","Z", FALSE);

	  }

/*Intercept direct speach, inpossible() and very short or long Strings here*/
/* to cut down unnecesary calls to the now slower replace */
/* other checks like read must be done dynamically because */
/* they depent on position -CK */
/* right : Ye read "Here lies ..."
   wrong : You read "Here lies ..."
   wrong : Ye read "'er lies ..." */
        if( (Role_if(PM_PIRATE) || Role_if(PM_KORSAIR) || (uwep && uwep->oartifact == ART_ARRRRRR_MATEY) ) &&(*line!='"')&&(strlen(line)<(BUFSZ-5))
             &&(!program_state.in_impossible)
             &&(strlen(line)>9)){
                /* >9: "You die ..." but not "It hits." */
		line = replace(line,"You","Ye", TRUE);
		line = replace(line,"you","ye", TRUE);
		line = replace(line,"His","'is", TRUE);
		line = replace(line," his"," 'is", TRUE);
		line = replace(line,"Her","'er", TRUE);
		line = replace(line," her"," 'er", TRUE);
		line = replace(line,"Are","Be", TRUE);
		line = replace(line," are"," be", TRUE);
		line = replace(line,"Is ","Be", TRUE);
		line = replace(line," is "," be ", TRUE);
		line = replace(line," is."," be.", TRUE);
		line = replace(line," is,"," be,", TRUE);
		if (Role_if(PM_KORSAIR) || (uwep && uwep->oartifact == ART_ARRRRRR_MATEY) ) { /* words beginning with a c will begin with a k for korsairs --Amy */
		line = replace(line,"C","K", FALSE);
		line = replace(line," c"," k", FALSE);
		line = replace(line,"(c","(k", FALSE);
		}
		line = replace(line,"Is ","Be ", TRUE);
		line = replace(line,"Of ","O' ", TRUE);
		line = replace(line," of "," o' ", TRUE);
		line = replace(line,"Of.","O'.", TRUE);
		line = replace(line," of."," o'.", TRUE);
		line = replace(line,"Of,","O',", TRUE);
		line = replace(line," of,"," o',", TRUE);
		line = replace(line," ear"," lug", TRUE);
		line = replace(line,"Ear","Lug", TRUE);
		line = replace(line,"eye","deadlight", TRUE);
		line = replace(line,"Eye","Deadlight", TRUE);
                /* If orkmid isn't contained, save some time -CK */
                if(strstr(line,"orkmid") )
                {
                 line = replace(line,"zorkmids ","doubloons ", TRUE);
                 line = replace(line,"Zorkmids ","Doubloons ", TRUE);
                 line = replace(line,"zorkmids.","doubloons.", TRUE);
                 line = replace(line,"Zorkmids.","Doubloons.", TRUE);
                 line = replace(line,"zorkmids,","doubloons,", TRUE);
                 line = replace(line,"Zorkmids,","Doubloons,", TRUE);
                 line = replace(line,"zorkmids)","doubloons)", TRUE);
                 line = replace(line,"Zorkmids)","Doubloons)", TRUE);
                 line = replace(line,"zorkmid ","doubloon ", TRUE);
                 line = replace(line,"Zorkmid ","Doubloon ", TRUE);
                 line = replace(line,"zorkmid.","doubloon.", TRUE);
                 line = replace(line,"Zorkmid.","Doubloon.", TRUE);
                 line = replace(line,"zorkmid,","doubloon,", TRUE);
                 line = replace(line,"Zorkmid,","Doubloon,", TRUE);
                 line = replace(line,"zorkmid)","doubloon)", TRUE);
                 line = replace(line,"Zorkmid)","Doubloon)", TRUE);
                } /* endif orkmid */
                /* If old coin isn't contained, save some time -CK */
                if(strstr(line,"old coin") )
                {
                 line = replace(line,"gold coins","pieces of eight", TRUE);
                 line = replace(line,"Gold coins","Pieces of eight", TRUE);
                 line = replace(line,"gold coin","piece of eight", TRUE);
                 line = replace(line,"Gold coin","Piece of eight", TRUE);
                }
                /* If old piece isn't contained, save some time -CK */
                if(strstr(line,"old piece") )
                {
                 line = replace(line,"gold pieces.","pieces of eight", TRUE);
                 line = replace(line,"Gold pieces.","Pieces of eight", TRUE);
                 line = replace(line,"gold pieces,","pieces of eight", TRUE);
                 line = replace(line,"Gold pieces,","Pieces of eight", TRUE);
                 line = replace(line,"gold pieces ","pieces of eight", TRUE);
                 line = replace(line,"Gold pieces ","Pieces of eight", TRUE);
                 line = replace(line,"gold piece.","piece of eight", TRUE);
                 line = replace(line,"Gold piece.","Piece of eight", TRUE);
                 line = replace(line,"gold piece,","piece of eight", TRUE);
                 line = replace(line,"Gold piece,","Piece of eight", TRUE);
                 line = replace(line,"gold piece ","piece of eight", TRUE);
                 line = replace(line,"Gold piece ","Piece of eight", TRUE);
                } /* endif old piece */
        }  /* endif role_if(PM_PIRATE),etc. */

	/* ZAPM has buckazoids; shopkeepers and stuff will also use that term, so no check for direct speech --Amy */
	  if (zapmrename() && (strlen(line)<(BUFSZ-5))
		 &&(!program_state.in_impossible)
             &&(strlen(line)>9)) {
                if(strstr(line,"orkmid") )
                {
                 line = replace(line,"zorkmids ","buckazoids ", FALSE);
                 line = replace(line,"Zorkmids ","Buckazoids ", FALSE);
                 line = replace(line,"zorkmids.","buckazoids.", FALSE);
                 line = replace(line,"Zorkmids.","Buckazoids.", FALSE);
                 line = replace(line,"zorkmids,","buckazoids,", FALSE);
                 line = replace(line,"Zorkmids,","Buckazoids,", FALSE);
                 line = replace(line,"zorkmids)","buckazoids)", FALSE);
                 line = replace(line,"Zorkmids)","Buckazoids)", FALSE);
                 line = replace(line,"zorkmid ","buckazoid ", FALSE);
                 line = replace(line,"Zorkmid ","Buckazoid ", FALSE);
                 line = replace(line,"zorkmid.","buckazoid.", FALSE);
                 line = replace(line,"Zorkmid.","Buckazoid.", FALSE);
                 line = replace(line,"zorkmid,","buckazoid,", FALSE);
                 line = replace(line,"Zorkmid,","Buckazoid,", FALSE);
                 line = replace(line,"zorkmid)","buckazoid)", FALSE);
                 line = replace(line,"Zorkmid)","Buckazoid)", FALSE);
                } /* endif orkmid */
                /* If old coin isn't contained, save some time -CK */
                if(strstr(line,"old coin") )
                {
                 line = replace(line,"gold coins","buckazoids", FALSE);
                 line = replace(line,"Gold coins","Buckazoids", FALSE);
                 line = replace(line,"gold coin","buckazoids", FALSE);
                 line = replace(line,"Gold coin","Buckazoids", FALSE);
                }
                /* If old piece isn't contained, save some time -CK */
                if(strstr(line,"old piece") )
                {
                 line = replace(line,"gold pieces.","buckazoids.", FALSE);
                 line = replace(line,"Gold pieces.","Buckazoids.", FALSE);
                 line = replace(line,"gold pieces,","buckazoids,", FALSE);
                 line = replace(line,"Gold pieces,","Buckazoids,", FALSE);
                 line = replace(line,"gold pieces ","buckazoids ", FALSE);
                 line = replace(line,"Gold pieces ","Buckazoids ", FALSE);
                 line = replace(line,"gold pieces","buckazoids", FALSE);
                 line = replace(line,"Gold pieces","Buckazoids", FALSE);
                 line = replace(line,"gold piece.","buckazoid.", FALSE);
                 line = replace(line,"Gold piece.","Buckazoid.", FALSE);
                 line = replace(line,"gold piece,","buckazoid,", FALSE);
                 line = replace(line,"Gold piece,","Buckazoid,", FALSE);
                 line = replace(line,"gold piece ","buckazoid ", FALSE);
                 line = replace(line,"Gold piece ","Buckazoid ", FALSE);
                 line = replace(line,"gold piece","buckazoid", FALSE);
                 line = replace(line,"Gold piece","Buckazoid", FALSE);
                } /* endif old piece */
	  } /* endif ZAPM etc. */

#if defined(DUMP_LOG)
	if (DUMPMSGS > 0 && !program_state.gameover) {
		/* count identical messages */
		if (!strncmp(msgs[lastmsg], line, BUFSZ)) {
			msgs_count[lastmsg] += 1;
		} else if (strncmp(line, "Unknown command", 15) ) {
			lastmsg = (lastmsg + 1) % DUMPMSGS;
			strncpy(msgs[lastmsg], line, BUFSZ);
			msgs_count[lastmsg] = 1;
		}
	}
#endif

	typ = msgpline_type(line);
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
	if (typ == MSGTYP_NOSHOW && !(MommaBugEffect || u.uprops[MOMMA_BUG].extrinsic || (uimplant && uimplant->oartifact == ART_DEINE_MUDDA) || have_mommystone()) && !(uwep && uwep->oartifact == ART_BRAND_BRAND) && !(u.twoweap && uswapwep && uswapwep->oartifact == ART_BRAND_BRAND) && !(uarmf && uarmf->oartifact == ART_GRENEUVENIA_S_HUG)) return;
	if (typ == MSGTYP_NOREP && !(MommaBugEffect || u.uprops[MOMMA_BUG].extrinsic || (uimplant && uimplant->oartifact == ART_DEINE_MUDDA) || have_mommystone()) && !(uwep && uwep->oartifact == ART_BRAND_BRAND) && !(u.twoweap && uswapwep && uswapwep->oartifact == ART_BRAND_BRAND) && !(uarmf && uarmf->oartifact == ART_GRENEUVENIA_S_HUG) && !strcmp(line, prevmsg)) return;
	putstr(WIN_MESSAGE, 0, line);

	if (HeapEffectBug || u.uprops[HEAP_EFFECT].extrinsic || have_heapstone()) {
		int heaping = rno(4);
		if (!rn2(7)) heaping += rno(8);
		while (heaping > 0) {
			heaping--;
			putstr(WIN_MESSAGE, 0, line);
		}
	}

	strncpy(prevmsg, line, BUFSZ);
	if (typ == MSGTYP_STOP && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE); /* --more-- */
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
		if (you_buf) free((void *) you_buf);
		you_buf_siz = siz + 10;
		you_buf = (char *) alloc((unsigned) you_buf_siz);
	}
	return you_buf;
}

void
free_youbuf()
{
	if (you_buf) free((void *) you_buf),  you_buf = (char *)0;
	you_buf_siz = 0;
}

/* `prefix' must be a string literal, not a pointer */
#define YouPrefix(pointer,prefix,text) \
 strcpy((pointer = You_buf((int)(strlen(text) + sizeof prefix))), prefix)

#define YouMessage(pointer,prefix,text) \
 strcat((YouPrefix(pointer, prefix, text), pointer), text)

/*VARARGS1*/
void
You VA_DECL(const char *, line)
	char *tmp;
	VA_START(line);
	VA_INIT(line, const char *);
	if ((FilteringBug || u.uprops[FILTERING_BUG].extrinsic || have_filteringstone()) && !program_state.in_impossible && !program_state.in_paniclog && !program_state.panicking && !program_state.gameover && rn2(5))
		vpline(fauxmessage(), VA_ARGS);
	else
		vpline(YouMessage(tmp, "You ", line), VA_ARGS);
	VA_END();
}

/*VARARGS1*/
void
Your VA_DECL(const char *,line)
	char *tmp;
	VA_START(line);
	VA_INIT(line, const char *);
	if ((FilteringBug || u.uprops[FILTERING_BUG].extrinsic || have_filteringstone()) && !program_state.in_impossible && !program_state.in_paniclog && !program_state.panicking && !program_state.gameover && rn2(5))
		vpline(fauxmessage(), VA_ARGS);
	else
		vpline(YouMessage(tmp, "Your ", line), VA_ARGS);
	VA_END();
}

/*VARARGS1*/
void
You_feel VA_DECL(const char *,line)
	char *tmp;
	VA_START(line);
	VA_INIT(line, const char *);
	if ((FilteringBug || u.uprops[FILTERING_BUG].extrinsic || have_filteringstone()) && !program_state.in_impossible && !program_state.in_paniclog && !program_state.panicking && !program_state.gameover && rn2(5))
		vpline(fauxmessage(), VA_ARGS);
	else
		vpline(YouMessage(tmp, "You feel ", line), VA_ARGS);
	VA_END();
}


/*VARARGS1*/
void
You_cant VA_DECL(const char *,line)
	char *tmp;
	VA_START(line);
	VA_INIT(line, const char *);
	if ((FilteringBug || u.uprops[FILTERING_BUG].extrinsic || have_filteringstone()) && !program_state.in_impossible && !program_state.in_paniclog && !program_state.panicking && !program_state.gameover && rn2(5))
		vpline(fauxmessage(), VA_ARGS);
	else
		vpline(YouMessage(tmp, "You can't ", line), VA_ARGS);
	VA_END();
}

/*VARARGS1*/
void
pline_The VA_DECL(const char *,line)
	char *tmp;
	VA_START(line);
	VA_INIT(line, const char *);
	if ((FilteringBug || u.uprops[FILTERING_BUG].extrinsic || have_filteringstone()) && !program_state.in_impossible && !program_state.in_paniclog && !program_state.panicking && !program_state.gameover && rn2(5))
		vpline(fauxmessage(), VA_ARGS);
	else
		vpline(YouMessage(tmp, "The ", line), VA_ARGS);
	VA_END();
}

/*VARARGS1*/
void
There VA_DECL(const char *,line)
	char *tmp;
	VA_START(line);
	VA_INIT(line, const char *);
	if ((FilteringBug || u.uprops[FILTERING_BUG].extrinsic || have_filteringstone()) && !program_state.in_impossible && !program_state.in_paniclog && !program_state.panicking && !program_state.gameover && rn2(5))
		vpline(fauxmessage(), VA_ARGS);
	else
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
	strcpy(tmp, "\"");
	strcat(tmp, line);
	strcat(tmp, "\"");
	vpline(tmp, VA_ARGS);
	VA_END();
}

/*VARARGS1*/
/* Note that these declarations rely on knowledge of the internals
 * of the variable argument handling stuff in "tradstdc.h"
 */

#if defined(USE_STDARG) || defined(USE_VARARGS)
static void vraw_printf(const char *,va_list);

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
	    vsnprintf(pbuf,sizeof pbuf,line,VA_ARGS);
	    raw_print(pbuf);
	}
}


/*VARARGS1*/
void
impossible VA_DECL(const char *, s)
	{
	char pbuf[BUFSZ];
	VA_START(s);
	VA_INIT(s, const char *);
	if (program_state.in_impossible)
		panic("impossible called impossible");
	program_state.in_impossible = 1;
	vsnprintf(pbuf,sizeof pbuf,s,VA_ARGS);
	pbuf[BUFSZ-1] = '\0'; /* sanity */
	paniclog("impossible", pbuf);
	pline("%s", pbuf); /*used to be vpline(s,VA_ARGS); but that was causing funky issues --Amy */
	pline("Program in disorder. Please inform Amy (Bluescreenofdeath at nethackwiki) about this bug.");
	pline("The admins can also be contacted on the #em.slashem.me IRC channel (Libera).");
	program_state.in_impossible = 0;
	VA_END();
	}
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
    static char string[BUFSZ];
    *string = '\0';

	int hybridcount = 0;

	if (flags.hybridangbander && (hybridcount++ < 20)) sprintf(eos(string), "angbander ");
	if (flags.hybridaquarian && (hybridcount++ < 20)) sprintf(eos(string), "aquarian ");
	if (flags.hybridcurser && (hybridcount++ < 20)) sprintf(eos(string), "curser ");
	if (flags.hybridhaxor && (hybridcount++ < 20)) sprintf(eos(string), "haxor ");
	if (flags.hybridhomicider && (hybridcount++ < 20)) sprintf(eos(string), "homicider ");
	if (flags.hybridsuxxor && (hybridcount++ < 20)) sprintf(eos(string), "suxxor ");
	if (flags.hybridwarper && (hybridcount++ < 20)) sprintf(eos(string), "warper ");
	if (flags.hybridrandomizer && (hybridcount++ < 20)) sprintf(eos(string), "randomizer ");
	if (flags.hybridnullrace && (hybridcount++ < 20)) sprintf(eos(string), "null ");
	if (flags.hybridmazewalker && (hybridcount++ < 20)) sprintf(eos(string), "mazewalker ");
	if (flags.hybridsoviet && (hybridcount++ < 20)) sprintf(eos(string), "soviet ");
	if (flags.hybridxrace && (hybridcount++ < 20)) sprintf(eos(string), "x-race ");
	if (flags.hybridheretic && (hybridcount++ < 20)) sprintf(eos(string), "heretic ");
	if (flags.hybridsokosolver && (hybridcount++ < 20)) sprintf(eos(string), "sokosolver ");
	if (flags.hybridspecialist && (hybridcount++ < 20)) sprintf(eos(string), "specialist ");
	if (flags.hybridamerican && (hybridcount++ < 20)) sprintf(eos(string), "american ");
	if (flags.hybridminimalist && (hybridcount++ < 20)) sprintf(eos(string), "minimalist ");
	if (flags.hybridnastinator && (hybridcount++ < 20)) sprintf(eos(string), "nastinator ");
	if (flags.hybridrougelike && (hybridcount++ < 20)) sprintf(eos(string), "rougelike ");
	if (flags.hybridsegfaulter && (hybridcount++ < 20)) sprintf(eos(string), "segfaulter ");
	if (flags.hybridironman && (hybridcount++ < 20)) sprintf(eos(string), "ironman ");
	if (flags.hybridamnesiac && (hybridcount++ < 20)) sprintf(eos(string), "amnesiac ");
	if (flags.hybridproblematic && (hybridcount++ < 20)) sprintf(eos(string), "problematic ");
	if (flags.hybridwindinhabitant && (hybridcount++ < 20)) sprintf(eos(string), "windinhabitant ");
	if (flags.hybridaggravator && (hybridcount++ < 20)) sprintf(eos(string), "aggravator ");
	if (flags.hybridevilvariant && (hybridcount++ < 20)) sprintf(eos(string), "evilvariant ");
	if (flags.hybridlevelscaler && (hybridcount++ < 20)) sprintf(eos(string), "levelscaler ");
	if (flags.hybriderosator && (hybridcount++ < 20)) sprintf(eos(string), "erosator ");
	if (flags.hybridroommate && (hybridcount++ < 20)) sprintf(eos(string), "roommate ");
	if (flags.hybridextravator && (hybridcount++ < 20)) sprintf(eos(string), "extravator ");
	if (flags.hybridhallucinator && (hybridcount++ < 20)) sprintf(eos(string), "hallucinator ");
	if (flags.hybridbossrusher && (hybridcount++ < 20)) sprintf(eos(string), "bossrusher ");
	if (flags.hybriddorian && (hybridcount++ < 20)) sprintf(eos(string), "dorian ");
	if (flags.hybridtechless && (hybridcount++ < 20)) sprintf(eos(string), "techless ");
	if (flags.hybridblait && (hybridcount++ < 20)) sprintf(eos(string), "blait ");
	if (flags.hybridgrouper && (hybridcount++ < 20)) sprintf(eos(string), "grouper ");
	if (flags.hybridscriptor && (hybridcount++ < 20)) sprintf(eos(string), "scriptor ");
	if (flags.hybridunbalancor && (hybridcount++ < 20)) sprintf(eos(string), "unbalancor ");
	if (flags.hybridbeacher && (hybridcount++ < 20)) sprintf(eos(string), "beacher ");
	if (flags.hybridstairseeker && (hybridcount++ < 20)) sprintf(eos(string), "stairseeker ");
	if (flags.hybridmatrayser && (hybridcount++ < 20)) sprintf(eos(string), "matrayser ");
	if (flags.hybridfeminizer && (hybridcount++ < 20)) sprintf(eos(string), "feminizer ");
	if (flags.hybridchallenger && (hybridcount++ < 20)) sprintf(eos(string), "challenger ");
	if (flags.hybridmatrixer && (hybridcount++ < 20)) sprintf(eos(string), "matrixer ");
	if (hybridcount >= 20) sprintf(eos(string), "(%d hybrids) ", hybridcount);

	if (!(flags.hybridangbander) && !(flags.hybridaquarian) && !(flags.hybridcurser) && !(flags.hybridhaxor) && !(flags.hybridhomicider) && !(flags.hybridsuxxor) && !(flags.hybridwarper) && !(flags.hybridrandomizer) && !(flags.hybridnullrace) && !(flags.hybridmazewalker) && !(flags.hybridsoviet) && !(flags.hybridxrace) && !(flags.hybridheretic) && !(flags.hybridsokosolver) && !(flags.hybridspecialist) && !(flags.hybridamerican) && !(flags.hybridminimalist) && !(flags.hybridnastinator) && !(flags.hybridrougelike) && !(flags.hybridsegfaulter) && !(flags.hybridironman) && !(flags.hybridamnesiac) && !(flags.hybridproblematic) && !(flags.hybridwindinhabitant) && !(flags.hybridaggravator) && !(flags.hybridevilvariant) && !(flags.hybridlevelscaler) && !(flags.hybriderosator) && !(flags.hybridroommate) && !(flags.hybridextravator) && !(flags.hybridhallucinator) && !(flags.hybridbossrusher) && !(flags.hybriddorian) && !(flags.hybridtechless) && !(flags.hybridblait) && !(flags.hybridgrouper) && !(flags.hybridscriptor) && !(flags.hybridunbalancor) && !(flags.hybridbeacher) && !(flags.hybridstairseeker) && !(flags.hybridmatrayser) && !(flags.hybridfeminizer) && !(flags.hybridchallenger) && !(flags.hybridmatrixer)) sprintf(eos(string), "none ");

    return (string);
}

const char *
hybrid_strcode()
{
    static char string[BUFSZ];
    *string = '\0';
	if (flags.hybridangbander) sprintf(eos(string), "Agb");
	if (flags.hybridaquarian) sprintf(eos(string), "Aqu");
	if (flags.hybridcurser) sprintf(eos(string), "Cur");
	if (flags.hybridhaxor) sprintf(eos(string), "Hax");
	if (flags.hybridhomicider) sprintf(eos(string), "Hom");
	if (flags.hybridsuxxor) sprintf(eos(string), "Sux");
	if (flags.hybridwarper) sprintf(eos(string), "War");
	if (flags.hybridrandomizer) sprintf(eos(string), "Ran");
	if (flags.hybridnullrace) sprintf(eos(string), "Nul");
	if (flags.hybridmazewalker) sprintf(eos(string), "Maz");
	if (flags.hybridsoviet) sprintf(eos(string), "Sov");
	if (flags.hybridxrace) sprintf(eos(string), "Xra");
	if (flags.hybridheretic) sprintf(eos(string), "Her");
	if (flags.hybridsokosolver) sprintf(eos(string), "Sok");
	if (flags.hybridspecialist) sprintf(eos(string), "Spe");
	if (flags.hybridamerican) sprintf(eos(string), "Ame");
	if (flags.hybridminimalist) sprintf(eos(string), "Min");
	if (flags.hybridnastinator) sprintf(eos(string), "Nas");
	if (flags.hybridrougelike) sprintf(eos(string), "Rou");
	if (flags.hybridsegfaulter) sprintf(eos(string), "Seg");
	if (flags.hybridironman) sprintf(eos(string), "Iro");
	if (flags.hybridamnesiac) sprintf(eos(string), "Amn");
	if (flags.hybridproblematic) sprintf(eos(string), "Pro");
	if (flags.hybridwindinhabitant) sprintf(eos(string), "Win");
	if (flags.hybridaggravator) sprintf(eos(string), "Agg");
	if (flags.hybridevilvariant) sprintf(eos(string), "Evi");
	if (flags.hybridlevelscaler) sprintf(eos(string), "Lvl");
	if (flags.hybriderosator) sprintf(eos(string), "Ero");
	if (flags.hybridroommate) sprintf(eos(string), "Roo");
	if (flags.hybridextravator) sprintf(eos(string), "Ext");
	if (flags.hybridhallucinator) sprintf(eos(string), "Hal");
	if (flags.hybridbossrusher) sprintf(eos(string), "Bos");
	if (flags.hybriddorian) sprintf(eos(string), "Dor");
	if (flags.hybridtechless) sprintf(eos(string), "Tec");
	if (flags.hybridblait) sprintf(eos(string), "Bla");
	if (flags.hybridgrouper) sprintf(eos(string), "Gro");
	if (flags.hybridscriptor) sprintf(eos(string), "Scr");
	if (flags.hybridunbalancor) sprintf(eos(string), "Unb");
	if (flags.hybridbeacher) sprintf(eos(string), "Bea");
	if (flags.hybridstairseeker) sprintf(eos(string), "Sta");
	if (flags.hybridmatrayser) sprintf(eos(string), "Mat");
	if (flags.hybridfeminizer) sprintf(eos(string), "Fem");
	if (flags.hybridchallenger) sprintf(eos(string), "Cha");
	if (flags.hybridmatrixer) sprintf(eos(string), "Max");

	if (!(flags.hybridangbander) && !(flags.hybridaquarian) && !(flags.hybridcurser) && !(flags.hybridhaxor) && !(flags.hybridhomicider) && !(flags.hybridsuxxor) && !(flags.hybridwarper) && !(flags.hybridrandomizer) && !(flags.hybridnullrace) && !(flags.hybridmazewalker) && !(flags.hybridsoviet) && !(flags.hybridxrace) && !(flags.hybridheretic) && !(flags.hybridsokosolver) && !(flags.hybridspecialist) && !(flags.hybridamerican) && !(flags.hybridminimalist) && !(flags.hybridnastinator) && !(flags.hybridrougelike) && !(flags.hybridsegfaulter) && !(flags.hybridironman) && !(flags.hybridamnesiac) && !(flags.hybridproblematic) && !(flags.hybridwindinhabitant) && !(flags.hybridaggravator) && !(flags.hybridevilvariant) && !(flags.hybridlevelscaler) && !(flags.hybriderosator) && !(flags.hybridroommate) && !(flags.hybridextravator) && !(flags.hybridhallucinator) && !(flags.hybridbossrusher) && !(flags.hybriddorian) && !(flags.hybridtechless) && !(flags.hybridblait) && !(flags.hybridgrouper) && !(flags.hybridscriptor) && !(flags.hybridunbalancor) && !(flags.hybridbeacher) && !(flags.hybridstairseeker) && !(flags.hybridmatrayser) && !(flags.hybridfeminizer) && !(flags.hybridchallenger) && !(flags.hybridmatrixer)) sprintf(eos(string), "none ");

    return (string);
}

const char *
generate_garbage_string()
{
	static char string[BUFSZ];
	string[0] = '\0';

	char tmpstr[2] = {0, 0};

	int stringlength = rnd(25);
	if (!rn2(10)) stringlength += rnd(25);

	while (stringlength --> 0) {
#ifdef UNIX
		/* Sadly, terminals can be screwed up if we use "unsafe" glyphs. Sure, this is SLEX, interface screws
		 * are meant to be a part of the game, but we don't want to fuck up the player's actual terminal session.
		 * After all, even SLEX has limits. Therefore we need to limit the strings to "safe" glyphs. --Amy */
		tmpstr[0] = 32 + rnd(94);
#else
		tmpstr[0] = rnd(255);
#endif

		/* get rid of some control codes that mess everything up */
		switch (tmpstr[0]) {
			case 7: case 10: case 11: case 13: case '%':
				tmpstr[0] = ' ';
		}

		sprintf(eos(string), "%s", tmpstr);
	}

	return string;
}

const char *
generate_garbage_char()
{
	static char string[BUFSZ];
	string[0] = '\0';

	char tmpstr[2] = {0, 0};

#ifdef UNIX
	tmpstr[0] = 32 + rnd(94);
#else
	tmpstr[0] = rnd(255);
#endif

	/* get rid of some control codes that mess everything up */
	switch (tmpstr[0]) {
		case 7: case 10: case 11: case 13: case '%':
			tmpstr[0] = ' ';
	}
	tmpstr[1] = '\0';

	sprintf(eos(string), "%s", tmpstr);

	return string;
}

void
mstatusline(mtmp)
register struct monst *mtmp;
{
	aligntyp alignment;
	char info[BUFSZ], monnambuf[BUFSZ];

	if (mtmp->ispriest || mtmp->data == &mons[PM_ALIGNED_PRIEST] || mtmp->data == &mons[PM_ELITE_PRIEST] || mtmp->data == &mons[PM_MASTER_PRIEST]
				|| mtmp->data == &mons[PM_ANGEL])
		alignment = EPRI(mtmp)->shralign;
	else
		alignment = mtmp->data->maligntyp;
	alignment = (alignment > 0) ? A_LAWFUL :
		(alignment < 0) ? A_CHAOTIC :
		A_NEUTRAL;

	info[0] = 0;
	if (mtmp->mtame) {	  strcat(info, ", tame");
#ifdef WIZARD
	    if (wizard) {
		sprintf(eos(info), " (%d", mtmp->mtame);
		if (!mtmp->isminion)
		    sprintf(eos(info), "; hungry %ld; apport %d; abuse %d",
			EDOG(mtmp)->hungrytime, EDOG(mtmp)->apport, EDOG(mtmp)->abuse);
		strcat(info, ")");
	    }
#endif
	}
	else if (mtmp->mpeaceful) strcat(info, ", peaceful");
	else if (mtmp->mtraitor)  strcat(info, ", traitor");
	if (mtmp->meating)	  strcat(info, ", eating");
	if (mtmp->mcan)		  strcat(info, ", cancelled");
	if (mtmp->mconf)	  strcat(info, ", confused");
	if (mtmp->healblock)	  strcat(info, ", healing blocked");
	if (mtmp->inertia)	  strcat(info, ", slowed by inertia");
	if (mtmp->bleedout)	  strcat(info, ", bleeding");
	if (mtmp->mblinded || !mtmp->mcansee)
				  strcat(info, ", blind");
	if (mtmp->mstun)	  strcat(info, ", stunned");
	if (mtmp->msleeping)	  strcat(info, ", asleep");
#if 0	/* unfortunately mfrozen covers temporary sleep and being busy
	   (donning armor, for instance) as well as paralysis */
	else if (mtmp->mfrozen)	  strcat(info, ", paralyzed");
#else
	else if (mtmp->mfrozen || !mtmp->mcanmove) {
		strcat(info, ", can't move");
		if (mtmp->masleep) strcat(info, " (sleeping)");
	}
#endif
				  /* [arbitrary reason why it isn't moving] */
	else if (mtmp->mstrategy & STRAT_WAITMASK)
				  strcat(info, ", meditating");
	else if (mtmp->mflee) {	  strcat(info, ", scared");
#ifdef WIZARD
	    if (wizard)		  sprintf(eos(info), " (%d)", mtmp->mfleetim);
#endif
	}
	if (mtmp->mtrapped)	  strcat(info, ", trapped");
	if (mtmp->mspeed)	  strcat(info,
					mtmp->mspeed == MFAST ? ", fast" :
					mtmp->mspeed == MSLOW ? ", slow" :
					", ???? speed");
	if (mtmp->mundetected)	  strcat(info, ", concealed");
	if (mtmp->minvis)	  strcat(info, ", invisible");
	if (mtmp == u.ustuck)	  strcat(info,
			(sticks(youmonst.data)) ? ", held by you" :
				u.uswallow ? (is_animal(u.ustuck->data) ?
				", swallowed you" :
				", engulfed you") :
				", holding you");
	if (mtmp == u.usteed)	  strcat(info, ", carrying you");
	if (mtmp->butthurt) sprintf(eos(info), ", butthurt (%d)", mtmp->butthurt);

	/* avoid "Status of the invisible newt ..., invisible" */
	/* and unlike a normal mon_nam, use "saddled" even if it has a name */
	strcpy(monnambuf, x_monnam(mtmp, ARTICLE_THE, (char *)0,
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

	if (mtmp->ispriest || mtmp->data == &mons[PM_ALIGNED_PRIEST] || mtmp->data == &mons[PM_MASTER_PRIEST] || mtmp->data == &mons[PM_ELITE_PRIEST]
				|| mtmp->data == &mons[PM_ANGEL])
		alignment = EPRI(mtmp)->shralign;
	else
		alignment = mtmp->data->maligntyp;
	alignment = (alignment > 0) ? A_LAWFUL :
		(alignment < 0) ? A_CHAOTIC :
		A_NEUTRAL;

	info[0] = 0;
	if (mtmp->mtame) {	  strcat(info, ", tame");
		sprintf(eos(info), " (%d", mtmp->mtame);
		if (!mtmp->isminion)
		    sprintf(eos(info), "; hungry %ld; apport %d",
			EDOG(mtmp)->hungrytime, EDOG(mtmp)->apport);
		strcat(info, ")");
	}
	else if (mtmp->mpeaceful) strcat(info, ", peaceful");
	else if (mtmp->mtraitor)  strcat(info, ", traitor");
	if (mtmp->meating)	  strcat(info, ", eating");
	if (mtmp->mcan)		  strcat(info, ", cancelled");
	if (mtmp->mconf)	  strcat(info, ", confused");
	if (mtmp->healblock)	  strcat(info, ", healing blocked");
	if (mtmp->inertia)	  strcat(info, ", slowed by inertia");
	if (mtmp->bleedout)	  strcat(info, ", bleeding");
	if (mtmp->mblinded || !mtmp->mcansee)
				  strcat(info, ", blind");
	if (mtmp->mstun)	  strcat(info, ", stunned");
	if (mtmp->msleeping)	  strcat(info, ", asleep");
#if 0	/* unfortunately mfrozen covers temporary sleep and being busy
	   (donning armor, for instance) as well as paralysis */
	else if (mtmp->mfrozen)	  strcat(info, ", paralyzed");
#else
	else if (mtmp->mfrozen || !mtmp->mcanmove) {
		strcat(info, ", can't move");
		if (mtmp->masleep) strcat(info, " (sleeping)");
	}
#endif
				  /* [arbitrary reason why it isn't moving] */
	else if (mtmp->mstrategy & STRAT_WAITMASK)
				  strcat(info, ", meditating");
	else if (mtmp->mflee) {	  strcat(info, ", scared");
	    sprintf(eos(info), " (%d)", mtmp->mfleetim);
	}
	if (mtmp->mtrapped)	  strcat(info, ", trapped");
	if (mtmp->mspeed)	  strcat(info,
					mtmp->mspeed == MFAST ? ", fast" :
					mtmp->mspeed == MSLOW ? ", slow" :
					", ???? speed");
	if (mtmp->mundetected)	  strcat(info, ", concealed");
	if (mtmp->minvis)	  strcat(info, ", invisible");
	if (mtmp == u.ustuck)	  strcat(info,
			(sticks(youmonst.data)) ? ", held by you" :
				u.uswallow ? (is_animal(u.ustuck->data) ?
				", swallowed you" :
				", engulfed you") :
				", holding you");
	if (mtmp == u.usteed)	  strcat(info, ", carrying you");

	/* avoid "Status of the invisible newt ..., invisible" */
	/* and unlike a normal mon_nam, use "saddled" even if it has a name */
	strcpy(monnambuf, x_monnam(mtmp, ARTICLE_THE, (char *)0,
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
		strcat(info, ", dying from");
		if (u.usick_type & SICK_VOMITABLE)
			strcat(info, " food poisoning");
		if (u.usick_type & SICK_NONVOMITABLE) {
			if (u.usick_type & SICK_VOMITABLE)
				strcat(info, " and");
			strcat(info, " illness");
		}
	}
	if (Stoned)		strcat(info, ", solidifying");
	if (Slimed)		strcat(info, ", becoming slimy");
	if (Strangled)		strcat(info, ", being strangled");
	if (PlayerBleeds)		strcat(info, ", bleeding");
	if (Vomiting)		strcat(info, ", nauseated"); /* !"nauseous" */
	if (Confusion)		strcat(info, ", confused");
	if (Blind) {
	    strcat(info, ", blind");
	    if (u.ucreamed) {
		if ((long)u.ucreamed < Blinded || Blindfolded
						|| !haseyes(youmonst.data))
		    strcat(info, ", cover");
		strcat(info, "ed by sticky goop");
	    }	/* note: "goop" == "glop"; variation is intentional */
	}
	if (Stunned)		strcat(info, ", stunned");
	if (Numbed)		strcat(info, ", numbed");
	if (Feared)		strcat(info, ", stricken with fear");
	if (Frozen)		strcat(info, ", frozen solid");
	if (Burned)		strcat(info, ", burned");
	if (Dimmed)		strcat(info, ", dimmed");
	if (!u.usteed)
	if (Wounded_legs) {
	    const char *what = body_part(LEG);
	    if ((Wounded_legs & BOTH_SIDES) == BOTH_SIDES)
		what = makeplural(what);
				sprintf(eos(info), ", injured %s", what);
	}
	if (IsGlib)		sprintf(eos(info), ", slippery %s",
					makeplural(body_part(HAND)));
	if (u.utrap)		strcat(info, ", trapped");
	if (Fast)		strcat(info, Very_fast ?
						", very fast" : ", fast");
	if (u.uundetected)	strcat(info, ", concealed");
	if (Invis)		strcat(info, ", invisible");
	if (u.ustuck) {
	    if (sticks(youmonst.data))
		strcat(info, ", holding ");
	    else
		strcat(info, ", held by ");
	    strcat(info, mon_nam(u.ustuck));
	}

	pline("Status of %s (%s%s):  Level %d  HP %d(%d)  Pw %d(%d)  AC %d%s.",
		playeraliasname,
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
	if(Role_if(PM_PIRATE) || Role_if(PM_KORSAIR) || (uwep && uwep->oartifact == ART_ARRRRRR_MATEY) ){
	pline("%s %s.",
	    FunnyHallu ? "Arr, Matey!  Ye" : "Avast!  All of a sudden, ye",
	    See_invisible ? "can see right through yerself" :
		"can't see yerself");
	}
	else{
	pline("%s %s.",
	    FunnyHallu ? "Far out, man!  You" : "Gee!  All of a sudden, you",
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

char *replace(st, orig, repl, extratest)
const char *st, *orig, *repl;
boolean extratest;
{
        char *buffer;
	char *ch;
        char *tmp;
        int i;  

        /* Direct speach ? (cheapest test, and only once ) */
        /*"Thou art doomed, scapegrace!" */
        /*"Who do you think you are, War?" */
        /*"Hello Dudley. Welcome to Delphi."*/
        if(extratest && ((*st)== '"') ) return st;

        /* at most 20 times we replace the word to prevent infinite loops */
        i=100;
REPEAT:
        /*Most calls won't match, so do the match first.*/
	if (!(ch = strstr(st, orig)))
		return st;

        /* You read "eyelbereth" */
        /*if( (tmp = strstr(st,"read")) && (tmp<ch) ) return st;*/
	/* note by Amy: already caught by the direct speech test above */

        /* A cursed rusty iron chain named Eye Pod */
        /* if( (tmp = strstr(st,"named")) && (tmp<ch) ) return st; */

        /* A raven called Hugin - Eye of Odin */
        /* if( (tmp = strstr(st,"called")) && (tmp<ch) ) return st; */

        /* A tiger eye ring (that is called tiger eye in inventory) */
        /* if( !strcmp(orig,"eye") && strstr(st,"tiger") ) return st; */

        /* The Eye of the Aethiopica, The Eyes of the Overworld */
        /* if( !strcmp(orig,"Eye") && (
            strstr(ch,"Aethiopica") ||
            strstr(ch,"Overworld")
          )) return st; */

	/* note by Amy - I think the Deadlight of the Aethiopica sounds funny... */
	/* tiger deadlight ring etc. too :D */

        /* Check if it will fit into the buffer */
        /* 2 is enough, but 5 is safer */
        /* Should be rare enough to come late */
        if( ( strlen(st)+strlen(repl)-strlen(orig)+5 )>BUFSZ ) return st;

/*Quote beginning in the middle of the string */
/*The voice of Moloch booms out "So, mortal!  You dare desecrate my High Temple!"*/
/* voice o' Moloch, but not Ye dare */
/* rare enough, to come last */
        if( (tmp=strstr(st,"\"")) && (tmp<ch) && extratest ) return st;

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


