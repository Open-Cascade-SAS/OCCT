// Copyright (c) 2024 OPEN CASCADE SAS
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

#ifndef _DE_ShapeFixConfigurationNode_HeaderFile
#define _DE_ShapeFixConfigurationNode_HeaderFile

#include <DE_ConfigurationNode.hxx>
#include <DE_ShapeFixParameters.hxx>
#include <TColStd_ListOfAsciiString.hxx>

class DE_ConfigurationContext;

//! Base class to work with shape healing parameters for child classes.
class DE_ShapeFixConfigurationNode : public DE_ConfigurationNode
{
  DEFINE_STANDARD_RTTIEXT(DE_ShapeFixConfigurationNode, DE_ConfigurationNode)
public:
  //! Initializes all field by default
  Standard_EXPORT DE_ShapeFixConfigurationNode();

  //! Copies values of all fields
  //! @param[in] theConfigurationNode object to copy
  Standard_EXPORT DE_ShapeFixConfigurationNode(
    const Handle(DE_ShapeFixConfigurationNode)& theConfigurationNode);

  //! Updates values according the resource
  //! @param[in] theResource input resource to use
  //! @return True if Load was successful
  Standard_EXPORT virtual bool Load(const Handle(DE_ConfigurationContext)& theResource)
    Standard_OVERRIDE;

  //! Writes configuration to the string
  //! @return result resource string
  Standard_EXPORT virtual TCollection_AsciiString Save() const Standard_OVERRIDE;

public:
  DE_ShapeFixParameters ShapeFixParameters; //!< Shape healing parameters
};

#endif // _DE_ShapeFixConfigurationNode_HeaderFile
