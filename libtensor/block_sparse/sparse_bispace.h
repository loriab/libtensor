#ifndef LIBTENSOR_SPARSE_BISPACE_H
#define LIBTENSOR_SPARSE_BISPACE_H

#include <vector>
#include "../defs.h"
#include "../core/sequence.h"
#include "../core/permutation.h"
#include "runtime_permutation.h"
#include "sparse_block_tree.h"
#include "sparsity_expr.h"
#include "range.h"

//TODO REMOVE
#include <iostream>

namespace libtensor {

//Useful types
typedef std::vector<size_t> block_list;

//Forward declarations to dodge 'specialization after instantiation' compiler errors 
template<size_t N>
class sparse_bispace; 

//Utility functions

/**  One-dimensional sparse block index space
 **/
template<>
class sparse_bispace<1> {
private:
    size_t m_dim; //!< Number of elements
    std::vector<size_t> m_abs_indices; //!< Block absolute starting indices
    
    //Constructor used to instantiate 1d bispaces via the contraction of an index in a 2d bispace
    //Explicitly need this constructor because 1d sparse_block_trees do not implement contract() method
    //Implemented below to avoid incomplete type errors
    sparse_bispace(const sparse_bispace<2>& parent,size_t contract_idx);
public: 

    /** \brief Creates the sparse block %index space with a given dimension
        \param dim Number of elements in this space.
     **/
    explicit sparse_bispace(size_t dim);
    
    /** \brief Returns the dimension of the block index space 
     **/
    size_t get_dim() const;

    /** \brief Returns the number blocks into which this space has been split 
     **/
    size_t get_n_blocks() const;

    /** \brief Splits this space into blocks with offsets starting at offsets
               in split_points. First block always starts at zero
        \param split_points Iterable container of absolute indices where each block should start 
        \throw out_of_bounds If a split_point value exceeds the index limits, or if a zero length vector is passed 
     **/
    void split(const std::vector<size_t> &split_points)
        throw(out_of_bounds);

    /** \brief Returns the size of the block with block index block_idx
        \throw out_of_bounds If (# of blocks  - 1) < block_idx || block_idx < 0
     **/
    size_t get_block_size(size_t block_idx) const 
        throw(out_of_bounds);

    /** \brief Returns the absolute starting index of the block with block index block_idx
        \throw out_of_bounds If (# of blocks  - 1) < block_idx < 0
     **/
    size_t get_block_abs_index(size_t block_idx) const 
        throw(out_of_bounds);

    /** \brief Returns a 2d sparse_bispace composed of the two arguments
     **/
    sparse_bispace<2> operator|(const sparse_bispace<1>& rhs);

    /** \brief Returns a N+1 d sparse bispace
     *         Called during resolution of sparsity expressions
     **/
    template<size_t M>
    sparse_bispace<M+1> operator|(const sparse_bispace<M>& rhs);

    /** \brief Returns a sparsity_expr corresponding to a 2d bispace 
     **/
    sparsity_expr<1,1> operator%(const sparse_bispace<1>& rhs);

    /** \brief Returns a copy of this object 
        \throw out_of_bounds If an inappropriate index is specified 
     **/
    const sparse_bispace<1>& operator[](size_t  idx) const
        throw(out_of_bounds);

    /** \brief Returns offset of a given tile in this bispace. The tile is specified by a vector of block indices
     **/
    size_t get_block_offset(const std::vector<size_t>& block_indices) const;

    /** \brief Returns offset of a given tile in this bispace assuming canonical (row-major) layout. The tile is specified by a vector of block indices
     **/
    size_t get_block_offset_canonical(const std::vector<size_t>& block_indices) const;

    /** Stub methods for general compatibility, even though can't have sparse groups in a 1d bispace
     **/
    size_t get_n_sparse_groups() const { return 0; }
    const impl::sparse_block_tree_any_order& get_sparse_group_tree(size_t group_idx) const { throw bad_parameter(g_ns,"sparse_bispace<1>","get_sparse_group_tree(...)",__FILE__,__LINE__,"not implemented"); }
    const size_t get_sparse_group_offset(size_t group_idx) const { throw bad_parameter(g_ns,"sparse_bispace<1>","get_sparse_group_tree(...)",__FILE__,__LINE__,"not implemented"); }

    /** \brief Returns whether this object is equal to another. 
     *         Equality is defined to be the same dimension and block splitting pattern
     **/
    bool operator==(const sparse_bispace<1>& rhs) const;

