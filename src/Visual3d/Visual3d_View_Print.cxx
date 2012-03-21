// Created by: THA
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

// e-mail       t-hartl@muenchen.matra-dtv.fr

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include <Visual3d_View.jxx>
#include <Visual3d_View.pxx>

/************************************************************************/
/* Print Methods                                                        */
/************************************************************************/

Standard_Boolean Visual3d_View::Print
  (const Aspect_Handle    hPrintDC, 
   const Standard_Boolean showBackground,
   const Standard_CString filename,
   const Aspect_PrintAlgo printAlgorithm,
   const Standard_Real    theScaleFactor) const
{
  return Print (MyViewManager->UnderLayer (),
                MyViewManager->OverLayer (),
                hPrintDC, showBackground,
                filename, printAlgorithm, 
                theScaleFactor);
}

Standard_Boolean Visual3d_View::Print
  (const Handle(Visual3d_Layer)& AnUnderLayer,
   const Handle(Visual3d_Layer)& AnOverLayer,
   const Aspect_Handle           hPrintDC,
   const Standard_Boolean        showBackground,
   const Standard_CString        aFilename,
   const Aspect_PrintAlgo        printAlgorithm,
   const Standard_Real           theScaleFactor) const
{
  if (IsDeleted ()) return Standard_False;

  if ((! IsDefined ()) || (! IsActive ())) return Standard_False;

  if (! MyWindow->IsMapped ()) return Standard_False;

  Aspect_CLayer2d OverCLayer;
  Aspect_CLayer2d UnderCLayer;

  OverCLayer.ptrLayer = UnderCLayer.ptrLayer = NULL;
  if (! AnOverLayer.IsNull ()) OverCLayer = AnOverLayer->CLayer ();
  if (! AnUnderLayer.IsNull ()) UnderCLayer = AnUnderLayer->CLayer ();

  return MyGraphicDriver->Print (MyCView, UnderCLayer, OverCLayer,
                                 hPrintDC, showBackground, aFilename,
                                 printAlgorithm, theScaleFactor);
}
