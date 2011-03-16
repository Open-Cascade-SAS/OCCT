// File:	BRepLib_MakeFace.cxx
// Created:	Fri Jul 23 15:51:48 1993
// Author:	Remi LEQUETTE
//		<rle@nonox>


#include <BRepLib_MakeFace.ixx>

#include <Geom_Plane.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAbs_CurveType.hxx>

#include <Geom2d_Line.hxx>

#include <BRep_Builder.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>

#include <BRepTopAdaptor_FClass2d.hxx>
#include <TopAbs_State.hxx>

#include <ElCLib.hxx>
#include <Precision.hxx>
#include <BRepLib.hxx>

#include <BRepLib_FindSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>

//=======================================================================
//function : BRepLib_MakeFace
//purpose  : 
//=======================================================================

BRepLib_MakeFace::BRepLib_MakeFace() :
       myError(BRepLib_NoFace)
{
}


//=======================================================================
//function : BRepLib_MakeFace
//purpose  : 
//=======================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const TopoDS_Face& F)
{
  Init(F);
}


//=======================================================================
//function : BRepLib_MakeFace
//purpose  : 
//=======================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Pln& P)
{
  Handle(Geom_Plane) GP = new Geom_Plane(P);
  Init(GP);
}


//=======================================================================
//function : BRepLib_MakeFace
//purpose  : 
//=======================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Cylinder& C)
{
  Handle(Geom_CylindricalSurface) GC = new Geom_CylindricalSurface(C);
  Init(GC);
}


//=======================================================================
//function : BRepLib_MakeFace
//purpose  : 
//=======================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Cone& C)
{
  Handle(Geom_ConicalSurface) GC = new Geom_ConicalSurface(C);
  Init(GC);
}


//=======================================================================
//function : BRepLib_MakeFace
//purpose  : 
//=======================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Sphere& S)
{
  Handle(Geom_SphericalSurface) GS = new Geom_SphericalSurface(S);
  Init(GS);
}


//=======================================================================
//function : BRepLib_MakeFace
//purpose  : 
//=======================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Torus& T)
{
  Handle(Geom_ToroidalSurface) GT = new Geom_ToroidalSurface(T);
  Init(GT);
}


//=======================================================================
//function : BRepLib_MakeFace
//purpose  : 
//=======================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const Handle(Geom_Surface)& S)
{
  Init(S);
}


//=======================================================================
//function : BRepLib_MakeFace
//purpose  : 
//=======================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Pln& P,
				   const Standard_Real UMin,
				   const Standard_Real UMax,
				   const Standard_Real VMin,
				   const Standard_Real VMax)
{
  Handle(Geom_Plane) GP = new Geom_Plane(P);
  Init(GP,UMin,UMax,VMin,VMax);
}


//=======================================================================
//function : BRepLib_MakeFace
//purpose  : 
//=======================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Cylinder& C,
				   const Standard_Real UMin,
				   const Standard_Real UMax,
				   const Standard_Real VMin,
				   const Standard_Real VMax)
{
  Handle(Geom_CylindricalSurface) GC = new Geom_CylindricalSurface(C);
  Init(GC,UMin,UMax,VMin,VMax);
}


//=======================================================================
//function : BRepLib_MakeFace
//purpose  : 
//=======================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Cone& C,
				   const Standard_Real UMin,
				   const Standard_Real UMax,
				   const Standard_Real VMin,
				   const Standard_Real VMax)
{
  Handle(Geom_ConicalSurface) GC = new Geom_ConicalSurface(C);
  Init(GC,UMin,UMax,VMin,VMax);
}


//=======================================================================
//function : BRepLib_MakeFace
//purpose  : 
//=======================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Sphere& S,
				   const Standard_Real UMin,
				   const Standard_Real UMax,
				   const Standard_Real VMin,
				   const Standard_Real VMax)
{
  Handle(Geom_SphericalSurface) GS = new Geom_SphericalSurface(S);
  Init(GS,UMin,UMax,VMin,VMax);
}


//=======================================================================
//function : BRepLib_MakeFace
//purpose  : 
//=======================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Torus& T,
				   const Standard_Real UMin,
				   const Standard_Real UMax,
				   const Standard_Real VMin,
				   const Standard_Real VMax)
{
  Handle(Geom_ToroidalSurface) GT = new Geom_ToroidalSurface(T);
  Init(GT,UMin,UMax,VMin,VMax);
}


