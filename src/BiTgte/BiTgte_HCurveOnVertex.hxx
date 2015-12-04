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

#ifndef _BiTgte_HCurveOnVertex_HeaderFile
#define _BiTgte_HCurveOnVertex_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BiTgte_CurveOnVertex.hxx>
#include <Adaptor3d_HCurve.hxx>
class Standard_OutOfRange;
class Standard_NoSuchObject;
class Standard_DomainError;
class BiTgte_CurveOnVertex;
class Adaptor3d_Curve;


class BiTgte_HCurveOnVertex;
DEFINE_STANDARD_HANDLE(BiTgte_HCurveOnVertex, Adaptor3d_HCurve)


class BiTgte_HCurveOnVertex : public Adaptor3d_HCurve
{

public:

  
  //! Creates an empty GenHCurve.
  Standard_EXPORT BiTgte_HCurveOnVertex();
  
  //! Creates a GenHCurve from a Curve
  Standard_EXPORT BiTgte_HCurveOnVertex(const BiTgte_CurveOnVertex& C);
  
  //! Sets the field of the GenHCurve.
  Standard_EXPORT void Set (const BiTgte_CurveOnVertex& C);
  
  //! Returns the curve used to create the GenHCurve.
  //! This is redefined from HCurve, cannot be inline.
  Standard_EXPORT const Adaptor3d_Curve& Curve() const Standard_OVERRIDE;
  
  //! Returns the curve used to create the GenHCurve.
  //! This is redefined from HCurve, cannot be inline.
  Standard_EXPORT Adaptor3d_Curve& GetCurve() Standard_OVERRIDE;
  
  //! Returns the curve used to create the GenHCurve.
    BiTgte_CurveOnVertex& ChangeCurve();




  DEFINE_STANDARD_RTTI_INLINE(BiTgte_HCurveOnVertex,Adaptor3d_HCurve)

protected:


  BiTgte_CurveOnVertex myCurve;


private:




};

#define TheCurve BiTgte_CurveOnVertex
#define TheCurve_hxx <BiTgte_CurveOnVertex.hxx>
#define Adaptor3d_GenHCurve BiTgte_HCurveOnVertex
#define Adaptor3d_GenHCurve_hxx <BiTgte_HCurveOnVertex.hxx>
#define Handle_Adaptor3d_GenHCurve Handle(BiTgte_HCurveOnVertex)

#include <Adaptor3d_GenHCurve.lxx>

#undef TheCurve
#undef TheCurve_hxx
#undef Adaptor3d_GenHCurve
#undef Adaptor3d_GenHCurve_hxx
#undef Handle_Adaptor3d_GenHCurve




#endif // _BiTgte_HCurveOnVertex_HeaderFile
