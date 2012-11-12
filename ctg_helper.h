#include <stdbool.h>
#include <stdint.h>
#include <string.h> /* strerror */
#include <stdio.h>
#include <assert.h>

extern bool big_endian;

#define my_ntohl(x) \
    (!big_endian ? \
    ((((uint32_t)(x) & 0xff000000) >> 24) | \
     (((uint32_t)(x) & 0x00ff0000) >>  8) | \
     (((uint32_t)(x) & 0x0000ff00) <<  8) | \
     (((uint32_t)(x) & 0x000000ff) << 24)) : \
     (x))

#define read_24(buf, pos)   \
    ((buf[pos]<<16) + (buf[(pos)+1]<<8) + (buf[(pos)+2]))
#define read_32(buf, pos)   \
    ((buf[pos]<<24) + (buf[pos+1]<<16) + (buf[(pos)+2]<<8) + (buf[(pos+3)+2]))

typedef struct {
    uint8_t buf[64];
    int buf_len;
} ctg_signature_t;

typedef struct {
    int num_moves;
    uint8_t moves[100];
    int total;
    int wins;
    int losses;
    int draws;
    int unknown1;
    int avg_rating_games;
    int avg_rating_score;
    int perf_rating_games;
    int perf_rating_score;
    int recommendation;
    int unknown2;
    int comment;
} ctg_entry_t;

typedef struct {
    int pad;
    int low;
    int high;
} page_bounds_t;

bool init_ctg_book(char* filename);
//bool ctg_get_page_index(int hash, int* page_index);
//static move_t byte_to_move(position_t* pos, uint8_t byte);
void ctg_iterate_page(int page_index);
void ctg_iterate_all_entries();