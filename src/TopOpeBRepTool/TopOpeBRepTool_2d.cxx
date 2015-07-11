// Created on: 1998-03-23
// Created by: Jean Yves LEBEY
// Copyright (c) 1998-1999 Matra Datavision
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

#include <gp_Vec2d.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Curve.hxx>
#include <ProjLib_ProjectedCurve.hxx>
#include <Geom_Surface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_HCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <TopOpeBRepTool_CurveTool.hxx>
#include <TopOpeBRepTool_DataMapOfShapeListOfC2DF.hxx>
#include <TopOpeBRepTool_C2DF.hxx>
#include <TopOpeBRepTool_ListOfC2DF.hxx>
#include <TopOpeBRepTool_ListIteratorOfListOfC2DF.hxx>
#include <TopOpeBRepTool_tol.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#ifdef DRAW
#include <TopOpeBRepTool_DRAW.hxx>
#include <Geom2d_Curve.hxx>
#endif

#ifdef OCCT_DEBUG
void debc2dnull(void) {}
Standard_EXPORT Standard_Boolean TopOpeBRepTool_GettraceC2D();
#endif

// structure e -> C2D/F
static TopOpeBRepTool_DataMapOfShapeListOfC2DF *GLOBAL_pmosloc2df = NULL;
static Standard_Integer GLOBAL_C2D_i = 0; // DEB

// structure ancetre
static TopTools_IndexedDataMapOfShapeListOfShape *GLOBAL_pidmoslosc2df = NULL; 
static TopoDS_Face *GLOBAL_pFc2df = NULL;
static TopoDS_Shape *GLOBAL_pS1c2df = NULL;
static TopoDS_Shape *GLOBAL_pS2c2df = NULL;

Standard_EXPORT Handle(Geom2d_Curve) MakePCurve(const ProjLib_ProjectedCurve& PC);

// ------------------------------------------------------------------------------------
static const TopoDS_Face& FC2D_FancestorE(const TopoDS_Edge& E)
{
  if (GLOBAL_pmosloc2df == NULL) GLOBAL_pmosloc2df = new TopOpeBRepTool_DataMapOfShapeListOfC2DF();
  Standard_Integer ancemp = (*GLOBAL_pidmoslosc2df).Extent();
  if (ancemp == 0) {
    TopExp::MapShapesAndAncestors(*GLOBAL_pS1c2df,TopAbs_EDGE,TopAbs_FACE,(*GLOBAL_pidmoslosc2df));  
    TopExp::MapShapesAndAncestors(*GLOBAL_pS2c2df,TopAbs_EDGE,TopAbs_FACE,(*GLOBAL_pidmoslosc2df));  
  }
  Standard_Boolean Eb = (*GLOBAL_pidmoslosc2df).Contains(E);
  if ( !Eb ) return *GLOBAL_pFc2df;
  const TopTools_ListOfShape& lf = (*GLOBAL_pidmoslosc2df).FindFromKey(E);
  if (lf.IsEmpty()) return *GLOBAL_pFc2df;
  const TopoDS_Face& F = TopoDS::Face(lf.First());
  return F;
}

// ------------------------------------------------------------------------------------
Standard_EXPORT int FC2D_Prepare(const TopoDS_Shape& S1,const TopoDS_Shape& S2)
{
  if (GLOBAL_pmosloc2df == NULL) GLOBAL_pmosloc2df = new TopOpeBRepTool_DataMapOfShapeListOfC2DF();
  GLOBAL_pmosloc2df->Clear();
  GLOBAL_C2D_i = 0;

  if (GLOBAL_pidmoslosc2df == NULL) GLOBAL_pidmoslosc2df = new TopTools_IndexedDataMapOfShapeListOfShape();
  GLOBAL_pidmoslosc2df->Clear();

  if (GLOBAL_pFc2df == NULL) GLOBAL_pFc2df = new TopoDS_Face();
  GLOBAL_pFc2df->Nullify();

  if (GLOBAL_pS1c2df == NULL) GLOBAL_pS1c2df = new TopoDS_Shape();
  *GLOBAL_pS1c2df = S1;

  if (GLOBAL_pS2c2df == NULL) GLOBAL_pS2c2df = new TopoDS_Shape();
  *GLOBAL_pS2c2df = S2;

  return 0;
}

