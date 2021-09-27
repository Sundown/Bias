#include "../global.h"

um_Error builtin_import(um_Vector* v_params, um_Noun* result) {
	if (v_params->size != 1) { return MakeErrorCode(ERROR_ARGS); }

	if (v_params->data[0].type != string_t) {
		return MakeError(ERROR_TYPE,
				 "import: arg(0) must be of type string");
	}

	const um_Result res
	    = um_load_file(v_params->data[0].value.string->value);
	if (res.error._) {
		*result = nil;
		return res.error;
	}

	*result = new ((bool)true);
	return MakeErrorCode(OK);
}

um_Error builtin_type(um_Vector* v_params, um_Noun* result) {
	if (v_params->size != 1) { return MakeErrorCode(ERROR_ARGS); }

	*result = new_type(v_params->data[0].type);
	return MakeErrorCode(OK);
}

um_Error builtin_getlist(um_Vector* v_params, um_Noun* result) {
	if (v_params->size != 2) { return MakeErrorCode(ERROR_ARGS); }
	if (v_params->data[0].type != real_t) {
		return MakeError(
		    ERROR_TYPE,
		    "list_index: first parameter must be number type");
	}
	if (!listp(v_params->data[1]) && v_params->data[1].type != vector_t) {
		return MakeError(
		    ERROR_TYPE,
		    "list_index: second parameter must be list or vector");
	}

	if (v_params->data[1].type != vector_t) {
		*result = v_params->data[1]
			      .value.vector_v
			      ->data[(size_t)v_params->data[0].value.number];
	} else {
		*result = *list_index(&v_params->data[1],
				      (size_t)v_params->data[0].value.number);
	}
	return MakeErrorCode(OK);
}

um_Error builtin_setlist(um_Vector* v_params, um_Noun* result) {
	if (v_params->size != 3) { return MakeErrorCode(ERROR_ARGS); }
	if (v_params->data[0].type != real_t) {
		return MakeError(
		    ERROR_TYPE,
		    "list_index: first parameter must be number type");
	}
	if (!listp(v_params->data[1])) {
		return MakeError(ERROR_TYPE,
				 "list_index: second parameter must be list");
	}
	um_Noun t = copy_list(v_params->data[1]);
	um_Noun* i = list_index(&t, v_params->data[0].value.number);
	i->type = v_params->data[2].type;
	i->value = v_params->data[2].value;
	*result = t;
	return MakeErrorCode(OK);
}

um_Error builtin_len(um_Vector* v_params, um_Noun* result) {
	if (v_params->size != 1) { return MakeErrorCode(ERROR_ARGS); }

	if (listp(v_params->data[0])) {
		*result = new ((double)list_len(v_params->data[0]));
	} else if (v_params->data[0].type == string_t) {
		*result = new (
		    (double)strlen(v_params->data[0].value.string->value));
	} else if (v_params->data[0].type == vector_t) {

		*result = new ((double)v_params->data[0].value.vector_v->size);
	} else {
		*result = new ((double)0);
		return MakeErrorCode(ERROR_TYPE);
	}

	return MakeErrorCode(OK);
}

um_Error builtin_range(um_Vector* v_params, um_Noun* result) {
	if (v_params->size > 2 || v_params->size < 1) {
		return MakeError(ERROR_ARGS,
				 "range: arg count must be nonzero below 3");
	}

	if (v_params->data[0].type != real_t
	    || (v_params->size > 1 && v_params->data[1].type != real_t)) {
		return MakeError(ERROR_TYPE,
				 "range: args must be type numeric");
	}

	double a = v_params->size > 1
		     ? cast(v_params->data[0], real_t).value.number
		     : 0,
	       b = v_params->size > 1
		     ? cast(v_params->data[1], real_t).value.number
		     : cast(v_params->data[0], real_t).value.number;

	um_Noun range = nil;

	if (a < b) {
		for (; a <= b; b--) { range = cons(new (b), range); }
	} else {
		for (; a >= b; b++) { range = cons(new (b), range); }
	}

	*result = range;
	return MakeErrorCode(OK);
}

um_Error builtin_car(um_Vector* v_params, um_Noun* result) {
	um_Noun a;
	if (v_params->size != 1) { return MakeErrorCode(ERROR_ARGS); }

	a = v_params->data[0];
	if (isnil(a)) {
		*result = nil;
	} else if (a.type != pair_t) {
		return MakeErrorCode(ERROR_TYPE);
	} else {
		*result = car(a);
	}

	return MakeErrorCode(OK);
}

um_Error builtin_cdr(um_Vector* v_params, um_Noun* result) {
	um_Noun a;
	if (v_params->size != 1) { return MakeErrorCode(ERROR_ARGS); }

	a = v_params->data[0];
	if (isnil(a)) {
		*result = nil;
	} else if (a.type != pair_t) {
		return MakeErrorCode(ERROR_TYPE);
	} else {
		*result = cdr(a);
	}

	return MakeErrorCode(OK);
}

