#include "Types.h"
#include "ELF.h"

struct SystemCallContext {
	uint64_t Args[ 8 ];
	uint64_t Index;
	OPD_t    Handler;
};

void InstallHooks();