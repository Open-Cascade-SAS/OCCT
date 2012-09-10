// Created by: NW,JPB,CAL,GG
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// Modified :
//		GG PRO5426 21/11/96
//              	Proteger le viewer 2D contre des tentatives d'affichage
//              	dans une fentre detruite ou iconifiee.
//		GG PRO6633 21/08/97
//             		Protection contre capture d'image de taille NULLE.
//		GG OPTIMISATION 18/09/97
//              	remplacer les appels myWindow->Position() par
//              	myWindow->Size() qui est plus optimum.
//		GG OPTIMISATION 20/10/97
//			Implementation de DoMapping()
//			qui met a jour le MAPPING de la fenetre lorsque
//			un evenemment de type GUI_IconifyEvent arrive dans
//			l'application.
//		GG PRO10676 15/12/97
//			Permettre de definir une couleur directement par ses
//			composantes.
//		GG PRO14093 03/07/98
//              	Eviter de "raiser" lorsqu'une erreur est detectee.
//		GG PRO14951 20/07/98
//			Ne pas modifier la couleur du background de la fenetre
//			lorsque aucune fille n'est cree.
//			(voir GUI_WidgetTools::DumpWidget())
//		GG CSR404 11/05/99
//			Avoid a crash in Dialog engine with a great number
//			of views.
//		GG G004	28/01/00 Add gama correction computation before dumping
//		an image.
// 		GG IMP060200
//			Xw_get_image() signature change.
//              GG IMP070200
//              Add MMSize() method
//      TCL at October 2000 (G002); new method SetBackground( CString aName )
//              SAV 24/11/01 SetBackground( Quantity_Color )

#define BUC60701	//GG 23/06/00 Enable to dump an exact image size
//			according to the window size
#define	RIC120302 	//GG Add a NEW XParentWindow methods which enable
//                      to retrieve the parent of the actual Xwindow ID.


//-Version

//-Design	Creation d'une fenetre X

//-Warning

//-References

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Xw_Window.ixx>
// Routines C a declarer en extern
//extern "C" {
#include <Xw_Cextern.hxx>
//}
#include <Aspect_Convert.hxx>
#include <Image_AlienPixMap.hxx>

#include <Xw_Extension.h>

//-Static data definitions

static XW_STATUS status ;

//============================================================================
//==== HashCode : Returns a HashCode CString
//============================================================================
inline Standard_Integer HashCode (const Standard_CString Value)
{
Standard_Integer  i,n,aHashCode = 0;
union {
  char             charPtr[80];
  int              intPtr[20];
} u;

  n = strlen(Value);

  if( n > 0 ) {
    if( n < 80 ) {
      n = (n+3)/4;
      u.intPtr[n-1] = 0;
      strcpy(u.charPtr,Value);
    } else {
      n = 20;
      strncpy(u.charPtr,Value,80);
    }

    for( i=0 ; i<n ; i++ ) {
      aHashCode = aHashCode ^ u.intPtr[i];
    }
  }

//printf(" HashCode of '%s' is %d\n",Value,aHashCode);

  return Abs(aHashCode) + 1;
}

void Xw_Window::PrintError() {
Standard_CString ErrorMessag ;
Standard_Integer ErrorNumber ;
Standard_Integer ErrorGravity ;

        status = XW_SUCCESS ;
        ErrorMessag = Xw_get_error(&ErrorNumber,&ErrorGravity) ;
        Xw_print_error() ;
}

//-Aliases

//-Global data definitions

//-Constructors

Xw_Window::Xw_Window (const Handle(Xw_GraphicDevice)& Device)
: Aspect_Window(Device)
{
  Init();
}


//=======================================================================
//function : Xw_Window
//purpose  :
//=======================================================================
Xw_Window::Xw_Window (const Handle(Xw_GraphicDevice)&   Device,
                      const Standard_Integer            aPart1,
                      const Standard_Integer            aPart2,
                      const Xw_WindowQuality            Quality,
                      const Quantity_NameOfColor        BackColor)
: Aspect_Window(Device)
{
  Init();
  Aspect_Handle aWindow = (aPart1 << 16) | (aPart2 & 0xFFFF);
  SetWindow (aWindow, Quality, BackColor);
}

//=======================================================================
//function : Xw_Window
//purpose  :
//=======================================================================
Xw_Window::Xw_Window (const Handle(Xw_GraphicDevice)&   Device,
                      const Aspect_Handle               aWindow,
                      const Xw_WindowQuality            Quality,
                      const Quantity_NameOfColor        BackColor)
: Aspect_Window(Device)
{
  Init();
  SetWindow (aWindow, Quality, BackColor);
}

//=======================================================================
//function : Xw_Window
//purpose  :
//=======================================================================
Xw_Window::Xw_Window (const Handle(Xw_GraphicDevice)&   Device,
                      const Standard_CString            Title,
                      const Quantity_Parameter          Xc,
                      const Quantity_Parameter          Yc,
                      const Quantity_Parameter          Width,
                      const Quantity_Parameter          Height,
                      const Xw_WindowQuality            Quality,
                      const Quantity_NameOfColor        BackColor,
                      const Aspect_Handle               Parent)
  : Aspect_Window(Device)
{
  Init();
  SetWindow (Title,Xc,Yc,Width,Height,Quality,BackColor,Parent);
}

//=======================================================================
//function : Xw_Window
//purpose  :
//=======================================================================
Xw_Window::Xw_Window (const Handle(Xw_GraphicDevice)& theDevice,
                      const Standard_CString          theTitle,
                      const Standard_Integer          thePxLeft,
                      const Standard_Integer          thePxTop,
                      const Standard_Integer          theWidth,
                      const Standard_Integer          theHeight,
                      const Xw_WindowQuality          theQuality,
                      const Quantity_NameOfColor      theBackColor,
                      const Aspect_Handle             theParent)
