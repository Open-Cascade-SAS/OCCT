// Created on: 1995-04-05
// Created by: Christophe MARION
// Copyright (c) 1995-1999 Matra Datavision
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


#include <Standard_Stream.hxx>
#include <HLRTest_Projector.ixx>

//=======================================================================
//function : HLRTest_Projector
//purpose  : 
//=======================================================================

HLRTest_Projector::HLRTest_Projector (const HLRAlgo_Projector& P) :
myProjector(P)
{
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void HLRTest_Projector::DrawOn (Draw_Display&) const 
{
}

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Draw_Drawable3D) HLRTest_Projector::Copy () const 
{
  return new HLRTest_Projector(myProjector);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void HLRTest_Projector::Dump (Standard_OStream& S) const 
{
  S << "Projector : \n";
  if (myProjector.Perspective())
    S << "perspective, focal = " << myProjector.Focus() << "\n";

  for (Standard_Integer i = 1; i <= 3; i++) {

    for (Standard_Integer j = 1; j <= 4; j++) {
      S << setw(15) << myProjector.Transformation().Value(i,j);
    }
    S << "\n";
  }
  S << endl;
}

//=======================================================================
//function : Whatis
//purpose  : 
//=======================================================================

void HLRTest_Projector::Whatis (Draw_Interpretor& I) const 
{
  I << "projector";
}

