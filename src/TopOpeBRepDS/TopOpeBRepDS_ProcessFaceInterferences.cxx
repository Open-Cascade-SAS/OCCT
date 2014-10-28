// Created on: 1997-02-14
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

#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TColStd_DataMapIteratorOfDataMapOfIntegerListOfInteger.hxx>
#include <TColStd_DataMapOfIntegerListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRep_Tool.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <BRepTools.hxx>

#include <TopOpeBRepTool_ShapeTool.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#include <TopOpeBRepDS_FaceEdgeInterference.hxx>
#include <TopOpeBRepDS_define.hxx>
#include <TopOpeBRepDS_ProcessInterferencesTool.hxx>
#include <TopOpeBRepDS_FaceEdgeInterference.hxx>
#include <TopOpeBRepDS_FaceInterferenceTool.hxx>
#include <TopOpeBRepDS_ListOfShapeOn1State.hxx>
#include <TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State.hxx>
#include <TopOpeBRepTool_ShapeClassifier.hxx>
#include <TopOpeBRepTool_PShapeClassifier.hxx>

Standard_EXPORT void FUN_UNKFstasta(const TopoDS_Face& FF,const TopoDS_Face& FS,
				    const TopoDS_Edge& EE,const Standard_Boolean EEofFF,
				    TopAbs_State& stateb,TopAbs_State& statea,
                                    TopOpeBRepTool_PShapeClassifier pClassif);

#define MDShfei Handle(TopOpeBRepDS_FaceEdgeInterference)
#define MAKEFEI(IJKLM) (Handle(TopOpeBRepDS_FaceEdgeInterference)::DownCast(IJKLM))

#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRepDS_GetcontextNOPFI();
extern Standard_Boolean TopOpeBRepDS_GettracePFI();
extern Standard_Boolean TopOpeBRepDS_GettracePI();
extern Standard_Boolean TopOpeBRepDS_GettraceSPSX(const Standard_Integer);
static Standard_Boolean TRCF(const Standard_Integer F) {
  Standard_Boolean b1 = TopOpeBRepDS_GettracePFI();
  Standard_Boolean b2 = TopOpeBRepDS_GettracePI();
  Standard_Boolean b3 = TopOpeBRepDS_GettraceSPSX(F);
  return (b1 || b2 || b3);
}
static void debredunkf(const Standard_Integer /*i*/){};
#endif

Standard_EXPORT Standard_Boolean FUN_Parameters(const gp_Pnt& Pnt,const TopoDS_Shape& F,Standard_Real& u,Standard_Real& v);
Standard_EXPORT Standard_Boolean FUN_Parameters(const Standard_Real& Param,const TopoDS_Shape& E,const TopoDS_Shape& F,Standard_Real& u,Standard_Real& v);

//=======================================================================
// 3D
// purpose : The compute of transition face <F> /face <FS>,
//           or edge <E>
// prequesitory : <E> is IN 2dmatter(F) and IN 2dmatter(FS)
//=======================================================================

