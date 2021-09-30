#!/usr/bin/perl
# -*- cperl -*-

use strict;
my $slex      = 1;  # Comment this out for vanilla Slash'EM version.
my $moncount  = 70; # How many monsters to do per page-load.
my $debugwrap = 0;  # Whether to print debugging info for the whitespace-munger.
my $dictfile  = "wordsEn.txt";


print qq[Content-type: text/plain\n\n];

my @symbol = qw(S_ANT S_BLOB S_COCKATRICE S_DOG S_EYE S_FELINE S_GREMLIN S_HUMANOID S_IMP
                S_JELLY S_KOBOLD S_LEPRECHAUN S_MIMIC S_NYMPH S_ORC S_PIERCER S_QUADRUPED
                S_RODENT S_SPIDER S_TRAPPER S_UNICORN S_VORTEX S_WORM S_XAN S_LIGHT
                S_ZOUTHERN S_ANGEL S_BAT S_CENTAUR S_DRAGON S_ELEMENTAL S_FUNGUS S_GNOME
                S_GIANT S_JABBERWOCK S_KOP S_LICH S_MUMMY S_NAGA S_OGRE S_PUDDING
                S_QUANTMECH S_RUSTMONST S_SNAKE S_TROLL S_UMBER S_VAMPIRE S_WRAITH S_XORN
                S_YETI S_ZOMBIE S_HUMAN S_GHOST S_GOLEM S_DEMON S_EEL S_LIZARD S_BAD_FOOD
                S_BAD_COINS);
if ($slex) {
  push @symbol, $_ for qw(S_GRUE S_WALLMONST S_RUBMONST S_TURRET S_FLYFISH);
}

open DICT, "<", $dictfile;
my @word = grep { /^[a-z]+[a-rt-z]$/ } map { chomp; $_ } <DICT>;
close DICT;

