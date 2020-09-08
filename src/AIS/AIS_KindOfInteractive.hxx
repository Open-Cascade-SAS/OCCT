// Created on: 1996-12-11
// Created by: Robert COUBLANC
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _AIS_KindOfInteractive_HeaderFile
#define _AIS_KindOfInteractive_HeaderFile

//! Declares the type of Interactive Object.
//! This type can be used for fast pre-filtering of objects of specific group.
enum AIS_KindOfInteractive
{
  AIS_KindOfInteractive_None,        //!< object of unknown type
  AIS_KindOfInteractive_Datum,       //!< presentation of construction element (datum)
                                     //!  such as points, lines, axes and planes
  AIS_KindOfInteractive_Shape,       //!< presentation of topological shape
  AIS_KindOfInteractive_Object,      //!< presentation of group of topological shapes
  AIS_KindOfInteractive_Relation,    //!< presentation of relation  (dimensions and constraints)
  AIS_KindOfInteractive_Dimension,   //!< presentation of dimension (length, radius, diameter and angle)
  AIS_KindOfInteractive_LightSource, //!< presentation of light source

  // old aliases
  AIS_KOI_None = AIS_KindOfInteractive_None,
  AIS_KOI_Datum = AIS_KindOfInteractive_Datum,
  AIS_KOI_Shape = AIS_KindOfInteractive_Shape,
  AIS_KOI_Object = AIS_KindOfInteractive_Object,
  AIS_KOI_Relation = AIS_KindOfInteractive_Relation,
  AIS_KOI_Dimension = AIS_KindOfInteractive_Dimension
};

#endif // _AIS_KindOfInteractive_HeaderFile
