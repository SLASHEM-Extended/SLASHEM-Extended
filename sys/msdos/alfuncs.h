#define greyscale(b)            grayscale(b)

inline BITMAP*
grayscale(colorimg)
BITMAP *colorimg;
{
        BITMAP *greyimg=create_bitmap(colorimg->w, colorimg->h);
        int x,y;
        long color;
        long gray;
        int r, g, b;

        for(y = 0; y < colorimg->h; y++)
            for(x = 0; x < colorimg->w; x++)
            {
                color = getpixel(colorimg, x, y);
                r = getr(color);
                g = getg(color);
                b = getb(color);
                if(r == 255 && g == 0 && b == 255) 
                        putpixel(greyimg, x, y, color);
                else
                {
                        gray = (r+g+b)/3;
                        gray /= 2;
                        gray += 64;
                        putpixel(greyimg, x, y, makecol(gray, gray, gray));
                }

            }
        
        return(greyimg);
}

void
draw_gray_sprite(dest, sprite, x, y)
BITMAP *dest;
BITMAP *sprite;
int x,y;
{
        BITMAP *gray_sprite;

        if (!dest || !sprite) return;

        gray_sprite = grayscale(sprite);

        draw_sprite(dest, gray_sprite, x, y);
        destroy_bitmap(gray_sprite);
}

inline void
tinted_blit(source, dest, source_x, source_y, dest_x, dest_y,
            width, height, color, alpha)
BITMAP *source;
BITMAP *dest;
int source_x,source_y,dest_x,dest_y,width,height;
long color;
int alpha;
{
        BITMAP *buffer = create_bitmap(width,height);

        blit(source, buffer, source_x, source_y, 0, 0, width, height);

        drawing_mode(DRAW_MODE_TRANS, buffer, 0, 0);
        set_trans_blender(255,255,255,alpha);
        rectfill(buffer, 0, 0, width, height, color);
        drawing_mode(DRAW_MODE_SOLID, buffer, 0, 0);

        blit(buffer, dest, 0, 0, dest_x, dest_y, width, height);
        destroy_bitmap(buffer);
}

BITMAP* mosaic(bitmap,factor)
BITMAP *bitmap;
int factor;
{
        int small_w, small_h;
        int width = bitmap->w;
        int height = bitmap->h;
        BITMAP *smallimg;
        BITMAP *final = create_bitmap(width, height);

        small_w = width/factor;
        small_h = height/factor;

        smallimg = create_bitmap(small_w, small_h);

        stretch_blit(bitmap, smallimg, 0, 0,
                     width, height, 0, 0, small_w, small_h);
                       
        stretch_blit(smallimg, final, 0, 0, small_w, small_h,
                     0, 0, width, height);

        destroy_bitmap(smallimg);

        return(final);
}

inline void
mosaic_blit(source, dest, source_x, source_y, dest_x, dest_y,
            width, height, factor)
BITMAP *source;
BITMAP *dest;
int source_x,source_y,dest_x,dest_y,width,height;
int factor;
{
        BITMAP *buffer = create_bitmap(width,height);
        BITMAP *buffer2;

        blit(source, buffer, source_x, source_y, 0, 0, width, height);

        buffer2 = mosaic(buffer, factor);

        blit(buffer2, dest, 0, 0, dest_x, dest_y, width, height);

        destroy_bitmap(buffer);
        destroy_bitmap(buffer2);

}

BITMAP* blur(bitmap,factor)
BITMAP *bitmap;
int factor;
{
        int small_w, small_h;
        int width = bitmap->w;
        int height = bitmap->h;
        BITMAP *smallimg;
        BITMAP *final = create_bitmap(width, height);

        small_w = width/factor;
        small_h = height/factor;

        smallimg = create_bitmap(small_w, small_h);

        nh_stretch_blit(bitmap, smallimg, 0, 0,
                     width, height, 0, 0, small_w, small_h);
                       
        nh_stretch_blit(smallimg, final, 0, 0, small_w, small_h,
                     0, 0, width, height);

        destroy_bitmap(smallimg);

        return(final);
}

inline void
blur_blit(source, dest, source_x, source_y, dest_x, dest_y,
            width, height, factor)
BITMAP *source;
BITMAP *dest;
int source_x,source_y,dest_x,dest_y,width,height;
int factor;
{
	BITMAP *buffer = create_bitmap(width,height);
	BITMAP *buffer2;

	blit(source, buffer, source_x, source_y, 0, 0, width, height);

	buffer2 = blur(buffer, factor);

	blit(buffer2, dest, 0, 0, dest_x, dest_y, width, height);
	destroy_bitmap(buffer);
	destroy_bitmap(buffer2);
}


