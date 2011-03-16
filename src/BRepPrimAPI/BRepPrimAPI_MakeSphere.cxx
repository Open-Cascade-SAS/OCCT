// File:	BRepPrimAPI_MakeSphere.cxx
// Created:	Fri Jul 23 15:51:53 1993
// Author:	Remi LEQUETTE
//		<rle@nonox>


#include <BRepPrimAPI_MakeSphere.ixx>
#include <BRepBuilderAPI.hxx>


#include <gp.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax2.hxx>


//-- lbr le 13 decembre 95
static gp_Ax2 SphereComputeAxes() { 
  static Standard_Integer firsttime=1;
  static Standard_Integer modif=0;
  static Standard_Real cosa=cos(0.111);
  static Standard_Real sina=sin(0.111);
  static Standard_Real ux=1.0;
  static Standard_Real uy=0.0;
  
  if(firsttime) { 
    modif = getenv("PRIM_SPHERE") != NULL;
    firsttime = 0;
  }
  if(modif) { 
    Standard_Real nux = cosa*ux+sina*uy;
    Standard_Real nuy =-sina*ux+cosa*uy;
    ux=nux; uy=nuy;
    return(gp_Ax2(gp::Origin(),gp::DZ(),gp_Dir(ux,uy,0.0)));
  }
  else { 
    return(gp::XOY());
  }
}


//=======================================================================
//function : BRepPrimAPI_MakeSphere
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeSphere::BRepPrimAPI_MakeSphere(const Standard_Real R) :
       mySphere(SphereComputeAxes(),R)
{
}


//=======================================================================
//function : BRepPrimAPI_MakeSphere
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeSphere::BRepPrimAPI_MakeSphere(const Standard_Real R, 
				       const Standard_Real angle) :
       mySphere(gp_Ax2(gp::Origin(), (angle<0.?-1:1)*gp::DZ(), gp::DX()),
		R)
{
  mySphere.Angle(Abs(angle));
}


//=======================================================================
//function : BRepPrimAPI_MakeSphere
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeSphere::BRepPrimAPI_MakeSphere(const Standard_Real R,
				       const Standard_Real angle1, 
				       const Standard_Real angle2) :
       mySphere(R)
{
  mySphere.VMin(angle1);
  mySphere.VMax(angle2);
}


//=======================================================================
//function : BRepPrimAPI_MakeSphere
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeSphere::BRepPrimAPI_MakeSphere(const Standard_Real R, 
				       const Standard_Real angle1,
				       const Standard_Real angle2, 
				       const Standard_Real angle3) :
       mySphere(R)
{
  mySphere.VMin(angle1);
  mySphere.VMax(angle2);
  mySphere.Angle(angle3);
}

//=======================================================================
//function : BRepPrimAPI_MakeSphere
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeSphere::BRepPrimAPI_MakeSphere(const gp_Pnt& Center,
				       const Standard_Real R) :
       mySphere(gp_Ax2(Center, gp_Dir(0,0,1), gp_Dir(1,0,0)),
		R)
{
}


//=======================================================================
//function : BRepPrimAPI_MakeSphere
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeSphere::BRepPrimAPI_MakeSphere(const gp_Pnt& Center,
				       const Standard_Real R, 
				       const Standard_Real angle) :
       mySphere(gp_Ax2(Center, gp_Dir(0,0,1), gp_Dir(1,0,0)),
		R)
{
  mySphere.Angle(angle);
}


//=======================================================================
//function : BRepPrimAPI_MakeSphere
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeSphere::BRepPrimAPI_MakeSphere(const gp_Pnt& Center,
				       const Standard_Real R,
				       const Standard_Real angle1,
				       const Standard_Real angle2) :
       mySphere(gp_Ax2(Center, gp_Dir(0,0,1), gp_Dir(1,0,0)),
		R)
{
  mySphere.VMin(angle1);
  mySphere.VMax(angle2);
}


//=======================================================================
//function : BRepPrimAPI_MakeSphere
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeSphere::BRepPrimAPI_MakeSphere(const gp_Pnt& Center,
				       const Standard_Real R,
				       const Standard_Real angle1,
				       const Standard_Real angle2,
				       const Standard_Real angle3) :
       mySphere(gp_Ax2(Center, gp_Dir(0,0,1), gp_Dir(1,0,0)),
		R)
{
  mySphere.VMin(angle1);
  mySphere.VMax(angle2);
  mySphere.Angle(angle3);
}


//=======================================================================
//function : BRepPrimAPI_MakeSphere
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeSphere::BRepPrimAPI_MakeSphere(const gp_Ax2& Axis,
				       const Standard_Real R) :
       mySphere(Axis, R)
{
}


//=======================================================================
//function : BRepPrimAPI_MakeSphere
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeSphere::BRepPrimAPI_MakeSphere(const gp_Ax2& Axis,
				       const Standard_Real R,
				       const Standard_Real angle) :
       mySphere( Axis, R)
{
  mySphere.Angle(angle);
}


//=======================================================================
//function : BRepPrimAPI_MakeSphere
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeSphere::BRepPrimAPI_MakeSphere(const gp_Ax2& Axis, 
				       const Standard_Real R,
				       const Standard_Real angle1, 
				       const Standard_Real angle2) :
       mySphere(Axis, R)
{
  mySphere.VMin(angle1);
  mySphere.VMax(angle2);
}


//=======================================================================
//function : BRepPrimAPI_MakeSphere
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeSphere::BRepPrimAPI_MakeSphere(const gp_Ax2& Axis, 
				       const Standard_Real R,
				       const Standard_Real angle1, 
				       const Standard_Real angle2, 
				       const Standard_Real angle3) :
       mySphere( Axis, R)
{
  mySphere.VMin(angle1);
  mySphere.VMax(angle2);
  mySphere.Angle(angle3);
}



//=======================================================================
//function : OneAxis
//purpose  : 
//=======================================================================

Standard_Address  BRepPrimAPI_MakeSphere::OneAxis()
{
  return &mySphere;
}


//=======================================================================
//function : Sphere
//purpose  : 
//=======================================================================

BRepPrim_Sphere&  BRepPrimAPI_MakeSphere::Sphere()
{
  return mySphere;
}


