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

void Graphic3d_GraphicDriver::GetGraduatedTrihedron(const Graphic3d_CView&,
                                                    /* Names of axes */
                                                    Standard_CString&, 
                                                    Standard_CString&, 
                                                    Standard_CString&,
                                                    /* Draw names */
                                                    Standard_Boolean&, 
                                                    Standard_Boolean&, 
                                                    Standard_Boolean&,
                                                    /* Draw values */
                                                    Standard_Boolean&, 
                                                    Standard_Boolean&, 
                                                    Standard_Boolean&,
                                                    /* Draw grid */
                                                    Standard_Boolean&,
                                                    /* Draw axes */
                                                    Standard_Boolean&,
                                                    /* Number of splits along axes */
                                                    Standard_Integer&, 
                                                    Standard_Integer&, 
                                                    Standard_Integer&,
                                                    /* Offset for drawing values */
                                                    Standard_Integer&, 
                                                    Standard_Integer&, 
                                                    Standard_Integer&,
                                                    /* Offset for drawing names of axes */
                                                    Standard_Integer&, 
                                                    Standard_Integer&, 
                                                    Standard_Integer&,
                                                    /* Draw tickmarks */
                                                    Standard_Boolean&, 
                                                    Standard_Boolean&, 
                                                    Standard_Boolean&,
                                                    /* Length of tickmarks */
                                                    Standard_Integer&, 
                                                    Standard_Integer&, 
                                                    Standard_Integer&,
                                                    /* Grid color */
                                                    Quantity_Color&,
                                                    /* X name color */
                                                    Quantity_Color&,
                                                    /* Y name color */
                                                    Quantity_Color&,
                                                    /* Z name color */
                                                    Quantity_Color&,
                                                    /* X color of axis and values */
                                                    Quantity_Color&,
                                                    /* Y color of axis and values */
                                                    Quantity_Color&,
                                                    /* Z color of axis and values */
                                                    Quantity_Color&,
                                                    /* Name of font for names of axes */
                                                    Standard_CString&,
                                                    /* Style of names of axes */
                                                    OSD_FontAspect&,
                                                    /* Size of names of axes */
                                                    Standard_Integer&,
                                                    /* Name of font for values */
                                                    Standard_CString&,
                                                    /* Style of values */
                                                    OSD_FontAspect&,
                                                    /* Size of values */
                                                    Standard_Integer&) const
{
}

void Graphic3d_GraphicDriver::GraduatedTrihedronDisplay(const Graphic3d_CView&,
                                                        Graphic3d_CGraduatedTrihedron&,
                                                        /* Names of axes */
                                                        const Standard_CString, 
                                                        const Standard_CString, 
                                                        const Standard_CString,
                                                        /* Draw names */
                                                        const Standard_Boolean, 
                                                        const Standard_Boolean, 
                                                        const Standard_Boolean,
                                                        /* Draw values */
                                                        const Standard_Boolean, 
                                                        const Standard_Boolean, 
                                                        const Standard_Boolean,
                                                        /* Draw grid */
                                                        const Standard_Boolean,
                                                        /* Draw axes */
                                                        const Standard_Boolean,
                                                        /* Number of splits along axes */
                                                        const Standard_Integer, 
                                                        const Standard_Integer, 
                                                        const Standard_Integer,
                                                        /* Offset for drawing values */
                                                        const Standard_Integer, 
                                                        const Standard_Integer, 
                                                        const Standard_Integer,
                                                        /* Offset for drawing names of axes */
                                                        const Standard_Integer, 
                                                        const Standard_Integer, 
                                                        const Standard_Integer,
                                                        /* Draw tickmarks */
                                                        const Standard_Boolean, 
                                                        const Standard_Boolean, 
                                                        const Standard_Boolean,
                                                        /* Length of tickmarks */
                                                        const Standard_Integer, 
                                                        const Standard_Integer, 
                                                        const Standard_Integer,
                                                        /* Grid color */
                                                        const Quantity_Color&,
                                                        /* X name color */
                                                        const Quantity_Color&,
                                                        /* Y name color */
                                                        const Quantity_Color&,
                                                        /* Z name color */
                                                        const Quantity_Color&,
                                                        /* X color of axis and values */
                                                        const Quantity_Color&,
                                                        /* Y color of axis and values */
                                                        const Quantity_Color&,
                                                        /* Z color of axis and values */
                                                        const Quantity_Color&,
                                                        /* Name of font for names of axes */
                                                        const Standard_CString,
                                                        /* Style of names of axes */
                                                        const OSD_FontAspect,
                                                        /* Size of names of axes */
                                                        const Standard_Integer,
                                                        /* Name of font for values */
                                                        const Standard_CString,
                                                        /* Style of values */
                                                        const OSD_FontAspect,
                                                        /* Size of values */
                                                        const Standard_Integer)
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
