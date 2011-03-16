//--------------------------------------------------------------------
//
//  File Name : IGESGraph_Pick.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGraph_Pick.ixx>

    IGESGraph_Pick::IGESGraph_Pick ()    {  }


    void IGESGraph_Pick::Init
  (const Standard_Integer nbProps, const Standard_Integer aPickStatus)
{
  theNbPropertyValues = nbProps;
  thePick             = aPickStatus;
  InitTypeAndForm(406,21);
}

    Standard_Integer IGESGraph_Pick::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Standard_Integer IGESGraph_Pick::PickFlag () const
{
  return thePick;
}

    Standard_Boolean IGESGraph_Pick::IsPickable () const
{
  return (thePick == 0);
}