// ------------------------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FC2D_HasC3D(const TopoDS_Edge& E)
{
  TopLoc_Location loc; Standard_Real f3d,l3d; Handle(Geom_Curve) C3D = BRep_Tool::Curve(E,loc,f3d,l3d);
  Standard_Boolean b = (!C3D.IsNull());
  return b;
}

// ------------------------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FC2D_HasCurveOnSurface(const TopoDS_Edge& E,const TopoDS_Face& F)
{
  Handle(Geom2d_Curve) C2D;
  Standard_Boolean hasold = FC2D_HasOldCurveOnSurface(E,F,C2D);
  Standard_Boolean hasnew = FC2D_HasNewCurveOnSurface(E,F,C2D);
  Standard_Boolean b = hasold || hasnew;
  return b;
}

// ------------------------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FC2D_HasOldCurveOnSurface(const TopoDS_Edge& E,const TopoDS_Face& F,Handle(Geom2d_Curve)& C2D,Standard_Real& f2d,Standard_Real& l2d,Standard_Real& tol)
{
  Standard_Boolean hasold = Standard_False;
  tol = BRep_Tool::Tolerance(E);
  C2D = BRep_Tool::CurveOnSurface(E,F,f2d,l2d);
  hasold = (!C2D.IsNull());
  return hasold;
}
Standard_EXPORT Standard_Boolean FC2D_HasOldCurveOnSurface(const TopoDS_Edge& E,const TopoDS_Face& F,Handle(Geom2d_Curve)& C2D)
{ Standard_Real f2d,l2d,tol; Standard_Boolean hasold = FC2D_HasOldCurveOnSurface(E,F,C2D,f2d,l2d,tol); return hasold;
}

// ------------------------------------------------------------------------------------
static TopOpeBRepTool_C2DF* FC2D_PNewCurveOnSurface(const TopoDS_Edge& E,const TopoDS_Face& F)
{
  TopOpeBRepTool_C2DF* pc2df = NULL;
  if (GLOBAL_pmosloc2df == NULL) return NULL;
  Standard_Boolean Eisb = GLOBAL_pmosloc2df->IsBound(E);
  if (!Eisb) return NULL;
  TopOpeBRepTool_ListIteratorOfListOfC2DF it(GLOBAL_pmosloc2df->Find(E));
  for(;it.More();it.Next()) {
    const TopOpeBRepTool_C2DF& c2df = it.Value();
    Standard_Boolean isf = c2df.IsFace(F);
    if (isf) {
      pc2df = (TopOpeBRepTool_C2DF*)&c2df;
      break;
    }
  }
  return pc2df;
}
Standard_EXPORT Standard_Boolean FC2D_HasNewCurveOnSurface(const TopoDS_Edge& E,const TopoDS_Face& F,Handle(Geom2d_Curve)& C2D,Standard_Real& f2d,Standard_Real& l2d,Standard_Real& tol)
{
  const TopOpeBRepTool_C2DF* pc2df = FC2D_PNewCurveOnSurface(E,F);
  Standard_Boolean hasnew = (pc2df != NULL);
  if (hasnew) C2D = pc2df->PC(f2d,l2d,tol);
  return hasnew;
}
Standard_EXPORT Standard_Boolean FC2D_HasNewCurveOnSurface(const TopoDS_Edge& E,const TopoDS_Face& F,Handle(Geom2d_Curve)& C2D)
{ Standard_Real f2d,l2d,tol; Standard_Boolean b = FC2D_HasNewCurveOnSurface(E,F,C2D,f2d,l2d,tol); return b;
}

// ------------------------------------------------------------------------------------
Standard_Integer FC2D_AddNewCurveOnSurface(Handle(Geom2d_Curve) C2D,const TopoDS_Edge& E,const TopoDS_Face& F,const Standard_Real& f2d,const Standard_Real& l2d,const Standard_Real& tol)
{
  if (C2D.IsNull()) return 1;
  TopOpeBRepTool_C2DF c2df(C2D,f2d,l2d,tol,F);
  if (GLOBAL_pmosloc2df == NULL) return 1;
  TopOpeBRepTool_ListOfC2DF thelist;
  GLOBAL_pmosloc2df->Bind(E, thelist);
  TopOpeBRepTool_ListOfC2DF& lc2df = GLOBAL_pmosloc2df->ChangeFind(E);
  lc2df.Append(c2df);
  return 0;
}

