

typedef struct {
	int type;
	union {
		int i;
		char *s;
	} v;
} acai_type;

typedef void *(*acai_func)(int argc, acai_type *argv[]);
