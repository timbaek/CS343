#include "q2tallyVotes.h"
#include "q2printer.h"
#include "q2voter.h"

using namespace std;

TallyVotes::TallyVotes( unsigned int voters, unsigned int group, Printer & printer )
    : voters(voters), group(group), numAvailVoters(voters), numWaitVoters(0), printer(printer)  {}

TallyVotes::Tour TallyVotes::vote( unsigned int id, Ballot ballot ) {
    if ( numAvailVoters < group ) {
        _Throw Failed();
    } // if

    countVotes( ballot );
#if defined ( OUTPUT )
    printer.print( id, Voter::Vote, ballot ); 
#endif
    numWaitVoters++;
    if ( numWaitVoters < group ) {                                      // Check if this voter is the last one to form a group
#if defined ( OUTPUT )
        printer.print( id, Voter::Block, numWaitVoters );               // If NOT, wait for more voters coming
#endif
        full.wait();
#if defined ( OUTPUT )
        printer.print( id, Voter::Unblock, numWaitVoters - 1 );
#endif
    } else {
#if defined ( OUTPUT )
        printer.print( id, Voter::Complete );                           // If yes, print the complete message
#endif
        tour = bestVote();
        resetVotes();                                                   // Reset voting result for the next group, if this voter is the last one in a group        
    } // if

    full.signal();

    numWaitVoters--;
    if ( numAvailVoters < group ) {
        _Throw Failed();
    } // if

    return tour;
} // TallyVotes::vote

void TallyVotes::done() {
    numAvailVoters--;
    if ( numAvailVoters < group ) {
        full.signal();
    }
} // TallyVotes::done