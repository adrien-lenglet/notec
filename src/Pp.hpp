#pragma once

#include "clib.hpp"

#include "TokenStream.hpp"
#include "Map.hpp"

class Cpp;

class Pp : private Map
{
	friend class Cpp;

	Token::Stream m_stream;
	uint32_t m_dirs;
	uint32_t m_keywords;

	inline void error(const char *str)
	{
		m_stream.error(str);
	}

	static inline constexpr size_t pmacro_count = 7;
	static inline constexpr const char *pmacros_names[pmacro_count] = {
		"__cplusplus",
		"__DATE__",
		"__FILE__",
		"__LINE__",
		"__STDC_HOSTED__",
		"__STDCPP_DEFAULT_NEW_ALIGNMENT__",
		"__TIME__"
	};

	struct KeywordDef {
		const char *name;
		Token::Op op;
	};
	static inline constexpr KeywordDef keyword_defs[] = {
		{"alignas", Token::Op::Alignas},
		{"alignof", Token::Op::Alignof},
		{"and", Token::Op::And},
		{"and_eq", Token::Op::BitAndEqual},
		{"asm", Token::Op::Asm},
		{"auto", Token::Op::Auto},
		{"bitand", Token::Op::BitAnd},
		{"bitor", Token::Op::BitOr},
		{"bool", Token::Op::Bool},
		{"break", Token::Op::Break},
		{"case", Token::Op::Case},
		{"catch", Token::Op::Catch},
		{"char", Token::Op::Char},
		{"char8_t", Token::Op::Char8_t},
		{"char16_t", Token::Op::Char16_t},
		{"char32_t", Token::Op::Char32_t},
		{"class", Token::Op::Class},
		{"compl", Token::Op::Tilde},
		{"concept", Token::Op::Concept},
		{"const", Token::Op::Const},
		{"consteval", Token::Op::Consteval},
		{"constexpr", Token::Op::Constexpr},
		{"constinit", Token::Op::Constinit},
		{"constinit", Token::Op::Constinit},
		{"const_cast", Token::Op::ConstCast},
		{"continue", Token::Op::Continue},
		{"co_await", Token::Op::CoAwait},
		{"co_return", Token::Op::CoReturn},
		{"co_yield", Token::Op::CoYield},

		{"decltype", Token::Op::Decltype},
		{"default", Token::Op::Default},
		{"delete", Token::Op::Delete},
		{"do", Token::Op::Do},
		{"double", Token::Op::Double},
		{"dynamic_cast", Token::Op::DynamicCast},
		{"else", Token::Op::Else},
		{"enum", Token::Op::Enum},
		{"explicit", Token::Op::Explicit},
		{"export", Token::Op::Export},
		{"extern", Token::Op::Extern},
		{"false", Token::Op::False},
		{"float", Token::Op::Float},
		{"for", Token::Op::For},
		{"friend", Token::Op::Friend},
		{"goto", Token::Op::Goto},
		{"if", Token::Op::If},
		{"inline", Token::Op::Inline},
		{"int", Token::Op::Int},
		{"long", Token::Op::Long},
		{"mutable", Token::Op::Mutable},
		{"namespace", Token::Op::Namespace},
		{"new", Token::Op::New},
		{"noexcept", Token::Op::Noexcept},
		{"not", Token::Op::Not},
		{"not_eq", Token::Op::NotEqual},
		{"nullptr", Token::Op::Nullptr},
		{"operator", Token::Op::Operator},
		{"or", Token::Op::Or},
		{"or_eq", Token::Op::BitOrEqual},
		{"private", Token::Op::Private},
		{"protected", Token::Op::Protected},
		{"public", Token::Op::Public},

		{"register", Token::Op::Register},
		{"reinterpret_cast", Token::Op::ReinterpretCast},
		{"requires", Token::Op::Requires},
		{"return", Token::Op::Return},
		{"short", Token::Op::Short},
		{"signed", Token::Op::Signed},
		{"sizeof", Token::Op::Sizeof},
		{"static", Token::Op::Static},
		{"static_assert", Token::Op::StaticAssert},
		{"static_cast", Token::Op::StaticCast},
		{"struct", Token::Op::Struct},
		{"switch", Token::Op::Switch},
		{"template", Token::Op::Template},
		{"this", Token::Op::This},
		{"thread_local", Token::Op::ThreadLocal},
		{"throw", Token::Op::Throw},
		{"true", Token::Op::True},
		{"try", Token::Op::Try},
		{"typedef", Token::Op::Typedef},
		{"typeid", Token::Op::Typeid},
		{"typename", Token::Op::Typename},
		{"union", Token::Op::Union},
		{"unsigned", Token::Op::Unsigned},
		{"using", Token::Op::Using},
		{"virtual", Token::Op::Virtual},
		{"void", Token::Op::Void},
		{"volatile", Token::Op::Volatile},
		{"wchar_t", Token::Op::Wchar_t},
		{"while", Token::Op::While},
		{"xor", Token::Op::BitXor},
		{"xor_eq", Token::Op::BitXorEqual}
	};

public:
	inline Pp(void)
	{
		m_dirs = create_root();
		m_keywords = create_root();
		m_macros = create_root();

		#define PP_DIRECTIVE_NEXT(id) insert(m_dirs, #id, &Pp::id)
		#define PP_DIRECTIVE_NEXT_V(name, id) insert(m_dirs, name, &Pp::id)
		PP_DIRECTIVE_NEXT(include);
		PP_DIRECTIVE_NEXT(define);
		PP_DIRECTIVE_NEXT(undef);
		PP_DIRECTIVE_NEXT_V("error", derror);
		PP_DIRECTIVE_NEXT(pragma);
		PP_DIRECTIVE_NEXT(line);
		PP_DIRECTIVE_NEXT_V("if", dif);
		PP_DIRECTIVE_NEXT(ifdef);
		PP_DIRECTIVE_NEXT(ifndef);
		PP_DIRECTIVE_NEXT(elif);
		PP_DIRECTIVE_NEXT_V("else", delse);
		PP_DIRECTIVE_NEXT(endif);
		#undef PP_DIRECTIVE_NEXT
		m_stack = m_stack_base;

		for (size_t i = 0; i < array_size(pmacros_names); i++) {
			insert(m_macros, pmacros_names[i]);
			alloc(1);
			store(static_cast<char>(i + 1));	// 0 is reserved for regular macro
		}

		for (size_t i = 0; i < array_size(keyword_defs); i++)
			insert(m_keywords, keyword_defs[i].name, keyword_defs[i].op);
	}
	inline ~Pp(void)
	{
	}

