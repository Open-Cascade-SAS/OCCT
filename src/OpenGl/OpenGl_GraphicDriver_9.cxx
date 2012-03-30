// Created on: 2011-10-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
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


#include <OpenGl_GraphicDriver.hxx>
#include <OpenGl_CView.hxx>
#include <OpenGl_Trihedron.hxx>
#include <OpenGl_GraduatedTrihedron.hxx>
#include <OpenGl_TextureBox.hxx>
#include <OpenGl_tgl_funcs.hxx>

#include <Quantity_NameOfColor.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <AlienImage_AlienImage.hxx>
#include <Image_Image.hxx>

void OpenGl_GraphicDriver::Environment(const Graphic3d_CView& ACView)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
  {
    aCView->View->SetTextureEnv(ACView.Context.TexEnvId);
    aCView->View->SetSurfaceDetail((Visual3d_TypeOfSurfaceDetail)ACView.Context.SurfaceDetail);
  }
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
  OpenGl_Trihedron::Setup(XColor,YColor,ZColor,SizeRatio,AxisDiametr,NbFacettes);
}

void OpenGl_GraphicDriver::TriedronDisplay (
  const Graphic3d_CView& ACView,
  const Aspect_TypeOfTriedronPosition APosition,
  const Quantity_NameOfColor AColor, 
  const Standard_Real AScale,
  const Standard_Boolean AsWireframe )
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
  {
    aCView->View->TriedronDisplay(APosition, AColor, AScale, AsWireframe);
  }
}

void OpenGl_GraphicDriver::TriedronErase (const Graphic3d_CView& ACView) 
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
  {
    aCView->View->TriedronErase();
  }
}

void OpenGl_GraphicDriver::TriedronEcho (const Graphic3d_CView& ACView,const Aspect_TypeOfTriedronEcho AType )
{
  // Do nothing
}

void OpenGl_GraphicDriver::BackgroundImage( const Standard_CString FileName, 
                                           const Graphic3d_CView& ACView,
                                           const Aspect_FillMethod FillStyle )
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
  {
    aCView->View->CreateBackgroundTexture( FileName, FillStyle );
  }
}

void OpenGl_GraphicDriver::SetBgImageStyle( const Graphic3d_CView& ACView,
                                           const Aspect_FillMethod FillStyle )
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
  {
    aCView->View->SetBackgroundTextureStyle( FillStyle );
  }
}

void OpenGl_GraphicDriver::SetBgGradientStyle(const Graphic3d_CView& ACView,const Aspect_GradientFillMethod FillType)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
  {
    aCView->View->SetBackgroundGradientType(FillType);
  }
}

void OpenGl_GraphicDriver::GraduatedTrihedronDisplay(const Graphic3d_CView& ACView, const Graphic3d_CGraduatedTrihedron& cubic)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
  {
    aCView->View->GraduatedTrihedronDisplay(cubic);
  }
}

void OpenGl_GraphicDriver::GraduatedTrihedronErase(const Graphic3d_CView& ACView)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
  {
    aCView->View->GraduatedTrihedronErase();
  }
}

void OpenGl_GraphicDriver::GraduatedTrihedronMinMaxValues(const Standard_ShortReal xmin,
                                                         const Standard_ShortReal ymin,
                                                         const Standard_ShortReal zmin,
                                                         const Standard_ShortReal xmax,
                                                         const Standard_ShortReal ymax,
                                                         const Standard_ShortReal zmax)
{
  OpenGl_GraduatedTrihedron::SetMinMax(xmin, ymin, zmin, xmax, ymax, zmax);
}

// Helper function, returns the nearest power of two greater than the argument value
inline Standard_Integer GetNearestPow2(Standard_Integer theValue)
{
  // Precaution against overflow
  Standard_Integer aHalfMax = IntegerLast() >> 1, aRes = 1;
  if ( theValue > aHalfMax ) theValue = aHalfMax;
  while ( aRes < theValue ) aRes <<= 1;
  return aRes;
}

Standard_Integer OpenGl_GraphicDriver::CreateTexture (const Graphic3d_TypeOfTexture Type,
                                                      const Handle_AlienImage_AlienImage &Image,
													  const Standard_CString FileName,
													  const Handle(TColStd_HArray1OfReal)& TexUpperBounds) const
{
  Handle(Image_Image) MyPic = Image->ToImage();

  Standard_Integer aGlWidth  = (Type == Graphic3d_TOT_2D_MIPMAP) ? MyPic->Width() : GetNearestPow2(MyPic->Width());
  Standard_Integer aGlHeight = (Type == Graphic3d_TOT_2D_MIPMAP) ? MyPic->Height() : GetNearestPow2(MyPic->Height());

  TexUpperBounds->SetValue(1, ((Standard_Real) (MyPic->Width())/((Standard_Real) aGlWidth)));
  TexUpperBounds->SetValue(2, ((Standard_Real) (MyPic->Height())/((Standard_Real) aGlHeight)));

  unsigned char *MyImageData = new unsigned char[aGlWidth*aGlHeight*4];
  unsigned char *MyData = MyImageData;
  int TexId;
  int i,j;
  Quantity_Color MyColor;

  if (MyImageData == NULL)
    return -1;

  static Standard_Integer textureRank=0;  
  char textureName[16];
  Standard_PCharacter fileName = textureName; 
  sprintf(fileName,"Tex%d",++textureRank);

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
          TexId = GetTextureData1D (fileName, aGlWidth, aGlHeight, MyImageData);
          break;

        case Graphic3d_TOT_2D:
          TexId = GetTextureData2D (fileName, aGlWidth, aGlHeight, MyImageData);
          break;

        case Graphic3d_TOT_2D_MIPMAP:
          TexId = GetTextureData2DMipMap (fileName, aGlWidth, aGlHeight, MyImageData);
          break;

        default:
          TexId = -1;
        }

        delete [] MyImageData;
        return TexId;

}

void OpenGl_GraphicDriver::DestroyTexture (const Standard_Integer theTexId) const
{
  FreeTexture (theTexId);
}

void OpenGl_GraphicDriver::ModifyTexture (const Standard_Integer        theTexId,
                                          const Graphic3d_CInitTexture& theInfo) const
{
  if (theInfo.doModulate)
    SetTextureModulate (theTexId);
  else
    SetTextureDecal (theTexId);

  if (theInfo.doRepeat)
    SetTextureRepeat (theTexId);
  else
    SetTextureClamp (theTexId);

  switch (theInfo.Mode)
  {
    case 0:
      SetModeObject (theTexId, theInfo.sparams, theInfo.tparams);
      break;

    case 1:
      SetModeSphere (theTexId);
      break;

    case 2:
      SetModeEye (theTexId, theInfo.sparams, theInfo.tparams);
      break;

    case 3:
      SetModeManual (theTexId);
      break;        
  }

  if (theInfo.doLinear)
    SetRenderLinear (theTexId);
  else
    SetRenderNearest (theTexId);

  SetTexturePosition (theTexId,
                      theInfo.sx, theInfo.sy,
                      theInfo.tx, theInfo.ty,
                      theInfo.angle);
}
