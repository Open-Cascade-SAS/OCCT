// Created on: 1995-04-24
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _ChFiDS_ChamfSpine_HeaderFile
#define _ChFiDS_ChamfSpine_HeaderFile

#include <Standard.hxx>

#include <Standard_Real.hxx>
#include <ChFiDS_ChamfMethod.hxx>
#include <ChFiDS_Spine.hxx>

class ChFiDS_ChamfSpine;
DEFINE_STANDARD_HANDLE(ChFiDS_ChamfSpine, ChFiDS_Spine)

//! Provides  data specific to chamfers
//! distances on  each  of faces.
class ChFiDS_ChamfSpine : public ChFiDS_Spine
{

public:
  Standard_EXPORT ChFiDS_ChamfSpine();

  Standard_EXPORT ChFiDS_ChamfSpine(const Standard_Real Tol);

  Standard_EXPORT void SetDist(const Standard_Real Dis);

  Standard_EXPORT void GetDist(Standard_Real& Dis) const;

  Standard_EXPORT void SetDists(const Standard_Real Dis1, const Standard_Real Dis2);

  Standard_EXPORT void Dists(Standard_Real& Dis1, Standard_Real& Dis2) const;

  Standard_EXPORT void GetDistAngle(Standard_Real& Dis, Standard_Real& Angle) const;

  Standard_EXPORT void SetDistAngle(const Standard_Real Dis, const Standard_Real Angle);

  Standard_EXPORT void SetMode(const ChFiDS_ChamfMode theMode);

  //! Return the method of chamfers used
  Standard_EXPORT ChFiDS_ChamfMethod IsChamfer() const;

  //! Return the mode of chamfers used
  // Standard_EXPORT ChFiDS_ChamfMode Mode() const;

  DEFINE_STANDARD_RTTIEXT(ChFiDS_ChamfSpine, ChFiDS_Spine)

protected:
private:
  Standard_Real d1;
  Standard_Real d2;
  // Standard_Boolean dison1;
  Standard_Real      angle;
  ChFiDS_ChamfMethod mChamf;
};

#endif // _ChFiDS_ChamfSpine_HeaderFile
