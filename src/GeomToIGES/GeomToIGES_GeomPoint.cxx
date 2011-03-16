// File:	GeomToIGES_GeomPoint.cxx

// modif du 14/09/95 mjm
// prise en compte de l'unite choisi par l'utilisateur
// pour l'ecriture du fichier IGES.

#include <GeomToIGES_GeomPoint.ixx>

#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>

#include <Geom_Point.hxx>

#include <IGESGeom_Point.hxx>

#include <Interface_Macros.hxx>


//=============================================================================
// GeomToIGES_GeomPoint
//=============================================================================

GeomToIGES_GeomPoint::GeomToIGES_GeomPoint
(const GeomToIGES_GeomEntity& GE)
:GeomToIGES_GeomEntity(GE)
{
}


//=============================================================================
// GeomToIGES_GeomPoint
//=============================================================================

GeomToIGES_GeomPoint::GeomToIGES_GeomPoint()

:GeomToIGES_GeomEntity()
{
}


//=============================================================================
// Transfer de Point de Geom vers IGES
// TranferPoint
//=============================================================================

Handle(IGESGeom_Point) GeomToIGES_GeomPoint::TransferPoint
( const Handle(Geom_Point)& P)
{
  Handle(IGESGeom_Point) Piges = new IGESGeom_Point;
  if (P.IsNull()) {
    return Piges;
  }

  Standard_Real X,Y,Z;
  P->Coord (X,Y,Z);
  Handle(IGESBasic_SubfigureDef) voidsubdef;
  Piges-> Init(gp_XYZ(X/GetUnit(),Y/GetUnit(),Z/GetUnit()), voidsubdef);
  return Piges;
}

//=============================================================================
// Transfer de Point de Geom vers IGES
// TranferPoint
//=============================================================================

Handle(IGESGeom_Point) GeomToIGES_GeomPoint::TransferPoint
( const Handle(Geom_CartesianPoint)& P)
{

  Handle(IGESGeom_Point) Piges = new IGESGeom_Point;
  if (P.IsNull()) {
    return Piges;
  }

  Standard_Real X,Y,Z;
  P->Coord (X,Y,Z);
  Handle(IGESBasic_SubfigureDef) voidsubdef;
  Piges-> Init(gp_XYZ(X/GetUnit(),Y/GetUnit(),Z/GetUnit()), voidsubdef);
  return Piges;
}
