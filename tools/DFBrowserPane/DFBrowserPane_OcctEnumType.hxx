// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef DFBrowserPane_OcctEnumType_H
#define DFBrowserPane_OcctEnumType_H

//! Information for OCCT enumeration type
enum DFBrowserPane_OcctEnumType
{
  DB_CONSTRAINT_TYPE = 0, //!< TDataXtd_ConstraintEnum values
  DB_NAMING_TYPE = 1,     //!< TNaming_NameType values
  DB_SHAPE_TYPE = 2,      //!< TopAbs_ShapeEnum values
  DB_NS_TYPE = 3,         //!< TNaming_Evolution values
  DB_GEOM_TYPE = 4,       //!< TDataXtd_GeometryEnum values
  DB_DIMENSION_TYPE = 5,  //!< TDataStd_RealEnum values
  DB_MATERIAL_TYPE = 6,   //!< Graphic3d_NameOfMaterial values
  DB_DISPLAY_MODE = 7,    //!< AIS_DisplayMode values
  DB_ORIENTATION_TYPE = 8, //!< TopAbs_Orientation values
  DB_CDM_CAN_CLOSE_STATUS //!< CDM_CanCloseStatus values
};

#endif
