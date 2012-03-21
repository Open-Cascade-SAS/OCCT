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
