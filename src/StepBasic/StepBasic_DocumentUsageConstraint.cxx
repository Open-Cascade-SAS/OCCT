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


#include <Standard_Type.hxx>
#include <StepBasic_Document.hxx>
#include <StepBasic_DocumentUsageConstraint.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_DocumentUsageConstraint,Standard_Transient)

StepBasic_DocumentUsageConstraint::StepBasic_DocumentUsageConstraint  ()    {  }

void  StepBasic_DocumentUsageConstraint::Init
  (const Handle(StepBasic_Document)& aSource,
   const Handle(TCollection_HAsciiString)& ase,
   const Handle(TCollection_HAsciiString)& asev)
{
  theSource = aSource;
  theSE  = ase;
  theSEV = asev;
}

Handle(StepBasic_Document)  StepBasic_DocumentUsageConstraint::Source () const
{  return theSource;  }

void  StepBasic_DocumentUsageConstraint::SetSource (const Handle(StepBasic_Document)& aSource)
{  theSource = aSource;  }

Handle(TCollection_HAsciiString)  StepBasic_DocumentUsageConstraint::SubjectElement () const
{  return theSE;  }

void  StepBasic_DocumentUsageConstraint::SetSubjectElement (const Handle(TCollection_HAsciiString)& ase)
{  theSE  = ase;  }

Handle(TCollection_HAsciiString)  StepBasic_DocumentUsageConstraint::SubjectElementValue () const
{  return theSEV;  }

void  StepBasic_DocumentUsageConstraint::SetSubjectElementValue (const Handle(TCollection_HAsciiString)& asev)
{  theSEV = asev;  }
