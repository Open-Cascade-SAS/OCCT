/***********************************************************************

     FONCTION :
     ----------
        Classe Xw_Driver :

     HISTORIQUE DES MODIFICATIONS   :
     --------------------------------
 
      30-01-98 : GG ; SPEC_MFT
              L'utilisation des polices MFT devient parametrable a travers
              la methode Aspect_Driver::UseMFT(). 
              Le symbol CSF_USE_MFT n'est donc plus reconnu.

***********************************************************************/

#define PRO5676 //GG_020197
//              Donner la possibillite d'exprimer la taille
//              par rapport a la hauteur totale du caractere ou par
//              rapport a la hauteur "ascent" situee au dessus de la
//              ligne de base.
//              (CapsHeight flag)

#define CTS50024        //GG_090997
//              Attention avec les nouvelles machines SGI,
//              le pitch vertical n'est pas identique au pitch horizontal.

#define MFT     //GG_220497 Etude G1343
//              Utilisation du FontManager MFT permettant de traiter
//              les chaines de caracteres orientees et slantees.
//              

#define OPTIM1  //GG_180997
//              Optimisation du SKETCHER :
//              remplacer les appels Xw_get_window_position() par
//              Xw_get_window_size() qui est plus optimum.


#define PRO12934 //GG_060598
//              Eviter de "raiser" lorsqu'une erreur est detectee.
//

#define PRO17334 //GG_050199
//              Raiser dans DrawMarker() lorsque l'index est hors limites

#include <Xw_Driver.ixx>
#include <Aspect_ColorMapEntry.hxx>
#include <Aspect_WidthMapEntry.hxx>
#include <Aspect_TypeMapEntry.hxx>
#include <Aspect_LineStyle.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Aspect_FontMapEntry.hxx>
#include <Aspect_FontStyle.hxx>
#include <Aspect_MarkMapEntry.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <Aspect_MarkerStyle.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColQuantity_Array1OfLength.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <TShort_Array1OfShortReal.hxx>

// Routines C a declarer en extern
//extern "C" {
#include <Xw_Cextern.hxx>
//}
typedef struct { float r, g, b; } GRAPHIC2D_PIXEL;

//-Static data definitions

static XW_STATUS status ;

static Standard_ShortReal theSinAngle,theCosAngle;
#define TRANSFORM(X,Y) \
        { Standard_Real x = X,y = Y; \
          X = x*theCosAngle - y*theSinAngle; \
          Y = x*theSinAngle + y*theCosAngle; \
        }

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

void Xw_Driver::PrintError() {
Standard_CString ErrorMessag ;
Standard_Integer ErrorNumber ;
Standard_Integer ErrorGravity ;
 
        status = XW_SUCCESS ;
        ErrorMessag = Xw_get_error(&ErrorNumber,&ErrorGravity) ;
#ifdef PRO12934
        Xw_print_error() ;
#else
        if( ErrorGravity > 2 ) Aspect_DriverError::Raise (ErrorMessag) ;
        else Xw_print_error() ;
#endif

}

// Category: Constructors

Xw_Driver::Xw_Driver (const Handle(Xw_Window)& aWindow)
: Aspect_WindowDriver(aWindow) {

        MyColorIndexs.Nullify() ;
        MyTypeIndexs.Nullify() ;
        MyWidthIndexs.Nullify() ;
        MyFontIndexs.Nullify() ;
        MyMarkerIndexs.Nullify() ;

        MyVisualClass      = aWindow->VisualClass() ;
        MyExtendedDrawable = aWindow->ExtendedWindow() ;
        MyExtendedColorMap = aWindow->ExtendedColorMap() ;
        MyExtendedTypeMap  = aWindow->ExtendedTypeMap() ;
        MyExtendedWidthMap = aWindow->ExtendedWidthMap() ;
        MyExtendedFontMap  = aWindow->ExtendedFontMap() ;
        MyExtendedMarkMap  = aWindow->ExtendedMarkMap() ;
        MyImage            = NULL;
        MyDrawMode = Aspect_TODM_ERASE ;
        MyPrimitiveType = Aspect_TOP_UNKNOWN ;
        MyRetainBuffer  = 0 ;
#ifdef MFT
        MyTextManager = new Xw_TextManager(MyExtendedDrawable,MyExtendedWidthMap);
#endif
        SetDrawMode(Aspect_TODM_REPLACE) ;
}

// Category: Methods to modify the class definition

void Xw_Driver::BeginDraw ( const Standard_Boolean DBmode,
                            const Standard_Integer aRetainBuffer ) {

        status = Xw_set_double_buffer(MyExtendedDrawable,
                                        (XW_DOUBLEBUFFERMODE)DBmode) ;

        if( !status ) PrintError() ;

        status = Xw_set_buffer(MyExtendedDrawable,aRetainBuffer) ;
        if( status ) {
            MyRetainBuffer = aRetainBuffer ;
        } else {
            PrintError() ;
        }
}

void Xw_Driver::EndDraw (const Standard_Boolean Synchronize) {

        if( MyRetainBuffer != 0 ) {
          MyRetainBuffer =  0 ;
          status = Xw_set_buffer(MyExtendedDrawable,0) ;
                        // Reset attribs after filling buffer
          Aspect_TypeOfDrawMode mode = MyDrawMode; 
          MyDrawMode = Aspect_TypeOfDrawMode(mode + 1);
          SetDrawMode(mode);
        } else {
          status = Xw_flush (MyExtendedDrawable,Synchronize);
        }
        if( !status ) PrintError() ;
}

Standard_Boolean Xw_Driver::OpenBuffer (const Standard_Integer aRetainBuffer,
                                        const Standard_ShortReal aPivotX,
                                        const Standard_ShortReal aPivotY,
                                        const Standard_Integer WidthIndex,
                                        const Standard_Integer ColorIndex,
                                        const Standard_Integer FontIndex,
                                        const Aspect_TypeOfDrawMode DrawMode) {
Standard_Integer theWidth = 0 ;
Standard_Integer theColor = 0 ;
Standard_Integer theFont = 0 ;
Standard_ShortReal xpivot = aPivotX ;
Standard_ShortReal ypivot = aPivotY ;
XW_DRAWMODE theDrawMode = (XW_DRAWMODE)DrawMode;

        if( !MyWidthIndexs.IsNull() ) {
            if( WidthIndex > 0 && WidthIndex >= MyWidthIndexs->Lower() &&
                WidthIndex <= MyWidthIndexs->Upper() ) {
                theWidth = MyWidthIndexs->Value(WidthIndex) ;
            }
        }
        if( !MyColorIndexs.IsNull() ) {
            if( ColorIndex > 0 && ColorIndex >= MyColorIndexs->Lower() &&
                ColorIndex <= MyColorIndexs->Upper() ) {
                theColor = MyColorIndexs->Value(ColorIndex) ;
            }
        }
        if( !MyFontIndexs.IsNull() ) {
            if( FontIndex > 0 && FontIndex >= MyFontIndexs->Lower() &&
                FontIndex <= MyFontIndexs->Upper() ) {
                theFont = MyFontIndexs->Value(FontIndex) ;
            }
        }

        status = Xw_open_buffer(MyExtendedDrawable,aRetainBuffer,
                        xpivot,ypivot,theWidth,theColor,theFont,theDrawMode) ;

        if( !status ) PrintError() ;

        return status;
}

void Xw_Driver::CloseBuffer (const Standard_Integer aRetainBuffer) const {

        status = Xw_close_buffer(MyExtendedDrawable,aRetainBuffer) ;

        if( !status ) PrintError() ;
}

void Xw_Driver::ClearBuffer (const Standard_Integer aRetainBuffer) const {

        status = Xw_clear_buffer(MyExtendedDrawable,aRetainBuffer) ;

        if( !status ) PrintError() ;
}

void Xw_Driver::DrawBuffer (const Standard_Integer aRetainBuffer) const {

        status = Xw_draw_buffer(MyExtendedDrawable,aRetainBuffer) ;

        if( !status ) PrintError() ;
}

void Xw_Driver::EraseBuffer (const Standard_Integer aRetainBuffer) const {

        status = Xw_erase_buffer(MyExtendedDrawable,aRetainBuffer) ;

        if( !status ) PrintError() ;
}

void Xw_Driver::MoveBuffer (const Standard_Integer aRetainBuffer,
                                 const Standard_ShortReal aPivotX,
                                 const Standard_ShortReal aPivotY) const {
Standard_ShortReal xpivot = aPivotX ;
Standard_ShortReal ypivot = aPivotY ;

        status = Xw_move_buffer(MyExtendedDrawable,aRetainBuffer,
                                                        xpivot,ypivot) ;

        if( !status ) PrintError() ;
}

void Xw_Driver::ScaleBuffer (const Standard_Integer aRetainBuffer,
                                 const Quantity_Factor aScaleX,
                                 const Quantity_Factor aScaleY) const {
Standard_ShortReal scalex = aScaleX;
Standard_ShortReal scaley = aScaleY;

        status = Xw_scale_buffer(MyExtendedDrawable,aRetainBuffer,scalex,scaley) ;

        if( !status ) PrintError() ;
}

void Xw_Driver::RotateBuffer (const Standard_Integer aRetainBuffer,
                                 const Quantity_PlaneAngle anAngle) const {
Standard_ShortReal angle = anAngle;

        status = Xw_rotate_buffer(MyExtendedDrawable,aRetainBuffer,angle) ;

        if( !status ) PrintError() ;
}

