// File:      OpenGl_GraphicDriver_print.cxx
// Created:   20 October 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#include <OpenGl_GraphicDriver.hxx>

#include <Standard_NotImplemented.hxx>
#include <OpenGl_CView.hxx>

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
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
    return aCView->WS->Print(ACView,ACUnderLayer,ACOverLayer,hPrintDC,showBackground,filename,printAlgorithm,theScaleFactor);
#else
  Standard_NotImplemented::Raise ("OpenGl_GraphicDriver::Print is implemented only on Windows");
#endif
  return Standard_False;
}
