// Author: Kirill Gavrilov
// Copyright (c) 2019 OPEN CASCADE SAS
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

#include <RWObj_TriangulationReader.hxx>

#include <BRep_Builder.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>

IMPLEMENT_STANDARD_RTTIEXT(RWObj_TriangulationReader, RWObj_Reader)

//================================================================
// Function : addMesh
// Purpose  :
//================================================================
Standard_Boolean RWObj_TriangulationReader::addMesh (const RWObj_SubMesh& theMesh,
                                                     const RWObj_SubMeshReason theReason)
{
  if (!myToCreateShapes)
  {
    return Standard_False;
  }

  if (Handle(Poly_Triangulation) aTris = GetTriangulation())
  {
    myNodes.Clear();
    myNodesUV.Clear();
    myNormals.Clear();
    myTriangles.Clear();
    if (theMesh.Group != myLastGroupName)
    {
      // flush previous group and start a new one
      if (addSubShape (myLastObjectShape, myLastGroupShape, Standard_False))
      {
        if (myShapeReceiver != NULL)
        {
          const RWObj_Material* aMaterial = myLastGroupShape.ShapeType() == TopAbs_FACE
                                        && !myLastFaceMaterial.IsEmpty()
                                          ? myMaterials.Seek (myLastFaceMaterial)
                                          : NULL;
          myShapeReceiver->BindNamedShape (myLastGroupShape, myLastGroupName, aMaterial, Standard_False);
        }
      }
      myLastGroupShape = TopoDS_Shape();
      myLastGroupName = theMesh.Group;
    }

    TopoDS_Face aNewFace;
    BRep_Builder aBuilder;
    aBuilder.MakeFace (aNewFace, aTris);
    addSubShape (myLastGroupShape, aNewFace, Standard_True);
    myLastFaceMaterial = theMesh.Material;
    if (myShapeReceiver != NULL)
    {
      const RWObj_Material* aMaterial = myMaterials.Seek (theMesh.Material);
      myShapeReceiver->BindNamedShape (aNewFace, "", aMaterial, Standard_False);
    }
  }

  if (theReason == RWObj_SubMeshReason_NewObject)
  {
    // forced flush at the end of the object
    if (addSubShape (myLastObjectShape, myLastGroupShape, Standard_False))
    {
      if (myShapeReceiver != NULL)
      {
        const RWObj_Material* aMaterial = myLastGroupShape.ShapeType() == TopAbs_FACE
                                          && !myLastFaceMaterial.IsEmpty()
                                            ? myMaterials.Seek (myLastFaceMaterial)
                                            : NULL;
        myShapeReceiver->BindNamedShape (myLastGroupShape, myLastGroupName, aMaterial, Standard_False);
      }
    }
    myLastGroupShape = TopoDS_Shape();
    myLastGroupName.Clear();

    if (addSubShape (myResultShape, myLastObjectShape, Standard_False))
    {
      if (myShapeReceiver != NULL)
      {
        myShapeReceiver->BindNamedShape (myLastObjectShape, theMesh.Object, NULL, Standard_True);
      }
    }
    myLastObjectShape = TopoDS_Compound();
  }
  return Standard_True;
}

// =======================================================================
// function : addSubShape
// purpose  :
// =======================================================================
Standard_Boolean RWObj_TriangulationReader::addSubShape (TopoDS_Shape& theParent,
                                                         const TopoDS_Shape& theSubShape,
                                                         const Standard_Boolean theToExpandCompound)
{
  if (theSubShape.IsNull())
  {
    return Standard_False;
  }

  BRep_Builder aBuilder;
  if (theParent.IsNull()
   && theToExpandCompound)
  {
    theParent = theSubShape;
    return Standard_True;
  }

  TopoDS_Compound aComp;
  if (!theParent.IsNull()
    && theParent.ShapeType() == TopAbs_COMPOUND)
  {
    aComp = TopoDS::Compound (theParent);
  }
  else
  {
    aBuilder.MakeCompound (aComp);
    if (!theParent.IsNull())
    {
      aBuilder.Add (aComp, theParent);
    }
  }
  aBuilder.Add (aComp, theSubShape);
  theParent = aComp;
  return Standard_True;
}

