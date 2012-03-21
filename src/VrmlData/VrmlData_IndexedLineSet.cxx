// Created on: 2007-08-01
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2012 OPEN CASCADE SAS
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



#include <VrmlData_IndexedLineSet.hxx>
#include <VrmlData_Scene.hxx>
#include <VrmlData_InBuffer.hxx>
#include <VrmlData_UnknownNode.hxx>
#include <BRep_Builder.hxx>
#include <Poly_Polygon3D.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>

#ifdef WNT
#define _CRT_SECURE_NO_DEPRECATE
#pragma warning (disable:4996)
#endif

IMPLEMENT_STANDARD_HANDLE  (VrmlData_IndexedLineSet, VrmlData_Geometry)
IMPLEMENT_STANDARD_RTTIEXT (VrmlData_IndexedLineSet, VrmlData_Geometry)

//=======================================================================
//function : GetColor
//purpose  : 
//=======================================================================

Quantity_Color VrmlData_IndexedLineSet::GetColor
                                        (const Standard_Integer /*iFace*/,
                                         const Standard_Integer /*iVertex*/)
{
  //TODO
  return Quantity_NOC_BLACK;
}

//=======================================================================
//function : TShape
//purpose  : Query the shape. This method checks the flag myIsModified;
//           if True it should rebuild the shape presentation.
//=======================================================================

const Handle(TopoDS_TShape)& VrmlData_IndexedLineSet::TShape ()
{
  if (myNbPolygons == 0)
    myTShape.Nullify();
  else if (myIsModified) {
    Standard_Integer i;
    BRep_Builder aBuilder;
    const gp_XYZ * arrNodes = myCoords->Values();

    // Create the Wire
    TopoDS_Wire aWire;
    aBuilder.MakeWire(aWire);
    for (i = 0; i < (int)myNbPolygons; i++) {
      const Standard_Integer * arrIndice;
      const Standard_Integer nNodes = Polygon(i, arrIndice);
      TColgp_Array1OfPnt   arrPoint (1, nNodes);
      TColStd_Array1OfReal arrParam (1, nNodes);
      for (Standard_Integer j = 0; j < nNodes; j++) {
        arrPoint(j+1).SetXYZ (arrNodes[arrIndice[j]]);
        arrParam(j+1) = j;
      }
      const Handle(Poly_Polygon3D) aPolyPolygon =
        new Poly_Polygon3D (arrPoint, arrParam);
      TopoDS_Edge anEdge;
      aBuilder.MakeEdge (anEdge, aPolyPolygon);
      aBuilder.Add (aWire, anEdge);
    }
    myTShape = aWire.TShape();
  }
  return myTShape;
}

//=======================================================================
//function : Clone
//purpose  : Create a copy of this node
//=======================================================================

Handle(VrmlData_Node) VrmlData_IndexedLineSet::Clone
                                (const Handle(VrmlData_Node)& theOther) const
{
  Handle(VrmlData_IndexedLineSet) aResult =
    Handle(VrmlData_IndexedLineSet)::DownCast (VrmlData_Node::Clone(theOther));
  if (aResult.IsNull())
    aResult =
      new VrmlData_IndexedLineSet(theOther.IsNull()? Scene(): theOther->Scene(),
                                  Name());

  if (&aResult->Scene() == &Scene()) {
    aResult->SetCoordinates (myCoords);
    aResult->SetColors      (myColors);
    aResult->SetPolygons    (myNbPolygons, myArrPolygons);
    aResult->SetColorInd    (myNbColors, myArrColorInd);
  } else {
    // Create a dummy node to pass the different Scene instance to methods Clone
    const Handle(VrmlData_UnknownNode) aDummyNode =
      new VrmlData_UnknownNode (aResult->Scene());
    if (myCoords.IsNull() == Standard_False)
      aResult->SetCoordinates (Handle(VrmlData_Coordinate)::DownCast
                               (myCoords->Clone (aDummyNode)));
    if (myColors.IsNull() == Standard_False)
      aResult->SetColors (Handle(VrmlData_Color)::DownCast
                          (myColors->Clone (aDummyNode)));
    //TODO: Replace the following lines with the relevant copying
    aResult->SetPolygons    (myNbPolygons, myArrPolygons);
    aResult->SetColorInd    (myNbColors, myArrColorInd);
  }
  aResult->SetColorPerVertex  (myColorPerVertex);
  return aResult;
}

//=======================================================================
//function : Read
//purpose  : Read the Node from input stream.
//=======================================================================

VrmlData_ErrorStatus VrmlData_IndexedLineSet::Read
                                        (VrmlData_InBuffer& theBuffer)
{
  VrmlData_ErrorStatus aStatus;
  const VrmlData_Scene& aScene = Scene();
  while (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
  {
    if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "colorPerVertex"))
      aStatus = ReadBoolean (theBuffer, myColorPerVertex);
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "coordIndex"))
      aStatus = aScene.ReadArrIndex (theBuffer, myArrPolygons, myNbPolygons);
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "colorIndex"))
      aStatus = aScene.ReadArrIndex (theBuffer, myArrColorInd, myNbColors);
    // These two checks should be the last one to avoid their interference
    // with the other tokens (e.g., coordIndex)
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "color"))
      aStatus = ReadNode (theBuffer, myColors,
                          STANDARD_TYPE(VrmlData_Color));
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "coord"))
      aStatus = ReadNode (theBuffer, myCoords,
                          STANDARD_TYPE(VrmlData_Coordinate));
    else
      break;
    if (!OK(aStatus))
      break;
  }
  // Read the terminating (closing) brace
  if (OK(aStatus) || aStatus == VrmlData_EmptyData)
    if (OK(aStatus, readBrace (theBuffer))) {
      // Post-processing
      ;
    }
  return aStatus;
}

//=======================================================================
//function : Write
//purpose  : Write the Node to output stream
//=======================================================================

VrmlData_ErrorStatus VrmlData_IndexedLineSet::Write
                                        (const char * thePrefix) const
{
  static char header[] = "IndexedLineSet {";
  const VrmlData_Scene& aScene = Scene();
  VrmlData_ErrorStatus aStatus;
  if (OK (aStatus, aScene.WriteLine (thePrefix, header, GlobalIndent()))) {

    if (OK(aStatus) && myCoords.IsNull() == Standard_False)
      aStatus = aScene.WriteNode ("coord", myCoords);
    if (OK(aStatus))
      aStatus = aScene.WriteArrIndex ("coordIndex", myArrPolygons,myNbPolygons);

    if (OK(aStatus) && myColorPerVertex == Standard_False)
      aStatus = aScene.WriteLine ("colorPerVertex  FALSE");
    if (OK(aStatus) && myColors.IsNull() == Standard_False)
      aStatus = aScene.WriteNode ("color", myColors);
    if (OK(aStatus))
      aStatus = aScene.WriteArrIndex ("colorIndex", myArrColorInd, myNbColors);

    aStatus = WriteClosing();
  }
  return aStatus;
}

//=======================================================================
//function : IsDefault
//purpose  : Returns True if the node is default,
//           so that it should not be written.
//=======================================================================

Standard_Boolean VrmlData_IndexedLineSet::IsDefault () const
{
  Standard_Boolean aResult (Standard_True);
  if (myNbPolygons)
    aResult = Standard_False;
  else if (myCoords.IsNull() == Standard_False)
    aResult = myCoords->IsDefault();
  return aResult;
}

