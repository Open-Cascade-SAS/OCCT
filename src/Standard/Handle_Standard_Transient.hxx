#ifndef _Handle_Standard_Transient_HeaderFile
#define _Handle_Standard_Transient_HeaderFile
#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif
#ifndef _Standard_PrimitiveTypes_HeaderFile
#include <Standard_PrimitiveTypes.hxx>
#endif
#ifndef _Standard_Transient_proto_HeaderFile
#include <Standard_Transient_proto.hxx>
#endif

#ifdef _WIN32
// Disable the warning "conversion from 'unsigned int' to Standard_Transient *"
#pragma warning (push)
#pragma warning (disable:4312)
#endif

#ifndef UndefinedHandleAddress 
#ifdef _OCC64
#define UndefinedHandleAddress ((Standard_Transient *)0xfefdfefdfefd0000)
#else
#define UndefinedHandleAddress ((Standard_Transient *)0xfefd0000)
#endif
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
    : entity(UndefinedHandleAddress) 
  {
  }

  //! Constructor from pointer to new object
  Handle(Standard_Transient) (const Standard_Transient *anItem)
    : entity ( anItem ? (Standard_Transient*)anItem : UndefinedHandleAddress )
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
  Standard_EXPORT ~Handle(Standard_Transient)()
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
    return entity == UndefinedHandleAddress;
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
  friend int operator==(const Standard_Transient *left, const Handle(Standard_Transient)& right)
  {
    return left == right.entity;
  }

  //! Check for inequality
  int operator!=(const Handle(Standard_Transient)& right) const
  {
    return entity != right.entity;
  }

  //! Check for inequality
  int operator!=(const Standard_Transient *right) const
  {
    return entity != right;
  }

  //! Check for inequality
  friend int operator!=(const Standard_Transient *left, const Handle(Standard_Transient)& right)
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
  // Redefined operators new and delete ensure that handles are 
  // allocated using OCC memory manager

  void* operator new(size_t,void* anAddress) 
  {
    return anAddress;
  }
  void* operator new(size_t size) 
  { 
    return Standard::Allocate(size); 
  }
  void  operator delete(void *anAddress, size_t ) 
  { 
    if (anAddress) Standard::Free(anAddress); 
  }

private:
  // Field
  Standard_Transient *entity;
};

#ifdef _WIN32
#pragma warning (pop)
#endif

#endif
