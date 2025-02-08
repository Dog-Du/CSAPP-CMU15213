#include "cachelab.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QUERY_BUFFER 512

struct Arguments {
    int s;
    int E;
    int b;
    int v;
    const char * file_name;
};

typedef struct Arguments Arguments;

struct Block {
    int64_t address;
    int used;
    int free;
};

typedef struct Block Block;

struct State {
    int hits;
    int misses;
    int evictions;
    Block **blocks;
};

typedef struct State State;

struct Query {
    char op;
    int64_t address;
    int size;
    int query_len;
    char buf[QUERY_BUFFER];
};

typedef struct Query Query;

void print_help() {
    printf(
        "Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n"
        "Options:\n"
        "-h         Print this help message.\n"
        "-v         Optional verbose flag.\n"
        "-s <num>   Number of set index bits.\n"
        "-E <num>   Number of lines per set.\n"
        "-b <num>   Number of block offset bits.\n"
        "-t <file>  Trace file.\n"
        "\n"
        "Examples:\n"
        "linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n"
        "linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n"
    );
}

void init_arguments(Arguments *arg) {
    arg->s = 0;
    arg->v = 0;
    arg->E = 0;
    arg->b =0;
    arg->file_name = NULL;
}

void solve_arg(int argc, const char *args[], Arguments *arg) {
    for (int i = 1;i < argc; ++i) {
        if (args[i][0] != '-') {
            print_help();
            exit(0);
        }

        switch(args[i][1]) {
            case 'v' : {
                arg->v = 1;
            } break;
            case 'b': {
                arg->b = strtol(args[++i], NULL, 10);
            } break;
            case 'E': {
                arg->E = strtol(args[++i], NULL, 10);
            } break;
            case 's': {
                arg->s = strtol(args[++i], NULL, 10);
            } break;
            case 't' : {
                arg->file_name = args[++i];
            } break;
            default: {
                print_help();
                exit(0);
            } break;
        }
    }

    if (arg->file_name == NULL) {
        printf("-t must.");
        exit(0);
    }

    if (arg->b <= 0 || arg->s <= 0 || arg->E <= 0) {
        printf("invalid argument");
        exit(0);
    }
}

void init_state(Arguments *arg, State *state) {
    state->evictions = 0;
    state->hits = 0;
    state->misses = 0;
    state->blocks = (Block **)malloc((1 << arg->s) * sizeof(Block *));

    for (int i = 0;i < (1 << arg->s); ++i) {
        state->blocks[i] = (Block *)malloc(arg->E * sizeof(Block));
        for (int j=0;j<arg->E; ++j) {
            state->blocks[i][j].address = 0;
            state->blocks[i][j].used = 0;
            state->blocks[i][j].free = 1;
        }
    }
}

void des_state(Arguments *arg, State *state) {
    for (int i = 0;i < (1 << arg->s); ++i) {
        free(state->blocks[i]);
    }
    free(state->blocks);
}

void swap_block(Block *left, Block *right) {
    Block tmp;
    tmp.address = left->address;
    tmp.free = left->free;
    tmp.used = left->used;

    left->address = right->address;
    left->free = right->free;
    left->used = right->used;

    right->address = tmp.address;
    right->free = tmp.free;
    right->used = tmp.used;
}

void move_block_to_front(Block *block, int i) {
    for (int j = i; j > 0; --j) {
        swap_block(&block[j], &block[j - 1]);
    }
}

