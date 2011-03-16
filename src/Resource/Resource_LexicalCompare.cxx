#include <TCollection_AsciiString.hxx>
#include <Resource_LexicalCompare.hxx>


// -----------
// Create :
// -----------

Resource_LexicalCompare::Resource_LexicalCompare()
{
}

// -----------
// IsLower :
// -----------

Standard_Boolean Resource_LexicalCompare::IsLower (
                 const TCollection_AsciiString& Left,
		 const TCollection_AsciiString& Right) const 
{
  return Left.IsLess(Right) ;
}

