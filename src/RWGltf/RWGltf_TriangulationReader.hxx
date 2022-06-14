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

class RWGltf_GltfLatePrimitiveArray;
class RWGltf_GltfPrimArrayData;

//! RWMesh_TriangulationReader implementation creating Poly_Triangulation.
class RWGltf_TriangulationReader : public RWMesh_TriangulationReader
{
  DEFINE_STANDARD_RTTIEXT(RWGltf_TriangulationReader, RWMesh_TriangulationReader)
public:

  //! Empty constructor.
  Standard_EXPORT RWGltf_TriangulationReader();

  //! Loads only primitive arrays saved as stream buffer
  //! (it is primarily glTF data encoded in base64 saved to temporary buffer during glTF file reading).
  Standard_EXPORT bool LoadStreamData (const Handle(RWMesh_TriangulationSource)& theSourceMesh,
                                       const Handle(Poly_Triangulation)& theDestMesh) const;

protected:

  //! Reports error.
  Standard_EXPORT virtual void reportError (const TCollection_AsciiString& theText) const;

  //! Loads only primitive arrays from file data.
  //! @param theSourceMesh    source triangulation
  //! @param theDestMesh      triangulation to be modified
  //! @param theFileSystem    shared file system to read from
  //! Note: this method skips "stream data" that should be loaded by LoadStreamData() call.
  Standard_EXPORT virtual bool load (const Handle(RWMesh_TriangulationSource)& theSourceMesh,
                                     const Handle(Poly_Triangulation)& theDestMesh,
                                     const Handle(OSD_FileSystem)& theFileSystem) const Standard_OVERRIDE;

  //! Performs additional actions to finalize data loading.
  //! @param theSourceMesh source triangulation
  //! @param theDestMesh   triangulation to be modified
  Standard_EXPORT virtual bool finalizeLoading (const Handle(RWMesh_TriangulationSource)& theSourceMesh,
                                                const Handle(Poly_Triangulation)& theDestMesh) const Standard_OVERRIDE;

  //! Loads only primitive arrays saved as stream buffer
  //! (it is primarily glTF data encoded in base64 saved to temporary buffer during glTF file reading).
  //! @param theSourceMesh    source triangulation
  //! @param theDestMesh      triangulation to be modified
  //! @param theToResetStream if TRUE reset input stream data buffer after its loading.
  Standard_EXPORT bool loadStreamData (const Handle(RWMesh_TriangulationSource)& theSourceMesh,
                                       const Handle(Poly_Triangulation)& theDestMesh,
                                       bool theToResetStream = true) const;

  //! Reads primitive array from stream data.
  //! @param theSourceGltfMesh source glTF triangulation
  //! @param theGltfData       primitive array element (stream data should not be NULL)
  //! @param theDestMesh       triangulation to be modified
  Standard_EXPORT bool readStreamData (const Handle(RWGltf_GltfLatePrimitiveArray)& theSourceGltfMesh,
                                       const RWGltf_GltfPrimArrayData& theGltfData,
                                       const Handle(Poly_Triangulation)& theDestMesh) const;

  //! Reads primitive array from file data.
  //! @param theSourceGltfMesh source glTF triangulation
  //! @param theGltfData       primitive array element (Uri of file stream should not be empty)
  //! @param theDestMesh       triangulation to be modified
  //! @param theFileSystem     shared file system to read from
  Standard_EXPORT bool readFileData (const Handle(RWGltf_GltfLatePrimitiveArray)& theSourceGltfMesh,
                                     const RWGltf_GltfPrimArrayData& theGltfData,
                                     const Handle(Poly_Triangulation)& theDestMesh,
                                     const Handle(OSD_FileSystem)& theFileSystem) const;

  //! Fills triangulation data and ignore non-triangulation primitives.
  //! @param theSourceGltfMesh source glTF triangulation
  //! @param theDestMesh       triangulation to be modified
  //! @param theStream         input stream to read from
  //! @param theAccessor       buffer accessor
  //! @param theType           array type
  //! @return FALSE on error
  Standard_EXPORT virtual bool readBuffer (const Handle(RWGltf_GltfLatePrimitiveArray)& theSourceGltfMesh,
                                           const Handle(Poly_Triangulation)& theDestMesh,
                                           std::istream& theStream,
                                           const RWGltf_GltfAccessor& theAccessor,
                                           RWGltf_GltfArrayType theType) const;

  //! Reads primitive array from file data compressed in Draco format.
  //! @param theSourceGltfMesh source glTF triangulation
  //! @param theGltfData       primitive array element (Uri of file stream should not be empty)
  //! @param theDestMesh       triangulation to be modified
  //! @param theFileSystem     shared file system to read from
  Standard_EXPORT virtual bool readDracoBuffer (const Handle(RWGltf_GltfLatePrimitiveArray)& theSourceGltfMesh,
                                                const RWGltf_GltfPrimArrayData& theGltfData,
                                                const Handle(Poly_Triangulation)& theDestMesh,
                                                const Handle(OSD_FileSystem)& theFileSystem) const;

protected:

  Handle(Poly_Triangulation) myTriangulation;

};

#endif // _RWGltf_TriangulationReader_HeaderFile
