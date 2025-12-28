// Created on: 2001-07-09
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <Message_Messenger.hxx>
#include <Message_ProgressScope.hxx>
#include <Storage_Schema.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Map.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <XmlMDF.hxx>
#include <XmlMDF_ADriver.hxx>
#include <XmlMDF_ADriverTable.hxx>
#include <XmlMDF_ReferenceDriver.hxx>
#include <XmlMDF_TagSourceDriver.hxx>
#include <XmlObjMgt_Document.hxx>
#include <XmlObjMgt_Persistent.hxx>
#include <TDocStd_Owner.hxx>
#include <TDocStd_Document.hxx>
#include <Standard_GUID.hxx>

IMPLEMENT_DOMSTRING(TagString, "tag")
IMPLEMENT_DOMSTRING(LabelString, "label")
#define DATATYPE_MIGRATION

// #define DATATYPE_MIGRATION_DEB
//=================================================================================================

static NCollection_Map<occ::handle<Standard_Transient>>& UnsuppTypesMap()
{
  static NCollection_Map<occ::handle<Standard_Transient>> anUnsuppTypes;
  return anUnsuppTypes;
}

//=======================================================================
// function : FromTo
// purpose  : Paste transient data into DOM_Element
//=======================================================================
void XmlMDF::FromTo(const occ::handle<TDF_Data>&            theData,
                    XmlObjMgt_Element&                      theElement,
                    XmlObjMgt_SRelocationTable&             theRelocTable,
                    const occ::handle<XmlMDF_ADriverTable>& theDrivers,
                    const Message_ProgressRange&            theRange)
{
  UnsuppTypesMap().Clear();
  //  int count =
  WriteSubTree(theData->Root(), theElement, theRelocTable, theDrivers, theRange);
  UnsuppTypesMap().Clear();
}

//=================================================================================================

int XmlMDF::WriteSubTree(const TDF_Label&                        theLabel,
                         XmlObjMgt_Element&                      theElement,
                         XmlObjMgt_SRelocationTable&             theRelocTable,
                         const occ::handle<XmlMDF_ADriverTable>& theDrivers,
                         const Message_ProgressRange&            theRange)
{
  XmlObjMgt_Document aDoc = theElement.getOwnerDocument();

  // create element "label"
  XmlObjMgt_Element aLabElem = aDoc.createElement(::LabelString());

  // write attributes
  int                   count = 0;
  TDF_AttributeIterator itr1(theLabel);
  for (; itr1.More(); itr1.Next())
  {
    const occ::handle<TDF_Attribute>& tAtt  = itr1.Value();
    const occ::handle<Standard_Type>& aType = tAtt->DynamicType();
    occ::handle<XmlMDF_ADriver>       aDriver;
    if (theDrivers->GetDriver(aType, aDriver))
    {
      count++;

      //    Add source to relocation table
      int anId = theRelocTable.Add(tAtt);

      //    Create DOM data item
      XmlObjMgt_Persistent pAtt;
      // In the document version 8 the attribute TPrsStd_AISPresentation
      // was replaced by TDataXtd_Presentation. Therefore, for old versions
      // we write old name of the attribute (TPrsStd_AISPresentation).
      const char* typeName = aDriver->TypeName().ToCString();
      if (theRelocTable.GetHeaderData()->StorageVersion().IntegerValue()
            < TDocStd_FormatVersion_VERSION_8
          && strcmp(typeName, "TDataXtd_Presentation") == 0)
      {
        typeName = "TPrsStd_AISPresentation";
      }
      pAtt.CreateElement(aLabElem, typeName, anId);

      //    Paste
      aDriver->Paste(tAtt, pAtt, theRelocTable);
    }
#ifdef OCCT_DEBUG
    else if (!UnsuppTypesMap().Contains(aType))
    {
      std::cout << "attribute driver for type " << aType->Name() << " not found" << std::endl;
      UnsuppTypesMap().Add(aType);
    }
#endif
  }

  // write sub-labels
  TDF_ChildIterator itr2(theLabel);
  double            child_count = 0;
  for (; itr2.More(); ++child_count, itr2.Next())
  {
  }
  itr2.Initialize(theLabel);
  Message_ProgressScope aPS(theRange, "Writing sub-tree", child_count, true);
  for (; itr2.More() && aPS.More(); itr2.Next())
  {
    const TDF_Label& aChildLab = itr2.Value();
    count += WriteSubTree(aChildLab, aLabElem, theRelocTable, theDrivers, aPS.Next());
  }

  if (count > 0 || TDocStd_Owner::GetDocument(theLabel.Data())->EmptyLabelsSavingMode())
  {
    theElement.appendChild(aLabElem);

    // set attribute "tag"
    aLabElem.setAttribute(::TagString(), theLabel.Tag());
  }
  return count;
}

