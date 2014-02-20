// Created on: 2008-03-28
// Created by: Sergey ZARITCHNY
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#include <MDataStd_RealArrayRetrievalDriver_1.ixx>

#include <PDataStd_RealArray.hxx>
#include <PDataStd_RealArray_1.hxx>
#include <TDataStd_RealArray.hxx>
#include <MDataStd.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MDataStd_RealArrayRetrievalDriver_1
//purpose  : 
//=======================================================================

MDataStd_RealArrayRetrievalDriver_1::MDataStd_RealArrayRetrievalDriver_1
  (const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataStd_RealArrayRetrievalDriver_1::VersionNumber() const
{ return 1; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataStd_RealArrayRetrievalDriver_1::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PDataStd_RealArray_1);
  return sourceType;
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MDataStd_RealArrayRetrievalDriver_1::NewEmpty () const {

  return new TDataStd_RealArray ();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataStd_RealArrayRetrievalDriver_1::Paste(const Handle(PDF_Attribute)& Source,
      const Handle(TDF_Attribute)& Target,
      const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataStd_RealArray_1) S = Handle(PDataStd_RealArray_1)::DownCast (Source);
  Handle(TDataStd_RealArray)   T = Handle(TDataStd_RealArray)::DownCast (Target);

  Standard_Integer i, lower = S->Lower() , upper = S->Upper();
  T->Init(lower, upper);
  
  for(i = lower; i<=upper; i++) T->SetValue( i, S->Value(i) );
  T->SetDelta(S->GetDelta());
}

