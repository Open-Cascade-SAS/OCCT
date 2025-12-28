// Created on: 1995-05-30
// Created by: Stagiaire Flore Lantheaume
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _ChFiKPart_ComputeData_ChPlnCon_HeaderFile
#define _ChFiKPart_ComputeData_ChPlnCon_HeaderFile

bool ChFiKPart_MakeChamfer(TopOpeBRepDS_DataStructure&         DStr,
                           const occ::handle<ChFiDS_SurfData>& Data,
                           const ChFiDS_ChamfMode              theMode,
                           const gp_Pln&                       Pln,
                           const gp_Cone&                      Con,
                           const double                        fu,
                           const double                        lu,
                           const TopAbs_Orientation            Or1,
                           const TopAbs_Orientation            Or2,
                           const double                        Dis1,
                           const double                        Dis2,
                           const gp_Circ&                      Spine,
                           const double                        First,
                           const TopAbs_Orientation            Ofpl,
                           const bool                          plandab);

#endif