um_Error builtin_cons(um_Vector* v_params, um_Noun* result) {
	if (v_params->size != 2) { return MakeErrorCode(ERROR_ARGS); }

	*result = cons(v_params->data[0], v_params->data[1]);

	return MakeErrorCode(OK);
}

um_Error builtin_apply(um_Vector* v_params, um_Noun* result) {
	um_Noun fn;
	um_Vector v;
	um_Error err;

	if (v_params->size != 2) { return MakeErrorCode(ERROR_ARGS); }

	fn = v_params->data[0];
	noun_to_vector(v_params->data[1], &v);
	err = apply(fn, &v, result);
	vector_free(&v);
	return err;
}

um_Error builtin_eq(um_Vector* v_params, um_Noun* result) {
	um_Noun a, b;
	size_t i;
	if (v_params->size <= 1) {
		*result = new ((bool)true);
		return MakeErrorCode(OK);
	}

	for (i = 0; i < v_params->size - 1; i++) {
		a = v_params->data[i];
		b = v_params->data[i + 1];
		if (!eq_h(a, b)) {
			*result = new ((bool)false);
			return MakeErrorCode(OK);
		}
	}

	*result = new ((bool)true);
	return MakeErrorCode(OK);
}

um_Error builtin_eq_l(um_Vector* v_params, um_Noun* result) {
	um_Noun a, b;
	size_t i;
	if (v_params->size <= 1) {
		*result = sym_true;
		return MakeErrorCode(OK);
	}

	for (i = 0; i < v_params->size - 1; i++) {
		a = v_params->data[i];
		b = v_params->data[i + 1];
		if (!eq_l(a, b)) {
			*result = new ((bool)false);
			return MakeErrorCode(OK);
		}
	}

	*result = new ((bool)true);
	return MakeErrorCode(OK);
}

um_Error builtin_pairp(um_Vector* v_params, um_Noun* result) {
	if (v_params->size != 1) { return MakeErrorCode(ERROR_ARGS); }

	*result = v_params->data[0].type == pair_t ? sym_true : nil;
	return MakeErrorCode(OK);
}

um_Error builtin_not(um_Vector* v_params, um_Noun* result) {
	if (v_params->size != 1) {
		*result = nil;
		return MakeErrorCode(ERROR_ARGS);
	}

	*result = cast(v_params->data[0], bool_t).value.bool_v
		    ? new ((bool)false)
		    : sym_true;
	return MakeErrorCode(OK);
}

um_Error builtin_exit(um_Vector* v_params, um_Noun* result) {
	um_Noun code = nil;
	if (v_params->size != 1) { return MakeErrorCode(ERROR_ARGS); }

	if (isnil(code = cast(v_params->data[0], real_t))) {
		return MakeErrorCode(ERROR_TYPE);
	}

	*result = code;

	exit(code.value.number);
}

um_Error builtin_macex(um_Vector* v_params, um_Noun* result) {
	long alen = v_params->size;
	if (alen == 1) {
		um_Error err = macex(v_params->data[0], result);
		return err;
	} else
		return MakeErrorCode(ERROR_ARGS);
	return MakeErrorCode(OK);
}

um_Error builtin_vector(um_Vector* v_params, um_Noun* result) {
	um_Vector v;
	size_t i;

	vector_new(&v);
	for (i = 0; i < v_params->size; i++) {
		if (!isnil(v_params->data[i])) {
			vector_add(&v, v_params->data[i]);
		}
	}

	*result = new_vector(&v);
	return MakeErrorCode(OK);
}

um_Error builtin_string(um_Vector* v_params, um_Noun* result) {
	char* s = um_new_string();
	size_t i;
	for (i = 0; i < v_params->size; i++) {
		if (!isnil(v_params->data[i])) {
			char* a = to_string(v_params->data[i], 0);
			append_string(&s, a);
			free(a);
		}
	}

	*result = new_string(s);
	return MakeErrorCode(OK);
}

um_Error builtin_print(um_Vector* v_params, um_Noun* result) {
	size_t i;
	for (i = 0; i < v_params->size; i++) {
		if (!isnil(v_params->data[i])) {
			puts(to_string(v_params->data[i], 0));
		}
	}

	*result = nil;
	return MakeErrorCode(OK);
}

um_Error builtin_eval(um_Vector* v_params, um_Noun* result) {
	if (v_params->size == 1)
		return macex_eval(v_params->data[0], result);
	else
		return MakeErrorCode(ERROR_ARGS);
}

