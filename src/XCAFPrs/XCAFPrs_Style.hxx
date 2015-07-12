// Created on: 2000-08-11
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _XCAFPrs_Style_HeaderFile
#define _XCAFPrs_Style_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Integer.hxx>
class Quantity_Color;


//! Represents a set of styling settings applicable to
//! a (sub)shape
class XCAFPrs_Style 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT XCAFPrs_Style();
  
  Standard_EXPORT Standard_Boolean IsSetColorSurf() const;
  
  Standard_EXPORT Quantity_Color GetColorSurf() const;
  
  Standard_EXPORT void SetColorSurf (const Quantity_Color& col);
  
  //! Manage surface color setting
  Standard_EXPORT void UnSetColorSurf();
  
  Standard_EXPORT Standard_Boolean IsSetColorCurv() const;
  
  Standard_EXPORT Quantity_Color GetColorCurv() const;
  
  Standard_EXPORT void SetColorCurv (const Quantity_Color& col);
  
  //! Manage curve color setting
  Standard_EXPORT void UnSetColorCurv();
  
  Standard_EXPORT void SetVisibility (const Standard_Boolean visibility);
  
  //! Manage visibility
  //! Note: Setting visibility to False makes colors undefined
  //! This is necessary for HashCode
  Standard_EXPORT Standard_Boolean IsVisible() const;
  
  //! Returs True if styles are the same
  //! Methods for using Style as key in maps
  Standard_EXPORT Standard_Boolean IsEqual (const XCAFPrs_Style& other) const;
Standard_Boolean operator == (const XCAFPrs_Style& other) const
{
  return IsEqual(other);
}
  
  //! Returns a HasCode value  for  the  Key <K>  in the
  //! range 0..Upper.
  Standard_EXPORT static Standard_Integer HashCode (const XCAFPrs_Style& S, const Standard_Integer Upper);
  
  //! Returns True  when the two  keys are the same. Two
  //! same  keys  must   have  the  same  hashcode,  the
  //! contrary is not necessary.
  Standard_EXPORT static Standard_Boolean IsEqual (const XCAFPrs_Style& S1, const XCAFPrs_Style& S2);




protected:





private:



  Standard_Boolean defColorSurf;
  Standard_Boolean defColorCurv;
  Standard_Boolean myVisibility;
  Quantity_Color myColorSurf;
  Quantity_Color myColorCurv;


};







#endif // _XCAFPrs_Style_HeaderFile
