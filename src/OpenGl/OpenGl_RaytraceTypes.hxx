// Created on: 2013-10-15
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013 OPEN CASCADE SAS
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

#ifndef _OpenGl_RaytraceTypes_Header
#define _OpenGl_RaytraceTypes_Header

#include <vector>

#include <NCollection_Vec4.hxx>
#include <NCollection_StdAllocator.hxx>

//! 4D vector of integers.
typedef NCollection_Vec4<int> OpenGl_RTVec4i;

//! 4D vector of floats.
typedef NCollection_Vec4<float> OpenGl_RTVec4f;

//! 4D vector of doubles.
typedef NCollection_Vec4<double> OpenGl_RTVec4d;

//! Array of 4D integer vectors.
typedef std::vector<OpenGl_RTVec4i,
                    NCollection_StdAllocator<OpenGl_RTVec4i> > OpenGl_RTArray4i;

//! Array of 4D floating point vectors.
typedef std::vector<OpenGl_RTVec4f,
                    NCollection_StdAllocator<OpenGl_RTVec4f> > OpenGl_RTArray4f;

#endif
