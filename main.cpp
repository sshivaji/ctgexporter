#include "ctg_helper.h"

bool big_endian;

int main(int argc, char* argv[]) {
    // Figure out if we're on a big- or little-endian system.
    int i = 1;
    big_endian = (*(char*) &i) == 0;

    if (argc != 3) {
        printf("CtgExporter currently takes 2 arguments, <ctg_filename> <polyglot_book_filename>\n");
        return -1;
    }

    init_ctg_book(argv[1]);
    ctg_iterate_all_entries();
    /** TODO:
     * 1. Convert huffman code of CTG entry to internal board representation, preferably stockfish internal.
     * 2. Uncomment byte_to_move method in ctg_helper.c to convert moves from byte values to move representations.
     * 3. Once internal board representations and moves are ready, create a new polyglot book entry: struct entry_t {
     * uint64 key;
     * uint16 move;
     * uint16 n;
     * uint16 sum;
     * uint16 colour;
     * };
     * 4. Add all moves from step 2 to the book entry of step 3.
     * 5. Write extra fields to a leveldb database following Sebastien Major's example.
     *
     * 
     */    

    return 0;
}

