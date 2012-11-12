
#include "ctg_helper.h"

/**
 * CTG Code used from https://github.com/AaronBecker/daydreamer
 * 
 */
page_bounds_t page_bounds;

FILE* ctg_file = NULL;
FILE* cto_file = NULL;

/**
 * This opens a CTG file and requires ctb and cto files.
 * TODO: Support opening a file with only .ctg and .ctb and .cto missing.
 * @param filename
 * @return 
 */

bool init_ctg_book(char* filename) {
    int name_len = strlen(filename);
    assert(filename[name_len - 3] == 'c' &&
            filename[name_len - 2] == 't' &&
            filename[name_len - 1] == 'g');
    char fbuf[1024];
    strcpy(fbuf, filename);
    if (ctg_file) {
        assert(cto_file);
        fclose(ctg_file);
        fclose(cto_file);
    }
    ctg_file = fopen(fbuf, "r");
    fbuf[name_len - 1] = 'o';
    cto_file = fopen(fbuf, "r");
    fbuf[name_len - 1] = 'b';
    FILE* ctb_file = fopen(fbuf, "r");
    fbuf[name_len - 1] = 'g';
    if (!ctg_file || !cto_file || !ctb_file) {
        printf("info string Couldn't load book %s\n", fbuf);
        return false;
    }

    // Read out upper and lower page limits.
    fread(&page_bounds, 12, 1, ctb_file);
    page_bounds.low = my_ntohl((uint32_t) page_bounds.low);
    //    printf("page_bounds.low %u\n", page_bounds.low);

    page_bounds.high = my_ntohl((uint32_t) page_bounds.high);
    //    printf("page_bounds.high %u\n", page_bounds.high);

    assert(page_bounds.low <= page_bounds.high);
    fclose(ctb_file);
    return true;
}

///*
// * Find the page index associated with a given position |hash|.
// */
//static bool ctg_get_page_index(int hash, int* page_index) {
//    uint32_t key = 0;
//    for (int mask = 1; key <= (uint32_t) page_bounds.high;
//            mask = (mask << 1) + 1) {
//        key = (hash & mask) + mask;
//        if (key >= (uint32_t) page_bounds.low) {
//            //printf("found entry with key=%d\n", key);
//            fseek(cto_file, 16 + key * 4, SEEK_SET);
//            fread(page_index, 4, 1, cto_file);
//            *page_index = my_ntohl((uint32_t) * page_index);
//            if (*page_index >= 0) return true;
//        }
//    }
//    //printf("didn't find entry\n");
//    return false;
//}



/*
 * Convert a ctg-format move to native format. The ctg move format seems
 * really bizarre; maybe there's some simpler formulation. The ctg move
 * indicates the piece type, the index of the piece to be moved (counting
 * from A1 to H8 by ranks), and the delta x and delta y of the move.
 * We just look these values up in big tables.
 */
