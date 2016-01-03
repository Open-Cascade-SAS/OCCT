// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef Draw_View_Header
#define Draw_View_Header

#include <gp_Trsf.hxx>
#include <Draw_Window.hxx>

class Draw_Viewer;

class Draw_View : public Draw_Window
{
public:

  //! Constructor
  Draw_View(Standard_Integer theId,
            Draw_Viewer*     theViewer,
            Standard_Integer theX,
            Standard_Integer theY,
            Standard_Integer theWidth,
            Standard_Integer theHeight);

#if defined(_WIN32) || defined(__WIN32__)
  Draw_View(Standard_Integer theId,
            Draw_Viewer*     theViewer,
            Standard_Integer theX,
            Standard_Integer theY,
            Standard_Integer theWidth,
            Standard_Integer theHeight,
            HWND             theWindow);
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
  Draw_View(Standard_Integer theId,
            Draw_Viewer*     theViewer,
            Standard_Integer theX,
            Standard_Integer theY,
            Standard_Integer theWidth,
            Standard_Integer theHeight,
            NSWindow*        theWindow);
#endif

  //! Constructor.
  Draw_View(Standard_Integer theId,
            Draw_Viewer*     theViewer,
            const char*      theTitle);

  //! Destructor.
  ~Draw_View();

public: // @name getters and setters

  //! Sets horizontal offset.
  void SetDx(const Standard_Integer theDx)
  {
    myDx = theDx;
  }

  //! Sets vertical offset.
  void SetDy(const Standard_Integer theDy)
  {
    myDy = theDy;
  }

  //! Sets parameter of zoom.
  void SetZoom(const Standard_Real theZoom)
  {
    myZoom = theZoom;
  }

  //! Sets view matrix.
  void SetMatrix(const gp_Trsf& theMatrix)
  {
    myMatrix = theMatrix;
  }

  //! Sets focal distance.
  void SetFocalDistance(const Standard_Real theDistance)
  {
    myFocalDistance = theDistance;
  }

  //! Gets horizontal offset.
  Standard_Integer GetDx() const
  {
    return myDx;
  }

  //! Gets vertical offset.
  Standard_Integer GetDy() const
  {
    return myDy;
  }

  //! Gets parameter of zoom.
  Standard_Real GetZoom() const
  {
    return myZoom;
  }

  //! Gets matrix of view.
  const gp_Trsf& GetMatrix() const
  {
    return myMatrix;
  }

  //! Gets focal distance.
  Standard_Real GetFocalDistance() const
  {
    return myFocalDistance;
  }

public: //! @name public inline methods

  //! Returns type of view.
  const char* Type()
  {
    return myType;
  }

  //! Returns true value if current view in 2D mode.
  Standard_Boolean Is2D() const
  {
    return myIs2D;
  }

  //! Returns true value if current view in perspective mode.
  Standard_Real IsPerspective() const
  {
    return myIsPers;
  }

public: //! @name view API

  //! Initialize view by the type.
  Standard_Boolean Init(const char* theType);

  //! Transformates view matrix.
  void Transform(const gp_Trsf& theTransformation);

  //! Resets frame of current view.
  void ResetFrame();

  //! Returns parameters of frame corners.
  void GetFrame(Standard_Integer& theX0,Standard_Integer& theY0,
                Standard_Integer& theX1,Standard_Integer& theY1);

  //! Perform window exposing.
  void WExpose();

protected:

  Standard_Integer       myId;
  Draw_Viewer*           myViewer;
  char                   myType[5];
  Standard_Boolean       myIsPers;
  Standard_Boolean       myIs2D;
  Standard_Real          myFocalDistance;
  Standard_Real          myZoom;
  gp_Trsf                myMatrix;
  Standard_Integer       myDx;
  Standard_Integer       myDy;
  Standard_Integer       myFrameX0;
  Standard_Integer       myFrameY0;
  Standard_Integer       myFrameX1;
  Standard_Integer       myFrameY1;
};

#endif
