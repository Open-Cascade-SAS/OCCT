// Created on: 1992-02-03
// Created by: Christian CAILLET
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

#include <Transfer_TransientMapper.hxx>

#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Transfer_DataInfo.hxx>
#include <Transfer_Finder.hxx>

 








#define TheKey Handle(Standard_Transient)
#define TheKey_hxx <Standard_Transient.hxx>
#define TheHasher TColStd_MapTransientHasher
#define TheHasher_hxx <TColStd_MapTransientHasher.hxx>
#define TheInfo Transfer_DataInfo
#define TheInfo_hxx <Transfer_DataInfo.hxx>
#define Transfer_Mapper Transfer_TransientMapper
#define Transfer_Mapper_hxx <Transfer_TransientMapper.hxx>
#define Handle_Transfer_Mapper Handle(Transfer_TransientMapper)
#include <Transfer_Mapper.gxx>

