// Created on: 1997-01-24
// Created by: Yves FRICAUD
// Copyright (c) 1997-1999 Matra Datavision
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


#include <TNaming_Iterator.hxx>
#include <TNaming_IteratorOnShapesSet.hxx>
#include <TNaming_ShapesSet.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>

//#define MDTV_DEB_INT
//=======================================================================
//function : TNaming_ShapesSet
//purpose  : 
//=======================================================================
TNaming_ShapesSet::TNaming_ShapesSet (const TopoDS_Shape&    CS,
				      const TopAbs_ShapeEnum Type)
{
  if (CS.IsNull()) return;
#ifdef OCCT_DEBUG_INT
  cout << "ShapeSet: CS TShape = " <<CS.TShape() << " Type = " << Type <<endl;
#endif	
  if (Type == TopAbs_SHAPE) { 
    if (CS.ShapeType() == TopAbs_SOLID ||
	CS.ShapeType() == TopAbs_FACE  ||
	CS.ShapeType() == TopAbs_EDGE  ||
	CS.ShapeType() == TopAbs_VERTEX ) {
      Add(CS);
    }
    else {
      for (TopoDS_Iterator it(CS) ; it.More(); it.Next()) {
	Add(it.Value());
      }
    }
  }
  else {

// corrected by vro 13.09.00:
    if (Type > CS.ShapeType()) {
      for (TopExp_Explorer exp(CS,Type) ; exp.More(); exp.Next()) {
	Add(exp.Current());
#ifdef OCCT_DEBUG_INT
	cout << "ShapeSet: sub-shape TShape = " <<exp.Current().TShape() <<endl;
#endif	
      }
    } else {
//      for (TopoDS_Iterator it(CS) ; it.More(); it.Next()) {
//	Add(it.Value());
//      }
      Add(CS);
    }
// end of correction by vro.
  }
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void TNaming_ShapesSet::Add(const TNaming_ShapesSet& Shapes) 
{
  TNaming_IteratorOnShapesSet it(Shapes);
  for (; it.More(); it.Next()) {
    myMap.Add(it.Value());
  }
}


//=======================================================================
//function : Filter
//purpose  : 
//=======================================================================

void TNaming_ShapesSet::Filter(const TNaming_ShapesSet& Shapes) 
{

  TNaming_ShapesSet ToRemove;  
  TNaming_IteratorOnShapesSet it(*this);
  for (; it.More(); it.Next()) {
    const TopoDS_Shape& S = it.Value();
    if (!Shapes.Contains(S)) {
      ToRemove.Add(S);
    }
  }
  Remove(ToRemove);
}

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

void TNaming_ShapesSet::Remove(const TNaming_ShapesSet& Shapes) 
{  
  TNaming_IteratorOnShapesSet it(Shapes);
  for (; it.More(); it.Next()) {
    myMap.Remove(it.Value());
  }
}


