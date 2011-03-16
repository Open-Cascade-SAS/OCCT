//--------------------------------------------------------------------
//
//  File Name : IGESAppli_PWBDrilledHole.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESAppli_PWBDrilledHole.ixx>

    IGESAppli_PWBDrilledHole::IGESAppli_PWBDrilledHole ()    {  }


    void  IGESAppli_PWBDrilledHole::Init
  (const Standard_Integer nbPropVal,
   const Standard_Real aDrillDia, const Standard_Real aFinishDia,
   const Standard_Integer aCode)
{
  theNbPropertyValues = nbPropVal;
  theDrillDiameter    = aDrillDia;
  theFinishDiameter   = aFinishDia;
  theFunctionCode     = aCode;
  InitTypeAndForm(406,26);
}


    Standard_Integer  IGESAppli_PWBDrilledHole::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Standard_Real  IGESAppli_PWBDrilledHole::DrillDiameterSize () const
{
  return theDrillDiameter;
}

    Standard_Real  IGESAppli_PWBDrilledHole::FinishDiameterSize () const
{
  return theFinishDiameter;
}

    Standard_Integer  IGESAppli_PWBDrilledHole::FunctionCode () const
{
  return theFunctionCode;
}
