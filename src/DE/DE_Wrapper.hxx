// Copyright (c) 2022 OPEN CASCADE SAS
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

#ifndef _DE_Wrapper_HeaderFile
#define _DE_Wrapper_HeaderFile

#include <DE_ConfigurationNode.hxx>
#include <Message_ProgressRange.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <Standard_Mutex.hxx>
#include <TColStd_ListOfAsciiString.hxx>

class TopoDS_Shape;
class XSControl_WorkSession;
class TDocStd_Document;

typedef NCollection_IndexedDataMap<TCollection_AsciiString, Handle(DE_ConfigurationNode)> DE_ConfigurationVendorMap;
typedef NCollection_DataMap<TCollection_AsciiString, DE_ConfigurationVendorMap> DE_ConfigurationFormatMap;

//! The main class for working with CAD file exchange.
//! Loads and Saves special CAD transfer property.
//! Consolidates all supported Formats and Vendors.
//! Automatically recognizes CAD format and uses the preferred existed Vendor.
//! Note:
//!   If Vendor's format is not binded, the configuration loading doesn't affect on its property.
//!
//! Nodes are grouped by Vendor's name and Format's type.
//! The Vendors may have the same supported CAD formats.
//! Use a Vendor's priority for transfer operations.
//!
//! The algorithm for standalone transfer operation:
//! 1) Work with global wrapper directly or make deep copy and work with it
//! 2) Update the supported vendors and formats
//!   2.1) Create and initialize specialized configuration node of the required format and Vendor.
//!   2.2) Bind the created node to the internal map(::Bind)
//! 3) Configure the transfer property by resource string or file (::Load)
//!   3.1) Configuration can disable or enable some Vendors and formats
//!   3.2) Configuration can change the priority of Vendors
//! 4) Initiate the transfer process by calling "::Write" or "::Read" methods
//! 5) Validate the transfer process output
class DE_Wrapper : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(DE_Wrapper, Standard_Transient)

public:

  //! Initializes all field by default
  Standard_EXPORT DE_Wrapper();

  //! Copies values of all fields
  //! @param[in] theWrapper object to copy
  Standard_EXPORT DE_Wrapper(const Handle(DE_Wrapper)& theWrapper);

  //! Gets global configuration singleton.
  //! If wrapper is not set, create it by default as base class object.
  //! @return point to global configuration
  Standard_EXPORT static const Handle(DE_Wrapper)& GlobalWrapper();

  //! Sets global configuration singleton
  //! @param[in] theWrapper object to set as global configuration
  Standard_EXPORT static void SetGlobalWrapper(const Handle(DE_Wrapper)& theWrapper);

  Standard_EXPORT static Standard_Mutex& GlobalLoadMutex();

