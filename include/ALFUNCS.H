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