: Aspect_Window (theDevice)
{
  Init();

  MyExtendedDisplay = theDevice->ExtendedDisplay();
  Standard_Integer aParentSizeX = 1;
  Standard_Integer aParentSizeY = 1;

  Aspect_Handle aRoot, aColormap, *aDisplay;
  Xw_TypeOfVisual aVisualClass;
  Standard_Integer aVisualDepth;
  Xw_get_display_info (MyExtendedDisplay,
                       &aDisplay, &aRoot, &aColormap, &aVisualClass, &aVisualDepth);
  Xw_get_screen_size (MyExtendedDisplay, &aParentSizeX, &aParentSizeY);
  if (theParent)
  {
    XWindowAttributes anAttributes;
    if(XGetWindowAttributes ((Display* )aDisplay, theParent, &anAttributes))
    {
      aParentSizeX = anAttributes.width;
      aParentSizeY = anAttributes.height;
    }
  }
  Quantity_Parameter aQCenterX, aQCenterY, aQSizeX, aQSizeY;
  Aspect_Convert::ConvertCoordinates (aParentSizeX, aParentSizeY,
                                      thePxLeft, thePxTop, theWidth, theHeight,
                                      aQCenterX, aQCenterY, aQSizeX, aQSizeY);
  SetWindow (theTitle, aQCenterX, aQCenterY, aQSizeX, aQSizeY,
             theQuality, theBackColor, theParent);
}

void Xw_Window::Init()
{
  MyBackgroundIndex = 0 ;

  MyXWindow = 0 ;
  MyXParentWindow = 0 ;
  MyXPixmap = 0 ;
  MyDepth = 0 ;
  MyExtendedDisplay = NULL ;
  MyExtendedWindow = NULL ;
  MyExtendedColorMap = NULL ;
  MyExtendedTypeMap = NULL ;
  MyExtendedWidthMap = NULL ;
  MyExtendedFontMap = NULL ;
  MyExtendedMarkMap = NULL ;

  MyQuality     = Xw_WQ_3DQUALITY;
  MyVisualClass = Xw_TOV_STATICGRAY;
}

//=======================================================================
//function : SetWindow
//purpose  :
//=======================================================================

void Xw_Window::SetWindow (const Aspect_Handle          aWindow,
                           const Xw_WindowQuality       Quality,
                           const Quantity_NameOfColor   BackColor)
{
  if( (Quality == Xw_WQ_TRANSPARENT) || (Quality == Xw_WQ_OVERLAY) ) {
    SetWindow ("",0.5,0.5,1.0,1.0,Quality,BackColor,aWindow);
  } else {
    Aspect_Handle window,root,colormap,pixmap ;
    Xw_TypeOfVisual visualclass ;
    int visualdepth,visualid ;
    const Handle(Xw_GraphicDevice)& Device =
      Handle(Xw_GraphicDevice)::DownCast(MyGraphicDevice);

    MyExtendedDisplay = Device->ExtendedDisplay() ;

    MyBackgroundIndex = 0 ;
    MyXWindow = aWindow ;
    MyXParentWindow = aWindow ;
    MyQuality = Quality ;

    if( !MyXWindow ) {
      PrintError() ;
    }

    MyExtendedWindow =
      Xw_def_window (MyExtendedDisplay,MyXWindow,Standard_False);

    status = Xw_get_window_info(MyExtendedWindow,&window,&pixmap,
                                &root,&colormap,&visualclass,
                                &visualdepth,&visualid);
    if( !status ) {
      PrintError() ;
    }

    Standard_Boolean Res1 =
	(Device->ExtendedColorMap3D() == Device->ExtendedColorMap2D());
    Standard_Boolean Res2 =
	(Quality == Xw_WQ_SAMEQUALITY);
    Standard_Boolean Res3 =
	(Quality == Xw_WQ_DRAWINGQUALITY && visualclass == Xw_TOV_PSEUDOCOLOR);
    Standard_Boolean Res4 =
	(Quality == Xw_WQ_3DQUALITY && visualclass == Xw_TOV_TRUECOLOR);

      //if (Quality == Xw_WQ_3DQUALITY && visualclass == Xw_TOV_PSEUDOCOLOR) {
	//Res4 = Standard_True;
	//visualclass = Xw_TOV_TRUECOLOR;
      //}

    if (Res1 || Res2 || Res3 || Res4) {

      MyVisualClass = visualclass ;
      MyXPixmap = pixmap ;
      MyDepth = visualdepth ;

      if( MyVisualClass == Xw_TOV_TRUECOLOR ) {
        MyColorMap = Device->ColorMap3D() ;
      } else {
        MyColorMap = Device->ColorMap2D() ;
      }

      if( visualid == MyColorMap->OverlayVisualID() ) {
        MyExtendedColorMap = MyColorMap->ExtendedOverlayColorMap();
      } else {
        MyExtendedColorMap = MyColorMap->ExtendedColorMap();
      }

      MyTypeMap = Device->TypeMap() ;
      MyExtendedTypeMap = Device->ExtendedTypeMap();
      MyWidthMap = Device->WidthMap() ;
      MyExtendedWidthMap = Device->ExtendedWidthMap();
      MyFontMap = Device->FontMap() ;
      MyExtendedFontMap = Device->ExtendedFontMap();
      MyMarkMap = Device->MarkMap() ;
      MyExtendedMarkMap = Device->ExtendedMarkMap();

      status = Xw_set_colormap(MyExtendedWindow,MyExtendedColorMap) ;

      if( !status ) {
        PrintError() ;
      }

      status = Xw_set_typemap(MyExtendedWindow,MyExtendedTypeMap) ;

      if( !status ) {
        PrintError() ;
      }

      status = Xw_set_widthmap(MyExtendedWindow,MyExtendedWidthMap) ;

      if( !status ) {
        PrintError() ;
      }

      status = Xw_set_fontmap(MyExtendedWindow,MyExtendedFontMap) ;

      if( !status ) {
        PrintError() ;
      }

      status = Xw_set_markmap(MyExtendedWindow,MyExtendedMarkMap) ;

      if( !status ) {
        PrintError() ;
      }
      SetBackground(BackColor) ;

    } else { 	// Create a child of this Window in the right Visual
      XW_WINDOWSTATE state = XW_WS_UNKNOWN ;
      int pxc, pyc;
      int width, height;
      float ratio;
      Quantity_Parameter Width = 1.0;
      Quantity_Parameter Height= 1.0;

      state = Xw_get_window_position (MyExtendedWindow,
                                      &pxc,&pyc,&width,&height);
      if( state == XW_WS_UNKNOWN ) {
        Xw_print_error() ;
      }
      ratio = float (width)/ float (height);
      if (ratio > 1.)
        Width = Width*ratio;
      else
        Height = Height/ratio;
      SetWindow ("",0.5,0.5,Width,Height,Quality,BackColor,MyXWindow);
    }
  }
}

