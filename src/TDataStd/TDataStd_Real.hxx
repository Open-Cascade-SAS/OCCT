// Created on: 1997-02-06
// Created by: Denis PASCAL
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _TDataStd_Real_HeaderFile
#define _TDataStd_Real_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <TDataStd_RealEnum.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_OStream.hxx>
#include <Standard_GUID.hxx>

class TDF_Label;
class TDF_Attribute;
class TDF_RelocationTable;


class TDataStd_Real;
DEFINE_STANDARD_HANDLE(TDataStd_Real, TDF_Attribute)

//! The basis to define a real number attribute.
class TDataStd_Real : public TDF_Attribute
{

public:

  
  //! class methods
  //! =============
  //! Returns the default GUID for real numbers.
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! Finds, or creates, a Real attribute with default GUID and sets <value>.
  //! The Real attribute  is  returned. The  Real  dimension is Scalar by default.
  //! Use SetDimension to overwrite.
  //! Real methods
  //! ============
  Standard_EXPORT static Handle(TDataStd_Real) Set (const TDF_Label& label, const Standard_Real value);
  
  //! Finds, or creates, a Real attribute with explicit GUID and sets <value>.
  //! The Real attribute  is  returned. 
  //! Real methods
  //! ============
  Standard_EXPORT static Handle(TDataStd_Real) Set (const TDF_Label& label, const Standard_GUID& guid,
	                            const Standard_Real value);

  Standard_EXPORT TDataStd_Real();
  
  //! Obsolete method that will be removed in next versions.
  //! This field is not supported in the persistence mechanism.
  Standard_DEPRECATED("TDataStd_Real::SetDimension() is deprecated. Please avoid usage of this method.")
  Standard_EXPORT void SetDimension (const TDataStd_RealEnum DIM);
  
  //! Obsolete method that will be removed in next versions.
  //! This field is not supported in the persistence mechanism.
  Standard_DEPRECATED("TDataStd_Real::GetDimension() is deprecated. Please avoid usage of this method.")
  Standard_EXPORT TDataStd_RealEnum GetDimension() const;
  

  //! Sets the real number V.
  Standard_EXPORT void Set (const Standard_Real V);
  
  //! Sets the explicit GUID for the attribute.
  Standard_EXPORT void SetID (const Standard_GUID& guid) Standard_OVERRIDE;

  //! Sets default GUID for the attribute.
  Standard_EXPORT void SetID() Standard_OVERRIDE;

  //! Returns the real number value contained in the attribute.
  Standard_EXPORT Standard_Real Get() const;
  
  //! Returns True if there is a reference on the same label
  Standard_EXPORT Standard_Boolean IsCaptured() const;
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& With) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& Into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(TDataStd_Real,TDF_Attribute)

protected:




private:


  Standard_Real myValue;
  //! An obsolete field that will be removed in next versions.
  TDataStd_RealEnum myDimension;
  Standard_GUID myID;

};







#endif // _TDataStd_Real_HeaderFile
