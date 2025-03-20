// Author: Kirill Gavrilov
// Copyright (c) 2016-2019 OPEN CASCADE SAS
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

#ifndef _RWMesh_HeaderFile
#define _RWMesh_HeaderFile

#include <TDF_Label.hxx>
#include <RWMesh_NameFormat.hxx>

//! Auxiliary tools for RWMesh package.
class RWMesh
{
public:
  //! Read name attribute from label.
  Standard_EXPORT static TCollection_AsciiString ReadNameAttribute(const TDF_Label& theLabel);

  //! Generate name for specified labels.
  //! @param[in] theFormat   name format to apply
  //! @param[in] theLabel    instance label
  //! @param[in] theRefLabel product label
  Standard_EXPORT static TCollection_AsciiString FormatName(RWMesh_NameFormat theFormat,
                                                            const TDF_Label&  theLabel,
                                                            const TDF_Label&  theRefLabel);
};

#endif // _RWMesh_HeaderFile
