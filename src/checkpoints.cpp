// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "txdb.h"
#include "main.h"
#include "uint256.h"


static const int nCheckpointSpan = 500;

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    //
    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    //
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        ( 0,      uint256("0x0000f2681fd4e6b50fdfe4522da52e822f20c4c01a5148091cee2e353fdb5c5e") );
       /*      ( 100,  uint256("0x268b1b7367d026fb9bc4e7d011d4398ee9e73f6242fed1c47e23fca99c1390fc") )
           ( 45000,  uint256("0x76b308654150b62dfe8921ef6cba7f32305a352cf16fb2e62b40ca40bb007b43") )
	    ( 100000, uint256("0x5ee559c27df65b055859d8bc94f93760a5cda8fb98ba250c44597f6da386555d") )
	    ( 150000, uint256("0xbd48377c1c509226a21f5d8482da5c7f2630923c8620dae25c16f7414e494915") )
	    ( 200000, uint256("0xe577ba5a204cb6f24a61038146e77e41e639c9dee57e2124f31f07878dd97ecc") )
	( 250000, uint256("0xb730e745765ece961ab2dc52d44b5c877f34ab7c070a8eb39d220a3949e82e9a") )
	( 300000, uint256("0x51319f6f7bdde0cf4708c9fa9e713ad8781fd6d1b1803da44b69eacdf686babe") )
	( 350000, uint256("0xbc05fa579650fdd8ec0df863948c151babf439791fdbe3b74a7d089f6c152e6f") )
    ;*/

    // TestNet has no checkpoints
    static MapCheckpoints mapCheckpointsTestnet;

    bool CheckHardened(int nHeight, const uint256& hash)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    int GetTotalBlocksEstimate()
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        if (checkpoints.empty())
            return 0;
        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }

    // Automatically select a suitable sync-checkpoint 
    const CBlockIndex* AutoSelectSyncCheckpoint()
    {
        const CBlockIndex *pindex = pindexBest;
        // Search backward for a block within max span and maturity window
        while (pindex->pprev && pindex->nHeight + nCheckpointSpan > pindexBest->nHeight)
            pindex = pindex->pprev;
        return pindex;
    }

    // Check against synchronized checkpoint
    bool CheckSync(int nHeight)
    {
        const CBlockIndex* pindexSync = AutoSelectSyncCheckpoint();

        if (nHeight <= pindexSync->nHeight)
            return false;
        return true;
    }
}
