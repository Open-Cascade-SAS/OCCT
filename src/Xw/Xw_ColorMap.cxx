#define PRO7349	//GG_11/04/97
//		Modification de la methode PixelOfColor 
//		qui renvoie maintenant un numero de pixel negatif
//		lorsqu'il correspond a une couleur approximee.

#define PRO10676        //GG_151297
//              Permettre de reserver un index de couleur modifiable
//              dans la colormap.

#include <Xw_ColorMap.ixx>
#include <Quantity_Color.hxx>
#include <Standard_ShortReal.hxx>

// Routines C a declarer en extern
//extern "C" {
#include <Xw_Cextern.hxx>
//}

//-Static data definitions

static XW_STATUS status ;

void Xw_ColorMap::PrintError() {
Standard_CString ErrorMessag ;
Standard_Integer ErrorNumber ;
Standard_Integer ErrorGravity ;
 
        status = XW_SUCCESS ;
        ErrorMessag = Xw_get_error(&ErrorNumber,&ErrorGravity) ;
        if( ErrorGravity > 2 ) Aspect_ColorMapDefinitionError::Raise (ErrorMessag) ;
        else Xw_print_error() ;
}

Xw_ColorMap::Xw_ColorMap () {
}

Xw_ColorMap::Xw_ColorMap (const Standard_CString Connexion, const Xw_TypeOfVisual Visual, const Xw_TypeOfMapping Mapping, const Standard_Integer Ncolors, const Standard_Boolean UseDefault) {
Aspect_Handle BaseMap = 0 ;

        MyExtendedDisplay = Xw_open_display((Standard_PCharacter)Connexion) ;

        if( !MyExtendedDisplay ) PrintError() ; 

	if( UseDefault ) {
	    Aspect_Handle *display,root ;
	    Xw_TypeOfVisual vclass ;
	    int depth ;

   	    status = Xw_get_display_info (MyExtendedDisplay,
					&display,&root,&BaseMap,&vclass,&depth);
	    if( Visual != vclass ) BaseMap = 0;
	}

	MyExtendedColorMap = Xw_def_colormap(MyExtendedDisplay,
					Visual,Ncolors,BaseMap,Mapping) ;
	
	if( !Xw_isdefine_colormap(MyExtendedColorMap) ) PrintError() ;

	MyExtendedOverlayColorMap = Xw_def_colormap(MyExtendedDisplay,
			Xw_TOV_OVERLAY,0,0,Xw_TOM_READONLY) ;

	SetHighlightColor(Quantity_NOC_WHITESMOKE) ;

	MyMapping = Mapping ;
}

void Xw_ColorMap::SetEntry (const Standard_Integer Index,const Standard_Real Red, const Standard_Real Green, const Standard_Real Blue) {


    switch ( MyMapping ) {
	
      case Xw_TOM_HARDRAMP: 
      case Xw_TOM_SIMPLERAMP:
      case Xw_TOM_BESTRAMP:
	status = Xw_def_color(MyExtendedColorMap,
			(int)Index,(float)Red,(float)Green,(float)Blue) ;

	if( !status ) PrintError() ;
	break ;

      case Xw_TOM_COLORCUBE:
      case Xw_TOM_READONLY:
	Aspect_BadAccess::Raise ("Try to write in a READ ONLY colormap");
    }

}

void Xw_ColorMap::SetEntry (const Aspect_ColorMapEntry& Entry) {
Standard_Real r,g,b ;

	(Entry.Color()).Values(r,g,b,Quantity_TOC_RGB) ;

	SetEntry(Entry.Index(),r,g,b) ;

}

void Xw_ColorMap::SetEntries (const Handle(Aspect_ColorMap)& Colormap) {
Standard_Integer size = Colormap->Size() ;
Standard_Integer i ;

	for( i=1 ; i<=size ; i++ ) {
	    SetEntry(Colormap->Entry(i)) ;
	}
}

void Xw_ColorMap::SetHighlightColor ( const Quantity_Color& aColor) {
Standard_Real r,g,b ;

	aColor.Values(r,g,b,Quantity_TOC_RGB) ;

        status = Xw_def_highlight_color(MyExtendedColorMap,
					(float)r,(float)g,(float)b) ;
	if( !status ) PrintError() ;

	if( MyExtendedOverlayColorMap ) {
          status = Xw_def_highlight_color(MyExtendedOverlayColorMap,
					(float)r,(float)g,(float)b) ;
	  if( !status ) PrintError() ;
	}
}

void Xw_ColorMap::Destroy() {

	if( MyExtendedColorMap ) {
	    Xw_close_colormap(MyExtendedColorMap) ;
	    MyExtendedColorMap = NULL ;
	}

	if( MyExtendedOverlayColorMap ) {
	    Xw_close_colormap(MyExtendedOverlayColorMap) ;
	    MyExtendedOverlayColorMap = NULL ;
	}
}

