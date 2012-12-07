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
#include <OpenGl_tgl_funcs.hxx>

void OpenGl_GraphicDriver::Environment(const Graphic3d_CView& theCView)
{
  const OpenGl_CView* aCView = (const OpenGl_CView* )theCView.ptrView;
  if (aCView == NULL)
  {
    return;
  }

  aCView->View->SetTextureEnv    (GetSharedContext(), theCView.Context.TextureEnv);
  aCView->View->SetSurfaceDetail ((Visual3d_TypeOfSurfaceDetail)theCView.Context.SurfaceDetail);
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
