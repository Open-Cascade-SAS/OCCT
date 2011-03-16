//============================================================================
//
// Title : Handle_Standard_Persistent.hxx 
// Role  : This file just include <Standard_Persistent.hxx>
//
//============================================================================

#ifndef _Handle_Standard_Persistent_HeaderFile
#define _Handle_Standard_Persistent_HeaderFile

#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif
#ifndef _Standard_PrimitiveTypes_HeaderFile
#include <Standard_PrimitiveTypes.hxx>
#endif
#ifndef _Standard_Persistent_proto_HeaderFile
#include <Standard_Persistent_proto.hxx>
#endif

#ifdef _WIN32
// Disable the warning "conversion from 'unsigned int' to Standard_Persistent *"
#pragma warning (push)
#pragma warning (disable:4312)
#endif

#ifndef PUndefinedAddress 
#ifdef _OCC64
#define PUndefinedAddress ((Standard_Persistent *)0xfefdfefdfefd0000)
#else
#define PUndefinedAddress ((Standard_Persistent *)0xfefd0000)
#endif
#endif

class Standard_Persistent;
class Handle_Standard_Type;
class Handle_Standard_Persistent;

Standard_EXPORT Standard_Integer HashCode(const Handle(Standard_Persistent)& ,
                                          const Standard_Integer);
Standard_EXPORT Standard_Address StandardCSFDB_Allocate(const Standard_Size);
Standard_EXPORT void StandardCSFDB_Free(Standard_Address&);

class Handle(Standard_Persistent)
 {
   private:

    Standard_Persistent *entity;

    Standard_EXPORT void RaiseNullObject(const Standard_CString S) const;

    void BeginScope() const
      {
       if (entity != PUndefinedAddress) entity->count++;
      }    

    void EndScope()
      {
       if (entity != PUndefinedAddress) 
         {
          entity->count--;
          if (entity->count == 0) {
	    entity->Delete();
	    entity = PUndefinedAddress ;
	  }
	}
      }


   public:

    void* operator new(size_t,void* anAddress) 
      {
        return anAddress;
      }
    void* operator new(size_t size) 
      { 
        return StandardCSFDB_Allocate(size); 
      }
    void  operator delete(void *anAddress) 
      { 
        if (anAddress) StandardCSFDB_Free(anAddress); 
      }

    Handle(Standard_Persistent)()
      {
       entity = PUndefinedAddress ;
      }

    Handle(Standard_Persistent)(const Handle(Standard_Persistent)& aTid) 
      {
       entity = aTid.entity;
       BeginScope();
      } 

    Handle(Standard_Persistent)(const Standard_Persistent *anItem)
      {
       if (!anItem)
           entity = PUndefinedAddress ;
       else {
	 entity = (Standard_Persistent *)anItem;
	 BeginScope();
       }
      }

     Standard_EXPORT void Dump(Standard_OStream& out) const;

    Standard_EXPORT ~Handle(Standard_Persistent)();

     Standard_EXPORT void  ShallowDump(Standard_OStream&) const;
     
    int operator==(const Handle(Standard_Persistent)& right) const
      {
       return entity == right.entity;
      }

    int operator==(const Standard_Persistent *right) const
      {
       return entity == right;
      }

    friend int operator==(const Standard_Persistent *left, const Handle(Standard_Persistent)& right)
      {
       return left == right.entity;
      }

    int operator!=(const Handle(Standard_Persistent)& right) const
      {
       return entity != right.entity;
      }

    int operator!=(const Standard_Persistent *right) const
      {
       return entity != right;
      }

    friend int operator!=(const Standard_Persistent *left, const Handle(Standard_Persistent)& right)
      {
       return left != right.entity;
      }

    void Nullify()
      {
       EndScope();
       entity =  PUndefinedAddress ;
      }

    Standard_Boolean IsNull() const
      {
       return entity == PUndefinedAddress ;
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
           entity = PUndefinedAddress ;
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
