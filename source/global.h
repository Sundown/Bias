/* MIT License

Copyright (c) 2021 sundown

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */

#ifndef um_H
#define um_H

#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define VECTOR_GROWTH_FACTOR 2

typedef enum {
	nil_t,
	noreturn_t,
	pair_t,
	noun_t,
	real_t,
	builtin_t,
	closure_t,
	macro_t,
	vector_t,
	string_t,
	input_t,
	output_t,
	table_t,
	error_t,
	type_t,
	bool_t
} um_NounType;

typedef enum {
	OK = 0,
	ERROR_SYNTAX,
	ERROR_UNBOUND,
	ERROR_ARGS,
	ERROR_TYPE,
	ERROR_FILE,
	ERROR_USER,
	ERROR_NOMUT,
	ERROR_COERCION_FAIL
} um_ErrorCode;

typedef struct {
	um_ErrorCode _;
	char* message;
} um_Error;

static const char* error_string[] = {"",
				     "Syntax error",
				     "Symbol unbound",
				     "Parameter error",
				     "Type error",
				     "File error",
				     "",
				     "Cannot mutate constant",
				     "Coercion error"};

typedef struct um_Noun um_Noun;
typedef struct um_Vector um_Vector;

typedef um_Error (*um_Builtin)(struct um_Vector* v_params,
			       struct um_Noun* result);

struct um_Noun {
	um_NounType type;
	bool mut;
	union {
		um_NounType type_v;
		bool bool_v;
		char character;
		um_Error error_v;
		double number;
		struct um_Pair* pair;
		char* symbol;
		struct um_String* string;
		FILE* fp;
		um_Builtin builtin;
		um_Vector* vector_v;
		struct um_Table* table;
		um_Error err_v;
	} value;
};

/* This should be returned by any functions which a user may interract with
 * directly */
struct um_Result {
	um_Error error;
	um_Noun data;
};
typedef struct um_Result um_Result;

struct um_Pair {
	struct um_Noun car, cdr;
	char mark;
	struct um_Pair* next;
};
typedef struct um_Pair um_Pair;

struct um_TableEntry {
	um_Noun k, v;
	struct um_TableEntry* next;
};
typedef struct um_TableEntry um_TableEntry;

struct um_Table {
	size_t capacity;
	size_t size;
	um_TableEntry** data;
	char mark;
	struct um_Table* next;
};
typedef struct um_Table um_Table;

struct um_Vector {
	um_Noun* data;
	um_Noun static_data[8];
	size_t capacity, size;
};

struct um_String {
	char* value;
	char mark;
	struct um_String* next;
};

static const um_Noun nil
    = {.type = nil_t, .mut = false, .value = {.type_v = nil_t}};

static const um_Noun um_noreturn
    = {.type = noreturn_t, .mut = false, .value = {.type_v = noreturn_t}};

um_Noun sym_quote, sym_const, sym_quasiquote, sym_unquote, sym_unquote_splicing,
    sym_def, sym_set, sym_defun, sym_fn, sym_if, sym_cond, sym_switch,
    sym_match, sym_mac, sym_apply, sym_cons, sym_string, sym_num, sym_char,
    sym_do, sym_true, sym_false,

    sym_nil_t, sym_pair_t, sym_noun_t, sym_f64_t, sym_builtin_t, sym_closure_t,
    sym_macro_t, sym_string_t, sym_vector_t, sym_input_t, sym_output_t,
    sym_error_t, sym_type_t, sym_bool_t;

um_Noun env;
static size_t stack_capacity = 0;
static size_t stack_size = 0;
static um_Noun* stack = NULL;
static um_Pair* pair_head = NULL;
static struct um_String* str_head = NULL;
static um_Table* table_head = NULL;
static size_t alloc_count = 0;
static size_t alloc_count_old = 0;
char** symbol_table;
size_t symbol_size;
size_t um_global_symbol_capacity;
um_Noun cur_expr;