//=======================================================================
//function : SetWindow
//purpose  :
//=======================================================================

void Xw_Window::SetWindow (const Standard_CString       Title,
                           const Quantity_Parameter     Xc,
                           const Quantity_Parameter     Yc,
                           const Quantity_Parameter     Width,
                           const Quantity_Parameter     Height,
                           const Xw_WindowQuality       Quality,
                           const Quantity_NameOfColor   BackColor,
                           const Aspect_Handle          Parent)
{
  Aspect_Handle window,root,colormap,pixmap,*display ;
  Xw_TypeOfVisual visualclass ;
  Standard_Integer visualdepth,visualid,istransparent = Standard_False ;

  const Handle(Xw_GraphicDevice)& Device =
    Handle(Xw_GraphicDevice)::DownCast(MyGraphicDevice);

  MyBackgroundIndex = 0 ;
  MyExtendedDisplay = Device->ExtendedDisplay() ;
  MyXParentWindow = Parent ;
  MyXWindow = 0 ;
  MyQuality = Quality ;

  switch (Quality) {
  case Xw_WQ_3DQUALITY :
  case Xw_WQ_PICTUREQUALITY :
    MyColorMap = Device->ColorMap3D() ;
    MyVisualClass = Device->VisualClass3D() ;
    MyExtendedColorMap = Device->ExtendedColorMap3D();
    break ;
  case Xw_WQ_DRAWINGQUALITY :
    MyColorMap = Device->ColorMap2D() ;
    MyVisualClass = Device->VisualClass2D() ;
    MyExtendedColorMap = Device->ExtendedColorMap2D();
    break ;
  case Xw_WQ_SAMEQUALITY :
    if( !MyXParentWindow ) {
      status = Xw_get_display_info (MyExtendedDisplay,
                                    &display,&root,&colormap,&visualclass,
                                    &visualdepth) ;
    } else {
      MyExtendedWindow = Xw_def_window(MyExtendedDisplay,
                                       MyXParentWindow,Standard_False);

      status = Xw_get_window_info(MyExtendedWindow,&window,
                                  &pixmap,&root,&colormap,&visualclass,&visualdepth,
                                  &visualid) ;
      Xw_close_window(MyExtendedWindow,Standard_False) ;
      MyExtendedWindow = NULL;
    }

    if( !status ) {
      PrintError() ;
    }
    if( visualclass == Xw_TOV_TRUECOLOR ) {
      MyColorMap = Device->ColorMap3D() ;
      MyVisualClass = Device->VisualClass3D() ;
      MyExtendedColorMap = Device->ExtendedColorMap3D();
    } else if( visualclass == Xw_TOV_PSEUDOCOLOR ) {
      MyColorMap = Device->ColorMap2D() ;
      MyVisualClass = Device->VisualClass2D() ;
      MyExtendedColorMap = Device->ExtendedColorMap2D();
    }
    break ;
  case Xw_WQ_TRANSPARENT :
    istransparent = Standard_True ;
  case Xw_WQ_OVERLAY :
    if( !MyXParentWindow ) {
      status = Xw_get_display_info (MyExtendedDisplay,
                                    &display,&root,&colormap,&visualclass,
                                    &visualdepth) ;
    } else {
      MyExtendedWindow = Xw_def_window(MyExtendedDisplay,
                                       MyXParentWindow,istransparent);

      status = Xw_get_window_info(MyExtendedWindow,&window,
                                  &pixmap,&root,&colormap,&visualclass,&visualdepth,
                                  &visualid) ;
      Xw_close_window(MyExtendedWindow,Standard_False) ;
      MyExtendedWindow = NULL;
    }

    if( !status ) {
      PrintError() ;
    }
    if( visualclass == Xw_TOV_TRUECOLOR ) {
      MyColorMap = Device->ColorMap3D() ;
      MyExtendedColorMap = Device->ExtendedOverlayColorMap3D();
      MyVisualClass = Xw_TOV_OVERLAY;
      if( !MyExtendedColorMap ) {
        MyVisualClass = Device->VisualClass3D() ;
        MyExtendedColorMap = Device->ExtendedColorMap3D();
      }
    } else if( visualclass == Xw_TOV_PSEUDOCOLOR ) {
      MyColorMap = Device->ColorMap2D() ;
      MyExtendedColorMap = Device->ExtendedOverlayColorMap2D();
      MyVisualClass = Xw_TOV_OVERLAY;
      if( !MyExtendedColorMap ) {
        MyVisualClass = Device->VisualClass2D() ;
        MyExtendedColorMap = Device->ExtendedColorMap2D();
      }
    }
    break ;
  }

  if( MyVisualClass != Xw_TOV_DEFAULT ) {
    MyTypeMap = Device->TypeMap() ;
    MyExtendedTypeMap = Device->ExtendedTypeMap();
    MyWidthMap = Device->WidthMap() ;
    MyExtendedWidthMap = Device->ExtendedWidthMap();
    MyFontMap = Device->FontMap() ;
    MyExtendedFontMap = Device->ExtendedFontMap();
    MyMarkMap = Device->MarkMap() ;
    MyExtendedMarkMap = Device->ExtendedMarkMap();

    MyXWindow = Xw_open_window(MyExtendedDisplay,MyVisualClass,MyXParentWindow,
                               (float)Xc,(float)Yc,(float)Width,(float)Height,
                               (Standard_PCharacter)Title,istransparent) ;
    if( !MyXWindow ) {
      PrintError() ;
    }

    MyExtendedWindow =
      Xw_def_window (MyExtendedDisplay,MyXWindow,istransparent);

    status = Xw_get_window_info(MyExtendedWindow,&window,&pixmap,
                                &root,&colormap,&visualclass,&visualdepth,
                                &visualid) ;
    if( !status ) {
      PrintError() ;
    }

    MyXPixmap = pixmap ;
    MyDepth = visualdepth ;

    status = Xw_set_colormap(MyExtendedWindow,MyExtendedColorMap) ;

    if( !status ) {
      PrintError() ;
    }

    status = Xw_set_typemap(MyExtendedWindow,MyExtendedTypeMap) ;

    if( !status ) {
      PrintError() ;
    }

    status = Xw_set_widthmap(MyExtendedWindow,MyExtendedWidthMap) ;

    if( !status ) {
      PrintError() ;
    }

    status = Xw_set_fontmap(MyExtendedWindow,MyExtendedFontMap) ;

    if( !status ) {
      PrintError() ;
    }

    status = Xw_set_markmap(MyExtendedWindow,MyExtendedMarkMap) ;

    if( !status ) {
      PrintError() ;
    }

    SetBackground(BackColor) ;

    if( MyXParentWindow && (MyXWindow != MyXParentWindow) ) Map() ;
  }
}

