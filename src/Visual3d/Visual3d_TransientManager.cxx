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
        Classe Visual3d_TransientManager :


        Declaration des variables specifiques au mode transient.

        Une vue est definie par :
                - un ViewManager
                - un ContextView
                - un ViewMapping
                - une ViewOrientation

     HISTORIQUE DES MODIFICATIONS   :
     --------------------------------
      October 1995 : GG : Creation.
      20-11-97  : CAL ; Disparition de la dependance avec math
      01-05-97 : CAL ; Ajout du Clear sur les TOS_COMPUTED.
      18-12-97 : FMN ; Ajout mode AJOUT.
      27-12-98 : FMN ; PERF: OPTIMISATION LOADER (LOPTIM)
      10-06-98 : CAL ; Modification de la signature de DrawStructure.
      25-09-98 : CAL ; Mise a jour de theCView = *(CALL_DEF_VIEW *)AView->CView ();
      10-11-98 : CAL ; PRO16361. theCView dans ClearImmediatMode.
      30-11-98 : FMN ; S4069 : Textes toujours visibles.
      01-12-98 : CAL ; S4062. Ajout des layers.

************************************************************************/

// for the class
#include <Visual3d_TransientManager.ixx>
#include <Visual3d_ViewPtr.hxx>
#include <Visual3d_View.pxx>

#include <Aspect_CLayer2d.hxx>
#include <Graphic3d_CView.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_TypeOfPrimitive.hxx>
#include <Visual3d_Layer.hxx>

enum TypeOfImmediat {
 Immediat_None,
 Immediat_Transient,
 Immediat_Ajout
};

//-Global data definitions
static Standard_Integer theDrawingState = 0;
static TypeOfImmediat theImmediatState = Immediat_None;

static Handle(Graphic3d_GraphicDriver)& _theGraphicDriver() {
    static Handle(Graphic3d_GraphicDriver) theGraphicDriver;
return theGraphicDriver;
}
#define theGraphicDriver _theGraphicDriver()

static Graphic3d_CView& _theCView() {
    static Graphic3d_CView theCView;
return theCView;
}
#define theCView _theCView()

static Aspect_CLayer2d UnderCLayer;
static Aspect_CLayer2d OverCLayer;

//
//-Constructors
//

Visual3d_TransientManager::Visual3d_TransientManager () {
}

//
//-Destructors
//

void Visual3d_TransientManager::Destroy () {
}

//-Methods, in order

Standard_Boolean Visual3d_TransientManager::BeginDraw (const Handle(Visual3d_View)& AView, const Standard_Boolean DoubleBuffer, const Standard_Boolean RetainMode) {

	if (theDrawingState > 0) {
	  Graphic3d_CView* pview = (Graphic3d_CView*) AView->CView();
	  if( theImmediatState == Immediat_Transient &&
	      pview->ViewId == theCView.ViewId ) {
	    theDrawingState++;
	    return theDrawingState;
	  } else
		Visual3d_TransientDefinitionError::Raise
			("Drawing in progress !");
	}

Handle(Visual3d_Layer) OverLayer = AView->OverLayer ();
Handle(Visual3d_Layer) UnderLayer = AView->UnderLayer ();
  OverCLayer.ptrLayer = UnderCLayer.ptrLayer = NULL;
  theCView = *(Graphic3d_CView* )AView->CView ();

	if (! UnderLayer.IsNull ()){
		UnderCLayer = UnderLayer->CLayer();
		theCView.ptrUnderLayer = (CALL_DEF_LAYER *) &UnderCLayer;
	}
	if (! OverLayer.IsNull ()){
		OverCLayer = OverLayer->CLayer();
		theCView.ptrOverLayer = (CALL_DEF_LAYER *) &OverCLayer;
	}

	// Begin rendering
	theGraphicDriver = AView->GraphicDriver();

	if (theGraphicDriver->BeginImmediatMode
		(theCView, UnderCLayer, OverCLayer, DoubleBuffer, RetainMode))
  {
		theDrawingState++;
		theImmediatState = Immediat_Transient;
	}

	return theDrawingState;
}

void Visual3d_TransientManager::EndDraw (const Standard_Boolean Synchronize) {

	if( theDrawingState <= 0 )
	  Visual3d_TransientDefinitionError::Raise ("Drawing not started !");

	theDrawingState--;
	if( theDrawingState > 0 ) return;
	theImmediatState = Immediat_None;

			// Flush all graphics
	theGraphicDriver->EndImmediatMode(Synchronize);
}

void Visual3d_TransientManager::ClearDraw (const Handle(Visual3d_View)& AView,
                                           const Standard_Boolean aFlush)
{
	if (theDrawingState > 0)
		Visual3d_TransientDefinitionError::Raise
			("Drawing in progress !");

	// Begin rendering
	theCView	= *(Graphic3d_CView* )AView->CView ();
  if (!AView->UnderLayer().IsNull()) {
    UnderCLayer = AView->UnderLayer()->CLayer();
    theCView.ptrUnderLayer = (CALL_DEF_LAYER *) &UnderCLayer;
  }
  if (!AView->OverLayer().IsNull()) {
    OverCLayer = AView->OverLayer()->CLayer();
    theCView.ptrOverLayer = (CALL_DEF_LAYER *) &OverCLayer;
  }

	theGraphicDriver = AView->GraphicDriver();
	theGraphicDriver->ClearImmediatMode (theCView, aFlush);
}

//
//-Mode Ajout
//

Standard_Boolean Visual3d_TransientManager::BeginAddDraw (const Handle(Visual3d_View)& AView)
{
	if (theDrawingState > 0)
  {
	  Graphic3d_CView* pview = (Graphic3d_CView* )AView->CView();
	  if( theImmediatState == Immediat_Ajout &&
	      pview->ViewId == theCView.ViewId ) {
	    theDrawingState++;
	    return theDrawingState;
	  } else
		Visual3d_TransientDefinitionError::Raise
			("Drawing in progress !");
	}

	// Begin rendering
	theCView	= *(Graphic3d_CView* )AView->CView ();
  if (!AView->UnderLayer().IsNull()) {
    UnderCLayer = AView->UnderLayer()->CLayer();
    theCView.ptrUnderLayer = (CALL_DEF_LAYER *) &UnderCLayer;
  }
  if (!AView->OverLayer().IsNull()) {
    OverCLayer = AView->OverLayer()->CLayer();
    theCView.ptrOverLayer = (CALL_DEF_LAYER *) &OverCLayer;
  }

	theGraphicDriver = AView->GraphicDriver ();

	if (theGraphicDriver->BeginAddMode (theCView))
  {
		theDrawingState++;
		theImmediatState = Immediat_Ajout;
	}

	return theDrawingState;
}

void Visual3d_TransientManager::EndAddDraw () {

	if( theDrawingState <= 0 )
	  Visual3d_TransientDefinitionError::Raise ("Drawing not started !");

	theDrawingState--;
	if( theDrawingState > 0 ) return;
	theImmediatState = Immediat_None;
			// Flush all graphics
	theGraphicDriver->EndAddMode();
}


//
//-Graphic definition methods
//

void Visual3d_TransientManager::DrawStructure (const Handle(Graphic3d_Structure)& theStructure)
{
	if (!theDrawingState)
  {
		Visual3d_TransientDefinitionError::Raise ("Drawing is not open !");
  }
  else if (!theStructure->IsEmpty())
  {
		theGraphicDriver->DrawStructure (*theStructure->CStructure());
	}
}
