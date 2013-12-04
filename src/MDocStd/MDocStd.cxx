// Created on: 1997-11-12
// Created by: Guest Design
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <MDocStd.ixx>
#include <MDocStd_DocumentStorageDriver.hxx>
#include <MDocStd_DocumentRetrievalDriver.hxx>
#include <Standard_Failure.hxx>
#include <MDocStd_XLinkStorageDriver.hxx>
#include <MDocStd_XLinkRetrievalDriver.hxx>
#include <CDM_MessageDriver.hxx>
#include <TDF_Tool.hxx>
static Standard_GUID StandardStorageDriver ("ad696000-5b34-11d1-b5ba-00a0c9064368");
static Standard_GUID StandardRetrievalDriver ("ad696001-5b34-11d1-b5ba-00a0c9064368");

/* ======= not used functionality ======
//#include <TDocStd_Document.hxx>
//#include <MDocStd_PersistentMap.hxx>
//#include <TDocStd_XLink.hxx>
//#include <TDocStd_XLinkIterator.hxx>
//#include <MDocStd_ListIteratorOfDocEntryList.hxx>
//#include <TDF_Label.hxx>
//#include <TDF_ChildIterator.hxx>
//#include <TNaming_NamedShape.hxx>
//#include <PNaming_NamedShape.hxx>
//#include <PTopoDS_Shape1.hxx>
//#include <TopoDS_Shape.hxx>
//#include <PTopoDS_TShape1.hxx>
//#include <PTopoDS_HArray1OfShape1.hxx>
//#include <PBRep_TVertex1.hxx>
//#include <PBRep_PointRepresentation.hxx>
//#include <PBRep_TEdge1.hxx>
//#include <PBRep_CurveRepresentation.hxx>
//#include <PBRep_TFace1.hxx>
//#include <PGeom_Surface.hxx>
//#include <TColStd_ListOfAsciiString.hxx>
//=======================================================================
//function : WeightWatcher
//purpose  : 
//=======================================================================

static void ShapeWeightWatcher (const PTopoDS_Shape1& PS,
				MDocStd_PersistentMap& aMap)
{
  if (aMap.Add(PS.TShape())) {
    Handle(PTopoDS_TShape1) TS = PS.TShape();
    Handle(PTopoDS_HArray1OfShape1) HS = TS->Shapes();
    if (!HS.IsNull()) {
      for (Standard_Integer i=1;i<=HS->Length();i++) {
	const PTopoDS_Shape1& VAL = HS->Value(i);
	if (!VAL.TShape().IsNull())ShapeWeightWatcher(VAL,aMap);
      }
      Handle(PBRep_TVertex1) TV = Handle(PBRep_TVertex1)::DownCast(TS);
      if (!TV.IsNull()) {
	Handle(PBRep_PointRepresentation) PR;
	TV->Points(PR);
	return;
      }
      Handle(PBRep_TEdge1) TE = Handle(PBRep_TEdge1)::DownCast(TS);
      if (!TE.IsNull()) {
	Handle(PBRep_CurveRepresentation) CR;
	TE->Curves(CR);
	return;
      }
      Handle(PBRep_TFace1) TF = Handle(PBRep_TFace1)::DownCast(TS);
      if (!TF.IsNull()) {
	Handle(PGeom_Surface) PGeomS;
	TF->Surface(PGeomS);
	return;
      }
    }
  }
}
//=======================================================================
//function : AttributeWeightWatcher
//purpose  : 
//=======================================================================

static void  AttributeWeightWatcher(const TDF_Label& LabS,
				    const Handle(MDF_SRelocationTable)& aReloc,
				    MDocStd_PersistentMap& aMap)
{
  TCollection_AsciiString STR;
  TDF_Tool::Entry(LabS,STR);
#ifdef DEB
  cout << "Label to process: "<<STR<<endl;
#endif
  Handle(TNaming_NamedShape) NS;
  Handle(PNaming_NamedShape) PNS;
  if (LabS.FindAttribute(TNaming_NamedShape::GetID(),NS)) {
    if (aReloc->HasRelocation(NS,PNS)) {
      const Handle(PTopoDS_HArray1OfShape1)& HOldS = PNS->OldShapes();  
      if (!HOldS.IsNull()) {
	for (Standard_Integer i=1;i<=HOldS->Length();i++) {
	  const PTopoDS_Shape1& POS = HOldS->Value(i);
	  if (!POS.TShape().IsNull()) ShapeWeightWatcher(POS,aMap);
	}
      }
      const Handle(PTopoDS_HArray1OfShape1)& HNewS = PNS->NewShapes();    
      if (!HNewS.IsNull()) {
	for (Standard_Integer j=1;j<=HNewS->Length();j++) {
	  const PTopoDS_Shape1& PTS = HNewS->Value(j);
	  if (!PTS.TShape().IsNull()) ShapeWeightWatcher(PTS,aMap);
	}
      }
    }
  }  

//traiter les fils
  for (TDF_ChildIterator itr(LabS) ; itr.More(); itr.Next()) {
    AttributeWeightWatcher(itr.Value(),aReloc,aMap);
  }
}
//=======================================================================
//function : WeightWatcher
//purpose  : 
//=======================================================================

void MDocStd::WeightWatcher(const Handle(TDF_Data)& aSource,
			    const Handle(MDF_SRelocationTable)& aReloc,
			    const TColStd_ListOfAsciiString& aEntry) 
{
  Handle(TDocStd_Document) D;
  D = TDocStd_Document::Get(aSource->Root());
  if (!D.IsNull()) {
    TDocStd_XLink* xRefPtr;
    MDocStd_PersistentMap aMap;
    //for (TDocStd_XLinkIterator xItr(aSource); xItr.More(); xItr.Next()) {
    for (TDocStd_XLinkIterator xItr(D); xItr.More(); xItr.Next()) {
      xRefPtr = xItr.Value();
      const TCollection_AsciiString & DocEntry=  xRefPtr->DocumentEntry();
      TColStd_ListIteratorOfListOfAsciiString It(aEntry);
      for (;It.More();It.Next()) {
	if (It.Value()==DocEntry) AttributeWeightWatcher(xRefPtr->Label(),aReloc,aMap);
      }
    }
  }
}

***** */
//=======================================================================
//function : AddStorageDrivers
//purpose  : 
//=======================================================================

