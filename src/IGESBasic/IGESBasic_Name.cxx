//--------------------------------------------------------------------
//
//  File Name : IGESBasic_Name.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESBasic_Name.ixx>


    IGESBasic_Name::IGESBasic_Name ()    {  }


    void  IGESBasic_Name::Init
  (const Standard_Integer nbPropVal,
   const Handle(TCollection_HAsciiString)& aName)
{
  theName             = aName;
  theNbPropertyValues = nbPropVal;
  InitTypeAndForm(406,15);
}


    Standard_Integer  IGESBasic_Name::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Handle(TCollection_HAsciiString)  IGESBasic_Name::Value () const
{
  return theName;
}
