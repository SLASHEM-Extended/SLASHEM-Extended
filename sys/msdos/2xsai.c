/* Modified 2xSaI code - thanks to Kreed for supplying original source 
 *
 * Fixed the for loops so that they don't declare variables (C convention)
 * Replaced all // comments (C convention)
 * Hopefully, all the code is certified C code :)
 *
 * Added basic transparent pixel detection (color 255, 0, 255)
 * (#define DETECT_TRANSPARENT_PIXELS)
 *	-transparent pixels are now converted internally to black
 *	-if original pixel was transparent and the changed pixel is still black
 *	 after modification,  the new pixel is also transparent
 */

#include "2xsai.h"

#define DETECT_TRANSPARENT_PIXELS

bool mmx_cpu = false;

static uint32 colorMask = 0xF7DEF7DE;
static uint32 lowPixelMask = 0x08210821;
static uint32 qcolorMask = 0xE79CE79C;
static uint32 qlowpixelMask = 0x18631863;

#ifdef DETECT_TRANSPARENT_PIXELS
#define TRANS_COLOR16 0xF81F
#define TRANS_COLOR15 0x7C1F
static uint16 transpixelcolor = TRANS_COLOR16;
#endif

#include <allegro.h>

int Init_2xSaI(uint32 BitFormat)
{
        if (BitFormat == 565)
        {
                colorMask = 0xF7DEF7DE;
                lowPixelMask = 0x08210821;
                qcolorMask = 0xE79CE79C;
                qlowpixelMask = 0x18631863;
#ifdef DETECT_TRANSPARENT_PIXELS
                transpixelcolor = TRANS_COLOR16;
#endif
        }
        else
        if (BitFormat == 555)
        {
                colorMask = 0x7BDE7BDE;
                lowPixelMask = 0x04210421;
                qcolorMask = 0x739C739C;
                qlowpixelMask = 0x0C630C63;
#ifdef DETECT_TRANSPARENT_PIXELS
                transpixelcolor = TRANS_COLOR15;
#endif
        }
        else
        {
                return 0;
        }
#ifdef MMX
        Init_2xSaIMMX(BitFormat);
#endif
        return 1;
}

STATIC inline int GetResult1(uint32 A, uint32 B, uint32 C, uint32 D, uint32 E)
{
 int x = 0; 
 int y = 0;
 int r = 0;
 if (A == C) x+=1; else if (B == C) y+=1;
 if (A == D) x+=1; else if (B == D) y+=1;
 if (x <= 1) r+=1; 
 if (y <= 1) r-=1;
 return r;
}

STATIC inline int GetResult2(uint32 A, uint32 B, uint32 C, uint32 D, uint32 E) 
{
 int x = 0; 
 int y = 0;
 int r = 0;
 if (A == C) x+=1; else if (B == C) y+=1;
 if (A == D) x+=1; else if (B == D) y+=1;
 if (x <= 1) r-=1; 
 if (y <= 1) r+=1;
 return r;
}


STATIC inline int GetResult(uint32 A, uint32 B, uint32 C, uint32 D)
{
 int x = 0; 
 int y = 0;
 int r = 0;
 if (A == C) x+=1; else if (B == C) y+=1;
 if (A == D) x+=1; else if (B == D) y+=1;
 if (x <= 1) r+=1; 
 if (y <= 1) r-=1;
 return r;
}


STATIC inline uint32 INTERPOLATE(uint32 A, uint32 B)
{
    if (A !=B)
    {
       return ( ((A & colorMask) >> 1) + ((B & colorMask) >> 1) + (A & B & lowPixelMask) );
    }
    else return A;
}

STATIC inline uint32 Q_INTERPOLATE(uint32 A, uint32 B, uint32 C, uint32 D)
{
        register uint32 x = ((A & qcolorMask) >> 2) +
                            ((B & qcolorMask) >> 2) +
                            ((C & qcolorMask) >> 2) +
                            ((D & qcolorMask) >> 2);
        register uint32 y = (A & qlowpixelMask) +
                            (B & qlowpixelMask) +
                            (C & qlowpixelMask) +
                            (D & qlowpixelMask);
        y = (y>>2) & qlowpixelMask;
        return x+y;
}

