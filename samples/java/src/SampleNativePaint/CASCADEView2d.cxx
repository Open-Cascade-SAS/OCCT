// File:	CASCADEView2d.cxx
// Created:	Wed Jul 18 11:11:22 2001
// Author:	
//		<avo@TIREX>

#ifdef WNT
# include <windows.h>
#endif

#include <Standard_Stream.hxx>
#include <jawt_md.h>
#include <CASCADEView2d.hxx>
#include <jcas.hxx>
#include <V2d_View.hxx>
#include <V2d_Viewer.hxx>

#ifdef WNT
# include <WNT_Window.hxx>
#include <WNT_WDriver.hxx>
// # include <Graphic2d_WNTGraphicDevice.hxx>
# include <OpenGl_GraphicDriver.hxx>
#else
# include <Xw_Window.hxx>
# include <Xw_Driver.hxx>
//  # include <Graphic2d_GraphicDevice.hxx>
#endif  // WNT

/*
 * Class:     CASCADEView2d
 * Method:    paint
 * Signature: (Ljava/awt/Graphics;)V
 */
JNIEXPORT void JNICALL Java_CASCADEView2d_paint(JNIEnv *env, jobject theCanvas, jobject theGraphics) {
//   cout << "Info: -------------------------- Java_CASCADEView2d_paint ("<< theCanvas <<") ----------------------" << endl;

  jclass jViewPort2dClass = env->FindClass("CASCADEView2d");
  if(!jViewPort2dClass){ jcas_ThrowException(env, "cant find canvas class"); return; }
  jfieldID jIsWindowID = env->GetFieldID(jViewPort2dClass, "hasWindow", "Z");

  if(!jIsWindowID){ jcas_ThrowException(env, "cant find the boolean field id"); return; }
  jboolean jIsWindow = env->GetBooleanField(theCanvas, jIsWindowID);

  Handle(V2d_View) theView;
  Handle(V2d_Viewer) theViewer;


  if(!jIsWindow){
    // get the window view and viewer
//     cout << "Info: First time, init VIEW." << endl;
    jfieldID jViewerID = env->GetFieldID(jViewPort2dClass, "myViewer", "LCASCADESamplesJni/V2d_Viewer;");
    if(!jViewerID) { jcas_ThrowException(env, "cant find the view field id");return; }
    jobject jViewer = env->GetObjectField(theCanvas, jViewerID);

    if(!jViewer) { // viewer and view are not created, create them now.
      jcas_ThrowException(env, "No jViewer in first paint\n");
      return;
    }else {
      void *ptrViewer = jcas_GetHandle(env, jViewer);
      if (ptrViewer != NULL) {
	theViewer = *((Handle(V2d_Viewer)*)ptrViewer);
      }else{
	cout << "Error: cant get V2d_Viewer from canvas field" << endl;
      }
    }

// start paste
    if (!theViewer.IsNull()){
	  // get drawing surface
	  //-----------------------------------------------

	  JAWT awt;
	  JAWT_DrawingSurface* ds;
	  JAWT_DrawingSurfaceInfo* dsi;
#ifdef WNT
	  JAWT_Win32DrawingSurfaceInfo* dsi_win;
#else
	  JAWT_X11DrawingSurfaceInfo* dsi_x11;
#endif //WNT
	  jboolean result;
	  jint lock;

	  // Get the AWT
	  awt.version = JAWT_VERSION_1_3;
	  result = JAWT_GetAWT(env, &awt);
	  if (result == JNI_FALSE)
	    {
	      cout << "AWT not found" << endl;
	      jcas_ThrowException(env, "AWT not found\n");
//	      alock.Release();
	      return;
	    }

	  // Get the drawing surface
	  ds = awt.GetDrawingSurface(env, theCanvas);
	  if (ds == NULL)
	    {
	      cout << "NULL drawing surface" << endl;
	      jcas_ThrowException(env, "NULL drawing surface\n");
	      return;
	    }

	  // Lock the drawing surface
	  lock = ds->Lock(ds);
	  if ((lock & JAWT_LOCK_ERROR) != 0)
	    {
	      cout << "Error locking surface" << endl;
	      jcas_ThrowException(env, "Error locking surface\n");
	      awt.FreeDrawingSurface(ds);
	      return;
	    }

	  // Get the drawing surface info
	  dsi = ds->GetDrawingSurfaceInfo(ds);
	  if (dsi == NULL)
	    {
	      cout << "Error getting surface info" << endl;
	      jcas_ThrowException(env, "Error getting surface info\n");
	      ds->Unlock(ds);
	      awt.FreeDrawingSurface(ds);
//	      alock.Release();
	      return;
	    }

	  // Get the platform-specific drawing info
	  Aspect_Handle theWindow;
#ifdef WNT
	  dsi_win = (JAWT_Win32DrawingSurfaceInfo*)dsi->platformInfo;
	  theWindow = dsi_win->hwnd;
#else
	  dsi_x11 = (JAWT_X11DrawingSurfaceInfo*)dsi->platformInfo;
	  theWindow = dsi_x11->drawable;
#endif

// 	      if (jIsWindow == JNI_FALSE)
// 		{
#ifdef WNT
		  long wd = GetWindowLong (( HWND )theWindow, GWL_USERDATA);
		  long wProc = GetWindowLong (( HWND )theWindow, GWL_WNDPROC);
#endif

	  //  get params to build view
	  jfieldID jBackColorID = env->GetFieldID(jViewPort2dClass, "myBackColor", "S");
	  jshort jBackColor = env->GetShortField(theCanvas, jBackColorID);

#ifdef WNT
  Handle(WNT_Window) w = new WNT_Window(Handle(WNT_GraphicDevice)::DownCast(theViewer->Device()),theWindow, (Quantity_NameOfColor) jBackColor);
  Handle(WNT_WDriver) d = new WNT_WDriver(w);
#else
  Handle(Xw_Window) w = new Xw_Window(Handle(Xw_GraphicDevice)::DownCast(theViewer->Device()),theWindow,Xw_WQ_DRAWINGQUALITY, (Quantity_NameOfColor) jBackColor);
  Handle(Xw_Driver) d = new Xw_Driver(w);
#endif

	  theView = new V2d_View(d, theViewer);
	  theView->Update();

#ifdef WNT
	  long wd1 = SetWindowLong ((HWND) theWindow, GWL_USERDATA, wd);
	  long wProc1 = SetWindowLong ((HWND) theWindow, GWL_WNDPROC, wProc);
#endif

		  cout << "Info: Set hasWindow = TRUE : id == " << jIsWindowID << endl;
		  jIsWindow = JNI_TRUE;
// 		}


	  env->SetBooleanField(theCanvas, jIsWindowID, jIsWindow);


/******************************
fill fields with VIEWER ONLY
*******************************/

	  Handle(V2d_View)* theViewPtr = new Handle(V2d_View);
	  *theViewPtr = theView;
	  jobject aJavaView = jcas_CreateObject(env,"CASCADESamplesJni/V2d_View", theViewPtr);
	  jfieldID jViewID = env->GetFieldID(jViewPort2dClass, "myView", "LCASCADESamplesJni/V2d_View;");
	  env->SetObjectField(theCanvas, jViewID, aJavaView);

// assume that viewer has been already created.
// 	  Handle(V2d_Viewer)* theViewerPtr = new Handle(V2d_Viewer);
// 	  *theViewerPtr = theViewer;
// 	  jfieldID jViewerID = env->GetFieldID(jViewPort2dClass, "myViewer", "LCASCADESamplesJni/V2d_Viewer;");
// 	  jobject aJavaViewer = jcas_CreateObject(env,"CASCADESamplesJni/V2d_Viewer", theViewerPtr);
// 	  env->SetObjectField(theCanvas, jViewerID, aJavaViewer);


	  // Free the AWT
	  //-----------------------------------------------

	  // Free the drawing surface info
	  ds->FreeDrawingSurfaceInfo(dsi);

	  // Unlock the drawing surface
	  ds->Unlock(ds);

	  // Free the drawing surface
	  awt.FreeDrawingSurface(ds);
	}else{
	  cout << "Error: Cant obtain V2d_Viewer" << endl;
	}
/////////////////////////// end paste
//   cout << "Info: Init viewer done" << endl;
} // end !hasWindow. 


// get stored view field


  jfieldID jViewID = env->GetFieldID(jViewPort2dClass, "myView", "LCASCADESamplesJni/V2d_View;");
  jobject jView = env->GetObjectField(theCanvas, jViewID);
  if(!jView){
    jcas_ThrowException(env, "Error getting viewer while has window ?\n");
    return;
  }

  void *ptrView = jcas_GetHandle(env, jView);
  if (ptrView != NULL)
    theView = *((Handle(V2d_View)*)ptrView);

// paint routine.
  if(theView.IsNull()){
    jcas_ThrowException(env, "Error getting V2d_View ?\n");
    return;
  }


	  JAWT awt;
	  JAWT_DrawingSurface* ds;
	  jboolean result;
	  jint lock;

	  // Get the AWT
	  awt.version = JAWT_VERSION_1_3;
	  result = JAWT_GetAWT(env, &awt);
	  if (result == JNI_FALSE)
	    {
	      cout << "AWT not found" << endl;
	      jcas_ThrowException(env, "AWT not found\n");
//	      alock.Release();
	      return;
	    }

	  // Get the drawing surface
	  ds = awt.GetDrawingSurface(env, theCanvas);
	  if (ds == NULL)
	    {
	      cout << "NULL drawing surface" << endl;
	      jcas_ThrowException(env, "NULL drawing surface\n");
//	      alock.Release();
	      return;
	    }

	  // Lock the drawing surface
	  lock = ds->Lock(ds);
//	  cout << "Result of locking is " << lock << endl;
	  if ((lock & JAWT_LOCK_ERROR) != 0)
	    {
	      cout << "Error locking surface" << endl;
	      jcas_ThrowException(env, "Error locking surface\n");
	      awt.FreeDrawingSurface(ds);
//	      alock.Release();
	      return;
	    }

	  // Redraw V2d_View after locking AWT drawing surface
//           theView->Redraw(); // 3d


  jfieldID jNeedResizeID = env->GetFieldID(jViewPort2dClass, "needResize", "Z");
  if(!jNeedResizeID){ 
    ds->Unlock(ds);
    awt.FreeDrawingSurface(ds);
    jcas_ThrowException(env, "cant find the needREsize field id"); 
    return; 
  }

  jboolean jNeedResize = env->GetBooleanField(theCanvas, jNeedResizeID);
  if(jNeedResize == JNI_TRUE){  
//     cout << "Info: bef update, need resize." << endl;
    theView->MustBeResized(V2d_TOWRE_ENLARGE_OBJECTS);
    jNeedResize = JNI_FALSE;
    env->SetBooleanField(theCanvas, jNeedResizeID, jNeedResize);
  }
  theView->Update();
  
  ds->Unlock(ds);

	  // Free the drawing surface
  awt.FreeDrawingSurface(ds);
  
}