    /** \brief Returns whether this object is not equal to another. 
     *         Equality is defined to be the same dimension and block splitting pattern
     **/
    bool operator!=(const sparse_bispace<1>& rhs) const;
};


inline sparse_bispace<1>::sparse_bispace(size_t dim) : m_dim(dim)
{ 
    m_abs_indices.push_back(0);
} 

inline size_t sparse_bispace<1>::get_dim() const
{
    return m_dim;
}

inline size_t sparse_bispace<1>::get_n_blocks() const
{
    return m_abs_indices.size();
}

inline void sparse_bispace<1>::split(const std::vector<size_t> &split_points) throw(out_of_bounds)
{
    if(split_points.size() < 1 || split_points.size() > (m_dim - 1))
    {
        throw out_of_bounds(g_ns,"sparse_bispace<1>","split(...)",
                __FILE__,__LINE__,"Must have 1 <= # of split points <= dim - 1"); 
    }

    for(int i = 0; i < split_points.size(); ++i)
    {
        size_t split_point = split_points[i];
        if(split_point > (m_dim - 1))
        {
            throw out_of_bounds(g_ns,"sparse_bispace<1>","split(...)",
                    __FILE__,__LINE__,"Split point indices cannot exceed (dim - 1)"); 
        }
        else if(split_point <= m_abs_indices.back())
        {
            throw out_of_bounds(g_ns,"sparse_bispace<1>","split(...)",
                    __FILE__,__LINE__,"Split point indices must be strictly increasing"); 
        }
        m_abs_indices.push_back(split_point);
    }
}

inline size_t sparse_bispace<1>::get_block_size(size_t block_idx) const throw(out_of_bounds)
{
    if(block_idx > (m_abs_indices.size() - 1))
    {
        throw out_of_bounds(g_ns,"sparse_bispace<1>","get_block_size(size_t block_idx)",
                __FILE__,__LINE__,"Cannot pass block_idx greater than (# of blocks - 1)"); 
    }
    else if(block_idx == (m_abs_indices.size() - 1))
    {
        return m_dim - m_abs_indices.back(); 
    }
    else
    {
        return m_abs_indices[block_idx + 1] - m_abs_indices[block_idx];
    }
}


inline size_t sparse_bispace<1>::get_block_abs_index(size_t block_idx) const throw(out_of_bounds)
{
    if(block_idx > (m_abs_indices.size() - 1))
    {
        throw out_of_bounds(g_ns,"sparse_bispace<1>","get_block_abs_index(size_t block_idx)",
                __FILE__,__LINE__,"Cannot pass block_idx greater than (# of blocks - 1)"); 
    }
    return m_abs_indices[block_idx];
}

    /** \brief Returns a copy of this object 
        \throw out_of_bounds If an inappropriate index is specified 
     **/
inline const sparse_bispace<1>& sparse_bispace<1>::operator[](size_t idx) const throw(out_of_bounds)
{
    if(idx != 0)
    {
        throw out_of_bounds(g_ns,"sparse_bispace<1>","operator[](...)",
                __FILE__,__LINE__,"Invalid subspace index specified (can only specify 0"); 
    }
    return *this;
}

/** \brief Returns offset of a given tile in this bispace. The tile is specified by a vector of block indices
 **/
inline size_t sparse_bispace<1>::get_block_offset(const std::vector<size_t>& block_indices) const
{
    if(block_indices.size() != 1)
    {
        throw out_of_bounds(g_ns,"sparse_bispace<1>","get_block_offset(...)",
                __FILE__,__LINE__,"vector passed with size != 1"); 
    }
    if(block_indices[0] > (m_abs_indices.size() - 1))
    {
        throw out_of_bounds(g_ns,"sparse_bispace<1>","get_block_offset(...)",
                __FILE__,__LINE__,"vector passed containing indices > max block idx"); 
    }
    return m_abs_indices[block_indices[0]];
}

inline size_t sparse_bispace<1>::get_block_offset_canonical(const std::vector<size_t>& block_indices) const
{
    return get_block_offset(block_indices); 
}

inline bool sparse_bispace<1>::operator==(const sparse_bispace<1>& rhs) const
{
    return (this->m_dim == rhs.m_dim) && (this->m_abs_indices == rhs.m_abs_indices);
}

inline bool sparse_bispace<1>::operator!=(const sparse_bispace<1>& rhs) const
{
    return ! (*this == rhs);
}

//General N-dimensional case
template<size_t N>
class sparse_bispace {
public: 
    static const char *k_clazz; //!< Class name
private:

    std::vector< sparse_bispace<1> > m_subspaces;

    //Contains the indices in m_subspaces at which each group of sparse coupled indices starts
    std::vector<size_t> m_sparse_indices_sets_offsets;

    //Contains the trees that describe the sparsity of each group of coupled indices  
    std::vector< impl::sparse_block_tree_any_order > m_sparse_block_trees;

    //Used to store the number of nonzero elements corresponding to the significant blocks in each tree
    //Used to construct m_dimensions
    std::vector<size_t> m_sparse_block_tree_dimensions; 

    //Internal-use array containing the dimension of each subspace/sparse composite subspace group. 
    //Used to calculate number of elements and offsets
    //Should never be edited directly - instead call init()
    std::vector<size_t> m_dimensions;

    //Internal use array for improving performance by pre-computing the inner size of each index
    std::vector<size_t> m_inner_sizes;
    
    //Used by get_block_offset to lookup offsets in sparse trees
    //Initialized only once by init() for performance
    std::vector< std::vector<size_t> > m_sparse_key_vecs;
    
    //Helper functions used to set the m_dimensions array used for calculating block offsets and nnz
    //Also initializes other sparsity data
    void init();
    
