// Container-aware signal handling for OCCT
// This provides a more sophisticated approach that preserves container compatibility

#include <OSD.hxx>
#include <signal.h>
#include <sys/prctl.h>
#include <unistd.h>

class ContainerSafeOCCT 
{
private:
    static bool m_isContainerEnvironment;
    static struct sigaction m_originalHandlers[8];
    static const int m_monitoredSignals[8];
    
public:
    // Detect if running in a container environment
    static bool IsContainerEnvironment()
    {
        // Check for common container indicators
        
        // 1. Check for Docker environment variables
        if (getenv("DOCKER_CONTAINER") || getenv("KUBERNETES_SERVICE_HOST"))
            return true;
            
        // 2. Check if we're PID 1 (common in containers)
        if (getpid() == 1)
            return true;
            
        // 3. Check cgroup information
        FILE* cgroup = fopen("/proc/1/cgroup", "r");
        if (cgroup) {
            char line[256];
            while (fgets(line, sizeof(line), cgroup)) {
                if (strstr(line, "docker") || strstr(line, "kubepods")) {
                    fclose(cgroup);
                    return true;
                }
            }
            fclose(cgroup);
        }
        
        // 4. Check for Azure Container Instances
        if (getenv("ACI_RESOURCE_GROUP") || getenv("AZURE_CLIENT_ID"))
            return true;
            
        return false;
    }
    
    // Safe initialization for container environments
    static void InitializeForContainer()
    {
        m_isContainerEnvironment = IsContainerEnvironment();
        
        if (m_isContainerEnvironment) {
            // In container environment - preserve existing handlers
            StoreOriginalHandlers();
            
            // Only handle unhandled signals, preserve container infrastructure
            OSD::SetSignal(OSD_SignalMode_SetUnhandled, Standard_False);
            
            // Disable floating point exceptions in containers
            OSD::SetFloatingSignal(Standard_False);
            
            // Register cleanup on exit
            atexit(RestoreOriginalHandlers);
        } else {
            // Normal environment - use standard OCCT signal handling
            OSD::SetSignal(OSD_SignalMode_Set, Standard_True);
        }
    }
    
    // Store original signal handlers before OCCT modifies them
    static void StoreOriginalHandlers()
    {
        for (int i = 0; i < 8; ++i) {
            sigaction(m_monitoredSignals[i], nullptr, &m_originalHandlers[i]);
        }
    }
    
    // Restore original handlers on cleanup
    static void RestoreOriginalHandlers()
    {
        if (m_isContainerEnvironment) {
            for (int i = 0; i < 8; ++i) {
                sigaction(m_monitoredSignals[i], &m_originalHandlers[i], nullptr);
            }
        }
    }
    
    // Thread-safe initialization
    static void InitializeThreadForContainer()
    {
        if (m_isContainerEnvironment) {
            // Don't override container thread signal handling
            OSD::SetThreadLocalSignal(OSD_SignalMode_AsIs, Standard_False);
        } else {
            // Normal thread initialization
            OSD::SetThreadLocalSignal(OSD_SignalMode_Set, Standard_True);
        }
    }
    
    // Check if we should catch OCCT signals in try-catch blocks
    static bool ShouldCatchOCCTSignals()
    {
        return !m_isContainerEnvironment;
    }
};

// Static member definitions
bool ContainerSafeOCCT::m_isContainerEnvironment = false;
struct sigaction ContainerSafeOCCT::m_originalHandlers[8];
const int ContainerSafeOCCT::m_monitoredSignals[8] = {
    SIGFPE, SIGHUP, SIGINT, SIGQUIT, SIGILL, SIGBUS, SIGSYS, SIGSEGV
};

// Usage example
int main()
{
    // Initialize OCCT with container awareness
    ContainerSafeOCCT::InitializeForContainer();
    
    // Your OCCT code with conditional signal catching
    if (ContainerSafeOCCT::ShouldCatchOCCTSignals()) {
        try {
            // Use OCC_CATCH_SIGNALS macro if available and not in container
            #ifdef OCC_CONVERT_SIGNALS
                OCC_CATCH_SIGNALS
            #endif
            
            // Your OCCT operations here
            
        } catch (const Standard_Failure& e) {
            // Handle OCCT exceptions
            std::cerr << "OCCT Exception: " << e.GetMessageString() << std::endl;
        }
    } else {
        // In container - don't use signal catching
        // Your OCCT operations here
    }
    
    return 0;
}
