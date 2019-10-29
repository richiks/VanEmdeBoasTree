/**
 * @headerfile VanEmdeBoasTree.h
 * @author: Richik Vivek Sen (rsen9@gatech.edu)
 * @date 10/28/2019
 * @brief Header file for VanEmdeBoasTree.cpp
 */

#ifndef VANEMDEBOASTREE_H
#define VANEMDEBOASTREE_H

#include <utility>  // For pair
#include <iterator> // For iterator, bidirectional_iterator_tag, reverse_iterator
#include <climits>  // For CHAR_BIT, ULONG_MAX

/**
 * A class representing a vEB-tree of unsigned shorts.
 */
class VanEmdeBoasTree {
public:
  /**
   * Constructor: VanEmdeBoasTree();
   * Usage: VanEmdeBoasTree myTree;
   * --------------------------------------------------------------------------
   * Constructs a new, empty vEB-tree.
   */
  VanEmdeBoasTree();

  /* Destructor: ~VanEmdeBoasTree();
   * Usage: (implicit)
   * --------------------------------------------------------------------------
   * Deallocates all memory allocated by the vEB-tree.
   */
  ~VanEmdeBoasTree();

  /**
   * Copy functions: VanEmdeBoasTree(const VanEmdeBoasTree& other);
   *                 VanEmdeBoasTree& operator= (const VanEmdeBoasTree& other);
   * Usage: VanEmdeBoasTree one = two;
   *        one = two;
   * --------------------------------------------------------------------------
   * Sets this VanEmdeBoasTree to be a deep-copy of some other vEB-tree.
   */
  VanEmdeBoasTree(const VanEmdeBoasTree& other);
  VanEmdeBoasTree& operator= (const VanEmdeBoasTree& other);

  /**
   * bool empty() const;
   * Usage: if (tree.empty()) { ... }
   * --------------------------------------------------------------------------
   * Returns whether this vEB contains no elements.
   */
  bool empty() const;

  /**
   * size_t size() const;
   * Usage: while (tree.size() > 1) { ... }
   * --------------------------------------------------------------------------
   * Returns the number of elements stored in the vEB-tree.
   */
  size_t size() const;

  /**
   * Type: const_iterator
   * --------------------------------------------------------------------------
   * A type representing an object that can visit but not modify the elements
   * of the vEB tree in sorted order.
   */
  class const_iterator;

  /**
   * const_iterator begin() const;
   * const_iterator end() const;
   * Usage: for (VanEmdeBoasTree::const_iterator itr = tree.begin();
   *             itr != tree.end(); ++itr) { ... }
   * --------------------------------------------------------------------------
   * Returns a range of iterators delineating the full contents of this
   * VanEmdeBoasTree.
   */
  const_iterator begin() const;
  const_iterator end() const;

  /**
   * Type: const_reverse_iterator
   * --------------------------------------------------------------------------
   * A type representing an object that can visit but not modify the elements
   * of the vEB tree in reverse sorted order.
   */
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  /**
   * const_reverse_iterator rbegin() const;
   * const_reverse_iterator rend() const;
   * Usage: for (VanEmdeBoasTree::const_reverse_iterator itr = tree.rbegin();
   *             itr != tree.rend(); ++itr) { ... }
   * --------------------------------------------------------------------------
   * Returns a range of iterators delineating the full contents of this
   * VanEmdeBoasTree in reverse order.
   */
  const_reverse_iterator rbegin() const;
  const_reverse_iterator rend() const;

  /**
   * const_iterator find(unsigned short value) const;
   * Usage: if (tree.find(137) != tree.end()) { ... }
   * --------------------------------------------------------------------------
   * Returns an iterator to the element in the tree with the specified value,
   * or end() as a sentinel if one does not exist.
   */
  const_iterator find(unsigned short value) const;

  /**
   * const_iterator predecessor(unsigned short value) const;
   * const_iterator successor(unsigned short value) const;
   * Usage: VanEmdeBoasTree::const_iterator itr = tree.predecessor(137);
   *        if (itr != end()) cout << *itr << endl;
   * --------------------------------------------------------------------------
   * predecessor returns an iterator to the first element in the tree whose
   * key is strictly less than the specified value (or end() if one does not
   * exist).  successor returns an iterator to the first element in the tree
   * whose key is strictly greater than the specified value (or end() if one
   * does not exist).
   */
  const_iterator predecessor(unsigned short value) const;
  const_iterator successor(unsigned short value) const;

  /**
   * std::pair<const_iterator, bool> insert(unsigned short value);
   * Usage: tree.insert(137);
   * --------------------------------------------------------------------------
   * Inserts the specified value into the vEB tree.  If the value did not
   * exist in the tree prior to the call, the return value is true paired with
   * an iterator to the element.  Otherwise, the return value is false paired
   * with an iterator to the value.
   */
  std::pair<const_iterator, bool> insert(unsigned short value);

  /**
   * bool erase(unsigned short value);
   * bool erase(const_iterator where);
   * Usage: tree.erase(137);  tree.erase(tree.begin());
   * --------------------------------------------------------------------------
   * Removes the element with the specified key (or the element indicated by
   * the specified const_iterator) from the vEB tree, returning whether the
   * element existed and was removed (true) or not.
   */
  bool erase(unsigned short value);
  bool erase(const_iterator where);