Standard_Boolean Xw_Driver::BufferIsOpen (const Standard_Integer aRetainBuffer) const {
Standard_Integer isopen,isdrawn,isempty;

        status = Xw_get_buffer_status(MyExtendedDrawable,aRetainBuffer,
                                        &isopen,&isdrawn,&isempty) ;

        return (isopen != 0 ? Standard_True : Standard_False);
}

Standard_Boolean Xw_Driver::BufferIsEmpty (const Standard_Integer aRetainBuffer) const {
Standard_Integer isopen,isdrawn,isempty;

        status = Xw_get_buffer_status(MyExtendedDrawable,aRetainBuffer,
                                        &isopen,&isdrawn,&isempty) ;
        if( !status ) PrintError() ;

        return (isempty != 0 ? Standard_True : Standard_False);
}

Standard_Boolean Xw_Driver::BufferIsDrawn (const Standard_Integer aRetainBuffer) const {
Standard_Integer isopen,isdrawn,isempty;

        status = Xw_get_buffer_status(MyExtendedDrawable,aRetainBuffer,
                                        &isopen,&isdrawn,&isempty) ;
        if( !status ) PrintError() ;

        return (isdrawn != 0 ? Standard_True : Standard_False);
}

void Xw_Driver::SetDrawMode (const Aspect_TypeOfDrawMode aMode) {

    if( MyDrawMode != aMode ) {
        MyDrawMode = aMode;
        MyLineColor = -1 ;
        MyLineType = -1 ;
        MyLineWidth = -1 ;
        MyPolyColor = -1 ;
        MyPolyType = Standard_False ;
        MyPolyTile = 0 ;
        MyTextColor = -1 ;
        MyTextType = 0 ;
        MyTextFont = -1 ;
        MyTextHScale = 1. ;
        MyTextWScale = 1. ;
        MyTextSlant = 0. ;
        MyTextIsUnderlined = Standard_False;
        MyMarkerColor = -1 ;
        MyMarkerType = Standard_False ;
        MyMarkerWidth = -1 ;
    }
}

Aspect_TypeOfResize Xw_Driver::ResizeSpace () {
XW_RESIZETYPE mode = Xw_resize_window(MyExtendedDrawable) ;
   
        if ( Window()->HBackground() )
           status = Xw_update_background_image( MyExtendedDrawable );
        if( mode == XW_TOR_UNKNOWN ) PrintError() ;

        if( mode != XW_NO_BORDER ) {
          MyTextColor = -1 ;
        }

        return (Aspect_TypeOfResize(mode)) ;
}

// Category: Methods to define the attributes

void Xw_Driver::InitializeColorMap (const Handle(Aspect_ColorMap)& Colormap) {
Aspect_ColorMapEntry entry ;
Quantity_Color color ;
Standard_Integer index,iindex,oindex,minindex=IntegerLast(),maxindex= -minindex;
Standard_Real red,green,blue ;
int hindex ;

        if( !Xw_isdefine_colormap(MyExtendedColorMap) ) PrintError() ;

        for( index=1 ; index<=Colormap->Size() ; index++ ) {
            entry = Colormap->Entry(index) ;
            maxindex = Max(maxindex,entry.Index()) ;
            minindex = Min(minindex,entry.Index()) ;
        }

        MyColorIndexs = new TColStd_HArray1OfInteger(minindex,maxindex,-1) ;

        for( index=1 ; index<=Colormap->Size() ; index++ ) {
            entry = Colormap->Entry(index) ;
            iindex = entry.Index() ;
            color = entry.Color() ;
            color.Values(red,green,blue,Quantity_TOC_RGB) ;
            status = Xw_get_color_index(MyExtendedColorMap,(float)red,
                                                           (float)green,
                                                           (float)blue,
                                                           &hindex) ;
            oindex = hindex ;
            MyColorIndexs->SetValue(iindex,oindex) ;
        }
}

void Xw_Driver::InitializeTypeMap (const Handle(Aspect_TypeMap)& Typemap) {
Aspect_TypeMapEntry entry ;
Aspect_LineStyle style ;
Aspect_TypeOfLine type ;
Standard_Integer index,iindex,oindex,minindex=IntegerLast(),maxindex= -minindex;
Standard_Integer i ;
int hindex,length ;
float *vstyle ;

        if( !Xw_isdefine_typemap(MyExtendedTypeMap) ) PrintError() ;

        for( index=1 ; index<=Typemap->Size() ; index++ ) {
            entry = Typemap->Entry(index) ;
            maxindex = Max(maxindex,entry.Index()) ;
            minindex = Min(minindex,entry.Index()) ;
        }

        MyTypeIndexs = new TColStd_HArray1OfInteger(minindex,maxindex,-1) ;

        for( index=1 ; index<=Typemap->Size() ; index++ ) {
            entry = Typemap->Entry(index) ;
            iindex = entry.Index() ;
            style = entry.Type() ;
            type = style.Style() ;
            length = (int) style.Length() ;
            if( type != Aspect_TOL_SOLID ) {
                const TColQuantity_Array1OfLength& values(style.Values()) ;
                TShort_Array1OfShortReal 
                                svalues(values.Lower(),values.Length()) ;
                for( i=values.Lower() ; i<=values.Upper() ; i++ )
                        svalues(i) = TOMILLIMETER(Standard_ShortReal(values(i))) ;
                        
                vstyle = (float*) &svalues.Value(values.Lower()) ;
                status = Xw_get_type_index(MyExtendedTypeMap,vstyle,length,
                                                           &hindex) ;
                oindex = hindex ;
            } else oindex = 0 ;
            MyTypeIndexs->SetValue(iindex,oindex) ;
        }
}

void Xw_Driver::InitializeWidthMap (const Handle(Aspect_WidthMap)& Widthmap) {
Aspect_WidthMapEntry entry ;
Standard_Integer index,iindex,oindex,minindex=IntegerLast(),maxindex= -minindex;
int hindex ;
float width ;

        if( !Xw_isdefine_widthmap(MyExtendedWidthMap) ) PrintError() ;

        for( index=1 ; index<=Widthmap->Size() ; index++ ) {
            entry = Widthmap->Entry(index) ;
            maxindex = Max(maxindex,entry.Index()) ;
            minindex = Min(minindex,entry.Index()) ;
        }

        MyWidthIndexs = new TColStd_HArray1OfInteger(minindex,maxindex,-1) ;

        for( index=1 ; index<=Widthmap->Size() ; index++ ) {
            entry = Widthmap->Entry(index) ;
            iindex = entry.Index() ;
            width = (float) TOMILLIMETER(entry.Width()) ;
            status = Xw_get_width_index(MyExtendedWidthMap,width,&hindex) ;
            oindex = hindex ;
            MyWidthIndexs->SetValue(iindex,oindex) ;
        }
}

void Xw_Driver::InitializeFontMap (const Handle(Aspect_FontMap)& Fontmap) {
Aspect_FontMapEntry entry ;
Aspect_FontStyle style ;
Standard_Integer index,iindex,oindex,minindex=IntegerLast(),maxindex= -minindex;
int hindex ;
float size,slant ;
//char env[64];

        if( !Xw_isdefine_fontmap(MyExtendedFontMap) ) PrintError() ;

        for( index=1 ; index<=Fontmap->Size() ; index++ ) {
            entry = Fontmap->Entry(index) ;
            maxindex = Max(maxindex,entry.Index()) ;
            minindex = Min(minindex,entry.Index()) ;
        }

#ifdef MFT
        if( UseMFT() ) {
          MyMFTFonts = new Xw_HListOfMFTFonts(minindex,maxindex,NULL) ;
          MyMFTSizes = new TShort_HArray1OfShortReal(minindex,maxindex,0.) ;
        }
#endif

        MyFontIndexs = new TColStd_HArray1OfInteger(minindex,maxindex,-1) ;

        for( index=1 ; index<=Fontmap->Size() ; index++ ) {
            entry = Fontmap->Entry(index) ;
            iindex = entry.Index() ;
            style = entry.Type() ;
            size = (float) TOMILLIMETER(style.Size()) ;
            slant = style.Slant() ;
#ifdef PRO5676
            if( style.CapsHeight() ) size = -size;
#endif
#ifdef MFT
            if( UseMFT() ) {
              TCollection_AsciiString aname(style.AliasName());
              Standard_Boolean found = MFT_FontManager::IsKnown(aname.ToCString()); 
              if( !found ) {
                cout << " Xw_Driver::SetFontMap.cann't found the MFT fontname '"
                        << aname << "',using 'Defaultfont'" << endl;
                aname = "Defaultfont"; found = Standard_True; 
              } 
              if( found ) { 
                Handle(MFT_FontManager) theFontManager;
                Aspect_FontStyle fstyle = MFT_FontManager::Font(aname.ToCString());
                found = Standard_False;
                for( Standard_Integer i=minindex ; i<=maxindex ; i++ ) {
                  theFontManager = MyMFTFonts->Value(i);
                  if( !theFontManager.IsNull() ) {
                    if( fstyle == theFontManager->Font() ) {
                      found = Standard_True;
                      break;
                    }
                  }
                } 
                if( !found ) {
                  theFontManager = new MFT_FontManager(aname.ToCString());
                }
                Quantity_Length theFontSize = Abs(size);
                Quantity_PlaneAngle theFontSlant = slant;
                Standard_Boolean theCapsHeight = 
                        (size < 0.) ? Standard_True : Standard_False;
                theFontManager->SetFontAttribs(theFontSize,theFontSize,
                                theFontSlant,0.,theCapsHeight);
                MyFontIndexs->SetValue(iindex,-1) ;
                MyMFTFonts->SetValue(iindex,theFontManager);
                MyMFTSizes->SetValue(iindex,size);
                continue;
              }
            }
#endif
            TCollection_AsciiString aname(style.FullName()) ;
            status = Xw_get_font_index(MyExtendedFontMap,
				       size,
				       (Standard_PCharacter)aname.ToCString(),
				       &hindex) ;
            oindex = hindex ;
            MyFontIndexs->SetValue(iindex,oindex) ;
        }
}

