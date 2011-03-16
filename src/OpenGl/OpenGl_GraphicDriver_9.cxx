
// File   OpenGl_GraphicDriver_9.cxx
// Created  1/08/97
// Author       PCT
// Modified
//    16-09-98 ; BGN : Points d'entree du Triedre (S3819, Phase 1)
//    22-09-98 ; BGN : S3989 (anciennement S3819)
//                               TypeOfTriedron* from Aspect (pas Graphic3d)

//-Copyright  MatraDatavision 1997

//-Version  

//-Design Declaration des variables specifiques aux Drivers

//-Warning  Un driver encapsule les Pex et OpenGl drivers

//-References 

//-Language C++ 2.0

//-Declarations

// for the class

#define IMP131100 //GG_Enable to creates more than one texture
//      with the same name.

#define OCC1188         //SAV added methods to set background image

#include <OpenGl_GraphicDriver.jxx>

#include <Aspect_DriverDefinitionError.hxx>
#include <Quantity_NameOfColor.hxx>
#include <AlienImage.hxx>
#include <Standard_Integer.hxx>

#include <OpenGl_tgl_funcs.hxx>

#include <stdlib.h>
#include <stdio.h>


#include <Image_Image.hxx>


//-Aliases

//-Global data definitions

//-Methods, in order

// Helper function, returns the nearest power of two greater than the argument value
inline Standard_Integer GetNearestPow2(Standard_Integer theValue)
{
  // Precaution against overflow
  Standard_Integer aHalfMax = IntegerLast() >> 1, aRes = 1;
  if ( theValue > aHalfMax ) theValue = aHalfMax;
  while ( aRes < theValue ) aRes <<= 1;
  return aRes;
}

Standard_Integer OpenGl_GraphicDriver::CreateTexture(const Graphic3d_TypeOfTexture Type,const Handle_AlienImage_AlienImage &Image,const Standard_CString FileName,const Handle(TColStd_HArray1OfReal)& TexUpperBounds) const
{
  // transmutation de l'image en un bon tableau RGBA pour la toolkit des textures
  // lamentable d'un point de vue vitesse...
  Handle(Image_Image) MyPic = Image->ToImage();

  //TODO: The temporary fix in order to make AIS_TexturedShape work. To be improved in next release.
  Standard_Integer aGlWidth  = (Type == Graphic3d_TOT_2D_MIPMAP) ? MyPic->Width() : GetNearestPow2(MyPic->Width());
  Standard_Integer aGlHeight = (Type == Graphic3d_TOT_2D_MIPMAP) ? MyPic->Height() : GetNearestPow2(MyPic->Height());

  TexUpperBounds->SetValue(1, ((Standard_Real) (MyPic->Width())/((Standard_Real) aGlWidth)));
  TexUpperBounds->SetValue(2, ((Standard_Real) (MyPic->Height())/((Standard_Real) aGlHeight)));

  unsigned char *MyImageData = (unsigned char *)malloc(aGlWidth*aGlHeight*4);
  unsigned char *MyData = MyImageData;
  int TexId;
  int i,j;
  Quantity_Color MyColor;

  if (MyImageData == NULL)
    return -1;

#ifdef IMP131100

  static Standard_Integer textureRank=0;  
  char textureName[16];
  Standard_PCharacter fileName = textureName; 
  sprintf(fileName,"Tex%d",++textureRank);
#else
  Standard_CString fileName = FileName
#endif

    for (j = MyPic->Height() - 1; j >= 0; j--)
      for (i = 0; i < aGlWidth; i++){
        if (i < MyPic->Width()){
          MyColor = MyPic->PixelColor(i, j);
          *MyData++ = (int)(255 * MyColor.Red());
          *MyData++ = (int)(255 * MyColor.Green());
          *MyData++ = (int)(255 * MyColor.Blue());
        }
        else {
          *MyData++ = (int)(0);
          *MyData++ = (int)(0);
          *MyData++ = (int)(0);
        }
        *MyData++ = 0xFF;
      }

      // Padding the lower part of the texture with black
      for (j = aGlHeight - 1; j >= MyPic->Height(); j--)
        for (i = 0; i < aGlWidth; i++){
          *MyData++ = (int)(0);
          *MyData++ = (int)(0);
          *MyData++ = (int)(0);
          *MyData++ = 0xFF;
        }  


        switch (Type)
        {
        case Graphic3d_TOT_1D:
          TexId = call_togl_create_texture(0, aGlWidth, aGlHeight, MyImageData, (Standard_PCharacter)fileName);
          break;

        case Graphic3d_TOT_2D:
          TexId = call_togl_create_texture(1, aGlWidth, aGlHeight, MyImageData, (Standard_PCharacter)fileName);
          break;

        case Graphic3d_TOT_2D_MIPMAP:
          TexId = call_togl_create_texture(2, aGlWidth, aGlHeight, MyImageData, (Standard_PCharacter)fileName);
          break;

        default:
          TexId = -1;
        }

        free(MyImageData);
        return TexId;

}

