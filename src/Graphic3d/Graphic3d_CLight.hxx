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

#ifndef _Graphic3d_CLight_HeaderFile
#define _Graphic3d_CLight_HeaderFile

#include <Graphic3d_TypeOfLightSource.hxx>
#include <Graphic3d_Vec.hxx>
#include <NCollection_List.hxx>

//! Light definition
struct Graphic3d_CLight
{

public:

  Graphic3d_Vec3d             Position;    //!< light position
  Graphic3d_Vec4              Color;       //!< light color
  Graphic3d_Vec4              Direction;   //!< direction of directional/spot light
  Graphic3d_Vec4              Params;      //!< packed light parameters
  Standard_ShortReal          Smoothness;  //!< radius (cone angle) for point (directional) light
  Standard_ShortReal          Intensity;   //!< intensity multiplier for light
  Graphic3d_TypeOfLightSource Type;        //!< Graphic3d_TypeOfLightSource enumeration
  Standard_Boolean            IsHeadlight; //!< flag to mark head light

  //! Const attenuation factor of positional light source
  Standard_ShortReal  ConstAttenuation()  const { return Params.x();  }

  //! Linear attenuation factor of positional light source
  Standard_ShortReal  LinearAttenuation() const { return Params.y();  }

  //! Const, Linear attenuation factors of positional light source
  Graphic3d_Vec2      Attenuation()       const { return Params.xy(); }

  //! Angle in radians of the cone created by the spot
  Standard_ShortReal  Angle()             const { return Params.z();  }

  //! Intensity distribution of the spot light, with 0..1 range.
  Standard_ShortReal  Concentration()     const { return Params.w();  }

  Standard_ShortReal& ChangeConstAttenuation()  { return Params.x();  }
  Standard_ShortReal& ChangeLinearAttenuation() { return Params.y();  }
  Graphic3d_Vec2&     ChangeAttenuation()       { return Params.xy(); }
  Standard_ShortReal& ChangeAngle()             { return Params.z();  }
  Standard_ShortReal& ChangeConcentration()     { return Params.w();  }

public:

  //! Empty constructor
  Graphic3d_CLight()
  : Position      (0.0,  0.0,  0.0),
    Color         (1.0f, 1.0f, 1.0f, 1.0f),
    Direction     (0.0f, 0.0f, 0.0f, 0.0f),
    Params        (0.0f, 0.0f, 0.0f, 0.0f),
    Smoothness    (0.0f),
    Intensity     (1.0f),
    Type          (Graphic3d_TOLS_AMBIENT),
    IsHeadlight   (Standard_False)
  {
    //
  }

public:

  DEFINE_STANDARD_ALLOC

};

typedef NCollection_List<Graphic3d_CLight> Graphic3d_ListOfCLight;

#endif // Graphic3d_CLight_HeaderFile
