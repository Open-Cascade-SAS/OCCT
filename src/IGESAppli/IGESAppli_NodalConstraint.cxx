//--------------------------------------------------------------------
//
//  File Name : IGESAppli_NodalConstraint.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
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
