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

#ifndef OPENGL_TRANSFORM_PERSISTENCE_H
#define OPENGL_TRANSFORM_PERSISTENCE_H

enum TP_FLAGS
{
  TPF_PAN             = 0x001,
  TPF_ZOOM            = 0x002,
  TPF_ROTATE          = 0x008,
  TPF_TRIEDRON        = 0x020,
  TPF_2D              = 0x040,
  TPF_2D_ISTOPDOWN    = 0x041
};

#endif /*OPENGL_TRANSFORM_PERSISTENCE_H*/
