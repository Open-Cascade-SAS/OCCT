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

#include <DE_Provider.hxx>
#include <NCollection_Vec3.hxx>
#include <Standard_TypeDef.hxx>
#include <RWObj_Material.hxx>
#include <NCollection_DataMap.hxx>

#include <istream>

//! Reads OBJ material libraries from files or streams.
//! Named texture data is owned and shared until the stream list is replaced.
class RWObj_MtlReader
{
public:
  //! Main constructor.
  Standard_EXPORT RWObj_MtlReader(
    NCollection_DataMap<TCollection_AsciiString, RWObj_Material>& theMaterials);

  //! Destructor.
  Standard_EXPORT ~RWObj_MtlReader();

  //! Read the file.
  Standard_EXPORT bool Read(const TCollection_AsciiString& theFolder,
                            const TCollection_AsciiString& theFile);

  //! Read material definitions from the beginning of a seekable stream.
  //! @param[in,out] theStream material input
  //! @param[in] theName material name used to resolve relative image references
  //! @return false for a read error or a missing companion
  Standard_EXPORT bool Read(std::istream& theStream, const TCollection_AsciiString& theName);

  //! Set streams for texture references instead of files.
  //! @param[in] theStreams caller-owned list; must remain valid during Read()
  void SetResourceStreams(const DE_Provider::ReadStreamList& theStreams)
  {
    myTextures.Clear();
    myResourceStreams = &theStreams;
  }

private:
  //! Validate scalar value
  bool validateScalar(const double theValue);

  //! Validate RGB color
  bool validateColor(const NCollection_Vec3<float>& theVec);

  //! Process texture path.
  bool processTexturePath(TCollection_AsciiString&       theTexturePath,
                          const TCollection_AsciiString& theFolder,
                          occ::handle<Image_Texture>*    theSource = nullptr);

private:
  const DE_Provider::ReadStreamList* myResourceStreams = nullptr; //!< Borrowed named input streams.
  NCollection_DataMap<TCollection_AsciiString, occ::handle<Image_Texture>>
    myTextures; //!< Images shared by material libraries in the current stream list.
  TCollection_AsciiString                                       myPath;
  NCollection_DataMap<TCollection_AsciiString, RWObj_Material>* myMaterials;
  int                                                           myNbLines;
};

#endif // _RWObj_MtlReader_HeaderFile
