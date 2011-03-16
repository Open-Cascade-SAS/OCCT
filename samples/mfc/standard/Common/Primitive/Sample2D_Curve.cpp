#include "stdafx.h"

#include "Sample2D_Curve.h"


IMPLEMENT_STANDARD_HANDLE(Sample2D_Curve,AIS2D_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(Sample2D_Curve,AIS2D_InteractiveObject)

#include <Graphic2d_SetOfCurves.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <Precision.hxx>
#include <Select2D_SensitiveBox.hxx>
#include <Select2D_SensitiveSegment.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Ax2d.hxx>

#include <Select2D_SensitiveArc.hxx>
#include <Geom2d_Curve.hxx>
#include <GeomTools_Curve2dSet.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <gp_Vec2d.hxx>
#include <OSD_Environment.hxx>
#include <Graphic2d_Array1OfVertex.hxx>
#include <Graphic2d_PolyLine.hxx>
#include "SelectMgr_Selection.hxx"
#include "Prs2d_AspectLine.hxx"
#include "Graphic2d_TypeOfPolygonFilling.hxx"

Sample2D_Curve::Sample2D_Curve(const Handle_Geom2d_Curve aGeom2dCurve,
			       const Standard_Integer aTypeOfLineIndex,
			       const Standard_Integer aWidthOfLineIndex,
			       const Standard_Integer aColorIndex)
    :AIS2D_InteractiveObject()
{
  myGeom2dCurve = aGeom2dCurve;
  myTypeOfLineIndex  = aTypeOfLineIndex ;
  myWidthOfLineIndex = aWidthOfLineIndex;
  myColorIndex  = aColorIndex ;
}


void Sample2D_Curve::SetContext(const Handle(AIS2D_InteractiveContext)& theContext) 
{
  AIS2D_InteractiveObject::SetContext(theContext);

  Handle(Prs2d_AspectLine) aLineAspect = new Prs2d_AspectLine;  
  aLineAspect->SetTypeOfFill(Graphic2d_TOPF_EMPTY);
  
  Handle(Graphic2d_SetOfCurves) segment;
  segment = new Graphic2d_SetOfCurves(this);

  segment->Add(myGeom2dCurve);

  aLineAspect->SetTypeIndex(myTypeOfLineIndex);
  aLineAspect->SetColorIndex(myColorIndex);
  aLineAspect->SetWidthIndex(myWidthOfLineIndex);

  SetAspect(aLineAspect, segment);

}

