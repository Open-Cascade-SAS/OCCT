// Created on: 2016-04-07
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

#ifndef _BRepMeshData_Curve_HeaderFile
#define _BRepMeshData_Curve_HeaderFile

#include <IMeshData_Curve.hxx>
#include <Standard_Type.hxx>
#include <NCollection_IncAllocator.hxx>
#include <IMeshData_Types.hxx>

//! Default implementation of curve data model entity.
class BRepMeshData_Curve : public IMeshData_Curve
{
public:
  DEFINE_INC_ALLOC

  //! Constructor.
  Standard_EXPORT BRepMeshData_Curve(const occ::handle<NCollection_IncAllocator>& theAllocator);

  //! Destructor.
  Standard_EXPORT virtual ~BRepMeshData_Curve();

  //! Inserts new discretization point at the given position.
  Standard_EXPORT virtual void InsertPoint(const int thePosition,
                                           const gp_Pnt&          thePoint,
                                           const double theParamOnPCurve) override;

  //! Adds new discretization point to pcurve.
  Standard_EXPORT virtual void AddPoint(const gp_Pnt&       thePoint,
                                        const double theParamOnCurve) override;

  //! Returns discretization point with the given index.
  Standard_EXPORT virtual gp_Pnt& GetPoint(const int theIndex) override;

  //! Removes point with the given index.
  Standard_EXPORT virtual void RemovePoint(const int theIndex) override;

  //! Returns parameter with the given index.
  Standard_EXPORT virtual double& GetParameter(const int theIndex)
    override;

  //! Returns number of parameters stored in curve.
  Standard_EXPORT virtual int ParametersNb() const override;

  //! Clears parameters list.
  Standard_EXPORT virtual void Clear(const bool isKeepEndPoints) override;

  DEFINE_STANDARD_RTTIEXT(BRepMeshData_Curve, IMeshData_Curve)

protected:
  //! Removes parameter with the given index.
  Standard_EXPORT virtual void removeParameter(const int theIndex) override;

private:
  IMeshData::Model::SequenceOfPnt  myPoints;
  IMeshData::Model::SequenceOfReal myParameters;
};

#endif