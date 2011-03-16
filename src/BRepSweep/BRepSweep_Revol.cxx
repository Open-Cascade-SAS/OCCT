// File:	BRepSweep_Revol.cxx
// Created:	Fri Jun 25 10:25:05 1993
// Author:	Laurent BOURESCHE
//		<lbo@phylox>


#include <BRepSweep_Revol.ixx>
#include <BRepSweep_Rotation.hxx>
#include <Sweep_NumShape.hxx>

#include <gp_Trsf.hxx>
#include <Precision.hxx>
//=======================================================================
//function : BRepSweep_Revol
//purpose  : 
//=======================================================================

BRepSweep_Revol::BRepSweep_Revol
  (const TopoDS_Shape& S, 
   const gp_Ax1& Ax, 
   const Standard_Real D,
   const Standard_Boolean C):
  myRotation(S.Oriented(TopAbs_FORWARD),
	     NumShape(D),
	     Location(Ax,D),
	     Axe(Ax,D),
	     Angle(D),
	     C)
{
  Standard_ConstructionError_Raise_if
    (Angle(D)<=Precision::Angular(),"BRepSweep_Revol::Constructor");
}

//=======================================================================
//function : BRepSweep_Revol
//purpose  : 
//=======================================================================

BRepSweep_Revol::BRepSweep_Revol
  (const TopoDS_Shape& S, 
   const gp_Ax1& Ax, 
   const Standard_Boolean C):
  myRotation(S.Oriented(TopAbs_FORWARD),
	     NumShape(2*PI),
	     Location(Ax,2*PI),
	     Axe(Ax,2*PI),
	     Angle(2*PI),
	     C)

{
}


//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

TopoDS_Shape  BRepSweep_Revol::Shape()
{
  return myRotation.Shape();
}


//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

TopoDS_Shape  BRepSweep_Revol::Shape(const TopoDS_Shape& aGenS)
{
  return myRotation.Shape(aGenS);
}


//=======================================================================
//function : FirstShape
//purpose  : 
//=======================================================================

TopoDS_Shape  BRepSweep_Revol::FirstShape()
{
  return myRotation.FirstShape();
}


//=======================================================================
//function : FirstShape
//purpose  : 
//=======================================================================

TopoDS_Shape  BRepSweep_Revol::FirstShape(const TopoDS_Shape& aGenS)
{
  return myRotation.FirstShape(aGenS);
}


//=======================================================================
//function : LastShape
//purpose  : 
//=======================================================================

TopoDS_Shape  BRepSweep_Revol::LastShape()
{
  return myRotation.LastShape();
}


//=======================================================================
//function : LastShape
//purpose  : 
//=======================================================================

TopoDS_Shape  BRepSweep_Revol::LastShape(const TopoDS_Shape& aGenS)
{
  return myRotation.LastShape(aGenS);
}


//=======================================================================
//function : NumShape
//purpose  : 
//=======================================================================

Sweep_NumShape  BRepSweep_Revol::NumShape(const Standard_Real D)const 
{
  Sweep_NumShape N;
  if (Abs(Angle(D) - 2*PI)<=Precision::Angular()){
    N.Init(2,TopAbs_EDGE,Standard_True,
	   Standard_False,Standard_False);
  }
  else{
    N.Init(2,TopAbs_EDGE);
  }
  return N;
}


//=======================================================================
//function : Location
//purpose  : 
//=======================================================================

TopLoc_Location  BRepSweep_Revol::Location(const gp_Ax1& Ax, 
					   const Standard_Real D)const 
{
  gp_Trsf gpt;
  gpt.SetRotation(Axe(Ax,D),Angle(D));
  TopLoc_Location L(gpt);
  return L;
}


//=======================================================================
//function : Axe
//purpose  : 
//=======================================================================

gp_Ax1  BRepSweep_Revol::Axe(const gp_Ax1& Ax, const Standard_Real D)const 
{
  gp_Ax1 A = Ax;
  if ( D < 0. ) A.Reverse();
  return A;
}


//=======================================================================
//function : Angle
//purpose  : 
//=======================================================================

Standard_Real  BRepSweep_Revol::Angle(const Standard_Real D)const 
{
  Standard_Real d = Abs(D);
  while(d>(2*PI + Precision::Angular())){
    d = d - 2*PI;
  }
  return d;
}


//=======================================================================
//function : Angle
//purpose  : 
//=======================================================================

Standard_Real  BRepSweep_Revol::Angle()const 
{
  return myRotation.Angle();
}


//=======================================================================
//function : Axe
//purpose  : 
//=======================================================================

gp_Ax1  BRepSweep_Revol::Axe()const 
{
  return myRotation.Axe();
}


