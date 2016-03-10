// Created on: 2004-05-11
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

#ifndef _BinTools_HeaderFile
#define _BinTools_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_ExtCharacter.hxx>
#include <Standard_OStream.hxx>
#include <Standard_IStream.hxx>

class TopoDS_Shape;
class BinTools_ShapeSet;
class BinTools_Curve2dSet;
class BinTools_CurveSet;
class BinTools_SurfaceSet;
class BinTools_LocationSet;


//! Tool to keep shapes in binary format
class BinTools 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static Standard_OStream& PutReal (Standard_OStream& OS, const Standard_Real theValue);
  
  Standard_EXPORT static Standard_OStream& PutInteger (Standard_OStream& OS, const Standard_Integer theValue);
  
  Standard_EXPORT static Standard_OStream& PutBool (Standard_OStream& OS, const Standard_Boolean theValue);
  
  Standard_EXPORT static Standard_OStream& PutExtChar (Standard_OStream& OS, const Standard_ExtCharacter theValue);
  
  Standard_EXPORT static Standard_IStream& GetReal (Standard_IStream& IS, Standard_Real& theValue);
  
  Standard_EXPORT static Standard_IStream& GetInteger (Standard_IStream& IS, Standard_Integer& theValue);
  
  Standard_EXPORT static Standard_IStream& GetBool (Standard_IStream& IS, Standard_Boolean& theValue);
  
  Standard_EXPORT static Standard_IStream& GetExtChar (Standard_IStream& IS, Standard_ExtCharacter& theValue);
  
  //! Writes <theShape> on <theStream> in binary format.
  Standard_EXPORT static void Write (const TopoDS_Shape& theShape, Standard_OStream& theStream);
  
  //! Reads a shape from <theStream> and returns it in <theShape>.
  Standard_EXPORT static void Read (TopoDS_Shape& theShape, Standard_IStream& theStream);
  
  //! Writes <theShape> in <theFile>.
  Standard_EXPORT static Standard_Boolean Write (const TopoDS_Shape& theShape, const Standard_CString theFile);
  
  //! Reads a shape from <theFile> and returns it in <theShape>.
  Standard_EXPORT static Standard_Boolean Read (TopoDS_Shape& theShape, const Standard_CString theFile);

protected:





private:




friend class BinTools_ShapeSet;
friend class BinTools_Curve2dSet;
friend class BinTools_CurveSet;
friend class BinTools_SurfaceSet;
friend class BinTools_LocationSet;

};







#endif // _BinTools_HeaderFile
