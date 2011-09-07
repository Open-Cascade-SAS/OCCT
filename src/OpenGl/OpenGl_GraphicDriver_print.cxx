// File         OpenGl_GraphicDriver_print.c
// Created      March 2000
// Author       THA
// e-mail       t-hartl@muenchen.matra-dtv.fr

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include <OpenGl_GraphicDriver.jxx>
#include <Aspect_DriverDefinitionError.hxx>
#include <Standard_NotImplemented.hxx>

#include <OpenGl_tgl_funcs.hxx>


/************************************************************************/
/* Print Methods                                                        */
/************************************************************************/

Standard_Boolean OpenGl_GraphicDriver::Print
  (const Graphic3d_CView& ACView, 
   const Aspect_CLayer2d& ACUnderLayer, 
   const Aspect_CLayer2d& ACOverLayer,
   const Aspect_Handle    hPrintDC,
   const Standard_Boolean showBackground,
   const Standard_CString filename,
   const Aspect_PrintAlgo printAlgorithm,
   const Standard_Real theScaleFactor) const
{

#ifdef WNT
  Graphic3d_CView MyCView = ACView;
  Aspect_CLayer2d MyCUnderLayer = ACUnderLayer;
  Aspect_CLayer2d MyCOverLayer = ACOverLayer;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_print");
    PrintCView (MyCView, 1);
  }
  return call_togl_print (&MyCView, &MyCUnderLayer, &MyCOverLayer,
                          hPrintDC, (int)showBackground, filename,
                          (int)printAlgorithm, (float)theScaleFactor);
#else
  Standard_NotImplemented::Raise ("OpenGl_GraphicDriver::Print is implemented "
    "only on Windows");
  return Standard_False;
#endif
}
