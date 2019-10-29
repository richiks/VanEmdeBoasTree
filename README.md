# VanEmdeBoasTree

 * An implementation of a set of integers backed by a van Emde Boas tree. A van
 * Emde Boas tree (or vEB-tree) has excellent performance guarantees - each
 * insertion, deletion, and lookup takes O(lg lg n) time, which is much better
 * than a standard BST or skiplist.  Moreover, it's possible to look up a
 * number's successor or predecessor (the smallest element larger than the
 * value, or the largest element smaller than the value, respectively) in time
 * O(lg lg n).  This makes vEB-trees extremely attractive when working with
 * large sets of integers.
 *
 * The main drawback of a vEB-tree is its large memory usage.  To store a 
 * collection of numbers in the range {0, 1, 2, ..., n - 1} takes O(n) memory.
 * For this reason, vEB-trees are not used very commonly.  When they are, they
 * usually use memory-saving optimization involving perfect hash tables.
 *
 * The internal structure of a vEB-tree is rather clever.  Suppose that you are
 * interested in storing some subset of the values in {0, 1, 2, ..., n - 1} in
 * the tree.  Let s = ceil(lg n); then 2^s is the smallest power of two at
 * least as large as n.  For now, assume that s is even; we'll deal with the
 * case where it isn't later.  This means that each of the values {0, 1, ...,
 * n - 1} can be written using a total of s bits.  Each value can therefore be
 * thought of as the concatenation of two s/2-bit numbers, which we'll refer to
 * as (high, low).  The vEB-tree works by maintaining an array of pointers to
 * s/2 smaller vEB-trees.  Whenever an element is looked up, its first s/2 bits
 * are used as the index into the smaller trees, which are then searched for
 * the remaining s/2 bits.  Since at each step of this process the lookup
 * halves the number of unvisited bits, the search will stop after lg s steps.
 * Since s = O(lg n), this lookup requires a total of O(lg lg n) steps, as
 * required.
 *
 * The main challenge of the vEB-tree is how to get successor and predecessor
 * working efficiently.  This requires a fairly clever set of tricks.  First,
 * at each level of the tree, we'll have the vEB-tree store the smallest and
 * largest elements of the tree specially.  These values won't be stored in the
 * tree proper, but rather as auxiliary data.  Next, in addition to the s/2
 * pointers to vEB-trees of size s/2, there will be one extra vEB-tree of size
 * s/2 containing a "summary" of the smaller trees.  This summary tree wil hold
 * values corresponding to which subtrees are nonempty.  For example, if the
 * vEB-tree holds values in its zeroth, fourth, and ninth subtrees, then this
 * summary tree would hold zero, four, and nine.
 *
 * To look up the successor of the element x, we begin by looking as the max
 * value of the subtree where x would normally be located.  If x is less than
 * this value, then we recursively search that tree for the successor, since
 * one must exist.  However, if x is at least as large as the max element (or
 * no max element is defined because the tree is empty), then the successor of
 * x must be the smallest element in the next nonempty subtree of the vEB-tree.
 * To find this tree, we do a successor query in the summary tree to find the
 * first nonempty tree, then return the minimum value of that tree.  A search
 * for x's predecessor works similarly.
 *
 * The memory usage of a vEB-tree is a bit tricky to analyze.  The memory used
 * by a vEB-tree for values up to n is given by
 *
 *    M(n) <= k0 sqrt(n) + (sqrt(n) + 1) M(sqrt(n)) 
 *    M(1)  = k1
 *
 * For some appropriate constants k0, k1.  We'll show that M(n) = O(n) by 
 * showing that M(n) <= max{c, rn + t} for some appropriate constants c, r, 
 * and t.  
 *
 * As a base case, for n = 1, we get that
 *
 *     M(1) = k1 <= c
 *
 * So if we pick c = k1, this holds.
 *
 * For the inductive case, we consider two cases - one in which sqrt(n) = 1
 * (where sqrt(n) is actually floor(sqrt(n))), and one in which sqrt(n) > 1.
 * This first case holds when n < 4:
 *
 *     M(n) <= k0 sqrt(n) + k1(sqrt(n) + 1)
 *           = (k0 + k1) sqrt(n) + 1
 *           = k0 + k1 + 1
 *
 * So we need to ensure that rn + t >= k0 + k1 + 1 when n = 2, 3.  Let's worry
 * about this later.  For the case where n >= 4:
 *
 *     M(n) <= k0 sqrt(n) + (sqrt(n) + 1)(r sqrt(n) + t) 
 *           = k0 sqrt(n) + r n + t sqrt(n) + r sqrt(n) + t
 *           = r n + (k0 + t + r) sqrt(n) + t
 *          <= r n + (k0 + t + r) sqrt(n) + t
 *
 * If we pick t such that r + t + k0 <= 0, then this expression is bounded by
 * rn + t and we're done.
 *
 * We now need to pick values of r and t such that
 *
 *    r + t + k0 <= 0
 *    2r + t >= k0 + k1 + 1
 *
 * Equivalently
 *
 *    -r - t >= k0
 *    2t + t >= k0 + k1 + 1
 *
 * Adding gives
 *
 *     r >= 2k0 + k1 + 1
 *
 * Substituting this in gives us
 *
 *     4k0 + 2k1 + 2 + t >= k0 + k1 + 1
 *     t >= -3k0 - k1 - 1
 *
 * and
 *
 *     2k0 + k1 + 1 + t + k0 < 0
 *     3k0 + k1 + 1 + t < 0
 *     t <= -3k0 - k1 - 1
 *
 * This means that if we pick
 *
 *     r =  2k0 + k1 + 1
 *     t = -3k0 - k1 - 1
 *     c =        k1
 *
 * Then the recurrence holds, and the memory usage is O(n).
