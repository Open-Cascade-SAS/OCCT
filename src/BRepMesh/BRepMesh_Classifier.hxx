// Created on: 2014-06-03
// Created by: Oleg AGASHIN
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _BRepMesh_Classifier_HeaderFile
#define _BRepMesh_Classifier_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <BRepTopAdaptor_SeqOfPtr.hxx>
#include <TColStd_SequenceOfBoolean.hxx>
#include <TopAbs_State.hxx>
#include <NCollection_Sequence.hxx>
#include <gp_Pnt2d.hxx>

//! Auxilary class contains information about correctness of discretized 
//! face and used for classification of points regarding face internals.
class BRepMesh_Classifier
{
public:

  DEFINE_STANDARD_ALLOC

  //! Constructor.
  Standard_EXPORT BRepMesh_Classifier();

  //! Destructor.
  Standard_EXPORT virtual ~BRepMesh_Classifier()
  {
    Destroy();
  }

  //! Method is called on destruction.
  //! Clears internal data structures.
  Standard_EXPORT void Destroy();
  
  //! Performs classification of the given point regarding to face internals.
  //! @param thePoint Point in parametric space to be classified.
  //! @return 
  Standard_EXPORT TopAbs_State Perform(const gp_Pnt2d& thePoint) const;

  //! Registers wire specified by sequence of points for 
  //! further classification of points.
  //! @param theWire Wire to be registered. Specified by sequence of points.
  //! @param theTolUV Tolerance to be used for calculations in parametric space.
  //! @param theUmin Lower U boundary of the face in parametric space.
  //! @param theUmax Upper U boundary of the face in parametric space.
  //! @param theVmin Lower V boundary of the face in parametric space.
  //! @param theVmax Upper V boundary of the face in parametric space.
  Standard_EXPORT void RegisterWire(
    const NCollection_Sequence<gp_Pnt2d>& theWire,
    const Standard_Real                   theTolUV,
    const Standard_Real                   theUmin,
    const Standard_Real                   theUmax,
    const Standard_Real                   theVmin,
    const Standard_Real                   theVmax);

private:

  BRepTopAdaptor_SeqOfPtr       myTabClass;
  TColStd_SequenceOfBoolean     myTabOrient;
};

#endif