public:

  //! Reads a CAD file, according internal configuration
  //! @param[in] thePath path to the import CAD file
  //! @param[out] theDocument document to save result
  //! @param[in] theWS current work session
  //! @param theProgress[in] progress indicator
  //! @return true if Read operation has ended correctly
  Standard_EXPORT Standard_Boolean Read(const TCollection_AsciiString& thePath,
                                        const Handle(TDocStd_Document)& theDocument,
                                        Handle(XSControl_WorkSession)& theWS,
                                        const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Writes a CAD file, according internal configuration
  //! @param[in] thePath path to the export CAD file
  //! @param[out] theDocument document to export
  //! @param[in] theWS current work session
  //! @param theProgress[in] progress indicator
  //! @return true if Write operation has ended correctly
  Standard_EXPORT Standard_Boolean Write(const TCollection_AsciiString& thePath,
                                         const Handle(TDocStd_Document)& theDocument,
                                         Handle(XSControl_WorkSession)& theWS,
                                         const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Reads a CAD file, according internal configuration
  //! @param[in] thePath path to the import CAD file
  //! @param[out] theDocument document to save result
  //! @param theProgress[in] progress indicator
  //! @return true if Read operation has ended correctly
  Standard_EXPORT Standard_Boolean Read(const TCollection_AsciiString& thePath,
                                        const Handle(TDocStd_Document)& theDocument,
                                        const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Writes a CAD file, according internal configuration
  //! @param[in] thePath path to the export CAD file
  //! @param[out] theDocument document to export
  //! @param theProgress[in] progress indicator
  //! @return true if Write operation has ended correctly
  Standard_EXPORT Standard_Boolean Write(const TCollection_AsciiString& thePath,
                                         const Handle(TDocStd_Document)& theDocument,
                                         const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Reads a CAD file, according internal configuration
  //! @param[in] thePath path to the import CAD file
  //! @param[out] theShape shape to save result
  //! @param[in] theWS current work session
  //! @param theProgress[in] progress indicator
  //! @return true if Read operation has ended correctly
  Standard_EXPORT Standard_Boolean Read(const TCollection_AsciiString& thePath,
                                        TopoDS_Shape& theShape,
                                        Handle(XSControl_WorkSession)& theWS,
                                        const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Writes a CAD file, according internal configuration
  //! @param[in] thePath path to the export CAD file
  //! @param[out] theShape shape to export
  //! @param[in] theWS current work session
  //! @param theProgress[in] progress indicator
  //! @return true if Write operation has ended correctly
  Standard_EXPORT Standard_Boolean Write(const TCollection_AsciiString& thePath,
                                         const TopoDS_Shape& theShape,
                                         Handle(XSControl_WorkSession)& theWS,
                                         const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Reads a CAD file, according internal configuration
  //! @param[in] thePath path to the import CAD file
  //! @param[out] theShape shape to save result
  //! @param theProgress[in] progress indicator
  //! @return true if Read operation has ended correctly
  Standard_EXPORT Standard_Boolean Read(const TCollection_AsciiString& thePath,
                                        TopoDS_Shape& theShape,
                                        const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Writes a CAD file, according internal configuration
  //! @param[in] thePath path to the export CAD file
  //! @param[out] theShape shape to export
  //! @param theProgress[in] progress indicator
  //! @return true if Write operation has ended correctly
  Standard_EXPORT Standard_Boolean Write(const TCollection_AsciiString& thePath,
                                         const TopoDS_Shape& theShape,
                                         const Message_ProgressRange& theProgress = Message_ProgressRange());

public:

  //! Updates values according the resource file
  //! @param[in] theResource file path to resource or resource value
  //! @param[in] theIsRecursive flag to update all nodes
  //! @return true if theResource has loaded correctly
  Standard_EXPORT Standard_Boolean Load(const TCollection_AsciiString& theResource = "",
                                        const Standard_Boolean theIsRecursive = Standard_True);

  //! Updates values according the resource
  //! @param[in] theResource input resource to use
  //! @param[in] theIsRecursive flag to update all nodes
  //! @return true if theResource has loaded correctly
  Standard_EXPORT Standard_Boolean Load(const Handle(DE_ConfigurationContext)& theResource,
                                        const Standard_Boolean theIsRecursive = Standard_True);

  //! Writes configuration to the resource file
  //! @param[in] theResourcePath file path to resource
  //! @param[in] theIsRecursive flag to write values of all nodes
  //! @param[in] theFormats list of formats to save. If empty, saves all available
  //! @param[in] theVendors list of providers to save. If empty, saves all available
  //! @return true if the Configuration has saved correctly
  Standard_EXPORT Standard_Boolean Save(const TCollection_AsciiString& theResourcePath,
                                        const Standard_Boolean theIsRecursive = Standard_True,
                                        const TColStd_ListOfAsciiString& theFormats = TColStd_ListOfAsciiString(),
                                        const TColStd_ListOfAsciiString& theVendors = TColStd_ListOfAsciiString());

  //! Writes configuration to the string
  //! @param[in] theIsRecursive flag to write values of all nodes
  //! @param[in] theFormats list of formats to save. If empty, saves all available
  //! @param[in] theVendors list of providers to save. If empty, saves all available
  //! @return result resource string
  Standard_EXPORT TCollection_AsciiString Save(const Standard_Boolean theIsRecursive = Standard_True,
                                               const TColStd_ListOfAsciiString& theFormats = TColStd_ListOfAsciiString(),
                                               const TColStd_ListOfAsciiString& theVendors = TColStd_ListOfAsciiString());

  //! Creates new node copy and adds to the map
  //! @param[in] theNode input node to copy
  //! @return Standard_True if binded
  Standard_EXPORT Standard_Boolean Bind(const Handle(DE_ConfigurationNode)& theNode);

  //! Removes node with the same type from the map
  //! @param[in] theNode input node to remove the same
  //! @return Standard_True if removed
  Standard_EXPORT Standard_Boolean UnBind(const Handle(DE_ConfigurationNode)& theNode);

  //! Finds a node associated with input format and vendor
  //! @param[in] theFormat input node CAD format
  //! @param[in] theVendor input node vendor name
  //! @param[out] theNode output node
  //! @return Standard_True if the node is found
  Standard_EXPORT Standard_Boolean Find(const TCollection_AsciiString& theFormat,
                                        const TCollection_AsciiString& theVendor,
                                        Handle(DE_ConfigurationNode)& theNode) const;

  //! Changes provider priority to one format if it exists
  //! @param[in] theFormat input node CAD format
  //! @param[in] theVendorPriority priority of work with vendors
  //! @param[in] theToDisable flag for disabling nodes that are not included in the priority
  Standard_EXPORT void ChangePriority(const TCollection_AsciiString& theFormat,
                                      const TColStd_ListOfAsciiString& theVendorPriority,
                                      const Standard_Boolean theToDisable = Standard_False);

  //! Changes provider priority to all loaded nodes
  //! @param[in] theVendorPriority priority of work with vendors
  //! @param[in] theToDisable flag for disabling nodes that are not included in the priority
  Standard_EXPORT void ChangePriority(const TColStd_ListOfAsciiString& theVendorPriority,
                                      const Standard_Boolean theToDisable = Standard_False);

  //! Find available provider from the configuration.
  //! If there are several providers, choose the one with the highest priority.
  //! @param[in] thePath path to the CAD file
  //! @param[in] theToImport flag to finds for import. Standard_True-import, Standard_False-export
  //! @param[out] theProvider created new provider
  //! @return Standard_True if provider found and created
  Standard_EXPORT virtual Standard_Boolean FindProvider(const TCollection_AsciiString& thePath,
                                                        const Standard_Boolean theToImport,
                                                        Handle(DE_Provider)& theProvider) const;

  //! Updates all registered nodes, all changes will be saved in nodes
  //! @param[in] theToForceUpdate flag that turns on/of nodes, according to updated ability to import/export
  Standard_EXPORT void UpdateLoad(const Standard_Boolean theToForceUpdate = Standard_False) const;

  //! Gets flag that keeps changes on configuration nodes which are being updated, false by default
  Standard_Boolean KeepUpdates() const { return myKeepUpdates; }

  //! Sets flag that keeps changes on configuration nodes which are being updated, false by default
  void SetKeepUpdates(const Standard_Boolean theToKeepUpdates) { myKeepUpdates = theToKeepUpdates; }

  //! Gets format map, contains vendor map with nodes
  //! @return internal map of formats
  Standard_EXPORT const DE_ConfigurationFormatMap& Nodes() const;

  //! Copies values of all fields
  //! @return new object with the same field values
  Standard_EXPORT virtual Handle(DE_Wrapper) Copy() const;

protected:

  //! Sorts the vendors according to the priority to work
  //! Formats omitted from the resource are not modified
  //! Vendors omitted from the format scope are disabled
  //! @param[in] theResource resource to get priority
  void sort(const Handle(DE_ConfigurationContext)& theResource);

public:

  DE_ConfigurationNode::DE_SectionGlobal GlobalParameters; //!< Internal parameters for the all translators

private:

  Standard_Boolean myKeepUpdates; //!< Flag that keeps changes on configuration nodes which are being updated
  DE_ConfigurationFormatMap myConfiguration; //!< Internal map of formats
};

#endif // _DE_Wrapper_HeaderFile
