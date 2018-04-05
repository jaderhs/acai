

typedef struct {
	int type;
	union {
		int i;
		float f;
		char *s;
	} v;
} acai_type;

typedef void *(*acai_func)(int argc, acai_type *argv[]);