//=======================================================================
// function : FromTo
// purpose  : Paste data from DOM_Element into transient document
//=======================================================================
bool XmlMDF::FromTo(const XmlObjMgt_Element&                theElement,
                    occ::handle<TDF_Data>&                  theData,
                    XmlObjMgt_RRelocationTable&             theRelocTable,
                    const occ::handle<XmlMDF_ADriverTable>& theDrivers,
                    const Message_ProgressRange&            theRange)
{
  TDF_Label aRootLab = theData->Root();
  NCollection_DataMap<TCollection_AsciiString, occ::handle<XmlMDF_ADriver>> aDriverMap;
  theDrivers->CreateDrvMap(aDriverMap);

  int count = 0;

  LDOM_Node         theNode = theElement.getFirstChild();
  XmlObjMgt_Element anElem  = (const XmlObjMgt_Element&)theNode;
  while (!anElem.isNull())
  {
    if (anElem.getNodeName().equals(::LabelString()))
    {
      int subcount = ReadSubTree(anElem, aRootLab, theRelocTable, aDriverMap, theRange);
      // check for error
      if (subcount < 0)
        return false;

      (void)count; // unused but set for debug
      count += subcount;
    }
    // anElem = (const XmlObjMgt_Element &) anElem.getNextSibling();
    LDOM_Node theNode1 = anElem.getNextSibling();
    anElem             = (const XmlObjMgt_Element&)theNode1;
  }

  return true;
}

//=================================================================================================

