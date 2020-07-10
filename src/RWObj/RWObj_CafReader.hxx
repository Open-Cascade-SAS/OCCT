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

#ifndef _RWObj_CafReader_HeaderFile
#define _RWObj_CafReader_HeaderFile

#include <RWMesh_CafReader.hxx>
#include <RWObj_TriangulationReader.hxx>

//! The OBJ mesh reader into XDE document.
class RWObj_CafReader : public RWMesh_CafReader, protected RWObj_IShapeReceiver
{
  DEFINE_STANDARD_RTTIEXT(RWObj_CafReader, RWMesh_CafReader)
public:

  //! Empty constructor.
  Standard_EXPORT RWObj_CafReader();

  //! Return single precision flag for reading vertex data (coordinates); FALSE by default.
  Standard_Boolean IsSinglePrecision() const { return myIsSinglePrecision; }

  //! Setup single/double precision flag for reading vertex data (coordinates).
  void SetSinglePrecision (Standard_Boolean theIsSinglePrecision) { myIsSinglePrecision = theIsSinglePrecision; }

protected:

  //! Read the mesh from specified file.
  Standard_EXPORT virtual Standard_Boolean performMesh (const TCollection_AsciiString& theFile,
                                                        const Message_ProgressRange& theProgress,
                                                        const Standard_Boolean theToProbe) Standard_OVERRIDE;

protected:

  //! Create reader context.
  //! Can be overridden by sub-class to read triangulation into application-specific data structures instead of Poly_Triangulation.
  Standard_EXPORT virtual Handle(RWObj_TriangulationReader) createReaderContext();

  //! @param theShape       shape to register
  //! @param theName        shape name
  //! @param theMaterial    shape material
  //! @param theIsRootShape indicates that this is a root object (free shape)
  Standard_EXPORT virtual void BindNamedShape (const TopoDS_Shape& theShape,
                                               const TCollection_AsciiString& theName,
                                               const RWObj_Material* theMaterial,
                                               const Standard_Boolean theIsRootShape) Standard_OVERRIDE;

protected:

  NCollection_DataMap<TCollection_AsciiString, Handle(XCAFDoc_VisMaterial)> myObjMaterialMap;
  Standard_Boolean myIsSinglePrecision; //!< flag for reading vertex data with single or double floating point precision
};

#endif // _RWObj_CafReader_HeaderFile
