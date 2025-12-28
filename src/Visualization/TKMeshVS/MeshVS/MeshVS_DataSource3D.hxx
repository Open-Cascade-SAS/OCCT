// Created on: 2005-01-21
// Created by: Alexander SOLOVYOV
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _MeshVS_DataSource3D_HeaderFile
#define _MeshVS_DataSource3D_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_DataMap.hxx>
#include <MeshVS_DataSource.hxx>

class MeshVS_DataSource3D : public MeshVS_DataSource
{

public:
  Standard_EXPORT occ::handle<NCollection_HArray1<NCollection_Sequence<int>>> GetPrismTopology(
    const int BasePoints) const;

  Standard_EXPORT occ::handle<NCollection_HArray1<NCollection_Sequence<int>>> GetPyramidTopology(
    const int BasePoints) const;

  Standard_EXPORT static occ::handle<NCollection_HArray1<NCollection_Sequence<int>>>
    CreatePrismTopology(const int BasePoints);

  Standard_EXPORT static occ::handle<NCollection_HArray1<NCollection_Sequence<int>>>
    CreatePyramidTopology(const int BasePoints);

  DEFINE_STANDARD_RTTIEXT(MeshVS_DataSource3D, MeshVS_DataSource)

private:
  NCollection_DataMap<int, occ::handle<NCollection_HArray1<NCollection_Sequence<int>>>>
    myPrismTopos;
  NCollection_DataMap<int, occ::handle<NCollection_HArray1<NCollection_Sequence<int>>>>
    myPyramidTopos;
};

#endif // _MeshVS_DataSource3D_HeaderFile
