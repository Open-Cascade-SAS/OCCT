//--------------------------------------------------------------------
//
//  File Name : IGESBasic_ExternalRefFileIndex.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESBasic_ExternalRefFileIndex.ixx>


    IGESBasic_ExternalRefFileIndex::IGESBasic_ExternalRefFileIndex ()    {  }

    void  IGESBasic_ExternalRefFileIndex::Init
  (const Handle(Interface_HArray1OfHAsciiString)& aNameArray,
   const Handle(IGESData_HArray1OfIGESEntity)& allEntities)
{
  if (aNameArray->Lower()  != 1 || allEntities->Lower() != 1 ||
      aNameArray->Length() != allEntities->Length())
    Standard_DimensionMismatch::Raise("IGESBasic_ExternalRefFileIndex: Init");

  theNames = aNameArray;
  theEntities = allEntities;
  InitTypeAndForm(402,12);
}

    Standard_Integer  IGESBasic_ExternalRefFileIndex::NbEntries () const
{
  return theNames->Length();
}

    Handle(TCollection_HAsciiString)  IGESBasic_ExternalRefFileIndex::Name
  (const Standard_Integer Index) const
{
  return theNames->Value(Index);
}

    Handle(IGESData_IGESEntity)  IGESBasic_ExternalRefFileIndex::Entity
  (const Standard_Integer Index) const
{
  return theEntities->Value(Index);
}
