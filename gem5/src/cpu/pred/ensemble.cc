/*
 * Copyright (c) 2011, 2014 ARM Limited
 * All rights reserved
 *
 * The license below extends only to copyright in the software and shall
 * not be construed as granting a license to any other intellectual
 * property including but not limited to intellectual property relating
 * to a hardware implementation of the functionality of the software
 * licensed hereunder.  You may use the software subject to the license
 * terms below provided that you ensure that this notice is replicated
 * unmodified and in its entirety in all distributions of the software,
 * modified or unmodified, in source code or in binary form.
 *
 * Copyright (c) 2004-2006 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Kevin Lim
 */

#include "cpu/pred/ensemble.hh"

#include "base/bitfield.hh"
#include "base/intmath.hh"

#define USE_GSHARE 1
//#define USE_FREE_WEIGHTS 1
#define ENSEMBLE_SIZE_3 1
#define MAX_WEIGHT_SUM 12
#define WEIGHT_COUNTER_SIZE 4

EnsembleBP::EnsembleBP(const EnsembleBPParams *params)
    : BPredUnit(params),
      localPredictorSize(params->localPredictorSize),
      localCtrBits(params->localCtrBits),
      localHistoryTableSize(params->localHistoryTableSize),
      localHistoryBits(ceilLog2(params->localPredictorSize)),
      globalPredictorSize(params->globalPredictorSize),
      globalCtrBits(params->globalCtrBits),
      globalHistory(params->numThreads, 0),
      globalHistoryBits(ceilLog2(params->globalPredictorSize)),
      gsharePredictorSize(params->gsharePredictorSize),
      gshareCtrBits(params->gshareCtrBits)
{
    if (!isPowerOf2(localPredictorSize)) {
        fatal("Invalid local predictor size!\n");
    }

    if (!isPowerOf2(globalPredictorSize)) {
        fatal("Invalid global predictor size!\n");
    }

    if (!isPowerOf2(gsharePredictorSize)) {
        fatal("Invalid global predictor size!\n");
    }

    // Set up the array of counters for the local predictor
    localCtrs.resize(localPredictorSize);
    localWeights.resize(localPredictorSize);

    for (int i = 0; i < localPredictorSize; ++i){
        localCtrs[i].setBits(localCtrBits);
        localWeights[i].setBits(WEIGHT_COUNTER_SIZE);
    }

    localPredictorMask = mask(localHistoryBits);

    if (!isPowerOf2(localHistoryTableSize)) {
        fatal("Invalid local history table size!\n");
    }

    // Setup the history table for the local table
    localHistoryTable.resize(localHistoryTableSize);

    for (int i = 0; i < localHistoryTableSize; ++i)
        localHistoryTable[i] = 0;

    // Setup the array of counters for the global predictor
    globalCtrs.resize(globalPredictorSize);
    globalWeights.resize(globalPredictorSize);

    for (int i = 0; i < globalPredictorSize; ++i){
        globalCtrs[i].setBits(globalCtrBits);
        globalWeights[i].setBits(WEIGHT_COUNTER_SIZE);
    }

    // Setup the array of counters for the gshare predictor
    gshareCtrs.resize(gsharePredictorSize);
    gshareWeights.resize(globalPredictorSize);

    for (int i = 0; i < gsharePredictorSize; ++i){
        gshareCtrs[i].setBits(gshareCtrBits); 
        gshareWeights[i].setBits(WEIGHT_COUNTER_SIZE);
    }

    // Set up the global history mask
    // this is equivalent to mask(log2(globalPredictorSize)
    globalHistoryMask = globalPredictorSize - 1;

    //Set up historyRegisterMask
    historyRegisterMask = mask(globalHistoryBits);

    //Check that predictors don't use more bits than they have available
    if (globalHistoryMask > historyRegisterMask) {
        fatal("Global predictor too large for global history bits!\n");
    }

    if (globalHistoryMask < historyRegisterMask) {
        inform("More global history bits than required by predictors\n");
    }

    // Set thresholds for the three predictors' counters
    // This is equivalent to (2^(Ctr))/2 - 1
    localThreshold  = (ULL(1) << (localCtrBits  - 1)) - 1;
    globalThreshold = (ULL(1) << (globalCtrBits - 1)) - 1;
    gshareThreshold = (ULL(1) << (gshareCtrBits - 1)) - 1;
}

inline
unsigned
EnsembleBP::calcLocHistIdx(Addr &branch_addr)
{
    // Get low order bits after removing instruction offset.
    return (branch_addr >> instShiftAmt) & (localHistoryTableSize - 1);
}

