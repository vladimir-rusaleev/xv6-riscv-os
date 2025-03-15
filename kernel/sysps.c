#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "procinfo.h"

int sys_ps_listinfo(void) {
	uint64 plist;
	int lim;
	
	argaddr(0, &plist);
	argint(1, &lim);

	return ps_listinfo((struct procinfo *)plist, lim);
}
