// Created on: 1997-03-04
// Created by: Robert COUBLANC
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

#include <AIS_InteractiveObject.hxx>
#include <AIS_SignatureFilter.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_SignatureFilter, AIS_TypeFilter)

AIS_SignatureFilter::AIS_SignatureFilter(const AIS_KindOfInteractive TheKind,
                                         const Standard_Integer      TheSignature)
    : AIS_TypeFilter(TheKind),
      mySig(TheSignature)
{
}

Standard_Boolean AIS_SignatureFilter::IsOk(const Handle(SelectMgr_EntityOwner)& anObj) const
{
  Handle(AIS_InteractiveObject) IO = Handle(AIS_InteractiveObject)::DownCast(anObj->Selectable());
  if (IO.IsNull())
    return Standard_False;

  return (IO->Signature() == mySig && IO->Type() == myKind);
}