Standard_EXPORT Standard_Boolean FUN_mkTonF(const TopoDS_Face& F, const TopoDS_Face& FS, const TopoDS_Edge& E, 
			       TopOpeBRepDS_Transition& T)
{
  Standard_Boolean isdgE = BRep_Tool::Degenerated(E); 
  if (isdgE) return Standard_False;
  T.Set(TopAbs_UNKNOWN,TopAbs_UNKNOWN);

  Standard_Real tola = 1.e-6; // nyitol
  Standard_Real f,l; FUN_tool_bounds(E,f,l);
  const Standard_Real PAR_T = 0.456789;
  Standard_Real pmil = (1.-PAR_T)*f + PAR_T*l;
  gp_Vec tgE; 
  Standard_Boolean ok; 
  
  ok = TopOpeBRepTool_TOOL::TggeomE(pmil,E,tgE);
  //modified by NIZNHY-PKV Fri Aug  4 10:59:44 2000 f
  if (!ok) {
    return Standard_False;
  }
    //modified by NIZNHY-PKV Fri Aug  4 10:59:48 2000 t
  
  gp_Pnt2d uvF; 
  ok = FUN_tool_parF(E,pmil,F,uvF);
  if (!ok) 
    return Standard_False;
 
  gp_Pnt2d uvFS; 
  ok = FUN_tool_parF(E,pmil,FS,uvFS);
  if (!ok) 
    return Standard_False;
  
  gp_Dir ngF = FUN_tool_nggeomF(uvF,F);
  Standard_Real xx = Abs(ngF.Dot(tgE));
  Standard_Boolean tgt = (Abs(1-xx) < tola);
  if (tgt) return Standard_False;

  gp_Dir ntFS; 
  ok = TopOpeBRepTool_TOOL::Nt(uvFS,FS, ntFS);
  if (!ok) 
    return Standard_False;
  gp_Dir beafter = ngF^tgE;
  Standard_Real yy = beafter.Dot(ntFS);
  Standard_Boolean unk = (Abs(yy) < tola);
  if (unk) 
    return Standard_False;

  if (yy < 0.) T.Set(TopAbs_FORWARD);
  else         T.Set(TopAbs_REVERSED);
  return Standard_True;
}


//------------------------------------------------------
// FUN_edgeofface :  True si le edge E est un edge de F
Standard_EXPORT Standard_Boolean FUN_edgeofface
//------------------------------------------------------
(const TopoDS_Shape& E,const TopoDS_Shape& F)
{
  Standard_Boolean isv = Standard_False;
  TopExp_Explorer ex;
  for (ex.Init(F,TopAbs_EDGE); ex.More(); ex.Next())
//  for (TopExp_Explorer ex(F,TopAbs_EDGE); ex.More(); ex.Next())
    if (ex.Current().IsSame(E) ) {
      isv = Standard_True;
      break;
    }
  return isv;
}

//------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_keepFinterference
//------------------------------------------------------
(const TopOpeBRepDS_DataStructure& DS,const Handle(TopOpeBRepDS_Interference)& I,const TopoDS_Shape& F)
{
  TopOpeBRepDS_Kind GT1,ST1; Standard_Integer G1,S1; FDS_data(I,GT1,G1,ST1,S1);
  
  Standard_Boolean res = Standard_True;
  if ( I->IsKind(STANDARD_TYPE(TopOpeBRepDS_FaceEdgeInterference)) ) {
    
    const TopoDS_Shape& EG = DS.Shape(I->Geometry());
    // I rejetee si son edge-geometrie est une arete de la face qui accede I.
    Standard_Boolean k3 = ! ::FUN_edgeofface(EG,F);
    res = res && k3;
  }
  
  return res;
}

//------------------------------------------------------
Standard_EXPORT void FUN_unkeepFdoubleGBoundinterferences
//------------------------------------------------------
(TopOpeBRepDS_ListOfInterference& LI,const TopOpeBRepDS_DataStructure& /*BDS*/,
#ifdef OCCT_DEBUG
 const Standard_Integer SIX)    
#else
 const Standard_Integer )    
