// File:	FWOSDriver_DriverFactory.cxx
// Created:	Tue Mar  4 14:39:45 1997
// Author:	Mister rmi
//		<rmi@frilox.paris1.matra-dtv.fr>


#include <FWOSDriver_DriverFactory.ixx>

#include <FWOSDriver_Driver.hxx>
FWOSDriver_DriverFactory::FWOSDriver_DriverFactory(){}


Handle(CDF_MetaDataDriver) FWOSDriver_DriverFactory::Build() const {
  static Handle(FWOSDriver_Driver) d =  new FWOSDriver_Driver();
  return d;
}
