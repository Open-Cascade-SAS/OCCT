//--------------------------------------------------------------------
//
//  File Name : IGESGraph_DrawingSize.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGraph_DrawingSize.ixx>

    IGESGraph_DrawingSize::IGESGraph_DrawingSize ()    {  }


    void IGESGraph_DrawingSize::Init
  (const Standard_Integer nbProps, const Standard_Real aXSize, 
   const Standard_Real    aYSize)
{
  theNbPropertyValues = nbProps;
  theXSize            = aXSize;
  theYSize            = aYSize;
  InitTypeAndForm(406,16);
}


    Standard_Integer IGESGraph_DrawingSize::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Standard_Real IGESGraph_DrawingSize::XSize () const
{
  return theXSize;
}

    Standard_Real IGESGraph_DrawingSize::YSize () const
{
  return theYSize;
}
