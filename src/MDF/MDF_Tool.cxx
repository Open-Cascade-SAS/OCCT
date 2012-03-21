// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//      	--------------

// Version:	0.0
//Version	Date		Purpose
//		0.0	Mar 13 1997	Creation



#include <MDF_Tool.ixx>

#include <MDF_ARDriver.hxx>
#include <MDF_ASDriver.hxx>
#include <MDF_RRelocationTable.hxx>
#include <MDF_SRelocationTable.hxx>

#include <PDF_Attribute.hxx>
#include <PDF_HAttributeArray1.hxx>

#include <TDF_Attribute.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_ListIteratorOfAttributeList.hxx>
#include <TDF_AttributeList.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>

#include <TColStd_HArray1OfInteger.hxx>

#include <PTColStd_DataMapIteratorOfPersistentTransientMap.hxx>
#include <PTColStd_DataMapIteratorOfTransientPersistentMap.hxx>
#include <PTColStd_PersistentTransientMap.hxx>
#include <PTColStd_TransientPersistentMap.hxx>

#include <Standard_GUID.hxx>

#ifdef DEB
#include <Standard_ConstructionError.hxx>
#endif

#define DeclareAndSpeedCast(V,T,Vdown) Handle(T) Vdown = *((Handle(T)*)& V)
#define DeclareConstAndSpeedCast(V,T,Vdown) const Handle(T)& Vdown = (Handle(T)&) V
#define SpeedCast(V,T,Vdown) Vdown = *((Handle(T)*)& V)

#undef DEB_MDF_TOOL

// Persistent structure:
//
// Label Array Item
//
// (Tag; NbOfAttributes; NbOfChildren)
//
// The attributes are stored sequencially in the Attribute Array.
// The children of a label are stored recursively after the label item itself.


//      W       W  RRRRR   III  TTTTT  EEEEE
//      W       W  R    R   I     T    E
//       W     W   R    R   I     T    E
//       W  W  W   RRRRR    I     T    EEE
//        WW WW    R  R     I     T    E
//        W   W    R   R   III    T    EEEEE


//=======================================================================
//function : MDF_NbAttributesWithDriver
//purpose  : 
//=======================================================================

static Standard_Integer MDF_NbAttributesWithDriver
(const TDF_Label& aLabel,
 const MDF_TypeASDriverMap& aDriverMap)
{
  Standard_Integer n = 0;
  for (TDF_AttributeIterator itr1(aLabel); itr1.More(); itr1.Next())
    if (aDriverMap.IsBound(itr1.Value()->DynamicType())) ++n;
  for (TDF_ChildIterator itr2(aLabel); itr2.More(); itr2.Next())
    n += MDF_NbAttributesWithDriver(itr2.Value(),aDriverMap);
  return n;
}


//=======================================================================
//function : WriteLabels
//purpose  : TRANSIENT -> PERSISTENT;
//           writes the arrays contents with empty attributes.
//=======================================================================

