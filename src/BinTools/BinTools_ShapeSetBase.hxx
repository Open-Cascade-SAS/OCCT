// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _BinTools_ShapeSetBase_HeaderFile
#define _BinTools_ShapeSetBase_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_OStream.hxx>
#include <Standard_IStream.hxx>
#include <Message_ProgressRange.hxx>
#include <BinTools_FormatVersion.hxx>

class TopoDS_Shape;
class gp_Pnt;

//! Writes to the stream a gp_Pnt data
Standard_OStream& operator << (Standard_OStream& OS, const gp_Pnt& P);

//! A base class for all readers/writers of TopoDS_Shape into/from stream.
class BinTools_ShapeSetBase
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! A default constructor.
  Standard_EXPORT BinTools_ShapeSetBase();
  
  Standard_EXPORT virtual ~BinTools_ShapeSetBase();

  //! Return true if shape should be stored with triangles.
  Standard_Boolean IsWithTriangles() const { return myWithTriangles; }
  //! Return true if shape should be stored triangulation with normals.
  Standard_Boolean IsWithNormals() const { return myWithNormals; }

  //! Define if shape will be stored with triangles.
  //! Ignored (always written) if face defines only triangulation (no surface).
  void SetWithTriangles (const Standard_Boolean theWithTriangles) { myWithTriangles = theWithTriangles; }
  //! Define if shape will be stored triangulation with normals.
  //! Ignored (always written) if face defines only triangulation (no surface).
  void SetWithNormals(const Standard_Boolean theWithNormals) { myWithNormals = theWithNormals; }

  //! Sets the BinTools_FormatVersion.
  Standard_EXPORT void SetFormatNb (const Standard_Integer theFormatNb);

  //! Returns the BinTools_FormatVersion.
  Standard_EXPORT Standard_Integer FormatNb() const { return myFormatNb; }
  
  //! Clears the content of the set.
  Standard_EXPORT virtual void Clear() {}
  
  //! Writes the content of  me  on the stream <OS> in binary
  //! format that can be read back by Read.
  //!
  //! Writes the locations.
  //!
  //! Writes the geometry calling WriteGeometry.
  //!
  //! Dumps the shapes from last to first.
  //! For each shape  :
  //! Write the type.
  //! calls WriteGeometry(S).
  //! Write the flags, the subshapes.
  Standard_EXPORT virtual void Write
    (Standard_OStream& /*OS*/, const Message_ProgressRange& /*theRange*/ = Message_ProgressRange()) {}
  
  //! Reads the content of me from the binary stream  <IS>. me
  //! is first cleared.
  //!
  //! Reads the locations.
  //!
  //! Reads the geometry calling ReadGeometry.
  //!
  //! Reads the shapes.
  //! For each shape
  //! Reads the type.
  //! calls ReadGeometry(T,S).
  //! Reads the flag, the subshapes.
  Standard_EXPORT virtual void Read
    (Standard_IStream& /*IS*/, const Message_ProgressRange& /*theRange*/ = Message_ProgressRange()) {}
  
  //! Writes   on  <OS>   the shape   <S>.    Writes the
  //! orientation, the index of the TShape and the index
  //! of the Location.
  Standard_EXPORT virtual void Write (const TopoDS_Shape& /*theShape*/, Standard_OStream& /*theStream*/) {}
  
  //! An empty virtual method for redefinition in shape-reader.
  Standard_EXPORT virtual void Read (Standard_IStream& /*theStream*/, TopoDS_Shape& /*theShape*/) {}
                                                                                    
  static const Standard_CString THE_ASCII_VERSIONS[BinTools_FormatVersion_UPPER + 1];
private:

  Standard_Integer myFormatNb;
  Standard_Boolean myWithTriangles;
  Standard_Boolean myWithNormals;
};

#endif // _BinTools_ShapeSet_HeaderFile
