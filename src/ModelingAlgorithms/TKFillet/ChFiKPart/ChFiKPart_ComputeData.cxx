// Created on: 1993-12-23
// Created by: Isabelle GRIGNON
// Copyright (c) 1993-1999 Matra Datavision
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

#include <BRepAdaptor_Surface.hxx>
#include <ChFiDS_ChamfSpine.hxx>
#include <ChFiDS_FilSpine.hxx>
#include <ChFiDS_Spine.hxx>
#include <ChFiDS_SurfData.hxx>
#include <ChFiKPart_ComputeData.hxx>
#include <ChFiKPart_ComputeData_ChAsymPlnCon.hxx>
#include <ChFiKPart_ComputeData_ChAsymPlnCyl.hxx>
#include <ChFiKPart_ComputeData_ChAsymPlnPln.hxx>
#include <ChFiKPart_ComputeData_ChPlnCon.hxx>
#include <ChFiKPart_ComputeData_ChPlnCyl.hxx>
#include <ChFiKPart_ComputeData_ChPlnPln.hxx>
#include <ChFiKPart_ComputeData_CS.hxx>
#include <ChFiKPart_ComputeData_Fcts.hxx>
#include <ChFiKPart_ComputeData_FilPlnCon.hxx>
#include <ChFiKPart_ComputeData_FilPlnCyl.hxx>
#include <ChFiKPart_ComputeData_FilPlnPln.hxx>
#include <ChFiKPart_ComputeData_Rotule.hxx>
#include <ChFiKPart_ComputeData_Sphere.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <gp_Circ.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <TopExp.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>

// #include <BRepAdaptor_Curve2d.hxx>
// #include <BRepAdaptor_Curve2d.hxx>
//=================================================================================================