void Xw_Driver::InitializeMarkMap (const Handle(Aspect_MarkMap)& Markmap) {
Aspect_MarkMapEntry entry ;
Aspect_MarkerStyle style ;
Aspect_TypeOfMarker type ;
Standard_Integer index,iindex,oindex,minindex=IntegerLast(),maxindex= -minindex;
int hindex,length ;

        if( !Xw_isdefine_markmap(MyExtendedMarkMap) ) PrintError() ;

        for( index=1 ; index<=Markmap->Size() ; index++ ) {
            entry = Markmap->Entry(index) ;
            maxindex = Max(maxindex,entry.Index()) ;
            minindex = Min(minindex,entry.Index()) ;
        }

        MyMarkerIndexs = new TColStd_HArray1OfInteger(minindex,maxindex,-1) ;

        for( index=1 ; index<=Markmap->Size() ; index++ ) {
            entry = Markmap->Entry(index) ;
            iindex = entry.Index() ;
            style = entry.Style() ;
            type = style.Type() ;
            length = style.Length() ;
            const TColStd_Array1OfBoolean& spoint(style.SValues());
            const TShort_Array1OfShortReal& xpoint(style.XValues());
            const TShort_Array1OfShortReal& ypoint(style.YValues());
            int *S = (int*) &spoint.Value(spoint.Lower());
            float *X = (float*) &xpoint.Value(xpoint.Lower());
            float *Y = (float*) &ypoint.Value(ypoint.Lower());
            status = Xw_get_marker_index(MyExtendedMarkMap,length,
                                                       S,X,Y,&hindex) ;
            oindex = hindex ;
            MyMarkerIndexs->SetValue(iindex,oindex) ;
        }
}

void Xw_Driver::SetLineAttrib (const Standard_Integer ColorIndex, const Standard_Integer TypeIndex, const Standard_Integer WidthIndex) {
Standard_Boolean setattrib = Standard_False;

    if( MyLineColor != ColorIndex ) {
        setattrib = Standard_True;
        if( !MyColorIndexs.IsNull() ) {
            if( ColorIndex < MyColorIndexs->Lower() ||
                ColorIndex > MyColorIndexs->Upper() ) {
                MyLineColor = MyColorIndexs->Lower();
                Aspect_DriverError::Raise ("Bad Color Index") ;
            } else MyLineColor = ColorIndex;
        } else MyLineColor = -1;
    }
    if( MyLineType != TypeIndex ) {
        setattrib = Standard_True;
        if( !MyTypeIndexs.IsNull() ) { 
            if( TypeIndex < MyTypeIndexs->Lower() ||
                TypeIndex > MyTypeIndexs->Upper() ) {
                MyLineType = MyTypeIndexs->Lower();
                Aspect_DriverError::Raise ("Bad Type Index") ;
            } else MyLineType = TypeIndex;
        } else MyLineType = -1;
    }
    if( MyLineWidth != WidthIndex ) {
        setattrib = Standard_True;
        if( !MyWidthIndexs.IsNull() ) { 
            if( WidthIndex < MyWidthIndexs->Lower() ||
                WidthIndex > MyWidthIndexs->Upper() ) {
                MyLineWidth = MyWidthIndexs->Lower();
                Aspect_DriverError::Raise ("Bad Width Index") ;
            } else MyLineWidth = WidthIndex;
        } else MyLineWidth = -1;
    }

    if( setattrib ) {
        Standard_Integer color ;
        Standard_Integer type = (MyLineType < 0) ? 0 :
                                        MyTypeIndexs->Value(MyLineType) ;
        Standard_Integer width = (MyLineWidth < 0) ? 0 :
                                        MyWidthIndexs->Value(MyLineWidth) ;

        if( MyLineColor > 0 ) {
          color = MyColorIndexs->Value(MyLineColor) ;
        } else {
          status = Xw_get_background_index(MyExtendedDrawable,&color) ; 
        }
        XW_ATTRIB attrib = Xw_set_line_attrib (MyExtendedDrawable,color,type,width,
                                        XW_DRAWMODE (MyDrawMode));
        if( !attrib ) PrintError() ;
    }

}

void Xw_Driver::SetPolyAttrib (const Standard_Integer ColorIndex, const Standard_Integer TileIndex, const Standard_Boolean DrawEdge) {
Standard_Boolean setattrib = Standard_False;

    if( MyPolyColor != ColorIndex ) {
        setattrib = Standard_True;
        if( !MyColorIndexs.IsNull() ) {
            if( (TileIndex >= 0) && (ColorIndex < MyColorIndexs->Lower() ||
                ColorIndex > MyColorIndexs->Upper()) ) {
                MyPolyColor = MyColorIndexs->Lower();
                Aspect_DriverError::Raise ("Bad Color Index") ;
            } else MyPolyColor = ColorIndex;
        } else MyPolyColor = -1;
    }
    if( MyPolyTile != TileIndex ) {
        setattrib = Standard_True;
        MyPolyTile = TileIndex;
    }
    if( MyPolyType != (int ) DrawEdge ) {
        setattrib = Standard_True;
        MyPolyType = DrawEdge;
    }

    if( setattrib ) {
        Standard_Integer color ;
        Standard_Integer type = MyPolyType ;
        Standard_Integer tile = MyPolyTile ;

        if( MyPolyColor > 0 ) {
          color = MyColorIndexs->Value(MyPolyColor) ;
        } else {
          status = Xw_get_background_index(MyExtendedDrawable,&color) ; 
        }
        XW_ATTRIB attrib = Xw_set_poly_attrib (MyExtendedDrawable,color,type,tile,
                                        XW_DRAWMODE (MyDrawMode));
        if( !attrib ) PrintError() ;
    }

}

void Xw_Driver::SetTextAttrib (const Standard_Integer ColorIndex, const Standard_Integer FontIndex) {
Standard_Boolean setattrib = Standard_False;

    if( MyTextColor != ColorIndex ) {
        setattrib = Standard_True;
        if( !MyColorIndexs.IsNull() ) {
            if( ColorIndex < MyColorIndexs->Lower() ||
                ColorIndex > MyColorIndexs->Upper() ) {
                MyTextColor = MyColorIndexs->Lower();
                Aspect_DriverError::Raise ("Bad Color Index") ;
            } else MyTextColor = ColorIndex;
        } else MyTextColor = -1;
    }
    if( MyTextFont != FontIndex ) {
        setattrib = Standard_True;
        if( !MyFontIndexs.IsNull() ) {
            if( FontIndex < MyFontIndexs->Lower() ||
                FontIndex > MyFontIndexs->Upper() ) {
                MyTextFont = MyFontIndexs->Lower();
                Aspect_DriverError::Raise ("Bad Font Index") ;
            } else MyTextFont = FontIndex;
        } else MyTextFont = -1;
    }
    if( MyTextType != 0 ) {
        setattrib = Standard_True;
        MyTextType = 0;
    }

    if( MyTextHScale != 1. || MyTextWScale != 1. ) {
        setattrib = Standard_True;
        MyTextHScale = MyTextWScale = 1.;
    }

    if( setattrib ) {
        Standard_Integer color ;
        Standard_Integer type = MyTextType;
        Standard_Integer font = (MyTextFont < 0) ? 0 :
                                        MyFontIndexs->Value(MyTextFont) ;

        if( MyTextColor > 0 ) {
          color = MyColorIndexs->Value(MyTextColor) ;
        } else {
          status = Xw_get_background_index(MyExtendedDrawable,&color) ; 
        }
#ifdef MFT
        if( font < 0 ) {
          Handle(MFT_FontManager) theFontManager = 
                                        MyMFTFonts->Value(MyTextFont);
          Quantity_Length theTextSize = MyMFTSizes->Value(MyTextFont);
          MyTextWSize = MyTextHSize = Abs(theTextSize);
          Quantity_PlaneAngle theTextSlant = 0.;
          Quantity_Factor thePrecision = 0.;    //Set default
          Standard_Boolean theCapsHeight = 
                        (theTextSize > 0.) ? Standard_False : Standard_True;
          theFontManager->SetFontAttribs(MyTextWSize,MyTextHSize,
                                theTextSlant,thePrecision,theCapsHeight);
          return;
        }
#endif
        XW_ATTRIB attrib = Xw_set_text_attrib (MyExtendedDrawable,
                                color,type,font,XW_DRAWMODE (MyDrawMode));

        if( !attrib ) PrintError() ;
    }

}

