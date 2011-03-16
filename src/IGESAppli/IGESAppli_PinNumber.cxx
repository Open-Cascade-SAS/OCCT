//--------------------------------------------------------------------
//
//  File Name : IGESAppli_PinNumber.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESAppli_PinNumber.ixx>


    IGESAppli_PinNumber::IGESAppli_PinNumber ()    {  }


    void  IGESAppli_PinNumber::Init
  (const Standard_Integer nbPropVal,
   const Handle(TCollection_HAsciiString)& aValue)
{
  thePinNumber        = aValue;
  theNbPropertyValues = nbPropVal;
  InitTypeAndForm(406,8);
}


    Standard_Integer  IGESAppli_PinNumber::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Handle(TCollection_HAsciiString)  IGESAppli_PinNumber::PinNumberVal () const
{
  return thePinNumber;
}
