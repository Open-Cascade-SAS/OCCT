// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <MDataStd_ExpressionStorageDriver.ixx>
#include <PDataStd_Expression.hxx>
#include <TDataStd_Expression.hxx>
#include <TCollection_ExtendedString.hxx>
#include <PCollection_HExtendedString.hxx>
#include <TDF_ListIteratorOfAttributeList.hxx> 
#include <PDF_HAttributeArray1.hxx> 
#include <Standard_NoSuchObject.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MDataStd_ExpressionStorageDriver
//purpose  : 
//=======================================================================

MDataStd_ExpressionStorageDriver::MDataStd_ExpressionStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataStd_ExpressionStorageDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataStd_ExpressionStorageDriver::SourceType() const
{ return STANDARD_TYPE(TDataStd_Expression);}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(PDF_Attribute) MDataStd_ExpressionStorageDriver::NewEmpty() const
{ return new PDataStd_Expression; }

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataStd_ExpressionStorageDriver::Paste(const Handle(TDF_Attribute)& Source,
					     const Handle(PDF_Attribute)& Target,
					     const Handle(MDF_SRelocationTable)& RelocTable) const
{
  Handle(TDataStd_Expression) S = Handle(TDataStd_Expression)::DownCast (Source);
  Handle(PDataStd_Expression) T = Handle(PDataStd_Expression)::DownCast (Target);
  Handle(PCollection_HExtendedString) Expression = new PCollection_HExtendedString (S->Name());
  T->SetName (Expression);

  Handle(TDF_Attribute) TV;   
  Handle(PDF_Attribute) PV;
  Standard_Integer nbvar = S->GetVariables().Extent();
  if (nbvar <= 0) return;
  Handle(PDF_HAttributeArray1) PVARS = new PDF_HAttributeArray1 (1, nbvar);
  TDF_ListIteratorOfAttributeList it;
  Standard_Integer index = 0;
  for (it.Initialize(S->GetVariables());it.More();it.Next()) {
    index++;
    TV = it.Value(); 
    if(!RelocTable->HasRelocation (TV, PV)) {
      Standard_NoSuchObject::Raise("MDataStd_ExpressionStorageDriver::Paste");
    }
    PVARS->SetValue (index,PV);
  }
  T->SetVariables (PVARS);
}