um_Error builtin_cast(um_Vector* v_params, um_Noun* result) {
	if (v_params->size == 2) {

		*result = cast(v_params->data[0],
			       v_params->data[1].type != type_t
				   ? v_params->data[1].type
				   : v_params->data[1].value.type_v);

		return MakeErrorCode(OK);
	} else {
		*result = nil;
		return MakeErrorCode(ERROR_ARGS);
	}
}

um_Error builtin_float(um_Vector* v_params, um_Noun* result) {
	if (v_params->size == 1) {
		*result = cast(v_params->data[0], real_t);

		return MakeErrorCode(OK);
	} else {
		return MakeErrorCode(ERROR_ARGS);
	}
}

um_Error builtin_and(um_Vector* v_params, um_Noun* result) {
	if (v_params->size != 2) { return MakeErrorCode(ERROR_ARGS); }

	*result = new ((bool)(cast(v_params->data[0], bool_t).value.bool_v
			      && cast(v_params->data[1], bool_t).value.bool_v));

	return MakeErrorCode(OK);
}

um_Error builtin_sin(um_Vector* v_params, um_Noun* result) {
	if (v_params->size == 1) {
		*result = new_number(
		    sin(cast(v_params->data[0], real_t).value.number));
		return MakeErrorCode(OK);
	} else {
		return MakeErrorCode(ERROR_ARGS);
	}
}

um_Error builtin_asin(um_Vector* v_params, um_Noun* result) {
	if (v_params->size == 1) {
		*result = new_number(
		    asin(cast(v_params->data[0], real_t).value.number));
		return MakeErrorCode(OK);
	} else {
		return MakeErrorCode(ERROR_ARGS);
	}
}

um_Error builtin_cos(um_Vector* v_params, um_Noun* result) {
	if (v_params->size == 1) {
		*result = new_number(
		    cos(cast(v_params->data[0], real_t).value.number));
		return MakeErrorCode(OK);
	} else {
		return MakeErrorCode(ERROR_ARGS);
	}
}

um_Error builtin_acos(um_Vector* v_params, um_Noun* result) {
	if (v_params->size == 1) {
		*result = new_number(
		    acos(cast(v_params->data[0], real_t).value.number));
		return MakeErrorCode(OK);
	} else {
		return MakeErrorCode(ERROR_ARGS);
	}
}

um_Error builtin_tan(um_Vector* v_params, um_Noun* result) {
	if (v_params->size == 1) {
		*result = new_number(
		    tan(cast(v_params->data[0], real_t).value.number));
		return MakeErrorCode(OK);
	} else {
		return MakeErrorCode(ERROR_ARGS);
	}
}

um_Error builtin_atan(um_Vector* v_params, um_Noun* result) {
	if (v_params->size == 1) {
		*result = new_number(
		    atan(cast(v_params->data[0], real_t).value.number));
		return MakeErrorCode(OK);
	} else {
		return MakeErrorCode(ERROR_ARGS);
	}
}

um_Error builtin_pow(um_Vector* v_params, um_Noun* result) {
	if (v_params->size != 2) { return MakeErrorCode(ERROR_ARGS); }

	double temp = pow(cast(v_params->data[0], real_t).value.number,
			  cast(v_params->data[1], real_t).value.number);

	*result = new_number(temp);

	return MakeErrorCode(OK);
}

um_Error builtin_cbrt(um_Vector* v_params, um_Noun* result) {
	if (v_params->size != 1) { return MakeErrorCode(ERROR_ARGS); }

	double temp = cbrt(cast(v_params->data[0], real_t).value.number);

	*result = new_number(temp);

	return MakeErrorCode(OK);
}

um_Error builtin_add(um_Vector* v_params, um_Noun* result) {
	size_t ac = v_params->size;
	um_Noun a0 = v_params->data[0], a1 = v_params->data[1];
	if (ac == 1) {
		*result = new (fabs(a0.value.number));
		return MakeErrorCode(OK);
	} else if (ac > 2 || ac < 1) {
		return MakeErrorCode(ERROR_ARGS);
	}

	double _temp
	    = cast(a0, real_t).value.number + cast(a1, real_t).value.number;

	*result = new (_temp);

	return MakeErrorCode(OK);
}

um_Error builtin_subtract(um_Vector* v_params, um_Noun* result) {
	size_t ac = v_params->size;
	um_Noun a0 = v_params->data[0], a1 = v_params->data[1];
	if (ac == 1) {
		*result = new (-fabs(a0.value.number));
		return MakeErrorCode(OK);
	} else if (ac > 2 || ac < 1) {
		return MakeErrorCode(ERROR_ARGS);
	}

	double _temp
	    = cast(a0, real_t).value.number - cast(a1, real_t).value.number;

	*result = new (_temp);
	return MakeErrorCode(OK);
}

