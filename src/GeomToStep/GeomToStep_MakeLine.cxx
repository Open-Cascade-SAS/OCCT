// File:	GeomToStep_MakeLine.cxx
// Created:	Thu Jun 17 11:11:28 1993
// Author:	Martine LANGLOIS
//		<mla@mastox>

#include <GeomToStep_MakeLine.ixx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_Line.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_Vector.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <GeomToStep_MakeLine.hxx>
#include <GeomToStep_MakeCartesianPoint.hxx>
#include <GeomToStep_MakeVector.hxx>
#include <TCollection_HAsciiString.hxx>


//=============================================================================
// Creation d' une line de prostep a partir d' une Lin de gp
//=============================================================================

GeomToStep_MakeLine::GeomToStep_MakeLine( const gp_Lin& L)
{
#define Vec_gen gp_Vec
#include <GeomToStep_MakeLine_gen.pxx>
#undef Vec_gen
}

//=============================================================================
// Creation d' une line de prostep a partir d' une Lin2d de gp
//=============================================================================

GeomToStep_MakeLine::GeomToStep_MakeLine( const gp_Lin2d& L)
{
#define Vec_gen gp_Vec2d
#include <GeomToStep_MakeLine_gen.pxx>
#undef Vec_gen
}

//=============================================================================
// Creation d' une line de prostep a partir d' une Line de Geom
//=============================================================================

GeomToStep_MakeLine::GeomToStep_MakeLine ( const Handle(Geom_Line)& Gline)
{
  gp_Lin L;
  L = Gline->Lin();
#define Vec_gen gp_Vec
#include <GeomToStep_MakeLine_gen.pxx>
#undef Vec_gen
}

//=============================================================================
// Creation d' une line de prostep a partir d' une Line de Geom2d
//=============================================================================

GeomToStep_MakeLine::GeomToStep_MakeLine ( const Handle(Geom2d_Line)& Gline)
{
  gp_Lin2d L;
  L = Gline->Lin2d();
#define Vec_gen gp_Vec2d
#include <GeomToStep_MakeLine_gen.pxx>
#undef Vec_gen
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_Line) &
      GeomToStep_MakeLine::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theLine;
}

