// File:	BRepPrimAPI_MakeWedge.cxx
// Created:	Fri Jul 23 15:51:56 1993
// Author:	Remi LEQUETTE
//		<rle@nonox>


#include <BRepPrimAPI_MakeWedge.ixx>
#include <TopoDS.hxx>


//=======================================================================
//function : BRepPrimAPI_MakeWedge
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeWedge::BRepPrimAPI_MakeWedge(const Standard_Real dx, 
				     const Standard_Real dy,
				     const Standard_Real dz,
				     const Standard_Real ltx) :
       myWedge(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(0,0,1),gp_Dir(1,0,0)),
	       dx,dy,dz,ltx)
{
}


//=======================================================================
//function : BRepPrimAPI_MakeWedge
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeWedge::BRepPrimAPI_MakeWedge(const gp_Ax2& Axes,
				     const Standard_Real dx, 
				     const Standard_Real dy,
				     const Standard_Real dz, 
				     const Standard_Real ltx) :
       myWedge(Axes,
	       dx,dy,dz,ltx)
{
}


//=======================================================================
//function : BRepPrimAPI_MakeWedge
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeWedge::BRepPrimAPI_MakeWedge(const Standard_Real dx, 
				     const Standard_Real dy, 
				     const Standard_Real dz, 
				     const Standard_Real xmin,
				     const Standard_Real zmin,
				     const Standard_Real xmax,
				     const Standard_Real zmax) :
       myWedge(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(0,0,1),gp_Dir(1,0,0)),
	       0,0,0,zmin,xmin,dx,dy,dz,zmax,xmax)
{
}


//=======================================================================
//function : BRepPrimAPI_MakeWedge
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeWedge::BRepPrimAPI_MakeWedge(const gp_Ax2& Axes,
				     const Standard_Real dx,
				     const Standard_Real dy, 
				     const Standard_Real dz,
				     const Standard_Real xmin,
				     const Standard_Real zmin, 
				     const Standard_Real xmax, 
				     const Standard_Real zmax) :
       myWedge(Axes,
	       0,0,0,zmin,xmin,dx,dy,dz,zmax,xmax)
{
}


//=======================================================================
//function : Wedge
//purpose  : 
//=======================================================================

BRepPrim_Wedge&  BRepPrimAPI_MakeWedge::Wedge()
{
  return myWedge;
}


//=======================================================================
//function : Shell
//purpose  : 
//=======================================================================

const TopoDS_Shell&  BRepPrimAPI_MakeWedge::Shell()
{
  Build();
  return myWedge.Shell();
}


//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void BRepPrimAPI_MakeWedge::Build()
{
  BRep_Builder B;
  B.MakeSolid(TopoDS::Solid(myShape));
  B.Add(myShape,myWedge.Shell());
  myShape.Closed(Standard_True);
  Done();
}

//=======================================================================
//function : Solid
//purpose  : 
//=======================================================================

const TopoDS_Solid&  BRepPrimAPI_MakeWedge::Solid()
{
  Build();
  return TopoDS::Solid(myShape);
}


//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeWedge::operator TopoDS_Shell()
{
  return Shell();
}


//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeWedge::operator TopoDS_Solid()
{
  return Solid();
}