/* clang-format off */
#define car(p)	    	((p).value.pair->car)
#define cdr(p)	    	((p).value.pair->cdr)
#define cdr2(p)	    	(cdr(p))
#define pop(n)	    	(n = cdr2(n))
#define isnil(n)	((n).type == nil_t || (n).type == noreturn_t)

#define ingest(s) do { um_Result _0 = um_interpret_string(s); \
	if (_0.error._) { um_print_error(_0.error); }} while (0)

#define MakeErrorCode(c) (um_Error){c, NULL}
#define MakeError(c, m)  (um_Error){c, m}
#define new(T) _Generic((T),  	\
	bool: new_bool,      	\
	char*: new_string,   	\
	double: new_number,  	\
	um_Builtin: new_builtin,\
	um_NounType: new_type  	\
)(T)

inline um_Noun new_number(double x) { return (um_Noun){real_t, true, {.number = x}}; }
inline um_Noun new_builtin(um_Builtin fn) { return (um_Noun){builtin_t, true, {.builtin = fn}}; }
inline um_Noun new_type(um_NounType t) { return (um_Noun){type_t, true, {.type_v = t}}; }
inline um_Noun new_bool(bool b) { return (um_Noun){bool_t, true, {.bool_v = b}}; }
inline um_Noun new_vector(um_Vector* v) { return (um_Noun){vector_t, true, {.vector_v = v}};}
/* clang-format on */

/*
	Begin necessary forward declarations
*/
um_Noun cons(um_Noun car_val, um_Noun cdr_val);
um_Noun intern(const char* buf);
um_Noun new_string(char* x);

void stack_add(um_Noun a);

um_Noun real_to_t(double x, um_NounType t);
um_Noun noun_to_t(char* x, um_NounType t);
um_Noun string_to_t(char* x, um_NounType t);
um_Noun bool_to_t(bool x, um_NounType t);
um_Noun type_to_t(um_NounType x, um_NounType t);
um_Noun nil_to_t(um_Noun x __attribute__((unused)), um_NounType t);

bool listp(um_Noun expr);
um_Noun reverse_list(um_Noun list);

um_Error macex_eval(um_Noun expr, um_Noun* result);
um_Error eval_expr(um_Noun expr, um_Noun env, um_Noun* result);

um_Noun env_create(um_Noun parent, size_t capacity);
um_Error env_bind(um_Noun env, um_Noun arg_names, um_Vector* v_params);
um_Error env_assign(um_Noun env, char* symbol, um_Noun value);
um_Error env_get(um_Noun env, char* symbol, um_Noun* result);

um_Noun new_table(size_t capacity);
um_TableEntry* table_get_sym(um_Table* tbl, char* k);
um_Error table_set_sym(um_Table* tbl, char* k, um_Noun v);

void garbage_collector_consider();
void garbage_collector_tag(um_Noun root);

void um_print_expr(um_Noun a);
void um_print_error(um_Error e);
void um_print_result(um_Result r);

size_t hash_code_sym(char* s);

char* um_new_string();
char* to_string(um_Noun a, bool write);
char* append_string(char** dst, char* src);

bool eq_l(um_Noun a, um_Noun b);
bool eq_h(um_Noun a, um_Noun b);
bool eq_pair_l(um_Noun a, um_Noun b);
bool eq_pair_h(um_Noun a, um_Noun b);

char* readline_fp(char* prompt, FILE* fp);
um_Error read_expr(const char* input, const char** end, um_Noun* result);

um_Result um_load_file(const char* path);
void um_init();
void um_repl();
um_Result um_interpret_string(const char* text);
um_Vector *vector_new(void);

void vector_add(um_Vector* a, um_Noun item);
void vector_clear(um_Vector* a);
void vector_free(um_Vector* a);
um_Vector *noun_to_vector(um_Noun a);
um_Noun vector_to_noun(um_Vector *a, size_t start);

