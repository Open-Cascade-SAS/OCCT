// File:	BRepCheck.cxx
// Created:	Fri Dec  8 15:03:28 1995
// Author:	Jacques GOUSSARD
//		<jag@bravox>


#include <BRepCheck.ixx>

#include <BRepCheck_ListIteratorOfListOfStatus.hxx>
#include <BRepCheck_Wire.hxx>

#include <BRep_Tool.hxx>

#include <TopoDS_Iterator.hxx>
#include <TopoDS.hxx>


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
#ifndef DEB
  default:
    break;
#endif
  }
}


