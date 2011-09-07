// File         Visual3d_View_Print.cxx
// Created      March 2000
// Author       THA
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
