//--------------------------------------------------------------------
//
//  File Name : IGESGraph_HighLight.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGraph_HighLight.ixx>

    IGESGraph_HighLight::IGESGraph_HighLight ()    {  }


    void IGESGraph_HighLight::Init
  (const Standard_Integer nbProps, const Standard_Integer aHighLightStatus)
{
  theNbPropertyValues = nbProps;
  theHighLight        = aHighLightStatus;
  InitTypeAndForm(406,20);
}

    Standard_Integer IGESGraph_HighLight::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Standard_Integer IGESGraph_HighLight::HighLightStatus () const
{
  return theHighLight;
}

    Standard_Boolean IGESGraph_HighLight::IsHighLighted () const
{
  return (theHighLight != 0);
}
