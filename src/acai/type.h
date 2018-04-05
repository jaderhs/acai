enum acai_type {
	AT_INTEGER,
	AT_FLOAT,
	AT_STRING
};

typedef struct {
	enum acai_type type;
	union {
		int i;
		float f;
		char *s;
	} v;
} acai_value;

typedef void *(*acai_func)(int argc, acai_value *argv[]);
