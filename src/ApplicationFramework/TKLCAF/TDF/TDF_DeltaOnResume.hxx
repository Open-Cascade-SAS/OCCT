// Created by: DAUTRY Philippe
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _TDF_DeltaOnResume_HeaderFile
#define _TDF_DeltaOnResume_HeaderFile

#include <Standard.hxx>

#include <TDF_AttributeDelta.hxx>
class TDF_Attribute;

class TDF_DeltaOnResume;
DEFINE_STANDARD_HANDLE(TDF_DeltaOnResume, TDF_AttributeDelta)

//! This class provides default services for an
//! AttributeDelta on an Resume action.
//!
//! Applying this AttributeDelta means FORGETTING its
//! attribute.
class TDF_DeltaOnResume : public TDF_AttributeDelta
{

public:
  //! Creates a TDF_DeltaOnResume.
  Standard_EXPORT TDF_DeltaOnResume(const Handle(TDF_Attribute)& anAtt);

  //! Applies the delta to the attribute.
  Standard_EXPORT void Apply() Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(TDF_DeltaOnResume, TDF_AttributeDelta)

protected:
private:
};

#endif // _TDF_DeltaOnResume_HeaderFile
