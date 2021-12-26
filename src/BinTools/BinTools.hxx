// Created on: 2004-05-11
// Created by: Sergey ZARITCHNY <szy@opencascade.com>
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

#ifndef _BinTools_HeaderFile
#define _BinTools_HeaderFile

#include <BinTools_FormatVersion.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Message_ProgressRange.hxx>

class TopoDS_Shape;


//! Tool to keep shapes in binary format
class BinTools 
{
public:

  DEFINE_STANDARD_ALLOC

  Standard_EXPORT static Standard_OStream& PutReal (Standard_OStream& OS, const Standard_Real& theValue);

  Standard_EXPORT static Standard_OStream& PutShortReal (Standard_OStream& OS, const Standard_ShortReal& theValue);

  Standard_EXPORT static Standard_OStream& PutInteger (Standard_OStream& OS, const Standard_Integer theValue);
  
  Standard_EXPORT static Standard_OStream& PutBool (Standard_OStream& OS, const Standard_Boolean theValue);
  
  Standard_EXPORT static Standard_OStream& PutExtChar (Standard_OStream& OS, const Standard_ExtCharacter theValue);

  Standard_EXPORT static Standard_IStream& GetReal (Standard_IStream& IS, Standard_Real& theValue);

  Standard_EXPORT static Standard_IStream& GetShortReal (Standard_IStream& IS, Standard_ShortReal& theValue);

  Standard_EXPORT static Standard_IStream& GetInteger (Standard_IStream& IS, Standard_Integer& theValue);
  
  Standard_EXPORT static Standard_IStream& GetBool (Standard_IStream& IS, Standard_Boolean& theValue);
  
  Standard_EXPORT static Standard_IStream& GetExtChar (Standard_IStream& IS, Standard_ExtCharacter& theValue);

  //! Writes the shape to the stream in binary format BinTools_FormatVersion_CURRENT.
  //! This alias writes shape with triangulation data.
  //! @param theShape [in]       the shape to write
  //! @param theStream [in][out] the stream to output shape into
  //! @param theRange            the range of progress indicator to fill in
  static void Write (const TopoDS_Shape& theShape,
                     Standard_OStream& theStream,
                     const Message_ProgressRange& theRange = Message_ProgressRange())
  {
    Write (theShape, theStream, Standard_True, Standard_False,
           BinTools_FormatVersion_CURRENT, theRange);
  }

  //! Writes the shape to the stream in binary format of specified version.
  //! @param theShape [in]         the shape to write
  //! @param theStream [in][out]   the stream to output shape into
  //! @param theWithTriangles [in] flag which specifies whether to save shape with (TRUE) or without (FALSE) triangles;
  //!                              has no effect on triangulation-only geometry
  //! @param theWithNormals [in]   flag which specifies whether to save triangulation with (TRUE) or without (FALSE) normals;
  //!                              has no effect on triangulation-only geometry
  //! @param theVersion [in]       the BinTools format version
  //! @param theRange              the range of progress indicator to fill in
  Standard_EXPORT static void Write(const TopoDS_Shape& theShape, Standard_OStream& theStream,
                                    const Standard_Boolean theWithTriangles,
                                    const Standard_Boolean theWithNormals,
                                    const BinTools_FormatVersion theVersion,
                                    const Message_ProgressRange& theRange = Message_ProgressRange());

  //! Reads a shape from <theStream> and returns it in <theShape>.
  Standard_EXPORT static void Read (TopoDS_Shape& theShape, Standard_IStream& theStream,
                                    const Message_ProgressRange& theRange = Message_ProgressRange());

  //! Writes the shape to the file in binary format BinTools_FormatVersion_CURRENT.
  //! @param theShape [in] the shape to write
  //! @param theFile [in]  the path to file to output shape into
  //! @param theRange      the range of progress indicator to fill in
  static Standard_Boolean Write (const TopoDS_Shape& theShape,
                                 const Standard_CString theFile,
                                 const Message_ProgressRange& theRange = Message_ProgressRange())
  {
    return Write (theShape, theFile, Standard_True, Standard_False,
                  BinTools_FormatVersion_CURRENT, theRange);
  }

  //! Writes the shape to the file in binary format of specified version.
  //! @param theShape [in]         the shape to write
  //! @param theFile [in]          the path to file to output shape into
  //! @param theWithTriangles [in] flag which specifies whether to save shape with (TRUE) or without (FALSE) triangles;
  //!                              has no effect on triangulation-only geometry
  //! @param theWithNormals [in]   flag which specifies whether to save triangulation with (TRUE) or without (FALSE) normals;
  //!                              has no effect on triangulation-only geometry
  //! @param theVersion [in]       the BinTools format version
  //! @param theRange              the range of progress indicator to fill in
  Standard_EXPORT static Standard_Boolean Write (const TopoDS_Shape& theShape,
                                                 const Standard_CString theFile,
                                                 const Standard_Boolean theWithTriangles,
                                                 const Standard_Boolean theWithNormals,
                                                 const BinTools_FormatVersion theVersion,
                                                 const Message_ProgressRange& theRange = Message_ProgressRange());

  //! Reads a shape from <theFile> and returns it in <theShape>.
  Standard_EXPORT static Standard_Boolean Read
    (TopoDS_Shape& theShape, const Standard_CString theFile,
     const Message_ProgressRange& theRange = Message_ProgressRange());

};

#endif // _BinTools_HeaderFile