void OpenGl_GraphicDriver::DestroyTexture(const Standard_Integer TexId) const
{
  call_togl_destroy_texture(TexId);
}


void OpenGl_GraphicDriver::ModifyTexture(const Standard_Integer TexId,const Graphic3d_CInitTexture& AValue) const
{
  call_togl_modify_texture(TexId, (CALL_DEF_INIT_TEXTURE *)&AValue);
}




void OpenGl_GraphicDriver::Environment(const Graphic3d_CView& ACView)
{
  call_togl_environment((CALL_DEF_VIEW *)&ACView);
}


//
// Triedron methods : the Triedron is a non-zoomable object.
//

void OpenGl_GraphicDriver::ZBufferTriedronSetup (
  const Quantity_NameOfColor XColor,
  const Quantity_NameOfColor YColor,
  const Quantity_NameOfColor ZColor,
  const Standard_Real        SizeRatio,
  const Standard_Real        AxisDiametr,
  const Standard_Integer     NbFacettes)
{
  float Xrgb[3];
  float Yrgb[3];
  float Zrgb[3];
  Standard_Real R,G,B;
  Quantity_Color(XColor).Values(R, G, B, Quantity_TOC_RGB);
  Xrgb[0] = float (R);
  Xrgb[1] = float (G);
  Xrgb[2] = float (B);
  Quantity_Color(YColor).Values(R, G, B, Quantity_TOC_RGB);
  Yrgb[0] = float (R);
  Yrgb[1] = float (G);
  Yrgb[2] = float (B);
  Quantity_Color(ZColor).Values(R, G, B, Quantity_TOC_RGB);
  Zrgb[0] = float (R);
  Zrgb[1] = float (G);
  Zrgb[2] = float (B);

  call_togl_ztriedron_setup(Xrgb, Yrgb, Zrgb, 
    (Standard_ShortReal)(SizeRatio), (Standard_ShortReal)(AxisDiametr), NbFacettes);
}

void OpenGl_GraphicDriver::TriedronDisplay (
  const Graphic3d_CView& ACView,
  const Aspect_TypeOfTriedronPosition APosition,
  const Quantity_NameOfColor AColor, 
  const Standard_Real AScale,
  const Standard_Boolean AsWireframe )
{
  Standard_Real R,G,B;
  float r,g,b;
  Quantity_Color Color(AColor);
  Color.Values(R,G,B,Quantity_TOC_RGB);
  r = float (R);
  g = float (G);
  b = float (B);

  call_togl_triedron_display((CALL_DEF_VIEW *)&ACView,(int)APosition, r,g,b, (float)AScale, AsWireframe);
}

void OpenGl_GraphicDriver::TriedronErase (const Graphic3d_CView& ACView) 
{
  call_togl_triedron_erase((CALL_DEF_VIEW *)&ACView);
}

void OpenGl_GraphicDriver::TriedronEcho (const Graphic3d_CView& ACView,const Aspect_TypeOfTriedronEcho AType )
{
  call_togl_triedron_echo( (CALL_DEF_VIEW *)&ACView,AType );
}

