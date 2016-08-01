// Created on: 1995-07-27
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
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


#include <Adaptor3d_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Graphic3d_ArrayOfTriangleStrips.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Precision.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <StdPrs_ShadedSurface.hxx>
#include <TColStd_Array1OfReal.hxx>

//=======================================================================
//function : Add
//purpose  :
//=======================================================================
void StdPrs_ShadedSurface::Add (const Handle(Prs3d_Presentation)& thePrs,
                                const Adaptor3d_Surface&          theSurface,
                                const Handle(Prs3d_Drawer)&       theDrawer)
{
  Standard_Integer N1 = theDrawer->UIsoAspect()->Number();
  Standard_Integer N2 = theDrawer->VIsoAspect()->Number();
  N1 = N1 < 3 ? 3 : N1;
  N2 = N2 < 3 ? 3 : N2;

  // If the surface is closed, the faces from back-side are not traced:
  Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup (thePrs);
  aGroup->SetGroupPrimitivesAspect (theDrawer->ShadingAspect()->Aspect());
  aGroup->SetClosed (theSurface.IsUClosed() && theSurface.IsVClosed());

  Standard_Integer aNBUintv = theSurface.NbUIntervals (GeomAbs_C1);
  Standard_Integer aNBVintv = theSurface.NbVIntervals (GeomAbs_C1);
  TColStd_Array1OfReal anInterU (1, aNBUintv + 1);
  TColStd_Array1OfReal anInterV (1, aNBVintv + 1);

  theSurface.UIntervals (anInterU, GeomAbs_C1);
  theSurface.VIntervals (anInterV, GeomAbs_C1);

  Standard_Real U1, U2, V1, V2, DU, DV;

  gp_Pnt P1, P2;
  gp_Vec D1U, D1V, D1, D2;

  for (Standard_Integer NU = 1; NU <= aNBUintv; ++NU)
  {
    for (Standard_Integer NV = 1; NV <= aNBVintv; ++NV)
    {
      U1 = anInterU (NU); U2 = anInterU (NU + 1);
      V1 = anInterV (NV); V2 = anInterV (NV + 1);

      U1 = (Precision::IsNegativeInfinite (U1)) ? - theDrawer->MaximalParameterValue() : U1;
      U2 = (Precision::IsPositiveInfinite (U2)) ?   theDrawer->MaximalParameterValue() : U2;

      V1 = (Precision::IsNegativeInfinite (V1)) ? - theDrawer->MaximalParameterValue() : V1;
      V2 = (Precision::IsPositiveInfinite (V2)) ?   theDrawer->MaximalParameterValue() : V2;

      DU = (U2 - U1) / N1;
      DV = (V2 - V1) / N2;

      Handle(Graphic3d_ArrayOfTriangleStrips) aPArray
        = new Graphic3d_ArrayOfTriangleStrips (2 * (N1 + 1) * (N2 + 1), N1 + 1,
                                               Standard_True, Standard_False, Standard_False, Standard_False);
      for (Standard_Integer i = 1; i <= N1 + 1; ++i)
      {
        aPArray->AddBound (N2 + 1);
        for (Standard_Integer j = 1; j <= N2 + 1; ++j)
        {
          theSurface.D1 (U1 + DU * (i - 1), V1 + DV * (j - 1), P2, D1U, D1V);
          D1 = D1U ^ D1V;
          D1.Normalize();
          theSurface.D1 (U1 + DU * i, V1 + DV * (j - 1), P2, D1U, D1V);
          D2 = D1U ^ D1V;
          D2.Normalize();
          aPArray->AddVertex (P1, D1);
          aPArray->AddVertex (P2, D2);
        }
      }
      aGroup->AddPrimitiveArray (aPArray);
    }
  }
}
