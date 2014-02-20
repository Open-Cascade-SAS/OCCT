// Created on: 1994-06-28
// Created by: Laurent BOURESCHE
// Copyright (c) 1994-1999 Matra Datavision
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

#include <Adaptor3d_HSurface.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Circ.hxx>

#ifndef ChFiKPart_ComputeData_CS_HeaderFile
#define ChFiKPart_ComputeData_CS_HeaderFile


void ChFiKPart_CornerSpine(const Handle(Adaptor3d_HSurface)& S1, 
			   const Handle(Adaptor3d_HSurface)& S2,
			   const gp_Pnt2d& P1S1,
			   const gp_Pnt2d& P2S1,
			   const gp_Pnt2d& P1S2,
			   const gp_Pnt2d& P2S2,
			   const Standard_Real R,
			   gp_Cylinder& cyl,
			   gp_Circ& circ,
			   Standard_Real& First,
			   Standard_Real& Last);


#endif
