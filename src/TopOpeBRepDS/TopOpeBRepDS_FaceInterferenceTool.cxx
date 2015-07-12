// Created on: 1994-11-08
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
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


#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepLProp_SLProps.hxx>
#include <Extrema_ExtPS.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <Precision.hxx>
#include <Standard_ProgramError.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_define.hxx>
#include <TopOpeBRepDS_FaceInterferenceTool.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_ProcessInterferencesTool.hxx>
#include <TopOpeBRepDS_ShapeShapeInterference.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRepDS_GettraceSTRANGE();
extern Standard_Boolean TopOpeBRepDS_GettracePEI();
extern Standard_Boolean TopOpeBRepDS_GettracePFI();
extern Standard_Boolean TopOpeBRepDS_GettracePI();
extern Standard_Boolean TopOpeBRepDS_GettraceSPSX(const Standard_Integer);
static Standard_Boolean FTRCF(const Standard_Integer F) {
  Standard_Boolean b1 = TopOpeBRepDS_GettracePFI();
  Standard_Boolean b2 = TopOpeBRepDS_GettracePI();
  Standard_Boolean b3 = TopOpeBRepDS_GettraceSPSX(F);
  return (b1 || b2 || b3);
}
//unreferenced function, commented 
/*
static Standard_Boolean FTRCE(const Standard_Integer E) {
  Standard_Boolean b1 = TopOpeBRepDS_GettracePEI();
  Standard_Boolean b2 = TopOpeBRepDS_GettracePI();
  Standard_Boolean b3 = TopOpeBRepDS_GettraceSPSX(E);
  return (b1 || b2 || b3);
}*/
extern void debredfac(const Standard_Integer I);
#endif

static Standard_Boolean STATIC_TOREVERSE = Standard_False; // xpu150498
#define M_FORWARD(ori) (ori == TopAbs_FORWARD)
#define M_REVERSED(ori) (ori == TopAbs_REVERSED)

//------------------------------------------------------
static void FUN_RaiseError(){Standard_ProgramError::Raise("TopOpeBRepDS_FaceInterferenceTool");}

//------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_Parameters
(const gp_Pnt& Pnt,const TopoDS_Shape& F,Standard_Real& u,Standard_Real& v)
{
  BRepAdaptor_Surface Surf(TopoDS::Face(F));
  // Get 2d coord of the projection of <Pnt> on surface of <F>.
  Standard_Real uvtol = Surf.Tolerance();
  Standard_Real fu=Surf.FirstUParameter(),lu=Surf.LastUParameter();
  Standard_Real fv=Surf.FirstVParameter(),lv=Surf.LastVParameter();
  Extrema_ExtPS extps(Pnt,Surf,fu,lu,fv,lv,uvtol,uvtol);
  if (!extps.IsDone()) {
    return Standard_False;
  }
  if (extps.NbExt() == 0) {
    return Standard_False;
  }
  extps.Point(1).Parameter(u,v);

  // xpu281098 : CTS21216 (FIR, f4,e7on)
  Standard_Real d2 = extps.SquareDistance(1);
  Standard_Real tolF = BRep_Tool::Tolerance(TopoDS::Face(F));
  Standard_Boolean ok = (d2 < tolF*tolF*1.e6); // NYINYI
  return ok;
}

//------------------------------------------------------
// Unused :
/*#ifdef OCCT_DEBUG
static Standard_Boolean FUN_Parameters
(const Standard_Real& Param,const TopoDS_Shape& E,const TopoDS_Shape& F,Standard_Real& u,Standard_Real& v)
{
  // Get 2d coordinates of point of param <Param> on <E> of <F>.
  BRepAdaptor_Curve2d curv2d(TopoDS::Edge(E),TopoDS::Face(F));
  gp_Pnt2d p2d;
  curv2d.D0(Param,p2d);
  p2d.Coord(u,v);
  return Standard_True;
}
#endif
*/

//------------------------------------------------------
Standard_EXPORT void FUN_ComputeGeomData
(const TopoDS_Shape& F,const gp_Pnt2d& uv,gp_Dir& Norm)
{ 
  gp_Vec ngF = FUN_tool_nggeomF(uv,TopoDS::Face(F));
  Norm = gp_Dir(ngF);
}

//------------------------------------------------------
static Standard_Boolean FUN_sphere(const TopoDS_Shape& F)
{
  Handle(Geom_Surface) su = TopOpeBRepTool_ShapeTool::BASISSURFACE(TopoDS::Face(F));
  GeomAdaptor_Surface GAS(su);  
  return (GAS.GetType() == GeomAbs_Sphere);
}

