// CryptoLeq Enhanced Assembler
// New York University at Abu Dhabi, MoMa Lab
// Copyright 2014-2015
// Author: Oleg Mazonka
// File: nodes.h

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <set>

#include "../unumber/unumberg.h"
#include "../processor/compiler.h"

using std::string;

struct Node;
struct Root;
struct Litem;
struct Token;
using Pnode = std::shared_ptr<Node>;
using Tokens = std::vector<Token>;
using Nodes = std::vector<Pnode>;

class Evaluator;

struct Token {
    enum Typk {
        tNull, tEof, tDot, tTld, tSmc, tEol, tLsq, tRsq, tIdt,
        tNum, tCma, tPls, tMin, tLbr, tRbr, tChr, tStr, tQmk,
        tLcu, tRcu, tEql, tCol, tDol, tIgn, tDid, tMac
    };

    int line;
    short pos;
    short file_idx = 0;
    Typk typk;
    Unumber un;
    string s;

    Token(int ln, int ps) : line(ln), pos(ps), typk(tNull) {}

    Token(const Token &t, Typk k) : Token(t, k, "", 0) {}

    Token(const Token &t, Typk k, const string &q) : Token(t, k, q, 0) {}

    Token(const Token &t, const Unumber &n) : Token(t, tNum, "", n) {}

    Token(const Token &t, Typk k, string q, const Unumber &n)
            : line(t.line), pos(t.pos), typk(k), un(n), s(std::move(q)) {}

    string str() const;

    bool is(Typk tp) const { return typk == tp; }

    bool isEos() const { return typk == tEol || typk == tSmc; }
};

struct Node : protected Token {
    Node *parent;
    Nodes children;

    Node(Node *p, Token t) : Token(std::move(t)), parent(p) {}

    Node(Token t) : Token(std::move(t)), parent(nullptr) {}

    virtual ~ Node() = default;

    void addChild(Pnode n);

    string dumpr(std::vector<int> &ind, bool middle) const;

    string dump() const {
        std::vector<int> i;
        return dumpr(i, false);
    }

    Root *root() const;

    const Litem *litem() const;

    Token tok() const { return Token(*this); }

    virtual Pnode clone() const = 0;

protected:
    virtual string print() const = 0;

    Pnode baseclone(Pnode) const;

    Node(const Node &) = delete;

    friend class Evaluator;
};

struct Labels : Node {
    Labels(Token t) : Node(std::move(t)) {}

    string print() const final { return "labels"; }

    Pnode clone() const final { return baseclone(Pnode(new Labels(tok()))); }
};

struct Root : Node {
    Root() : Node(Token(0, 0)) {}

    string print() const final { return "root"; }

    Compiler *comp{};
    Nodes names_pending;
    std::map<string, Cell> names_defined;
    std::map<string, Pnode> names_arraysz;
    std::map<string, Pnode> allLabs;

    std::map<string, Pnode> macdefs;

    void addMacdef(Pnode i);

    string dump_names();

    bool getvalue(const string &s, Cell &x, Token tk);

    void setvalue(const string &s, const Cell &x) {
        names_defined[s] = x;
    }

    Nodes::const_iterator find_pending(const string &s) const;

    void addarrsz(const string &name, Pnode inst);

    void resolve_pending();

    void resolve_arraysz();

    Pnode clone() const final;
};


struct Instruction : Node {
    enum Typ {
        eData, eCurl, eThree
    };
    Typ typ;

    bool tilda;

    Instruction(Token t) : Node(t), typ(eThree), tilda(false) {}

    string print() const final {
        return string("instruction ") + (tilda ? "~" : "") + ".c3"[typ];
    }

    Pnode clone() const final;
};

struct HasTsValue {
    virtual Cell val() const = 0;
};

struct Litem : Node {
    enum Typ {
        eNull, eDef, eItm
    };
    Typ typ;

    Cell address, value;

    Litem(Token tk, Pnode lab, Pnode itm, Typ tp);

    string print() const final;

    Pnode clone() const final;
};

