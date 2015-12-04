// Created on: 1996-12-16
// Created by: Bruno DUMORTIER
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _BiTgte_HCurveOnEdge_HeaderFile
#define _BiTgte_HCurveOnEdge_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BiTgte_CurveOnEdge.hxx>
#include <Adaptor3d_HCurve.hxx>
class Standard_OutOfRange;
class Standard_NoSuchObject;
class Standard_DomainError;
class BiTgte_CurveOnEdge;
class Adaptor3d_Curve;


class BiTgte_HCurveOnEdge;
DEFINE_STANDARD_HANDLE(BiTgte_HCurveOnEdge, Adaptor3d_HCurve)


class BiTgte_HCurveOnEdge : public Adaptor3d_HCurve
{

public:

  
  //! Creates an empty GenHCurve.
  Standard_EXPORT BiTgte_HCurveOnEdge();
  
  //! Creates a GenHCurve from a Curve
  Standard_EXPORT BiTgte_HCurveOnEdge(const BiTgte_CurveOnEdge& C);
  
  //! Sets the field of the GenHCurve.
  Standard_EXPORT void Set (const BiTgte_CurveOnEdge& C);
  
  //! Returns the curve used to create the GenHCurve.
  //! This is redefined from HCurve, cannot be inline.
  Standard_EXPORT const Adaptor3d_Curve& Curve() const Standard_OVERRIDE;
  
  //! Returns the curve used to create the GenHCurve.
  //! This is redefined from HCurve, cannot be inline.
  Standard_EXPORT Adaptor3d_Curve& GetCurve() Standard_OVERRIDE;
  
  //! Returns the curve used to create the GenHCurve.
    BiTgte_CurveOnEdge& ChangeCurve();




  DEFINE_STANDARD_RTTI_INLINE(BiTgte_HCurveOnEdge,Adaptor3d_HCurve)

protected:


  BiTgte_CurveOnEdge myCurve;


private:




};

#define TheCurve BiTgte_CurveOnEdge
#define TheCurve_hxx <BiTgte_CurveOnEdge.hxx>
#define Adaptor3d_GenHCurve BiTgte_HCurveOnEdge
#define Adaptor3d_GenHCurve_hxx <BiTgte_HCurveOnEdge.hxx>
#define Handle_Adaptor3d_GenHCurve Handle(BiTgte_HCurveOnEdge)

#include <Adaptor3d_GenHCurve.lxx>

#undef TheCurve
#undef TheCurve_hxx
#undef Adaptor3d_GenHCurve
#undef Adaptor3d_GenHCurve_hxx
#undef Handle_Adaptor3d_GenHCurve




#endif // _BiTgte_HCurveOnEdge_HeaderFile
