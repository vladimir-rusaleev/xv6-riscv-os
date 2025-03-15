enum procinfo_states { PROCINFO_USED, PROCINFO_SLEEPING, PROCINFO_RUNNABLE, 
	PROCINFO_RUNNING, PROCINFO_ZOMBIE };

struct procinfo {
	int pid;
	char proc_name[16];
	enum procinfo_states state;
	int parent_pid;
};