// ------------------------------------------------------------------------------------
static Handle(Geom2d_Curve) FC2D_make2d(const TopoDS_Edge& E,const TopoDS_Face& F,Standard_Real& f2d,Standard_Real& l2d,Standard_Real& tol,const Standard_Boolean trim3d = Standard_False);
static Handle(Geom2d_Curve) FC2D_make2d(const TopoDS_Edge& E,const TopoDS_Face& F,Standard_Real& f2d,Standard_Real& l2d,Standard_Real& tol,const Standard_Boolean trim3d)
{
  Handle(Geom2d_Curve) C2D = BRep_Tool::CurveOnSurface(E,F,f2d,l2d);
  if (!C2D.IsNull()) return C2D;

  // pas de 2D
  Standard_Real f3d,l3d;TopLoc_Location eloc;
  Handle(Geom_Curve) C1 = BRep_Tool::Curve(E,eloc,f3d,l3d);
  Standard_Boolean hasC3D = (!C1.IsNull());

  if ( hasC3D ) {
    Standard_Boolean elocid = eloc.IsIdentity();
    Handle(Geom_Curve) C2;
    if (elocid ) C2 = C1;
    else C2 = Handle(Geom_Curve)::DownCast(C1->Transformed(eloc.Transformation()));
    Standard_Real f=0.,l=0.;
    if (trim3d) {f=f3d; l=l3d;}
    C2D = TopOpeBRepTool_CurveTool::MakePCurveOnFace(F,C2,tol,f,l);
    f2d = f3d; l2d = l3d;
    return C2D;
  }
  else {
    // E sans courbe 2d sur F, E sans courbe 3d
    // une face accedant a E : FE
    const TopoDS_Face& FE = FC2D_FancestorE(E);
    if (FE.IsNull()) return C2D;
    Standard_Boolean compminmaxUV = Standard_False;
    BRepAdaptor_Surface BAS(F,compminmaxUV);
    Handle(BRepAdaptor_HSurface) BAHS = new BRepAdaptor_HSurface(BAS);
    BRepAdaptor_Curve AC(E,FE);
    Handle(BRepAdaptor_HCurve) AHC = new BRepAdaptor_HCurve(AC);
    Standard_Real tolin; FTOL_FaceTolerances3d(F,FE,tolin);
    ProjLib_ProjectedCurve projcurv(BAHS,AHC,tolin);    
    C2D = MakePCurve(projcurv);
    Standard_Real f,l; BRep_Tool::Range(E,f,l);
    f2d = f; l2d = l;
  }
  
#ifdef OCCT_DEBUG
  if (TopOpeBRepTool_GettraceC2D() && C2D.IsNull()) {
    cout<<"#FC2D_make2d1 --> PCurve IsNull"<<endl;
  }
#endif

  return C2D;
} // make2d1

// ------------------------------------------------------------------------------------
//modified by NIZHNY-MZV  Mon Oct  4 10:37:36 1999
Standard_EXPORT Handle(Geom2d_Curve) FC2D_MakeCurveOnSurface(const TopoDS_Edge& E,const TopoDS_Face& F,Standard_Real& f,Standard_Real& l,Standard_Real& tol,const Standard_Boolean trim3d)
{
#ifdef DRAW
  if (TopOpeBRepTool_GettraceC2D()) {
    cout<<"\n#FC2D_MakeCurveOnSurface : "<<endl;
    TCollection_AsciiString fnam("c2df");GLOBAL_C2D_i++;fnam=fnam+GLOBAL_C2D_i;FDRAW_DINS("",F,fnam,"");
    TCollection_AsciiString enam("c2de");GLOBAL_C2D_i++;enam=enam+GLOBAL_C2D_i;FDRAW_DINE(" ",E,enam,"\n");
    cout.flush(); debc2dnull();
  }
#endif
  
  Handle(Geom2d_Curve) C2D = FC2D_make2d(E,F,f,l,tol,trim3d);
  FC2D_AddNewCurveOnSurface(C2D,E,F,f,l,tol);
  return C2D;
}

