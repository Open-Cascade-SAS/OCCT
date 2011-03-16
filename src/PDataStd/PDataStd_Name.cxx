#include <PDataStd_Name.ixx>

PDataStd_Name::PDataStd_Name() {}

PDataStd_Name::PDataStd_Name(const Handle(PCollection_HExtendedString)& V)
: myValue (V) {}

Handle(PCollection_HExtendedString) PDataStd_Name::Get() const
{ return myValue; }

void PDataStd_Name::Set(const Handle(PCollection_HExtendedString)& V) 
{ myValue = V; }