inline
void
EnsembleBP::updateGlobalHistTaken(ThreadID tid)
{
    globalHistory[tid] = (globalHistory[tid] << 1) | 1;
    globalHistory[tid] = globalHistory[tid] & historyRegisterMask;
}

inline
void
EnsembleBP::updateGlobalHistNotTaken(ThreadID tid)
{
    globalHistory[tid] = (globalHistory[tid] << 1);
    globalHistory[tid] = globalHistory[tid] & historyRegisterMask;
}

inline
void
EnsembleBP::updateLocalHistTaken(unsigned local_history_idx)
{
    localHistoryTable[local_history_idx] =
        (localHistoryTable[local_history_idx] << 1) | 1;
}

inline
void
EnsembleBP::updateLocalHistNotTaken(unsigned local_history_idx)
{
    localHistoryTable[local_history_idx] =
        (localHistoryTable[local_history_idx] << 1);
}


void
EnsembleBP::btbUpdate(ThreadID tid, Addr branch_addr, void * &bp_history)
{
    unsigned local_history_idx = calcLocHistIdx(branch_addr);
    //Update Global History to Not Taken (clear LSB)
    globalHistory[tid] &= (historyRegisterMask & ~ULL(1));
    //Update Local History to Not Taken
    localHistoryTable[local_history_idx] =
       localHistoryTable[local_history_idx] & (localPredictorMask & ~ULL(1));
}

bool
EnsembleBP::lookup(ThreadID tid, Addr branch_addr, void * &bp_history)
{

    unsigned local_history_idx;
    unsigned local_predictor_idx;
    unsigned gshare_predictor_idx;

    // Individual Prediction Variables
    bool local_prediction;
    bool global_prediction;
    bool gshare_prediction;

    // Weighted accumulated voting result
    int accumulation = 0;

    assert(gshare_predictor_idx < globalPredictorSize);

    //Lookup in the local predictor to get its branch prediction
    local_history_idx = calcLocHistIdx(branch_addr);
    local_predictor_idx = localHistoryTable[local_history_idx]
        & localPredictorMask;
    local_prediction = localCtrs[local_predictor_idx].read() > localThreshold;

    //Lookup in the global predictor to get its branch prediction
    global_prediction = globalThreshold <
      globalCtrs[globalHistory[tid] & globalHistoryMask].read();

#ifdef USE_GSHARE
    gshare_predictor_idx = (((branch_addr >> instShiftAmt)
                        ^ globalHistory[tid])
                        & globalHistoryMask);
#endif
#ifndef USE_GSHARE
    gshare_predictor_idx = globalHistory[tid] & globalHistoryMask;
#endif
    gshare_prediction = gshareThreshold < gshareCtrs[gshare_predictor_idx].read();


    // Create BPHistory and pass it back to be recorded.
    BPHistory *history = new BPHistory;
    history->globalHistory = globalHistory[tid];
    history->localPredTaken = local_prediction;
    history->globalPredTaken = global_prediction;
    history->gsharePredTaken = gshare_prediction;
    history->localHistoryIdx = local_history_idx;
    history->localHistory = local_predictor_idx;
    bp_history = (void *)history;

    assert(local_history_idx < localHistoryTableSize);

    // Add global to verdict
    if(global_prediction)
        accumulation += globalWeights[globalHistory[tid] & globalHistoryMask].read();
    else
        accumulation -= globalWeights[globalHistory[tid] & globalHistoryMask].read();

    // Add local to verdict
    if(local_prediction)
        accumulation += localWeights[local_predictor_idx].read();
    else
        accumulation -= localWeights[local_predictor_idx].read();

#ifdef ENSEMBLE_SIZE_3
    // Add GShare to verdict
    if(gshare_prediction)
        accumulation += gshareWeights[gshare_predictor_idx].read();
    else
        accumulation -= gshareWeights[gshare_predictor_idx].read();
#endif

    history->finalPrediction = (accumulation >= 0);
    return history->finalPrediction;
}

void
EnsembleBP::uncondBranch(ThreadID tid, Addr pc, void * &bp_history)
{
    // Create BPHistory and pass it back to be recorded.
    BPHistory *history = new BPHistory;
    history->globalHistory = globalHistory[tid];
    history->localPredTaken = true;
    history->globalPredTaken = true;
    history->gsharePredTaken = true;
    history->globalUsed = true;
    history->finalPrediction = true;
    history->localHistoryIdx = invalidPredictorIndex;
    history->localHistory = invalidPredictorIndex;
    bp_history = static_cast<void *>(history);

    updateGlobalHistTaken(tid);
}

