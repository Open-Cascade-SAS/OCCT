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

import jcas.Standard_Boolean;
import CASCADESamplesJni.TopLoc_Location;
import CASCADESamplesJni.TopAbs_Orientation;
import jcas.Standard_Short;
import CASCADESamplesJni.TopoDS_TShape;
import CASCADESamplesJni.TopAbs_ShapeEnum;
import jcas.Standard_Integer;


public class TopoDS_Shape extends jcas.Object {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
public TopoDS_Shape() {
   TopoDS_Shape_Create_0();
}

private final native void TopoDS_Shape_Create_0();

native public final boolean IsNull();
native public final void Nullify();
final public TopLoc_Location Location() {
   return TopoDS_Shape_Location_1();
}

private final native TopLoc_Location TopoDS_Shape_Location_1();

final public void Location(TopLoc_Location Loc) {
    TopoDS_Shape_Location_2(Loc);
}

private final native void TopoDS_Shape_Location_2(TopLoc_Location Loc);

native public final TopoDS_Shape Located(TopLoc_Location Loc);
final public short Orientation() {
   return TopoDS_Shape_Orientation_1();
}

private final native short TopoDS_Shape_Orientation_1();

final public void Orientation(short Orient) {
    TopoDS_Shape_Orientation_2(Orient);
}

private final native void TopoDS_Shape_Orientation_2(short Orient);

native public final TopoDS_Shape Oriented(short Or);
final public TopoDS_TShape TShape() {
   return TopoDS_Shape_TShape_1();
}

private final native TopoDS_TShape TopoDS_Shape_TShape_1();

native public final short ShapeType();
final public boolean Free() {
   return TopoDS_Shape_Free_1();
}

private final native boolean TopoDS_Shape_Free_1();

final public void Free(boolean F) {
    TopoDS_Shape_Free_2(F);
}

private final native void TopoDS_Shape_Free_2(boolean F);

final public boolean Modified() {
   return TopoDS_Shape_Modified_1();
}

private final native boolean TopoDS_Shape_Modified_1();

final public void Modified(boolean M) {
    TopoDS_Shape_Modified_2(M);
}

private final native void TopoDS_Shape_Modified_2(boolean M);

final public boolean Checked() {
   return TopoDS_Shape_Checked_1();
}

private final native boolean TopoDS_Shape_Checked_1();

final public void Checked(boolean C) {
    TopoDS_Shape_Checked_2(C);
}

private final native void TopoDS_Shape_Checked_2(boolean C);

final public boolean Orientable() {
   return TopoDS_Shape_Orientable_1();
}

private final native boolean TopoDS_Shape_Orientable_1();

final public void Orientable(boolean C) {
    TopoDS_Shape_Orientable_2(C);
}

private final native void TopoDS_Shape_Orientable_2(boolean C);

final public boolean Closed() {
   return TopoDS_Shape_Closed_1();
}

private final native boolean TopoDS_Shape_Closed_1();

final public void Closed(boolean C) {
    TopoDS_Shape_Closed_2(C);
}

private final native void TopoDS_Shape_Closed_2(boolean C);

final public boolean Infinite() {
   return TopoDS_Shape_Infinite_1();
}

private final native boolean TopoDS_Shape_Infinite_1();

final public void Infinite(boolean C) {
    TopoDS_Shape_Infinite_2(C);
}

private final native void TopoDS_Shape_Infinite_2(boolean C);

final public boolean Convex() {
   return TopoDS_Shape_Convex_1();
}

private final native boolean TopoDS_Shape_Convex_1();

final public void Convex(boolean C) {
    TopoDS_Shape_Convex_2(C);
}

private final native void TopoDS_Shape_Convex_2(boolean C);

native public final void Move(TopLoc_Location position);
native public final TopoDS_Shape Moved(TopLoc_Location position);
native public final void Reverse();
native public final TopoDS_Shape Reversed();
native public final void Complement();
native public final TopoDS_Shape Complemented();
native public final void Compose(short Orient);
native public final TopoDS_Shape Composed(short Orient);
native public final boolean IsPartner(TopoDS_Shape other);
native public final boolean IsSame(TopoDS_Shape other);
native public final boolean IsEqual(TopoDS_Shape other);
native public final boolean IsNotEqual(TopoDS_Shape other);
native public final int HashCode(int Upper);
native public final void EmptyCopy();
native public final TopoDS_Shape EmptyCopied();
final public void TShape(TopoDS_TShape T) {
    TopoDS_Shape_TShape_2(T);
}

private final native void TopoDS_Shape_TShape_2(TopoDS_TShape T);



public native static void FinalizeValue(long anHID);

public void finalize() {
   synchronized(myCasLock) {
     if ( aVirer != 0 ) FinalizeValue(HID);
     HID = 0;
   }
}

}
