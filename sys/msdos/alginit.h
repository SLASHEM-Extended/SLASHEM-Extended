/* WAC - added some error checking code so that it exits gracefully rather than 
 *       crashing if some of the load_bitmap() calls fail
 *     - since TTY works again, will switch to TTY if we haven't set the 
 *	 graphics mode yet and there's an error
 *
 *     - Added support for one big tile file for all the monsters/objects/other tiles
 *	 I can't seem to get the sub-bitmaps coexisting in the mix so far, so right
 *	 now it needs a lot of memory when loading - it loads the big tile bitmap
 *	 then has to make a lot of smaller ones. :( (pretty much using 2x the amount
 *	 of space that it really should)
 *     - tiles_loaded also determines whether all gfx bitmaps are drawn, etc as well
 *	 well as tiles being loaded.
 *     - use new_bitmap instead of create_bitmap - check for running out of memory
 *     - 8-bit color mode support is barely functional
 *		-indexed tile loading not done
 *		-blending, color conversion problems
 *		-fade out doesn't work
 *     - don't do antialiasing, 2xSaI on the tiles here
 */

#include "2xsai.h"

static BITMAP * FDECL(load_tile_bitmap,(char *, RGB *));
static BITMAP * FDECL(new_bitmap,(int, int));
static BITMAP * FDECL(load_convert_bitmap,(char *, RGB *));
static void NDECL(display_nh_title);
static void FDECL(resize_tile_bitmap, (BITMAP *, BITMAP *, int, int, int, int));

static boolean font_loaded = FALSE;
static PALETTE loadpal;		/* For loading - using NULL pointers causes
				 * problems in pure DOS, it seems :P
 				 */

static RGB background_black = { 0,  0,  0  };

