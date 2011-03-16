#include <Storage_DefaultCallBack.ixx>

Storage_DefaultCallBack::Storage_DefaultCallBack()
{
}

Handle(Standard_Persistent) Storage_DefaultCallBack::New() const
{
  Handle(Standard_Persistent) res = new Standard_Persistent;

  return res;
}

void Storage_DefaultCallBack::Add(const Handle(Standard_Persistent)&, const Handle(Storage_Schema)&) const
{
}

void Storage_DefaultCallBack::Write(const Handle(Standard_Persistent)&,Storage_BaseDriver&,const Handle(Storage_Schema)&) const
{
}

void Storage_DefaultCallBack::Read(const Handle(Standard_Persistent)&,Storage_BaseDriver& f,const Handle(Storage_Schema)&) const
{
  f.SkipObject();
}