void
EnsembleBP::updateAdditionalStats(bool taken, void* bp_history, unsigned gshare_index, unsigned global_index, unsigned local_index,
                                    bool gshareContributed, bool globalContributed, bool localContributed)
{
    // Get BP History for prediction information
    BPHistory *history = (BPHistory*)bp_history;
    bool mispredict = true;
    bool prediction = history->finalPrediction;

    if(taken == prediction)
        mispredict = false;

    // If the expert with the highest weight was overtaken by two experts with lower weights in the vote
    if(((!gshareContributed && globalContributed && localContributed) 
            && (gshareWeights[gshare_index].read() > globalWeights[global_index].read())
            && (gshareWeights[gshare_index].read() > localWeights[local_index].read()))
        ||
        ((gshareContributed && !globalContributed && localContributed) 
            && (globalWeights[global_index].read() > gshareWeights[gshare_index].read())
            && (globalWeights[global_index].read() > localWeights[local_index].read()))
        ||
        ((gshareContributed && globalContributed && !localContributed) 
            && (localWeights[local_index].read() > gshareWeights[gshare_index].read())
            && (localWeights[local_index].read() > globalWeights[local_index].read())))
    {
        lowWeightExpertsWon++;
    }

    if((history->localPredTaken == history->globalPredTaken) && (history->globalPredTaken == history->gsharePredTaken))
    {
        // All experts voted the same way
        if(mispredict)
            allExpertsWrong++;
        else
            allExpertsRight++;
    }

    if(mispredict)
    {
        if(((history->localPredTaken == taken)) || (history->globalPredTaken == taken) || (history->gsharePredTaken == taken))
        {
            // There was at least one correct expert
            atLeastOneCorrectExpertOnMispredict++;
        }
    }
}

