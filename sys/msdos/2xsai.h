/*
 * A basic header for the 2xSaI routines
 */

/* 
 * Some basic declarations 
 */

#define false 0
#define true 1
#define bool int
#define STATIC static
#define uint32 unsigned long
#define uint16 unsigned short
#define uint8 unsigned char

extern void Super2xSaI(uint8 *srcPtr, uint32 srcPitch,
	     uint8 *deltaPtr,
	     uint8 *dstPtr, uint32 dstPitch, int width, int height);

extern void _2xSaI(uint8 *srcPtr, uint32 srcPitch,
	     uint8 *deltaPtr,
	     uint8 *dstPtr, uint32 dstPitch, int width, int height);

extern void SuperEagle(uint8 *srcPtr, uint32 srcPitch,
	     uint8 *deltaPtr,
	     uint8 *dstPtr, uint32 dstPitch, int width, int height);
#if 0
extern void Scale_2xSaI(uint8 *srcPtr, uint32 srcPitch,
	     uint8 *deltaPtr,
         BITMAP *dstBitmap, int width, int height);
#endif

/* 
 * A couple handy macros
 * Note that these will be broken in a future version if the deltaPtr ends up 
 * ever being changed
 * 
 * Note that the dest bitmap should be twice the width and height of the source 
 */
#define Super2xSaiBlit(src, dest) Super2xSaI((uint8 *) src->dat, src->w*2, \
	     (uint8 *)src->dat, \
	     (uint8 *)dest->dat, dest->w*2, src->w, src->h)

#define SuperEagleBlit(src, dest) SuperEagle((uint8 *) src->dat, src->w*2, \
	     (uint8 *)src->dat, \
	     (uint8 *)dest->dat, dest->w*2, src->w, src->h)

#define _2xSaiBlit(src, dest) _2xSaI((uint8 *) src->dat, src->w*2, \
	     (uint8 *)src->dat, \
	     (uint8 *)dest->dat, dest->w*2, src->w, src->h)

