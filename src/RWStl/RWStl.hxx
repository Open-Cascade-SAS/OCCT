// Created on: 2017-06-13
// Created by: Alexander MALYSHEV
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _RWStl_HeaderFile
#define _RWStl_HeaderFile

#include <OSD_Path.hxx>
#include <Poly_Triangulation.hxx>
#include <Standard_Macro.hxx>
#include <Message_ProgressScope.hxx>
#include <NCollection_Sequence.hxx>

//! This class provides methods to read and write triangulation from / to the STL files.
class RWStl
{
public:
  //! Write triangulation to binary STL file.
  //! binary format of an STL file.
  //! Returns false if the cannot be opened;
  Standard_EXPORT static Standard_Boolean WriteBinary(
    const Handle(Poly_Triangulation)& theMesh,
    const OSD_Path&                   thePath,
    const Message_ProgressRange&      theProgress = Message_ProgressRange());

  //! write the meshing in a file following the
  //! Ascii  format of an STL file.
  //! Returns false if the cannot be opened;
  Standard_EXPORT static Standard_Boolean WriteAscii(
    const Handle(Poly_Triangulation)& theMesh,
    const OSD_Path&                   thePath,
    const Message_ProgressRange&      theProgress = Message_ProgressRange());

  //! Read specified STL file and returns its content as triangulation.
  //! In case of error, returns Null handle.
  Standard_EXPORT static Handle(Poly_Triangulation) ReadFile(
    const OSD_Path&              theFile,
    const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Read specified STL file and returns its content as triangulation.
  //! In case of error, returns Null handle.
  static Handle(Poly_Triangulation) ReadFile(
    const Standard_CString       theFile,
    const Message_ProgressRange& theProgress = Message_ProgressRange())
  {
    return ReadFile(theFile, M_PI / 2.0, theProgress);
  }

  //! Read specified STL file and returns its content as triangulation.
  //! @param[in] theFile file path to read
  //! @param[in] theMergeAngle maximum angle in radians between triangles to merge equal nodes;
  //! M_PI/2 means ignore angle
  //! @param[in] theProgress progress indicator
  //! @return result triangulation or NULL in case of error
  Standard_EXPORT static Handle(Poly_Triangulation) ReadFile(
    const Standard_CString       theFile,
    const Standard_Real          theMergeAngle,
    const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Read specified STL file and fills triangulation list for multi-domain case.
  //! @param[in] theFile file path to read
  //! @param[in] theMergeAngle maximum angle in radians between triangles to merge equal nodes;
  //! M_PI/2 means ignore angle
  //! @param[out] theTriangList triangulation list for multi-domain case
  //! @param[in] theProgress progress indicator
  Standard_EXPORT static void ReadFile(
    const Standard_CString                            theFile,
    const Standard_Real                               theMergeAngle,
    NCollection_Sequence<Handle(Poly_Triangulation)>& theTriangList,
    const Message_ProgressRange&                      theProgress = Message_ProgressRange());

  //! Read triangulation from a binary STL file
  //! In case of error, returns Null handle.
  Standard_EXPORT static Handle(Poly_Triangulation) ReadBinary(
    const OSD_Path&              thePath,
    const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Read triangulation from an Ascii STL file
  //! In case of error, returns Null handle.
  Standard_EXPORT static Handle(Poly_Triangulation) ReadAscii(
    const OSD_Path&              thePath,
    const Message_ProgressRange& theProgress = Message_ProgressRange());

private:
  //! Write ASCII version.
  static Standard_Boolean writeASCII(const Handle(Poly_Triangulation)& theMesh,
                                     FILE*                             theFile,
                                     const Message_ProgressRange&      theProgress);

  //! Write binary version.
  static Standard_Boolean writeBinary(const Handle(Poly_Triangulation)& theMesh,
                                      FILE*                             theFile,
                                      const Message_ProgressRange&      theProgress);
};

#endif