// ------------------------------------------------------------------------------------
Standard_EXPORT Handle(Geom2d_Curve) FC2D_CurveOnSurface(const TopoDS_Edge& E,const TopoDS_Face& F,Standard_Real& f,Standard_Real& l,Standard_Real& tol,const Standard_Boolean trim3d)
{
  Handle(Geom2d_Curve) C2D;
  Standard_Boolean hasold = FC2D_HasOldCurveOnSurface(E,F,C2D,f,l,tol);
  if (hasold) {
    return C2D;
  }
  Standard_Boolean hasnew = FC2D_HasNewCurveOnSurface(E,F,C2D,f,l,tol);
  if (hasnew) {
    return C2D;
  }
  C2D = FC2D_MakeCurveOnSurface(E,F,f,l,tol,trim3d);
  return C2D;
}

// ------------------------------------------------------------------------------------
Standard_EXPORT Handle(Geom2d_Curve) FC2D_EditableCurveOnSurface(const TopoDS_Edge& E,const TopoDS_Face& F,Standard_Real& f,Standard_Real& l,Standard_Real& tol,const Standard_Boolean trim3d)
{
  Standard_Boolean hasold = Standard_False;
  {
    Handle(Geom2d_Curve) C2D;
    hasold = FC2D_HasOldCurveOnSurface(E,F,C2D,f,l,tol);
    if (hasold) {
      Handle(Geom2d_Curve) copC2D = Handle(Geom2d_Curve)::DownCast(C2D->Copy());
      return copC2D;
    }
  }
  Standard_Boolean hasnew = Standard_False;
  {
    Handle(Geom2d_Curve) newC2D;
    hasnew = FC2D_HasNewCurveOnSurface(E,F,newC2D,f,l,tol);
    if (hasnew) {
      return newC2D;
    }  
  }
  Handle(Geom2d_Curve) makC2D = FC2D_MakeCurveOnSurface(E,F,f,l,tol,trim3d);
  return makC2D;
}

// ------------------------------------------------------------------------------------
static void FC2D_translate(Handle(Geom2d_Curve) C2D,
//                           const TopoDS_Edge& E,
                           const TopoDS_Edge& ,
                           const TopoDS_Face& F,
                           const TopoDS_Edge& EF)
{
  TopLoc_Location sloc; const Handle(Geom_Surface)& S1 = BRep_Tool::Surface(F,sloc);
  Standard_Boolean isperio = S1->IsUPeriodic() || S1->IsVPeriodic();
  gp_Dir2d d2d; gp_Pnt2d O2d; Standard_Boolean isuiso,isviso; 
  Standard_Boolean uviso = TopOpeBRepTool_TOOL::UVISO(C2D,isuiso,isviso,d2d,O2d);
  Standard_Boolean EFnull = EF.IsNull();
  
  if (isperio && uviso && !EFnull) {
    // C2D prend comme origine dans F l'origine de la pcurve de EF dans F
    TopoDS_Face FFOR = F;
    FFOR.Orientation(TopAbs_FORWARD);
    gp_Pnt2d p1,p2; BRep_Tool::UVPoints(EF,FFOR,p1,p2);
    Standard_Real pEF = isuiso ? p1.X() : p1.Y();
    Standard_Real pC2D = isuiso ? O2d.X() : O2d.Y();
    Standard_Real factor = pEF - pC2D;
    Standard_Boolean b = (Abs(factor) > 1.e-6);
    if ( b ) {
      gp_Vec2d transl(1.,0.); if (isviso) transl = gp_Vec2d(0.,1.);
      transl.Multiply(factor);
      C2D->Translate(transl);
    }
  }
}

