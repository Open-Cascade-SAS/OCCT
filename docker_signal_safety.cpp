// Docker-safe OCCT initialization for Linux containers
// This example shows how to properly initialize OCCT in container environments

#include <OSD.hxx>
#include <Standard_ErrorHandler.hxx>

// Container-safe OCCT initialization
void InitializeOCCTForContainer()
{
    // Method 1: Completely disable signal handling
    // This is the safest approach for Docker containers
    OSD::SetSignal(OSD_SignalMode_AsIs, Standard_False);
    
    // Alternative Method 2: Only set unhandled signals
    // This preserves existing container signal handlers
    // OSD::SetSignal(OSD_SignalMode_SetUnhandled, Standard_False);
    
    // Method 3: Explicitly unset all signal handlers
    // Use this to restore default system behavior
    // OSD::SetSignal(OSD_SignalMode_Unset, Standard_False);
}

// Thread-safe initialization for multi-threaded container applications
void InitializeOCCTThreadForContainer()
{
    // For new threads, ensure they don't override container signal handlers
    OSD::SetThreadLocalSignal(OSD_SignalMode_AsIs, Standard_False);
}

// Example usage in main function
int main()
{
    // Initialize OCCT with container-safe settings
    InitializeOCCTForContainer();
    
    // Your OCCT application code here...
    
    return 0;
}