bool ChFiKPart_ComputeData::Compute(TopOpeBRepDS_DataStructure&      DStr,
                                                occ::handle<ChFiDS_SurfData>&         Data,
                                                const occ::handle<Adaptor3d_Surface>& S1,
                                                const occ::handle<Adaptor3d_Surface>& S2,
                                                const TopAbs_Orientation         Or1,
                                                const TopAbs_Orientation         Or2,
                                                const occ::handle<ChFiDS_Spine>&      Sp,
                                                const int           Iedge)
{
  double Wref = 0.;

  occ::handle<ChFiDS_FilSpine>   Spine  = occ::down_cast<ChFiDS_FilSpine>(Sp);
  occ::handle<ChFiDS_ChamfSpine> CSpine = occ::down_cast<ChFiDS_ChamfSpine>(Sp);
  bool          surfok = false;
  GeomAbs_SurfaceType       typ1   = S1->GetType();
  GeomAbs_SurfaceType       typ2   = S2->GetType();
  GeomAbs_CurveType         ctyp;

  if (!Spine.IsNull())
    ctyp = Spine->CurrentElementarySpine(Iedge).GetType();
  else
    ctyp = CSpine->CurrentElementarySpine(Iedge).GetType();

  // Return orientations.
  TopAbs_Orientation          OrFace1 = TopAbs_FORWARD, OrFace2 = TopAbs_FORWARD;
  occ::handle<BRepAdaptor_Surface> HS = occ::down_cast<BRepAdaptor_Surface>(S1);
  if (!HS.IsNull())
    OrFace1 = HS->Face().Orientation();
  HS = occ::down_cast<BRepAdaptor_Surface>(S2);
  if (!HS.IsNull())
    OrFace2 = HS->Face().Orientation();

  if (!Spine.IsNull())
  {
    double Radius = Spine->Radius(Iedge);
    if (typ1 == GeomAbs_Plane && typ2 == GeomAbs_Plane)
    {
      surfok = ChFiKPart_MakeFillet(DStr,
                                    Data,
                                    S1->Plane(),
                                    S2->Plane(),
                                    Or1,
                                    Or2,
                                    Radius,
                                    Spine->Line(),
                                    Wref,
                                    OrFace1);
    }
    else if (typ1 == GeomAbs_Plane && typ2 == GeomAbs_Cylinder)
    {
      if (ctyp == GeomAbs_Line)
        surfok = ChFiKPart_MakeFillet(DStr,
                                      Data,
                                      S1->Plane(),
                                      S2->Cylinder(),
                                      S2->FirstUParameter(),
                                      S2->LastUParameter(),
                                      Or1,
                                      Or2,
                                      Radius,
                                      Spine->Line(),
                                      Wref,
                                      OrFace1,
                                      true);
      else
        surfok = ChFiKPart_MakeFillet(DStr,
                                      Data,
                                      S1->Plane(),
                                      S2->Cylinder(),
                                      S2->FirstUParameter(),
                                      S2->LastUParameter(),
                                      Or1,
                                      Or2,
                                      Radius,
                                      Spine->Circle(),
                                      Wref,
                                      OrFace1,
                                      true);
    }
    else if (typ1 == GeomAbs_Cylinder && typ2 == GeomAbs_Plane)
    {
      if (ctyp == GeomAbs_Line)
        surfok = ChFiKPart_MakeFillet(DStr,
                                      Data,
                                      S2->Plane(),
                                      S1->Cylinder(),
                                      S1->FirstUParameter(),
                                      S1->LastUParameter(),
                                      Or2,
                                      Or1,
                                      Radius,
                                      Spine->Line(),
                                      Wref,
                                      OrFace2,
                                      false);
      else
        surfok = ChFiKPart_MakeFillet(DStr,
                                      Data,
                                      S2->Plane(),
                                      S1->Cylinder(),
                                      S1->FirstUParameter(),
                                      S1->LastUParameter(),
                                      Or2,
                                      Or1,
                                      Radius,
                                      Spine->Circle(),
                                      Wref,
                                      OrFace2,
                                      false);
    }
    else if (typ1 == GeomAbs_Plane && typ2 == GeomAbs_Cone)
    {
      surfok = ChFiKPart_MakeFillet(DStr,
                                    Data,
                                    S1->Plane(),
                                    S2->Cone(),
                                    S2->FirstUParameter(),
                                    S2->LastUParameter(),
                                    Or1,
                                    Or2,
                                    Radius,
                                    Spine->Circle(),
                                    Wref,
                                    OrFace1,
                                    true);
    }
    else if (typ1 == GeomAbs_Cone && typ2 == GeomAbs_Plane)
    {
      surfok = ChFiKPart_MakeFillet(DStr,
                                    Data,
                                    S2->Plane(),
                                    S1->Cone(),
                                    S1->FirstUParameter(),
                                    S1->LastUParameter(),
                                    Or2,
                                    Or1,
                                    Radius,
                                    Spine->Circle(),
                                    Wref,
                                    OrFace2,
                                    false);
    }
    else
    {
      throw Standard_NotImplemented("particular case not written");
    }
  }
  else if (!CSpine.IsNull())
  {

    ChFiDS_ChamfMode aMode = CSpine->Mode();

    if (CSpine->IsChamfer() == ChFiDS_Sym)
    {
      double dis;
      CSpine->GetDist(dis);

      if (typ1 == GeomAbs_Plane && typ2 == GeomAbs_Plane)
      {
        surfok = ChFiKPart_MakeChamfer(DStr,
                                       Data,
                                       aMode,
                                       S1->Plane(),
                                       S2->Plane(),
                                       Or1,
                                       Or2,
                                       dis,
                                       dis,
                                       CSpine->Line(),
                                       Wref,
                                       OrFace1);
      }
      else if (typ1 == GeomAbs_Plane && typ2 == GeomAbs_Cylinder)
      {
        if (ctyp == GeomAbs_Circle)
          surfok = ChFiKPart_MakeChamfer(DStr,
                                         Data,
                                         aMode,
                                         S1->Plane(),
                                         S2->Cylinder(),
                                         S2->FirstUParameter(),
                                         S2->LastUParameter(),
                                         Or1,
                                         Or2,
                                         dis,
                                         dis,
                                         CSpine->Circle(),
                                         Wref,
                                         OrFace1,
                                         true);
        else
          surfok = ChFiKPart_MakeChamfer(DStr,
                                         Data,
                                         aMode,
                                         S1->Plane(),
                                         S2->Cylinder(),
                                         S2->FirstUParameter(),
                                         S2->LastUParameter(),
                                         Or1,
                                         Or2,
                                         dis,
                                         dis,
                                         CSpine->Line(),
                                         Wref,
                                         OrFace1,
                                         true);
      }
      else if (typ1 == GeomAbs_Cylinder && typ2 == GeomAbs_Plane)
      {
        if (ctyp == GeomAbs_Circle)
          surfok = ChFiKPart_MakeChamfer(DStr,
                                         Data,
                                         aMode,
                                         S2->Plane(),
                                         S1->Cylinder(),
                                         S1->FirstUParameter(),
                                         S1->LastUParameter(),
                                         Or2,
                                         Or1,
                                         dis,
                                         dis,
                                         CSpine->Circle(),
                                         Wref,
                                         OrFace2,
                                         false);
        else
          surfok = ChFiKPart_MakeChamfer(DStr,
                                         Data,
                                         aMode,
                                         S2->Plane(),
                                         S1->Cylinder(),
                                         S1->FirstUParameter(),
                                         S1->LastUParameter(),
                                         Or2,
                                         Or1,
                                         dis,
                                         dis,
                                         CSpine->Line(),
                                         Wref,
                                         OrFace2,
                                         false);
      }
      else if (typ1 == GeomAbs_Plane && typ2 == GeomAbs_Cone)
      {
        surfok = ChFiKPart_MakeChamfer(DStr,
                                       Data,
                                       aMode,
                                       S1->Plane(),
                                       S2->Cone(),
                                       S2->FirstUParameter(),
                                       S2->LastUParameter(),
                                       Or1,
                                       Or2,
                                       dis,
                                       dis,
                                       CSpine->Circle(),
                                       Wref,
                                       OrFace1,
                                       true);
      }
      else if (typ1 == GeomAbs_Cone && typ2 == GeomAbs_Plane)
      {
        surfok = ChFiKPart_MakeChamfer(DStr,
                                       Data,
                                       aMode,
                                       S2->Plane(),
                                       S1->Cone(),
                                       S1->FirstUParameter(),
                                       S1->LastUParameter(),
                                       Or2,
                                       Or1,
                                       dis,
                                       dis,
                                       CSpine->Circle(),
                                       Wref,
                                       OrFace2,
                                       false);
      }
      else
      {
        throw Standard_NotImplemented("particular case not written");
      }
    }
    else if (CSpine->IsChamfer() == ChFiDS_TwoDist)
    {
      double dis1, dis2;
      CSpine->Dists(dis1, dis2);
      if (typ1 == GeomAbs_Plane && typ2 == GeomAbs_Plane)
      {
        surfok = ChFiKPart_MakeChamfer(DStr,
                                       Data,
                                       aMode,
                                       S1->Plane(),
                                       S2->Plane(),
                                       Or1,
                                       Or2,
                                       dis1,
                                       dis2,
                                       CSpine->Line(),
                                       Wref,
                                       OrFace1);
      }
      else if (typ1 == GeomAbs_Plane && typ2 == GeomAbs_Cylinder)
      {
        if (ctyp == GeomAbs_Circle)
          surfok = ChFiKPart_MakeChamfer(DStr,
                                         Data,
                                         aMode,
                                         S1->Plane(),
                                         S2->Cylinder(),
                                         S2->FirstUParameter(),
                                         S2->LastUParameter(),
                                         Or1,
                                         Or2,
                                         dis1,
                                         dis2,
                                         CSpine->Circle(),
                                         Wref,
                                         OrFace1,
                                         true);
        else
          surfok = ChFiKPart_MakeChamfer(DStr,
                                         Data,
                                         aMode,
                                         S1->Plane(),
                                         S2->Cylinder(),
                                         S2->FirstUParameter(),
                                         S2->LastUParameter(),
                                         Or1,
                                         Or2,
                                         dis1,
                                         dis2,
                                         CSpine->Line(),
                                         Wref,
                                         OrFace1,
                                         true);
      }
      else if (typ1 == GeomAbs_Cylinder && typ2 == GeomAbs_Plane)
      {
        if (ctyp == GeomAbs_Circle)
          surfok = ChFiKPart_MakeChamfer(DStr,
                                         Data,
                                         aMode,
                                         S2->Plane(),
                                         S1->Cylinder(),
                                         S1->FirstUParameter(),
                                         S1->LastUParameter(),
                                         Or2,
                                         Or1,
                                         dis2,
                                         dis1,
                                         CSpine->Circle(),
                                         Wref,
                                         OrFace2,
                                         false);
        else
          surfok = ChFiKPart_MakeChamfer(DStr,
                                         Data,
                                         aMode,
                                         S2->Plane(),
                                         S1->Cylinder(),
                                         S1->FirstUParameter(),
                                         S1->LastUParameter(),
                                         Or2,
                                         Or1,
                                         dis2,
                                         dis1,
                                         CSpine->Line(),
                                         Wref,
                                         OrFace2,
                                         false);
      }
      else if (typ1 == GeomAbs_Plane && typ2 == GeomAbs_Cone)
      {
        surfok = ChFiKPart_MakeChamfer(DStr,
                                       Data,
                                       aMode,
                                       S1->Plane(),
                                       S2->Cone(),
                                       S2->FirstUParameter(),
                                       S2->LastUParameter(),
                                       Or1,
                                       Or2,
                                       dis1,
                                       dis2,
                                       CSpine->Circle(),
                                       Wref,
                                       OrFace1,
                                       true);
      }
      else if (typ1 == GeomAbs_Cone && typ2 == GeomAbs_Plane)
      {
        surfok = ChFiKPart_MakeChamfer(DStr,
                                       Data,
                                       aMode,
                                       S2->Plane(),
                                       S1->Cone(),
                                       S1->FirstUParameter(),
                                       S1->LastUParameter(),
                                       Or2,
                                       Or1,
                                       dis1,
                                       dis2,
                                       CSpine->Circle(),
                                       Wref,
                                       OrFace2,
                                       false);
      }
      else
      {
        throw Standard_NotImplemented("particular case not written");
      }
    }
    else
    {
      double    dis, Angle;
      bool DisOnP = true;
      CSpine->GetDistAngle(dis, Angle);
      if (typ1 == GeomAbs_Plane && typ2 == GeomAbs_Plane)
      {
        surfok = ChFiKPart_MakeChAsym(DStr,
                                      Data,
                                      S1->Plane(),
                                      S2->Plane(),
                                      Or1,
                                      Or2,
                                      dis,
                                      Angle,
                                      CSpine->Line(),
                                      Wref,
                                      OrFace1,
                                      DisOnP);
      }
      else if (typ1 == GeomAbs_Plane && typ2 == GeomAbs_Cylinder)
      {
        if (ctyp == GeomAbs_Circle)
          surfok = ChFiKPart_MakeChAsym(DStr,
                                        Data,
                                        S1->Plane(),
                                        S2->Cylinder(),
                                        S2->FirstUParameter(),
                                        S2->LastUParameter(),
                                        Or1,
                                        Or2,
                                        dis,
                                        Angle,
                                        CSpine->Circle(),
                                        Wref,
                                        OrFace1,
                                        true,
                                        DisOnP);
        else
          surfok = ChFiKPart_MakeChAsym(DStr,
                                        Data,
                                        S1->Plane(),
                                        S2->Cylinder(),
                                        S2->FirstUParameter(),
                                        S2->LastUParameter(),
                                        Or1,
                                        Or2,
                                        dis,
                                        Angle,
                                        CSpine->Line(),
                                        Wref,
                                        OrFace1,
                                        true,
                                        DisOnP);
      }
      else if (typ1 == GeomAbs_Cylinder && typ2 == GeomAbs_Plane)
      {
        if (ctyp == GeomAbs_Circle)
          surfok = ChFiKPart_MakeChAsym(DStr,
                                        Data,
                                        S2->Plane(),
                                        S1->Cylinder(),
                                        S1->FirstUParameter(),
                                        S1->LastUParameter(),
                                        Or2,
                                        Or1,
                                        dis,
                                        Angle,
                                        CSpine->Circle(),
                                        Wref,
                                        OrFace2,
                                        false,
                                        DisOnP);
        else
          surfok = ChFiKPart_MakeChAsym(DStr,
                                        Data,
                                        S2->Plane(),
                                        S1->Cylinder(),
                                        S1->FirstUParameter(),
                                        S1->LastUParameter(),
                                        Or2,
                                        Or1,
                                        dis,
                                        Angle,
                                        CSpine->Line(),
                                        Wref,
                                        OrFace2,
                                        false,
                                        DisOnP);
      }
      else if (typ1 == GeomAbs_Plane && typ2 == GeomAbs_Cone)
      {
        surfok = ChFiKPart_MakeChAsym(DStr,
                                      Data,
                                      S1->Plane(),
                                      S2->Cone(),
                                      S2->FirstUParameter(),
                                      S2->LastUParameter(),
                                      Or1,
                                      Or2,
                                      dis,
                                      Angle,
                                      CSpine->Circle(),
                                      Wref,
                                      OrFace1,
                                      true,
                                      DisOnP);
      }
      else if (typ1 == GeomAbs_Cone && typ2 == GeomAbs_Plane)
      {
        surfok = ChFiKPart_MakeChAsym(DStr,
                                      Data,
                                      S2->Plane(),
                                      S1->Cone(),
                                      S1->FirstUParameter(),
                                      S1->LastUParameter(),
                                      Or2,
                                      Or1,
                                      dis,
                                      Angle,
                                      CSpine->Circle(),
                                      Wref,
                                      OrFace2,
                                      false,
                                      DisOnP);
      }
      else
      {
        throw Standard_NotImplemented("particular case not written");
      }
    }
  }
  return surfok;
}

