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
import CASCADESamplesJni.V3d_Viewer;
import jcas.Standard_Boolean;
import CASCADESamplesJni.AIS_InteractiveObject;
import jcas.Standard_Integer;
import CASCADESamplesJni.Quantity_NameOfColor;
import jcas.Standard_Short;
import CASCADESamplesJni.AIS_KindOfInteractive;
import jcas.Standard_Real;
import CASCADESamplesJni.TopLoc_Location;
import CASCADESamplesJni.Aspect_TypeOfFacingModel;
import CASCADESamplesJni.Quantity_Color;
import CASCADESamplesJni.Graphic3d_NameOfMaterial;
import CASCADESamplesJni.Aspect_TypeOfDegenerateModel;
import CASCADESamplesJni.AIS_Drawer;
import CASCADESamplesJni.AIS_DisplayStatus;
import CASCADESamplesJni.TColStd_ListOfInteger;
import CASCADESamplesJni.TCollection_ExtendedString;
import CASCADESamplesJni.AIS_DisplayMode;
import CASCADESamplesJni.Prs3d_LineAspect;
import CASCADESamplesJni.AIS_TypeOfIso;
import CASCADESamplesJni.Prs3d_BasicAspect;
import CASCADESamplesJni.AIS_StatusOfDetection;
import CASCADESamplesJni.V3d_View;
import CASCADESamplesJni.AIS_StatusOfPick;
import CASCADESamplesJni.TColgp_Array1OfPnt2d;
import CASCADESamplesJni.TopoDS_Shape;
import CASCADESamplesJni.SelectMgr_EntityOwner;
import CASCADESamplesJni.Standard_Transient;
import CASCADESamplesJni.AIS_ClearMode;
import CASCADESamplesJni.Geom_Transformation;
import CASCADESamplesJni.Prs3d_Drawer;
import CASCADESamplesJni.SelectMgr_Filter;
import CASCADESamplesJni.TopAbs_ShapeEnum;
import CASCADESamplesJni.SelectMgr_ListOfFilter;
import CASCADESamplesJni.AIS_ListOfInteractive;
import CASCADESamplesJni.TCollection_AsciiString;
import jcas.Standard_CString;
import CASCADESamplesJni.SelectMgr_SelectionManager;
import CASCADESamplesJni.PrsMgr_PresentationManager3d;
import CASCADESamplesJni.StdSelect_ViewerSelector3d;


