#include <PDF_TagSource.ixx>

PDF_TagSource::PDF_TagSource() { }

PDF_TagSource::PDF_TagSource(const Standard_Integer V)
: myValue (V) {}

Standard_Integer PDF_TagSource::Get() const
{ return myValue; }

void PDF_TagSource::Set(const Standard_Integer V) 
{ myValue = V; }

