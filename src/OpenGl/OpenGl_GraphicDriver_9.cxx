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

Standard_Integer OpenGl_GraphicDriver::CreateTexture (const Graphic3d_TypeOfTexture        theType,
                                                      const Image_PixMap&                  theImage,
                                                      const Standard_CString               theFileName,
                                                      const Handle(TColStd_HArray1OfReal)& theTexUpperBounds) const
{
  if (theImage.IsEmpty())
  {
    return -1;
  }

  Standard_Integer aGlWidth  = (Standard_Integer )theImage.Width();
  Standard_Integer aGlHeight = (Standard_Integer )theImage.Height();
  if (theType != Graphic3d_TOT_2D_MIPMAP)
  {
    aGlWidth  = GetNearestPow2 (aGlWidth);
    aGlHeight = GetNearestPow2 (aGlHeight);
  }
  theTexUpperBounds->SetValue (1, Standard_Real(theImage.Width())  / Standard_Real(aGlWidth));
  theTexUpperBounds->SetValue (2, Standard_Real(theImage.Height()) / Standard_Real(aGlHeight));

  Image_PixMap anImage;
  if (!anImage.InitTrash (Image_PixMap::ImgRGBA, Standard_Size(aGlWidth), Standard_Size(aGlHeight)))
  {
    return -1;
  }

  anImage.SetTopDown (false);
  Image_PixMapData<Image_ColorRGBA>& aDataNew = anImage.EditData<Image_ColorRGBA>();
  Quantity_Color aSrcColor;
  for (Standard_Size aRow = 0; aRow < theImage.SizeY(); ++aRow)
  {
    for (Standard_Size aCol = 0; aCol < theImage.SizeX(); ++aCol)
    {
      aSrcColor = theImage.PixelColor (aCol, aRow);
      Image_ColorRGBA& aColor = aDataNew.ChangeValue (aRow, aCol);
      aColor.r() = int(255.0 * aSrcColor.Red());
      aColor.g() = int(255.0 * aSrcColor.Green());
      aColor.b() = int(255.0 * aSrcColor.Blue());
      aColor.a() = 0xFF;
    }

    for (Standard_Size aCol = theImage.SizeX(); aCol < anImage.SizeX(); ++aCol)
    {
      Image_ColorRGBA& aColor = aDataNew.ChangeValue (aRow, aCol);
      aColor.r() = 0x00;
      aColor.g() = 0x00;
      aColor.b() = 0x00;
      aColor.a() = 0xFF;
    }
  }

  // Padding the lower part of the texture with black
  for (Standard_Size aRow = theImage.SizeY(); aRow < anImage.SizeY(); ++aRow)
  {
    for (Standard_Size aCol = 0; aCol < anImage.SizeX(); ++aCol)
    {
      Image_ColorRGBA& aColor = aDataNew.ChangeValue (aRow, aCol);
      aColor.r() = 0x00;
      aColor.g() = 0x00;
      aColor.b() = 0x00;
      aColor.a() = 0xFF;
    }
  }

  static Standard_Integer TheTextureRank = 0;
  char aTextureStrId[255];
  sprintf (aTextureStrId, "Tex%d", ++TheTextureRank);

  switch (theType)
  {
    case Graphic3d_TOT_1D:        return GetTextureData1D       (aTextureStrId, aGlWidth, aGlHeight, anImage.Data());
    case Graphic3d_TOT_2D:        return GetTextureData2D       (aTextureStrId, aGlWidth, aGlHeight, anImage.Data());
    case Graphic3d_TOT_2D_MIPMAP: return GetTextureData2DMipMap (aTextureStrId, aGlWidth, aGlHeight, anImage.Data());
    default:                      return -1;
  }
}

void OpenGl_GraphicDriver::DestroyTexture (const Standard_Integer theTexId) const
{
  FreeTexture (GetSharedContext(), theTexId);
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
