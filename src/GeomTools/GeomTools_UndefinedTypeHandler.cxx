// File:	GeomTools_UndefinedTypeHandler.cxx
// Created:	Fri Oct 29 11:25:54 1999
// Author:	Pavel DURANDIN
//		<pdn@friendox>


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
