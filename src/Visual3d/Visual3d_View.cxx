// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

/***********************************************************************

     FONCTION :
     ----------
        Classe Visual3d_View.cxx :


        Declaration of variables specific to views.

        A view is defined by :
                -  ViewManager
                -  ContextView
                -  ViewMapping
                -  ViewOrientation

     HISTORIQUE DES MODIFICATIONS   :
     --------------------------------
      Mars 1992 : NW,JPB,CAL ; Creation.
      04-02-97  : FMN ; Suppression de PSOutput, XWDOutput ...
      22-04-97  : CAL ; Ajout du Plot.
      03-06-97  : FMN ; Correction calcul SetRatio
      06-05-97  : CAL ; Ajout du Clear sur les TOS_COMPUTED.
      28-07-97  : PCT ; Ajout lumiere frontale headlight.
      19-09-97  : CAL ; Remplacement de Window->Position par Window->Size;
      17-10-97  : CAL ; Le Projects nouveau est arrive. (NewProjects)
      24-10-97  : CAL ; Retrait de DownCast.
      24-10-97  : CAL ; Retrait des DataStructure (Vieille maquette inutilisee).
      17-11-97  : FMN ; Ajout DoResize dans Resized()
      20-11-97  : CAL ; Disparition de la dependance avec math
      ??-11-97  : CAL ; Retrait de la dependance avec math. Calcul developpe.
      ??-11-97  : CAL ; Ajout de NumberOfDisplayedStructures
      07-08-97  : PCT ; ajout support texture mapping
      15-01-98  : FMN ; FRA60019 calcul Ratio pour MyViewMappingReset
      15-01-98  : CAL ; Ajout de la transformation d'une TOS_COMPUTED
      26-01-98  : CAL ; Ajout de la methode HaveTheSameOwner
      06-02-98  : FMN ; CTS19079: TOP TEN: Probleme de highlight dans EDesigner.
      05-03-98  : CAL ; GraphicTransform que si Trsf non invalidante
      27-03-98  : CAL ; Ajout de TheStructure = devant Compute (...) !!!!
      16-04-98  : CAL ; Remise a ID de la Trsf sur la COMPUTED
      20-05-98  : CAL ; Perfs. Connection entre structures COMPUTED.
      24-06-98  : CAL ; PRO14526 SetTransparency sur une vue non active.
      17-07-98  : CAL ; PRO14872 test sur aTrsf dans ::SetTransform.
      16-09-98  : BGN ; Points d'entree du Triedre (S3819, Phase 1)
      22-09-98  : BGN ; S3989 (anciennement S3819)
                        TypeOfTriedron* from Aspect (pas Visual3d)
      06-10-98  : CAL ; Ajout d'un TIMER si CSF_GraphicTimer est definie.
      16-10-98  : CAL ; Ajout d'un TIMER si CSF_GraphicTimer est definie.
      02-11-98  : CAL ; Retrait de OSD::SetSignal.
      18-11-98  : FMN ; Correction sur la gestion de la perspective
      02-12-98  : CAL ; S4062. Ajout des layers.
      13-09-99  : GG  ; GER61454 Adds LightLimit() and PlaneLimit() methods
      10-11-99  : GG  ; PRO19603 Add Redraw( area ) method
      14-01-00  : GG  ; IMP140100 Add ViewManager( ) method
      22-10-01  : SAV ; Added EnableDepthTest() method.
      30-04-02  : JMB ; MyDisplayStructure : use a Map instead of a Set (very bad performance
                        in data access when a lot of objects are used)
                        Will improve performance of selection mechanisms

************************************************************************/

#define BUC60572        //GG_03-08-99    Add protection on Zclipping & Zcueing planes
//              positions.

#define GER61454        //GG 14-09-99 Activates model clipping planes

#define RIC120302       //GG Add a NEW SetWindow method which enable
//                      to connect a graphic widget and context to OGL.

//BUC61044 25/10/01 SAV ; added functionality to control gl depth testing from higher API
//BUC61045 25/10/01 SAV ; added functionality to control gl lighting from higher API

//OCC1188 SAV Added methods to set background image

/*----------------------------------------------------------------------*/
/*
 * Constantes
 */

#define NO_DEBUG
#define NO_DESTROY
#define NO_TRACE
#define NO_TRACE_ACCEPT
#define NO_TRACE_CLEAR
#define NO_TRACE_CONNECT
#define NO_TRACE_HIGH
#define NO_TRACE_COMP
#define NO_TRACE_TRSF
#define NO_TRACE_DISPLAY
#define NO_TRACE_ISCOMP
#define NO_TRACE_LENGTH
#define NO_TRACE_LAYER

/*----------------------------------------------------------------------*/
/*
 * Includes
 */

#ifdef WNT
#include <windows.h>
#endif

// for the class
#include <Visual3d_View.ixx>
#include <Visual3d_View.pxx>
#include <Visual3d_DepthCueingDefinitionError.hxx>
#include <Visual3d_ZClippingDefinitionError.hxx>

#ifdef OLD_METHOD
#include <math_Vector.hxx>
#endif /* OLD_METHOD */

#include <OSD.hxx>

#include <Graphic3d_Structure.hxx>
#include <Graphic3d_MapOfStructure.hxx>
#include <Graphic3d_MapIteratorOfMapOfStructure.hxx>
#include <Graphic3d_DataStructureManager.hxx>

#include <Graphic3d_GraphicDriver.hxx>

#include <Graphic3d_Vector.hxx>
#include <Graphic3d_Vertex.hxx>

#include <Visual3d_Light.hxx>
#include <Visual3d_SetOfLight.hxx>
#include <Visual3d_HSetOfLight.hxx>
#include <Visual3d_SetIteratorOfSetOfLight.hxx>

#include <Visual3d_SetIteratorOfSetOfView.hxx>

#include <Graphic3d_TextureEnv.hxx>

#include <TColStd_HArray2OfReal.hxx>

#if (defined(_WIN32) || defined(__WIN32__))
  #include <WNT_Window.hxx>
#elif (defined(__APPLE__) && !defined(MACOSX_USE_GLX))
  #include <Cocoa_Window.hxx>
#else
  #include <Xw_Window.hxx>
#endif

#include <float.h>

//-Aliases

//-Global data definitions

//-Constructors

Visual3d_View::Visual3d_View (const Handle(Visual3d_ViewManager)& AManager):
MyContext (),
MyViewMapping (),
MyViewMappingReset (),
MyViewOrientation (),
MyViewOrientationReset (),
MyTransformation (0, 3, 0, 3),
MyMatrixOfMapping (0, 3, 0, 3),
MyMatrixOfOrientation (0, 3, 0, 3),
MyTOCOMPUTESequence (),
MyCOMPUTEDSequence (),
MyDisplayedStructure ()
{
Standard_Integer i, j;

        MyPtrViewManager        = AManager.operator->();
        MyCView.ViewId          = int (AManager->Identification (this));
        MyCView.Active          = 0;
        MyCView.IsDeleted       = 0;

        MyCView.WsId                  = -1;
        MyCView.DefWindow.IsDefined   = 0;

        MyCView.Context.NbActiveLight = 0;

        for (i=0; i<=3; i++)
                for (j=0; j<=3; j++)
                        if (i == j)     MyTransformation (i, j) = 1.0;
                        else            MyTransformation (i, j) = 0.0;

Standard_Real X, Y, Z;

        (MyViewOrientation.ViewReferencePoint ()).Coord (X, Y, Z);
        MyCView.Orientation.ViewReferencePoint.x        = float (X);
        MyCView.Orientation.ViewReferencePoint.y        = float (Y);
        MyCView.Orientation.ViewReferencePoint.z        = float (Z);
        (MyViewOrientation.ViewReferencePlane ()).Coord (X, Y, Z);
        MyCView.Orientation.ViewReferencePlane.x        = float (X);
        MyCView.Orientation.ViewReferencePlane.y        = float (Y);
        MyCView.Orientation.ViewReferencePlane.z        = float (Z);
        (MyViewOrientation.ViewReferenceUp ()).Coord (X, Y, Z);
        MyCView.Orientation.ViewReferenceUp.x           = float (X);
        MyCView.Orientation.ViewReferenceUp.y           = float (Y);
        MyCView.Orientation.ViewReferenceUp.z           = float (Z);

Standard_Real Sx, Sy, Sz;

        MyViewOrientation.AxialScale(Sx, Sy, Sz);
        MyCView.Orientation.ViewScaleX                  = float (Sx);
        MyCView.Orientation.ViewScaleY                  = float (Sy);
        MyCView.Orientation.ViewScaleZ                  = float (Sz);

        // NKV : 23/07/07 - Define custom MODELVIEW matrix
        MyCView.Orientation.IsCustomMatrix = 0;
        memset( (float*)MyCView.Orientation.ModelViewMatrix, 0, 16*sizeof( float ) );
        MyCView.Orientation.ModelViewMatrix[0][0] =
        MyCView.Orientation.ModelViewMatrix[1][1] =
        MyCView.Orientation.ModelViewMatrix[2][2] =
        MyCView.Orientation.ModelViewMatrix[3][3] = 1.;
        //

Standard_Real um, vm, uM, vM;

        MyCView.Mapping.Projection      = int (MyViewMapping.Projection ());
        (MyViewMapping.ProjectionReferencePoint ()).Coord (X, Y, Z);
        MyCView.Mapping.ProjectionReferencePoint.x      = float (X);
        MyCView.Mapping.ProjectionReferencePoint.y      = float (Y);
        MyCView.Mapping.ProjectionReferencePoint.z      = float (Z);
        MyCView.Mapping.ViewPlaneDistance       =
                        float (MyViewMapping.ViewPlaneDistance ());
        MyCView.Mapping.BackPlaneDistance       =
                        float (MyViewMapping.BackPlaneDistance ());
        MyCView.Mapping.FrontPlaneDistance      =
                        float (MyViewMapping.FrontPlaneDistance ());
        MyViewMapping.WindowLimit (um, vm, uM, vM);
        MyCView.Mapping.WindowLimit.um  = float (um);
        MyCView.Mapping.WindowLimit.vm  = float (vm);
        MyCView.Mapping.WindowLimit.uM  = float (uM);
        MyCView.Mapping.WindowLimit.vM  = float (vM);

        // NKV : 23/07/07 - Define custom MODELVIEW matrix
        MyCView.Mapping.IsCustomMatrix = 0;
        memset( (float*)MyCView.Mapping.ProjectionMatrix, 0, 16*sizeof( float ) );
        MyCView.Mapping.ProjectionMatrix[0][0] =
        MyCView.Mapping.ProjectionMatrix[1][1] =
        MyCView.Mapping.ProjectionMatrix[2][2] =
        MyCView.Mapping.ProjectionMatrix[3][3] = 1.;
        //

        MyCView.Context.ZBufferActivity = -1;

        MyMatOfMapIsEvaluated   = Standard_False;
        MyMatOfOriIsEvaluated   = Standard_False;

        IsInitialized = Standard_False;
        ComputedModeIsActive      = Standard_False;
        MyCView.Backfacing        = 0;

        MyCView.ptrUnderLayer = 0;
        MyCView.ptrOverLayer = 0;
        MyCView.GContext = 0;
        MyCView.GDisplayCB = 0;
        MyCView.GClientData = 0;

        MyGraphicDriver = MyViewManager->GraphicDriver();

}

Visual3d_View::Visual3d_View (const Handle(Visual3d_ViewManager)& AManager, const Visual3d_ViewOrientation& VO, const Visual3d_ViewMapping& VM, const Visual3d_ContextView& CTX):
MyTransformation (0, 3, 0, 3),
MyMatrixOfMapping (0, 3, 0, 3),
MyMatrixOfOrientation (0, 3, 0, 3),
MyTOCOMPUTESequence (),
MyCOMPUTEDSequence (),
MyDisplayedStructure ()
{
Standard_Integer i, j;

        MyPtrViewManager        = AManager.operator->();
        MyViewOrientation       = VO;
        MyViewMapping           = VM;
        MyContext               = CTX;
        MyViewOrientationReset  = VO;
        MyViewMappingReset      = VM;

        MyCView.ViewId          = int (AManager->Identification (this));
        MyCView.Active          = 0;
        MyCView.IsDeleted       = 0;

        MyCView.WsId                  = -1;
        MyCView.DefWindow.IsDefined   = 0;

        MyCView.Context.NbActiveLight = 0;

        for (i=0; i<=3; i++)
                for (j=0; j<=3; j++)
                        if (i == j)     MyTransformation (i, j) = 1.0;
                        else            MyTransformation (i, j) = 0.0;

Standard_Real X, Y, Z;

        (MyViewOrientation.ViewReferencePoint ()).Coord (X, Y, Z);
        MyCView.Orientation.ViewReferencePoint.x        = float (X);
        MyCView.Orientation.ViewReferencePoint.y        = float (Y);
        MyCView.Orientation.ViewReferencePoint.z        = float (Z);
        (MyViewOrientation.ViewReferencePlane ()).Coord (X, Y, Z);
        MyCView.Orientation.ViewReferencePlane.x        = float (X);
        MyCView.Orientation.ViewReferencePlane.y        = float (Y);
        MyCView.Orientation.ViewReferencePlane.z        = float (Z);
        (MyViewOrientation.ViewReferenceUp ()).Coord (X, Y, Z);
        MyCView.Orientation.ViewReferenceUp.x           = float (X);
        MyCView.Orientation.ViewReferenceUp.y           = float (Y);
        MyCView.Orientation.ViewReferenceUp.z           = float (Z);

Standard_Real Sx, Sy, Sz;

        MyViewOrientation.AxialScale(Sx, Sy, Sz);
        MyCView.Orientation.ViewScaleX                  = float (Sx);
        MyCView.Orientation.ViewScaleY                  = float (Sy);
        MyCView.Orientation.ViewScaleZ                  = float (Sz);

        // NKV : 23/07/07 - Define custom MODELVIEW matrix
        if (MyViewOrientation.IsCustomMatrix()) {
          MyCView.Orientation.IsCustomMatrix = 1;
          for ( i = 0; i < 4; i++)
            for ( j = 0; j < 4; j++)
              MyCView.Orientation.ModelViewMatrix[i][j] =
                (Standard_ShortReal)MyViewOrientation.MyModelViewMatrix->Value(i,j);
        }
        else {
          MyCView.Orientation.IsCustomMatrix = 0;
          memset( (float*)MyCView.Orientation.ModelViewMatrix, 0, 16*sizeof( float ) );
          MyCView.Orientation.ModelViewMatrix[0][0] =
          MyCView.Orientation.ModelViewMatrix[1][1] =
          MyCView.Orientation.ModelViewMatrix[2][2] =
          MyCView.Orientation.ModelViewMatrix[3][3] = 1.;
        }
        //

Standard_Real um, vm, uM, vM;

        MyCView.Mapping.Projection      = int (MyViewMapping.Projection ());
        (MyViewMapping.ProjectionReferencePoint ()).Coord (X, Y, Z);
        MyCView.Mapping.ProjectionReferencePoint.x      = float (X);
        MyCView.Mapping.ProjectionReferencePoint.y      = float (Y);
        MyCView.Mapping.ProjectionReferencePoint.z      = float (Z);
        MyCView.Mapping.ViewPlaneDistance       =
                        float (MyViewMapping.ViewPlaneDistance ());
        MyCView.Mapping.BackPlaneDistance       =
                        float (MyViewMapping.BackPlaneDistance ());
        MyCView.Mapping.FrontPlaneDistance      =
                        float (MyViewMapping.FrontPlaneDistance ());
        MyViewMapping.WindowLimit (um, vm, uM, vM);
        MyCView.Mapping.WindowLimit.um  = float (um);
        MyCView.Mapping.WindowLimit.vm  = float (vm);
        MyCView.Mapping.WindowLimit.uM  = float (uM);
        MyCView.Mapping.WindowLimit.vM  = float (vM);

        // NKV : 23/07/07 - Define custom MODELVIEW matrix
        if (MyViewMapping.IsCustomMatrix()) {
          MyCView.Mapping.IsCustomMatrix = 1;
          for ( i = 0; i < 4; i++)
            for ( j = 0; j < 4; j++)
              MyCView.Mapping.ProjectionMatrix[i][j] =
                (Standard_ShortReal)MyViewMapping.MyProjectionMatrix->Value(i,j);
        }
        else {
          MyCView.Mapping.IsCustomMatrix = 0;
          memset( (float*)MyCView.Mapping.ProjectionMatrix, 0, 16*sizeof( float ) );
          MyCView.Mapping.ProjectionMatrix[0][0] =
          MyCView.Mapping.ProjectionMatrix[1][1] =
          MyCView.Mapping.ProjectionMatrix[2][2] =
          MyCView.Mapping.ProjectionMatrix[3][3] = 1.;
        }
        //

        MyCView.Context.ZBufferActivity = -1;

        MyMatOfMapIsEvaluated   = Standard_False;
        MyMatOfOriIsEvaluated   = Standard_False;

        IsInitialized = Standard_False;
        ComputedModeIsActive      = Standard_False;

        MyCView.ptrUnderLayer = 0;
        MyCView.ptrOverLayer = 0;
        MyCView.GContext = 0;
        MyCView.GDisplayCB = 0;
        MyCView.GClientData = 0;

        MyGraphicDriver = MyViewManager->GraphicDriver();

}

//-Destructors

//-Methods, in order
// RIC120302
void Visual3d_View::SetWindow (const Handle(Aspect_Window)&      AWindow,
                               const Aspect_RenderingContext     AContext,
                               const Aspect_GraphicCallbackProc& ADisplayCB,
                               const Standard_Address            AClientData)
{
  if (IsDeleted ()) return;

  if (IsDefined ())
    Visual3d_ViewDefinitionError::Raise ("Window already defined");

  MyCView.GContext        = AContext;
  MyCView.GDisplayCB      = ADisplayCB;
  MyCView.GClientData     = AClientData;
  SetWindow(AWindow);
}
// RIC120302

