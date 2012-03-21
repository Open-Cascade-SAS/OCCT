// Created on: 2000-07-25
// Created by: Vincent DELOS
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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



#include <BooleanOperations_ShapeAndInterferences.ixx>

//===========================================================================
//function : ShapeAndInterferences
//purpose  : 
//===========================================================================
BooleanOperations_ShapeAndInterferences::BooleanOperations_ShapeAndInterferences():
    myState(BooleanOperations_UNKNOWN)
{
}
//modified by NIZNHY-PKV Wed Feb  2 12:55:46 2005f
/*
//===========================================================================
//function : SetInterference
//purpose  : 
//===========================================================================
  void BooleanOperations_ShapeAndInterferences::SetInterference
    (const BooleanOperations_InterferenceResult& Interf) 
{
  myInterferencesList.SetInterference(Interf);
}

//===========================================================================
//function : Dump
//purpose  : 
//===========================================================================
  void BooleanOperations_ShapeAndInterferences::Dump() const
{
  cout<<endl<<"myBoundingBox :"<<endl;
  myBoundingBox.Dump();
  myAncestorsAndSuccessors.Dump();

  myInterferencesList.Dump();
  cout<<endl<<"myState = "<<myState<<endl;
}
*/
//modified by NIZNHY-PKV Wed Feb  2 12:55:56 2005