//------------------------------------------------------
//unreferenced function, commented 
/*#ifdef OCCT_DEBUG
static void FUN_middleUV(const TopoDS_Shape& F,Standard_Real& u,Standard_Real& v)
{
  Handle(Geom_Surface) su = TopOpeBRepTool_ShapeTool::BASISSURFACE(TopoDS::Face(F));
  GeomAdaptor_Surface GAS(su);
  u = (GAS.LastUParameter() + GAS.FirstUParameter())*.5;
  v = (GAS.LastVParameter() + GAS.FirstVParameter())*.5;
}
#endif*/

//------------------------------------------------------
Standard_EXPORT void FUN_ComputeGeomData
(const TopoDS_Shape& F,const gp_Pnt2d& uv,
 gp_Dir& Norm,gp_Dir& D1,gp_Dir& D2,Standard_Real& Cur1,Standard_Real& Cur2)
{    
  BRepAdaptor_Surface surf(TopoDS::Face(F));
  Standard_Real uu = uv.X(),vv = uv.Y();
  
  Standard_Boolean sphere = FUN_sphere(F);
  Standard_Boolean plane = FUN_tool_plane(F);
  
  // Getting the principle directions,the normal and the curvatures
  BRepLProp_SLProps props(surf,uu,vv,2,Precision::Confusion());
  Standard_Boolean curdef = props.IsCurvatureDefined();
  if (!curdef) Standard_ProgramError::Raise("TopOpeBRepDS_FaceInterferenceTool::Init");
  Standard_Boolean umbilic = props.IsUmbilic();
  if (umbilic) { 
    Cur1 = Cur2 = props.MeanCurvature(); 

    // xpu030998 : cto901A3
    Standard_Real toll = 1.e-8;
    Standard_Boolean ooplane = (Abs(Cur1)<toll) && (Abs(Cur2)<toll);
    plane = plane || ooplane;

    if      (plane) 
      Norm = FUN_tool_nggeomF(uv, TopoDS::Face(F));
    else if (sphere) {
      gp_Pnt center = surf.Sphere().Location();
      gp_Pnt value  = surf.Value(uu,vv);  
      Norm = gp_Dir(gp_Vec(center,value)); // recall : input data for TopTrans_SurfaceTransition
                                           //          describes "direct" geometry 
    }
    else                
      Standard_Failure::Raise("FUN_ComputeGeomData");
    
    D1 = Norm; Standard_Real x = D1.X(),y = D1.Y(),z = D1.Z(),tol = Precision::Confusion(); 
    Standard_Boolean nullx = (Abs(x)<tol),nully = (Abs(y)<tol),nullz = (Abs(z)<tol);
    if      (nullx && nully) D2 = gp_Dir(1,0,0);
    else if (nullx && nullz) D2 = gp_Dir(1,0,0);
    else if (nully && nullz) D2 = gp_Dir(0,1,0);
    else                     D2 = gp_Dir(y*z,x*z,-2.*x*y);
  }
  else {
    Cur1 = props.MaxCurvature();
    Cur2 = props.MinCurvature();
    props.CurvatureDirections(D1,D2);        
    Norm = FUN_tool_nggeomF(uv,TopoDS::Face(F));
  }
}

//=======================================================================
//function : TopOpeBRepDS_FaceInterferenceTool
//purpose  : 
//=======================================================================
TopOpeBRepDS_FaceInterferenceTool::TopOpeBRepDS_FaceInterferenceTool
(const TopOpeBRepDS_PDataStructure& PBDS)
: myPBDS(PBDS),myrefdef(Standard_False),myOnEdDef(Standard_False)
{
}

