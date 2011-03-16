// File:	GeomTools.cxx
// Created:	Thu Jan 21 19:59:19 1993
// Author:	Remi LEQUETTE
//		<rle@phylox>

#include <GeomTools.ixx>

#include <GeomTools_SurfaceSet.hxx>
#include <GeomTools_CurveSet.hxx>
#include <GeomTools_Curve2dSet.hxx>
#include <GeomTools_UndefinedTypeHandler.hxx>

static Handle(GeomTools_UndefinedTypeHandler) theActiveHandler = new GeomTools_UndefinedTypeHandler;

void  GeomTools::Dump(const Handle(Geom_Surface)& S, Standard_OStream& OS)
{
  GeomTools_SurfaceSet::PrintSurface(S,OS);
}

void  GeomTools::Write(const Handle(Geom_Surface)& S, Standard_OStream& OS)
{
  GeomTools_SurfaceSet::PrintSurface(S,OS,Standard_True);
}

void GeomTools::Read(Handle(Geom_Surface)& S, Standard_IStream& IS)
{
  GeomTools_SurfaceSet::ReadSurface(IS,S);
}

void  GeomTools::Dump(const Handle(Geom_Curve)& C, Standard_OStream& OS)
{
  GeomTools_CurveSet::PrintCurve(C,OS);
}

void  GeomTools::Write(const Handle(Geom_Curve)& C, Standard_OStream& OS)
{
  GeomTools_CurveSet::PrintCurve(C,OS,Standard_True);
}

void GeomTools::Read(Handle(Geom_Curve)& C, Standard_IStream& IS)
{
  GeomTools_CurveSet::ReadCurve(IS,C);
}

void  GeomTools::Dump(const Handle(Geom2d_Curve)& C, Standard_OStream& OS)
{
  GeomTools_Curve2dSet::PrintCurve2d(C,OS);
}

void  GeomTools::Write(const Handle(Geom2d_Curve)& C, Standard_OStream& OS)
{
  GeomTools_Curve2dSet::PrintCurve2d(C,OS,Standard_True);
}

void  GeomTools::Read(Handle(Geom2d_Curve)& C, Standard_IStream& IS)
{
  GeomTools_Curve2dSet::ReadCurve2d(IS,C);
}

//=======================================================================
//function : SetUndefinedTypeHandler
//purpose  : 
//=======================================================================

void GeomTools::SetUndefinedTypeHandler(const Handle(GeomTools_UndefinedTypeHandler)& aHandler)
{
  if(!aHandler.IsNull())
    theActiveHandler = aHandler;
}

//=======================================================================
//function : GetUndefinedTypeHandler
//purpose  : 
//=======================================================================

Handle(GeomTools_UndefinedTypeHandler) GeomTools::GetUndefinedTypeHandler()
{
  return theActiveHandler;
}
