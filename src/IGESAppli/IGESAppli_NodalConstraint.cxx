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

#include <IGESAppli_NodalConstraint.ixx>
#include <Standard_DimensionMismatch.hxx>


IGESAppli_NodalConstraint::IGESAppli_NodalConstraint ()    {  }


    void  IGESAppli_NodalConstraint::Init
  (const Standard_Integer aType,
   const Handle(IGESAppli_Node)& aNode,
   const Handle(IGESDefs_HArray1OfTabularData)& allTabData)
{
  if (allTabData->Lower() != 1)
    Standard_DimensionMismatch::Raise("IGESAppli_NodalConstraint : Init");
  theType = aType;
  theNode = aNode;
  theTabularDataProps = allTabData;
  InitTypeAndForm(418,0);
}

    Standard_Integer  IGESAppli_NodalConstraint::NbCases () const
{
  return theTabularDataProps->Length();
}

    Standard_Integer  IGESAppli_NodalConstraint::Type () const
{
  return theType;
}

    Handle(IGESAppli_Node)  IGESAppli_NodalConstraint::NodeEntity () const
{
  return theNode;
}

    Handle(IGESDefs_TabularData)  IGESAppli_NodalConstraint::TabularData
  (const Standard_Integer Index) const
{
  return theTabularDataProps->Value(Index);
}