#define HOR
#define VER
void Super2xSaI(uint8 *srcPtr, uint32 srcPitch,
	     uint8 *deltaPtr,
	     uint8 *dstPtr, uint32 dstPitch, int width, int height)
{
    uint32 *dP;
    uint16 *bP;

#ifdef MMX_BLA  /* no MMX version yet */
    if (cpu_mmx && width != 512)
    {
	for (height; height; height-=1)
	{
	    	bP = (uint16 *) srcPtr;
        	xP = (uint16 *) deltaPtr;
	    	dP = (uint32 *) dstPtr;
        	_2xSaISuperEagleLine  ((uint8 *) bP, (uint8 *) xP, srcPitch, width, (uint8 *) dP, dstPitch);
		dstPtr += dstPitch << 1;
	    	srcPtr += srcPitch;
        	deltaPtr += srcPitch;
        }
    }
    else
    {
#endif
        uint32 Nextline = srcPitch >> 1;

		for (height; height; height-=1)
	{
	    uint32 finish;
		
	    bP = (uint16 *) srcPtr;
	    dP = (uint32 *) dstPtr;
            for (finish = width; finish; finish -= 1 )
            {
           	uint32 color4, color5, color6;
           	uint32 color1, color2, color3;
           	uint32 colorA0, colorA1, colorA2, colorA3,
                  	colorB0, colorB1, colorB2, colorB3,
                  	colorS1, colorS2;
           	uint32 product1a, product1b,
                  	product2a, product2b;

#ifdef DETECT_TRANSPARENT_PIXELS
                bool trans1 = 0, trans2 = 0;
#endif

/*
 *---------------------------------------    B1 B2
 *                                         4  5  6 S2
 *                                         1  2  3 S1
 *                                           A1 A2
 */

            	colorB0 = *(bP- Nextline - 1);
            	colorB1 = *(bP- Nextline);
            	colorB2 = *(bP- Nextline + 1);
            	colorB3 = *(bP- Nextline + 2);

            	color4 = *(bP - 1);
            	color5 = *(bP);
            	color6 = *(bP + 1);
            	colorS2 = *(bP + 2);

            	color1 = *(bP + Nextline - 1);
            	color2 = *(bP + Nextline);
            	color3 = *(bP + Nextline + 1);
            	colorS1 = *(bP + Nextline + 2);

            	colorA0 = *(bP + Nextline + Nextline - 1);
            	colorA1 = *(bP + Nextline + Nextline);
            	colorA2 = *(bP + Nextline + Nextline + 1);
            	colorA3 = *(bP + Nextline + Nextline + 2);

#ifdef DETECT_TRANSPARENT_PIXELS
                if (color5 == transpixelcolor) trans1 = 1;
                if (color2 == transpixelcolor) trans2 = 1;
                
                if (colorB0 == transpixelcolor) colorB0 = 0;
                if (colorB1 == transpixelcolor) colorB1 = 0;
                if (colorB2 == transpixelcolor) colorB2 = 0;
                if (colorB3 == transpixelcolor) colorB3 = 0;

                if (color4 == transpixelcolor) color4 = 0;
                if (color5 == transpixelcolor) color5 = 0;
                if (color6 == transpixelcolor) color6 = 0;
                if (colorS2 == transpixelcolor) colorS2 = 0;

                if (color1 == transpixelcolor) color1 = 0;
                if (color2 == transpixelcolor) color2 = 0;
                if (color3 == transpixelcolor) color3 = 0;
                if (colorS1 == transpixelcolor) colorS1 = 0;

                if (colorA0 == transpixelcolor) colorA0 = 0;
                if (colorA1 == transpixelcolor) colorA1 = 0;
                if (colorA2 == transpixelcolor) colorA2 = 0;
                if (colorA3 == transpixelcolor) colorA3 = 0;
                
#endif

/*--------------------------------------*/
                if (color2 == color6 && color5 != color3)
                {
                   product2b = product1b = color2;
                }
                else
                if (color5 == color3 && color2 != color6)
                {
                   product2b = product1b = color5;
                }
                else
                if (color5 == color3 && color2 == color6 && color5 != color6)
                {
                   register int r = 0;

                   r += GetResult (color6, color5, color1, colorA1);
                   r += GetResult (color6, color5, color4, colorB1);
                   r += GetResult (color6, color5, colorA2, colorS1);
                   r += GetResult (color6, color5, colorB2, colorS2);

                   if (r > 0)
                      product2b = product1b = color6;
                   else
                   if (r < 0)
                      product2b = product1b = color5;
                   else
                   {
                      product2b = product1b = INTERPOLATE (color5, color6);
                   }

                }
                else
                {

#ifdef VER
                   if (color6 == color3 && color3 == colorA1 && color2 != colorA2 && color3 != colorA0)
                      product2b = Q_INTERPOLATE (color3, color3, color3, color2);
                   else
                   if (color5 == color2 && color2 == colorA2 && colorA1 != color3 && color2 != colorA3)
                      product2b = Q_INTERPOLATE (color2, color2, color2, color3);
                   else
#endif
                      product2b = INTERPOLATE (color2, color3);

#ifdef VER
                   if (color6 == color3 && color6 == colorB1 && color5 != colorB2 && color6 != colorB0)
                      product1b = Q_INTERPOLATE (color6, color6, color6, color5);
                   else
                   if (color5 == color2 && color5 == colorB2 && colorB1 != color6 && color5 != colorB3)
                      product1b = Q_INTERPOLATE (color6, color5, color5, color5);
                   else
#endif
                      product1b = INTERPOLATE (color5, color6);
                }

#ifdef HOR
                if (color5 == color3 && color2 != color6 && color4 == color5 && color5 != colorA2)
                   product2a = INTERPOLATE (color2, color5);
                else
                if (color5 == color1 && color6 == color5 && color4 != color2 && color5 != colorA0)
                   product2a = INTERPOLATE(color2, color5);
                else
#endif
                   product2a = color2;

#ifdef HOR
                if (color2 == color6 && color5 != color3 && color1 == color2 && color2 != colorB2)
                   product1a = INTERPOLATE (color2, color5);
                else
                if (color4 == color2 && color3 == color2 && color1 != color5 && color2 != colorB0)
                   product1a = INTERPOLATE(color2, color5);
                else
#endif
                   product1a = color5;

#ifdef DETECT_TRANSPARENT_PIXELS
                if (trans1 && !product1a) product1a = transpixelcolor;
                if (trans1 && !product1b) product1b = transpixelcolor;
                if (trans2 && !product2a) product2a = transpixelcolor;
                if (trans2 && !product2b) product2b = transpixelcolor;
#endif

            	product1a = product1a | (product1b << 16);
            	product2a = product2a | (product2b << 16);

		*(dP) = product1a;
		*(dP+(dstPitch>>2)) = product2a;

                bP += 1;
                dP += 1;
            } /* end of for ( finish= width etc..) */

	    dstPtr += dstPitch << 1;
            srcPtr += srcPitch;
            deltaPtr += srcPitch;
	}; /* endof: for (height; height; height--) */
#ifdef MMX_BLA
    }
#endif
}





