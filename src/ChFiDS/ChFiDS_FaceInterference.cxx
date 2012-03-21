// Created on: 1993-11-18
// Created by: Isabelle GRIGNON
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