void OpenGl_GraphicDriver::BackgroundImage( const Standard_CString FileName, 
                                            const Graphic3d_CView& ACView,
                                            const Aspect_FillMethod FillStyle )
{
#ifdef OCC1188
  Graphic3d_CView MyCView = ACView;
  Standard_Integer width, height;
  Handle(Image_Image) image;

  Standard_Boolean result = AlienImage::LoadImageFile( FileName, image, width, height );

  if ( result ) {
    unsigned char *data = (unsigned char*)malloc( width * height * 3 );
    unsigned char *pdata = data;
    Standard_Integer i,j;
    Quantity_Color color;

    if ( data != NULL ) {
      for ( j = height - 1; j >= 0; j-- )
        for ( i = 0; i < width; i++ ) {
          color = image->PixelColor( i, j );
          *pdata++ = (int)( 255 * color.Red() );
          *pdata++ = (int)( 255 * color.Green() );
          *pdata++ = (int)( 255 * color.Blue() );
        }
        call_togl_create_bg_texture( (CALL_DEF_VIEW*)&MyCView, width, height, data, (int)FillStyle );
        // delete data here
        free( data );
    }
  }
#endif // OCC1188
}

void OpenGl_GraphicDriver::SetBgImageStyle( const Graphic3d_CView& ACView,
                                            const Aspect_FillMethod FillStyle )
{
#ifdef OCC1188
  Graphic3d_CView MyCView = ACView;
  call_togl_set_bg_texture_style( (CALL_DEF_VIEW*)&MyCView, (int)FillStyle );
#endif //OCC1188
}

void OpenGl_GraphicDriver::SetBgGradientStyle(const Graphic3d_CView& ACView,const Aspect_GradientFillMethod FillType)
{ 
  call_togl_set_gradient_type( ACView.WsId, FillType);
}