void Xw_Window::SetBackground (const Aspect_Background& Background) {
Quantity_Color Color = Background.Color() ;

	SetBackground(Color.Name());

}

void Xw_Window::SetBackground (const Quantity_NameOfColor BackColor) {
Quantity_Color Color ;
Standard_Real r,g,b ;
Standard_Integer index;

    Standard_Integer bcolor = Standard_Integer(BackColor);
    if( (MyQuality != Xw_WQ_TRANSPARENT) && (bcolor >= 0) ) {
	MyBackground.SetColor(BackColor) ;

	Color = MyBackground.Color() ;
	Color.Values(r,g,b,Quantity_TOC_RGB) ;

	status = Xw_get_color_index(MyExtendedColorMap,
					(float)r,(float)g,(float)b,&index);

        if( status ) {
	  MyBackgroundIndex = index;
          status = Xw_close_background_pixmap(MyExtendedWindow);
          status = Xw_set_background_index (MyExtendedWindow,index) ;
        }

        if( !status ) {
	    PrintError() ;
        }
    }
}

void Xw_Window::SetBackground(const Quantity_Color& color )
{
  Standard_Real r,g,b;
  Standard_Integer index;

  Standard_Integer bcolor = Standard_Integer( color.Name() );

  if( ( MyQuality != Xw_WQ_TRANSPARENT ) && ( bcolor >= 0 ) ) {

    MyBackground.SetColor( color ) ;
    color.Values( r, g, b, Quantity_TOC_RGB );

    status = Xw_get_color_index( MyExtendedColorMap,
				 (float)r, (float)g, (float)b, &index);

    if( status ) {
      MyBackgroundIndex = index;
      status = Xw_close_background_pixmap( MyExtendedWindow );
      status = Xw_set_background_index( MyExtendedWindow, index );
    }

    if( !status ) {
      PrintError() ;
    }
  }
}

//=======================================================================
//function : SetBackground
//purpose  :
//=======================================================================

void Xw_Window::SetBackground (const Aspect_Handle aPixmap)
{
  status = Xw_set_background_pixmap( MyExtendedWindow, aPixmap);
  if ( status ) {
    MyHBackground = aPixmap;
    MyBackgroundImage.Clear();
    MyBackgroundFillMethod = Aspect_FM_NONE;
  } else {
    PrintError();
  }
}

Standard_Boolean Xw_Window::SetBackground( const Standard_CString aName,
				 	     const Aspect_FillMethod aMethod ) {

 if( !aName ) return Standard_False;

 status = XW_ERROR;
 Standard_Integer hash_code = ::HashCode( aName );
 XW_EXT_IMAGEDATA* pimage = (XW_EXT_IMAGEDATA* )Xw_get_image_handle (MyExtendedWindow, (void* )hash_code);

 if ( !pimage )
  pimage = (XW_EXT_IMAGEDATA* )Xw_load_image(MyExtendedWindow,(void*)hash_code,(Standard_PCharacter)aName );

 if ( pimage )
    status = Xw_put_background_image( MyExtendedWindow, pimage, aMethod );

  if ( status ) {
    MyBackgroundImage = aName;
    MyBackgroundFillMethod = aMethod;
    Xw_get_background_pixmap( MyExtendedWindow, MyHBackground);
  } else {
    PrintError();
  }

  return status;

}  // end Xw_Window::SetBackground

void Xw_Window::SetBackground (const Aspect_GradientBackground& GrBackground) {

  Quantity_Color Color1, Color2;
  GrBackground.Colors(Color1,Color2);
  SetBackground(Color1,Color2, GrBackground.BgGradientFillMethod());

}

void Xw_Window::SetBackground( const Quantity_Color& color1,
                               const Quantity_Color& color2,
                               const Aspect_GradientFillMethod aMethod) {

  Standard_Integer bcolor = Standard_Integer( color1.Name() ) + Standard_Integer( color2.Name() );
  if( ( MyQuality != Xw_WQ_TRANSPARENT ) && ( bcolor >= 0 ) )
    MyGradientBackground.SetColors( color1, color2, aMethod ) ;

}

