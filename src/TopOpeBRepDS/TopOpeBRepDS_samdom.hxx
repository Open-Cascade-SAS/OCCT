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

Standard_EXPORT void             FDSSDM_prepare(const Handle(TopOpeBRepDS_HDataStructure)&);
Standard_EXPORT void             FDSSDM_makes1s2(const TopoDS_Shape&   S,
                                                 TopTools_ListOfShape& L1,
                                                 TopTools_ListOfShape& L2);
Standard_EXPORT void             FDSSDM_s1s2makesordor(const TopTools_ListOfShape& L1,
                                                       const TopTools_ListOfShape& L2,
                                                       const TopoDS_Shape&         S,
                                                       TopTools_ListOfShape&       LSO,
                                                       TopTools_ListOfShape&       LDO);
Standard_EXPORT void             FDSSDM_s1s2(const TopoDS_Shape&   S,
                                             TopTools_ListOfShape& LS1,
                                             TopTools_ListOfShape& LS2);
Standard_EXPORT void             FDSSDM_sordor(const TopoDS_Shape&   S,
                                               TopTools_ListOfShape& LSO,
                                               TopTools_ListOfShape& LDO);
Standard_EXPORT Standard_Boolean FDSSDM_contains(const TopoDS_Shape&         S,
                                                 const TopTools_ListOfShape& L);
Standard_EXPORT void             FDSSDM_copylist(const TopTools_ListOfShape& Lin,
                                                 const Standard_Integer      I1,
                                                 const Standard_Integer      I2,
                                                 TopTools_ListOfShape&       Lou);
Standard_EXPORT void FDSSDM_copylist(const TopTools_ListOfShape& Lin, TopTools_ListOfShape& Lou);

#endif
