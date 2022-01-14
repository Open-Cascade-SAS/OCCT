// Created on: 2003-11-27
// Created by: Alexander SOLOVYOV
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef _MeshVS_Drawer_HeaderFile
#define _MeshVS_Drawer_HeaderFile

#include <Standard.hxx>

#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <MeshVS_DataMapOfIntegerBoolean.hxx>
#include <TColStd_DataMapOfIntegerReal.hxx>
#include <MeshVS_DataMapOfIntegerColor.hxx>
#include <MeshVS_DataMapOfIntegerMaterial.hxx>
#include <MeshVS_DataMapOfIntegerAsciiString.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class Quantity_Color;
class Graphic3d_MaterialAspect;
class TCollection_AsciiString;


class MeshVS_Drawer;
DEFINE_STANDARD_HANDLE(MeshVS_Drawer, Standard_Transient)

//! This class provided the common interface to share between classes
//! big set of constants affecting to object appearance. By default, this class
//! can store integers, doubles, OCC colors, OCC materials. Each of OCC enum members
//! can be stored as integers.
class MeshVS_Drawer : public Standard_Transient
{

public:

  
  //! This method copies other drawer contents to this.
  Standard_EXPORT virtual void Assign (const Handle(MeshVS_Drawer)& aDrawer);
  
  Standard_EXPORT void SetInteger (const Standard_Integer Key, const Standard_Integer Value);
  
  Standard_EXPORT void SetDouble (const Standard_Integer Key, const Standard_Real Value);
  
  Standard_EXPORT void SetBoolean (const Standard_Integer Key, const Standard_Boolean Value);
  
  Standard_EXPORT void SetColor (const Standard_Integer Key, const Quantity_Color& Value);
  
  Standard_EXPORT void SetMaterial (const Standard_Integer Key, const Graphic3d_MaterialAspect& Value);
  
  Standard_EXPORT void SetAsciiString (const Standard_Integer Key, const TCollection_AsciiString& Value);
  
  Standard_EXPORT Standard_Boolean GetInteger (const Standard_Integer Key, Standard_Integer& Value) const;
  
  Standard_EXPORT Standard_Boolean GetDouble (const Standard_Integer Key, Standard_Real& Value) const;
  
  Standard_EXPORT Standard_Boolean GetBoolean (const Standard_Integer Key, Standard_Boolean& Value) const;
  
  Standard_EXPORT Standard_Boolean GetColor (const Standard_Integer Key, Quantity_Color& Value) const;
  
  Standard_EXPORT Standard_Boolean GetMaterial (const Standard_Integer Key, Graphic3d_MaterialAspect& Value) const;
  
  Standard_EXPORT Standard_Boolean GetAsciiString (const Standard_Integer Key, TCollection_AsciiString& Value) const;
  
  Standard_EXPORT Standard_Boolean RemoveInteger (const Standard_Integer Key);
  
  Standard_EXPORT Standard_Boolean RemoveDouble (const Standard_Integer Key);
  
  Standard_EXPORT Standard_Boolean RemoveBoolean (const Standard_Integer Key);
  
  Standard_EXPORT Standard_Boolean RemoveColor (const Standard_Integer Key);
  
  Standard_EXPORT Standard_Boolean RemoveMaterial (const Standard_Integer Key);
  
  Standard_EXPORT Standard_Boolean RemoveAsciiString (const Standard_Integer Key);




  DEFINE_STANDARD_RTTIEXT(MeshVS_Drawer,Standard_Transient)

protected:




private:


  TColStd_DataMapOfIntegerInteger myIntegers;
  MeshVS_DataMapOfIntegerBoolean myBooleans;
  TColStd_DataMapOfIntegerReal myDoubles;
  MeshVS_DataMapOfIntegerColor myColors;
  MeshVS_DataMapOfIntegerMaterial myMaterials;
  MeshVS_DataMapOfIntegerAsciiString myAsciiString;


};







#endif // _MeshVS_Drawer_HeaderFile
