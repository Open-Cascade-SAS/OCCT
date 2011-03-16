//--------------------------------------------------------------------
//
//  File Name : IGESBasic_ExternalReferenceFile.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESBasic_ExternalReferenceFile.ixx>
#include <Standard_DimensionMismatch.hxx>


    IGESBasic_ExternalReferenceFile::IGESBasic_ExternalReferenceFile ()    {  }


    void  IGESBasic_ExternalReferenceFile::Init
  (const Handle(Interface_HArray1OfHAsciiString)& aNameArray)
{
  if (aNameArray->Lower() != 1)
    Standard_DimensionMismatch::Raise("IGESBasic_ExternalReferenceFile : Init");
  theNames = aNameArray;
  InitTypeAndForm(406,12);
}

    Standard_Integer  IGESBasic_ExternalReferenceFile::NbListEntries () const
{
  return theNames->Length();
}

    Handle(TCollection_HAsciiString)  IGESBasic_ExternalReferenceFile::Name
  (const Standard_Integer Index) const
{
  return theNames->Value(Index);
}
