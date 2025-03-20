// Created on: 1993-04-13
// Created by: JCV
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

#ifndef _gp_TrsfForm_HeaderFile
#define _gp_TrsfForm_HeaderFile

//! Identifies the type of a geometric transformation.
enum gp_TrsfForm
{
  gp_Identity,     //!< No transformation (matrix is identity)
  gp_Rotation,     //!< Rotation
  gp_Translation,  //!< Translation
  gp_PntMirror,    //!< Central symmetry
  gp_Ax1Mirror,    //!< Rotational symmetry
  gp_Ax2Mirror,    //!< Bilateral symmetry
  gp_Scale,        //!< Scale
  gp_CompoundTrsf, //!< Combination of the above transformations
  gp_Other         //!< Transformation with not-orthogonal matrix
};

#endif // _gp_TrsfForm_HeaderFile
