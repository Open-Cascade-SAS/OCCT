// Created on: 1999-10-29
// Created by: Pavel DURANDIN
// Copyright (c) 1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _GeomTools_UndefinedTypeHandler_HeaderFile
#define _GeomTools_UndefinedTypeHandler_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Standard_OStream.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_IStream.hxx>
#include <Standard_Integer.hxx>
class Geom_Curve;
class Geom2d_Curve;
class Geom_Surface;


class GeomTools_UndefinedTypeHandler;
DEFINE_STANDARD_HANDLE(GeomTools_UndefinedTypeHandler, Standard_Transient)


class GeomTools_UndefinedTypeHandler : public Standard_Transient
{

public:

  
  Standard_EXPORT GeomTools_UndefinedTypeHandler();
  
  Standard_EXPORT virtual void PrintCurve (const Handle(Geom_Curve)& C, Standard_OStream& OS, const Standard_Boolean compact = Standard_False) const;
  
  Standard_EXPORT virtual Standard_IStream& ReadCurve (const Standard_Integer ctype, Standard_IStream& IS, Handle(Geom_Curve)& C) const;
  
  Standard_EXPORT virtual void PrintCurve2d (const Handle(Geom2d_Curve)& C, Standard_OStream& OS, const Standard_Boolean compact = Standard_False) const;
  
  Standard_EXPORT virtual Standard_IStream& ReadCurve2d (const Standard_Integer ctype, Standard_IStream& IS, Handle(Geom2d_Curve)& C) const;
  
  Standard_EXPORT virtual void PrintSurface (const Handle(Geom_Surface)& S, Standard_OStream& OS, const Standard_Boolean compact = Standard_False) const;
  
  Standard_EXPORT virtual Standard_IStream& ReadSurface (const Standard_Integer ctype, Standard_IStream& IS, Handle(Geom_Surface)& S) const;




  DEFINE_STANDARD_RTTIEXT(GeomTools_UndefinedTypeHandler,Standard_Transient)

protected:




private:




};







#endif // _GeomTools_UndefinedTypeHandler_HeaderFile
