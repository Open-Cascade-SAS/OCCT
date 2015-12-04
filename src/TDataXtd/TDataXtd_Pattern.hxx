// Created on: 2009-04-06
// Created by: Sergey ZARITCHNY
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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

#ifndef _TDataXtd_Pattern_HeaderFile
#define _TDataXtd_Pattern_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDF_Attribute.hxx>
#include <Standard_Integer.hxx>
#include <TDataXtd_Array1OfTrsf.hxx>
class Standard_GUID;


class TDataXtd_Pattern;
DEFINE_STANDARD_HANDLE(TDataXtd_Pattern, TDF_Attribute)

//! a general pattern model
class TDataXtd_Pattern : public TDF_Attribute
{

public:

  
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! Returns the ID of the attribute.
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  //! Returns the ID of the attribute.
  Standard_EXPORT virtual const Standard_GUID& PatternID() const = 0;
  
  //! Give the number of transformation
  Standard_EXPORT virtual Standard_Integer NbTrsfs() const = 0;
  
  //! Give the transformations
  Standard_EXPORT virtual void ComputeTrsfs (TDataXtd_Array1OfTrsf& Trsfs) const = 0;




  DEFINE_STANDARD_RTTIEXT(TDataXtd_Pattern,TDF_Attribute)

protected:




private:




};







#endif // _TDataXtd_Pattern_HeaderFile
