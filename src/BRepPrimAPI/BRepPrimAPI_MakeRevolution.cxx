// File:	BRepPrimAPI_MakeRevolution.cxx
// Created:	Fri Jul 23 15:51:50 1993
// Author:	Remi LEQUETTE
//		<rle@nonox>


#include <BRepPrimAPI_MakeRevolution.ixx>
#include <BRepBuilderAPI.hxx>
#include <GeomProjLib.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Plane.hxx>

//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

static Handle(Geom2d_Curve) Project(const Handle(Geom_Curve)& M,
				    const gp_Ax3 Axis)
{
  Handle(Geom2d_Curve) C;
  C = GeomProjLib::Curve2d(M,new Geom_Plane(Axis));
  return C;
}

static Handle(Geom2d_Curve) Project(const Handle(Geom_Curve)& M)
{
  return Project(M,gp_Ax2(gp::Origin(),-gp::DY(),gp::DX()));
}

//=======================================================================
//function : BRepPrimAPI_MakeRevolution
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeRevolution::BRepPrimAPI_MakeRevolution
  (const Handle(Geom_Curve)& Meridian) :
  myRevolution(gp::XOY(),
	       Meridian->FirstParameter(),
	       Meridian->LastParameter(),
	       Meridian,
	       Project(Meridian))
{
}


//=======================================================================
//function : BRepPrimAPI_MakeRevolution
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeRevolution::BRepPrimAPI_MakeRevolution
  (const Handle(Geom_Curve)& Meridian, 
   const Standard_Real angle) :
  myRevolution(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(0,0,1),gp_Dir(1,0,0)),
	       Meridian->FirstParameter(),
	       Meridian->LastParameter(),
	       Meridian,
	       Project(Meridian))
{
  myRevolution.Angle(angle);
}


//=======================================================================
//function : BRepPrimAPI_MakeRevolution
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeRevolution::BRepPrimAPI_MakeRevolution
  (const Handle(Geom_Curve)& Meridian, 
   const Standard_Real VMin, 
   const Standard_Real VMax) :
  myRevolution(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(0,0,1),gp_Dir(1,0,0)),
	       VMin,
	       VMax,
	       Meridian,
	       Project(Meridian))
{
}


//=======================================================================
//function : BRepPrimAPI_MakeRevolution
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeRevolution::BRepPrimAPI_MakeRevolution
  (const Handle(Geom_Curve)& Meridian,
   const Standard_Real VMin, 
   const Standard_Real VMax, 
   const Standard_Real angle) :
  myRevolution(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(0,0,1),gp_Dir(1,0,0)),
	       VMin,
	       VMax,
	       Meridian,
	       Project(Meridian))
{
  myRevolution.Angle(angle);
}


//=======================================================================
//function : BRepPrimAPI_MakeRevolution
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeRevolution::BRepPrimAPI_MakeRevolution
  (const gp_Ax2& Axes, 
   const Handle(Geom_Curve)& Meridian) :
  myRevolution(Axes,
	       Meridian->FirstParameter(),
	       Meridian->LastParameter(),
	       Meridian,
	       Project(Meridian))
{
}


//=======================================================================
//function : BRepPrimAPI_MakeRevolution
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeRevolution::BRepPrimAPI_MakeRevolution
  (const gp_Ax2& Axes, 
   const Handle(Geom_Curve)& Meridian, 
   const Standard_Real angle) :
  myRevolution(Axes,
	       Meridian->FirstParameter(),
	       Meridian->LastParameter(),
	       Meridian,
	       Project(Meridian))
{
  myRevolution.Angle(angle);
}


//=======================================================================
//function : BRepPrimAPI_MakeRevolution
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeRevolution::BRepPrimAPI_MakeRevolution
  (const gp_Ax2& Axes, 
   const Handle(Geom_Curve)& Meridian, 
   const Standard_Real VMin, 
   const Standard_Real VMax) :
  myRevolution(Axes,
	       VMin,
	       VMax,
	       Meridian,
	       Project(Meridian))
{
}


//=======================================================================
//function : BRepPrimAPI_MakeRevolution
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeRevolution::BRepPrimAPI_MakeRevolution
  (const gp_Ax2& Axes, 
   const Handle(Geom_Curve)& Meridian, 
   const Standard_Real VMin, 
   const Standard_Real VMax, 
   const Standard_Real angle) :
  myRevolution(Axes,
	       VMin,
	       VMax,
	       Meridian,
	       Project(Meridian))
{
  myRevolution.Angle(angle);
}


//=======================================================================
//function : OneAxis
//purpose  : 
//=======================================================================

Standard_Address  BRepPrimAPI_MakeRevolution::OneAxis()
{
  return &myRevolution;
}


//=======================================================================
//function : Revolution
//purpose  : 
//=======================================================================

BRepPrim_Revolution&  BRepPrimAPI_MakeRevolution::Revolution()
{
  return myRevolution;
}


