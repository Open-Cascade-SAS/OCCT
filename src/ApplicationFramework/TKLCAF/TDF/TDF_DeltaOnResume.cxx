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

//      	---------------------
// Version:	0.0
// Version	Date		Purpose
//		0.0	Jul  6 1998	Creation

#include <TDF_DeltaOnResume.hxx>

#include <Standard_Dump.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDF_DeltaOnResume, TDF_AttributeDelta)

//=================================================================================================

TDF_DeltaOnResume::TDF_DeltaOnResume(const Handle(TDF_Attribute)& anAtt)
    : TDF_AttributeDelta(anAtt)
{
}

//=================================================================================================

void TDF_DeltaOnResume::Apply()
{
  // Undo = Forget.
  Label().ForgetAttribute(Attribute());
#ifdef OCCT_DEBUG
  std::cout << "Forget attribute" << std::endl;
#endif
}

//=================================================================================================

void TDF_DeltaOnResume::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_AttributeDelta)
}
