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
 *          Andrew Pensinger
 */

#include "cpu/pred/tritournament.hh"

#include "base/bitfield.hh"
#include "base/intmath.hh"

TriTournamentBP::TriTournamentBP(const TriTournamentBPParams *params)
    : BPredUnit(params),
      localPredictorSize(params->localPredictorSize),
      localCtrBits(params->localCtrBits),
      localHistoryTableSize(params->localHistoryTableSize),
      localHistoryBits(ceilLog2(params->localPredictorSize)),
      localPredictorSize2(params->localPredictorSize2),
      localCtrBits2(params->localCtrBits2),
      localHistoryTableSize2(params->localHistoryTableSize2),
      localHistoryBits2(ceilLog2(params->localPredictorSize2)),
      globalPredictorSize(params->globalPredictorSize),
      globalCtrBits(params->globalCtrBits),
      globalHistory(params->numThreads, 0),
      globalHistoryBits(
          ceilLog2(params->globalPredictorSize) >
          ceilLog2(params->choicePredictorSize) ?
          ceilLog2(params->globalPredictorSize) :
          ceilLog2(params->choicePredictorSize)),
      choicePredictorSize(params->choicePredictorSize),
      choiceCtrBits(params->choiceCtrBits)
{
    // TODO Initialize second choice predictor
    if (!isPowerOf2(localPredictorSize)) {
        fatal("Invalid local predictor size!\n");
    }

    if (!isPowerOf2(localPredictorSize2)) {
        fatal("Invalid local predictor size!\n");
    }

    if (!isPowerOf2(globalPredictorSize)) {
        fatal("Invalid global predictor size!\n");
    }

    //Set up the array of counters for the local predictor
    localCtrs.resize(localPredictorSize);

    for (int i = 0; i < localPredictorSize; ++i)
        localCtrs[i].setBits(localCtrBits);

    localPredictorMask = mask(localHistoryBits);

    if (!isPowerOf2(localHistoryTableSize)) {
        fatal("Invalid local history table size!\n");
    }

    //Setup the history table for the local table
    localHistoryTable.resize(localHistoryTableSize);

    for (int i = 0; i < localHistoryTableSize; ++i)
        localHistoryTable[i] = 0;

    //Set up the array of counters for the second local predictor
    localCtrs2.resize(localPredictorSize2);

    for (int i = 0; i < localPredictorSize2; ++i)
        localCtrs2[i].setBits(localCtrBits2);

    localPredictorMask2 = mask(localHistoryBits2);

    if (!isPowerOf2(localHistoryTableSize2)) {
        fatal("Invalid second local history table size!\n");
    }

    //Setup the history table for the second local table
    localHistoryTable2.resize(localHistoryTableSize2);

    for (int i = 0; i < localHistoryTableSize2; ++i)
        localHistoryTable2[i] = 0;

    //Setup the array of counters for the global predictor
    globalCtrs.resize(globalPredictorSize);

    for (int i = 0; i < globalPredictorSize; ++i)
        globalCtrs[i].setBits(globalCtrBits);

    // Set up the global history mask
    // this is equivalent to mask(log2(globalPredictorSize)
    globalHistoryMask = globalPredictorSize - 1;

    if (!isPowerOf2(choicePredictorSize)) {
        fatal("Invalid choice predictor size!\n");
    }

    // Set up choiceHistoryMask
    // this is equivalent to mask(log2(choicePredictorSize)
    choiceHistoryMask = choicePredictorSize - 1;

    //Setup the array of counters for the choice predictor
    choiceCtrs.resize(choicePredictorSize);

    for (int i = 0; i < choicePredictorSize; ++i)
        choiceCtrs[i].setBits(choiceCtrBits);

    //Set up historyRegisterMask
    historyRegisterMask = mask(globalHistoryBits);

    //Check that predictors don't use more bits than they have available
    if (globalHistoryMask > historyRegisterMask) {
        fatal("Global predictor too large for global history bits!\n");
    }
    if (choiceHistoryMask > historyRegisterMask) {
        fatal("Choice predictor too large for global history bits!\n");
    }

    if (globalHistoryMask < historyRegisterMask &&
        choiceHistoryMask < historyRegisterMask) {
        inform("More global history bits than required by predictors\n");
    }

    // Set thresholds for the three predictors' counters
    // This is equivalent to (2^(Ctr))/2 - 1
    localThreshold  = (ULL(1) << (localCtrBits  - 1)) - 1;
    localThreshold2 = (ULL(1) << (localCtrBits2 - 1)) - 1;
    globalThreshold = (ULL(1) << (globalCtrBits - 1)) - 1;
    choiceThreshold = (ULL(1) << (choiceCtrBits - 1)) - 1;
    // TODO Uncomment: choiceThreshold2 = (ULL(1) << (choiceCtrBits)) - 1;
}

