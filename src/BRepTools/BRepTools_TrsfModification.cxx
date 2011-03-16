// File:	BRepTools_TrsfModification.cxx
// Created:	Thu Aug 25 16:44:16 1994
// Author:	Jacques GOUSSARD
//		<jag@ecolox>


#include <BRepTools_TrsfModification.ixx>

#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <BRep_Tool.hxx>
#include <gp_GTrsf2d.hxx>
#include <gp_TrsfForm.hxx>

#include <gp.hxx>
#include <GeomLib.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>

#include <TopoDS.hxx>
#include <Precision.hxx>

//=======================================================================
//function : BRepTools_TrsfModification
//purpose  : 
//=======================================================================

BRepTools_TrsfModification::BRepTools_TrsfModification(const gp_Trsf& T) :
myTrsf(T)
{
}


//=======================================================================
//function : Trsf
//purpose  : 
//=======================================================================

gp_Trsf& BRepTools_TrsfModification::Trsf ()
{
  return myTrsf;
}

//=======================================================================
//function : NewSurface
//purpose  : 
//=======================================================================

Standard_Boolean BRepTools_TrsfModification::NewSurface
      (const TopoDS_Face& F,
       Handle(Geom_Surface)& S,
       TopLoc_Location& L,
       Standard_Real& Tol,
       Standard_Boolean& RevWires,
       Standard_Boolean& RevFace)
{
  S = BRep_Tool::Surface(F,L);
  Tol = BRep_Tool::Tolerance(F);
  Tol *= Abs(myTrsf.ScaleFactor());
  RevWires = Standard_False;
  RevFace = myTrsf.IsNegative();

  gp_Trsf LT = L.Transformation();
  LT.Invert();
  LT.Multiply(myTrsf);
  LT.Multiply(L.Transformation());

  S = Handle(Geom_Surface)::DownCast(S->Transformed(LT));
  
  return Standard_True;
}


//=======================================================================
//function : NewCurve
//purpose  : 
//=======================================================================

Standard_Boolean BRepTools_TrsfModification::NewCurve
    (const TopoDS_Edge& E, 
     Handle(Geom_Curve)& C,
     TopLoc_Location& L, 
     Standard_Real& Tol)
{
  Standard_Real f,l;
  C = BRep_Tool::Curve(E,L,f,l);

  Tol = BRep_Tool::Tolerance(E);
  Tol *= Abs(myTrsf.ScaleFactor());

  gp_Trsf LT = L.Transformation();
  LT.Invert();
  LT.Multiply(myTrsf);
  LT.Multiply(L.Transformation());

  if (!C.IsNull()) {
    C = Handle(Geom_Curve)::DownCast(C->Transformed(LT));
  }

  return Standard_True;
}

//=======================================================================
//function : NewPoint
//purpose  : 
//=======================================================================

Standard_Boolean BRepTools_TrsfModification::NewPoint
    (const TopoDS_Vertex& V, 
     gp_Pnt& P, 
     Standard_Real& Tol)
{
  P = BRep_Tool::Pnt(V);
  Tol = BRep_Tool::Tolerance(V);
  Tol *= Abs(myTrsf.ScaleFactor());
  P.Transform(myTrsf);

  return Standard_True;
}

//=======================================================================
//function : NewCurve2d
//purpose  : 
//=======================================================================

Standard_Boolean BRepTools_TrsfModification::NewCurve2d
    (const TopoDS_Edge& E, 
     const TopoDS_Face& F, 
     const TopoDS_Edge&, 
     const TopoDS_Face&, 
     Handle(Geom2d_Curve)& C,
     Standard_Real& Tol)
{
  TopLoc_Location loc;
  Tol = BRep_Tool::Tolerance(E);
  Standard_Real scale = myTrsf.ScaleFactor();
  Tol *= Abs(scale);
  const Handle(Geom_Surface)& S = BRep_Tool::Surface(F,loc);

  Standard_Real f,l;
  Handle(Geom2d_Curve) NewC = BRep_Tool::CurveOnSurface(E,F,f,l);
  if (NewC.IsNull())
    return Standard_False;
  
  Standard_Real newf,newl;

  Handle(Standard_Type) TheType = NewC->DynamicType();
    
  if ( TheType == STANDARD_TYPE(Geom2d_TrimmedCurve)) {
    Handle(Geom2d_TrimmedCurve) TC = Handle(Geom2d_TrimmedCurve)::DownCast(NewC);
    NewC = TC->BasisCurve();
  }

  Standard_Real fc = NewC->FirstParameter(), lc = NewC->LastParameter();

  if(!NewC->IsPeriodic()) {
    if(fc - f > Precision::PConfusion()) f = fc;
    if(l - lc > Precision::PConfusion()) l = lc;
  }

  newf = f;
  newl = l;
  if (Abs(scale) != 1.) {
    
    NewC = new Geom2d_TrimmedCurve(NewC,f,l);
    gp_GTrsf2d gtrsf = S->ParametricTransformation(myTrsf);
    
    if ( gtrsf.Form() != gp_Identity) {
      NewC = GeomLib::GTransform(NewC,gtrsf);
      if (NewC.IsNull()) {
	Standard_DomainError::Raise("TrsfModification:Error in NewCurve2d");
	  return Standard_False;
	}
      newf = NewC->FirstParameter();
      newl = NewC->LastParameter();
    }
  }
  // il faut parfois recadrer les ranges 3d / 2d
  TopoDS_Vertex V1, V2;
  TopExp::Vertices(E,V1,V2);
  TopoDS_Shape initEFOR = E.Oriented(TopAbs_FORWARD); // skl
  TopoDS_Edge EFOR = TopoDS::Edge(initEFOR/*E.Oriented(TopAbs_FORWARD)*/); //skl
  NewParameter(V1,EFOR,f,Tol);
  NewParameter(V2,EFOR,l,Tol);
  GeomLib::SameRange(Tol,NewC,newf,newl,f,l,C);
  
  return Standard_True;
}

//=======================================================================
//function : NewParameter
//purpose  : 
//=======================================================================

Standard_Boolean BRepTools_TrsfModification::NewParameter
   (const TopoDS_Vertex& V, 
    const TopoDS_Edge& E, 
    Standard_Real& P, 
    Standard_Real& Tol)
{
  if (V.IsNull()) return Standard_False; // infinite edge may have Null vertex
  
  TopLoc_Location loc;
  Tol = BRep_Tool::Tolerance(V);
  Tol *= Abs(myTrsf.ScaleFactor());
  P = BRep_Tool::Parameter(V,E);

  Standard_Real f,l;

  Handle(Geom_Curve) C = BRep_Tool::Curve(E,loc,f,l);
  if (!C.IsNull()) {
    P = C->TransformedParameter(P,myTrsf);
  }

  return Standard_True;
}

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape BRepTools_TrsfModification::Continuity
  (const TopoDS_Edge& E,
   const TopoDS_Face& F1,
   const TopoDS_Face& F2,
   const TopoDS_Edge&,
   const TopoDS_Face&,
   const TopoDS_Face&)
{
  return BRep_Tool::Continuity(E,F1,F2);
}