void Xw_Window::SetDoubleBuffer (const Standard_Boolean DBmode) {

        status = Xw_set_double_buffer(MyExtendedWindow,
					(XW_DOUBLEBUFFERMODE)DBmode) ;

        if( !status ) {
	    PrintError() ;
        }
}

void Xw_Window::Flush() const {

        status = Xw_flush (MyExtendedWindow,Standard_False);

        if( !status ) {
	    PrintError() ;
	}
}

void Xw_Window::Map () const {
#ifdef RIC120302
	if( MyXWindow == MyXParentWindow ) return;
#endif
  if (IsVirtual()) return;
	status = Xw_set_window_state (MyExtendedWindow, XW_MAP);
	if( !status ) {
	    PrintError() ;
	}
}

void Xw_Window::Unmap () const {
#ifdef RIC120302
	if( MyXWindow == MyXParentWindow ) return;
#endif
	status = Xw_set_window_state (MyExtendedWindow, XW_ICONIFY);
	if( !status ) {
	    PrintError() ;
	}
}

Aspect_TypeOfResize Xw_Window::DoResize () const {
XW_RESIZETYPE state ;

	state = Xw_resize_window (MyExtendedWindow);
//	if( state == Aspect_TOR_UNKNOWN ) {
	if( state == XW_TOR_UNKNOWN ) {
	    PrintError() ;
	}

	return (Aspect_TypeOfResize(state)) ;
}

Standard_Boolean Xw_Window::DoMapping () const {
int pxc,pyc,width,height;
XW_WINDOWSTATE state;

      state = Xw_get_window_position (MyExtendedWindow,&pxc,&pyc,&width,&height);

      return IsMapped();
}

void Xw_Window::Destroy () {
Standard_Boolean destroy =
	( MyXWindow == MyXParentWindow ) ? Standard_False : Standard_True;

	status = Xw_close_window (MyExtendedWindow,destroy);
	if( !status ) {
	    PrintError() ;
	}

	MyXWindow = 0 ;
	MyExtendedWindow = NULL ;
	MyExtendedColorMap = NULL ;
	MyExtendedTypeMap = NULL ;
	MyExtendedWidthMap = NULL ;
	MyExtendedFontMap = NULL ;
	MyExtendedMarkMap = NULL ;
}

void Xw_Window::Clear () const {

        status = Xw_erase_window (MyExtendedWindow);

        if( !status ) {
	    PrintError() ;
        }
}

void Xw_Window::ClearArea (const Standard_Integer Xc, const Standard_Integer Yc, const Standard_Integer Width, const Standard_Integer Height) const {

        status = Xw_erase_area (MyExtendedWindow,int(Xc),int(Yc),
                                                 int(Width),int(Height));

        if( !status ) {
	    PrintError() ;
        }
}

void Xw_Window::Restore () const {

	if( !IsMapped() ) return;
        status = Xw_restore_window (MyExtendedWindow);

        if( !status ) {
	    PrintError() ;
        }
}

void Xw_Window::RestoreArea (const Standard_Integer Xc, const Standard_Integer Yc, const Standard_Integer Width, const Standard_Integer Height) const {

	if( !IsMapped() ) return;
        status = Xw_restore_area (MyExtendedWindow,int(Xc),int(Yc),
                                                 int(Width),int(Height));

        if( !status ) {
	    PrintError() ;
        }
}

Standard_Boolean Xw_Window::Dump (const Standard_CString theFilename,
                                  const Standard_Real theGammaValue) const
{
  int aDummy, aWidth, aHeight;
  XW_WINDOWSTATE state = Xw_get_window_position (MyExtendedWindow,
                                                 &aDummy, &aDummy, &aWidth, &aHeight);
  if (state == XW_WS_UNKNOWN)
  {
    return Standard_False;
  }

  return DumpArea (theFilename, aWidth / 2, aHeight / 2,
                   aWidth, aHeight, theGammaValue);
}

Standard_Boolean Xw_Window::DumpArea (const Standard_CString theFilename,
                                      const Standard_Integer theXc, const Standard_Integer theYc,
                                      const Standard_Integer theWidth, const Standard_Integer theHeight,
                                      const Standard_Real theGammaValue) const
{
  int aWidth  = Abs(theWidth);
  int aHeight = Abs(theHeight);
  XW_EXT_IMAGEDATA* pimage = NULL;
  if (DoubleBuffer())
  {
    Aspect_Handle window, root, colormap, pixmap;
    Xw_TypeOfVisual visualclass;
    int visualdepth, visualid;
    Xw_get_window_info (MyExtendedWindow, &window, &pixmap,
                        &root, &colormap, &visualclass,
                        &visualdepth, &visualid);

    pimage = (XW_EXT_IMAGEDATA* )Xw_get_image_from_pixmap (MyExtendedWindow, (Standard_PCharacter )theFilename,
                                                           pixmap,
                                                           theXc, theYc, aWidth, aHeight);
  }
  else
  {
    pimage = (XW_EXT_IMAGEDATA* )Xw_get_image (MyExtendedWindow, (Standard_PCharacter )theFilename,
                                               theXc, theYc, aWidth, aHeight);
  }

  if (pimage == NULL)
  {
    return Standard_False;
  }

  if (theGammaValue != 1.0)
  {
    Xw_gamma_image (pimage, Standard_ShortReal (theGammaValue));
  }
  XW_STATUS aStatus = Xw_save_image (MyExtendedWindow, pimage, (Standard_PCharacter )theFilename);
  Xw_close_image (pimage);

  return Standard_Boolean(aStatus);
}