inline
unsigned
TriTournamentBP::calcLocHistIdx(Addr &branch_addr)
{
    // Get low order bits after removing instruction offset.
    return (branch_addr >> instShiftAmt) & (localHistoryTableSize - 1);
}

inline
unsigned
TriTournamentBP::calcLocHistIdx2(Addr &branch_addr)
{
    // Get low order bits after removing instruction offset.
    return (branch_addr >> instShiftAmt) & (localHistoryTableSize2 - 1);
}

inline
void
TriTournamentBP::updateGlobalHistTaken(ThreadID tid)
{
    globalHistory[tid] = (globalHistory[tid] << 1) | 1;
    globalHistory[tid] = globalHistory[tid] & historyRegisterMask;
}

inline
void
TriTournamentBP::updateGlobalHistNotTaken(ThreadID tid)
{
    globalHistory[tid] = (globalHistory[tid] << 1);
    globalHistory[tid] = globalHistory[tid] & historyRegisterMask;
}

inline
void
TriTournamentBP::updateLocalHistTaken(unsigned local_history_idx)
{
    localHistoryTable[local_history_idx] =
        (localHistoryTable[local_history_idx] << 1) | 1;
}

inline
void
TriTournamentBP::updateLocalHistNotTaken(unsigned local_history_idx)
{
    localHistoryTable[local_history_idx] =
        (localHistoryTable[local_history_idx] << 1);
}

inline
void
TriTournamentBP::updateLocalHistTaken2(unsigned local_history_idx)
{
    localHistoryTable2[local_history_idx] =
        (localHistoryTable2[local_history_idx] << 1) | 1;
}

inline
void
TriTournamentBP::updateLocalHistNotTaken2(unsigned local_history_idx)
{
    localHistoryTable2[local_history_idx] =
        (localHistoryTable2[local_history_idx] << 1);
}

void
TriTournamentBP::btbUpdate(ThreadID tid, Addr branch_addr, void * &bp_history)
{
    unsigned local_history_idx = calcLocHistIdx(branch_addr);
    unsigned local_history_idx2 = calcLocHistIdx2(branch_addr);
    //Update Global History to Not Taken (clear LSB)
    globalHistory[tid] &= (historyRegisterMask & ~ULL(1));
    //Update Local History to Not Taken
    localHistoryTable[local_history_idx] =
       localHistoryTable[local_history_idx] & (localPredictorMask & ~ULL(1));
    //Update Second Local History to Not Taken
    localHistoryTable2[local_history_idx2] =
        localHistoryTable2[local_history_idx2] & (localPredictorMask2 & ~ULL(1));
}