  /**
   * void swap(VanEmdeBoasTree& rhs);
   * Usage: tree.swap(otherTree);
   * --------------------------------------------------------------------------
   * Exchanges the contents of this vEB-tree and some other vEB-tree in O(1)
   * time and space.
   */
  void swap(VanEmdeBoasTree& rhs);

private:
  /* A type representing a vEB-tree structure.  It stores the min and max
   * elements at the current level of the tree, an array of pointers to smaller
   * vEB trees, and a pointer to a summary vEB tree.
   *
   * This vEB-tree implementation uses two major optimizations.  First, rather
   * than storing the complete tree structure, once the number of bits in
   * consideration becomes sufficiently small (say, such that the values are
   * only four bits long), we bottom out and use a bit array instead of the
   * standard implementation.  This saves an enormous amount of overhead, since
   * a bit array is substantially more compact than all of the necessary
   * pointers to sublevels.
   *
   * Second, because each vEB-tree node stores a fixed-sized array whose length
   * varies from level to level, we design the structure intending to store the
   * pointers to subtrees beyond the end of the struct by overallocating space
   * for it.  This is a standard optimization that avoids a lot of unnecessary
   * pointer indirections.
   *
   * This struct's layout is very brittle.  The position of the the first array
   * element must be the very last member, since the overallocated memory needs
   * to be flush against the array itself.
   */
  struct Node {
    /* The min and max values here. */
    unsigned short mMin, mMax;

    /* Whether either of these values are set. */
    bool mIsEmpty;

    /* A pointer to the summary structure.  This is typed as a void* because
     * at a certain point, this pointer will not point at a Node, but rather
     * at a block of raw memory acting as a bitvector.
     */
    void* mSummary;

    /* An array of one element, representing the first of (possibly) many
     * pointers to subtrees.  This MUST be the last element of the struct!
     * We use a void* here because this might actually be pointing at a bit
     * array, rather than another Node.
     */
    void* mChildren[1];

    /* Operator new overallocates the node to ensure space exists for the Node
     * pointers.  The argument to this function is the number of extra pointers
     * that should be allocated.
     */
    void* operator new (size_t size, size_t numPointers);

    /* Operator delete only needed in case the constructor throws.  There is
     * no constructor, so this is technically not needed, but in the interests
     * of forward-thinking we provide it anyway.
     */
    void operator delete (void* memory, size_t numBits);
  };

  /* A pointer to the root vEB-tree node. */
  void* mRoot;

  /* A cache of the size of the tree. */
  size_t mSize;

  /* Internally, this class uses size_t's to represent "either a valid unsigned
   * short or a sentinel indicating that nothing exists."  Thiss constant
   * represents the "not actually a value" term.
   */
  static const size_t kNil = ULONG_MAX;

  /* Make const_iterator a friend so it can access internal structure. */
  friend class const_iterator;

  /* Helper function to recursively construct a vEB-tree to hold the specified
   * number of bits.  Because this might just return a bit array, the function
   * returns a void*.
   */
  static void* recCreateTree(size_t numBits);

  /* Helper function to recursively clone a vEB-tree holding the specified
   * number of bits.
   */
  static void* recCloneTree(void* root, size_t numBits);

  /* Helper function to recursively destroy a vEB-tree of the specified number
   * of bits.
   */
  static void recDeleteTree(void* root, size_t numBits);

  /* Helper function to recursively search the tree for a value, reporting
   * whether or not it exists.
   */
  static bool recFindElement(unsigned short value, void* root, size_t numBits);

  /* Helper function to recursively insert an entry into the tree, reporting
   * whether the value was added (true) or already existed (false).
   */
  static bool recInsertElement(unsigned short value, void* root, size_t numBits);

  /* Helper function to recursively delete an entry from the tree, reporting
   * whether it already existed.
   */
  static bool recEraseElement(unsigned short value, void* root, size_t numBits);

  /* Helper function to return the largest or smallest elements of a vEB-tree,
   * handing back the sentinel if the tree is empty.
   */
  static size_t treeMax(void* root, size_t numBits);
  static size_t treeMin(void* root, size_t numBits);

  /* Helper function to return whether a tree is empty. */
  static bool isTreeEmpty(void* root, size_t numBits);

  /* Helper function to return the successor or predecessor of a given entry in
   * the tree, or the sentinel if none exists.
   */
  static size_t recSuccessor(unsigned short value, void* root, size_t numBits);
  static size_t recPredecessor(unsigned short value, void* root, size_t numBits);
};

/* Definition of the const_iterator type. */
class VanEmdeBoasTree::const_iterator:
  public std::iterator<std::bidirectional_iterator_tag, const unsigned short> {
public:
  /* Default constructor creates a garbage const_iterator. */
  const_iterator();

  /* Forwards and backwards motion. */
  const_iterator& operator++ ();
  const_iterator& operator-- ();
  const const_iterator operator++ (int);
  const const_iterator operator-- (int);

  /* Pointer dereference.  No arrow is defined because the iterator visits
   * unsigned shorts.  Since the values are immutable, we hand back a value
   * rather than a reference.
   */
  const unsigned short operator* () const;

  /* Equality and disequality testing. */
  bool operator== (const const_iterator& rhs) const;
  bool operator!= (const const_iterator& rhs) const;

private:
  /* Make VanEmdeBoasTree a friend of this class so it can invoke the private
   * constructor.
   */
  friend class VanEmdeBoasTree;

  /* Constructor creates an iterator that starts off at the specified value. */
  const_iterator(size_t valueOrSentinel, const VanEmdeBoasTree* owner);

  /* Internally, the iterator works by maintaining a size_t containing either
   * the value currently being iterated over, or kNil as a sentinel.  The ++
   * and -- operators just call predecessor and successor on the owner tree to
   * move to the previous and next values.
   */
  size_t mCurr;
  const VanEmdeBoasTree* mOwner;
};

#endif // VANEMDEBOASTREE_H
