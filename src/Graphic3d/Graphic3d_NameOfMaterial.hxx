// Created on: 1993-03-31
// Created by: NW,JPB,CAL
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Graphic3d_NameOfMaterial_HeaderFile
#define _Graphic3d_NameOfMaterial_HeaderFile

//! List of named materials (predefined presets).
//! Each preset defines either physical (having natural color) or generic (mutable color) material (@sa Graphic3d_TypeOfMaterial).
enum Graphic3d_NameOfMaterial
{
  Graphic3d_NameOfMaterial_Brass,           //!< Brass        (Physic)
  Graphic3d_NameOfMaterial_Bronze,          //!< Bronze       (Physic)
  Graphic3d_NameOfMaterial_Copper,          //!< Copper       (Physic)
  Graphic3d_NameOfMaterial_Gold,            //!< Gold         (Physic)
  Graphic3d_NameOfMaterial_Pewter,          //!< Pewter       (Physic)
  Graphic3d_NameOfMaterial_Plastered,       //!< Plastered    (Generic)
  Graphic3d_NameOfMaterial_Plastified,      //!< Plastified   (Generic)
  Graphic3d_NameOfMaterial_Silver,          //!< Silver       (Physic)
  Graphic3d_NameOfMaterial_Steel,           //!< Steel        (Physic)
  Graphic3d_NameOfMaterial_Stone,           //!< Stone        (Physic)
  Graphic3d_NameOfMaterial_ShinyPlastified, //!< Shiny Plastified (Generic)
  Graphic3d_NameOfMaterial_Satin,           //!< Satin        (Generic)
  Graphic3d_NameOfMaterial_Metalized,       //!< Metalized    (Generic)
  Graphic3d_NameOfMaterial_Ionized,         //!< Ionized      (Generic)
  Graphic3d_NameOfMaterial_Chrome,          //!< Chrome       (Physic)
  Graphic3d_NameOfMaterial_Aluminum,        //!< Aluminum     (Physic)
  Graphic3d_NameOfMaterial_Obsidian,        //!< Obsidian     (Physic)
  Graphic3d_NameOfMaterial_Neon,            //!< Neon         (Physic)
  Graphic3d_NameOfMaterial_Jade,            //!< Jade         (Physic)
  Graphic3d_NameOfMaterial_Charcoal,        //!< Charcoal     (Physic)
  Graphic3d_NameOfMaterial_Water,           //!< Water        (Physic)
  Graphic3d_NameOfMaterial_Glass,           //!< Glass        (Physic)
  Graphic3d_NameOfMaterial_Diamond,         //!< Diamond      (Physic)
  Graphic3d_NameOfMaterial_Transparent,     //!< Transparent  (Physic)
  Graphic3d_NameOfMaterial_DEFAULT,         //!< Default      (Generic);
                                            //!  normally used as out-of-range value pointing to some application default
  Graphic3d_NameOfMaterial_UserDefined,     //!< User-defined (Physic);
                                            //!  used for any material with non-standard definition

  // old aliases
  Graphic3d_NOM_BRASS         = Graphic3d_NameOfMaterial_Brass,
  Graphic3d_NOM_BRONZE        = Graphic3d_NameOfMaterial_Bronze,
  Graphic3d_NOM_COPPER        = Graphic3d_NameOfMaterial_Copper,
  Graphic3d_NOM_GOLD          = Graphic3d_NameOfMaterial_Gold,
  Graphic3d_NOM_PEWTER        = Graphic3d_NameOfMaterial_Pewter,
  Graphic3d_NOM_PLASTER       = Graphic3d_NameOfMaterial_Plastered,
  Graphic3d_NOM_PLASTIC       = Graphic3d_NameOfMaterial_Plastified,
  Graphic3d_NOM_SILVER        = Graphic3d_NameOfMaterial_Silver,
  Graphic3d_NOM_STEEL         = Graphic3d_NameOfMaterial_Steel,
  Graphic3d_NOM_STONE         = Graphic3d_NameOfMaterial_Stone,
  Graphic3d_NOM_SHINY_PLASTIC = Graphic3d_NameOfMaterial_ShinyPlastified,
  Graphic3d_NOM_SATIN         = Graphic3d_NameOfMaterial_Satin,
  Graphic3d_NOM_METALIZED     = Graphic3d_NameOfMaterial_Metalized,
  Graphic3d_NOM_NEON_GNC      = Graphic3d_NameOfMaterial_Ionized,
  Graphic3d_NOM_CHROME        = Graphic3d_NameOfMaterial_Chrome,
  Graphic3d_NOM_ALUMINIUM     = Graphic3d_NameOfMaterial_Aluminum,
  Graphic3d_NOM_OBSIDIAN      = Graphic3d_NameOfMaterial_Obsidian,
  Graphic3d_NOM_NEON_PHC      = Graphic3d_NameOfMaterial_Neon,
  Graphic3d_NOM_JADE          = Graphic3d_NameOfMaterial_Jade,
  Graphic3d_NOM_CHARCOAL      = Graphic3d_NameOfMaterial_Charcoal,
  Graphic3d_NOM_WATER         = Graphic3d_NameOfMaterial_Water,
  Graphic3d_NOM_GLASS         = Graphic3d_NameOfMaterial_Glass,
  Graphic3d_NOM_DIAMOND       = Graphic3d_NameOfMaterial_Diamond,
  Graphic3d_NOM_TRANSPARENT   = Graphic3d_NameOfMaterial_Transparent,
  Graphic3d_NOM_DEFAULT       = Graphic3d_NameOfMaterial_DEFAULT,
  Graphic3d_NOM_UserDefined   = Graphic3d_NameOfMaterial_UserDefined
};

#endif // _Graphic3d_NameOfMaterial_HeaderFile
