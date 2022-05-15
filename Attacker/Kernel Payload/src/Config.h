#define PS3_HEN
#ifdef PS3_HEN
    #define SYS_CALLS_HOOK
#else
    #define HAS_DEBUG_PRINTS
    #define SYS_CALLS_HOOK
#endif