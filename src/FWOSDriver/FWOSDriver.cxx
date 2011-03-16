// File:	FWOSDriver.cxx
// Created:	Tue Mar  4 16:38:53 1997
// Author:	Mister rmi
//		<rmi@frilox.paris1.matra-dtv.fr>


#include <FWOSDriver.ixx>
#include <FWOSDriver_DriverFactory.hxx>

#include <Plugin_Macro.hxx>

// avoid warnings on 'extern "C"' functions returning C++ classes
#ifdef WNT
#pragma warning(4:4190)
#endif

PLUGIN(FWOSDriver)

Handle(Standard_Transient) FWOSDriver::Factory(const Standard_GUID& aGUID) {
  static Handle(FWOSDriver_DriverFactory) f;
  if(f.IsNull()) f = new FWOSDriver_DriverFactory;
  return f;
}
