// Created on: 1998-06-16
// Created by: Philippe NOUAILLE
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _ChFiKPart_ComputeData_ChAsymPlnPln_HeaderFile
#define _ChFiKPart_ComputeData_ChAsymPlnPln_HeaderFile

Standard_Boolean ChFiKPart_MakeChAsym(TopOpeBRepDS_DataStructure&    DStr,
                                      const Handle(ChFiDS_SurfData)& Data,
                                      const gp_Pln&                  Pl1,
                                      const gp_Pln&                  Pl2,
                                      const TopAbs_Orientation       Or1,
                                      const TopAbs_Orientation       Or2,
                                      const Standard_Real            Dis,
                                      const Standard_Real            Angle,
                                      const gp_Lin&                  Spine,
                                      const Standard_Real            First,
                                      const TopAbs_Orientation       Of1,
                                      const Standard_Boolean         DisOnP1);

#endif
