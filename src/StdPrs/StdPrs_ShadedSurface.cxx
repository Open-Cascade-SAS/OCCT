// Created on: 1995-07-27
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
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


#define G005    //ATS,GG 04/01/01 Use PrimitiveArray instead Sets of primitives
//              for performance improvment

#include <StdPrs_ShadedSurface.ixx>

#include <Graphic3d_ArrayOfTriangleStrips.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_Group.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Precision.hxx>
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

  if (!theDrawer->ShadingAspectGlobal())
  {
    // If the surface is closed, the faces from back-side are not traced:
    Handle(Graphic3d_AspectFillArea3d) anAsp = theDrawer->ShadingAspect()->Aspect();
    if (theSurface.IsUClosed() && theSurface.IsVClosed())
    {
      anAsp->SuppressBackFace();
    }
    else
    {
      anAsp->AllowBackFace();
    }
    Prs3d_Root::CurrentGroup (thePrs)->SetGroupPrimitivesAspect (anAsp);
  }

  Standard_Integer aNBUintv = theSurface.NbUIntervals (GeomAbs_C1);
  Standard_Integer aNBVintv = theSurface.NbVIntervals (GeomAbs_C1);
  TColStd_Array1OfReal anInterU (1, aNBUintv + 1);
  TColStd_Array1OfReal anInterV (1, aNBVintv + 1);

  theSurface.UIntervals (anInterU, GeomAbs_C1);
  theSurface.VIntervals (anInterV, GeomAbs_C1);

  Standard_Real U1, U2, V1, V2, DU, DV;

  gp_Pnt P1, P2;
  gp_Vec D1U, D1V, D1, D2;

  Prs3d_Root::CurrentGroup (thePrs)->BeginPrimitives();
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
      Prs3d_Root::CurrentGroup (thePrs)->AddPrimitiveArray (aPArray);
    }
    Prs3d_Root::CurrentGroup (thePrs)->EndPrimitives();
  }
}
