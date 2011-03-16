//--------------------------------------------------------------------
//
//  File Name : IGESBasic_SubfigureDef.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESBasic_SubfigureDef.ixx>
#include <Standard_DimensionMismatch.hxx>


    IGESBasic_SubfigureDef::IGESBasic_SubfigureDef ()    {  }


    void  IGESBasic_SubfigureDef::Init
  (const Standard_Integer aDepth,
   const Handle(TCollection_HAsciiString)& aName,
   const Handle(IGESData_HArray1OfIGESEntity)& allAssocEntities)
{
  if (!allAssocEntities.IsNull() && allAssocEntities->Lower() != 1)
    Standard_DimensionMismatch::Raise("IGESBasic_SubfigureDef : Init");
  theDepth         = aDepth;
  theName          = aName;
  theAssocEntities = allAssocEntities;
  InitTypeAndForm(308,0);
}

    Standard_Integer  IGESBasic_SubfigureDef::Depth () const
{
  return theDepth;
}

    Handle(TCollection_HAsciiString)  IGESBasic_SubfigureDef::Name () const
{
  return theName;
}

    Standard_Integer  IGESBasic_SubfigureDef::NbEntities () const
{
  return (theAssocEntities.IsNull() ? 0 : theAssocEntities->Length());
}

    Handle(IGESData_IGESEntity)  IGESBasic_SubfigureDef::AssociatedEntity
  (const Standard_Integer Index) const
{
  return theAssocEntities->Value(Index);
}

    Handle(Standard_Transient) IGESBasic_SubfigureDef::Value
  (const Standard_Integer Index) const
{
  return theAssocEntities->Value(Index);
}
