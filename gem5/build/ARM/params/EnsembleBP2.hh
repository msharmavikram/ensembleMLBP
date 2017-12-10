#ifndef __PARAMS__EnsembleBP2__
#define __PARAMS__EnsembleBP2__

class EnsembleBP2;

#include <cstddef>
#include "base/types.hh"
#include <cstddef>
#include "base/types.hh"
#include <cstddef>
#include "base/types.hh"
#include <cstddef>
#include "base/types.hh"
#include <cstddef>
#include "base/types.hh"
#include <cstddef>
#include "base/types.hh"
#include <cstddef>
#include "base/types.hh"
#include <cstddef>
#include "base/types.hh"
#include <cstddef>
#include "base/types.hh"
#include <cstddef>
#include "base/types.hh"
#include <cstddef>
#include "base/types.hh"
#include <cstddef>
#include "base/types.hh"
#include <cstddef>
#include "base/types.hh"
#include <cstddef>
#include "base/types.hh"
#include <cstddef>
#include "base/types.hh"
#include <cstddef>
#include "base/types.hh"
#include <cstddef>
#include "base/types.hh"
#include <cstddef>
#include "base/types.hh"
#include <cstddef>
#include "base/types.hh"
#include <cstddef>
#include "base/types.hh"
#include <cstddef>
#include "base/types.hh"
#include <cstddef>
#include "base/types.hh"
#include <cstddef>
#include "base/types.hh"
#include <cstddef>
#include "base/types.hh"
#include <cstddef>
#include "base/types.hh"

#include "params/BranchPredictor.hh"

struct EnsembleBP2Params
    : public BranchPredictorParams
{
    EnsembleBP2 * create();
    unsigned cacheSize;
    unsigned choiceCtrBits;
    unsigned choiceCtrBits2;
    unsigned choicePredictorSize;
    unsigned choicePredictorSize2;
    unsigned globalCtrBits;
    unsigned globalPredictorSize;
    unsigned gshareCtrBits;
    unsigned gsharePredictorSize;
    unsigned histBufferSize;
    unsigned historyLength;
    unsigned localCtrBits;
    unsigned localCtrBits2;
    unsigned localHistoryTableSize;
    unsigned localHistoryTableSize2;
    unsigned localPredictorSize;
    unsigned localPredictorSize2;
    unsigned logSizeBiMP;
    unsigned logSizeLoopPred;
    unsigned logSizeTagTables;
    unsigned maxHist;
    unsigned minHist;
    unsigned minTagWidth;
    unsigned nHistoryTables;
    unsigned tagTableCounterBits;
};

#endif // __PARAMS__EnsembleBP2__
