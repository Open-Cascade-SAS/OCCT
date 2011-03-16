//--------------------------------------------------------------------
//
//  File Name : IGESGraph_DefinitionLevel.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGraph_DefinitionLevel.ixx>
#include <Standard_DimensionMismatch.hxx>


    IGESGraph_DefinitionLevel::IGESGraph_DefinitionLevel ()    {  }


    void IGESGraph_DefinitionLevel::Init
  (const Handle(TColStd_HArray1OfInteger)& allLevelNumbers)
{
  if (allLevelNumbers->Lower() != 1)
    Standard_DimensionMismatch::Raise("IGESGraph_DefinitionLevel : Init");
  theLevelNumbers = allLevelNumbers;
  InitTypeAndForm(406,1);
}

    Standard_Integer IGESGraph_DefinitionLevel::NbPropertyValues () const
{
  return ( theLevelNumbers->Length() );
}

    Standard_Integer IGESGraph_DefinitionLevel::NbLevelNumbers () const
{
  return ( theLevelNumbers->Length() );
}

    Standard_Integer IGESGraph_DefinitionLevel::LevelNumber
  (const Standard_Integer LevelIndex) const
{
  return ( theLevelNumbers->Value(LevelIndex) );
}
