#include "ctg_helper.h"
#include "board.h"
#include "fen.h"

/**
 * CTG Code used from https://github.com/AaronBecker/daydreamer
 * 
 */
page_bounds_t page_bounds;

FILE* ctg_file = NULL;
FILE* cto_file = NULL;
board_t board[1];

/**
 * This opens a CTG file and requires ctb and cto files.
 * TODO: Support opening a file with only .ctg and .ctb and .cto missing. Use code from Sebasien Major's implementation.
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
#ifdef DEBUG
    printf("total pages:%d\n", page_bounds.high - page_bounds.low + 1);
#endif
    my_log_open("ctgexporter.log");

    for (int i = page_bounds.low; i <= page_bounds.high; ++i) {
        ctg_iterate_page(i);
    }
}

void parse_bits(bits_t *bits, int& square) {

    //    printf("Bits[0] is %d%d%d%d%d%d%d%d", bits[0], bits[1], bits[2], bits[3], bits[4], bits[5], bits[6], bits[7]);
    u_int16_t pos = 0;
    //    int bits_remaining = 8;
    bool match = true;

    std::vector<bool> result;
    // Reserve enough memory to fit a and b in order to avoid
    // unnecessary reallocations.
    result.reserve(bits->residue.size() + bits->bits.size());
    result.insert(result.end(), bits->residue.begin(), bits->residue.end());
    result.insert(result.end(), bits->bits.begin(), bits->bits.end());

    // Clear out residue and bits after copying
    bits->residue.clear();
    bits->bits.clear();

    if (square>=64)
        printf("Final end");
    printf("Result vector:\n");
    for (std::vector<bool>::iterator it = result.begin(); it != result.end(); ++it) {
        bool b = *it;
        printf("%d", b);
    }
    printf("\n");


    while (match && square <= 63) {
        printf("sq:%d\n", square);

        if (pos < result.size() && result[pos] == 0) {
            printf("Empty square\n");
            pos++;
            square++;
        } else if (pos <= result.size() - 3 && result[pos] == 1 && result[pos + 1] == 1) {
            if (result[pos + 2] == 0) {
                printf("White pawn\n");
            } else if (result[pos + 2] == 1) {
                printf("Black pawn\n");
            }
            pos += 3;
            square++;

        } else if (pos <= result.size() - 5 && result[pos] == 1 && result[pos + 1] == 0 && result[pos + 2] == 1 && result[pos + 3] == 1) {
            if (result[pos + 4] == 0) {
                printf("White Rook\n");
            } else if (result[pos + 4] == 1) {
                printf("Black Rook\n");
            }

            pos += 5;
            square++;

        } else if (pos <= result.size() - 5 && result[pos] == 1 && result[pos + 1] == 0 && result[pos + 2] == 1 && result[pos + 3] == 0) {
            if (result[pos + 4] == 0) {
                printf("White Bishop\n");
            } else if (result[pos + 4] == 1) {
                printf("Black Bishop\n");
            }

            pos += 5;
            square++;
        } else if (pos <= result.size() - 5 && result[pos] == 1 && result[pos + 1] == 0 && result[pos + 2] == 0 && result[pos + 3] == 1) {
            if (result[pos + 4] == 0) {
                printf("White Knight\n");
            } else if (result[pos + 4] == 1) {
                printf("Black Knight\n");
            }

            pos += 5;
            square++;
        } else if (pos <= result.size() - 6 && result[pos] == 1 && result[pos + 1] == 0 && result[pos + 2] == 0 && result[pos + 3] == 0 && result[pos + 4] == 1) {
            if (result[pos + 5] == 0) {
                printf("White Queen\n");
            } else if (result[pos + 5] == 1) {
                printf("Black Queen\n");
            }

            pos += 6;
            square++;
        } else if (pos <= result.size() - 6 && result[pos] == 1 && result[pos + 1] == 0 && result[pos + 2] == 0 && result[pos + 3] == 0 && result[pos + 4] == 0) {
            if (result[pos + 5] == 0) {
                printf("White King\n");
            } else if (result[pos + 5] == 1) {
                printf("Black King\n");
            }

            pos += 6;
            square++;
        } else {
            match = false;
            printf("Residue reached -- pos=%d\n", pos);
            for (std::vector<bool>::iterator it = result.begin() + pos; it != result.end(); ++it) {
                bits->residue.push_back(*it);
            }
        }
    }

    if (square > 63) {
//        printf("Residue post square 64 -- pos=%d\n", pos);
        for (std::vector<bool>::iterator it = result.begin() + pos; it != result.end(); ++it) {
            bits->residue.push_back(*it);
        }
//        square++;
    }
}

/*
 * Compute the huffman encoding of the given position, according to
 * ctg convention.
 */
