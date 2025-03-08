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

//      	----------------------
// Version:	0.0
// Version	Date		Purpose
//		0.0	Sep  8 1997	Creation

#include <TDF_AttributeDelta.hxx>

#include <Standard_Dump.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDF_AttributeDelta, Standard_Transient)

//=================================================================================================

TDF_AttributeDelta::TDF_AttributeDelta(const Handle(TDF_Attribute)& anAttribute)
    : myAttribute(anAttribute),
      myLabel(anAttribute->Label())
{
}

//=================================================================================================

TDF_Label TDF_AttributeDelta::Label() const
{
  return myLabel;
}

//=================================================================================================

Handle(TDF_Attribute) TDF_AttributeDelta::Attribute() const
{
  return myAttribute;
}

//=================================================================================================

Standard_GUID TDF_AttributeDelta::ID() const
{
  return myAttribute->ID();
}

//=================================================================================================

Standard_OStream& TDF_AttributeDelta::Dump(Standard_OStream& OS) const
{
  static TCollection_AsciiString entry;
  TDF_Tool::Entry(Label(), entry);
  OS << this->DynamicType()->Name() << " at " << entry;
  OS << " on " << Attribute()->DynamicType()->Name();
  return OS;
}

//=================================================================================================

void TDF_AttributeDelta::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myAttribute.get())

  TCollection_AsciiString aStrForTDF_Label;
  TDF_Tool::Entry(myLabel, aStrForTDF_Label);
  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, aStrForTDF_Label)
}
