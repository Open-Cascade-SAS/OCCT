// File:      XmlDrivers_DocumentRetrievalDriver.cxx
// Created:   Mon Jul  9 12:29:49 MSK DST 2001
// Author:    Julia DOROVSKIKH
// Copyright: Open Cascade 2001

#include <XmlDrivers_DocumentRetrievalDriver.ixx>

#include <XmlDrivers.hxx>
#include <XmlMDataStd.hxx>
#include <XmlMNaming.hxx>
#include <XmlObjMgt_Element.hxx>
#include <XmlMNaming_NamedShapeDriver.hxx>
#include <TNaming_NamedShape.hxx>
#include <XmlMDF_ADriver.hxx>
#include <CDM_MessageDriver.hxx>
//=======================================================================
//function : XmlDrivers_DocumentRetrievalDriver
//purpose  : Constructor
//=======================================================================
XmlDrivers_DocumentRetrievalDriver::XmlDrivers_DocumentRetrievalDriver()
{
}

//=======================================================================
//function : AttributeDrivers
//purpose  : 
//=======================================================================
Handle(XmlMDF_ADriverTable) XmlDrivers_DocumentRetrievalDriver::AttributeDrivers
       (const Handle(CDM_MessageDriver)& theMessageDriver) 
{
  return XmlDrivers::AttributeDrivers (theMessageDriver);
}

//=======================================================================
//function : PropagateDocumentVersion
//purpose  : 
//=======================================================================
void XmlDrivers_DocumentRetrievalDriver::PropagateDocumentVersion(
                                   const Standard_Integer theDocVersion )
{
#ifdef DEB
//    cout << "DocCurVersion =" << theDocVersion <<endl;
#endif
  XmlMDataStd::SetDocumentVersion(theDocVersion);
  XmlMNaming::SetDocumentVersion(theDocVersion);
}

//=======================================================================
//function : ReadShapeSection
//purpose  : Implementation of ReadShapeSection
//=======================================================================
Handle(XmlMDF_ADriver) XmlDrivers_DocumentRetrievalDriver::ReadShapeSection(
                               const XmlObjMgt_Element&       theElement,
                               const Handle(CDM_MessageDriver)& theMsgDriver)
{
  if (myDrivers.IsNull()) myDrivers = AttributeDrivers (theMsgDriver);
  Handle(XmlMNaming_NamedShapeDriver) aNamedShapeDriver;
  if (myDrivers -> GetDriver (STANDARD_TYPE(TNaming_NamedShape),
                              aNamedShapeDriver))
    aNamedShapeDriver -> ReadShapeSection (theElement);
  return aNamedShapeDriver;
}

//=======================================================================
//function : ShapeSetCleaning
//purpose  : definition of ShapeSetCleaning
//=======================================================================
void XmlDrivers_DocumentRetrievalDriver::ShapeSetCleaning(
			      const Handle(XmlMDF_ADriver)& theDriver) 
{
  Handle(XmlMNaming_NamedShapeDriver) aNamedShapeDriver = 
    Handle(XmlMNaming_NamedShapeDriver)::DownCast(theDriver);
  if (aNamedShapeDriver.IsNull() == Standard_False)
    aNamedShapeDriver -> Clear();
}


