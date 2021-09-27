#include "../global.h"



void um_init() {
	srand((unsigned)time(0));
	if (!um_global_symbol_capacity) { um_global_symbol_capacity = 1000; }
	env = env_create(nil, um_global_symbol_capacity);

	symbol_table = calloc(um_global_symbol_capacity, sizeof(char*));

	sym_quote = intern("quote");
	sym_quasiquote = intern("quasiquote");
	sym_unquote = intern("unquote");
	sym_unquote_splicing = intern("unquote-splicing");
	sym_def = intern("def");
	sym_const = intern("const");
	sym_defun = intern("defun");
	sym_fn = intern("lambda");
	sym_if = intern("if");
	sym_cond = intern("cond");
	sym_switch = intern("switch");
	sym_match = intern("match");

	sym_mac = intern("mac");
	sym_apply = intern("apply");
	sym_cons = intern("cons");
	sym_string = intern("str");
	sym_string = intern("vec");
	sym_num = intern("num");
	sym_char = intern("char");
	sym_do = intern("do");
	sym_set = intern("set");
	sym_true = intern("true");
	sym_false = intern("false");

	sym_nil_t = intern("@Nil");
	sym_pair_t = intern("@Pair");
	sym_noun_t = intern("@Noun");
	sym_f64_t = intern("@Float");
	sym_builtin_t = intern("@Builtin");
	sym_closure_t = intern("@Closure");

	sym_macro_t = intern("@Macro");
	sym_string_t = intern("@String");
	sym_vector_t = intern("@Vector");
	sym_input_t = intern("@Input");
	sym_output_t = intern("@Output");
	sym_error_t = intern("@Error");
	sym_type_t = intern("@Type");
	sym_bool_t = intern("@Bool");

#define add_builtin(name, fn_ptr) \
	env_assign(env, intern(name).value.symbol, new_builtin(fn_ptr))

	env_assign(env, sym_true.value.symbol, new ((bool)true));
	env_assign(env, sym_false.value.symbol, new ((bool)false));
	env_assign(env, intern("nil").value.symbol, nil);
	env_assign(env, intern("_").value.symbol, um_noreturn);

	env_assign(env, sym_nil_t.value.symbol, new ((um_NounType)nil_t));
	env_assign(env, sym_pair_t.value.symbol, new ((um_NounType)pair_t));
	env_assign(env, sym_noun_t.value.symbol, new ((um_NounType)noun_t));
	env_assign(env, sym_f64_t.value.symbol, new ((um_NounType)real_t));
	env_assign(
	    env, sym_builtin_t.value.symbol, new ((um_NounType)builtin_t));
	env_assign(
	    env, sym_closure_t.value.symbol, new ((um_NounType)closure_t));
	env_assign(env, sym_macro_t.value.symbol, new ((um_NounType)macro_t));
	env_assign(env, sym_string_t.value.symbol, new ((um_NounType)string_t));
	env_assign(env, sym_vector_t.value.symbol, new ((um_NounType)vector_t));
	env_assign(env, sym_input_t.value.symbol, new ((um_NounType)input_t));
	env_assign(env, sym_output_t.value.symbol, new ((um_NounType)output_t));
	env_assign(env, sym_error_t.value.symbol, new ((um_NounType)error_t));
	env_assign(env, sym_type_t.value.symbol, new ((um_NounType)type_t));
	env_assign(env, sym_bool_t.value.symbol, new ((um_NounType)bool_t));

	add_builtin("car", builtin_car);
	add_builtin("cdr", builtin_cdr);
	add_builtin("cons", builtin_cons);

	add_builtin("+", builtin_add);
	add_builtin("-", builtin_subtract);
	add_builtin("*", builtin_multiply);
	add_builtin("/", builtin_divide);
	add_builtin("%", builtin_modulo);

	add_builtin("__math_gt", builtin_greater);
	add_builtin("__math_lt", builtin_less);
	add_builtin("=", builtin_eq);
	add_builtin("eq?", builtin_eq);
	add_builtin("eqv?", builtin_eq_l);

	add_builtin("__builtin_pow", builtin_pow);
	add_builtin("__builtin_cbrt", builtin_cbrt);
	add_builtin("not", builtin_not);
	add_builtin("__builtin_sin", builtin_sin);
	add_builtin("__builtin_cos", builtin_cos);
	add_builtin("__builtin_tan", builtin_tan);
	add_builtin("__builtin_asin", builtin_asin);
	add_builtin("__builtin_acos", builtin_acos);
	add_builtin("__builtin_atan", builtin_atan);
	add_builtin("len", builtin_len);
	add_builtin("eval", builtin_eval);
	add_builtin("type", builtin_type);
	add_builtin("exit", builtin_exit);
	add_builtin("apply", builtin_apply);
	add_builtin("macex", builtin_macex);
	add_builtin("str", builtin_string);
	add_builtin("print", builtin_print);
	add_builtin("pair?", builtin_pairp);
	add_builtin("float", builtin_float);
	add_builtin("range", builtin_range);
	add_builtin("cast", builtin_cast);
	add_builtin("getlist", builtin_getlist);
	add_builtin("and", builtin_and);
	add_builtin("setlist", builtin_setlist);
	add_builtin("__builtin_vector", builtin_vector);
	add_builtin("__builtin_ceil", builtin_ceil);
	add_builtin("__builtin_floor", builtin_floor);
	add_builtin("__builtin_format_hex", builtin_hex);
	add_builtin("__builtin_format_precision", builtin_precision);
	add_builtin("import", builtin_import);

	add_builtin("__builtin_format_upper", builtin_upper);
	add_builtin("__builtin_format_lower", builtin_lower);

	add_builtin("if", new_builtin(NULL).value.builtin);
	add_builtin("fn", new_builtin(NULL).value.builtin);
	add_builtin("do", new_builtin(NULL).value.builtin);
	add_builtin("def", new_builtin(NULL).value.builtin);
	add_builtin("const", new_builtin(NULL).value.builtin);
	add_builtin("mac", new_builtin(NULL).value.builtin);
	add_builtin("cond", new_builtin(NULL).value.builtin);
	add_builtin("switch", new_builtin(NULL).value.builtin);
	add_builtin("match", new_builtin(NULL).value.builtin);
	add_builtin("defun", new_builtin(NULL).value.builtin);
	add_builtin("quote", new_builtin(NULL).value.builtin);
	add_builtin("lambda", new_builtin(NULL).value.builtin);

	ingest("\
(defun compose (f g)\
	(lambda (x) (f (g x))))");

	ingest("\
(def (foldl proc init list)\
	(if !(nil? list)\
		(foldl proc (proc init (car list)) (cdr list))\
		init))");

	ingest("\
(def (foldr p i l)\
	(if !(nil? l)\
		(p (car l) (foldr p i (cdr l)))\
		i))");

	ingest("\
(def (nil? x)\
	(= x ()))");

	ingest("\
(def (list . items)\
	(foldr cons nil items))");

	ingest("\
(def (unary-map proc list)\
	(foldr\
		(lambda (x rest) (cons (proc x) rest))\
		nil\
		list))");

	ingest("\
(def (map proc . arg-lists)\
	(if !(nil? (car arg-lists))\
		(cons\
			(apply proc (unary-map car arg-lists))\
			(apply map (cons proc (unary-map cdr arg-lists))))\
		nil))");

	ingest("\
(def (caar x)\
	(car (car x)))");

	ingest("\
(def (cadr x)\
	(car (cdr x)))");

	ingest("\
(mac unless (cond expr)\
	(list 'if condition () expr))");

	ingest("\
(def (append a b)\
	(foldr cons b a))");

	ingest("\
(mac quasiquote (x)\
 	(if (pair? x)\
  		(if (= (car x) 'unquote)\
   			(cadr x)\
    			(if (if (pair? (car x)) (= (caar x) 'unquote-splicing))\
     				(list 'append (cadr (car x)) (list 'quasiquote (cdr x)))\
     				(list 'cons (list 'quasiquote (car x)) (list 'quasiquote (cdr x)))))\
    		(list 'quote x)))");

	ingest("\
(mac let (defs . body)\
	`((lambda ,(map car defs) ,@body) ,@(map cadr defs)))");

	ingest("\
(defun std (fun)\
	(switch fun \
		('vector __builtin_vector) \
		('list list) \
		('map map) \
		('cast cast)))");

	ingest("\
(defun format (fun)\
	(switch fun\
		('hex __builtin_format_hex)\
		('precision __builtin_format_precision)\
		('upper __builtin_format_upper)\
		('lower __builtin_format_lower)))");

	ingest("\
(defun math (fun)\
	(switch fun\
		('pi 3.1415926535897931)\
		('e 2.7182818284590452)\
		('ceil __builtin_ceil)\
		('floor __builtin_floor)\
		('tan __builtin_tan)\
		('sin __builtin_sin)\
		('cos __builtin_cos)\
		('atan __builtin_atan)\
		('asin __builtin_asin)\
		('acos __builtin_acos)\
		('range range)\
		('sqrt (lambda (x) (math::pow x (float 0.5))))\
		('cbrt __builtin_cbrt)\
		('square (lambda (x) (math::pow x 2)))\
		('cube (lambda (x) (math::pow x 3)))\
		('sum (lambda (x) (reduce + x 0)))\
                ('abs (lambda (x) (if (math::less x 0) (+ x) x)))\
		('product (lambda (x) (reduce * x 1)))\
		('less __math_lt)\
		('greater __math_gt)\
		('sigma (lambda (f s e)\
			(reduce + (map f (range s e)) 0)))\
		('min (lambda (x) \
			(if (nil? (cdr x))\
				(car x) \
				(foldl (lambda (a b) (if (math::less a b) a b)) (car x) (cdr x)))))\
		('max (lambda (x) \
			(if (nil? (cdr x))\
				(car x) \
				(foldl (lambda (a b) (if (math::less a b) b a)) (car x) (cdr x)))))\
		('pow __builtin_pow)))");

	ingest("\
(def (for-each proc items)\
  	(if (nil? items)\
   		_\
   		(if ((lambda (x) true) (proc (car items))) \
    			(for-each proc (cdr items)))))");

	ingest("\
(def (filter pred lst)\
   	(if (nil? lst)\
    		()\
    		(if (pred (car lst))\
     			(cons (car lst)\
     			(filter pred (cdr lst)))\
   	(filter pred (cdr lst)))))");

	ingest("\
(defun curry (f)\
	(lambda (a) (lambda (b) (f a b))))");

	ingest("\
(def (reduce f l id)\
        (if (nil? l)\
                id\
                (f (car l) (reduce f (cdr l) id))))");
}
