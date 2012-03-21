// Created on: 1999-10-29
// Created by: Pavel DURANDIN
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <GeomTools_UndefinedTypeHandler.ixx>

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
