// Created on: 2013-01-29
// Created by: Kirill GAVRILOV
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

#ifndef _OpenGl_Vec_H__
#define _OpenGl_Vec_H__

#include <NCollection_Vec4.hxx>

typedef NCollection_Vec2<int> OpenGl_Vec2i;
typedef NCollection_Vec3<int> OpenGl_Vec3i;
typedef NCollection_Vec4<int> OpenGl_Vec4i;

typedef NCollection_Vec2<char> OpenGl_Vec2b;
typedef NCollection_Vec3<char> OpenGl_Vec3b;
typedef NCollection_Vec4<char> OpenGl_Vec4b;

typedef NCollection_Vec2<unsigned int> OpenGl_Vec2u;
typedef NCollection_Vec3<unsigned int> OpenGl_Vec3u;
typedef NCollection_Vec4<unsigned int> OpenGl_Vec4u;

typedef NCollection_Vec2<unsigned char> OpenGl_Vec2ub;
typedef NCollection_Vec3<unsigned char> OpenGl_Vec3ub;
typedef NCollection_Vec4<unsigned char> OpenGl_Vec4ub;

typedef NCollection_Vec2<float> OpenGl_Vec2;
typedef NCollection_Vec3<float> OpenGl_Vec3;
typedef NCollection_Vec4<float> OpenGl_Vec4;

typedef NCollection_Vec2<double> OpenGl_Vec2d;
typedef NCollection_Vec3<double> OpenGl_Vec3d;
typedef NCollection_Vec4<double> OpenGl_Vec4d;

#endif // _OpenGl_Vec_H__
