// Created on: 1998-08-25
// Created by: Pavel DURANDIN
// Copyright (c) 1998-1999 Matra Datavision
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


#include <ShapeAnalysis_FreeBoundData.hxx>
#include <Standard_Type.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_HSequenceOfShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeAnalysis_FreeBoundData,Standard_Transient)

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
