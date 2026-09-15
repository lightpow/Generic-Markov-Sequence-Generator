#define FILE_PATH_ERROR "Error: incorrect file path\n"
#define NUM_ARGS_ERROR  "Usage: tweets_generator <seed> <num_tweets> "\
                        "<file_path> [words_to_read] [seed_word]\n"

#define DELIMITERS " \n\t\r"
#define NUM_OF_MODS 3
#define ARGS_NUM1 4
#define ARGS_NUM2 5
#define ARGS_NUM3 6
#define FILE_LINE_LENGTH 1000
#define DECIMAL 10
#define MAX_GENERATION_LENGTH 20

#include <stdio.h>
#include <string.h>

#include "markov_chain.h"


static const int valid_args_num[NUM_OF_MODS] = {
    ARGS_NUM1, ARGS_NUM2, ARGS_NUM3
};



/**
 * checks if a given string has dot at the end.
 * @param smth
 * @return true if so, false otherwise.
 */
static bool is_finish_word(const void *smth)
{
    const char *word = smth;
    if ( !word || (strlen(word) < 1))
    {
        return false;
    }
    if ( word[strlen(word)-1] == '.')
    {
        return true;
    }
    return false;
}


/**
 * copies the given string by memory allocation.
 * @param to_copy
 * @return the new copied string.
 */
static void* copy_f(const void *to_copy)
{
    const char* src = to_copy;
    if (src == NULL)
    {
        return NULL;
    }
    char *dest = malloc(strlen(src) + 1);
    if (dest != NULL)
    {
        strcpy(dest, src);
    }
    return dest;
}


/**
 * wrapper for the print function.
 * @param data
 */
static void print_f(const void *data)
{
    printf("%s", (const char *)data);
}


/**
 * wrapper for the comp function.
 * @param first
 * @param second
 * @return
 */
static int compare_f(const void *first, const void *second)
{
    return strcmp(first, second);
}


/**
 * checks if the program has the correct args number.
 * @param current_argc
 * @param valid_counts
 * @param num_elements
 * @return true if valid, false otherwise.
 */
bool check_args_generic(int current_argc, const int* valid_counts,
                                                size_t num_elements)
{
    for (size_t i = 0; i < num_elements; ++i) {
        if (current_argc == valid_counts[i]) {
            return true;
        }
    }
    return false;
}



/**
 * generates a chosen number of sequences.
 * @param k the sequences number.
 * @param markov_chain
 * @param first_node node to start with.
 */
static void sequence_gen(int k, MarkovNode *first_node, MarkovChain *markov_chain)
{
    if (!markov_chain || !markov_chain->database)
    {
        return;
    }

    for ( int i = 0; i < k; i++)
    {
        int check = 0;
        if (!first_node)
        {
            first_node = get_first_random_node(markov_chain);
            check = 1;
        }
        generate_random_sequence(markov_chain, first_node,
                            MAX_GENERATION_LENGTH);
        if ( check)
        {
            first_node = NULL;
        }
    }
}



/**
 * fills the database using words from the given file.
 * @param fp the input file.
 * @param words_to_read the amount of words to read.
 * @param markov_chain the database.
 * @return 1 if failed, 0  otherwise.
 */
static int fill_database(FILE *fp, int words_to_read,
                        MarkovChain *markov_chain)
{
    if ( !fp || !markov_chain )
    {
        return 1;
    }
    int words_read = 0;
    char words[FILE_LINE_LENGTH+1];
    Node *current = NULL;
    Node *previous = NULL;
    while (fgets(words, FILE_LINE_LENGTH, fp) != NULL)
    {
        char *fin = strtok(words, DELIMITERS);
        while(fin)
        {
            current = add_to_database(markov_chain, fin);
            if ( !current)
            {
                free_database(&markov_chain);
                return 1;
            }
            if ( previous && !(previous->data->finish))
            {
                int stat = add_node_to_frequency_list(previous->data,
                                           current->data);
                if (stat)
                {
                    free_database(&markov_chain);
                    return 1;
                }
            }
            words_read++;
            if ( (words_to_read > 0 ) && (words_to_read <= words_read))
            {
                return 0;
            }
            if ( current->data->finish)
            {
                current = NULL;
            }
            previous = current;
            fin = strtok(NULL, DELIMITERS);
        }
    }
    return 0;
}



/**
 * runs the program by checking if given args and generate tweets.
 * @param argc
 * @param argv <seed> <num_tweets> <file_path> ##optional##[words_to_read] ##optional##[seed_word]
 * @return
 */
int main(int argc, char **argv)
{
    if ( ! check_args_generic(argc, valid_args_num, NUM_OF_MODS))
    {
        printf("%s\n", NUM_ARGS_ERROR);
        return EXIT_FAILURE;
    }
    unsigned int seed = (unsigned int)strtol(argv[1], NULL, DECIMAL);
    int num_tweets = (int)strtol(argv[2], NULL, DECIMAL);
    char *file_path = argv[3];
    int words_to_read = 0;
    if ( argc == ARGS_NUM2 || argc == ARGS_NUM3)
    {
        words_to_read = (int)strtol(argv[4], NULL, DECIMAL);
    }
    srand(seed);
    FILE *fp = fopen(file_path, "r");
    if (!fp)
    {
        printf("%s\n", FILE_PATH_ERROR);
        return EXIT_FAILURE;
    }
    MarkovChain *markov_chain = initiate_markov_chain(free, copy_f, compare_f,
                                                    print_f, is_finish_word);
    if ( !markov_chain )
    {
        fclose(fp);
        return EXIT_FAILURE;
    }
    if ( fill_database(fp, words_to_read, markov_chain))
    {
        free_database(&markov_chain);
        fclose(fp);
        return EXIT_FAILURE;
    }
    MarkovNode *seed_node = NULL;
    if ( argc == ARGS_NUM3)
    {
        seed_node = get_node_from_database(markov_chain, argv[5])->data;
    }
    sequence_gen(num_tweets, seed_node, markov_chain);
    free_database(&markov_chain);
    fclose(fp);
    return EXIT_SUCCESS;
}

