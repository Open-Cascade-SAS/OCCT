// File:	BRepPrim_Sphere.cxx
// Created:	Fri Nov  6 13:28:47 1992
// Author:	Remi LEQUETTE
//		<rle@phylox>


#include <BRepPrim_Sphere.ixx>

#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <Precision.hxx>

#include <Geom_SphericalSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom2d_Circle.hxx>

// parametres sur le meridien

#define PMIN (-0.5*PI)
#define PMAX (0.5*PI)

//=======================================================================
//function : BRepPrim_Sphere
//purpose  : 
//=======================================================================

BRepPrim_Sphere::BRepPrim_Sphere(const Standard_Real Radius) :
       BRepPrim_Revolution(gp::XOY(),PMIN,PMAX),
       myRadius(Radius)
{
  SetMeridian();
}

//=======================================================================
//function : BRepPrim_Sphere
//purpose  : 
//=======================================================================

BRepPrim_Sphere::BRepPrim_Sphere(const gp_Pnt& Center, 
				 const Standard_Real Radius) :
       BRepPrim_Revolution(gp_Ax2(Center,gp_Dir(0,0,1),gp_Dir(1,0,0)),
			   PMIN,PMAX),
       myRadius(Radius)
{
  SetMeridian();
}

//=======================================================================
//function : BRepPrim_Sphere
//purpose  : 
//=======================================================================

BRepPrim_Sphere::BRepPrim_Sphere(const gp_Ax2& Axes, 
				 const Standard_Real Radius) :
       BRepPrim_Revolution(Axes,PMIN,PMAX),
       myRadius(Radius)
{
  SetMeridian();
}

//=======================================================================
//function : MakeEmptyLateralFace
//purpose  : 
//=======================================================================

TopoDS_Face  BRepPrim_Sphere::MakeEmptyLateralFace()const
{
  Handle(Geom_SphericalSurface) S =
    new Geom_SphericalSurface(Axes(),myRadius);
  TopoDS_Face F;
  myBuilder.Builder().MakeFace(F,S,Precision::Confusion());
  return F;
}

//=======================================================================
//function : SetMeridian
//purpose  : 
//=======================================================================

void BRepPrim_Sphere::SetMeridian()
{
  // Offset the parameters on the meridian
  // to trim the edge in 3pi/2, 5pi/2

  SetMeridianOffset(2*PI);

  gp_Dir D = Axes().YDirection();
  D.Reverse();
  gp_Ax2 A(Axes().Location(),D,Axes().XDirection());
  Handle(Geom_Circle) C = new Geom_Circle(A,myRadius);
  Handle(Geom2d_Circle) C2d = 
    new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(0,0),gp_Dir2d(1,0)),
		      myRadius);
  Meridian(C,C2d);
}


