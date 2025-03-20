// Created on: 2016-07-07
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#include <IMeshData_Types.hxx>
#include <NCollection_Handle.hxx>

#include <memory>

class gp_Pnt2d;
class CSLib_Class2d;

//! Auxiliary class intended for classification of points
//! regarding internals of discrete face.
class BRepMesh_Classifier : public Standard_Transient
{
public:
  //! Constructor.
  Standard_EXPORT BRepMesh_Classifier();

  //! Destructor.
  Standard_EXPORT virtual ~BRepMesh_Classifier();

  //! Performs classification of the given point regarding to face internals.
  //! @param thePoint Point in parametric space to be classified.
  //! @return TopAbs_IN if point lies within face boundaries and TopAbs_OUT elsewhere.
  Standard_EXPORT TopAbs_State Perform(const gp_Pnt2d& thePoint) const;

  //! Registers wire specified by sequence of points for
  //! further classification of points.
  //! @param theWire Wire to be registered. Specified by sequence of points.
  //! @param theTolUV Tolerance to be used for calculations in parametric space.
  //! @param theUmin Lower U boundary of the face in parametric space.
  //! @param theUmax Upper U boundary of the face in parametric space.
  //! @param theVmin Lower V boundary of the face in parametric space.
  //! @param theVmax Upper V boundary of the face in parametric space.
  Standard_EXPORT void RegisterWire(const NCollection_Sequence<const gp_Pnt2d*>&   theWire,
                                    const std::pair<Standard_Real, Standard_Real>& theTolUV,
                                    const std::pair<Standard_Real, Standard_Real>& theRangeU,
                                    const std::pair<Standard_Real, Standard_Real>& theRangeV);

  DEFINE_STANDARD_RTTIEXT(BRepMesh_Classifier, Standard_Transient)

private:
  NCollection_Vector<NCollection_Handle<CSLib_Class2d>> myTabClass;
  IMeshData::VectorOfBoolean                            myTabOrient;
};

#endif