	inline Token::Stream& get_tstream(void)
	{
		return m_stream;
	}

	inline Stream& get_stream(void)	// call on error and for custom stream modifications before polling (testing only)
	{
		return m_stream.get_stream();
	}

	inline void open(const char *filename)	// to call once before polling
	{
		m_stream.push(filename);
	}

	inline void include_dir(const char *dir)
	{
		if (!m_stream.get_stream().include_dir(dir))
			error("Include directories overflow");
	}

private:
	inline void assert_token(const char *token)
	{
		if (token == nullptr)
			error("Expected token");
	}

	inline void assert_token_type(const char *token, Token::Type type)
	{
		if (Token::type(token) != type)
			error("Bad token type");
	}

	inline const char* next_token(void)
	{
		while (true) {
			const char *res;
			res = m_stream.next();
			if (res != nullptr)
				return res;
			if (m_stream.pop()) {
				if (next_token_dir_exp(res))
					error("Expected no futher token");
				return res;
			} else
				return nullptr;
		}
	}

	template <auto PollMet>
	inline bool _next_token_dir(const char *&res)
	{
		auto lrow = m_stream.get_row();
		auto lstack = m_stream.get_stack();
		res = (this->*PollMet)();
		if (lrow != m_stream.get_row())
			if (!m_stream.get_line_escaped() || lstack != m_stream.get_stack())
				return false;
		return res != nullptr;
	}

	inline bool next_token_dir(const char* &res)
	{
		return _next_token_dir<&Pp::next_token>(res);
	}

	inline bool next_token_dir_exp(const char* &res)
	{
		return _next_token_dir<&Pp::next_base>(res);
	}

	inline bool next_token_dir_exp_nntexp(const char* &res)
	{
		return _next_token_dir<&Pp::next_base_nntexp>(res);
	}

	inline const char* include(void)
	{
		const char *n;
		if (!next_token_dir_exp(n))
			error("Expected string");
		assert_token(n);
		auto t = Token::type(n);
		if (t == Token::Type::StringLiteral)
			m_stream.push(n);
		else if (Token::is_op(n, Token::Op::Less)) {
			char stack_base[stack_size];
			auto stack = stack_base;
			*stack++ = static_cast<char>(Token::Type::StringSysInclude);
			auto s = stack++;
			auto base = stack;
			while (true) {
				if (!next_token_dir_exp(n))
					error("Expected token");
				if (Token::is_op(n, Token::Op::Greater))
					break;
				tok_str(n, stack, stack_base + stack_size, nullptr);
			}
			*s = static_cast<uint8_t>(stack - base);
			m_stream.push(stack_base);
		} else
			error("Expected string");
		return next_token();
	}

	uint32_t m_macros;

	struct TokType {	// exts
		static inline constexpr char arg = 6;	// data: arg ndx, __VA_ARGS__ is encoded as one extra arg (ndx arg_count)
		static inline constexpr char opt = 7;	// data: arg count
		static inline constexpr char spat = 8;	// data: spat count (at least 2)
		static inline constexpr char str = 9;	// no data, following is either arg or opt
		static inline constexpr char end = 10;	// no data
	};

