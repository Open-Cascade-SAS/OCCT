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

#ifndef _RWGltf_TriangulationReader_HeaderFile
#define _RWGltf_TriangulationReader_HeaderFile

#include <RWMesh_TriangulationReader.hxx>
#include <RWGltf_GltfAccessor.hxx>
#include <RWGltf_GltfArrayType.hxx>
#include <RWGltf_GltfPrimitiveMode.hxx>

class RWGltf_GltfLatePrimitiveArray;

//! RWMesh_TriangulationReader implementation creating Poly_Triangulation.
class RWGltf_TriangulationReader : public RWMesh_TriangulationReader
{
  DEFINE_STANDARD_RTTIEXT(RWGltf_TriangulationReader, RWMesh_TriangulationReader)
public:

  //! Empty constructor.
  Standard_EXPORT RWGltf_TriangulationReader();

protected:

  //! Reports error.
  Standard_EXPORT virtual void reportError (const TCollection_AsciiString& theText) const;

  //! Loads primitive array.
  Standard_EXPORT virtual bool load (const Handle(RWMesh_TriangulationSource)& theSourceMesh,
                                     const Handle(Poly_Triangulation)& theDestMesh,
                                     const Handle(OSD_FileSystem)& theFileSystem) const Standard_OVERRIDE;

  //! Performs additional actions to finalize data loading.
  Standard_EXPORT virtual bool finalizeLoading (const Handle(RWMesh_TriangulationSource)& theSourceMesh,
                                                const Handle(Poly_Triangulation)& theDestMesh) const Standard_OVERRIDE;

  //! Fills triangulation data and ignore non-triangulation primitives.
  //! @param theSourceMesh source triangulation
  //! @param theDestMesh   triangulation to be modified
  //! @param theStream     input stream to read from
  //! @param theAccessor   buffer accessor
  //! @param theType       array type
  //! @return FALSE on error
  Standard_EXPORT virtual bool readBuffer (const Handle(RWGltf_GltfLatePrimitiveArray)& theSourceMesh,
                                           const Handle(Poly_Triangulation)& theDestMesh,
                                           std::istream& theStream,
                                           const RWGltf_GltfAccessor& theAccessor,
                                           RWGltf_GltfArrayType theType) const;

protected:

  Handle(Poly_Triangulation) myTriangulation;

};

#endif // _RWGltf_TriangulationReader_HeaderFile