//=======================================================================
//function : BRepLib_MakeFace
//purpose  : 
//=======================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const Handle(Geom_Surface)& S,
				   const Standard_Real UMin,
				   const Standard_Real UMax,
				   const Standard_Real VMin,
				   const Standard_Real VMax)
{
  Init(S,UMin,UMax,VMin,VMax);
}


//=======================================================================
//function : BRepLib_MakeFace
//purpose  : 
//=======================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const TopoDS_Wire& W,
				   const Standard_Boolean OnlyPlane)

{
  // Find a surface through the wire
  BRepLib_FindSurface FS(W, -1, OnlyPlane);
  if (!FS.Found()) {
    myError = BRepLib_NotPlanar;
    return;
  }
 
  // build the face and add the wire
  BRep_Builder B;
  myError = BRepLib_FaceDone;

  Standard_Real tol = Max(1.2*FS.ToleranceReached(), FS.Tolerance());

  B.MakeFace(TopoDS::Face(myShape),FS.Surface(),FS.Location(),tol);
  Add(W);

  BRepLib::UpdateTolerances(myShape);

  CheckInside();
}


//=======================================================================
//function : BRepLib_MakeFace
//purpose  : 
//=======================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Pln& P,
				   const TopoDS_Wire& W,
				   const Standard_Boolean Inside)
{
  Handle(Geom_Plane) Pl = new Geom_Plane(P);
  Init(Pl,Standard_False);
  Add(W);
  if (Inside) CheckInside();
}


//=======================================================================
//function : BRepLib_MakeFace
//purpose  : 
//=======================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Cylinder& C,
				   const TopoDS_Wire& W,
				   const Standard_Boolean Inside)
{
  Handle(Geom_CylindricalSurface) GC = new Geom_CylindricalSurface(C);
  Init(GC,Standard_False);
  Add(W);
  if (Inside) CheckInside();
}


//=======================================================================
//function : BRepLib_MakeFace
//purpose  : 
//=======================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Cone& C,
				   const TopoDS_Wire& W,
				   const Standard_Boolean Inside)
{
  Handle(Geom_ConicalSurface) GC = new Geom_ConicalSurface(C);
  Init(GC,Standard_False);
  Add(W);
  if (Inside) CheckInside();
}


//=======================================================================
//function : BRepLib_MakeFace
//purpose  : 
//=======================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Sphere& S,
				   const TopoDS_Wire& W,
				   const Standard_Boolean Inside)
{
  Handle(Geom_SphericalSurface) GS = new Geom_SphericalSurface(S);
  Init(GS,Standard_False);
  Add(W);
  if (Inside) CheckInside();
}


//=======================================================================
//function : BRepLib_MakeFace
//purpose  : 
//=======================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Torus& T,
				   const TopoDS_Wire& W,
				   const Standard_Boolean Inside)
{
  Handle(Geom_ToroidalSurface) GT = new Geom_ToroidalSurface(T);
  Init(GT,Standard_False);
  Add(W);
  if (Inside) CheckInside();
}


//=======================================================================
//function : BRepLib_MakeFace
//purpose  : 
//=======================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const Handle(Geom_Surface)& S,
				   const TopoDS_Wire& W,
				   const Standard_Boolean Inside)
{
  Init(S,Standard_False);
  Add(W);
  if (Inside) CheckInside();
}


