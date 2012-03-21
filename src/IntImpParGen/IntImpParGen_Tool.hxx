// Created on: 1992-06-10
// Created by: Laurent BUCHARD
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



#ifndef IntImpParGen_Tool_HeaderFile
#define IntImpParGen_Tool_HeaderFile

#ifndef IntRes2d_Domain_HeaderFile
#include <IntRes2d_Domain.hxx>
#endif

#ifndef IntRes2d_Position_HeaderFile
#include <IntRes2d_Position.hxx>
#endif

#ifndef IntRes2d_Transition_HeaderFile
#include <IntRes2d_Transition.hxx>
#endif

#ifndef gp_Vec2d_HeaderFile
#include <gp_Vec2d.hxx>
#endif

#ifndef gp_Pnt2d_HeaderFile
#include <gp_Pnt2d.hxx>
#endif

 
Standard_Real NormalizeOnDomain(Standard_Real&,const IntRes2d_Domain&);

void Determine_Position(IntRes2d_Position&,
			const IntRes2d_Domain&,
			const gp_Pnt2d&,const Standard_Real);
  
void Determine_Transition(const IntRes2d_Position Pos1,
			  gp_Vec2d&               Tan1,
			  const gp_Vec2d&         Norm1,
			  IntRes2d_Transition&    Trans1,
			  const IntRes2d_Position Pos2,
			  gp_Vec2d&               Tan2,
			  const gp_Vec2d&         Norm2,
			  IntRes2d_Transition&    Trans2,
			  const Standard_Real     ToleranceAng);

#endif