//=================================================================================================

bool ChFiKPart_ComputeData::ComputeCorner(TopOpeBRepDS_DataStructure&      DStr,
                                                      const occ::handle<ChFiDS_SurfData>&   Data,
                                                      const occ::handle<Adaptor3d_Surface>& S1,
                                                      const occ::handle<Adaptor3d_Surface>& S2,
                                                      const TopAbs_Orientation         OrFace1,
                                                      const TopAbs_Orientation,
                                                      const TopAbs_Orientation Or1,
                                                      const TopAbs_Orientation Or2,
                                                      const double      minRad,
                                                      const double      majRad,
                                                      const gp_Pnt2d&          P1S1,
                                                      const gp_Pnt2d&          P2S1,
                                                      const gp_Pnt2d&          P1S2,
                                                      const gp_Pnt2d&          P2S2)
{
  bool    surfok;
  GeomAbs_SurfaceType typ1 = S1->GetType();
  GeomAbs_SurfaceType typ2 = S2->GetType();
  if (typ1 != GeomAbs_Plane)
  {
    throw Standard_ConstructionError("la face du conge torique doit etre plane");
  }
  // The guideline is the circle corresponding
  // to the section of S2, and other construction elements.

  gp_Cylinder   cyl;
  gp_Circ       circ;
  double First, Last, fu, lu;
  ChFiKPart_CornerSpine(S1, S2, P1S1, P2S1, P1S2, P2S2, majRad, cyl, circ, First, Last);
  if (typ2 == GeomAbs_Cylinder)
  {
    cyl = S2->Cylinder();
    fu  = P1S2.X();
    lu  = P2S2.X();
  }
  else
  {
    fu = First;
    lu = Last;
  }
  surfok = ChFiKPart_MakeFillet(DStr,
                                Data,
                                S1->Plane(),
                                cyl,
                                fu,
                                lu,
                                Or1,
                                Or2,
                                minRad,
                                circ,
                                First,
                                OrFace1,
                                true);
  if (surfok)
  {
    if (typ2 != GeomAbs_Cylinder)
    {
      Data->ChangeInterferenceOnS2().ChangePCurveOnFace() =
        ChFiKPart_PCurve(P1S2, P2S2, First, Last);
    }
    Data->ChangeVertexFirstOnS1().SetPoint(S1->Value(P1S1.X(), P1S1.Y()));
    Data->ChangeVertexLastOnS1().SetPoint(S1->Value(P2S1.X(), P2S1.Y()));
    Data->ChangeVertexFirstOnS2().SetPoint(S2->Value(P1S2.X(), P1S2.Y()));
    Data->ChangeVertexLastOnS2().SetPoint(S2->Value(P2S2.X(), P2S2.Y()));
    Data->ChangeInterferenceOnS1().SetFirstParameter(First);
    Data->ChangeInterferenceOnS1().SetLastParameter(Last);
    Data->ChangeInterferenceOnS2().SetFirstParameter(First);
    Data->ChangeInterferenceOnS2().SetLastParameter(Last);
    return true;
  }
  return false;
}

