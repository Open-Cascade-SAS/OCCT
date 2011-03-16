#ifndef _Standard_Transient_proto_HeaderFile
#define _Standard_Transient_proto_HeaderFile

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
    //! Operator new for placement in pre-allocated memory
    void* operator new(size_t,void* anAddress) 
      {
        return anAddress;
      }
    //! Operator new for memory allocation uses Open CASCADE memory manager
    void* operator new(size_t size) 
      { 
        return Standard::Allocate(size); 
      }
    //! Operator delete symmetric to operator new
    void  operator delete(void *anAddress) 
      { 
        if (anAddress) Standard::Free((Standard_Address&)anAddress); 
      }

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

    //! Returns a hash code value for this object. 
    //! The value is in the range 1..Upper.
    Standard_EXPORT virtual Standard_Integer HashCode(const Standard_Integer Upper) const;

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

   Standard_Integer count;
};

Standard_EXPORT const Handle(Standard_Type)& STANDARD_TYPE(Standard_Transient);

#endif 