/*ONLY use with 640x480x16 or higher resolutions*/
/*Only use this if 2*width * 2*height fits on the current screen*/
void SuperEagle(uint8 *srcPtr, uint32 srcPitch,
	     uint8 *deltaPtr,
		 uint8 *dstPtr, uint32 dstPitch, int width, int height)
{
    uint32 *dP;
    uint16 *bP;
    uint16 *xP;

#ifdef MMX
    if (mmx_cpu && width != 512)
    {
	for (height; height; height-=1)
	{
		bP = (uint16 *) srcPtr;
		xP = (uint16 *) deltaPtr;
		dP = (uint32 *) dstPtr;
        	_2xSaISuperEagleLine  ((uint8 *) bP, (uint8 *) xP, srcPitch, width, (uint8 *)dP, dstPitch);
		dstPtr += dstPitch << 1;
	    	srcPtr += srcPitch;
        	deltaPtr += srcPitch;
        }
    }
    else
    {
#endif
        uint32 Nextline = srcPitch >> 1;

        for (height; height; height-=1)
	{
	    uint32 finish;
	    
	    bP = (uint16 *) srcPtr;
	    dP = (uint32 *) dstPtr;
            for (finish = width; finish; finish -= 1 )
            {

           	uint32 color4, color5, color6;
           	uint32 color1, color2, color3;
           	uint32 colorA0, colorA1, colorA2, colorA3,
                  colorB0, colorB1, colorB2, colorB3,
                  colorS1, colorS2;
           	uint32 product1a, product1b,
                  product2a, product2b;
#ifdef DETECT_TRANSPARENT_PIXELS
                bool trans1 = 0, trans2 = 0;
#endif

            	colorB0 = *(bP- Nextline - 1);
            	colorB1 = *(bP- Nextline);
            	colorB2 = *(bP- Nextline + 1);
            	colorB3 = *(bP- Nextline + 2);

            	color4 = *(bP - 1);
            	color5 = *(bP);
            	color6 = *(bP + 1);
            	colorS2 = *(bP + 2);

            	color1 = *(bP + Nextline - 1);
            	color2 = *(bP + Nextline);
            	color3 = *(bP + Nextline + 1);
            	colorS1 = *(bP + Nextline + 2);

            	colorA0 = *(bP + Nextline + Nextline - 1);
            	colorA1 = *(bP + Nextline + Nextline);
            	colorA2 = *(bP + Nextline + Nextline + 1);
            	colorA3 = *(bP + Nextline + Nextline + 2);

#ifdef DETECT_TRANSPARENT_PIXELS
                if (color5 == transpixelcolor) trans1 = 1;
                if (color2 == transpixelcolor) trans2 = 1;
                
                if (colorB0 == transpixelcolor) colorB0 = 0;
                if (colorB1 == transpixelcolor) colorB1 = 0;
                if (colorB2 == transpixelcolor) colorB2 = 0;
                if (colorB3 == transpixelcolor) colorB3 = 0;

                if (color4 == transpixelcolor) color4 = 0;
                if (color5 == transpixelcolor) color5 = 0;
                if (color6 == transpixelcolor) color6 = 0;
                if (colorS2 == transpixelcolor) colorS2 = 0;

                if (color1 == transpixelcolor) color1 = 0;
                if (color2 == transpixelcolor) color2 = 0;
                if (color3 == transpixelcolor) color3 = 0;
                if (colorS1 == transpixelcolor) colorS1 = 0;

                if (colorA0 == transpixelcolor) colorA0 = 0;
                if (colorA1 == transpixelcolor) colorA1 = 0;
                if (colorA2 == transpixelcolor) colorA2 = 0;
                if (colorA3 == transpixelcolor) colorA3 = 0;
                
#endif

                /* -------------------------------------- */
                if (color2 == color6 && color5 != color3)
                {
                   product1b = product2a = color2;
                   if ((color1 == color2 && color6 == colorS2) ||
                       (color2 == colorA1 && color6 == colorB2))
                   {
                       product1a = INTERPOLATE (color2, color5);
                       product1a = INTERPOLATE (color2, product1a);
                       product2b = INTERPOLATE (color2, color3);
                       product2b = INTERPOLATE (color2, product2b);
#if 0
                       product1a = color2;
                       product2b = color2;
#endif

                   }
                   else
                   {
                      product1a = INTERPOLATE (color5, color6);
                      product2b = INTERPOLATE (color2, color3);
                   }
                }
                else
                if (color5 == color3 && color2 != color6)
                {
                   product2b = product1a = color5;
                   if ((colorB1 == color5 && color3 == colorA2) ||
                       (color4 == color5 && color3 == colorS1))
                   {
                       product1b = INTERPOLATE (color5, color6);
                       product1b = INTERPOLATE (color5, product1b);
                       product2a = INTERPOLATE (color5, color2);
                       product2a = INTERPOLATE (color5, product2a);
#if 0
                       product1b = color5;
                       product2a = color5;
#endif
                   }
                   else
                   {
                      product1b = INTERPOLATE (color5, color6);
                      product2a = INTERPOLATE (color2, color3);
                   }
                }
                else
                if (color5 == color3 && color2 == color6 && color5 != color6)
                {
                   register int r = 0;

                   r += GetResult (color6, color5, color1, colorA1);
                   r += GetResult (color6, color5, color4, colorB1);
                   r += GetResult (color6, color5, colorA2, colorS1);
                   r += GetResult (color6, color5, colorB2, colorS2);

                   if (r > 0)
                   {
                      product1b = product2a = color2;
                      product1a = product2b = INTERPOLATE (color5, color6);
                   }
                   else
                   if (r < 0)
                   {
                      product2b = product1a = color5;
                      product1b = product2a = INTERPOLATE (color5, color6);
                   }
                   else
                   {
                      product2b = product1a = color5;
                      product1b = product2a = color2;
                   }
                }
                else
                {

                   if ((color2 == color5) || (color3 == color6))
                   {
                      product1a = color5;
                      product2a = color2;
                      product1b = color6;
                      product2b = color3;

                   }
                   else
                   {
                      product1b = product1a = INTERPOLATE (color5, color6);
                      product1a = INTERPOLATE (color5, product1a);
                      product1b = INTERPOLATE (color6, product1b);

                      product2a = product2b = INTERPOLATE (color2, color3);
                      product2a = INTERPOLATE (color2, product2a);
                      product2b = INTERPOLATE (color3, product2b);
                   }
                }

#ifdef DETECT_TRANSPARENT_PIXELS
                if (trans1 && !product1a) product1a = transpixelcolor;
                if (trans1 && !product1b) product1b = transpixelcolor;
                if (trans2 && !product2a) product2a = transpixelcolor;
                if (trans2 && !product2b) product2b = transpixelcolor;
#endif

            	product1a = product1a | (product1b << 16);
            	product2a = product2a | (product2b << 16);

		*(dP) = product1a;
		*(dP+(dstPitch>>2)) = product2a;

                bP += 1;
                dP += 1;
            } /* end of for ( finish= width etc..) */

	    dstPtr += dstPitch << 1;
            srcPtr += srcPitch;
            deltaPtr += srcPitch;
	}; /* endof: for (height; height; height--) */
#ifdef MMX
    }
#endif
}



