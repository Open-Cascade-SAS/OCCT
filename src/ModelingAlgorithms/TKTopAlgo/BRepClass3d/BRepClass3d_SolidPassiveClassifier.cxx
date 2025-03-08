// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2024 OPEN CASCADE SAS
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

#include <BRepClass3d_SolidPassiveClassifier.hxx>

#include <BRepClass3d_Intersector3d.hxx>
#include <IntCurveSurface_IntersectionPoint.hxx>
#include <IntCurveSurface_TransitionOnCurve.hxx>
#include <Standard_DomainError.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Lin.hxx>

BRepClass3d_SolidPassiveClassifier::BRepClass3d_SolidPassiveClassifier()
    : isSet(Standard_False),
      myParam(0.0),
      myTolerance(0.0),
      myState(TopAbs_UNKNOWN),
      hasIntersect(Standard_False)
{
}

void BRepClass3d_SolidPassiveClassifier::Reset(const gp_Lin& L,
                                               const Standard_Real,
                                               const Standard_Real Tol)
{
  myLin       = L;
  myParam     = RealLast();
  myTolerance = Tol;
  myState     = TopAbs_UNKNOWN;
  isSet       = Standard_True;
}

void BRepClass3d_SolidPassiveClassifier::Compare(const TopoDS_Face& Face, const TopAbs_Orientation)
{
  if (!isSet)
  {
#ifdef OCCT_DEBUG
    std::cout << " Call to BRepClass3d_SolidPassiveClassifier::Compare  without a Reset ! ";
#endif
    return;
  }

  hasIntersect = Standard_False;
  myIntersector.Perform(myLin, myParam, myTolerance, Face);
  if (myIntersector.IsDone())
  {
    if (myIntersector.HasAPoint())
    {
      hasIntersect = Standard_True;
      if (myIntersector.WParameter() < myParam)
      {
        myParam = myIntersector.WParameter();
        myFace  = myIntersector.Face();
        if (Abs(myParam) <= myTolerance)
        {
          //-- #########################################
#ifdef OCCT_DEBUG
          std::cout << " myParam = " << myParam << " ds BRepClass3d_SolidPassiveClassifier.gxx  "
                    << std::endl;
#endif
          //-- #########################################
          myState = TopAbs_ON;
        }
        else
        {
          //-- The intersection point between the line and a face F of the solid
          //-- is in the face F or On a boundary of the face
          if (myIntersector.Transition() == IntCurveSurface_Out)
          {
            //-- The line is going from inside the solid to outside the solid.
            myState = TopAbs_IN;
          }
          else if (myIntersector.Transition() == IntCurveSurface_In)
          {
            myState = TopAbs_OUT;
          }
#ifdef OCCT_DEBUG
          else
          {
            std::cout << " -------- Probleme ds BRepClass3d_SolidPassiveClassifier.gxx "
                      << std::endl;
          }
#endif
        }
      }
      else
      {
        //-- No point has been found by the myIntersector.
        //-- Or a Point has been found with a greater parameter.
      }
    } //-- myIntersector Has a point
    else
    {
      //-- The myIntersector failed.
    }
  } //-- Face has not been rejected
}