void
EnsembleBP::update(ThreadID tid, Addr branch_addr, bool taken,
                     void *bp_history, bool squashed)
{
    assert(bp_history);
    BPHistory *history = static_cast<BPHistory *>(bp_history);

    unsigned local_history_idx = calcLocHistIdx(branch_addr);
    assert(local_history_idx < localHistoryTableSize);

    // Unconditional branches do not use local history.
    bool old_local_pred_valid = history->localHistory !=
            invalidPredictorIndex;

    // If this is a misprediction, restore the speculatively
    // updated state (global history register and local history)
    // and update again.
    if (squashed) {
        // Global history restore and update
        globalHistory[tid] = (history->globalHistory << 1) | taken;
        globalHistory[tid] &= historyRegisterMask;

        // Local history restore and update.
        if (old_local_pred_valid) {
            localHistoryTable[local_history_idx] =
                        (history->localHistory << 1) | taken;
        }

        return;
    }

    unsigned old_local_pred_index = history->localHistory &
        localPredictorMask;
    assert(old_local_pred_index < localPredictorSize);

    // Update the counters with the proper
    // resolution of the branch. Histories are updated
    // speculatively, restored upon squash() calls, and
    // recomputed upon update(squash = true) calls,
    // so they do not need to be updated.
    unsigned global_predictor_idx =
            history->globalHistory & globalHistoryMask;

#ifdef USE_GSHARE
    unsigned gshare_predictor_idx = (((branch_addr >> instShiftAmt)
                        ^ globalHistory[tid])
                        & globalHistoryMask);
#endif
#ifndef USE_GSHARE
    unsigned gshare_predictor_idx = globalHistory[tid] & globalHistoryMask;
#endif

    unsigned contributorCount = 0;
    unsigned learningRate = 0;
    unsigned adjustmentRate = 0;
    unsigned weightSum = 0;

    bool localContributed = false;
    bool globalContributed = false;
    bool gshareContributed = false;

    if(history->finalPrediction == history->globalPredTaken)
    {
        globalContributed = true;
        contributorCount++;
    }
    if(history->finalPrediction == history->localPredTaken)
    {
        localContributed = true;
        contributorCount++;
    }
    if(history->finalPrediction == history->gsharePredTaken)
    {
        gshareContributed = true;
        contributorCount++;
    }


    updateAdditionalStats(taken, bp_history, gshare_predictor_idx, global_predictor_idx, old_local_pred_index,
                        gshareContributed, globalContributed, localContributed);

#ifndef USE_FREE_WEIGHTS
    // Contributor count 0 is impossible and count 3 will not impact weights.
    learningRate = (contributorCount == 1) ? 2 : ((contributorCount == 2) ? 1 : 0);
    adjustmentRate = (contributorCount == 1) ? 1 : ((contributorCount == 2) ? 2 : 0);

    // Learning
    if(gshareContributed)
    {
        if(history->finalPrediction == taken)
        {
            for(int i = 0; i < learningRate; i++)
                gshareWeights[gshare_predictor_idx].increment();
        }
        else
        {
            for(int i = 0; i < learningRate; i++)
                gshareWeights[gshare_predictor_idx].decrement();
        }
    }
    if(globalContributed)
    {
        if(history->finalPrediction == taken)
        {
            for(int i = 0; i < learningRate; i++)
                globalWeights[global_predictor_idx].increment();
        }
        else
        {
            for(int i = 0; i < learningRate; i++)
                globalWeights[global_predictor_idx].decrement();
        }
        
    }
    if(localContributed)
    {
        if(history->finalPrediction == taken)
        {
            for(int i = 0; i < learningRate; i++)
                localWeights[old_local_pred_index].increment();
        }
        else
        {
            for(int i = 0; i < learningRate; i++)
                localWeights[old_local_pred_index].decrement();
        }
        
    }

    // Adjustments
    weightSum = (gshareWeights[gshare_predictor_idx].read() + globalWeights[global_predictor_idx].read() + localWeights[old_local_pred_index].read());
    if(weightSum > MAX_WEIGHT_SUM)
    {
        if(!gshareContributed)
        {
            if(history->finalPrediction == taken)
            {
                // Ensemble was correct but GShare voted the other way
                for(int i = 0; i < adjustmentRate; i++)
                    gshareWeights[gshare_predictor_idx].decrement();
            }
            else
            {
                // Ensemble was incorrect and GShare opposed verdict
                for(int i = 0; i < adjustmentRate; i++)
                    gshareWeights[gshare_predictor_idx].increment();
            }
        }
        if(!globalContributed)
        {
            if(history->finalPrediction == taken)
            {
                // Ensemble was correct but GShare voted the other way
                for(int i = 0; i < adjustmentRate; i++)
                    globalWeights[global_predictor_idx].decrement();
            }
            else
            {
                // Ensemble was incorrect and GShare opposed verdict
                for(int i = 0; i < adjustmentRate; i++)
                    globalWeights[global_predictor_idx].increment();
            }
        }
        if(!localContributed)
        {
            if(history->finalPrediction == taken)
            {
                // Ensemble was correct but GShare voted the other way
                for(int i = 0; i < adjustmentRate; i++)
                    localWeights[old_local_pred_index].decrement();
            }
            else
            {
                // Ensemble was incorrect and GShare opposed verdict
                for(int i = 0; i < adjustmentRate; i++)
                    localWeights[old_local_pred_index].increment();
            }
        }
    }
#endif

    if (taken) {
          globalCtrs[global_predictor_idx].increment();
          gshareCtrs[gshare_predictor_idx].increment();
          if (old_local_pred_valid) {
                 localCtrs[old_local_pred_index].increment();
          }
    } else {
          globalCtrs[global_predictor_idx].decrement();
          gshareCtrs[gshare_predictor_idx].decrement();
          if (old_local_pred_valid) {
              localCtrs[old_local_pred_index].decrement();
          }
    }

#ifdef USE_FREE_WEIGHTS
    // If Global voted in the same direction as the verdict, update weights
    if(history->finalPrediction == history->globalPredTaken)
    {
        if(history->finalPrediction == taken)
        {
            globalWeights[global_predictor_idx].increment();
        }
        else
        {
            globalWeights[global_predictor_idx].decrement();
        }
    }

    // If Local voted in the same direction as the verdict, update weights
    if(history->finalPrediction == history->localPredTaken)
    {
        if(history->finalPrediction == taken)
        {
            localWeights[old_local_pred_index].increment();
        }
        else
        {
            localWeights[old_local_pred_index].decrement();
        }
    } 

    // If GShare voted in the same direction as the verdict, update weights
    if(history->finalPrediction == history->gsharePredTaken)
    {
        if(history->finalPrediction == taken)
        {
            gshareWeights[gshare_predictor_idx].increment();
        }
        else
        {
            gshareWeights[gshare_predictor_idx].decrement();
        }
    }
#endif

    // We're done with this history, now delete it.
    delete history;
}

void
EnsembleBP::squash(ThreadID tid, void *bp_history)
{
    BPHistory *history = static_cast<BPHistory *>(bp_history);

    // Restore global history to state prior to this branch.
    globalHistory[tid] = history->globalHistory;

    // Restore local history
    if (history->localHistoryIdx != invalidPredictorIndex) {
        localHistoryTable[history->localHistoryIdx] = history->localHistory;
    }

    // Delete this BPHistory now that we're done with it.
    delete history;
}

EnsembleBP*
EnsembleBPParams::create()
{
    return new EnsembleBP(this);
}

unsigned
EnsembleBP::getGHR(ThreadID tid, void *bp_history) const
{
    return static_cast<BPHistory *>(bp_history)->globalHistory;
}

#ifdef DEBUG
int
EnsembleBP::BPHistory::newCount = 0;
#endif