int XmlMDF::ReadSubTree(
  const XmlObjMgt_Element&                                                         theElement,
  const TDF_Label&                                                                 theLabel,
  XmlObjMgt_RRelocationTable&                                                      theRelocTable,
  const NCollection_DataMap<TCollection_AsciiString, occ::handle<XmlMDF_ADriver>>& theDriverMap,
  const Message_ProgressRange&                                                     theRange)
{
  // Extraction of the driver subset.
  int count = 0;

  // XmlObjMgt_Element anElem = (const XmlObjMgt_Element &) theElement.getFirstChild();
  LDOM_Node             theNode = theElement.getFirstChild();
  XmlObjMgt_Element     anElem  = (const XmlObjMgt_Element&)theNode;
  Message_ProgressScope aPS(theRange, "Reading sub-tree", 2, true);
  while (!anElem.isNull())
  {
    if (anElem.getNodeType() == LDOM_Node::ELEMENT_NODE)
    {
      if (anElem.getNodeName().equals(::LabelString()))
      {
        // read tag
        int                 tag;
        XmlObjMgt_DOMString aTag(anElem.getAttribute(::TagString()));
        if (!aTag.GetInteger(tag))
        {
          TCollection_ExtendedString anErrorMessage =
            TCollection_ExtendedString("Wrong Tag value for OCAF Label: ") + aTag;
          theDriverMap.Find("TDF_TagSource")->myMessageDriver->Send(anErrorMessage, Message_Fail);
          return -1;
        }
        // create label
        TDF_Label aLab = theLabel.FindChild(tag, true);

        // read sub-tree
        int subcount = ReadSubTree(anElem, aLab, theRelocTable, theDriverMap, aPS.Next());
        // check for error
        if (subcount == -1)
          return -1;
        count += subcount;
      }
      else
      {
        // read attribute
        XmlObjMgt_DOMString aName = anElem.getNodeName();

#ifdef DATATYPE_MIGRATION
        TCollection_AsciiString newName;
        if (Storage_Schema::CheckTypeMigration(aName, newName))
        {
  #ifdef OCCT_DEBUG
          std::cout << "CheckTypeMigration:OldType = " << aName.GetString()
                    << " Len = " << strlen(aName.GetString()) << std::endl;
          std::cout << "CheckTypeMigration:NewType = " << newName << " Len = " << newName.Length()
                    << std::endl;
  #endif
          aName = newName.ToCString();
        }
#endif

        if (theDriverMap.IsBound(aName))
        {
          count++;
          const occ::handle<XmlMDF_ADriver>& driver = theDriverMap.Find(aName);
          XmlObjMgt_Persistent               pAtt(anElem);
          int                                anID = pAtt.Id();
          if (anID <= 0)
          { // check for ID validity
            TCollection_ExtendedString anErrorMessage =
              TCollection_ExtendedString("Wrong ID of OCAF attribute with type ") + aName;
            driver->myMessageDriver->Send(anErrorMessage, Message_Fail);
            return -1;
          }
          occ::handle<TDF_Attribute> tAtt;
          bool                       isBound = theRelocTable.IsBound(anID);
          if (isBound)
            tAtt = occ::down_cast<TDF_Attribute>(theRelocTable.Find(anID));
          else
            tAtt = driver->NewEmpty();

          if (tAtt->Label().IsNull())
          {
            try
            {
              theLabel.AddAttribute(tAtt);
            }
            catch (const Standard_DomainError&)
            {
              // For attributes that can have arbitrary GUID (e.g. TDataStd_Integer), exception
              // will be raised in valid case if attribute of that type with default GUID is already
              // present  on the same label; the reason is that actual GUID will be read later.
              // To avoid this, set invalid (null) GUID to the newly added attribute (see #29669)
              static const Standard_GUID fbidGuid;
              tAtt->SetID(fbidGuid);
              theLabel.AddAttribute(tAtt);
            }
          }
          else
            driver->myMessageDriver->Send(TCollection_ExtendedString("XmlDriver warning: ")
                                            + "attempt to attach attribute " + aName
                                            + " to a second label",
                                          Message_Warning);

          if (!driver->Paste(pAtt, tAtt, theRelocTable))
          {
            // error converting persistent to transient
            driver->myMessageDriver->Send(TCollection_ExtendedString("XmlDriver warning: ")
                                            + "failure reading attribute " + aName,
                                          Message_Warning);
          }
          else if (isBound == false)
            theRelocTable.Bind(anID, tAtt);
        }
#ifdef OCCT_DEBUG
        else
        {
          const TCollection_AsciiString anAsciiName = aName;
          std::cerr << "XmlDriver warning: "
                    << "label contains object of unknown type " << anAsciiName << std::endl;
        }
#endif
      }
    }
    // anElem = (const XmlObjMgt_Element &) anElem.getNextSibling();
    LDOM_Node theNode1 = anElem.getNextSibling();
    anElem             = (const XmlObjMgt_Element&)theNode1;

    if (!aPS.More())
      return -1;
  }

  // AfterRetrieval
  if (count > 0)
  {
  }

  return count;
}

//=================================================================================================

void XmlMDF::AddDrivers(const occ::handle<XmlMDF_ADriverTable>& aDriverTable,
                        const occ::handle<Message_Messenger>&   aMessageDriver)
{
  aDriverTable->AddDriver(new XmlMDF_TagSourceDriver(aMessageDriver));
  aDriverTable->AddDriver(new XmlMDF_ReferenceDriver(aMessageDriver));
}
