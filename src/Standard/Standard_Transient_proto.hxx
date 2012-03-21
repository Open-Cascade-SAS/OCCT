// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef _Standard_Transient_proto_HeaderFile
#define _Standard_Transient_proto_HeaderFile

#ifndef _Standard_DefineAlloc_HeaderFile
#include <Standard_DefineAlloc.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif
#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif

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

    Standard_EXPORT virtual void ShallowDump(Standard_OStream& ) const;
    
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
