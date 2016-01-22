// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _SelectMgr_VectorTypes_HeaderFile
#define _SelectMgr_VectorTypes_HeaderFile

#include <gp_Trsf.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Mat4.hxx>
#include <NCollection_Vec3.hxx>
#include <NCollection_Vec4.hxx>

typedef NCollection_Vec3<Standard_Real>    SelectMgr_Vec3;
typedef NCollection_Vec4<Standard_Real>    SelectMgr_Vec4;
typedef NCollection_Mat4<Standard_Real>    SelectMgr_Mat4;

namespace SelectMgr_MatOp
{
  inline SelectMgr_Vec3 Transform (const gp_Trsf& theTrsf,
                            const SelectMgr_Vec3& theVec)
  {
    SelectMgr_Vec3 aRes (0.0);
    for (Standard_Integer aRow = 1; aRow <= 3; ++aRow)
    {
      for (Standard_Integer aCol = 1; aCol <= 3; ++aCol)
      {
        aRes[aRow - 1] += theVec[aCol - 1] * theTrsf.Value (aRow, aCol);
      }
      aRes[aRow - 1] += theTrsf.Value (aRow, 4);
    }

    return aRes;
  }
}

#endif // _SelectMgr_VectorTypes_HeaderFile