for (1 .. $moncount) {

  my @groupsize = qw(G_UNIQ G_VLGROUP G_LGROUP G_SGROUP);
  if ($slex) { push @groupsize, $_ for qw(G_RGROUP G_PLATOON|G_RGROUP);5 }
  my @gflag = (qw(G_GENO G_NOCORPSE),
               ($groupsize[rand @groupsize]),
              );

  my $name = join " ", map { $word[rand @word] } 1 .. (2 + int rand rand rand 7);
  my $sym = $symbol[rand @symbol];
  my $lvl = (int rand 56) + 45;
  my $mov = (60 > rand 100) ? (12 + int rand rand rand 50) : (12 - int rand rand 12);
  my $ac  = (int rand 41) - 30;
  my $mr  = int rand 100;
  if (50 > rand 100) {
    $mr   = (int rand 50) + 51;
  }
  my $aln = (int rand 3) ? 0 : (int rand 41) - 20;
  my $frq = int rand 5;
  if ($frq == 0) {
    $frq = 1;
  }
  my $gen = (join "|", ((grep { 45 > rand 100 } @gflag), grep { $_ > 0 } ($frq))) || "0";
  my @atk = map { "NO_ATTK" } 1 .. 6;

# Explosion attacks make the monster blow itself up and should therefore be uncommon --Amy
  my @at  = map { "AT_$_" } qw(NONE CLAW BITE KICK BUTT TUCH STNG HUGS SPIT ENGL BREA
                               EXPL BOOM GAZE TENT WEAP NONE CLAW BITE KICK BUTT TUCH
                               STNG HUGS SPIT ENGL BREA GAZE TENT WEAP WEAP RATH);
  if ($slex) {
    push @at, "AT_$_" for qw{SCRA LASH TRAM BEAM WEAP MAGC SCRA LASH TRAM BEAM MAGC};
  } else {
    push @at, "AT_$_" for qw{THRO SPEL WAND MEGA TRAP GODS};
  }
  my @ad =  map { "AD_$_" } qw(PHYS MAGM FIRE COLD SLEE DISN ELEC DRST ACID LITE BLND
                               STUN SLOW PLYS DRLI DREN LEGS STON STCK SGLD SITM SEDU
                               TLPT RUST CONF DGST HEAL WRAP WERE DRDX DRCO DRIN DISE
                               DCAY SSEX HALU DETH PEST FAMN SLIM CLRC SPEL RBRE SAMU
                               CURS);
  if ($slex) {
    push @ad, "AD_$_" for qw{CALM ENCH POLY CORR TCKL NGRA GLIB DARK WTHR LUCK NUMB
                             FRZE DISP BURN FEAR NPRO POIS THIR LAVA FAKE LETH CNCL
                             BANI WISD SHRD WET  MALK UVUU ABDC AXUS CHKH HODS CHRN
                             WEEP VAMP WEBS STTP DEPR WRAT LAZY DRCH DFOO NEXU SOUN
                             GRAV INER TIME MANA PLAS SKIL VENO DREA NAST BADE SLUD
                             ICUR VULN FUMB DIMN AMNE ICEB VAPO EDGE VOMT LITT FREN
                             NGEN CHAO INSA TRAP WGHT NTHR CAST RNG  MIDI ALIN SIN
                             AGGR MINA CONT SPC2 DEST TREM IDAM ANTI PAIN TECH MEMO
                             TRAI STAT DAMA THIE RAGN PART RUNS NACU SANI RBAD ILLU
                             BLEE SHAN SCOR TERR FEMI LEVI MCRE FLAM DEBU UNPR NIVE
                             TDRA BLAS DROP};
  }
  for my $attkn (0 .. 1 + int rand 4.999) {
    my $at = $at[rand @at];
    my $ad = $ad[rand @ad];
    my $dice = int rand 10;
    my $num  = (int rand 21) + 1;
    $atk[$attkn] = qq[ATTK($at, $ad, $dice, $num)];
  }
  my $attacks = wrap((join ", ", @atk), 6);
  my $weight  = int rand rand 5000;
  my $nutr    = int rand rand 2000;
  my @ms = map { "MS_$_" } qw(SILENT BARK MEW ROAR GROWL SQEEK SQAWK HISS BUZZ GRUNT NEIGH
                              WAIL GURGLE BURBLE ANIMAL SHRIEK BONES LAUGH MUMBLE IMITATE
                              ORC HUMANOID ARREST SOLDIER DJINNI NURSE SEDUCE VAMPIRE
                              CUSS SPELL WERE BOAST GYPSY SHEEP CHICKEN COW PARROT);
  if ($slex) {
    push @ms, "MS_$_" for qw(SUPERMAN FART_QUIET FART_NORMAL FART_LOUD BOSS SOUND SHOE
                             STENCH DOUGLAS_ADAMS PUPIL TEACHER PRINCIPAL SMITH
                             WHORE CONVERT HCALIEN CASINO GIBBERISH GLYPHS SOCKS PANTS
                             HANDY CAR JAPANESE SOVIET SNORE PHOTO REPAIR DRUGS COMBAT
                             MUTE CORONA TRUMPET PAIN BRAG PRINCESSLEIA SISSY SING ALLA
                             POKEDEX BOT APOC);
  } else {
    push @ms, "MS_$_" for qw(GUARD BRIBE RIDER LEADER NEMESIS GUARDIAN SELL ORACLE PRIEST);
  }
  my @siz = map { "MZ_$_" } qw(TINY SMALL MEDIUM HUMAN LARGE HUGE GIGANTIC);
  my $sound   = $ms[rand @ms];
  my $size    = $siz[rand @siz];
  my @allres  = map { "MR_$_" } qw(FIRE COLD SLEEP DISINT ELEC POISON ACID STONE);
  if ($slex) { push @allres, "MR_$_" for qw(DEATH DRAIN); }
  my @res     = (grep { 40 > rand 100 } @allres);
  my @grant   = (grep {  8 > rand 100 } @res);
  my $size    = wrap((join ", ", ("SIZ($weight, $nutr, 0, $sound, $size)",
                                  ((join "|", @res) || "0"),
                                  ((join "|", @grant) || "0"))), 6);

  my @race    = qw(HUMAN ELF DWARF GNOME ORC HOBBIT GIANT);
  my @flagone = map { "M1_$_" } qw(FLY SWIM AMORPHOUS WALLWALK CLING TUNNEL NEEDPICK
                                   CONCEAL HIDE AMPHIBIOUS BREATHLESS NOTAKE NOEYES NOHANDS
                                   NOLIMBS NOHEAD MINDLESS HUMANOID ANIMAL SLITHY UNSOLID
                                   THICK_HIDE OVIPAROUS REGEN SEE_INVIS TPORT TPORT_CNTRL
                                   ACID POIS CARNIVORE HERBIVORE OMNIVORE METALLIVORE);
  my @flagtwo = map { "M2_$_" } (qw(NOPOLY UNDEAD VAMPIRE LORD PRINCE MINION DEMON PNAME WANDER
                                    STALK NASTY STRONG ROCKTHROW GREEDY JEWELS COLLECT MAGIC));
  if (not $slex) {
    push @flagtwo, "M2_$_" for qw(MERC WERE);
  }
  push @flagtwo, "M2_" . $race[rand @race];
  push @flagtwo, map { "M2_$_" }
    (((5 > rand 100) ? "NEUTER" : (50 > rand 100) ? "MALE" : "FEMALE"),
     ((30 > rand 100) ? "HOSTILE" : "PEACEFUL"));
  my @wants   = map { "M3_WANTS$_" } qw(AMUL BELL BOOK CAND ARTI ALL);
  my @flagthree = map { "M3_$_" } qw(WAITFORU CLOSE INFRAVISION INFRAVISIBLE TRAITOR NOTAME);
  my @flagfour = ();
  if ($slex) {
    push @flagthree, "M3_$_" for qw(AVOIDER LITHIVORE NOPLRPOLY NONMOVING EGOTYPE
                                    TELEPATHIC SPIDER PETRIFIES MIND_FLAYER NO_DECAY MIMIC
                                    PERMAMIMIC SLIME);
# I want to make it so that all entries have M3_FREQ_UNCOMMON2|M3_FREQ_UNCOMMON5|M3_FREQ_UNCOMMON10
# jonadab: I'm not familiar enough with perl unfortunately; can you add that functionality? ;) --Amy
    @flagfour = map { "M4_$_" } qw(BAT REVIVE RAT SHADE REFLECT MULTIHUED ORGANIVORE);
  }
  push @flagthree, $wants[rand @wants];
  my @mf = (((join "|", grep { 15 > rand 100 } @flagone) || "0"),
            ((join "|", grep { 25 > rand 100 } @flagtwo) || "0"),
            ((join "|", "M3_FREQ_UNCOMMON2", "M3_FREQ_UNCOMMON5", "M3_FREQ_UNCOMMON10", grep { ($slex ? 12 : 35) > rand 100 } @flagthree) || "M3_FREQ_UNCOMMON2|M3_FREQ_UNCOMMON5|M3_FREQ_UNCOMMON10"));
  if ($slex) {
    push @mf, ((join "|", grep { 5 > rand 100 } @flagfour) || "0");
  }

  push @mf, ("M5_JONADAB, 0, 0, 0, 0, 0");

  my @clr = ((map { "CLR_$_" } qw(BLACK RED BLUE GREEN BROWN MAGENTA CYAN GRAY ORANGE
                                  BRIGHT_GREEN YELLOW BRIGHT_BLUE BRIGHT_MAGENTA BRIGHT_CYAN
                                  WHITE)));
  if ($slex) {
    @clr = grep { not /CLR_BLUE/ } @clr;
  }
  my $color = $clr[rand @clr];
  my $flags = wrap((join ", ", @mf, $color), 6);

  print qq[
    MON("$name", $sym,
    LVL($lvl, $mov, $ac, $mr, $aln), ($gen),
    A($attacks),
    $size,
    $flags),\n];
# this is needed for slex
}

