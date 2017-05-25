// Created on: 1994-08-30
// Created by: Jacques GOUSSARD
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


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Draft_EdgeInfo.hxx>
#include <Draft_FaceInfo.hxx>
#include <Draft_Modification.hxx>
#include <Draft_VertexInfo.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomProjLib.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_Type.hxx>
#include <StdFail_NotDone.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Draft_Modification,BRepTools_Modification)

//=======================================================================
//function : Draft_Modification
//purpose  : 
//=======================================================================
Draft_Modification::Draft_Modification (const TopoDS_Shape& S) :
myComp(Standard_False),myShape(S)
{
  TopExp::MapShapesAndAncestors(myShape,TopAbs_EDGE,TopAbs_FACE,myEFMap);
}



//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void Draft_Modification::Clear ()
{
  myComp = Standard_False;
  myFMap.Clear();
  myEMap.Clear();
  myVMap.Clear();
  myEFMap.Clear();
  badShape.Nullify();
  errStat = Draft_NoError;
}



//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void Draft_Modification::Init(const TopoDS_Shape& S)
{
  myShape = S;
  Clear();
  TopExp::MapShapesAndAncestors(myShape,TopAbs_EDGE,TopAbs_FACE,myEFMap);
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

Standard_Boolean Draft_Modification::Add(const TopoDS_Face& F,
  const gp_Dir& Direction,
  const Standard_Real Angle,
  const gp_Pln& NeutralPlane,
  const Standard_Boolean Flag)
{
  if (!badShape.IsNull()) {
    throw Standard_ConstructionError();
  }

  if (myComp) {
    Clear();
  }
  curFace = F;
  return InternalAdd(F,Direction,Angle,NeutralPlane, Flag);
}


//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

void Draft_Modification::Remove(const TopoDS_Face& F)
{
  if (!myFMap.Contains(F) || myComp) {
    throw Standard_NoSuchObject();
  }

  conneF.Clear();
  TopTools_ListIteratorOfListOfShape ltod;

  curFace = myFMap.FindFromKey(F).RootFace();
  for (Standard_Integer i = 1; i <= myFMap.Extent(); i++)
  {
    const TopoDS_Face& theF = myFMap.FindKey(i);
    if (myFMap.FindFromKey(theF).RootFace().IsSame(curFace)) {
      conneF.Append(theF);
      if (theF.IsSame(badShape)) {
        badShape.Nullify();
      }
    }
  }

  ltod.Initialize(conneF);
  while (ltod.More()) {
    myFMap.RemoveKey(TopoDS::Face(ltod.Value()));
    ltod.Next();
  }

  conneF.Clear();
  for (Standard_Integer i = 1; i <= myEMap.Extent(); i++)
  {
    const TopoDS_Edge& theE = myEMap.FindKey(i);
    if (myEMap.FindFromKey(theE).RootFace().IsSame(curFace))
      conneF.Append(theE);
  }
  ltod.Initialize(conneF);
  while (ltod.More()) {
    myEMap.RemoveKey(TopoDS::Edge(ltod.Value()));
    ltod.Next();
  }
}


//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean Draft_Modification::IsDone() const
{
  return myComp && badShape.IsNull();
} 


//=======================================================================
//function : Error
//purpose  : 
//=======================================================================

Draft_ErrorStatus Draft_Modification::Error() const
{
  return errStat;
} 


//=======================================================================
//function : ProblematicShape
//purpose  : 
//=======================================================================

const TopoDS_Shape& Draft_Modification::ProblematicShape() const
{
  return badShape;
} 


//=======================================================================
//function : ConnectedFaces
//purpose  : 
//=======================================================================

const TopTools_ListOfShape & Draft_Modification::ConnectedFaces(const TopoDS_Face& F)
{
  if (!myFMap.Contains(F)) {
    throw Standard_NoSuchObject();
  }
  if (!IsDone()) {
    throw StdFail_NotDone();
  }
  conneF.Clear();
  curFace = myFMap.FindFromKey(F).RootFace();

  for (Standard_Integer i = 1; i <= myFMap.Extent(); i++)
  {
    const TopoDS_Face& theF = myFMap.FindKey(i);
    if (myFMap.FindFromKey(theF).RootFace().IsSame(curFace)) {
      conneF.Append(theF);
    }
  }

  return conneF;


}


//=======================================================================
//function : ModifiedFaces
//purpose  : 
//=======================================================================

const TopTools_ListOfShape & Draft_Modification::ModifiedFaces()
{
  if (!badShape.IsNull()) {
    throw StdFail_NotDone();
  }
  conneF.Clear();

  for (Standard_Integer i = 1; i <= myFMap.Extent(); i++)
  {
    const TopoDS_Face& theF = myFMap.FindKey(i);
    if (!myFMap.FindFromKey(theF).RootFace().IsNull()) {
      conneF.Append(theF);
    }
  }

  return conneF;


}


//=======================================================================
//function : NewSurface
//purpose  : 
//=======================================================================

Standard_Boolean Draft_Modification::NewSurface(const TopoDS_Face& F,
  Handle(Geom_Surface)& S,
  TopLoc_Location& L,
  Standard_Real& Tol,
  Standard_Boolean& RevWires,
  Standard_Boolean& RevFace)
{
  if (!IsDone()) {throw Standard_DomainError();}

  if (!myFMap.Contains(F) || !myFMap.FindFromKey(F).NewGeometry()) {
    return Standard_False;
  }

  RevWires = Standard_False;
  RevFace = Standard_False;
  Tol = BRep_Tool::Tolerance(F);

  S = BRep_Tool::Surface(F,L);

  L.Identity();

  S = myFMap.FindFromKey(F).Geometry();

  return Standard_True;
}


//=======================================================================
//function : NewCurve
//purpose  : 
//=======================================================================

Standard_Boolean Draft_Modification::NewCurve(const TopoDS_Edge& E,
  Handle(Geom_Curve)& C,
  TopLoc_Location& L, 
  Standard_Real& Tol)
{
  if (!IsDone()) {throw Standard_DomainError();}

  if (!myEMap.Contains(E)) 
    return Standard_False;

  const Draft_EdgeInfo& Einf= myEMap.FindFromKey(E);
  if (!myEMap.FindFromKey(E).NewGeometry())
    return Standard_False;

  Tol = Einf.Tolerance();
  Tol = Max(Tol, BRep_Tool::Tolerance(E));
  L.Identity();
  C = myEMap.FindFromKey(E).Geometry();

  return Standard_True;

}


//=======================================================================
//function : NewPoint
//purpose  : 
//=======================================================================

Standard_Boolean Draft_Modification::NewPoint(const TopoDS_Vertex& V,
  gp_Pnt& P, 
  Standard_Real& Tol)
{
  if (!IsDone()) {throw Standard_DomainError();};

  if (!myVMap.Contains(V)) {
    return Standard_False;
  }

  Tol = BRep_Tool::Tolerance(V);
  P = myVMap.FindFromKey(V).Geometry();
  return Standard_True;
}


//=======================================================================
//function : NewCurve2d
//purpose  : 
//=======================================================================

Standard_Boolean Draft_Modification::NewCurve2d(const TopoDS_Edge& E, 
  const TopoDS_Face& F, 
  const TopoDS_Edge& NewE, 
  const TopoDS_Face&, 
  Handle(Geom2d_Curve)& C,
  Standard_Real& Tol)
{

  if (!IsDone()) {throw Standard_DomainError();};

  if (!myEMap.Contains(E)) {
    return Standard_False;
  }

  Standard_Real Fp,Lp;
  BRep_Tool::Range(NewE,Fp,Lp);

  Handle(Geom_Surface) SB = myFMap.FindFromKey(F).Geometry();

  Tol = BRep_Tool::Tolerance(E);

  const Draft_EdgeInfo& Einf = myEMap.FindFromKey(E);
  if ( Einf.FirstFace().IsSame(F) && !Einf.FirstPC().IsNull()) {
    C = Einf.FirstPC();
  }
  else if ( Einf.SecondFace().IsSame(F) && !Einf.SecondPC().IsNull()) {
    C = Einf.SecondPC();
  }
  else {

    if (!myEMap.FindFromKey(E).NewGeometry()) {
      Standard_Real Fpi,Lpi;
      BRep_Tool::Range(E,Fpi,Lpi);
      if (Fpi <= Fp && Fp <= Lpi && Fpi <= Lp && Lp <= Lpi) {
        return Standard_False;
      }
    }

    //  if (!BRep_Tool::IsClosed(E,F)) {
    BRep_Tool::Range(NewE,Fp,Lp);
    Handle(Geom_TrimmedCurve) TC = new Geom_TrimmedCurve(myEMap.FindFromKey(E).Geometry(),
      Fp,Lp);
    Fp = TC->FirstParameter();
    Lp = TC->LastParameter();
    BRep_Builder B;
    B.Range( NewE, Fp, Lp );
    C = GeomProjLib::Curve2d(TC,Fp, Lp, SB, Tol);
  }

  Handle(Standard_Type) typs = SB->DynamicType();
  if (typs == STANDARD_TYPE(Geom_RectangularTrimmedSurface) ) {
    SB = Handle(Geom_RectangularTrimmedSurface)::DownCast(SB)->BasisSurface();
    typs = SB->DynamicType();
  }

  Standard_Boolean JeRecadre = Standard_False;
  if (typs == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)) {
    Handle(Geom_Curve) aC = 
      Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(SB)->BasisCurve();
    Handle(Standard_Type) typc = aC->DynamicType();
    if (typc == STANDARD_TYPE(Geom_Circle)) JeRecadre = Standard_True;
  }

  JeRecadre = JeRecadre || 
    (typs == STANDARD_TYPE(Geom_CylindricalSurface)) || 
    (typs == STANDARD_TYPE(Geom_SphericalSurface)) || 
    (typs == STANDARD_TYPE(Geom_ConicalSurface));

  if ( JeRecadre) {
    Standard_Boolean bTranslate;
    Standard_Real aD2, aT1, aT2;
    gp_Pnt2d  PF, NewPF, aP2DT;
    gp_Vec2d aV2DT, vectra(2.*M_PI,0.);
    Handle(Geom2d_Curve) aC2DE;
    //
    aC2DE=BRep_Tool::CurveOnSurface(E, F, aT1, aT2);
    //
    PF=aC2DE->Value(0.5*(aT1+aT2));
    //
    NewPF=C->Value(0.5*(Fp+Lp));
    //
    aD2=NewPF.SquareDistance(PF);
    //
    bTranslate=Standard_False;
    if (NewPF.Translated(vectra).SquareDistance(PF) < aD2) {
      aV2DT=vectra;
      bTranslate=!bTranslate; //True
    }
    else if (NewPF.Translated(-vectra).SquareDistance(PF) < aD2) {
      aV2DT=-vectra;
      bTranslate=!bTranslate; //True
    }
    //
    if (bTranslate) {
      C->Translate(aV2DT);
    }
  }
  //
  Handle(Geom_Curve) aC3d = BRep_Tool::Curve(NewE, Fp, Lp);
  Tol = BRepTools::EvalAndUpdateTol(NewE,aC3d, C, SB, Fp, Lp);
  return Standard_True;
}


