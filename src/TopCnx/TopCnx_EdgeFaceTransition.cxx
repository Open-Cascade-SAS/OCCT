// File:	TopCnx_EdgeFaceTransition.cxx
// Created:	Wed Aug 12 17:08:45 1992
// Author:	Remi LEQUETTE
//		<rle@phylox>


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