void MDocStd::AddStorageDrivers(const Handle(MDF_ASDriverHSequence)& aDriverSeq, 
				const Handle(CDM_MessageDriver)& theMsgDriver) 
{ aDriverSeq->Append(new MDocStd_XLinkStorageDriver(theMsgDriver)); }


//=======================================================================
//function : AddRetrievalDrivers
//purpose  : 
//=======================================================================

void MDocStd::AddRetrievalDrivers(const Handle(MDF_ARDriverHSequence)& aDriverSeq,
				  const Handle(CDM_MessageDriver)& theMsgDriver) 
{ aDriverSeq->Append(new MDocStd_XLinkRetrievalDriver(theMsgDriver)); }

//=======================================================================
//function : Factory
//purpose  : 
//=======================================================================

Handle(Standard_Transient) MDocStd::Factory(const Standard_GUID& aGUID) {

  if(aGUID == StandardStorageDriver)  {
#ifdef DEB
    cout << "MDocStd : Storage Plugin" << endl;
#endif
    static Handle(MDocStd_DocumentStorageDriver) standard_sd = new MDocStd_DocumentStorageDriver;
    return standard_sd;
  }
  if(aGUID == StandardRetrievalDriver) {  
#ifdef DEB
    cout << "MDocStd : Retrieval Plugin" << endl;
#endif
    static Handle (MDocStd_DocumentRetrievalDriver) standard_rd = new MDocStd_DocumentRetrievalDriver;
    return standard_rd;
  }

  Standard_Failure::Raise("MDocStd : Factory: unknown GUID");
  Handle(Standard_Transient) t;
  return t;

}
