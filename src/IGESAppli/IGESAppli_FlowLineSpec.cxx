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

#include <IGESAppli_FlowLineSpec.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_FlowLineSpec,IGESData_IGESEntity)

IGESAppli_FlowLineSpec::IGESAppli_FlowLineSpec ()    {  }

    void  IGESAppli_FlowLineSpec::Init
  (const Handle(Interface_HArray1OfHAsciiString)& allProperties)
{
  if (allProperties->Lower() != 1)
    throw Standard_DimensionMismatch("IGESAppli_FlowLineSpec : Init");
  theNameAndModifiers = allProperties;
  InitTypeAndForm(406,14);
}

    Standard_Integer  IGESAppli_FlowLineSpec::NbPropertyValues () const
{
  return theNameAndModifiers->Length();
}

    Handle(TCollection_HAsciiString)  IGESAppli_FlowLineSpec::FlowLineName () const
{
  return theNameAndModifiers->Value(1);
}

    Handle(TCollection_HAsciiString)  IGESAppli_FlowLineSpec::Modifier
  (const Standard_Integer Index) const
{
  return theNameAndModifiers->Value(Index);
}
