// Created on: 1999-10-29
// Created by: Pavel DURANDIN
// Copyright (c) 1999-1999 Matra Datavision
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


#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomTools_UndefinedTypeHandler.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomTools_UndefinedTypeHandler,Standard_Transient)

GeomTools_UndefinedTypeHandler::GeomTools_UndefinedTypeHandler()
{
}

//=======================================================================
//function : PrintCurve
//purpose  : 
//=======================================================================

void GeomTools_UndefinedTypeHandler::PrintCurve(const Handle(Geom_Curve)& /*C*/,
						Standard_OStream& OS,
						const Standard_Boolean compact) const
{
  if (!compact)
    OS << "****** UNKNOWN CURVE TYPE ******\n";
  else 
    cout << "****** UNKNOWN CURVE TYPE ******" << endl;
}

//=======================================================================
//function : ReadCurve
//purpose  : 
//=======================================================================

Standard_IStream& GeomTools_UndefinedTypeHandler::ReadCurve(const Standard_Integer /*ctype*/,
							    Standard_IStream& IS,
							    Handle(Geom_Curve)& /*C*/) const
{
  return IS;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void GeomTools_UndefinedTypeHandler::PrintCurve2d(const Handle(Geom2d_Curve)& /*C*/,
						  Standard_OStream& OS,
						  const Standard_Boolean compact) const
{
  if (!compact)
    OS << "****** UNKNOWN CURVE2d TYPE ******\n";
  else 
    cout << "****** UNKNOWN CURVE2d TYPE ******" << endl;
}

//=======================================================================
//function : ReadCurve2d
//purpose  : 
//=======================================================================

Standard_IStream& GeomTools_UndefinedTypeHandler::ReadCurve2d(const Standard_Integer /*ctype*/,
							      Standard_IStream& IS,
							      Handle(Geom2d_Curve)& /*C*/) const
{
  return IS;
}

//=======================================================================
//function : PrintSurface
//purpose  : 
//=======================================================================

void GeomTools_UndefinedTypeHandler::PrintSurface(const Handle(Geom_Surface)& /*S*/,
						  Standard_OStream& OS,
						  const Standard_Boolean compact) const
{
  if (!compact)
    OS << "****** UNKNOWN SURFACE TYPE ******\n";
  else 
    cout << "****** UNKNOWN SURFACE TYPE ******" << endl;
}

Standard_IStream& GeomTools_UndefinedTypeHandler::ReadSurface(const Standard_Integer /*ctype*/,
							      Standard_IStream& IS,
							      Handle(Geom_Surface)& /*S*/) const
{
  return IS;
}
