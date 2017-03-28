
#ifndef StdStorage_MapOfRoots_HeaderFile
#define StdStorage_MapOfRoots_HeaderFile

#include <TCollection_AsciiString.hxx>
#include <StdStorage_Root.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_DataMap.hxx>

typedef NCollection_DataMap<TCollection_AsciiString, Handle(StdStorage_Root), TCollection_AsciiString> StdStorage_MapOfRoots;
typedef NCollection_DataMap<TCollection_AsciiString, Handle(StdStorage_Root), TCollection_AsciiString>::Iterator StdStorage_DataMapIteratorOfMapOfRoots;

#endif // StdStorage_MapOfRoots_HeaderFile
