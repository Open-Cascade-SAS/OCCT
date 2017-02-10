// Created on: 2001-09-11
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _XmlMXCAFDoc_HeaderFile
#define _XmlMXCAFDoc_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class XmlMDF_ADriverTable;
class CDM_MessageDriver;
class XmlMXCAFDoc_AreaDriver;
class XmlMXCAFDoc_CentroidDriver;
class XmlMXCAFDoc_ColorDriver;
class XmlMXCAFDoc_GraphNodeDriver;
class XmlMXCAFDoc_LocationDriver;
class XmlMXCAFDoc_VolumeDriver;
class XmlMXCAFDoc_DatumDriver;
class XmlMXCAFDoc_DimTolDriver;
class XmlMXCAFDoc_MaterialDriver;
class XmlMXCAFDoc_NotesToolDriver;
class XmlMXCAFDoc_NoteDriver;
class XmlMXCAFDoc_NoteCommentDriver;
class XmlMXCAFDoc_NoteBinDataDriver;
class XmlMXCAFDoc_ClippingPlaneToolDriver;
class XmlMXCAFDoc_ColorToolDriver;
class XmlMXCAFDoc_DocumentToolDriver;
class XmlMXCAFDoc_LayerToolDriver;
class XmlMXCAFDoc_ShapeToolDriver;
class XmlMXCAFDoc_DimTolToolDriver;
class XmlMXCAFDoc_MaterialToolDriver;
class XmlMXCAFDoc_ViewToolDriver;


//! Storage and Retrieval drivers for modelling attributes.
//! Transient attributes are defined in package XCAFDoc
class XmlMXCAFDoc 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Adds the attribute drivers to <aDriverTable>.
  Standard_EXPORT static void AddDrivers (const Handle(XmlMDF_ADriverTable)& aDriverTable, const Handle(CDM_MessageDriver)& anMsgDrv);




protected:





private:




friend class XmlMXCAFDoc_AreaDriver;
friend class XmlMXCAFDoc_CentroidDriver;
friend class XmlMXCAFDoc_ColorDriver;
friend class XmlMXCAFDoc_GraphNodeDriver;
friend class XmlMXCAFDoc_LocationDriver;
friend class XmlMXCAFDoc_VolumeDriver;
friend class XmlMXCAFDoc_DatumDriver;
friend class XmlMXCAFDoc_DimTolDriver;
friend class XmlMXCAFDoc_MaterialDriver;
friend class XmlMXCAFDoc_ClippingPlaneToolDriver;
friend class XmlMXCAFDoc_ColorToolDriver;
friend class XmlMXCAFDoc_DocumentToolDriver;
friend class XmlMXCAFDoc_LayerToolDriver;
friend class XmlMXCAFDoc_ShapeToolDriver;
friend class XmlMXCAFDoc_DimTolToolDriver;
friend class XmlMXCAFDoc_MaterialToolDriver;
friend class XmlMXCAFDoc_ViewToolDriver;

};







#endif // _XmlMXCAFDoc_HeaderFile
