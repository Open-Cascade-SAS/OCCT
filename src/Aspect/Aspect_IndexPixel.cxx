
#include <stdio.h>

#include <Standard_Stream.hxx>
#include <Aspect_IndexPixel.ixx>

Aspect_IndexPixel::Aspect_IndexPixel () {

  myIndex = 0;

}

Aspect_IndexPixel::Aspect_IndexPixel (const Standard_Integer anIndex) {

  myIndex = anIndex;

}

Standard_Integer Aspect_IndexPixel::Value() const { 

return myIndex;

}


void Aspect_IndexPixel::SetValue(const Standard_Integer anIndex) {

  myIndex = anIndex;

}

void Aspect_IndexPixel::Print(Standard_OStream& s) const

{
	s << dec << setw(4) << myIndex;
}

// ------------------------------------------------------------------
// Hascode : Computes a hascoding value for a given Aspect_IndexPixel
// ------------------------------------------------------------------
Standard_Integer Aspect_IndexPixel::HashCode(const Standard_Integer Upper) const
{
   return ( myIndex % Upper ) + 1 ;
}


Standard_Boolean Aspect_IndexPixel::IsEqual(const Aspect_IndexPixel& Other) const
{
  return (myIndex == Other.myIndex);
}

Standard_Boolean Aspect_IndexPixel::IsNotEqual(const Aspect_IndexPixel& Other) const
{
  return !IsEqual(Other);
}