Quantity_Color Xw_ColorMap::HighlightColor ( ) const {
float r,g,b ;

        status = Xw_get_highlight_color (MyExtendedColorMap,&r,&g,&b);

	Standard_Real Red = r;
	Standard_Real Green = g;
	Standard_Real Blue = b;

	return Quantity_Color(Red,Green,Blue,Quantity_TOC_RGB) ;
}

Standard_Integer Xw_ColorMap::HighlightPixel ( ) const {
unsigned long pixel ;

	status = Xw_get_highlight_pixel(MyExtendedColorMap,&pixel) ;

	if( !status ) PrintError() ;

	return (Standard_Integer(pixel)) ;
}

Standard_Integer Xw_ColorMap::PixelOfColor ( const Quantity_Color &aColor ) const {
Standard_Real Red,Green,Blue ;
unsigned long pixel ;
Standard_Integer isapproximate;

	aColor.Values(Red,Green,Blue,Quantity_TOC_RGB) ;

#ifdef PRO7349
	status = Xw_get_color_pixel(MyExtendedColorMap,
			(float)Red,(float)Green,(float)Blue,&pixel,&isapproximate) ;
	if( !status ) PrintError() ;

	if( isapproximate ) return (-Standard_Integer(pixel));

	else return (Standard_Integer(pixel)) ;
#else
	status = Xw_get_color_pixel(MyExtendedColorMap,
			(float)Red,(float)Green,(float)Blue,&pixel) ;

	if( !status ) PrintError() ;

	return (Standard_Integer(pixel)) ;
#endif
}

#ifdef PRO10676
Standard_Integer Xw_ColorMap::AllocatesPixelOfColor ( ) const {
	unsigned long pixel;
	if( Xw_alloc_pixel(MyExtendedColorMap,&pixel) ) 
				return Standard_Integer(pixel);
	else {
	  Xw_print_error();
	  return -1;
	}
}

void Xw_ColorMap::FreePixelOfColor ( const Standard_Integer aPixel ) const {
	unsigned long pixel = aPixel;
	Xw_free_pixel(MyExtendedColorMap,pixel);
}

Standard_Boolean Xw_ColorMap::SetColorOfPixel ( const Standard_Integer aPixel,
					        const Quantity_Color &aColor ) const {
Standard_Real Red,Green,Blue ;
	aColor.Values(Red,Green,Blue,Quantity_TOC_RGB) ;
	Standard_ShortReal r = Red,g = Green,b = Blue;
	unsigned long pixel = aPixel;
	return Xw_set_pixel(MyExtendedColorMap,pixel,r,g,b);
}
#endif

Standard_Integer Xw_ColorMap::Entry ( const Standard_Integer Index, Standard_Real &Red, Standard_Real &Green, Standard_Real &Blue ) const {
unsigned long pixel = 0 ;
float red,green,blue ;

	status = Xw_get_color(MyExtendedColorMap,Index,
					&red,&green,&blue,&pixel) ;

	if( !status ) PrintError() ;

	Red = red; Green = green; Blue = blue;

	return (Standard_Integer(pixel)) ;
}

Xw_TypeOfVisual Xw_ColorMap::VisualClass( ) const {
Xw_TypeOfVisual vclass ;

	vclass = Xw_get_colormap_visual(MyExtendedColorMap) ;

	if( vclass == Xw_TOV_DEFAULT ) PrintError() ;

	return (vclass) ;
}

Xw_TypeOfVisual Xw_ColorMap::OverlayVisualClass( ) const {
Xw_TypeOfVisual vclass = Xw_TOV_DEFAULT;

	if( MyExtendedOverlayColorMap ) {
	  vclass = Xw_get_colormap_visual(MyExtendedOverlayColorMap) ;
	}

	return (vclass) ;
}

Standard_Integer Xw_ColorMap::VisualID( ) const {
Aspect_Handle *VisualInfo ;
Xw_TypeOfVisual VisualClass ;
int MaxColor,BasePixel,MaxUserColor,MaxDefineColor,FirstFreeColorIndex,visualid = 0 ;

        status = Xw_get_colormap_info(MyExtendedColorMap,&VisualInfo,
                &VisualClass,&visualid,&MaxColor,&BasePixel,
                &MaxUserColor,&MaxDefineColor,&FirstFreeColorIndex) ;

	if( !status ) Xw_print_error();

	return (visualid) ;
}

Standard_Integer Xw_ColorMap::OverlayVisualID( ) const {
Aspect_Handle *VisualInfo ;
Xw_TypeOfVisual VisualClass ;
int MaxColor,BasePixel,MaxUserColor,MaxDefineColor,FirstFreeColorIndex,visualid = 0;

      if( MyExtendedOverlayColorMap ) { 
        status = Xw_get_colormap_info(MyExtendedOverlayColorMap,&VisualInfo,
                &VisualClass,&visualid,&MaxColor,&BasePixel,
                &MaxUserColor,&MaxDefineColor,&FirstFreeColorIndex) ;

        if( !status ) Xw_print_error() ;
      }
      return (visualid) ;
}

