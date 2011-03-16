
/*
 * Modified:  zov : 21-Apr-1998  : Reads a BMP/GIF/XWD image from file
 *

   XW_EXT_IMAGEDATA* Xw_load_image (awindow,aimageinfo,filename):
   XW_EXT_WINDOW  *awindow
   XW_USERDATA    *aimageinfo
   char           *filename     - XWD/BMP/GIF file name

   Get an image from a file named filename.

   Returns the image descriptor address if successful,
	         or  NULL if an error occured.

   Note!
   It is strictly recommended to pass the filename with extension.
   Otherwise an extension env(CSF_DefaultImageFormat) is appended.
   In fact, it doesn't matter what extension has been used, because
   file format will be automatically detected. (zov)
*/

#define PRO16753       /* GG 261198
//              1) Don't free pname because this pointer is a
//              static address in Xw_get_filename() space.
//              2) open() C function return a int number < 0
//              when the open failed.
*/

#include <Xw_Extension.h>
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <fcntl.h>
#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif

#ifdef TRACE
#define TRACE_LOAD_IMAGE
#endif

#ifdef XW_PROTOTYPE
XW_EXT_IMAGEDATA* Xw_load_xwd_image (void*, void*, char*, int, XColor**, int*);
XW_EXT_IMAGEDATA* Xw_load_bmp_image (void*, void*, char*, int, XColor**, int*);
XW_EXT_IMAGEDATA* Xw_load_gif_image (void*, void*, char*, int, XColor**, int*);

void* Xw_load_image (void *awindow,void *aimageinfo,char *filename)
#else
XW_EXT_IMAGEDATA* Xw_load_xwd_image ();
XW_EXT_IMAGEDATA* Xw_load_bmp_image ();
XW_EXT_IMAGEDATA* Xw_load_gif_image ();

void* Xw_load_image (awindow,aimageinfo,filename)
     void *awindow;
     void *aimageinfo;
     char *filename ;
#endif /*XW_PROTOTYPE*/
{
  XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW *)awindow;
  XW_EXT_IMAGEDATA *pimage;
  XW_STATUS status;
  XColor *pcolors = NULL;
  int ncolors = 0, fimage = 0;
  //  char *wname = NULL, *pname, *pchDefExt, achHeader[8];
  char *pname, *pchDefExt, achHeader[8];


  if( !Xw_isdefine_window(pwindow) ) {
    /*ERROR*Bad EXT_WINDOW Address*/
    Xw_set_error(24,"Xw_load_image",pwindow) ;
    return (NULL) ;
  }


  /* Append desired extension to the file name if no ext. has been specified */
  pchDefExt = getenv ("CSF_DefaultImageFormat");
  pname = Xw_get_filename (filename,(char*)( pchDefExt? pchDefExt: "xwd"));

  
#ifdef DEBUG
  fprintf (stderr, "\r\nXw_load_image: Loading '%s' (extended name: %s)...",
	   filename, pname? pname: "NULL");
#endif /*DEBUG*/
  

  /* Open the file
   * and detect the image format using signature
   */
  if (pname) {

    static int _bFirstTime = 1;
    if (_bFirstTime) {

      _bFirstTime = 0;

      if (!pchDefExt)
        fprintf (stderr,
		 "\r\nWarning: variable CSF_DefaultImageFormat is undefined!"
		 " Assuming 'xwd'.\r\n");
      else if (strlen (pchDefExt) != 3 ||
	          0 != strcasecmp (pchDefExt, "xwd")
	       && 0 != strcasecmp (pchDefExt, "bmp")
	       && 0 != strcasecmp (pchDefExt, "gif"))
        fprintf (stderr,
		 "\r\nWarning: value '%s' of CSF_DefaultImageFormat "
		 "is incorrect!\r\n", pchDefExt);
    }

    fimage = open (pname, O_RDONLY);

#ifndef PRO16753
    Xw_free (pname);
#endif
  }

#ifdef PRO16753
  if( fimage < 0 ) {
#else
  if (!fimage) {
#endif

    fprintf (stderr, "\r\nXw_load_image: Error: "
	     "Can't open file '%s'!", filename);
    return (NULL);
  }
  if (read (fimage, achHeader,sizeof (achHeader)) != sizeof (achHeader)) {

    fprintf (stderr, "\r\nXw_load_image: Error: "
	     "Can't read file '%s' to determine format!", filename);
    close(fimage);
    return (NULL);
  }

  lseek (fimage, 0, SEEK_SET);


  /*
   * Detect file format; then load the image
   * and obtain array of colors used
   */
  if (0 == strncmp (achHeader, "GIF87a", 6))
    pimage = Xw_load_gif_image (pwindow, aimageinfo, filename,
				fimage, &pcolors, &ncolors);
  else if (0 == strncmp (achHeader, "GIF89a", 6)) {

    fprintf (stderr, 
	     "\r\nXw_load_image: Warning: GIF89a format specified (file %s).",
	     filename);
    pimage = Xw_load_gif_image (pwindow, aimageinfo, filename,
				fimage, &pcolors, &ncolors);
  }
  else if (0 == strncmp (achHeader, "BM", 2))
    pimage = Xw_load_bmp_image (pwindow, aimageinfo, filename,
				fimage, &pcolors, &ncolors);
  else /* Then it must be XWD */
    pimage = Xw_load_xwd_image (pwindow, aimageinfo, filename,
				fimage, &pcolors, &ncolors);

  close (fimage);


  if (! pimage) {

    fprintf (stderr, "\r\nXw_load_image: Error: Failed to read %s!",
	     filename);
    close(fimage);
    return (NULL);
  }


  /* Color relocation:
   * Converse image to what the window format supports.
   */
  status = Xw_convert_image (pwindow, pimage, pcolors, ncolors);

  if (pcolors)
    Xw_free(pcolors);

  if( status == XW_ERROR ) {
    Xw_del_imagedata_structure (pimage) ;
    return (NULL) ;
  }


#ifdef  TRACE_LOAD_IMAGE
  if( Xw_get_trace() ) {
    printf (" %lx = Xw_load_image(%lx,'%s')\n",(long ) pimage,(long ) pwindow,filename);
  }
#endif
  return (pimage);
}