//=================================================================================================

bool ChFiKPart_ComputeData::ComputeCorner(TopOpeBRepDS_DataStructure&      DStr,
                                                      const occ::handle<ChFiDS_SurfData>&   Data,
                                                      const occ::handle<Adaptor3d_Surface>& S1,
                                                      const occ::handle<Adaptor3d_Surface>& S2,
                                                      const TopAbs_Orientation         OrFace1,
                                                      const TopAbs_Orientation         OrFace2,
                                                      const TopAbs_Orientation         Or1,
                                                      const TopAbs_Orientation         Or2,
                                                      const double              Rad,
                                                      const gp_Pnt2d&                  PS1,
                                                      const gp_Pnt2d&                  P1S2,
                                                      const gp_Pnt2d&                  P2S2)
{
  return ChFiKPart_Sphere(DStr, Data, S1, S2, OrFace1, OrFace2, Or1, Or2, Rad, PS1, P1S2, P2S2);
}

//=================================================================================================

bool ChFiKPart_ComputeData::ComputeCorner(TopOpeBRepDS_DataStructure&      DStr,
                                                      const occ::handle<ChFiDS_SurfData>&   Data,
                                                      const occ::handle<Adaptor3d_Surface>& S,
                                                      const occ::handle<Adaptor3d_Surface>& S1,
                                                      const occ::handle<Adaptor3d_Surface>& S2,
                                                      const TopAbs_Orientation         OfS,
                                                      const TopAbs_Orientation         OS,
                                                      const TopAbs_Orientation         OS1,
                                                      const TopAbs_Orientation         OS2,
                                                      const double              Radius)
{
  GeomAbs_SurfaceType typ  = S->GetType();
  GeomAbs_SurfaceType typ1 = S1->GetType();
  GeomAbs_SurfaceType typ2 = S2->GetType();
  if (typ != GeomAbs_Plane || typ1 != GeomAbs_Plane || typ2 != GeomAbs_Plane)
  {
    throw Standard_ConstructionError("torus joint only between the planes");
  }
  return ChFiKPart_MakeRotule(DStr,
                              Data,
                              S->Plane(),
                              S1->Plane(),
                              S2->Plane(),
                              OS,
                              OS1,
                              OS2,
                              Radius,
                              OfS);
}
