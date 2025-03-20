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

#include <MeshVS_DataMapOfHArray1OfSequenceOfInteger.hxx>
#include <MeshVS_DataSource.hxx>
#include <MeshVS_HArray1OfSequenceOfInteger.hxx>
#include <Standard_Integer.hxx>

class MeshVS_DataSource3D;
DEFINE_STANDARD_HANDLE(MeshVS_DataSource3D, MeshVS_DataSource)

class MeshVS_DataSource3D : public MeshVS_DataSource
{

public:
  Standard_EXPORT Handle(MeshVS_HArray1OfSequenceOfInteger) GetPrismTopology(
    const Standard_Integer BasePoints) const;

  Standard_EXPORT Handle(MeshVS_HArray1OfSequenceOfInteger) GetPyramidTopology(
    const Standard_Integer BasePoints) const;

  Standard_EXPORT static Handle(MeshVS_HArray1OfSequenceOfInteger) CreatePrismTopology(
    const Standard_Integer BasePoints);

  Standard_EXPORT static Handle(MeshVS_HArray1OfSequenceOfInteger) CreatePyramidTopology(
    const Standard_Integer BasePoints);

  DEFINE_STANDARD_RTTIEXT(MeshVS_DataSource3D, MeshVS_DataSource)

protected:
private:
  MeshVS_DataMapOfHArray1OfSequenceOfInteger myPrismTopos;
  MeshVS_DataMapOfHArray1OfSequenceOfInteger myPyramidTopos;
};

#endif // _MeshVS_DataSource3D_HeaderFile
