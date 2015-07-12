// Created on: 1993-11-18
// Created by: Isabelle GRIGNON
// Copyright (c) 1993-1999 Matra Datavision
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


#include <ChFiDS_FaceInterference.hxx>
#include <Geom2d_Curve.hxx>

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