void alleg_Init(void)
{
     char filename[60];
     int i;

     FILE *fp;
     char            buf[BUFSZ];
     char            *bufp;
     uchar   translate[MAXPCHARS];
     int   len;

     windowprocs.win_cliparound = alleg_cliparound;

     if(tiles_loaded) {
     	/* 
     	 * Seems we need this, since we can't detect our current video state
     	 * Otherwise,  things will crash if we shell out (to text-mode)
     	 * then return without fixing the videomode
     	 */
	alleg_SwitchMode(ALLEG_MODEGFX);
        return;
     }

     alleg_load_cnf();
     
     set_color_depth(alleg_colordepth);

     if(alleg_SwitchMode(ALLEG_MODEGFX) < 0) {
        	/* This shouldn't occur! But it might... */

		raw_printf("Reverting to TTY mode, could not initialize %dx%dx%d video mode.",
                        X_RES, Y_RES, alleg_colordepth);
		wait_synch();
		iflags.usevga = 0;
		iflags.tile_view = FALSE;
		iflags.over_view = FALSE;
		CO = 80;
		LI = 25;
	/*	clear_screen()	/* not vga_clear_screen() */
		return;
     }
     
     iflags.grmode = 1; /* In graphics mode now */

     CO = X_RES/FONTX;
     LI = Y_RES/FONTY;

     X_WIDTH = X_RES;
     Y_HEIGHT = (Y_RES - 4*FONTY);

#if 0
     if(tiles_loaded)    
#endif
     { 
        int x, y;

        clear_to_color(screen, makecol(0,0,0));

	/* Load the tiles.  Try the big amalgamation before index */
	/* This should also set up tile_x and tile_y */
	
	if (strlen(bigtile_file) > 0) {
		/* Try to load user-specified tile file */
		sprintf(filename, bigtile_file);
        	alltiles = load_bitmap(filename, tilepal);
	}
	if (!alltiles) {
	    /* Try default tile files */
	    for (i = 0; (i < SIZE(default_tilesets) && !alltiles); i++) {
		sprintf(filename, default_tilesets[i]);
        	alltiles = load_bitmap(filename, tilepal);       	
	    }
	}
        if (alltiles) {
        	int tilefile_x = alltiles->w / TILES_PER_ROW;
        	int tilefile_y = alltiles->h / TILES_PER_COL;

       		if (alleg_colordepth == 8) {
       			/* Set palette for 8-bit mode */
       			set_palette(tilepal);
       			
       			/* setup transparent drawing */
       			create_trans_table(&half_solidity, tilepal,
       						128, 128, 128, NULL);      			
			color_map = &half_solidity;
       		}

        	if (!tile_x) {
        	    tile_x = tilefile_x;
        	}
        	if  (!tile_y) {
        	    tile_y = tilefile_y;
        	}
        	       	       	
        	/* Autodetect 3D tiles if needed
        	 * Look for tile_x : tile_y ratio of 
        	 * 3 : 4
        	 */
        	 
        	if (draw_3Dtiles == -1) {
        	    if (((tile_x * 4) / 3) == tile_y)
        		draw_3Dtiles = 1;
        	    else draw_3Dtiles = 0;
        	}

	        sprintf(filename, "%serror%s", TILEDIR, EXT);
	        errorbmp = load_tile_bitmap (filename, loadpal);
	        if (!errorbmp) {
			error("could not open %s!", filename);
	        }
	        
		display_nh_title();
      
        	for(i = 0; i < TOTAL_TILES_USED; i++) {
		    int col = (int)(i % TILES_PER_ROW);
		    int row = (int)(i / TILES_PER_ROW);
		    
		    /* using create_sub_bitmap 
		     * doesn't seem to agree well with some of the drawing
		     * functions in vidalleg.c so we have to blit a copy
		     *
		     * However, we now can do stretching here as well...
		     */
		    tilecache[i] = new_bitmap(tile_x, tile_y);
		    resize_tile_bitmap(alltiles, tilecache[i],col*tilefile_x, row*tilefile_y,
		    				      tilefile_x, tilefile_y);
#if 0
		    nh_stretch_blit(alltiles, tilecache[i], col*tilefile_x, row*tilefile_y,
		    				      tilefile_x, tilefile_y,
		    				      0,0, tile_x, tile_y);
#endif
		    inc_progress_meter();
        	}
        	destroy_bitmap (alltiles);
        } else {
	        sprintf(filename, "%sindex", TILEDIR);
	        if ((fp = fopenp(filename, "r")) == (FILE *)0)
	        {
	                error("could not open 32tile.bmp or tiles/index!");
	                return;
	        }

	        i = 0;

	        while(fgets(buf,120,fp) && i < TOTAL_TILES_USED)
	        {
	                if (*buf == '#')
	                        continue;

	                bufp = eos(buf);
	                while (--bufp > buf && isspace(*bufp))
	                        continue;

	                if (bufp <= buf)
	                        continue;               /* skip all-blank lines */
	                else
	                        *(bufp + 1) = '\0';     /* terminate line */

	                /* find the '=' or ':' */
	                bufp = index(buf, ':');
	                if (!bufp) {
	                        error("invalid entry in index!");
	                }

	                bufp++; /* we only want what's after the ':' */

	                /* skip  whitespace between '=' and value */
	                do { ++bufp; } while (isspace(*bufp));

	                bufp = mungspaces(bufp);

	                sprintf(filename, "%s%s", TILEDIR, bufp);

	                tilecache[i] = load_tile_bitmap(filename, loadpal);

	                if(!tilecache[i]) 
	                {
	                        tilecache[i] = errorbmp;	                        
	                } 
	                
	                if (i == 0) {
		                if (!tile_x || !tile_y) {
		                    /* Fill in unknown tile size info */
	                    
		                    if (tilecache[i]) {
		                    	/* First tile is not an error */
		                    	if (!tile_x) tile_x = tilecache[i]->w;
		                	if (!tile_y) tile_y = tilecache[i]->h;

		                    } else {
		                    	/* Default to 32x32 */
		                    	if (!tile_x) tile_x = 32;
		                    	if (!tile_y) tile_y = 32;
		                    }
		                    
        	       	       	
		        	    /* Autodetect 3D tiles if needed
		        	     * Look for tile_x : tile_y ratio of 
		        	     * 3 : 4
		        	     */
        	 
		        	    if (draw_3Dtiles == -1) {
					if (((tile_x * 4) / 3) == tile_y)
					    draw_3Dtiles = 1;
					else draw_3Dtiles = 0;
		        	    }
		                    
		                }
		                
			        display_nh_title();
	                    
		        	sprintf(filename, "%serror%s", TILEDIR, EXT);
			        errorbmp = load_tile_bitmap (filename, loadpal);
			        if (!errorbmp) {
					error("could not open %s!", filename);
			        }      	                	
	                }

	                inc_progress_meter();
	                i++;
	        }
	        
	        while (i < TOTAL_TILES_USED) {
                        tilecache[i] = errorbmp;	                        
	                inc_progress_meter();
	                i++;
	        }

	        fclose(fp);
        }
     
        for(i=0; i < NUM_SUBSTITUTES; i++)
        {
                sprintf(filename, "%s%s%s%s", TILEDIR, "subs\\", 
                                              substnames[i], EXT);
                substitutes[i] = load_tile_bitmap(filename, loadpal);

#if 0 /* We want to leave this null so that we don't sub in */
                if(!substitutes[i])
                        substitutes[i] = errorbmp; 
#endif

                inc_progress_meter();
        }

        for(i = 0; i < (min(tile_x/2, tile_y/2) - 1); i++)
        {
                shields[i] = new_bitmap(tile_x,tile_y);
                clear_to_color(shields[i], makecol(255,0,255));
                circlefill(shields[i], tile_x/2, tile_y/2, min(tile_x/2, tile_y/2), makecol(255,255,255));
                circlefill(shields[i], tile_x/2, tile_y/2, i, makecol(255,0,255));
                inc_progress_meter();
        }

        for(i = 0; i < SIZE(enames); i++)
        {
                sprintf(filename, "%s%s%s%s", TILEDIR, "spfx\\", 
                                              enames[i], EXT);
                explosions[i] = load_convert_bitmap(filename, loadpal);
        	if (!explosions[i]) {
			error("could not open %s!", filename);
	        }
                inc_progress_meter();
        }

        for(i = 0; i < 8; i++)
        {
                BITMAP *animation;
                int j;

                sprintf(filename, "%s%s%s%s", TILEDIR, "spfx\\", 
                                              anames[i], EXT);
        
                animation = load_convert_bitmap(filename, loadpal);
	        if (!animation) {
			error("could not open %s!", filename);
	        }

                for(j = 0; j < 28; j++)
                {
                        auras[i][j] = new_bitmap(tile_x*2,tile_y*2);
                        nh_stretch_blit(animation, auras[i][j], 
                        		0, j*64, 64, 64,
                        		0, 0, tile_x*2, tile_y*2);
                        inc_progress_meter();
                }

                destroy_bitmap(animation);
        }

        sprintf(filename, "%s%s%s", TILEDIR, "32on", EXT);
        hudwidgets[ON32] = load_convert_bitmap(filename, loadpal);
        if (!hudwidgets[ON32]) {
		error("could not open %s!", filename);
        }

        sprintf(filename, "%s%s%s", TILEDIR, "32off", EXT);
        hudwidgets[OFF32] = load_convert_bitmap(filename, loadpal);
        if (!hudwidgets[OFF32]) {
		error("could not open %s!", filename);
        }

        sprintf(filename, "%s%s%s", TILEDIR, "16on", EXT);
        hudwidgets[ON16] = load_convert_bitmap(filename, loadpal);
        if (!hudwidgets[ON16]) {
		error("could not open %s!", filename);
        }

        sprintf(filename, "%s%s%s", TILEDIR, "16off", EXT);
        hudwidgets[OFF16] = load_convert_bitmap(filename, loadpal);
        if (!hudwidgets[OFF16]) {
		error("could not open %s!", filename);
        }

        {
                BITMAP *wbmp;
                sprintf(filename, "%s%s%s", TILEDIR, "map", EXT);
                wbmp = load_convert_bitmap(filename, loadpal);
	        if (!wbmp) {
			error("could not open %s!", filename);
	        }

                mapwidgets[0] = new_bitmap(5,5);
                blit(wbmp, mapwidgets[0], 0, 0, 0, 0, 5, 5);

                mapwidgets[1] = new_bitmap(5,5);
                blit(wbmp, mapwidgets[1], 5, 0, 0, 0, 5, 5);

                mapwidgets[2] = new_bitmap(5,5);
                blit(wbmp, mapwidgets[2], 5, 5, 0, 0, 5, 5);
        }

        tiles_loaded = 1;
        
        TILEX = tile_x;
        TILEY = tile_y;

	/*  These need tile_x and tile_y to be set up properly first */
	
	if (draw_3Dtiles) 
	    sprintf(filename, "%scursor3d%s", TILEDIR, EXT);
	else sprintf(filename, "%scursor%s", TILEDIR, EXT);

	cursorbmp = load_tile_bitmap (filename, loadpal); 
	if (!cursorbmp) {
		error("could not open %s!", filename);
	}
	
	ovcursor = new_bitmap(X_RES/80, Y_HEIGHT/23);
       	nh_stretch_blit (cursorbmp, ovcursor, 0, 0, tile_x, tile_y,
                                           0, 0, X_RES/80, Y_HEIGHT/23);

	txt_subscreen = new_bitmap(80*FONTX, 23*FONTY);
	clear_to_color(txt_subscreen, makecol(0,0,0));

	tile_subscreen = new_bitmap(80*tile_x, 23*tile_y);
	clear_to_color(tile_subscreen, makecol(0,0,0));

	subscreen = tile_subscreen;

	minimap = new_bitmap(80, 24);
	clear_to_color(minimap, makecol(0,0,0));

	under_cursor = new_bitmap(tile_x, tile_y);
     } 
#if 0
     else {
      	/* Set palette for 8-bit mode */
       	if (alleg_colordepth == 8) {
       		set_palette(tilepal);
       	}
     }
#endif


     windowprocs.win_cliparound = alleg_cliparound;
     iflags.tile_view = TRUE;
     iflags.over_view = FALSE;
     g_attribute = attrib_gr_normal;
     alleg_clear_screen(makecol(0,0,0));
}

