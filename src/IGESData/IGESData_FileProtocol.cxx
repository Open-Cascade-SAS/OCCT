#include  <IGESData_FileProtocol.ixx>


    IGESData_FileProtocol::IGESData_FileProtocol ()    {  }

    void  IGESData_FileProtocol::Add (const Handle(IGESData_Protocol)& protocol)
{
  if      (theresource.IsNull()) theresource = protocol;
  else if (theresource->IsInstance(protocol->DynamicType())) return; // passer
  else if (!thenext.IsNull()) thenext->Add(protocol);
  else {
    thenext = new IGESData_FileProtocol;
    thenext->Add(protocol);
  }
}

    Standard_Integer  IGESData_FileProtocol::NbResources () const
{
  Standard_Integer nb = (theresource.IsNull() ? 0 : 1);
  if (!thenext.IsNull()) nb += thenext->NbResources();
  return nb;
}

    Handle(Interface_Protocol) IGESData_FileProtocol::Resource
  (const Standard_Integer num) const
{
  Handle(IGESData_Protocol) res;
  if (num == 1) return theresource;
  else if (!thenext.IsNull()) return thenext->Resource(num-1);
  return res;  // Null
}