// ------------------------------------------------------------------------------------
static Handle(Geom2d_Curve) FC2D_make2d(const TopoDS_Edge& E,const TopoDS_Face& F,const TopoDS_Edge& EF,Standard_Real& f2d,Standard_Real& l2d,Standard_Real& tol,const Standard_Boolean trim3d = Standard_False);
static Handle(Geom2d_Curve) FC2D_make2d(const TopoDS_Edge& E,const TopoDS_Face& F,const TopoDS_Edge& EF,Standard_Real& f2d,Standard_Real& l2d,Standard_Real& tol,const Standard_Boolean trim3d)
{
  Handle(Geom2d_Curve) C2D = BRep_Tool::CurveOnSurface(E,F,f2d,l2d);
  if (!C2D.IsNull()) return C2D;

  // pas de 2D
  Standard_Real f3d,l3d;TopLoc_Location eloc;
  Handle(Geom_Curve) C1 = BRep_Tool::Curve(E,eloc,f3d,l3d);
  Standard_Boolean hasC3D = (!C1.IsNull());

  if ( hasC3D ) {
    Standard_Boolean elocid = eloc.IsIdentity();
    Handle(Geom_Curve) C2;
    if (elocid ) C2 = C1;
    else C2 = Handle(Geom_Curve)::DownCast(C1->Transformed(eloc.Transformation()));
    Standard_Real f=0.,l=0.;
    if (trim3d) {f=f3d; l=l3d;}
    C2D = TopOpeBRepTool_CurveTool::MakePCurveOnFace(F,C2,tol,f,l);
    f2d = f3d; l2d = l3d;
    FC2D_translate(C2D,E,F,EF);
    return C2D;
  }
  else {
    // E sans courbe 2d sur F, E sans courbe 3d
    // une face accedant a E : FE
    const TopoDS_Face& FE = FC2D_FancestorE(E);
    if (FE.IsNull()) return C2D;
    Standard_Boolean compminmaxUV = Standard_False;
    BRepAdaptor_Surface BAS(F,compminmaxUV);
    Handle(BRepAdaptor_HSurface) BAHS = new BRepAdaptor_HSurface(BAS);
    BRepAdaptor_Curve AC(E,FE);
    Handle(BRepAdaptor_HCurve) AHC = new BRepAdaptor_HCurve(AC);
    Standard_Real tolin; FTOL_FaceTolerances3d(F,FE,tolin);
    ProjLib_ProjectedCurve projcurv(BAHS,AHC,tolin);
    C2D = MakePCurve(projcurv);
    Standard_Real f,l; BRep_Tool::Range(E,f,l);
    f2d = f; l2d = l;
    FC2D_translate(C2D,E,F,EF);
  }

#ifdef OCCT_DEBUG
  if (TopOpeBRepTool_GettraceC2D() && C2D.IsNull()) {
    cout<<"#FC2D_make2d2 --> PCurve IsNull"<<endl;
  }
#endif

  return C2D;
} // make2d2

// ------------------------------------------------------------------------------------
Standard_EXPORT Handle(Geom2d_Curve) FC2D_CurveOnSurface(const TopoDS_Edge& E,const TopoDS_Face& F,const TopoDS_Edge& EF,Standard_Real& f2d,Standard_Real& l2d,Standard_Real& tol,const Standard_Boolean trim3d)
{
  Handle(Geom2d_Curve) C2D;

  Standard_Boolean hasold = FC2D_HasOldCurveOnSurface(E,F,C2D,f2d,l2d,tol);
  if (hasold) return C2D;
  
  TopOpeBRepTool_C2DF* pc2df = FC2D_PNewCurveOnSurface(E,F);
  if (pc2df != NULL) {
    C2D = pc2df->PC(f2d,l2d,tol);
    FC2D_translate(C2D,E,F,EF);
    pc2df->SetPC(C2D,f2d,l2d,tol);
    return C2D;
  }
  
#ifdef DRAW
  if (TopOpeBRepTool_GettraceC2D()) {
    cout<<"\n#FC2D_CurveOnSurface : "<<endl;
    TCollection_AsciiString fnam("c2df");GLOBAL_C2D_i++;fnam=fnam+GLOBAL_C2D_i;FDRAW_DINS("",F,fnam,"");
    TCollection_AsciiString enam("c2de");GLOBAL_C2D_i++;enam=enam+GLOBAL_C2D_i;FDRAW_DINE(" ",E,enam,"\n");
    cout.flush();debc2dnull();
  }
#endif
  
  C2D = FC2D_make2d(E,F,EF,f2d,l2d,tol,trim3d);
  FC2D_AddNewCurveOnSurface(C2D,E,F,f2d,l2d,tol);
  return C2D;
}
