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

#ifndef _TDF_DeltaOnForget_HeaderFile
#define _TDF_DeltaOnForget_HeaderFile

#include <Standard.hxx>

#include <TDF_AttributeDelta.hxx>
class TDF_Attribute;


class TDF_DeltaOnForget;
DEFINE_STANDARD_HANDLE(TDF_DeltaOnForget, TDF_AttributeDelta)

//! This class provides default services for an
//! AttributeDelta on an Forget action.
//!
//! Applying this AttributeDelta means RESUMING its
//! attribute.
class TDF_DeltaOnForget : public TDF_AttributeDelta
{

public:

  
  //! Creates a TDF_DeltaOnForget.
  Standard_EXPORT TDF_DeltaOnForget(const Handle(TDF_Attribute)& anAtt);
  
  //! Applies the delta to the attribute.
  Standard_EXPORT void Apply() Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(TDF_DeltaOnForget,TDF_AttributeDelta)

protected:




private:




};







#endif // _TDF_DeltaOnForget_HeaderFile
