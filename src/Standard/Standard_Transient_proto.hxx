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

#ifndef _Standard_Transient_proto_HeaderFile
#define _Standard_Transient_proto_HeaderFile

#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <Standard.hxx>

class Handle_Standard_Transient;
class Standard_Type;
class Handle_Standard_Type;

//! Abstract class which forms the root of the entire 
//! Transient class hierarchy.

class Standard_Transient
{
    //---- uses the friend Standard_Transient class
    friend class Handle(Standard_Transient);

 public:
    
    DEFINE_STANDARD_ALLOC

    //! Empty constructor
    Standard_Transient() : count(0) {}

    //! Copy constructor -- does nothing
    Standard_Transient(const Standard_Transient&) : count(0) {}

    //! Assignment operator, needed to avoid copying reference counter
    Standard_Transient& operator= (const Standard_Transient&) { return *this; }

    //! Destructor must be virtual
    Standard_EXPORT virtual ~Standard_Transient();

    //! Memory deallocator for transient classes
    Standard_EXPORT virtual void Delete() const;

    //! Returns a type information object about this object.
    Standard_EXPORT virtual const Handle_Standard_Type& DynamicType() const;

    //! Returns a true value if this is an instance of Type.
    Standard_EXPORT Standard_Boolean IsInstance(const Handle_Standard_Type& theType) const;  

    //! Returns a true value if this is an instance of TypeName.
    Standard_EXPORT Standard_Boolean IsInstance(const Standard_CString theTypeName) const;  

    //! Returns true if this is an instance of Type or an
    //! instance of any class that inherits from Type.
    //! Note that multiple inheritance is not supported by OCCT RTTI mechanism.
    Standard_EXPORT Standard_Boolean IsKind(const Handle_Standard_Type& theType) const;

    //! Returns true if this is an instance of TypeName or an
    //! instance of any class that inherits from TypeName.
    //! Note that multiple inheritance is not supported by OCCT RTTI mechanism.
    Standard_EXPORT Standard_Boolean IsKind(const Standard_CString theTypeName) const;

    //! Returns a Handle which references this object.
    //! Must never be called to objects created in stack.
    Standard_EXPORT virtual Handle_Standard_Transient This() const;

    //! Get the reference counter of this object.
    Standard_EXPORT Standard_Integer GetRefCount() const { return count; }

 private:

   volatile Standard_Integer count;
};

Standard_EXPORT const Handle(Standard_Type)& STANDARD_TYPE(Standard_Transient);

#endif 
