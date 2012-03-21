// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <StepBasic_DocumentUsageConstraint.ixx>

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
