/**
 * Cache Lab Simulator
 * By: Tom Graham
 * Date: May, 2018
 */

#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>

//We can use one struct for the entire cache.
struct Cache{
    long valid;
    long tag;
    long lru;
};

// These are used to keep track of the hits, misses and evictions.
int hit_count = 0;
int miss_count = 0;
int evict_count = 0;

// Setup variables, no help flag as it defaults to the help menu.
// -v: Verbose flag that displays trace info
// -s <s>: Number of set index bits (S = 2s is the number of sets)
// -E <E>: Associativity (number of lines per set)
// -b <b>: Number of block bits (B = 2b is the block size)
// -t <tracefile>: Name of the valgrind trace to replay
int s, b, E, v, size, B;

/**
 * Initialize the cache struct.
 */
struct Cache **cache = NULL;

//Set up the counter for lru and a mask,
//both being set to long for precision.
long mask = 0;
long lruCounter = 0;

void fileTrace(char *fileName);
void print(char* argv);
void initCache();
void cleanCache();
void useCache(unsigned long address);

/**
 * A function used to run the trace of the valgrind file.
 * @param fileName the variable used for the actual filename of the trace
 */
void fileTrace(char *fileName)
{
    char buf[1024];

    //Try to open the file.
    FILE *fp = fopen(fileName, "r");

    //If file is empty, print error and return.
    if (fp == NULL)
    {
        printf("The file cannot be found.\n\n");
        return;
    }

    //Now that we have our file open, read a line from the stream
    //and store it in buf
    while (fgets(buf, 1024, fp))
    {
        unsigned long address = 0;
        unsigned size = 0;

        //Now we check for the chars, L, S, or M.
        //L is a data load.
        //S is a data store
        //M is data modify
        if (buf[1] == 'S' ||
            buf[1] == 'L' ||
            buf[1] == 'M'){

            //Then after that we know the next position is two indices over
            //So we add 2 and the address should next then, the size.
            sscanf(buf + 2, "%lx,%u", &address, &size);

            //If the verbose flag is 1 then we print.
            if (v)
                printf("%c %lx,%u ", buf[1], address, size);
            useCache(address);

            //If it is M then we modify the data, simply by calling the useCache function.
            if (buf[1] == 'M')
            {
                useCache(address);
            }

            if (v) putchar('\n');
        }
    }
    //Close the file
    fclose(fp);
}

/**
 * This function is used to initiate the cache.
 */
void initCache()
{
    //We first allocate the memory for the cache.
    cache = malloc(size * sizeof(*cache));

    //We then use calloc to our advantage and allocate memory
    //for the cache while also initializing it to zero
    for (int i = 0; i < size; i++)
    {
        //Our E is number of lines per set.
        cache[i] = calloc(E, sizeof(*cache[i]));
    }

    mask = size - 1;
}

void useCache(unsigned long address)
{
    //We go back out our favorite bitwise operations.
    //We right shift the address by b and use the mask to get the set.
    long set = (address >> b) &mask;

    //We right shift the address again by b + s to get the tag.
    long tag = (address >> (b+s));

    int x = 0;

    //We loop through the cache.
    while (x < E)
    {
        //We check if both the tag and the valid bit are not 0.
        if (cache[set][x].tag == tag && cache[set][x].valid != 0)
        {
            //If true we have a hit, so we increase our hit counter, and print hit.
            hit_count++;
            if (v) printf("hit ");

            //We also update the lru.
            cache[set][x].lru = lruCounter++;
            return;
        }
        else{
            x++;
        }
    }

    //If it is not a hit then we know it is a miss so everything else is a miss.
    miss_count++;

    //We initialize the variables.
    //We need to initalzie the lrum to the lowest memory address for comparisons.
    long lrum = 0xFFFFFFFF;
    int evictL = 0;

    if (v) printf("miss ");

    int i = 0;
    while (i < E)
    {
        //If the lru min is greater than the counter
        if (lrum > cache[set][i].lru)
        {
            evictL = i;
            lrum = cache[set][i].lru;
        }
        i++;
    }

    //We check if the valid bit is 1, if it is then increase the hit count, and print eviction.
    if (cache[set][evictL].valid == 1)
    {
        evict_count++;
        if (v) printf("eviction ");
    }

    //We set the rest of the cache before exiting function
    cache[set][evictL].valid = 1;
    cache[set][evictL].tag = tag;
    cache[set][evictL].lru = lruCounter++;
}

void cleanCache()
{
    free(cache);
}

void print(char * argv)
{
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    exit(0);
}

int main (int argc, char * argv[])
{
    //We initialize the variables.
    char opt = 0;
    char *file = NULL;

    //We use getopt to handle the arguments from the command line.
    while ((opt = getopt(argc, argv, "s:E:b:t:vh")) != -1)
    {
        switch (opt)
        {
            case 's':
                s = atoi(optarg);

                if (s == 0){
                    return 1;
                }
                break;
            case 'E':
                E = atoi(optarg);
                if (E == 0){
                    return 1;
                }
                break;
            case 'b':
                b = atoi(optarg);
                if (b == 0){
                    return 1;
                }
                break;
            case 't':
                file = optarg;
                if (file == 0){
                    return 1;
                }
                break;
            case 'v':
                v = 1;
                break;
            default:
                print(argv[0]);
                return 1;
        }
    }

    //We must update the size and number of block bits, using bitwise we left shift
    // the 1 by s and set it to size, and same for B.
    size = 1 << s;
    B = 1 << b;

    //We then call each function and print.
    initCache();
    fileTrace(file);
    cleanCache();

    printSummary(hit_count, miss_count, evict_count);
    return 0;
}