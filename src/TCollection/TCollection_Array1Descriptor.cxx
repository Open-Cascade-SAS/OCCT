
//
//
//

#include <TCollection_Array1Descriptor.ixx>

// ---------------------------------------------------------------------

TCollection_Array1Descriptor::TCollection_Array1Descriptor 
    (const Standard_Integer Lower,     
     const Standard_Integer Upper,
     const Standard_Address anAddress
     )
{
  myLower  = Lower;
  myUpper  = Upper;
  myAddress= anAddress;
}

// ---------------------------------------------------------------------

Standard_Integer TCollection_Array1Descriptor::Upper() const 
{ 
  return myUpper;
} 

// ---------------------------------------------------------------------
Standard_Integer TCollection_Array1Descriptor::Lower() const 
{ 
  return myLower;
} 

// ---------------------------------------------------------------------
Standard_Address TCollection_Array1Descriptor::Address() const 
{ 
  return myAddress;
} 
