#include "stk.h"
#include "stk.sub.h"
#include <bits/stdc++.h>
#include <dlfcn.h>
using namespace std;

enum class ExprId : unsigned char {
	IMM,
	ADD, SUB, MUL, DIV, MOD, NEG,
	AND, OR, XOR, SHL, SHR, NOT,
	DRF, DLB
};

struct ExprBase {
	virtual ~ExprBase() {}
	virtual stkval Eval() const = 0;
	virtual ExprId Id() const = 0;
};

vector<shared_ptr<ExprBase>> exprs;

struct UnaryExprBase : public ExprBase {
	stkhdl exp;
	UnaryExprBase(stkhdl e) : exp(e) {}
};

struct BinaryExprBase : public ExprBase {
	stkhdl exp1, exp2;
	BinaryExprBase(stkhdl e1, stkhdl e2) : exp1(e1), exp2(e2) {}
};

struct ExprImm : public UnaryExprBase {
	using UnaryExprBase::UnaryExprBase;
	virtual stkval Eval() const override final {
		return exp;
	}
	virtual ExprId Id() const override final {
		return ExprId::IMM;
	}
};

struct ExprNeg : public UnaryExprBase {
	using UnaryExprBase::UnaryExprBase;
	virtual stkval Eval() const override final {
		return -exprs[exp]->Eval();
	}
	virtual ExprId Id() const override final {
		return ExprId::NEG;
	}
};

struct ExprNot : public UnaryExprBase {
	using UnaryExprBase::UnaryExprBase;
	virtual stkval Eval() const override final {
		return ~exprs[exp]->Eval();
	}
	virtual ExprId Id() const override final {
		return ExprId::NOT;
	}
};

struct ExprDrf : public UnaryExprBase {
	using UnaryExprBase::UnaryExprBase;
	virtual stkval Eval() const override final {
		return *at(exprs[exp]->Eval());
	}
	virtual ExprId Id() const override final {
		return ExprId::DRF;
	}
};

struct ExprDlb : public UnaryExprBase {
	using UnaryExprBase::UnaryExprBase;
	virtual stkval Eval() const override final {
		return where(exp);
	}
	virtual ExprId Id() const override final {
		return ExprId::DLB;
	}
};

#define BinaryExpr(name, op, id) \
	struct Expr##name : public BinaryExprBase { \
		using BinaryExprBase::BinaryExprBase; \
		virtual stkval Eval() const override final { \
			return exprs[exp1]->Eval() op exprs[exp2]->Eval(); \
		} \
		virtual ExprId Id() const override final { \
			return ExprId::id; \
		} \
	};

BinaryExpr(Add, +, ADD)
BinaryExpr(Sub, -, SUB)
BinaryExpr(Mul, *, MUL)
BinaryExpr(Div, /, DIV)
BinaryExpr(Mod, %, MOD)
BinaryExpr(And, &, AND)
BinaryExpr(Or, |, OR)
BinaryExpr(Xor, ^, XOR)
BinaryExpr(Shl, <<, SHL)
BinaryExpr(Shr, >>, SHR)

struct Cmd {
	unsigned char flag;
#ifdef __x86_64__
	stkhdl lib : 56;
#else
	stkhdl lib : 24;
#endif
	stkhdl cmd;
	stkhdl p1, p2;
	Cmd(stkhdl l, stkhdl c) : flag(0), lib(l), cmd(c), p1(0), p2(0) {}
	Cmd(stkhdl l, stkhdl c, stkhdl p) : flag(1), lib(l), cmd(c), p1(p), p2(0) {}
	Cmd(stkhdl l, stkhdl c, stkhdl p, stkhdl q) : flag(2), lib(l), cmd(c), p1(p), p2(q) {}
	void write(ostream& os) const {
		os.write((const char*)this, 4 * sizeof(stkhdl));
	}
	void read(istream& os) {
		os.read((char*)this, 4 * sizeof(stkhdl));
	}
};

typedef void (*stkProc)(stkhdl p1, stkhdl p2);

vector<stkval> stk;
unordered_map<stkhdl, unordered_map<stkhdl, stkProc>> proctbl;
unordered_map<string, stkhdl> stridx;
vector<string> strtbl;
unordered_map<stkhdl, stkval> lbltbl;
unordered_map<stkhdl, void*> loaded;
vector<Cmd> cmd;
vector<stkhdl> use;
vector<FILE*> files;

ostream& operator<<(ostream& os, const Cmd& c) {
	os << strtbl[c.lib] << '.' << strtbl[c.cmd];
	if (c.p1) {
		os << ' ' << eval(c.p1);
		if (c.p2) {
			os << ' ' << eval(c.p2);
		}
	}
	return os;
}

#ifdef WIN32
#define SYMPRF "_"
#define LIBSUF ".dll"
#else
#define SYMPRF ""
#define LIBSUF ".so"
#endif

void* procs[] = {
	(void*)push, (void*)pop, (void*)size, (void*)at, (void*)eval, (void*)where, (void*)tostr
};

void Load(stkhdl p1, stkhdl) {
	if (loaded.find(p1) != loaded.end()) {
		return;
	} else {
		unsigned cnt;
		const char* name;
		loaded[p1] = dlopen(strtbl[p1].c_str(), RTLD_LAZY);
		const proc_node* node = ((stkInitProc)dlsym(loaded[p1], SYMPRF "__stk_init"))(procs, &cnt, &name);
		for (unsigned i = 0; i < cnt; ++i) {
			proctbl[getWord(name)][getWord(node[i].name)] = node[i].proc;
		}
	}
}

