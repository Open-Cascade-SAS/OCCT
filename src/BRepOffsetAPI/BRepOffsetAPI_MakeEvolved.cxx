// Created on: 1995-09-18
// Created by: Bruno DUMORTIER
// Copyright (c) 1995-1999 Matra Datavision
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
