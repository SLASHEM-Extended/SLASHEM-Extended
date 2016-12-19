/* Write live game progress changes to a log file
 * Needs xlog-v3 patch. */

#include "hack.h"

#ifdef LIVELOGFILE

/* Encodes the current xlog "achieve" status to an integer */
long
encodeachieve(void)
{
  /* Achievement bitfield:
   * bit  meaning
   *  0   obtained the Bell of Opening
   *  1   entered gehennom (by any means)
   *  2   obtained the Candelabrum of Invocation
   *  3   obtained the Book of the Dead
   *  4   performed the invocation ritual
   *  5   obtained the amulet
   *  6   entered elemental planes
   *  7   entered astral plane
   *  8   ascended (not "escaped in celestial disgrace!")
   *  9   obtained the luckstone from the Mines
   *  10  obtained the sokoban prize
   *  11  killed medusa
   */

  long r;

  r = 0;

  if(achieve.get_bell)            r |= 1L << 0;
  if(achieve.enter_gehennom)      r |= 1L << 1;
  if(achieve.get_candelabrum)     r |= 1L << 2;
  if(achieve.get_book)            r |= 1L << 3;
  if(achieve.perform_invocation)  r |= 1L << 4;
  if(achieve.get_amulet)          r |= 1L << 5;
  if(In_endgame(&u.uz))           r |= 1L << 6;
  if(Is_astralevel(&u.uz))        r |= 1L << 7;
  if(achieve.ascended)            r |= 1L << 8;
  if(achieve.get_luckstone)       r |= 1L << 9;
  if(achieve.finish_sokoban)      r |= 1L << 10;
  if(achieve.killed_medusa)       r |= 1L << 11;
  if(achieve.killed_nightmare)    r |= 1L << 12;
  if(achieve.killed_vecna)        r |= 1L << 13;
  if(achieve.killed_beholder)     r |= 1L << 14;
  if(achieve.killed_ruggo)        r |= 1L << 15;
  if(achieve.killed_kroo)         r |= 1L << 16;
  if(achieve.killed_grund)        r |= 1L << 17;
  if(achieve.killed_largestgiant) r |= 1L << 18;
  if(achieve.killed_shelob)       r |= 1L << 19;
  if(achieve.killed_girtab)       r |= 1L << 20;
  if(achieve.killed_aphrodite)    r |= 1L << 21;
  if(achieve.killed_frankenstein) r |= 1L << 22;
  if(achieve.killed_croesus)      r |= 1L << 23;
  if(achieve.killed_dagon)        r |= 1L << 24;
  if(achieve.killed_hydra)        r |= 1L << 25;

  return r;
}

/* Keep the last xlog "achieve" value to be able to compare */
long last_achieve_int;

/* Generic buffer for snprintf */
#define STRBUF_LEN (4096)
char strbuf[STRBUF_LEN];

/* Open the live log file */
boolean livelog_start() {


	last_achieve_int = encodeachieve();

	return TRUE;
}

