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
   *  12  killed Nightmare
   *  13  killed Vecna
   *  14  killed Beholder
   *  15  killed Ruggo
   *  16  killed Kroo
   *  17  killed Grund
   *  18  killed The Largest Giant
   *  19  killed Shelob
   *  20  killed Girtab
   *  21  killed Aphrodite
   *  22  killed Frankenstein
   *  23  killed Croesus
   *  24  killed Dagon
   *  25  killed Hydra
   *  26  imbued the Bell of Opening
   *  27  imbued the Amulet of Yendor
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
  if(achieve.imbued_bell)         r |= 1L << 26;
  if(achieve.imbued_amulet)       r |= 1L << 27;

  return r;
}

/* Encodes the current xlog "achieveX" status to an integer */
long
encodeachieveX(void)
{
  /* Achievement bitfield:
   * bit  meaning
   *  0   killed an elder priest
   *  1   killed the Motherfucker Glass Golem
   *  2   killed Tiksrvzllat
   *  3   killed the BOFH
   *  4   reached the bottom of the Swimming Pools
   *  5   killed Erogenous Katia
   *  6   killed the Witch King of Angmar
   *  7   obtained the stone of magic resistance from the Deep Mines
   *  8   completed all the DevNull dungeons
   *  9   killed the Minotaur of the Maze
   *  10  killed Kalwina
   */

  long r;

  r = 0;

  if(achieveX.killed_elderpriest)   r |= 1L << 0;
  if(achieveX.killed_glassgolem)    r |= 1L << 1;
  if(achieveX.killed_tiksrvzllat)   r |= 1L << 2;
  if(achieveX.killed_bofh)          r |= 1L << 3;
  if(achieveX.swimmingpool_cleared) r |= 1L << 4;
  if(achieveX.killed_katia)         r |= 1L << 5;
  if(achieveX.killed_witchking)     r |= 1L << 6;
  if(achieveX.get_magresstone)      r |= 1L << 7;
  if(achieveX.devnull_complete)     r |= 1L << 8;
  if(achieveX.killed_minotaur)      r |= 1L << 9;
  if(achieveX.killed_kalwina)       r |= 1L << 10;

  return r;
}

/* Keep the last xlog "achieve" value to be able to compare */
long last_achieve_int;
long last_achieveX_int;

/* Generic buffer for snprintf */
#define STRBUF_LEN (4096)
char strbuf[STRBUF_LEN];

/* Open the live log file */
boolean livelog_start() {


	last_achieve_int = encodeachieve();
	last_achieveX_int = encodeachieveX();

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
		if(!(livelogfile = fopen_datafile_area(LOGAREA, LIVELOGFILE, "a", SCOREPREFIX))) {
			pline("Cannot open live log file!");
		} else {
			fprintf(livelogfile,"%s", buffer);
			(void) fclose(livelogfile);
		}
		unlock_file_area(LOGAREA, LIVELOGFILE);
	}
}

/* Writes changes in the achieve structure to the live log.
 * Called from various places in the NetHack source,
 * usually where xlog's achieve is set. */
void livelog_achieve_update() {
	long achieve_int, achieve_diff, achieveX_int, achieveX_diff;

	achieve_int = encodeachieve();
	achieve_diff = last_achieve_int ^ achieve_int;

	achieveX_int = encodeachieveX();
	achieveX_diff = last_achieveX_int ^ achieveX_int;

	/* livelog_achieve_update is sometimes called when there's
	 * no actual change. */
	if(achieve_diff == 0 && achieveX_diff == 0) {
		return;
	}

	snprintf(strbuf, STRBUF_LEN,
		"player=%s%s%s%s:role=%s:race=%s:gender=%s:align=%s:hybrid=%s:turns=%ld:achieve=0x%lx:achieve_diff=0x%lx:achieveX=0x%lx:achieveX_diff=0x%lx\n",
		plalias[0] ? plalias : plname,
		plalias[0] ? " (" : "",
		plalias[0] ? plname : "",
		plalias[0] ? ")" : "",
		urole.filecode, urace.filecode, genders[flags.female].filecode, aligns[1-u.ualign.type].filecode,
		hybrid_strcode(),
		moves, 
		achieve_int,
		achieve_diff,
		achieveX_int,
		achieveX_diff);
	livelog_write_string(strbuf);

	last_achieve_int = achieve_int;
	last_achieveX_int = achieveX_int;

}

