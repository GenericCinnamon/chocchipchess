#pragma once
#include <unordered_map>
#include <inttypes.h>
using namespace std;

class TranspositionTable
{
public:
    TranspositionTable(int size);
    enum EntryType {
        UPPER,
        LOWER,
        EXACT
    };
    struct TTEntry {
        int eval;
        uint8_t depth;
        EntryType flag;
    };
    int size;
private:
    unordered_map<uint64_t, TTEntry> tt;
    unordered_map<uint64_t, TTEntry>::iterator last_cleared;
    int percentToClear = 40;
public:
    TTEntry* retrieve(uint64_t zobrist_hash);
    void insert(uint64_t zobrist_hash, int eval, uint8_t depth, EntryType entry_type);
    void setSize(int size_mb);
    void clear();
};

