// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _DBC_BaseArray_HeaderFile
#define _DBC_BaseArray_HeaderFile

#include <DBC_DBVArray.hxx>
#include <Standard_Storable.hxx>
#include <Standard_OStream.hxx>
#include <Standard_TypeDef.hxx>

class Standard_NullObject;
class Standard_NegativeValue;
class Standard_DimensionMismatch;


#ifndef Standard_EXPORT
#if defined(WNT) && !defined(HAVE_NO_DLL)
#define Standard_EXPORT __declspec(dllexport)
#else
#define Standard_EXPORT
#endif
#endif

class Standard_EXPORT DBC_BaseArray :
  public os_virtual_behavior /* New */ {

public:

 // Methods PUBLIC
 // 
DBC_BaseArray();
DBC_BaseArray(const Standard_Integer Size);
DBC_BaseArray(const DBC_BaseArray& BaseArray);
  Standard_Integer Length() const;
  Standard_Integer Upper() const;
  Standard_Address Lock() const;
  void Unlock() const;
  void Destroy() ;
~DBC_BaseArray()
{
  Destroy();
}

protected:

 // Methods PROTECTED
 // 


 // Fields PROTECTED
 //
Standard_Integer mySize;
DBC_DBVArray myData;


private: 

 // Methods PRIVATE
 // 


 // Fields PRIVATE
 //


};


#include <DBC_BaseArray.lxx>

#endif