void stack_add(um_Noun a);
void stack_restore(int saved_size);
void stack_restore_add(int saved_size, um_Noun a);

um_Noun cast(um_Noun a, um_NounType t);
char* type_to_string(um_NounType a);
char* error_to_string(um_Error e);

um_Error macex(um_Noun expr, um_Noun* result);

um_Noun copy_list(um_Noun list);
um_Noun* list_index(um_Noun* list, size_t index);
size_t list_len(um_Noun xs);
um_Error apply(um_Noun fn, um_Vector* v_params, um_Noun* result);

um_Error builtin_import(um_Vector* v_params, um_Noun* result);
um_Error builtin_type(um_Vector* v_params, um_Noun* result);
um_Error builtin_getlist(um_Vector* v_params, um_Noun* result);
um_Error builtin_setlist(um_Vector* v_params, um_Noun* result);
um_Error builtin_len(um_Vector* v_params, um_Noun* result);
um_Error builtin_range(um_Vector* v_params, um_Noun* result);
um_Error builtin_car(um_Vector* v_params, um_Noun* result);
um_Error builtin_cdr(um_Vector* v_params, um_Noun* result);
um_Error builtin_cons(um_Vector* v_params, um_Noun* result);
um_Error builtin_apply(um_Vector* v_params, um_Noun* result);
um_Error builtin_eq(um_Vector* v_params, um_Noun* result);
um_Error builtin_eq_l(um_Vector* v_params, um_Noun* result);
um_Error builtin_pairp(um_Vector* v_params, um_Noun* result);
um_Error builtin_not(um_Vector* v_params, um_Noun* result);
um_Error builtin_exit(um_Vector* v_params, um_Noun* result);
um_Error builtin_macex(um_Vector* v_params, um_Noun* result);
um_Error builtin_vector(um_Vector* v_params, um_Noun* result);
um_Error builtin_string(um_Vector* v_params, um_Noun* result);
um_Error builtin_print(um_Vector* v_params, um_Noun* result);
um_Error builtin_eval(um_Vector* v_params, um_Noun* result);
um_Error builtin_cast(um_Vector* v_params, um_Noun* result);
um_Error builtin_float(um_Vector* v_params, um_Noun* result);
um_Error builtin_and(um_Vector* v_params, um_Noun* result);
um_Error builtin_sin(um_Vector* v_params, um_Noun* result);
um_Error builtin_asin(um_Vector* v_params, um_Noun* result);
um_Error builtin_cos(um_Vector* v_params, um_Noun* result);
um_Error builtin_acos(um_Vector* v_params, um_Noun* result);
um_Error builtin_tan(um_Vector* v_params, um_Noun* result);
um_Error builtin_atan(um_Vector* v_params, um_Noun* result);
um_Error builtin_pow(um_Vector* v_params, um_Noun* result);
um_Error builtin_cbrt(um_Vector* v_params, um_Noun* result);
um_Error builtin_add(um_Vector* v_params, um_Noun* result);
um_Error builtin_subtract(um_Vector* v_params, um_Noun* result);
um_Error builtin_modulo(um_Vector* v_params, um_Noun* result);
um_Error builtin_multiply(um_Vector* v_params, um_Noun* result);
um_Error builtin_divide(um_Vector* v_params, um_Noun* result);
um_Error builtin_less(um_Vector* v_params, um_Noun* result);
um_Error builtin_greater(um_Vector* v_params, um_Noun* result);
um_Error builtin_floor(um_Vector* v_params, um_Noun* result);
um_Error builtin_ceil(um_Vector* v_params, um_Noun* result);
um_Error builtin_hex(um_Vector* v_params, um_Noun* result);
um_Error builtin_precision(um_Vector* v_params, um_Noun* result);
um_Error builtin_upper(um_Vector* v_params, um_Noun* result);
um_Error builtin_lower(um_Vector* v_params, um_Noun* result);
um_Error builtin_append(um_Vector* v_params, um_Noun* result);

#endif
