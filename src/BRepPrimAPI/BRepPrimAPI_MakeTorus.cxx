// File:	BRepPrimAPI_MakeTorus.cxx
// Created:	Fri Jul 23 15:51:54 1993
// Author:	Remi LEQUETTE
//		<rle@nonox>


#include <BRepPrimAPI_MakeTorus.ixx>
#include <BRepBuilderAPI.hxx>


//=======================================================================
//function : BRepPrimAPI_MakeTorus
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeTorus::BRepPrimAPI_MakeTorus(const Standard_Real R1,
				     const Standard_Real R2) :
       myTorus(R1, R2)
{
}


//=======================================================================
//function : BRepPrimAPI_MakeTorus
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeTorus::BRepPrimAPI_MakeTorus(const Standard_Real R1, 
				     const Standard_Real R2,
				     const Standard_Real angle) :
       myTorus(R1, R2)
{
  myTorus.Angle(angle);
}


//=======================================================================
//function : BRepPrimAPI_MakeTorus
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeTorus::BRepPrimAPI_MakeTorus(const Standard_Real R1,
				     const Standard_Real R2, 
				     const Standard_Real angle1, 
				     const Standard_Real angle2) :
       myTorus( R1, R2)
{
  myTorus.VMin(angle1);
  myTorus.VMax(angle2);
}


//=======================================================================
//function : BRepPrimAPI_MakeTorus
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeTorus::BRepPrimAPI_MakeTorus(const Standard_Real R1,
				     const Standard_Real R2,
				     const Standard_Real angle1, 
				     const Standard_Real angle2, 
				     const Standard_Real angle) :
       myTorus( R1, R2)
{
  myTorus.VMin(angle1);
  myTorus.VMax(angle2);
  myTorus.Angle(angle);
}


//=======================================================================
//function : BRepPrimAPI_MakeTorus
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeTorus::BRepPrimAPI_MakeTorus(const gp_Ax2& Axes, 
				     const Standard_Real R1,
				     const Standard_Real R2) :
       myTorus(Axes, R1, R2)
{
}


//=======================================================================
//function : BRepPrimAPI_MakeTorus
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeTorus::BRepPrimAPI_MakeTorus(const gp_Ax2& Axes, 
				     const Standard_Real R1,
				     const Standard_Real R2,
				     const Standard_Real angle) :
       myTorus(Axes, R1, R2)
{
  myTorus.Angle(angle);
}


//=======================================================================
//function : BRepPrimAPI_MakeTorus
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeTorus::BRepPrimAPI_MakeTorus(const gp_Ax2& Axes, 
				     const Standard_Real R1, 
				     const Standard_Real R2, 
				     const Standard_Real angle1,
				     const Standard_Real angle2) :
       myTorus(Axes, R1, R2)
{
  myTorus.VMin(angle1);
  myTorus.VMax(angle2);
}


//=======================================================================
//function : BRepPrimAPI_MakeTorus
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeTorus::BRepPrimAPI_MakeTorus(const gp_Ax2& Axes,
				     const Standard_Real R1, 
				     const Standard_Real R2,
				     const Standard_Real angle1,
				     const Standard_Real angle2, 
				     const Standard_Real angle) :
       myTorus(Axes, R1, R2)
{
  myTorus.VMin(angle1);
  myTorus.VMax(angle2);
  myTorus.Angle(angle);
}


//=======================================================================
//function : OneAxis
//purpose  : 
//=======================================================================

Standard_Address  BRepPrimAPI_MakeTorus::OneAxis()
{
  return &myTorus;
}


//=======================================================================
//function : Torus
//purpose  : 
//=======================================================================

BRepPrim_Torus&  BRepPrimAPI_MakeTorus::Torus()
{
  return myTorus;
}


