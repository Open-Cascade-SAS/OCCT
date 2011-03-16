// File         OpenGl_GraphicDriver_713.cxx
// Created      22-10-01
// Author       SAV


// 09/07/02     SAV  - merged with file Graphic3d_GraphicDriver_713.cxx created 16/06/2000 by ATS  for G005 study.
//Following routines are entry points for creation of PARRAY primitives for (used for presentation).

#include <Graphic3d_GraphicDriver.jxx>
#include <Aspect_DriverDefinitionError.hxx>

#define  BUC61044    /* 25/10/01 SAV ; added functionality to control gl depth testing
from higher API */
#define  BUC61045    /* 25/10/01 SAV ; added functionality to control gl lighting 
from higher API */


void Graphic3d_GraphicDriver::SetDepthTestEnabled( const Graphic3d_CView&, const Standard_Boolean ) const {
}

Standard_Boolean Graphic3d_GraphicDriver::IsDepthTestEnabled( const Graphic3d_CView& ) const {
  return Standard_True;
}

void Graphic3d_GraphicDriver::ReadDepths( const Graphic3d_CView&,
                                          const Standard_Integer,
                                          const Standard_Integer,
                                          const Standard_Integer,
                                          const Standard_Integer,
                                          const Standard_Address ) const
{
}

void Graphic3d_GraphicDriver::SetGLLightEnabled( const Graphic3d_CView&, const Standard_Boolean ) const { 
}

Standard_Boolean Graphic3d_GraphicDriver::IsGLLightEnabled( const Graphic3d_CView& ) const {
  return Standard_True;
}


void Graphic3d_GraphicDriver :: PrimitiveArray ( const Graphic3d_CGroup&,const Graphic3d_PrimitiveArray&,const Standard_Boolean) {
}


void Graphic3d_GraphicDriver :: UserDraw ( const Graphic3d_CGroup&,
                                           const Graphic3d_CUserDraw& )
{
}

void Graphic3d_GraphicDriver::EnableVBO( const Standard_Boolean )
{
}