#endif
{
#ifdef OCCT_DEBUG
  Standard_Boolean TRC=TRCF(SIX);
#endif
//                 BDS.Shape(SIX);
  TopOpeBRepDS_ListIteratorOfListOfInterference it1;
  
  // process interferences of LI with VERTEX geometry
  
  it1.Initialize(LI);
  while (it1.More() ) {
    Handle(TopOpeBRepDS_Interference)& I1 = it1.Value();
    TopOpeBRepDS_Kind GT1,ST1; Standard_Integer G1,S1;
    const TopOpeBRepDS_Transition& T1 = I1->Transition();
    Standard_Boolean isunk1 = T1.IsUnknown();
    if (isunk1) { it1.Next(); continue; }

    FDS_data(I1,GT1,G1,ST1,S1);
    Handle(TopOpeBRepDS_ShapeShapeInterference) SSI1 = Handle(TopOpeBRepDS_ShapeShapeInterference)::DownCast(I1);
    if (SSI1.IsNull()) { it1.Next(); continue; }

    Standard_Boolean isB1 = SSI1->GBound();
    
    TopOpeBRepDS_ListIteratorOfListOfInterference it2(it1);
    it2.Next();
    Standard_Boolean cond1 = Standard_False;    
    Standard_Boolean cond2 = Standard_False;    
    
    while ( it2.More() ) {
      const Handle(TopOpeBRepDS_Interference)& I2 = it2.Value();
      TopOpeBRepDS_Kind GT2,ST2; Standard_Integer G2,S2;
      const TopOpeBRepDS_Transition& T2 = I2->Transition();
      Standard_Boolean isunk2 = T2.IsUnknown();
      if (isunk2) { it2.Next(); continue; }

      FDS_data(I2,GT2,G2,ST2,S2);
      Handle(TopOpeBRepDS_ShapeShapeInterference) SSI2 = Handle(TopOpeBRepDS_ShapeShapeInterference)::DownCast(I2);
      if ( SSI2.IsNull() ) { it2.Next(); continue; }

      Standard_Boolean isB2 = SSI2->GBound();       
      cond2 = (GT2 == GT1 && GT1 == TopOpeBRepDS_EDGE && G2 == G1 &&
	       ST2 == ST1 && ST1 == TopOpeBRepDS_FACE && S2 != S1 &&
	       isB1 && isB2);
      
      if (cond2) {
#ifdef OCCT_DEBUG
	if(TRC){cout<<"face "<<SIX<<" : G2 "<< G2 <<" GBound ";I2->Dump(cout);cout<<endl;}
#endif
	cond1 = Standard_True;
	LI.Remove(it2);
      }
      else it2.Next();
    } // it2.More()

    if (cond1) {
#ifdef OCCT_DEBUG
      if(TRC){cout<<"face "<<SIX<<" : G1 "<< G1 <<" GBound ";I1->Dump(cout);cout<<endl;}
#endif
      LI.Remove(it1);
    }
    else it1.Next();
  } // it1.More()

} // FUN_unkeepFdoubleGBoundinterferences

