#include "transposition_table.h"
#include "board_definitions.h"


TranspositionTable::TranspositionTable(int size)
{
    setSize(size);
}

TranspositionTable::TTEntry* TranspositionTable::retrieve(uint64_t zobrist_hash)
{
    auto entry = tt.find(zobrist_hash);

    if (entry != tt.end())
        return &entry->second;
    return NULL;
}

void TranspositionTable::insert(uint64_t zobrist_hash, int eval, uint8_t depth, EntryType entry_type)
{
    TTEntry entry;
    entry.eval = eval;
    entry.depth = depth;
    entry.flag = entry_type;
    tt[zobrist_hash] = entry;

    if (tt.size() >= size)
        clear();

}

void TranspositionTable::clear()
{
    tt.clear();
}

void TranspositionTable::setSize(int size_mb)
{
    size = size_mb * 1000 * 1000 / 32 / 4;
    if (tt.size() > size)
        tt.clear();
}
