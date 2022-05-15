#include "../Types.h"

namespace lv2 {
    typedef int32_t  sys_prx_id_t;
    typedef uint64_t sys_prx_flags_t;

    typedef uint32_t sys_prx_user_pchar_t;
    typedef uint32_t sys_prx_user_segment_vector_t;
    typedef uint32_t sys_prx_user_libent_addr_t;
    typedef uint32_t sys_prx_user_libstub_addr_t;
    typedef uint32_t sys_prx_user_p_prx_id_t;
    typedef uint32_t sys_prx_user_p_const_void_t;
    typedef uint32_t sys_prx_user_p_stop_level_t;

    typedef struct {
        uint64_t Base;				/* segment base */
        uint64_t FileSize;			/* segment size */
        uint64_t MemorySize;		/* segment size */
        uint64_t Index;				/* segment index */
        uint64_t Type;				/* segment type == ELF type */
    } sys_prx_segment_info_t;

    typedef struct sys_prx_module_info_t {
        uint64_t                      Size;
        char                          Name[30];
        char                          Version[2];
        uint32_t                      ModAttribute;
        uint32_t                      StartEntry;
        uint32_t                      StopEntry;
        uint32_t                      NumberOfAllSegments;
        sys_prx_user_pchar_t          FileName;
        uint32_t                      FileNameSize;
        sys_prx_user_segment_vector_t Segments;
        uint32_t                      NumberOfSegments;
    } sys_prx_module_info_t;

    typedef struct sys_prx_module_info_option_t {
        uint64_t Size;
        union {
            uint32_t ModuleInfo32;
            uint64_t ModuleInfo64;
        };
    } sys_prx_module_info_option_t;

    typedef struct sys_prx_unload_module_option_t {
	    uint64_t Size;				/* sizeof(this) */
    } sys_prx_unload_module_option_t;
}