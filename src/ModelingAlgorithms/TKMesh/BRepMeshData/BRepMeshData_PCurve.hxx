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

#ifndef _BRepMeshData_PCurve_HeaderFile
#define _BRepMeshData_PCurve_HeaderFile

#include <IMeshData_PCurve.hxx>
#include <Standard_Type.hxx>
#include <NCollection_IncAllocator.hxx>

//! Default implementation of pcurve data model entity.
class BRepMeshData_PCurve : public IMeshData_PCurve
{
public:
  DEFINE_INC_ALLOC

  //! Constructor.
  Standard_EXPORT BRepMeshData_PCurve(const IMeshData::IFacePtr&              theDFace,
                                      const TopAbs_Orientation                theOrientation,
                                      const Handle(NCollection_IncAllocator)& theAllocator);

  //! Destructor.
  Standard_EXPORT virtual ~BRepMeshData_PCurve();

  //! Inserts new discretization point at the given position.
  Standard_EXPORT virtual void InsertPoint(const Standard_Integer thePosition,
                                           const gp_Pnt2d&        thePoint,
                                           const Standard_Real theParamOnPCurve) Standard_OVERRIDE;

  //! Adds new discretization point to pcurve.
  Standard_EXPORT virtual void AddPoint(const gp_Pnt2d&     thePoint,
                                        const Standard_Real theParamOnPCurve) Standard_OVERRIDE;

  //! Returns discretization point with the given index.
  Standard_EXPORT virtual gp_Pnt2d& GetPoint(const Standard_Integer theIndex) Standard_OVERRIDE;

  //! Returns index in mesh corresponded to discretization point with the given index.
  Standard_EXPORT virtual Standard_Integer& GetIndex(const Standard_Integer theIndex)
    Standard_OVERRIDE;

  //! Removes point with the given index.
  Standard_EXPORT virtual void RemovePoint(const Standard_Integer theIndex) Standard_OVERRIDE;

  //! Returns parameter with the given index.
  Standard_EXPORT virtual Standard_Real& GetParameter(const Standard_Integer theIndex)
    Standard_OVERRIDE;

  //! Returns number of parameters stored in pcurve.
  Standard_EXPORT virtual Standard_Integer ParametersNb() const Standard_OVERRIDE;

  //! Clears parameters list.
  Standard_EXPORT virtual void Clear(const Standard_Boolean isKeepEndPoints) Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(BRepMeshData_PCurve, IMeshData_PCurve)

protected:
  //! Removes parameter with the given index.
  Standard_EXPORT virtual void removeParameter(const Standard_Integer theIndex) Standard_OVERRIDE;

private:
  IMeshData::Model::SequenceOfPnt2d   myPoints2d;
  IMeshData::Model::SequenceOfReal    myParameters;
  IMeshData::Model::SequenceOfInteger myIndices;
};

#endif