	static inline constexpr size_t define_arg_size = 128;
	static inline constexpr const char* va_args = "\xB__VA_ARGS__";
	static inline constexpr const char* va_opt = "\xA__VA_OPT__";

	enum class TokPoll : char {
		Do = 0,
		Dont = 1,
		End = 2
	};
	TokPoll define_add_token(const char *&n, bool has_va, size_t arg_count, const char *args, const char *arg_top, size_t last);

	inline bool define_is_tok_spattable_ll(const char *token)
	{
		auto t = Token::type(token);
		return static_cast<char>(t) <= static_cast<char>(Token::Type::Identifier);
	}

	inline bool define_is_tok_spattable(const char *token)
	{
		auto t = Token::type(token);
		return define_is_tok_spattable_ll(token) ||// t == Token::Type::Operator ||
			t == static_cast<Token::Type>(TokType::arg) || t == static_cast<Token::Type>(TokType::opt);
	}

	inline const char* define(void)
	{
		const char *n;
		if (!next_token_dir(n))
			error("Expected token");
		assert_token_type(n, Token::Type::Identifier);
		token_nter(nn, n);
		size_t base_size = m_size;
		bool suc_ins = insert(m_macros, nn);
		auto name_off = m_stream.get_off();
		alloc(2);
		m_buffer[m_size++] = 0;	// regular macro
		if (next_token_dir(n)) {	// arguments or tokens
			char args[define_arg_size];
			char *arg_top = args;
			size_t arg_count = 0;
			bool has_va = false;
			bool is_dir_cont = true;
			if (m_stream.get_off() == name_off + 1 && Token::is_op(n, Token::Op::LPar)) {	// has args first
				bool expect_id = false;
				bool expect_end = false;
				while (true) {
					if (!next_token_dir(n))
						error("Expected token");
					if (Token::type(n) == Token::Type::Identifier) {
						auto size = Token::size(n);
						if (arg_top + size + 1 >= args + define_arg_size)
							error("Argument overflow");
						auto data = Token::data(n);
						for (uint8_t i = 0; i < size; i++)
							*arg_top++ = *data++;
						*arg_top++ = 0;
						arg_count++;
						if (!next_token_dir(n))
							error("Expected token");
					} else if (Token::is_op(n, Token::Op::Expand)) {
						has_va = true;
						expect_end = true;
						if (!next_token_dir(n))
							error("Expected token");
					} else if (expect_id)
						error("Expected token");
					if (Token::type(n) == Token::Type::Operator) {
						auto o = Token::op(n);
						if (o == Token::Op::Comma) {
							if (expect_end)
								error("Expected ')'");
							if (arg_count == 0)
								error("Expected arg before");
							expect_id = true;
							continue;
						} else if (o == Token::Op::RPar)
							break;
					}
					error("Expected ',' or ')'");
				}
				if (arg_count == 0 && !has_va)	// one unamed argument at beginning
					arg_count = 1;	// no first argument is indistiguishable from empty first arg
				is_dir_cont = next_token_dir(n);
			}
			m_buffer[m_size++] = arg_count | (has_va ? 0x80 : 0);
			size_t last = -1;
			if (is_dir_cont)
				while (true) {
					auto cur = m_size;
					auto p = define_add_token(n, has_va, arg_count, args, arg_top, last);
					if (p == TokPoll::End)
						break;
					if (p == TokPoll::Do)
						if (!next_token_dir(n))
							break;
					last = cur;
				}
		} else {	// zero token macro
			m_buffer[m_size++] = 0;	// zero args
		}
		alloc(1);
		m_buffer[m_size++] = TokType::end;
		if (!suc_ins) {	// assert redefinition is same
			auto size = m_size - base_size;
			auto n = &m_buffer[m_size - size];
			auto e = resolve(m_macros, nn);
			for (size_t i = 0; i < size; i++)
				if (e[i] != n[i])
					error("Redefinition do not match");
			m_size -= size;	// don't keep same buffer
		}
		return n;
	}

	// give opportunity to trim macro buffer if definition at end?
	// doesn't seem worth extra code for now and macros already fairly slim
	inline const char* undef(void)
	{
		const char *n;
		if (!next_token_dir(n))
			error("Expected token");
		assert_token_type(n, Token::Type::Identifier);
		token_nter(nn, n);
		remove(m_macros, nn);
		if (next_token_dir(n))
			error("Expected no further token");
		return n;
	}

	inline const char* derror(void)
	{
		error("#error");
		__builtin_unreachable();
	}

	static inline constexpr auto once = make_cstr("once");

	inline const char* pragma(void)
	{
		const char *n;
		if (next_token_dir(n)) {
			if (Token::type(n) == Token::Type::Identifier) {
				if (streq(n + 1, once)) {
					auto sign = m_stream.get_file_sign();
					m_stream.get_stream().insert_ponce(sign);
				}
				if (next_token_dir(n))
					error("Expected no further token");
			}
		}
		return n;
	}

