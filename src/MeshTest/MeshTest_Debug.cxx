// Created on: 2016-05-31
// Created by: Mikhail Sazonov
// Copyright (c) 2016 OPEN CASCADE SAS
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

#include <BRepMesh_FaceAttribute.hxx>
#include <Draw_Segment3D.hxx>
#include <DrawTrSurf_Polygon3D.hxx>
#include <Draw.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <Poly_Polygon3D.hxx>

// This file defines global functions not declared in any public header,
// intended for use from debugger prompt (Command Window in Visual Studio)

//=======================================================================
//function : MeshTest_DrawLinks
//purpose  : Draw links from mesh data structure of type BRepMesh_FaceAttribute
//=======================================================================
Standard_EXPORT const char* MeshTest_DrawLinks(const char* theNameStr, void* theFaceAttr)
{
  if (theNameStr == 0 || theFaceAttr == 0)
  {
    return "Error: name or face attribute is null";
  }
  try {
    const Handle(BRepMesh_FaceAttribute)& aFaceAttr = *(Handle(BRepMesh_FaceAttribute)*)theFaceAttr;
    const Handle(BRepMesh_DataStructureOfDelaun)& aMeshData = aFaceAttr->ChangeStructure();
    if (aMeshData.IsNull())
      return "Null mesh data structure";
    Standard_Integer nbLinks = aMeshData->NbLinks();
    cout << "nblink=" << nbLinks << endl;
    TCollection_AsciiString aName(theNameStr);
    for (Standard_Integer i = 1; i <= nbLinks; i++)
    {
      const BRepMesh_Edge& aLink = aMeshData->GetLink(i);
      if (aLink.Movability() == BRepMesh_Deleted)
        continue;
      Standard_Integer n1 = aLink.FirstNode();
      Standard_Integer n2 = aLink.LastNode();
      const BRepMesh_Vertex& aV1 = aMeshData->GetNode(n1);
      const BRepMesh_Vertex& aV2 = aMeshData->GetNode(n2);
      const gp_Pnt& aP1 = aFaceAttr->GetPoint(aV1);
      const gp_Pnt& aP2 = aFaceAttr->GetPoint(aV2);
      Handle(Draw_Segment3D) aSeg = new Draw_Segment3D(aP1, aP2, Draw_bleu);
      Draw::Set((aName + "_" + i).ToCString(), aSeg);
    }
    return theNameStr;
  }
  catch (Standard_Failure const& anException)
  {
    return anException.GetMessageString();
  }
}

//=======================================================================
//function : MeshTest_DrawTriangles
//purpose  : Draw triangles from mesh data structure of type BRepMesh_FaceAttribute
//=======================================================================
Standard_EXPORT const char* MeshTest_DrawTriangles(const char* theNameStr, void* theFaceAttr)
{
  if (theNameStr == 0 || theFaceAttr == 0)
  {
    return "Error: name or face attribute is null";
  }
  try {
    const Handle(BRepMesh_FaceAttribute)& aFaceAttr =
      *(Handle(BRepMesh_FaceAttribute)*)theFaceAttr;
    const Handle(BRepMesh_DataStructureOfDelaun)& aMeshData = aFaceAttr->ChangeStructure();
    if (aMeshData.IsNull())
      return "Null mesh data structure";
    Standard_Integer nbElem = aMeshData->NbElements();
    cout << "nbelem=" << nbElem << endl;
    TCollection_AsciiString aName(theNameStr);
    for (Standard_Integer i = 1; i <= nbElem; i++)
    {
      const BRepMesh_Triangle& aTri = aMeshData->GetElement(i);
      if (aTri.Movability() == BRepMesh_Deleted)
        continue;
      Standard_Integer n[3];
      aMeshData->ElementNodes(aTri, n);
      const BRepMesh_Vertex& aV1 = aMeshData->GetNode(n[0]);
      const BRepMesh_Vertex& aV2 = aMeshData->GetNode(n[1]);
      const BRepMesh_Vertex& aV3 = aMeshData->GetNode(n[2]);
      gp_Pnt aP[4] = { aFaceAttr->GetPoint(aV1), aFaceAttr->GetPoint(aV2), 
                       aFaceAttr->GetPoint(aV3), aFaceAttr->GetPoint(aV1) };
      TColgp_Array1OfPnt aPnts(aP[0], 1, 4);
      Handle(Poly_Polygon3D) aPoly = new Poly_Polygon3D(aPnts);
      Handle(DrawTrSurf_Polygon3D) aDPoly = new DrawTrSurf_Polygon3D(aPoly);
      Draw::Set((aName + "_" + i).ToCString(), aDPoly);
    }
    return theNameStr;
  }
  catch (Standard_Failure const& anException)
  {
    return anException.GetMessageString();
  }
}
