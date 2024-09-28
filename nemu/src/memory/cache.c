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

void swaddr_write(swaddr_t addr, size_t len, uint32_t data);
uint32_t swaddr_read(swaddr_t addr, size_t len);

typedef struct {
    uint8_t byte[B];
    uint8_t tag;
    bool valid;
} cache_line;

typedef struct {
    cache_line lines[E];
    int used;
} cache_set;

cache_set cache[S];

void init_cache() {
    Log("Hello");
    int i, j;
    for(i = 0; i < S; i++) {
        cache[i].used = 0;
        for(j = 0; j < E; j++) {
            cache[i].lines[j].valid = false;
        }
    }
}

uint32_t cache_read(swaddr_t addr, size_t len) {
    int offset = (addr & b_MASK), set = ((addr >> b) & s_MASK), tag = ((addr >> b >> s) & t_MASK);
    int Len = (len + offset) / B + !!(len + offset);
    int i;

    uint8_t data[2 * B] = {};

    swaddr_t dram_addr = (tag << b << s) + (set << b);
    cache_line *target_line = &cache[set].lines[rand() % E];

    for(i = 0; i < E; i++) if(cache[set].lines[i].valid && cache[set].lines[i].tag == tag) target_line = &cache[set].lines[i];

    if(target_line == NULL) {
        target_line = &cache[set].lines[rand() % E];
        for(i = 0; i < B; i++) {
            target_line->byte[i] = swaddr_read(dram_addr + i, 1);
            data[i] = target_line->byte[i];
        }
        target_line->valid = true;
        target_line->tag = tag;
    } else {
        for(i = 0; i < B; i++) {
            data[i] = target_line->byte[i];
        }
    }

    if(Len > B) {
        dram_addr = (tag << b << s) + ((set + 1) << b);
        for(i = 0; i < E; i++) if(cache[set].lines[i].valid && cache[set].lines[i].tag == tag) target_line =  &cache[set].lines[i];

        if(target_line == NULL) {
            target_line = &cache[set + 1].lines[rand() % E];
            for(i = 0; i < B; i++) {
                target_line->byte[i] = swaddr_read(dram_addr + i, 1);
                data[B + i] = target_line->byte[i];
            }
            target_line->valid = true;
            target_line->tag = tag;
        } else {
            for(i = 0; i < B; i++) {
                data[B + i] = target_line->byte[i];
            }
        }
    }

    return unalign_rw(data + offset, 4);
}

void cache_write(swaddr_t addr, size_t len, uint32_t data[]) {
    int offset = addr & b_MASK, set = (addr >> b) & s_MASK, tag = (addr >> b >> s) & t_MASK;
    int Len = len + offset;
    int i;

    for(i = 0; i < len; i++) swaddr_write(addr + i, 1, data[i]);

    swaddr_t dram_addr = (tag << b << s) + (set << b);
    cache_line *target_line;
    
    target_line = &cache[set].lines[rand() % E];
    for(i = 0; i < E; i++) if(cache[set].lines[i].valid && cache[set].lines[i].tag == tag) target_line = &cache[set].lines[i];

    for(i = 0; i < B; i++) target_line->byte[i] = swaddr_read(dram_addr + i, 1);
    target_line->valid = true;
    target_line->tag = tag;

    if(Len > B) {
        set++;
        dram_addr = (tag << b << s) + ((set) << b);
        target_line = &cache[set].lines[rand() % E];
        for(i = 0; i < E; i++) if(cache[set].lines[i].valid && cache[set].lines[i].tag == tag) target_line =  &cache[set].lines[i];

        for(i = 0; i < B; i++) target_line->byte[i] = swaddr_read(dram_addr + i, 1);
        target_line->valid = true;
        target_line->tag = tag;
    }
}

// 1.解析地址  t e b  2.扫描是否存在   3.不存在：从dram中载入   4.读取信息，返回

