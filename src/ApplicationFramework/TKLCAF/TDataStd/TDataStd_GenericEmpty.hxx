// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef _TDataStd_GenericEmpty_HeaderFile
#define _TDataStd_GenericEmpty_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDF_DerivedAttribute.hxx>

class TDF_RelocationTable;

//! An ancestor attribute for all attributes which have no fields.
//! If an attribute inherits this one it should not have drivers for persistence.
class TDataStd_GenericEmpty : public TDF_Attribute
{

public:
  Standard_EXPORT void Restore(const occ::handle<TDF_Attribute>&) override {};

  Standard_EXPORT void Paste(const occ::handle<TDF_Attribute>&,
                             const occ::handle<TDF_RelocationTable>&) const override
  {
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

  DEFINE_STANDARD_RTTIEXT(TDataStd_GenericEmpty, TDF_Attribute)
};

#endif // _TDataStd_GenericEmpty_HeaderFile
