using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Interop;
using System.Runtime.InteropServices;

namespace IE_WPF_D3D
{
  /// <summary>
  /// Tool object for output OCCT rendering with Direct3D.
  /// </summary>
  class D3DViewer
  {
    /// <summary> Direct3D renderer. </summary>
    private IntPtr myD3DRender;

    /// <summary> Direct3D output image. </summary>
    private D3DImage myD3DImage = new D3DImage ();

    /// <summary> Direct3D color surface. </summary>
    private IntPtr myColorSurf;

    public OCCViewer Viewer;

    /// <summary> Creates new Direct3D-based OCCT viewer. </summary>
    public D3DViewer ()
    {
      myD3DImage.IsFrontBufferAvailableChanged
        += new DependencyPropertyChangedEventHandler (OnIsFrontBufferAvailableChanged);

      BeginRenderingScene ();
    }

    /// <summary> Creates new Direct3D-based OCCT viewer. </summary>
    private void OnIsFrontBufferAvailableChanged (object sender, DependencyPropertyChangedEventArgs e)
    {
      // If the front buffer is available, then WPF has just created a new
      // Direct3D device, thus we need to start rendering our custom scene
      if (myD3DImage.IsFrontBufferAvailable)
      {
        BeginRenderingScene ();
      }
      else
      {
        // If the front buffer is no longer available, then WPF has lost Direct3D
        // device, thus we need to stop rendering until the new device is created
        StopRenderingScene ();
      }
    }

    private bool myIsFailed = false;

    /// <summary> Initializes Direct3D-OCCT rendering. </summary>
    private void BeginRenderingScene ()
    {
      if (myIsFailed)
      {
        return;
      }

      if (myD3DImage.IsFrontBufferAvailable)
      {
        IntPtr aWinHandle;
        IntPtr aD3DDevice;

        // Initialize Direct3D device and render target
        myD3DRender = Direct3DProxy.InitRender(out aWinHandle, out aD3DDevice);

        Viewer = new OCCViewer();

        if (!Viewer.InitInterop (aWinHandle, aD3DDevice))
        {
          MessageBox.Show ("Failed to initialize OpenGL-Direct3D interoperability!",
            "Error", MessageBoxButton.OK, MessageBoxImage.Error);

          myIsFailed = true;

          if (myD3DRender != IntPtr.Zero)
              Direct3DProxy.ReleaseRender(ref myD3DRender);

          return;
        }

        // Leverage the Rendering event of WPF composition
        // target to update the our custom Direct3D scene
        CompositionTarget.Rendering += OnRendering;
      }
    }

    /// <summary> Initializes Direct3D-OCCT rendering. </summary>
    public void StopRenderingScene ()
    {
      // This method is called when WPF loses its Direct3D device,
      // so we should just release our custom Direct3D scene
      CompositionTarget.Rendering -= OnRendering;

      if (myD3DRender != IntPtr.Zero)
          Direct3DProxy.ReleaseRender(ref myD3DRender);

      myColorSurf = IntPtr.Zero;
    }

    /// <summary> Initializes Direct3D-OCCT rendering. </summary>
    private void OnRendering (object sender, EventArgs e)
    {
      UpdateScene ();
    }

    /// <summary> Current size of rendering window. </summary>
    private WndSize mySize = new WndSize(1, 1);

    /// <summary> Initializes Direct3D-OCCT rendering. </summary>
    private void UpdateScene ()
    {
      if (!myIsFailed && myD3DImage.IsFrontBufferAvailable && myColorSurf != IntPtr.Zero)
      {
        myD3DImage.Lock ();
        {
          // Update the scene (via a call into our custom library)
          Viewer.View.RedrawView ();

          // Invalidate the updated region of the D3DImage
          myD3DImage.AddDirtyRect(new Int32Rect(0, 0, mySize.cx, mySize.cy));
        }
        myD3DImage.Unlock ();
      }
    }

    /// <summary> Resizes Direct3D surfaces and OpenGL FBO. </summary>
    public void Resize (int theSizeX, int theSizeY)
    {
        mySize = new WndSize(theSizeX, theSizeY);

      if (!myIsFailed && myD3DImage.IsFrontBufferAvailable)
      {
        IntPtr aColorSurfShare;

        // Initialize Direct3D device and render target
        Direct3DProxy.ResizeWindow(ref myD3DRender, ref mySize, out myColorSurf, out aColorSurfShare);

        // Set the back buffer for Direct3D WPF image
        myD3DImage.Lock ();
        {
          myD3DImage.SetBackBuffer (D3DResourceType.IDirect3DSurface9, myColorSurf);
        }
        myD3DImage.Unlock ();

        Viewer.View.ResizeBridgeFBO(mySize.cx, mySize.cy, myColorSurf, aColorSurfShare);
      }
    }

    public D3DImage Image
    {
      get
      {
        return myD3DImage;
      }
    }
  }
}
