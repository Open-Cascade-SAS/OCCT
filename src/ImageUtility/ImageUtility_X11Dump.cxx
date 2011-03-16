// Modified	27/12/98 : FMN ; PERF: OPTIMISATION LOADER (LOPTIM)
//

#include <ImageUtility_X11Dump.ixx>

#include <Standard.hxx>
#include <Image_PseudoColorImage.hxx>
#include <Image_ColorImage.hxx>
#include <Aspect_ColorMap.hxx>
#include <X11/Xutil.h>
#include <Aspect_ColorMapEntry.hxx>
#include <Image_PixelRowOfDIndexedImage.hxx>
#include <Image_PixelRowOfDColorImage.hxx>




ImageUtility_X11Dump::ImageUtility_X11Dump(
	const TCollection_AsciiString& DisplayName ,
	const Handle(Image_Image)& aImage )

{ myImage = aImage ;

  myDisplay = XOpenDisplay( DisplayName.ToCString() ) ;

  XVisualInfo VInfo, *PVinfo ;
  int Nitems ;
  XSetWindowAttributes WinAttr ;

  Visual *theVisual = DefaultVisual( myDisplay, DefaultScreen( myDisplay ) );

  VInfo.visualid = theVisual->visualid ;

  PVinfo = XGetVisualInfo( myDisplay, VisualIDMask, &VInfo, &Nitems ) ;

  if ( aImage->Type() == Image_TOI_PseudoColorImage ) {
	if ( PVinfo->depth != 8 || PVinfo->c_class != PseudoColor ) {
		VInfo.depth   = 8 ;
		VInfo.c_class = PseudoColor ;
		PVinfo = XGetVisualInfo( myDisplay,
					 VisualDepthMask|VisualClassMask,
					 &VInfo, &Nitems ) ;
	}
  }
  else {
	if ( PVinfo->depth != 24 || PVinfo->c_class != TrueColor ) {
		VInfo.depth   = 24 ;
		VInfo.c_class = TrueColor ;
		PVinfo = XGetVisualInfo( myDisplay,
					 VisualDepthMask|VisualClassMask,
					 &VInfo, &Nitems ) ;
	}
  }

  WinAttr.background_pixel = 0 ;
  WinAttr.border_pixel     = 0 ;

  WinAttr.colormap = XCreateColormap( myDisplay,
				     RootWindow( myDisplay, 
					DefaultScreen( myDisplay ) ),
					PVinfo->visual,
					(PVinfo->c_class==TrueColor)?
						AllocNone:AllocAll ) ;


  myWindow = XCreateWindow( myDisplay,
			  RootWindow( myDisplay, DefaultScreen( myDisplay ) ),
			  0, 0,
			  ( unsigned int ) aImage->Width(),
			  ( unsigned int ) aImage->Height(),
			  0,
			  PVinfo->depth,
			  InputOutput,
			  PVinfo->visual,
			  CWBackPixel | CWBorderPixel | CWColormap, &WinAttr ) ;

  myXImage = XCreateImage( myDisplay,
			   PVinfo->visual,
			   PVinfo->depth,
			   ZPixmap,
			   0,
			   NULL,
			   ( unsigned int ) aImage->Width(),
			   ( unsigned int ) aImage->Height(),
			   BitmapPad( myDisplay ),
			   0 ) ;

  myGC = XCreateGC( myDisplay, myWindow, 0, NULL ) ;

  XSetPlaneMask( myDisplay, myGC, AllPlanes ) ;
  XSetFunction( myDisplay, myGC, GXcopy ) ;

  UpdateX11Colormap() ;
  UpdateX11XImage() ;

  XMapWindow( myDisplay, myWindow ) ;

  XFlush( myDisplay ) ;		  

}

ImageUtility_X11Dump::ImageUtility_X11Dump(
	const ImageUtility_X11Dump& anOtherX11Dump ,
	const Handle(Image_Image)& aImage )

{ myImage = aImage ;

  myDisplay = anOtherX11Dump.X11Display() ;

  myWindow = anOtherX11Dump.X11Window() ;

  XWindowAttributes WinAttr ;

  XGetWindowAttributes( myDisplay, myWindow, &WinAttr ) ;

  myXImage = XCreateImage( myDisplay,
			   WinAttr.visual,
			   WinAttr.depth,
			   ZPixmap,
			   0,
			   NULL,
			   ( unsigned int ) aImage->Width(),
			   ( unsigned int ) aImage->Height(),
			   BitmapPad( myDisplay ),
			   0 ) ;

  myGC = anOtherX11Dump.X11GC() ;

  UpdateX11XImage() ;

  XFlush( myDisplay ) ;		  
}

ImageUtility_X11GC 	ImageUtility_X11Dump::X11GC() const 
{ return myGC ; }

ImageUtility_X11Window 	ImageUtility_X11Dump::X11Window() const 
{ return myWindow ; }

ImageUtility_X11Display ImageUtility_X11Dump::X11Display() const 
{ return myDisplay ; }

ImageUtility_X11XImage 	ImageUtility_X11Dump::X11XImage() const 
{ return myXImage ; }

void ImageUtility_X11Dump::UpdateX11Colormap() const

