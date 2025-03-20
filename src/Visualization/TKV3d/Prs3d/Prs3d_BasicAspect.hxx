// Created on: 2000-02-15
// Created by: Gerard GRAS
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _Prs3d_BasicAspect_HeaderFile
#define _Prs3d_BasicAspect_HeaderFile

#include <Standard.hxx>
#include <Standard_OStream.hxx>
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>

//! All basic Prs3d_xxxAspect must inherits from this class
//! The aspect classes qualifies how to represent a given kind of object.
class Prs3d_BasicAspect : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Prs3d_BasicAspect, Standard_Transient)

  //! Dumps the content of me into the stream
  virtual void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const = 0;
};

DEFINE_STANDARD_HANDLE(Prs3d_BasicAspect, Standard_Transient)

#endif // _Prs3d_BasicAspect_HeaderFile
