/* the nethack cyborg! */
/* by proudft@ccnet.com */

#include "hack.h"
#include "func_tab.h"

#ifdef BORG
#define HP_TRACK 25
#define HP_TREND 3

char borg_on=0;
char borg_line[80] = {0};
int predicted_hp;

void adjust_hp_list(void)
{
   static int hp_list[HP_TRACK] = {0};
   int l;
   int hp_diff;
   for (l=0;l<HP_TRACK-1;l++) {
      hp_list[l+1] = hp_list[l];
   }
   hp_list[0] = u.uhp;
   hp_diff = 0;
/*   for (l=0;l<HP_TREND;l++) {
      hp_diff += (hp_list[l] - u.uhpmax);
   }
   hp_diff /= HP_TREND;
   predicted_hp = u.uhp - hp_diff;*/
   predicted_hp = u.uhp - ((hp_list[0] - u.uhpmax)*2);
}

char borg_input(void)
{
   char c;
   adjust_hp_list();
   if (predicted_hp <= 0 || (u.uhp < u.uhpmax / 8)) pline("Emergency!");

   /* He's quite lame for now... */
   c = rand()%9 + '1';
   if (c == '5') c = '.';
   sprintf(borg_line,"%c",c);
   return ('.');
}
#endif
