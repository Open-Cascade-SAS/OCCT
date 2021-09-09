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

class RWMesh_TriangulationReader;

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

  //! Return TRUE if all scenes in the document should be loaded, FALSE by default which means only main (default) scene will be loaded.
  bool ToLoadAllScenes() const { return myToLoadAllScenes; }

  //! Set flag to flag to load all scenes in the document, FALSE by default which means only main (default) scene will be loaded.
  void SetLoadAllScenes (bool theToLoadAll) { myToLoadAllScenes = theToLoadAll; }

  //! Set flag to use Mesh name in case if Node name is empty, TRUE by default.
  bool ToUseMeshNameAsFallback() { return myUseMeshNameAsFallback; }

  //! Set flag to use Mesh name in case if Node name is empty.
  void SetMeshNameAsFallback (bool theToFallback) { myUseMeshNameAsFallback = theToFallback; }

  //! Return flag to fill in triangulation using double or single precision; FALSE by default.
  bool IsDoublePrecision() const { return myIsDoublePrecision; }

  //! Set flag to fill in triangulation using double or single precision.
  void SetDoublePrecision (bool theIsDouble) { myIsDoublePrecision = theIsDouble; }

  //! Returns TRUE if data loading should be skipped and can be performed later; FALSE by default.
  bool ToSkipLateDataLoading() { return myToSkipLateDataLoading; }

  //! Sets flag to skip data loading.
  void SetToSkipLateDataLoading (bool theToSkip) { myToSkipLateDataLoading = theToSkip; }

  //! Returns TRUE if data should be loaded into itself without its transfering to new structure.
  //! It allows to keep information about deferred storage to load/unload this data later.
  //! TRUE by default.
  bool ToKeepLateData() { return myToKeepLateData; }

  //! Sets flag to keep information about deferred storage to load/unload data later.
  void SetToKeepLateData (bool theToKeep) { myToKeepLateData = theToKeep; }

  //! Returns TRUE if additional debug information should be print; FALSE by default.
  bool ToPrintDebugMessages() const { return myToPrintDebugMessages; }

  //! Sets flag to print debug information.
  void SetToPrintDebugMessages (const Standard_Boolean theToPrint) { myToPrintDebugMessages = theToPrint; }

protected:

  //! Read the mesh from specified file.
  Standard_EXPORT virtual Standard_Boolean performMesh (const TCollection_AsciiString& theFile,
                                                        const Message_ProgressRange& theProgress,
                                                        const Standard_Boolean theToProbe) Standard_OVERRIDE;

  //! Create primitive array reader context.
  //! Can be overridden by sub-class to read triangulation into application-specific data structures instead of Poly_Triangulation.
  //! Default implementation creates RWGltf_TriangulationReader.
  Standard_EXPORT virtual Handle(RWMesh_TriangulationReader) createMeshReaderContext() const;

  //! Read late data from RWGltf_GltfLatePrimitiveArray stored as Poly_Triangulation within faces.
  Standard_EXPORT virtual Standard_Boolean readLateData (NCollection_Vector<TopoDS_Face>& theFaces,
                                                         const TCollection_AsciiString& theFile,
                                                         const Message_ProgressRange& theProgress);

  //! Set reader for each late data.
  Standard_EXPORT void updateLateDataReader (NCollection_Vector<TopoDS_Face>& theFaces,
                                             const Handle(RWMesh_TriangulationReader)& theReader) const;

protected:

  class CafReader_GltfBaseLoadingFunctor;
  class CafReader_GltfFullDataLoadingFunctor;
  class CafReader_GltfStreamDataLoadingFunctor;

protected:

  Standard_Boolean myToParallel;            //!< flag to use multithreading; FALSE by default
  Standard_Boolean myToSkipEmptyNodes;      //!< ignore nodes without Geometry; TRUE by default
  Standard_Boolean myToLoadAllScenes;       //!< flag to load all scenes in the document, FALSE by default
  Standard_Boolean myUseMeshNameAsFallback; //!< flag to use Mesh name in case if Node name is empty, TRUE by default
  Standard_Boolean myIsDoublePrecision;     //!< flag to fill in triangulation using single or double precision
  Standard_Boolean myToSkipLateDataLoading; //!< flag to skip triangulation loading
  Standard_Boolean myToKeepLateData;        //!< flag to keep information about deferred storage to load/unload triangulation later
  Standard_Boolean myToPrintDebugMessages;  //!< flag to print additional debug information

};

#endif // _RWGltf_CafReader_HeaderFile
