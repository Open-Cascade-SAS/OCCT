// Created on: 1996-07-24
// Created by: Herve LOUESSARD
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

#ifndef _LocalAnalysis_HeaderFile
#define _LocalAnalysis_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Standard_OStream.hxx>

class LocalAnalysis_SurfaceContinuity;
class LocalAnalysis_CurveContinuity;

//! This package gives tools to check the local continuity
//! between two  points situated  on two curves or two surfaces.
class LocalAnalysis 
{
public:

  DEFINE_STANDARD_ALLOC

  //! This  class  compute s and gives tools to check the local
  //! continuity between two points situated on 2 curves.
  //!
  //! This function gives information about a variable CurveContinuity
  Standard_EXPORT static void Dump (const LocalAnalysis_SurfaceContinuity& surfconti, Standard_OStream& o);
  

  //! This function gives information about a variable SurfaceContinuity
  Standard_EXPORT static void Dump (const LocalAnalysis_CurveContinuity& curvconti, Standard_OStream& o);

};

#endif // _LocalAnalysis_HeaderFile
