// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESBasic_ExternalReferenceFile.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESBasic_ExternalReferenceFile,IGESData_IGESEntity)

IGESBasic_ExternalReferenceFile::IGESBasic_ExternalReferenceFile ()    {  }


    void  IGESBasic_ExternalReferenceFile::Init
  (const Handle(Interface_HArray1OfHAsciiString)& aNameArray)
{
  if (aNameArray->Lower() != 1)
    throw Standard_DimensionMismatch("IGESBasic_ExternalReferenceFile : Init");
  theNames = aNameArray;
  InitTypeAndForm(406,12);
}

    Standard_Integer  IGESBasic_ExternalReferenceFile::NbListEntries () const
{
  return theNames->Length();
}

    Handle(TCollection_HAsciiString)  IGESBasic_ExternalReferenceFile::Name
  (const Standard_Integer Index) const
{
  return theNames->Value(Index);
}