/*
 * Investigated problems with lower resolutions.  It seems that since this is based 
 * off tty code,  so it thinks that there isn't room,  even though the clipping
 * is actually done in this code.  
 */
#define MIN_XRES (COLNO*FONTX)
#define MIN_YRES ((ROWNO+3)*FONTX)
#define MIN_COLORDEPTH 8

void
alleg_load_cnf()
{
        FILE *fp;
        char            buf[BUFSZ];
        char            *bufp;
 
       if ((fp = fopenp("alleg.cnf", "r")) != (FILE *)0) {
        while(fgets(buf,80,fp))
        {
                if (*buf == '#')
                        continue;

                /* remove trailing whitespace */
                bufp = eos(buf);
                while (--bufp > buf && isspace(*bufp))
                        continue;

                if (bufp <= buf)
                        continue;               /* skip all-blank lines */
                else
                        *(bufp + 1) = '\0';     /* terminate line */

                /* find the '=' or ':' */
                bufp = index(buf, '=');
                if (!bufp)
                        continue;

                /* skip  whitespace between '=' and value */
                do { ++bufp; } while (isspace(*bufp));

                if(!strncmpi(buf, "videomode", 9)) {
                        if(!strncmpi(bufp, "vesa1", 5))
                                video_mode = GFX_VESA1;
                        else if(!strncmpi(bufp, "vesa2l", 6))
                                video_mode = GFX_VESA2L;
                        else if(!strncmpi(bufp, "vesa2b", 6))
                                video_mode = GFX_VESA2B;
                        else if(!strncmpi(bufp, "vesa3", 5))
                                video_mode = GFX_VESA3;
                        else if(!strncmpi(bufp, "vbeaf", 5))
                                video_mode = GFX_VBEAF;
                } else if(!strncmpi(buf, "hud", 3)) {
                        if(!strncmpi(bufp, "equip", 5))
                                hud_setting = 1;
                        else if(!strncmpi(bufp, "map", 3))
                                hud_setting = 2;
                        else if(!strncmpi(bufp, "combo", 5))
                                hud_setting = 3;
                } else if(!strncmpi(buf, "barstyle", 8)) {
                        if(!strncmpi(bufp, "fancy", 5))
                                fancy_meters = 1;
                        else 
                                fancy_meters = 0;
                } else if(!strncmpi(buf, "width", 5)) {
                        X_RES = string_to_int(bufp);   
                } else if(!strncmpi(buf, "height", 6)) {
                        Y_RES = string_to_int(bufp);
                } else if(!strncmpi(buf, "colordepth", 10)) {
                        alleg_colordepth = string_to_int(bufp);
                } else if(!strncmpi(buf, "text", 4)) {
                        attrib_allegro_normal = string_to_int(bufp);
                } else if(!strncmpi(buf, "hilite", 6)) {
                        attrib_allegro_intense = string_to_int(bufp);
                } else if(!strncmpi(buf, "status", 6)) {
                        attrib_allegro_status = string_to_int(bufp);
                } else if(!strncmpi(buf, "scroll_lim", 10)) {
                        scroll_lim = string_to_int(bufp);
                } else if(!strncmpi(buf, "smoothing", 9)) {
                        smoothing = string_to_int(bufp);
                } else if(!strncmpi(buf, "fxdelay", 7)) {
                        fx_delay = string_to_int(bufp);

                } else if(!strncmpi(buf, "tile_width", 10)) {
                        tile_x = string_to_int(bufp);   
                } else if(!strncmpi(buf, "tile_height", 11)) {
                        tile_y = string_to_int(bufp);   
                } else if(!strncmpi(buf, "bigtile_file", 12)) {
                        Strcpy(bigtile_file, bufp);
                } else if(!strncmpi(buf, "interpolate", 11)) {
                        use_2xsai = string_to_int(bufp);   
                } else if(!strncmpi(buf, "draw_3Dtiles", 12)) {
                        if(!strncmpi(bufp, "Y", 1))
                        	draw_3Dtiles = 1;
                        else if(!strncmpi(bufp, "1", 1))
                        	draw_3Dtiles = 1;
                        else if(!strncmpi(bufp, "N", 1))
                        	draw_3Dtiles = 0;
                        else if(!strncmpi(bufp, "0", 1))
                        	draw_3Dtiles = 0;
                        else draw_3Dtiles = -1;
                }

		if (use_2xsai && alleg_colordepth != 16) {
			use_2xsai = !use_2xsai;
		}
			

                if(X_RES < MIN_XRES)
                        error("Width specified too small.");
                if(Y_RES < MIN_YRES)
                        error("Height specified too small.");
                if(alleg_colordepth < MIN_COLORDEPTH)
                        error("minimum color depth is 15.");
                if(smoothing > 2)
                        smoothing = 2;

                attrib_gr_normal    = attrib_allegro_normal;
                attrib_gr_intense   = attrib_allegro_intense;
        }
        fclose(fp);
        }
}

