// Created on: 1997-11-10
// Created by: Jean Yves LEBEY
// Copyright (c) 1997-1999 Matra Datavision
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

#include <TopOpeBRepDS_define.hxx>

#include <gp_Pnt2d.hxx>
#include <TopOpeBRepDS_ProcessInterferencesTool.hxx>
#include <TopOpeBRepDS_FaceInterferenceTool.hxx>
#include <TopExp.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <TopOpeBRepTool_GEOMETRY.hxx>
#include <TopOpeBRepTool_PROJECT.hxx>
#include <TopOpeBRepTool_TOPOLOGY.hxx>
#include <TopOpeBRepTool_PShapeClassifier.hxx>
#include <TopOpeBRepTool_SC.hxx>
#include <TopOpeBRepTool_tol.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <BRepTools.hxx>
#include <GeomProjLib.hxx>
#include <Geom2d_Curve.hxx>
#include <ProjLib_ProjectedCurve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_Curve.hxx>

Standard_EXPORT occ::handle<Geom2d_Curve> MakePCurve(const ProjLib_ProjectedCurve& PC);

static TopAbs_State FUN_staPinF3d(const gp_Pnt& P, const TopoDS_Face& F)
// prequesitory : the compute of state(P,3dmatter of F)
// - solid classifier -
{
  TopAbs_State     st = TopAbs_UNKNOWN;
  gp_Pnt2d         UV;
  double    d  = 1.e2;
  bool ok = FUN_tool_projPonboundedF(P, F, UV, d);
  if (!ok)
    return st;
  double tolF = BRep_Tool::Tolerance(F);
  if (d < tolF)
    return TopAbs_IN; // TopAbs_ON;
  gp_Pnt pF;
  FUN_tool_value(UV, F, pF);
  gp_Dir ntF = FUN_tool_nggeomF(UV, F);
  if (F.Orientation() == TopAbs_REVERSED)
    ntF.Reverse();
  gp_Dir        PpF(gp_Vec(P, pF));
  double dot = ntF.Dot(PpF);
  st                = (dot > 0) ? TopAbs_IN : TopAbs_OUT;
  return st;
}

