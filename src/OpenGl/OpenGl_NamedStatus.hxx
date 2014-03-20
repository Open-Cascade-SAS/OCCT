// Created on: 2011-09-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef _OpenGl_NamedStatus_Header
#define _OpenGl_NamedStatus_Header

// Dynamic fields
#define OPENGL_NS_HIDE         (1<<0)
#define OPENGL_NS_HIGHLIGHT    (1<<1)
#define OPENGL_NS_RESMAT       (1<<2)
#define OPENGL_NS_IMMEDIATE    (1<<3)
#define OPENGL_NS_TEXTURE      (1<<4)
#define OPENGL_NS_ANTIALIASING (1<<5)
#define OPENGL_NS_2NDPASSNEED  (1<<6)
#define OPENGL_NS_2NDPASSDO    (1<<7)
#define OPENGL_NS_FORBIDSETTEX (1<<8)
#define OPENGL_NS_WHITEBACK    (1<<9)

#endif //_OpenGl_NamedStatus_Header
