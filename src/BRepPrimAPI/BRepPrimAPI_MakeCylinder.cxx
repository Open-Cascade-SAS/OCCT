// Created on: 1993-07-23
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <BRepPrimAPI_MakeCylinder.ixx>
#include <BRepBuilderAPI.hxx>


#include <gp.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax2.hxx>


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