//static move_t byte_to_move(position_t* pos, uint8_t byte) {
//    const char* piece_code =
//            "PNxQPQPxQBKxPBRNxxBKPBxxPxQBxBxxxRBQPxBPQQNxxPBQNQBxNxNQQQBQBxxx"
//            "xQQxKQxxxxPQNQxxRxRxBPxxxxxxPxxPxQPQxxBKxRBxxxRQxxBxQxxxxBRRPRQR"
//            "QRPxxNRRxxNPKxQQxxQxQxPKRRQPxQxBQxQPxRxxxRxQxRQxQPBxxRxQxBxPQQKx"
//            "xBBBRRQPPQBPBRxPxPNNxxxQRQNPxxPKNRxRxQPQRNxPPQQRQQxNRBxNQQQQxQQx";
//    const int piece_index[256] = {
//        5, 2, 9, 2, 2, 1, 4, 9, 2, 2, 1, 9, 1, 1, 2, 1,
//        9, 9, 1, 1, 8, 1, 9, 9, 7, 9, 2, 1, 9, 2, 9, 9,
//        9, 2, 2, 2, 8, 9, 1, 3, 1, 1, 2, 9, 9, 6, 1, 1,
//        2, 1, 2, 9, 1, 9, 1, 1, 2, 1, 1, 2, 1, 9, 9, 9,
//        9, 2, 1, 9, 1, 1, 9, 9, 9, 9, 8, 1, 2, 2, 9, 9,
//        1, 9, 1, 9, 2, 3, 9, 9, 9, 9, 9, 9, 7, 9, 9, 5,
//        9, 1, 2, 2, 9, 9, 1, 1, 9, 2, 1, 0, 9, 9, 1, 2,
//        9, 9, 2, 9, 1, 9, 9, 9, 9, 2, 1, 2, 3, 2, 1, 1,
//        1, 1, 6, 9, 9, 1, 1, 1, 9, 9, 1, 1, 1, 9, 2, 1,
//        9, 9, 2, 9, 1, 9, 2, 1, 1, 1, 1, 3, 9, 1, 9, 2,
//        2, 9, 1, 8, 9, 2, 9, 9, 9, 2, 9, 2, 9, 2, 2, 9,
//        2, 6, 1, 9, 9, 2, 9, 1, 9, 2, 9, 5, 2, 2, 1, 9,
//        9, 1, 2, 1, 2, 2, 2, 7, 7, 2, 2, 6, 2, 1, 9, 4,
//        9, 2, 2, 2, 9, 9, 9, 1, 2, 1, 1, 1, 9, 9, 5, 1,
//        2, 1, 9, 2, 9, 1, 4, 1, 1, 1, 9, 4, 1, 1, 2, 1,
//        2, 1, 9, 2, 2, 2, 0, 1, 2, 2, 2, 2, 9, 1, 2, 9
//    };
//    const int forward[256] = {
//        1, -1, 9, 0, 1, 1, 1, 9, 0, 6, -1, 9, 1, 3, 0, -1,
//        9, 9, 7, 1, 1, 5, 9, 9, 1, 9, 6, 1, 9, 7, 9, 9,
//        9, 0, 2, 6, 1, 9, 7, 1, 5, 0, -2, 9, 9, 1, 1, 0,
//        -2, 0, 5, 9, 2, 9, 1, 4, 4, 0, 6, 5, 5, 9, 9, 9,
//        9, 5, 7, 9, -1, 3, 9, 9, 9, 9, 2, 5, 2, 1, 9, 9,
//        6, 9, 0, 9, 1, 1, 9, 9, 9, 9, 9, 9, 1, 9, 9, 2,
//        9, 6, 2, 7, 9, 9, 3, 1, 9, 7, 4, 0, 9, 9, 0, 7,
//        9, 9, 7, 9, 0, 9, 9, 9, 9, 6, 3, 6, 1, 1, 3, 0,
//        6, 1, 1, 9, 9, 2, 0, 5, 9, 9, -2, 1, -1, 9, 2, 0,
//        9, 9, 1, 9, 3, 9, 1, 0, 0, 4, 6, 2, 9, 2, 9, 4,
//        3, 9, 2, 1, 9, 5, 9, 9, 9, 0, 9, 6, 9, 0, 3, 9,
//        4, 2, 6, 9, 9, 0, 9, 5, 9, 3, 9, 1, 0, 2, 0, 9,
//        9, 2, 2, 2, 0, 4, 5, 1, 2, 7, 3, 1, 5, 0, 9, 1,
//        9, 1, 1, 1, 9, 9, 9, 1, 0, 2, -2, 2, 9, 9, 1, 1,
//        -1, 7, 9, 3, 9, 0, 2, 4, 2, -1, 9, 1, 1, 7, 1, 0,
//        0, 1, 9, 2, 2, 1, 0, 1, 0, 6, 0, 2, 9, 7, 3, 9
//    };
//    const int left[256] = {
//        -1, 2, 9, -2, 0, 0, 1, 9, -4, -6, 0, 9, 1, -3, -3, 2,
//        9, 9, -7, 0, -1, -5, 9, 9, 0, 9, 0, 1, 9, -7, 9, 9,
//        9, -7, 2, -6, 1, 9, 7, 1, -5, -6, -1, 9, 9, -1, -1, -1,
//        1, -3, -5, 9, -1, 9, -2, 0, 4, -5, -6, 5, 5, 9, 9, 9,
//        9, -5, 7, 9, -1, -3, 9, 9, 9, 9, 0, 5, -1, 0, 9, 9,
//        0, 9, -6, 9, 1, 0, 9, 9, 9, 9, 9, 9, -1, 9, 9, 0,
//        9, -6, 0, 7, 9, 9, 3, -1, 9, 0, -4, 0, 9, 9, -5, -7,
//        9, 9, 7, 9, -2, 9, 9, 9, 9, 6, 0, 0, -1, 0, 3, -1,
//        6, 0, 1, 9, 9, 1, -7, 0, 9, 9, -1, -1, 1, 9, 2, -7,
//        9, 9, -1, 9, 0, 9, -1, 1, -3, 0, 0, 0, 9, 0, 9, 4,
//        0, 9, -2, 0, 9, 0, 9, 9, 9, -2, 9, 6, 9, -4, -3, 9,
//        0, 0, 6, 9, 9, -5, 9, 0, 9, -3, 9, 0, -5, 0, -1, 9,
//        9, -2, -2, 2, -1, 0, 0, 1, 0, 0, 3, 0, 5, -2, 9, 0,
//        9, 1, -2, 2, 9, 9, 9, 1, -6, 2, 1, 0, 9, 9, 1, 1,
//        -2, 0, 9, 0, 9, -4, 0, -4, 0, -2, 9, -1, 0, -7, 1, -4,
//        -7, -1, 9, 1, 0, -1, 0, 2, -1, 0, -3, -2, 9, 0, 3, 9
//    };
//
//    // Find the piece. Note: the board may be mirrored/flipped.
//    bool flip_board = pos->side_to_move == BLACK;
//    color_t white = flip_board ? BLACK : WHITE;
//    bool mirror_board = square_file(pos->pieces[white][0]) < FILE_E &&
//            pos->castle_rights == 0;
//    int file_from = -1, file_to = -1, rank_from = -1, rank_to = -1;
//
//    // Handle castling.
//    if (byte == 107) {
//        file_from = 4;
//        file_to = 6;
//        rank_from = rank_to = flip_board ? 7 : 0;
//        return squares_to_move(pos,
//                create_square(file_from, rank_from),
//                create_square(file_to, rank_to));
//    }
//    if (byte == 246) {
//        file_from = 4;
//        file_to = 2;
//        rank_from = rank_to = flip_board ? 7 : 0;
//        return squares_to_move(pos,
//                create_square(file_from, rank_from),
//                create_square(file_to, rank_to));
//    }
//
//    // Look up piece type. Note: positions are always white to move.
//    piece_t pc = NONE;
//    char glyph = piece_code[byte];
//    switch (piece_code[byte]) {
//        case 'P': pc = WP;
//            break;
//        case 'N': pc = WN;
//            break;
//        case 'B': pc = WB;
//            break;
//        case 'R': pc = WR;
//            break;
//        case 'Q': pc = WQ;
//            break;
//        case 'K': pc = WK;
//            break;
//        default: printf("%d -> (%c)\n", byte, glyph);
//            assert(false);
//    }
//
//    // Find the piece.
//    int nth_piece = piece_index[byte], piece_count = 0;
//    bool found = false;
//    for (int file = 0; file < 8 && !found; ++file) {
//        for (int rank = 0; rank < 8 && !found; ++rank) {
//            square_t sq = create_square(file, rank);
//            if (flip_board) sq = mirror_rank(sq);
//            if (mirror_board) sq = mirror_file(sq);
//            piece_t piece = flip_board ?
//                    flip_piece[pos->board[sq]] : pos->board[sq];
//            if (piece == pc) piece_count++;
//            if (piece_count == nth_piece) {
//                file_from = file;
//                rank_from = rank;
//                found = true;
//            }
//        }
//    }
//    assert(found);
//
//    // Normalize rank and file values.
//    file_to = file_from - left[byte];
//    file_to = (file_to + 8) % 8;
//    rank_to = rank_from + forward[byte];
//    rank_to = (rank_to + 8) % 8;
//    if (flip_board) {
//        rank_from = 7 - rank_from;
//        rank_to = 7 - rank_to;
//    }
//    if (mirror_board) {
//        file_from = 7 - file_from;
//        file_to = 7 - file_to;
//    }
//    return squares_to_move(pos,
//            create_square(file_from, rank_from),
//            create_square(file_to, rank_to));
//}