//=======================================================================
//function : NewParameter
//purpose  : 
//=======================================================================

Standard_Boolean Draft_Modification::NewParameter(const TopoDS_Vertex& V,
  const TopoDS_Edge& E,
  Standard_Real& P,
  Standard_Real& Tol)
{

  if (!IsDone()) {throw Standard_DomainError();};

  if (!myVMap.Contains(V)) {
    return Standard_False;
  }

  P = myVMap.ChangeFromKey(V).Parameter(E);
  Handle(Geom_Curve) GC = myEMap.FindFromKey(E).Geometry();
  Handle(Standard_Type) typc = GC->DynamicType();
  if (typc == STANDARD_TYPE(Geom_TrimmedCurve)) {
    GC = Handle(Geom_TrimmedCurve)::DownCast(GC);
    typc = GC->DynamicType();
  }

  if (GC->IsClosed()) {
    TopoDS_Vertex FV = TopExp::FirstVertex(E);
    Standard_Real paramf;
    if (myVMap.Contains(FV)) {
      paramf = myVMap.ChangeFromKey(FV).Parameter(E);
    }
    else {
      paramf = BRep_Tool::Parameter(FV,E);
    }

    //Patch
    Standard_Real FirstPar = GC->FirstParameter(), LastPar = GC->LastParameter();
    Standard_Real pconf = Precision::PConfusion();
    if (Abs( paramf - LastPar ) <= pconf)
    {
      paramf = FirstPar;
      FV.Orientation(E.Orientation());
      if (V.IsEqual( FV ))
        P = paramf;
    }

    FV.Orientation(E.Orientation());
    if (!V.IsEqual(FV) && P <= paramf) {
      if (GC->IsPeriodic()) {
        P += GC->Period();
      }
      else {
        P = GC->LastParameter();
      }
    }
  }

  Tol = Max (BRep_Tool::Tolerance(V), BRep_Tool::Tolerance(E));
  return Standard_True;
}



//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Draft_Modification::Continuity(const TopoDS_Edge& E,
  const TopoDS_Face& F1,
  const TopoDS_Face& F2,
  const TopoDS_Edge&,
  const TopoDS_Face&,
  const TopoDS_Face&)
{
  return BRep_Tool::Continuity(E,F1,F2);
}


