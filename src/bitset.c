#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "bitset/bitset.h"

#define bitset_atom_popcnt  __builtin_popcountl
#define bitset_atom_ctz     __builtin_ctzl

int bitset_init(bitset_t* bs, size_t nbits) {
    bs->nbits = nbits;
    bs->natoms = BITS_TO_NATOMS(nbits);
    bs->bits  = malloc(bs->natoms * sizeof(bitset_atom_t));
    if (!bs->bits) {
        return -1;
    }
    memset(bs->bits, 0, bs->natoms * sizeof(bitset_atom_t));
    return 0;
}

void bitset_wipe(bitset_t* bs) {
    free(bs->bits);
}

void bitset_copy(bitset_t* dest, const bitset_t* src) {
    assert (dest->natoms == src->natoms);
    memcpy(dest->bits, src->bits, src->natoms * sizeof(bitset_atom_t));
}

int bitset_resize(bitset_t* bitset, size_t nbits) {
    size_t new_natoms = BITS_TO_NATOMS(nbits);
    bitset_atom_t* new_bits = realloc(bitset->bits,
                                      new_natoms * sizeof(bitset_atom_t));
    if (!new_bits) {
        return -1;
    }
    bitset->nbits = nbits;
    bitset->natoms = new_natoms;
    bitset->bits = new_bits;
    return 0;
}

/* {{{ Single bit manipulation */

void bitset_set(bitset_t* bs, size_t bit) {
    assert (bit < bs->nbits);
    bs->bits[bit / BITS_PER_ATOM] |= (1 << (bit & BITS_PER_ATOM));
}

void bitset_set_bit(bitset_t* bs, size_t idx, bit_t bit) {
    if (bitset_get(bs, idx)) {
        bitset_unset(bs, idx);
    } else {
        bitset_set(bs, idx);
    }
}

void bitset_unset(bitset_t* bs, size_t bit) {
    assert (bit < bs->nbits);
    bs->bits[bit / BITS_PER_ATOM] &= ~(1 << (bit & BITS_PER_ATOM));
}

bit_t bitset_get(const bitset_t* bs, size_t bit) {
    assert (bit < bs->nbits);
    return (bs->bits[bit / BITS_PER_ATOM] >> (bit & BITS_PER_ATOM)) & 1;
}

/* }}} */
/* {{{ Cool functions */

size_t bitset_popcnt(const bitset_t* bs) {
    size_t sum = 0;
    for (size_t i = 0; i < bs->natoms; i++) {
        sum += bitset_atom_popcnt(bs->bits[i]);
    }
    return sum;
}

int bitset_first_set(const bitset_t* bs) {
    size_t atom = 0;
    while (bs->bits[atom] == 0 && atom < bs->natoms) {
        atom++;
    }
    if (atom == bs->natoms) {
        return -1;
    }
    return atom * BITS_PER_ATOM + bitset_atom_ctz(bs->bits[atom]);
}

int bitset_next_set(const bitset_t* bs, size_t from) {
    size_t atom = from / BITS_PER_ATOM;

    // We first look in the first atom, that we mask to begin from the
    // "from" index.
    bitset_atom_t mask = (BITSET_ATOM_MAX << (from & BITS_PER_ATOM));
    if ((bs->bits[atom] & mask) != 0) {
        return atom * BITS_PER_ATOM + bitset_atom_ctz(atom);
    }

    // No more bits in the first atom, so we check for a set bit in rest
    // of atoms.
    atom++;
    while (bs->bits[atom] == 0 && atom < bs->natoms) {
        atom++;
    }
    if (atom == bs->natoms) {
        return -1;
    }
    return atom * BITS_PER_ATOM + bitset_atom_ctz(bs->bits[atom]);
}

/* }}} */
/* {{{ Sets functions */

void bitset_and_to(bitset_t* dest, const bitset_t* a, const bitset_t* b) {
    assert (a->natoms == b->natoms);
    assert (dest->natoms == a->natoms);

    for (size_t i = 0; i < a->natoms; i++) {
        dest->bits[i] = a->bits[i] & b->bits[i];
    }
}

void bitset_or_to(bitset_t* dest, const bitset_t* a, const bitset_t* b) {
    assert (a->natoms == b->natoms);
    assert (dest->natoms == a->natoms);

    for (size_t i = 0; i < a->natoms; i++) {
        dest->bits[i] = a->bits[i] | b->bits[i];
    }
}

void bitset_and(bitset_t* a, const bitset_t* b) {
    assert (a->natoms == b->natoms);

    for (size_t i = 0; i < a->natoms; i++) {
        a->bits[i] &= b->bits[i];
    }
}

void bitset_or(bitset_t* a, const bitset_t* b) {
    assert (a->natoms == b->natoms);

    for (size_t i = 0; i < a->natoms; i++) {
        a->bits[i] |= b->bits[i];
    }
}

/* }}} */

