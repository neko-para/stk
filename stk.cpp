#include "stk.h"
#include "stk.sub.h"
#include <bits/stdc++.h>
#include <dlfcn.h>
using namespace std;

int rescnt = 1;

struct Act {
	enum {
		IMM, ADD, SUB, MUL, DIV, MOD, NEG, DRF, DLB
	} act;
	int x1, x2;
};

struct Cmd {
	int lib, cmd;
	int p1, p2;
};

typedef void (*stkProc)(int p1, int p2);

vector<int> stk;
unordered_map<int, unordered_map<int, stkProc>> proctbl;
unordered_map<string, int> stridx;
unordered_map<int, string> strtbl;
unordered_map<int, Act> acttbl;
unordered_map<int, int> lbltbl;
unordered_map<int, void*> loaded;
vector<Cmd> cmd;
vector<int> use;

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

void Load(int p1, int) {
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

void Using(int p1, int) {
	use.push_back(p1);
}

void init() {
	proctbl[getWord("__global__")][getWord("load")] = Load;
	proctbl[getWord("__global__")][getWord("using")] = Using;
	use.push_back(getWord("__global__"));
}

void push(int v) {
	stk.push_back(v);
}

void pop() {
	stk.pop_back();
}

int size() {
	return stk.size();
}

int* at(int x) {
	return &stk[x < 0 ? x + stk.size() : x];
}

static const char* actstr[] = {
	"Imm", "Add", "Sub", "Mul", "Neg", "Drf", "Dlb"
};

int eval(int x) {
	const Act& is = acttbl[x];
	//cerr << "do " << actstr[is.act] << " with " << is.x1 << ' ' << is.x2 << endl;
	switch(is.act) {
	case Act::IMM:
		return is.x1;
	case Act::ADD:
		return eval(is.x1) + eval(is.x2);
	case Act::SUB:
		return eval(is.x1) - eval(is.x2);
	case Act::MUL:
		return eval(is.x1) * eval(is.x2);
	case Act::DIV:
		return eval(is.x1) / eval(is.x2);
	case Act::MOD:
		return eval(is.x1) % eval(is.x2);
	case Act::NEG:
		return -eval(is.x1);
	case Act::DRF:
		return *at(eval(is.x1));
	case Act::DLB:
		return where(is.x1);
	}
}

int where(int s) {
	return lbltbl[s];
}

const char* tostr(int s) {
	return strtbl[s].c_str();
}

int getWord(const char* str) {
	auto it = stridx.find(str);
	if (it != stridx.end()) {
		return it->second;
	} else {
		strtbl[rescnt] = str;
		stridx[str] = rescnt;
		return rescnt++;
	}
}

inline bool isodigit(char c) {
	return unsigned(c - '0') < 8;
}

inline int parsex(char c) {
	return isdigit(c) ? (c ^ '0') : (toupper(c) - 'A' + 10);
}

int parseWord(const char* str) {
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

int getImm(int num) {
	acttbl[rescnt] = (Act){
		Act::IMM,
		num, 0
	};
	return rescnt++;
}

int getAdd(int p1, int p2) {
	acttbl[rescnt] = (Act){
		Act::ADD,
		p1, p2
	};
	return rescnt++;
}

int getSub(int p1, int p2) {
	acttbl[rescnt] = (Act){
		Act::SUB,
		p1, p2
	};
	return rescnt++;
}

int getMul(int p1, int p2) {
	acttbl[rescnt] = (Act){
		Act::MUL,
		p1, p2
	};
	return rescnt++;
}

int getDiv(int p1, int p2) {
	acttbl[rescnt] = (Act){
		Act::DIV,
		p1, p2
	};
	return rescnt++;
}

int getMod(int p1, int p2) {
	acttbl[rescnt] = (Act){
		Act::MOD,
		p1, p2
	};
	return rescnt++;
}

int getNeg(int p1) {
	acttbl[rescnt] = (Act){
		Act::NEG,
		p1, 0
	};
	return rescnt++;
}

int getDrf(int p1) {
	acttbl[rescnt] = (Act){
		Act::DRF,
		p1, 0
	};
	return rescnt++;
}

int getDlb(int p1) {
	acttbl[rescnt] = (Act){
		Act::DLB,
		p1, 0
	};
	return rescnt++;
}

void Label(int l) {
	lbltbl[l] = cmd.size() - 1;
}

void Inst(int lb, int is) {
	cmd.push_back((Cmd){
		lb, is, 0, 0
	});
}

void Inst1(int lb, int is, int p1) {
	cmd.push_back((Cmd){
		lb, is, p1, 0
	});
}

void Inst2(int lb, int is, int p1, int p2) {
	cmd.push_back((Cmd){
		lb, is, p1, p2
	});
}

stkProc getProc(int cmd) {
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
		const Cmd& c = cmd[stk[0]];
		if (c.lib) {
			proctbl[c.lib][c.cmd](c.p1, c.p2);
		} else {
			getProc(c.cmd)(c.p1, c.p2);
		}
		++stk[0];
	}
}
