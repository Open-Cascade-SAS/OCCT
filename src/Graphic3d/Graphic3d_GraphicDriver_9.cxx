// File   Graphic3d_GraphicDriver_8.cxx
// Created  Mardi 28 janvier 1997
// Author CAL
// modified:    1/07/97 ; PCT : ajout texture mapping
//    16-09-98 ; BGN : Points d'entree du Triedre (S3819, Phase 1)
//    22-09-98 ; BGN : S3989 (anciennement S3819)
//                               TypeOfTriedron* from Aspect (pas Graphic3d)
//      02.15.100 : JR : Clutter

//-Copyright  MatraDatavision 1997

//-Version  

//-Design Declaration des variables specifiques aux Drivers

//-Warning  Un driver encapsule les Pex et OpenGl drivers

//-References 

//-Language C++ 2.0

//-Declarations


//SAV 23/12/02 : Added methods to set background image.

// for the class
#include <Graphic3d_GraphicDriver.jxx>

#include <Aspect_DriverDefinitionError.hxx>

#include <stdlib.h>

//-Aliases

//-Global data definitions

//-Methods, in order

Standard_Integer Graphic3d_GraphicDriver::CreateTexture(const Graphic3d_TypeOfTexture ,const Handle_AlienImage_AlienImage &,const Standard_CString,const Handle(TColStd_HArray1OfReal)& TexUpperBounds ) const
{
  return -1;

}

void Graphic3d_GraphicDriver::DestroyTexture(const Standard_Integer ) const
{
}


void Graphic3d_GraphicDriver::ModifyTexture(const Standard_Integer ,const Graphic3d_CInitTexture& ) const
{
}
    
void Graphic3d_GraphicDriver::Environment(const Graphic3d_CView& )
{
}


//
// Triedron methods : the Triedron is a non-zoomable object.
//


void Graphic3d_GraphicDriver::ZBufferTriedronSetup (
  const Quantity_NameOfColor,
  const Quantity_NameOfColor,
  const Quantity_NameOfColor,
  const Standard_Real,
  const Standard_Real,
  const Standard_Integer)
{
}
     
void Graphic3d_GraphicDriver::TriedronDisplay (
  const Graphic3d_CView& ,
  const Aspect_TypeOfTriedronPosition ,
  const Quantity_NameOfColor , 
  const Standard_Real,
  const Standard_Boolean   )
{
}
   
void Graphic3d_GraphicDriver::TriedronErase (const Graphic3d_CView& ) 
{
}


void Graphic3d_GraphicDriver::TriedronEcho (const Graphic3d_CView& ,const Aspect_TypeOfTriedronEcho  )
{
}


void Graphic3d_GraphicDriver::BackgroundImage( const Standard_CString /*FileName*/, 
                                              const Graphic3d_CView& /*ACView*/,
                                              const Aspect_FillMethod /*FillStyle*/)
{
}

void Graphic3d_GraphicDriver::SetBgImageStyle( const Graphic3d_CView& /*ACView*/,
                                              const Aspect_FillMethod /*FillStyle*/)
{
}

void Graphic3d_GraphicDriver::SetBgGradientStyle( const Graphic3d_CView& /*ACView*/,
                                                  const Aspect_GradientFillMethod /*FillStyle*/)
{
}

void Graphic3d_GraphicDriver::GraduatedTrihedronDisplay(const Graphic3d_CView&,
                                                        const Graphic3d_CGraduatedTrihedron&)
{
}

void Graphic3d_GraphicDriver::GraduatedTrihedronErase(const Graphic3d_CView&)
{
}

void Graphic3d_GraphicDriver::GraduatedTrihedronMinMaxValues(const Standard_ShortReal,
                                                             const Standard_ShortReal,
                                                             const Standard_ShortReal,
                                                             const Standard_ShortReal,
                                                             const Standard_ShortReal,
                                                             const Standard_ShortReal)
{
}