//=======================================================================
//function : Init
//purpose  : Initializes reference data for face/curve complex transition           
//=======================================================================
void TopOpeBRepDS_FaceInterferenceTool::Init
(const TopoDS_Shape& FFI,const TopoDS_Shape& EE,const Standard_Boolean EEisnew,const Handle(TopOpeBRepDS_Interference)& Iin)
{
  Handle(TopOpeBRepDS_ShapeShapeInterference) I (Handle(TopOpeBRepDS_ShapeShapeInterference)::DownCast(Iin)); if (I.IsNull()) return;  
  const TopoDS_Face& FI = TopoDS::Face(FFI);
  const TopoDS_Edge& E = TopoDS::Edge(EE);
#ifdef OCCT_DEBUG
//  Standard_Integer iFI = myPBDS->Shape(FI);
  //  Standard_Boolean TRCFI = FTRCF(iFI);if (TRCFI) debredfac(iFI);
#endif
  
  //   xpu150498
  STATIC_TOREVERSE = Standard_False;
  if (EEisnew) {
    Standard_Integer G = I->Geometry(); const TopoDS_Edge& EG = TopoDS::Edge(myPBDS->Shape(G));    
    TopOpeBRepDS_Config cf; Standard_Boolean cfok = FDS_Config3d(E,EG,cf);
    if (!cfok) { FUN_RaiseError(); return; }
    if (cf == TopOpeBRepDS_DIFFORIENTED) STATIC_TOREVERSE = Standard_True;
  } // xpu150498

  myFaceOrientation = FI.Orientation();
  myFaceOriented    = I->Support();
  
  myEdge = E;
  // Get a middle point on <E>
  // Geometric data is described locally around this point.
  // initialize : isLine,myParOnEd,myPntOnEd,myTole,Tgt.
  
  TopAbs_Orientation oEinFI; Standard_Boolean edonfa = FUN_tool_orientEinFFORWARD(E,FI,oEinFI);
//  isLine = FUN_tool_line(E);
  isLine = Standard_False;
  
  if (!myOnEdDef) {
    Standard_Boolean ok = FUN_tool_findPinE(E,myPntOnEd,myParOnEd);
    if (!ok) { FUN_RaiseError(); return;}
  }
  
  myTole = Precision::Angular();
  gp_Pnt2d uv; Standard_Boolean ok = Standard_False; Standard_Real d = 0.;
  if (edonfa) ok = FUN_tool_paronEF(E,myParOnEd,FI,uv);
  else        ok = FUN_tool_projPonF(myPntOnEd,FI,uv,d);
  if (!ok) { FUN_RaiseError(); return;}
  
  gp_Vec tmp; ok = TopOpeBRepTool_TOOL::TggeomE(myParOnEd,E,tmp);
  if (!ok) { FUN_RaiseError(); return;}
  gp_Dir Tgt(tmp);
  gp_Dir Norm;
  if(isLine) {
    FUN_ComputeGeomData(FI,uv,Norm); 
    myTool.Reset(Tgt,Norm);
  }
  else { 
    gp_Dir D1,D2;
    Standard_Real Cur1,Cur2;
    FUN_ComputeGeomData(FI,uv,Norm,D1,D2,Cur1,Cur2);
    myTool.Reset(Tgt,Norm,D1,D2,Cur1,Cur2); 
  }
  myrefdef = Standard_True;
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
void TopOpeBRepDS_FaceInterferenceTool::Add
(const TopoDS_Shape& FFI,const TopoDS_Shape& FFT,const TopoDS_Shape& EE,const Standard_Boolean EEisnew,const Handle(TopOpeBRepDS_Interference)& Iin)
{
  Handle(TopOpeBRepDS_ShapeShapeInterference) I (Handle(TopOpeBRepDS_ShapeShapeInterference)::DownCast(Iin)); if (I.IsNull()) return;  
  const TopoDS_Face& FI = TopoDS::Face(FFI);
  const TopoDS_Face& FT = TopoDS::Face(FFT);
  const TopoDS_Edge& E = TopoDS::Edge(EE);
#ifdef OCCT_DEBUG
  Standard_Integer iFI =
#endif
            myPBDS->Shape(FI);
//    myPBDS->Shape(FT);

#ifdef OCCT_DEBUG
  Standard_Boolean TRCFI = FTRCF(iFI);if (TRCFI) debredfac(iFI);
#endif

  if (!E.IsSame(myEdge)) {FUN_RaiseError();return;}

  if (!myrefdef) { 
    Init(FI,E,EEisnew,I); // premiere interference sur face orientee : Init
    return;
  }

#ifdef OCCT_DEBUG
//  TopAbs_Orientation FTori = FT.Orientation();
#endif
  TopOpeBRepDS_Kind GT,ST; Standard_Integer G,S; FDS_data(I,GT,G,ST,S);
#ifdef OCCT_DEBUG
//  Standard_Boolean gisb = I->GBound(); 
#endif
  const TopoDS_Edge& EG = TopoDS::Edge(myPBDS->Shape(G));
#ifdef OCCT_DEBUG
//  Standard_Boolean ghassd3d =
#endif
                 FDS_HasSameDomain3d(*myPBDS,EG);
#ifdef OCCT_DEBUG
//  Standard_Integer gr = myPBDS->SameDomainRef(G);
//  const TopoDS_Edge& ER = TopoDS::Edge(myPBDS->Shape(gr));
//  Standard_Integer gisr = (G == gr);
#endif
  Standard_Boolean same = !STATIC_TOREVERSE; // xpu150498
  
  TopAbs_Orientation oriloc = I->Transition().Orientation(TopAbs_IN);
  // xpu150498 : CTS20205 : sp(e5) = sp(e4 of rank=1) and c3d(e5) c3d(e4) are diff oriented
  //            As transitions on face<iFI> are given relative to the geometry of e5,
  //            we have to complement them.
  //             cto 016 E1
  Standard_Boolean rev = !same && (M_FORWARD(oriloc) || M_REVERSED(oriloc)); //xpu150498 
  if (rev) oriloc = TopAbs::Complement(oriloc); //xpu150498 

  TopAbs_Orientation oritan;
  TopAbs_Orientation oriEFT; Standard_Boolean egofft = FUN_tool_orientEinFFORWARD(EG,FT,oriEFT);
  TopAbs_Orientation oriEFI; Standard_Boolean egoffi = FUN_tool_orientEinFFORWARD(EG,FI,oriEFI);
  if      (egofft) {
    oritan = oriEFT;    
    if (EEisnew && !same) oritan = TopAbs::Complement(oriEFT);
  }
  else if (egoffi) {
    oritan = oriEFI;    
    if (EEisnew && !same) oritan = TopAbs::Complement(oriEFI);
  }
  else { FUN_RaiseError(); return; }

  gp_Pnt2d uv; Standard_Boolean ok = Standard_False;
  if (egofft) ok = FUN_tool_paronEF(E,myParOnEd,FT,uv);
  if (!ok) {Standard_Real d; ok = FUN_tool_projPonF(myPntOnEd,FT,uv,d);}
  if (!ok) { FUN_RaiseError(); return;}

  gp_Dir Norm;
  if(isLine) {
    FUN_ComputeGeomData(FT,uv,Norm);
//    if (Fori == TopAbs_REVERSED) Norm.Reverse();
    myTool.Compare(myTole,Norm,oriloc,oritan);
  }
  else {
    gp_Dir D1,D2; Standard_Real Cur1,Cur2;
    FUN_ComputeGeomData(FT,uv,Norm,D1,D2,Cur1,Cur2);
//    if (Fori == TopAbs_REVERSED) Norm.Reverse();
    myTool.Compare(myTole,Norm,D1,D2,Cur1,Cur2,oriloc,oritan); 
  }  
}

//=======================================================================
//function : Add
//purpose  :
//=======================================================================
void TopOpeBRepDS_FaceInterferenceTool::Add
//(const TopoDS_Shape& F,const TopOpeBRepDS_Curve& C,const Handle(TopOpeBRepDS_Interference)& I)
(const TopoDS_Shape& ,const TopOpeBRepDS_Curve& ,const Handle(TopOpeBRepDS_Interference)& )
{
  // NYI
}

//=======================================================================
//function : Transition
//purpose  : 
//=======================================================================
void TopOpeBRepDS_FaceInterferenceTool::Transition(const Handle(TopOpeBRepDS_Interference)& I) const 
{
  TopOpeBRepDS_Transition& T = I->ChangeTransition();

  if (myFaceOrientation == TopAbs_INTERNAL) {
    T.Set(TopAbs_IN,TopAbs_IN);
  }
  else if (myFaceOrientation == TopAbs_EXTERNAL) {
    T.Set(TopAbs_OUT,TopAbs_OUT);
  }
  else {
    I->Support(myFaceOriented);
    TopAbs_State stb = myTool.StateBefore();
    TopAbs_State sta = myTool.StateAfter();
    T.Set(stb,sta);
    //xpu150498 
    TopAbs_Orientation o = T.Orientation(TopAbs_IN);
    Standard_Boolean rev = STATIC_TOREVERSE && (M_FORWARD(o) || M_REVERSED(o));
    if (rev) o = TopAbs::Complement(o);
    T.Set(o);
    //xpu150498 
  }
}

//=======================================================================
//function : SetEdgePntPar
//purpose  : 
//=======================================================================
void TopOpeBRepDS_FaceInterferenceTool::SetEdgePntPar(const gp_Pnt& P,const Standard_Real p)
{
  myPntOnEd = P;
  myParOnEd = p;
  myOnEdDef = Standard_True;
}

//=======================================================================
//function : GetEdgePnt
//purpose  : 
//=======================================================================
void TopOpeBRepDS_FaceInterferenceTool::GetEdgePntPar(gp_Pnt& P,Standard_Real& p) const
{
  if (!myOnEdDef) Standard_ProgramError::Raise("GetEdgePntPar");
  P = myPntOnEd;
  p = myParOnEd;
}

//=======================================================================
//function : IsEdgePnt
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_FaceInterferenceTool::IsEdgePntParDef() const
{
  return myOnEdDef;
}
