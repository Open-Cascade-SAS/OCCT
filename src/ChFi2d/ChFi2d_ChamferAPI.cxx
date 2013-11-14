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

#include <ChFi2d_ChamferAPI.hxx>

#include <Precision.hxx>
#include <gp_Pnt.hxx>
#include <GC_MakeLine.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS.hxx>

// An empty constructor.
ChFi2d_ChamferAPI::ChFi2d_ChamferAPI()
{

}

// A constructor accepting a wire consisting of two linear edges.
ChFi2d_ChamferAPI::ChFi2d_ChamferAPI(const TopoDS_Wire& theWire) :
	myStart1(0.), 
	myEnd1(0.), 
	myStart2(0.), 
	myEnd2(0.),
	myCommonStart1(Standard_False), 
	myCommonStart2(Standard_False)
{
  Init(theWire);
}

// A constructor accepting two linear edges.
ChFi2d_ChamferAPI::ChFi2d_ChamferAPI(const TopoDS_Edge& theEdge1, const TopoDS_Edge& theEdge2) :
	myStart1(0.), 
	myEnd1(0.), 
	myStart2(0.), 
	myEnd2(0.),
	myCommonStart1(Standard_False), 
	myCommonStart2(Standard_False)
{
  Init(theEdge1, theEdge2);
}

// Initializes the class by a wire consisting of two libear edges.
void ChFi2d_ChamferAPI::Init(const TopoDS_Wire& theWire)
{
  TopoDS_Edge E1, E2;
  TopoDS_Iterator itr(theWire);
  for (; itr.More(); itr.Next())
  {
    if (E1.IsNull())
      E1 = TopoDS::Edge(itr.Value());
    else if (E2.IsNull())
      E2 = TopoDS::Edge(itr.Value());
    else
      break;
  }
  Init(E1, E2);
}

// Initializes the class by two linear edges.
void ChFi2d_ChamferAPI::Init(const TopoDS_Edge& theEdge1, const TopoDS_Edge& theEdge2)
{
  myEdge1 = theEdge1;
  myEdge2 = theEdge2;
}

// Constructs a chamfer edge.
// Returns true if the edge is constructed.
Standard_Boolean ChFi2d_ChamferAPI::Perform()
{
  myCurve1 = BRep_Tool::Curve(myEdge1, myStart1, myEnd1);
  myCurve2 = BRep_Tool::Curve(myEdge2, myStart2, myEnd2);
  // searching for common points
  if (myCurve1->Value(myStart1).IsEqual(myCurve2->Value(myEnd2), Precision::Confusion())) 
  {
    myCommonStart1 = true;
    myCommonStart2 = false;
  } 
  else 
  {
    if (myCurve1->Value(myEnd1).IsEqual(myCurve2->Value(myStart2), Precision::Confusion())) 
    {
      myCommonStart1 = false;
      myCommonStart2 = true;
    } 
    else
    {
      if (myCurve1->Value(myEnd1).IsEqual(myCurve2->Value(myEnd2), Precision::Confusion())) 
      {
        myCommonStart1 = false;
        myCommonStart2 = false;
      } 
      else 
      {
        myCommonStart1 = true;
        myCommonStart2 = true;
      }
    }
  }
  return Standard_True;
}

// Returns the result (chamfer edge, modified edge1, modified edge2).
TopoDS_Edge ChFi2d_ChamferAPI::Result(TopoDS_Edge& theEdge1, TopoDS_Edge& theEdge2,
                                      const Standard_Real theLength1, const Standard_Real theLength2) 
{
  TopoDS_Edge aResult;
  if (Abs(myEnd1 - myStart1) < theLength1) 
    return aResult;
  if (Abs(myEnd2 - myStart2) < theLength2) 
    return aResult;

  Standard_Real aCommon1 = (myCommonStart1?myStart1:myEnd1) + (((myStart1 > myEnd1)^myCommonStart1)?theLength1:-theLength1);
  Standard_Real aCommon2 = (myCommonStart2?myStart2:myEnd2) + (((myStart2 > myEnd2)^myCommonStart2)?theLength2:-theLength2);

  // make chamfer edge
  GC_MakeLine aML(myCurve1->Value(aCommon1), myCurve2->Value(aCommon2));
  BRepBuilderAPI_MakeEdge aBuilder(aML.Value(), myCurve1->Value(aCommon1), myCurve2->Value(aCommon2));
  aResult = aBuilder.Edge();
  // divide first edge
  BRepBuilderAPI_MakeEdge aDivider1(myCurve1, aCommon1, (myCommonStart1?myEnd1:myStart1));
  theEdge1 = aDivider1.Edge();
  // divide second edge
  BRepBuilderAPI_MakeEdge aDivider2(myCurve2, aCommon2, (myCommonStart2?myEnd2:myStart2));
  theEdge2 = aDivider2.Edge();

  return aResult;
}