    //Used to initialize offsets stored in sparse_block_tree members
    size_t set_offsets(impl::sparse_block_tree_any_order& tree,const std::vector<size_t>& positions);

    /** \brief Constructors a composite sparse_bispace from two component spaces
     *         Used to implement operator|(...) between multi-dimensional spaces
     **/
    template<size_t L> 
    sparse_bispace(const sparse_bispace<N-L>& lhs,const sparse_bispace<L>& rhs);

    /** \brief Constructor used by the following operator pattern:
     *         <N> = <L> & <N - L> << { <N - L + 1> }
     **/
    template<size_t L>
    sparse_bispace(const sparse_bispace<N-L+1>& lhs,const std::vector< sparse_bispace<1> >& rhs_subspaces,const std::vector< sequence<L,size_t> >& sig_blocks);

    /** Used by contract() to produce lower-rank bispace
     **/
    sparse_bispace(const sparse_bispace<N+1>& parent,size_t contract_idx);   
    
    /** Used by fuse() to produce higher-rank bispace
     **/
    template<size_t L> 
    sparse_bispace(const sparse_bispace<N-L+1>& lhs, const sparse_bispace<L>& rhs);

    //Worker function used in implementing constructors
    //Special case for when RHS has no sparse members bcs 1d
    //Offset parameter does nothing, needed for proper overloading
    void absorb_sparsity(const sparse_bispace<1>& rhs,size_t offset=0); 

    //General case
    //offset specifies how much to shift the location of the specified sparse indices sets
    template<size_t L>
    void absorb_sparsity(const sparse_bispace<L>& rhs,size_t offset=0); 
public:

    /** \brief Returns the number of non-zero elements in this sparse bispace
     **/
    size_t get_nnz() const;

    /** \brief Combines the two operands to produce a new space
     **/
    template<size_t M> 
    sparse_bispace<N+M> operator|(const sparse_bispace<M>& rhs);

    /** \brief Retrieves the appropriate index subspace of this multidimensional space
        \throw out_of_bounds If an inappropriate index is specified 
     **/
    const sparse_bispace<1>& operator[](size_t  idx) const
        throw(out_of_bounds);

    /** \brief Returns offset of a given tile in this bispace assuming block-major layout. The tile is specified by a vector of block indices
     **/
    size_t get_block_offset(const std::vector<size_t>& block_indices);

    /** \brief Returns offset of a given tile in this bispace assuming canonical (row-major) layout. The tile is specified by a vector of block indices
     **/
    size_t get_block_offset_canonical(const std::vector<size_t>& block_indices) const;


    /** \brief Returns an appropriately permuted copy of this bispace 
     **/
    sparse_bispace<N> permute(const permutation<N>& perm) const; 

    /** \brief Returns the bispace resulting from the removal of a particular subspace and the
     *         subsequent aggregation of any sparsity involving that space
     **/
    sparse_bispace<N-1> contract(size_t contract_idx) const;


    template<size_t L>
    sparse_bispace<N+L-1> fuse(const sparse_bispace<L>& rhs) const;

    /** \brief Returns the number of sparse index groups (0 for fully dense)
     **/
    size_t get_n_sparse_groups() const { return m_sparse_block_trees.size(); }

    /** \brief Access the tree corresponding to sparse index group group_idx
     *         Two N-D spaces are equal if all of their subspaces are equal and in the same order
     **/
    const impl::sparse_block_tree_any_order& get_sparse_group_tree(size_t group_idx) const;

    /** \brief Get the subspace index corresponding to the beginning of a given sparsity coupled index group 
     **/
    const size_t get_sparse_group_offset(size_t group_idx) const;

    /** \brief Returns whether this object is equal to another of the same dimension. 
     *         Two N-D spaces are equal if all of their subspaces are equal and in the same order  
     **/
    bool operator==(const sparse_bispace<N>& rhs) const;

    /** \brief Returns whether this object is not equal to another of the same dimension. 
     *         Two N-D spaces are equal if all of their subspaces are equal and in the same order
     **/
    bool operator!=(const sparse_bispace<N>& rhs) const;


    //Friend all other types of sparse_bispaces to allow for creation of larger ones from smaller ones
    template<size_t M>
    friend class sparse_bispace;

