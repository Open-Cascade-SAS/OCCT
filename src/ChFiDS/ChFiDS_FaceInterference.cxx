// File:	ChFiDS_FaceInterference.cxx
// Created:	Thu Nov 18 12:07:12 1993
// Author:	Isabelle GRIGNON
//		<isg@zerox>


#include <ChFiDS_FaceInterference.ixx>

//=======================================================================
//function : ChFiDS_FaceInterference
//purpose  : 
//=======================================================================

ChFiDS_FaceInterference::ChFiDS_FaceInterference()
{
}

void ChFiDS_FaceInterference::SetParameter(const Standard_Real U1,
					    const Standard_Boolean IsFirst)
{
  if(IsFirst) SetFirstParameter(U1);
  else SetLastParameter(U1);
}

void ChFiDS_FaceInterference::SetTransition(const TopAbs_Orientation Trans)
{
  LineTransition = Trans; 
}

Standard_Real ChFiDS_FaceInterference::Parameter
(const Standard_Boolean IsFirst)const
{
  if(IsFirst) return FirstParameter();
  else return LastParameter();
}