/* This loads a bitmap and stretches it to tile_x by tile_y if needed
 * Assumes only 1 (one) tile in the file
 */
static BITMAP *
load_tile_bitmap(filename, colors) 
char * filename;
RGB * colors;
{
    BITMAP * retbmp;
   
    retbmp = load_convert_bitmap(filename, colors);
    
    /* tile_x or tile_y not initialized yet! */
    /* No bitmap loaded! */
    if (!tile_x || !tile_y || !retbmp) return retbmp;
    
    /* Check size */
    if (retbmp->w != tile_x || retbmp->h != tile_y) {
	/* Stretch to fit*/
        BITMAP *tmp = retbmp;

        retbmp = new_bitmap(tile_x, tile_y);
        resize_tile_bitmap(tmp, retbmp, 0, 0, tmp->w, tmp->h);
#if 0
        nh_stretch_blit(tmp, retbmp, 0, 0, tmp->w, tmp->h, 0, 0, tile_x, tile_y);
#endif
	destroy_bitmap(tmp);  
    }
    return (retbmp);
}
     
/* This creates a new bitmap and checks whether it succeeded */
static BITMAP *
new_bitmap(x, y)
int x, y;
{
	BITMAP * new_bmp;
	
	new_bmp = create_bitmap(x, y);
	if (!new_bmp) {
		error("Could not generate bitmap (%i, %i) - possibly out of memory?", x, y);
	} else return new_bmp;
}