    //Friend sparsity expr so it can build bispaces when expression is evaluated
    template<size_t L,size_t M>
    friend class sparsity_expr;
};

//Determines the dimensions of each sparsity group in the bispace
//bispaces not belonging to a group simply contribute their dense dimension
template<size_t N>
void sparse_bispace<N>::init()
{
    //Should only be called once
    if(m_dimensions.size() > 0)
    {
        throw bad_parameter(g_ns,"sparse_bispace<N>","init(...)",
            __FILE__,__LINE__,"init should only be called once");
    }

    size_t subspace_idx = 0; 
    size_t cur_group_idx = 0;
    while(subspace_idx < N)
    {
        //Anything sparse in this bispace?
        bool treat_as_sparse = false;
        if(cur_group_idx < m_sparse_indices_sets_offsets.size())
        {
            //Are we in a sparse group?
            if(subspace_idx == m_sparse_indices_sets_offsets[cur_group_idx])
            {
                treat_as_sparse = true;
            }
        }

        if(treat_as_sparse)
        {
            //We are in a sparse group, use the total group size
            m_dimensions.push_back(m_sparse_block_tree_dimensions[cur_group_idx]);
            subspace_idx += m_sparse_block_trees[cur_group_idx].get_order();
            ++cur_group_idx;
        }
        else
        {
            m_dimensions.push_back(m_subspaces[subspace_idx].get_dim());
            ++subspace_idx;
        }
    }
    
    //Initialize the sparse key vectors for each tree only once to save performance
    for(size_t group_idx = 0; group_idx < m_sparse_block_trees.size(); ++group_idx)
    {
        const impl::sparse_block_tree_any_order& sbt = m_sparse_block_trees[group_idx];
        size_t cur_order = sbt.get_order();
        m_sparse_key_vecs.push_back(std::vector<size_t>(cur_order));
    }
    
    //Precompute inner sizes
    m_inner_sizes.resize(m_dimensions.size());
    for(size_t inner_size_idx = 0; inner_size_idx < m_dimensions.size(); ++inner_size_idx)
    {
        size_t inner_size = 1;
        for(size_t factor_idx = inner_size_idx+1; factor_idx < m_dimensions.size(); ++factor_idx)
        {
            inner_size *= m_dimensions[factor_idx];
        }
        m_inner_sizes[inner_size_idx] = inner_size;
    }
}

//Worker function used in implementing constructors
//Special case for when RHS has no sparse members bcs 1d
template<size_t N>
void sparse_bispace<N>::absorb_sparsity(const sparse_bispace<1>& rhs,size_t offset)
{
}

//General case
template<size_t N> template<size_t L>
void sparse_bispace<N>::absorb_sparsity(const sparse_bispace<L>& rhs,size_t offset)
{
    //Copy sparsity
    for(size_t i = 0; i < rhs.m_sparse_indices_sets_offsets.size(); ++i)
    {
        m_sparse_indices_sets_offsets.push_back(rhs.m_sparse_indices_sets_offsets[i]+offset);
        m_sparse_block_trees.push_back(rhs.m_sparse_block_trees[i]);
        m_sparse_block_tree_dimensions.push_back(rhs.m_sparse_block_tree_dimensions[i]);
    }
}

//Constructor used by the following pattern: 
//  <N> = <N-L> | <L> pattern
//
template<size_t N> template<size_t L> 
sparse_bispace<N>::sparse_bispace(const sparse_bispace<N-L>& lhs,const sparse_bispace<L>& rhs)
{
    //Copy subspaces
    m_subspaces.reserve(N);
    for(size_t i = 0; i < N-L; ++i)
    {
        m_subspaces.push_back(lhs[i]);
    }
    for(size_t i = 0; i < L; ++i)
    {
        m_subspaces.push_back(rhs[i]);
    }

    absorb_sparsity(lhs);
    absorb_sparsity(rhs,N-L);

    init();
}

//Constructor used by the following operator pattern:
//  <N> = <L> & <N - L> << { <N - L + 1> }
template<size_t N> template<size_t L>
sparse_bispace<N>::sparse_bispace(const sparse_bispace<N-L+1>& lhs,const std::vector< sparse_bispace<1> >& rhs_subspaces,const std::vector< sequence<L,size_t> >& sig_blocks)
{
    //Copy subspaces
    for(size_t i = 0; i < (N-L+1); ++i)
    {
        m_subspaces.push_back(lhs[i]);
    }

    for(size_t i = 0; i < (L-1); ++i)
    {
        m_subspaces.push_back(rhs_subspaces[i]);
    }

    //Initialize the new sparse_block_tree with offset information  
    impl::sparse_block_tree<L> sbt(sig_blocks,m_subspaces);
    std::vector<size_t>  positions(L);
    for(size_t i = 0; i < L; ++i)
    {
        positions[i] = N-L+i;
    }

    m_sparse_block_tree_dimensions.push_back(sbt.get_nnz());
    m_sparse_block_trees.push_back(sbt);
    m_sparse_indices_sets_offsets.push_back(N-L);

    init();
}


template<size_t N>
size_t sparse_bispace<N>::get_nnz() const
{
    //TODO: Case where all elements are zero, fully sparse tensors????
    size_t nnz = 1;
    for(int i = 0; i < m_dimensions.size(); ++i)
    {
        nnz *= m_dimensions[i];
    }
    return nnz;
}

template<size_t N> template<size_t M> 
sparse_bispace<N+M> sparse_bispace<N>::operator|(const sparse_bispace<M>& rhs)
{
    return sparse_bispace<N+M>(*this,rhs);
}

//TODO: Should make these check (N-1) instead of m_subspaces.size()
template<size_t N>
const sparse_bispace<1>& sparse_bispace<N>::operator[](size_t idx) const throw(out_of_bounds)
{
    if(idx > (m_subspaces.size() - 1))
    {
        throw out_of_bounds(g_ns,"sparse_bispace<N>","operator[](...)",
                __FILE__,__LINE__,"idx > (# of subspaces - 1) was specified"); 
    }
    return m_subspaces[idx];
}

/** \brief Returns offset of a given tile in this bispace. The tile is specified by a vector of block indices
 **/
template<size_t N>
size_t sparse_bispace<N>::get_block_offset(const std::vector<size_t>& block_indices)
{
    //We process the blocks in chunks corresponding to each set that is coupled by sparsity
    size_t offset = 0;
    size_t outer_size = 1;
    size_t subspace_idx = 0;
    size_t cur_group_idx = 0;
    size_t cur_sparse_indices_set_idx = 0; 
    size_t abs_index;
    while(subspace_idx < block_indices.size())
    {
        //The outer size will be scaled by a factor corresponding to the size of
        //a single block dimension in the dense case or multiple in the sparse case
        size_t outer_size_scale_fac;

        bool treat_as_sparse = false;
        if(cur_sparse_indices_set_idx < m_sparse_indices_sets_offsets.size())
        {
            if(subspace_idx == m_sparse_indices_sets_offsets[cur_sparse_indices_set_idx])
            {
                treat_as_sparse = true;
            }
        }
        if(treat_as_sparse)
        {
            //Get the current key
            const impl::sparse_block_tree_any_order& sbt = m_sparse_block_trees[cur_sparse_indices_set_idx];
            size_t cur_order = sbt.get_order();
            std::vector<size_t>& cur_key = m_sparse_key_vecs[cur_sparse_indices_set_idx];
            for(size_t key_idx = 0; key_idx < cur_order; ++key_idx)
            {
                cur_key[key_idx] = block_indices[subspace_idx+key_idx];
            }

            //Outer size consists of the size of all blocks involved in this sparse group
            outer_size_scale_fac = 1;
            for(size_t outer_size_idx = 0; outer_size_idx < cur_order; ++outer_size_idx)
            {
                outer_size_scale_fac *= m_subspaces[subspace_idx+outer_size_idx].get_block_size(cur_key[outer_size_idx]);
            }
            abs_index = (*sbt.search(cur_key))[0].first;
            ++cur_sparse_indices_set_idx;
            subspace_idx += cur_order;
        }
        else
        {
            //Treat as dense (1 element chunk)
            size_t block_idx = block_indices[subspace_idx];
            abs_index = m_subspaces[subspace_idx].get_block_abs_index(block_idx);
            outer_size_scale_fac = m_subspaces[subspace_idx].get_block_size(block_idx);
            ++subspace_idx;
        }
        
        offset += outer_size * abs_index * m_inner_sizes[cur_group_idx];
        outer_size *= outer_size_scale_fac;
        ++cur_group_idx;
    }

    return offset;
}

template<size_t N>
size_t sparse_bispace<N>::get_block_offset_canonical(const std::vector<size_t>& block_indices) const
{
    size_t offset = 0; 
    for(size_t i = 0; i < block_indices.size(); ++i)
    {
        size_t inner_size = 1;
        for(size_t inner_size_idx = i+1; inner_size_idx < N; ++inner_size_idx)
        {
            //TODO: This needs to call some sparsity-aware function to determine this
            inner_size *= m_subspaces[inner_size_idx].get_dim();
        }
        offset += m_subspaces[i].get_block_abs_index(block_indices[i])*inner_size;
    }
    return offset;
}

template<size_t N> 
sparse_bispace<N> sparse_bispace<N>::permute(const permutation<N>& perm) const
{
    sparse_bispace<N> copy(*this);

    //Permute subspaces
    for(size_t i = 0; i < N; ++i)
    {
        copy.m_subspaces[i] = m_subspaces[perm[i]];
    }

    //Permute trees
    for(size_t i = 0; i < m_sparse_indices_sets_offsets.size(); ++i)
    {
        //Does the permutation apply to this sparse tree
        size_t sparse_set_offset = m_sparse_indices_sets_offsets[i];
        size_t order = m_sparse_block_trees[i].get_order();

        size_t lower_bound = sparse_set_offset;
        size_t upper_bound = (sparse_set_offset+order-1);

        //Convert the parts of the permutation that apply to this tree into 
        //tree-relative indices
        std::vector<size_t> perm_entries;
        std::vector< sparse_bispace<1> > tree_subspaces;
        for(size_t order_idx = 0; order_idx < order; ++order_idx)
        {
            //Where do 
            size_t dest_idx = sparse_set_offset+order_idx;
            size_t src_idx = perm[dest_idx];
            //Should support this eventually, but for now throw if idx 
            if(src_idx < lower_bound || src_idx > upper_bound)
            {
                throw bad_parameter(g_ns,"sparse_bispace<N>","permute(...)",
                    __FILE__,__LINE__,"permutation breaks up sparse tuple"); 
            }
            size_t rel_idx = src_idx - sparse_set_offset;
            perm_entries.push_back(rel_idx);

            //We have ALREADY permuted m_subspaces, so we must save the ORIGINAL POSITION, not the DEST
            tree_subspaces.push_back(m_subspaces[src_idx]);
        }

        runtime_permutation tree_perm(perm_entries);
        //Don't permute if identity
        if(tree_perm != runtime_permutation(order))
        {
            copy.m_sparse_block_trees[i] = m_sparse_block_trees[i].permute(tree_perm);
            copy.m_sparse_block_trees[i].set_offsets_sizes_nnz(tree_subspaces);
        }
    }
    return copy;
}

template<size_t N>
sparse_bispace<N>::sparse_bispace(const sparse_bispace<N+1>& parent,size_t contract_idx) 
{

    //Extract all relevant subspaces
    for(size_t i = 0; i < N+1; ++i)
    {
        if(i == contract_idx)
        {
            continue;
        }
        m_subspaces.push_back(parent.m_subspaces[i]);
    }

    //Contract sparse information appropriately, by default just copying unaffected trees
    for(size_t group_idx = 0; group_idx < parent.m_sparse_block_trees.size(); ++group_idx)
    {
        const impl::sparse_block_tree_any_order& cur_tree = parent.m_sparse_block_trees[group_idx];
        size_t offset =   parent.m_sparse_indices_sets_offsets[group_idx];
        size_t order = cur_tree.get_order();

        //Does contraction shift the offset of this tree?
        size_t new_group_offset = offset > contract_idx ? offset-1 : offset;
        
        //Are we contracting this tree?
        if((offset <= contract_idx) && (contract_idx < offset+order))
        {
            //Will contraction wipe out this sparsity?
            if(order == 2)
            {
                continue;
            }
            else
            {

                //What are the subspaces relevant to this tree?
                std::vector< sparse_bispace<1> > tree_subspaces;
                std::vector<size_t> positions;
                for(size_t subspace_idx = new_group_offset; subspace_idx < new_group_offset+order-1; ++subspace_idx)
                {
                    tree_subspaces.push_back(m_subspaces[subspace_idx]);
                    positions.push_back(subspace_idx);
                }

                //Tree-relative idx
                size_t rel_idx = contract_idx - offset;
                impl::sparse_block_tree_any_order new_tree = cur_tree.contract(rel_idx,tree_subspaces);
                m_sparse_block_tree_dimensions.push_back(new_tree.get_nnz());
                m_sparse_block_trees.push_back(new_tree);
                m_sparse_indices_sets_offsets.push_back(new_group_offset);
            }
        }
        else
        {
            m_sparse_indices_sets_offsets.push_back(new_group_offset);
            m_sparse_block_trees.push_back(cur_tree);
            m_sparse_block_tree_dimensions.push_back(parent.m_sparse_block_tree_dimensions[group_idx]);
        }
    }

    init();
}

template<size_t N>
sparse_bispace<N-1> sparse_bispace<N>::contract(size_t contract_idx) const
{
    if(contract_idx > N-1)
    {
        throw bad_parameter(g_ns,"sparse_bispace<N>","contract(...)",
            __FILE__,__LINE__,"contraction index too large"); 
    }
    return sparse_bispace<N-1>(*this,contract_idx);
}

template<size_t N> template<size_t L> 
sparse_bispace<N>::sparse_bispace(const sparse_bispace<N-L+1>& lhs, const sparse_bispace<L>& rhs)
{
    //Copy all subspaces,skipping the shared one
    for(size_t i = 0; i < N - L + 1; ++i)
    {
        m_subspaces.push_back(lhs.m_subspaces[i]);
    }
    for(size_t i = 1; i < L; ++i)
    {
        m_subspaces.push_back(rhs.m_subspaces[i]);
    }

    //We actually need to fuse sparse trees if there is overlap at the end of lhs and beginning of rhs
    bool fuse_sparsity = false;
    if(lhs.m_sparse_indices_sets_offsets.size() > 0)
    {
        size_t last_lhs_group_offset  = lhs.m_sparse_indices_sets_offsets.back();
        size_t last_lhs_group_order =  lhs.m_sparse_block_trees.back().get_order();
        size_t lhs_sparsity_end = last_lhs_group_offset + last_lhs_group_order;
        if((lhs_sparsity_end == (N-L+1)) && rhs.m_sparse_indices_sets_offsets[0] == 0)
        {
            fuse_sparsity = true;
        }
    }

    //Absorb all the sparsity, then patch it up later to account for fusion
    absorb_sparsity(lhs);
    absorb_sparsity(rhs,N-L);
    
    //Patch up the sparsity to account for fusion if appropriate
    if(fuse_sparsity)
    {
        size_t last_lhs_tree_idx = lhs.m_sparse_block_trees.size() - 1;
        size_t first_rhs_tree_idx = last_lhs_tree_idx + 1;
        m_sparse_block_trees[last_lhs_tree_idx] = m_sparse_block_trees[last_lhs_tree_idx].fuse(m_sparse_block_trees[first_rhs_tree_idx]);
        m_sparse_block_trees[last_lhs_tree_idx].set_offsets_sizes_nnz(m_subspaces);

        //delete the no longer needed rhs tree information
        m_sparse_indices_sets_offsets.erase(m_sparse_indices_sets_offsets.begin() + first_rhs_tree_idx);
        m_sparse_block_trees.erase(m_sparse_block_trees.begin() + first_rhs_tree_idx);
        m_sparse_block_tree_dimensions.erase(m_sparse_block_tree_dimensions.begin() + first_rhs_tree_idx); 
    }

    init();
}

template<size_t N> template<size_t L> 
sparse_bispace<N+L-1> sparse_bispace<N>::fuse(const sparse_bispace<L>& rhs) const
{
    if(m_subspaces[N-1] != rhs[0])
    {
        throw bad_parameter(g_ns,"sparse_bispace<N>","fuse(...)",
            __FILE__,__LINE__,"fuse point doesn't match"); 
    }
    return sparse_bispace<N+L-1>(*this,rhs);
}

template<size_t N>
const impl::sparse_block_tree_any_order& sparse_bispace<N>::get_sparse_group_tree(size_t group_idx) const
{
    if(group_idx > (m_sparse_block_trees.size() - 1))
    {
        throw bad_parameter(g_ns,"sparse_bispace<N>","get_sparse_group_tree(...)",
            __FILE__,__LINE__,"group_idx too large"); 
    }
    return m_sparse_block_trees[group_idx];
}

template<size_t N>
const size_t sparse_bispace<N>::get_sparse_group_offset(size_t group_idx) const
{
    if(group_idx > (m_sparse_block_trees.size() - 1))
    {
        throw bad_parameter(g_ns,"sparse_bispace<N>","get_sparse_group_tree(...)",
            __FILE__,__LINE__,"group_idx too large"); 
    }
    return m_sparse_indices_sets_offsets[group_idx];
}



template<size_t N>
bool sparse_bispace<N>::operator==(const sparse_bispace<N>& rhs) const
{
    //Check that subspaces are equivalent
    for(int i = 0; i < N; ++i)
    {
        if(m_subspaces[i] != rhs.m_subspaces[i])
        {
            return false;
        }
    }

    //Check that trees are equivalent
    if(m_sparse_indices_sets_offsets.size() != rhs.m_sparse_indices_sets_offsets.size())
    {
        return false;
    }
    for(size_t i = 0; i < m_sparse_indices_sets_offsets.size(); ++i)
    {
        if(m_sparse_indices_sets_offsets[i] != rhs.m_sparse_indices_sets_offsets[i])
        {
            return false;
        }
    }

    for(size_t i = 0; i < m_sparse_block_trees.size(); ++i)
    {
        if(m_sparse_block_trees[i] != rhs.m_sparse_block_trees[i])
        {
            return false;
        }
    }

    return true;
}

template<size_t N>
bool sparse_bispace<N>::operator!=(const sparse_bispace<N>& rhs) const
{
    return !(*this == rhs);
}

template<size_t N>
const char *sparse_bispace<N>::k_clazz = "sparse_bispace<N>";

//These two methods require the definition of sparse_bispace<2>, so they are stuck down here
inline sparse_bispace<1>::sparse_bispace(const sparse_bispace<2>& parent,size_t contract_idx)
{
    const sparse_bispace<1>& target = parent.m_subspaces[contract_idx ? 1 : 0];
    m_dim = target.get_dim();
    m_abs_indices.reserve(target.m_abs_indices.size());
    for(size_t i = 0;  i < target.m_abs_indices.size(); ++i)
    {
        m_abs_indices[i] = target.m_abs_indices[i];
    }
}

inline sparse_bispace<2> sparse_bispace<1>::operator|(const sparse_bispace<1>& rhs)
{
    return sparse_bispace<2>(*this,rhs);
}

template<size_t M>
inline sparse_bispace<M+1> sparse_bispace<1>::operator|(const sparse_bispace<M>& rhs)
{
    return sparse_bispace<M+1>(*this,rhs);
}

inline sparsity_expr<1,1> sparse_bispace<1>::operator%(const sparse_bispace<1>& rhs)
{
    return sparsity_expr<1,1>(*this,rhs);
}

//Implementation of methods in sparsity_expr and sparse_block_tree requiring sparse_bispace definition
    
//Internal method for recursively constructing a list of all subspaces
template<size_t M>
void sparsity_expr<M,1>::retrieve_subspaces(std::deque< sparse_bispace<1> >& subspaces) const
{
    subspaces.push_front(m_cur_subspace);
}
    
template<size_t M,size_t N>
void sparsity_expr<M,N>::retrieve_subspaces(std::deque< sparse_bispace<1> >& subspaces) const
{
    subspaces.push_front(m_cur_subspace);
    m_sub_expr.retrieve_subspaces(subspaces);
}
    
template<size_t M>
sparse_bispace<2> sparsity_expr<M,1>::operator<<(const std::vector< sequence<2,size_t> >& sig_blocks)
{
    return sparse_bispace<2>(m_parent_bispace,std::vector< sparse_bispace<1> >(1,m_cur_subspace),sig_blocks);
}

template<size_t M,size_t N>
sparse_bispace<M+N> sparsity_expr<M,N>::operator<<(const std::vector< sequence<N+1,size_t> >& sig_blocks)
{

    std::deque< sparse_bispace<1> > subspaces;
    retrieve_subspaces(subspaces);

    return sparse_bispace<M+N>(m_parent_bispace,std::vector< sparse_bispace<1> >(subspaces.begin(),subspaces.end()),sig_blocks);
}

//TODO: Make this immutable, etc?? Need to make this class hard to abuse
//Type erasure class for sparse_bispaces.
//Used in functions that can take a number of sparse bispaces of different order as arguments
class sparse_bispace_any_order
{
private:
    class sparse_bispace_generic_i {
    public:
        virtual const sparse_bispace<1>& operator[](size_t idx) const = 0; 
        virtual size_t get_order() const = 0;
        virtual size_t get_block_offset(const std::vector<size_t>& block_indices) = 0;
        virtual size_t get_block_offset_canonical(const std::vector<size_t>& block_indices) const = 0;
        virtual sparse_bispace_generic_i* clone() const = 0;
        virtual size_t get_n_sparse_groups() const  = 0;
        virtual impl::sparse_block_tree_any_order get_sparse_group_tree(size_t group_idx) const  = 0;
        virtual size_t get_sparse_group_offset(size_t group_idx) const = 0; 

