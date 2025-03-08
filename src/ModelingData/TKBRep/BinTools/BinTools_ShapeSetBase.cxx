// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <BinTools.hxx>
#include <BinTools_ShapeSetBase.hxx>
#include <TopoDS_Shape.hxx>

const Standard_CString BinTools_ShapeSetBase::THE_ASCII_VERSIONS[BinTools_FormatVersion_UPPER + 1] =
  {"",
   "Open CASCADE Topology V1 (c)",
   "Open CASCADE Topology V2 (c)",
   "Open CASCADE Topology V3 (c)",
   "Open CASCADE Topology V4, (c) Open Cascade"};

//=======================================================================
// function : operator << (gp_Pnt)
// purpose  :
//=======================================================================
Standard_OStream& operator<<(Standard_OStream& OS, const gp_Pnt& P)
{
  BinTools::PutReal(OS, P.X());
  BinTools::PutReal(OS, P.Y());
  BinTools::PutReal(OS, P.Z());
  return OS;
}

//=================================================================================================

BinTools_ShapeSetBase::BinTools_ShapeSetBase()
    : myFormatNb(BinTools_FormatVersion_CURRENT),
      myWithTriangles(Standard_False),
      myWithNormals(Standard_False)
{
}

//=================================================================================================

BinTools_ShapeSetBase::~BinTools_ShapeSetBase() {}

//=================================================================================================

void BinTools_ShapeSetBase::SetFormatNb(const Standard_Integer theFormatNb)
{
  Standard_ASSERT_RETURN(theFormatNb >= BinTools_FormatVersion_LOWER
                           && theFormatNb <= BinTools_FormatVersion_UPPER,
                         "Error: unsupported BinTools version.", );

  myFormatNb = theFormatNb;
}
