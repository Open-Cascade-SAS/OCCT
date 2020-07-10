// Author: Kirill Gavrilov
// Copyright (c) 2016-2019 OPEN CASCADE SAS
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

#ifndef _RWGltf_CafReader_HeaderFile
#define _RWGltf_CafReader_HeaderFile

#include <Message_ProgressRange.hxx>
#include <NCollection_Vector.hxx>
#include <RWMesh_CafReader.hxx>
#include <TopoDS_Face.hxx>

class RWGltf_PrimitiveArrayReader;

//! The glTF (GL Transmission Format) mesh reader into XDE document.
class RWGltf_CafReader : public RWMesh_CafReader
{
  DEFINE_STANDARD_RTTIEXT(RWGltf_CafReader, RWMesh_CafReader)
public:

  //! Empty constructor.
  Standard_EXPORT RWGltf_CafReader();

  //! Return TRUE if multithreaded optimizations are allowed; FALSE by default.
  bool ToParallel() const { return myToParallel; }

  //! Setup multithreaded execution.
  void SetParallel (bool theToParallel) { myToParallel = theToParallel; }

  //! Return TRUE if Nodes without Geometry should be ignored, TRUE by default.
  bool ToSkipEmptyNodes() { return myToSkipEmptyNodes; }

  //! Set flag to ignore nodes without Geometry.
  void SetSkipEmptyNodes (bool theToSkip) { myToSkipEmptyNodes = theToSkip; }

  //! Set flag to use Mesh name in case if Node name is empty, TRUE by default.
  bool ToUseMeshNameAsFallback() { return myUseMeshNameAsFallback; }

  //! Set flag to use Mesh name in case if Node name is empty.
  void SetMeshNameAsFallback (bool theToFallback) { myUseMeshNameAsFallback = theToFallback; }

protected:

  //! Read the mesh from specified file.
  Standard_EXPORT virtual Standard_Boolean performMesh (const TCollection_AsciiString& theFile,
                                                        const Message_ProgressRange& theProgress,
                                                        const Standard_Boolean theToProbe) Standard_OVERRIDE;

  //! Create primitive array reader context.
  //! Can be overridden by sub-class to read triangulation into application-specific data structures instead of Poly_Triangulation.
  //! Default implementation creates RWGltf_TriangulationReader.
  Standard_EXPORT virtual Handle(RWGltf_PrimitiveArrayReader) createMeshReaderContext();

  //! Read late data from RWGltf_GltfLatePrimitiveArray stored as Poly_Triangulation within faces.
  Standard_EXPORT virtual Standard_Boolean readLateData (NCollection_Vector<TopoDS_Face>& theFaces,
                                                         const TCollection_AsciiString& theFile,
                                                         const Message_ProgressRange& theProgress);
protected:

  class CafReader_GltfReaderFunctor;

protected:

  Standard_Boolean myToParallel;            //!< flag to use multithreading; FALSE by default
  Standard_Boolean myToSkipEmptyNodes;      //!< ignore nodes without Geometry; TRUE by default
  Standard_Boolean myUseMeshNameAsFallback; //!< flag to use Mesh name in case if Node name is empty, TRUE by default

};

#endif // _RWGltf_CafReader_HeaderFile
