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

#include <MDF_ReferenceRetrievalDriver.ixx>
#include <PCollection_HAsciiString.hxx>
#include <PDF_Reference.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TDF_Reference.hxx>
#include <CDM_MessageDriver.hxx>

#define DeclareAndSpeedCast(V,T,Vdown) Handle(T) Vdown = *((Handle(T)*)& V)
#define DeclareConstAndSpeedCast(V,T,Vdown) const Handle(T)& Vdown = (Handle(T)&) V
#define SpeedCast(V,T,Vdown) Vdown = *((Handle(T)*)& V)

MDF_ReferenceRetrievalDriver::MDF_ReferenceRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}

Standard_Integer MDF_ReferenceRetrievalDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDF_ReferenceRetrievalDriver::SourceType() const
{ return STANDARD_TYPE (PDF_Reference); }

Handle(TDF_Attribute) MDF_ReferenceRetrievalDriver::NewEmpty() const
{ return new TDF_Reference; }

void MDF_ReferenceRetrievalDriver::Paste (
  const Handle(PDF_Attribute)&        Source,
  const Handle(TDF_Attribute)&        Target,
//  const Handle(MDF_RRelocationTable)& RelocTable) const
  const Handle(MDF_RRelocationTable)& ) const

{
  DeclareAndSpeedCast(Source,PDF_Reference,S);
  DeclareAndSpeedCast(Target,TDF_Reference,T);
  TDF_Label tLab; // Null label.
  Handle(PCollection_HAsciiString) pEntry = S->ReferencedLabel();
  if (!pEntry.IsNull()) {
    TCollection_AsciiString entry = pEntry->Convert();
    TDF_Tool::Label(T->Label().Data(),entry,tLab,Standard_True);
  }
  T->Set(tLab);
}

