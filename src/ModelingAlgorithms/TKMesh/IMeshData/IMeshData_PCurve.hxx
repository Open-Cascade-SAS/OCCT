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

#ifndef _IMeshData_PCurve_HeaderFile
#define _IMeshData_PCurve_HeaderFile

#include <IMeshData_ParametersList.hxx>
#include <Standard_Type.hxx>
#include <IMeshData_Face.hxx>

class gp_Pnt2d;

//! Interface class representing pcurve of edge associated with discrete face.
//! Indexation of points starts from zero.
class IMeshData_PCurve : public IMeshData_ParametersList
{
public:
  //! Destructor.
  virtual ~IMeshData_PCurve() {}

  //! Inserts new discretization point at the given position.
  Standard_EXPORT virtual void InsertPoint(const Standard_Integer thePosition,
                                           const gp_Pnt2d&        thePoint,
                                           const Standard_Real    theParamOnPCurve) = 0;

  //! Adds new discretization point to pcurve.
  Standard_EXPORT virtual void AddPoint(const gp_Pnt2d&     thePoint,
                                        const Standard_Real theParamOnPCurve) = 0;

  //! Returns discretization point with the given index.
  Standard_EXPORT virtual gp_Pnt2d& GetPoint(const Standard_Integer theIndex) = 0;

  //! Returns index in mesh corresponded to discretization point with the given index.
  Standard_EXPORT virtual Standard_Integer& GetIndex(const Standard_Integer theIndex) = 0;

  //! Removes point with the given index.
  Standard_EXPORT virtual void RemovePoint(const Standard_Integer theIndex) = 0;

  //! Returns forward flag of this pcurve.
  Standard_Boolean IsForward() const { return (myOrientation != TopAbs_REVERSED); }

  //! Returns internal flag of this pcurve.
  Standard_Boolean IsInternal() const { return (myOrientation == TopAbs_INTERNAL); }

  //! Returns orientation of the edge associated with current pcurve.
  TopAbs_Orientation GetOrientation() const { return myOrientation; }

  //! Returns discrete face pcurve is associated to.
  const IMeshData::IFacePtr& GetFace() const { return myDFace; }

  DEFINE_STANDARD_RTTIEXT(IMeshData_PCurve, IMeshData_ParametersList)

protected:
  //! Constructor.
  IMeshData_PCurve(const IMeshData::IFacePtr& theDFace, const TopAbs_Orientation theOrientation)
      : myDFace(theDFace),
        myOrientation(theOrientation)
  {
  }

private:
  IMeshData::IFacePtr myDFace;
  TopAbs_Orientation  myOrientation;
};

#endif