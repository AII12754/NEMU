#include <stdlib.h>
#include "common.h"

#define m 32
#define b 6
#define s 7
#define t (m - b - s)
#define B (1 << b)
#define S (1 << s)
#define E 8
#define b_MASK ((1 << b) - 1)
#define s_MASK ((1 << s) - 1)
#define t_MASK ((1 << t) - 1)

uint32_t dram_read(hwaddr_t, size_t);

typedef struct {
    uint8_t byte[B];
    uint8_t tag;
    bool valid;
} cache_line;

typedef struct {
    cache_line lines[E];
} cache_set;

cache_set cache[S];

void init_cache() {
    int i, j;
    for(i = 0; i < S; i++) for(j = 0; j < E; j++) cache[i].lines[j].valid = false;
}

uint32_t cache_read(hwaddr_t addr, size_t len) {
    //Log("%d %d", addr, (int)len);
    int offset = (addr & b_MASK), set = ((addr >> b) & s_MASK), tag = ((addr >> b >> s) & t_MASK);
    int Len = len + offset;
    int i;

    union {
        uint8_t _8[4];
        uint32_t _32;
    } data;
    data._32 = 0;

    hwaddr_t dram_addr = (tag << b << s) + (set << b);
    cache_line *target_line = &cache[set].lines[rand() % E];

    for(i = 0; i < E; i++) if(cache[set].lines[i].valid && cache[set].lines[i].tag == tag) target_line = &cache[set].lines[i];

    for(i = 0; i < B; i++) target_line->byte[i] = dram_read(dram_addr + i, 1);
    target_line->valid = true;
    target_line->tag = tag;

    for(i = 0; i < len; i++) if(i + offset < B) data._8[i] = target_line->byte[offset + i];


    if(Len > B) {
        set++;
        dram_addr = (tag << b << s) + (set << b);
        target_line = &cache[set].lines[rand() % E];
        for(i = 0; i < E; i++) if(cache[set].lines[i].valid && cache[set].lines[i].tag == tag) target_line = &cache[set].lines[i];

        for(i = 0; i < B; i++) target_line->byte[i] = dram_read(dram_addr + i, 1);
        target_line->valid = true;
        target_line->tag = tag;
        
        for(i = 0; i < len; i++) if(i + offset >= B) data._8[i] = target_line->byte[offset + i - B];
    }

    return data._32;
}

void cache_write(hwaddr_t addr, size_t len, uint32_t Data) {
    //Log("Hello write");
    int offset = addr & b_MASK, set = (addr >> b) & s_MASK, tag = (addr >> b >> s) & t_MASK;
    int Len = len + offset;
    int i;

    while(Len > 0) {
        hwaddr_t dram_addr = (tag << b << s) + (set << b);
        cache_line *target_line= &cache[set].lines[rand() % E];
        
        for(i = 0; i < E; i++) if(cache[set].lines[i].valid && cache[set].lines[i].tag == tag) target_line = &cache[set].lines[i];
        for(i = 0; i < B; i++) target_line->byte[i] = dram_read(dram_addr + i, 1);
        target_line->valid = true;
        target_line->tag = tag;
        Len -= B;
        set++;
    }
}
