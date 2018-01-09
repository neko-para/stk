#include "stk.h"
#include "stk.sub.h"
#include <bits/stdc++.h>
#include <dlfcn.h>
using namespace std;

int rescnt;

struct Act {
	enum {
		IMM,
		ADD,
		SUB,
		MUL,
		DIV,
		MOD,
		NEG,
		DRF,
		DLB
	} act;
	int x1, x2;
};

struct Cmd {
	int cmd;
	int p1, p2;
};

typedef void (*stkProc)(int p1, int p2);

vector<int> stk;
unordered_map<int, stkProc> proctbl;
unordered_map<string, int> stridx;
unordered_map<int, string> strtbl;
unordered_map<int, Act> acttbl;
unordered_map<int, int> lbltbl;
unordered_map<int, void*> loaded;
vector<Cmd> cmd;

#ifdef WIN32
#define SYMPRF "_"
#else
#define SYMPRF ""
#endif

void* procs[] = {
	(void*)push, (void*)pop, (void*)size, (void*)at, (void*)eval, (void*)where
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
			proctbl[getWord((string(name) + "." + node[i].name).c_str())] = node[i].proc;
		}
	}
}

void init() {
	proctbl[getWord("load")] = Load;
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

void Inst(int is) {
	cmd.push_back((Cmd){
		is, 0, 0
	});
}

void Inst1(int is, int p1) {
	cmd.push_back((Cmd){
		is, p1, 0
	});
}

void Inst2(int is, int p1, int p2) {
	cmd.push_back((Cmd){
		is, p1, p2
	});
}

void run() {
	stk.push_back(0); // EIP
	stk.push_back(0); // EAX
	while (stk[0] != cmd.size()) {
		const Cmd& c = cmd[stk[0]];
		cerr << stk[0] << endl;
		proctbl[c.cmd](c.p1, c.p2);
		++stk[0];
	}
}
