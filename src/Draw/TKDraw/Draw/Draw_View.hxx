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
  Draw_View(int theId,
            Draw_Viewer*     theViewer,
            int theX,
            int theY,
            int theWidth,
            int theHeight,
            Aspect_Drawable  theWindow = 0);

  //! Constructor.
  Draw_View(int theId, Draw_Viewer* theViewer, const char* theTitle);

  //! Destructor.
  ~Draw_View();

public: // @name getters and setters
  //! Gets horizontal offset.
  int GetDx() const { return myDx; }

  //! Sets horizontal offset.
  void SetDx(const int theDx) { myDx = theDx; }

  //! Gets vertical offset.
  int GetDy() const { return myDy; }

  //! Sets vertical offset.
  void SetDy(const int theDy) { myDy = theDy; }

  //! Gets parameter of zoom.
  double GetZoom() const { return myZoom; }

  //! Sets parameter of zoom.
  void SetZoom(const double theZoom) { myZoom = theZoom; }

  //! Gets matrix of view.
  const gp_Trsf& GetMatrix() const { return myMatrix; }

  //! Sets view matrix.
  void SetMatrix(const gp_Trsf& theMatrix) { myMatrix = theMatrix; }

  //! Gets focal distance.
  double GetFocalDistance() const { return myFocalDistance; }

  //! Sets focal distance.
  void SetFocalDistance(const double theDistance) { myFocalDistance = theDistance; }

  //! Returns type of view.
  const char* Type() { return myType; }

  //! Returns true value if current view in 2D mode.
  bool Is2D() const { return myIs2D; }

  //! Returns true value if current view in perspective mode.
  double IsPerspective() const { return myIsPers; }

public: //! @name view API
  //! Initialize view by the type.
  bool Init(const char* theType);

  //! Transform view matrix.
  void Transform(const gp_Trsf& theTransformation);

  //! Resets frame of current view.
  void ResetFrame();

  //! Returns parameters of frame corners.
  void GetFrame(int& theX0,
                int& theY0,
                int& theX1,
                int& theY1);

  //! Perform window exposing.
  virtual void WExpose() override;

protected:
  int myId;
  Draw_Viewer*     myViewer;
  char             myType[5];
  bool myIsPers;
  bool myIs2D;
  double    myFocalDistance;
  double    myZoom;
  gp_Trsf          myMatrix;
  int myDx;
  int myDy;
  int myFrameX0;
  int myFrameY0;
  int myFrameX1;
  int myFrameY1;
};

#endif
