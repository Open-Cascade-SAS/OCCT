// File         Graphic3d_GraphicDriver_Print.c
// Created      March 2000
// Author       THA
// e-mail t-hartl@muenchen.matra-dtv.fr

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include <Graphic3d_GraphicDriver.jxx>
#include <Aspect_DriverDefinitionError.hxx>

/************************************************************************/
/* Print Methods                                                        */
/************************************************************************/

Standard_Boolean Graphic3d_GraphicDriver::Print (const Graphic3d_CView& , 
                                                 const Aspect_CLayer2d& , 
                                                 const Aspect_CLayer2d& , 
                                                 const Aspect_Handle ,
                                                 const Standard_Boolean ,
                                                 const Standard_CString ,
                                                 const Aspect_PrintAlgo ,
	                                         const Standard_Real ) const
{
  return Standard_False;
}
