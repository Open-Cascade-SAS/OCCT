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

#ifndef ChFiKPart_ComputeData_Rotule_HeaderFile
#define ChFiKPart_ComputeData_Rotule_HeaderFile

Standard_Boolean ChFiKPart_MakeRotule(TopOpeBRepDS_DataStructure& DStr,
				      const Handle(ChFiDS_SurfData)& Data, 
				      const gp_Pln& pl, 
				      const gp_Pln& pl1, 
				      const gp_Pln& pl2, 
				      const TopAbs_Orientation opl,
				      const TopAbs_Orientation opl1,
				      const TopAbs_Orientation opl2,
				      const Standard_Real r, 
				      const TopAbs_Orientation ofpl);


#endif
