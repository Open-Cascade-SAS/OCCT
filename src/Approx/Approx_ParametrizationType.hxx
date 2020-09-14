// Created on: 1993-01-26
// Created by: Laurent PAINNOT
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Approx_ParametrizationType_HeaderFile
#define _Approx_ParametrizationType_HeaderFile


enum Approx_ParametrizationType
{
  Approx_ChordLength, //!< parameters of points are proportionate to distances between them
  Approx_Centripetal, //!< parameters of points are proportionate to square roots of distances between them
  Approx_IsoParametric //!< parameters of points are distributed uniformly
};

#endif // _Approx_ParametrizationType_HeaderFile
