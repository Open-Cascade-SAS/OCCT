// Created on: 2012-11-12
// Created by: Kirill GAVRILOV
// Copyright (c) 2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 65 (the "License") You may not use the content of this file
// except in compliance with the License Please obtain a copy of the License
// at http://wwwopencascadeorg and read it completely before using this file
//
// The Initial Developer of the Original Code is Open CASCADE SAS, having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement Please see the License for the specific terms
// and conditions governing the rights and limitations under the License

#ifndef _Cocoa_Window_H__
#define _Cocoa_Window_H__

#ifdef __OBJC__
  @class NSView;
  @class NSWindow;
#else
  struct NSView;
  struct NSWindow;
#endif

#include <Aspect_Window.hxx>

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>

#include <Aspect_FillMethod.hxx>
#include <Aspect_GradientFillMethod.hxx>
#include <Aspect_Handle.hxx>
#include <Aspect_TypeOfResize.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Quantity_Parameter.hxx>
#include <Quantity_Ratio.hxx>

class Aspect_WindowDefinitionError;
class Aspect_WindowError;
class Aspect_Background;
class Quantity_Color;
class Aspect_GradientBackground;

//! This class defines Cocoa window
class Cocoa_Window : public Aspect_Window
{

public:

  //! Creates a NSWindow and NSView defined by his position and size in pixels
  Standard_EXPORT Cocoa_Window (const Standard_CString theTitle,
                                const Standard_Integer thePxLeft,
                                const Standard_Integer thePxTop,
                                const Standard_Integer thePxWidth,
                                const Standard_Integer thePxHeight);

  //! Creates a wrapper over existing NSView handle
  Standard_EXPORT Cocoa_Window (NSView* theViewNS);

  //! Destroies the Window and all resourses attached to it
  Standard_EXPORT virtual  void Destroy();

  ~Cocoa_Window()
  {
    Destroy();
  }

  //! Modifies the window background
  Standard_EXPORT virtual void SetBackground (const Aspect_Background& theBackground);

  //! Modifies the window background
  Standard_EXPORT virtual void SetBackground (const Quantity_NameOfColor theBackColor);

  //! Modifies the window background
  Standard_EXPORT virtual void SetBackground (const Quantity_Color& theColor);

  //! Modifies the window background
  Standard_EXPORT void SetBackground (const Aspect_Handle theBackPixmap);

  //! Loads the window background from an image file
  Standard_EXPORT Standard_Boolean SetBackground (const Standard_CString  theName,
                                                  const Aspect_FillMethod theMethod = Aspect_FM_CENTERED);

  //! Modifies the window gradient background
  Standard_EXPORT virtual void SetBackground (const Aspect_GradientBackground& theBackground);

  //! Modifies the window gradient background
  Standard_EXPORT void SetBackground (const Quantity_Color& theCol1,
                                      const Quantity_Color& theCol2,
                                      const Aspect_GradientFillMethod theMethod = Aspect_GFM_HOR);

  //! Activates/Deactivates the Double Buffering capability for this window
  Standard_EXPORT virtual void SetDoubleBuffer (const Standard_Boolean theDBmode);

  //! Flushes all graphics to the screen and Swap the Double buffer
  Standard_EXPORT virtual void Flush() const;

  //! Opens the window <me>
  Standard_EXPORT virtual void Map() const;

  //! Closes the window <me>
  Standard_EXPORT virtual void Unmap() const;

  //! Applies the resizing to the window <me>
  Standard_EXPORT virtual Aspect_TypeOfResize DoResize() const;

  //! Apply the mapping change to the window <me>
  Standard_EXPORT virtual Standard_Boolean DoMapping() const;

  //! Clears the Window in the Background color
  Standard_EXPORT virtual void Clear() const;

  //! Clears the Window Area defined by his center and PIXEL size in the Background color
  Standard_EXPORT virtual void ClearArea (const Standard_Integer theCenterX,
                                          const Standard_Integer theCenterY,
                                          const Standard_Integer theWidth,
                                          const Standard_Integer theHeight) const;

  //! Restores The Window from the BackingStored Window
  Standard_EXPORT virtual void Restore() const;

  //! Restores The Window Area defined by his center and PIXEL size from the BackingStored Window
  Standard_EXPORT virtual void RestoreArea (const Standard_Integer theCenterX,
                                            const Standard_Integer theCenterY,
                                            const Standard_Integer theWidth,
                                            const Standard_Integer theHeight) const;

