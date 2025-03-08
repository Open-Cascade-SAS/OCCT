// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _TDF_DeltaOnAddition_HeaderFile
#define _TDF_DeltaOnAddition_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDF_AttributeDelta.hxx>
class TDF_Attribute;

class TDF_DeltaOnAddition;
DEFINE_STANDARD_HANDLE(TDF_DeltaOnAddition, TDF_AttributeDelta)

//! This class provides default services for an
//! AttributeDelta on an ADDITION action.
//!
//! Applying this AttributeDelta means REMOVING its
//! attribute.
class TDF_DeltaOnAddition : public TDF_AttributeDelta
{

public:
  //! Creates a TDF_DeltaOnAddition.
  Standard_EXPORT TDF_DeltaOnAddition(const Handle(TDF_Attribute)& anAtt);

  //! Applies the delta to the attribute.
  Standard_EXPORT void Apply() Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(TDF_DeltaOnAddition, TDF_AttributeDelta)

protected:
private:
};

#endif // _TDF_DeltaOnAddition_HeaderFile