Standard_Boolean Xw_Window::ToPixMap (Image_PixMap& thePixMap) const
{
  int aDummy, aWidth, aHeight;
  XW_WINDOWSTATE state = Xw_get_window_position (MyExtendedWindow,
                                                 &aDummy, &aDummy, &aWidth, &aHeight);
  if (state == XW_WS_UNKNOWN)
  {
    return Standard_False;
  }

  XW_EXT_IMAGEDATA* pimage = NULL;
  if (DoubleBuffer())
  {
    Aspect_Handle window, root, colormap, pixmap;
    Xw_TypeOfVisual visualclass;
    int visualdepth, visualid;
    Xw_get_window_info (MyExtendedWindow, &window, &pixmap,
                        &root, &colormap, &visualclass,
                        &visualdepth, &visualid);

    pimage = (XW_EXT_IMAGEDATA* )Xw_get_image_from_pixmap (MyExtendedWindow, NULL,
                                                           pixmap,
                                                           aWidth / 2, aHeight / 2, aWidth, aHeight);
  }
  else
  {
    pimage = (XW_EXT_IMAGEDATA* )Xw_get_image (MyExtendedWindow, NULL,
                                               aWidth / 2, aHeight / 2, aWidth, aHeight);
  }

  if (pimage == NULL)
  {
    return Standard_False;
  }

  XImage* pximage = (pimage->zximage) ? pimage->zximage : pimage->pximage;
  XW_EXT_WINDOW* pwindow = (XW_EXT_WINDOW* )MyExtendedWindow;
  if (pwindow->pcolormap->visual->c_class != TrueColor)
  {
    return Standard_False;
  }

  const bool isBigEndian = (pximage->byte_order == MSBFirst);
  Image_PixMap::ImgFormat aFormat = (pximage->bits_per_pixel == 32)
                                  ? (isBigEndian ? Image_PixMap::ImgRGB32 : Image_PixMap::ImgBGR32)
                                  : (isBigEndian ? Image_PixMap::ImgRGB   : Image_PixMap::ImgBGR);
  Image_PixMap aWrapper;
  aWrapper.InitWrapper (aFormat, (Standard_Byte* )pximage->data, pximage->width, pximage->height, pximage->bytes_per_line);
  aWrapper.SetTopDown (true);

  Standard_Boolean isSuccess = thePixMap.InitCopy (aWrapper);
  Xw_close_image (pimage);
  return isSuccess;
}

Standard_Boolean Xw_Window::Load (const Standard_CString aFilename) const {
Standard_Integer hashcode = ::HashCode(aFilename) ;
XW_WINDOWSTATE state;
int pxc,pyc,wwidth,wheight,iwidth,iheight,idepth,resize = Standard_False;
float izoom;

    state = Xw_get_window_position (MyExtendedWindow,&pxc,&pyc,&wwidth,&wheight);

    status = XW_ERROR;
    XW_EXT_IMAGEDATA* pimage = (XW_EXT_IMAGEDATA* )Xw_get_image_handle( MyExtendedWindow,(void*)hashcode ) ;
    if( !pimage ) {
        pimage = (XW_EXT_IMAGEDATA* )Xw_load_image(MyExtendedWindow,(void*)hashcode,(Standard_PCharacter)aFilename );
    }

    if( pimage ) {
        status = Xw_get_image_info(pimage,&izoom,&iwidth,&iheight,&idepth);
	iwidth = (int)(iwidth/izoom);
	iheight = (int)(iheight/izoom);
    }

    if( status && state != XW_WS_UNKNOWN ) {
      float uxc,uyc ;
      if( iwidth > wwidth ) {
	resize = Standard_True;
	wwidth = iwidth;
      }
      if( iheight > wheight ) {
	resize = Standard_True;
	wheight = iheight;
      }
      if( resize ) {
        status = Xw_set_window_position (MyExtendedWindow,
						pxc,pyc,wwidth,wheight);
      }
      status = Xw_get_window_pixelcoord(MyExtendedWindow,
					wwidth/2,wheight/2,&uxc,&uyc);
      status = Xw_draw_image(MyExtendedWindow, pimage, uxc, uyc);
      Xw_flush(MyExtendedWindow, Standard_True);
    } else status = XW_ERROR ;

    if( !status ) {
      PrintError() ;
    }

    return Standard_Boolean(status);
}

Standard_Boolean Xw_Window::LoadArea (const Standard_CString aFilename, const Standard_Integer Xc, const Standard_Integer Yc, const Standard_Integer Width, const Standard_Integer Height) const {
Standard_Integer hashcode = ::HashCode(aFilename) ;
int pxc,pyc,wwidth,wheight,iwidth,iheight,idepth;
float izoom,uxc,uyc;

    XW_WINDOWSTATE state = Xw_get_window_position (MyExtendedWindow,&pxc,&pyc,&wwidth,&wheight);
    status = XW_ERROR;
    XW_EXT_IMAGEDATA* pimage = (XW_EXT_IMAGEDATA* )Xw_get_image_handle( MyExtendedWindow,(void*)hashcode ) ;
    if( !pimage ) {
        pimage = (XW_EXT_IMAGEDATA* )Xw_load_image(MyExtendedWindow,(void*)hashcode,(Standard_PCharacter)aFilename );
    }

    if( pimage ) {
        status = Xw_get_image_info(pimage,&izoom,&iwidth,&iheight,&idepth);
	iwidth = (int)(iwidth/izoom);
	iheight = (int)(iheight/izoom);
    }

    if( status && state != XW_WS_UNKNOWN ) {
	if( iwidth > Width || iheight > Height ) {
            izoom = (float)Min(Width,Height)/(float)Max(iwidth,iheight) ;
	    status = Xw_zoom_image(pimage,izoom) ;
	}
        status = Xw_get_window_pixelcoord(MyExtendedWindow,Xc,Yc,&uxc,&uyc);
        status = Xw_draw_image(MyExtendedWindow, pimage, uxc, uyc);
	Xw_flush(MyExtendedWindow, Standard_True);
    } else status = XW_ERROR ;

    if( !status ) {
      PrintError() ;
    }

    return Standard_Boolean(status);
}