public class AIS_InteractiveContext extends CASCADESamplesJni.MMgt_TShared {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
public AIS_InteractiveContext(V3d_Viewer MainViewer) {
   AIS_InteractiveContext_Create_1(MainViewer);
}

private final native void AIS_InteractiveContext_Create_1(V3d_Viewer MainViewer);

public AIS_InteractiveContext(V3d_Viewer MainViewer,V3d_Viewer Collector) {
   AIS_InteractiveContext_Create_2(MainViewer,Collector);
}

private final native void AIS_InteractiveContext_Create_2(V3d_Viewer MainViewer,V3d_Viewer Collector);

native public final boolean IsCollectorClosed();
native public final void CloseCollector();
native public final void OpenCollector();
final public void Display(AIS_InteractiveObject anIobj,boolean updateviewer) {
    AIS_InteractiveContext_Display_1(anIobj,updateviewer);
}

private final native void AIS_InteractiveContext_Display_1(AIS_InteractiveObject anIobj,boolean updateviewer);

final public void Display(AIS_InteractiveObject anIobj,int amode,int aSelectionMode,boolean updateviewer,boolean allowdecomposition) {
    AIS_InteractiveContext_Display_2(anIobj,amode,aSelectionMode,updateviewer,allowdecomposition);
}

private final native void AIS_InteractiveContext_Display_2(AIS_InteractiveObject anIobj,int amode,int aSelectionMode,boolean updateviewer,boolean allowdecomposition);

native public final void Load(AIS_InteractiveObject aniobj,int SelectionMode,boolean AllowDecomp);
native public final void Erase(AIS_InteractiveObject aniobj,boolean updateviewer,boolean PutInCollector);
native public final void EraseMode(AIS_InteractiveObject aniobj,int aMode,boolean updateviewer);
native public final void EraseAll(boolean PutInCollector,boolean updateviewer);
native public final void DisplayAll(boolean OnlyFromCollector,boolean updateviewer);
native public final void DisplayFromCollector(AIS_InteractiveObject anIObj,boolean updateviewer);
native public final void EraseSelected(boolean PutInCollector,boolean updateviewer);
native public final void DisplaySelected(boolean updateviewer);
native public final boolean KeepTemporary(AIS_InteractiveObject anIObj,int InWhichLocal);
native public final void Clear(AIS_InteractiveObject aniobj,boolean updateviewer);
native public final void ClearPrs(AIS_InteractiveObject aniobj,int aMode,boolean updateviewer);
native public final void Remove(AIS_InteractiveObject aniobj,boolean updateviewer);
native public final void Hilight(AIS_InteractiveObject aniobj,boolean updateviewer);
native public final void HilightWithColor(AIS_InteractiveObject aniobj,short aCol,boolean updateviewer);
native public final void Unhilight(AIS_InteractiveObject aniobj,boolean updateviewer);
native public final void SetDisplayPriority(AIS_InteractiveObject anIobj,int aPriority);
final public void Redisplay(AIS_InteractiveObject aniobj,boolean updateviewer,boolean allmodes) {
    AIS_InteractiveContext_Redisplay_1(aniobj,updateviewer,allmodes);
}

private final native void AIS_InteractiveContext_Redisplay_1(AIS_InteractiveObject aniobj,boolean updateviewer,boolean allmodes);

final public void Redisplay(short aTypeOfObject,int Signature,boolean updateviewer) {
    AIS_InteractiveContext_Redisplay_2(aTypeOfObject,Signature,updateviewer);
}

private final native void AIS_InteractiveContext_Redisplay_2(short aTypeOfObject,int Signature,boolean updateviewer);

native public final void RecomputePrsOnly(AIS_InteractiveObject anIobj,boolean updateviewer,boolean allmodes);
native public final void RecomputeSelectionOnly(AIS_InteractiveObject anIObj);
native public final void Update(AIS_InteractiveObject anIobj,boolean updateviewer);
final public void SetDisplayMode(AIS_InteractiveObject aniobj,int aMode,boolean updateviewer) {
    AIS_InteractiveContext_SetDisplayMode_1(aniobj,aMode,updateviewer);
}

private final native void AIS_InteractiveContext_SetDisplayMode_1(AIS_InteractiveObject aniobj,int aMode,boolean updateviewer);

native public final void UnsetDisplayMode(AIS_InteractiveObject aniobj,boolean updateviewer);
native public final void SetSelectionMode(AIS_InteractiveObject aniobj,int aMode);
native public final void UnsetSelectionMode(AIS_InteractiveObject aniobj);
final public void SetSensitivity(double aPrecision) {
    AIS_InteractiveContext_SetSensitivity_1(aPrecision);
}

private final native void AIS_InteractiveContext_SetSensitivity_1(double aPrecision);

final public void SetSensitivity(int aPrecision) {
    AIS_InteractiveContext_SetSensitivity_2(aPrecision);
}

private final native void AIS_InteractiveContext_SetSensitivity_2(int aPrecision);

native public final void SetLocation(AIS_InteractiveObject aniobj,TopLoc_Location aLocation);
native public final void ResetLocation(AIS_InteractiveObject aniobj);
native public final boolean HasLocation(AIS_InteractiveObject aniobj);
native public final TopLoc_Location Location(AIS_InteractiveObject aniobj);
native public final void SetCurrentFacingModel(AIS_InteractiveObject aniobj,short aModel);
final public void SetColor(AIS_InteractiveObject aniobj,short aColor,boolean updateviewer) {
    AIS_InteractiveContext_SetColor_1(aniobj,aColor,updateviewer);
}

private final native void AIS_InteractiveContext_SetColor_1(AIS_InteractiveObject aniobj,short aColor,boolean updateviewer);

final public void SetColor(AIS_InteractiveObject aniobj,Quantity_Color aColor,boolean updateviewer) {
    AIS_InteractiveContext_SetColor_2(aniobj,aColor,updateviewer);
}

private final native void AIS_InteractiveContext_SetColor_2(AIS_InteractiveObject aniobj,Quantity_Color aColor,boolean updateviewer);

native public final void UnsetColor(AIS_InteractiveObject aniobj,boolean updateviewer);
native public  void SetWidth(AIS_InteractiveObject aniobj,double aValue,boolean updateviewer);
native public  void UnsetWidth(AIS_InteractiveObject aniobj,boolean updateviewer);
native public final void SetMaterial(AIS_InteractiveObject aniobj,short aName,boolean updateviewer);
native public final void UnsetMaterial(AIS_InteractiveObject anObj,boolean updateviewer);
native public final void SetTransparency(AIS_InteractiveObject aniobj,double aValue,boolean updateviewer);
native public final void UnsetTransparency(AIS_InteractiveObject aniobj,boolean updateviewer);
final public void SetDegenerateModel(AIS_InteractiveObject aniobj,short aModel,double aRatio) {
    AIS_InteractiveContext_SetDegenerateModel_1(aniobj,aModel,aRatio);
}

private final native void AIS_InteractiveContext_SetDegenerateModel_1(AIS_InteractiveObject aniobj,short aModel,double aRatio);

final public void SetDegenerateModel(short aModel,double aSkipRatio) {
    AIS_InteractiveContext_SetDegenerateModel_2(aModel,aSkipRatio);
}

private final native void AIS_InteractiveContext_SetDegenerateModel_2(short aModel,double aSkipRatio);

native public final void SetLocalAttributes(AIS_InteractiveObject aniobj,AIS_Drawer aDrawer,boolean updateviewer);
native public final void UnsetLocalAttributes(AIS_InteractiveObject anObj,boolean updateviewer);
native public final void SetTrihedronSize(double aSize,boolean updateviewer);
native public final double TrihedronSize();
final public void SetPlaneSize(double aSizeX,double aSizeY,boolean updateviewer) {
    AIS_InteractiveContext_SetPlaneSize_1(aSizeX,aSizeY,updateviewer);
}

private final native void AIS_InteractiveContext_SetPlaneSize_1(double aSizeX,double aSizeY,boolean updateviewer);

final public void SetPlaneSize(double aSize,boolean updateviewer) {
    AIS_InteractiveContext_SetPlaneSize_2(aSize,updateviewer);
}

private final native void AIS_InteractiveContext_SetPlaneSize_2(double aSize,boolean updateviewer);

native public final boolean PlaneSize(Standard_Real XSize,Standard_Real YSize);
native public final short DisplayStatus(AIS_InteractiveObject anIobj);
native public final TColStd_ListOfInteger DisplayedModes(AIS_InteractiveObject aniobj);
final public boolean IsDisplayed(AIS_InteractiveObject anIobj) {
   return AIS_InteractiveContext_IsDisplayed_1(anIobj);
}

private final native boolean AIS_InteractiveContext_IsDisplayed_1(AIS_InteractiveObject anIobj);

final public boolean IsDisplayed(AIS_InteractiveObject aniobj,int aMode) {
   return AIS_InteractiveContext_IsDisplayed_2(aniobj,aMode);
}

private final native boolean AIS_InteractiveContext_IsDisplayed_2(AIS_InteractiveObject aniobj,int aMode);

final public boolean IsHilighted(AIS_InteractiveObject aniobj) {
   return AIS_InteractiveContext_IsHilighted_1(aniobj);
}

private final native boolean AIS_InteractiveContext_IsHilighted_1(AIS_InteractiveObject aniobj);

final public boolean IsHilighted(AIS_InteractiveObject anIobj,Standard_Boolean WithColor,Standard_Short theHiCol) {
   return AIS_InteractiveContext_IsHilighted_2(anIobj,WithColor,theHiCol);
}

private final native boolean AIS_InteractiveContext_IsHilighted_2(AIS_InteractiveObject anIobj,Standard_Boolean WithColor,Standard_Short theHiCol);

native public final boolean IsInCollector(AIS_InteractiveObject anIObj);
native public final int DisplayPriority(AIS_InteractiveObject anIobj);
native public final boolean HasColor(AIS_InteractiveObject aniobj);
final public short Color(AIS_InteractiveObject aniobj) {
   return AIS_InteractiveContext_Color_1(aniobj);
}

private final native short AIS_InteractiveContext_Color_1(AIS_InteractiveObject aniobj);

final public void Color(AIS_InteractiveObject aniobj,Quantity_Color acolor) {
    AIS_InteractiveContext_Color_2(aniobj,acolor);
}

private final native void AIS_InteractiveContext_Color_2(AIS_InteractiveObject aniobj,Quantity_Color acolor);

native public  double Width(AIS_InteractiveObject aniobj);
native public final void Status(AIS_InteractiveObject anObj,TCollection_ExtendedString astatus);
native public final void UpdateCurrentViewer();
native public final void UpdateCollector();
native public final int DisplayMode();
native public final short HilightColor();
final public short SelectionColor() {
   return AIS_InteractiveContext_SelectionColor_1();
}

private final native short AIS_InteractiveContext_SelectionColor_1();

native public final short PreSelectionColor();
native public final short DefaultColor();
native public final short SubIntensityColor();
native public final void SetHilightColor(short aHiCol);
final public void SelectionColor(short aCol) {
    AIS_InteractiveContext_SelectionColor_2(aCol);
}

private final native void AIS_InteractiveContext_SelectionColor_2(short aCol);

native public final void SetPreselectionColor(short aCol);
native public final void SetSubIntensityColor(short aCol);
final public void SetDisplayMode(short AMode,boolean updateviewer) {
    AIS_InteractiveContext_SetDisplayMode_2(AMode,updateviewer);
}

private final native void AIS_InteractiveContext_SetDisplayMode_2(short AMode,boolean updateviewer);

final public void SetDeviationCoefficient(AIS_InteractiveObject aniobj,double aCoefficient,boolean updateviewer) {
    AIS_InteractiveContext_SetDeviationCoefficient_1(aniobj,aCoefficient,updateviewer);
}

private final native void AIS_InteractiveContext_SetDeviationCoefficient_1(AIS_InteractiveObject aniobj,double aCoefficient,boolean updateviewer);

final public void SetDeviationAngle(AIS_InteractiveObject aniobj,double anAngle,boolean updateviewer) {
    AIS_InteractiveContext_SetDeviationAngle_1(aniobj,anAngle,updateviewer);
}

private final native void AIS_InteractiveContext_SetDeviationAngle_1(AIS_InteractiveObject aniobj,double anAngle,boolean updateviewer);

native public final void SetAngleAndDeviation(AIS_InteractiveObject aniobj,double anAngle,boolean updateviewer);
final public void SetHLRDeviationCoefficient(AIS_InteractiveObject aniobj,double aCoefficient,boolean updateviewer) {
    AIS_InteractiveContext_SetHLRDeviationCoefficient_1(aniobj,aCoefficient,updateviewer);
}

private final native void AIS_InteractiveContext_SetHLRDeviationCoefficient_1(AIS_InteractiveObject aniobj,double aCoefficient,boolean updateviewer);

native public final void SetHLRDeviationAngle(AIS_InteractiveObject aniobj,double anAngle,boolean updateviewer);
final public void SetHLRAngleAndDeviation(AIS_InteractiveObject aniobj,double anAngle,boolean updateviewer) {
    AIS_InteractiveContext_SetHLRAngleAndDeviation_1(aniobj,anAngle,updateviewer);
}

private final native void AIS_InteractiveContext_SetHLRAngleAndDeviation_1(AIS_InteractiveObject aniobj,double anAngle,boolean updateviewer);

final public void SetDeviationCoefficient(double aCoefficient) {
    AIS_InteractiveContext_SetDeviationCoefficient_2(aCoefficient);
}

private final native void AIS_InteractiveContext_SetDeviationCoefficient_2(double aCoefficient);

native public final double DeviationCoefficient();
final public void SetDeviationAngle(double anAngle) {
    AIS_InteractiveContext_SetDeviationAngle_2(anAngle);
}

private final native void AIS_InteractiveContext_SetDeviationAngle_2(double anAngle);

native public final double DeviationAngle();
final public void SetHLRDeviationCoefficient(double aCoefficient) {
    AIS_InteractiveContext_SetHLRDeviationCoefficient_2(aCoefficient);
}

private final native void AIS_InteractiveContext_SetHLRDeviationCoefficient_2(double aCoefficient);

native public final double HLRDeviationCoefficient();
native public final void SetHLRAngle(double anAngle);
native public final double HLRAngle();
final public void SetHLRAngleAndDeviation(double anAngle) {
    AIS_InteractiveContext_SetHLRAngleAndDeviation_2(anAngle);
}

private final native void AIS_InteractiveContext_SetHLRAngleAndDeviation_2(double anAngle);

native public final Prs3d_LineAspect HiddenLineAspect();
native public final void SetHiddenLineAspect(Prs3d_LineAspect anAspect);
native public final boolean DrawHiddenLine();
native public final void EnableDrawHiddenLine();
native public final void DisableDrawHiddenLine();
native public final void SetIsoNumber(int NbIsos,short WhichIsos);
native public final int IsoNumber(short WhichIsos);
final public void IsoOnPlane(boolean SwitchOn) {
    AIS_InteractiveContext_IsoOnPlane_1(SwitchOn);
}

private final native void AIS_InteractiveContext_IsoOnPlane_1(boolean SwitchOn);

final public boolean IsoOnPlane() {
   return AIS_InteractiveContext_IsoOnPlane_2();
}

private final native boolean AIS_InteractiveContext_IsoOnPlane_2();

native public final void SetSelectedAspect(Prs3d_BasicAspect anAspect,boolean globalChange,boolean updateViewer);
native public final short MoveTo(int XPix,int YPix,V3d_View aView);
native public final boolean HasNextDetected();
native public final int HilightNextDetected(V3d_View aView);
native public final int HilightPreviousDetected(V3d_View aView);
final public short Select(int XPMin,int YPMin,int XPMax,int YPMax,V3d_View aView,boolean updateviewer) {
   return AIS_InteractiveContext_Select_1(XPMin,YPMin,XPMax,YPMax,aView,updateviewer);
}

private final native short AIS_InteractiveContext_Select_1(int XPMin,int YPMin,int XPMax,int YPMax,V3d_View aView,boolean updateviewer);

final public short Select(TColgp_Array1OfPnt2d Polyline,V3d_View aView,boolean updateviewer) {
   return AIS_InteractiveContext_Select_2(Polyline,aView,updateviewer);
}

private final native short AIS_InteractiveContext_Select_2(TColgp_Array1OfPnt2d Polyline,V3d_View aView,boolean updateviewer);

final public short Select(boolean updateviewer) {
   return AIS_InteractiveContext_Select_3(updateviewer);
}

private final native short AIS_InteractiveContext_Select_3(boolean updateviewer);

final public short ShiftSelect(boolean updateviewer) {
   return AIS_InteractiveContext_ShiftSelect_1(updateviewer);
}

private final native short AIS_InteractiveContext_ShiftSelect_1(boolean updateviewer);

final public short ShiftSelect(int XPMin,int YPMin,int XPMax,int YPMax,V3d_View aView,boolean updateviewer) {
   return AIS_InteractiveContext_ShiftSelect_2(XPMin,YPMin,XPMax,YPMax,aView,updateviewer);
}

private final native short AIS_InteractiveContext_ShiftSelect_2(int XPMin,int YPMin,int XPMax,int YPMax,V3d_View aView,boolean updateviewer);

native public final void SetCurrentObject(AIS_InteractiveObject aniobj,boolean updateviewer);
native public final void AddOrRemoveCurrentObject(AIS_InteractiveObject aniobj,boolean updateviewer);
native public final void UpdateCurrent();
native public final boolean WasCurrentTouched();
native public final void SetOkCurrent();
native public final boolean IsCurrent(AIS_InteractiveObject aniobj);
native public final void InitCurrent();
native public final boolean MoreCurrent();
native public final void NextCurrent();
native public final AIS_InteractiveObject Current();
native public final int NbCurrents();
native public final AIS_InteractiveObject FirstCurrentObject();
native public final void HilightCurrents(boolean updateviewer);
native public final void UnhilightCurrents(boolean updateviewer);
native public final void ClearCurrents(boolean updateviewer);
native public final void SetSelected(AIS_InteractiveObject aniObj,boolean updateviewer);
native public final void SetSelectedCurrent();
native public final void UpdateSelected(boolean updateviewer);
final public void AddOrRemoveSelected(AIS_InteractiveObject aniobj,boolean updateviewer) {
    AIS_InteractiveContext_AddOrRemoveSelected_1(aniobj,updateviewer);
}

private final native void AIS_InteractiveContext_AddOrRemoveSelected_1(AIS_InteractiveObject aniobj,boolean updateviewer);

native public final void HilightSelected(boolean updateviewer);
native public final void UnhilightSelected(boolean updateviewer);
native public final void ClearSelected(boolean updateviewer);
final public void AddOrRemoveSelected(TopoDS_Shape aShape,boolean updateviewer) {
    AIS_InteractiveContext_AddOrRemoveSelected_2(aShape,updateviewer);
}

private final native void AIS_InteractiveContext_AddOrRemoveSelected_2(TopoDS_Shape aShape,boolean updateviewer);

final public void AddOrRemoveSelected(SelectMgr_EntityOwner anOwner,boolean updateviewer) {
    AIS_InteractiveContext_AddOrRemoveSelected_3(anOwner,updateviewer);
}

private final native void AIS_InteractiveContext_AddOrRemoveSelected_3(SelectMgr_EntityOwner anOwner,boolean updateviewer);

native public final boolean IsSelected(AIS_InteractiveObject aniobj);
native public final void InitSelected();
native public final boolean MoreSelected();
native public final void NextSelected();
native public final int NbSelected();
native public final boolean HasSelectedShape();
native public final TopoDS_Shape SelectedShape();
native public final SelectMgr_EntityOwner SelectedOwner();
native public final AIS_InteractiveObject Interactive();
native public final AIS_InteractiveObject SelectedInteractive();
native public final boolean HasApplicative();
native public final Standard_Transient Applicative();
native public final boolean HasDetected();
native public final boolean HasDetectedShape();
native public final TopoDS_Shape DetectedShape();
native public final AIS_InteractiveObject DetectedInteractive();
native public final SelectMgr_EntityOwner DetectedOwner();
native public final void InitDetected();
native public final boolean MoreDetected();
native public final void NextDetected();
native public final TopoDS_Shape DetectedCurrentShape();
native public final AIS_InteractiveObject DetectedCurrentObject();
native public final int OpenLocalContext(boolean UseDisplayedObjects,boolean AllowShapeDecomposition,boolean AcceptEraseOfObjects,boolean BothViewers);
native public final void CloseLocalContext(int Index,boolean updateviewer);
native public final int IndexOfCurrentLocal();
native public final void CloseAllContexts(boolean updateviewer);
native public final void ResetOriginalState(boolean updateviewer);
native public final void ClearLocalContext(short TheMode);
native public final void UseDisplayedObjects();
native public final void NotUseDisplayedObjects();
native public final boolean BeginImmediateDraw();
native public final boolean ImmediateAdd(AIS_InteractiveObject anIObj,int aMode);
native public final boolean ImmediateRemove(AIS_InteractiveObject anIObj,int aMode);
final public boolean EndImmediateDraw(V3d_View aView,boolean DoubleBuf) {
   return AIS_InteractiveContext_EndImmediateDraw_1(aView,DoubleBuf);
}

private final native boolean AIS_InteractiveContext_EndImmediateDraw_1(V3d_View aView,boolean DoubleBuf);

final public boolean EndImmediateDraw(boolean DoubleBuf) {
   return AIS_InteractiveContext_EndImmediateDraw_2(DoubleBuf);
}

private final native boolean AIS_InteractiveContext_EndImmediateDraw_2(boolean DoubleBuf);

native public final boolean IsImmediateModeOn();
native public final void Drag(V3d_View aView,AIS_InteractiveObject anObject,Geom_Transformation aTranformation,boolean postConcatenate,boolean update,boolean zBuffer);
native public final void SetAutomaticHilight(boolean aStatus);
native public final boolean AutomaticHilight();
native public final void Activate(AIS_InteractiveObject anIobj,int aMode);
final public void Deactivate(AIS_InteractiveObject anIObj) {
    AIS_InteractiveContext_Deactivate_1(anIObj);
}

private final native void AIS_InteractiveContext_Deactivate_1(AIS_InteractiveObject anIObj);

final public void Deactivate(AIS_InteractiveObject anIobj,int aMode) {
    AIS_InteractiveContext_Deactivate_2(anIobj,aMode);
}

private final native void AIS_InteractiveContext_Deactivate_2(AIS_InteractiveObject anIobj,int aMode);

native public final void ActivatedModes(AIS_InteractiveObject anIobj,TColStd_ListOfInteger theList);
native public final void SetShapeDecomposition(AIS_InteractiveObject anIobj,boolean aStatus);
native public final void SetTemporaryAttributes(AIS_InteractiveObject anObj,Prs3d_Drawer aDrawer,boolean updateviewer);
final public void SubIntensityOn(AIS_InteractiveObject aniobj,boolean updateviewer) {
    AIS_InteractiveContext_SubIntensityOn_1(aniobj,updateviewer);
}

private final native void AIS_InteractiveContext_SubIntensityOn_1(AIS_InteractiveObject aniobj,boolean updateviewer);

final public void SubIntensityOff(AIS_InteractiveObject aniobj,boolean updateviewer) {
    AIS_InteractiveContext_SubIntensityOff_1(aniobj,updateviewer);
}

private final native void AIS_InteractiveContext_SubIntensityOff_1(AIS_InteractiveObject aniobj,boolean updateviewer);

final public void SubIntensityOn(boolean updateviewer) {
    AIS_InteractiveContext_SubIntensityOn_2(updateviewer);
}

private final native void AIS_InteractiveContext_SubIntensityOn_2(boolean updateviewer);

final public void SubIntensityOff(boolean updateviewer) {
    AIS_InteractiveContext_SubIntensityOff_2(updateviewer);
}

private final native void AIS_InteractiveContext_SubIntensityOff_2(boolean updateviewer);

native public final void AddFilter(SelectMgr_Filter aFilter);
native public final void RemoveFilter(SelectMgr_Filter aFilter);
native public final void RemoveFilters();
native public final void ActivateStandardMode(short aStandardActivation);
native public final void DeactivateStandardMode(short aStandardActivation);
native public final TColStd_ListOfInteger ActivatedStandardModes();
native public final SelectMgr_ListOfFilter Filters();
native public final Prs3d_Drawer DefaultDrawer();
native public final V3d_Viewer CurrentViewer();
final public void DisplayedObjects(AIS_ListOfInteractive aListOfIO,boolean OnlyFromNeutral) {
    AIS_InteractiveContext_DisplayedObjects_1(aListOfIO,OnlyFromNeutral);
}

private final native void AIS_InteractiveContext_DisplayedObjects_1(AIS_ListOfInteractive aListOfIO,boolean OnlyFromNeutral);

final public void DisplayedObjects(short WhichKind,int WhichSignature,AIS_ListOfInteractive aListOfIO,boolean OnlyFromNeutral) {
    AIS_InteractiveContext_DisplayedObjects_2(WhichKind,WhichSignature,aListOfIO,OnlyFromNeutral);
}

private final native void AIS_InteractiveContext_DisplayedObjects_2(short WhichKind,int WhichSignature,AIS_ListOfInteractive aListOfIO,boolean OnlyFromNeutral);

native public final V3d_Viewer Collector();
final public void ObjectsInCollector(AIS_ListOfInteractive aListOfIO) {
    AIS_InteractiveContext_ObjectsInCollector_1(aListOfIO);
}

private final native void AIS_InteractiveContext_ObjectsInCollector_1(AIS_ListOfInteractive aListOfIO);

final public void ObjectsInCollector(short WhichKind,int WhichSignature,AIS_ListOfInteractive aListOfIO) {
    AIS_InteractiveContext_ObjectsInCollector_2(WhichKind,WhichSignature,aListOfIO);
}

private final native void AIS_InteractiveContext_ObjectsInCollector_2(short WhichKind,int WhichSignature,AIS_ListOfInteractive aListOfIO);

native public final void ObjectsInside(AIS_ListOfInteractive aListOfIO,short WhichKind,int WhichSignature);
native public final boolean HasOpenedContext();
native public final TCollection_AsciiString CurrentName();
native public final TCollection_AsciiString SelectionName();
native public final Standard_CString DomainOfMainViewer();
native public final Standard_CString DomainOfCollector();
native public final SelectMgr_SelectionManager SelectionManager();
native public final PrsMgr_PresentationManager3d MainPrsMgr();
native public final PrsMgr_PresentationManager3d CollectorPrsMgr();
native public final StdSelect_ViewerSelector3d MainSelector();
native public final StdSelect_ViewerSelector3d CollectorSelector();
native public final int PurgeDisplay(boolean CollectorToo);
native public final int HighestIndex();
final public void DisplayActiveAreas(V3d_View aView) {
    AIS_InteractiveContext_DisplayActiveAreas_1(aView);
}

private final native void AIS_InteractiveContext_DisplayActiveAreas_1(V3d_View aView);

native public final void ClearActiveAreas(V3d_View aView);
final public void DisplayActiveSensitive(V3d_View aView) {
    AIS_InteractiveContext_DisplayActiveSensitive_1(aView);
}

private final native void AIS_InteractiveContext_DisplayActiveSensitive_1(V3d_View aView);

native public final void ClearActiveSensitive(V3d_View aView);
final public void DisplayActiveSensitive(AIS_InteractiveObject anObject,V3d_View aView) {
    AIS_InteractiveContext_DisplayActiveSensitive_2(anObject,aView);
}

private final native void AIS_InteractiveContext_DisplayActiveSensitive_2(AIS_InteractiveObject anObject,V3d_View aView);

final public void DisplayActiveAreas(AIS_InteractiveObject anObject,V3d_View aView) {
    AIS_InteractiveContext_DisplayActiveAreas_2(anObject,aView);
}

private final native void AIS_InteractiveContext_DisplayActiveAreas_2(AIS_InteractiveObject anObject,V3d_View aView);

native public final boolean IsInLocal(AIS_InteractiveObject anObject,Standard_Integer TheIndex);
public AIS_InteractiveContext() {
}




}