um_Error builtin_modulo(um_Vector* v_params, um_Noun* result) {
	um_Noun a0 = v_params->data[0], a1 = v_params->data[1];
	if (v_params->size != 2) { return MakeErrorCode(ERROR_ARGS); }

	double _temp = (long)cast(a0, real_t).value.number
		     % (long)cast(a1, real_t).value.number;

	*result = new (_temp);

	return MakeErrorCode(OK);
}

um_Error builtin_multiply(um_Vector* v_params, um_Noun* result) {
	um_Noun a0 = v_params->data[0], a1 = v_params->data[1];
	if (v_params->size != 2) { return MakeErrorCode(ERROR_ARGS); }

	double _temp
	    = cast(a0, real_t).value.number * cast(a1, real_t).value.number;

	*result = new (_temp);

	return MakeErrorCode(OK);
}

um_Error builtin_divide(um_Vector* v_params, um_Noun* result) {
	um_Noun a0 = v_params->data[0], a1 = v_params->data[1];
	if (v_params->size != 2) { return MakeErrorCode(ERROR_ARGS); }

	double _temp
	    = cast(a0, real_t).value.number / cast(a1, real_t).value.number;

	*result = new (_temp);

	return MakeErrorCode(OK);
}

um_Error builtin_less(um_Vector* v_params, um_Noun* result) {
	um_Noun a0 = v_params->data[0], a1 = v_params->data[1];
	if (v_params->size != 2) { return MakeErrorCode(ERROR_ARGS); }

	*result = new ((bool)(cast(a0, real_t).value.number
			      < cast(a1, real_t).value.number));

	return MakeErrorCode(OK);
}

um_Error builtin_greater(um_Vector* v_params, um_Noun* result) {
	um_Noun a0 = v_params->data[0], a1 = v_params->data[1];
	if (v_params->size != 2) { return MakeErrorCode(ERROR_ARGS); }

	*result = new ((bool)(cast(a0, real_t).value.number
			      > cast(a1, real_t).value.number));

	return MakeErrorCode(OK);
}

um_Error builtin_floor(um_Vector* v_params, um_Noun* result) {
	um_Noun a0 = v_params->data[0];
	if (v_params->size != 1) { return MakeErrorCode(ERROR_ARGS); }

	*result = new (floor(cast(a0, real_t).value.number));

	return MakeErrorCode(OK);
}

um_Error builtin_ceil(um_Vector* v_params, um_Noun* result) {
	um_Noun a0 = v_params->data[0];
	if (v_params->size != 1) { return MakeErrorCode(ERROR_ARGS); }

	*result = new (ceil(cast(a0, real_t).value.number));

	return MakeErrorCode(OK);
}

um_Error builtin_hex(um_Vector* v_params, um_Noun* result) {
	um_Noun a0 = v_params->data[0];
	if (v_params->size != 1) { return MakeErrorCode(ERROR_ARGS); }

	char* str = calloc(33, sizeof(char));
	sprintf(str, "%x", (int32_t)cast(a0, real_t).value.number);

	char* tmp = strdup(str);

	*result = new (tmp);

	free(str);

	return MakeErrorCode(OK);
}

um_Error builtin_precision(um_Vector* v_params, um_Noun* result) {
	double v = cast(v_params->data[0], real_t).value.number;
	long n = (long)cast(v_params->data[1], real_t).value.number;
	if (v_params->size != 2) { return MakeErrorCode(ERROR_ARGS); }

	char* tmp = calloc(65, sizeof(char));
	char* fmt = calloc(7, sizeof(char));
	sprintf(fmt, "%%0.%ldf", n > 9 ? 9 : n);
	sprintf(tmp, fmt, v);

	char* str = strdup(tmp);

	*result = new (str);

	free(tmp);

	return MakeErrorCode(OK);
}

um_Error builtin_upper(um_Vector* v_params, um_Noun* result) {
	if (v_params->size != 1) { return MakeErrorCode(ERROR_ARGS); }
	char *s = strdup(cast(v_params->data[0], string_t).value.string->value),
	     *tmp = s;

	while (*tmp) {
		*tmp = (*tmp > 'a' && *tmp <= 'z') ? toupper(*tmp) : *tmp;
		tmp++;
	}

	*result = new (s);
	return MakeErrorCode(OK);
}

um_Error builtin_lower(um_Vector* v_params, um_Noun* result) {
	if (v_params->size != 1) { return MakeErrorCode(ERROR_ARGS); }
	char *s = strdup(cast(v_params->data[0], string_t).value.string->value),
	     *tmp = s;

	while (*tmp) {
		*tmp = (*tmp > 'A' && *tmp <= 'Z') ? tolower(*tmp) : *tmp;
		tmp++;
	}

	*result = new (s);
	return MakeErrorCode(OK);
}