void Xw_Driver::SetTextAttrib (const Standard_Integer ColorIndex, const Standard_Integer FontIndex, const Quantity_PlaneAngle aSlant, const Quantity_Factor aHScale, const Quantity_Factor aWScale,const Standard_Boolean isUnderlined) {
Standard_Boolean setattrib = Standard_False;

    if( MyTextColor != ColorIndex ) {
        setattrib = Standard_True;
        if( !MyColorIndexs.IsNull() ) {
            if( ColorIndex < MyColorIndexs->Lower() ||
                ColorIndex > MyColorIndexs->Upper() ) {
                MyTextColor = MyColorIndexs->Lower();
                Aspect_DriverError::Raise ("Bad Color Index") ;
            } else MyTextColor = ColorIndex;
        } else MyTextColor = -1;
    }
    if( MyTextFont != FontIndex ) {
        setattrib = Standard_True;
        if( !MyFontIndexs.IsNull() ) {
            if( FontIndex < MyFontIndexs->Lower() ||
                FontIndex > MyFontIndexs->Upper() ) {
                MyTextFont = MyFontIndexs->Lower();
                Aspect_DriverError::Raise ("Bad Font Index") ;
            } else MyTextFont = FontIndex;
        } else MyTextFont = -1;
    }
    if( MyTextType != 1 || (MyTextSlant != aSlant) || 
        (MyTextHScale != aHScale) || (MyTextWScale != aWScale) ) {
        setattrib = Standard_True;
        MyTextType = 1;
        MyTextSlant = aSlant;
        MyTextHScale = aHScale;
        MyTextWScale = (aWScale > 0.) ? aWScale : aHScale;
    }

    if( MyTextIsUnderlined != isUnderlined ) {
        setattrib = Standard_True;
        MyTextIsUnderlined = isUnderlined;
    }

    if( setattrib ) {
        Standard_Integer color ;
        Standard_Integer font = (MyTextFont < 0) ? 0 :
                                        MyFontIndexs->Value(MyTextFont) ;
        XW_EXTENDEDTEXT attributes ;
        attributes.type = XW_EXTENDEDTEXT_UNDERLINE;
        attributes.xscale = MyTextWScale;
        attributes.yscale = MyTextHScale;
        attributes.slant = MyTextSlant;
        attributes.width = attributes.height = 0.;
        attributes.spacing = 0.;
        attributes.isUnderlined = MyTextIsUnderlined;
        if( MyTextSlant != 0. ) attributes.type |= XW_EXTENDEDTEXT_SLANT;
        if( MyTextWScale  > 0. ) { 
          attributes.type |= XW_EXTENDEDTEXT_XSCALE;
        }
        if( MyTextHScale  > 0. ) { 
          attributes.type |= XW_EXTENDEDTEXT_YSCALE;
        }
        if( MyTextColor > 0 ) {
          color = MyColorIndexs->Value(MyTextColor) ;
        } else {
          status = Xw_get_background_index(MyExtendedDrawable,&color) ; 
        }
#ifdef MFT
        if( font < 0 ) {
          Handle(MFT_FontManager) theFontManager = 
                                        MyMFTFonts->Value(MyTextFont);
          Quantity_Length theTextSize = MyMFTSizes->Value(MyTextFont);
          MyTextWSize = MyTextWScale * Abs(theTextSize);
          MyTextHSize = MyTextHScale * Abs(theTextSize);
          Quantity_PlaneAngle theTextSlant = MyTextSlant;
          Quantity_Factor thePrecision = 0.;    //Set default
          Standard_Boolean theCapsHeight = 
                        (theTextSize > 0.) ? Standard_False : Standard_True;
          theFontManager->SetFontAttribs(MyTextWSize,MyTextHSize,
                                theTextSlant,thePrecision,theCapsHeight);
          return;
        }
#endif
        XW_ATTRIB attrib = Xw_set_extended_text_attrib (MyExtendedDrawable,
                        color,font,XW_DRAWMODE (MyDrawMode),&attributes);

        if( !attrib ) PrintError() ;
    }
}
 
void Xw_Driver::SetMarkerAttrib (const Standard_Integer ColorIndex, const Standard_Integer WidthIndex, const Standard_Boolean FillMarker) {
Standard_Boolean setattrib = Standard_False;

    if( MyMarkerColor != ColorIndex ) {
        setattrib = Standard_True;
        if( !MyColorIndexs.IsNull() ) {
            if( ColorIndex < MyColorIndexs->Lower() ||
                ColorIndex > MyColorIndexs->Upper() ) {
                MyMarkerColor = MyColorIndexs->Lower();
                Aspect_DriverError::Raise ("Bad Color Index") ;
            } else MyMarkerColor = ColorIndex;
        } else MyMarkerColor = -1;
    }
    if( MyMarkerWidth != WidthIndex ) {
        setattrib = Standard_True;
        if( !MyWidthIndexs.IsNull() ) { 
            if( WidthIndex < MyWidthIndexs->Lower() ||
                WidthIndex > MyWidthIndexs->Upper() ) {
                MyMarkerWidth = MyWidthIndexs->Lower();
                Aspect_DriverError::Raise ("Bad Width Index") ;
            } else MyMarkerWidth = WidthIndex;
        } else MyMarkerWidth = -1;
    }
    if( MyMarkerType != FillMarker ) {
        setattrib = Standard_True;
        MyMarkerType = FillMarker;
    }

    if( setattrib ) {
        Standard_Integer color ;
        Standard_Integer type = MyMarkerType;
        Standard_Integer width = (MyMarkerWidth < 0) ? 0 :
                                        MyWidthIndexs->Value(MyMarkerWidth) ;

        if( MyMarkerColor > 0 ) {
          color = MyColorIndexs->Value(MyMarkerColor) ;
        } else {
          status = Xw_get_background_index(MyExtendedDrawable,&color) ; 
        }
        XW_ATTRIB attrib = Xw_set_marker_attrib (MyExtendedDrawable,color,type,width,
                                        XW_DRAWMODE (MyDrawMode));

        if( !attrib ) PrintError() ;
    }
}

// Category: Methods to manage images

Standard_Boolean Xw_Driver::IsKnownImage(const Handle(Standard_Transient)& anImage) {

        MyImage = Xw_get_image_handle( MyExtendedDrawable, 
                                (void*)(anImage->HashCode(IntegerLast())) ) ;

        if( MyImage ) return (Standard_True) ;
        else return Standard_False;
}

Standard_Boolean Xw_Driver::SizeOfImageFile(const Standard_CString anImageFile,
                                            Standard_Integer &aWidth,
                                            Standard_Integer &aHeight) const {
Standard_Integer hashcode = ::HashCode(anImageFile) ;
XW_EXT_IMAGEDATA *pimage;
int width,height,depth;
float zoom;

        status = XW_ERROR;
        pimage = Xw_get_image_handle( MyExtendedDrawable,(void*)hashcode ) ;
        if( !pimage ) {
            pimage = Xw_load_image(MyExtendedDrawable,
				   (void*)hashcode,
				   (Standard_PCharacter)anImageFile );
        }

        if( pimage ) {
          status = Xw_get_image_info(pimage,&zoom,&width,&height,&depth);
          aWidth = width;
          aHeight = height;
        }

        return status;
}

void Xw_Driver::ClearImage (const Handle(Standard_Transient)& anImage) {

        MyImage = Xw_get_image_handle( MyExtendedDrawable, 
                                (void*) (anImage->HashCode(IntegerLast())) ) ;

        if( MyImage ) {
            status = Xw_close_image (MyImage);
            MyImage = NULL;
        }
}

void Xw_Driver::ClearImageFile (const Standard_CString anImageFile) {
Standard_Integer hashcode = ::HashCode(anImageFile) ;
XW_EXT_IMAGEDATA *pimage;

        pimage = Xw_get_image_handle( MyExtendedDrawable,(void*)hashcode ) ;

        if( pimage ) {
            status = Xw_close_image (pimage);
        }
}

void Xw_Driver::DrawImage (const Handle(Standard_Transient)& anImage, const Standard_ShortReal aX, const Standard_ShortReal aY) {

        MyImage = Xw_get_image_handle( MyExtendedDrawable, 
                                (void*) (anImage->HashCode(IntegerLast())) ) ;

        if( MyImage ) {
            status = Xw_draw_image (MyExtendedDrawable, MyImage, aX, aY);
        }
}

void Xw_Driver::DrawImageFile (const Standard_CString anImageFile,
                               const Standard_ShortReal aX,
                               const Standard_ShortReal aY,
                               const Quantity_Factor aScale)
{
  XW_EXT_IMAGEDATA *pimage;
  Standard_Integer hashcode = ::HashCode(anImageFile);

  pimage = Xw_get_image_handle( MyExtendedDrawable,(void*)hashcode );
  if (!pimage) {
    pimage = Xw_load_image(MyExtendedDrawable,
                           (void*)(hashcode),
			   (Standard_PCharacter)anImageFile );
  }

  if (pimage && aScale > 0.) {
    float zoom = aScale;
    status = Xw_zoom_image (pimage,zoom);
  }

  if (status == XW_SUCCESS && pimage) {
    status = Xw_draw_image (MyExtendedDrawable, pimage, aX, aY);
  } else if (pimage) {
    status = Xw_draw_zoomed_image (MyExtendedDrawable, pimage, aX, aY, aScale);
    if (status != XW_SUCCESS)
      PrintError();
  } else PrintError ();
}

