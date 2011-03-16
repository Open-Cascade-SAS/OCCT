//--------------------------------------------------------------------
//
//  File Name : IGESAppli_FlowLineSpec.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESAppli_FlowLineSpec.ixx>
#include <Standard_DimensionMismatch.hxx>


    IGESAppli_FlowLineSpec::IGESAppli_FlowLineSpec ()    {  }

    void  IGESAppli_FlowLineSpec::Init
  (const Handle(Interface_HArray1OfHAsciiString)& allProperties)
{
  if (allProperties->Lower() != 1)
    Standard_DimensionMismatch::Raise("IGESAppli_FlowLineSpec : Init");
  theNameAndModifiers = allProperties;
  InitTypeAndForm(406,14);
}

    Standard_Integer  IGESAppli_FlowLineSpec::NbPropertyValues () const
{
  return theNameAndModifiers->Length();
}

    Handle(TCollection_HAsciiString)  IGESAppli_FlowLineSpec::FlowLineName () const
{
  return theNameAndModifiers->Value(1);
}

    Handle(TCollection_HAsciiString)  IGESAppli_FlowLineSpec::Modifier
  (const Standard_Integer Index) const
{
  return theNameAndModifiers->Value(Index);
}
