#include "search.h"
#include <math.h>



Search::Search()
{
    
}

//function negamax(node, depth, α, β, color)
int Search::negamax(Position& position, int depth, int alpha, int beta)
{
    if (stopThinking)
        throw exit_search();

    // From https://en.wikipedia.org/wiki/Negamax

    //alphaOrig : = α
    int alpha_original = alpha;

    // Transposition Table Lookup; node is the lookup key for ttEntry
    //ttEntry : = TranspositionTableLookup(node)
    auto tt_entry = tt->retrieve(zobristHash);

    //if ttEntry is valid and ttEntry.depth ≥ depth
    if (tt_entry != NULL && tt_entry->depth >= depth)
    {
        // Add depth punishment if there's a checkmate coming up
        int retrieved_eval = tt_entry->eval;


        //if ttEntry.Flag = EXACT
        if (tt_entry->flag == TranspositionTable::EntryType::EXACT)
            //return ttEntry.Value
            return retrieved_eval;
        //else if ttEntry.Flag = LOWERBOUND
        else if (tt_entry->flag == TranspositionTable::EntryType::LOWER)
            //α : = max(α, ttEntry.Value)
            alpha = max(alpha, retrieved_eval);
        //else if ttEntry.Flag = UPPERBOUND
        else if (tt_entry->flag == TranspositionTable::EntryType::UPPER)
            //β : = min(β, ttEntry.Value)
            beta = min(beta, retrieved_eval);

        //if α ≥ β
        if (alpha >= beta)
            //return ttEntry.Value
            return tt_entry->eval;
    }
    //if depth = 0 or node is a terminal node
    if (depth == 0)
        //return color * the heuristic value of node
        return sideSign(sideToMove) * calculateStaticEval();

    //bestValue : = -∞
    int best_value = -CHECKMATE_VALUE;
    //childNodes : = GenerateMoves(node)
    auto move_list = genMoves();

    // If this is a terminal node return the checkmate or stalemate score
    if (move_list.size() == 0) {
        int ending_value = checkCheck(sideToMove) ? -CHECKMATE_VALUE : 0;
        tt->insert(zobristHash, ending_value, depth, TranspositionTable::EntryType::EXACT);
        return ending_value;
    }

    //childNodes : = OrderMoves(childNodes)
    sort(move_list.begin(), move_list.end(), Move::greater);

    //foreach child in childNodes
    for (auto move : move_list)
    {
        performMove(&move);
        //v : = -negamax(child, depth - 1, -β, -α, -color)
        int value = -negamax(depth - 1, -beta, -alpha);

        undoMove(&move);
        //bestValue : = max(bestValue, v)
        best_value = max(best_value, value);
        //α : = max(α, v)
        alpha = max(alpha, value);
        //if α ≥ β
        if (alpha >= beta)
            //break
            break;
    }


    // Transposition Table Store; node is the lookup key for ttEntry
    TranspositionTable::EntryType entry_type;
    //ttEntry.Value : = bestValue
    //if bestValue ≤ alphaOrig
    if (best_value <= alpha_original)
        //ttEntry.Flag : = UPPERBOUND
        entry_type = TranspositionTable::EntryType::UPPER;
    //else if bestValue ≥ β
    else if (best_value >= beta)
        //ttEntry.Flag : = LOWERBOUND
        entry_type = TranspositionTable::EntryType::LOWER;
    //else
    else
        //ttEntry.Flag : = EXACT
        entry_type = TranspositionTable::EntryType::EXACT;
    //endif
    //ttEntry.depth : = depth
    //TranspositionTableStore(node, ttEntry)

    tt->insert(zobristHash, best_value, depth, entry_type);

    //return bestValue
    return best_value;
}


int Search::terminateSearch(int alpha, int beta)
{
    if (stopThinking)
        throw exit_search();

    vector<Move> move_list = genMoves();

    move_list.erase(remove_if(move_list.begin(), move_list.end(), Move::isNotCapture), move_list.end());

    if (move_list.size() == 0)
        return calculateStaticEval();

    int best_value = sideToMove == WHITE ? INT_MIN : INT_MAX;
    sort(move_list.begin(), move_list.end(), sideToMove == WHITE ? Move::greater : Move::lesser);

    for (auto& move : move_list) {
        int eval;

        performMove(&move);
        auto tt_entry = tt->retrieve(zobristHash);
        if (tt_entry == NULL) {
            eval = terminateSearch(alpha, beta);
            tt->insert(zobristHash, eval, 1, TranspositionTable::EntryType::EXACT);
        }
        else
            eval = tt_entry->eval;
        undoMove(&move);

        best_value = sideToMove == WHITE ? max(best_value, eval) : min(best_value, eval);
        sideToMove == WHITE ? alpha = max(alpha, eval) : beta = min(beta, eval);

        if (beta <= alpha)
            break;
    }

    return best_value;
}


/*
ponder - analyse the position without care for depth of the search
*/
void Search::ponderPosition(Position& position)
{
    ponderPosition(position, 0);
}


/*
ponder - analyse the position without care for depth of the search
*/
void Search::ponderPosition(Position& position, int depth)
{
    if (stopThinking)
        return;

    try {
        string send = "info score cp ";
        int score = sideSign(sideToMove) * negamax(depth, INT_MIN, INT_MAX);
        send += to_string(score) + " depth " + to_string(depth) + " pv";
        auto pvs = new vector<Move>();
        getPV(pvs, depth);
        for (auto it : *pvs)
            send += " " + it.getName();
        output->output(send);
    }
    catch (exit_search e) {}

    vector<Move> move_list = genMoves();
    if (move_list.size() == 0)
        return;

    sort(move_list.begin(), move_list.end(), Move::greater);
    auto best_move = move_list.at(0);

    if (abs(best_move.eval) > CHECKMATE_THRESHOLD)
        output->debug("Found a checkmate in " + to_string(best_move.depth) + " starting with " + best_move.getName());

    int padding = 10;

    string debugStringMoveName = "Move    |";
    string debugStringEval = "Eval    |";
    string debugStringDepth = "Depth   |";
    for (auto move : move_list) {
        string move_name = move.getName();
        string move_eval = to_string(move.eval);
        string move_depth = to_string(move.depth);

        debugStringMoveName += " " + string(10 - move_name.length(), ' ') + move.getName() + " |";
        debugStringEval += " " + string(10 - move_eval.length(), ' ') + to_string(move.eval) + " |";
        debugStringDepth += " " + string(10 - move_depth.length(), ' ') + to_string(move.depth) + " |";
    }

    output->debug("Debug: move list\n" + debugStringMoveName + "\n" + debugStringEval + "\n" + debugStringDepth + "\n");
    output->debug("Debug: bestmove " + best_move.getName());

    ponder(depth + 1);
}

/*
getPV - return what the engine considers the best line for the given depth
*/
void Search::getPV(vector<Move>* move_list, int depth)
{
    Move* pv = topMove();
    if (pv == NULL)
        return;
    move_list->push_back(Move(*pv));
    performMove(pv);
    if (depth > 0)
        getPV(move_list, depth - 1);
    undoMove(pv);
}


/*
topMove - Return the current top move
*/
Move* Search::topMove()
{
    // Evaluate each move
    vector<Move> move_list = genMoves();
    sort(move_list.begin(), move_list.end(), Move::greater);

    if (move_list.size() == 0)
        return NULL;

    return new Move(move_list[0]);
}