void MDF_Tool::WriteLabels
(const Handle(TDF_Data)& aSource,
 const Handle(PDF_Data)& aTarget,
 const MDF_TypeASDriverMap& aDriverMap,
 const Handle(MDF_SRelocationTable)& aReloc)
{
  TDF_Label aSourceLab = aSource->Root();

  // Label array sizing.
  Handle(PColStd_HArray1OfInteger) theLabels;
  Standard_Integer nbLab = TDF_Tool::NbLabels(aSourceLab);
  if (nbLab) {
    theLabels = new PColStd_HArray1OfInteger(1,nbLab*3);
    aTarget->Labels(theLabels);
  }
  // In fact, some labels whithout attributes attached to themselves
  // nor to their descendants will not be kept.

  // Attribute array sizing.
  Handle(PDF_HAttributeArray1) theAttributes;
  Standard_Integer nbAtt = 0;
  if (nbLab) {
    nbAtt = MDF_NbAttributesWithDriver(aSourceLab,aDriverMap);
    if (nbAtt) {
      theAttributes = new PDF_HAttributeArray1(1,nbAtt);
      aTarget->Attributes(theAttributes);
    }
    // In fact, some attributes will not be kept by the filter.
  }

  if (nbLab && nbAtt) {
    // labAlloc & attAlloc content the last used cell address.
    Standard_Integer labAlloc = 0;
    Standard_Integer attAlloc = 0;
    MDF_Tool::WriteLabels(aSourceLab, aTarget->Labels(), aTarget->Attributes(),
			  aDriverMap, aReloc, labAlloc, attAlloc);

    // Label array REsizing.
    if (labAlloc < theLabels->Upper()) {
#ifdef DEB
      cout<<"MDF_Tool::WriteLabels LABEL array resizing from "<<theLabels->Upper()<<" to "<<labAlloc<<endl;
#endif
      Handle(PColStd_HArray1OfInteger) newLabels =
	new PColStd_HArray1OfInteger(1,labAlloc);
      for (Standard_Integer i = 1; i <= labAlloc; ++i)
	newLabels->SetValue(i,theLabels->Value(i));
      aTarget->Labels(newLabels);
    }


    // Attribute array REsizing.
    if (attAlloc < theAttributes->Upper()) {
#ifdef DEB
      cout<<"MDF_Tool::WriteLabels ATTRIBUTES array resizing from "<<theAttributes->Upper()<<" to "<<attAlloc<<endl;
#endif
      Handle(PDF_HAttributeArray1) newAttributes =
	new PDF_HAttributeArray1(1,attAlloc);
      for (Standard_Integer i = 1; i <= attAlloc; ++i)
	newAttributes->SetValue(i,theAttributes->Value(i));
      aTarget->Attributes(newAttributes);
    }
  }
}


//=======================================================================
//function : WriteLabels
//purpose  : TRANSIENT -> PERSISTENT;
//           writes the arrays contents with empty attributes.
//=======================================================================

void MDF_Tool::WriteLabels
(const TDF_Label& aSourceLab,
 const Handle(PColStd_HArray1OfInteger)& theLabels,
 const Handle(PDF_HAttributeArray1)& theAttributes,
 const MDF_TypeASDriverMap& aDriverMap,
 const Handle(MDF_SRelocationTable)& aReloc,
 Standard_Integer& labAlloc,
 Standard_Integer& attAlloc)
{
  // Cells allocation.
  Standard_Integer labIndex = labAlloc;
  labAlloc += 3;
  Standard_Integer attIndex = attAlloc; // Backup the value.

  // Tag.
  theLabels->SetValue(++labIndex,aSourceLab.Tag());

  Standard_Integer count = 0;

  // Attributes.
  Handle(PDF_Attribute) pAtt;
  TDF_AttributeIterator itr1(aSourceLab);
  for ( ; itr1.More(); itr1.Next()) {
    const Handle(TDF_Attribute)& tAtt = itr1.Value();
    const Handle(Standard_Type)& type = tAtt->DynamicType();
    if (aDriverMap.IsBound(type)) {
      ++count;
      const Handle(MDF_ASDriver)& driver = aDriverMap.Find(type);
      pAtt = driver->NewEmpty();
      theAttributes->SetValue(++attAlloc,pAtt);
      aReloc->SetRelocation(tAtt,pAtt);
    }
  }
  theLabels->SetValue(++labIndex,count); // Nb Attributes.
  count = 0;

  // Label Children.
  Standard_Integer childLabIndex;
  TDF_ChildIterator itr2(aSourceLab);
  for ( ; itr2.More(); itr2.Next()) {
    childLabIndex = labAlloc;
    const TDF_Label& label = itr2.Value();
    MDF_Tool::WriteLabels(label, theLabels, theAttributes,
			  aDriverMap, aReloc, labAlloc, attAlloc);
    if (childLabIndex < labAlloc) ++count;
  }

#ifdef DEB_MDF_TOOL
  aSourceLab.EntryDump(cout);
  cout<<" Count="<<count<<" attIndex="<<attIndex<<" attAlloc="<<attAlloc;
#endif
  if (attIndex < attAlloc) { // At least one attribute has been stored.
    theLabels->SetValue(++labIndex,count); // Nb Children.
  }
  else {
    labAlloc -= 2;
    theLabels->SetValue(labAlloc--,0); // To clean the tag value.
#ifdef DEB_MDF_TOOL
    cout<<"    <== Cleared";
#endif
  }
#ifdef DEB_MDF_TOOL
  cout<<endl;
#endif
}