static bool board_from_ctg_signature(board_t * board, ctg_signature_t* sig) {

    //    // Note: initial byte is reserved for length and flags info
    //    memset(sig, 0, sizeof(ctg_signature_t));
    //    int bit_position = 8;
    //    uint8_t bits = 0, num_bits = 0;

    // The board is flipped if it's black's turn, and mirrored if the king is
    // on the queenside with no castling rights for either side.
    //    bool flip_board = pos->side_to_move == BLACK;
    //    color_t white = flip_board ? BLACK : WHITE;
    //    bool mirror_board = square_file(pos->pieces[white][0]) < FILE_E &&
    //        pos->castle_rights == 0;

    //    int sq;

    printf("\n%d byte signature", sig->buf_len);
//    printf("\n header %d", sig->buf[0]& 0x40);
    bits_t bits;
    int sq = 0;


    for (int i = 0; i < sig->buf_len; ++i) {
        if (i % 8 == 0) printf("\n");
        for (int j = 0; j < 8; ++j) {
            bool b = sig->buf[i] & (1 << (7 - j)) ? 1 : 0;
            bits.bits.push_back(b);

            //            if (bit_len == 8) {
            //                bit_pos = 0;
            //                bit_len = 0;
            //            }
            //
            printf("%d", b);
            //            bits.bits[bit_pos] = bit;
            //            bit_pos++;
            //            bit_len++;

        }
        printf("\n");
        //        if (sq<=63) {
//        parse_bits(&bits, sq);
        //            printf("sq:%d\n",sq);

        //        }

    }



    // For each board square, append the huffman bit sequence for its contents.
    //    for (int file = 0; file < 8; ++file) {
    //        for (int rank = 0; rank < 8; ++rank) {
    //            //            square_t sq = create_square(file, rank);
    //            sq = square_make(file, rank);
    //            //
    //            //            if (flip_board) sq = mirror_rank(sq);
    //            //            if (mirror_board) sq = mirror_file(sq);
    //            //            piece_t piece = flip_board ?
    //            //                flip_piece[pos->board[sq]] :
    //            //                pos->board[sq];
    //            //            switch (piece) {
    //            //                case EMPTY: bits = 0x0; num_bits = 1; break;
    //            //                case WP: bits = 0x3; num_bits = 3; break;
    //            //                case BP: bits = 0x7; num_bits = 3; break;
    //            //                case WN: bits = 0x9; num_bits = 5; break;
    //            //                case BN: bits = 0x19; num_bits = 5; break;
    //            //                case WB: bits = 0x5; num_bits = 5; break;
    //            //                case BB: bits = 0x15; num_bits = 5; break;
    //            //                case WR: bits = 0xD; num_bits = 5; break;
    //            //                case BR: bits = 0x1D; num_bits = 5; break;
    //            //                case WQ: bits = 0x11; num_bits = 6; break;
    //            //                case BQ: bits = 0x31; num_bits = 6; break;
    //            //                case WK: bits = 0x1; num_bits = 6; break;
    //            //                case BK: bits = 0x21; num_bits = 6; break;
    //            //                default: assert(false);
    //            //            }
    //            //            append_bits_reverse(sig, bits, bit_position, num_bits);
    //            //            bit_position += num_bits;
    //        }
    //    }
    return true;

    // Encode castling and en passant rights. These must sit flush at the end
    // of the final byte, so we also have to figure out how much to pad.
    //    int ep = -1;
    //    int flag_bit_length = 0;
    //    if (pos->ep_square) {
    //        ep = square_file(pos->ep_square);
    //        if (mirror_board) ep = 7 - ep;
    //        flag_bit_length = 3;
    //    }
    //    int castle = 0;
    //    if (has_oo_rights(pos, white)) castle += 4;
    //    if (has_ooo_rights(pos, white)) castle += 8;
    //    if (has_oo_rights(pos, white^1)) castle += 1;
    //    if (has_ooo_rights(pos, white^1)) castle += 2;
    //    if (castle) flag_bit_length += 4;
    //    uint8_t flag_bits = castle;
    //    if (ep != -1) {
    //        flag_bits <<= 3;
    //        for (int i=0; i<3; ++i, ep>>=1) if (ep&1) flag_bits |= (1<<(2-i));
    //    }
    //
    //    //printf("\nflag bits: %d\n", flag_bits);
    //    //printf("bit_position: %d\n", bit_position%8);
    //    //printf("flag_bit_length: %d\n", flag_bit_length);
    //
    //    // Insert padding so that flags fit at the end of the last byte.
    //    int pad_bits = 0;
    //    if (8-(bit_position % 8) < flag_bit_length) {
    //        //printf("padding byte\n");
    //        pad_bits = 8 - (bit_position % 8);
    //        append_bits_reverse(sig, 0, bit_position, pad_bits);
    //        bit_position += pad_bits;
    //    }
    //
    //    pad_bits = 8 - (bit_position % 8) - flag_bit_length;
    //    if (pad_bits < 0) pad_bits += 8;
    //    //printf("padding %d bits\n", pad_bits);
    //    append_bits_reverse(sig, 0, bit_position, pad_bits);
    //    bit_position += pad_bits;
    //    append_bits_reverse(sig, flag_bits, bit_position, flag_bit_length);
    //    bit_position += flag_bit_length;
    //    sig->buf_len = (bit_position + 7) / 8;
    //
    //    // Write header byte
    //    sig->buf[0] = ((uint8_t)(sig->buf_len));
    //    if (ep != -1) sig->buf[0] |= 1<<5;
    //    if (castle) sig->buf[0] |= 1<<6;
}