	inline const char* line(void)
	{
		const char *n;
		if (!next_token_dir_exp(n))
			error("Expected token");
		if (Token::type(n) != Token::Type::NumberLiteral)
			error("Expected number");
		size_t l = 0;
		{
			auto s = Token::size(n);
			auto d = Token::data(n);
			size_t w = 1;
			for (uint8_t i = 0; i < s; i++) {
				uint8_t ir = s - 1 - i;
				auto c = d[ir];
				if (!Token::Char::is_digit(c))
					error("Expected only digits");
				l += static_cast<size_t>(static_cast<uint8_t>(c - '0')) * w;
				w *= 10;
			}
			l--;	// cancel directive linefeed
		}
		m_stream.set_line(l);
		if (!next_token_dir_exp(n))
			return n;
		if (Token::type(n) != Token::Type::StringLiteral)
			error("Expected string");
		m_stream.set_file_alias(n);
		if (next_token_dir_exp(n))
			error("Expected no further token");
		return n;
	}

	inline const char* directive(void)
	{
		const char *n;
		if (!next_token_dir(n))
			return n;
		assert_token_type(n, Token::Type::Identifier);
		token_nter(nn, n);
		const char* (Pp::*dir)(void);
		if (!resolve(m_dirs, nn, dir))
			error("Unknown directive");
		return (this->*dir)();
	}

	static inline constexpr size_t stack_size = 256;
	char m_stack_base[stack_size];
	char *m_stack;

	struct StackFrameType {
		static inline constexpr uint8_t macro = 0;
		static inline constexpr uint8_t tok = 1;
		static inline constexpr uint8_t arg = 2;
		static inline constexpr uint8_t end = 3;
	};

	void tok_skip(const char *&n);
	void tok_str(const char *&n, char *&c, const char *c_top, const char *args);

	inline bool macro(char *entry, const char *&res)
	{
		auto off = load_part<3, uint32_t>(entry);
		const char *n = m_buffer + off;
		auto t = *n;
		if (t == TokType::end) {
			res = nullptr;
			return true;
		} else if (t == TokType::arg) {
			::store_part<3>(entry, static_cast<uint32_t>(off + 2));
			auto m = static_cast<uint8_t>(n[1]);
			auto n = entry + 4;
			for (uint8_t i = 0; i < m; i++) {
				while (*n != TokType::end)
					n += Token::whole_size(n);
				n++;
			}
			if (m_stack + 5 > m_stack_base + stack_size)
				error("Macro stack overflow");
			*m_stack++ = StackFrameType::arg;
			m_stack += ::store(m_stack, static_cast<uint16_t>(n - m_stack_base));
			m_stack += ::store(m_stack, static_cast<uint16_t>(5));
			return false;
		} else if (t == TokType::opt) {
			if (entry[3])
				n += 2;
			else {
				auto m = static_cast<uint8_t>(n[1]);
				n += 2;
				for (uint8_t i = 0; i < m; i++)
					tok_skip(n);
			}
			::store_part<3>(entry, static_cast<uint32_t>(n - m_buffer));
			return false;
		} else if (t == TokType::str) {
			res = m_stack;
			auto c = m_stack;
			*c++ = static_cast<char>(Token::Type::StringLiteral);
			auto size = reinterpret_cast<uint8_t*>(c++);
			n++;
			tok_str(n, c, m_stack_base + stack_size, entry + 3);
			*size = c - m_stack - 2;
			::store_part<3>(entry, static_cast<uint32_t>(n - m_buffer));
			return true;
		} else if (t == TokType::spat) {	// hardest one, bufferize generated tokens on a separate stack then output them one by one on the lowest level
			auto m = static_cast<uint8_t>(n[1]);
			n += 2;
			::store_part<3>(entry, static_cast<uint32_t>(n - m_buffer));
			char stack_base[stack_size];
			auto stack = stack_base;
			char *last = nullptr;
			bool can_spat = false;
			auto base = m_stack;
			auto next = n;
			for (uint8_t i = 0; i < m; i++) {
				tok_skip(next);	// next is next spat arg in macro seq

				while (m_stack >= base) {	// can't poll tokens from earlier frames
					auto s = load<uint16_t>(m_stack - 2);
					if (stack_poll(m_stack - s, n)) {
						if (n == nullptr)
							m_stack -= s;
						else {
							if (can_spat && last != nullptr) {
								if (!define_is_tok_spattable_ll(n))
									error("Invalid token for spatting");
								auto s = Token::size(n);
								if (stack + s > stack_base + stack_size)
									error("Macro stack overflow");
								auto d = Token::data(n);
								for (uint8_t i = 0; i < s; i++)
									*stack++ = *d++;
								last[1] += s;
							} else {
								last = stack;
								auto s = Token::whole_size(n);
								if (stack + s > stack_base + stack_size)
									error("Macro stack overflow");
								for (uint8_t i = 0; i < s; i++)
									*stack++ = *n++;
							}
							can_spat = false;
						}
					}
					if (m_stack == base)	// do not signal current token while upper macro stack has not been consumed
						if (m_buffer + load_part<3, uint32_t>(entry) >= next)	// next spat token reached
							break;
				}
				can_spat = true;
			}
			if (stack + 1 > stack_base + stack_size)
				error("Macro stack overflow");
			*stack++ = TokType::end;
			uint16_t s = stack - stack_base;
			if (m_stack + 5 + s > m_stack_base + stack_size)
				error("Macro stack overflow");
			*m_stack++ = StackFrameType::arg;
			m_stack += ::store(m_stack, static_cast<uint16_t>(m_stack + 2 - m_stack_base));
			stack = stack_base;
			for (uint8_t i = 0; i < s; i++)
				*m_stack++ = *stack++;
			m_stack += ::store(m_stack, static_cast<uint16_t>(5 + s));
			return false;
		}
		auto s = Token::whole_size(n);
		::store_part<3>(entry, static_cast<uint32_t>(off + s));
		res = n;
		return true;
	}

