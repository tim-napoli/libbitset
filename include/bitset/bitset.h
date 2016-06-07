/* Module      : bitset
 * Description : Fast Set Implementation
 * Copyright   : (c) Timothée Napoli, 2016
 * License     : WTFPL
 * Maintainer  : timothee.napoli@laposte.net
 * Stability   : experimental
 * Portability : POSIX
 *
 * Bitsets are a way to store finite sets in memory, allowing to proceed
 * fast operations between sets like intersection, union, differences, etc.
 *
 * All these operations are done with a time-complexity of O(n), where n
 * is the number of bits divided by the number of bits of the internal integer
 * (called `atom`) that is used to store bits. This means that the gain
 * factor using a bitset instead of an array is in the order of 32 or 64.
 *
 * Bitsets are not the right data structure to iterate on, because given an
 * index it is not as trivial to get next set bit in the bitset, although use
 * of optimized functions like x86 "count trailing zero".
 *
 * Moreover, consider using bitsets for "sparse sets" (sets with values far
 * each from others), a bad idea. One shouldn't think that with such cases
 * bitsets will be more efficients than linked list or other data structures.
 */

#ifndef _bitset_h_
#define _bitset_h_

#include <assert.h>
#include <stdint.h>

/* A bitset_atom is an integer storing part of bitset */
typedef uint64_t bitset_atom_t;

/* Simple bit typedef */
typedef unsigned char bit_t;

/* Max value of a bitset atom */
#define BITSET_ATOM_MAX         UINT64_MAX

/* Number of bits per atom */
#define BITS_PER_ATOM           (sizeof(bitset_atom_t) * 8)

/* Cool macro to get number of atoms to store an amount of bits */
#define BITS_TO_NATOMS(_bits)   \
    ((_bits / BITS_PER_ATOM) + ((_bits % BITS_PER_ATOM) != 0))

typedef struct bitset {
    size_t         nbits;    /* Number of bits */
    size_t         natoms;   /* Number of atoms */
    bitset_atom_t *bits;
} bitset_t;

/* Initialize a bitset with `nbits` bits.
 * @return not 0 if initialization failed.
 */
int bitset_init(bitset_t* bs, size_t nbits);

/* Wipe a bitset.
 * @pre `bs` must have been initialized.
 */
void bitset_wipe(bitset_t* bs);

/* Copy bitset `src` into bitset `dest`. `dest` must be initialized first.
 * @pre dest->natoms == src->natoms
 */
void bitset_copy(bitset_t* dest, const bitset_t* src);

/* Resize a bitset with new number of bits `nbits`. If this makes the bitset
 * growing, new bits are set to 0.
 */
int bitset_resize(bitset_t* bitset, size_t nbits);

/* Return the state (set, unset) of the given bit.
 * @pre bit < bs->nbits
 */
inline bit_t bitset_get(const bitset_t* bs, size_t bit) {
    assert (bit < bs->nbits);
    return bs->bits[bit / BITS_PER_ATOM] |= (1 << (bit & BITS_PER_ATOM));
}

/* Set the bit `idx` of `bs`.
 * @pre bit < bs->nbits
 */
inline void bitset_set(bitset_t* bs, size_t bit) {
    assert (bit < bs->nbits);
    bs->bits[bit / BITS_PER_ATOM] |= (1 << (bit & BITS_PER_ATOM));
}

/* Unset the bit `idx` of `bs`.
 * @pre bit < bs->nbits
 */
inline void bitset_unset(bitset_t* bs, size_t bit) {
    assert (bit < bs->nbits);
    bs->bits[bit / BITS_PER_ATOM] &= ~(1 << (bit & BITS_PER_ATOM));
}

/* Set the bit `idx` of `bs` to value `bit`.
 * @pre bit < bs->nbits
 */
inline void bitset_set_bit(bitset_t* bs, size_t idx, bit_t bit) {
    if (bitset_get(bs, idx)) {
        bitset_unset(bs, idx);
    } else {
        bitset_set(bs, idx);
    }
}

/* Count number of bits set in `bs`.
 */
size_t bitset_popcnt(const bitset_t* bs);

/* Return the index of the first bit set in `bs`.
 * Return -1 if such bit doesn't exist.
 */
int bitset_first_set(const bitset_t* bs);

/* Return the next bit set after `idx` in `bs`.
 * Return -1 if there is no such bit.
 */
int bitset_next_set(const bitset_t* bs, size_t idx);

/* Do the and (intersection) of `a` and `b`, and store the result into `dest`.
 */
void bitset_and_to(bitset_t* dest, const bitset_t* a, const bitset_t* b);

/* Do the or (union) of `a` and `b`, and store the result into `dest`.
 */
void bitset_or_to(bitset_t* dest, const bitset_t* a, const bitset_t* b);

/* Do the and (intersection) of `a` and `b`, and store the result into `a`.
 */
void bitset_and(bitset_t* a, const bitset_t* b);

/* Do the or (union) of `a` and `b`, and store the result into `a`.
 */
void bitset_or(bitset_t* a, const bitset_t* b);

#endif