{ 
  if ( myImage->Type() != Image_TOI_PseudoColorImage ) return ;

  Handle(Image_PseudoColorImage) aPImage = 
		Handle(Image_PseudoColorImage)::DownCast( myImage ) ;
  Handle(Aspect_ColorMap) TheColorMap = aPImage->ColorMap() ;
  Aspect_ColorMapEntry aEntry ;
  XColor aColor ;
  Standard_Integer i ;
  XWindowAttributes WinAttr ;

  XGetWindowAttributes( myDisplay, myWindow, &WinAttr ) ;

  for ( i = 0 ; i < TheColorMap->Size() ; i++ ) {
	aEntry = TheColorMap->Entry( i+1 ) ;

	aColor.pixel = aEntry.Index() % WinAttr.visual->map_entries ;
	aColor.red   = (unsigned short)(aEntry.Color().Red()   * 0xffff + 0.5 );
	aColor.green = (unsigned short)(aEntry.Color().Green() * 0xffff + 0.5 );
	aColor.blue  = (unsigned short)(aEntry.Color().Blue()  * 0xffff + 0.5 );
	aColor.flags = DoRed | DoGreen | DoBlue ;

	XStoreColor( myDisplay, WinAttr.colormap, &aColor ) ;
  }
}

static unsigned int get_range( unsigned long col_mask )

{
  while ( !(col_mask & 1) ) col_mask >>= 1 ;

  return( col_mask ) ;
}

static unsigned int get_shift( unsigned long col_mask , unsigned long rgb_bit )

{ long shift ;

  if ( ( col_mask >> rgb_bit ) == 0 ) {
    shift = 0 ;
  }
  else if ( ( col_mask >> ( 2 * rgb_bit ) ) == 0 ) {
    shift = rgb_bit ;
  }
  else {
    shift = 2 * rgb_bit ;
  }

  return( shift ) ;
}


void ImageUtility_X11Dump::UpdateX11XImage()

{ Standard_Integer x,y, width, height, X, Y, bits_per_line;

  width  = myImage->Width() ; 	
  height = myImage->Height() ;

  X = myImage->LowerX() ;
  Y = myImage->LowerY() ;

  if ( 	myXImage->data == NULL || 
	myXImage->width != width || 
	myXImage->height != height ) {
	if ( myXImage->data != NULL ) {
	  Standard_Address ToBeFree = ( Standard_Address ) myXImage->data ;
	  Standard::Free( ToBeFree); 
	  myXImage->data = NULL ;
	}

	myXImage->width  	 = width  ;
	myXImage->height 	 = height ;
	bits_per_line = width * myXImage->bits_per_pixel ;
	if ( bits_per_line % myXImage->bitmap_pad ) {
		bits_per_line += myXImage->bitmap_pad ;
	}

	myXImage->bytes_per_line =  bits_per_line / 8 ;

	myXImage->data = ( char * ) 
	  Standard::Allocate(myXImage->bytes_per_line*myXImage->height);
  }

  if ( myImage->Type() == Image_TOI_PseudoColorImage ) {
  	char *pl, *p ;
  	Handle(Image_PseudoColorImage) aPImage = 
		Handle(Image_PseudoColorImage)::DownCast( myImage ) ;
  	Image_PixelRowOfDIndexedImage aRow( 0, myImage->Width()-1 ) ;

  	for ( y = 0 , pl = myXImage->data ; 
	      y < height ; 
	      y++, pl += myXImage->bytes_per_line ) {
		aPImage->Row( X, Y+y, aRow ) ;
		for ( x = 0 , p = pl ; x < width ; x++, p++ ) {
			*p = ( char ) ( aRow(x).Value() & 0xff ) ;
		}
	}
  }
  else {
  	unsigned int *pl, *p ;
  	Handle(Image_ColorImage) aCImage = 
		Handle(Image_ColorImage)::DownCast( myImage ) ;
  	Image_PixelRowOfDColorImage aRow( 0, myImage->Width()-1 ) ;

	XWindowAttributes WinAttr ;
	Standard_Real r,g,b ;
	unsigned int lr,lg,lb, rgb_range, r_shift, g_shift, b_shift ;

	XGetWindowAttributes( myDisplay, myWindow, &WinAttr ) ;

	rgb_range = get_range( WinAttr.visual->red_mask ) ;
	r_shift	  = get_shift( WinAttr.visual->red_mask, 
			       WinAttr.visual->bits_per_rgb ) ;
	g_shift	  = get_shift( WinAttr.visual->green_mask, 
			       WinAttr.visual->bits_per_rgb ) ;
	b_shift	  = get_shift( WinAttr.visual->blue_mask, 
			       WinAttr.visual->bits_per_rgb ) ;

  	for ( y = 0 , pl = ( unsigned int *) myXImage->data ; 
	      y < height ; 
	      y++, pl += ( myXImage->bytes_per_line/sizeof( unsigned int ) ) ) {
		aCImage->Row( X, Y+y, aRow ) ;
		for ( x = 0 , p = pl ; x < width ; x++, p++ ) {
		  aRow(x).Value().Values( r,g,b , Quantity_TOC_RGB ) ;
   		  lr = ( unsigned int ) ( r * ( rgb_range ) +0.5) ;
		  lg = ( unsigned int ) ( g * ( rgb_range ) +0.5) ;
		  lb = ( unsigned int ) ( b * ( rgb_range ) +0.5) ;
		  *p = 	(lr<<r_shift) | (lg<<g_shift) |	(lb<<b_shift) ;
		}
	}
  }
}


void ImageUtility_X11Dump::DisplayX11XImage() const

{ XPutImage( 	myDisplay, 
		myWindow, 
		myGC,
		myXImage,
		0,0,
		0,0,
		myXImage->width,myXImage->height ) ;

}