	inline bool tok(char *entry, const char *&res)
	{
		if (*entry == TokType::end) {
			res = nullptr;
			return true;
		}
		*entry = TokType::end;
		res = entry + 1;
		return true;
	}

	bool m_reached_end = false;

	inline bool arg(char *entry, const char *&res)
	{
		auto off = load<uint16_t>(entry);
		auto n = m_stack_base + off;
		if (*n == TokType::end) {
			res = nullptr;
			return true;
		}
		auto s = Token::whole_size(n);
		::store(entry, static_cast<uint16_t>(off + s));
		res = n;
		return true;
	}

	inline bool end(char *entry, const char *&res)
	{
		static_cast<void>(entry);
		m_reached_end = true;
		res = nullptr;
		return true;
	}

	using stack_t = bool (Pp::*)(char *entry, const char *&res);
	static inline constexpr stack_t stacks[] = {
		&Pp::macro,	// 0
		&Pp::tok,	// 1
		&Pp::arg,	// 2
		&Pp::end	// 3
	};

	inline bool stack_poll(char *entry, const char *&res)
	{
		return (this->*stacks[*entry])(entry + 1, res);
	}

	static inline constexpr auto t_cplusplus = make_tstr(Token::Type::NumberLiteral, "202002L");
	static inline constexpr auto t_date = make_tstr(Token::Type::StringLiteral, "May 27 2000");
	static inline constexpr auto t_stdc_hosted = make_tstr(Token::Type::NumberLiteral, "0");
	static inline constexpr auto t_stdcpp_default_new_alignment = make_tstr(Token::Type::NumberLiteral, "4");
	static inline constexpr auto t_time = make_tstr(Token::Type::StringLiteral, "12:00:00");

	inline const char* i__cplusplus(void)
	{
		return t_cplusplus;
	}

	inline const char* i__DATE__(void)
	{
		return t_date;
	}

	inline const char* i__FILE__(void)
	{
		return m_stream.get_file_path();
	}

	inline const char* i__LINE__(void)
	{
		if (m_stack + 2 > m_stack_base + stack_size)
			error("Macro stack overflow");
		auto res = m_stack;
		auto n = res;
		*n++ = static_cast<char>(Token::Type::NumberLiteral);
		auto &s = reinterpret_cast<uint8_t&>(*n++);
		s = 0;
		auto base = n;
		auto l = m_stream.get_row();
		while (l > 0) {
			s++;
			if (m_stack + 2 + s > m_stack_base + stack_size)
				error("Macro stack overflow");
			*n++ = static_cast<char>(l % 10) + '0';
			l /= 10;
		}
		auto h = s / 2;
		for (uint8_t i = 0; i < h; i++) {
			auto ir = s - 1 - i;
			auto tmp = base[i];
			base[i] = base[ir];
			base[ir] = tmp;
		}
		return res;
	}

	inline const char* i__STDC_HOSTED__(void)
	{
		return t_stdc_hosted;
	}

	inline const char* i__STDCPP_DEFAULT_NEW_ALIGNMENT__(void)
	{
		return t_stdcpp_default_new_alignment;
	}

	inline const char* i__TIME__(void)
	{
		return t_time;
	}

	using pmac_t = const char* (Pp::*)(void);
	static inline constexpr pmac_t pmacros[1 + pmacro_count] = {
		nullptr,		// regular macro
		&Pp::i__cplusplus,	// 0
		&Pp::i__DATE__,		// 1
		&Pp::i__FILE__,		// 2
		&Pp::i__LINE__,		// 3
		&Pp::i__STDC_HOSTED__,		// 4
		&Pp::i__STDCPP_DEFAULT_NEW_ALIGNMENT__,		// 5
		&Pp::i__TIME__		// 6
	};