Standard_Integer Xw_ColorMap::MaxColors( ) const {
Aspect_Handle *VisualInfo ;
Xw_TypeOfVisual VisualClass ;
int MaxColor,BasePixel,MaxUserColor,MaxDefineColor,FirstFreeColorIndex,visualid ;

        status = Xw_get_colormap_info(MyExtendedColorMap,&VisualInfo,
                &VisualClass,&visualid,&MaxColor,&BasePixel,
                &MaxUserColor,&MaxDefineColor,&FirstFreeColorIndex) ;

	if( status ) {
	  if( VisualClass == Xw_TOV_TRUECOLOR ) return MaxColor ;
	  else return MaxUserColor;
	} else {
          Xw_print_error() ;
	  return 0 ;
	}
}

Standard_Integer Xw_ColorMap::MaxOverlayColors( ) const {
Aspect_Handle *VisualInfo ;
Xw_TypeOfVisual VisualClass ;
int MaxColor,BasePixel,MaxUserColor,MaxDefineColor,FirstFreeColorIndex,visualid ;

	status = XW_ERROR;
	if( MyExtendedOverlayColorMap ) {
          status = Xw_get_colormap_info(MyExtendedOverlayColorMap,&VisualInfo,
                &VisualClass,&visualid,&MaxColor,&BasePixel,
                &MaxUserColor,&MaxDefineColor,&FirstFreeColorIndex) ;
          if( !status ) Xw_print_error() ;
	} 

	return (status) ? MaxUserColor : 0;
}

Standard_Address Xw_ColorMap::XVisual( ) const {
Aspect_Handle *VisualInfo ;
Xw_TypeOfVisual VisualClass ;
int MaxColor,BasePixel,MaxUserColor,MaxDefineColor,FirstFreeColorIndex,visualid ;

        status = Xw_get_colormap_info(MyExtendedColorMap,&VisualInfo,
                &VisualClass,&visualid,&MaxColor,&BasePixel,
                &MaxUserColor,&MaxDefineColor,&FirstFreeColorIndex) ;

	if( !status ) PrintError() ;

	return (VisualInfo) ;
}

Standard_Address Xw_ColorMap::XOverlayVisual( ) const {
Aspect_Handle *VisualInfo = NULL ;
Xw_TypeOfVisual VisualClass ;
int MaxColor,BasePixel,MaxUserColor,MaxDefineColor,FirstFreeColorIndex,visualid ;

	status = XW_ERROR;
	if( MyExtendedOverlayColorMap ) {
          status = Xw_get_colormap_info(MyExtendedOverlayColorMap,&VisualInfo,
                &VisualClass,&visualid,&MaxColor,&BasePixel,
                &MaxUserColor,&MaxDefineColor,&FirstFreeColorIndex) ;
          if( !status ) Xw_print_error() ;
	}

	return (status) ? VisualInfo : NULL ;
}

Aspect_Handle Xw_ColorMap::XColorMap( ) const {
Aspect_Handle colormap ;

	colormap = Xw_get_colormap_xid(MyExtendedColorMap) ;

	if( !colormap ) PrintError() ;

	return (colormap) ;
}

Aspect_Handle Xw_ColorMap::XOverlayColorMap( ) const {
Aspect_Handle colormap = 0;

	if( MyExtendedOverlayColorMap ) {
	  colormap = Xw_get_colormap_xid(MyExtendedOverlayColorMap) ;
	}

	return (colormap);
}

Standard_Address Xw_ColorMap::ExtendedColorMap () const {

        return (MyExtendedColorMap);

}

Standard_Address Xw_ColorMap::ExtendedOverlayColorMap () const {

        return (MyExtendedOverlayColorMap);

}

Standard_Boolean Xw_ColorMap::XColorCube ( Aspect_Handle &ColormapID,
                         Standard_Integer &VisualID,
                         Standard_Integer &BasePixel,
                         Standard_Integer &RedMax,
                         Standard_Integer &RedMult,
                         Standard_Integer &GreenMax,
                         Standard_Integer &GreenMult,
                         Standard_Integer &BlueMax,
                         Standard_Integer &BlueMult ) const
{

	status = Xw_get_colormap_colorcube(MyExtendedColorMap,
		&ColormapID,&VisualID,&BasePixel,
			&RedMax,&RedMult,&GreenMax,&GreenMult,&BlueMax,&BlueMult);

	return status;
}


Standard_Boolean Xw_ColorMap::XGrayRamp ( Aspect_Handle &ColormapID,
                         Standard_Integer &VisualID,
                         Standard_Integer &BasePixel,
                         Standard_Integer &GrayMax,
                         Standard_Integer &GrayMult) const
{

	status = Xw_get_colormap_grayramp(MyExtendedColorMap,
		&ColormapID,&VisualID,&BasePixel,&GrayMax,&GrayMult);

	return status;
}

