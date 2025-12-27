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
#include <DE_Provider.hxx>
#include <Message_ProgressRange.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_List.hxx>

#include <mutex>

class TopoDS_Shape;
class XSControl_WorkSession;
class TDocStd_Document;

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
  Standard_EXPORT DE_Wrapper(const occ::handle<DE_Wrapper>& theWrapper);

  //! Gets global configuration singleton.
  //! If wrapper is not set, create it by default as base class object.
  //! @return point to global configuration
  Standard_EXPORT static const occ::handle<DE_Wrapper>& GlobalWrapper();

  //! Sets global configuration singleton
  //! @param[in] theWrapper object to set as global configuration
  Standard_EXPORT static void SetGlobalWrapper(const occ::handle<DE_Wrapper>& theWrapper);

  Standard_EXPORT static std::mutex& GlobalLoadMutex();

public:
  //! Reads a CAD file, according internal configuration
  //! @param[in] thePath path to the import CAD file
  //! @param[out] theDocument document to save result
  //! @param[in] theWS current work session
  //! @param[in] theProgress progress indicator
  //! @return true if Read operation has ended correctly
  Standard_EXPORT bool
    Read(const TCollection_AsciiString&  thePath,
         const occ::handle<TDocStd_Document>& theDocument,
         occ::handle<XSControl_WorkSession>&  theWS,
         const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Writes a CAD file, according internal configuration
  //! @param[in] thePath path to the export CAD file
  //! @param[out] theDocument document to export
  //! @param[in] theWS current work session
  //! @param[in] theProgress progress indicator
  //! @return true if Write operation has ended correctly
  Standard_EXPORT bool
    Write(const TCollection_AsciiString&  thePath,
          const occ::handle<TDocStd_Document>& theDocument,
          occ::handle<XSControl_WorkSession>&  theWS,
          const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Reads a CAD file, according internal configuration
  //! @param[in] thePath path to the import CAD file
  //! @param[out] theDocument document to save result
  //! @param[in] theProgress progress indicator
  //! @return true if Read operation has ended correctly
  Standard_EXPORT bool
    Read(const TCollection_AsciiString&  thePath,
         const occ::handle<TDocStd_Document>& theDocument,
         const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Writes a CAD file, according internal configuration
  //! @param[in] thePath path to the export CAD file
  //! @param[out] theDocument document to export
  //! @param[in] theProgress progress indicator
  //! @return true if Write operation has ended correctly
  Standard_EXPORT bool
    Write(const TCollection_AsciiString&  thePath,
          const occ::handle<TDocStd_Document>& theDocument,
          const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Reads a CAD file, according internal configuration
  //! @param[in] thePath path to the import CAD file
  //! @param[out] theShape shape to save result
  //! @param[in] theWS current work session
  //! @param[in] theProgress progress indicator
  //! @return true if Read operation has ended correctly
  Standard_EXPORT bool
    Read(const TCollection_AsciiString& thePath,
         TopoDS_Shape&                  theShape,
         occ::handle<XSControl_WorkSession>& theWS,
         const Message_ProgressRange&   theProgress = Message_ProgressRange());

  //! Writes a CAD file, according internal configuration
  //! @param[in] thePath path to the export CAD file
  //! @param[out] theShape shape to export
  //! @param[in] theWS current work session
  //! @param[in] theProgress progress indicator
  //! @return true if Write operation has ended correctly
  Standard_EXPORT bool
    Write(const TCollection_AsciiString& thePath,
          const TopoDS_Shape&            theShape,
          occ::handle<XSControl_WorkSession>& theWS,
          const Message_ProgressRange&   theProgress = Message_ProgressRange());

  //! Reads a CAD file, according internal configuration
  //! @param[in] thePath path to the import CAD file
  //! @param[out] theShape shape to save result
  //! @param[in] theProgress progress indicator
  //! @return true if Read operation has ended correctly
  Standard_EXPORT bool
    Read(const TCollection_AsciiString& thePath,
         TopoDS_Shape&                  theShape,
         const Message_ProgressRange&   theProgress = Message_ProgressRange());

  //! Writes a CAD file, according internal configuration
  //! @param[in] thePath path to the export CAD file
  //! @param[out] theShape shape to export
  //! @param[in] theProgress progress indicator
  //! @return true if Write operation has ended correctly
  Standard_EXPORT bool
    Write(const TCollection_AsciiString& thePath,
          const TopoDS_Shape&            theShape,
          const Message_ProgressRange&   theProgress = Message_ProgressRange());

  //! Reads streams according to internal configuration
  //! @param[in] theStreams streams to read from
  //! @param[out] theDocument document to save result
  //! @param[in] theWS current work session
  //! @param[in] theProgress progress indicator
  //! @return true if Read operation has ended correctly
  Standard_EXPORT bool
    Read(DE_Provider::ReadStreamList&    theStreams,
         const occ::handle<TDocStd_Document>& theDocument,
         occ::handle<XSControl_WorkSession>&  theWS,
         const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Writes streams according to internal configuration
  //! @param[in] theStreams streams to write to
  //! @param[out] theDocument document to export
  //! @param[in] theWS current work session
  //! @param[in] theProgress progress indicator
  //! @return true if Write operation has ended correctly
  Standard_EXPORT bool
    Write(DE_Provider::WriteStreamList&   theStreams,
          const occ::handle<TDocStd_Document>& theDocument,
          occ::handle<XSControl_WorkSession>&  theWS,
          const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Reads streams according to internal configuration
  //! @param[in] theStreams streams to read from
  //! @param[out] theDocument document to save result
  //! @param[in] theProgress progress indicator
  //! @return true if Read operation has ended correctly
  Standard_EXPORT bool
    Read(DE_Provider::ReadStreamList&    theStreams,
         const occ::handle<TDocStd_Document>& theDocument,
         const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Writes streams according to internal configuration
  //! @param[in] theStreams streams to write to
  //! @param[out] theDocument document to export
  //! @param[in] theProgress progress indicator
  //! @return true if Write operation has ended correctly
  Standard_EXPORT bool
    Write(DE_Provider::WriteStreamList&   theStreams,
          const occ::handle<TDocStd_Document>& theDocument,
          const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Reads streams according to internal configuration
  //! @param[in] theStreams streams to read from
  //! @param[out] theShape shape to save result
  //! @param[in] theWS current work session
  //! @param[in] theProgress progress indicator
  //! @return true if Read operation has ended correctly
  Standard_EXPORT bool
    Read(DE_Provider::ReadStreamList&   theStreams,
         TopoDS_Shape&                  theShape,
         occ::handle<XSControl_WorkSession>& theWS,
         const Message_ProgressRange&   theProgress = Message_ProgressRange());

  //! Writes streams according to internal configuration
  //! @param[in] theStreams streams to write to
  //! @param[out] theShape shape to export
  //! @param[in] theWS current work session
  //! @param[in] theProgress progress indicator
  //! @return true if Write operation has ended correctly
  Standard_EXPORT bool
    Write(DE_Provider::WriteStreamList&  theStreams,
          const TopoDS_Shape&            theShape,
          occ::handle<XSControl_WorkSession>& theWS,
          const Message_ProgressRange&   theProgress = Message_ProgressRange());

  //! Reads streams according to internal configuration
  //! @param[in] theStreams streams to read from
  //! @param[out] theShape shape to save result
  //! @param[in] theProgress progress indicator
  //! @return true if Read operation has ended correctly
  Standard_EXPORT bool
    Read(DE_Provider::ReadStreamList& theStreams,
         TopoDS_Shape&                theShape,
         const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Writes streams according to internal configuration
  //! @param[in] theStreams streams to write to
  //! @param[out] theShape shape to export
  //! @param[in] theProgress progress indicator
  //! @return true if Write operation has ended correctly
  Standard_EXPORT bool
    Write(DE_Provider::WriteStreamList& theStreams,
          const TopoDS_Shape&           theShape,
          const Message_ProgressRange&  theProgress = Message_ProgressRange());

public:
  //! Updates values according the resource file
  //! @param[in] theResource file path to resource or resource value
  //! @param[in] theIsRecursive flag to update all nodes
  //! @return true if theResource has loaded correctly
  Standard_EXPORT bool Load(const TCollection_AsciiString& theResource = "",
                                        const bool theIsRecursive      = true);

  //! Updates values according the resource
  //! @param[in] theResource input resource to use
  //! @param[in] theIsRecursive flag to update all nodes
  //! @return true if theResource has loaded correctly
  Standard_EXPORT bool Load(const occ::handle<DE_ConfigurationContext>& theResource,
                                        const bool theIsRecursive = true);

  //! Writes configuration to the resource file
  //! @param[in] theResourcePath file path to resource
  //! @param[in] theIsRecursive flag to write values of all nodes
  //! @param[in] theFormats list of formats to save. If empty, saves all available
  //! @param[in] theVendors list of providers to save. If empty, saves all available
  //! @return true if the Configuration has saved correctly
  Standard_EXPORT bool
    Save(const TCollection_AsciiString&   theResourcePath,
         const bool           theIsRecursive = true,
         const NCollection_List<TCollection_AsciiString>& theFormats     = NCollection_List<TCollection_AsciiString>(),
         const NCollection_List<TCollection_AsciiString>& theVendors     = NCollection_List<TCollection_AsciiString>());

  //! Writes configuration to the string
  //! @param[in] theIsRecursive flag to write values of all nodes
  //! @param[in] theFormats list of formats to save. If empty, saves all available
  //! @param[in] theVendors list of providers to save. If empty, saves all available
  //! @return result resource string
  Standard_EXPORT TCollection_AsciiString
    Save(const bool           theIsRecursive = true,
         const NCollection_List<TCollection_AsciiString>& theFormats     = NCollection_List<TCollection_AsciiString>(),
         const NCollection_List<TCollection_AsciiString>& theVendors     = NCollection_List<TCollection_AsciiString>());

  //! Creates new node copy and adds to the map
  //! @param[in] theNode input node to copy
  //! @return true if binded
  Standard_EXPORT bool Bind(const occ::handle<DE_ConfigurationNode>& theNode);

  //! Removes node with the same type from the map
  //! @param[in] theNode input node to remove the same
  //! @return true if removed
  Standard_EXPORT bool UnBind(const occ::handle<DE_ConfigurationNode>& theNode);

  //! Finds a node associated with input format and vendor
  //! @param[in] theFormat input node CAD format
  //! @param[in] theVendor input node vendor name
  //! @param[out] theNode output node
  //! @return true if the node is found
  Standard_EXPORT bool Find(const TCollection_AsciiString& theFormat,
                                        const TCollection_AsciiString& theVendor,
                                        occ::handle<DE_ConfigurationNode>&  theNode) const;

  //! Changes provider priority to one format if it exists
  //! @param[in] theFormat input node CAD format
  //! @param[in] theVendorPriority priority of work with vendors
  //! @param[in] theToDisable flag for disabling nodes that are not included in the priority
  Standard_EXPORT void ChangePriority(const TCollection_AsciiString&   theFormat,
                                      const NCollection_List<TCollection_AsciiString>& theVendorPriority,
                                      const bool theToDisable = false);

  //! Changes provider priority to all loaded nodes
  //! @param[in] theVendorPriority priority of work with vendors
  //! @param[in] theToDisable flag for disabling nodes that are not included in the priority
  Standard_EXPORT void ChangePriority(const NCollection_List<TCollection_AsciiString>& theVendorPriority,
                                      const bool theToDisable = false);

  //! Find available provider from the configuration.
  //! If there are several providers, choose the one with the highest priority.
  //! @param[in] thePath path to the CAD file
  //! @param[in] theToImport flag to finds for import. true-import, false-export
  //! @param[out] theProvider created new provider
  //! @return true if provider found and created
  Standard_EXPORT virtual bool FindProvider(const TCollection_AsciiString& thePath,
                                                        const bool         theToImport,
                                                        occ::handle<DE_Provider>& theProvider) const;

  //! Find available read provider from the configuration for file-based operations.
  //! If there are several providers, choose the one with the highest priority.
  //! @param[in] thePath path to the CAD file (for extension and content checking)
  //! @param[in] theCheckContent flag to enable content checking via file reading
  //! @param[out] theProvider created new provider
  //! @return true if provider found and created
  Standard_EXPORT virtual bool FindReadProvider(const TCollection_AsciiString& thePath,
                                                            const bool theCheckContent,
                                                            occ::handle<DE_Provider>& theProvider) const;

  //! Find available read provider from the configuration for stream-based operations.
  //! If there are several providers, choose the one with the highest priority.
  //! @param[in] thePath path to the CAD file (for extension extraction)
  //! @param[in] theStream input stream for content checking
  //! @param[out] theProvider created new provider
  //! @return true if provider found and created
  Standard_EXPORT virtual bool FindReadProvider(const TCollection_AsciiString& thePath,
                                                            std::istream&        theStream,
                                                            occ::handle<DE_Provider>& theProvider) const;

  //! Find available write provider from the configuration.
  //! If there are several providers, choose the one with the highest priority.
  //! @param[in] thePath path to the CAD file (for extension checking only)
  //! @param[out] theProvider created new provider
  //! @return true if provider found and created
  Standard_EXPORT virtual bool FindWriteProvider(
    const TCollection_AsciiString& thePath,
    occ::handle<DE_Provider>&           theProvider) const;

  //! Updates all registered nodes, all changes will be saved in nodes
  //! @param[in] theToForceUpdate flag that turns on/of nodes, according to updated ability to
  //! import/export
  Standard_EXPORT void UpdateLoad(const bool theToForceUpdate = false) const;

  //! Gets flag that keeps changes on configuration nodes which are being updated, false by default
  bool KeepUpdates() const { return myKeepUpdates; }

  //! Sets flag that keeps changes on configuration nodes which are being updated, false by default
  void SetKeepUpdates(const bool theToKeepUpdates) { myKeepUpdates = theToKeepUpdates; }

  //! Gets format map, contains vendor map with nodes
  //! @return internal map of formats
  Standard_EXPORT const NCollection_DataMap<TCollection_AsciiString, NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<DE_ConfigurationNode>>>& Nodes() const;

  //! Copies values of all fields
  //! @return new object with the same field values
  Standard_EXPORT virtual occ::handle<DE_Wrapper> Copy() const;

protected:
  //! Sorts the vendors according to the priority to work
  //! Formats omitted from the resource are not modified
  //! Vendors omitted from the format scope are disabled
  //! @param[in] theResource resource to get priority
  void sort(const occ::handle<DE_ConfigurationContext>& theResource);

public:
  // clang-format off
  DE_ConfigurationNode::DE_SectionGlobal GlobalParameters; //!< Internal parameters for the all translators

private:
  bool myKeepUpdates; //!< Flag that keeps changes on configuration nodes which are being updated
  // clang-format on
  NCollection_DataMap<TCollection_AsciiString, NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<DE_ConfigurationNode>>> myConfiguration; //!< Internal map of formats
};

#endif // _DE_Wrapper_HeaderFile
