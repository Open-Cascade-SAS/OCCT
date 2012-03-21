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

#include <IGESSolid_BooleanTree.ixx>
#include <Standard_DimensionError.hxx>


IGESSolid_BooleanTree::IGESSolid_BooleanTree ()    {  }


    void  IGESSolid_BooleanTree::Init
  (const Handle(IGESData_HArray1OfIGESEntity)& operands,
   const Handle(TColStd_HArray1OfInteger)& operations)
{
  if (operands->Lower()  != 1 || operations->Lower() != 1 ||
      operands->Length() != operations->Length())
    Standard_DimensionError::Raise("IGESSolid_BooleanTree : Init");

  theOperations = operations;
  theOperands   = operands;
  InitTypeAndForm(180,0);
}

    Standard_Integer IGESSolid_BooleanTree::Length () const
{
  return theOperands->Length();
}

    Standard_Boolean IGESSolid_BooleanTree::IsOperand
  (const Standard_Integer Index) const
{
  return (!theOperands->Value(Index).IsNull());
}

    Handle(IGESData_IGESEntity) IGESSolid_BooleanTree::Operand
  (const Standard_Integer Index) const
{
  return theOperands->Value(Index);
}

    Standard_Integer IGESSolid_BooleanTree::Operation
  (const Standard_Integer Index) const
{
  if (theOperands->Value(Index).IsNull())
    return theOperations->Value(Index);
  else
    return 0;      // It is not an operation. (operations can be : 1-2-3)
}
