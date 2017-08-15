// File:	TOcaf_Application.cxx
// Created:	Mon Jul  3  2000
// Author:	J Mazars

//Modified by Sergey RUIN (An instanciation of the drivers)

#include <stdafx.h>
#include <TOcaf_Application.hxx>
#include <TDF_Label.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <TPrsStd_AISViewer.hxx>
#include <TNaming_NamedShape.hxx>
#include "TFunction_DriverTable.hxx"
#include "TOcafFunction_BoxDriver.hxx"
#include "TOcafFunction_CylDriver.hxx"
#include "TOcafFunction_CutDriver.hxx"

IMPLEMENT_STANDARD_RTTIEXT (TOcaf_Application, TDocStd_Application)

//=======================================================================
//function : TOcaf_Application
//purpose  : 
//=======================================================================

TOcaf_Application::TOcaf_Application()
{
  // Instanciate a TOcafFunction_BoxDriver and add it to the TFunction_DriverTable
  TFunction_DriverTable::Get()->AddDriver (TOcafFunction_BoxDriver::GetID(),
                                           new TOcafFunction_BoxDriver());

  // Instanciate a TOcafFunction_Cyl  Driver and add it to the TFunction_DriverTable
  TFunction_DriverTable::Get()->AddDriver (TOcafFunction_CylDriver::GetID(),
                                           new TOcafFunction_CylDriver());

  // Instanciate a TOcafFunction_CutDriver and add it to the TFunction_DriverTable
  Handle(TOcafFunction_CutDriver) myCutDriver = new TOcafFunction_CutDriver();
  TFunction_DriverTable::Get()->AddDriver (TOcafFunction_CutDriver::GetID(),
                                           new TOcafFunction_CutDriver());
}
