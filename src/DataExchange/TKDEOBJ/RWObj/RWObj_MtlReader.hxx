// Author: Kirill Gavrilov
// Copyright (c) 2015-2019 OPEN CASCADE SAS
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

#ifndef _RWObj_MtlReader_HeaderFile
#define _RWObj_MtlReader_HeaderFile

#include <Graphic3d_Vec3.hxx>
#include <RWObj_Material.hxx>
#include <NCollection_DataMap.hxx>

//! Reader of mtl files.
class RWObj_MtlReader
{
public:
  //! Main constructor.
  RWObj_MtlReader(NCollection_DataMap<TCollection_AsciiString, RWObj_Material>& theMaterials);

  //! Destructor.
  ~RWObj_MtlReader();

  //! Read the file.
  bool Read(const TCollection_AsciiString& theFolder, const TCollection_AsciiString& theFile);

private:
  //! Validate scalar value
  bool validateScalar(const Standard_Real theValue);

  //! Validate RGB color
  bool validateColor(const Graphic3d_Vec3& theVec);

  //! Process texture path.
  void processTexturePath(TCollection_AsciiString&       theTexturePath,
                          const TCollection_AsciiString& theFolder);

private:
  FILE*                                                         myFile;
  TCollection_AsciiString                                       myPath;
  NCollection_DataMap<TCollection_AsciiString, RWObj_Material>* myMaterials;
  int                                                           myNbLines;
};

#endif // _RWObj_MtlReader_HeaderFile
