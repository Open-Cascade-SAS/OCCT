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

//============================================================================
// Title : Handle_Standard_Persistent.hxx 
// Role  : This file just include <Standard_Persistent.hxx>
//============================================================================

#ifndef _Handle_Standard_Persistent_HeaderFile
#define _Handle_Standard_Persistent_HeaderFile

#include <Standard_DefineAlloc.hxx>
#include <Standard_TypeDef.hxx>
#include <Standard_Persistent_proto.hxx>

#ifdef _WIN32
// Disable the warning "conversion from 'unsigned int' to Standard_Persistent *"
#pragma warning (push)
#pragma warning (disable:4312)
#endif

class Standard_Persistent;
class Handle_Standard_Type;
class Handle_Standard_Persistent;

Standard_EXPORT Standard_Integer HashCode(const Handle(Standard_Persistent)& ,
                                          const Standard_Integer);

class Handle(Standard_Persistent)
 {
   private:

    Standard_Persistent *entity;

    Standard_EXPORT void RaiseNullObject(const Standard_CString S) const;

    void BeginScope() const
      {
       if (entity != 0) entity->count++;
      }    

    void EndScope()
      {
       if (entity != 0) 
         {
          entity->count--;
          if (entity->count == 0) {
	    entity->Delete();
	    entity = 0 ;
	  }
	}
      }


   public:

    DEFINE_STANDARD_ALLOC

    Handle(Standard_Persistent)()
      {
       entity = 0 ;
      }

    Handle(Standard_Persistent)(const Handle(Standard_Persistent)& aTid) 
      {
       entity = aTid.entity;
       BeginScope();
      } 

    Handle(Standard_Persistent)(const Standard_Persistent *anItem)
      {
       if (!anItem)
           entity = 0 ;
       else {
	 entity = (Standard_Persistent *)anItem;
	 BeginScope();
       }
      }

     Standard_EXPORT void Dump(Standard_OStream& out) const;

    Standard_EXPORT ~Handle(Standard_Persistent)();

    bool operator==(const Handle(Standard_Persistent)& right) const
      {
       return entity == right.entity;
      }

    bool operator==(const Standard_Persistent *right) const
      {
       return entity == right;
      }

    friend bool operator==(const Standard_Persistent *left, const Handle(Standard_Persistent)& right)
      {
       return left == right.entity;
      }

    bool operator!=(const Handle(Standard_Persistent)& right) const
      {
       return entity != right.entity;
      }

    bool operator!=(const Standard_Persistent *right) const
      {
       return entity != right;
      }

    friend bool operator!=(const Standard_Persistent *left, const Handle(Standard_Persistent)& right)
      {
       return left != right.entity;
      }

    void Nullify()
      {
       EndScope();
       entity =  0 ;
      }

    Standard_Boolean IsNull() const
      {
       return entity == 0 ;
      } 

    Standard_Persistent* Access() const
      {
       return entity;
      } 

   protected:

    Standard_Persistent* ControlAccess() const
      {
       return entity;
      } 

    void Assign(const Standard_Persistent *anItem)
      {
       EndScope();
       if (!anItem)
           entity = 0 ;
       else {
	 entity = (Standard_Persistent *)anItem;
	 BeginScope();
       }
      }


  public:

   operator Standard_Persistent*()
     {
       return Access();
     }


   Standard_EXPORT Handle(Standard_Persistent)& operator=(const Handle(Standard_Persistent)& aHandle);
   Standard_EXPORT Handle(Standard_Persistent)& operator=(const Standard_Persistent* anItem);

   Standard_Persistent* operator->() 
     {
      return ControlAccess();
     }

   Standard_Persistent* operator->() const
     {
      return ControlAccess();
     }

   Standard_Persistent& operator*()
     {
      return *(ControlAccess());
     }

   const Standard_Persistent& operator*() const
     {
      return *(ControlAccess());
     }

   Standard_EXPORT static const Handle(Standard_Persistent) DownCast(const Handle(Standard_Persistent)& AnObject);
};

class Standard_Type;

#ifdef _WIN32
#pragma warning (pop)
#endif

#endif
