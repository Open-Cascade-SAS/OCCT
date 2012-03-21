// Created on: 1992-08-12
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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



#include <TopCnx_EdgeFaceTransition.ixx>


//=======================================================================
//function : TopCnx_EdgeFaceTransition
//purpose  : 
//=======================================================================

TopCnx_EdgeFaceTransition::TopCnx_EdgeFaceTransition() :
       nbBoundForward(0),
       nbBoundReversed(0)
{
}

//=======================================================================
//function : Reset
//purpose  : 
//=======================================================================

void  TopCnx_EdgeFaceTransition::Reset(const gp_Dir& Tgt,
				       const gp_Dir& Norm,
				       const Standard_Real Curv)
{
  myCurveTransition.Reset(Tgt,Norm,Curv);
  nbBoundForward = nbBoundReversed = 0;
}

//=======================================================================
//function : Reset
//purpose  : 
//=======================================================================

void  TopCnx_EdgeFaceTransition::Reset(const gp_Dir& Tgt)
{
  myCurveTransition.Reset(Tgt);
  nbBoundForward = nbBoundReversed = 0;
}

//=======================================================================
//function : AddInterference
//purpose  : 
//=======================================================================

void  TopCnx_EdgeFaceTransition::AddInterference(const Standard_Real Tole,
						 const gp_Dir& Tang,
						 const gp_Dir& Norm,
						 const Standard_Real Curv, 
						 const TopAbs_Orientation Or,
						 const TopAbs_Orientation Tr,
						 const TopAbs_Orientation BTr)
{
  myCurveTransition.Compare(Tole,Tang,Norm,Curv,Tr,Or);
  switch (BTr) {
    
  case TopAbs_FORWARD :
    nbBoundForward++;
    break;

  case TopAbs_REVERSED :
    nbBoundReversed++;
    break;

  case TopAbs_INTERNAL :
  case TopAbs_EXTERNAL :
    break;
  }
}

//=======================================================================
//function : Transition
//purpose  : 
//=======================================================================

TopAbs_Orientation  TopCnx_EdgeFaceTransition::Transition()const 
{
  TopAbs_State Bef = myCurveTransition.StateBefore();
  TopAbs_State Aft = myCurveTransition.StateAfter();
  if (Bef == TopAbs_IN) {
    if      (Aft == TopAbs_IN ) 
      return TopAbs_INTERNAL;
    else if (Aft == TopAbs_OUT) 
      return TopAbs_REVERSED;
    else
      cout << "\n*** Complex Transition : unprocessed state"<<endl;
  }
  else if (Bef == TopAbs_OUT) {
    if      (Aft == TopAbs_IN ) 
      return TopAbs_FORWARD;
    else if (Aft == TopAbs_OUT) 
      return TopAbs_EXTERNAL;
    else 
      cout << "\n*** Complex Transition : unprocessed state"<<endl;
  }
  else 
    cout << "\n*** Complex Transition : unprocessed state"<<endl;
  return TopAbs_INTERNAL;
}

//=======================================================================
//function : BoundaryTransition
//purpose  : 
//=======================================================================

TopAbs_Orientation  TopCnx_EdgeFaceTransition::BoundaryTransition()const 
{
  if (nbBoundForward > nbBoundReversed) 
    return TopAbs_FORWARD;
  else if (nbBoundForward < nbBoundReversed) 
    return TopAbs_REVERSED;
  else if ((nbBoundReversed % 2) == 0)
    return TopAbs_EXTERNAL;
  else
    return TopAbs_EXTERNAL;
}


