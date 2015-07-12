// Created on: 1994-10-03
// Created by: Christian CAILLET
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

#include <TransferBRep_OrientedShapeMapper.hxx>

#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <TopTools_OrientedShapeMapHasher.hxx>
#include <TransferBRep_ShapeInfo.hxx>
#include <Transfer_Finder.hxx>

 








#define TheKey TopoDS_Shape
#define TheKey_hxx <TopoDS_Shape.hxx>
#define TheHasher TopTools_OrientedShapeMapHasher
#define TheHasher_hxx <TopTools_OrientedShapeMapHasher.hxx>
#define TheInfo TransferBRep_ShapeInfo
#define TheInfo_hxx <TransferBRep_ShapeInfo.hxx>
#define Transfer_Mapper TransferBRep_OrientedShapeMapper
#define Transfer_Mapper_hxx <TransferBRep_OrientedShapeMapper.hxx>
#define Handle_Transfer_Mapper Handle(TransferBRep_OrientedShapeMapper)
#include <Transfer_Mapper.gxx>

