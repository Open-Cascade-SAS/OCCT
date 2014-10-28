// Created on: 1998-02-14
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

#ifndef OCCT_DEBUG_HeaderFile
#define _TopOpeBRepDS_reDEB_HeaderFile

#ifdef OCCT_DEBUG
#include <TopOpeBRepDS_define.hxx>
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettracePEI();
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettracePI();
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceSPSX(const Standard_Integer);
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceEDPR();
Standard_EXPORT Standard_Boolean DSREDUEDGETRCE(const Standard_Integer);
Standard_EXPORT void debreducerE(const Standard_Integer);
Standard_EXPORT void debreducerEP(const Standard_Integer);
Standard_EXPORT void debreducerEV(const Standard_Integer,const Standard_Integer);
Standard_EXPORT void debreducer3d(const Standard_Integer);
Standard_EXPORT void debredpvg(const Standard_Integer);
#endif

#endif