void OpenGl_GraphicDriver::GetGraduatedTrihedron(const Graphic3d_CView& view,
                                                 /* Names of axes */
                                                 Standard_CString& xname, 
                                                 Standard_CString& yname, 
                                                 Standard_CString& zname,
                                                 /* Draw names */
                                                 Standard_Boolean& xdrawname, 
                                                 Standard_Boolean& ydrawname, 
                                                 Standard_Boolean& zdrawname,
                                                 /* Draw values */
                                                 Standard_Boolean& xdrawvalues, 
                                                 Standard_Boolean& ydrawvalues, 
                                                 Standard_Boolean& zdrawvalues,
                                                 /* Draw grid */
                                                 Standard_Boolean& drawgrid,
                                                 /* Draw axes */
                                                 Standard_Boolean& drawaxes,
                                                 /* Number of splits along axes */
                                                 Standard_Integer& nbx, 
                                                 Standard_Integer& nby, 
                                                 Standard_Integer& nbz,
                                                 /* Offset for drawing values */
                                                 Standard_Integer& xoffset, 
                                                 Standard_Integer& yoffset, 
                                                 Standard_Integer& zoffset,
                                                 /* Offset for drawing names of axes */
                                                 Standard_Integer& xaxisoffset, 
                                                 Standard_Integer& yaxisoffset, 
                                                 Standard_Integer& zaxisoffset,
                                                 /* Draw tickmarks */
                                                 Standard_Boolean& xdrawtickmarks, 
                                                 Standard_Boolean& ydrawtickmarks, 
                                                 Standard_Boolean& zdrawtickmarks,
                                                 /* Length of tickmarks */
                                                 Standard_Integer& xtickmarklength, 
                                                 Standard_Integer& ytickmarklength, 
                                                 Standard_Integer& ztickmarklength,
                                                 /* Grid color */
                                                 Quantity_Color& gridcolor,
                                                 /* X name color */
                                                 Quantity_Color& xnamecolor,
                                                 /* Y name color */
                                                 Quantity_Color& ynamecolor,
                                                 /* Z name color */
                                                 Quantity_Color& znamecolor,
                                                 /* X color of axis and values */
                                                 Quantity_Color& xcolor,
                                                 /* Y color of axis and values */
                                                 Quantity_Color& ycolor,
                                                 /* Z color of axis and values */
                                                 Quantity_Color& zcolor,
                                                 /* Name of font for names of axes */
                                                 Standard_CString& fontOfNames,
                                                 /* Style of names of axes */
                                                 OSD_FontAspect& styleOfNames,
                                                 /* Size of names of axes */
                                                 Standard_Integer& sizeOfNames,
                                                 /* Name of font for values */
                                                 Standard_CString& fontOfValues,
                                                 /* Style of values */
                                                 OSD_FontAspect& styleOfValues,
                                                 /* Size of values */
                                                 Standard_Integer& sizeOfValues) const
{
    Graphic3d_CGraduatedTrihedron cubic;
    call_togl_graduatedtrihedron_get((CALL_DEF_VIEW*)&view, &cubic);

    /* Names of axes */
    xname = cubic.xname;
    yname = cubic.yname; 
    zname = cubic.zname;
    /* Draw names */
    xdrawname = cubic.xdrawname; 
    ydrawname = cubic.ydrawname; 
    zdrawname = cubic.zdrawname;
    /* Draw values */
    xdrawvalues = cubic.xdrawvalues; 
    ydrawvalues = cubic.ydrawvalues; 
    zdrawvalues = cubic.zdrawvalues;
    /* Draw grid */
    drawgrid = cubic.drawgrid;
    /* Draw axes */
    drawaxes = cubic.drawaxes;
    /* Number of splits along axes */
    nbx = cubic.nbx; 
    nby = cubic.nby; 
    nbz = cubic.nbz;
    /* Offset for drawing values */
    xoffset = cubic.xoffset; 
    yoffset = cubic.yoffset; 
    zoffset = cubic.zoffset;
    /* Offset for drawing names of axes */
    xaxisoffset = cubic.xaxisoffset; 
    yaxisoffset = cubic.yaxisoffset; 
    zaxisoffset = cubic.zaxisoffset;
    /* Draw tickmarks */
    xdrawtickmarks = cubic.xdrawtickmarks; 
    ydrawtickmarks = cubic.ydrawtickmarks; 
    zdrawtickmarks = cubic.zdrawtickmarks;
    /* Length of tickmarks */
    xtickmarklength = cubic.xtickmarklength; 
    ytickmarklength = cubic.ytickmarklength; 
    ztickmarklength = cubic.ztickmarklength;
    /* Grid color */
    gridcolor.SetValues(cubic.gridcolor[0], cubic.gridcolor[1], cubic.gridcolor[2], Quantity_TOC_RGB);
    /* X name color */
    xnamecolor.SetValues(cubic.xnamecolor[0], cubic.xnamecolor[1], cubic.xnamecolor[2], Quantity_TOC_RGB);
    /* Y name color */
    ynamecolor.SetValues(cubic.ynamecolor[0], cubic.ynamecolor[1], cubic.ynamecolor[2], Quantity_TOC_RGB);
    /* Z name color */
    znamecolor.SetValues(cubic.znamecolor[0], cubic.znamecolor[1], cubic.znamecolor[2], Quantity_TOC_RGB);
    /* X color of axis and values */
    xcolor.SetValues(cubic.xcolor[0], cubic.xcolor[1], cubic.xcolor[2], Quantity_TOC_RGB);
    /* Y color of axis and values */
    ycolor.SetValues(cubic.ycolor[0], cubic.ycolor[1], cubic.ycolor[2], Quantity_TOC_RGB);
    /* Z color of axis and values */
    zcolor.SetValues(cubic.zcolor[0], cubic.zcolor[1], cubic.zcolor[2], Quantity_TOC_RGB);
    /* Name of font for names of axes */
    fontOfNames = cubic.fontOfNames;
    /* Style of names of axes */
    styleOfNames = cubic.styleOfNames;
    /* Size of names of axes */
    sizeOfNames = cubic.sizeOfNames;
    /* Name of font for values */
    fontOfValues = cubic.fontOfValues;
    /* Style of values */
    styleOfValues = cubic.styleOfValues;
    /* Size of values */
    sizeOfValues = cubic.sizeOfValues;
    /* Name of font for names of axes */
    fontOfNames = cubic.fontOfNames;
    /* Style of names of axes */
    styleOfNames = cubic.styleOfNames;
    /* Size of names of axes */
    sizeOfNames = cubic.sizeOfNames;
    /* Name of font for values */
    fontOfValues = cubic.fontOfValues;
    /* Style of values */
    styleOfValues = cubic.styleOfValues;
    /* Size of values */
    sizeOfValues = cubic.sizeOfValues;
}

