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

void OpenGl_GraphicDriver::GraduatedTrihedronDisplay(const Graphic3d_CView& view,
                                                     const Graphic3d_CGraduatedTrihedron& cubic)
{
    call_togl_graduatedtrihedron_display((CALL_DEF_VIEW*)&view, cubic);
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