/*
 * Push the given bits on to the end of |sig|. This is a helper function that
 * makes the huffman encoding of positions a little cleaner.
 */
static void append_bits_reverse(ctg_signature_t* sig,
        uint8_t bits,
        int bit_position,
        int num_bits) {
    uint8_t * sig_byte = &sig->buf[bit_position / 8];
    int offset = bit_position % 8;
    for (int i = offset; i < num_bits + offset; ++i, bits >>= 1) {
        if (bits & 1) *sig_byte |= 1 << (7 - (i % 8));
        if (i % 8 == 7) *(++sig_byte) = 0;
    }
}

/*
 * Print out the huffman signature |sig|, as both bits and bytes.
 */
static void print_signature(ctg_signature_t * sig) {
    // Print as bits.
    printf("\n%d byte signature", sig->buf_len);
    for (int i = 0; i < sig->buf_len; ++i) {
        if (i % 8 == 0) printf("\n");
        for (int j = 0; j < 8; ++j) {
            printf("%d", sig->buf[i] & (1 << (7 - j)) ? 1 : 0);
        }
        printf(" ");
    }

    // Print as chars.
    for (int i = 0; i < sig->buf_len; ++i) {
        if (i % 8 == 0) printf("\n");
        printf("%3d ", (char) sig->buf[i]);
    }
    printf("\n");
}

/* 
 * Iterate all ctg entries for a particular page.
 */
void ctg_iterate_page(int page_index) {
//    board_start(board);
    //  board_clear(board);
    //    board_disp(board);

    // Pages are a uniform 4096 bytes.
    uint8_t buf[4096];
    fseek(ctg_file, 4096 * (page_index + 1), SEEK_SET);
    if (!fread(buf, 1, 4096, ctg_file)) return;
    int num_positions = (buf[0] << 8) + buf[1];
    //    #ifdef DEBUG
    //        printf("found %d positions\n", num_positions);
    //    #endif

    // Just scan through the list until we find a matching signature.
    int pos = 4;
    for (int i = 0; i < num_positions; ++i) {
        int entry_size = buf[pos] % 32;
        printf("Castling bit:%d\n",(buf[pos]& 0x40)/64);
        printf("Enpassant bit:%d\n",(buf[pos]& 0x20)/32);

        ctg_signature_t ctg_signature;
        ctg_signature_t *signature = &ctg_signature;
        signature->buf_len = entry_size;
        for (int j = 1; j < entry_size; ++j)
            signature->buf[j - 1] = buf[pos + j];
#ifdef DEBUG
        board_from_ctg_signature(board, signature);
        //        print_signature(signature);
#endif
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
        pos += 1;
    }
}

