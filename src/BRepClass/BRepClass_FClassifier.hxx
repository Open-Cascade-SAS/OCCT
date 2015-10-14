// Created on: 1992-11-18
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _BRepClass_FClassifier_HeaderFile
#define _BRepClass_FClassifier_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepClass_FClass2dOfFClassifier.hxx>
#include <BRepClass_Edge.hxx>
#include <Standard_Real.hxx>
#include <IntRes2d_Position.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_State.hxx>
class Standard_DomainError;
class BRepClass_FaceExplorer;
class BRepClass_Edge;
class BRepClass_Intersector;
class BRepClass_FClass2dOfFClassifier;
class gp_Pnt2d;



class BRepClass_FClassifier 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor, undefined algorithm.
  Standard_EXPORT BRepClass_FClassifier();
  
  //! Creates an algorithm to classify the Point  P with
  //! Tolerance <T> on the face described by <F>.
  Standard_EXPORT BRepClass_FClassifier(BRepClass_FaceExplorer& F, const gp_Pnt2d& P, const Standard_Real Tol);
  
  //! Classify  the Point  P  with  Tolerance <T> on the
  //! face described by <F>.
  Standard_EXPORT void Perform (BRepClass_FaceExplorer& F, const gp_Pnt2d& P, const Standard_Real Tol);
  
  //! Returns the result of the classification.
  Standard_EXPORT TopAbs_State State() const;
  
  //! Returns  True when  the   state was computed by  a
  //! rejection. The state is OUT.
    Standard_Boolean Rejected() const;
  
  //! Returns True if  the face  contains  no wire.  The
  //! state is IN.
    Standard_Boolean NoWires() const;
  
  //! Returns   the    Edge  used   to    determine  the
  //! classification. When the State is ON  this  is the
  //! Edge containing the point.
  Standard_EXPORT const BRepClass_Edge& Edge() const;
  
  //! Returns the parameter on Edge() used to determine  the
  //! classification.
  Standard_EXPORT Standard_Real EdgeParameter() const;
  
  //! Returns the  position of  the   point on the  edge
  //! returned by Edge.
    IntRes2d_Position Position() const;




protected:



  BRepClass_FClass2dOfFClassifier myClassifier;
  BRepClass_Edge myEdge;
  Standard_Real myEdgeParameter;
  IntRes2d_Position myPosition;
  Standard_Boolean rejected;
  Standard_Boolean nowires;


private:





};

#define TheFaceExplorer BRepClass_FaceExplorer
#define TheFaceExplorer_hxx <BRepClass_FaceExplorer.hxx>
#define TheEdge BRepClass_Edge
#define TheEdge_hxx <BRepClass_Edge.hxx>
#define TheIntersection2d BRepClass_Intersector
#define TheIntersection2d_hxx <BRepClass_Intersector.hxx>
#define TopClass_FClass2d BRepClass_FClass2dOfFClassifier
#define TopClass_FClass2d_hxx <BRepClass_FClass2dOfFClassifier.hxx>
#define TopClass_FaceClassifier BRepClass_FClassifier
#define TopClass_FaceClassifier_hxx <BRepClass_FClassifier.hxx>

#include <TopClass_FaceClassifier.lxx>

#undef TheFaceExplorer
#undef TheFaceExplorer_hxx
#undef TheEdge
#undef TheEdge_hxx
#undef TheIntersection2d
#undef TheIntersection2d_hxx
#undef TopClass_FClass2d
#undef TopClass_FClass2d_hxx
#undef TopClass_FaceClassifier
#undef TopClass_FaceClassifier_hxx




#endif // _BRepClass_FClassifier_HeaderFile