//------------------------------------------------------
Standard_EXPORT void FUN_resolveFUNKNOWN
//------------------------------------------------------
(TopOpeBRepDS_ListOfInterference& LI,TopOpeBRepDS_DataStructure& BDS,
 const Standard_Integer SIX,
 const TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State& MEsp,
 TopOpeBRepTool_PShapeClassifier pClassif)
{
#ifdef OCCT_DEBUG
  Standard_Boolean TRC=TRCF(SIX); if (TRC) debredunkf(SIX);
  Standard_Boolean modif = Standard_False;
#endif

  const TopoDS_Shape& F = BDS.Shape(SIX);
  TopOpeBRepDS_ListIteratorOfListOfInterference it1;
  
  const TopoDS_Face& FF = TopoDS::Face(F);
//  Standard_Real fE,lE; BRep_Tool::Range(EE,fE,lE);
  
  // process interferences of LI with UNKNOWN transition
  
  for (it1.Initialize(LI); it1.More(); it1.Next() ) {
    Handle(TopOpeBRepDS_Interference)& I1 = it1.Value();
    const TopOpeBRepDS_Transition& T1 = I1->Transition();
    Standard_Boolean isunk = T1.IsUnknown();
    if (!isunk) continue;

#ifdef OCCT_DEBUG
    modif = Standard_True;
    if(TRC){debredunkf(SIX);TCollection_AsciiString s="F";s=s+SIX+" T UNKNOWN ";I1->Dump(cout,s,"\n");}
#endif
    
    TopOpeBRepDS_Kind GT1,ST1; Standard_Integer G1,S1; TopAbs_ShapeEnum tsb1,tsa1; Standard_Integer isb1,isa1; 
    FDS_Idata(I1,tsb1,isb1,tsa1,isa1,GT1,G1,ST1,S1);
    Standard_Boolean idt = (tsb1==TopAbs_FACE && tsa1==TopAbs_FACE 
	       && GT1==TopOpeBRepDS_EDGE && ST1==TopOpeBRepDS_FACE);
    Standard_Boolean idi = (isb1==S1 && isa1==S1);
    Standard_Boolean etgf = idt && idi; // face tangent a une face en 1 edge
    if (!etgf) continue;

    const TopoDS_Edge& EE = TopoDS::Edge(BDS.Shape(G1));
    Standard_Real fE,lE; BRep_Tool::Range(EE,fE,lE);
    
#ifdef OCCT_DEBUG
    if(TRC){debredunkf(SIX);TCollection_AsciiString s="F";s=s+SIX+" etgf ";I1->Dump(cout,s,"\n");}
#endif

    Handle(TopOpeBRepDS_FaceEdgeInterference) fei = MAKEFEI(I1);
    if (fei.IsNull()) continue;

    const TopoDS_Face& FS = TopoDS::Face(BDS.Shape(S1));

//    if (!go) continue;

    // la face FF transitionne par la transition T1.IsUnknown()
    // en l'arete EE par rapport a la face FS.
    // range de EE = [fE,lE].
    // EE est une arete de FF ou non.
    // EE est une arete de section.
    // EE est une arete de couture.
    Standard_Boolean isEEGB = fei->GBound();
    Standard_Boolean isEEsp = MEsp.IsBound(EE);
    TopoDS_Edge EEsp = EE;
    if (isEEsp) {
      const TopOpeBRepDS_ListOfShapeOn1State& los1 = MEsp.Find(EE);
      isEEsp = los1.IsSplit();
      if (isEEsp) {
	const TopTools_ListOfShape& los = los1.ListOnState();
	Standard_Integer n = los.Extent();
	if ( n ) {
	  EEsp = TopoDS::Edge(los.First());
	  if (!EEsp.IsSame(EE)) isEEGB = Standard_False;
          if (n > 1) {
            // MSV: treat the case of multiple splits:
            //      select the split which lies on both faces
            TopTools_ListIteratorOfListOfShape it(los);
            for (; it.More(); it.Next()) {
              const TopoDS_Edge& aE = TopoDS::Edge(it.Value());
              Standard_Real f,l; FUN_tool_bounds(aE,f,l);
              const Standard_Real PAR_T = 0.456789;
              Standard_Real pmil = (1.-PAR_T)*f + PAR_T*l;
              gp_Pnt2d uvF; 
              if (FUN_tool_parF(aE,pmil,FF,uvF) && FUN_tool_parF(aE,pmil,FS,uvF)) {
                EEsp = aE;
                break;
              }
            }
          }
	}
      }
    }
    Standard_Boolean isSO = Standard_True;
    if (!EEsp.IsSame(EE))
      if (!FUN_tool_curvesSO(EEsp,EE,isSO)) continue;

    TopAbs_State stateb,statea;     
    TopOpeBRepDS_Transition T; Standard_Boolean ok = FUN_mkTonF(FF,FS,EEsp,T); //xpu230498
    if (ok) {stateb = T.Before(); statea =T.After();} //xpu230498
    else {
      TopOpeBRepTool_PShapeClassifier pClass = 0;
      if (pClassif) {
        // MSV: find Solids of the same object rank as FS
        //      to determine transition relatively solid rather then face
        //      if possible (see pb. in CFE002 C2, when SIX==13)
        Standard_Integer rankFS = BDS.AncestorRank(S1);
        TopoDS_Shape aSRef = BDS.Shape(rankFS);
        TopExp_Explorer ex(aSRef,TopAbs_SOLID);
        if (ex.More()) {
          pClass = pClassif;
          pClass->SetReference(aSRef);
        }
      }
      FUN_UNKFstasta(FF,FS,EEsp,isEEGB,stateb,statea,pClass);
    }
    if (stateb==TopAbs_UNKNOWN || statea==TopAbs_UNKNOWN) continue;

    TopOpeBRepDS_Transition& newT1 = I1->ChangeTransition();
    if (!isSO) {
      TopAbs_State stmp = stateb; stateb = statea; statea = stmp;
    }
    newT1.Set(stateb,statea,tsb1,tsa1);

#ifdef OCCT_DEBUG
    if(TRC){debredunkf(SIX);TCollection_AsciiString s="F";s=s+SIX+" T corrected ";I1->Dump(cout,s,"\n");}
#endif

  }

#ifdef OCCT_DEBUG
  if(TRC && modif){ FDS_dumpLI(LI,"apres correction UNKNOWN : "); }
#endif  

  FUN_unkeepUNKNOWN(LI,BDS,SIX);
  
#ifdef OCCT_DEBUG
  if(TRC && modif){ FDS_dumpLI(LI,"sans suppression UNKNOWN residuels : "); }
#endif  
}