void Xw_Window::SetCursor (const Standard_Integer anId, const Quantity_NameOfColor aColor) const {
Quantity_Color Color(aColor) ;
Standard_Real r,g,b ;

	Color.Values(r,g,b,Quantity_TOC_RGB) ;

    	status = Xw_set_hard_cursor (MyExtendedWindow,(int)anId,0,
							(float)r,
							(float)g,
							(float)b) ;

        if( !status ) {
            PrintError() ;
        }
}

Standard_Boolean Xw_Window::BackingStore () const {

	if( MyXPixmap ) status = XW_SUCCESS ;
	else {
	    status = Xw_open_pixmap(MyExtendedWindow) ;
	}

        return (Standard_Boolean(status)) ;
}

Standard_Boolean Xw_Window::DoubleBuffer () const {
XW_DOUBLEBUFFERMODE state = Xw_get_double_buffer(MyExtendedWindow) ;

        return (state == XW_ENABLE ? Standard_True : Standard_False) ;
}

Standard_Boolean Xw_Window::IsMapped () const {
  if (IsVirtual()) {
    return Standard_True;
  }
XW_WINDOWSTATE state;
      state = Xw_get_window_state (MyExtendedWindow);
      switch (state) {
        case XW_WS_UNKNOWN:
	  return Standard_False;
        case XW_ICONIFY:
	  return Standard_False;
        case XW_PUSH:
	  return Standard_True;
        case XW_MAP:
	  return Standard_True;
#ifndef DEB
	default:
	  return Standard_False;
#endif
      }
  return Standard_False;
}

Standard_Real Xw_Window::Ratio () const {
int width, height;

	status = Xw_get_window_size (MyExtendedWindow,&width,&height);
	if( !status ) {
	    Xw_print_error() ;
	}

	return ((Standard_Real)width/height) ;
}

void Xw_Window::Size (Standard_Real &Width, Standard_Real &Height) const {
int width, height;

	status = Xw_get_window_size (MyExtendedWindow,&width,&height);
	if( !status ) {
	    Xw_print_error() ;
	}

	Width = Xw_get_screen_pixelvalue (MyExtendedDisplay,width) ;
	Height = Xw_get_screen_pixelvalue (MyExtendedDisplay,height) ;
}

void Xw_Window::Size (Standard_Integer &Width, Standard_Integer &Height) const {
int width, height;
	status = Xw_get_window_size (MyExtendedWindow,&width,&height);
	if( !status ) {
	    Xw_print_error() ;
	}

	Width = width ;
	Height = height ;
}

void Xw_Window::MMSize (Standard_Real &Width, Standard_Real &Height) const {
int width, height;

	status = Xw_get_window_size (MyExtendedWindow,&width,&height);
	if( !status ) {
	    Xw_print_error() ;
	}

	float x,y ;
        status = Xw_get_window_pixelcoord(MyExtendedWindow,
                                                width,0,&x,&y) ;
        if( !status ) {
            PrintError() ;
        }

        Width = Standard_Real(x); Height = Standard_Real(y);
}

void Xw_Window::Position (Standard_Real &X1, Standard_Real &Y1, Standard_Real &X2, Standard_Real &Y2) const {
XW_WINDOWSTATE state = XW_WS_UNKNOWN ;
int pxc, pyc;
int width, height;
float x1,y1,x2,y2 ;

	state = Xw_get_window_position (MyExtendedWindow,
						&pxc,&pyc,&width,&height);
	if( state == XW_WS_UNKNOWN ) {
	    Xw_print_error() ;
	}
	Xw_get_screen_pixelcoord (MyExtendedDisplay,pxc-width/2,
						   pyc-height/2,
						   &x1,&y1) ;
	Xw_get_screen_pixelcoord (MyExtendedDisplay,pxc+width/2,
						   pyc+height/2,
						   &x2,&y2) ;
	X1 = x1 ; Y1 = y2 ; X2 = x2 ; Y2 = y1 ;
}

void Xw_Window::Position (Standard_Integer &X1, Standard_Integer &Y1, Standard_Integer &X2, Standard_Integer &Y2) const {
XW_WINDOWSTATE state = XW_WS_UNKNOWN ;
int pxc, pyc;
int width, height;

	state = Xw_get_window_position (MyExtendedWindow,
						&pxc,&pyc,&width,&height);
	if( state == XW_WS_UNKNOWN ) {
	    Xw_print_error() ;
	}

	X1 = pxc - width/2 ; Y1 = pyc - height/2 ;
	X2 = X1 + width - 1 ; Y2 = Y1 + height - 1 ;
}

Standard_Real Xw_Window::Convert (const Standard_Integer Pv) const {
Standard_Real Dv ;

    	Dv = Xw_get_screen_pixelvalue(MyExtendedDisplay,(int)Pv) ;

	return (Dv) ;
}

void Xw_Window::Convert (const Standard_Integer Px, const Standard_Integer Py, Standard_Real &Dx, Standard_Real &Dy ) const {
float x,y ;

    	status = Xw_get_screen_pixelcoord(MyExtendedDisplay,
						(int)Px,(int)Py,&x,&y) ;

	if( !status ) {
	    Xw_print_error() ;
    	}

	Dx = x ; Dy = y ;
}

Standard_Integer Xw_Window::Convert (const Standard_Real Dv) const {
Standard_Integer Pv ;

    	Pv = Xw_get_pixel_screenvalue(MyExtendedDisplay,(float)Dv) ;

	return (Pv) ;
}

void Xw_Window::Convert (const Standard_Real Dx, const Standard_Real Dy, Standard_Integer &Px, Standard_Integer &Py ) const {
int x,y ;

    	status = Xw_get_pixel_screencoord(MyExtendedDisplay,
						(float)Dx,(float)Dy,&x,&y) ;

	if( !status ) {
	    Xw_print_error() ;
    	}

	Px = x ; Py = y ;
}

Handle(Xw_ColorMap) Xw_Window::ColorMap() const {

	return (MyColorMap) ;
}

Handle(Xw_TypeMap) Xw_Window::TypeMap() const {

	return (MyTypeMap) ;
}