void Xw_Driver::FillAndDrawImage (const Handle(Standard_Transient)& anImage, const Standard_ShortReal aX, const Standard_ShortReal aY, const Standard_Integer aWidth, const Standard_Integer aHeight, const Standard_Address anArrayOfPixels) {
float oldr, oldg, oldb;
int oldi = 0, oldj = 0, oldindex;
int num;

// Image 614x614 :
// temps CPU avec Standard_Integer < 20 secondes
// temps CPU avec register         < 19 secondes
Standard_Integer i, j;

GRAPHIC2D_PIXEL *pixels;
GRAPHIC2D_PIXEL pixel;

        if ((aWidth > 0) && (aHeight > 0)) {
            if( !MyImage ) {
                MyImage = Xw_open_image (MyExtendedDrawable,
                        (void*) (anImage->HashCode(IntegerLast())),
                                int (aWidth),int (aHeight));
            }
            if( !MyImage ) {
                PrintError() ;
            } else {
                switch (MyVisualClass) {
                   case (Xw_TOV_STATICGRAY) :
                   break;
                   case (Xw_TOV_GRAYSCALE) :
                   break;
                   case (Xw_TOV_STATICCOLOR) :
                   break;
                   case (Xw_TOV_PSEUDOCOLOR) :
                        pixels  = (GRAPHIC2D_PIXEL *) anArrayOfPixels;

                        for (i=0; i<aHeight; i++) {
                        oldr = oldg = oldb = -1.0;
                        oldindex = -1;
                        num = 0;

                        for (j=0; j<aWidth; j++) {
                           pixel = *(pixels+aWidth*i+j);
                           if ((pixel.r == oldr) && (pixel.g == oldg)
                                && (pixel.b == oldb)) num++;
                           else {
                                if (num) {
                                   status = Xw_get_color_index (
                                                MyExtendedColorMap,
                                                oldr, oldg, oldb, &oldindex);
                                   if (status) status = Xw_put_pixel (MyImage,
                                                oldi, oldj, oldindex, num);
                                }
                                num     = 1;
                                oldr    = pixel.r;
                                oldg    = pixel.g;
                                oldb    = pixel.b;
                                oldi    = int (i);
                                oldj    = int (j);
                           }
                        }

                        // Fin de ligne
                        status = Xw_get_color_index (MyExtendedColorMap,
                                                oldr, oldg, oldb, &oldindex);
                        if (status)
                                status = Xw_put_pixel (MyImage,
                                                oldi, oldj, oldindex, num);

                        }
                        this->DrawImage (anImage, aX, aY);
                   break;
                   case (Xw_TOV_TRUECOLOR) :
                        pixels  = (GRAPHIC2D_PIXEL *) anArrayOfPixels;

                        for (i=0; i<aHeight; i++) {
                        oldr = oldg = oldb = -1.0;
                        num = 0;

                        for (j=0; j<aWidth; j++) {
                           pixel = *(pixels+aWidth*i+j);
                           if ((pixel.r == oldr) && (pixel.g == oldg)
                                && (pixel.b == oldb)) num++;
                           else {
                                if (num) {
                                   status = Xw_put_rgbpixel (MyImage,
                                        oldi, oldj, oldr, oldg, oldb, num);
                                }
                                num     = 1;
                                oldr    = pixel.r;
                                oldg    = pixel.g;
                                oldb    = pixel.b;
                                oldi    = int (i);
                                oldj    = int (j);
                           }
                        }

                        // Fin de ligne
                        status = Xw_put_rgbpixel (MyImage,
                                        oldi, oldj, oldr, oldg, oldb, num);
                        }
                        this->DrawImage (anImage, aX, aY);
                   break;
                   case (Xw_TOV_DIRECTCOLOR) :
                   case (Xw_TOV_DEFAULT) :
                   case (Xw_TOV_PREFERRED_TRUECOLOR) :
                   case (Xw_TOV_PREFERRED_PSEUDOCOLOR) :
                   case (Xw_TOV_PREFERRED_OVERLAY) :
                   case (Xw_TOV_OVERLAY) :
                        // Cas impossible, mais plus de warning C++ :
                        // switch ( Xw_TypeOfVisual ) with  6 cases
                        //                                      ( 9 enumerators)
                   break;
                }

            }
        }
}

void Xw_Driver::FillAndDrawImage (const Handle(Standard_Transient)& anImage, const Standard_ShortReal aX, const Standard_ShortReal aY, const Standard_Integer anIndexOfLine, const Standard_Integer aWidth, const Standard_Integer aHeight, const Standard_Address anArrayOfPixels) {
float oldr, oldg, oldb;
int oldi = 0, oldj = 0, oldindex;
int num;

// Image 614x614 :
// temps CPU avec Standard_Integer < 21 secondes
// temps CPU avec register         < 20 secondes
Standard_Integer j;

GRAPHIC2D_PIXEL *pixels;
GRAPHIC2D_PIXEL pixel;

        if ((anIndexOfLine < 0) || (anIndexOfLine >= aHeight))
                Aspect_DriverError::Raise ("Bad Index in Xw_Driver::FillImage");

        // Debut d'image
        if ((aWidth > 0) && (aHeight > 0) && (anIndexOfLine == 0)) {
            if( !MyImage ) {    
                MyImage = Xw_open_image (MyExtendedDrawable,
                        (void*) (anImage->HashCode(IntegerLast())),
                                int (aWidth),int (aHeight));
            }
        }
        if ((aWidth > 0) && (aHeight > 0) && (anIndexOfLine >= 0)) {
            if( !MyImage ) {
                PrintError() ;
            }
            else {
                switch (MyVisualClass) {
                   case (Xw_TOV_STATICGRAY) :
                   break;
                   case (Xw_TOV_GRAYSCALE) :
                   break;
                   case (Xw_TOV_STATICCOLOR) :
                   break;
                   case (Xw_TOV_PSEUDOCOLOR) :
                        pixels  = (GRAPHIC2D_PIXEL *) anArrayOfPixels;

                        oldr = oldg = oldb = -1.0;
                        oldi = int (anIndexOfLine);
                        oldindex = -1;
                        num = 0;

                        for (j=0; j<aWidth; j++) {
                           pixel = *(pixels+j);
                           if ((pixel.r == oldr) && (pixel.g == oldg)
                                && (pixel.b == oldb)) num++;
                           else {
                                if (num) {
                                   status = Xw_get_color_index (MyExtendedColorMap,
                                                oldr, oldg, oldb, &oldindex);
                                   if (status)
                                        status = Xw_put_pixel (MyImage,
                                                oldi, oldj, oldindex, num);
                                }
                                num     = 1;
                                oldr    = pixel.r;
                                oldg    = pixel.g;
                                oldb    = pixel.b;
                                oldj    = int (j);
                           }
                        }

                        // Fin de ligne
                        status = Xw_get_color_index (MyExtendedColorMap,
                                                oldr, oldg, oldb, &oldindex);
                        if (status)
                                status = Xw_put_pixel (MyImage,
                                                oldi, oldj, oldindex, num);

                        // Fin d'image
                        if (anIndexOfLine == aHeight - 1)
                                this->DrawImage (anImage, aX, aY);
                   break;
                   case (Xw_TOV_TRUECOLOR) :
                        pixels  = (GRAPHIC2D_PIXEL *) anArrayOfPixels;

                        oldr = oldg = oldb = -1.0;
                        oldi = int (anIndexOfLine);
                        num = 0;

                        for (j=0; j<aWidth; j++) {
                           pixel = *(pixels+j);
                           if ((pixel.r == oldr) && (pixel.g == oldg)
                                && (pixel.b == oldb)) num++;
                           else {
                                if (num) {
                                   status = Xw_put_rgbpixel (MyImage,
                                        oldi, oldj, oldr, oldg, oldb, num);
                                }
                                num     = 1;
                                oldr    = pixel.r;
                                oldg    = pixel.g;
                                oldb    = pixel.b;
                                oldj    = int (j);
                           }
                        }

                        // Fin de ligne
                        status = Xw_put_rgbpixel (MyImage,
                                        oldi, oldj, oldr, oldg, oldb, num);

                        // Fin d'image
                        if (anIndexOfLine == aHeight - 1)
                                this->DrawImage (anImage, aX, aY);
                   break;
                   case (Xw_TOV_DIRECTCOLOR) :
                   case (Xw_TOV_DEFAULT) :
                   case (Xw_TOV_PREFERRED_PSEUDOCOLOR) :
                   case (Xw_TOV_PREFERRED_TRUECOLOR) :
                   case (Xw_TOV_PREFERRED_OVERLAY) :
                   case (Xw_TOV_OVERLAY) :
                        // Cas impossible, mais plus de warning C++ :
                        // switch ( Xw_TypeOfVisual ) with  6 cases
                        //                                      ( 9 enumerators)
                   break;
                }

            }
        }
}

// Category: Methods to draw primitives

void Xw_Driver::DrawPolygon (const TShort_Array1OfShortReal& aListX, const TShort_Array1OfShortReal& aListY) {
int npoint = (int)aListX.Length () ;
float *X = (float*) &aListX.Value(aListX.Lower()) ;
float *Y = (float*) &aListY.Value(aListY.Lower()) ;

        if( npoint != (int)aListY.Length () )
                Aspect_DriverError::Raise ("2 differents lengths!") ;

        if( npoint > 0 ) {
            status = Xw_draw_poly(MyExtendedDrawable,npoint,X,Y) ;
            if( !status ) PrintError() ;
        }
}