bool
TriTournamentBP::lookup(ThreadID tid, Addr branch_addr, void * &bp_history)
{
    bool local_prediction;
    unsigned local_history_idx;
    unsigned local_predictor_idx;

    bool local_prediction2;
    unsigned local_history_idx2;
    unsigned local_predictor_idx2;

    bool global_prediction;
    bool choice_prediction;

    //Lookup in the local predictor to get its branch prediction
    local_history_idx = calcLocHistIdx(branch_addr);
    local_predictor_idx = localHistoryTable[local_history_idx]
        & localPredictorMask;
    local_prediction = localCtrs[local_predictor_idx].read() > localThreshold;

    //Lookup in the second local predictor to get its branch prediction
    local_history_idx2 = calcLocHistIdx2(branch_addr);
    local_predictor_idx2 = localHistoryTable2[local_history_idx2]
        & localPredictorMask2;
    local_prediction2 = localCtrs2[local_predictor_idx2].read() > localThreshold2;

    //Lookup in the global predictor to get its branch prediction
    global_prediction = globalThreshold <
      globalCtrs[globalHistory[tid] & globalHistoryMask].read();

    //Lookup in the choice predictor to see which one to use
    choice_prediction = choiceThreshold <
      choiceCtrs[globalHistory[tid] & choiceHistoryMask].read();

    // TODO Look up in the second choice predictor

    // Create BPHistory and pass it back to be recorded.
    BPHistory *history = new BPHistory;
    history->globalHistory = globalHistory[tid];
    history->localPredTaken = local_prediction;
    history->localPredTaken2 = local_prediction2;
    history->globalPredTaken = global_prediction;
    history->globalUsed = choice_prediction;
    // TODO Add localUsed
    history->localHistoryIdx = local_history_idx;
    history->localHistory = local_predictor_idx;
    history->localHistoryIdx2 = local_history_idx2;
    history->localHistory2 = local_predictor_idx2;
    bp_history = (void *)history;

    assert(local_history_idx < localHistoryTableSize);

    // Speculative update of the global history and the
    // selected local history.
    // TODO Add second choice predictor
    if (choice_prediction) {
        if (global_prediction) {
            updateGlobalHistTaken(tid);
            updateLocalHistTaken(local_history_idx);
            return true;
        } else {
            updateGlobalHistNotTaken(tid);
            updateLocalHistNotTaken(local_history_idx);
            return false;
        }
    } else {
        if (local_prediction) {
            updateGlobalHistTaken(tid);
            updateLocalHistTaken(local_history_idx);
            return true;
        } else {
            updateGlobalHistNotTaken(tid);
            updateLocalHistNotTaken(local_history_idx);
            return false;
        }
    }
}

void
TriTournamentBP::uncondBranch(ThreadID tid, Addr pc, void * &bp_history)
{
    // Create BPHistory and pass it back to be recorded.
    BPHistory *history = new BPHistory;
    history->globalHistory = globalHistory[tid];
    history->localPredTaken = true;
    history->localPredTaken2 = true;
    history->globalPredTaken = true;
    history->globalUsed = true;
    history->local2Used = false;
    history->localHistoryIdx = invalidPredictorIndex;
    history->localHistory = invalidPredictorIndex;
    history->localHistoryIdx2 = invalidPredictorIndex;
    history->localHistory2 = invalidPredictorIndex;
    bp_history = static_cast<void *>(history);

    updateGlobalHistTaken(tid);
}

void
TriTournamentBP::updateAdditionalStats(bool taken, void* bp_history)
{
    // Get BP History for prediction information
    BPHistory *history = (BPHistory*)bp_history;
    bool mispredict = true;
    bool prediction = history->globalUsed ? history->globalPredTaken : history->localPredTaken;
    prediction = history->local2Used ? history->localPredTaken2 : prediction;

    if(taken == prediction)
        mispredict = false;

    if(mispredict)
    {
        if((!history->local2Used && (
                (history->globalUsed && (history->localPredTaken == taken)) ||
                (!history->globalUsed && (history->globalPredTaken == taken))
           )) ||
           (history->local2Used && (history->localPredTaken2 == taken)))
        {
            // Potential correct predictions should count up here.
            atLeastOneCorrectExpert++;
        }
    }
}

