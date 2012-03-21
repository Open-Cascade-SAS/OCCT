// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESAppli_FlowLineSpec.ixx>
#include <Standard_DimensionMismatch.hxx>


IGESAppli_FlowLineSpec::IGESAppli_FlowLineSpec ()    {  }

    void  IGESAppli_FlowLineSpec::Init
  (const Handle(Interface_HArray1OfHAsciiString)& allProperties)
{
  if (allProperties->Lower() != 1)
    Standard_DimensionMismatch::Raise("IGESAppli_FlowLineSpec : Init");
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
