// Created on: 2013-10-15
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