void Xw_Driver::DrawPolyline (const TShort_Array1OfShortReal& aListX, const TShort_Array1OfShortReal& aListY) {
int npoint = (int)aListX.Length () ;
float *X = (float*) &aListX.Value(aListX.Lower()) ;
float *Y = (float*) &aListY.Value(aListY.Lower()) ;

        if( npoint != (int)aListY.Length () )
                Aspect_DriverError::Raise ("2 differents lengths!") ;

        if( npoint > 0 ) {
            status = Xw_draw_line(MyExtendedDrawable,npoint,X,Y) ;
            if( !status ) PrintError() ; 
        }
}

void Xw_Driver::DrawSegment (const Standard_ShortReal X1, const Standard_ShortReal Y1, const Standard_ShortReal X2, const Standard_ShortReal Y2) {

        status = Xw_draw_segment (MyExtendedDrawable, X1, Y1, X2, Y2);

        if( !status ) PrintError() ;

}

void Xw_Driver::DrawText (const TCollection_ExtendedString& aText, const Standard_ShortReal Xpos, const Standard_ShortReal Ypos, const Standard_ShortReal Angle , const Aspect_TypeOfText aType) {

        if( aText.Length() > 0 ) {
#ifdef MFT
          Standard_Integer font = (MyTextFont < 0) ? 0 :
                                        MyFontIndexs->Value(MyTextFont) ;
          if( font < 0 ) { 
            Handle(MFT_FontManager) theFontManager = 
                                        MyMFTFonts->Value(MyTextFont);
            if( !aText.IsAscii() && !theFontManager->IsComposite() ) {
              cout << "*ERROR*Xw_Driver::DrawText.UNABLE to draw an extended text with an ANSI font" << endl;
              return;
            }
            Standard_Real underlinePosition =
                (MyTextIsUnderlined) ? theFontManager->UnderlinePosition() : 0.;
            Standard_Integer theTextColor;
            if( MyTextColor > 0 ) {
              theTextColor = MyColorIndexs->Value(MyTextColor) ;
            } else {
              Xw_get_background_index(MyExtendedDrawable,&theTextColor) ; 
            }           
            Aspect_TypeOfText theTextType = aType;
            MyTextManager->SetTextAttribs(theTextColor,theTextType,underlinePosition);
            theFontManager->DrawText(MyTextManager,aText.ToExtString(),
                        Quantity_Length(Xpos),Quantity_Length(Ypos),
                                                Quantity_PlaneAngle(Angle));
          } else
#endif
          if( aText.IsAscii() ) {
            TCollection_AsciiString atext(aText,'?');
            status = Xw_draw_text (MyExtendedDrawable, 
				   Xpos, 
				   Ypos,
				   (Standard_PCharacter)atext.ToCString (), 
				   Angle, 
				   (int)aType);
          } else status = XW_ERROR ;    /*Must be Implemented more later*/

          if( !status ) PrintError() ;
        }
}

void Xw_Driver::DrawText (const Standard_CString aText, const Standard_ShortReal Xpos, const Standard_ShortReal Ypos, const Standard_ShortReal Angle , const Aspect_TypeOfText aType) {

        if( strlen(aText) > 0 ) {
          TCollection_ExtendedString etext(aText);
          this->DrawText(etext,Xpos,Ypos,Angle,aType);
        }
}

void Xw_Driver::DrawPolyText (const TCollection_ExtendedString& aText, const Standard_ShortReal Xpos, const Standard_ShortReal Ypos, const Quantity_Factor aMarge, const Standard_ShortReal Angle , const Aspect_TypeOfText aType) {
Standard_ShortReal marge = aMarge ;

        if( aText.Length() > 0 ) {
#ifdef MFT
          Standard_Integer font = (MyTextFont < 0) ? 0 :
                                        MyFontIndexs->Value(MyTextFont) ;
          if( font < 0 ) {
            Handle(MFT_FontManager) theFontManager =
                                        MyMFTFonts->Value(MyTextFont);
            if( !aText.IsAscii() && !theFontManager->IsComposite() ) {
              cout << "*ERROR*Xw_Driver::DrawPolyText.UNABLE to draw an extended text with an ANSI font" << endl;
              return;
            }
            Standard_Real underlinePosition =
                (MyTextIsUnderlined) ? theFontManager->UnderlinePosition() : 0.;
            Standard_ShortReal width,height,xoffset,yoffset,marge;
            this->TextSize(aText,width,height,xoffset,yoffset,MyTextFont) ;
            if( underlinePosition > yoffset ) yoffset = underlinePosition;
            marge = height * aMarge;
            xoffset -= marge;
            yoffset += marge;
            width += 2.*marge;
            height += 2.*marge;
            if( MyPolyTile < 0 ) {
              this->DrawRectangle(Xpos,Ypos,Angle,xoffset,yoffset,width,height);
            } else {
              this->FillRectangle(Xpos,Ypos,Angle,xoffset,yoffset,width,height);
            }
            Standard_Integer theTextColor;
            if( (MyTextColor > 0) && ((MyTextColor != MyPolyColor) ||
                                                (MyPolyTile < 0)) ) {
              theTextColor = MyColorIndexs->Value(MyTextColor) ;
            } else {
              Xw_get_background_index(MyExtendedDrawable,&theTextColor) ; 
            }
            Aspect_TypeOfText theTextType = aType;
            MyTextManager->SetTextAttribs(theTextColor,theTextType,underlinePosition);
            theFontManager->DrawText(MyTextManager,aText.ToExtString(),
                        Quantity_Length(Xpos),Quantity_Length(Ypos),
                                                Quantity_PlaneAngle(Angle));
          } else
#endif
          if( aText.IsAscii() ) {
            TCollection_AsciiString atext(aText,'?');
            status = Xw_draw_polytext (MyExtendedDrawable, 
				       Xpos, 
				       Ypos, 
				       (Standard_PCharacter)atext.ToCString (),
				       Angle, 
				       marge, 
				       (int)aType);
          } else status = XW_ERROR ;    /*Must be Implemented more later*/

          if( !status ) PrintError() ;
        }
}

void Xw_Driver::DrawPolyText (const Standard_CString aText, const Standard_ShortReal Xpos, const Standard_ShortReal Ypos, const Quantity_Factor aMarge, const Standard_ShortReal Angle , const Aspect_TypeOfText aType) {
//Standard_ShortReal marge = aMarge ;

        if( strlen(aText) > 0 ) {
          TCollection_ExtendedString etext(aText);
          DrawPolyText(etext,Xpos,Ypos,aMarge,Angle,aType);
        }
}