/* This loads the files required for the title and displays them 
 * It also initializes the progress bar (for tile loading)
 */
static void
display_nh_title()
{
	char filename[60];
	FILE *fp;
	char buf[BUFSZ];
	BITMAP *titlebmp;
	BITMAP *palbmp;
	int x,y, i;
	int bottom_of_title;
	FONT *loadfont;

	/* WAC load font file if present (do this only once!) */
#if 0
	if (!font_loaded) {
		font_loaded = TRUE;
		loadfont = load_font("anethack.fnt");
		if (loadfont) font = loadfont;
		else {
		    error("Could not load anethack.fnt.");
		}
	}
#endif

	/* Background */
        sprintf(filename, "%stitlebg%s", TILEDIR, EXT);
        titlebmp=load_convert_bitmap(filename, loadpal);
	
        if (titlebmp) {
	        for(x = 0; x < X_RES; x += titlebmp->w)
	                for(y = 0; y < Y_RES; y += titlebmp->h)
	                        blit(titlebmp, screen, 0, 0, x, y, 
	                             titlebmp->w, titlebmp->h);

	        destroy_bitmap(titlebmp);
        } else clear_to_color(screen, colorpal[CLR_WHITE]);


        sprintf(filename, "%stitle%s", TILEDIR, EXT);
        titlebmp=load_convert_bitmap(filename, loadpal);
        
        if (titlebmp) {
	        masked_blit(titlebmp, screen, 0, 0, (X_RES-titlebmp->w)/2, 
	                    (Y_RES-titlebmp->h)/4, titlebmp->w, titlebmp->h);

	        bottom_of_title = (Y_RES-titlebmp->h)/4 + titlebmp->h;

	        destroy_bitmap(titlebmp);
        } else
        	bottom_of_title = Y_RES;
                   
        text_mode(-1); /* transparent text output */
        
        textout_centre(screen, font, mungspaces(COPYRIGHT_BANNER_A), 
                X_RES/2, bottom_of_title + FONTY*2, makecol(0,0,0));
        textout_centre(screen, font, mungspaces(COPYRIGHT_BANNER_B),
                X_RES/2, bottom_of_title + FONTY*3, makecol(0,0,0));
        textout_centre(screen, font, mungspaces(COPYRIGHT_BANNER_C),
                X_RES/2, bottom_of_title + FONTY*4, makecol(0,0,0));
        textout_centre(screen, font, mungspaces(COPYRIGHT_BANNER_D),
                X_RES/2, bottom_of_title + FONTY*5, makecol(0,0,0));

        sprintf(filename, "%scredits", TILEDIR);
        if ((fp = fopenp(filename, "r")) != (FILE *)0) {
                fgets(buf,80,fp);
                textout_centre(screen, font, mungspaces(buf),
                        X_RES/2, bottom_of_title + FONTY*6, makecol(0,0,0));
                fclose(fp);
        }

        text_mode(makecol(0,0,0)); /* on black */

	/* load the Nethack compatible palette */
        sprintf(filename, "%spal%s", TILEDIR, EXT);
        palbmp=load_convert_bitmap(filename, loadpal);
        if (!palbmp) {
               error("could not open %s!", filename);
        }

        for(i=0; i < 16; i++)
        {
                colorpal[i] = getpixel(palbmp, 0, i);
        }
        destroy_bitmap(palbmp);


        init_progress_meter();
}

