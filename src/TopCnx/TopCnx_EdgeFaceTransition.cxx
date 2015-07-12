// Created on: 1992-08-12
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <gp_Dir.hxx>
#include <TopCnx_EdgeFaceTransition.hxx>

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
#ifdef OCCT_DEBUG
    else
      cout << "\n*** Complex Transition : unprocessed state"<<endl;
#endif
  }
  else if (Bef == TopAbs_OUT) {
    if      (Aft == TopAbs_IN ) 
      return TopAbs_FORWARD;
    else if (Aft == TopAbs_OUT) 
      return TopAbs_EXTERNAL;
#ifdef OCCT_DEBUG
    else 
      cout << "\n*** Complex Transition : unprocessed state"<<endl;
#endif
  }
#ifdef OCCT_DEBUG
  else 
    cout << "\n*** Complex Transition : unprocessed state"<<endl;
#endif
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


