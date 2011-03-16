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

void Visual3d_View::Print (const Aspect_Handle    hPrintDC, 
                           const Standard_Boolean showBackground, 
                           const Standard_CString filename) const
{
  Print (MyViewManager->UnderLayer (), 
         MyViewManager->OverLayer (), 
         hPrintDC, 
         showBackground, 
         filename);
}

void Visual3d_View::Print (const Handle(Visual3d_Layer)& AnUnderLayer, 
                           const Handle(Visual3d_Layer)& AnOverLayer,
                           const Aspect_Handle           hPrintDC,
                           const Standard_Boolean        showBackground,
                           const Standard_CString        aFilename) const
{
  if (IsDeleted ()) return;

  if ((! IsDefined ()) || (! IsActive ())) return;

  if (! MyWindow->IsMapped ()) return;

  Aspect_CLayer2d OverCLayer;
  Aspect_CLayer2d UnderCLayer;

  OverCLayer.ptrLayer = UnderCLayer.ptrLayer = NULL;
  if (! AnOverLayer.IsNull ()) OverCLayer = AnOverLayer->CLayer ();
  if (! AnUnderLayer.IsNull ()) UnderCLayer = AnUnderLayer->CLayer ();

  MyGraphicDriver->Print (MyCView, UnderCLayer, OverCLayer,
                          hPrintDC, showBackground, aFilename);
}
