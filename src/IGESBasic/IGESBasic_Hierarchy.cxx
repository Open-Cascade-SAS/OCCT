//--------------------------------------------------------------------
//
//  File Name : IGESBasic_Hierarchy.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESBasic_Hierarchy.ixx>

    IGESBasic_Hierarchy::IGESBasic_Hierarchy ()    {  }


    void  IGESBasic_Hierarchy::Init
  (const Standard_Integer nbPropVal,
   const Standard_Integer aLineFont,     const Standard_Integer aView,
   const Standard_Integer anEntityLevel, const Standard_Integer aBlankStatus,
   const Standard_Integer aLineWt,       const Standard_Integer aColorNum)
{
  theLineFont         = aLineFont;
  theView             = aView;
  theEntityLevel      = anEntityLevel;
  theBlankStatus      = aBlankStatus;
  theLineWeight       = aLineWt;
  theColorNum         = aColorNum;
  theNbPropertyValues = nbPropVal;
  InitTypeAndForm(406,10);
}


    Standard_Integer  IGESBasic_Hierarchy::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Standard_Integer  IGESBasic_Hierarchy::NewLineFont () const
{
  return theLineFont;
}

    Standard_Integer  IGESBasic_Hierarchy::NewView () const
{
  return theView;
}

    Standard_Integer  IGESBasic_Hierarchy::NewEntityLevel () const
{
  return theEntityLevel;
}

    Standard_Integer  IGESBasic_Hierarchy::NewBlankStatus () const
{
  return theBlankStatus;
}

    Standard_Integer  IGESBasic_Hierarchy::NewLineWeight () const
{
  return theLineWeight;
}

    Standard_Integer  IGESBasic_Hierarchy::NewColorNum () const
{
  return theColorNum;
}
