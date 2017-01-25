// Created on: 2005-04-18
// Created by: Eugeny NAPALKOV <eugeny.napalkov@opencascade.com>
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

#ifndef _BinMXCAFDoc_HeaderFile
#define _BinMXCAFDoc_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class BinMDF_ADriverTable;
class CDM_MessageDriver;
class BinMXCAFDoc_AreaDriver;
class BinMXCAFDoc_CentroidDriver;
class BinMXCAFDoc_ClippingPlaneToolDriver;
class BinMXCAFDoc_ColorDriver;
class BinMXCAFDoc_GraphNodeDriver;
class BinMXCAFDoc_LocationDriver;
class BinMXCAFDoc_VolumeDriver;
class BinMXCAFDoc_DatumDriver;
class BinMXCAFDoc_GeomToleranceDriver;
class BinMXCAFDoc_DimensionDriver;
class BinMXCAFDoc_DimTolDriver;
class BinMXCAFDoc_MaterialDriver;
class BinMXCAFDoc_ColorToolDriver;
class BinMXCAFDoc_DocumentToolDriver;
class BinMXCAFDoc_LayerToolDriver;
class BinMXCAFDoc_ShapeToolDriver;
class BinMXCAFDoc_DimTolToolDriver;
class BinMXCAFDoc_MaterialToolDriver;
class BinMXCAFDoc_ViewDriver;
class BinMXCAFDoc_ViewToolDriver;



class BinMXCAFDoc 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Adds the attribute drivers to <theDriverTable>.
  Standard_EXPORT static void AddDrivers (const Handle(BinMDF_ADriverTable)& theDriverTable, const Handle(CDM_MessageDriver)& theMsgDrv);




protected:





private:




friend class BinMXCAFDoc_AreaDriver;
friend class BinMXCAFDoc_CentroidDriver;
friend class BinMXCAFDoc_ClippingPlaneToolDriver;
friend class BinMXCAFDoc_ColorDriver;
friend class BinMXCAFDoc_GraphNodeDriver;
friend class BinMXCAFDoc_LocationDriver;
friend class BinMXCAFDoc_VolumeDriver;
friend class BinMXCAFDoc_DatumDriver;
friend class BinMXCAFDoc_GeomToleranceDriver;
friend class BinMXCAFDoc_DimensionDriver;
friend class BinMXCAFDoc_DimTolDriver;
friend class BinMXCAFDoc_MaterialDriver;
friend class BinMXCAFDoc_ColorToolDriver;
friend class BinMXCAFDoc_DocumentToolDriver;
friend class BinMXCAFDoc_LayerToolDriver;
friend class BinMXCAFDoc_ShapeToolDriver;
friend class BinMXCAFDoc_DimTolToolDriver;
friend class BinMXCAFDoc_MaterialToolDriver;
friend class BinMXCAFDoc_ViewDriver;
friend class BinMXCAFDoc_ViewToolDriver;

};







#endif // _BinMXCAFDoc_HeaderFile
