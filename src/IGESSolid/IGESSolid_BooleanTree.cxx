//--------------------------------------------------------------------
//
//  File Name : IGESSolid_BooleanTree.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
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