        virtual bool equals(const sparse_bispace_generic_i* rhs) const = 0;

        virtual ~sparse_bispace_generic_i() { };
    };

    template<size_t N>
    class sparse_bispace_generic_wrapper : public sparse_bispace_generic_i {
    private:
        sparse_bispace<N> m_bispace;
    public:
        sparse_bispace_generic_wrapper(const sparse_bispace<N>& bispace) : m_bispace(bispace) {};

        const sparse_bispace<1>& operator[](size_t idx) const { return m_bispace[idx]; }
        size_t get_order() const { return N; }
        size_t get_block_offset(const std::vector<size_t>& block_indices) { return m_bispace.get_block_offset(block_indices); }
        size_t get_block_offset_canonical(const std::vector<size_t>& block_indices) const { return m_bispace.get_block_offset_canonical(block_indices); }
        size_t get_n_sparse_groups() const  { return m_bispace.get_n_sparse_groups(); }
        impl::sparse_block_tree_any_order get_sparse_group_tree(size_t group_idx) const { return m_bispace.get_sparse_group_tree(group_idx); };
        size_t get_sparse_group_offset(size_t group_idx) const { return m_bispace.get_sparse_group_offset(group_idx); }

        //Same order is assured upstream
        bool equals(const sparse_bispace_generic_i* rhs) const { return m_bispace == static_cast< const sparse_bispace_generic_wrapper<N>* >(rhs)->m_bispace; }


