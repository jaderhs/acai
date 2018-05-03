enum acai_type {

	AT_AUTO,

	AT_INTEGER, //AT_INT64
	AT_INT8,
	AT_INT16,
	AT_INT32,
	AT_INT64,
	AT_INT128,

	AT_UINT8,
	AT_UINT16,
	AT_UINT32,
	AT_UINT64,
	AT_UINT128,

	AT_FLOAT, //AT_FLOAT32
	AT_FLOAT32,
	AT_FLOAT64,
	AT_FLOAT80,

	AT_BOOLEAN,
	AT_STRING
};

enum acai_value_flags {
	AVF_NULL = 1 >> 0,
	AVF_ZERO = 1 >> 1,
};

struct _acai_value_list {
	void *value; //acai_value
	struct _acai_value_list *next;
	struct _acai_value_list *prev;
};

typedef struct {
	unsigned long type; //acai_type
	unsigned long flags;
	union {
		int b;

		signed char i8;
		signed short i16;
		signed int i32;
		signed long long i64;

		unsigned char u8;
		unsigned short u16;
		unsigned int u32;
		unsigned long long u64;

		unsigned char ui128[16];

		float f32;
		double f64;
		long double f80;

		char *s;

		struct _acai_value_list *list;
	} v;
} acai_value;

#define ACAI_VALUE_IS_NULL(av) (av->flags & AVF_NULL)!=0

typedef int (*acai_func)(int argc, acai_value *argv[], acai_value *ret);
