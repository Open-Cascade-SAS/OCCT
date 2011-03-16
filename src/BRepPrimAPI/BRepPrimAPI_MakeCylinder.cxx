// File:	BRepPrimAPI_MakeCylinder.cxx
// Created:	Fri Jul 23 15:51:45 1993
// Author:	Remi LEQUETTE
//		<rle@nonox>


#include <BRepPrimAPI_MakeCylinder.ixx>
#include <BRepBuilderAPI.hxx>


#include <gp.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax2.hxx>

//-- lbr le 13 decembre 95
static gp_Ax2 CylinderComputeAxes() { 
  static Standard_Integer firsttime=1;
  static Standard_Integer modif=0;
  static Standard_Real cosa=cos(0.1);
  static Standard_Real sina=sin(0.1);
  static Standard_Real ux=1.0;
  static Standard_Real uy=0.0;
  
  if(firsttime) { 
    modif = getenv("PRIM_CYLINDER") != NULL;
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
//function : BRepPrimAPI_MakeCylinder
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeCylinder::BRepPrimAPI_MakeCylinder(const Standard_Real R,
					   const Standard_Real H) :
       myCylinder(CylinderComputeAxes(), R , H)
{
}


//=======================================================================
//function : BRepPrimAPI_MakeCylinder
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeCylinder::BRepPrimAPI_MakeCylinder(const Standard_Real R, 
					   const Standard_Real H, 
					   const Standard_Real Angle) :
       myCylinder( R , H)
{
  myCylinder.Angle(Angle);
}


//=======================================================================
//function : BRepPrimAPI_MakeCylinder
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeCylinder::BRepPrimAPI_MakeCylinder(const gp_Ax2& Axes, 
					   const Standard_Real R, 
					   const Standard_Real H) :
       myCylinder( Axes, R , H)
{
}


//=======================================================================
//function : BRepPrimAPI_MakeCylinder
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeCylinder::BRepPrimAPI_MakeCylinder(const gp_Ax2& Axes,
					   const Standard_Real R, 
					   const Standard_Real H,
					   const Standard_Real Angle) :
       myCylinder( Axes, R , H)
{
  myCylinder.Angle(Angle);
}


//=======================================================================
//function : OneAxis
//purpose  : 
//=======================================================================

Standard_Address  BRepPrimAPI_MakeCylinder::OneAxis()
{
  return &myCylinder;
}


//=======================================================================
//function : Cylinder
//purpose  : 
//=======================================================================

BRepPrim_Cylinder&  BRepPrimAPI_MakeCylinder::Cylinder()
{
  return myCylinder;
}