/*ONLY use with 640x480x16 or higher resolutions*/
/*Only use this if 2*width * 2*height fits on the current screen*/
void _2xSaI(uint8 *srcPtr, uint32 srcPitch,
	     uint8 *deltaPtr,
	     uint8 *dstPtr, uint32 dstPitch, int width, int height)
{
    uint32 *dP;
    uint16 *bP;
    uint16 *xP;

#ifdef MMX
    if (mmx_cpu && width != 512)
    {
	for (height; height; height-=1)
	{

	    bP = (uint16 *) srcPtr;
	    xP = (uint16 *) deltaPtr;
	    dP = (uint32 *) dstPtr;
            _2xSaILine  ((uint8 *) bP, (uint8 *) xP, srcPitch, width, (uint8 *)dP, dstPitch);
	    dstPtr += dstPitch << 1;
	    srcPtr += srcPitch;
            deltaPtr += srcPitch;
        }
    }
    else
    {
#endif
        uint32 Nextline = srcPitch >> 1;

        for (height; height; height-=1)
	{
	    uint32 finish;
	    
	    bP = (uint16 *) srcPtr;
	    dP = (uint32 *) dstPtr;
            for (finish = width; finish; finish -= 1 )
            {


                register uint32 colorA, colorB;
                uint32 colorC, colorD,
                       colorE, colorF, colorG, colorH,
                       colorI, colorJ, colorK, colorL,
                       colorM, colorN, colorO, colorP;
                uint32 product, product1, product2;
#ifdef DETECT_TRANSPARENT_PIXELS
                bool trans1 = 0, trans2 = 0;
#endif


/*---------------------------------------
 * Map of the pixels:                    I|E F|J
 *                                       G|A B|K
 *                                       H|C D|L
 *                                       M|N O|P
 */
                colorI = *(bP- Nextline - 1);
                colorE = *(bP- Nextline);
                colorF = *(bP- Nextline + 1);
                colorJ = *(bP- Nextline + 2);

                colorG = *(bP - 1);
                colorA = *(bP);
                colorB = *(bP + 1);
                colorK = *(bP + 2);

                colorH = *(bP + Nextline - 1);
                colorC = *(bP + Nextline);
                colorD = *(bP + Nextline + 1);
                colorL = *(bP + Nextline + 2);

                colorM = *(bP + Nextline + Nextline - 1);
                colorN = *(bP + Nextline + Nextline);
                colorO = *(bP + Nextline + Nextline + 1);
                colorP = *(bP + Nextline + Nextline + 2);

#ifdef DETECT_TRANSPARENT_PIXELS
                if (colorA == transpixelcolor) trans1 = 1;
                if (colorC == transpixelcolor) trans2 = 1;
                
                if (colorI == transpixelcolor) colorI = 0;
                if (colorE == transpixelcolor) colorE = 0;
                if (colorF == transpixelcolor) colorF = 0;
                if (colorJ == transpixelcolor) colorJ = 0;
                
                if (colorG == transpixelcolor) colorG = 0;
                if (colorA == transpixelcolor) colorA = 0;
                if (colorB == transpixelcolor) colorB = 0;
                if (colorK == transpixelcolor) colorK = 0;
                
                if (colorH == transpixelcolor) colorH = 0;
                if (colorC == transpixelcolor) colorC = 0;
                if (colorD == transpixelcolor) colorD = 0;
                if (colorL == transpixelcolor) colorL = 0;
                
                if (colorM == transpixelcolor) colorM = 0;
                if (colorN == transpixelcolor) colorN = 0;
                if (colorO == transpixelcolor) colorO = 0;
                if (colorP == transpixelcolor) colorP = 0;                
#endif

                        if ((colorA == colorD) && (colorB != colorC))
                        {
                           if ( ((colorA == colorE) && (colorB == colorL)) ||
                                ((colorA == colorC) && (colorA == colorF) && (colorB != colorE) && (colorB == colorJ)) )
                           {
                              product = colorA;
                           }
                           else
                           {
                              product = INTERPOLATE(colorA, colorB);
                           }

                           if (((colorA == colorG) && (colorC == colorO)) ||
                               ((colorA == colorB) && (colorA == colorH) && (colorG != colorC) && (colorC == colorM)) )
                           {
                              product1 = colorA;
                           }
                           else
                           {
                              product1 = INTERPOLATE(colorA, colorC);
                           }
                           product2 = colorA;
                        }
                        else
                        if ((colorB == colorC) && (colorA != colorD))
                        {
                           if (((colorB == colorF) && (colorA == colorH)) ||
                               ((colorB == colorE) && (colorB == colorD) && (colorA != colorF) && (colorA == colorI)) )
                           {
                              product = colorB;
                           }
                           else
                           {
                              product = INTERPOLATE(colorA, colorB);
                           }

                           if (((colorC == colorH) && (colorA == colorF)) ||
                               ((colorC == colorG) && (colorC == colorD) && (colorA != colorH) && (colorA == colorI)) )
                           {
                              product1 = colorC;
                           }
                           else
                           {
                              product1 = INTERPOLATE(colorA, colorC);
                           }
                           product2 = colorB;
                        }
                        else
                        if ((colorA == colorD) && (colorB == colorC))
                        {
                           if (colorA == colorB)
                           {
                              product = colorA;
                              product1 = colorA;
                              product2 = colorA;
                           }
                           else
                           {
                              register int r = 0;
                              product1 = INTERPOLATE(colorA, colorC);
                              product = INTERPOLATE(colorA, colorB);

                              r += GetResult1 (colorA, colorB, colorG, colorE, colorI);
                              r += GetResult2 (colorB, colorA, colorK, colorF, colorJ);
                              r += GetResult2 (colorB, colorA, colorH, colorN, colorM);
                              r += GetResult1 (colorA, colorB, colorL, colorO, colorP);

                              if (r > 0)
                                  product2 = colorA;
                              else
                              if (r < 0)
                                  product2 = colorB;
                              else
                              {
                                  product2 = Q_INTERPOLATE(colorA, colorB, colorC, colorD);
                              }
                           }
                        }
                        else
                        {
                           product2 = Q_INTERPOLATE(colorA, colorB, colorC, colorD);

                           if ((colorA == colorC) && (colorA == colorF) && (colorB != colorE) && (colorB == colorJ))
                           {
                              product = colorA;
                           }
                           else
                           if ((colorB == colorE) && (colorB == colorD) && (colorA != colorF) && (colorA == colorI))
                           {
                              product = colorB;
                           }
                           else
                           {
                              product = INTERPOLATE(colorA, colorB);
                           }

                           if ((colorA == colorB) && (colorA == colorH) && (colorG != colorC) && (colorC == colorM))
                           {
                              product1 = colorA;
                           }
                           else
                           if ((colorC == colorG) && (colorC == colorD) && (colorA != colorH) && (colorA == colorI))
                           {
                              product1 = colorC;
                           }
                           else
                           {
                              product1 = INTERPOLATE(colorA, colorC);
                           }
                        }

#ifdef DETECT_TRANSPARENT_PIXELS
                        if (trans1) colorA = transpixelcolor;
			if (trans1 && !product) product = transpixelcolor;
                        if (trans2 && !product1) product1 = transpixelcolor;
                        if (trans2 && !product2) product2 = transpixelcolor;
#endif

                        product = colorA | (product << 16);
                        product1 = product1 | (product2 << 16);
			*(dP) = product;
			*(dP+(dstPitch>>2)) = product1;

                    bP += 1;
                    dP += 1;
                } /* end of for ( finish= width etc..) */

	    dstPtr += dstPitch << 1;
            srcPtr += srcPitch;
            deltaPtr += srcPitch;
	}; /* endof: for (height; height; height--) */
#ifdef MMX
    }
#endif
}


