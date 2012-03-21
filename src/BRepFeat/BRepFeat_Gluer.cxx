// Created on: 1996-03-08
// Created by: Jacques GOUSSARD
// Copyright (c) 1996-1999 Matra Datavision
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



#include <BRepFeat_Gluer.ixx>
#include <TopoDS.hxx>
#include <TopTools_ListOfShape.hxx>

//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void BRepFeat_Gluer::Build()
{
  myGluer.Perform();
  if (myGluer.IsDone()) {
    Done();
    myShape = myGluer.ResultingShape();
  }
  else {
    NotDone();
  }
}



//=======================================================================
//function : isdeleted
//purpose  : 
//=======================================================================

Standard_Boolean BRepFeat_Gluer::IsDeleted
   (const TopoDS_Shape& F) 
{
  return (myGluer.DescendantFaces(TopoDS::Face(F)).IsEmpty());
}

//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepFeat_Gluer::Modified
   (const TopoDS_Shape& F)
{
  if (F.ShapeType() == TopAbs_FACE) {
    const TopTools_ListOfShape& LS = myGluer.DescendantFaces(TopoDS::Face(F));
    if (!LS.IsEmpty()) {
      if (!LS.First().IsSame(F))
	return myGluer.DescendantFaces(TopoDS::Face(F));
    }
  }
  static TopTools_ListOfShape LIM;
  return  LIM;
}

