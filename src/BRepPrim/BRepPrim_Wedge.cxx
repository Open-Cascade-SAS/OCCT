// File:	BRepPrim_Wedge.cxx
// Created:	Mon Jan  9 14:18:58 1995
// Author:	Modelistation
//		<model@mastox>


#include <BRepPrim_Wedge.ixx>



//=======================================================================
//function : BRepPrim_Wedge
//purpose  : 
//=======================================================================

BRepPrim_Wedge::BRepPrim_Wedge(const gp_Ax2& Axes, const Standard_Real dx, const Standard_Real dy, const Standard_Real dz) :
BRepPrim_GWedge(BRepPrim_Builder(),Axes,dx,dy,dz)
{
}

//=======================================================================
//function : BRepPrim_Wedge
//purpose  : 
//=======================================================================

 BRepPrim_Wedge::BRepPrim_Wedge(const gp_Ax2& Axes, const Standard_Real dx, const Standard_Real dy, const Standard_Real dz, const Standard_Real ltx) :
BRepPrim_GWedge(BRepPrim_Builder(),Axes,dx,dy,dz,ltx)
{
}

//=======================================================================
//function : BRepPrim_Wedge
//purpose  : 
//=======================================================================

 BRepPrim_Wedge::BRepPrim_Wedge(const gp_Ax2& Axes, 
				const Standard_Real xmin, const Standard_Real ymin, const Standard_Real zmin, 
				const Standard_Real z2min, const Standard_Real x2min,
				const Standard_Real xmax, const Standard_Real ymax, const Standard_Real zmax, 
				const Standard_Real z2max, const Standard_Real x2max) :
BRepPrim_GWedge(BRepPrim_Builder(),Axes,xmin,ymin,zmin,z2min,x2min,xmax,ymax,zmax,z2max,x2max)
{
}

