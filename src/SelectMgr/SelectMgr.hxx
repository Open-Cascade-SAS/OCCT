// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef _SelectMgr_HeaderFile
#define _SelectMgr_HeaderFile

#include <Standard_Type.hxx>

class Graphic3d_Structure;
class Graphic3d_TransformPers;
class SelectMgr_Selection;
class gp_Trsf;

//! Auxiliary tools for SelectMgr package.
class SelectMgr
{
public:
  //! Compute debug presentation for sensitive objects.
  Standard_EXPORT static void ComputeSensitivePrs(
    const Handle(Graphic3d_Structure)&     theStructure,
    const Handle(SelectMgr_Selection)&     theSel,
    const gp_Trsf&                         theLoc,
    const Handle(Graphic3d_TransformPers)& theTrsfPers);
};

#endif
