// Patch for OSD_signal.cxx to make it container-aware
// This shows how to modify OCCT's signal handling to be container-safe

// Add this to the beginning of OSD_signal.cxx after the includes:

#ifdef __linux__
#include <unistd.h>
#include <cstdlib>

// Function to detect container environment
static Standard_Boolean IsContainerEnvironment()
{
    // Check for container indicators
    if (getenv("DOCKER_CONTAINER") || 
        getenv("KUBERNETES_SERVICE_HOST") ||
        getenv("ACI_RESOURCE_GROUP") ||
        getenv("OCCT_DISABLE_SIGNALS"))
        return Standard_True;
        
    // Check if running as PID 1 (common in containers)
    if (getpid() == 1)
        return Standard_True;
        
    // Check cgroup for container indicators
    FILE* cgroup = fopen("/proc/1/cgroup", "r");
    if (cgroup) {
        char line[256];
        while (fgets(line, sizeof(line), cgroup)) {
            if (strstr(line, "docker") || strstr(line, "kubepods")) {
                fclose(cgroup);
                return Standard_True;
            }
        }
        fclose(cgroup);
    }
    
    return Standard_False;
}
#endif

// Modify the SetSignal function to check for container environment:

void OSD::SetSignal(OSD_SignalMode theSignalMode, Standard_Boolean theFloatingSignal)
{
#ifdef __linux__
    // Check if we're in a container environment
    static Standard_Boolean isContainer = IsContainerEnvironment();
    if (isContainer && theSignalMode != OSD_SignalMode_AsIs) {
        // In container environment, be more conservative
        if (theSignalMode == OSD_SignalMode_Set) {
            // Change to SetUnhandled to preserve container signal handling
            theSignalMode = OSD_SignalMode_SetUnhandled;
        }
        // Always disable floating point signals in containers
        theFloatingSignal = Standard_False;
        
        #ifdef OCCT_DEBUG
        std::cout << "OCCT: Container environment detected, using conservative signal handling" << std::endl;
        #endif
    }
#endif

    SetFloatingSignal(theFloatingSignal);
    
    OSD_WasSetSignal = theSignalMode;
    if (theSignalMode == OSD_SignalMode_AsIs)
    {
        return; // nothing to be done with signal handlers
    }

    // Rest of the existing SetSignal implementation...
    // (Keep the existing code unchanged)
}

// Also modify the Handler function to be more container-friendly:

#ifdef SA_SIGINFO
static void Handler(const int theSignal,
                    siginfo_t* /*theSigInfo*/,
                    const Standard_Address /*theContext*/)
#else
static void Handler(const int theSignal)
#endif
{
#ifdef __linux__
    // In container environments, be less aggressive with signal handling
    static Standard_Boolean isContainer = IsContainerEnvironment();
    if (isContainer) {
        switch (theSignal) {
            case SIGINT:
                // In containers, SIGINT should be handled gracefully
                fCtrlBrk = Standard_True;
                return; // Don't call exit() in container
            case SIGTERM:
                // SIGTERM is used by container orchestrators
                fCtrlBrk = Standard_True;
                return; // Let the container handle it
            default:
                break;
        }
    }
#endif

    // Existing handler code with modifications for container safety...
    struct sigaction oldact, act;
    // re-install the signal
    if (!sigaction(theSignal, NULL, &oldact))
    {
        if (sigaction(theSignal, &oldact, &act))
            perror("sigaction");
    }
    else
    {
        perror("sigaction");
    }

    if (ADR_ACT_SIGIO_HANDLER != NULL)
        (*ADR_ACT_SIGIO_HANDLER)();

    sigset_t set;
    sigemptyset(&set);
    switch (theSignal)
    {
        case SIGHUP:
            OSD_SIGHUP::NewInstance("SIGHUP 'hangup' detected.")->Jump();
#ifndef __linux__
            exit(SIGHUP);
#endif
            break;
        case SIGINT:
            fCtrlBrk = Standard_True;
#ifdef __linux__
            // In Linux containers, don't exit on SIGINT
            static Standard_Boolean isContainer = IsContainerEnvironment();
            if (!isContainer)
#endif
            {
                // OSD_SIGINT::NewInstance("SIGINT 'interrupt' detected.")->Jump();
                // exit(SIGINT);
            }
            break;
        // ... rest of the cases with similar container-aware modifications
    }
}