void Visual3d_View::SetWindow (const Handle(Aspect_Window)& theWindow)
{

        if (IsDeleted ()) return;

        if (IsDefined ())
                Visual3d_ViewDefinitionError::Raise ("Window already defined");

        MyWindow        = theWindow;
        MyCView.WsId                    = MyCView.ViewId;
        MyCView.DefWindow.IsDefined     = 1;

#if (defined(_WIN32) || defined(__WIN32__))
  const Handle(WNT_Window) aWin   = Handle(WNT_Window)::DownCast (theWindow);
  MyCView.DefWindow.XWindow       = (HWND )(aWin->HWindow());
  MyCView.DefWindow.XParentWindow = (HWND )(aWin->HParentWindow());
  WNT_WindowData* aWinData = (WNT_WindowData* )GetWindowLongPtr ((HWND )(aWin->HWindow()), GWLP_USERDATA);
  aWinData->WNT_WDriver_Ptr = (void* )this;
  aWinData->WNT_VMgr        = (void* )MyPtrViewManager;
#elif (defined(__APPLE__) && !defined(MACOSX_USE_GLX))
  const Handle(Cocoa_Window) aWin = Handle(Cocoa_Window)::DownCast (theWindow);
  MyCView.DefWindow.XWindow       = (Aspect_Drawable )aWin->HView();
  MyCView.DefWindow.XParentWindow = NULL;
  //MyCView.DefWindow.XParentWindow = aWin->HParentWindow();
#else
  const Handle(Xw_Window) aWin    = Handle(Xw_Window)::DownCast (theWindow);
  MyCView.DefWindow.XWindow       = aWin->XWindow();
  //MyCView.DefWindow.XParentWindow = aWin->XParentWindow();
#endif

        Standard_Integer Width, Height;
        theWindow->Size (Width, Height);

        MyCView.DefWindow.dx    = float( Width );
        MyCView.DefWindow.dy    = float( Height );

        Standard_Real R, G, B;
        MyBackground    = MyWindow->Background ();
        (MyBackground.Color ()).Values (R, G, B, Quantity_TOC_RGB);
        MyCView.DefWindow.Background.r  = float (R);
        MyCView.DefWindow.Background.g  = float (G);
        MyCView.DefWindow.Background.b  = float (B);

        UpdateView ();
        if (! MyGraphicDriver->View (MyCView))
                Visual3d_ViewDefinitionError::Raise ("Association failed");

        MyGradientBackground = MyWindow->GradientBackground();
        SetGradientBackground(MyGradientBackground,1);

        Standard_Boolean AWait = Standard_False;        // => immediate update
        MyGraphicDriver->SetVisualisation (MyCView);
        MyGraphicDriver->AntiAliasing (MyCView, MyContext.AliasingIsOn ());
        MyGraphicDriver->DepthCueing (MyCView, MyContext.DepthCueingIsOn ());
        MyGraphicDriver->ClipLimit (MyCView, AWait);
        MyGraphicDriver->Environment(MyCView);

        // Make view manager z layer list consistent with the view's list.
        MyViewManager->InstallZLayers (this);

        // Update planses of model clipping
        UpdatePlanes ();

        // Update light sources
        UpdateLights ();

        /*
         * Association view-window does not cause the display
         * of structures that can be displayed in the new view.
         * In fact, association view-window is done, but the
         * display is produced only if the view is activated (Activate).
         */

        SetRatio ();

}

Handle(Aspect_Window) Visual3d_View::Window () const {

        if (! IsDefined ())
                Visual3d_ViewDefinitionError::Raise ("Window not defined");

        return (MyWindow);

}

Standard_Boolean Visual3d_View::IsDefined () const {

        if (MyCView.DefWindow.IsDefined)
                return (Standard_True);
        else
                return (Standard_False);

}

Standard_Boolean Visual3d_View::IsDeleted () const {

        if (MyCView.IsDeleted)
                return (Standard_True);
        else
                return (Standard_False);

}

void Visual3d_View::Destroy () {

#ifdef DESTROY
        cout << "Visual3d_View::Destroy (" << Identification () << ")\n";
#endif

        // Since MyPtrViewManager can be already distroyed,
        // avoid attempts to access it in SetBackground()
        MyPtrViewManager = 0;
        Remove ();

}

void Visual3d_View::Remove () {

#ifdef DESTROY
        cout << "Visual3d_View::Remove (" << Identification () << ")" << endl;
#endif

        if (IsDeleted ()) return;
        if (! IsDefined ()) return;

        MyTOCOMPUTESequence.Clear ();
        MyCOMPUTEDSequence.Clear ();
        MyDisplayedStructure.Clear ();

        Quantity_Color BlackColor (0.0, 0.0, 0.0, Quantity_TOC_RGB);
        Aspect_Background BlackBackground (BlackColor);
        SetBackground (BlackBackground);

        Aspect_GradientBackground BlackGradBackground;
        SetGradientBackground (BlackGradBackground,0);

	if (MyPtrViewManager)
	  MyPtrViewManager->UnIdentification( MyCView.ViewId );

        MyGraphicDriver->RemoveView (MyCView);

        MyCView.WsId            = -1;
        MyCView.IsDeleted       = 1;
        MyCView.DefWindow.IsDefined     = 0;

        MyMatOfMapIsEvaluated   = Standard_False;
        MyMatOfOriIsEvaluated   = Standard_False;

        IsInitialized = Standard_False;

        MyWindow.Nullify ();

}

void Visual3d_View::Resized () {

        if (IsDeleted ()) return;

        if (! IsDefined ())
                Visual3d_ViewDefinitionError::Raise ("Window not defined");
        MyWindow->DoResize() ;
        SetRatio ();

}

void Visual3d_View::SetRatio () {

        if (IsDeleted ()) return;

Aspect_TypeOfUpdate UpdateMode = MyViewManager->UpdateMode ();
        MyViewManager->SetUpdateMode (Aspect_TOU_WAIT);

Standard_Real Umin, Vmin, Umax, Vmax;
Standard_Integer Dxw, Dyw;
Standard_Real Dxv, Dyv;
Standard_Real Xc, Yc;
Standard_Real Rap;

        Rap     = 0. ;
        MyWindow->Size (Dxw, Dyw);
        MyViewMapping.WindowLimit(Umin,Vmin,Umax,Vmax) ;
        Xc      = (Umin + Umax)/2. ; Yc = (Vmin + Vmax)/2. ;
        Dxv     = Umax - Umin ; Dyv     = Vmax - Vmin ;
        if( Dxw > 0 && Dyw > 0 ) Rap = (Standard_Real)Dyw/(Standard_Real)Dxw ;
        if( Rap > 0.0 ) {

            if( Dxv <= Dyv )
            {
                if (Rap <= 1.0)
                {
                    Dyv = Dxv;
                    Dxv = Dxv/Rap;
                }
                else
                {
                    Dxv = Dxv;
                    Dyv = Dxv*Rap;
                }
            }
            else
            {
                if (Rap <= 1.0)
                {
                    Dxv = Dyv/Rap;
                    Dyv = Dyv;
                }
                else
                {
                    Dxv = Dyv;
                    Dyv = Dyv*Rap;
                }
            }

            Umin        = Xc - Dxv/2. ; Vmin    = Yc - Dyv/2. ;
            Umax        = Xc + Dxv/2. ; Vmax    = Yc + Dyv/2. ;

            MyViewMapping.SetWindowLimit(Umin,Vmin,Umax,Vmax) ;

            // Update before SetViewMapping.

            MyCView.DefWindow.dx        = float( Dxw );
            MyCView.DefWindow.dy        = float( Dyw );

            SetViewMapping (MyViewMapping);
//          SetViewMappingDefault ();
            // FMN+ Update Ratio for MyViewMappingReset

            MyViewMappingReset.WindowLimit(Umin,Vmin,Umax,Vmax) ;
            Xc  = (Umin + Umax)/2. ; Yc = (Vmin + Vmax)/2. ;
            Dxv = Umax - Umin ; Dyv     = Vmax - Vmin ;

            if( Dxv <= Dyv )
            {
                if (Rap <= 1.0)
                {
                    Dyv = Dxv;
                    Dxv = Dxv/Rap;
                }
                else
                {
                    Dxv = Dxv;
                    Dyv = Dxv*Rap;
                }
            }
            else
            {
                if (Rap <= 1.0)
                {
                    Dxv = Dyv/Rap;
                    Dyv = Dyv;
                }
                else
                {
                    Dxv = Dyv;
                    Dyv = Dyv*Rap;
                }
            }

            Umin        = Xc - Dxv/2. ; Vmin    = Yc - Dyv/2. ;
            Umax        = Xc + Dxv/2. ; Vmax    = Yc + Dyv/2. ;

            MyViewMappingReset.SetWindowLimit(Umin,Vmin,Umax,Vmax) ;

            // FMN- Update Ratio for MyViewMappingReset

            MyGraphicDriver->RatioWindow (MyCView);
        }

        // Force recalculation of 2 matrices.
        //
        // The current view can help to reconstruct a copied view
        // that is itself. Owing to SetWindow and SetRatio the
        // recalculation of matrices of this new view is imposed.
        MyMatOfMapIsEvaluated   = Standard_False;
        MyMatOfOriIsEvaluated   = Standard_False;

        MyViewManager->SetUpdateMode (UpdateMode);
        if (UpdateMode == Aspect_TOU_ASAP) Update ();

}

void Visual3d_View::UpdateLights()
{
  if (IsDeleted()
   || !IsDefined())
  {
    return;
  }

  if (MyContext.Model() == Visual3d_TOM_NONE)
  {
    // activate only a white ambient light
    Graphic3d_CLight aCLight;
    aCLight.Type        = Visual3d_TOLS_AMBIENT;
    aCLight.IsHeadlight = Standard_False;
    aCLight.Color.r() = aCLight.Color.g() = aCLight.Color.b() = 1.0f;

    MyCView.Context.NbActiveLight = 1;
    MyCView.Context.ActiveLight   = &aCLight;
    MyGraphicDriver->SetLight (MyCView);
    MyCView.Context.ActiveLight   = NULL;
    return;
  }

  MyCView.Context.NbActiveLight = Min (MyContext.NumberOfActivatedLights(),
                                       MyGraphicDriver->InquireLightLimit());
  if (MyCView.Context.NbActiveLight < 1)
  {
    MyGraphicDriver->SetLight (MyCView);
    return;
  }

  // parcing of light sources
  MyCView.Context.ActiveLight = new Graphic3d_CLight[MyCView.Context.NbActiveLight];
  for (Standard_Integer aLightIter = 0; aLightIter < MyCView.Context.NbActiveLight; ++aLightIter)
  {
    MyCView.Context.ActiveLight[aLightIter] = MyContext.ActivatedLight (aLightIter + 1)->CLight();
  }
  MyGraphicDriver->SetLight (MyCView);
  delete[] MyCView.Context.ActiveLight;
  MyCView.Context.ActiveLight = NULL;
}

void Visual3d_View::UpdatePlanes()
{
  MyCView.Context.ClipPlanes = MyContext.ClipPlanes();
  if (IsDeleted() || !IsDefined())
  {
    return;
  }

  MyGraphicDriver->SetClipPlanes (MyCView);
}

void Visual3d_View::SetBackground (const Aspect_Background& ABack) {

        if (IsDeleted ()) return;

        if (! IsDefined ())
                Visual3d_ViewDefinitionError::Raise ("Window not defined");

        // At this level, only GL can update the background.
        // It is not necessary to call MyWindow->SetBackground (ABack); as
        // this method starts update of window background by X
        // (if the windowing is X)

        Standard_Real R, G, B;
        MyBackground    = ABack;
        (MyBackground.Color ()).Values (R, G, B, Quantity_TOC_RGB);
        MyCView.DefWindow.Background.r  = float (R);
        MyCView.DefWindow.Background.g  = float (G);
        MyCView.DefWindow.Background.b  = float (B);

        MyGraphicDriver->Background (MyCView);

        if (MyPtrViewManager && MyViewManager->UpdateMode () == Aspect_TOU_ASAP)
          Update ();

}

void Visual3d_View::SetGradientBackground(const Aspect_GradientBackground& ABack, const Standard_Boolean update)
{

  if (IsDeleted ()) return;

  if (! IsDefined ())
    Visual3d_ViewDefinitionError::Raise ("Window not defined");

  MyGradientBackground = ABack;
  Quantity_Color aCol1,aCol2;
  MyGradientBackground.Colors(aCol1,aCol2);
  MyGraphicDriver->GradientBackground(MyCView, aCol1, aCol2, MyGradientBackground.BgGradientFillMethod());

  if ( update )
     Update ();
  else if (MyPtrViewManager && MyViewManager->UpdateMode () == Aspect_TOU_ASAP)
    Update();
}

void Visual3d_View::SetBackgroundImage( const Standard_CString FileName,
                                        const Aspect_FillMethod FillStyle,
                                        const Standard_Boolean update )
{
  if ( IsDeleted() )
    return;
  if ( !IsDefined() )
    Visual3d_ViewDefinitionError::Raise ("Window not defined");

  MyGraphicDriver->BackgroundImage( FileName, MyCView, FillStyle );

  if ( update )
    Update();
  else if ( MyViewManager->UpdateMode() == Aspect_TOU_ASAP )
    Update();
}

void Visual3d_View::SetBgImageStyle( const Aspect_FillMethod FillStyle,
                                     const Standard_Boolean update )
{
  if ( IsDeleted() )
    return;
  if ( !IsDefined() )
    Visual3d_ViewDefinitionError::Raise ("Window not defined");

  MyGraphicDriver->SetBgImageStyle( MyCView, FillStyle );

  if ( update )
    Update();
  else if ( MyViewManager->UpdateMode() == Aspect_TOU_ASAP )
    Update();
}

Aspect_Background Visual3d_View::Background () const {

        return (MyBackground);

}

void Visual3d_View::SetBgGradientStyle( const Aspect_GradientFillMethod FillStyle,
                                        const Standard_Boolean update )
{
  if ( IsDeleted() )
    return;
  if ( !IsDefined() )
    Visual3d_ViewDefinitionError::Raise ("Window not defined");

  MyGraphicDriver->SetBgGradientStyle( MyCView, FillStyle );

  if ( update )
    Update();
  else if ( MyViewManager->UpdateMode() == Aspect_TOU_ASAP )
    Update();

}

Aspect_GradientBackground Visual3d_View::GradientBackground () const {

        return MyGradientBackground;

}

void Visual3d_View::SetTransform (const TColStd_Array2OfReal& AMatrix) {

        if (IsDeleted ()) return;

Standard_Integer lr, ur, lc, uc;
Standard_Integer i, j;

        // Assign the new transformation in an array [0..3][0..3]
        // Avoid problems if the has defined a matrice [1..4][1..4]
        //                                                 ou [3..6][-1..2] !!
        lr      = AMatrix.LowerRow ();
        ur      = AMatrix.UpperRow ();
        lc      = AMatrix.LowerCol ();
        uc      = AMatrix.UpperCol ();

        if ( (ur - lr + 1 != 4) || (uc - lc + 1 != 4) )
                Visual3d_TransformError::Raise ("Transform : not a 4x4 matrix");

        for (i=0; i<=3; i++)
        for (j=0; j<=3; j++)
                MyTransformation (i, j) = AMatrix (lr + i, lc + j);

Graphic3d_Vector VPN;
Graphic3d_Vertex VRP;
Graphic3d_Vector VUP;
Standard_Real Sx, Sy, Sz;

Visual3d_ViewOrientation NewViewOrientation;

        VPN     = MyViewOrientation.ViewReferencePlane ();
        VRP     = MyViewOrientation.ViewReferencePoint ();
        VUP     = MyViewOrientation.ViewReferenceUp ();
	MyViewOrientation.AxialScale(Sx, Sy, Sz);

        NewViewOrientation.SetViewReferencePlane
                (Graphic3d_Structure::Transforms (Transform (), VPN));

        NewViewOrientation.SetViewReferencePoint
                (Graphic3d_Structure::Transforms (Transform (), VRP));

        NewViewOrientation.SetViewReferenceUp
                (Graphic3d_Structure::Transforms (Transform (), VUP));
	NewViewOrientation.SetAxialScale(Sx, Sy, Sz);

        SetViewOrientation (NewViewOrientation);

        MyMatOfOriIsEvaluated   = Standard_False;

}

const TColStd_Array2OfReal& Visual3d_View::Transform () const {

        return (MyTransformation);

}