void Using(stkhdl p1, stkhdl) {
	use.push_back(p1);
}

void init() {
	exprs.emplace_back(new ExprImm(0));
	strtbl.emplace_back("");
	stridx[""] = 0;
	proctbl[getWord("__global__")][getWord("load")] = Load;
	proctbl[getWord("__global__")][getWord("using")] = Using;
	use.push_back(getWord("__global__"));
}

void push(stkval v) {
	stk.push_back(v);
}

void pop() {
	stk.pop_back();
}

stkval size() {
	return stk.size();
}

stkval* at(stkval x) {
	return &stk[x < 0 ? x + stk.size() : x];
}

static const char* actstr[] = {
	"Imm", "Add", "Sub", "Mul", "Neg", "Drf", "Dlb"
};

stkval eval(stkhdl x) {
	return exprs[x]->Eval();
}

stkval where(stkhdl s) {
	return lbltbl[s];
}

const char* tostr(stkhdl s) {
	return strtbl[s].c_str();
}

void pushFile(FILE* file) {
	files.push_back(file);
}

int popFile(FILE** file) {
	if (files.size()) {
		*file = files.back();
		files.pop_back();
		return 0;
	} else {
		return 1;
	}
}

stkhdl getWord(const char* str) {
	auto it = stridx.find(str);
	if (it != stridx.end()) {
		return it->second;
	} else {
		strtbl.push_back(str);
		return stridx[str] = strtbl.size() - 1;
	}
}

inline bool isodigit(char c) {
	return unsigned(c - '0') < 8;
}

inline stkhdl parsex(char c) {
	return isdigit(c) ? (c ^ '0') : (toupper(c) - 'A' + 10);
}

stkhdl parseWord(const char* str) {
	string s = str + 1;
	s.pop_back();
	string r;
	const char* beg = s.c_str();
	const char* end = beg + s.length();
	for (auto i = beg; i != end; ++i) {
		if (*i == '\\') {
			++i;
			switch(*i) {
				case '\n':
					break;
				case '"':
				case '\\':
					r.push_back(*i);
					break;
				case 'a':
					r.push_back('\a');
					break;
				case 'b':
					r.push_back('\b');
					break;
				case 'e':
					r.push_back('\033');
					break;
				case 'f':
					r.push_back('\f');
					break;
				case 'n':
					r.push_back('\n');
					break;
				case 'r':
					r.push_back('\r');
					break;
				case 't':
					r.push_back('\t');
					break;
				case 'v':
					r.push_back('\v');
					break;
				case 'x': {
					if (isxdigit(i[2])) {
						r.push_back((parsex(i[1]) << 4) | parsex(i[2]));
						++i;
					} else {
						r.push_back(parsex(i[1]));
					}
					++i;
					break;
				}
				default: {
					if (isodigit(i[2])) {
						if (isodigit(i[3])) {
							r.push_back(((i[1] ^ '0') * 10 + (i[2] ^ '0')) * 10 + (i[3] ^ '0'));
							++i;
						} else {
							r.push_back((i[1] ^ '0') * 10 + (i[2] ^ '0'));
						}
						++i;
					} else {
						r.push_back(i[1] ^ '0');
					}
					++i;
					break;
				}
			}
		} else {
			r.push_back(*i);
		}
	}
	return getWord(r.c_str());
}

stkhdl getImm(stkval p) {
	exprs.emplace_back(new ExprImm(p));
	return exprs.size() - 1;
}

#define UnaryGet(name) \
	stkhdl get##name(stkhdl p) { \
		exprs.emplace_back(new Expr##name(p)); \
		return exprs.size() - 1; \
	}

#define BinaryGet(name) \
	stkhdl get##name(stkhdl p1, stkhdl p2) { \
		exprs.emplace_back(new Expr##name(p1, p2)); \
		return exprs.size() - 1; \
	}

BinaryGet(Add)
BinaryGet(Sub)
BinaryGet(Mul)
BinaryGet(Div)
BinaryGet(Mod)
UnaryGet(Neg)
BinaryGet(And)
BinaryGet(Or)
BinaryGet(Xor)
BinaryGet(Shl)
BinaryGet(Shr)
UnaryGet(Not)
UnaryGet(Drf)
UnaryGet(Dlb)

void Label(stkhdl l) {
	lbltbl[l] = cmd.size() - 1;
}

void Inst(stkhdl lb, stkhdl is) {
	cmd.emplace_back(lb, is);
}

void Inst1(stkhdl lb, stkhdl is, stkhdl p1) {
	cmd.emplace_back(lb, is, p1);
}

void Inst2(stkhdl lb, stkhdl is, stkhdl p1, stkhdl p2) {
	cmd.emplace_back(lb, is, p1, p2);
}

stkProc getProc(stkhdl cmd) {
	for (auto i : use) {
		if (proctbl[i].find(cmd) != proctbl[i].end()) {
			return proctbl[i][cmd];
		}
	}
	return 0;
}

void run() {
	stk.push_back(0); // EIP
	stk.push_back(0); // EAX
	while (stk[0] != cmd.size()) {
		// cerr << cmd[stk[0]] << endl;
		const Cmd& c = cmd[stk[0]];
		if (c.lib) {
			proctbl[c.lib][c.cmd](c.p1, c.p2);
		} else {
			getProc(c.cmd)(c.p1, c.p2);
		}
		++stk[0];
	}
}
