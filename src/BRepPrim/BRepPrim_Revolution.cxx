// File:	BRepPrim_Revolution.cxx
// Created:	Fri Nov  6 10:02:39 1992
// Author:	Remi LEQUETTE
//		<rle@phylox>



#include <BRepPrim_Revolution.ixx>

#include <Precision.hxx>
#include <Geom_SurfaceOfRevolution.hxx>

//=======================================================================
//function : BRepPrim_Revolution
//purpose  : 
//=======================================================================

BRepPrim_Revolution::BRepPrim_Revolution(const gp_Ax2& A, 
					 const Standard_Real VMin,
					 const Standard_Real VMax,
					 const Handle(Geom_Curve)& M,
					 const Handle(Geom2d_Curve)& PM) :
       BRepPrim_OneAxis(BRepPrim_Builder(),A,VMin,VMax),
       myMeridian(M),
       myPMeridian(PM)
{
}

//=======================================================================
//function : BRepPrim_Revolution
//purpose  : 
//=======================================================================

BRepPrim_Revolution::BRepPrim_Revolution(const gp_Ax2& A, 
					 const Standard_Real VMin,
					 const Standard_Real VMax) :
       BRepPrim_OneAxis(BRepPrim_Builder(),A,VMin,VMax)
{
}

//=======================================================================
//function : Meridian
//purpose  : 
//=======================================================================

void BRepPrim_Revolution::Meridian(const Handle(Geom_Curve)& M,
				   const Handle(Geom2d_Curve)& PM)
{
  myMeridian  = M;
  myPMeridian = PM;
}

//=======================================================================
//function : MakeEmptyLateralFace
//purpose  : 
//=======================================================================

TopoDS_Face  BRepPrim_Revolution::MakeEmptyLateralFace() const
{
  Handle(Geom_SurfaceOfRevolution) S =
    new Geom_SurfaceOfRevolution(myMeridian,Axes().Axis());

  TopoDS_Face F;
  myBuilder.Builder().MakeFace(F,S,Precision::Confusion());
  return F;
}

//=======================================================================
//function : MakeEmptyMeridianEdge
//purpose  : 
//=======================================================================

TopoDS_Edge  BRepPrim_Revolution::MakeEmptyMeridianEdge(const Standard_Real Ang) const 
{
  TopoDS_Edge E;
  Handle(Geom_Curve) C = Handle(Geom_Curve)::DownCast(myMeridian->Copy());
  gp_Trsf T;
  T.SetRotation(Axes().Axis(),Ang);
  C->Transform(T);
  myBuilder.Builder().MakeEdge(E,C,Precision::Confusion());
  return E;
}

//=======================================================================
//function : MeridianValue
//purpose  : 
//=======================================================================

gp_Pnt2d  BRepPrim_Revolution::MeridianValue(const Standard_Real V) const 
{
  return myPMeridian->Value(V);
}

//=======================================================================
//function : SetMeridianPCurve
//purpose  : 
//=======================================================================

void  BRepPrim_Revolution::SetMeridianPCurve(TopoDS_Edge& E, 
					     const TopoDS_Face& F) const 
{
  myBuilder.Builder().UpdateEdge(E,myPMeridian,F,Precision::Confusion());
}

