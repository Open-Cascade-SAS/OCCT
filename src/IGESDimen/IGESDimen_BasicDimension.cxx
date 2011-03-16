//--------------------------------------------------------------------
//
//  File Name : IGESDimen_BasicDimension.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESDimen_BasicDimension.ixx>


    IGESDimen_BasicDimension::IGESDimen_BasicDimension ()    {  }

    void  IGESDimen_BasicDimension::Init
  (const Standard_Integer nbPropVal, 
   const gp_XY& thell, const gp_XY& thelr,
   const gp_XY& theur, const gp_XY& theul)
{
  theNbPropertyValues = nbPropVal;
  theLowerLeft  = thell;
  theLowerRight = thelr;
  theUpperRight = theur;
  theUpperLeft  = theul;
  InitTypeAndForm(406,31);
}


    Standard_Integer  IGESDimen_BasicDimension::NbPropertyValues () const 
{
  return theNbPropertyValues;
}

    gp_Pnt2d  IGESDimen_BasicDimension::LowerLeft () const 
{
  gp_Pnt2d g(theLowerLeft);
  return g;
}


    gp_Pnt2d  IGESDimen_BasicDimension::LowerRight () const 
{
  gp_Pnt2d g(theLowerRight);
  return g;
}

    gp_Pnt2d  IGESDimen_BasicDimension::UpperRight () const 
{
  gp_Pnt2d g(theUpperRight);
  return g;
}

    gp_Pnt2d  IGESDimen_BasicDimension::UpperLeft () const 
{
  gp_Pnt2d g(theUpperLeft);
  return g;
}