/*
 * Iterate all ctg entries for a given file.
 */

void ctg_iterate_all_entries() {
    for (int i = page_bounds.low; i <= page_bounds.high; ++i) {
        ctg_iterate_page(i);
    }
}

 /* 
  * Iterate all ctg entries for a particular page.
  */
void ctg_iterate_page(int page_index) {
    // Pages are a uniform 4096 bytes.
    uint8_t buf[4096];
    fseek(ctg_file, 4096 * (page_index + 1), SEEK_SET);
    if (!fread(buf, 1, 4096, ctg_file)) return;
    int num_positions = (buf[0] << 8) + buf[1];
    #ifdef DEBUG
        printf("found %d positions\n", num_positions);
    #endif

    // Just scan through the list until we find a matching signature.
    int pos = 4;
    for (int i = 0; i < num_positions; ++i) {
        int entry_size = buf[pos] % 32;
        //        bool equal = true;
        //        if (sig->buf_len != entry_size) equal = false;
        //        for (int j = 0; j < sig->buf_len && equal; ++j) {
        //            if (buf[pos + j] != sig->buf[j]) equal = false;
        //        }

        //        if (!equal) {
        //            pos += entry_size + buf[pos + entry_size] + 33;
        //            continue;
        //        }

        // Found it, fill in the entry and return. Annoyingly, most of the
        // fields are 24 bits long.
        pos += entry_size;
        entry_size = buf[pos];
        ctg_entry_t ctg_entry;
        ctg_entry_t *entry = &ctg_entry;

        for (int j = 1; j < entry_size; ++j) entry->moves[j - 1] = buf[pos + j];
        entry->num_moves = (entry_size - 1) / 2;

//        #ifdef DEBUG
//            printf("entry->num_moves: %d\n", entry->num_moves);
//        #endif

        pos += entry_size;
        entry->total = read_24(buf, pos);

//        #ifdef DEBUG
//            printf("entry->total: %d\n", entry->total);
//        #endif

        pos += 3;
        entry->losses = read_24(buf, pos);
        pos += 3;
        entry->wins = read_24(buf, pos);
        pos += 3;
        entry->draws = read_24(buf, pos);
        pos += 3;
        entry->unknown1 = read_32(buf, pos);

        pos += 4;
        entry->avg_rating_games = read_24(buf, pos);
        pos += 3;
        entry->avg_rating_score = read_32(buf, pos);
        pos += 4;
        entry->perf_rating_games = read_24(buf, pos);
        pos += 3;
        entry->perf_rating_score = read_32(buf, pos);
        pos += 4;
        entry->recommendation = buf[pos];
        pos += 1;
        entry->unknown2 = buf[pos];

        pos += 1;
        entry->comment = buf[pos];
    }
}