//=======================================================================
//function : BRepLib_MakeFace
//purpose  : 
//=======================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const TopoDS_Face& F,
				   const TopoDS_Wire& W)
{
  Init(F);
  Add(W);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepLib_MakeFace::Init(const TopoDS_Face& F)
{
  // copy the face
  myShape = F.EmptyCopied();
  myError = BRepLib_FaceDone;
  
  BRep_Builder B;
  TopoDS_Iterator It(F);
  while (It.More()) {
    B.Add(myShape,It.Value());
    It.Next();
  }
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepLib_MakeFace::Init(const Handle(Geom_Surface)& S,
			     const Standard_Boolean Bound)
{
  myError = BRepLib_FaceDone;
  if (Bound) {
    Standard_Real UMin,UMax,VMin,VMax;
    S->Bounds(UMin,UMax,VMin,VMax);
    Init(S,UMin,UMax,VMin,VMax);
  }
  else {
    BRep_Builder B;
    B.MakeFace(TopoDS::Face(myShape),S,Precision::Confusion());
  }
  BRep_Builder B;
  B.NaturalRestriction(TopoDS::Face(myShape),Standard_True);
}


//=======================================================================
//function : IsDegenerated
//purpose  : fonction statique qui verifie qu'une courbe n'est pas reduite 
//           a un point, pour ainsi coder l'edge Degenere.
//=======================================================================

static Standard_Boolean IsDegenerated(const Handle(Geom_Curve)& C,
				      const Standard_Real       tol)
{
  GeomAdaptor_Curve AC(C);
  
  GeomAbs_CurveType Type = AC.GetType();
  if (Type == GeomAbs_Circle) {
    gp_Circ Circ = AC.Circle();
    return (Circ.Radius() < tol);
  }
  else if (Type == GeomAbs_BSplineCurve) {
    Handle(Geom_BSplineCurve) BS = AC.BSpline();
    Standard_Integer NbPoles = BS->NbPoles();
    Standard_Real tol2 = tol*tol;
    gp_Pnt P1,P2;
    P1 = BS->Pole(1);
    for (Standard_Integer i = 2; i <= NbPoles; i++) {
      P2 = BS->Pole(i);
      if (P1.SquareDistance(P2) > tol2) return Standard_False;
//      P1 = P2;
    }
    return Standard_True;
  }
  else if (Type == GeomAbs_BezierCurve) {
    Handle(Geom_BezierCurve) BZ = AC.Bezier();
    Standard_Integer NbPoles = BZ->NbPoles();
    Standard_Real tol2 = tol*tol;
    gp_Pnt P1,P2;
    P1 = BZ->Pole(1);
    for (Standard_Integer i = 2; i <= NbPoles; i++) {
      P2 = BZ->Pole(i);
      if (P1.SquareDistance(P2) > tol2) return Standard_False;
//      P1 = P2;
    }
    return Standard_True;
  }
  
  return Standard_False;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepLib_MakeFace::Init(const Handle(Geom_Surface)& SS,
			     const Standard_Real Um,
			     const Standard_Real UM,
			     const Standard_Real Vm,
			     const Standard_Real VM)
{
  myError = BRepLib_FaceDone;

  Standard_Real UMin = Um;
  Standard_Real UMax = UM;
  Standard_Real VMin = Vm;
  Standard_Real VMax = VM;

  Standard_Real umin,umax,vmin,vmax,T;

  Handle(Geom_Surface) S = SS;
  Handle(Geom_RectangularTrimmedSurface) RS = 
    Handle(Geom_RectangularTrimmedSurface)::DownCast(S);
  if (!RS.IsNull())
    S = RS->BasisSurface();
  

  Standard_Boolean OffsetSurface = 
    (S->DynamicType() == STANDARD_TYPE(Geom_OffsetSurface));

  // adjust periodical surface or reordonate
  // check if the values are in the natural range
  Standard_Real epsilon = Precision::PConfusion();
  
  S->Bounds(umin,umax,vmin,vmax);

  if (OffsetSurface) {
    Handle(Geom_OffsetSurface) OS = Handle(Geom_OffsetSurface)::DownCast(S);
    Handle(Geom_Surface) Base = OS->BasisSurface();

    if (Base->DynamicType() == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)) {
      if (Precision::IsInfinite(umin) || Precision::IsInfinite(umax))
 	S = new Geom_RectangularTrimmedSurface(OS, UMin, UMax, VMin, VMax);
      else
	S = new Geom_RectangularTrimmedSurface(OS, VMin, VMax, Standard_False);
    }  else if (Base->DynamicType() == STANDARD_TYPE(Geom_SurfaceOfRevolution)) {
      if (Precision::IsInfinite(vmin) || Precision::IsInfinite(vmax))
	S = new Geom_RectangularTrimmedSurface(OS, VMin, VMax, Standard_False);
    }
  }

  if (S->IsUPeriodic()) {
    ElCLib::AdjustPeriodic(umin,umax,epsilon,UMin,UMax);
  }
  else if (UMin > UMax) {
    T = UMin;
    UMin = UMax;
    UMax = T;
    if ((umin - UMin > epsilon) || (UMax - umax > epsilon)) {
      myError = BRepLib_ParametersOutOfRange;
      return;
    }
  }

  if (S->IsVPeriodic()) {
    ElCLib::AdjustPeriodic(vmin,vmax,epsilon,VMin,VMax);
  }
  else if (VMin > VMax) {
    T = VMin;
    VMin = VMax;
    VMax = T;
    if ((vmin - VMin > epsilon) || (VMax - vmax > epsilon)) {
      myError = BRepLib_ParametersOutOfRange;
      return;
    }
  }
  

  // compute infinite flags
  Standard_Boolean umininf = Precision::IsNegativeInfinite(UMin);
  Standard_Boolean umaxinf = Precision::IsPositiveInfinite(UMax);
  Standard_Boolean vmininf = Precision::IsNegativeInfinite(VMin);
  Standard_Boolean vmaxinf = Precision::IsPositiveInfinite(VMax);
  
  // closed flag
  Standard_Boolean uclosed = 
    S->IsUClosed() && 
      Abs(UMin - umin) < epsilon && 
	Abs(UMax - umax) < epsilon;
  
  Standard_Boolean vclosed = 
    S->IsVClosed() && 
      Abs(VMin - vmin) < epsilon && 
	Abs(VMax - vmax) < epsilon;
  
  
  // compute 3d curves and degenerate flag
  Standard_Real tol = Precision::Confusion();
  Handle(Geom_Curve) Cumin,Cumax,Cvmin,Cvmax;
  Standard_Boolean Dumin,Dumax,Dvmin,Dvmax;
  Dumin = Dumax = Dvmin = Dvmax = Standard_False;

  if (!umininf) {
    Cumin = S->UIso(UMin);
    Dumin = IsDegenerated(Cumin,tol);
  }
  if (!umaxinf) {
    Cumax = S->UIso(UMax);
    Dumax = IsDegenerated(Cumax,tol);
  }
  if (!vmininf) {
    Cvmin = S->VIso(VMin);
    Dvmin = IsDegenerated(Cvmin,tol);
  }
  if (!vmaxinf) {
    Cvmax = S->VIso(VMax);
    Dvmax = IsDegenerated(Cvmax,tol);
  }

  // compute vertices
  BRep_Builder B;
  
  TopoDS_Vertex V00,V10,V11,V01;
  
  if (!umininf) {
    if (!vmininf) B.MakeVertex(V00,S->Value(UMin,VMin),tol);
    if (!vmaxinf) B.MakeVertex(V01,S->Value(UMin,VMax),tol);
  }
  if (!umaxinf) {
    if (!vmininf) B.MakeVertex(V10,S->Value(UMax,VMin),tol);
    if (!vmaxinf) B.MakeVertex(V11,S->Value(UMax,VMax),tol);
  }

  if (uclosed) {
    V10 = V00;
    V11 = V01;
  }

  if (vclosed) {
    V01 = V00;
    V11 = V10;
  }

  if (Dumin) V00 = V01;
  if (Dumax) V10 = V11;
  if (Dvmin) V00 = V10;
  if (Dvmax) V01 = V11;

  // make the lines
  Handle(Geom2d_Line) Lumin,Lumax,Lvmin,Lvmax;
  if (!umininf)
    Lumin = new Geom2d_Line(gp_Pnt2d(UMin,0),gp_Dir2d(0,1));
  if (!umaxinf)
    Lumax = new Geom2d_Line(gp_Pnt2d(UMax,0),gp_Dir2d(0,1));
  if (!vmininf)
    Lvmin = new Geom2d_Line(gp_Pnt2d(0,VMin),gp_Dir2d(1,0));
  if (!vmaxinf)
    Lvmax = new Geom2d_Line(gp_Pnt2d(0,VMax),gp_Dir2d(1,0));
  
  // make the face
  TopoDS_Face& F = TopoDS::Face(myShape);
  B.MakeFace(F,S,tol);

  // make the edges
  TopoDS_Edge eumin,eumax,evmin,evmax;

  if (!umininf) {
    if (!Dumin)
      B.MakeEdge(eumin,Cumin,tol);
    else
      B.MakeEdge(eumin);
    if (uclosed) 
      B.UpdateEdge(eumin,Lumax,Lumin,F,tol);
    else
      B.UpdateEdge(eumin,Lumin,F,tol);
    B.Degenerated(eumin,Dumin);
    if (!vmininf) {
      V00.Orientation(TopAbs_FORWARD);
      B.Add(eumin,V00);
    }
    if (!vmaxinf) {
      V01.Orientation(TopAbs_REVERSED);
      B.Add(eumin,V01);
    }
    B.Range(eumin,VMin,VMax);
  }

  if (!umaxinf) {
    if (uclosed)
      eumax = eumin;
    else {
      if (!Dumax)
	B.MakeEdge(eumax,Cumax,tol);
      else
	B.MakeEdge(eumax);
      B.UpdateEdge(eumax,Lumax,F,tol);
      B.Degenerated(eumax,Dumax);
      if (!vmininf) {
	V10.Orientation(TopAbs_FORWARD);
	B.Add(eumax,V10);
      }
      if (!vmaxinf) {
	V11.Orientation(TopAbs_REVERSED);
	B.Add(eumax,V11);
      }
      B.Range(eumax,VMin,VMax);
    }
  }

  if (!vmininf) {
    if (!Dvmin)
      B.MakeEdge(evmin,Cvmin,tol);
    else
      B.MakeEdge(evmin);
    if (vclosed)
      B.UpdateEdge(evmin,Lvmin,Lvmax,F,tol);
    else
      B.UpdateEdge(evmin,Lvmin,F,tol);
    B.Degenerated(evmin,Dvmin);
    if (!umininf) {
      V00.Orientation(TopAbs_FORWARD);
      B.Add(evmin,V00);
    }
    if (!umaxinf) {
      V10.Orientation(TopAbs_REVERSED);
      B.Add(evmin,V10);
    }
    B.Range(evmin,UMin,UMax);
  }

  if (!vmaxinf) {
    if (vclosed)
      evmax = evmin;
    else {
      if (!Dvmax)
	B.MakeEdge(evmax,Cvmax,tol);
      else
	B.MakeEdge(evmax);
      B.UpdateEdge(evmax,Lvmax,F,tol);
      B.Degenerated(evmax,Dvmax);
      if (!umininf) {
	V01.Orientation(TopAbs_FORWARD);
	B.Add(evmax,V01);
      }
      if (!umaxinf) {
	V11.Orientation(TopAbs_REVERSED);
	B.Add(evmax,V11);
      }
      B.Range(evmax,UMin,UMax);
    }
  }

  // make the wires and add them to the face
  eumin.Orientation(TopAbs_REVERSED);
  evmax.Orientation(TopAbs_REVERSED);
  
  TopoDS_Wire W;

  if (!umininf && !umaxinf && vmininf && vmaxinf) {
    // two wires in u
    B.MakeWire(W);
    B.Add(W,eumin);
    B.Add(F,W);
    B.MakeWire(W);
    B.Add(W,eumax);
    B.Add(F,W);
    F.Closed(uclosed);
  }
    
  else if (umininf && umaxinf && !vmininf && !vmaxinf) {
    // two wires in v
    B.MakeWire(W);
    B.Add(W,evmin);
    B.Add(F,W);
    B.MakeWire(W);
    B.Add(W,evmax);
    B.Add(F,W);
    F.Closed(vclosed);
  }
    
  else if (!umininf || !umaxinf || !vmininf || !vmaxinf) {
    // one wire
    B.MakeWire(W);
    if (!umininf) B.Add(W,eumin);
    if (!vmininf) B.Add(W,evmin);
    if (!umaxinf) B.Add(W,eumax);
    if (!vmaxinf) B.Add(W,evmax);
    B.Add(F,W);
    W.Closed(!umininf && !umaxinf && !vmininf && !vmaxinf);
    F.Closed(uclosed && vclosed);
  }

  if (OffsetSurface) { 
    // Les Isos sont Approximees a Precision::Approximation()
    // et on code Precision::Confusion() dans l'arete.
    // ==> Un petit passage dans SamePrameter pour regler les tolerances.
    BRepLib::SameParameter( F, tol, Standard_True);
  }

  Done();
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void  BRepLib_MakeFace::Add(const TopoDS_Wire& W)
{
  BRep_Builder B;
  B.Add(myShape,W);
  B.NaturalRestriction(TopoDS::Face(myShape),Standard_False);
  Done();
}


//=======================================================================
//function : Face
//purpose  : 
//=======================================================================

const TopoDS_Face&  BRepLib_MakeFace::Face()const 
{
  return TopoDS::Face(myShape);
}



//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepLib_MakeFace::operator TopoDS_Face() const
{
  return Face();
}

//=======================================================================
//function : Error
//purpose  : 
//=======================================================================

BRepLib_FaceError BRepLib_MakeFace::Error() const
{
  return myError;
}


//=======================================================================
//function : CheckInside
//purpose  : Reverses the current face if not a bounded area
//=======================================================================

void BRepLib_MakeFace::CheckInside()
{
  // compute the area and return the face if the area is negative
  TopoDS_Face F = TopoDS::Face(myShape);
  BRepTopAdaptor_FClass2d FClass(F,0.);
  if ( FClass.PerformInfinitePoint() == TopAbs_IN) {
    BRep_Builder B;
    TopoDS_Shape S = myShape.EmptyCopied();
    TopoDS_Iterator it(myShape);
    while (it.More()) {
      B.Add(S,it.Value().Reversed());
      it.Next();
    }
    myShape = S;
  }
}