/* Locks the live log file and writes 'buffer' */
void livelog_write_string(char* buffer) {
	FILE* livelogfile;

#ifdef FILE_AREAS
	if (lock_file_area(LOGAREA, LIVELOGFILE, 10)) {
#else
	if (lock_file(LIVELOGFILE, SCOREPREFIX, 10)) {
#endif
		if(!(livelogfile = fopen_datafile(LIVELOGFILE, "a", SCOREPREFIX))) {
			pline("Cannot open live log file!");
		} else {
			fprintf(livelogfile, buffer);
			(void) fclose(livelogfile);
		}
		unlock_file_area(LOGAREA, LIVELOGFILE);
	}
}

/* Writes changes in the achieve structure to the live log.
 * Called from various places in the NetHack source,
 * usually where xlog's achieve is set. */
void livelog_achieve_update() {
	long achieve_int, achieve_diff;

	achieve_int = encodeachieve();
	achieve_diff = last_achieve_int ^ achieve_int;

	/* livelog_achieve_update is sometimes called when there's
	 * no actual change. */
	if(achieve_diff == 0) {
		return;
	}

	snprintf(strbuf, STRBUF_LEN,
		"player=%s:role=%s:race=%s:gender=%s:align=%s:hybrid=%s:turns=%ld:achieve=0x%lx:achieve_diff=0x%lx\n",
		plname, 
		urole.filecode, urace.filecode, genders[flags.female].filecode, aligns[1-u.ualign.type].filecode,
		hybrid_strcode(),
		moves, 
		achieve_int,
		achieve_diff);
	livelog_write_string(strbuf);

	last_achieve_int = achieve_int;

	if (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "team splat cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "vosklitsatel'nyy znak plashch komanda") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "jamoasi xavfsizlik plash") )) pline("TROPHY GET!");

	if (uarmc && uarmc->oartifact == ART_JUNETHACK______WINNER) {
		u.uhpmax += 10;
		u.uenmax += 10;
		if (Upolyd) u.mhmax += 10;
		pline("Well done! Your maximum health and mana were increased to make sure you'll get even more trophies! Go for it!");
	}

}

/* Reports wishes */
void
livelog_wish(item)
char *item;
{
	snprintf(strbuf, STRBUF_LEN,
		"player=%s:role=%s:race=%s:gender=%s:align=%s:hybrid=%s:turns=%ld:wish=%s\n",
		plname,
		urole.filecode, urace.filecode, genders[flags.female].filecode, aligns[1-u.ualign.type].filecode,
		hybrid_strcode(),
		moves,
		item);
	livelog_write_string(strbuf);
}

/* Reports wishes */
void
livelog_avert_death()
{
	snprintf(strbuf, STRBUF_LEN,
		"player=%s:role=%s:race=%s:gender=%s:align=%s:hybrid=%s:turns=%ld:message=%s\n",
		plname,
		urole.filecode, urace.filecode, genders[flags.female].filecode, aligns[1-u.ualign.type].filecode,
		hybrid_strcode(),
		moves,
		"averted death");
	livelog_write_string(strbuf);
}

/* Shout */
#ifdef LIVELOG_SHOUT

int 
doshout()
{
	char buf[BUFSZ], qbuf[QBUFSZ];
	char* p;
	
	Sprintf(qbuf,"Shout what?");
	getlin(qbuf, buf);
	
	You("shout into the void: %s", buf);

	/* filter livelog delimiter */
	for (p = buf; *p != 0; p++)
		if( *p == ':' )
			*p = ' ';

	snprintf(strbuf, STRBUF_LEN,
		"player=%s:role=%s:race=%s:gender=%s:align=%s:hybrid=%s:turns=%ld:shout=%s\n",
		plname,
		urole.filecode, urace.filecode, genders[flags.female].filecode, aligns[1-u.ualign.type].filecode,
		hybrid_strcode(),
		moves,
		buf);
	livelog_write_string(strbuf);
	
	return 0;
}

#endif /* LIVELOG_SHOUT */

#ifdef LIVELOG_BONES_KILLER 
void 
livelog_bones_killed(mtmp) 
struct monst *mtmp; 
{ 
	char *name = NAME(mtmp); 
 
	if (name && mtmp->former_rank && mtmp->former_rank[0]) { 
		/* $player killed the $bones_monst of $bones_killed the former 
		 * $bones_rank on $turns on dungeon level $dlev! */ 
		snprintf(strbuf, STRBUF_LEN, 
				"player=%s:role=%s:race=%s:gender=%s:align=%s:hybrid=%s:turns=%ld:dlev=%d:" 
				"bones_killed=%s:bones_rank=%s:bones_monst=%s\n", 
				plname, 
				urole.filecode, urace.filecode, genders[flags.female].filecode, aligns[1-u.ualign.type].filecode,
				hybrid_strcode(),
				moves, 
				depth(&u.uz), 
				name, 
				mtmp->former_rank, 
				mtmp->data->mname); 
		livelog_write_string(strbuf); 
	} else if ((mtmp->data->geno & G_UNIQ) 
#ifdef BLACKMARKET 
	           || (mtmp->data == &mons[PM_BLACK_MARKETEER]) 
#endif 
		  ) { 
		char *n = noit_mon_nam(mtmp); 
		/* $player killed a uniq monster */ 
		snprintf(strbuf, STRBUF_LEN, 
				"player=%s:role=%s:race=%s:gender=%s:align=%s:hybrid=%s:turns=%ld:killed_uniq=%s\n", 
				plname, 
				urole.filecode, urace.filecode, genders[flags.female].filecode, aligns[1-u.ualign.type].filecode,
				hybrid_strcode(),
				moves, 
				n); 
		livelog_write_string(strbuf); 
	} 
} 
#endif /* LIVELOG_BONES_KILLER */ 

#endif /* LIVELOGFILE */
