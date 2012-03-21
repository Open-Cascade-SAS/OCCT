// Created on: 1995-12-21
// Created by: Jean Yves LEBEY
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

//modified by NIZNHY-PKV Tue Apr  3 15:55:58 2001

#include <BOP_FaceAreaBuilder.ixx>

//=======================================================================
//function : BOP_FaceAreaBuilder
//purpose  : 
//=======================================================================
BOP_FaceAreaBuilder::BOP_FaceAreaBuilder()
{
}

//=======================================================================
//function : BOP_FaceAreaBuilder
//purpose  : 
//=======================================================================
  BOP_FaceAreaBuilder::BOP_FaceAreaBuilder(BOP_LoopSet& LS, 
					   BOP_LoopClassifier& LC,
					   const Standard_Boolean ForceClass)
{
  InitFaceAreaBuilder(LS,LC,ForceClass);
}

//=======================================================================
//function : InitFaceAreaBuilder
//purpose  : 
//=======================================================================
  void BOP_FaceAreaBuilder::InitFaceAreaBuilder(BOP_LoopSet& LS, 
						BOP_LoopClassifier& LC,
						const Standard_Boolean ForceClass)
{
  InitAreaBuilder(LS,LC,ForceClass);
}