sub wrap {
  my ($orig, $origprefixlen) = @_;
  my @line = ();
  my $remainder = $orig;
  my $prefixlen = $origprefixlen;
  while (80 < $prefixlen + length $remainder) {
    my $prefix = " " x $prefixlen;
    my ($before, $after) = $remainder =~ /^(.{60,78}[,])\s*(.*)/;
    my $min = 70;
    my $nocomma = 0;
    while (not defined $before) {
      my $re = $nocomma ? "^(.{$min,78}[,|])\s*(.*)" : "^(.{$min,78},)\s*(.*)";
      ($before, $after) = $remainder =~ $re;
      if (not defined $before) {
        if ($min > 10) {
          $min--;
        } elsif (not $nocomma) {
          $nocomma++;
          $min = 70;
        } else {
          ($before, $after) = $remainder =~ /^(.{1,60})\s*(.*)/;
          $before = "$before\\";
        }}}
    push @line, $before;
    $remainder = $prefix . $after;
    $prefixlen = 4 if $prefixlen > 10;
  }
  my $wrapped = "" . join("\n", (@line, $remainder));
  return "[
--------------------------------------------------------------------------------
wrapping: $orig
         1         2         3         4         5         6         7         8
12345678901234567890123456789012345678901234567890123456789012345678901234567890
    " .$wrapped  . "
--------------------------------------------------------------------------------
    ]" if ($debugwrap and (80 < $origprefixlen + length($orig)));
  return $wrapped;
}