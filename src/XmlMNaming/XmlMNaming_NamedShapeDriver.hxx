// Created on: 2001-09-14
// Created by: Alexander GRIGORIEV
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

#ifndef _XmlMNaming_NamedShapeDriver_HeaderFile
#define _XmlMNaming_NamedShapeDriver_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BRepTools_ShapeSet.hxx>
#include <XmlMDF_ADriver.hxx>
#include <Standard_Boolean.hxx>
#include <TDocStd_FormatVersion.hxx>
#include <XmlObjMgt_RRelocationTable.hxx>
#include <XmlObjMgt_SRelocationTable.hxx>
#include <XmlObjMgt_Element.hxx>
class Message_Messenger;
class TDF_Attribute;
class XmlObjMgt_Persistent;
class TopTools_LocationSet;


class XmlMNaming_NamedShapeDriver;
DEFINE_STANDARD_HANDLE(XmlMNaming_NamedShapeDriver, XmlMDF_ADriver)


class XmlMNaming_NamedShapeDriver : public XmlMDF_ADriver
{
public:

  Standard_EXPORT XmlMNaming_NamedShapeDriver(const Handle(Message_Messenger)& aMessageDriver);
  
  Standard_EXPORT virtual Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean Paste
                              (const XmlObjMgt_Persistent& theSource,
                               const Handle(TDF_Attribute)& theTarget,
                               XmlObjMgt_RRelocationTable& theRelocTable) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Paste
                              (const Handle(TDF_Attribute)& theSource,
                               XmlObjMgt_Persistent& theTarget,
                               XmlObjMgt_SRelocationTable& theRelocTable) const Standard_OVERRIDE;
  
  //! Input the shapes from DOM element
  Standard_EXPORT void ReadShapeSection (const XmlObjMgt_Element& anElement,
                                         const Message_ProgressRange& theRange = Message_ProgressRange());
  
  //! Output the shapes into DOM element
  Standard_EXPORT void WriteShapeSection (XmlObjMgt_Element& anElement,
                                          TDocStd_FormatVersion theStorageFormatVersion,
                                          const Message_ProgressRange& theRange = Message_ProgressRange());
  
  //! Clear myShapeSet
  Standard_EXPORT void Clear();
  
  //! get the format of topology
    TopTools_LocationSet& GetShapesLocations();

  DEFINE_STANDARD_RTTIEXT(XmlMNaming_NamedShapeDriver,XmlMDF_ADriver)

private:

  BRepTools_ShapeSet myShapeSet;

};


#include <XmlMNaming_NamedShapeDriver.lxx>

#endif // _XmlMNaming_NamedShapeDriver_HeaderFile
