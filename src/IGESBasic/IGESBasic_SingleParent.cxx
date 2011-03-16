//--------------------------------------------------------------------
//
//  File Name : IGESBasic_SingleParent.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESBasic_SingleParent.ixx>
#include <Standard_DimensionMismatch.hxx>


    IGESBasic_SingleParent::IGESBasic_SingleParent ()    {  }


    void  IGESBasic_SingleParent::Init
  (const Standard_Integer nbParentEntities,
   const Handle(IGESData_IGESEntity)& aParentEntity,
   const Handle(IGESData_HArray1OfIGESEntity)& allChildren)
{
  if (!allChildren.IsNull() && allChildren->Lower() != 1)
    Standard_DimensionMismatch::Raise("IGESBasic_SingleParent : Init");
  theParentEntity     = aParentEntity;
  theChildren         = allChildren;
  theNbParentEntities = nbParentEntities;
  InitTypeAndForm(402,9);
}


    Standard_Integer  IGESBasic_SingleParent::NbChildren () const
{
  return(theChildren.IsNull() ? 0 : theChildren->Length());
}

    Handle(IGESData_IGESEntity)  IGESBasic_SingleParent::Child
  (const Standard_Integer Index) const
{
  return theChildren->Value(Index);
}

    Standard_Integer  IGESBasic_SingleParent::NbParentEntities () const
{
  return theNbParentEntities;
}
    Handle(IGESData_IGESEntity)  IGESBasic_SingleParent::SingleParent () const
{  return theParentEntity;  }

