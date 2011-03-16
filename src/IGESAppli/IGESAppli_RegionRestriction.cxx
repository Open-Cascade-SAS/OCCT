//--------------------------------------------------------------------
//
//  File Name : IGESAppli_RegionRestriction.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESAppli_RegionRestriction.ixx>


    IGESAppli_RegionRestriction::IGESAppli_RegionRestriction ()    {  }


    void  IGESAppli_RegionRestriction::Init
  (const Standard_Integer nbPropVal,  const Standard_Integer aViasRest,
   const Standard_Integer aCompoRest, const Standard_Integer aCktRest)
{
  theNbPropertyValues  = nbPropVal;
  theElectViasRestrict = aViasRest;
  theElectCompRestrict = aCompoRest;
  theElectCktRestrict  = aCktRest;
  InitTypeAndForm(406,2);
}


    Standard_Integer  IGESAppli_RegionRestriction::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Standard_Integer IGESAppli_RegionRestriction::ElectricalViasRestriction () const
{
  return theElectViasRestrict;
}

    Standard_Integer IGESAppli_RegionRestriction::ElectricalComponentRestriction
  () const
{
  return theElectCompRestrict;
}

    Standard_Integer IGESAppli_RegionRestriction::ElectricalCktRestriction () const
{
  return theElectCktRestrict;
}
