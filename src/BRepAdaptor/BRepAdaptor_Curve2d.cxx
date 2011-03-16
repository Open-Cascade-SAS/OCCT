// File:	BRepAdaptor_Curve2d.cxx
// Created:	Tue Jul 13 19:13:38 1993
// Author:	Remi LEQUETTE
//		<rle@nonox>


#include <BRepAdaptor_Curve2d.ixx>
#include <BRep_Tool.hxx>

//=======================================================================
//function : BRepAdaptor_Curve2d
//purpose  : 
//=======================================================================

BRepAdaptor_Curve2d::BRepAdaptor_Curve2d() 
{
}


//=======================================================================
//function : BRepAdaptor_Curve2d
//purpose  : 
//=======================================================================

BRepAdaptor_Curve2d::BRepAdaptor_Curve2d(const TopoDS_Edge& E, 
					 const TopoDS_Face& F) 
{
  Initialize(E,F);
}


//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void  BRepAdaptor_Curve2d::Initialize(const TopoDS_Edge& E, 
				      const TopoDS_Face& F)
{
  myEdge = E;
  myFace = F;
  Standard_Real pf,pl;
  const Handle(Geom2d_Curve) PC = BRep_Tool::CurveOnSurface(E,F,pf,pl);
  Geom2dAdaptor_Curve::Load(PC,pf,pl);
}

//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================

const TopoDS_Edge& BRepAdaptor_Curve2d::Edge() const
{
  return myEdge;
}

//=======================================================================
//function : Face
//purpose  : 
//=======================================================================

const TopoDS_Face& BRepAdaptor_Curve2d::Face() const
{
  return myFace;
}


