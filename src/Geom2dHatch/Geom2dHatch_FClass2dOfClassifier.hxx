// Created on: 1994-02-03
// Created by: Jean Marc LACHAUME
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Geom2dHatch_FClass2dOfClassifier_HeaderFile
#define _Geom2dHatch_FClass2dOfClassifier_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <gp_Lin2d.hxx>
#include <TopTrans_CurveTransition.hxx>
#include <Geom2dHatch_Intersector.hxx>
#include <Standard_Integer.hxx>
#include <TopAbs_State.hxx>
#include <TopAbs_Orientation.hxx>
class Standard_DomainError;
class Geom2dAdaptor_Curve;
class Geom2dHatch_Intersector;
class gp_Lin2d;



class Geom2dHatch_FClass2dOfClassifier 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an undefined classifier.
  Standard_EXPORT Geom2dHatch_FClass2dOfClassifier();
  
  //! Starts  a  classification process.   The  point to
  //! classify is the origin of  the  line <L>.  <P>  is
  //! the original length of the segment on <L>  used to
  //! compute  intersections.   <Tol> is the   tolerance
  //! attached to the line segment in intersections.
  Standard_EXPORT void Reset (const gp_Lin2d& L, const Standard_Real P, const Standard_Real Tol);
  
  //! Updates  the classification process with  the edge
  //! <E> from the boundary.
  Standard_EXPORT void Compare (const Geom2dAdaptor_Curve& E, const TopAbs_Orientation Or);
  
  //! Returns the current value of the parameter.
    Standard_Real Parameter() const;
  
  //! Returns the intersecting algorithm.
    Geom2dHatch_Intersector& Intersector();
  
  //! Returns  0  if  the   last  compared   edge had no
  //! relevant intersection.  Else returns  the index of
  //! this   intersection  in the    last   intersection
  //! algorithm.
    Standard_Integer ClosestIntersection() const;
  
  //! Returns the current state of the point.
    TopAbs_State State() const;
  
  //! Returns the Standard_True if the closest intersection point
  //! represents head or end of the edge. Returns Standard_False
  //! otherwise.
    Standard_Boolean IsHeadOrEnd() const;




protected:





private:



  Standard_Boolean myIsSet;
  Standard_Boolean myFirstCompare;
  Standard_Boolean myFirstTrans;
  gp_Lin2d myLin;
  Standard_Real myParam;
  Standard_Real myTolerance;
  TopTrans_CurveTransition myTrans;
  Geom2dHatch_Intersector myIntersector;
  Standard_Integer myClosest;
  TopAbs_State myState;
  Standard_Boolean myIsHeadOrEnd;


};

#define TheEdge Geom2dAdaptor_Curve
#define TheEdge_hxx <Geom2dAdaptor_Curve.hxx>
#define TheIntersector Geom2dHatch_Intersector
#define TheIntersector_hxx <Geom2dHatch_Intersector.hxx>
#define TopClass_Classifier2d Geom2dHatch_FClass2dOfClassifier
#define TopClass_Classifier2d_hxx <Geom2dHatch_FClass2dOfClassifier.hxx>

#include <TopClass_Classifier2d.lxx>

#undef TheEdge
#undef TheEdge_hxx
#undef TheIntersector
#undef TheIntersector_hxx
#undef TopClass_Classifier2d
#undef TopClass_Classifier2d_hxx




#endif // _Geom2dHatch_FClass2dOfClassifier_HeaderFile