/* Reports wishes */
void
livelog_wish(item)
char *item;
{
	snprintf(strbuf, STRBUF_LEN,
		"player=%s%s%s%s:role=%s:race=%s:gender=%s:align=%s:hybrid=%s:turns=%ld:wish=%s\n",
		plalias[0] ? plalias : plname,
		plalias[0] ? " (" : "",
		plalias[0] ? plname : "",
		plalias[0] ? ")" : "",
		urole.filecode, urace.filecode, genders[flags.female].filecode, aligns[1-u.ualign.type].filecode,
		hybrid_strcode(),
		moves,
		item);
	livelog_write_string(strbuf);
}

/* Reports achievements */
void
livelog_report_trophy(string)
char *string;
{
	snprintf(strbuf, STRBUF_LEN,
		"player=%s%s%s%s:role=%s:race=%s:gender=%s:align=%s:hybrid=%s:turns=%ld:message=%s\n",
		plalias[0] ? plalias : plname,
		plalias[0] ? " (" : "",
		plalias[0] ? plname : "",
		plalias[0] ? ")" : "",
		urole.filecode, urace.filecode, genders[flags.female].filecode, aligns[1-u.ualign.type].filecode,
		hybrid_strcode(),
		moves,
		string);
	livelog_write_string(strbuf);
}

/* Reports lifesaving */
void
livelog_avert_death()
{
	snprintf(strbuf, STRBUF_LEN,
		"player=%s%s%s%s:role=%s:race=%s:gender=%s:align=%s:hybrid=%s:turns=%ld:message=%s\n",
		plalias[0] ? plalias : plname,
		plalias[0] ? " (" : "",
		plalias[0] ? plname : "",
		plalias[0] ? ")" : "",
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
	
	sprintf(qbuf,"Shout what?");
	getlin(qbuf, buf);
	
	You("shout into the void: %s", buf);

	/* filter livelog delimiter */
	for (p = buf; *p != 0; p++)
		if( *p == ':' )
			*p = ' ';

	snprintf(strbuf, STRBUF_LEN,
		"player=%s%s%s%s:role=%s:race=%s:gender=%s:align=%s:hybrid=%s:turns=%ld:shout=%s\n",
		plalias[0] ? plalias : plname,
		plalias[0] ? " (" : "",
		plalias[0] ? plname : "",
		plalias[0] ? ")" : "",
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
				"player=%s%s%s%s:role=%s:race=%s:gender=%s:align=%s:hybrid=%s:turns=%ld:dlev=%d:" 
				"bones_killed=%s:bones_rank=%s:bones_monst=%s\n", 
				plalias[0] ? plalias : plname,
				plalias[0] ? " (" : "",
				plalias[0] ? plname : "",
				plalias[0] ? ")" : "",
				urole.filecode, urace.filecode, genders[flags.female].filecode, aligns[1-u.ualign.type].filecode,
				hybrid_strcode(),
				moves, 
				depth(&u.uz), 
				name, 
				mtmp->former_rank, 
				mtmp->data->mname); 
		livelog_write_string(strbuf); 
	} else if ((mtmp->data->geno & G_UNIQ) 
	           || (mtmp->data == &mons[PM_BLACK_MARKETEER]) 
		  ) { 
		char *n = noit_mon_nam(mtmp); 
		/* $player killed a uniq monster */ 
		snprintf(strbuf, STRBUF_LEN, 
				"player=%s%s%s%s:role=%s:race=%s:gender=%s:align=%s:hybrid=%s:turns=%ld:killed_uniq=%s\n", 
				plalias[0] ? plalias : plname,
				plalias[0] ? " (" : "",
				plalias[0] ? plname : "",
				plalias[0] ? ")" : "",
				urole.filecode, urace.filecode, genders[flags.female].filecode, aligns[1-u.ualign.type].filecode,
				hybrid_strcode(),
				moves, 
				n); 
		livelog_write_string(strbuf); 
	} 
} 
#endif /* LIVELOG_BONES_KILLER */ 

#endif /* LIVELOGFILE */