void Visual3d_View::SetViewOrientation (const Visual3d_ViewOrientation& VO) {

        if (IsDeleted ()) return;

        MyViewOrientation       = VO;

        Standard_Real X, Y, Z;
        // Tests on modification of parameters.
        Standard_Boolean VUPIsModified  = Standard_False;
        Standard_Boolean VRPIsModified  = Standard_False;
        Standard_Boolean VRUIsModified  = Standard_False;
        Standard_Boolean ScaleIsModified  = Standard_False;
        Standard_Boolean CustomIsModified = Standard_False;

        (MyViewOrientation.ViewReferencePoint ()).Coord (X, Y, Z);
        VUPIsModified =
           MyCView.Orientation.ViewReferencePoint.x != float (X)
        || MyCView.Orientation.ViewReferencePoint.y != float (Y)
        || MyCView.Orientation.ViewReferencePoint.z != float (Z);
        MyCView.Orientation.ViewReferencePoint.x        = float (X);
        MyCView.Orientation.ViewReferencePoint.y        = float (Y);
        MyCView.Orientation.ViewReferencePoint.z        = float (Z);

        (MyViewOrientation.ViewReferencePlane ()).Coord (X, Y, Z);
        VRPIsModified =
           MyCView.Orientation.ViewReferencePlane.x != float (X)
        || MyCView.Orientation.ViewReferencePlane.y != float (Y)
        || MyCView.Orientation.ViewReferencePlane.z != float (Z);
        MyCView.Orientation.ViewReferencePlane.x        = float (X);
        MyCView.Orientation.ViewReferencePlane.y        = float (Y);
        MyCView.Orientation.ViewReferencePlane.z        = float (Z);

        (MyViewOrientation.ViewReferenceUp ()).Coord (X, Y, Z);
        VRUIsModified =
           MyCView.Orientation.ViewReferenceUp.x != float (X)
        || MyCView.Orientation.ViewReferenceUp.y != float (Y)
        || MyCView.Orientation.ViewReferenceUp.z != float (Z);
        MyCView.Orientation.ViewReferenceUp.x           = float (X);
        MyCView.Orientation.ViewReferenceUp.y           = float (Y);
        MyCView.Orientation.ViewReferenceUp.z           = float (Z);

        MyViewOrientation.AxialScale(X, Y, Z);
        ScaleIsModified =
           MyCView.Orientation.ViewScaleX != float (X)
        || MyCView.Orientation.ViewScaleY != float (Y)
        || MyCView.Orientation.ViewScaleZ != float (Z);
        MyCView.Orientation.ViewScaleX                  = float (X);
        MyCView.Orientation.ViewScaleY                  = float (Y);
        MyCView.Orientation.ViewScaleZ                  = float (Z);

        CustomIsModified =
          (MyCView.Orientation.IsCustomMatrix != MyViewOrientation.IsCustomMatrix());
        MyCView.Orientation.IsCustomMatrix = MyViewOrientation.IsCustomMatrix();
        if ( MyViewOrientation.IsCustomMatrix() ) {
          Standard_Integer i, j;
          for (i = 0; i < 4; i++)
            for (j = 0; j < 4; j++) {
             if (!CustomIsModified) CustomIsModified =
               MyCView.Orientation.ModelViewMatrix[i][j] != MyViewOrientation.MyModelViewMatrix->Value(i,j);
            }
        }

#ifdef TRACE_TRSF
        cout << "Visual3d_View::SetViewOrientation\n";
        if (VUPIsModified || VRPIsModified || VRUIsModified || ScaleIsModified || CustomIsModified)
          cout <<   "VUPIsModified : " << VUPIsModified
          << ", VRPIsModified : " << VRPIsModified
          << ", VRUIsModified : " << VRUIsModified
          << ", CustomIsModified : " << CustomIsModified
          << ", ScaleIsModified : " << ScaleIsModified << "\n" << flush;
        else
          cout << "no modification\n" << flush;
#endif

        // restart if one of parameters is modified
        if (!IsInitialized || VUPIsModified || VRPIsModified
            || VRUIsModified || ScaleIsModified || CustomIsModified) {

        MyMatOfOriIsEvaluated = !VUPIsModified && !VRPIsModified
                              && !VRUIsModified && !ScaleIsModified;

        if (! IsDefined ()) return;

        Standard_Boolean AWait = Standard_False;        // => immediate update
        MyGraphicDriver->ViewOrientation (MyCView, AWait);
        IsInitialized = Standard_True;
        Compute ();

        if (MyViewManager->UpdateMode () == Aspect_TOU_ASAP) Update ();
        }

}

Visual3d_ViewOrientation Visual3d_View::ViewOrientation () const {

        return (MyViewOrientation);

}

Visual3d_ViewOrientation Visual3d_View::ViewOrientationDefault () const {

        return (MyViewOrientationReset);

}

void Visual3d_View::SetViewOrientationDefault () {

        MyViewOrientationReset.Assign (MyViewOrientation);

}

void Visual3d_View::ViewOrientationReset () {

        if (IsDeleted ()) return;

        MyViewOrientation       = MyViewOrientationReset;

        Standard_Real X, Y, Z;

        // Tests on modification of parameters.
        Standard_Boolean VUPIsModified  = Standard_False;
        Standard_Boolean VRPIsModified  = Standard_False;
        Standard_Boolean VRUIsModified  = Standard_False;
        Standard_Boolean ScaleIsModified  = Standard_False;
        Standard_Boolean CustomIsModified = Standard_False;

        (MyViewOrientation.ViewReferencePoint ()).Coord (X, Y, Z);
        VUPIsModified =
           MyCView.Orientation.ViewReferencePoint.x != float (X)
        || MyCView.Orientation.ViewReferencePoint.y != float (Y)
        || MyCView.Orientation.ViewReferencePoint.z != float (Z);
        MyCView.Orientation.ViewReferencePoint.x        = float (X);
        MyCView.Orientation.ViewReferencePoint.y        = float (Y);
        MyCView.Orientation.ViewReferencePoint.z        = float (Z);

        (MyViewOrientation.ViewReferencePlane ()).Coord (X, Y, Z);
        VRPIsModified =
           MyCView.Orientation.ViewReferencePlane.x != float (X)
        || MyCView.Orientation.ViewReferencePlane.y != float (Y)
        || MyCView.Orientation.ViewReferencePlane.z != float (Z);
        MyCView.Orientation.ViewReferencePlane.x        = float (X);
        MyCView.Orientation.ViewReferencePlane.y        = float (Y);
        MyCView.Orientation.ViewReferencePlane.z        = float (Z);

        (MyViewOrientation.ViewReferenceUp ()).Coord (X, Y, Z);
        VRUIsModified =
           MyCView.Orientation.ViewReferenceUp.x != float (X)
        || MyCView.Orientation.ViewReferenceUp.y != float (Y)
        || MyCView.Orientation.ViewReferenceUp.z != float (Z);
        MyCView.Orientation.ViewReferenceUp.x           = float (X);
        MyCView.Orientation.ViewReferenceUp.y           = float (Y);
        MyCView.Orientation.ViewReferenceUp.z           = float (Z);

        MyViewOrientation.AxialScale(X, Y, Z);
        ScaleIsModified =
           MyCView.Orientation.ViewScaleX != float (X)
        || MyCView.Orientation.ViewScaleY != float (Y)
        || MyCView.Orientation.ViewScaleZ != float (Z);
        MyCView.Orientation.ViewScaleX                  = float (X);
        MyCView.Orientation.ViewScaleY                  = float (Y);
        MyCView.Orientation.ViewScaleZ                  = float (Z);

        CustomIsModified =
        MyCView.Orientation.IsCustomMatrix != MyViewOrientation.IsCustomMatrix();
        MyCView.Orientation.IsCustomMatrix = MyViewOrientation.IsCustomMatrix();
        if ( MyViewOrientation.IsCustomMatrix() ) {
          Standard_Integer i, j;
          for (i = 0; i < 4; i++)
            for (j = 0; j < 4; j++) {
             if (!CustomIsModified) CustomIsModified =
               MyCView.Orientation.ModelViewMatrix[i][j] != MyViewOrientation.MyModelViewMatrix->Value(i,j);
            }
        }


#ifdef TRACE_TRSF
        cout << "Visual3d_View::ViewOrientationReset\n";
        if (VUPIsModified || VRPIsModified || VRUIsModified || ScaleIsModified || CustomIsModified)
          cout <<   "VUPIsModified : " << VUPIsModified
          << ", VRPIsModified : " << VRPIsModified
          << ", VRUIsModified : " << VRUIsModified
          << ", CustomIsModified : " << CustomIsModified
          << ", ScaleIsModified : " << ScaleIsModified << "\n" << flush;
        else
          cout << "no modification\n" << flush;
#endif

        // Restart if one of parameters is modified
        if (!IsInitialized || VUPIsModified || VRPIsModified
            || VRUIsModified || ScaleIsModified || CustomIsModified) {

        MyMatOfOriIsEvaluated = !VUPIsModified && !VRPIsModified
                              && !VRUIsModified && !ScaleIsModified;

        if (! IsDefined ()) return;

        Standard_Boolean AWait = Standard_False;        // => immediate update
        MyGraphicDriver->ViewOrientation (MyCView, AWait);
        IsInitialized = Standard_True;
        Compute ();

        if (MyViewManager->UpdateMode () == Aspect_TOU_ASAP) Update ();
        }
}

void Visual3d_View::SetViewMapping (const Visual3d_ViewMapping& VM) {

        if (IsDeleted ()) return;

Visual3d_TypeOfProjection OldType = MyViewMapping.Projection ();
Visual3d_TypeOfProjection NewType = VM.Projection ();

        MyViewMapping   = VM;

Standard_Real X, Y, Z;
Standard_Real um, vm, uM, vM;

        MyCView.Mapping.Projection      = int (MyViewMapping.Projection ());
        (MyViewMapping.ProjectionReferencePoint ()).Coord (X, Y, Z);
        MyCView.Mapping.ProjectionReferencePoint.x      = float (X);
        MyCView.Mapping.ProjectionReferencePoint.y      = float (Y);
        MyCView.Mapping.ProjectionReferencePoint.z      = float (Z);
        MyCView.Mapping.ViewPlaneDistance       =
                        float (MyViewMapping.ViewPlaneDistance ());
        MyCView.Mapping.BackPlaneDistance       =
                        float (MyViewMapping.BackPlaneDistance ());
        MyCView.Mapping.FrontPlaneDistance      =
                        float (MyViewMapping.FrontPlaneDistance ());
        MyViewMapping.WindowLimit (um, vm, uM, vM);
        MyCView.Mapping.WindowLimit.um  = float (um);
        MyCView.Mapping.WindowLimit.vm  = float (vm);
        MyCView.Mapping.WindowLimit.uM  = float (uM);
        MyCView.Mapping.WindowLimit.vM  = float (vM);

        MyCView.Mapping.IsCustomMatrix = MyViewMapping.IsCustomMatrix();
        if (MyViewMapping.IsCustomMatrix()) {
          Standard_Integer i, j;
          for (i = 0; i < 4; i++)
            for (j = 0; j < 4; j++)
              MyCView.Mapping.ProjectionMatrix[i][j] =
                (Standard_ShortReal)MyViewMapping.MyProjectionMatrix->Value(i,j);
        }

        MyMatOfMapIsEvaluated   = Standard_False;

        if (! IsDefined ()) return;

Standard_Boolean AWait = Standard_False;        // => immediate update
        MyGraphicDriver->ViewMapping (MyCView, AWait);

        // Passage Parallele/Perspective
        if (OldType != NewType)
                Compute ();

        if (MyViewManager->UpdateMode () == Aspect_TOU_ASAP) Update ();

}

Visual3d_ViewMapping Visual3d_View::ViewMapping () const {

        return (MyViewMapping);

}

Visual3d_ViewMapping Visual3d_View::ViewMappingDefault () const {

        return (MyViewMappingReset);

}

void Visual3d_View::SetViewMappingDefault () {

        MyViewMappingReset.Assign (MyViewMapping);

}

void Visual3d_View::ViewMappingReset () {

        if (IsDeleted ()) return;

        MyViewMapping   = MyViewMappingReset;

Standard_Real X, Y, Z;
Standard_Real um, vm, uM, vM;

        MyCView.Mapping.Projection      = int (MyViewMapping.Projection ());
        (MyViewMapping.ProjectionReferencePoint ()).Coord (X, Y, Z);
        MyCView.Mapping.ProjectionReferencePoint.x      = float (X);
        MyCView.Mapping.ProjectionReferencePoint.y      = float (Y);
        MyCView.Mapping.ProjectionReferencePoint.z      = float (Z);
        MyCView.Mapping.ViewPlaneDistance       =
                        float (MyViewMapping.ViewPlaneDistance ());
        MyCView.Mapping.BackPlaneDistance       =
                        float (MyViewMapping.BackPlaneDistance ());
        MyCView.Mapping.FrontPlaneDistance      =
                        float (MyViewMapping.FrontPlaneDistance ());
        MyViewMapping.WindowLimit (um, vm, uM, vM);
        MyCView.Mapping.WindowLimit.um  = float (um);
        MyCView.Mapping.WindowLimit.vm  = float (vm);
        MyCView.Mapping.WindowLimit.uM  = float (uM);
        MyCView.Mapping.WindowLimit.vM  = float (vM);

        MyCView.Mapping.IsCustomMatrix = MyViewMapping.IsCustomMatrix();
        if (MyViewMapping.IsCustomMatrix()) {
          Standard_Integer i, j;
          for (i = 0; i < 4; i++)
            for (j = 0; j < 4; j++)
              MyCView.Mapping.ProjectionMatrix[i][j] =
                (Standard_ShortReal)MyViewMapping.MyProjectionMatrix->Value(i,j);
        }

        MyMatOfMapIsEvaluated   = Standard_False;

        if (! IsDefined ()) return;

Standard_Boolean AWait = Standard_False;        // => immediate update
        MyGraphicDriver->ViewMapping (MyCView, AWait);

        if (MyViewManager->UpdateMode () == Aspect_TOU_ASAP) Update ();

}

void Visual3d_View::SetContext (const Visual3d_ContextView& CTX) {

        if (IsDeleted ()) return;

Visual3d_TypeOfVisualization OldVisualMode;
Visual3d_TypeOfVisualization NewVisualMode;

        // To manage display only in case of
        // change of visualisation mode.
        OldVisualMode   = MyContext.Visualization ();
        NewVisualMode   = CTX.Visualization ();

Visual3d_TypeOfModel OldModel;
Visual3d_TypeOfModel NewModel;

        // To manage change of visualisation only in case
        // of change of mode of visualisation or of type of shading.
        OldModel        = MyContext.Model ();
        NewModel        = CTX.Model ();

Standard_Boolean OldAliasingMode;
Standard_Boolean NewAliasingMode;

        // To manage antialiasing only in case of change.
        OldAliasingMode = MyContext.AliasingIsOn ();
        NewAliasingMode = CTX.AliasingIsOn ();

Standard_Boolean OldDepthCueingMode;
Standard_Boolean NewDepthCueingMode;

Standard_Real OldDepthCueingFrontPlane;
Standard_Real NewDepthCueingFrontPlane;
Standard_Real OldDepthCueingBackPlane;
Standard_Real NewDepthCueingBackPlane;

        // To manage the depth cueing only in case of change.
        OldDepthCueingMode              = MyContext.DepthCueingIsOn ();
        NewDepthCueingMode              = CTX.DepthCueingIsOn ();

        OldDepthCueingFrontPlane        = MyContext.DepthCueingFrontPlane ();
        NewDepthCueingFrontPlane        = CTX.DepthCueingFrontPlane ();
        OldDepthCueingBackPlane         = MyContext.DepthCueingBackPlane ();
        NewDepthCueingBackPlane         = CTX.DepthCueingBackPlane ();

Standard_Boolean OldFrontZClippingMode;
Standard_Boolean NewFrontZClippingMode;
Standard_Boolean OldBackZClippingMode;
Standard_Boolean NewBackZClippingMode;

Standard_Real OldZClippingFrontPlane;
Standard_Real NewZClippingFrontPlane;
Standard_Real OldZClippingBackPlane;
Standard_Real NewZClippingBackPlane;

        // To manage the Zclipping only in case of change.
        OldFrontZClippingMode   = MyContext.FrontZClippingIsOn ();
        NewFrontZClippingMode   = CTX.FrontZClippingIsOn ();
        OldBackZClippingMode    = MyContext.BackZClippingIsOn ();
        NewBackZClippingMode    = CTX.BackZClippingIsOn ();

        OldZClippingFrontPlane  = MyContext.ZClippingFrontPlane ();
        NewZClippingFrontPlane  = CTX.ZClippingFrontPlane ();
        OldZClippingBackPlane   = MyContext.ZClippingBackPlane ();
        NewZClippingBackPlane   = CTX.ZClippingBackPlane ();

        Handle(Graphic3d_TextureEnv) aTexEnvOld = MyContext.TextureEnv();
        Handle(Graphic3d_TextureEnv) aTexEnvNew = CTX.TextureEnv();

        Visual3d_TypeOfSurfaceDetail OldSurfaceDetail = MyContext.SurfaceDetail();
        Visual3d_TypeOfSurfaceDetail NewSurfaceDetail = CTX.SurfaceDetail();

        MyContext       = CTX;

        UpdateView ();

Standard_Boolean AWait = Standard_False;        // => immediate update
        if (IsDefined ()) {
                // management of visualization modes and types of shading.
                if ((OldVisualMode != NewVisualMode) || (OldModel != NewModel))
                        MyGraphicDriver->SetVisualisation (MyCView);

                // management of antialiasing.
                if (OldAliasingMode != NewAliasingMode)
                MyGraphicDriver->AntiAliasing (MyCView, NewAliasingMode);

                // management of depth_cueing.
                if ((OldDepthCueingMode != NewDepthCueingMode) ||
                    (OldDepthCueingFrontPlane != NewDepthCueingFrontPlane) ||
                    (OldDepthCueingBackPlane != NewDepthCueingBackPlane)) {
#ifdef BUC60572
                    if( NewDepthCueingMode &&
                        (NewDepthCueingBackPlane >= NewDepthCueingFrontPlane) )
                        Visual3d_DepthCueingDefinitionError::Raise
                        ("Bad value for DepthCueingPlanes position");
#endif
                        MyGraphicDriver->DepthCueing
                                (MyCView, NewDepthCueingMode);
                }

                // management of Zclipping
                if ((OldFrontZClippingMode != NewFrontZClippingMode) ||
                    (OldBackZClippingMode != NewBackZClippingMode) ||
                    (OldZClippingFrontPlane != NewZClippingFrontPlane) ||
                    (OldZClippingBackPlane != NewZClippingBackPlane)) {
#ifdef BUC60572
                    if( NewBackZClippingMode && NewFrontZClippingMode &&
                        (NewZClippingBackPlane >= NewZClippingFrontPlane) )
                        Visual3d_ZClippingDefinitionError::Raise
                        ("Bad value for ZClippingPlanes position");
#endif
                        MyGraphicDriver->ClipLimit (MyCView, AWait);
                }

                // management of textures
                if ((aTexEnvOld != aTexEnvNew) || (OldSurfaceDetail != NewSurfaceDetail))
                {
                  MyGraphicDriver->Environment(MyCView);
                }

                // Update of planes of model clipping
                UpdatePlanes ();

                // Update of light sources
                UpdateLights ();
        }

        if (OldVisualMode != NewVisualMode) {

                /*
                 * Change of context =>
                 * Remove structures that cannot be displayed
                 * in the new visualisation mode.
                 * It is not necessary to warn ViewManager as
                 * this structure should not disappear from
                 * the list of structures displayed in it.
                 */
Graphic3d_MapIteratorOfMapOfStructure S1Iterator (MyDisplayedStructure);
Visual3d_TypeOfAnswer Answer;

                Standard_Integer i      = MyDisplayedStructure.Extent ();

Graphic3d_SequenceOfStructure FooSequence;

                while (S1Iterator.More ()) {
                        Answer  = AcceptDisplay (S1Iterator.Key ());
                        // If the structure can't be displayed in the
                        // new context of the view, it is removed.
                        if ((Answer == Visual3d_TOA_NO) ||
                            (Answer == Visual3d_TOA_COMPUTE))
                                //Erase (S1Iterator.Key (), Aspect_TOU_WAIT);
                                FooSequence.Append (S1Iterator.Key ());

                        // S1Iterator.Next () is located on the next structure
                        S1Iterator.Next ();
                }

Standard_Integer Length = FooSequence.Length ();
                // The stored structures are removed
                for (i=1; i<=Length; i++)
                        Erase (FooSequence.Value (i), Aspect_TOU_WAIT);
                if (Length != 0) FooSequence.Clear ();

                /*
                 * Change of context =>
                 * Display structures that can be displayed
                 * with the new visualisation mode.
                 * All structures with status Displayed are removed from the ViewManager
                 * and displayed in the view directly, if the structure is not already
                 * displayed and if the view accepts it in its context.
                 */

                i       = MyViewManager->NumberOfDisplayedStructures ();
                Graphic3d_MapOfStructure Map;
                MyViewManager->DisplayedStructures(Map);
                Graphic3d_MapIteratorOfMapOfStructure it(Map);

                for (; it.More(); it.Next()) {
                  Handle(Graphic3d_Structure) SG = it.Key();
                    if (! IsDisplayed (SG)) {
                      Answer = AcceptDisplay(SG);
                        // If the structure can be displayed in the
                        // new context of the view, it is displayed.
                        if ((Answer == Visual3d_TOA_YES) ||
                            (Answer == Visual3d_TOA_COMPUTE))
                            //Display (MyViewManager->DisplayedStructure (j),
                                //Aspect_TOU_WAIT);
                        FooSequence.Append (SG);
                    }
                }

                Length  = FooSequence.Length ();
                // The stored structures are displayed
                for (i=1; i<=Length; i++)
                        Display (FooSequence.Value (i), Aspect_TOU_WAIT);
                if (Length != 0) FooSequence.Clear ();
        }

        if (MyViewManager->UpdateMode () == Aspect_TOU_ASAP) Update ();

}

