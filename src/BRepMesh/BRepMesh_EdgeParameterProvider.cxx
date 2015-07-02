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

#include <BRepMesh_EdgeParameterProvider.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <Precision.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepMesh_EdgeParameterProvider::BRepMesh_EdgeParameterProvider(
  const TopoDS_Edge&                   theEdge,
  const TopoDS_Face&                   theFace,
  const Handle(TColStd_HArray1OfReal)& theParameters)
  : myParameters(theParameters),
    myIsSameParam(BRep_Tool::SameParameter(theEdge)),
    myScale(1.),
    myCurveAdaptor(theEdge, theFace)
{
  if (myIsSameParam)
    return;

  // Extract actual parametric values
  Standard_Real aLastParam;
  BRep_Tool::Range(theEdge, theFace, myFirstParam, aLastParam);

  myFoundParam = myCurParam = myFirstParam;

  // Extract parameters stored in polygon
  myOldFirstParam =
    myParameters->Value(myParameters->Lower());

  const Standard_Real aOldLastParam =
    myParameters->Value(myParameters->Upper());

  // Calculate scale factor between actual and stored parameters
  if ((myOldFirstParam != myFirstParam || aOldLastParam != aLastParam) &&
    myOldFirstParam != aOldLastParam)
  {
    myScale = (aLastParam - myFirstParam) /
      (aOldLastParam - myOldFirstParam);
  }

  myProjector.Initialize(myCurveAdaptor, myCurveAdaptor.FirstParameter(),
    myCurveAdaptor.LastParameter(), Precision::PConfusion());
}

//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================
Standard_Real BRepMesh_EdgeParameterProvider::Parameter(
  const Standard_Integer theIndex,
  const gp_Pnt&          thePoint3d)
{
  if (myIsSameParam)
    return myParameters->Value(theIndex);

  // Use scaled
  Standard_Real aPrevParam = myCurParam;
  myCurParam = myFirstParam + myScale * 
    (myParameters->Value(theIndex) - myOldFirstParam);

  myFoundParam += (myCurParam - aPrevParam);

  myProjector.Perform(thePoint3d, myFoundParam);
  if (myProjector.IsDone())
    myFoundParam = myProjector.Point().Parameter();

  return myFoundParam;
}
