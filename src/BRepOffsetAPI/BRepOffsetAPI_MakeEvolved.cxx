// Created on: 1995-09-18
// Created by: Bruno DUMORTIER
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <BRepFill.hxx>
#include <BRepFill_Evolved.hxx>
#include <BRepOffsetAPI_MakeEvolved.hxx>
#include <gp_Ax3.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>

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