void OpenGl_GraphicDriver::GraduatedTrihedronDisplay(const Graphic3d_CView& view,
                                                     Graphic3d_CGraduatedTrihedron& cubic,
                                                     /* Names of axes */
                                                     const Standard_CString xname, 
                                                     const Standard_CString yname, 
                                                     const Standard_CString zname,
                                                     /* Draw names */
                                                     const Standard_Boolean xdrawname, 
                                                     const Standard_Boolean ydrawname, 
                                                     const Standard_Boolean zdrawname,
                                                     /* Draw values */
                                                     const Standard_Boolean xdrawvalues, 
                                                     const Standard_Boolean ydrawvalues, 
                                                     const Standard_Boolean zdrawvalues,
                                                     /* Draw grid */
                                                     const Standard_Boolean drawgrid,
                                                     /* Draw axes */
                                                     const Standard_Boolean drawaxes,
                                                     /* Number of splits along axes */
                                                     const Standard_Integer nbx, 
                                                     const Standard_Integer nby, 
                                                     const Standard_Integer nbz,
                                                     /* Offset for drawing values */
                                                     const Standard_Integer xoffset, 
                                                     const Standard_Integer yoffset, 
                                                     const Standard_Integer zoffset,
                                                     /* Offset for drawing names of axes */
                                                     const Standard_Integer xaxisoffset, 
                                                     const Standard_Integer yaxisoffset, 
                                                     const Standard_Integer zaxisoffset,
                                                     /* Draw tickmarks */
                                                     const Standard_Boolean xdrawtickmarks, 
                                                     const Standard_Boolean ydrawtickmarks, 
                                                     const Standard_Boolean zdrawtickmarks,
                                                     /* Length of tickmarks */
                                                     const Standard_Integer xtickmarklength, 
                                                     const Standard_Integer ytickmarklength, 
                                                     const Standard_Integer ztickmarklength,
                                                     /* Grid color */
                                                     const Quantity_Color& gridcolor,
                                                     /* X name color */
                                                     const Quantity_Color& xnamecolor,
                                                     /* Y name color */
                                                     const Quantity_Color& ynamecolor,
                                                     /* Z name color */
                                                     const Quantity_Color& znamecolor,
                                                     /* X color of axis and values */
                                                     const Quantity_Color& xcolor,
                                                     /* Y color of axis and values */
                                                     const Quantity_Color& ycolor,
                                                     /* Z color of axis and values */
                                                     const Quantity_Color& zcolor,
                                                     /* Name of font for names of axes */
                                                     const Standard_CString fontOfNames,
                                                     /* Style of names of axes */
                                                     const OSD_FontAspect styleOfNames,
                                                     /* Size of names of axes */
                                                     const Standard_Integer sizeOfNames,
                                                     /* Name of font for values */
                                                     const Standard_CString fontOfValues,
                                                     /* Style of values */
                                                     const OSD_FontAspect styleOfValues,
                                                     /* Size of values */
                                                     const Standard_Integer sizeOfValues)
{
    /* Names of axes */
    cubic.xname = (char*) xname;
    cubic.yname = (char*) yname; 
    cubic.zname = (char*) zname;
    /* Draw names */
    cubic.xdrawname = xdrawname; 
    cubic.ydrawname = ydrawname; 
    cubic.zdrawname = zdrawname;
    /* Draw values */
    cubic.xdrawvalues = xdrawvalues; 
    cubic.ydrawvalues = ydrawvalues; 
    cubic.zdrawvalues = zdrawvalues;
    /* Draw grid */
    cubic.drawgrid = drawgrid;
    /* Draw axes */
    cubic.drawaxes = drawaxes;
    /* Number of splits along axes */
    cubic.nbx = nbx; 
    cubic.nby = nby; 
    cubic.nbz = nbz;
    /* Offset for drawing values */
    cubic.xoffset = xoffset; 
    cubic.yoffset = yoffset; 
    cubic.zoffset = zoffset;
    /* Offset for drawing names of axes */
    cubic.xaxisoffset = xaxisoffset; 
    cubic.yaxisoffset = yaxisoffset; 
    cubic.zaxisoffset = zaxisoffset;
    /* Draw tickmarks */
    cubic.xdrawtickmarks = xdrawtickmarks; 
    cubic.ydrawtickmarks = ydrawtickmarks; 
    cubic.zdrawtickmarks = zdrawtickmarks;
    /* Length of tickmarks */
    cubic.xtickmarklength = xtickmarklength; 
    cubic.ytickmarklength = ytickmarklength; 
    cubic.ztickmarklength = ztickmarklength;
    /* Grid color */
    cubic.gridcolor[0] = (Standard_ShortReal) gridcolor.Red();
    cubic.gridcolor[1] = (Standard_ShortReal) gridcolor.Green();
    cubic.gridcolor[2] = (Standard_ShortReal) gridcolor.Blue();
    /* X name color */
    cubic.xnamecolor[0] = (Standard_ShortReal) xnamecolor.Red();
    cubic.xnamecolor[1] = (Standard_ShortReal) xnamecolor.Green();
    cubic.xnamecolor[2] = (Standard_ShortReal) xnamecolor.Blue();
    /* Y name color */
    cubic.ynamecolor[0] = (Standard_ShortReal) ynamecolor.Red();
    cubic.ynamecolor[1] = (Standard_ShortReal) ynamecolor.Green();
    cubic.ynamecolor[2] = (Standard_ShortReal) ynamecolor.Blue();
    /* Z name color */
    cubic.znamecolor[0] = (Standard_ShortReal) znamecolor.Red();
    cubic.znamecolor[1] = (Standard_ShortReal) znamecolor.Green();
    cubic.znamecolor[2] = (Standard_ShortReal) znamecolor.Blue();
    /* X color of axis and values */
    cubic.xcolor[0] = (Standard_ShortReal) xcolor.Red();
    cubic.xcolor[1] = (Standard_ShortReal) xcolor.Green();
    cubic.xcolor[2] = (Standard_ShortReal) xcolor.Blue();
    /* Y color of axis and values */
    cubic.ycolor[0] = (Standard_ShortReal) ycolor.Red();
    cubic.ycolor[1] = (Standard_ShortReal) ycolor.Green();
    cubic.ycolor[2] = (Standard_ShortReal) ycolor.Blue();
    /* Z color of axis and values */
    cubic.zcolor[0] = (Standard_ShortReal) zcolor.Red();
    cubic.zcolor[1] = (Standard_ShortReal) zcolor.Green();
    cubic.zcolor[2] = (Standard_ShortReal) zcolor.Blue();
    /* Name of font for names of axes */
    cubic.fontOfNames = (char*) fontOfNames;
    /* Style of names of axes */
    cubic.styleOfNames = styleOfNames;
    /* Size of names of axes */
    cubic.sizeOfNames = sizeOfNames;
    /* Name of font for values */
    cubic.fontOfValues = (char*) fontOfValues;
    /* Style of values */
    cubic.styleOfValues = styleOfValues;
    /* Size of values */
    cubic.sizeOfValues = sizeOfValues;
    /* Name of font for names of axes */
    cubic.fontOfNames = (char*) fontOfNames;
    /* Style of names of axes */
    cubic.styleOfNames = styleOfNames;
    /* Size of names of axes */
    cubic.sizeOfNames = sizeOfNames;
    /* Name of font for values */
    cubic.fontOfValues = (char*) fontOfValues;
    /* Style of values */
    cubic.styleOfValues = styleOfValues;
    /* Size of values */
    cubic.sizeOfValues = sizeOfValues;

    call_togl_graduatedtrihedron_display((CALL_DEF_VIEW*)&view, &cubic);
}

void OpenGl_GraphicDriver::GraduatedTrihedronErase(const Graphic3d_CView& view)
{
    call_togl_graduatedtrihedron_erase((CALL_DEF_VIEW*)&view);
}

void OpenGl_GraphicDriver::GraduatedTrihedronMinMaxValues(const Standard_ShortReal xmin,
                                                          const Standard_ShortReal ymin,
                                                          const Standard_ShortReal zmin,
                                                          const Standard_ShortReal xmax,
                                                          const Standard_ShortReal ymax,
                                                          const Standard_ShortReal zmax)
{
    call_togl_graduatedtrihedron_minmaxvalues(xmin, ymin, zmin, xmax, ymax, zmax);
}