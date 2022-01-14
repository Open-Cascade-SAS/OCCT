// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _Graphic3d_LightSet_HeaderFile
#define _Graphic3d_LightSet_HeaderFile

#include <Graphic3d_CLight.hxx>

//! Class defining the set of light sources.
class Graphic3d_LightSet : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_LightSet, Standard_Transient)
public:

  //! Iteration filter flags.
  enum IterationFilter
  {
    IterationFilter_None            = 0x0000, //!< no filter
    IterationFilter_ExcludeAmbient  = 0x0002, //!< exclude ambient  light sources
    IterationFilter_ExcludeDisabled = 0x0004, //!< exclude disabled light sources
    IterationFilter_ExcludeNoShadow = 0x0008, //!< exclude light sources not casting shadow
    IterationFilter_ExcludeDisabledAndAmbient = IterationFilter_ExcludeAmbient | IterationFilter_ExcludeDisabled,
    IterationFilter_ActiveShadowCasters = IterationFilter_ExcludeDisabledAndAmbient | IterationFilter_ExcludeNoShadow,
  };

  //! Iterator through light sources.
  class Iterator
  {
  public:
    //! Empty constructor.
    Iterator() : myFilter (0) {}

    //! Constructor with initialization.
    Iterator (const Graphic3d_LightSet& theSet,
              IterationFilter theFilter = IterationFilter_None)
    : myIter (theSet.myLights),
      myFilter (theFilter)
    {
      skipFiltered();
    }

    //! Constructor with initialization.
    Iterator (const Handle(Graphic3d_LightSet)& theSet,
              IterationFilter theFilter = IterationFilter_None)
    : myFilter (theFilter)
    {
      if (!theSet.IsNull())
      {
        myIter = NCollection_IndexedDataMap<Handle(Graphic3d_CLight), Standard_Size>::Iterator (theSet->myLights);
        skipFiltered();
      }
    }

    //! Returns TRUE if iterator points to a valid item.
    Standard_Boolean More() const { return myIter.More(); }

    //! Returns current item.
    const Handle(Graphic3d_CLight)& Value() const { return myIter.Key(); }

    //! Moves to the next item.
    void Next()
    {
      myIter.Next();
      skipFiltered();
    }

  protected:

    //! Skip filtered items.
    void skipFiltered()
    {
      if (myFilter == 0)
      {
        return;
      }

      for (; myIter.More(); myIter.Next())
      {
        if ((myFilter & IterationFilter_ExcludeAmbient) != 0
         && myIter.Key()->Type() == Graphic3d_TypeOfLightSource_Ambient)
        {
          continue;
        }
        else if ((myFilter & IterationFilter_ExcludeDisabled) != 0
              && !myIter.Key()->IsEnabled())
        {
          continue;
        }
        else if ((myFilter & IterationFilter_ExcludeNoShadow) != 0
              && !myIter.Key()->ToCastShadows())
        {
          continue;
        }

        break;
      }
    }

  protected:
    NCollection_IndexedDataMap<Handle(Graphic3d_CLight), Standard_Size>::Iterator myIter;
    Standard_Integer myFilter;
  };

public:

  //! Empty constructor.
  Standard_EXPORT Graphic3d_LightSet();

  //! Return lower light index.
  Standard_Integer Lower() const { return 1; }

  //! Return upper light index.
  Standard_Integer Upper() const { return myLights.Extent(); }

  //! Return TRUE if lights list is empty.
  Standard_Boolean IsEmpty() const { return myLights.IsEmpty(); }

  //! Return number of light sources.
  Standard_Integer Extent() const { return myLights.Extent(); }

  //! Return the light source for specified index within range [Lower(), Upper()].
  const Handle(Graphic3d_CLight)& Value (Standard_Integer theIndex) const { return myLights.FindKey (theIndex); }

  //! Return TRUE if light source is defined in this set.
  Standard_Boolean Contains (const Handle(Graphic3d_CLight)& theLight) const { return myLights.Contains (theLight); }

  //! Append new light source.
  Standard_EXPORT Standard_Boolean Add (const Handle(Graphic3d_CLight)& theLight);

  //! Remove light source.
  Standard_EXPORT Standard_Boolean Remove (const Handle(Graphic3d_CLight)& theLight);

  //! Returns total amount of lights of specified type.
  Standard_Integer NbLightsOfType (Graphic3d_TypeOfLightSource theType) const { return myLightTypes[theType]; }

//! @name cached state of lights set updated by UpdateRevision()
public:

  //! Update light sources revision.
  Standard_EXPORT Standard_Size UpdateRevision();

  //! Return light sources revision.
  //! @sa UpdateRevision()
  Standard_Size Revision() const { return myRevision; }

  //! Returns total amount of enabled lights EXCLUDING ambient.
  //! @sa UpdateRevision()
  Standard_Integer NbEnabled() const { return myNbEnabled; }

  //! Returns total amount of enabled lights of specified type.
  //! @sa UpdateRevision()
  Standard_Integer NbEnabledLightsOfType (Graphic3d_TypeOfLightSource theType) const { return myLightTypesEnabled[theType]; }

  //! Returns total amount of enabled lights castings shadows.
  //! @sa UpdateRevision()
  Standard_Integer NbCastShadows() const { return myNbCastShadows; }

  //! Returns cumulative ambient color, which is computed as sum of all enabled ambient light sources.
  //! Values are NOT clamped (can be greater than 1.0f) and alpha component is fixed to 1.0f.
  //! @sa UpdateRevision()
  const Graphic3d_Vec4& AmbientColor() const { return myAmbient; }

  //! Returns a string defining a list of enabled light sources as concatenation of letters 'd' (Directional), 'p' (Point), 's' (Spot)
  //! depending on the type of light source in the list.
  //! Example: "dppp".
  //! @sa UpdateRevision()
  const TCollection_AsciiString& KeyEnabledLong() const { return myKeyEnabledLong; }

  //! Returns a string defining a list of enabled light sources as concatenation of letters 'd' (Directional), 'p' (Point), 's' (Spot)
  //! depending on the type of light source in the list, specified only once.
  //! Example: "dp".
  //! @sa UpdateRevision()
  const TCollection_AsciiString& KeyEnabledShort() const { return myKeyEnabledShort; }

protected:
  NCollection_IndexedDataMap<Handle(Graphic3d_CLight), Standard_Size>
                          myLights;                 //!< list of light sources with their cached state (revision)
  Graphic3d_Vec4          myAmbient;                //!< cached value of cumulative ambient color
  TCollection_AsciiString myKeyEnabledLong;         //!< key identifying the list of enabled light sources by their type
  TCollection_AsciiString myKeyEnabledShort;        //!< key identifying the list of enabled light sources by the number of sources of each type
  Standard_Integer        myLightTypes       [Graphic3d_TypeOfLightSource_NB]; //!< counters per each light source type defined in the list
  Standard_Integer        myLightTypesEnabled[Graphic3d_TypeOfLightSource_NB]; //!< counters per each light source type enabled in the list
  Standard_Integer        myNbEnabled;              //!< number of enabled light sources, excluding ambient
  Standard_Integer        myNbCastShadows;          //!< number of enabled light sources casting shadows
  Standard_Size           myRevision;               //!< current revision of light source set
  Standard_Size           myCacheRevision;          //!< revision of cached state
};

DEFINE_STANDARD_HANDLE(Graphic3d_LightSet, Standard_Transient)

#endif // _Graphic3d_LightSet_HeaderFile
