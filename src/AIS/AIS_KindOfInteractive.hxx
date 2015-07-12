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
//! This is one of the following:
//! -   the Datum
//! -   the Object
//! -   the Relation
//! -   the Dimension
//! -   the None type.
//! The Datum is the construction element. These include
//! points, lines, axes and planes. The object brings
//! together topological shapes. The Relation includes
//! dimensions and constraints. The Dimension includes
//! length, radius, diameter and angle dimensions.
//! When the object is of an unknown type, the None
//! type is declared.
enum AIS_KindOfInteractive
{
AIS_KOI_None,
AIS_KOI_Datum,
AIS_KOI_Shape,
AIS_KOI_Object,
AIS_KOI_Relation,
AIS_KOI_Dimension
};

#endif // _AIS_KindOfInteractive_HeaderFile