void Xw_Driver::DrawMarker (const Standard_Integer aMarker, const Standard_ShortReal Xpos, const Standard_ShortReal Ypos, const Standard_ShortReal Width, const Standard_ShortReal Height, const Standard_ShortReal Angle ) {
int marker = -1;
float width = TOMILLIMETER(Width);
float height = TOMILLIMETER(Height);

        if( !MyMarkerIndexs.IsNull() ) {
          if( aMarker >= MyMarkerIndexs->Lower()
                                && aMarker <= MyMarkerIndexs->Upper() ) {
            marker = MyMarkerIndexs->Value(aMarker);
          }
        }

#ifdef PRO17334
        if( aMarker < 0 ) 
                Aspect_DriverError::Raise ("Bad Marker Index") ;
        if( aMarker > 0 ) {
#else
        if( aMarker >= 0 ) {
#endif
          status = Xw_draw_marker (MyExtendedDrawable, marker, Xpos, Ypos,
                                                width, height, Angle) ;
        } else {
          status = Xw_draw_point (MyExtendedDrawable, Xpos, Ypos) ;
        }

        if( !status ) PrintError() ;
}

Standard_Boolean Xw_Driver::DrawArc (const Standard_ShortReal Xpos, const Standard_ShortReal Ypos, const Standard_ShortReal aXradius, const Standard_ShortReal aYradius, const Standard_ShortReal aStartAngle, const Standard_ShortReal aOpenAngle) {

        status = Xw_draw_arc(MyExtendedDrawable, Xpos, Ypos, 
                                aXradius, aYradius, aStartAngle, aOpenAngle);
        if( !status ) PrintError() ;

        return status;
}

Standard_Boolean Xw_Driver::DrawPolyArc (const Standard_ShortReal Xpos, const Standard_ShortReal Ypos, const Standard_ShortReal aXradius, const Standard_ShortReal aYradius, const Standard_ShortReal aStartAngle, const Standard_ShortReal aOpenAngle) {

        status = Xw_draw_polyarc(MyExtendedDrawable, Xpos, Ypos, 
                                aXradius, aYradius, aStartAngle, aOpenAngle);
        if( !status ) PrintError() ;

        return status;
}

void Xw_Driver::BeginPolyline (const Standard_Integer aNumber) {

        if( MyPrimitiveType != Aspect_TOP_UNKNOWN ) ClosePrimitive();

        if( aNumber < 0 ) return;

        status = Xw_begin_line (MyExtendedDrawable, aNumber) ;
        if( status ) {
            MyPrimitiveType = Aspect_TOP_POLYLINE;
        } else {
            PrintError() ;
        }
}

void Xw_Driver::BeginPolygon (const Standard_Integer aNumber) {

        if( MyPrimitiveType != Aspect_TOP_UNKNOWN ) ClosePrimitive();

        if( aNumber < 0 ) return;

        status = Xw_begin_poly (MyExtendedDrawable, aNumber, 0) ;
        if( status ) {
            MyPrimitiveType = Aspect_TOP_POLYGON;
        } else {
            PrintError() ;
        }
}

void Xw_Driver::BeginSegments () {

        if( MyPrimitiveType != Aspect_TOP_UNKNOWN ) ClosePrimitive();

        status = Xw_begin_segments (MyExtendedDrawable, 0) ;
        if( status ) {
            MyPrimitiveType = Aspect_TOP_SEGMENTS;
        } else {
            PrintError() ;
        }
}

void Xw_Driver::BeginArcs () {

        if( MyPrimitiveType != Aspect_TOP_UNKNOWN ) ClosePrimitive();

        status = Xw_begin_arcs (MyExtendedDrawable, 0) ;
        if( status ) {
            MyPrimitiveType = Aspect_TOP_ARCS;
        } else {
            PrintError() ;
        }
}

void Xw_Driver::BeginPolyArcs () {

        if( MyPrimitiveType != Aspect_TOP_UNKNOWN ) ClosePrimitive();

        status = Xw_begin_polyarcs (MyExtendedDrawable, 0) ;
        if( status ) {
            MyPrimitiveType = Aspect_TOP_POLYARCS;
        } else {
            PrintError() ;
        }
}

void Xw_Driver::BeginMarkers () {

        if( MyPrimitiveType != Aspect_TOP_UNKNOWN ) ClosePrimitive();

        status = Xw_begin_markers (MyExtendedDrawable, 0) ;
        if( status ) {
            MyPrimitiveType = Aspect_TOP_MARKERS;
        } else {
            PrintError() ;
        }
}

void Xw_Driver::BeginPoints () {

        if( MyPrimitiveType != Aspect_TOP_UNKNOWN ) ClosePrimitive();

        status = Xw_begin_points (MyExtendedDrawable, 0) ;
        if( status ) {
            MyPrimitiveType = Aspect_TOP_POINTS;
        } else {
            PrintError() ;
        }
}

void Xw_Driver::DrawPoint (const Standard_ShortReal X, const Standard_ShortReal Y) {
        switch( MyPrimitiveType ) {
          case Aspect_TOP_POLYLINE:
            status = Xw_line_point (MyExtendedDrawable, X, Y) ;
            break;
          case Aspect_TOP_POLYGON:
            status = Xw_poly_point (MyExtendedDrawable, X, Y) ;
            break;
          default:
            status = Xw_draw_point (MyExtendedDrawable, X, Y) ;
        }

        if( !status ) {
            PrintError() ;
        }
}

void Xw_Driver::ClosePrimitive () {

        switch( MyPrimitiveType ) {
          case Aspect_TOP_POLYLINE:
            status = Xw_close_line (MyExtendedDrawable) ;
            break;
          case Aspect_TOP_POLYGON:
            status = Xw_close_poly (MyExtendedDrawable) ;
            break;
          case Aspect_TOP_SEGMENTS:
            status = Xw_close_segments (MyExtendedDrawable) ;
            break;
          case Aspect_TOP_ARCS:
            status = Xw_close_arcs (MyExtendedDrawable) ;
            break;
          case Aspect_TOP_POLYARCS:
            status = Xw_close_polyarcs (MyExtendedDrawable) ;
            break;
          case Aspect_TOP_MARKERS:
            status = Xw_close_markers (MyExtendedDrawable) ;
            break;
          case Aspect_TOP_POINTS:
            status = Xw_close_points (MyExtendedDrawable) ;
            break;
          default:
            MyPrimitiveType = Aspect_TOP_UNKNOWN;
        }
        MyPrimitiveType = Aspect_TOP_UNKNOWN;
        if( !status ) PrintError() ;
}

void Xw_Driver::WorkSpace ( Quantity_Length &Width, Quantity_Length &Height ) const {

#ifdef OPTIM1
int width,height ;
        status = Xw_get_window_size(MyExtendedDrawable,&width,&height) ;

        if( !status ) {
            Xw_print_error() ;
        }
#else
XW_WINDOWSTATE state ;
int xc,yc,width,height ;

        state = Xw_get_window_position(MyExtendedDrawable,
                                                &xc,&yc,&width,&height) ;

        if( state == XW_WS_UNKNOWN ) {
            Xw_print_error() ;
        }
#endif

#ifdef CTS50024
        float x,y ;
        status = Xw_get_window_pixelcoord(MyExtendedDrawable,
                                                width,0,&x,&y) ;
        if( !status ) {
            PrintError() ;
        }

        Width = Quantity_Length(x); Height = Quantity_Length(y);
#else
        Width = Xw_get_window_pixelvalue(MyExtendedDrawable,width) ;
        Height = Xw_get_window_pixelvalue(MyExtendedDrawable,height) ;
#endif
}

Quantity_Length Xw_Driver::Convert (const Standard_Integer Pv) const {
Standard_ShortReal Dv ;

        Dv = Xw_get_window_pixelvalue(MyExtendedDrawable,(int)Pv) ;

        return Quantity_Length(Dv) ;
}

void Xw_Driver::Convert (const Standard_Integer Px, const Standard_Integer Py, Quantity_Length &Dx, Quantity_Length &Dy ) const {
float x,y ;

        status = Xw_get_window_pixelcoord(MyExtendedDrawable,
                                                (int)Px,(int)Py,&x,&y) ;

        if( !status ) {
            PrintError() ;
        }

        Dx = Quantity_Length(x) ; Dy = Quantity_Length(y) ;
}

Standard_Integer Xw_Driver::Convert (const Quantity_Length Dv) const {
Standard_Integer Pv ;

        Pv = Xw_get_pixel_windowvalue(MyExtendedDrawable,(float)Dv) ;

        return (Pv) ;
}

void Xw_Driver::Convert (const Quantity_Length Dx, const Quantity_Length Dy, Standard_Integer &Px, Standard_Integer &Py ) const {
int x,y ;

        status = Xw_get_pixel_windowcoord(MyExtendedDrawable,
                                                (float)Dx,(float)Dy,&x,&y) ;

        if( !status ) {
            PrintError() ;
        }

        Px = x ; Py = y ;
}

void Xw_Driver::TextSize (const TCollection_ExtendedString& aText, Standard_ShortReal& Width, Standard_ShortReal& Height, const Standard_Integer aFontIndex ) const {
Standard_ShortReal xoffset,yoffset;

        this->TextSize(aText,Width,Height,xoffset,yoffset,aFontIndex) ;
}

void Xw_Driver::TextSize (const TCollection_ExtendedString& aText, Standard_ShortReal& Width, Standard_ShortReal& Height, Standard_ShortReal& Xoffset, Standard_ShortReal& Yoffset, const Standard_Integer aFontIndex ) const {

Standard_Integer theFont = (aFontIndex >= 0 ) ? aFontIndex : MyTextFont ;

        if( !MyFontIndexs.IsNull() ) {
            if( theFont < MyFontIndexs->Lower() ||
                theFont > MyFontIndexs->Upper() ) {
                Aspect_DriverError::Raise ("Bad Font Index") ;
            }
        }
        Standard_Integer font = (theFont < 0) ? 0 :
                                        MyFontIndexs->Value(theFont) ;
#ifdef MFT
        if( font < 0 ) {
          Handle(MFT_FontManager) theFontManager = 
                                        MyMFTFonts->Value(theFont);
          Quantity_Length theWidth = 0.,theAscent = 0.,theLbearing = 0.,theDescent = 0.;
          if( aText.IsAscii() ) {
              TCollection_AsciiString atext(aText,'?');
              theFontManager->TextSize(atext.ToCString(),
                                theWidth,theAscent,theLbearing,theDescent);
          } else if( theFontManager->IsComposite() ) {
              theFontManager->TextSize(aText.ToExtString(),
                                theWidth,theAscent,theLbearing,theDescent);
          } else {
// cout << "*ERROR*Xw_Driver::TextSize(...).Try to gets extended text size with an ANSI font" << endl;
          }
          if( MyTextIsUnderlined ) {
            Quantity_Length theUnderlinePosition = 
                        theFontManager->UnderlinePosition();
            theDescent = Max(theDescent,theUnderlinePosition);
          }
          Width = theWidth;
          Height = theAscent + theDescent;
          Xoffset = theLbearing;
          Yoffset = theDescent;
          return;
        }
#endif
        if (aText.IsAscii ()) {
          TCollection_AsciiString atext(aText,'?');
          status = Xw_get_text_size (MyExtendedDrawable, 
				     font, 
				     (Standard_PCharacter)atext.ToCString(), 
				     &Width, 
				     &Height, 
				     &Xoffset, 
				     &Yoffset) ;
          if( !status ) PrintError() ;
        } else {                /*Must be Implemented more later*/
          Width = Height = Xoffset = Yoffset = 0.;
        }
}

void Xw_Driver::TextSize (const Standard_CString aText, Standard_ShortReal& Width, Standard_ShortReal& Height, Standard_ShortReal& Xoffset, Standard_ShortReal& Yoffset, const Standard_Integer aFontIndex) const {
//Standard_Integer theFont = (aFontIndex >= 0 ) ? aFontIndex : MyTextFont ;

        TCollection_ExtendedString etext(aText);
        this->TextSize(etext,Width,Height,Xoffset,Yoffset,aFontIndex);
}

Standard_CString Xw_Driver::FontSize (Quantity_PlaneAngle &aSlant, Standard_ShortReal &aSize, Standard_ShortReal &aBheight, const Standard_Integer aFontIndex) const {
Standard_Integer theFont = (aFontIndex >= 0 ) ? aFontIndex : MyTextFont ;
Standard_CString fstring;
float size,bheight;

        if( !MyFontIndexs.IsNull() ) {
            if( theFont < MyFontIndexs->Lower() ||
                theFont > MyFontIndexs->Upper() ) {
                return NULL ;
            }
        }
        Standard_Integer font = (theFont < 0) ? 0 :
                                        MyFontIndexs->Value(theFont) ;
#ifdef MFT
        if( font < 0 ) {
          Handle(MFT_FontManager) theFontManager = 
                                        MyMFTFonts->Value(theFont);
          Quantity_Length theWidth,theHeight,theDescent;
          Quantity_PlaneAngle theSlant;
          Quantity_Factor thePrecision;
          Standard_Boolean theCapsHeight;
          fstring = theFontManager->FontAttribs(
            theWidth,theHeight,theDescent,theSlant,thePrecision,theCapsHeight);
          aSize = theHeight;
          aBheight = theDescent;
          aSlant = theSlant;
          return fstring;
        }
#endif
        fstring = Xw_get_font (MyExtendedFontMap,font,&size,&bheight);
        if( !fstring ) PrintError() ;

        aSlant = 0.;    // To be implemented
        aSize = FROMMILLIMETER(size);
        aBheight = FROMMILLIMETER(bheight);
        return fstring;

}

void Xw_Driver::AngleOfBuffer(const Standard_Integer aRetainBuffer,Quantity_PlaneAngle &anAngle) const {
Standard_ShortReal xpivot,ypivot,xscale,yscale,angle ;

        status = Xw_get_buffer_info(MyExtendedDrawable,aRetainBuffer,
                                &xpivot,&ypivot,&xscale,&yscale,&angle) ;

        anAngle = angle ;
}

void Xw_Driver::ScaleOfBuffer(const Standard_Integer aRetainBuffer,Quantity_Factor &aScaleX,Quantity_Factor &aScaleY) const {
Standard_ShortReal xpivot,ypivot,xscale,yscale,angle ;

        status = Xw_get_buffer_info(MyExtendedDrawable,aRetainBuffer,
                                &xpivot,&ypivot,&xscale,&yscale,&angle) ;

        aScaleX = xscale ; 
        aScaleY = yscale ; 
}

void Xw_Driver::PositionOfBuffer(const Standard_Integer aRetainBuffer,Standard_ShortReal &aPivotX,Standard_ShortReal &aPivotY) const {
Standard_ShortReal xpivot,ypivot,xscale,yscale,angle ;

        status = Xw_get_buffer_info(MyExtendedDrawable,aRetainBuffer,
                                &xpivot,&ypivot,&xscale,&yscale,&angle) ;

        aPivotX = xpivot ; 
        aPivotY = ypivot ; 
}

void Xw_Driver::ColorBoundIndexs(Standard_Integer &aMinIndex, Standard_Integer &aMaxIndex) const {

        if( MyColorIndexs.IsNull() ) {
          aMinIndex = aMaxIndex = 0;
        } else {        
          aMinIndex = MyColorIndexs->Lower() ;
          aMaxIndex = MyColorIndexs->Upper() ;
        }
}

Standard_Integer Xw_Driver::LocalColorIndex(const Standard_Integer anIndex) const {

        if( (anIndex < MyColorIndexs->Lower()) || 
                (anIndex > MyColorIndexs->Upper()) ) return (-1);

        return MyColorIndexs->Value(anIndex);
}

void Xw_Driver::FontBoundIndexs(Standard_Integer &aMinIndex, Standard_Integer &aMaxIndex) const {

        if( MyFontIndexs.IsNull() ) {
          aMinIndex = aMaxIndex = 0;
        } else {        
          aMinIndex = MyFontIndexs->Lower() ;
          aMaxIndex = MyFontIndexs->Upper() ;
        }
}

Standard_Integer Xw_Driver::LocalFontIndex(const Standard_Integer anIndex) const {

        if( (anIndex < MyFontIndexs->Lower()) || 
                (anIndex > MyFontIndexs->Upper()) ) return (-1);

        return MyFontIndexs->Value(anIndex);
}

void Xw_Driver::TypeBoundIndexs(Standard_Integer &aMinIndex, Standard_Integer &aMaxIndex) const {

        if( MyTypeIndexs.IsNull() ) {
          aMinIndex = aMaxIndex = 0;
        } else {        
          aMinIndex = MyTypeIndexs->Lower() ;
          aMaxIndex = MyTypeIndexs->Upper() ;
        }
}

Standard_Integer Xw_Driver::LocalTypeIndex(const Standard_Integer anIndex) const {

        if( (anIndex < MyTypeIndexs->Lower()) || 
                (anIndex > MyTypeIndexs->Upper()) ) return (-1);

        return MyTypeIndexs->Value(anIndex);
}

void Xw_Driver::WidthBoundIndexs(Standard_Integer &aMinIndex, Standard_Integer &aMaxIndex) const {

        if( MyWidthIndexs.IsNull() ) {
          aMinIndex = aMaxIndex = 0;
        } else {        
          aMinIndex = MyWidthIndexs->Lower() ;
          aMaxIndex = MyWidthIndexs->Upper() ;
        }
}

Standard_Integer Xw_Driver::LocalWidthIndex(const Standard_Integer anIndex) const {

        if( (anIndex < MyWidthIndexs->Lower()) || 
                (anIndex > MyWidthIndexs->Upper()) ) return (-1);

        return MyWidthIndexs->Value(anIndex);
}

void Xw_Driver::MarkBoundIndexs(Standard_Integer &aMinIndex, Standard_Integer &aMaxIndex) const {

        if( MyMarkerIndexs.IsNull() ) {
          aMinIndex = aMaxIndex = 0;
        } else {        
          aMinIndex = MyMarkerIndexs->Lower() ;
          aMaxIndex = MyMarkerIndexs->Upper() ;
        }
}

Standard_Integer Xw_Driver::LocalMarkIndex(const Standard_Integer anIndex) const {

        if( (anIndex < MyMarkerIndexs->Lower()) || 
                (anIndex > MyMarkerIndexs->Upper()) ) return (-1);

        return MyMarkerIndexs->Value(anIndex);
}

void Xw_Driver::DrawRectangle(const Standard_ShortReal Xpos, const Standard_ShortReal Ypos, const Standard_ShortReal Angle, const Standard_ShortReal Xoffset, const Standard_ShortReal Yoffset, const Standard_ShortReal Width, const Standard_ShortReal Height) const {
Standard_ShortReal xpoly,ypoly;

    theSinAngle = Sin(Angle);
    theCosAngle = Cos(Angle);
    Xw_begin_line(MyExtendedDrawable,5);
    xpoly = Xoffset;
    ypoly = -Yoffset;
    TRANSFORM(xpoly,ypoly);
    Xw_line_point(MyExtendedDrawable,Xpos + xpoly, Ypos + ypoly);
    xpoly = Xoffset + Width;
    ypoly = -Yoffset;
    TRANSFORM(xpoly,ypoly);
    Xw_line_point(MyExtendedDrawable,Xpos + xpoly, Ypos + ypoly);
    xpoly = Xoffset + Width;
    ypoly = Height - Yoffset;
    TRANSFORM(xpoly,ypoly);
    Xw_line_point(MyExtendedDrawable,Xpos + xpoly, Ypos + ypoly);
    xpoly = Xoffset;
    ypoly = Height - Yoffset;
    TRANSFORM(xpoly,ypoly);
    Xw_line_point(MyExtendedDrawable,Xpos + xpoly, Ypos + ypoly);
    xpoly = Xoffset;
    ypoly = -Yoffset;
    TRANSFORM(xpoly,ypoly);
    Xw_line_point(MyExtendedDrawable,Xpos + xpoly, Ypos + ypoly);
    Xw_close_line(MyExtendedDrawable);
}

void Xw_Driver::FillRectangle(const Standard_ShortReal Xpos, const Standard_ShortReal Ypos, const Standard_ShortReal Angle, const Standard_ShortReal Xoffset, const Standard_ShortReal Yoffset, const Standard_ShortReal Width, const Standard_ShortReal Height) const {
Standard_ShortReal xpoly,ypoly;

    theSinAngle = Sin(Angle);
    theCosAngle = Cos(Angle);
    Xw_begin_poly(MyExtendedDrawable,4,0);
    xpoly = Xoffset;
    ypoly = -Yoffset;
    TRANSFORM(xpoly,ypoly);
    Xw_poly_point(MyExtendedDrawable,Xpos + xpoly, Ypos + ypoly);
    xpoly = Xoffset + Width;
    ypoly = -Yoffset;
    TRANSFORM(xpoly,ypoly);
    Xw_poly_point(MyExtendedDrawable,Xpos + xpoly, Ypos + ypoly);
    xpoly = Xoffset + Width;
    ypoly = Height - Yoffset;
    TRANSFORM(xpoly,ypoly);
    Xw_poly_point(MyExtendedDrawable,Xpos + xpoly, Ypos + ypoly);
    xpoly = Xoffset;
    ypoly = Height - Yoffset;
    TRANSFORM(xpoly,ypoly);
    Xw_poly_point(MyExtendedDrawable,Xpos + xpoly, Ypos + ypoly);
    Xw_close_poly(MyExtendedDrawable);
}