//=======================================================================
//function : WriteAttributes
//purpose  : TRANSIENT -> PERSISTENT
//           writes the persistent attributes content.
//=======================================================================

void MDF_Tool::WriteAttributes
(const MDF_TypeASDriverMap& aDriverMap,
 const Handle(MDF_SRelocationTable)& aReloc) 
{
  // Iterer sur la map pour traduire si target.IsNull().
  const PTColStd_TransientPersistentMap& attMap = aReloc->AttributeTable();
  PTColStd_DataMapIteratorOfTransientPersistentMap itr(attMap);
  Handle(PDF_Attribute) pAtt;
  for ( ; itr.More(); itr.Next()) {
    DeclareConstAndSpeedCast(itr.Key(),TDF_Attribute,tAtt);
    const Handle(Standard_Type)& type = tAtt->DynamicType();
    if (aDriverMap.IsBound(type)) {
      SpeedCast(itr.Value(),PDF_Attribute,pAtt);
      const Handle(MDF_ASDriver)& driver = aDriverMap.Find(type);
      driver->Paste(tAtt, pAtt, aReloc);
    }
  }
}



//      RRRRR   EEEEE    A    DDDD
//      R    R  E       A A   D   D
//      R    R  E      A   A  D   D
//      RRRRR   EEE    AAAAA  D   D
//      R  R    E      A   A  D   D
//      R   R   EEEEE  A   A  DDDD

//=======================================================================
//function : ReadLabels
//purpose  : PERSISTENT -> TRANSIENT;
//           reads the arrays contents and build the objects
//           with empty attributes.
//=======================================================================

void MDF_Tool::ReadLabels
(const Handle(PDF_Data)& aSource,
 const Handle(TDF_Data)& aTarget,
 const MDF_TypeARDriverMap& aDriverMap,
 const Handle(MDF_RRelocationTable)& aReloc)
{
  // Root label creation.
  //TDF_Insertor ins(aTarget->Root());
  TDF_Label ins = aTarget->Root();

  // labAlloc & attAlloc content the last used cell address.
  Standard_Integer labRead = 1; // Root already used above.
  Standard_Integer attRead = 0;
  MDF_Tool::ReadLabels(ins, aSource->Labels(), aSource->Attributes(),
		       aDriverMap, aReloc, labRead, attRead);
}


//=======================================================================
//function : ReadLabels
//purpose  : PERSISTENT -> TRANSIENT;
//           Reads the arrays contents and build the objects
//           with empty attributes.
//=======================================================================

