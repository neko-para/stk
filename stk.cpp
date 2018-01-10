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
	virtual long Eval() const = 0;
	virtual ExprId Id() const = 0;
};

vector<shared_ptr<ExprBase>> exprs;

struct UnaryExprBase : public ExprBase {
	long exp;
	UnaryExprBase(long e) : exp(e) {}
};

struct BinaryExprBase : public ExprBase {
	long exp1, exp2;
	BinaryExprBase(long e1, long e2) : exp1(e1), exp2(e2) {}
};

struct ExprImm : public UnaryExprBase {
	using UnaryExprBase::UnaryExprBase;
	virtual long Eval() const override final {
		return exp;
	}
	virtual ExprId Id() const override final {
		return ExprId::IMM;
	}
};

struct ExprNeg : public UnaryExprBase {
	using UnaryExprBase::UnaryExprBase;
	virtual long Eval() const override final {
		return -exprs[exp]->Eval();
	}
	virtual ExprId Id() const override final {
		return ExprId::NEG;
	}
};

struct ExprNot : public UnaryExprBase {
	using UnaryExprBase::UnaryExprBase;
	virtual long Eval() const override final {
		return ~exprs[exp]->Eval();
	}
	virtual ExprId Id() const override final {
		return ExprId::NOT;
	}
};

struct ExprDrf : public UnaryExprBase {
	using UnaryExprBase::UnaryExprBase;
	virtual long Eval() const override final {
		return *at(exprs[exp]->Eval());
	}
	virtual ExprId Id() const override final {
		return ExprId::DRF;
	}
};

struct ExprDlb : public UnaryExprBase {
	using UnaryExprBase::UnaryExprBase;
	virtual long Eval() const override final {
		return where(exp);
	}
	virtual ExprId Id() const override final {
		return ExprId::DLB;
	}
};

#define BinaryExpr(name, op, id) \
	struct Expr##name : public BinaryExprBase { \
		using BinaryExprBase::BinaryExprBase; \
		virtual long Eval() const override final { \
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
	long lib, cmd;
	long p1, p2;
};

typedef void (*stkProc)(long p1, long p2);

vector<long> stk;
unordered_map<long, unordered_map<long, stkProc>> proctbl;
unordered_map<string, long> stridx;
vector<string> strtbl;
unordered_map<long, long> lbltbl;
unordered_map<long, void*> loaded;
vector<Cmd> cmd;
vector<long> use;
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

void Load(long p1, long) {
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

void Using(long p1, long) {
	use.push_back(p1);
}

void init() {
	proctbl[getWord("__global__")][getWord("load")] = Load;
	proctbl[getWord("__global__")][getWord("using")] = Using;
	use.push_back(getWord("__global__"));
	exprs.emplace_back(new ExprImm(0));
	strtbl.emplace_back("");
	stridx[""] = 0;
}

void push(long v) {
	stk.push_back(v);
}

void pop() {
	stk.pop_back();
}

long size() {
	return stk.size();
}

long* at(long x) {
	return &stk[x < 0 ? x + stk.size() : x];
}

static const char* actstr[] = {
	"Imm", "Add", "Sub", "Mul", "Neg", "Drf", "Dlb"
};

long eval(long x) {
	return exprs[x]->Eval();
}

long where(long s) {
	return lbltbl[s];
}

const char* tostr(long s) {
	return strtbl[s].c_str();
}

void pushFile(FILE* file) {
	files.push_back(file);
}

long popFile(FILE** file) {
	if (files.size()) {
		*file = files.back();
		files.pop_back();
		return 0;
	} else {
		return 1;
	}
}

long getWord(const char* str) {
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

inline long parsex(char c) {
	return isdigit(c) ? (c ^ '0') : (toupper(c) - 'A' + 10);
}

long parseWord(const char* str) {
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

#define UnaryGet(name) \
	long get##name(long p) { \
		exprs.emplace_back(new Expr##name(p)); \
		return exprs.size() - 1; \
	}

#define BinaryGet(name) \
	long get##name(long p1, long p2) { \
		exprs.emplace_back(new Expr##name(p1, p2)); \
		return exprs.size() - 1; \
	}

UnaryGet(Imm)
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

void Label(long l) {
	lbltbl[l] = cmd.size() - 1;
}

void Inst(long lb, long is) {
	cmd.push_back((Cmd){
		lb, is, 0, 0
	});
}

void Inst1(long lb, long is, long p1) {
	cmd.push_back((Cmd){
		lb, is, p1, 0
	});
}

void Inst2(long lb, long is, long p1, long p2) {
	cmd.push_back((Cmd){
		lb, is, p1, p2
	});
}

stkProc getProc(long cmd) {
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
		cerr << cmd[stk[0]] << endl;
		const Cmd& c = cmd[stk[0]];
		if (c.lib) {
			proctbl[c.lib][c.cmd](c.p1, c.p2);
		} else {
			getProc(c.cmd)(c.p1, c.p2);
		}
		++stk[0];
	}
}
