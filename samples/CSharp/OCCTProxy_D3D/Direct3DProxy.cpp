#include <iostream>
#include <windows.h>
#include <d3dx9.h>

using namespace System::Runtime::InteropServices;

typedef HRESULT (WINAPI *DIRECT3DCREATE9EX)(UINT SDKVersion, IDirect3D9Ex**);

LRESULT WINAPI MsgProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  return DefWindowProcW (hWnd, msg, wParam, lParam);
}

WNDCLASSEXW THE_WNDCLASS_D3D =
{
  sizeof (WNDCLASSEXW),
  CS_CLASSDC, MsgProc, NULL, NULL,
  GetModuleHandle (NULL),
  NULL, NULL, NULL, NULL, L"D3D", NULL
};

// =======================================================================
// function : GetVertexProcessingCaps
// purpose  :
// =======================================================================
DWORD GetVertexProcessingCaps (LPDIRECT3D9 theD3D)
{
  D3DCAPS9 aCaps;
  return !SUCCEEDED (theD3D->GetDeviceCaps (D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &aCaps))
      || !(aCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
       ? D3DCREATE_SOFTWARE_VERTEXPROCESSING
       : D3DCREATE_HARDWARE_VERTEXPROCESSING;
}

//! Encapsulates state of Direct3D renderer.
class D3DRender
{
public:

  D3DRender (int theSizeX = 512,
             int theSizeY = 512)
  : D3D (NULL),
    D3DEx (NULL),
    D3DDevice (NULL),
    D3DDeviceEx (NULL),
    D3DColorSurf (NULL),
    D3DColorSurfShare (NULL),
    FuncCreate9Ex (NULL),
    WinSizeX (theSizeX),
    WinSizeY (theSizeY)
  {
    CheckRegisterClass();
    WinHandle = CreateWindowW (L"D3D", L"D3D",
                               WS_OVERLAPPEDWINDOW, 0, 0, 1, 1,
                               NULL, NULL, THE_WNDCLASS_D3D.hInstance, NULL);
    Init();
  }

  ~D3DRender()
  {
    // do not release class instance shared between all renderers
    //UnregisterClass (NULL, THE_WNDCLASS_D3D.hInstance);
    if (D3DDevice != NULL)
    {
      D3DDevice->Release();
    }
    if (D3DDeviceEx != NULL)
    {
      D3DDeviceEx->Release();
    }
    if (D3D != NULL)
    {
      D3D->Release();
    }
    if (D3DEx != NULL)
    {
      D3DEx->Release();
    }
    if (D3DColorSurf != NULL)
    {
      D3DColorSurf->Release();
      D3DColorSurfShare = NULL;
    }
  }

  //! Creates Direct3D render target.
  bool CreateRenderTarget()
  {
    if (!SetWindowPos (WinHandle, 0, 0, 0, WinSizeX, WinSizeY, 0))
    {
      return false;
    }

    if (D3DColorSurf != NULL)
    {
      D3DColorSurf->Release();
      D3DColorSurfShare = NULL;
    }

    // Note: Render target surface should be lockable on
    // Windows XP and non-lockable on Windows Vista or higher
    if (FAILED (D3DDevice->CreateRenderTarget (WinSizeX, WinSizeY,
                                               D3DFMT_X8R8G8B8, D3DMULTISAMPLE_NONE, 0, FuncCreate9Ex != NULL,
                                               &D3DColorSurf, FuncCreate9Ex == NULL ? NULL : &D3DColorSurfShare)))
    {
      return false;
    }

    D3DDevice->SetRenderTarget (0, D3DColorSurf);
    return true;
  }

private:

  void Init()
  {
    // Vista requires the D3D "Ex" functions for optimal performance.
    // The Ex functions are only supported with WDDM drivers, so they will not be available on XP.
    HMODULE aD3D9 = GetModuleHandleW (L"d3d9");
    FuncCreate9Ex = (DIRECT3DCREATE9EX )GetProcAddress (aD3D9, "Direct3DCreate9Ex");
    
    // Set up the structure used to create the D3DDevice
    D3DPRESENT_PARAMETERS aParams;
    ZeroMemory (&aParams, sizeof(aParams));
    aParams.Windowed         = TRUE;
    aParams.BackBufferHeight = 1;
    aParams.BackBufferWidth  = 1;
    aParams.SwapEffect       = D3DSWAPEFFECT_DISCARD;
    aParams.BackBufferFormat = D3DFMT_X8R8G8B8;
    (FuncCreate9Ex != NULL) ? InitializeD3DEx (aParams) : InitializeD3D (aParams);
  }
  
  bool InitializeD3D (D3DPRESENT_PARAMETERS theParams)
  {
    D3D = Direct3DCreate9 (D3D_SDK_VERSION);
    if (D3D == NULL)
      return false;

    DWORD aVertProcessCaps = GetVertexProcessingCaps (D3D);
    HRESULT aResult = D3D->CreateDevice (D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, WinHandle,
                                         aVertProcessCaps | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE,
                                         &theParams, &D3DDevice);
    return SUCCEEDED (aResult);
  }
  
  bool InitializeD3DEx (D3DPRESENT_PARAMETERS theParams)
  {
    if (FAILED (FuncCreate9Ex (D3D_SDK_VERSION, &D3DEx))
     || FAILED (D3DEx->QueryInterface (__uuidof (IDirect3D9), reinterpret_cast<void**> (&D3D))))
    {
      return false;
    }

    DWORD aVertProcessCaps = GetVertexProcessingCaps (D3D);
    if (FAILED (D3DEx->CreateDeviceEx (D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, WinHandle,
                                       aVertProcessCaps | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE,
                                       &theParams, NULL, &D3DDeviceEx)))
      return false;

    return SUCCEEDED (D3DDeviceEx->QueryInterface (__uuidof (IDirect3DDevice9), reinterpret_cast<void**> (&D3DDevice)));
  }

  static void CheckRegisterClass()
  {
    static bool isRegistered = false;
    if (!isRegistered)
    {
      RegisterClassExW (&THE_WNDCLASS_D3D);
      isRegistered = true;
    }
  }

public:

  LPDIRECT3D9         D3D;
  LPDIRECT3D9EX       D3DEx;
  LPDIRECT3DDEVICE9   D3DDevice;
  LPDIRECT3DDEVICE9EX D3DDeviceEx;

  LPDIRECT3DSURFACE9  D3DColorSurf;
  HANDLE              D3DColorSurfShare;

  DIRECT3DCREATE9EX   FuncCreate9Ex;

  HWND                WinHandle;
  INT                 WinSizeX;
  INT                 WinSizeY;

};

public ref struct WndSize
{
public:

  WndSize(int theSizeX, int theSizeY)
  {
    mySize = new SIZE();
    mySize->cx = theSizeX;
    mySize->cy = theSizeY;
  }

  ~WndSize()
  {
    delete mySize;
  }

  property int cx
  {
    int get() { return mySize->cx; }
  }

  property int cy
  {
    int get() { return mySize->cy; }
  }

  LPSIZE GetPointer()
  {
    return mySize;
  }

private:

  LPSIZE mySize;
};

public ref class Direct3DProxy
{
public :

  Direct3DProxy()
  {
    //
  }

  // =======================================================================
  // function : InitializeScene
  // purpose  :
  // =======================================================================
  static System::IntPtr InitRender ([Out] System::IntPtr% theWinHandle,
                                    [Out] System::IntPtr% theD3DDevice)
  {
    D3DRender* aRender = new D3DRender();
    theWinHandle = System::IntPtr(aRender->WinHandle);
    theD3DDevice = System::IntPtr(aRender->D3DDevice);

    return System::IntPtr(aRender);
  }

  // =======================================================================
  // function : ResizeWindow
  // purpose  :
  // =======================================================================
  static void ResizeWindow ([In]  System::IntPtr% theRender,
                            [In]  WndSize^%       theWndSize,
                            [Out] System::IntPtr% theColorSurf,
                            [Out] System::IntPtr% theColorSurfShare)
  {
    D3DRender* aRender = reinterpret_cast<D3DRender*> (theRender.ToPointer());
    LPSIZE aSize = theWndSize->GetPointer();
    aRender->WinSizeX = aSize->cx;
    aRender->WinSizeY = aSize->cy;
    aRender->CreateRenderTarget();

    theColorSurf      = System::IntPtr(aRender->D3DColorSurf);
    theColorSurfShare = System::IntPtr(aRender->D3DColorSurfShare);
  }

  // =======================================================================
  // function : ReleaseRender
  // purpose  :
  // =======================================================================
  static void ReleaseRender ([In] System::IntPtr% theRender)
  {
    D3DRender* aRender = reinterpret_cast<D3DRender*> (theRender.ToPointer());
    delete aRender;
  }
};