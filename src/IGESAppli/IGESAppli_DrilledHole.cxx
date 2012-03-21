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
