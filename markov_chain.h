#ifndef _MARKOV_CHAIN_H
#define _MARKOV_CHAIN_H

#include "linked_list.h"
#include <stdio.h>  // For printf()
#include <stdlib.h> // For malloc()
#include <stdbool.h> // for bool

#define ALLOCATION_ERROR_MESSAGE "Allocation failure: Failed to allocate"\
         "new memory\n"


/***************************/
/*   functions typedefs    */
/***************************/
typedef void (* print_func)(const void *data_ptr);
typedef int (* comp_func)(const void *first_node, const void *second_node);
typedef void (* free_data)(void *data_ptr);
typedef bool (* is_last)(const void *data_ptr);
typedef void* (* copy_func)(const void *data_ptr);



/***************************/
/*        STRUCTS          */
/***************************/

typedef struct MarkovNode{
    void *data;
    struct MarkovNodeFrequency *frequency_list;
    int frequency_list_size;
    int total_frequency;
    int finish; // 1 if the word is a tweets finish word, 0 otherwise.
} MarkovNode;

typedef struct MarkovNodeFrequency {
    struct MarkovNode *markov_node;
    int frequency;
} MarkovNodeFrequency;

/* DO NOT ADD or CHANGE */
typedef struct MarkovChain {
    LinkedList *database;

    print_func p_func;

    comp_func cp_func;

    free_data f_func;

    copy_func cy_func;

    is_last il_func;
} MarkovChain;



/***************************/
/*        DECLARATIONS     */
/***************************/

/**
 * Check if data_ptr is in database. If so, return the markov_node wrapping
 * it in the markov_chain, otherwise return NULL.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return Pointer to the Node wrapping given state, NULL if state not in
 * database.
 */
Node *get_node_from_database(MarkovChain *markov_chain, void *data_ptr);

/**
* If data_ptr in markov_chain, return its node. Otherwise, create new
 * node, add to end of markov_chain's database and return it.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return node wrapping given data_ptr in given chain's database
 */
Node *add_to_database(MarkovChain *markov_chain, void *data_ptr);

/**
 * Add the second markov_node to the frequency list of the first markov_node.
 * If already in list, update its frequency value.
 * @param first_node
 * @param second_node
 * @return success/failure: 0 if the process was successful, 1 if in
 * case of allocation error.
 */
int add_node_to_frequency_list(MarkovNode *first_node,
                               MarkovNode *second_node);

/**
 * Free markov_chain and all of it's content from memory
 * @param chain_ptr markov_chain to free
 */
void free_database(MarkovChain **chain_ptr);

/**
 * returns the Node in the i place in the frequently list.
 * @param frequencies
 * @param i
 * @param total the frequently list length.
 * @return the markov_node with the given freq number.
 */
MarkovNode *get_node_by_frequency
(const MarkovNodeFrequency* frequencies,  int i, int total);

/**
 * Get one random markov node from the given markov_chain's database.
 * @param markov_chain
 * @return MarkovNode of the chosen state that is not a "last state"
 * in sequence.
 */
MarkovNode *get_first_random_node(MarkovChain *markov_chain);

/**
 * Choose the next node, by its occurrence frequency in current node.
 * @param cur_markov_node MarkovNode to choose from
 * @return MarkovNode of the chosen state
 */
MarkovNode *get_next_random_node(MarkovNode *cur_markov_node);

/**
 * Receive markov_chain, generate and print random sequences out of it. The sequence most have at least 2 words in it.
 * @param markov_chain
 * @param first_node markov_node to start with, if NULL- choose a
 * random markov_node
 * @param  max_length maximum length of chain to generate
 */
void generate_random_sequence(MarkovChain *markov_chain,
                              MarkovNode *first_node, int max_length);


/**
 * init the markov chain for the main func.
 * @param free_f
 * @param copy_f
 * @param compare_f
 * @param print_f
 * @param is_finish_word
 * @return a new allocated empty data base aka markov_chain.
 */
MarkovChain *initiate_markov_chain(free_data free_f, copy_func copy_f,
        comp_func compare_f, print_func print_f, is_last is_finish_word);


#endif /* MARKOV_CHAIN_H */
