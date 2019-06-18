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

#ifndef _RWGltf_PrimitiveArrayReader_HeaderFile
#define _RWGltf_PrimitiveArrayReader_HeaderFile

#include <Poly_Triangulation.hxx>
#include <RWMesh_CoordinateSystemConverter.hxx>
#include <RWGltf_GltfAccessor.hxx>
#include <RWGltf_GltfArrayType.hxx>
#include <RWGltf_GltfPrimitiveMode.hxx>
#include <TCollection_AsciiString.hxx>

class RWGltf_GltfLatePrimitiveArray;

//! The interface for shared file.
struct RWGltf_GltfSharedIStream
{
  std::ifstream           Stream; //!< shared file
  TCollection_AsciiString Path;   //!< path to currently opened stream
};

//! Interface for reading primitive array from glTF buffer.
class RWGltf_PrimitiveArrayReader : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(RWGltf_PrimitiveArrayReader, Standard_Transient)
public:

  //! Constructor.
  RWGltf_PrimitiveArrayReader() {}

  //! Return prefix for reporting issues.
  const TCollection_AsciiString& ErrorPrefix() const { return myErrorPrefix; }

  //! Set prefix for reporting issues.
  void SetErrorPrefix (const TCollection_AsciiString& theErrPrefix) { myErrorPrefix = theErrPrefix; }

  //! Return transformation from glTF to OCCT coordinate system.
  const RWMesh_CoordinateSystemConverter& CoordinateSystemConverter() const { return myCoordSysConverter; }

  //! Set transformation from glTF to OCCT coordinate system.
  void SetCoordinateSystemConverter (const RWMesh_CoordinateSystemConverter& theConverter) { myCoordSysConverter = theConverter; }

  //! Load primitive array.
  Handle(Poly_Triangulation) Load (const Handle(RWGltf_GltfLatePrimitiveArray)& theMesh)
  {
    if (load (theMesh))
    {
      return result();
    }
    return Handle(Poly_Triangulation)();
  }

protected:

  //! Reset cache before loading primitive array.
  Standard_EXPORT virtual void reset() = 0;

  //! Load primitive array.
  Standard_EXPORT virtual bool load (const Handle(RWGltf_GltfLatePrimitiveArray)& theMesh);

  //! Return result primitive array.
  Standard_EXPORT virtual Handle(Poly_Triangulation) result() = 0;

  //! Read primitive array data.
  //! @param theStream   input stream to read from
  //! @param theName     entity name for logging errors
  //! @param theAccessor buffer accessor
  //! @param theType     array type
  //! @param theMode     primitive mode
  //! @return FALSE on error
  Standard_EXPORT virtual bool readBuffer (std::istream& theStream,
                                           const TCollection_AsciiString& theName,
                                           const RWGltf_GltfAccessor& theAccessor,
                                           RWGltf_GltfArrayType theType,
                                           RWGltf_GltfPrimitiveMode theMode) = 0;

  //! Report error.
  Standard_EXPORT virtual void reportError (const TCollection_AsciiString& theText);

protected:

  TCollection_AsciiString          myErrorPrefix;
  RWGltf_GltfSharedIStream         mySharedStream;
  RWMesh_CoordinateSystemConverter myCoordSysConverter;

};

#endif // _RWGltf_PrimitiveArrayReader_HeaderFile
