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

#ifndef _IMeshData_Curve_HeaderFile
#define _IMeshData_Curve_HeaderFile

#include <IMeshData_ParametersList.hxx>
#include <Standard_Type.hxx>

class gp_Pnt;

//! Interface class representing discrete 3d curve of edge.
//! Indexation of points starts from zero.
class IMeshData_Curve : public IMeshData_ParametersList
{
public:

  //! Destructor.
  virtual ~IMeshData_Curve()
  {
  }

  //! Inserts new discretization point at the given position.
  Standard_EXPORT virtual void InsertPoint(
    const Standard_Integer thePosition,
    const gp_Pnt&          thePoint,
    const Standard_Real    theParamOnPCurve) = 0;

  //! Adds new discretization point to curve.
  Standard_EXPORT virtual void AddPoint (
    const gp_Pnt&       thePoint,
    const Standard_Real theParamOnCurve) = 0;

  //! Returns discretization point with the given index.
  Standard_EXPORT virtual gp_Pnt& GetPoint (const Standard_Integer theIndex) = 0;

  //! Removes point with the given index.
  Standard_EXPORT virtual void RemovePoint (const Standard_Integer theIndex) = 0;

  DEFINE_STANDARD_RTTIEXT(IMeshData_Curve, IMeshData_ParametersList)

protected:

  //! Constructor.
  IMeshData_Curve()
  {
  }
};

#endif