	inline const char* next_stack(void)
	{
		const char *n;
		if (m_stack > m_stack_base) {
			n = nullptr;
			do {
				auto s = load<uint16_t>(m_stack - 2);
				if (stack_poll(m_stack - s, n)) {
					if (n == nullptr)
						m_stack -= s;
					else
						break;
				}
			} while (m_stack > m_stack_base);
			if (n == nullptr && !m_reached_end)
				n = next_token();
		} else
			n = next_token();
		return n;
	}
	const char* next_base_nntexp(void);
	const char* next_base(void);

	inline void push_stream_token(const char *n)
	{
		if (n == nullptr) {
			if (m_stack + 3 > m_stack_base + stack_size)
				error("Macro stack overflow");
			*m_stack++ = StackFrameType::end;
			m_stack += ::store(m_stack, static_cast<uint16_t>(3));
		} else {
			token_copy(nc, n);	// copy token before pushing it to stack (might be located on top)
			n = nc;
			if (m_stack + 4 + sizeof(nc) > m_stack_base + stack_size)
				error("Macro stack overflow");
			*m_stack++ = StackFrameType::tok;
			*m_stack++ = 0;	// is TokType::end when already substitued
			for (uint8_t i = 0; i < sizeof(nc); i++)
				*m_stack++ = *n++;
			m_stack += ::store(m_stack, static_cast<uint16_t>(4 + sizeof(nc)));
		}
	}

	size_t m_cond_level = 0;

	struct Val
	{
		uint32_t v;
		bool is_s;
	};

	Val parse_nlit(const char *n)
	{
		uint32_t v = 0;
		auto s = Token::size(n);
		auto d = Token::data(n);
		uint8_t i = 0;
		auto c = d[i++];
		if (c == '0') {
			if (s >= 2) {
				c = Token::Char::lower(d[i]);
				if (c == 'b') {
					i++;
					while (i < s) {
						auto c = d[i];
						if (!(c >= '0' && c <= '1'))
							break;
						i++;
						v = v * 2 + static_cast<uint32_t>(static_cast<uint8_t>(c - '0'));
					}
					if (i == 2)
						error("Expected at least one digit");
					goto polled_digits;
				} else if (c == 'x') {
					i++;
					while (i < s) {
						auto c = Token::Char::lower(d[i]);
						if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f')))
							break;
						i++;
						uint8_t d;
						if (c >= 'a' && c <= 'f')
							d = c - 'a' + 10;
						else
							d = c - '0';
						v = v * 16 + static_cast<uint32_t>(d);
					}
					if (i == 2)
						error("Expected at least one digit");
					goto polled_digits;
				}
			}
			if (s >= 1) {
				while (i < s) {
					auto c = d[i];
					if (!(c >= '0' && c <= '7'))
						break;
					i++;
					v = v * 8 + static_cast<uint32_t>(static_cast<uint8_t>(c - '0'));
				}
				if (i == 0)
					error("Expected at least one digit");
				goto polled_digits;
			}
		}
		i = 0;
		while (i < s) {
			auto c = d[i];
			if (!(c >= '0' && c <= '9'))
				break;
			i++;
			v = v * 10 + static_cast<uint32_t>(static_cast<uint8_t>(c - '0'));
		}
		if (i == 0)
			error("Expected at least one digit");
		polled_digits:;
		auto left = static_cast<uint8_t>(s - i);
		Val r{v, true};
		if (left == 0)
			return r;
		if (left == 1) {
			if (Token::Char::lower(d[i]) == 'l')
				return r;
			if (Token::Char::lower(d[i]) == 'u') {
				r.is_s = false;
				return r;
			}
		}
		if (left == 2) {
			if (Token::Char::lower(d[i]) == 'u' && Token::Char::lower(d[i + 1]) == 'l') {
				r.is_s = false;
				return r;
			}
		}
		error("Bad end of literal");
		__builtin_unreachable();
	}

	struct OpDesc
	{
		Token::Op op;
		uint8_t prec;
		using Comp = Val (Val a, Val b, Val payload);
		Comp *comp;
	};

	#define PP_IMPL_OP(id, op) \
	static inline Val op_##id(Val a, Val b, Val)						\
	{												\
		if (!a.is_s || !b.is_s)									\
			return Val{a.v op b.v, false};							\
		else											\
			return Val{static_cast<uint32_t>(static_cast<int32_t>(a.v) op static_cast<int32_t>(b.v)), true};	\
	}
	PP_IMPL_OP(mul, *)
	PP_IMPL_OP(div, /)
	PP_IMPL_OP(mod, %)

