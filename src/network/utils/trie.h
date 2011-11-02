/* -*- Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2011 University of California, Los Angeles
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef _TRIE_H_
#define _TRIE_H_

#ifndef __GNUC__
#error "gcc is required to compile this code"
#else

#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/trie_policy.hpp>
#include <ext/pb_ds/tag_and_trait.hpp>

#if __GNUC__ < 4 || (__GNUC__==4 && __GNUC_MINOR__<=1)
#error "gcc version at least 4.1 is required to compile this code"
#elif (__GNUC__==4 && __GNUC_MINOR__<3)
#define PB_DS   pb_ds
#else
#define PB_DS   __gnu_pbds
#endif

#endif // __GNUC__

/**
 * At least for now, trie implementation is based on gcc policy-based data containers
 *
 * Important!!! This trie simplifies/optimizes work with IPv4 prefixes.
 * There is an assumption that nobody will use IP netmasks other than 255.255.255.0, 255.255.0.0, 255.0.0.0, or 0.0.0.0.
 * As a result, branching happens on a byte level, not on bit a level.
 */

namespace ns3 {

/**
 * \brief IPv4 version of element-access traits for patricia trie
 * 
 * Refer to http://gcc.gnu.org/onlinedocs/libstdc++/ext/pb_ds/trie_based_containers.html
 */
class Ipv4AddressTrieEAccessTraits
{
public:
  // Size type.
  typedef size_t size_type;

  // Key type.
  typedef Ipv4Address key_type;

  // Const key reference type.
  typedef const Ipv4Address & const_key_reference;

  // Element const iterator type.
  typedef const uint8_t * const_iterator;

  // Element type.
  typedef uint8_t e_type;

  enum
    {
      min_e_val = 0,
      max_e_val = 255,
      max_size = max_e_val - min_e_val + 1
    };

public:
  // Returns a const_iterator to the first element of r_key.
  inline static const_iterator
  begin (const_key_reference r_key);

  // Returns a const_iterator to the after-last element of r_key.
  inline static const_iterator
  end (const_key_reference r_key);

  // Maps an element to a position.
  inline static size_type
  e_pos (e_type e);
};

Ipv4AddressTrieEAccessTraits::const_iterator
Ipv4AddressTrieEAccessTraits::begin (Ipv4AddressTrieEAccessTraits::const_key_reference r_key)
{
  return reinterpret_cast<Ipv4AddressTrieEAccessTraits::const_iterator> (&r_key.Get ());
}

Ipv4AddressTrieEAccessTraits::const_iterator
Ipv4AddressTrieEAccessTraits::end (Ipv4AddressTrieEAccessTraits::const_key_reference r_key)
{
  return sizeof (r_key.Get ()) + reinterpret_cast<Ipv4AddressTrieEAccessTraits::const_iterator> (&r_key.Get ());
}

Ipv4AddressTrieEAccessTraits::size_type
Ipv4AddressTrieEAccessTraits::e_pos (Ipv4AddressTrieEAccessTraits::e_type e)
{
  return e;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#include <ext/pb_ds/detail/trie_policy/trie_policy_base.hpp>

#define PB_DS_BASE_C_DEC \
  PB_DS::detail::trie_policy_base<Const_Node_Iterator,Node_Iterator,E_Access_Traits,Allocator>

template<typename Const_Node_Iterator,
         class Node_Iterator,
         class E_Access_Traits,
         class Allocator>
class TrieLongestPrefixMatch : private PB_DS_BASE_C_DEC
{
private:
  typedef PB_DS_BASE_C_DEC base_type;
public:
  typedef typename base_type::key_type key_type;
  typedef typename base_type::const_key_reference const_key_reference;

  // Element access traits.
  typedef E_Access_Traits e_access_traits;

  // Const element iterator.
  typedef typename e_access_traits::const_iterator const_e_iterator;

  // Allocator type.
  typedef Allocator allocator;
  
  // Size type.
  typedef typename allocator::size_type size_type;
  typedef PB_DS::detail::null_node_metadata metadata_type;
  typedef Const_Node_Iterator const_node_iterator;
  typedef Node_Iterator node_iterator;
  typedef typename const_node_iterator::value_type const_iterator;
  typedef typename node_iterator::value_type iterator;

  inline const_iterator
  longest_prefix_match (const_key_reference r_key)
  {
    return longest_prefix_match (
                                 get_e_access_traits().begin (r_key),
                                 get_e_access_traits().end (r_key));
  }
  
  inline const_iterator
  longest_prefix_match (typename e_access_traits::const_iterator b, typename e_access_traits::const_iterator e)
  {
    if (empty())
      return (end());

    node_iterator nd_it = node_begin ();
    node_iterator end_nd_it = node_end ();

    const e_access_traits& r_traits = get_e_access_traits ();
    const size_type given_range_length = std::distance (b, e);

    while (true)
      {
        if (nd_it == end_nd_it) return end ();
        
        const size_type common_range_length =
          base_type::common_prefix_len (nd_it, b, e, r_traits);

        if (common_range_length >= given_range_length) // not sure what this condition is for...
          {
            return rightmost_it (nd_it);
          }

        nd_it = next_child (nd_it, b, e, end_nd_it, r_traits);
      }
  }
  
protected:
  inline void
  operator()(node_iterator node_it, const_node_iterator end_nd_it) const
  {
  }

private:
  // Returns true if the container is empty.
  virtual bool
  empty() const = 0;

  // Returns the const iterator associated with the just-after last element.
  virtual const_iterator
  end() const = 0;

  // Returns the iterator associated with the just-after last element.
  virtual iterator
  end() = 0;

  // Returns the const_node_iterator associated with the trie's root node.
  virtual const_node_iterator
  node_begin() const = 0;

  // Returns the node_iterator associated with the trie's root node.
  virtual node_iterator
  node_begin() = 0;

  // Returns the const_node_iterator associated with a just-after leaf node.
  virtual const_node_iterator
  node_end() const = 0;

  // Returns the node_iterator associated with a just-after leaf node.
  virtual node_iterator
  node_end() = 0;

  // Access to the cmp_fn object.
  virtual const e_access_traits&
  get_e_access_traits() const = 0;

  node_iterator
  next_child(node_iterator nd_it, const_e_iterator b, const_e_iterator e,
             node_iterator end_nd_it, const e_access_traits &r_traits)
  {
    const size_type num_children = nd_it.num_children();

    node_iterator ret = end_nd_it;
    size_type max_length = 0;

    for (size_type i = 0; i < num_children; ++i)
      {
        node_iterator pot = nd_it.get_child (i);

        const size_type common_range_length =
          PB_DS_BASE_C_DEC::common_prefix_len (pot, b, e, r_traits);

        if (common_range_length > max_length)
          {
            ret = pot;

            max_length = common_range_length;
          }
      }

    return (ret);    
  }
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


template<class MappedValue>
class Ipv4AddressTrie : public PB_DS::trie<Ipv4Address,MappedValue,Ipv4AddressTrieEAccessTraits,
                                           PB_DS::pat_trie_tag,TrieLongestPrefixMatch>
{
};

} // namespace ns3

#endif // _TRIE_H_
