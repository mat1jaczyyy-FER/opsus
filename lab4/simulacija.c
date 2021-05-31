// Dominik Matijaca 0036524568
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char byte;
typedef unsigned int uint;

typedef struct {
    int length;
    byte id, allocated;
} memory_segment_t;

memory_segment_t* memory;
int n;
int size = 0;
uint z = 0;

void memory_insert(int index, memory_segment_t segment) {
    if (index >= n || index > size) exit(1);

    for (int i = size - 1; i >= index; i--)
        memory[i + 1] = memory[i];
    
    memory[index] = segment;
    size++;
}

void memory_push(memory_segment_t segment) {
    memory_insert(size, segment);
}

void memory_remove(int index) {
    if (index >= size) exit(1);

    for (int i = index; i < size; i++)
        memory[i] = memory[i + 1];
    
    size--;
}

typedef struct {
    char disp[3];
    int left;
} print_state_t;

print_state_t print_create(int index) {
    if (index >= size) exit(1);

    print_state_t ret = {"--", memory[index].length};

    if (memory[index].allocated)
        sprintf(ret.disp, "%02X", memory[index].id);

    return ret;
}

void memory_print() {
    printf("\n      ");

    int cols = n >= 0x10? 0xF : (n - 1);

    for (int i = 0; i <= cols; i++)
        printf("  %01X", i);

    printf("\n     ┌─");

    for (int i = 0; i <= cols; i++)
        printf("───");
    
    int j = 0;

    for (int i = 0; i < size; i++) {
        print_state_t cur = print_create(i);

        for (; cur.left > 0; j++, cur.left--) {
            if ((j & 0xF) == 0)
                printf("\n%04X │", j);

            printf(" %s", cur.disp);
        }
    }

    printf("\n\n");
}

void finish() {
    free(memory);
    exit(0);
}

int main(int argc, char** argv) {
    if (argc != 2) return 1;

    n = atoi(argv[1]);
    memory = malloc(sizeof(*memory) * n);

    sigset(SIGINT, finish);

    memory_push((memory_segment_t){n});

    memory_print();

    while (1) {
        char input = getchar() & 0xDF;
        uint s;

        switch (input) {
            case 'Z':
                scanf("%d", &s);

                z = (z + 1) & 0xFF;
                printf("Novi zahtjev %02X za %d mjesta...\n", z, s);

                int min_i = -1;
                int min_length = n + 1;

                for (int i = 0; i < size; i++) {
                    if (!memory[i].allocated && memory[i].length >= s && memory[i].length < min_length) {
                        min_i = i;
                        min_length = memory[i].length;
                    }
                }
                
                if (min_i == -1) {
                    printf("Ne postoji prikladan dio memorije za ovaj zahtjev.\n");
                    break;
                }

                memory[min_i] = (memory_segment_t){s, z, 1};

                if (min_length != s)
                    memory_insert(min_i + 1, (memory_segment_t){min_length - s});

                memory_print();
                break;
            
            case 'O':
                scanf("%x", &s);
                s &= 0xFF;

                printf("Oslobađa se zahtjev %02X...\n", s);

                for (int i = 0; i < size; i++) {
                    if (memory[i].id != s) continue;

                    memory[i].allocated = 0;

                    if (i - 1 >= 0 && !memory[i - 1].allocated) {
                        memory[i - 1].length += memory[i].length;
                        memory_remove(i--);
                    }
                    
                    if (i + 1 < size && !memory[i + 1].allocated) {
                        memory[i].length += memory[i + 1].length;
                        memory_remove(i + 1);
                    }
                }

                memory_print();
                break;
            
            case 'D':
                printf("Defragmentacija\n");

                int empty = n;

                int j = 0;
                for (int i = 0; i < size; i++) {
                    if (!memory[i].allocated) continue;

                    empty -= memory[i].length;
                    memory[j++] = memory[i];
                }

                memory[j] = (memory_segment_t){empty};
                size = j + 1;

                memory_print();
                break;
        }
    }
}