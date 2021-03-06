#ifndef __Q2TALLYVOTES_H__
#define __Q2TALLYVOTES_H__

_Monitor Printer;

#if defined( EXT )                           // external scheduling monitor solution
_Monitor TallyVotes {
#elif defined( INT )                         // internal scheduling monitor solution
_Monitor TallyVotes {
    uCondition full;
#elif defined( INTB )                        // internal scheduling monitor solution with barging
_Monitor TallyVotes {
    uCondition bench;                        // only one condition variable (you may change the variable name)
    unsigned int tickets;
    unsigned int curTicket;
    void wait();                             // barging version of wait
    void signalAll();                        // unblock all waiting tasks
#elif defined( AUTO )                        // automatic-signal monitor solution
#include "AutomaticSignal.h"
_Monitor TallyVotes {
    AUTOMATIC_SIGNAL;
    bool full;
#elif defined( TASK )                        // internal/external scheduling task solution
_Task TallyVotes {
    uCondition bench;
    bool failed;
    void main();
#else
    #error unsupported voter type
#endif
  public:                                    // common interface
    _Event Failed {};
    TallyVotes( unsigned int voters, unsigned int group, Printer & printer );
    struct Ballot { unsigned int picture, statue, giftshop; };
    enum Tour { Picture = 'p', Statue = 's', GiftShop = 'g' };
    Tour vote( unsigned int id, Ballot ballot );
    void done();
  private:
    // common declarations
    struct Ballot total = { 0, 0, 0 };
    unsigned int id;
    unsigned int voters;
    unsigned int group;
    unsigned int numAvailVoters;
    unsigned int numWaitVoters;
    Printer &printer;
    Ballot ballot;
    Tour tour;
    void countVotes ( Ballot ballot ) {
        total.picture  += ballot.picture;
        total.statue   += ballot.statue;
        total.giftshop += ballot.giftshop;
    }
    void resetVotes () {
        total = { 0, 0, 0 };
    }
    Tour bestVote() {
        Tour state = Tour::Picture;
        if (total.picture > total.giftshop) {
            if (total.picture < total.statue) {
                state = Tour::Statue;
            } // if
        } else if (total.picture == total.giftshop) {
            if (total.picture < total.statue) {
                state = Tour::Statue;
            } else {
                state = Tour::GiftShop;
            } // if
        } else {
            if (total.statue > total.giftshop) {
                state = Tour::Statue;
            } else {
                state = Tour::GiftShop;
            } // if
        } // if
        return state;
    }
};

#endif