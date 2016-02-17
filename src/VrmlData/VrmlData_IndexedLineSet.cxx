// Created on: 2007-08-01
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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
#include <VrmlData_Color.hxx>
#include <VrmlData_Coordinate.hxx>

IMPLEMENT_STANDARD_RTTIEXT(VrmlData_IndexedLineSet,VrmlData_Geometry)

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#pragma warning (disable:4996)
#endif


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
    {
      Handle(VrmlData_Node) aNode;
      aStatus = ReadNode (theBuffer, aNode,
                          STANDARD_TYPE(VrmlData_Color));
      myColors = Handle(VrmlData_Color)::DownCast (aNode);
    }
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "coord"))
    {
      Handle(VrmlData_Node) aNode;
      aStatus = ReadNode (theBuffer, aNode,
                          STANDARD_TYPE(VrmlData_Coordinate));
      myCoords = Handle(VrmlData_Coordinate)::DownCast (aNode);
    }
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