        sparse_bispace_generic_i* clone() const { return new sparse_bispace_generic_wrapper(m_bispace); }
    };

    sparse_bispace_generic_i* m_spb_ptr;
    
    //This needs to exist given the current implementation of sequence<>, so we hack and hide it
    sparse_bispace_any_order() { m_spb_ptr = NULL; };
public:
    
    //Constructor
    template<size_t N>
    sparse_bispace_any_order(const sparse_bispace<N>& bispace) { m_spb_ptr = new sparse_bispace_generic_wrapper<N>(bispace); }

    //Copy constructor
    sparse_bispace_any_order(const sparse_bispace_any_order& rhs) { rhs.m_spb_ptr ? m_spb_ptr = rhs.m_spb_ptr->clone() : m_spb_ptr = 0; }

    //Overloaded assignment operator
    sparse_bispace_any_order& operator=(const sparse_bispace_any_order& rhs) { if(m_spb_ptr) { delete m_spb_ptr; } rhs.m_spb_ptr ? m_spb_ptr = rhs.m_spb_ptr->clone() : m_spb_ptr = 0; return *this; }

    const sparse_bispace<1>& operator[](size_t idx) const { return (*m_spb_ptr)[idx]; }
    size_t get_order() const { return m_spb_ptr->get_order(); }
    size_t get_block_offset(const std::vector<size_t>& block_indices) { return m_spb_ptr->get_block_offset(block_indices); }
    size_t get_block_offset_canonical(const std::vector<size_t>& block_indices) const { return m_spb_ptr->get_block_offset_canonical(block_indices); }
    size_t get_n_sparse_groups() const { return m_spb_ptr->get_n_sparse_groups(); }
    impl::sparse_block_tree_any_order get_sparse_group_tree(size_t group_idx) const { return m_spb_ptr->get_sparse_group_tree(group_idx); }
    size_t get_sparse_group_offset(size_t group_idx) const { return m_spb_ptr->get_sparse_group_offset(group_idx); } 


    //We have to check NULL bcs of stupid default constructor hack
    virtual ~sparse_bispace_any_order() { if(m_spb_ptr != NULL) { delete m_spb_ptr; } };

    bool operator==(const sparse_bispace_any_order& rhs) const { if(get_order() != rhs.get_order()) { return false; } return m_spb_ptr->equals(rhs.m_spb_ptr); }
    bool operator!=(const sparse_bispace_any_order& rhs) const { return !(*this == rhs); }

    //For default constructor hack
    template<size_t N,typename T>
    friend class sequence;
};

} // namespace libtensor

#endif // LIBTENSOR_SPARSE_BISPACE_H