void
TriTournamentBP::update(ThreadID tid, Addr branch_addr, bool taken,
                     void *bp_history, bool squashed)
{
    assert(bp_history);

    BPHistory *history = static_cast<BPHistory *>(bp_history);

    unsigned local_history_idx = calcLocHistIdx(branch_addr);
    assert(local_history_idx < localHistoryTableSize);
    unsigned local_history_idx2 = calcLocHistIdx2(branch_addr);
    assert(local_history_idx2 < localHistoryTableSize2);

    // Unconditional branches do not use local history.
    bool old_local_pred_valid = history->localHistory !=
        invalidPredictorIndex;
    bool old_local_pred_valid2 = history->localHistory2 !=
        invalidPredictorIndex;

    updateAdditionalStats(taken, bp_history);

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

        // Second local history restore and update.
        if (old_local_pred_valid2) {
            localHistoryTable2[local_history_idx2] =
                (history->localHistory2 << 1) | taken;
        }

        return;
    }

    unsigned old_local_pred_index = history->localHistory &
        localPredictorMask;
    assert(old_local_pred_index < localPredictorSize);
    unsigned old_local_pred_index2 = history->localHistory2 &
        localPredictorMask2;
    assert(old_local_pred_index2 < localPredictorSize2);

    // Update the choice predictor to tell it which one was correct if
    // there was a prediction.
    // TODO Update the second choice predictor
    if (history->localPredTaken != history->globalPredTaken &&
        old_local_pred_valid)
    {
         // If the local prediction matches the actual outcome,
         // decrement the counter. Otherwise increment the
         // counter.
         unsigned choice_predictor_idx =
           history->globalHistory & choiceHistoryMask;
         if (history->localPredTaken == taken) {
             choiceCtrs[choice_predictor_idx].decrement();
         } else if (history->globalPredTaken == taken) {
             choiceCtrs[choice_predictor_idx].increment();
         }
    }

    // Update the counters with the proper
    // resolution of the branch. Histories are updated
    // speculatively, restored upon squash() calls, and
    // recomputed upon update(squash = true) calls,
    // so they do not need to be updated.
    unsigned global_predictor_idx =
            history->globalHistory & globalHistoryMask;
    if (taken) {
          globalCtrs[global_predictor_idx].increment();
          if (old_local_pred_valid) {
                 localCtrs[old_local_pred_index].increment();
          }
          if (old_local_pred_valid2) {
              localCtrs2[old_local_pred_index2].increment();
          }
    } else {
          globalCtrs[global_predictor_idx].decrement();
          if (old_local_pred_valid) {
              localCtrs[old_local_pred_index].decrement();
          }
          if (old_local_pred_valid2) {
              localCtrs2[old_local_pred_index2].decrement();
          }
    }

    // We're done with this history, now delete it.
    delete history;
}

void
TriTournamentBP::squash(ThreadID tid, void *bp_history)
{
    BPHistory *history = static_cast<BPHistory *>(bp_history);

    // Restore global history to state prior to this branch.
    globalHistory[tid] = history->globalHistory;

    // Restore local history
    if (history->localHistoryIdx != invalidPredictorIndex) {
        localHistoryTable[history->localHistoryIdx] = history->localHistory;
    }

    // Restore local history
    if (history->localHistoryIdx2 != invalidPredictorIndex) {
        localHistoryTable2[history->localHistoryIdx2] = history->localHistory2;
    }

    // Delete this BPHistory now that we're done with it.
    delete history;
}

TriTournamentBP*
TriTournamentBPParams::create()
{
    return new TriTournamentBP(this);
}

unsigned
TriTournamentBP::getGHR(ThreadID tid, void *bp_history) const
{
    return static_cast<BPHistory *>(bp_history)->globalHistory;
}

#ifdef DEBUG
int
TriTournamentBP::BPHistory::newCount = 0;
#endif
