// File:	StepToTopoDS_TranslateCurveBoundedSurface.cxx
// Created:	Fri Feb 12 13:30:17 1999
// Author:	Andrey BETENEV
//		<abv@doomox.nnov.matra-dtv.fr>
//#4  szv          S4163: optimization
//:   abv 07.04.99 S4136: turn off fixing intersection of non-adjacent edges

#include <StepToTopoDS_TranslateCurveBoundedSurface.ixx>

#include <Precision.hxx>

#include <Geom_BoundedSurface.hxx>
#include <StepToGeom_MakeSurface.hxx>
#include <StepGeom_HArray1OfSurfaceBoundary.hxx>
#include <StepToTopoDS_TranslateCompositeCurve.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <BRep_Builder.hxx>

#include <BRepBuilderAPI_MakeFace.hxx>
#include <StepGeom_BSplineSurface.hxx>
#include <ShapeAlgo.hxx>
#include <ShapeAlgo_AlgoContainer.hxx>

//=======================================================================
//function : StepToTopoDS_TranslateCurveBoundedSurface
//purpose  : 
//=======================================================================

StepToTopoDS_TranslateCurveBoundedSurface::StepToTopoDS_TranslateCurveBoundedSurface ()
{
}

//=======================================================================
//function : StepToTopoDS_TranslateCurveBoundedSurface
//purpose  : 
//=======================================================================

StepToTopoDS_TranslateCurveBoundedSurface::StepToTopoDS_TranslateCurveBoundedSurface (
					   const Handle(StepGeom_CurveBoundedSurface) &CBS, 
					   const Handle(Transfer_TransientProcess) &TP)
{
  Init ( CBS, TP );
}

Standard_Boolean StepToTopoDS_TranslateCurveBoundedSurface::Init (
		 const Handle(StepGeom_CurveBoundedSurface) &CBS, 
		 const Handle(Transfer_TransientProcess) &TP)
{
  myFace.Nullify();
  if ( CBS.IsNull() ) return Standard_False;
  
  // translate basis surface 
  Handle(StepGeom_Surface) S = CBS->BasisSurface();
  Handle(Geom_Surface) Surf;
  if ( !StepToGeom_MakeSurface::Convert(S,Surf) ) {
    TP->AddFail ( CBS, "Basis surface not translated" );
    return Standard_False;
  }

  // abv 30.06.00: trj4_k1_geo-tu.stp #108: do as in TranslateFace
  // pdn to force bsplsurf to be periodic
  Handle(StepGeom_BSplineSurface) sgbss = Handle(StepGeom_BSplineSurface)::DownCast(S);
  if (!sgbss.IsNull()) {
/*
    StepGeom_BSplineSurfaceForm form = sgbss->SurfaceForm();
    if ((form == StepGeom_bssfCylindricalSurf)||
	(form == StepGeom_bssfConicalSurf)||
	(form == StepGeom_bssfSphericalSurf)||
	(form == StepGeom_bssfToroidalSurf)||
	(form == StepGeom_bssfSurfOfRevolution)||
	(form == StepGeom_bssfGeneralisedCone)||
        (form == StepGeom_bssfUnspecified)) 
*/
    {
      Handle(Geom_Surface) periodicSurf = ShapeAlgo::AlgoContainer()->ConvertToPeriodic (Surf);
      if(!periodicSurf.IsNull()) {
	TP->AddWarning(S,"Surface forced to be periodic");
	Surf = periodicSurf;
      }
    }
  }
    
  // create face
  BRep_Builder B;
  B.MakeFace ( myFace, Surf, Precision::Confusion() );

  // add natural bound if implicit
  if ( CBS->ImplicitOuter() ) { 
    if ( Surf->IsKind(STANDARD_TYPE(Geom_BoundedSurface)) ) {
      BRepBuilderAPI_MakeFace mf (Surf, Precision::Confusion());
      myFace = mf.Face();
    }
    else TP->AddWarning ( CBS, "Cannot make natural bounds on infinite surface" );
  }

  // translate boundaries 
  Handle(StepGeom_HArray1OfSurfaceBoundary) bnd = CBS->Boundaries();
  Standard_Integer nb = bnd->Length();
  for ( Standard_Integer i=1; i <= nb; i++ ) {
    Handle(StepGeom_CompositeCurve) cc = bnd->Value ( i ).BoundaryCurve();
    if ( cc.IsNull() ) continue;
    StepToTopoDS_TranslateCompositeCurve TrCC ( cc, TP, S, Surf );
    if ( ! TrCC.IsDone() ) {
      TP->AddWarning ( CBS, "Boundary not translated" );
      continue;
    }
    B.Add ( myFace, TrCC.Value() );
  }

  done = ! myFace.IsNull();
  return done;
}
	
//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

const TopoDS_Face &StepToTopoDS_TranslateCurveBoundedSurface::Value () const
{
  return myFace;
}
