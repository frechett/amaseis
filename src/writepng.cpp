// Write *.png
#include "as1.h"
typedef unsigned char uch;
//
// error handling
static  jmp_buf jmpbuf;

#if 0
static void readpng2_error_handler(png_structp png_ptr,
                                   png_const_charp msg)
{
    //mainprog_info  *mainprog_ptr;
    char buff[100];
    sprintf(buff,"libpng error: %s\n", msg);

    //mainprog_ptr = png_get_error_ptr(png_ptr);
    //if (mainprog_ptr == NULL) {
    //    fprintf(stderr,
    //      "readpng2 severe error:  jmpbuf not recoverable;"
    //    "terminating.\n");
    //    fflush(stderr);
    //    exit(99);
    //}

    longjmp(jmpbuf, 1);
}
#endif
static void handle_error(const char *s, ...)
{
	char buff[80];
	va_list args;
  va_start(args, s);
  vsprintf(buff,s,args);
  va_end(args);
  MessageBox(0,buff,"Write PNG error",MB_ICONEXCLAMATION);
}
// write a png using data from the current Windows screen.
bool writepng(TDib *dib, TPalette *palette, char *filename)
{
	bool ret = true;
	png_structp  png_ptr;
  png_infop  info_ptr;
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
      NULL, NULL, NULL);
  if (!png_ptr)
  	return ret;

  info_ptr = png_create_info_struct(png_ptr);

  if (setjmp(png_jmpbuf(png_ptr))) {
  	png_destroy_write_struct(&png_ptr, NULL);
    handle_error("Error during init_io");
    return true;
  }
  FILE *fp = fopen(filename,"wb");
  if(fp == NULL)
  	return ret;
 	png_init_io(png_ptr,fp);

  // write header
  if (setjmp(png_jmpbuf(png_ptr))) {
  	png_destroy_write_struct(&png_ptr, NULL);
    handle_error("Error during writing header");
    return true;
  }

  png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);
  int width 	= png_ptr->width = dib->Width();
  int height 	= png_ptr->height = dib->Height();
  //int width = 40;
  //int height = 30;
  //int pixel_depth = png_ptr->pixel_depth = dib->BitsPixel();
	int compression_type = png_ptr->compression_type = PNG_COMPRESSION_TYPE_BASE;
  int filter_type = png_ptr->filter_type =  PNG_FILTER_TYPE_BASE;
  int interlace_type = PNG_INTERLACE_NONE;
  int pixel_depth = 8;

  int num_entries = info_ptr->num_palette = palette->GetNumEntries();
  int size = num_entries*sizeof(tagPALETTEENTRY);
  tagPALETTEENTRY *entries = (tagPALETTEENTRY *)malloc(size);
  memset(entries,0,size);
  palette->GetPaletteEntries(0,num_entries,entries);
  // The palette is 4-tuples. We just need 3-tuples.
  // png_color_struct
  tagPALETTEENTRY *e1 = entries;
  png_color_struct *p1 = (png_color_struct *)entries;
  for(int i=0;i<num_entries;i++,e1++,p1++) {
  	// One is RGB, the other GBR
		tagPALETTEENTRY e = *e1;
    png_color_struct p;
		p.blue = e.peRed;
    p.green = e.peGreen;
    p.red = e.peBlue;
    *p1 = p;
  }
  // Go back to beginning
  p1 = (png_color_struct *)entries;
  // We used to stuff the palette directly into png_ptr but learned
  // we have to use the png_set function.
  if (setjmp(png_jmpbuf(png_ptr))) {
  	png_destroy_write_struct(&png_ptr, NULL);
    handle_error("Error setting palette");
    return true;
  }
	png_set_PLTE(png_ptr,info_ptr,p1,num_entries);
  if (setjmp(png_jmpbuf(png_ptr))) {
  	png_destroy_write_struct(&png_ptr, NULL);
    handle_error("Error setting up header");
    return true;
  }
  png_set_IHDR(png_ptr, info_ptr, width, height, pixel_depth,
			PNG_COLOR_TYPE_PALETTE,		// Palette
      interlace_type,
      compression_type, filter_type);

  if (setjmp(png_jmpbuf(png_ptr))) {
  	png_destroy_write_struct(&png_ptr, NULL);
    handle_error("Error writing header");
    return true;
  }
  static valid = info_ptr->valid;
  png_write_info_before_PLTE(png_ptr, info_ptr);
  //png_write_PLTE(png_ptr, info_ptr->palette,(png_uint_32)info_ptr->num_palette);

  png_write_info(png_ptr, info_ptr);

  if (setjmp(png_jmpbuf(png_ptr))) {
  	png_destroy_write_struct(&png_ptr, NULL);
    handle_error("Error writing bytes");
    return true;
  }
  // Work on image data.
  //int rowbytes = info_ptr->rowbytes = width*info_ptr->pixel_depth/8;
  png_byte *dib_image_data = (uch *)dib->GetBits();
  // image has two points in each byte
  size = width*height;
  // This DIB image is a 4-bit
  png_byte *p = dib_image_data;
  png_byte *image_data = (png_byte *)malloc(size);
  png_byte *p2 = image_data;
  size /= 2;
  for(int i = 0;i<size;i++,p++) {
  	png_byte b = *p;
    *p2++ = b>>4;		// high byte
    *p2++ = b&0x0f;	// low byte
  }
  //int rowbytes = width;
  // Data is upside down. Start at bottom.
  //p = image_data;
  p2 -= info_ptr->rowbytes;
  png_bytep * row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
  for(int y=0;y<height;y++,p2 -= info_ptr->rowbytes)
  	row_pointers[y] = (png_byte *) p2;

  png_write_image(png_ptr, row_pointers);
  if (setjmp(png_jmpbuf(png_ptr))) {
  	png_destroy_write_struct(&png_ptr, NULL);
    handle_error("Error in png_write_end");
    return true;
  }
  png_write_end(png_ptr, NULL);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  ret = false;
  free(image_data);
  free(p1);
  free(row_pointers);
  fclose(fp);

  return ret;
}


