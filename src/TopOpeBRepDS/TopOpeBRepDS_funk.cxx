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
#include <TopOpeBRepDS_FaceEdgeInterference.hxx>
#include <TopOpeBRepDS_FaceInterferenceTool.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TColStd_DataMapIteratorOfDataMapOfIntegerListOfInteger.hxx>
#include <TColStd_DataMapOfIntegerListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <TopOpeBRepDS_FaceEdgeInterference.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_SC.hxx>
#include <TopOpeBRepTool_box.hxx>
#include <TopOpeBRepTool_tol.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <BRepTools.hxx>
#include <GeomProjLib.hxx>
#include <Geom2d_Curve.hxx>
#include <ProjLib_ProjectedCurve.hxx>
#include <Geom_Surface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_HCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HCurve.hxx>

Standard_EXPORT Handle(Geom2d_Curve) MakePCurve(const ProjLib_ProjectedCurve& PC);

static TopAbs_State FUN_staPinF3d(const gp_Pnt& P, const TopoDS_Face& F)
// prequesitory : the compute of state(P,3dmatter of F)     
// - solid classifier - 
{
  TopAbs_State st = TopAbs_UNKNOWN;
  gp_Pnt2d UV; Standard_Real d = 1.e2; Standard_Boolean ok = FUN_tool_projPonboundedF(P,F,UV,d);
  if (!ok) return st;
  Standard_Real tolF = BRep_Tool::Tolerance(F);
  if (d < tolF) return TopAbs_IN; // TopAbs_ON;
  gp_Pnt pF; FUN_tool_value(UV,F,pF);
  gp_Dir ntF = FUN_tool_nggeomF(UV,F);
  if (F.Orientation() == TopAbs_REVERSED) ntF.Reverse();
  gp_Dir PpF(gp_Vec(P,pF));
  Standard_Real dot = ntF.Dot(PpF);
  st = (dot > 0) ? TopAbs_IN : TopAbs_OUT;
  return st;
}

