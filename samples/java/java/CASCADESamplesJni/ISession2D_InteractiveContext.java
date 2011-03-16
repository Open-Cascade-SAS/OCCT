// Java Native Class from Cas.Cade
//                     Copyright (C) 1991,1999 by
//  
//                      MATRA DATAVISION, FRANCE
//  
// This software is furnished in accordance with the terms and conditions
// of the contract and with the inclusion of the above copyright notice.
// This software or any other copy thereof may not be provided or otherwise
// be made available to any other person. No title to an ownership of the
// software is hereby transferred.
//  
// At the termination of the contract, the software and all copies of this
// software must be deleted.
//


package CASCADESamplesJni;

import jcas.Object;
import CASCADESamplesJni.V2d_Viewer;
import CASCADESamplesJni.AIS_InteractiveObject;
import jcas.Standard_Boolean;
import jcas.Standard_Integer;
import CASCADESamplesJni.V2d_View;


public class ISession2D_InteractiveContext extends CASCADESamplesJni.MMgt_TShared {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
public ISession2D_InteractiveContext() {
   ISession2D_InteractiveContext_Create_1();
}

private final native void ISession2D_InteractiveContext_Create_1();

public ISession2D_InteractiveContext(V2d_Viewer aViewer) {
   ISession2D_InteractiveContext_Create_2(aViewer);
}

private final native void ISession2D_InteractiveContext_Create_2(V2d_Viewer aViewer);

native public  void Initialize(V2d_Viewer aViewer);
 public void Display(AIS_InteractiveObject anObject,boolean Redraw) {
    ISession2D_InteractiveContext_Display_1(anObject,Redraw);
}

private  native void ISession2D_InteractiveContext_Display_1(AIS_InteractiveObject anObject,boolean Redraw);

final public void Display(AIS_InteractiveObject anObject,int aDisplayMode,int aSelectionMode,boolean Redraw) {
    ISession2D_InteractiveContext_Display_2(anObject,aDisplayMode,aSelectionMode,Redraw);
}

private final native void ISession2D_InteractiveContext_Display_2(AIS_InteractiveObject anObject,int aDisplayMode,int aSelectionMode,boolean Redraw);

native public  void Erase(AIS_InteractiveObject anObject,boolean Redraw);
native public  void DisplayAll(boolean Redraw);
native public  void EraseAll(boolean Redraw);
native public  boolean IsDisplayed(AIS_InteractiveObject anObject,int aMode);
native public  void Redisplay(AIS_InteractiveObject anObject,boolean Redraw,boolean allmodes);
native public  void Clear(AIS_InteractiveObject anObject,boolean Redraw);
native public  void Remove(AIS_InteractiveObject anObject,boolean Redraw);
native public  void Highlight(AIS_InteractiveObject anObject,boolean Redraw);
native public  void Unhighlight(AIS_InteractiveObject anObject,boolean Redraw);
native public  boolean IsHilighted(AIS_InteractiveObject anObject);
 public void Move(int x1,int y1,V2d_View aView) {
    ISession2D_InteractiveContext_Move_1(x1,y1,aView);
}

private  native void ISession2D_InteractiveContext_Move_1(int x1,int y1,V2d_View aView);

 public void Move(int x1,int y1,int x2,int y2,V2d_View aView) {
    ISession2D_InteractiveContext_Move_2(x1,y1,x2,y2,aView);
}

private  native void ISession2D_InteractiveContext_Move_2(int x1,int y1,int x2,int y2,V2d_View aView);

native public  void Pick(boolean MultiSelection);
native public final void DisplayAreas();
native public final void ClearAreas();



}
