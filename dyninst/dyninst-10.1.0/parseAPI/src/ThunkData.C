#include "dyntypes.h"
#include "ThunkData.h"
#include "IndirectASTVisitor.h"
#include "CFG.h"
#include "Absloc.h"
#include "AbslocInterface.h"
#include "debug_parse.h"

using namespace std;
using namespace Dyninst;
using namespace Dyninst::DataflowAPI;
using namespace Dyninst::ParseAPI;
	
void ReachFact::ReverseDFS(ParseAPI::Block *cur, set<ParseAPI::Block*> &visited) {
    if (visited.find(cur) != visited.end()) return;
    visited.insert(cur);
    boost::lock_guard<Block> g(*cur);

    for (auto eit = cur->sources().begin(); eit != cur->sources().end(); ++eit) 
        if ((*eit)->type() != CALL && (*eit)->type() != RET) ReverseDFS((*eit)->src(), visited);
}

void ReachFact::NaturalDFS(ParseAPI::Block *cur, set<ParseAPI::Block*> &visited) {
    if (visited.find(cur) != visited.end()) return;
    visited.insert(cur);
    boost::lock_guard<Block> g(*cur);

    for (auto eit = cur->targets().begin(); eit != cur->targets().end(); ++eit) 
        if ((*eit)->type() != CALL && (*eit)->type() != RET) NaturalDFS((*eit)->trg(), visited);
}



void ReachFact::ReachBlocks() {

    for (auto tit = thunks.begin(); tit != thunks.end(); ++tit) {
        ParseAPI::Block *thunkBlock = tit->second.block;
	ReverseDFS(thunkBlock, thunk_ins[thunkBlock]);
	NaturalDFS(thunkBlock, thunk_outs[thunkBlock]);
	thunk_outs[thunkBlock].erase(thunkBlock);
    }
}

ReachFact::ReachFact(ThunkData &t): thunks(t) {
    ReachBlocks();
}

