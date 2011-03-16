// File   Graphic3d_GraphicDriver_7.cxx
// Created  Mardi 28 janvier 1997
// Author CAL
// Modified     GG 10/11/99 PRO19603 Change the Redraw method (add redraw area capabillity)
//    EUG 07/10/99 G003 Add DegenerateStructure() and 
//              SetBackFacingModel() methods.
//      02.15.100 : JR : Clutter

//-Copyright  MatraDatavision 1997

//-Version  

//-Design Declaration des variables specifiques aux Drivers

//-Warning  Un driver encapsule les Pex et OpenGl drivers

//-References 

//-Language C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_GraphicDriver.jxx>

#include <Aspect_DriverDefinitionError.hxx>

//-Aliases

//-Global data definitions

//-Methods, in order

void Graphic3d_GraphicDriver::ActivateView (const Graphic3d_CView& ) {
}

void Graphic3d_GraphicDriver::AntiAliasing (const Graphic3d_CView& , const Standard_Boolean ) {
}

void Graphic3d_GraphicDriver::Background (const Graphic3d_CView& ) {
}

void Graphic3d_GraphicDriver::GradientBackground( const Graphic3d_CView& ACView,
                                                  const Quantity_Color& AColor1, const Quantity_Color& AColor2,
                                                  const Aspect_GradientFillMethod AType ){  
}

void Graphic3d_GraphicDriver::Blink (const Graphic3d_CStructure& , const Standard_Boolean ) {
}

void Graphic3d_GraphicDriver::BoundaryBox (const Graphic3d_CStructure& , const Standard_Boolean ) {
}

void Graphic3d_GraphicDriver::HighlightColor (const Graphic3d_CStructure& , const Standard_ShortReal , const Standard_ShortReal , const Standard_ShortReal , const Standard_Boolean ) {
}

void Graphic3d_GraphicDriver::NameSetStructure (const Graphic3d_CStructure& ) {
}

void Graphic3d_GraphicDriver::ClipLimit (const Graphic3d_CView& , const Standard_Boolean ) {
}

void Graphic3d_GraphicDriver::DeactivateView (const Graphic3d_CView& ) {
}

void Graphic3d_GraphicDriver::DepthCueing (const Graphic3d_CView& , const Standard_Boolean ) {
}

Standard_Boolean Graphic3d_GraphicDriver::ProjectRaster (const Graphic3d_CView& , const Standard_ShortReal , const Standard_ShortReal , const Standard_ShortReal , Standard_Integer& , Standard_Integer& ) {
  return Standard_False;
}

Standard_Boolean Graphic3d_GraphicDriver::UnProjectRaster (const Graphic3d_CView& , const Standard_Integer , const Standard_Integer , const Standard_Integer , const Standard_Integer , const Standard_Integer , const Standard_Integer , Standard_ShortReal& , Standard_ShortReal& , Standard_ShortReal& ) {
  return Standard_False;
}

Standard_Boolean Graphic3d_GraphicDriver::UnProjectRasterWithRay (const Graphic3d_CView& , const Standard_Integer , const Standard_Integer , const Standard_Integer , const Standard_Integer , const Standard_Integer , const Standard_Integer , Standard_ShortReal& , Standard_ShortReal& , Standard_ShortReal& , Standard_ShortReal& , Standard_ShortReal& , Standard_ShortReal& ) {
  return Standard_False;
}

void Graphic3d_GraphicDriver::RatioWindow (const Graphic3d_CView& ) {
}

void Graphic3d_GraphicDriver::Redraw (const Graphic3d_CView& , const Aspect_CLayer2d& , const Aspect_CLayer2d& , const Standard_Integer , const Standard_Integer , const Standard_Integer , const Standard_Integer ) {
}

void Graphic3d_GraphicDriver::RemoveView (const Graphic3d_CView& ) {
}

void Graphic3d_GraphicDriver::SetLight (const Graphic3d_CView& ) {
}

void Graphic3d_GraphicDriver::SetPlane (const Graphic3d_CView& ) {
}

void Graphic3d_GraphicDriver::SetVisualisation (const Graphic3d_CView& ) {
}

void Graphic3d_GraphicDriver::TransformStructure (const Graphic3d_CStructure& ) {
}

void Graphic3d_GraphicDriver :: DegenerateStructure ( const Graphic3d_CStructure& ) {
} 


void Graphic3d_GraphicDriver::Transparency (const Graphic3d_CView& , const Standard_Boolean ) {
}


void Graphic3d_GraphicDriver::Update (const Graphic3d_CView& , const Aspect_CLayer2d& , const Aspect_CLayer2d& ) {
}


Standard_Boolean Graphic3d_GraphicDriver::View (Graphic3d_CView& ) {
  return Standard_True;
}

void Graphic3d_GraphicDriver::ViewMapping (const Graphic3d_CView& , const Standard_Boolean ) {
}

void Graphic3d_GraphicDriver::ViewOrientation (const Graphic3d_CView& , const Standard_Boolean ) {
}

void Graphic3d_GraphicDriver :: SetBackFacingModel ( const Graphic3d_CView&  ) {
}  // end Graphic3d_GraphicDriver :: SetBackFacingModel
