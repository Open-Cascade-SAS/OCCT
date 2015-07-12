// Created on: 1995-12-08
// Created by: Jacques GOUSSARD
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


#include <BRep_Tool.hxx>
#include <BRepCheck.hxx>
#include <BRepCheck_ListIteratorOfListOfStatus.hxx>
#include <BRepCheck_Wire.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Wire.hxx>

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
void BRepCheck::Add(BRepCheck_ListOfStatus& lst, const BRepCheck_Status stat)
{
  BRepCheck_ListIteratorOfListOfStatus it(lst);
  while (it.More()) {
    if (it.Value() == BRepCheck_NoError && stat != BRepCheck_NoError) {
      lst.Remove(it);
    }
    else {
      if (it.Value() == stat) {
        return;
      }
      it.Next();
    }
  }
  lst.Append(stat);
}
//=======================================================================
//function : SelfIntersection
//purpose  : 
//=======================================================================
Standard_Boolean BRepCheck::SelfIntersection(const TopoDS_Wire& W,
          const TopoDS_Face& myFace,
          TopoDS_Edge& RetE1,
          TopoDS_Edge& RetE2)
{
  Handle(BRepCheck_Wire) chkw = new BRepCheck_Wire(W);
  BRepCheck_Status stat = chkw->SelfIntersect(myFace,RetE1,RetE2);
  return (stat == BRepCheck_SelfIntersectingWire);
}
//=======================================================================
//function : Print
//purpose  : 
//=======================================================================
void BRepCheck::Print(const BRepCheck_Status stat,
                      Standard_OStream& OS)
{

  switch (stat) {
  case BRepCheck_NoError:
    OS << "BRepCheck_NoError\n";
    break;
  case BRepCheck_InvalidPointOnCurve:
    OS << "BRepCheck_InvalidPointOnCurve\n";
    break;
  case BRepCheck_InvalidPointOnCurveOnSurface:
    OS << "BRepCheck_InvalidPointOnCurveOnSurface\n";
    break;
  case BRepCheck_InvalidPointOnSurface:
    OS << "BRepCheck_InvalidPointOnSurface\n";
    break;
  case BRepCheck_No3DCurve:
    OS << "BRepCheck_No3DCurve\n";
    break;
  case BRepCheck_Multiple3DCurve:
    OS << "BRepCheck_Multiple3DCurve\n";
    break;
  case BRepCheck_Invalid3DCurve:
    OS << "BRepCheck_Invalid3DCurve\n";
    break;
  case BRepCheck_NoCurveOnSurface:
    OS << "BRepCheck_NoCurveOnSurface\n";
    break;
  case BRepCheck_InvalidCurveOnSurface:
    OS << "BRepCheck_InvalidCurveOnSurface\n";
    break;
  case BRepCheck_InvalidCurveOnClosedSurface:
    OS << "BRepCheck_InvalidCurveOnClosedSurface\n";
    break;
  case BRepCheck_InvalidSameRangeFlag:
    OS << "BRepCheck_InvalidSameRangeFlag\n";
    break;
  case BRepCheck_InvalidSameParameterFlag:
    OS << "BRepCheck_InvalidSameParameterFlag\n";
    break;
  case BRepCheck_InvalidDegeneratedFlag:
    OS << "BRepCheck_InvalidDegeneratedFlag\n";
    break;
  case BRepCheck_FreeEdge:
    OS << "BRepCheck_FreeEdge\n";
    break;
  case BRepCheck_InvalidMultiConnexity:
    OS << "BRepCheck_InvalidMultiConnexity\n";
    break;
  case BRepCheck_InvalidRange:
    OS << "BRepCheck_InvalidRange\n";
    break;
  case BRepCheck_EmptyWire:
    OS << "BRepCheck_EmptyWire\n";
    break;
  case BRepCheck_RedundantEdge:
    OS << "BRepCheck_RedundantEdge\n";
    break;
  case BRepCheck_SelfIntersectingWire:
    OS << "BRepCheck_SelfIntersectingWire\n";
    break;
  case BRepCheck_NoSurface:
    OS << "BRepCheck_NoSurface\n";
    break;
  case BRepCheck_InvalidWire:
    OS << "BRepCheck_InvalidWire\n";
    break;
  case BRepCheck_RedundantWire:
    OS << "BRepCheck_RedundantWire\n";
    break;
  case BRepCheck_IntersectingWires:
    OS << "BRepCheck_IntersectingWires\n";
    break;
  case BRepCheck_InvalidImbricationOfWires:
    OS << "BRepCheck_InvalidImbricationOfWires\n";
    break;
  case BRepCheck_EmptyShell:
    OS << "BRepCheck_EmptyShell\n";
    break;
  case BRepCheck_RedundantFace:
    OS << "BRepCheck_RedundantFace\n";
    break;
  case BRepCheck_UnorientableShape:
    OS << "BRepCheck_UnorientableShape\n";
    break;
  case BRepCheck_NotClosed:
    OS << "BRepCheck_NotClosed\n";
    break;
  case BRepCheck_NotConnected:
    OS << "BRepCheck_NotConnected\n";
    break;
  case BRepCheck_SubshapeNotInShape:      
    OS << "BRepCheck_SubshapeNotInShape\n";
    break;
  case BRepCheck_BadOrientation:
    OS << "BRepCheck_BadOrientation\n";
    break;
  case BRepCheck_BadOrientationOfSubshape:
    OS << "BRepCheck_BadOrientationOfSubshape\n";
    break;
  case BRepCheck_CheckFail:
    OS << "BRepCheck_CheckFail\n";
    break;
  case BRepCheck_InvalidPolygonOnTriangulation:
    OS << "BRepCheck_InvalidPolygonOnTriangulation\n";
    break;
  case BRepCheck_InvalidToleranceValue:
    OS << "BRepCheck_InvalidToleranceValue\n";
    break;
  case BRepCheck_InvalidImbricationOfShells:
    OS << "BRepCheck_InvalidImbricationOfShells\n";
    break;
  case BRepCheck_EnclosedRegion:
    OS << "BRepCheck_EnclosedRegion\n";
    break;
  default:
    break;
  }
}


