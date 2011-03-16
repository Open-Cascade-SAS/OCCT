// File:	BRepOffsetAPI_MakeEvolved.cxx
// Created:	Mon Sep 18 14:27:09 1995
// Author:	Bruno DUMORTIER
//		<dub@fuegox>


#include <BRepOffsetAPI_MakeEvolved.ixx>

#include <gp_Ax3.hxx>
#include <BRepFill.hxx>

//=======================================================================
//function : BRepOffsetAPI_MakeEvolved
//purpose  : 
//=======================================================================

BRepOffsetAPI_MakeEvolved::BRepOffsetAPI_MakeEvolved()
{
}


//=======================================================================
//function : BRepOffsetAPI_MakeEvolved
//purpose  : 
//=======================================================================

BRepOffsetAPI_MakeEvolved::BRepOffsetAPI_MakeEvolved(const TopoDS_Wire&     Spine,
					 const TopoDS_Wire&     Profil, 
					 const GeomAbs_JoinType Join,
					 const Standard_Boolean AxeProf,
					 const Standard_Boolean Solid,
					 const Standard_Boolean ProfOnSpine,
					 const Standard_Real    Tol)
{
  gp_Ax3 Axis(gp_Pnt(0.,0.,0.),
	      gp_Dir(0.,0.,1.),
	      gp_Dir(1.,0.,0.));

  if ( !AxeProf) {
    Standard_Boolean POS;
    BRepFill::Axe(Spine,Profil,Axis,POS,Tol);
    if (ProfOnSpine && !POS) return;
  }

  myEvolved.Perform(Spine,Profil,Axis,Join,Solid);
  Build();
  Done();
}


//=======================================================================
//function : BRepOffsetAPI_MakeEvolved
//purpose  : 
//=======================================================================

BRepOffsetAPI_MakeEvolved::BRepOffsetAPI_MakeEvolved(const TopoDS_Face&     Spine,
					 const TopoDS_Wire&     Profil,
					 const GeomAbs_JoinType Join,
					 const Standard_Boolean AxeProf,
					 const Standard_Boolean Solid,
					 const Standard_Boolean ProfOnSpine,
					 const Standard_Real    Tol)
{
  gp_Ax3 Axis(gp_Pnt(0.,0.,0.),
	      gp_Dir(0.,0.,1.),
	      gp_Dir(1.,0.,0.));

  if ( !AxeProf) {
    Standard_Boolean POS;
    BRepFill::Axe(Spine,Profil,Axis,POS,Tol);
    if (ProfOnSpine && !POS) return;
  }

  myEvolved.Perform(Spine,Profil,Axis,Join,Solid);
  Build();
}


//=======================================================================
//function : BRepFill_Evolved&
//purpose  : 
//=======================================================================

const BRepFill_Evolved& BRepOffsetAPI_MakeEvolved::Evolved() const 
{
  return myEvolved;
}


//=======================================================================
//function :
//purpose  : 
//=======================================================================

void BRepOffsetAPI_MakeEvolved::Build()
{
  myShape = myEvolved.Shape();
  if (myEvolved.IsDone())  Done();
}


//=======================================================================
//function : Top
//purpose  : 
//=======================================================================

const TopoDS_Shape&  BRepOffsetAPI_MakeEvolved::Top() const 
{
  return myEvolved.Top();
}

//=======================================================================
//function : Bottom
//purpose  : 
//=======================================================================

const TopoDS_Shape&  BRepOffsetAPI_MakeEvolved::Bottom() const 
{
  return myEvolved.Bottom();
}

//=======================================================================
//function : GeneratedShapes
//purpose  : 
//=======================================================================

const TopTools_ListOfShape&  BRepOffsetAPI_MakeEvolved::GeneratedShapes ( 
   const TopoDS_Shape& SpineShape,
   const TopoDS_Shape& ProfShape )
const 
{
  return myEvolved.GeneratedShapes(SpineShape,ProfShape);
}