using Plitem = std::shared_ptr<Litem>;

struct Item : Node {
    enum Typ {
        eNull, eExpr, eNum, eId, eStr
    };
    Typ typ;
    bool tilda;

    Item(Token tk) : Node(std::move(tk)), typ(eNull), tilda(false) {}

    string print() const final;

    Pnode clone() const final;
};

using Pitem = std::shared_ptr<Item>;

struct Expr : Node, HasTsValue {
    enum Typ {
        eTerm, ePlus, eMinus
    };
    Typ typ;

    Expr(Token tk) : Node(tk), typ(eTerm) {}

    string print() const final { return string("expr ") + "t+-"[typ]; }

    Cell val() const final;

    Pnode clone() const final;
};

struct Idn : Node {
    using Node::s;

    Idn(Token t) : Node(std::move(t)) {}

    string print() const final { return "id: " + s; }

    Pnode clone() const final { return baseclone(Pnode(new Idn(tok()))); }
};

struct Cnst : Node, HasTsValue {
    enum Typ {
        eTsnum, eChr, eQmk
    };
    Typ typ;

    Cnst(Token t, Typ tp) : Node(t), typ(tp) {}

    Cnst(Token t, Pnode n, Typ tp) : Node(t), typ(tp) { addChild(n); }

    string print() const final;

    using Token::s;

    Cell val() const final;

    Pnode clone() const final;
};

struct Unum : Node {
    using Node::un;

    Unum(Token t) : Node(t) {}

    string print() const final { return "unum " + un.str(); }

    Pnode clone() const final { return baseclone(Pnode(new Unum(tok()))); }
};

struct Tsnum : Node, HasTsValue {
    Cell val() const final;

    string print() const final { return "tsnum"; }

    Tsnum(Token k, Pnode t) : Node(std::move(k)) { addChild(t); }

    Tsnum(Token k, Pnode t, Pnode s) : Node(std::move(k)) {
        addChild(std::move(t));
        addChild(std::move(s));
    }

    Pnode clone() const final;
};

struct Term : Node, HasTsValue {
    enum Typ {
        eNull, eExpr, eMinus, eCnst, eId, eFunc
    };
    Typ typ;

    Term(Token tk, Pnode n, Typ tp) : Node(tk), typ(tp) { addChild(n); }

    Term(Token tk, Pnode pn, Nodes ns, Typ tp) : Node(tk), typ(tp) {
        addChild(pn);
        for (auto i: ns) addChild(i);
    }

    string print() const final { return string("term ") + "#e-cif"[typ]; }

    Cell val() const final;

    Cell func_call() const;

    Cell func_unit() const;

    Cell func_enc() const;

    Cell func_X() const;

    Cell func_Ts() const;

    Cell func_T() const;

    Cell func_S() const;

    Cell func_invN() const;

    Cell func_powN() const;

    Cell func_comp(int f, Token k) const;

    Pnode clone() const final;

    static Cell testB2beta(Cell t, Token k);

    static Unumber testFkf(Unumber t, Token k);

    static Unumber testRnd(Unumber x, Token k, Unumber N);
};

struct Macuse final : Node {
    Macuse(Token tk, Pnode prelabs) : Node(std::move(tk)) { addChild(std::move(prelabs)); }

    string print() const final { return "." + name(); }

    string name() const { return tok().s; }

    Pnode clone() const final;
};

struct Macdef final : Node {
    string name;
    Nodes argnams;
    Nodes globals;

    Pnode prelabs;
    Nodes argvals;
    std::map<string, Pnode> locdefs;
    std::map<string, Nodes> iduse;
    std::set<string> undefs;

    std::set<string> entries; // globals which are defined inside
    // such as entry points in macro functions: start/end

    Macdef(Token tk) : Node(tk) {}

    string print() const final;

    Pnode clone() const final;

    int idxGlobal(const string &name_in) const { return idx(name_in, globals); }

    int idxArgs(const string &name_in) const { return idx(name_in, argnams); }

protected:
    static int idx(const string &name, const Nodes &nodes);
};

