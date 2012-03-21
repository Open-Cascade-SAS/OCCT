// Created on: 2011-09-20
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


#ifndef _OpenGl_NamedStatus_Header
#define _OpenGl_NamedStatus_Header

// Dynamic fields
#define OPENGL_NS_HIDE         (1<<0)
#define OPENGL_NS_HIGHLIGHT    (1<<1)
#define OPENGL_NS_WIREFRAME    (1<<2)
#define OPENGL_NS_RESMAT       (1<<3)
#define OPENGL_NS_ADD          (1<<4)
#define OPENGL_NS_IMMEDIATE    (1<<5)
#define OPENGL_NS_TEXTURE      (1<<6)
#define OPENGL_NS_ANTIALIASING (1<<7)
#define OPENGL_NS_ANIMATION    (1<<8)
#define OPENGL_NS_UPDATEAM     (1<<9)
#define OPENGL_NS_DEGENERATION (1<<10)
#define OPENGL_NS_2NDPASSNEED  (1<<11)
#define OPENGL_NS_2NDPASSDO    (1<<12)
#define OPENGL_NS_FORBIDSETTEX (1<<13)
#define OPENGL_NS_FLIST        (1<<14)
#define OPENGL_NS_WHITEBACK    (1<<15)

#endif //_OpenGl_NamedStatus_Header