const Visual3d_ContextView& Visual3d_View::Context () const {

        return (MyContext);

}

void Visual3d_View::DisplayedStructures (Graphic3d_MapOfStructure& SG) const
{

  if (IsDeleted ()) return;

  Graphic3d_MapIteratorOfMapOfStructure Iterator (MyDisplayedStructure);

  while (Iterator.More ()) {
    SG.Add (Iterator.Key ());

    // Iterator.Next () is located on the next structure
    Iterator.Next ();
  }

}

void Visual3d_View::Activate () {

        if (IsDeleted ()) return;

        if (! IsDefined ())
                Visual3d_ViewDefinitionError::Raise ("Window not defined");

        if (! IsActive ()) {

                MyGraphicDriver->ActivateView (MyCView);
                MyGraphicDriver->Background (MyCView);
                MyGraphicDriver->Transparency
                        (MyCView, MyViewManager->Transparency ());

                MyCView.Active  = 1;

                /*
                 * Activation of a new view =>
                 * Display structures that can be displayed in this new view.
                 * All structures with status
                 * Displayed in ViewManager are returned and displayed in
                 * the view directly, if the structure is not already
                 * displayed and if the view accepts it in its context.
                 */

		Visual3d_TypeOfAnswer Answer;
                Graphic3d_MapOfStructure Map;
                MyViewManager->DisplayedStructures(Map);
                Graphic3d_MapIteratorOfMapOfStructure it(Map);

                for (; it.More(); it.Next()) {
                  Handle(Graphic3d_Structure) SG = it.Key();
                    if (! IsDisplayed (SG)) {
                      Answer    = AcceptDisplay(SG);
                        // If the structure can be displayed in the
                        // new context of the view, it is displayed.
                        if ((Answer == Visual3d_TOA_YES) ||
                            (Answer == Visual3d_TOA_COMPUTE))
                        Display (SG,Aspect_TOU_WAIT);
                    }
                }

        }

        // If the activation/desactivation of ZBuffer should be automatic
        // depending on the presence or absence of facets.
        if (MyViewManager->ZBufferAuto ()) {
Standard_Boolean BContainsFacet = ContainsFacet ();
Standard_Boolean BZBuffer       = ZBufferIsActivated ();
                // If the view contains facets
                // and if ZBuffer is not active
                if (BContainsFacet && ! BZBuffer)
                        SetZBufferActivity (1);
                // If the view does not contain facets
                // and if ZBuffer is active
                if (! BContainsFacet && BZBuffer)
                        SetZBufferActivity (0);
        }

        if (MyViewManager->UpdateMode () == Aspect_TOU_ASAP) Update ();

}

Standard_Boolean Visual3d_View::IsActive () const {

        if (IsDeleted ()) return (Standard_False);

        if (MyCView.Active)
                return (Standard_True);
        else
                return (Standard_False);

}

void Visual3d_View::Deactivate () {

        if (IsDeleted ()) return;

        if (! IsDefined ())
                Visual3d_ViewDefinitionError::Raise ("Window not defined");

        if (IsActive ()) {

                MyGraphicDriver->DeactivateView (MyCView);

                /*
                 * Deactivation of a view =>
                 * Removal of structures displayed in this view.
                 * All structures with status
                 * Displayed in ViewManager are returned and removed from
                 * the view directly, if the structure is not already
                 * displayed and if the view accepts it in its context.
                */

		Visual3d_TypeOfAnswer Answer;
                Graphic3d_MapOfStructure Map;
                MyViewManager->DisplayedStructures(Map);
                Graphic3d_MapIteratorOfMapOfStructure it(Map);

                for (; it.More(); it.Next()) {
                  Handle(Graphic3d_Structure) SG = it.Key();
                    if (! IsDisplayed (SG)) {
                        Answer  = AcceptDisplay(SG);
                        // If the structure was displayed it is removed.
                        if ((Answer == Visual3d_TOA_YES) ||
                            (Answer == Visual3d_TOA_COMPUTE))
                                Erase (SG,Aspect_TOU_WAIT);
                    }
                }

                if (MyViewManager->UpdateMode () == Aspect_TOU_ASAP) Update ();

                // No action currently possible in the view
                MyCView.Active  = 0;
        }

}

void Visual3d_View::Redraw () {

        Redraw (MyViewManager->UnderLayer (), MyViewManager->OverLayer ());

}

void Visual3d_View::Redraw (const Standard_Integer x, const Standard_Integer y, const Standard_Integer width, const Standard_Integer height) {

        Redraw (MyViewManager->UnderLayer (), MyViewManager->OverLayer (), x, y, width, height);
}

void Visual3d_View::Redraw (const Handle(Visual3d_Layer)& AnUnderLayer, const Handle(Visual3d_Layer)& AnOverLayer) {

        if (IsDeleted ()) return;

        if ((! IsDefined ()) || (! IsActive ())) return;

        if (! MyWindow->IsMapped ()) return;

        // san - 14/04/2004 - set up Z buffer state before redrawing
       // If the activation/desactivation of ZBuffer should be automatic
        // depending on the presence or absence of facets.
        if (MyViewManager->ZBufferAuto ()) {
                Standard_Boolean BContainsFacet = ContainsFacet ();
                Standard_Boolean BZBuffer       = ZBufferIsActivated ();
                // If the view contains facets
                // and if ZBuffer is not active
                if (BContainsFacet && ! BZBuffer)
                        SetZBufferActivity (1);
                // If the view contains only facets
                // and if ZBuffer is active
                if (! BContainsFacet && BZBuffer)
                        SetZBufferActivity (0);
        }

        Aspect_CLayer2d OverCLayer;
        Aspect_CLayer2d UnderCLayer;
        OverCLayer.ptrLayer = UnderCLayer.ptrLayer = NULL;
        if (! AnOverLayer.IsNull ()) OverCLayer = AnOverLayer->CLayer ();
        if (! AnUnderLayer.IsNull ()) UnderCLayer = AnUnderLayer->CLayer ();
        MyGraphicDriver->Redraw (MyCView, UnderCLayer, OverCLayer);

}

void Visual3d_View::Redraw (const Handle(Visual3d_Layer)& AnUnderLayer, const Handle(Visual3d_Layer)& AnOverLayer, const Standard_Integer x, const Standard_Integer y, const Standard_Integer width, const Standard_Integer height) {

        if (IsDeleted ()) return;

        if ((! IsDefined ()) || (! IsActive ())) return;

        if (! MyWindow->IsMapped ()) return;

        // san - 14/04/2004 - set up Z buffer state before redrawing
          // If activation/desactivation of ZBuffer should be automatic
        // depending on the presence or absence of facets.
        if (MyViewManager->ZBufferAuto ()) {
        Standard_Boolean BContainsFacet = ContainsFacet ();
        Standard_Boolean BZBuffer       = ZBufferIsActivated ();
                // If the view contains facets
                // and if ZBuffer is not active
                if (BContainsFacet && ! BZBuffer)
                        SetZBufferActivity (1);
                // If the view contains only facets
                // and if ZBuffer is active
                if (! BContainsFacet && BZBuffer)
                        SetZBufferActivity (0);
        }

        Aspect_CLayer2d OverCLayer;
        Aspect_CLayer2d UnderCLayer;
        OverCLayer.ptrLayer = UnderCLayer.ptrLayer = NULL;
        if (! AnOverLayer.IsNull ()) OverCLayer = AnOverLayer->CLayer ();
        if (! AnUnderLayer.IsNull ()) UnderCLayer = AnUnderLayer->CLayer ();
        MyGraphicDriver->Redraw (MyCView, UnderCLayer, OverCLayer, x, y, width, height);

}

void Visual3d_View::Update () {

        Update (MyViewManager->UnderLayer (), MyViewManager->OverLayer ());

}

void Visual3d_View::Update (const Handle(Visual3d_Layer)& AnUnderLayer, const Handle(Visual3d_Layer)& AnOverLayer) {

        if (IsDeleted ()) return;

        if ((! IsDefined ()) || (! IsActive ())) return;

        if (! MyWindow->IsMapped ()) return;

        if (MyGraphicDriver->IsDeviceLost())
        {
          MyViewManager->ReComputeStructures();
          MyGraphicDriver->ResetDeviceLostFlag();
        }

        // If activation/desactivation of ZBuffer should be automatic
        // depending on the presence or absence of facets.
        if (MyViewManager->ZBufferAuto ()) {
Standard_Boolean BContainsFacet = ContainsFacet ();
Standard_Boolean BZBuffer       = ZBufferIsActivated ();
                // If the view contains facets
                // and if ZBuffer is not active
                        if (BContainsFacet && ! BZBuffer)
                        SetZBufferActivity (1);
                   // If the view does not contain facets
                // and if ZBuffer is active
                if (! BContainsFacet && BZBuffer)
                        SetZBufferActivity (0);
        }

Aspect_CLayer2d OverCLayer;
Aspect_CLayer2d UnderCLayer;
        OverCLayer.ptrLayer = UnderCLayer.ptrLayer = NULL;
        if (! AnUnderLayer.IsNull ()) UnderCLayer = AnUnderLayer->CLayer ();
        if (! AnOverLayer.IsNull ()) OverCLayer = AnOverLayer->CLayer ();
        //OSD::SetSignal (Standard_False);
        MyGraphicDriver->Update (MyCView, UnderCLayer, OverCLayer);
        //OSD::SetSignal (Standard_True);

}

Visual3d_TypeOfAnswer Visual3d_View::AcceptDisplay (const Handle(Graphic3d_Structure)& AStructure) const {

// Return type of visualization of the view
Visual3d_TypeOfVisualization ViewType = MyContext.Visualization ();

// Return type of visualization of the structure
Graphic3d_TypeOfStructure StructType = AStructure->Visual ();

Visual3d_TypeOfAnswer Result = Visual3d_TOA_NO;

        if (StructType == Graphic3d_TOS_ALL)
                // The structure accepts any type of view
                Result  = Visual3d_TOA_YES;
        else {
                if ((StructType == Graphic3d_TOS_SHADING)
                        && (ViewType == Visual3d_TOV_SHADING))
                        Result  = Visual3d_TOA_YES;
                if ((StructType == Graphic3d_TOS_WIREFRAME)
                        && (ViewType == Visual3d_TOV_WIREFRAME))
                        Result  = Visual3d_TOA_YES;
                if ((StructType == Graphic3d_TOS_COMPUTED)
                        && (ViewType == Visual3d_TOV_WIREFRAME))
                        Result  = Visual3d_TOA_COMPUTE;
                if ((StructType == Graphic3d_TOS_COMPUTED)
                        && (ViewType == Visual3d_TOV_SHADING))
                        Result  = Visual3d_TOA_COMPUTE;
        }

#ifdef TRACE_ACCEPT
        if (Result == Visual3d_TOA_YES)
        cout << "YES = Visual3d_View" << MyCView.ViewId
             << "::AcceptDisplay (" << AStructure->Identification () << ")\n";
        if (Result == Visual3d_TOA_NO)
        cout << "NO = Visual3d_View" << MyCView.ViewId
             << "::AcceptDisplay (" << AStructure->Identification () << ")\n";
        if (Result == Visual3d_TOA_COMPUTE)
        cout << "COMPUTE = Visual3d_View" << MyCView.ViewId
             << "::AcceptDisplay (" << AStructure->Identification () << ")\n";
        cout << flush;
#endif

        return (Result);

}

void Visual3d_View::ChangeDisplayPriority (const Handle(Graphic3d_Structure)& AStructure, const Standard_Integer /*OldPriority*/, const Standard_Integer NewPriority) {

        if (IsDeleted ()) return;
        if (! IsDefined ()) return;
        if (! IsActive ()) return;
        if (! IsDisplayed (AStructure)) return;

Standard_Integer Index = IsComputed (AStructure);
        if (Index != 0 && ComputedMode())
        {
#ifdef TRACE
	Standard_Integer StructId = MyCOMPUTEDSequence.Value (Index)->Identification ();
        cout << "Visual3d_View" << MyCView.ViewId << "::ChangeDisplayPriority ("
             << AStructure->Identification () << "/" << StructId
             << ", " << OldPriority << ", " << NewPriority << ")\n";
        cout << flush;
#endif
                MyGraphicDriver->EraseStructure
                        (MyCView,
                         *(Graphic3d_CStructure *)
                            MyCOMPUTEDSequence.Value (Index)->CStructure ());
                MyGraphicDriver->DisplayStructure
                        (MyCView,
                         *(Graphic3d_CStructure *)
                            MyCOMPUTEDSequence.Value (Index)->CStructure (),
                         int (NewPriority));
        }
        else {
#ifdef TRACE
        Standard_Integer StructId = AStructure->Identification ();
        cout << "Visual3d_View" << MyCView.ViewId << "::ChangeDisplayPriority ("
             << AStructure->Identification () << "/" << StructId
             << ", " << OldPriority << ", " << NewPriority << ")\n";
        cout << flush;
#endif
                MyGraphicDriver->EraseStructure
                        (MyCView,
                         *(Graphic3d_CStructure *)AStructure->CStructure ());
                MyGraphicDriver->DisplayStructure
                        (MyCView,
                         *(Graphic3d_CStructure *)AStructure->CStructure (),
                         int (NewPriority));
        }

}

void Visual3d_View::Clear (const Handle(Graphic3d_Structure)& AStructure, const Standard_Boolean WithDestruction) {

#ifdef TRACE_CLEAR
        cout << "Visual3d_View" << MyCView.ViewId << "::Clear ("
             << AStructure->Identification () << ")\n";
        cout << flush;
#endif

Standard_Integer Index = IsComputed (AStructure);
        if (Index != 0) {
#ifdef TRACE_CLEAR
        cout << "Structure " << AStructure->Identification ()
             << " calculated in the view "
             << Identification () << ", by structure "
             << MyCOMPUTEDSequence.Value (Index)->Identification ()
             << " is emptied.\n";
        cout << flush;
#endif
                MyCOMPUTEDSequence.Value (Index)->GraphicClear (WithDestruction);
                MyCOMPUTEDSequence.Value (Index)->SetHLRValidation (Standard_False);
        }

}

