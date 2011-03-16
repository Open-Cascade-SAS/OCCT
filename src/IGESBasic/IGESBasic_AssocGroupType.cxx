//--------------------------------------------------------------------
//
//  File Name : IGESBasic_AssocGroupType.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESBasic_AssocGroupType.ixx>


    IGESBasic_AssocGroupType::IGESBasic_AssocGroupType ()    {  }


    void  IGESBasic_AssocGroupType::Init
  (const Standard_Integer nbDataFields,
   const Standard_Integer aType, const Handle(TCollection_HAsciiString)& aName)
{
  theNbData = nbDataFields;
  theType   = aType;
  theName   = aName;
  InitTypeAndForm(406,23);
}


    Standard_Integer  IGESBasic_AssocGroupType::NbData () const
{
  return theNbData;
}

    Standard_Integer  IGESBasic_AssocGroupType::AssocType () const
{
  return theType;
}

    Handle(TCollection_HAsciiString)  IGESBasic_AssocGroupType::Name () const
{
  return theName;
}