//========================== DEB start
#ifdef OCCT_DEBUG
//Standard_IMPORT void FUN_ComputeGeomData(const TopoDS_Shape& F,const Standard_Real& u,const Standard_Real& v,gp_Dir& Norm);
//------------------------------------------------------
// Automatic testing for complex faces interference on line with cylinders and planes xpu NYI: general case
//------------------------------------------------------
Standard_Boolean TestTransition(const TopOpeBRepDS_Transition& T,const TopoDS_Face& F,const TopoDS_Edge& E,const TopoDS_Solid& So) // DEB only
{
  TopAbs_State b, a;
  b = T.Before(); a = T.After();
  
  BRepAdaptor_Curve ac(E);
  GeomAbs_CurveType ctyp = ac.GetType();
  //  TCollection_AsciiString nt; TestTopOpeDraw_TTOT::CurveToString(ctyp,nt);
  if (ctyp != GeomAbs_Line) {
    //    cout << " Case "<<nt;
    cout <<" not treated"<<endl;
    return Standard_True;
  }
  BRepAdaptor_Surface as(F);
  GeomAbs_SurfaceType styp = as.GetType();
  //  TestTopOpeDraw_TTOT::CurveToString(styp,nt);
  if (!((styp == GeomAbs_Plane)||(styp == GeomAbs_Cylinder))) {
    //    cout << " Case "<<nt;
    cout<<" not treated"<<endl;
    return Standard_True;
  }
  Standard_Real du = TopOpeBRepTool_ShapeTool::Tolerance(E); du *= 5.; // edge tolerance
  Standard_Real u, v, up, vp, p = (ac.FirstParameter() + ac.LastParameter())*.5;
  gp_Pnt pnt, pntnorm; // middle point on edge
  gp_Vec tgt, beafter; // curve tangent vector at pnt
  gp_Dir normS; // normal and direction (Before, After) on reference surface.
  
  ac.D1(p, pnt, tgt);
  Standard_Boolean prodone = FUN_Parameters(pnt, F, u, v); // DEB u, v pntnorm coordinates on F
  if (!prodone){
    cout<<" not treated : prendre un point sur le common des aretes de section SDM"<<endl;
    return Standard_True;
  }
#ifdef OCCT_DEBUG
//  FUN_ComputeGeomData (F, u, v, normS);
#endif
  normS = FUN_tool_nggeomF(gp_Pnt2d(u,v), F); 
  beafter  = normS^gp_Vec(tgt); beafter.Scale(du); // beafter of length du
  pntnorm = gp_Pnt(pnt.XYZ()).Translated(beafter); // (pnt, pntnorm) = du*beafter
  FUN_Parameters(pntnorm, F, up, vp); // DEB up, vp pntnorm coordinates on F
  
  gp_Pnt pAfter, pBefore;
  as.D0(up, vp, pAfter);
  as.D0(up-2*(up-u), vp -2*(vp-v), pBefore);
  
  Standard_Boolean transok;
  TopAbs_State stb, sta;
  BRepClass3d_SolidClassifier classif(So);
  
  classif.Perform(pBefore, Precision::Confusion()); stb = classif.State();  
  classif.Perform(pAfter, Precision::Confusion()); sta = classif.State();
  transok = (stb == b)&&(sta == a);
  
  if(!transok) {
    cout<<"States found by classifier :"<<endl;
	cout << "Before  :";
	TopAbs::Print(stb, cout);
	cout << "After  :";
	TopAbs::Print(sta, cout);
	cout << endl;
  }
  
  return transok;
}

#endif
//========================== DEB end 

