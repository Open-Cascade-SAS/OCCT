//--------------------------------------------------------------------
//
//  File Name : IGESGraph_NominalSize.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGraph_NominalSize.ixx>


    IGESGraph_NominalSize::IGESGraph_NominalSize ()    {  }


    void IGESGraph_NominalSize::Init
  (const Standard_Integer          nbProps,
   const Standard_Real             aNominalSizeValue,
   const Handle(TCollection_HAsciiString)& aNominalSizeName,
   const Handle(TCollection_HAsciiString)& aStandardName)
{
  theNbPropertyValues = nbProps;
  theNominalSizeValue = aNominalSizeValue;
  theNominalSizeName  = aNominalSizeName;
  theStandardName     = aStandardName;
  InitTypeAndForm(406,13);
}

    Standard_Integer IGESGraph_NominalSize::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Standard_Real IGESGraph_NominalSize::NominalSizeValue () const
{
  return theNominalSizeValue;
}

    Handle(TCollection_HAsciiString) IGESGraph_NominalSize::NominalSizeName () const
{
  return theNominalSizeName;
}

    Standard_Boolean IGESGraph_NominalSize::HasStandardName () const
{
  return (! theStandardName.IsNull() );
}

    Handle(TCollection_HAsciiString) IGESGraph_NominalSize::StandardName () const
{
  return theStandardName;
}
