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
#include <Standard_Type.hxx>

#include <MMgt_TShared.hxx>


class Prs3d_BasicAspect;
DEFINE_STANDARD_HANDLE(Prs3d_BasicAspect, MMgt_TShared)

//! All basic Prs3d_xxxAspect must inherits from this class
//! The aspect classes qualifies how to represent
//! a given kind of object.
class Prs3d_BasicAspect : public MMgt_TShared
{

public:





  DEFINE_STANDARD_RTTI(Prs3d_BasicAspect,MMgt_TShared)

protected:




private:




};







#endif // _Prs3d_BasicAspect_HeaderFile
