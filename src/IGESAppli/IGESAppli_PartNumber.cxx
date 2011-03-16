//--------------------------------------------------------------------
//
//  File Name : IGESAppli_PartNumber.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESAppli_PartNumber.ixx>


    IGESAppli_PartNumber::IGESAppli_PartNumber ()    {  }


    void  IGESAppli_PartNumber::Init
  (const Standard_Integer nbPropVal,
   const Handle(TCollection_HAsciiString)& aGenName,
   const Handle(TCollection_HAsciiString)& aMilName,
   const Handle(TCollection_HAsciiString)& aVendName,
   const Handle(TCollection_HAsciiString)& anIntName)
{
  theNbPropertyValues = nbPropVal;
  theGenericNumber    = aGenName;
  theMilitaryNumber   = aMilName;
  theVendorNumber     = aVendName;
  theInternalNumber   = anIntName;
  InitTypeAndForm(406,9);
}


    Standard_Integer  IGESAppli_PartNumber::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Handle(TCollection_HAsciiString)  IGESAppli_PartNumber::GenericNumber () const
{
  return theGenericNumber;
}

    Handle(TCollection_HAsciiString)  IGESAppli_PartNumber::MilitaryNumber () const
{
  return theMilitaryNumber;
}

    Handle(TCollection_HAsciiString)  IGESAppli_PartNumber::VendorNumber () const
{
  return theVendorNumber;
}

    Handle(TCollection_HAsciiString)  IGESAppli_PartNumber::InternalNumber () const
{
  return theInternalNumber;
}
