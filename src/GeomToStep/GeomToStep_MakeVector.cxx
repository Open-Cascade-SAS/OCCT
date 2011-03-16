// File:	GeomToStep_MakeVector.cxx
// Created:	Thu Jun 17 14:56:55 1993
// Author:	Martine LANGLOIS
//		<mla@mastox>

#include <GeomToStep_MakeVector.ixx>
#include <StdFail_NotDone.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Vec.hxx>
#include <Geom_Vector.hxx>
#include <StepGeom_Vector.hxx>
#include <GeomToStep_MakeDirection.hxx>
#include <TCollection_HAsciiString.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d' un vector de prostep a partir d' un Vec de gp
//=============================================================================

GeomToStep_MakeVector::GeomToStep_MakeVector( const gp_Vec& V)
{
  gp_Dir D = gp_Dir(V);
  Standard_Real lFactor = UnitsMethods::LengthFactor();
#include <GeomToStep_MakeVector_gen.pxx>
}
//=============================================================================
// Creation d' un vector de prostep a partir d' un Vec2d de gp
//=============================================================================

GeomToStep_MakeVector::GeomToStep_MakeVector( const gp_Vec2d& V)
{
  gp_Dir2d D = gp_Dir2d(V);
  Standard_Real lFactor = 1.;
#include <GeomToStep_MakeVector_gen.pxx>
}

//=============================================================================
// Creation d' un vector de prostep a partir d' un Vector de Geom
//=============================================================================

GeomToStep_MakeVector::GeomToStep_MakeVector ( const Handle(Geom_Vector)&
					    GVector)
{
  gp_Vec V;
  V = GVector->Vec();
  gp_Dir D = gp_Dir(V);
  Standard_Real lFactor = UnitsMethods::LengthFactor();
#include <GeomToStep_MakeVector_gen.pxx>
}

//=============================================================================
// Creation d' un vector de prostep a partir d' un Vector de Geom2d
//=============================================================================

GeomToStep_MakeVector::GeomToStep_MakeVector ( const Handle(Geom2d_Vector)&
					    GVector)
{
  gp_Vec2d V;
  V = GVector->Vec2d();
  gp_Dir2d D = gp_Dir2d(V);
  Standard_Real lFactor = 1.;
#include <GeomToStep_MakeVector_gen.pxx>
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_Vector) &
      GeomToStep_MakeVector::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theVector;
}
