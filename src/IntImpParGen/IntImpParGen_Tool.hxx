// Created on: 1992-06-10
// Created by: Laurent BUCHARD
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

#ifndef IntImpParGen_Tool_HeaderFile
#define IntImpParGen_Tool_HeaderFile

#include <IntRes2d_Domain.hxx>
#include <IntRes2d_Transition.hxx>
#include <gp_Pnt2d.hxx>
 
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
