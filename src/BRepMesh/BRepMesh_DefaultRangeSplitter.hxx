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

#ifndef _BRepMesh_DefaultRangeSplitter_HeaderFile
#define _BRepMesh_DefaultRangeSplitter_HeaderFile

#include <IMeshData_Face.hxx>

struct IMeshTools_Parameters;

//! Default tool to define range of discrete face model and
//! obtain grid points distributed within this range.
class BRepMesh_DefaultRangeSplitter
{
public:
  //! Constructor.
  BRepMesh_DefaultRangeSplitter()
      : myIsValid(Standard_True)
  {
  }

  //! Destructor.
  virtual ~BRepMesh_DefaultRangeSplitter() {}

  //! Resets this splitter. Must be called before first use.
  Standard_EXPORT virtual void Reset(const IMeshData::IFaceHandle& theDFace,
                                     const IMeshTools_Parameters&  theParameters);

  //! Registers border point.
  Standard_EXPORT virtual void AddPoint(const gp_Pnt2d& thePoint);

  //! Updates discrete range of surface according to its geometric range.
  Standard_EXPORT virtual void AdjustRange();

  //! Returns True if computed range is valid.
  Standard_EXPORT virtual Standard_Boolean IsValid();

  //! Scales the given point from real parametric space
  //! to face basis and otherwise.
  //! @param thePoint point to be scaled.
  //! @param isToFaceBasis if TRUE converts point to face basis,
  //! otherwise performs reverse conversion.
  //! @return scaled point.
  Standard_EXPORT gp_Pnt2d Scale(const gp_Pnt2d&        thePoint,
                                 const Standard_Boolean isToFaceBasis) const;

  //! Returns list of nodes generated using surface data and specified parameters.
  //! By default returns null ptr.
  Standard_EXPORT virtual Handle(IMeshData::ListOfPnt2d) GenerateSurfaceNodes(
    const IMeshTools_Parameters& theParameters) const;

  //! Returns point in 3d space corresponded to the given
  //! point defined in parametric space of surface.
  gp_Pnt Point(const gp_Pnt2d& thePoint2d) const
  {
    return GetSurface()->Value(thePoint2d.X(), thePoint2d.Y());
  }

protected:
  //! Computes parametric tolerance taking length along U and V into account.
  Standard_EXPORT virtual void computeTolerance(const Standard_Real theLenU,
                                                const Standard_Real theLenV);

  //! Computes parametric delta taking length along U and V and value of tolerance into account.
  Standard_EXPORT virtual void computeDelta(const Standard_Real theLengthU,
                                            const Standard_Real theLengthV);

public:
  //! Returns face model.
  const IMeshData::IFaceHandle& GetDFace() const { return myDFace; }

  //! Returns surface.
  const Handle(BRepAdaptor_Surface)& GetSurface() const { return myDFace->GetSurface(); }

  //! Returns U range.
  const std::pair<Standard_Real, Standard_Real>& GetRangeU() const { return myRangeU; }

  //! Returns V range.
  const std::pair<Standard_Real, Standard_Real>& GetRangeV() const { return myRangeV; }

  //! Returns delta.
  const std::pair<Standard_Real, Standard_Real>& GetDelta() const { return myDelta; }

  const std::pair<Standard_Real, Standard_Real>& GetToleranceUV() const { return myTolerance; }

private:
  //! Computes length along U direction.
  Standard_Real computeLengthU();

  //! Computes length along V direction.
  Standard_Real computeLengthV();

  //! Updates discrete range of surface according to its geometric range.
  void updateRange(const Standard_Real    theGeomFirst,
                   const Standard_Real    theGeomLast,
                   const Standard_Boolean isPeriodic,
                   Standard_Real&         theDiscreteFirst,
                   Standard_Real&         theDiscreteLast);

protected:
  IMeshData::IFaceHandle                  myDFace;
  std::pair<Standard_Real, Standard_Real> myRangeU;
  std::pair<Standard_Real, Standard_Real> myRangeV;
  std::pair<Standard_Real, Standard_Real> myDelta;
  std::pair<Standard_Real, Standard_Real> myTolerance;
  Standard_Boolean                        myIsValid;
};

#endif