void Visual3d_View::Connect (const Handle(Graphic3d_Structure)& AMother, const Handle(Graphic3d_Structure)& ADaughter) {

#ifdef TRACE_CONNECT
        cout << "Visual3d_View" << MyCView.ViewId << "::Connect ("
             << AMother->Identification ()
             << ADaughter->Identification () << ")\n";
        cout << flush;
#endif

Standard_Integer IndexM = IsComputed (AMother);
Standard_Integer IndexD = IsComputed (ADaughter);

        if (IndexM != 0 && IndexD != 0) {
#ifdef TRACE_CONNECT
        cout << "Structure " << AMother->Identification ()
             << " is connected to structure "
             << ADaughter->Identification () << endl;
        cout << "These structures are calculated.\n";
        cout << "In the view " << Identification ()
             << "Structure "
             << MyCOMPUTEDSequence.Value (IndexM)->Identification ()
             << " is connected to the structure "
             << MyCOMPUTEDSequence.Value (IndexD)->Identification ()
             << endl;
#endif
                MyCOMPUTEDSequence.Value (IndexM)->GraphicConnect (MyCOMPUTEDSequence.Value (IndexD));
        }

}

void Visual3d_View::Disconnect (const Handle(Graphic3d_Structure)& AMother, const Handle(Graphic3d_Structure)& ADaughter) {

#ifdef TRACE_CONNECT
        cout << "Visual3d_View" << MyCView.ViewId << "::Disconnect ("
             << AMother->Identification ()
             << ADaughter->Identification () << ")\n";
        cout << flush;
#endif

Standard_Integer IndexM = IsComputed (AMother);
Standard_Integer IndexD = IsComputed (ADaughter);

        if (IndexM != 0 && IndexD != 0) {
#ifdef TRACE_CONNECT
        cout << "Structure " << AMother->Identification ()
             << " is disconnected from the structure "
             << ADaughter->Identification () << endl;
        cout << "These structures are calculated.\n";
        cout << "In the view " << Identification ()
             << "Structure "
             << MyCOMPUTEDSequence.Value (IndexM)->Identification ()
             << " is disconnected from the structure "
             << MyCOMPUTEDSequence.Value (IndexD)->Identification ()
             << endl;
#endif
                MyCOMPUTEDSequence.Value (IndexM)->GraphicDisconnect (MyCOMPUTEDSequence.Value (IndexD));
        }

}

void Visual3d_View::Display (const Handle(Graphic3d_Structure)& AStructure) {

        Display (AStructure, MyViewManager->UpdateMode ());

}

void Visual3d_View::Display (const Handle(Graphic3d_Structure)& AStructure, const Aspect_TypeOfUpdate AnUpdateMode) {

        if (IsDeleted ()) return;
        if (! IsDefined ()) return;
        if (! IsActive ()) return;

        // If Display on a structure present in the list
        // of calculated structures while it is not
        // or more, of calculated type =>
        // - removes it as well as the associated old computed
        // THis happens when hlhsr becomes again of type e
        // non computed after SetVisual.
Standard_Integer Index = IsComputed (AStructure);

        if ((Index != 0) && (AStructure->Visual () != Graphic3d_TOS_COMPUTED)) {
                MyTOCOMPUTESequence.Remove (Index);
                MyCOMPUTEDSequence.Remove (Index);
                Index = 0;
        }

	Visual3d_TypeOfAnswer Answer = AcceptDisplay (AStructure);

        if (Answer == Visual3d_TOA_NO) {
                return;
        }

        if (!ComputedMode())
        {
          Answer = Visual3d_TOA_YES;
        }

        if (Answer == Visual3d_TOA_YES ) {
                if (IsDisplayed (AStructure)) return;
                MyGraphicDriver->DisplayStructure (
                        MyCView,
                        *(Graphic3d_CStructure *)AStructure->CStructure (),
                        int (AStructure->DisplayPriority ())
                );
                MyDisplayedStructure.Add (AStructure);
                if (AnUpdateMode == Aspect_TOU_ASAP) Update ();
        }

        if (Answer == Visual3d_TOA_COMPUTE) {
            if (Index != 0) {
                // Already computed, is COMPUTED still valid?
Standard_Integer OldStructId =
        MyCOMPUTEDSequence.Value (Index)->Identification ();

                // Case COMPUTED valide
                if (MyCOMPUTEDSequence.Value (Index)->HLRValidation ()) {
                    // to be displayed
                    if (! IsDisplayed (AStructure)) {
                        MyDisplayedStructure.Add (AStructure);
                        MyGraphicDriver->DisplayStructure (
                            MyCView,
                            *(Graphic3d_CStructure *)MyCOMPUTEDSequence.Value (Index)->CStructure (),
                            int (AStructure->DisplayPriority ())
                        );
                        if (AnUpdateMode == Aspect_TOU_ASAP) Update ();
                    }
                    return;
                }

                // Case COMPUTED invalid
                else {
                    // Is there another valid representation ?
                    // Find in the sequence of already calculated structures
                    // 1/ Structure having the same Owner as <AStructure>
                    // 2/ That is not <AStructure>
                    // 3/ The COMPUTED which of is valid
                    Standard_Integer NewIndex = HaveTheSameOwner (AStructure);

                    // Case of COMPUTED invalid, WITH a valid of replacement
                    if (NewIndex != 0) {
                        // to be displayed
                        if (! IsDisplayed (AStructure)) {
                            MyCOMPUTEDSequence.SetValue
                                (Index, MyCOMPUTEDSequence.Value (NewIndex));
                            OldStructId = MyCOMPUTEDSequence.Value (NewIndex)->
                                                        Identification ();
                            MyDisplayedStructure.Add (AStructure);
                            MyGraphicDriver->DisplayStructure (
                                MyCView,
                                *(Graphic3d_CStructure *)MyCOMPUTEDSequence.Value (NewIndex)->CStructure (),
                                int (AStructure->DisplayPriority ())
                            );
                            if (AnUpdateMode == Aspect_TOU_ASAP) Update ();
                        }
                        return;
                    }

                    // Cas COMPUTED invalid, WITHOUT a valid of replacement
                    else {
                        // COMPUTED is removed if displayed
                        if (IsDisplayed (AStructure))
                            MyGraphicDriver->EraseStructure (
                                MyCView,
                                *(Graphic3d_CStructure *)MyCOMPUTEDSequence.Value (Index)->CStructure ()
                            );
                    }
                }
            } // if (Index != 0)

            // Compute + Validation
#ifdef OLD
Handle(Graphic3d_Structure) TheStructure = AStructure->Compute (this);
#else
Handle(Graphic3d_Structure) TheStructure;
TColStd_Array2OfReal ATrsf (0, 3, 0, 3);
            AStructure->Transform (ATrsf);
            if (Index != 0) {
TColStd_Array2OfReal Ident (0, 3, 0, 3);
Standard_Integer ii, jj;
        for (ii=0; ii<=3; ii++)
            for (jj=0; jj<=3; jj++)
                Ident (ii, jj) = (ii == jj ? 1.0 : 0.0);
                TheStructure = MyCOMPUTEDSequence.Value (Index);
                TheStructure->SetTransform (Ident, Graphic3d_TOC_REPLACE);
                if (AStructure->IsTransformed ()) {
                    AStructure->Compute (this, ATrsf, TheStructure);
                }
                else {
                    AStructure->Compute (this, TheStructure);
                }
            }
            else {
                if (AStructure->IsTransformed ()) {
                    TheStructure = AStructure->Compute (this, ATrsf);
                }
                else {
                    TheStructure = AStructure->Compute (this);
                }
            }
#endif
            TheStructure->SetHLRValidation (Standard_True);

            // TOCOMPUTE and COMPUTED associated to sequences are added
            MyTOCOMPUTESequence.Append (AStructure);
            MyCOMPUTEDSequence.Append (TheStructure);

            // The previous are removed if necessary
            if (Index != 0) {
                MyTOCOMPUTESequence.Remove (Index);
                MyCOMPUTEDSequence.Remove (Index);
            }

// Return type of visualisation of the view
Visual3d_TypeOfVisualization ViewType = MyContext.Visualization ();

// Of which type will be the computed ?
Standard_Boolean ComputeWireframe = ((ViewType == Visual3d_TOV_WIREFRAME) &&
                (AStructure->ComputeVisual () != Graphic3d_TOS_SHADING));

Standard_Boolean ComputeShading = ((ViewType == Visual3d_TOV_SHADING) &&
                (AStructure->ComputeVisual () != Graphic3d_TOS_WIREFRAME));

            if (ComputeWireframe)
                TheStructure->SetVisual (Graphic3d_TOS_WIREFRAME);
            if (ComputeShading)
                TheStructure->SetVisual (Graphic3d_TOS_SHADING);

            if (! ComputeShading && ! ComputeWireframe)
                Answer = Visual3d_TOA_NO;
            else
                Answer = AcceptDisplay (TheStructure);

            if (AStructure->IsHighlighted()) {
                TheStructure->SetHighlightColor (AStructure->HighlightColor ());
                TheStructure->GraphicHighlight (Aspect_TOHM_COLOR);
            }

            // It is displayed only if the calculated structure
            // has a proper type corresponding to the one of the view.
            if (Answer != Visual3d_TOA_NO) {
                if (! IsDisplayed (AStructure))
                        MyDisplayedStructure.Add (AStructure);
                MyGraphicDriver->DisplayStructure (
                        MyCView,
                        *(Graphic3d_CStructure *)TheStructure->CStructure (),
                        int (AStructure->DisplayPriority ())
                );
                if (AnUpdateMode == Aspect_TOU_ASAP) Update ();
            }
        } // Visual3d_TOA_COMPUTE
}

void Visual3d_View::Erase (const Handle(Graphic3d_Structure)& AStructure) {

        if (IsDeleted ()) return;

        Erase (AStructure, MyViewManager->UpdateMode ());

}

void Visual3d_View::Erase (const Handle(Graphic3d_Structure)& AStructure, const Aspect_TypeOfUpdate AnUpdateMode) {

Standard_Integer StructId;

        if (IsDeleted ()) return;

        // No test on window as the structure is displayed only if
        // the window exists, so only one test is enough.
        if (IsDisplayed (AStructure)) {
Visual3d_TypeOfAnswer Answer = AcceptDisplay (AStructure);

                if (!ComputedMode())
                {
                  Answer = Visual3d_TOA_YES;
                }

                if (Answer != Visual3d_TOA_COMPUTE) {
                        MyGraphicDriver->EraseStructure (
                                MyCView,
                                *(Graphic3d_CStructure *)AStructure->CStructure ()
                        );
                }

                if (Answer == Visual3d_TOA_COMPUTE) {
Standard_Integer Index = IsComputed (AStructure);
#ifdef TRACE_COMP
        cout << "Visual3d_View" << MyCView.ViewId << "::Erase ("
             << AStructure->Identification () << ");\n";
        cout << "Index : " << Index << "\n";
        cout << flush;
#endif

                    if (Index != 0 && ComputedMode())
                    {
                        StructId =
                        MyCOMPUTEDSequence.Value (Index)->Identification ();
#ifdef TRACE_COMP
        cout << "Structure " << AStructure->Identification ()
             << " calculated, in the view "
             << Identification () << ", by the structure "
             << StructId << " is removed. \n";
        cout << flush;
#endif
                        MyGraphicDriver->EraseStructure
                                (MyCView, *(Graphic3d_CStructure *)MyCOMPUTEDSequence.Value (Index)->CStructure ());
                    }
                    // else is impossible
                }
                MyDisplayedStructure.Remove (AStructure);
                if (AnUpdateMode == Aspect_TOU_ASAP) Update ();
        }

}

void Visual3d_View::Highlight (const Handle(Graphic3d_Structure)& AStructure, const Aspect_TypeOfHighlightMethod AMethod) {

#ifdef TRACE_HIGH
        cout << "Visual3d_View" << MyCView.ViewId << "::Highlight ("
             << AStructure->Identification () << ")\n";
        cout << flush;
#endif

Standard_Integer Index = IsComputed (AStructure);
        if (Index != 0) {
#ifdef TRACE_HIGH
        cout << "Structure " << AStructure->Identification ()
             << " calculated, in the view "
             << Identification () << ", by the structure "
             << MyCOMPUTEDSequence.Value (Index)->Identification ()
             << " passes in highlight mode.\n";
        cout << flush;
#endif
                (MyCOMPUTEDSequence.Value (Index))->SetHighlightColor
                        (AStructure->HighlightColor ());
                (MyCOMPUTEDSequence.Value (Index))->GraphicHighlight (AMethod);
        }

}

void Visual3d_View::SetTransform (const Handle(Graphic3d_Structure)& AStructure, const TColStd_Array2OfReal& ATrsf) {

#ifdef TRACE_TRSF
        cout << "Visual3d_View" << MyCView.ViewId << "::Transform ("
             << AStructure->Identification () << ")\n";
        cout << flush;
#endif

Standard_Integer Index = IsComputed (AStructure);
        if (Index != 0) {
#ifdef TRACE_TRSF
        cout << "The structure " << AStructure->Identification ()
             << " calculated, in the view "
             << Identification () << ", by the structure "
             << MyCOMPUTEDSequence.Value (Index)->Identification ()
             << " is transformed.\n";
        cout << flush;
#endif
                // Test is somewhat light !
                // trsf is transferred only if it is :
                // a translation
                // a scale
                if (ATrsf (0, 1) != 0. || ATrsf (0, 2) != 0.
                 || ATrsf (1, 0) != 0. || ATrsf (1, 2) != 0.
                 || ATrsf (2, 0) != 0. || ATrsf (2, 1) != 0.)
                    ReCompute (AStructure);
                else
                    MyCOMPUTEDSequence.Value (Index)->GraphicTransform (ATrsf);
        }

}

void Visual3d_View::UnHighlight (const Handle(Graphic3d_Structure)& AStructure) {

#ifdef TRACE_HIGH
        cout << "Visual3d_View" << MyCView.ViewId << "::UnHighlight ("
             << AStructure->Identification () << ")\n";
        cout << flush;
#endif

Standard_Integer Index = IsComputed (AStructure);
        if (Index != 0) {
#ifdef TRACE_HIGH
        cout << "Structure " << AStructure->Identification ()
             << " calculated, in the view "
             << Identification () << ", by the structure "
             << MyCOMPUTEDSequence.Value (Index)->Identification ()
             << " passes in unhighlight mode.\n";
        cout << flush;
#endif
                MyCOMPUTEDSequence.Value (Index)->GraphicUnHighlight ();
        }


}

Standard_Integer Visual3d_View::IsComputed (const Handle(Graphic3d_Structure)& AStructure) const {

Standard_Integer StrId = AStructure->Identification ();

Standard_Integer Result = 0;
Standard_Integer Length = MyTOCOMPUTESequence.Length ();

        // Recherche de la structure <AStructure> dans la
        // sequence des structures deja calculees
        for (Standard_Integer i=1; i<=Length && Result==0; i++)
                if ((MyTOCOMPUTESequence.Value (i))->Identification ()
                        == StrId) Result        = i;
#ifdef TRACE_ISCOMP
        cout << "\n In the view " << Identification () << " the structure ";
        if (Result != 0)
                cout << StrId << " is calculated by "
                     << MyCOMPUTEDSequence.Value (Result)->Identification ()
                     << "\n" << flush;
        else
                cout << StrId << " is not calculated\n" << flush;
#endif

#ifdef TRACE_LENGTH
        if (MyTOCOMPUTESequence.Length () != MyCOMPUTEDSequence.Length ()) {
                cout << "In Visual3d_View::IsComputed, ";
                cout << "TOCOMPUTE " << MyTOCOMPUTESequence.Length ()
                     << " != COMPUTED " << MyCOMPUTEDSequence.Length ()
                     << "\n" << flush;
        }
#endif

        return (Result);

}

Standard_Boolean Visual3d_View::IsDisplayed (const Handle(Graphic3d_Structure)& AStructure) const {

Standard_Boolean Result = Standard_False;

        if (IsDeleted ()) return Result;

        Result  = MyDisplayedStructure.Contains (AStructure);

        return Result;

}

#ifdef IMPLEMENTED
Standard_Boolean Visual3d_View::ContainsComputedStructure () const {

Standard_Boolean Result = Standard_False;

        if (MyDisplayedStructure.IsEmpty ()) return Result;

Graphic3d_MapIteratorOfMapOfStructure Iterator (MyDisplayedStructure);

        Standard_Integer i      = MyDisplayedStructure.Extent ();

        // Stop at the first structure of type TOS_COMPUTED
        while (! Result && Iterator.More ()) {
                Result  =
                (((Iterator.Key ())->Visual ()) == Graphic3d_TOS_COMPUTED);

                // Iterator.Next () is located on the
                // next structure
                Iterator.Next ();
        }

        return Result;

}
#endif

Standard_Boolean Visual3d_View::ContainsFacet () const {

        return ContainsFacet (MyDisplayedStructure);

}

Standard_Boolean Visual3d_View::ContainsFacet (const Graphic3d_MapOfStructure& ASet) const {

Standard_Boolean Result = Standard_False;

        if (ASet.IsEmpty ()) return Result;

Graphic3d_MapIteratorOfMapOfStructure Iterator (ASet);

        // Stop at the first structure containing a facet
        for ( Iterator.Initialize (ASet);
              Iterator.More () && ! Result;
              Iterator.Next ())
                Result  = (Iterator.Key ())->ContainsFacet ();

        return Result;

}

void Visual3d_View::MinMaxValues (Standard_Real& XMin, Standard_Real& YMin, Standard_Real& ZMin, Standard_Real& XMax, Standard_Real& YMax, Standard_Real& ZMax) const {

        MinMaxValues
        (MyDisplayedStructure, XMin, YMin, ZMin, XMax, YMax, ZMax);

}

