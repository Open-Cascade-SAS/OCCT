//--------------------------------------------------------------------
//
//  File Name : IGESGraph_IntercharacterSpacing.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGraph_IntercharacterSpacing.ixx>

    IGESGraph_IntercharacterSpacing::IGESGraph_IntercharacterSpacing ()    {  }

    void IGESGraph_IntercharacterSpacing::Init
  (const Standard_Integer nbProps, const Standard_Real anISpace)
{
  theNbPropertyValues = nbProps;
  theISpace           = anISpace;
  InitTypeAndForm(406,18);
}

    Standard_Integer IGESGraph_IntercharacterSpacing::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Standard_Real IGESGraph_IntercharacterSpacing::ISpace () const
{
  return theISpace;
}
