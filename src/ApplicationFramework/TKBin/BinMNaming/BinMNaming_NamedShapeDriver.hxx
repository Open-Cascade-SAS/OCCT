// Created on: 2004-04-08
// Created by: Sergey ZARITCHNY <szy@opencascade.com>
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

#ifndef _BinMNaming_NamedShapeDriver_HeaderFile
#define _BinMNaming_NamedShapeDriver_HeaderFile

#include <Standard.hxx>

#include <BinTools_ShapeSet.hxx>
#include <Standard_Integer.hxx>
#include <BinMDF_ADriver.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <BinObjMgt_SRelocationTable.hxx>
#include <Standard_IStream.hxx>
#include <Standard_OStream.hxx>
class Message_Messenger;
class TDF_Attribute;
class BinObjMgt_Persistent;
class BinTools_LocationSet;

class BinMNaming_NamedShapeDriver;
DEFINE_STANDARD_HANDLE(BinMNaming_NamedShapeDriver, BinMDF_ADriver)

//! NamedShape  Attribute Driver.
class BinMNaming_NamedShapeDriver : public BinMDF_ADriver
{

public:
  Standard_EXPORT BinMNaming_NamedShapeDriver(const Handle(Message_Messenger)& theMessageDriver);

  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

  Standard_EXPORT Standard_Boolean
    Paste(const BinObjMgt_Persistent&  Source,
          const Handle(TDF_Attribute)& Target,
          BinObjMgt_RRelocationTable&  RelocTable) const Standard_OVERRIDE;

  Standard_EXPORT void Paste(const Handle(TDF_Attribute)& Source,
                             BinObjMgt_Persistent&        Target,
                             BinObjMgt_SRelocationTable&  RelocTable) const Standard_OVERRIDE;

  //! Input the shapes from Bin Document file
  Standard_EXPORT void ReadShapeSection(
    Standard_IStream&            theIS,
    const Message_ProgressRange& therange = Message_ProgressRange());

  //! Output the shapes into Bin Document file
  Standard_EXPORT void WriteShapeSection(
    Standard_OStream&            theOS,
    const Standard_Integer       theDocVer,
    const Message_ProgressRange& therange = Message_ProgressRange());

  //! Clear myShapeSet
  Standard_EXPORT void Clear();

  //! Return true if shape should be stored with triangles.
  Standard_Boolean IsWithTriangles() const { return myWithTriangles; }

  //! Return true if shape should be stored with triangulation normals.
  Standard_Boolean IsWithNormals() const { return myWithNormals; }

  //! set whether to store triangulation
  void SetWithTriangles(const Standard_Boolean isWithTriangles);
  //! set whether to store triangulation with normals
  void SetWithNormals(const Standard_Boolean isWithNormals);
  //! get the shapes locations
  Standard_EXPORT BinTools_LocationSet& GetShapesLocations() const;

  //! Sets the flag for quick part of the document access: shapes are stored in the attribute.
  Standard_EXPORT void EnableQuickPart(const Standard_Boolean theValue)
  {
    myIsQuickPart = theValue;
  }

  //! Returns true if quick part of the document access is enabled: shapes are stored in the
  //! attribute.
  Standard_EXPORT Standard_Boolean IsQuickPart() { return myIsQuickPart; }

  //! Returns shape-set of the needed type
  Standard_EXPORT BinTools_ShapeSetBase* ShapeSet(const Standard_Boolean theReading);

  DEFINE_STANDARD_RTTIEXT(BinMNaming_NamedShapeDriver, BinMDF_ADriver)

private:
  BinTools_ShapeSetBase* myShapeSet;
  Standard_Boolean       myWithTriangles;
  Standard_Boolean       myWithNormals;
  //! Enables storing of whole shape data just in the attribute, not in a separated shapes section
  Standard_Boolean myIsQuickPart;
};

#include <BinMNaming_NamedShapeDriver.lxx>

#endif // _BinMNaming_NamedShapeDriver_HeaderFile
