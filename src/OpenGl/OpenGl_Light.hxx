// Created on: 2011-07-13
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
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

#ifndef OpenGl_Light_Header
#define OpenGl_Light_Header

#include <Graphic3d_CLight.hxx>
#include <Visual3d_TypeOfLightSource.hxx>
#include <NCollection_List.hxx>

#define OpenGLMaxLights 8

typedef Graphic3d_CLight               OpenGl_Light;
typedef NCollection_List<OpenGl_Light> OpenGl_ListOfLight;

#endif // OpenGl_Light_Header