//=============================================================================
//function : GetTriangulation
//purpose  :
//=============================================================================
Handle(Poly_Triangulation) RWObj_TriangulationReader::GetTriangulation()
{
  if (myTriangles.IsEmpty())
  {
    return Handle(Poly_Triangulation)();
  }

  const Standard_Boolean hasNormals = myNodes.Length() == myNormals.Length();
  const Standard_Boolean hasUV      = myNodes.Length() == myNodesUV.Length();

  Handle(Poly_Triangulation) aPoly = new Poly_Triangulation (myNodes.Length(), myTriangles.Length(), hasUV);
  for (Standard_Integer aNodeIter = 0; aNodeIter < myNodes.Size(); ++aNodeIter)
  {
    const gp_Pnt& aNode = myNodes.Value (aNodeIter);
    aPoly->ChangeNode (aNodeIter + 1) = aNode;
  }
  if (hasUV)
  {
    for (Standard_Integer aNodeIter = 0; aNodeIter < myNodes.Size(); ++aNodeIter)
    {
      const Graphic3d_Vec2& aNode = myNodesUV.Value (aNodeIter);
      aPoly->ChangeUVNode (aNodeIter + 1).SetCoord (aNode.x(), aNode.y());
    }
  }
  if (hasNormals)
  {
    const Handle(TShort_HArray1OfShortReal) aNormals = new TShort_HArray1OfShortReal (1, myNodes.Length() * 3);
    Standard_ShortReal* aNormArr = &aNormals->ChangeFirst();
    Standard_Integer aNbInvalid = 0;
    for (Standard_Integer aNodeIter = 0; aNodeIter < myNodes.Size(); ++aNodeIter)
    {
      const Graphic3d_Vec3& aNorm = myNormals.Value (aNodeIter);
      const float aMod2 = aNorm.SquareModulus();
      if (aMod2 > 0.001f)
      {
        aNormArr[aNodeIter * 3 + 0] = aNorm.x();
        aNormArr[aNodeIter * 3 + 1] = aNorm.y();
        aNormArr[aNodeIter * 3 + 2] = aNorm.z();
      }
      else
      {
        ++aNbInvalid;
        aNormArr[aNodeIter * 3 + 0] = 0.0f;
        aNormArr[aNodeIter * 3 + 1] = 0.0f;
        aNormArr[aNodeIter * 3 + 2] = 1.0f;
      }
    }
    if (aNbInvalid != myNodes.Length())
    {
      aPoly->SetNormals (aNormals);
    }
  }

  for (Standard_Integer aTriIter = 0; aTriIter < myTriangles.Size(); ++aTriIter)
  {
    aPoly->ChangeTriangle (aTriIter + 1) = myTriangles (aTriIter);
  }

  return aPoly;
}

//================================================================
// Function : ResultShape
// Purpose  :
//================================================================
TopoDS_Shape RWObj_TriangulationReader::ResultShape()
{
  if (!myToCreateShapes)
  {
    if (Handle(Poly_Triangulation) aTris = GetTriangulation())
    {
      TopoDS_Face aFace;
      BRep_Builder aBuilder;
      aBuilder.MakeFace (aFace, aTris);
      return aFace;
    }
    return TopoDS_Shape();
  }

  if (!myResultShape.IsNull()
    && myResultShape.ShapeType() == TopAbs_COMPOUND
    && myResultShape.NbChildren() == 1
    && myActiveSubMesh.Object.IsEmpty())
  {
    TopoDS_Iterator aChildIter (myResultShape);
    return aChildIter.Value();
  }
  return myResultShape;
}
