// Created on: 1993-08-11
// Created by: Bruno DUMORTIER
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _ProjLib_HCompProjectedCurve_HeaderFile
#define _ProjLib_HCompProjectedCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <ProjLib_CompProjectedCurve.hxx>
#include <Adaptor2d_HCurve2d.hxx>
class Standard_OutOfRange;
class Standard_NoSuchObject;
class Standard_DomainError;
class ProjLib_CompProjectedCurve;
class Adaptor2d_Curve2d;


class ProjLib_HCompProjectedCurve;
DEFINE_STANDARD_HANDLE(ProjLib_HCompProjectedCurve, Adaptor2d_HCurve2d)


class ProjLib_HCompProjectedCurve : public Adaptor2d_HCurve2d
{

public:

  
  //! Creates an empty GenHCurve2d.
  Standard_EXPORT ProjLib_HCompProjectedCurve();
  
  //! Creates a GenHCurve2d from a Curve
  Standard_EXPORT ProjLib_HCompProjectedCurve(const ProjLib_CompProjectedCurve& C);
  
  //! Sets the field of the GenHCurve2d.
  Standard_EXPORT void Set (const ProjLib_CompProjectedCurve& C);
  
  //! Returns the curve used to create the GenHCurve2d.
  //! This is redefined from HCurve2d, cannot be inline.
  Standard_EXPORT const Adaptor2d_Curve2d& Curve2d() const Standard_OVERRIDE;
  
  //! Returns the curve used to create the GenHCurve.
    ProjLib_CompProjectedCurve& ChangeCurve2d();




  DEFINE_STANDARD_RTTI_INLINE(ProjLib_HCompProjectedCurve,Adaptor2d_HCurve2d)

protected:


  ProjLib_CompProjectedCurve myCurve;


private:




};

#define TheCurve ProjLib_CompProjectedCurve
#define TheCurve_hxx <ProjLib_CompProjectedCurve.hxx>
#define Adaptor2d_GenHCurve2d ProjLib_HCompProjectedCurve
#define Adaptor2d_GenHCurve2d_hxx <ProjLib_HCompProjectedCurve.hxx>
#define Handle_Adaptor2d_GenHCurve2d Handle(ProjLib_HCompProjectedCurve)

#include <Adaptor2d_GenHCurve2d.lxx>

#undef TheCurve
#undef TheCurve_hxx
#undef Adaptor2d_GenHCurve2d
#undef Adaptor2d_GenHCurve2d_hxx
#undef Handle_Adaptor2d_GenHCurve2d




#endif // _ProjLib_HCompProjectedCurve_HeaderFile
