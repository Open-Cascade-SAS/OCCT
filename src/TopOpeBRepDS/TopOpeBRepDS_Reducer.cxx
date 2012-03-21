// Copyright (c) 1997-1999 Matra Datavision
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

#include <TopOpeBRepDS_Reducer.ixx>
#include <TopOpeBRepDS_EIR.hxx>
#include <TopOpeBRepDS_FIR.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>

//=======================================================================
//function : TopOpeBRepDS_Reducer
//purpose  : 
//=======================================================================
TopOpeBRepDS_Reducer::TopOpeBRepDS_Reducer
(const Handle(TopOpeBRepDS_HDataStructure)& HDS) : myHDS(HDS)
{}

//=======================================================================
//function : ProcessEdgeInterferences
//purpose  : 
//=======================================================================
void TopOpeBRepDS_Reducer::ProcessEdgeInterferences()
{
  TopOpeBRepDS_EIR eir(myHDS);
  eir.ProcessEdgeInterferences();
}

//=======================================================================
//function : ProcessFaceInterferences
//purpose  : 
//=======================================================================
void TopOpeBRepDS_Reducer::ProcessFaceInterferences
(const TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State& M)
{
  //modified by NIZHNY-MZV  Tue Nov 16 16:12:15 1999
  //FUN_ds_FEIGb1TO0(myHDS,M); //xpu250199

  TopOpeBRepDS_FIR fir(myHDS);
  fir.ProcessFaceInterferences(M);
}
