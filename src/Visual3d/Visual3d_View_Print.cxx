// Created by: THA
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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
