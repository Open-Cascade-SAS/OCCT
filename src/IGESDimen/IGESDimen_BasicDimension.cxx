// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//--------------------------------------------------------------------
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
