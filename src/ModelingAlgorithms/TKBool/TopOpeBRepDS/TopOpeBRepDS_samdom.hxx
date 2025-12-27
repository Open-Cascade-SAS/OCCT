// Created on: 1998-06-15
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRepDS_samdom_HeaderFile
#define _TopOpeBRepDS_samdom_HeaderFile

#include <TopOpeBRepDS_define.hxx>

Standard_EXPORT void             FDSSDM_prepare(const occ::handle<TopOpeBRepDS_HDataStructure>&);
Standard_EXPORT void             FDSSDM_makes1s2(const TopoDS_Shape&   S,
                                                 NCollection_List<TopoDS_Shape>& L1,
                                                 NCollection_List<TopoDS_Shape>& L2);
Standard_EXPORT void             FDSSDM_s1s2makesordor(const NCollection_List<TopoDS_Shape>& L1,
                                                       const NCollection_List<TopoDS_Shape>& L2,
                                                       const TopoDS_Shape&         S,
                                                       NCollection_List<TopoDS_Shape>&       LSO,
                                                       NCollection_List<TopoDS_Shape>&       LDO);
Standard_EXPORT void             FDSSDM_s1s2(const TopoDS_Shape&   S,
                                             NCollection_List<TopoDS_Shape>& LS1,
                                             NCollection_List<TopoDS_Shape>& LS2);
Standard_EXPORT void             FDSSDM_sordor(const TopoDS_Shape&   S,
                                               NCollection_List<TopoDS_Shape>& LSO,
                                               NCollection_List<TopoDS_Shape>& LDO);
Standard_EXPORT bool FDSSDM_contains(const TopoDS_Shape&         S,
                                                 const NCollection_List<TopoDS_Shape>& L);
Standard_EXPORT void             FDSSDM_copylist(const NCollection_List<TopoDS_Shape>& Lin,
                                                 const int      I1,
                                                 const int      I2,
                                                 NCollection_List<TopoDS_Shape>&       Lou);
Standard_EXPORT void FDSSDM_copylist(const NCollection_List<TopoDS_Shape>& Lin, NCollection_List<TopoDS_Shape>& Lou);

#endif
