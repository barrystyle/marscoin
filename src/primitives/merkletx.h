// primitives/merkletx.h
#ifndef BITCOIN_PRIMITIVES_MERKLETX_H
#define BITCOIN_PRIMITIVES_MERKLETX_H

#include "primitives/transaction.h"
#include "primitives/block.h"
#include "uint256.h"
#include "amount.h"


// Forward declarations
class CValidationState;
class CBlockIndex;

/** A transaction with a merkle branch linking it to the block chain. */
class CMerkleTx : public CTransaction
{
private:
    /** Constant used in hashBlock to indicate tx has been abandoned */
    static const uint256 ABANDON_HASH;
    int GetDepthInMainChainINTERNAL(const CBlockIndex* &pindexRet) const;
    mutable bool fMerkleVerified;  // I see this used in GetDepthInMainChainINTERNAL

    

public:
    uint256 hashBlock;
    std::vector<uint256> vMerkleBranch;
    int nIndex;

    CMerkleTx()
    {
        Init();
    }

    CMerkleTx(const CTransaction& txIn) : CTransaction(txIn)
    {
        Init();
    }

    void Init()
    {
        hashBlock = uint256();
        nIndex = -1;
    }

    template<typename Stream>
    void Serialize(Stream& s, int nType, int nVersion) const {
        // Serialize base transaction first
        CTransaction::Serialize(s, nType, nVersion);
        
        // Then serialize merkle-specific data
        ::Serialize(s, hashBlock, nType, nVersion);
        ::Serialize(s, vMerkleBranch, nType, nVersion);
        ::Serialize(s, nIndex, nType, nVersion);
    }

    template<typename Stream>
    void Unserialize(Stream& s, int nType, int nVersion) {
        // Unserialize base transaction first
        CTransaction::Unserialize(s, nType, nVersion);
        
        // Then unserialize merkle-specific data
        ::Unserialize(s, hashBlock, nType, nVersion);
        ::Unserialize(s, vMerkleBranch, nType, nVersion);
        ::Unserialize(s, nIndex, nType, nVersion);
    }

   
    // Merkle methods
    int SetMerkleBranch(const CBlock& block);
    void SetMerkleBranch(const CBlockIndex* pindex, int posInBlock);
    void InitMerkleBranch(const CBlock& block, int posInBlock);

    // Chain verification methods
    int GetDepthInMainChain(const CBlockIndex* &pindexRet) const;
    int GetDepthInMainChain() const { const CBlockIndex *pindexRet; return GetDepthInMainChain(pindexRet); }
    bool IsInMainChain() const { const CBlockIndex *pindexRet; return GetDepthInMainChain(pindexRet) > 0; }
    int GetBlocksToMaturity() const;

    // Memory pool methods
    bool AcceptToMemoryPool(bool fLimitFree, bool fRejectInsaneFee);
    bool AcceptToMemoryPool(const CAmount& nAbsurdFee, CValidationState& state);

    // Utility methods
    bool hashUnset() const { return (hashBlock == uint256() || hashBlock == ABANDON_HASH); }
};

#endif // BITCOIN_PRIMITIVES_MERKLETX_H