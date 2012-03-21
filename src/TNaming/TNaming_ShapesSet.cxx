// Created on: 1997-01-24
// Created by: Yves FRICAUD
// Copyright (c) 1997-1999 Matra Datavision
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



#include <TNaming_ShapesSet.ixx>
#include <TNaming_IteratorOnShapesSet.hxx>
#include <TNaming_Iterator.hxx>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopExp_Explorer.hxx>

#define BUC60862
//#define MDTV_DEB_INT
//=======================================================================
//function : TNaming_ShapesSet
//purpose  : 
//=======================================================================

TNaming_ShapesSet::TNaming_ShapesSet (const TopoDS_Shape&    CS,
				      const TopAbs_ShapeEnum Type)
{
  if (CS.IsNull()) return;
#ifdef MDTV_DEB_INT
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

#ifdef BUC60862
// corrected by vro 13.09.00:
    if (Type > CS.ShapeType()) {
      for (TopExp_Explorer exp(CS,Type) ; exp.More(); exp.Next()) {
	Add(exp.Current());
#ifdef MDTV_DEB_INT
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
#else
    for (TopExp_Explorer exp(CS,Type) ; exp.More(); exp.Next()) {
      Add(exp.Current());
    }
#endif

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