void Visual3d_View::MinMaxValues (const Graphic3d_MapOfStructure& ASet, Standard_Real& XMin, Standard_Real& YMin, Standard_Real& ZMin, Standard_Real& XMax, Standard_Real& YMax, Standard_Real& ZMax) const {

  if (ASet.IsEmpty ()) {
    XMin = RealFirst ();
    YMin = RealFirst ();
    ZMin = RealFirst ();

    XMax = RealLast ();
    YMax = RealLast ();
    ZMax = RealLast ();
  }
  else {

  Standard_Real Xm, Ym, Zm, XM, YM, ZM;
  Graphic3d_MapIteratorOfMapOfStructure Iterator (ASet);

  XMin = RealLast ();
  YMin = RealLast ();
  ZMin = RealLast ();

  XMax = RealFirst ();
  YMax = RealFirst ();
  ZMax = RealFirst ();

  for ( Iterator.Initialize (ASet);
        Iterator.More ();
        Iterator.Next ()) {

          if (!Iterator.Key()->IsVisible())
          {
            continue;
          }

      if ( (Iterator.Key ())->IsInfinite ()){
        //XMin, YMin .... ZMax are initialized by means of infinite line data
        (Iterator.Key ())->MinMaxValues (Xm, Ym, Zm, XM, YM, ZM);
        if ( Xm != RealFirst() && Xm < XMin )
          XMin = Xm ;
        if ( Ym != RealFirst() && Ym < YMin )
          YMin = Ym ;
        if ( Zm != RealFirst() && Zm < ZMin )
          ZMin = Zm ;
        if ( XM != RealLast()  && XM > XMax )
          XMax = XM ;
        if ( YM != RealLast()  && YM > YMax )
          YMax = YM ;
        if ( ZM != RealLast()  && ZM > ZMax )
          ZMax = ZM ;
      }
      // Only non-empty and non-infinite structures
      // are taken into account for calculation of MinMax
      if (! (Iterator.Key ())->IsInfinite () &&
          ! (Iterator.Key ())->IsEmpty ()) {
            (Iterator.Key ())->MinMaxValues(Xm, Ym, Zm, XM, YM, ZM);
          /* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */
          //"FitAll" operation ignores object with transform persitence parameter
          if( (Iterator.Key ())->TransformPersistenceMode() == Graphic3d_TMF_None )
          {
            if (Xm < XMin) XMin = Xm;
            if (Ym < YMin) YMin = Ym;
            if (Zm < ZMin) ZMin = Zm;
            if (XM > XMax) XMax = XM;
            if (YM > YMax) YMax = YM;
            if (ZM > ZMax) ZMax = ZM;
          }
        }
    }

    // The following cases are relevant
    // For exemple if all structures are empty or infinite
    if (XMax < XMin) { Xm = XMin; XMin = XMax; XMax = Xm; }
    if (YMax < YMin) { Ym = YMin; YMin = YMax; YMax = Ym; }
    if (ZMax < ZMin) { Zm = ZMin; ZMin = ZMax; ZMax = Zm; }
  }
  Standard_Real Sx, Sy, Sz;
  MyViewOrientation.AxialScale(Sx, Sy, Sz);
  XMin = (Sx > 1. && XMin < RealFirst ()/Sx)?RealFirst (): XMin*Sx;
  XMax = (Sx > 1. && XMax > RealLast  ()/Sx)?RealLast  (): XMax*Sx;
  YMin = (Sy > 1. && YMin < RealFirst ()/Sy)?RealFirst (): YMin*Sy;
  YMax = (Sy > 1. && YMax > RealLast  ()/Sy)?RealLast  (): YMax*Sy;
  ZMin = (Sz > 1. && ZMin < RealFirst ()/Sz)?RealFirst (): ZMin*Sz;
  ZMax = (Sz > 1. && ZMax > RealLast  ()/Sz)?RealLast  (): ZMax*Sz;
}

void Visual3d_View::MinMaxValues (Standard_Real& XMin, Standard_Real& YMin, Standard_Real& XMax, Standard_Real& YMax) {

        MinMaxValues (MyDisplayedStructure, XMin, YMin, XMax, YMax);

}

void Visual3d_View::MinMaxValues (const Graphic3d_MapOfStructure& ASet, Standard_Real& XMin, Standard_Real& YMin, Standard_Real& XMax, Standard_Real& YMax) {

Standard_Real Xm, Ym, Zm, XM, YM, ZM;
Standard_Real Xp, Yp, Zp;

        MinMaxValues (ASet, Xm, Ym, Zm, XM, YM, ZM);

        Projects (Xm, Ym, Zm, Xp, Yp, Zp);
        XMin    = Xp;
        YMin    = Yp;

        Projects (XM, YM, ZM, Xp, Yp, Zp);
        XMax    = Xp;
        YMax    = Yp;

        if (XMax < XMin) { Xp = XMax; XMax = XMin; XMin = Xp; }
        if (YMax < YMin) { Yp = YMax; YMax = YMin; YMin = Yp; }
}

const TColStd_Array2OfReal& Visual3d_View::MatrixOfOrientation () {

        if (! MyMatOfOriIsEvaluated) {
                MyGraphicDriver->InquireMat
                    (MyCView, MyMatrixOfOrientation, MyMatrixOfMapping);
                MyMatOfMapIsEvaluated   = Standard_True;
                MyMatOfOriIsEvaluated   = Standard_True;
        }

        return (MyMatrixOfOrientation);

}

const TColStd_Array2OfReal& Visual3d_View::MatrixOfMapping () {

        if (! MyMatOfMapIsEvaluated) {
                MyGraphicDriver->InquireMat
                    (MyCView, MyMatrixOfOrientation, MyMatrixOfMapping);
                MyMatOfMapIsEvaluated   = Standard_True;
                MyMatOfOriIsEvaluated   = Standard_True;
        }

        return (MyMatrixOfMapping);

}

Standard_Integer Visual3d_View::NumberOfDisplayedStructures () const {

Standard_Integer Result = MyDisplayedStructure.Extent ();

        return (Result);

}

#ifdef OLD_METHOD
void Visual3d_View::Projects (const Standard_Real AX, const Standard_Real AY, const Standard_Real AZ, Standard_Real& APX, Standard_Real& APY, Standard_Real& APZ) const {

math_Vector PtDC (0,3), PtWC (0,3);

// RLE method:
// Otherwise use new on Visual3d_View (constructor+destructor)
// as Projects is a const method or MatrixOfOrientation and
// MatrixOfMapping is not.
Visual3d_View * const newthis = (Visual3d_View * const) this;
        newthis->MatrixOfOrientation ();
        newthis->MatrixOfMapping ();

// World Coordinate Space
        PtWC (0) = AX;
        PtWC (1) = AY;
        PtWC (2) = AZ;
        PtWC (3) = 1.0;

        // WCS -> View Reference Coordinate Space
        math_Vector PtVRC(0,3);
        PtVRC = MyMatrixOfOrientation.Multiplied (PtWC);

        // VRCS -> Normalized Projection Coordinate Space
        math_Vector PtNPC(0,3);
        PtNPC = MyMatrixOfMapping.Multiplied (PtVRC);
        for (Standard_Integer i=0; i<3; i++) PtNPC (i) = PtNPC (i) / PtNPC (3);

#ifdef DEBUG
        printf("Display coordinates PtNPC: %f,%f,%f,%f\n",
                        PtNPC(0),PtNPC(1),PtNPC(2),PtNPC(3));
#endif // DEBUG

        Standard_Real Ratio;
#ifdef DEBUG
        // NPCS -> Device Coordinate Space
        Standard_Real Dx        = Standard_Real (MyCView.DefWindow.dx);
        Standard_Real Dy        = Standard_Real (MyCView.DefWindow.dy);

        Ratio           = Dx / Dy;
        PtDC (0)        = PtNPC (0) * Dx;
        PtDC (1)        = Dy - PtNPC (1) * Dy * Ratio;

        printf("Display coordinates : %f,%f,%f,%f\n",
                        PtDC(0),PtDC(1),PtDC(2),PtDC(3));
#endif // DEBUG

        // NPCS -> Window Space
Standard_Real um, vm, uM, vM;
        MyViewMapping.WindowLimit (um, vm, uM, vM);

        Ratio   = (uM - um) / (vM - vm);
        if (Ratio >= 1.0)
            PtNPC (1)   = PtNPC (1) * Ratio;
        else
            PtNPC (0)   = PtNPC (0) / Ratio;

#ifdef DEBUG
        printf("Display coordinates PtNPC: %f,%f,%f,%f\n",
                        PtNPC(0),PtNPC(1),PtNPC(2),PtNPC(3));
#endif // DEBUG

        Standard_Real fpd       = MyViewMapping.FrontPlaneDistance ();
        Standard_Real bpd       = MyViewMapping.BackPlaneDistance ();

        /*
         * Coordinates of PtNPC are described in the space
         * [0-1]x[0-1]x[0-1].
         * It is necessary to transform x and y in the window space.
         * It is necessary to transform z in the space of back and front
         * plane, taking into account clipping planes.
         * Z clipping planes are defined between 0 and 1.
        */

        APX     = PtNPC (0) * (uM - um) + um;
        APY     = PtNPC (1) * (vM - vm) + vm;
        APZ     = PtNPC (2) * (fpd - bpd) + bpd;

#ifdef DEBUG
        Standard_Integer l,c;
        printf("OrientationMatrix :");
        for( l=0 ; l<4 ; l++ ) {
          printf("\n    %d->",l);
          for( c=0 ; c<4 ; c++ ) {
            printf(" %f ,",MyMatrixOfOrientation(c,l));
          }
        }
        printf("\n\n");
        printf("MappingMatrix :");
        for( l=0 ; l<4 ; l++ ) {
          printf("\n    %d->",l);
          for( c=0 ; c<4 ; c++ ) {
            printf(" %f ,",MyMatrixOfMapping(c,l));
          }
        }
        printf("\n\n");
        printf("World coordinates : %f,%f,%f,%f\n",
                        PtWC(0),PtWC(1),PtWC(2),PtWC(3));
        printf("View coordinates : %f,%f,%f,%f\n",
                        PtVRC(0),PtVRC(1),PtVRC(2),PtVRC(3));
        printf("Display coordinates : %f,%f,%f,%f\n",
                        PtNPC(0),PtNPC(1),PtNPC(2),PtNPC(3));
        printf("Window limit : %f,%f,%f,%f\n",um,vm,uM,vM);
        printf("Ratio : %f\n",Ratio);
        printf("Front-plane : %f back-plane : %f\n",fpd,bpd);
        printf("Projection : %f,%f,%f\n \n",APX,APY,APZ);
#endif

}
#endif /* OLD_METHOD */

// OCC18942: This method is likely to duplicate Visual3d_ViewManager::ConvertCoord() one,
// therefore it is necessary to consider merging the two methods or making them call the same
// graphic driver's method after OCCT 6.3.
void Visual3d_View::Projects (const Standard_Real AX, const Standard_Real AY, const Standard_Real AZ, Standard_Real& APX, Standard_Real& APY, Standard_Real& APZ) {

Standard_Real PtX, PtY, PtZ, PtT;
Standard_Real APT;
static Standard_Real Ratio, um, vm, uM, vM;
static Standard_Real fpd, bpd;

        if (! MyMatOfOriIsEvaluated || ! MyMatOfMapIsEvaluated) {
                MyGraphicDriver->InquireMat
                    (MyCView, MyMatrixOfOrientation, MyMatrixOfMapping);
                MyMatOfOriIsEvaluated = MyMatOfMapIsEvaluated = Standard_True;
        }

        // WCS -> View Reference Coordinate Space
        PtX     = MyMatrixOfOrientation (0, 0) * AX
                + MyMatrixOfOrientation (0, 1) * AY
                + MyMatrixOfOrientation (0, 2) * AZ
                + MyMatrixOfOrientation (0, 3);
        PtY     = MyMatrixOfOrientation (1, 0) * AX
                + MyMatrixOfOrientation (1, 1) * AY
                + MyMatrixOfOrientation (1, 2) * AZ
                + MyMatrixOfOrientation (1, 3);
        PtZ     = MyMatrixOfOrientation (2, 0) * AX
                + MyMatrixOfOrientation (2, 1) * AY
                + MyMatrixOfOrientation (2, 2) * AZ
                + MyMatrixOfOrientation (2, 3);
        PtT     = MyMatrixOfOrientation (3, 0) * AX
                + MyMatrixOfOrientation (3, 1) * AY
                + MyMatrixOfOrientation (3, 2) * AZ
                + MyMatrixOfOrientation (3, 3);

        // VRCS -> Normalized Projection Coordinate Space
        APX     = MyMatrixOfMapping (0, 0) * PtX
                + MyMatrixOfMapping (0, 1) * PtY
                + MyMatrixOfMapping (0, 2) * PtZ
                + MyMatrixOfMapping (0, 3) * PtT;
        APY     = MyMatrixOfMapping (1, 0) * PtX
                + MyMatrixOfMapping (1, 1) * PtY
                + MyMatrixOfMapping (1, 2) * PtZ
                + MyMatrixOfMapping (1, 3) * PtT;
        APZ     = MyMatrixOfMapping (2, 0) * PtX
                + MyMatrixOfMapping (2, 1) * PtY
                + MyMatrixOfMapping (2, 2) * PtZ
                + MyMatrixOfMapping (2, 3) * PtT;
        APT     = MyMatrixOfMapping (3, 0) * PtX
                + MyMatrixOfMapping (3, 1) * PtY
                + MyMatrixOfMapping (3, 2) * PtZ
                + MyMatrixOfMapping (3, 3) * PtT;

        APX /= APT;
        APY /= APT;
        APZ /= APT;

        // NPCS -> Window Space
        MyViewMapping.WindowLimit (um, vm, uM, vM);
        fpd     = MyViewMapping.FrontPlaneDistance ();
        bpd     = MyViewMapping.BackPlaneDistance ();

        if(MyCView.Mapping.IsCustomMatrix) {
        	// OCC18942: SAN - If orientation and mapping matrices are those used by OpenGL
        	// visualization, then X, Y and Z coordinates normally vary between -1 and 1
        	APX     = ( APX + 1 ) * 0.5 * (uM - um) + um;
        	APY     = ( APY + 1 ) * 0.5 * (vM - vm) + vm;
        } else {
        	Ratio   = (uM - um) / (vM - vm);
        	if (Ratio >= 1.0)
        		APY *= Ratio;
        	else
        		APX /= Ratio;

        	/*
         * Coordinates of APX, APY, APZ are described in the space
         * [0-1]x[0-1]x[0-1].
         * It is necessary to transform x and y in the window space.
         * It is necessary to transform z in the space of back and front
         * plane, taking into account clipping planes.
         * Z clipping planes are defined between 0 and 1.
         	*/
            APX     = APX * (uM - um) + um;
            APY     = APY * (vM - vm) + vm;
        }
        APZ     = APZ * (fpd - bpd) + bpd;
}

Standard_Integer Visual3d_View::Identification () const {

        return (Standard_Integer (MyCView.ViewId));

}


Standard_Boolean Visual3d_View::ZBufferIsActivated () const {

        if (IsDeleted ()) return (Standard_False);

        if ((! IsDefined ()) || (! IsActive ())) return (Standard_False);

// -1 => not forced by the programmer => depends on the type of visualisation
//  0 ou 1 => forced by the programmer

        if (MyCView.Context.ZBufferActivity == -1)
                if (MyContext.Visualization () == Visual3d_TOV_SHADING)
                        return (Standard_True);
                else
                        return (Standard_False);
        else
                if (MyCView.Context.ZBufferActivity)
                        return (Standard_True);
                else
                        return (Standard_False);

}

void Visual3d_View::SetTransparency (const Standard_Boolean AnActivity) {

        if (IsDeleted ()) return;

        if ((! IsDefined ()) || (! IsActive ())) return;

        MyGraphicDriver->Transparency (MyCView, AnActivity);

}

void Visual3d_View::SetZBufferActivity (const Standard_Integer AnActivity) {

        if (MyCView.Context.ZBufferActivity == AnActivity) return;

        if (IsDeleted ()) return;

        if ((! IsDefined ()) || (! IsActive ())) return;

        MyCView.Context.ZBufferActivity = AnActivity;
        MyGraphicDriver->SetVisualisation (MyCView);

}

void Visual3d_View::UpdateView () {

        MyCView.Context.Aliasing        = (MyContext.AliasingIsOn () ? 1:0);
        MyCView.Context.BackZClipping   =
                (MyContext.BackZClippingIsOn () ? 1:0);
        MyCView.Context.FrontZClipping  =
                (MyContext.FrontZClippingIsOn() ? 1:0);
        MyCView.Context.DepthCueing     =
                (MyContext.DepthCueingIsOn () ? 1:0);

        MyCView.Context.ZClipFrontPlane =
                                float (MyContext.ZClippingFrontPlane ());
        MyCView.Context.ZClipBackPlane          =
                                float (MyContext.ZClippingBackPlane ());
        MyCView.Context.DepthFrontPlane =
                                float (MyContext.DepthCueingFrontPlane ());
        MyCView.Context.DepthBackPlane          =
                                float (MyContext.DepthCueingBackPlane ());

        MyCView.Context.Model           = int (MyContext.Model ());
        MyCView.Context.Visualization   = int (MyContext.Visualization ());

        MyCView.Context.TextureEnv    = MyContext.TextureEnv();
        MyCView.Context.SurfaceDetail = MyContext.SurfaceDetail();

}