	PP_IMPL_OP(plus, +)
	PP_IMPL_OP(minus, -)

	PP_IMPL_OP(bit_left, <<)
	PP_IMPL_OP(bit_right, >>)

	PP_IMPL_OP(less, <)
	PP_IMPL_OP(less_equal, <=)
	PP_IMPL_OP(greater, >)
	PP_IMPL_OP(greater_equal, >=)

	PP_IMPL_OP(equal_equal, ==)
	PP_IMPL_OP(not_equal, !=)

	PP_IMPL_OP(bit_and, &)

	PP_IMPL_OP(bit_xor, ^)

	PP_IMPL_OP(bit_or, |)

	PP_IMPL_OP(and, &&)
	PP_IMPL_OP(or, ||)
	#undef PP_IMPL_OP

	static inline Val op_huh(Val a, Val b, Val payload)
	{
		if (!payload.v || !b.v) {
			payload.is_s = false;
			b.is_s = false;
		}
		if (a.v)
			return payload;
		else
			return b;
	}

	static inline constexpr OpDesc op_descs[] = {
		{Token::Op::Mul, 3, op_mul},
		{Token::Op::Div, 3, op_div},
		{Token::Op::Mod, 3, op_mod},

		{Token::Op::Plus, 4, op_plus},
		{Token::Op::Minus, 4, op_minus},

		{Token::Op::BitLeft, 5, op_bit_left},
		{Token::Op::BitRight, 5, op_bit_right},

		{Token::Op::Less, 6, op_less},
		{Token::Op::LessEqual, 6, op_less_equal},
		{Token::Op::Greater, 6, op_greater},
		{Token::Op::GreaterEqual, 6, op_greater_equal},

		{Token::Op::EqualEqual, 7, op_equal_equal},
		{Token::Op::NotEqual, 7, op_not_equal},

		{Token::Op::BitAnd, 8, op_bit_and},

		{Token::Op::BitXor, 9, op_bit_xor},

		{Token::Op::BitOr, 10, op_bit_or},

		{Token::Op::And, 11, op_and},

		{Token::Op::Or, 12, op_or},

		{Token::Op::Huh, 13, op_huh}
	};

	// assumes n should be skipped, exits on end or unprocessable op
	inline Val gval_b(const char *&n, bool &has_some)
	{
		auto v = gval_s(n, has_some);
		if (!has_some)
			return v;
		if (!next_token_dir_exp(n)) {
			has_some = false;
			return v;
		}
		return gval(n, has_some, v);
	}

	static inline constexpr auto tdefined = make_cstr("defined");
	static inline constexpr auto ttrue = make_cstr("true");
	//static inline constexpr auto tfalse = make_cstr("false");

	// assumes n should be skipped, exits on parsed token that should be skipped
	inline Val gval_s(const char *&n, bool &has_some)
	{
		if (!has_some)
			error("Expected token");
		if (!next_token_dir_exp(n))
			error("Expected token");
		auto t = Token::type(n);
		if (t == Token::Type::Operator) {
			auto o = Token::op(n);
			if (o == Token::Op::LPar) {
				auto r = gval_b(n, has_some);
				if (!has_some)
					error("Expected token");
				if (!Token::is_op(n, Token::Op::RPar))
					error("Expected )");
				return r;
			} else if (o == Token::Op::Plus) {
				return gval_s(n, has_some);
			} else if (o == Token::Op::Minus) {
				auto r = gval_s(n, has_some);
				r.v = -r.v;
				return r;
			} else if (o == Token::Op::Not) {
				auto r = gval_s(n, has_some);
				r.v = !r.v;
				return r;
			} else if (o == Token::Op::Tilde) {
				auto r = gval_s(n, has_some);
				r.v = ~r.v;
				return r;
			}
		} else if (t == Token::Type::NumberLiteral)
			return parse_nlit(n);
		else if (t == Token::Type::ValueChar8)
			return Val{static_cast<uint32_t>(static_cast<int32_t>(n[1])), true};
		else if (t == Token::Type::Identifier) {
			if (streq(n + 1, tdefined)) {
				if (!next_token_dir_exp_nntexp(n))
					error("Expected token");
				bool has_par = false;
				if (Token::is_op(n, Token::Op::LPar)) {
					has_par = true;
					if (!next_token_dir_exp_nntexp(n))
						error("Expected token");
				}
				assert_token_type(n, Token::Type::Identifier);
				token_nter(nn, n);
				auto r = Val{static_cast<bool>(resolve(m_macros, nn)), true};
				if (has_par) {
					if (!next_token_dir_exp_nntexp(n))
						error("Expected )");
					if (!Token::is_op(n, Token::Op::RPar))
						error("Expected )");
				}
				return r;
			} else if (streq(n + 1, ttrue)) {
				return Val{1, true};
			}
			return Val{0, true};
		}
		error("Bad token");
		__builtin_unreachable();
	}

