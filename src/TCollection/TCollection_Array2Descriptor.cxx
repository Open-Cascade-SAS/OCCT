
//
//
//

#include <TCollection_Array2Descriptor.ixx>

// ---------------------------------------------------------------------

TCollection_Array2Descriptor::TCollection_Array2Descriptor 
    (const Standard_Integer aLowerRow,     
     const Standard_Integer aUpperRow,
     const Standard_Integer aLowerCol,     
     const Standard_Integer aUpperCol,
     const Standard_Address anAddress
     )
{
  myLowerRow  = aLowerRow;
  myUpperRow  = aUpperRow;
  myLowerCol  = aLowerCol;
  myUpperCol  = aUpperCol;
  myAddress   = anAddress;
}

// ---------------------------------------------------------------------

Standard_Integer TCollection_Array2Descriptor::UpperRow() const 
{ 
  return myUpperRow;
} 

// ---------------------------------------------------------------------
Standard_Integer TCollection_Array2Descriptor::LowerRow() const 
{ 
  return myLowerRow;
} 

// ---------------------------------------------------------------------

Standard_Integer TCollection_Array2Descriptor::UpperCol() const 
{ 
  return myUpperCol;
} 

// ---------------------------------------------------------------------
Standard_Integer TCollection_Array2Descriptor::LowerCol() const 
{ 
  return myLowerCol;
} 

// ---------------------------------------------------------------------
Standard_Address TCollection_Array2Descriptor::Address() const 
{ 
  return myAddress;
} 
