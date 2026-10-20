// Copyright (c) 2022 OPEN CASCADE SAS
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

#ifndef _VrmlAPI_Reader_HeaderFile
#define _VrmlAPI_Reader_HeaderFile

#include <DE_Provider.hxx>
#include <RWMesh_CafReader.hxx>

//! The Vrml mesh reader into XDE document.
class VrmlAPI_CafReader : public RWMesh_CafReader
{
  DEFINE_STANDARD_RTTIEXT(VrmlAPI_CafReader, RWMesh_CafReader)

public:
  //! Set streams for Inline scenes and images instead of files.
  //! @param[in] theStreams caller-owned list used by Perform(); null restores file lookup
  void SetResourceStreams(const DE_Provider::ReadStreamList* theStreams)
  {
    myResourceStreams = theStreams;
  }

protected:
  //! Read the mesh data from specified file.
  //! @param theStream   input stream
  //! @param theFile     path of additional files
  //! @param theProgress progress indicator
  //! @param theToProbe  flag for probing file without complete reading. Not supported.
  //! @return false when theToProbe is set to true or reading has completed with error.
  Standard_EXPORT bool performMesh(std::istream&                  theStream,
                                   const TCollection_AsciiString& theFile,
                                   const Message_ProgressRange&   theProgress,
                                   const bool                     theToProbe) override;

private:
  const DE_Provider::ReadStreamList* myResourceStreams = nullptr; //!< Borrowed named streams.
};

#endif // _VrmlAPI_Reader_HeaderFile