void MDF_Tool::ReadLabels
(TDF_Label& anIns,
 const Handle(PColStd_HArray1OfInteger)& theLabels,
 const Handle(PDF_HAttributeArray1)& theAttributes,
 const MDF_TypeARDriverMap& aDriverMap,
 const Handle(MDF_RRelocationTable)& aReloc,
 Standard_Integer& labRead,
 Standard_Integer& attRead)
{
  // Empty attributes creation.
  Handle(TDF_Attribute) tAtt;
  // Number of attributes.
  Standard_Integer nbAtt = theLabels->Value(++labRead);
  for (Standard_Integer iAtt = 1 ; iAtt <= nbAtt; iAtt++) {
    const Handle(PDF_Attribute)& pAtt = theAttributes->Value(++attRead);
    if (pAtt.IsNull()) {
      // This case occurs only if somebody modified manually the file.
      tAtt.Nullify();
    }
    else {
      const Standard_GUID anEmptyID;
      Handle(MDF_RRelocationTable) anEmptyRelocaTable = new MDF_RRelocationTable(Standard_True);
      const Handle(Standard_Type)& type = pAtt->DynamicType();
      if (aDriverMap.IsBound(type)) {
	const Handle(MDF_ARDriver)& driver = aDriverMap.Find(type);
	tAtt = driver->NewEmpty();
	if (tAtt->ID() == anEmptyID) driver->Paste(pAtt, tAtt, anEmptyRelocaTable);
	anIns.AddAttribute(tAtt);
      }
      aReloc->SetRelocation(pAtt,tAtt);
    }
  }

  // Label Children creation.
  Standard_Integer tag;
  // Number of labels.
  Standard_Integer nbLab = theLabels->Value(++labRead);
  for (Standard_Integer iLab = 1 ; iLab <= nbLab; iLab++) {
    // Label creation.
    tag = theLabels->Value(++labRead);


    //TDF_Insertor ins(anIns,tag);
    TDF_Label ins = anIns.FindChild(tag,Standard_True);

    MDF_Tool::ReadLabels(ins, theLabels, theAttributes,
			 aDriverMap, aReloc, labRead, attRead);
  }
}


//=======================================================================
//function : ReadAttributes
//purpose  : PERSISTENT -> TRANSIENT
//           Reads the persistent attributes content.
//=======================================================================

void MDF_Tool::ReadAttributes
(const MDF_TypeARDriverMap& aDriverMap,
 const Handle(MDF_RRelocationTable)& aReloc) 
{
//  cout<<"MDF_Tool::ReadAttributes ..."<<endl;
  // Iterer sur la map pour traduire si target.IsNull().
  const PTColStd_PersistentTransientMap& attMap = aReloc->AttributeTable();
  PTColStd_DataMapIteratorOfPersistentTransientMap itr(attMap);
  Handle(TDF_Attribute) tAtt;
  for ( ; itr.More(); itr.Next()) {
    DeclareConstAndSpeedCast(itr.Key(),PDF_Attribute,pAtt);
    if (!pAtt.IsNull()) { // See above...
      const Handle(Standard_Type)& type = pAtt->DynamicType();
      if (aDriverMap.IsBound(type)) {
	SpeedCast(itr.Value(),TDF_Attribute,tAtt);
	const Handle(MDF_ARDriver)& driver = aDriverMap.Find(type);
	driver->Paste(pAtt, tAtt, aReloc);
      }
    }
  }

  TDF_AttributeList attList;
  for (itr.Initialize(attMap); itr.More(); itr.Next()) {
    SpeedCast(itr.Value(),TDF_Attribute,tAtt);
    attList.Append(tAtt);
  }

  TDF_ListIteratorOfAttributeList itr2;
  Standard_Boolean noDeadLock = Standard_True;
  Standard_Integer nbAtt = attList.Extent();
  while (noDeadLock && (nbAtt != 0)) {
    itr2.Initialize(attList);
    while (itr2.More()) {
      if (!itr2.Value()->AfterRetrieval())
	itr2.Next();
      else
	attList.Remove(itr2);
    }
    noDeadLock = (nbAtt > attList.Extent());
    nbAtt = attList.Extent();
  }

  if (!noDeadLock) {
#ifdef DEB
    cout<<"AfterRetrieval(): dead lock between these attributes:"<<endl;
    for (itr2.Initialize(attList); itr2.More(); itr2.Next())
      cout<<"Attribute type = "<<itr2.Value()->DynamicType()->Name()<<endl;;
    Standard_ConstructionError::Raise("AfterRetrieval: dead lock.");
#endif
    for (itr2.Initialize(attList); itr2.More(); itr2.Next())
      itr2.Value()->AfterRetrieval(Standard_True);
  }
}
