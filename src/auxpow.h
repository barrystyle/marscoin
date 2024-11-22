#ifndef BITCOIN_AUXPOW_H
#define BITCOIN_AUXPOW_H

#include "primitives/merkletx.h"
#include "primitives/pureheader.h"
#include "primitives/transaction.h"
#include "serialize.h"
#include "uint256.h"

#include <vector>

class CBlock;
class CBlockHeader;
class CBlockIndex;
class CValidationState;

/** Header for merge-mining data in the coinbase. */
static const unsigned char pchMergedMiningHeader[] = { 0xfa, 0xbe, 'm', 'm' };

/**
 * Data for the merge-mining auxpow. This extends a normal merkle transaction
 * to store additional data required for merge mining.
 */
class CAuxPow : public CMerkleTx
{
public:
    /** The merkle branch connecting the aux block to our coinbase. */
    std::vector<uint256> vChainMerkleBranch;
    
    /** Merkle tree index of the aux block header in the coinbase. */
    int nChainIndex;
    
    /** Parent block header (on which the real PoW is done). */
    CPureBlockHeader parentBlock;

    CAuxPow() : CMerkleTx() {}
    explicit CAuxPow(const CTransaction& txIn) : CMerkleTx(txIn) {}

    template<typename Stream>
    void Serialize(Stream& s, int nType, int nVersion) const {
        CMerkleTx::Serialize(s, nType, nVersion);
        ::Serialize(s, vChainMerkleBranch, nType, nVersion);
        ::Serialize(s, nChainIndex, nType, nVersion);
        ::Serialize(s, parentBlock, nType, nVersion);
    }

    template<typename Stream>
    void Unserialize(Stream& s, int nType, int nVersion) {
        CMerkleTx::Unserialize(s, nType, nVersion);
        ::Unserialize(s, vChainMerkleBranch, nType, nVersion);
        ::Unserialize(s, nChainIndex, nType, nVersion);
        ::Unserialize(s, parentBlock, nType, nVersion);
    }

    bool check(const uint256& hashAuxBlock, int nChainId) const;
    
    inline uint256 getParentBlockPoWHash() const {
        return parentBlock.GetPoWHash();
    }
    
    inline const CPureBlockHeader& getParentBlock() const {
        return parentBlock;
    }

    static int getExpectedIndex(uint32_t nNonce, int nChainId, unsigned h);
    static uint256 CheckMerkleBranch(uint256 hash, const std::vector<uint256>& vMerkleBranch, int nIndex);
    static void initAuxPow(CBlockHeader& header);
};

#endif // BITCOIN_AUXPOW_H