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

#include <RWGltf_MaterialMetallicRoughness.hxx>
#include <RWGltf_MaterialCommon.hxx>

#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <OSD_OpenFile.hxx>
#include <Standard_ArrayStreamBuffer.hxx>

#include <fstream>

IMPLEMENT_STANDARD_RTTIEXT(RWGltf_GltfLatePrimitiveArray, Poly_Triangulation)

// =======================================================================
// function : RWGltf_GltfLatePrimitiveArray
// purpose  :
// =======================================================================
RWGltf_GltfLatePrimitiveArray::RWGltf_GltfLatePrimitiveArray (const TCollection_AsciiString& theId,
                                                              const TCollection_AsciiString& theName)
: Poly_Triangulation (3, 1, false),
  myId (theId),
  myName (theName),
  myPrimMode (RWGltf_GltfPrimitiveMode_UNKNOWN)
{
  SetBoundingBox (Bnd_Box());
}

// =======================================================================
// function : ~RWGltf_GltfLatePrimitiveArray
// purpose  :
// =======================================================================
RWGltf_GltfLatePrimitiveArray::~RWGltf_GltfLatePrimitiveArray()
{
  //
}

// =======================================================================
// function : BaseColor
// purpose  :
// =======================================================================
Quantity_ColorRGBA RWGltf_GltfLatePrimitiveArray::BaseColor() const
{
  if (!myMaterialPbr.IsNull())
  {
    return myMaterialPbr->BaseColor;
  }
  else if (!myMaterialCommon.IsNull())
  {
    return Quantity_ColorRGBA (myMaterialCommon->DiffuseColor, 1.0f - myMaterialCommon->Transparency);
  }
  return Quantity_ColorRGBA();
}

// =======================================================================
// function : AddPrimArrayData
// purpose  :
// =======================================================================
RWGltf_GltfPrimArrayData& RWGltf_GltfLatePrimitiveArray::AddPrimArrayData (RWGltf_GltfArrayType theType)
{
  if (theType == RWGltf_GltfArrayType_Position)
  {
    // make sure positions go first
    myData.Prepend (RWGltf_GltfPrimArrayData (theType));
    return myData.ChangeFirst();
  }
  else if (theType == RWGltf_GltfArrayType_Indices)
  {
    // make sure indexes go after vertex positions but before any other vertex attributes
    if (myData.First().Type == RWGltf_GltfArrayType_Position)
    {
      myData.InsertAfter (myData.Lower(), RWGltf_GltfPrimArrayData (theType));
      return myData.ChangeValue (myData.Lower() + 1);
    }
    else
    {
      myData.Prepend (RWGltf_GltfPrimArrayData (theType));
      return myData.ChangeFirst();
    }
  }
  else
  {
    myData.Append (RWGltf_GltfPrimArrayData (theType));
    return myData.ChangeLast();
  }
}

// =======================================================================
// function : SetBoundingBox
// purpose  :
// =======================================================================
void RWGltf_GltfLatePrimitiveArray::SetBoundingBox (const Bnd_Box& theBox)
{
  myBox = theBox;

  if (theBox.IsVoid())
  {
    Poly_Triangulation::myNodes = TColgp_Array1OfPnt();
    Poly_Triangulation::myTriangles = Poly_Array1OfTriangle();
    return;
  }

  // define 8 nodes so that AABB will be huge enough to include mesh even with transformation applied
  Poly_Triangulation::myNodes.Resize (1, 8, false);
  const gp_Pnt aMin = theBox.CornerMin();
  const gp_Pnt aMax = theBox.CornerMax();
  Poly_Triangulation::ChangeNode(1).SetCoord(aMin.X(), aMin.Y(), aMin.Z());
  Poly_Triangulation::ChangeNode(2).SetCoord(aMax.X(), aMax.Y(), aMax.Z());
  Poly_Triangulation::ChangeNode(3).SetCoord(aMin.X(), aMin.Y(), aMax.Z());
  Poly_Triangulation::ChangeNode(4).SetCoord(aMin.X(), aMax.Y(), aMax.Z());
  Poly_Triangulation::ChangeNode(5).SetCoord(aMax.X(), aMax.Y(), aMin.Z());
  Poly_Triangulation::ChangeNode(6).SetCoord(aMax.X(), aMin.Y(), aMin.Z());
  Poly_Triangulation::ChangeNode(7).SetCoord(aMin.X(), aMax.Y(), aMin.Z());
  Poly_Triangulation::ChangeNode(8).SetCoord(aMax.X(), aMin.Y(), aMax.Z());

  Poly_Triangulation::myTriangles.Resize (1, 1, false);
  Poly_Triangulation::ChangeTriangle (1).Set (1, 2, 1);
  //Poly_Triangulation::myTriangles = Poly_Array1OfTriangle();
}