void Visual3d_View::Compute () {

Standard_Integer i;
Standard_Integer Length = MyCOMPUTEDSequence.Length ();
        for (i=1; i<=Length; i++)
            (MyCOMPUTEDSequence.Value (i))->SetHLRValidation (Standard_False);

        if (!ComputedMode())
        {
          return;
        }

        /*
         * Force HLRValidation to False on all structures
         * calculated in the view.
         */
#ifdef TRACE_LENGTH
        if (MyTOCOMPUTESequence.Length () != MyCOMPUTEDSequence.Length ()) {
                cout << "In Visual3d_View::Compute, ";
                cout << "TOCOMPUTE " << MyTOCOMPUTESequence.Length ()
                     << " != COMPUTED " << MyCOMPUTEDSequence.Length ()
                     << "\n" << flush;
        }
#endif

        /*
         * Change of orientation or of projection type =>
         * Remove structures that were calculated for the
         * previous orientation.
         * Recalculation of new structures.
         */
Graphic3d_MapIteratorOfMapOfStructure S1Iterator (MyDisplayedStructure);
Visual3d_TypeOfAnswer Answer;

        i       = MyDisplayedStructure.Extent ();

Graphic3d_SequenceOfStructure FooSequence;

        while (S1Iterator.More ()) {
                Answer  = AcceptDisplay (S1Iterator.Key ());
                // If the structure was calculated, it is recalculated.
                if (Answer == Visual3d_TOA_COMPUTE) {
#ifdef TRACE
                        cout << "AcceptDisplay ("
                             << (S1Iterator.Key ())->Identification ()
                             << ") == Visual3d_TOA_COMPUTE;\n";
                        cout << flush;
#endif
                        //Display (S1Iterator.Key (), Aspect_TOU_WAIT);
                        FooSequence.Append (S1Iterator.Key ());
                }

                // S1Iterator.Next () is located on the next structure
                S1Iterator.Next ();
        }

        Length  = FooSequence.Length ();
        // All stored structures are displayed
        for (i=1; i<=Length; i++)
                Display (FooSequence.Value (i), Aspect_TOU_WAIT);
        if (Length != 0) FooSequence.Clear ();

}

void Visual3d_View::ReCompute (const Handle(Graphic3d_Structure)& AStructure) {
        if (!ComputedMode()) return;

        if (IsDeleted ()) return;

        if ((! IsDefined ()) || (! IsActive ())) return;

        if (! MyWindow->IsMapped ()) return;

        if (! AStructure->IsDisplayed ()) return;

Visual3d_TypeOfAnswer Answer;

        Answer  = AcceptDisplay (AStructure);

        if (Answer == Visual3d_TOA_COMPUTE) {
Standard_Integer Index = IsComputed (AStructure);
                if (Index == 0) {
#ifdef TRACE_COMP
        cout << "Structure " << AStructure->Identification ()
             << " is not calculated in the view "
             << Identification () << "\n";
        cout << flush;
#endif
                }
                else {
		  Standard_Integer OldStructId, NewStructId;
                        OldStructId =
                        MyCOMPUTEDSequence.Value (Index)->Identification ();
#ifdef TRACE_COMP
	Standard_Integer StructId = AStructure->Identification ();
        cout << "Structure " << StructId
             << " calculated, in the view "
             << Identification () << ", by the structure "
             << OldStructId << " is recalculated.\n";
        cout << flush;
#endif

        // Compute + Validation
#ifdef OLD
Handle(Graphic3d_Structure) TheStructure = AStructure->Compute (this);
#else
Handle(Graphic3d_Structure) TheStructure;
TColStd_Array2OfReal ATrsf (0, 3, 0, 3);
        AStructure->Transform (ATrsf);
        if (Index != 0) {
TColStd_Array2OfReal Ident (0, 3, 0, 3);
Standard_Integer ii, jj;
        for (ii=0; ii<=3; ii++)
            for (jj=0; jj<=3; jj++)
                Ident (ii, jj) = (ii == jj ? 1.0 : 0.0);
            TheStructure = MyCOMPUTEDSequence.Value (Index);
            TheStructure->SetTransform (Ident, Graphic3d_TOC_REPLACE);
            if (AStructure->IsTransformed ()) {
                AStructure->Compute (this, ATrsf, TheStructure);
            }
            else {
                AStructure->Compute (this, TheStructure);
            }
        }
        else {
            if (AStructure->IsTransformed ()) {
                TheStructure = AStructure->Compute (this, ATrsf);
            }
            else {
                TheStructure = AStructure->Compute (this);
            }
        }
#endif
        TheStructure->SetHLRValidation (Standard_True);

// Return type of visualisation of the view
Visual3d_TypeOfVisualization ViewType = MyContext.Visualization ();

// Of which type will be the computed ?
Standard_Boolean ComputeWireframe = ((ViewType == Visual3d_TOV_WIREFRAME) &&
        (AStructure->ComputeVisual () != Graphic3d_TOS_SHADING));

Standard_Boolean ComputeShading = ((ViewType == Visual3d_TOV_SHADING) &&
        (AStructure->ComputeVisual () != Graphic3d_TOS_WIREFRAME));

                        if (ComputeWireframe)
                            TheStructure->SetVisual (Graphic3d_TOS_WIREFRAME);
                        if (ComputeShading)
                            TheStructure->SetVisual (Graphic3d_TOS_SHADING);

                        if (AStructure->IsHighlighted ()) {
                            TheStructure->SetHighlightColor
                                (AStructure->HighlightColor ());
                            TheStructure->GraphicHighlight (Aspect_TOHM_COLOR);
                        }

                        // Ot is ranged
                        // Find structure <AStructure>
                        // in the sequence of structures to be calculated.
                        NewStructId = TheStructure->Identification ();

                        // The previous calculation is removed and the new one is dislayed
                        MyGraphicDriver->EraseStructure (
                                MyCView,
                                *(Graphic3d_CStructure *)MyCOMPUTEDSequence.Value (Index)->CStructure ());
                        MyGraphicDriver->DisplayStructure (
                                MyCView,
                                *(Graphic3d_CStructure *)TheStructure->CStructure (),
                                int (AStructure->DisplayPriority ())
                        );

#ifdef TRACE_LENGTH
        if (MyTOCOMPUTESequence.Length () != MyCOMPUTEDSequence.Length ()) {
                cout << "In Visual3d_View::ReCompute, ";
                cout << "TOCOMPUTE " << MyTOCOMPUTESequence.Length ()
                     << " != COMPUTED " << MyCOMPUTEDSequence.Length ()
                     << "\n" << flush;
        }
#endif

                          // hlhsr and the new associated compute are added
                          MyTOCOMPUTESequence.Append (AStructure);
                          MyCOMPUTEDSequence.Append (TheStructure);

#ifdef TRACE_LENGTH
        if (MyTOCOMPUTESequence.Length () != MyCOMPUTEDSequence.Length ())
                cout << "\tTOCOMPUTE " << MyTOCOMPUTESequence.Length ()
                     << " != COMPUTED " << MyCOMPUTEDSequence.Length ()
                     << "\n" << flush;
#endif

                          // hlhsr and the new associated compute are removed

                          MyTOCOMPUTESequence.Remove (Index);
                          MyCOMPUTEDSequence.Remove (Index);

#ifdef TRACE_LENGTH
        if (MyTOCOMPUTESequence.Length () != MyCOMPUTEDSequence.Length ())
                cout << "\tTOCOMPUTE " << MyTOCOMPUTESequence.Length ()
                     << " != COMPUTED " << MyCOMPUTEDSequence.Length ()
                     << "\n" << flush;
#endif
                }
        }

}

const Handle(Graphic3d_GraphicDriver)& Visual3d_View::GraphicDriver () const {

        return MyGraphicDriver;

}

void Visual3d_View::Plot (const Handle(Graphic3d_Plotter)& thePlotter) const
{
  for (Graphic3d_MapIteratorOfMapOfStructure S1Iterator (MyDisplayedStructure); S1Iterator.More(); S1Iterator.Next())
  {
    Standard_Integer Index = IsComputed (S1Iterator.Key ());
    // displayed structure is plotted as if it was not calculated
    if (Index == 0)
      (S1Iterator.Key ())->Plot (thePlotter);
    else
      (MyCOMPUTEDSequence.Value (Index))->Plot (thePlotter);
  }
}

Standard_Integer Visual3d_View::HaveTheSameOwner (const Handle(Graphic3d_Structure)& AStructure) const {

Standard_Integer Result = 0;
Standard_Integer Length = MyTOCOMPUTESequence.Length ();

        // Find in the sequence of already calculated structures
        // 1/ Structure with the same Owner as <AStructure>
        // 2/ Which is not <AStructure>
        // 3/ COMPUTED which of is valid
        for (Standard_Integer i=1; i<=Length && Result==0; i++)
           if ((MyTOCOMPUTESequence.Value (i)->Owner () == AStructure->Owner ())
            && (MyTOCOMPUTESequence.Value (i)->Identification () !=
                                                AStructure->Identification ())
            && (MyCOMPUTEDSequence.Value (i)->HLRValidation ())) Result   = i;

        return (Result);

}

Standard_Address Visual3d_View::CView () const {

        return Standard_Address (&MyCView);

}


// Triedron methods : the Triedron is a non-zoomable object.

void Visual3d_View::ZBufferTriedronSetup(const Quantity_NameOfColor XColor,
                                    const Quantity_NameOfColor YColor,
                                    const Quantity_NameOfColor ZColor,
                                    const Standard_Real        SizeRatio,
                                    const Standard_Real        AxisDiametr,
                                    const Standard_Integer     NbFacettes) {
  MyGraphicDriver->ZBufferTriedronSetup(XColor, YColor, ZColor,
                                        SizeRatio, AxisDiametr, NbFacettes);
}



void Visual3d_View::TriedronDisplay (const Aspect_TypeOfTriedronPosition APosition,
 const Quantity_NameOfColor AColor, const Standard_Real AScale, const Standard_Boolean AsWireframe ) {

        MyGraphicDriver->TriedronDisplay (MyCView,APosition, AColor, AScale, AsWireframe);

}

void Visual3d_View::TriedronErase () {

        MyGraphicDriver->TriedronErase (MyCView);


}

void Visual3d_View::TriedronEcho (const Aspect_TypeOfTriedronEcho AType ) {

        MyGraphicDriver->TriedronEcho (MyCView,AType);

}

Standard_Boolean checkFloat(const Standard_Real value)
{
    return value > -FLT_MAX && value < FLT_MAX;
}

void SetMinMaxValuesCallback(void* Visual3dView)
{
    Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;

    Handle(Visual3d_View) view = (Handle(Visual3d_View)&) Visual3dView;
    view->MinMaxValues(xmin, ymin, zmin, xmax, ymax, zmax);

    if (checkFloat(xmin) && checkFloat(ymin) && checkFloat(zmin) &&
        checkFloat(xmax) && checkFloat(ymax) && checkFloat(zmax))
    {
        Handle(Graphic3d_GraphicDriver) driver = Handle(Graphic3d_GraphicDriver)::DownCast(view->GraphicDriver());
        driver->GraduatedTrihedronMinMaxValues((Standard_ShortReal)xmin, (Standard_ShortReal)ymin, (Standard_ShortReal)zmin,
                                               (Standard_ShortReal)xmax, (Standard_ShortReal)ymax, (Standard_ShortReal)zmax);
    }
}

Standard_Boolean Visual3d_View::GetGraduatedTrihedron
    (/* Names of axes */
     TCollection_ExtendedString& xname,
     TCollection_ExtendedString& yname,
     TCollection_ExtendedString& zname,
     /* Draw names */
     Standard_Boolean& xdrawname,
     Standard_Boolean& ydrawname,
     Standard_Boolean& zdrawname,
     /* Draw values */
     Standard_Boolean& xdrawvalues,
     Standard_Boolean& ydrawvalues,
     Standard_Boolean& zdrawvalues,
     /* Draw grid */
     Standard_Boolean& drawgrid,
     /* Draw axes */
     Standard_Boolean& drawaxes,
     /* Number of splits along axes */
     Standard_Integer& nbx,
     Standard_Integer& nby,
     Standard_Integer& nbz,
     /* Offset for drawing values */
     Standard_Integer& xoffset,
     Standard_Integer& yoffset,
     Standard_Integer& zoffset,
     /* Offset for drawing names of axes */
     Standard_Integer& xaxisoffset,
     Standard_Integer& yaxisoffset,
     Standard_Integer& zaxisoffset,
     /* Draw tickmarks */
     Standard_Boolean& xdrawtickmarks,
     Standard_Boolean& ydrawtickmarks,
     Standard_Boolean& zdrawtickmarks,
     /* Length of tickmarks */
     Standard_Integer& xtickmarklength,
     Standard_Integer& ytickmarklength,
     Standard_Integer& ztickmarklength,
     /* Grid color */
     Quantity_Color& gridcolor,
     /* Colors of axis names */
     Quantity_Color& xnamecolor,
     Quantity_Color& ynamecolor,
     Quantity_Color& znamecolor,
     /* Colors of axis and values */
     Quantity_Color& xcolor,
     Quantity_Color& ycolor,
     Quantity_Color& zcolor,
     /* Name of font for names of axes */
     TCollection_AsciiString& fontOfNames,
     /* Style of names of axes */
     Font_FontAspect& styleOfNames,
     /* Size of names of axes */
     Standard_Integer& sizeOfNames,
     /* Name of font for values */
     TCollection_AsciiString& fontOfValues,
     /* Style of values */
     Font_FontAspect& styleOfValues,
     /* Size of values */
     Standard_Integer& sizeOfValues) const
{
    if (!MyGTrihedron.ptrVisual3dView)
        return Standard_False;

    /* Names of axes */
    xname = MyGTrihedron.xname;
    yname = MyGTrihedron.yname;
    zname = MyGTrihedron.zname;
    /* Draw names */
    xdrawname = MyGTrihedron.xdrawname;
    ydrawname = MyGTrihedron.ydrawname;
    zdrawname = MyGTrihedron.zdrawname;
    /* Draw values */
    xdrawvalues = MyGTrihedron.xdrawvalues;
    ydrawvalues = MyGTrihedron.ydrawvalues;
    zdrawvalues = MyGTrihedron.zdrawvalues;
    /* Draw grid */
    drawgrid = MyGTrihedron.drawgrid;
    /* Draw axes */
    drawaxes = MyGTrihedron.drawaxes;
    /* Number of splits along axes */
    nbx = MyGTrihedron.nbx;
    nby = MyGTrihedron.nby;
    nbz = MyGTrihedron.nbz;
    /* Offset for drawing values */
    xoffset = MyGTrihedron.xoffset;
    yoffset = MyGTrihedron.yoffset;
    zoffset = MyGTrihedron.zoffset;
    /* Offset for drawing names of axes */
    xaxisoffset = MyGTrihedron.xaxisoffset;
    yaxisoffset = MyGTrihedron.yaxisoffset;
    zaxisoffset = MyGTrihedron.zaxisoffset;
    /* Draw tickmarks */
    xdrawtickmarks = MyGTrihedron.xdrawtickmarks;
    ydrawtickmarks = MyGTrihedron.ydrawtickmarks;
    zdrawtickmarks = MyGTrihedron.zdrawtickmarks;
    /* Length of tickmarks */
    xtickmarklength = MyGTrihedron.xtickmarklength;
    ytickmarklength = MyGTrihedron.ytickmarklength;
    ztickmarklength = MyGTrihedron.ztickmarklength;
    /* Grid color */
    gridcolor = MyGTrihedron.gridcolor;
    /* Colors of axis names */
    xnamecolor = MyGTrihedron.xnamecolor;
    ynamecolor = MyGTrihedron.ynamecolor;
    znamecolor = MyGTrihedron.znamecolor;
    /* Colors of axis and values */
    xcolor = MyGTrihedron.xcolor;
    ycolor = MyGTrihedron.ycolor;
    zcolor = MyGTrihedron.zcolor;
    /* Name of font for names of axes */
    fontOfNames = MyGTrihedron.fontOfNames;
    /* Style of names of axes */
    styleOfNames = MyGTrihedron.styleOfNames;
    /* Size of names of axes */
    sizeOfNames = MyGTrihedron.sizeOfNames;
    /* Name of font for values */
    fontOfValues = MyGTrihedron.fontOfValues;
    /* Style of values */
    styleOfValues = MyGTrihedron.styleOfValues;
    /* Size of values */
    sizeOfValues = MyGTrihedron.sizeOfValues;

    return Standard_True;
}

