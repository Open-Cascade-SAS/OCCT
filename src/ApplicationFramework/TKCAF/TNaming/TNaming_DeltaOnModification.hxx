// Created on: 1997-12-03
// Created by: Yves FRICAUD
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

#ifndef _TNaming_DeltaOnModification_HeaderFile
#define _TNaming_DeltaOnModification_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TDF_DeltaOnModification.hxx>
class TNaming_NamedShape;

//! This class provides default services for an
//! AttributeDelta on a MODIFICATION action.
//!
//! Applying this AttributeDelta means GOING BACK to
//! the attribute previously registered state.
class TNaming_DeltaOnModification : public TDF_DeltaOnModification
{

public:
  //! Initializes a TDF_DeltaOnModification.
  Standard_EXPORT TNaming_DeltaOnModification(const occ::handle<TNaming_NamedShape>& NS);

  //! Applies the delta to the attribute.
  Standard_EXPORT virtual void Apply() override;

  DEFINE_STANDARD_RTTIEXT(TNaming_DeltaOnModification, TDF_DeltaOnModification)

private:
  occ::handle<NCollection_HArray1<TopoDS_Shape>> myOld;
  occ::handle<NCollection_HArray1<TopoDS_Shape>> myNew;
};

#endif // _TNaming_DeltaOnModification_HeaderFile