Standard_EXPORT void FUN_UNKFstasta(const TopoDS_Face&              FF,
                                    const TopoDS_Face&              FS,
                                    const TopoDS_Edge&              EE,
                                    const bool          EEofFF,
                                    TopAbs_State&                   stateb,
                                    TopAbs_State&                   statea,
                                    TopOpeBRepTool_PShapeClassifier pClassif)
{
  BRep_Builder BB;

  stateb = statea = TopAbs_UNKNOWN;
  double      fE, lE;
  occ::handle<Geom_Curve> CEE = BRep_Tool::Curve(EE, fE, lE);

  if (CEE.IsNull())
    return; // NYI : get points from 2d curve
  occ::handle<Geom_Surface> SFF = BRep_Tool::Surface(FF);

  double ttE  = 0.41237118973;
  double parE = (1 - ttE) * fE + ttE * lE;
  gp_Pnt        PE;
  gp_Vec        VE;
  CEE->D1(parE, PE, VE);

  GeomAPI_ProjectPointOnSurf PonS(PE, SFF);
  if (!PonS.Extrema().IsDone())
    return;
  if (PonS.NbPoints() == 0)
    return;

  double u, v;
  PonS.Parameters(1, u, v);
  gp_Vec d1u, d1v;
  gp_Pnt puv;
  SFF->D1(u, v, puv, d1u, d1v);
  gp_Vec        N = d1u.Crossed(d1v);
  double FUMin, FUMax, FVMin, FVMax;

  // les bornes de FF
  BRepTools::UVBounds(FF, FUMin, FUMax, FVMin, FVMax);

  // les bornes de EE dans FF
  double EUMin, EUMax, EVMin, EVMax;
  if (EEofFF)
  {
    BRepTools::UVBounds(FF, EE, EUMin, EUMax, EVMin, EVMax);
  }
  else
  { // EE n'est pas une arete de FF => EE est une arete de FS
    occ::handle<Geom2d_Curve> CEEFFx;
    if (CEE.IsNull())
    {
      bool            compminmaxUV = false;
      BRepAdaptor_Surface         BAS(FS, compminmaxUV);
      occ::handle<BRepAdaptor_Surface> BAHS = new BRepAdaptor_Surface(BAS);
      BRepAdaptor_Curve           AC(EE, FS);
      occ::handle<BRepAdaptor_Curve>   AHC = new BRepAdaptor_Curve(AC);
      double               tolin;
      FTOL_FaceTolerances3d(FF, FS, tolin);
      ProjLib_ProjectedCurve projcurv(BAHS, AHC, tolin);
      CEEFFx = MakePCurve(projcurv);
    }
    else
    {
      // modified by NIZHNY-MKK  Mon Apr  2 15:41:01 2001.BEGIN
      TopExp_Explorer anExp(FF, TopAbs_EDGE);
      for (; anExp.More(); anExp.Next())
      {
        if (EE.IsSame(anExp.Current()))
        {
          CEEFFx = BRep_Tool::CurveOnSurface(EE, FF, fE, lE);
        }
      }
      if (CEEFFx.IsNull())
      {
        // modified by NIZHNY-MKK  Mon Apr  2 15:41:16 2001.END

        CEEFFx = GeomProjLib::Curve2d(CEE, fE, lE, SFF);

        // modified by NIZHNY-MKK  Mon Apr  2 15:41:26 2001.BEGIN
      }
      // modified by NIZHNY-MKK  Mon Apr  2 15:41:31 2001.END
    }
    if (CEEFFx.IsNull())
      return;

    TopoDS_Edge EEx;
    BB.MakeEdge(EEx, CEE, BRep_Tool::Tolerance(EE));
    TopoDS_Vertex vf, vr;
    TopExp::Vertices(EE, vf, vr);
    BB.Add(EEx, vf);
    BB.UpdateVertex(vf, fE, EEx, BRep_Tool::Tolerance(vf));
    BB.Add(EEx, vr);
    BB.UpdateVertex(vr, lE, EEx, BRep_Tool::Tolerance(vr));

    TopoDS_Face FFx;
    BB.MakeFace(FFx, SFF, BRep_Tool::Tolerance(FF));
    BB.UpdateEdge(EEx, CEEFFx, FFx, BRep_Tool::Tolerance(FF));
    BRepTools::UVBounds(FFx, EEx, EUMin, EUMax, EVMin, EVMax);
  }

  //  bool EisoU = (abs(EVMax-EVMin) < Precision::Confusion());
  bool EisoU = (fabs(EVMax - EVMin) < Precision::Confusion());
  //  bool EisoV = (abs(EUMax-EUMin) < Precision::Confusion());
  bool EisoV = (fabs(EUMax - EUMin) < Precision::Confusion());
  // xpu161098 : bad analysis : we should choose smaller factor
  //   cto009C1 (FF3,FS10,EG9)
  double ttu  = 1.e-2; // double ttu = 0.1;
  double paru = fabs(ttu * (FUMax - FUMin));
  double ttv  = 1.e-2; // double ttv = 0.1;
  double parv = fabs(ttv * (FVMax - FVMin));

  double up = u;
  double vp = v;
  if (EisoV)
    up += paru;
  else if (EisoU)
    vp += parv;
  else
  {
    up += paru;
    vp += parv;
  }
  gp_Pnt Pb;
  SFF->D0(up, vp, Pb);

  double um = u;
  double vm = v;
  if (EisoV)
    um -= paru;
  else if (EisoU)
    vm -= parv;
  else
  {
    um -= paru;
    vm -= parv;
  }
  gp_Pnt Pa;
  SFF->D0(um, vm, Pa);

  bool permute = false;
  double    dot;
  gp_Vec           VEcroN = VE.Crossed(N);
  if (EisoV)
  {
    dot = VEcroN.Dot(d1u);
    if (dot < 0.)
      permute = true;
  }
  else if (EisoU)
  {
    dot = VEcroN.Dot(d1v);
    if (dot < 0.)
      permute = true;
  }
  else
  {
    dot = VEcroN.Dot(d1v);
    if (dot < 0.)
      permute = true;
  }
  if (permute)
  {
    gp_Pnt P;
    P  = Pa;
    Pa = Pb;
    Pb = P;
  }

  if (pClassif)
  {
    // xpu151098 : evolution solid classifier (ex cto009H1)
    // MSV : made it!
    pClassif->StateP3DReference(Pb);
    stateb = pClassif->State();
    pClassif->StateP3DReference(Pa);
    statea = pClassif->State();
  }
  else
  {
    stateb = ::FUN_staPinF3d(Pb, FS);
    statea = ::FUN_staPinF3d(Pa, FS);
  }
}
