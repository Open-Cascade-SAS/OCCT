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

#ifndef _Handle_Standard_Transient_HeaderFile
#define _Handle_Standard_Transient_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_PrimitiveTypes.hxx>
#include <Standard_Transient_proto.hxx>

#ifdef _WIN32
// Disable the warning "conversion from 'unsigned int' to Standard_Transient *"
#pragma warning (push)
#pragma warning (disable:4312)
#endif

class Handle_Standard_Transient;

Standard_EXPORT Standard_Integer HashCode(const Handle(Standard_Transient)& ,const Standard_Integer);

/** 
 * Base class for hierarchy of smart pointers (Handles) for Transient
 * objects. Uses reference counting technique to control life time
 * of the referred object.
 *
 * Note that Handle should never be initialized by pointer to object
 * created in the stack; only dinamically allocated pointers shall be used. 
 */
     
class Handle(Standard_Transient)
{
public:
  
  // Public methods
  
  //! Empty constructor
  Handle(Standard_Transient) () 
    : entity(0) 
  {
  }

  //! Constructor from pointer to new object
  Handle(Standard_Transient) (const Standard_Transient *anItem)
    : entity ( (Standard_Transient*)anItem )
  {
    BeginScope();
  }

  //! Copy constructor
  Handle(Standard_Transient) (const Handle(Standard_Transient)& aTid) 
    : entity ( aTid.entity )
  {
    BeginScope();
  } 

  //! Destructor
  ~Handle(Standard_Transient)()
  {
    EndScope();
  }

  //! Assignment operator
  Handle(Standard_Transient)& operator=(const Handle(Standard_Transient)& aHandle)
  {
    Assign(aHandle.Access());
    return *this;
  }

  //! Assignment operator
  Handle(Standard_Transient)& operator=(const Standard_Transient* anItem)
  {
    Assign(anItem);
    return *this;
  }

  //! Nullify the handle
  void Nullify()
  {
    EndScope();
  }

  //! Check for being null
  Standard_Boolean IsNull() const
  {
    return entity == 0;
  } 

  //! Returns pointer to referred object
  Standard_Transient* Access()
  {
    return entity;
  } 
  
  //! Returns const pointer to referred object
  const Standard_Transient* Access() const
  {
    return entity;
  } 

  //! Cast to pointer to referred object
  operator Standard_Transient*()
  {
    return entity;
  }

  //! Cast to const pointer to referred object
  operator const Standard_Transient*() const
  {
    return entity;
  }

  //! Member access operator (note non-const)
  Standard_Transient* operator->() const
  {
    return entity;
  }

  //! Dereferencing operator
  Standard_Transient& operator*()
  {
    return *entity;
  }

  //! Const dereferencing operator
  const Standard_Transient& operator*() const
  {
    return *entity;
  }

  //! Check for equality
  int operator==(const Handle(Standard_Transient)& right) const
  {
    return entity == right.entity;
  }

  //! Check for equality
  int operator==(const Standard_Transient *right) const
  {
    return entity == right;
  }

  //! Check for equality
  friend bool operator==(const Standard_Transient *left, const Handle(Standard_Transient)& right)
  {
    return left == right.entity;
  }

  //! Check for inequality
  bool operator!=(const Handle(Standard_Transient)& right) const
  {
    return entity != right.entity;
  }

  //! Check for inequality
  bool operator!=(const Standard_Transient *right) const
  {
    return entity != right;
  }

  //! Check for inequality
  friend bool operator!=(const Standard_Transient *left, const Handle(Standard_Transient)& right)
  {
    return left != right.entity;
  }

  //! Down casting operator; dummy provided for consistency with other classes
  //! (descendants)
  static const Handle(Standard_Transient)& DownCast(const Handle(Standard_Transient)& AnObject)
  { 
    return AnObject;
  }

  //! Dump pointer to a referred object to a stream
  Standard_EXPORT void Dump(Standard_OStream& out) const;

protected:
  // Protected methods for descendants

  //! Returns non-const pointer to referred object
  Standard_Transient* ControlAccess() const
  {
    return entity;
  } 

  //! Assignment
  Standard_EXPORT void Assign (const Standard_Transient *anItem);
  
private:
  // Private methods

  //! Increment reference counter of referred object 
  Standard_EXPORT void BeginScope();

  //! Decrement reference counter and if 0, destroy referred object
  Standard_EXPORT void EndScope();

public:

  DEFINE_STANDARD_ALLOC

private:
  // Field
  Standard_Transient *entity;
};

//! Function in global scope to check handles for equality.
//! Will be used with standard OCCT collections like NCollection_DataMap within NCollection_DefaultHasher
//! when there are no specialization defined for concrete type.
//! Notice that this implementation compares only pointers to objects!
inline Standard_Boolean IsEqual (const Handle(Standard_Transient)& theFirst,
                                 const Handle(Standard_Transient)& theSecond)
{
  return theFirst == theSecond;
}

#ifdef _WIN32
#pragma warning (pop)
#endif

#endif
