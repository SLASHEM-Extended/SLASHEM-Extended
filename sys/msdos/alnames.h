#define NUM_SUBSTITUTES 21
BITMAP *substitutes[NUM_SUBSTITUTES];

static const char *substnames[] = {
                "dummy",    
                "arc_fem",  
                "bar_fem",  
                "hea_fem",  
                "kni_fem",  
                "mon_fem",  
                "ran_fem",  
                "rog_fem",  
                "sam_fem",  
                "tou_fem",  
                "wiz_fem",  
                "pri_mall",
                "pri_feml",
                "pri_maln",
                "pri_femn",
                "pri_malc",
                "pri_femc",
                "altarl", /* lawful altar */
                "altarn", /* neutral altar */
                "altarc", /* chaotic altar */
                "altarm", /* Moloch's altar */
};

#define SUB_ARCF   1
#define SUB_BARF   2
#define SUB_HEAF   3
#define SUB_KNIF   4
#define SUB_MONF   5
#define SUB_RANF   6
#define SUB_ROGF   7
#define SUB_SAMF   8
#define SUB_TOUF   9
#define SUB_WIZF   10
#define SUB_PLM    11
#define SUB_PLF    12
#define SUB_PNM    13
#define SUB_PNF    14
#define SUB_PCM    15
#define SUB_PCF    16
#define SUB_ALTARL 17
#define SUB_ALTARN 18
#define SUB_ALTARC 19
#define SUB_ALTARM 20

static const char *enames[] = {
        "gasspore",
        "magblast",
        "fireball",
        "snowball",
        "sleepexp",
        "dthfield",
        "balllit",
        "poisball",
        "acidball"
};

static const char *anames[] = {
        "fail",
        "attack",
        "heal",
        "divine",
        "enchant",
        "cleric",
        "escape",
        "matter"
};