void Visual3d_View::GraduatedTrihedronDisplay
    (/* Names of axes */
     const TCollection_ExtendedString &xname,
     const TCollection_ExtendedString &yname,
     const TCollection_ExtendedString &zname,
     /* Draw names */
     const Standard_Boolean xdrawname,
     const Standard_Boolean ydrawname,
     const Standard_Boolean zdrawname,
     /* Draw values */
     const Standard_Boolean xdrawvalues,
     const Standard_Boolean ydrawvalues,
     const Standard_Boolean zdrawvalues,
     /* Draw grid */
     const Standard_Boolean drawgrid,
     /* Draw axes */
     const Standard_Boolean drawaxes,
     /* Number of splits along axes */
     const Standard_Integer nbx,
     const Standard_Integer nby,
     const Standard_Integer nbz,
     /* Offset for drawing values */
     const Standard_Integer xoffset,
     const Standard_Integer yoffset,
     const Standard_Integer zoffset,
     /* Offset for drawing names of axes */
     const Standard_Integer xaxisoffset,
     const Standard_Integer yaxisoffset,
     const Standard_Integer zaxisoffset,
     /* Draw tickmarks */
     const Standard_Boolean xdrawtickmarks,
     const Standard_Boolean ydrawtickmarks,
     const Standard_Boolean zdrawtickmarks,
     /* Length of tickmarks */
     const Standard_Integer xtickmarklength,
     const Standard_Integer ytickmarklength,
     const Standard_Integer ztickmarklength,
     /* Grid color */
     const Quantity_Color& gridcolor,
     /* Colors of axis names */
     const Quantity_Color& xnamecolor,
     const Quantity_Color& ynamecolor,
     const Quantity_Color& znamecolor,
     /* Colors of axis and values */
     const Quantity_Color& xcolor,
     const Quantity_Color& ycolor,
     const Quantity_Color& zcolor,
     /* Name of font for names of axes */
     const TCollection_AsciiString &fontOfNames,
     /* Style of names of axes */
     const Font_FontAspect styleOfNames,
     /* Size of names of axes */
     const Standard_Integer sizeOfNames,
     /* Name of font for values */
     const TCollection_AsciiString &fontOfValues,
     /* Style of values */
     const Font_FontAspect styleOfValues,
     /* Size of values */
     const Standard_Integer sizeOfValues)
{
    /* Names of axes */
    MyGTrihedron.xname = xname;
    MyGTrihedron.yname = yname;
    MyGTrihedron.zname = zname;
    /* Draw names */
    MyGTrihedron.xdrawname = xdrawname;
    MyGTrihedron.ydrawname = ydrawname;
    MyGTrihedron.zdrawname = zdrawname;
    /* Draw values */
    MyGTrihedron.xdrawvalues = xdrawvalues;
    MyGTrihedron.ydrawvalues = ydrawvalues;
    MyGTrihedron.zdrawvalues = zdrawvalues;
    /* Draw grid */
    MyGTrihedron.drawgrid = drawgrid;
    /* Draw axes */
    MyGTrihedron.drawaxes = drawaxes;
    /* Number of splits along axes */
    MyGTrihedron.nbx = nbx;
    MyGTrihedron.nby = nby;
    MyGTrihedron.nbz = nbz;
    /* Offset for drawing values */
    MyGTrihedron.xoffset = xoffset;
    MyGTrihedron.yoffset = yoffset;
    MyGTrihedron.zoffset = zoffset;
    /* Offset for drawing names of axes */
    MyGTrihedron.xaxisoffset = xaxisoffset;
    MyGTrihedron.yaxisoffset = yaxisoffset;
    MyGTrihedron.zaxisoffset = zaxisoffset;
    /* Draw tickmarks */
    MyGTrihedron.xdrawtickmarks = xdrawtickmarks;
    MyGTrihedron.ydrawtickmarks = ydrawtickmarks;
    MyGTrihedron.zdrawtickmarks = zdrawtickmarks;
    /* Length of tickmarks */
    MyGTrihedron.xtickmarklength = xtickmarklength;
    MyGTrihedron.ytickmarklength = ytickmarklength;
    MyGTrihedron.ztickmarklength = ztickmarklength;
    /* Grid color */
    MyGTrihedron.gridcolor = gridcolor;
    /* Colors of axis names */
    MyGTrihedron.xnamecolor = xnamecolor;
    MyGTrihedron.ynamecolor = ynamecolor;
    MyGTrihedron.znamecolor = znamecolor;
    /* Colors of axis and values */
    MyGTrihedron.xcolor = xcolor;
    MyGTrihedron.ycolor = ycolor;
    MyGTrihedron.zcolor = zcolor;
    /* Name of font for names of axes */
    MyGTrihedron.fontOfNames = fontOfNames;
    /* Style of names of axes */
    MyGTrihedron.styleOfNames = styleOfNames;
    /* Size of names of axes */
    MyGTrihedron.sizeOfNames = sizeOfNames;
    /* Name of font for values */
    MyGTrihedron.fontOfValues = fontOfValues;
    /* Style of values */
    MyGTrihedron.styleOfValues = styleOfValues;
    /* Size of values */
    MyGTrihedron.sizeOfValues = sizeOfValues;

    MyGTrihedron.ptrVisual3dView = this;
    MyGTrihedron.cbCubicAxes = SetMinMaxValuesCallback;
    MyGraphicDriver->GraduatedTrihedronDisplay(MyCView,MyGTrihedron);
}

void Visual3d_View::GraduatedTrihedronErase()
{
    MyGTrihedron.ptrVisual3dView = NULL;
    MyGraphicDriver->GraduatedTrihedronErase(MyCView);
}

const Handle(Visual3d_Layer)& Visual3d_View::UnderLayer () const {

        return (MyViewManager->UnderLayer ());

}

const Handle(Visual3d_Layer)& Visual3d_View::OverLayer () const {

        return (MyViewManager->OverLayer ());

}

Standard_Integer Visual3d_View::LightLimit() const {

        Standard_Integer maxlight = MyGraphicDriver->InquireLightLimit ();
//      printf(" $$$ Visual3d_View::LightLimit() is %d\n",maxlight);
        return maxlight;
}

Standard_Integer Visual3d_View::PlaneLimit() const {

        Standard_Integer maxplane = MyGraphicDriver->InquirePlaneLimit ();
//      printf(" $$$ Visual3d_View::PlaneLimit() is %d\n",maxplane);
        return maxplane;
}

Handle(Visual3d_ViewManager) Visual3d_View::ViewManager() const
{
  return MyPtrViewManager;
}

void Visual3d_View :: SetComputedMode ( const Standard_Boolean aMode )
{
  if ((aMode &&  ComputedModeIsActive) ||
     (!aMode && !ComputedModeIsActive))
  {
    return;
  }

 Graphic3d_MapIteratorOfMapOfStructure S1Iterator ( MyDisplayedStructure );
 Visual3d_TypeOfAnswer                 Answer;
 Standard_Integer                      StructId;
 Standard_Integer                      i = MyDisplayedStructure.Extent ();
 ComputedModeIsActive = aMode;
 if (!ComputedModeIsActive) {

  while (  S1Iterator.More ()  ) {

   Answer = AcceptDisplay (  S1Iterator.Key ()  );

   if ( Answer == Visual3d_TOA_COMPUTE ) {

    Standard_Integer Index = IsComputed (  S1Iterator.Key ()  );

    if ( Index != 0 ) {

     StructId = MyCOMPUTEDSequence.Value ( Index ) -> Identification ();

     MyGraphicDriver -> EraseStructure (
                         MyCView,
                         *( Graphic3d_CStructure* )
                          MyCOMPUTEDSequence.Value ( Index ) -> CStructure ()
                        );
     MyGraphicDriver -> DisplayStructure (
                         MyCView,
                                         *( Graphic3d_CStructure* )
                          S1Iterator.Key () -> CStructure (),
                         int (  S1Iterator.Key () -> DisplayPriority ()  )
                                    );
    }  // end if ( Index != 0 ) . . .

   }  // end if ( Answer . . . )

   S1Iterator.Next ();

  }  // end while

 } else {

  while (  S1Iterator.More ()  ) {

   Answer = AcceptDisplay (  S1Iterator.Key ()  );

   if ( Answer == Visual3d_TOA_COMPUTE ) {

    Standard_Integer Index = IsComputed (  S1Iterator.Key ()  );

    if ( Index != 0 ) {

     StructId = MyCOMPUTEDSequence.Value ( Index ) -> Identification ();

     MyGraphicDriver -> EraseStructure (
                         MyCView,
                         *( Graphic3d_CStructure* )
                          S1Iterator.Key () -> CStructure ()
                        );
     MyGraphicDriver -> DisplayStructure (
                         MyCView,
                                         *( Graphic3d_CStructure* )
                          MyCOMPUTEDSequence.Value ( Index ) -> CStructure (),
                                         int (  S1Iterator.Key () -> DisplayPriority ()  )
                        );

     Display (  S1Iterator.Key (), Aspect_TOU_WAIT  );

     if (  S1Iterator.Key () -> IsHighlighted ()  ) {

      if (  !MyCOMPUTEDSequence.Value ( Index ) -> IsHighlighted ()  ) {

       MyCOMPUTEDSequence.Value ( Index ) -> SetHighlightColor (
                                              S1Iterator.Key () ->
                                               HighlightColor ()
                                             );
       MyCOMPUTEDSequence.Value ( Index ) -> GraphicHighlight (
                                              Aspect_TOHM_COLOR
                                             );
      }  // end if

     }  // end if

    } else {

     Handle( Graphic3d_Structure ) AStructure = S1Iterator.Key ();
#ifdef OLD
     Handle( Graphic3d_Structure ) TheStructure = AStructure -> Compute ( this );
#else
     Handle( Graphic3d_Structure ) TheStructure;
     TColStd_Array2OfReal          ATrsf ( 0, 3, 0, 3 );

     AStructure -> Transform ( ATrsf );

     if ( Index != 0 ) {

      TColStd_Array2OfReal Ident ( 0, 3, 0, 3 );
      Standard_Integer     ii, jj;

      for ( ii = 0; ii <= 3; ++ii )

       for ( jj = 0; jj <= 3; ++jj  )

        Ident ( ii, jj ) = ( ii == jj ? 1.0 : 0.0 );

      TheStructure = MyCOMPUTEDSequence.Value ( Index );

      TheStructure -> SetTransform ( Ident, Graphic3d_TOC_REPLACE );

      if (  AStructure->IsTransformed ()  )

       AStructure -> Compute ( this, ATrsf, TheStructure );

      else

       AStructure -> Compute ( this, TheStructure );

     } else {

      if ( AStructure -> IsTransformed ()  )

       TheStructure = AStructure -> Compute ( this, ATrsf );

      else

       TheStructure = AStructure -> Compute ( this );

     }  // end else
#endif  // OLD
     TheStructure -> SetHLRValidation ( Standard_True );

     Visual3d_TypeOfVisualization ViewType = MyContext.Visualization ();

     Standard_Boolean ComputeWireframe =
      (  ( ViewType == Visual3d_TOV_WIREFRAME ) &&
             (  S1Iterator.Key () -> ComputeVisual () != Graphic3d_TOS_SHADING  )
      );

     Standard_Boolean ComputeShading =
      (  ( ViewType == Visual3d_TOV_SHADING ) &&
             (  S1Iterator.Key () -> ComputeVisual () != Graphic3d_TOS_WIREFRAME  )
      );

     if ( ComputeWireframe ) TheStructure -> SetVisual ( Graphic3d_TOS_WIREFRAME );
     if ( ComputeShading   ) TheStructure -> SetVisual ( Graphic3d_TOS_SHADING   );

     if (  S1Iterator.Key () -> IsHighlighted ()  ) {

      TheStructure -> SetHighlightColor (
                       S1Iterator.Key () -> HighlightColor ()
                      );
      TheStructure -> GraphicHighlight ( Aspect_TOHM_COLOR );

     }  // end if

     Standard_Integer Result = 0;
     Standard_Integer Length = MyTOCOMPUTESequence.Length ();
     StructId = S1Iterator.Key () -> Identification ();

     for ( i = 1; i <= Length && Result == 0; ++i )

      if (  MyTOCOMPUTESequence.Value ( i ) -> Identification () == StructId  )

       Result = i;

     if ( Result )

      MyCOMPUTEDSequence.ChangeValue ( Result ) = TheStructure;

     else {

      MyTOCOMPUTESequence.Append (  S1Iterator.Key ()  );
      MyCOMPUTEDSequence.Append  ( TheStructure );

     }  // end else

     MyGraphicDriver -> EraseStructure (
                         MyCView,
                         *( Graphic3d_CStructure* )
                          S1Iterator.Key () -> CStructure ()
                        );
     MyGraphicDriver -> DisplayStructure (
                         MyCView,
                         *( Graphic3d_CStructure* )TheStructure -> CStructure (),
                         int (  S1Iterator.Key () -> DisplayPriority ()  )
                        );
    }  // end else

   }  // end if

   S1Iterator.Next ();

  }  // end while

  if (  MyViewManager -> UpdateMode () == Aspect_TOU_ASAP  ) Update ();

 }  // end else

}  // end Visual3d_View :: SetComputedMode

Standard_Boolean Visual3d_View :: ComputedMode () const {

 return ComputedModeIsActive;

}  // end Visual3d_View :: ComputedMode

void Visual3d_View :: SetBackFacingModel (
                       const Visual3d_TypeOfBackfacingModel aModel
                      ) {
 switch ( aModel ) {

  default:
  case Visual3d_TOBM_AUTOMATIC:
   MyCView.Backfacing = 0;
   break;

  case Visual3d_TOBM_FORCE:
   MyCView.Backfacing = 1;
   break;

  case Visual3d_TOBM_DISABLE:
   MyCView.Backfacing = -1;
   break;

 }  // end switch

 MyGraphicDriver -> SetBackFacingModel ( MyCView );

}  // end Visual3d_View :: SetBackFacingModel

Visual3d_TypeOfBackfacingModel Visual3d_View :: BackFacingModel () const {

 switch ( MyCView.Backfacing ) {

  case 0:
   return Visual3d_TOBM_AUTOMATIC;

  case 1:
   return Visual3d_TOBM_FORCE;

 }  // end switch

 return Visual3d_TOBM_DISABLE;

}  // end Visual3d_View :: BackFacingModel

void Visual3d_View::EnableDepthTest( const Standard_Boolean enable ) const
{
  MyGraphicDriver->SetDepthTestEnabled( MyCView, enable );
}

Standard_Boolean Visual3d_View::IsDepthTestEnabled() const
{
  return MyGraphicDriver->IsDepthTestEnabled( MyCView );
}

void Visual3d_View::ReadDepths(const Standard_Integer x,
                               const Standard_Integer y,
                               const Standard_Integer width,
                               const Standard_Integer height,
                               const Standard_Address buffer) const
{
  MyGraphicDriver->ReadDepths( MyCView, x, y, width, height, buffer );
}

Graphic3d_PtrFrameBuffer Visual3d_View::FBOCreate(const Standard_Integer theWidth,
                                                  const Standard_Integer theHeight)
{
  return MyGraphicDriver->FBOCreate( MyCView, theWidth, theHeight );
}

void Visual3d_View::FBORelease(Graphic3d_PtrFrameBuffer& theFBOPtr)
{
  MyGraphicDriver->FBORelease( MyCView, theFBOPtr );
}

void Visual3d_View::FBOGetDimensions(const Graphic3d_PtrFrameBuffer theFBOPtr,
                                     Standard_Integer& theWidth,    Standard_Integer& theHeight,
                                     Standard_Integer& theWidthMax, Standard_Integer& theHeightMax)
{
  MyGraphicDriver->FBOGetDimensions( MyCView, theFBOPtr,
                                     theWidth, theHeight,
                                     theWidthMax, theHeightMax );
}

void Visual3d_View::FBOChangeViewport(Graphic3d_PtrFrameBuffer& theFBOPtr,
                                      const Standard_Integer theWidth, const Standard_Integer theHeight)
{
  MyGraphicDriver->FBOChangeViewport( MyCView, theFBOPtr,
                                     theWidth, theHeight );
}

Standard_Boolean Visual3d_View::BufferDump (Image_PixMap&               theImage,
                                            const Graphic3d_BufferType& theBufferType)
{
  return MyGraphicDriver->BufferDump (MyCView, theImage, theBufferType);
}

void Visual3d_View::EnableGLLight( const Standard_Boolean enable ) const
{
  MyGraphicDriver->SetGLLightEnabled( MyCView, enable );
}


Standard_Boolean Visual3d_View::IsGLLightEnabled() const
{
  return MyGraphicDriver->IsGLLightEnabled( MyCView );
}

Standard_Boolean Visual3d_View::Export (const Standard_CString       theFileName,
                                        const Graphic3d_ExportFormat theFormat,
                                        const Graphic3d_SortType     theSortType,
                                        const Standard_Real          thePrecision,
                                        const Standard_Address       theProgressBarFunc,
                                        const Standard_Address       theProgressObject) const
{
  Handle(Visual3d_Layer) anUnderLayer = MyViewManager->UnderLayer();
  Handle(Visual3d_Layer) anOverLayer  = MyViewManager->OverLayer();

  Aspect_CLayer2d anOverCLayer;
  Aspect_CLayer2d anUnderCLayer;
  anOverCLayer.ptrLayer = anUnderCLayer.ptrLayer = NULL;

  if (!anOverLayer.IsNull())
    anOverCLayer = anOverLayer->CLayer();
  if (!anUnderLayer.IsNull())
    anUnderCLayer = anUnderLayer->CLayer();

  Standard_Integer aWidth, aHeight;
  Window()->Size (aWidth, aHeight);

  return MyGraphicDriver->Export (theFileName, theFormat, theSortType,
                                  aWidth, aHeight, MyCView, anUnderCLayer, anOverCLayer,
                                  thePrecision, theProgressBarFunc, theProgressObject);
}

//=======================================================================
//function : AddZLayer
//purpose  :
//=======================================================================

void Visual3d_View::AddZLayer (const Standard_Integer theLayerId)
{
  MyGraphicDriver->AddZLayer (MyCView, theLayerId);
}

//=======================================================================
//function : RemoveZLayer
//purpose  :
//=======================================================================

void Visual3d_View::RemoveZLayer (const Standard_Integer theLayerId)
{
  MyGraphicDriver->RemoveZLayer (MyCView, theLayerId);
}

//=======================================================================
//function : ChangeZLayer
//purpose  :
//=======================================================================

void Visual3d_View::ChangeZLayer (const Handle(Graphic3d_Structure)& theStructure,
                                  const Standard_Integer theLayerId)
{
  MyGraphicDriver->ChangeZLayer (
    (*(Graphic3d_CStructure*)theStructure->CStructure()), MyCView, theLayerId);
}
