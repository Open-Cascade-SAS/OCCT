// Created on: 1998-08-25
// Created by: Pavel DURANDIN
// Copyright (c) 1998-1999 Matra Datavision
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



#include <ShapeAnalysis_FreeBoundData.ixx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopoDS.hxx>

//=======================================================================
//function : ShapeAnalysis_FreeBoundData
//purpose  : Empty constructor
//=======================================================================

ShapeAnalysis_FreeBoundData::ShapeAnalysis_FreeBoundData()
{
  myNotches = new TopTools_HSequenceOfShape();
  Clear();
}

//=======================================================================
//function : ShapeAnalysis_FreeBoundData
//purpose  : Creates object with contour given in the form of TopoDS_Wire
//=======================================================================

ShapeAnalysis_FreeBoundData::ShapeAnalysis_FreeBoundData(const TopoDS_Wire& freebound)
{
  myNotches = new TopTools_HSequenceOfShape();
  Clear();
  SetFreeBound(freebound);
}

//=======================================================================
//function : Clear
//purpose  : Clears all properties of the contour. Contour bound itself is not cleared.
//=======================================================================

void ShapeAnalysis_FreeBoundData::Clear()
{
  myArea = -1;
  myPerimeter = -1;
  myRatio = -1;
  myWidth = -1;
  myNotches->Clear();
  myNotchesParams.Clear();
}

//=======================================================================
//function : AddNotch
//purpose  : Adds notch on free bound with its maximum width
//=======================================================================

void ShapeAnalysis_FreeBoundData::AddNotch(const TopoDS_Wire& notch,const Standard_Real width)
{
  if (myNotchesParams.IsBound(notch)) return;
  myNotches->Append(notch);
  myNotchesParams.Bind(notch, width);
}


//=======================================================================
//function : NotchWidth
//purpose  : Returns maximum width of notch specified by its rank number
//    	     on the contour
//=======================================================================

Standard_Real ShapeAnalysis_FreeBoundData::NotchWidth(const Standard_Integer index) const
{
  TopoDS_Wire wire = TopoDS::Wire(myNotches -> Value(index));
  return myNotchesParams.Find(wire);
}

//=======================================================================
//function : NotchWidth
//purpose  : Returns maximum width of notch specified as TopoDS_Wire
//    	     on the contour
//=======================================================================

Standard_Real ShapeAnalysis_FreeBoundData::NotchWidth(const TopoDS_Wire& notch) const
{
  return myNotchesParams.Find(notch);
}