// 这个方法不行。
void touch_block1(Arguments *arg, State *state, Block *block, int64_t address, int size) {
    for (int i = 0; i < arg->E; ++i) {
        if (block[i].address <= (address >> (arg->b + arg->s)) && block[i].free == 0) {
            // 因为题目说不会出现越块的访问情况。所以这样就相当于hit
            for (int j = 0;j < arg->E; ++j) {
                    if (block[j].used < block[i].used && block[j].free == 0) {
                        ++block[j].used;
                    }
                }

            block[i].used = 0;

            state->hits++;
            if (arg->v) {
                printf("hit ");
            }

            return;
        }
    }

    state->misses++;
    if (arg->v) {
        printf("miss ");
    }

    for (int i = 0; i < arg->E; ++i) {
        if (block[i].free == 1) {
            for (int j = 0;j < arg->E; ++j) {
                if (block[j].free == 0) {
                    ++block[j].used;
                }
            }

            block[i].free = 0;
            block[i].used = 0;
            block[i].address = address >> (arg->b + arg->s);
            return;
        }
    }

    state->evictions++;
    if (arg->v) {
        printf("eviction ");
    }

    int j = 0;
    for (int i = 1;i < arg->E; ++i) {
        ++block[i].used;
        if (block[i].used > block[j].used) {
            j = i;
        }
    }

    block[j].used = 0;
    block[j].address = address >> (arg->b + arg->s);
}

// 要用队列的方法。
// 2025.2.6 - 2025.2.6 ：
void touch_block2(Arguments *arg, State *state, Block *block, int64_t address, int size) {
    for (int i = 0; i < arg->E; ++i) {
        if (block[i].address == (address >> (arg->b + arg->s)) && block[i].free == 0) {
            // 因为题目说不会出现越块的访问情况。所以这样就相当于hit
            block[i].used = 0;
            block[i].free = 0;

            move_block_to_front(block, i);

            state->hits++;
            if (arg->v) {
                printf("hit ");
            }

            return;
        }
    }

    state->misses++;
    if (arg->v) {
        printf("miss ");
    }

    for (int i = 0; i < arg->E; ++i) {
        if (block[i].free == 1) {
            block[i].free = 0;
            block[i].used = 0;
            block[i].address = address >> (arg->b + arg->s);

            move_block_to_front(block, i);
            return;
        }
    }

    state->evictions++;
    if (arg->v) {
        printf("eviction ");
    }

    int evict = arg->E - 1;
    block[evict].address = address >> (arg->b + arg->s);
    block[evict].free = 0;
    block[evict].used = 0;
    move_block_to_front(block, evict);
}

void solve_load(Arguments *arg, State *state, Query *query) {
    int group = (query->address >> arg->b) & ((1 << arg->s) - 1);
    touch_block2(arg, state, state->blocks[group], query->address, query->size);
}

void solve_store(Arguments *arg, State *state, Query *query) {
    solve_load(arg, state, query);
}

void solve_modify(Arguments *arg, State *state, Query *query) {
    solve_load(arg, state, query);
    solve_store(arg, state, query);
}

int read_line(FILE *file, Query *query) {
    if (!fgets(query->buf, QUERY_BUFFER, file)) {
        return 0;
    }

    query->query_len = strlen(query->buf);
    if (query->buf[0] == ' ') {
        sscanf(query->buf + 1, "%c %lx,%d", &query->op, &query->address, &query->size);
    } else {
        sscanf(query->buf, "%c %lx,%d", &query->op, &query->address, &query->size);
    }
    query->buf[query->query_len - 1] = '\0';

    return 1;
}

int main(int argc, const char *args[])
{
    Arguments arg;
    State state;
    Query query;
    FILE *file;

    init_arguments(&arg);
    solve_arg(argc, args, &arg);
    init_state(&arg, &state);

    file = fopen(arg.file_name, "r");

    memset(query.buf, 0, QUERY_BUFFER);

    while (read_line(file, &query)) {
        if (arg.v && query.op != 'I') {
            printf("%s ", query.buf + (query.op == 'I' ? 0 : 1));
        }

        switch (query.op) {
            case 'I' : {

            } break;
            case 'L' : {
                solve_load(&arg, &state, &query);
            } break;
            case 'S' : {
                solve_store(&arg, &state, &query);
            } break;
            case 'M' : {
                solve_modify(&arg, &state, &query);
            } break;
        }

        if (arg.v && query.op != 'I') {
            printf("\n");
        }
        memset(query.buf, 0, QUERY_BUFFER);
    }

    des_state(&arg, &state);
    fclose(file);

    printSummary(state.hits, state.misses, state.evictions);
    return 0;
}
