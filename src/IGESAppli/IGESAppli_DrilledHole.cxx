//--------------------------------------------------------------------
//
//  File Name : IGESAppli_DrilledHole.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESAppli_DrilledHole.ixx>


    IGESAppli_DrilledHole::IGESAppli_DrilledHole ()    {  }

    void  IGESAppli_DrilledHole::Init
  (const Standard_Integer nbPropVal,
   const Standard_Real    aSize,    const Standard_Real    anotherSize,
   const Standard_Integer aPlating, const Standard_Integer aLayer,
   const Standard_Integer anotherLayer)
{
  theNbPropertyValues = nbPropVal;
  theDrillDiaSize     = aSize;
  theFinishDiaSize    = anotherSize;
  thePlatingFlag      = aPlating;
  theNbLowerLayer     = aLayer;
  theNbHigherLayer    = anotherLayer;
  InitTypeAndForm(406,6);
}


    Standard_Integer  IGESAppli_DrilledHole::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Standard_Real  IGESAppli_DrilledHole::DrillDiaSize () const
{
  return theDrillDiaSize;
}

    Standard_Real  IGESAppli_DrilledHole::FinishDiaSize () const
{
  return theFinishDiaSize;
}

    Standard_Boolean  IGESAppli_DrilledHole::IsPlating () const
{
  return (thePlatingFlag != 0);
}

    Standard_Integer  IGESAppli_DrilledHole::NbLowerLayer () const
{
  return theNbLowerLayer;
}

    Standard_Integer  IGESAppli_DrilledHole::NbHigherLayer () const
{
  return theNbHigherLayer;
}
