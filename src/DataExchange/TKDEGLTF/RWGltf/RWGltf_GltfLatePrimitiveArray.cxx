// Author: Kirill Gavrilov
// Copyright (c) 2018-2019 OPEN CASCADE SAS
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

#include <RWGltf_GltfLatePrimitiveArray.hxx>

#include <RWGltf_GltfPrimArrayData.hxx>
#include <RWGltf_MaterialMetallicRoughness.hxx>
#include <RWGltf_MaterialCommon.hxx>
#include <RWGltf_TriangulationReader.hxx>

IMPLEMENT_STANDARD_RTTIEXT(RWGltf_GltfLatePrimitiveArray, RWMesh_TriangulationSource)

//=================================================================================================

RWGltf_GltfLatePrimitiveArray::RWGltf_GltfLatePrimitiveArray(const TCollection_AsciiString& theId,
                                                             const TCollection_AsciiString& theName)
    : myId(theId),
      myName(theName),
      myPrimMode(RWGltf_GltfPrimitiveMode_UNKNOWN)
{
}

//=================================================================================================

RWGltf_GltfLatePrimitiveArray::~RWGltf_GltfLatePrimitiveArray()
{
  //
}

//=================================================================================================

Quantity_ColorRGBA RWGltf_GltfLatePrimitiveArray::BaseColor() const
{
  if (!myMaterialPbr.IsNull())
  {
    return myMaterialPbr->BaseColor;
  }
  else if (!myMaterialCommon.IsNull())
  {
    return Quantity_ColorRGBA(myMaterialCommon->DiffuseColor,
                              1.0f - myMaterialCommon->Transparency);
  }
  return Quantity_ColorRGBA();
}

//=================================================================================================

RWGltf_GltfPrimArrayData& RWGltf_GltfLatePrimitiveArray::AddPrimArrayData(
  RWGltf_GltfArrayType theType)
{
  if (theType == RWGltf_GltfArrayType_Position)
  {
    // make sure positions go first
    myData.Prepend(RWGltf_GltfPrimArrayData(theType));
    return myData.ChangeFirst();
  }
  else if (theType == RWGltf_GltfArrayType_Indices)
  {
    // make sure indexes go after vertex positions but before any other vertex attributes
    if (myData.First().Type == RWGltf_GltfArrayType_Position)
    {
      myData.InsertAfter(myData.Lower(), RWGltf_GltfPrimArrayData(theType));
      return myData.ChangeValue(myData.Lower() + 1);
    }
    else
    {
      myData.Prepend(RWGltf_GltfPrimArrayData(theType));
      return myData.ChangeFirst();
    }
  }
  else
  {
    myData.Append(RWGltf_GltfPrimArrayData(theType));
    return myData.ChangeLast();
  }
}

//=================================================================================================

Handle(Poly_Triangulation) RWGltf_GltfLatePrimitiveArray::LoadStreamData() const
{
  Handle(RWGltf_TriangulationReader) aGltfReader =
    Handle(RWGltf_TriangulationReader)::DownCast(myReader);
  if (aGltfReader.IsNull())
  {
    return Handle(Poly_Triangulation)();
  }

  Handle(RWMesh_TriangulationSource) aResult = new RWMesh_TriangulationSource();
  if (!aGltfReader->LoadStreamData(this, aResult))
  {
    return Handle(Poly_Triangulation)();
  }
  aResult->SetMeshPurpose(aResult->MeshPurpose() | Poly_MeshPurpose_Loaded);
  return aResult;
}
