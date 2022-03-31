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

#ifndef _RWGltf_GltfOStreamWriter_HeaderFile
#define _RWGltf_GltfOStreamWriter_HeaderFile

// disable warnings, occures in rapidjson
#include <Standard_WarningsDisable.hxx>
#include <rapidjson/prettywriter.h>
#include <rapidjson/ostreamwrapper.h>
#include <Standard_WarningsRestore.hxx>

//! rapidjson::Writer wrapper for forward declaration.
class RWGltf_GltfOStreamWriter : public rapidjson::Writer<rapidjson::OStreamWrapper>
{
public:
  //! Main constructor.
  RWGltf_GltfOStreamWriter (rapidjson::OStreamWrapper& theOStream)
  : rapidjson::Writer<rapidjson::OStreamWrapper> (theOStream) {}
};

#endif // _RWGltf_GltfOStreamWriter_HeaderFile