Standard_EXPORT void FUN_UNKFstasta(const TopoDS_Face& FF,const TopoDS_Face& FS,
				    const TopoDS_Edge& EE,const Standard_Boolean EEofFF,
				    TopAbs_State& stateb,TopAbs_State& statea,
                                    TopOpeBRepTool_PShapeClassifier pClassif)
{
  BRep_Builder BB;
  
  stateb = statea = TopAbs_UNKNOWN;
  Standard_Real fE,lE; Handle(Geom_Curve) CEE = BRep_Tool::Curve(EE,fE,lE);
  
  if (CEE.IsNull()) return; // NYI : get points from 2d curve
  Handle(Geom_Surface) SFF = BRep_Tool::Surface(FF);
  
  Standard_Real ttE = 0.41237118973; Standard_Real parE = (1-ttE)*fE + ttE*lE;
  gp_Pnt PE;gp_Vec VE;CEE->D1(parE,PE,VE);
  
  GeomAPI_ProjectPointOnSurf PonS(PE,SFF);
  if (!PonS.Extrema().IsDone()) return;
  if (PonS.NbPoints() == 0) return;
  
  Standard_Real u,v; PonS.Parameters(1,u,v);
  gp_Vec d1u,d1v;gp_Pnt puv; SFF->D1(u,v,puv,d1u,d1v);
  gp_Vec N = d1u.Crossed(d1v);
  Standard_Real FUMin,FUMax,FVMin,FVMax;
  
  // les bornes de FF
  BRepTools::UVBounds(FF,FUMin,FUMax,FVMin,FVMax);
  
  // les bornes de EE dans FF
  Standard_Real EUMin,EUMax,EVMin,EVMax;
  if (EEofFF) {
    BRepTools::UVBounds(FF,EE,EUMin,EUMax,EVMin,EVMax);
  }
  else { // EE n'est pas une arete de FF => EE est une arete de FS
    Handle(Geom2d_Curve) CEEFFx;
    if (CEE.IsNull()) {
      Standard_Boolean compminmaxUV = Standard_False;
      BRepAdaptor_Surface BAS(FS,compminmaxUV);
      Handle(BRepAdaptor_HSurface) BAHS = new BRepAdaptor_HSurface(BAS);
      BRepAdaptor_Curve AC(EE,FS);
      Handle(BRepAdaptor_HCurve) AHC = new BRepAdaptor_HCurve(AC);
      Standard_Real tolin; FTOL_FaceTolerances3d(FF,FS,tolin);
      ProjLib_ProjectedCurve projcurv(BAHS,AHC,tolin);
      CEEFFx = MakePCurve(projcurv);
    }
    else {
      // modified by NIZHNY-MKK  Mon Apr  2 15:41:01 2001.BEGIN
      TopExp_Explorer anExp(FF, TopAbs_EDGE);
      for(; anExp.More(); anExp.Next()) {
        if(EE.IsSame(anExp.Current())) {
	  CEEFFx = BRep_Tool::CurveOnSurface(EE, FF, fE, lE);
	}
      }
      if(CEEFFx.IsNull()) {
	// modified by NIZHNY-MKK  Mon Apr  2 15:41:16 2001.END

	CEEFFx = GeomProjLib::Curve2d(CEE,fE,lE,SFF);

      // modified by NIZHNY-MKK  Mon Apr  2 15:41:26 2001.BEGIN
      }
      // modified by NIZHNY-MKK  Mon Apr  2 15:41:31 2001.END
    }
    if (CEEFFx.IsNull()) return;

    TopoDS_Edge EEx; BB.MakeEdge(EEx,CEE,BRep_Tool::Tolerance(EE));
    TopoDS_Vertex vf,vr; TopExp::Vertices(EE,vf,vr);
    BB.Add(EEx,vf); BB.UpdateVertex(vf,fE,EEx,BRep_Tool::Tolerance(vf));
    BB.Add(EEx,vr); BB.UpdateVertex(vr,lE,EEx,BRep_Tool::Tolerance(vr));

    TopoDS_Face FFx; BB.MakeFace(FFx,SFF,BRep_Tool::Tolerance(FF));
    BB.UpdateEdge(EEx,CEEFFx,FFx,BRep_Tool::Tolerance(FF));
    BRepTools::UVBounds(FFx,EEx,EUMin,EUMax,EVMin,EVMax);
  }

  //  Standard_Boolean EisoU = (abs(EVMax-EVMin) < Precision::Confusion()); 
  Standard_Boolean EisoU = (fabs(EVMax-EVMin) < Precision::Confusion()); 
  //  Standard_Boolean EisoV = (abs(EUMax-EUMin) < Precision::Confusion()); 
  Standard_Boolean EisoV = (fabs(EUMax-EUMin) < Precision::Confusion()); 
  // xpu161098 : bad analysis : we should choose smaller factor
  //   cto009C1 (FF3,FS10,EG9)
  Standard_Real ttu = 1.e-2; //Standard_Real ttu = 0.1; 
  Standard_Real paru = fabs(ttu*(FUMax-FUMin));
  Standard_Real ttv = 1.e-2;//Standard_Real ttv = 0.1; 
  Standard_Real parv = fabs(ttv*(FVMax-FVMin));
  
  Standard_Real up = u; Standard_Real vp = v;
  if      ( EisoV ) up += paru;
  else if ( EisoU ) vp += parv;
  else { up += paru; vp += parv; }
  gp_Pnt Pb; SFF->D0(up,vp,Pb);
  
  Standard_Real um = u; Standard_Real vm = v;
  if      ( EisoV ) um -= paru;
  else if ( EisoU ) vm -= parv;
  else { um -= paru; vm -= parv; }
  gp_Pnt Pa; SFF->D0(um,vm,Pa);
  
  Standard_Boolean permute = Standard_False;
  Standard_Real dot;
  gp_Vec VEcroN = VE.Crossed(N);
  if      ( EisoV ) {
    dot = VEcroN.Dot(d1u);
    if ( dot < 0.) permute = Standard_True;
  }
  else if ( EisoU ) {
    dot = VEcroN.Dot(d1v);
    if ( dot < 0.) permute = Standard_True;
  }
  else {
    dot = VEcroN.Dot(d1v);
    if ( dot < 0.) permute = Standard_True;
  }
  if (permute) {
    gp_Pnt P;
    P = Pa;
    Pa = Pb;
    Pb = P;
  }

  if (pClassif) {
    // xpu151098 : evolution solid classifier (ex cto009H1)
    // MSV : made it!
    pClassif->StateP3DReference(Pb);
    stateb = pClassif->State();
    pClassif->StateP3DReference(Pa);
    statea = pClassif->State();
  }
  else {
    stateb = ::FUN_staPinF3d(Pb,FS);
    statea = ::FUN_staPinF3d(Pa,FS);
  }
}