Handle(Xw_WidthMap) Xw_Window::WidthMap() const {

	return (MyWidthMap) ;
}

Handle(Xw_FontMap) Xw_Window::FontMap() const {

	return (MyFontMap) ;
}

Handle(Xw_MarkMap) Xw_Window::MarkMap() const {

	return (MyMarkMap) ;
}

Aspect_Handle Xw_Window::XWindow () const {

	return (MyXWindow);

}

void Xw_Window::XWindow (Standard_Integer& aPart1, Standard_Integer& aPart2) const {

  aPart1 = (Standard_Integer)((MyXWindow >> 16 ) & 0xffff);
  aPart2 = (Standard_Integer)(MyXWindow & 0xffff);
}

// RIC120302
Aspect_Handle Xw_Window::XParentWindow () const {

	return (MyXParentWindow);

}

void Xw_Window::XParentWindow (Standard_Integer& aPart1, Standard_Integer& aPart2) const {

  aPart1 = (Standard_Integer)((MyXParentWindow >> 16 ) & 0xffff);
  aPart2 = (Standard_Integer)(MyXParentWindow & 0xffff);
}
// RIC120302

Aspect_Handle Xw_Window::XPixmap () const {
Aspect_Handle window,pixmap,root,colormap ;
Xw_TypeOfVisual visualclass ;
int visualdepth,visualid ;

	status = Xw_get_window_info(MyExtendedWindow,&window,&pixmap,
			&root,&colormap,&visualclass,&visualdepth,&visualid) ;

	return (pixmap);
}

Standard_Address Xw_Window::XVisual( ) const
{
  Aspect_Handle *VisualInfo ;
  Xw_TypeOfVisual VisualClass ;
  int MaxColor,BaseColor,MaxUserColor,MaxDefineColor,FirstFreeColorIndex,visualid ;

  status = Xw_get_colormap_info(MyExtendedColorMap,&VisualInfo,
                                &VisualClass,&visualid,&MaxColor,&BaseColor,
                                &MaxUserColor,&MaxDefineColor,&FirstFreeColorIndex);

  if( !status ) {
    PrintError() ;
  }

  return (VisualInfo) ;
}

Aspect_Handle Xw_Window::XColorMap( ) const
{
  Aspect_Handle colormap ;

  colormap = Xw_get_colormap_xid(MyExtendedColorMap) ;

  if( !colormap ) {
    PrintError() ;
  }

  return (colormap) ;
}

Standard_Boolean Xw_Window::PointerPosition (Standard_Integer& X, Standard_Integer& Y) const {
Standard_Boolean cstatus ;
int x,y ;

	cstatus = Xw_get_cursor_position(MyExtendedWindow,&x,&y) ;
	X = x ;
	Y = y ;

	return (cstatus);
}

Xw_TypeOfVisual Xw_Window::VisualClass () const {

	return (MyVisualClass);

}

Standard_Integer Xw_Window::VisualDepth () const {

	return (MyDepth);

}

Standard_Integer Xw_Window::VisualID( ) const {
Aspect_Handle *VisualInfo ;
Xw_TypeOfVisual VisualClass ;
int MaxColor,BasePixel,MaxUserColor,MaxDefineColor,FirstFreeColorIndex,visualid
;

        status = Xw_get_colormap_info(MyExtendedColorMap,&VisualInfo,
                &VisualClass,&visualid,&MaxColor,&BasePixel,
                &MaxUserColor,&MaxDefineColor,&FirstFreeColorIndex) ;

        return (visualid) ;
}

Xw_WindowQuality Xw_Window::Quality () const {

	return (MyQuality);

}

Standard_Boolean Xw_Window::PixelOfColor ( const Quantity_NameOfColor aColor, Standard_Integer &aPixel ) const {
Standard_Real Red,Green,Blue ;
unsigned long pixel ;
Standard_Integer isapproximate;
Quantity_Color color(aColor);

        color.Values(Red,Green,Blue,Quantity_TOC_RGB) ;

        status = Xw_get_color_pixel(MyExtendedColorMap,
                        (float)Red,(float)Green,(float)Blue,&pixel,&isapproximate) ;
        if( !status ) PrintError() ;

	aPixel = Standard_Integer(pixel);

	return (isapproximate) ? Standard_True : Standard_False;
}

Standard_Boolean Xw_Window::PixelOfColor ( const Quantity_Color &aColor, Standard_Integer &aPixel ) const {
Standard_Real Red,Green,Blue ;
unsigned long pixel ;
Standard_Integer isapproximate;

        aColor.Values(Red,Green,Blue,Quantity_TOC_RGB) ;

        status = Xw_get_color_pixel(MyExtendedColorMap,
                        (float)Red,(float)Green,(float)Blue,&pixel,&isapproximate) ;
        if( !status ) PrintError() ;

	aPixel = Standard_Integer(pixel);

	return (isapproximate) ? Standard_True : Standard_False;
}

Standard_Boolean Xw_Window::BackgroundPixel ( Standard_Integer &aPixel ) const {
unsigned long pixel ;

        status = Xw_get_background_pixel(MyExtendedWindow,&pixel) ;
        if( !status ) PrintError() ;

	aPixel = Standard_Integer(pixel);

	return status;
}


Standard_Address Xw_Window::ExtendedWindow () const {

	return (MyExtendedWindow);

}

Standard_Address Xw_Window::ExtendedColorMap () const {

	return (MyExtendedColorMap);

}

Standard_Address Xw_Window::ExtendedTypeMap () const {

	return (MyExtendedTypeMap);

}

Standard_Address Xw_Window::ExtendedWidthMap () const {

	return (MyExtendedWidthMap);

}

Standard_Address Xw_Window::ExtendedFontMap () const {

	return (MyExtendedFontMap);

}

Standard_Address Xw_Window::ExtendedMarkMap () const {

	return (MyExtendedMarkMap);

}
