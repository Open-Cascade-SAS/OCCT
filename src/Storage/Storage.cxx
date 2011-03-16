#include <TCollection_AsciiString.hxx>
#include <Storage.hxx>

TCollection_AsciiString Storage::Version() 
{
  TCollection_AsciiString v("1.3");

  return v;
}

