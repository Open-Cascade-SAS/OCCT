//--------------------------------------------------------------------
//
//  File Name : IGESAppli_PWBArtworkStackup.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESAppli_PWBArtworkStackup.ixx>
#include <Standard_DimensionMismatch.hxx>


    IGESAppli_PWBArtworkStackup::IGESAppli_PWBArtworkStackup ()    {  }


    void  IGESAppli_PWBArtworkStackup::Init
  (const Standard_Integer nbPropVal,
   const Handle(TCollection_HAsciiString)& anArtIdent,
   const Handle(TColStd_HArray1OfInteger)& allLevelNums)
{
  if (allLevelNums->Lower() != 1)
    Standard_DimensionMismatch::Raise("IGESAppli_PWBArtworkStackup : Init");
  theNbPropertyValues    = nbPropVal;
  theArtworkStackupIdent = anArtIdent;
  theLevelNumbers        = allLevelNums;
  InitTypeAndForm(406,25);
}

    Standard_Integer  IGESAppli_PWBArtworkStackup::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Handle(TCollection_HAsciiString)  IGESAppli_PWBArtworkStackup::Identification () const
{
  return theArtworkStackupIdent;
}

    Standard_Integer  IGESAppli_PWBArtworkStackup::NbLevelNumbers () const
{
  return theLevelNumbers->Length();
}

    Standard_Integer  IGESAppli_PWBArtworkStackup::LevelNumber
  (const Standard_Integer Index) const
{
  return theLevelNumbers->Value(Index);
}
