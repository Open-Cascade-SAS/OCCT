//! Created on: 2016-06-03
//! Created by: NIKOLAI BUKHALOV
//! Copyright (c) 2016 OPEN CASCADE SAS
//!
//! This file is part of Open CASCADE Technology software library.
//!
//! This library is free software; you can redistribute it and/or modify it under
//! the terms of the GNU Lesser General Public License version 2.1 as published
//! by the Free Software Foundation, with special exception defined in the file
//! OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
//! distribution for complete text of the license and disclaimer of any warranty.
//!
//! Alternatively, this file may be used under the terms of Open CASCADE
//! commercial license or contractual agreement.

#ifndef _IntPatch_SpecPntType_HeaderFile
#define _IntPatch_SpecPntType_HeaderFile

//! This enum describe the  different kinds of
//! special (singular) points of Surface-Surface
//! intersection algorithm. Such as pole of sphere,
//! apex of cone, point on U- or V-seam etc.

enum IntPatch_SpecPntType
{
  IntPatch_SPntNone,
  IntPatch_SPntSeamU,
  IntPatch_SPntSeamV,
  IntPatch_SPntSeamUV,
  IntPatch_SPntPoleSeamU,
  IntPatch_SPntPole
};

#endif // _IntPatch_SpecPntType_HeaderFile