/* a dodgy hack to convert all files loaded to the current palette (for 8bit)
 * This code loads the the bitmap in truecolor (24bit) mode then converts it
 * to the current palette via blit()
 *
 * It returne load_bitmap() if alleg_colordepth != 8
 */
static BITMAP *
load_convert_bitmap(filename, colors) 
char * filename;
RGB * colors;
{
    BITMAP * retbmp;

    if (alleg_colordepth == 8) {
    	BITMAP * tmp_bmp;
	set_color_depth(24);
	
	tmp_bmp = load_bitmap(filename, colors);
	
	if (tmp_bmp) {
		
	    retbmp = create_bitmap_ex(8, tmp_bmp->w, tmp_bmp->h);
	    blit(tmp_bmp, retbmp, 0, 0, 0, 0, tmp_bmp->w, tmp_bmp->h);
	    
	    destroy_bitmap(tmp_bmp);
	    
	} else retbmp = tmp_bmp;	
	
	set_color_depth(8);
    } else {
	/* Load the bitmap */
	retbmp = load_bitmap(filename, colors);
    }
    
    return retbmp;
}

static void
resize_tile_bitmap (source, dest, source_x, source_y, width, height)
BITMAP *source, *dest;
int source_x, source_y, width, height;
{
	/* Basic sanity checks */
	if (!source || !dest || (dest->w != tile_x) || (dest->h != tile_y))
		return;
	
	if (!draw_3Dtiles || ((width * 4 / 3) == height)) {
	    nh_stretch_blit(source, dest, source_x, source_y, width, height,
					  0, 0, tile_x, tile_y);
	} else {
	    clear_to_color(dest, makecol(255, 0, 255));
	    nh_stretch_blit(source, dest, source_x, source_y, width, height,
					  TILE_3D_OFFSETX/2, TILE_3D_OFFSETY/2, 
					  TILE_3D_WIDTH, TILE_3D_HEIGHT);
	}
}