  //! Dumps the Window to file
  Standard_EXPORT virtual Standard_Boolean Dump (const Standard_CString theFilename,
                                                 const Standard_Real    theGammaValue = 1.0) const;

  //! Dumps the Window Area defined by his center and PIXEL size to an image file
  Standard_EXPORT virtual Standard_Boolean DumpArea (const Standard_CString theFilename,
                                                     const Standard_Integer theCenterX,
                                                     const Standard_Integer theCenterY,
                                                     const Standard_Integer theWidth,
                                                     const Standard_Integer theHeight,
                                                     const Standard_Real    theGammaValue = 1.0) const;

  //! Loads the image file to this Window
  //! @return TRUE if the loading occurs normaly
  Standard_EXPORT virtual Standard_Boolean Load (const Standard_CString theFilename) const;

  //! Loads the image file to Window Area
  Standard_EXPORT virtual Standard_Boolean LoadArea (const Standard_CString theFilename,
                                                     const Standard_Integer theCenterX,
                                                     const Standard_Integer theCenterY,
                                                     const Standard_Integer theWidth,
                                                     const Standard_Integer theHeight) const;

  //! Returns the BackingStore capability for this Window
  Standard_EXPORT virtual Standard_Boolean BackingStore() const;

  //! Returns the DoubleBuffer state
  Standard_EXPORT virtual Standard_Boolean DoubleBuffer() const;

  //! Returns True if the window <me> is opened
  Standard_EXPORT virtual Standard_Boolean IsMapped() const;

  //! Returns The Window RATIO equal to the physical WIDTH/HEIGHT dimensions
  Standard_EXPORT virtual Quantity_Ratio Ratio() const;

  //! Returns The Window POSITION in DSU
  Standard_EXPORT virtual void Position (Quantity_Parameter& X1,
                                         Quantity_Parameter& Y1,
                                         Quantity_Parameter& X2,
                                         Quantity_Parameter& Y2) const;

  //! Returns The Window POSITION in PIXEL
  Standard_EXPORT virtual void Position (Standard_Integer& X1,
                                         Standard_Integer& Y1,
                                         Standard_Integer& X2,
                                         Standard_Integer& Y2) const;

  //! Returns The Window SIZE in DSU
  Standard_EXPORT virtual void Size (Quantity_Parameter& theWidth,
                                     Quantity_Parameter& theHeight) const;

  //! Returns The Window SIZE in PIXEL
  Standard_EXPORT virtual void Size (Standard_Integer& theWidth,
                                     Standard_Integer& theHeight) const;

  //! Returns The Window SIZE in MM
  Standard_EXPORT virtual void MMSize (Standard_Real& theWidth,
                                       Standard_Real& theHeight) const;

  //! Returns the DSU value depending of the PIXEL value
  Standard_EXPORT virtual Quantity_Parameter Convert (const Standard_Integer PV) const;

  //! Returns the PIXEL value depending of the DSU value
  Standard_EXPORT virtual Standard_Integer Convert (const Quantity_Parameter DV) const;

  //! Returns the DSU position depending of the PIXEL position
  Standard_EXPORT virtual void Convert (const Standard_Integer PX,
                                        const Standard_Integer PY,
                                        Quantity_Parameter&    DX,
                                        Quantity_Parameter&    DY) const;

  //! Returns the PIXEL position depending of the DSU position
  Standard_EXPORT virtual void Convert (const Quantity_Parameter DX,
                                        const Quantity_Parameter DY,
                                        Standard_Integer&        PX,
                                        Standard_Integer&        PY) const;

  //! @return associated NSView
  Standard_EXPORT NSView* HView() const;

  //! Setup new NSView.
  Standard_EXPORT void SetHView (NSView* theView);

protected:

  NSWindow*        myHWindow;
  NSView*          myHView;
  Standard_Integer myXLeft;
  Standard_Integer myYTop;
  Standard_Integer myXRight;
  Standard_Integer myYBottom;

public:

  DEFINE_STANDARD_RTTI(Cocoa_Window)

};

DEFINE_STANDARD_HANDLE(Cocoa_Window, Aspect_Window)

#endif // _Cocoa_Window_H__
