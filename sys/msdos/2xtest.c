/* Some code I used to test the 2xSaI routines */

#include "2xsai2.c"

int
main()
{
	BITMAP *tile_in, *tile_out;
	PALETTE tilepal;
	
	set_color_depth(16);
	
	tile_in = load_bitmap("tmp.bmp", tilepal);
	
	tile_out = create_bitmap((tile_in->w)*2, (tile_in->h)*2);
	Super2xSaiBlit(tile_in, tile_out);
	save_bitmap("tmp2.bmp", tile_out, tilepal);
	SuperEagleBlit(tile_in, tile_out);
	save_bitmap("tmp3.bmp", tile_out, tilepal);
	_2xSaiBlit(tile_in, tile_out);
	save_bitmap("tmp4.bmp", tile_out, tilepal);
	
	printf("Color15: %0x\n", makecol15(255, 0, 255));
	printf("Color16: %0x\n", makecol16(255, 0, 255));
}


