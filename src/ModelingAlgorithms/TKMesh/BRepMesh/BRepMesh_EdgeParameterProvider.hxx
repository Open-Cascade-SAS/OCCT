// Created on: 2014-08-13
// Created by: Oleg AGASHIN
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef _BRepMesh_EdgeParameterProvider_HeaderFile
#define _BRepMesh_EdgeParameterProvider_HeaderFile

#include <IMeshData_Types.hxx>
#include <IMeshData_Edge.hxx>
#include <IMeshData_Face.hxx>
#include <TopoDS.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Extrema_LocateExtPC.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Geom2dAdaptor_Curve.hxx>

class gp_Pnt;
class TopoDS_Edge;
class TopoDS_Face;

//! Auxiliary class provides correct parameters
//! on curve regarding SameParameter flag.
template <class ParametersCollection>
class BRepMesh_EdgeParameterProvider : public Standard_Transient
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor. Initializes empty provider.
  BRepMesh_EdgeParameterProvider()
      : myIsSameParam(Standard_False),
        myFirstParam(0.0),
        myOldFirstParam(0.0),
        myScale(0.0),
        myCurParam(0.0),
        myFoundParam(0.0)
  {
  }

  //! Constructor.
  //! @param theEdge edge which parameters should be processed.
  //! @param theFace face the parametric values are defined for.
  //! @param theParameters parameters corresponded to discretization points.
  BRepMesh_EdgeParameterProvider(const IMeshData::IEdgeHandle& theEdge,
                                 const TopAbs_Orientation      theOrientation,
                                 const IMeshData::IFaceHandle& theFace,
                                 const ParametersCollection&   theParameters)
  {
    Init(theEdge, theOrientation, theFace, theParameters);
  }

  //! Initialized provider by the given data.
  void Init(const IMeshData::IEdgeHandle& theEdge,
            const TopAbs_Orientation      theOrientation,
            const IMeshData::IFaceHandle& theFace,
            const ParametersCollection&   theParameters)
  {
    myParameters  = theParameters;
    myIsSameParam = theEdge->GetSameParam();
    myScale       = 1.;

    // Extract actual parametric values
    const TopoDS_Edge aEdge = TopoDS::Edge(theEdge->GetEdge().Oriented(theOrientation));

    myCurveAdaptor.Initialize(aEdge, theFace->GetFace());
    if (myIsSameParam)
    {
      return;
    }

    myFirstParam                   = myCurveAdaptor.FirstParameter();
    const Standard_Real aLastParam = myCurveAdaptor.LastParameter();

    myFoundParam = myCurParam = myFirstParam;

    // Extract parameters stored in polygon
    myOldFirstParam                   = myParameters->Value(myParameters->Lower());
    const Standard_Real aOldLastParam = myParameters->Value(myParameters->Upper());

    // Calculate scale factor between actual and stored parameters
    if ((myOldFirstParam != myFirstParam || aOldLastParam != aLastParam)
        && myOldFirstParam != aOldLastParam)
    {
      myScale = (aLastParam - myFirstParam) / (aOldLastParam - myOldFirstParam);
    }

    myProjector.Initialize(myCurveAdaptor,
                           myCurveAdaptor.FirstParameter(),
                           myCurveAdaptor.LastParameter(),
                           Precision::PConfusion());
  }

  //! Returns parameter according to SameParameter flag of the edge.
  //! If SameParameter is TRUE returns value from parameters w/o changes,
  //! elsewhere scales initial parameter and tries to determine resulting
  //! value using projection of the corresponded 3D point on PCurve.
  Standard_Real Parameter(const Standard_Integer theIndex, const gp_Pnt& thePoint3d) const
  {
    if (myIsSameParam)
    {
      return myParameters->Value(theIndex);
    }

    // Use scaled
    const Standard_Real aParam = myParameters->Value(theIndex);

    const Standard_Real aPrevParam = myCurParam;
    myCurParam                     = myFirstParam + myScale * (aParam - myOldFirstParam);

    const Standard_Real aPrevFoundParam = myFoundParam;
    myFoundParam += (myCurParam - aPrevParam);

    myProjector.Perform(thePoint3d, myFoundParam);
    if (myProjector.IsDone())
    {
      const Standard_Real aFoundParam = myProjector.Point().Parameter();
      if ((aPrevFoundParam < myFoundParam && aPrevFoundParam < aFoundParam)
          || (aPrevFoundParam > myFoundParam && aPrevFoundParam > aFoundParam))
      {
        // Rude protection against case when amplified parameter goes before
        // previous one due to period or other reason occurred in projector.
        // Using parameter returned by projector as is can produce self-intersections.
        myFoundParam = aFoundParam;
      }
    }

    return myFoundParam;
  }

  //! Returns pcurve used to compute parameters.
  const Handle(Adaptor2d_Curve2d)& GetPCurve() const
  {
    return myCurveAdaptor.CurveOnSurface().GetCurve();
  }

private:
  ParametersCollection myParameters;

  Standard_Boolean myIsSameParam;
  Standard_Real    myFirstParam;

  Standard_Real myOldFirstParam;
  Standard_Real myScale;

  mutable Standard_Real myCurParam;
  mutable Standard_Real myFoundParam;

  BRepAdaptor_Curve myCurveAdaptor;

  mutable Extrema_LocateExtPC myProjector;
};

#endif
