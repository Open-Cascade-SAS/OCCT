#include <Handle_Standard_Persistent.hxx>



Handle(Standard_Persistent)& Handle(Standard_Persistent)::operator=
	(const Handle(Standard_Persistent)& aHandle)
     {
      Assign(aHandle.Access());
      return *this;
     }



Handle(Standard_Persistent)& Handle(Standard_Persistent)::operator=
(const Standard_Persistent* anItem)
     {
      Assign((const Standard_Persistent *)anItem);
      return *this;
     }