	inline bool find_od(Token::Op op, OpDesc &od)
	{
		for (size_t i = 0; i < array_size(op_descs); i++)
			if (op_descs[i].op == op) {
				od = op_descs[i];
				return true;
			}
		return false;
	}

	inline bool find_od(const char *&n, OpDesc &od)
	{
		auto t = Token::type(n);
		if (t == Token::Type::Operator)
			return find_od(Token::op(n), od);
		return false;
	}

	// assumes n on end or op to parse, exits on end or unprocessable op
	inline Val gval(const char *&n, bool &has_some, Val v, uint8_t prec = 255)
	{
		OpDesc o;
		while (has_some && find_od(n, o)) {
			if (o.prec > prec)
				break;
			Val payload;
			if (o.op == Token::Op::Huh) {
				payload = gval_b(n, has_some);
				if (!has_some)
					error("Expected :");
				if (!Token::is_op(n, Token::Op::Colon))
					error("Expected :");
			}
			auto b = gval_s(n, has_some);
			if (!has_some)
				error("Expected token");
			if (!next_token_dir_exp(n))
				has_some = false;
			while (has_some) {
				OpDesc o2;
				if (!find_od(n, o2))
					break;
				if (o2.op == Token::Op::Huh) {	// right-associative
					if (o2.prec != o.prec)
						break;
				} else {
					if (o2.prec >= o.prec)
						break;
				}
				b = gval(n, has_some, b, o.prec);
			}
			v = o.comp(v, b, payload);
		}
		return v;
	}

	bool eval(const char *&n)
	{
		bool has_some = true;
		auto v = gval_b(n, has_some);
		if (has_some)
			error("Expected no further token");
		return v.v != 0;
	}

	struct FindBlockBan {
		static inline constexpr size_t none = 0;
		static inline constexpr size_t elif = 1;
		static inline constexpr size_t delse = 2;
	};

	inline const char* dfind_block(size_t ban, const char *n)
	{
		size_t depth = 0;
		while (true) {
			if (n == nullptr)
				error("Expected token");
			if (Token::is_op(n, Token::Op::Sharp)) {
				if (next_token_dir(n)) {
					const char* (Pp::*dir)(void);
					{
						token_nter(nn, n);
						if (!resolve(m_dirs, nn, dir))
							error("Unknown directive");
					}
					if (dir == &Pp::ifdef || dir == &Pp::ifndef || dir == &Pp::dif)
						depth++;
					else if (dir == &Pp::elif) {
						if (depth == 0) {
							if (ban == FindBlockBan::delse)
								error("#elif after #else");
							if (ban == FindBlockBan::none) {
								if (eval(n))
									return n;
								goto after_polling;
							}
						}
					} else if (dir == &Pp::delse) {
						if (depth == 0) {
							if (ban == FindBlockBan::delse)
								error("#else after #else");
							if (ban == FindBlockBan::none)
								break;
						}
					} else if (dir == &Pp::endif) {
						if (depth == 0)
							break;
						depth--;
					}
					while (next_token_dir(n));
					goto after_polling;
				}
			}
			n = next_token();
			after_polling:;
		}
		if (next_token_dir(n))
			error("Expected no further token");
		return n;
	}

	inline const char* dif_find_block(bool match, const char *n)
	{
		m_cond_level++;
		if (!match)
			n = dfind_block(FindBlockBan::none, n);
		return n;
	}

	inline const char* dif(void)
	{
		const char *n;
		auto m = eval(n);
		return dif_find_block(m, n);
	}

	inline const char* ifdef_gen(bool seek_undef)
	{
		const char *n;
		if (!next_token_dir(n))
			error("Expected token");
		if (Token::type(n) != Token::Type::Identifier)
			error("Expected identifier");
		bool match;
		{
			token_nter(nn, n);
			match = static_cast<bool>(resolve(m_macros, nn)) ^ seek_undef;
		}
		if (next_token_dir(n))
			error("Expected no further token");
		return dif_find_block(match, n);
	}

	inline const char* ifdef(void)
	{
		return ifdef_gen(false);
	}

	inline const char* ifndef(void)
	{
		return ifdef_gen(true);
	}

	inline const char* elif(void)
	{
		const char *n;
		while (next_token_dir(n));
		return dfind_block(FindBlockBan::elif, n);
	}

	// we reached end of if or elif block
	inline const char* delse(void)
	{
		const char *n;
		if (next_token_dir(n))
			error("Expected no further token");
		return dfind_block(FindBlockBan::delse, n);
	}

	inline const char* endif(void)
	{
		const char *n;
		if (next_token_dir(n))
			error("Expected no further token");
		if (m_cond_level == 0)
			error("Uncoherent");
		m_cond_level--;
		return n;
	}

public:
	const char* next(void);

	size_t get_count(void) const
	{
		return m_size;
	}
};