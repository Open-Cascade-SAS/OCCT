// File:	GeomToStep_MakePolyline.cxx
// Created:	Mon Jul 12 16:40:28 1993
// Author:	Martine LANGLOIS
//		<mla@mastox>

#include <GeomToStep_MakePolyline.ixx>
#include <StdFail_NotDone.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <StepGeom_Polyline.hxx>
#include <StepGeom_HArray1OfCartesianPoint.hxx>
#include <GeomToStep_MakeCartesianPoint.hxx>
#include <TCollection_HAsciiString.hxx>

//=============================================================================
// Creation d' une polyline Step a partir d' une Array1OfPnt 
//=============================================================================

GeomToStep_MakePolyline::GeomToStep_MakePolyline( const TColgp_Array1OfPnt& P)
{
  gp_Pnt P1;
#include <GeomToStep_MakePolyline_gen.pxx>
}

//=============================================================================
// Creation d' une polyline Step a partir d' une Array1OfPnt2d
//=============================================================================

GeomToStep_MakePolyline::GeomToStep_MakePolyline( const TColgp_Array1OfPnt2d& P)
{
  gp_Pnt2d P1;
#include <GeomToStep_MakePolyline_gen.pxx>
}
//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_Polyline) &
      GeomToStep_MakePolyline::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return thePolyline;
}
