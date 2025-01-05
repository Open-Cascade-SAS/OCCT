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

#ifndef _RWGltf_GltfLatePrimitiveArray_HeaderFile
#define _RWGltf_GltfLatePrimitiveArray_HeaderFile

#include <RWMesh_TriangulationSource.hxx>

#include <NCollection_Sequence.hxx>
#include <RWGltf_GltfArrayType.hxx>
#include <RWGltf_GltfPrimitiveMode.hxx>
#include <Quantity_ColorRGBA.hxx>

class RWGltf_GltfPrimArrayData;
class RWGltf_MaterialMetallicRoughness;
class RWGltf_MaterialCommon;

//! Mesh data wrapper for delayed primitive array loading from glTF file.
class RWGltf_GltfLatePrimitiveArray : public RWMesh_TriangulationSource
{
  DEFINE_STANDARD_RTTIEXT(RWGltf_GltfLatePrimitiveArray, RWMesh_TriangulationSource)
public:
  //! Constructor.
  Standard_EXPORT RWGltf_GltfLatePrimitiveArray(const TCollection_AsciiString& theId,
                                                const TCollection_AsciiString& theName);

  //! Destructor.
  Standard_EXPORT virtual ~RWGltf_GltfLatePrimitiveArray();

  //! Entity id.
  const TCollection_AsciiString& Id() const { return myId; }

  //! Entity name.
  const TCollection_AsciiString& Name() const { return myName; }

  //! Assign entity name.
  void SetName(const TCollection_AsciiString& theName) { myName = theName; }

  //! Return type of primitive array.
  RWGltf_GltfPrimitiveMode PrimitiveMode() const { return myPrimMode; }

  //! Set type of primitive array.
  void SetPrimitiveMode(RWGltf_GltfPrimitiveMode theMode) { myPrimMode = theMode; }

  //! Return true if primitive array has assigned material
  bool HasStyle() const { return !myMaterialPbr.IsNull() || !myMaterialCommon.IsNull(); }

  //! Return base color.
  Standard_EXPORT Quantity_ColorRGBA BaseColor() const;

  //! Return PBR material definition.
  const Handle(RWGltf_MaterialMetallicRoughness)& MaterialPbr() const { return myMaterialPbr; }

  //! Set PBR material definition.
  void SetMaterialPbr(const Handle(RWGltf_MaterialMetallicRoughness)& theMat)
  {
    myMaterialPbr = theMat;
  }

  //! Return common (obsolete) material definition.
  const Handle(RWGltf_MaterialCommon)& MaterialCommon() const { return myMaterialCommon; }

  //! Set common (obsolete) material definition.
  void SetMaterialCommon(const Handle(RWGltf_MaterialCommon)& theMat) { myMaterialCommon = theMat; }

  //! Return primitive array data elements.
  const NCollection_Sequence<RWGltf_GltfPrimArrayData>& Data() const { return myData; }

  //! Add primitive array data element.
  Standard_EXPORT RWGltf_GltfPrimArrayData& AddPrimArrayData(RWGltf_GltfArrayType theType);

  //! Return TRUE if there is deferred storage and some triangulation data
  //! that can be loaded using LoadDeferredData().
  virtual Standard_Boolean HasDeferredData() const Standard_OVERRIDE
  {
    return !myData.IsEmpty() && (NbDeferredTriangles() > 0 || NbDeferredNodes() > 0);
  }

  //! Load primitive array saved as stream buffer to new triangulation object.
  Standard_EXPORT Handle(Poly_Triangulation) LoadStreamData() const;

protected:
  NCollection_Sequence<RWGltf_GltfPrimArrayData> myData;
  Handle(RWGltf_MaterialMetallicRoughness)       myMaterialPbr;    //!< PBR material
  Handle(RWGltf_MaterialCommon)                  myMaterialCommon; //!< common (obsolete) material
  TCollection_AsciiString                        myId;             //!< entity id
  TCollection_AsciiString                        myName;           //!< entity name
  RWGltf_GltfPrimitiveMode                       myPrimMode;       //!< type of primitive array
};

#endif // _RWGltf_GltfLatePrimitiveArray_HeaderFile
