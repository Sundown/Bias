#include "../global.h"

um_Noun cast(um_Noun a, um_NounType t) {
	if (a.type == t) { return a; }

	switch (a.type) {
		case nil_t:
		case noreturn_t: return nil_to_t(nil, t);
		case real_t: return real_to_t(a.value.number, t);
		case noun_t: return noun_to_t(a.value.symbol, t);
		case string_t: return string_to_t(a.value.string->value, t);
		case bool_t: return bool_to_t(a.value.bool_v, t);
		case type_t: return type_to_t(a.value.type_v, t);
		default:
			return nil; /* TODO can probably add more
				       coercions for semi-primitive
				       types */
	}
}

char* type_to_string(um_NounType a) {
	switch (a) {
		case nil_t: return "Nil";
		case pair_t: return "Pair";
		case string_t: return "String";
		case noun_t: return "Noun";
		case real_t: return "Float";
		case builtin_t: return "Builtin";
		case closure_t: return "Closure";
		case macro_t: return "Macro";
		case input_t: return "Input";
		case output_t: return "Output";
		case table_t: return "Table";
		case bool_t: return "Bool";
		case type_t: return "Type";
		case error_t: return "Error";
		case vector_t: return "Vector";
		default: return "Unknown";
	}
}

char* error_to_string(um_Error e) {
	char* s = calloc(e.message != NULL ? strlen(e.message) : 0 + 27,
			 sizeof(char));
	e._ != MakeErrorCode(ERROR_USER)._&& e.message
	    ? sprintf(s, "%s\n%s\n", error_string[e._], e.message)
	    : sprintf(s, "%s\n", error_string[e._]);

	return s;
}

um_Noun nil_to_t(um_Noun x __attribute__((unused)), um_NounType t) {
	switch (t) {
		case nil_t: return nil;
		case real_t: return new_number(NAN);
		case pair_t: return cons(nil, nil);
		case bool_t: return new_bool(false);
		case type_t: return new_type(nil_t);
		case string_t: return new_string("nil");
		case noun_t: return intern("nil");
		default: return nil;
	}
}

um_Noun real_to_t(double x, um_NounType t) {
	if (t == real_t) { return new_number(x); }

	char* buf = NULL;
	if (t == noun_t || t == string_t) {
		buf = calloc(65, sizeof(char));
		snprintf(buf, 65, "%f", x);
		buf = realloc(buf, strlen(buf) * sizeof(char) + 1);
	}

	switch (t) {
		case nil_t: return nil;
		case noun_t: return intern(buf);
		case bool_t: return new_bool(x > 0 && isnormal(x) && !isnan(x));
		case pair_t: return cons(new_number(x), nil);
		case string_t: return new_string(buf);
		case type_t: return new_type(real_t);
		default: return nil;
	}
}

um_Noun noun_to_t(char* x, um_NounType t) {
	switch (t) {
		case pair_t: return cons(intern(x), nil);
		case noun_t: return intern(x);
		case real_t: return new_number(strtod(x, NULL));
		case string_t: return new_string(x);
		case type_t: return new_type(noun_t);
		case bool_t:
			return new_bool(x != NULL
					&& x != intern("nil").value.symbol);
		default: return nil;
	}
}

um_Noun string_to_t(char* x, um_NounType t) {
	switch (t) {
		case pair_t: return cons(intern(x), nil);
		case noun_t: return intern(x);
		case real_t: return new_number(strtod(x, NULL));
		case string_t: return new_string(x);
		case type_t: return new_type(noun_t);
		case bool_t:
			return new_bool(x != NULL && strcmp(x, "nil")
					&& strcmp(x, "false"));
		default: return nil;
	}
}

um_Noun bool_to_t(bool x, um_NounType t) {
	switch (t) {
		case bool_t: return new_bool(x);
		case pair_t: return cons(new_bool(x), nil);
		case real_t: return new_number((double)x);
		case noun_t: return x ? intern("true") : intern("false");
		case string_t:
			return x ? new_string("true") : new_string("false");
		case type_t: return new_type(bool_t);
		default: return nil;
	}
}

um_Noun type_to_t(um_NounType x, um_NounType t) {
	switch (t) {
		case type_t: return new_type(type_t);
		case noun_t: return intern(error_string[x]);
		case string_t: return new_string((char*)error_string[x]);
		case bool_t: return new_bool(!x);
		case pair_t: return cons(new_type(x), nil);
		default: return nil;
	}
}
