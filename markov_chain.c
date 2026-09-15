#include "markov_chain.h"

#include <string.h>

/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
static int get_random_number(int max_number)
{
    return rand() % max_number;
}



Node* get_node_from_database(MarkovChain *markov_chain, void *data_ptr)
{
    if ( !markov_chain || !data_ptr || (markov_chain->database == NULL) )
    {
        return NULL;
    }

    LinkedList *list = markov_chain->database;
    Node *current = list->first;
    while (current != NULL)
    {
        if (current->data && current->data->data)
        {
            if (markov_chain->cp_func(current->data->data,
                data_ptr) == 0)
            {
                return current;
            }
        }
        current = current->next;
    }
    return NULL;
}



Node* add_to_database(MarkovChain *markov_chain, void *data_ptr)
{
    if (!markov_chain || !data_ptr || (markov_chain->database == NULL))
    {
        return NULL;
    }
    Node *existing = get_node_from_database(markov_chain, data_ptr);
    if (existing)
    {
        return existing;
    }
    LinkedList *list = markov_chain->database;
    MarkovNode *tmp = malloc(sizeof(MarkovNode));
    if (!tmp)
    {
        printf("%s\n", ALLOCATION_ERROR_MESSAGE);
        return NULL;
    }
    tmp->data = markov_chain->cy_func(data_ptr);
    if (!tmp->data)
    {
        free(tmp);
        printf("%s\n", ALLOCATION_ERROR_MESSAGE);
        return NULL;
    }
    tmp->finish = 0;
    if ( markov_chain->il_func(data_ptr))
    {
        tmp->finish = 1;
    }
    tmp->total_frequency = 0;
    tmp->frequency_list_size = 0;
    tmp->frequency_list = NULL;
    if (add(list, tmp))
    {
        markov_chain->f_func(tmp->data);
        free(tmp);
        printf("%s\n", ALLOCATION_ERROR_MESSAGE);
        return NULL;
    }
    return list->last;
}


int add_node_to_frequency_list(MarkovNode *first_node, MarkovNode *second_node)

{
    if ( !first_node || !second_node )
    {
        return 1;
    }
    for (int i = 0 ; i < first_node->frequency_list_size; i++)
    {
        if (first_node->frequency_list[i].markov_node == second_node)
        {
            first_node->frequency_list[i].frequency++;
            first_node->total_frequency++;
            return 0;
        }
    }
    MarkovNodeFrequency *tmp = realloc(first_node->frequency_list,
    sizeof(MarkovNodeFrequency)*(first_node->frequency_list_size + 1));
    if (tmp == NULL)
    {
        return 1;
    }
    first_node->frequency_list = tmp;
    first_node->frequency_list[first_node->frequency_list_size].markov_node =
            second_node;
    first_node->frequency_list[first_node->frequency_list_size].frequency = 1;
    first_node->total_frequency++;
    first_node->frequency_list_size++;
    return 0;
}


void free_database(MarkovChain ** ptr_chain)
{
    if ( !ptr_chain || !*ptr_chain || !(*ptr_chain)->database )
    {
        return;
    }
    Node *current = (*ptr_chain)->database->first;
    while (current != NULL)
    {
        Node *next = current->next;
        free(current->data->frequency_list);
        (*ptr_chain)->f_func(current->data->data);
        free(current->data);
        free(current);
        current = next;
    }
    free((*ptr_chain)->database);
    free(*ptr_chain);
    *ptr_chain = NULL;
}


MarkovNode *get_node_by_frequency (const MarkovNodeFrequency* frequencies,
                                    int i, int total)
{
    if( !frequencies || i >= total || i < 0)
    {
        return NULL;
    }

    const MarkovNodeFrequency *mv = frequencies;
    int mx = -1;
    int mn = -1;
    while(mx<total)
    {
        mx += mv->frequency;
        if( i <= mx && i > mn)
        {
            return mv->markov_node;
        }
        mv++;
        mn = mx;
    }
    return NULL;
}



MarkovNode* get_first_random_node(MarkovChain *markov_chain)
{
    if ( !markov_chain || !markov_chain->database ||
        !markov_chain->database->first)
    {
        return NULL;
    }
    MarkovNode *to_return = NULL;
    while ( !to_return)
    {
        int i = 0;
        int idx = get_random_number(markov_chain->database->size);
        Node *cur_node = markov_chain->database->first;
        for (; i < idx; i++)
        {
            cur_node = cur_node->next;
        }
        if ( !cur_node->data->finish)
        {
            to_return = cur_node->data;
        }
    }
    return to_return;
}


MarkovNode* get_next_random_node(MarkovNode *cur_markov_node)
{
    if (!cur_markov_node || cur_markov_node->frequency_list == NULL)
    {
        return NULL;
    }
    int idx = get_random_number(cur_markov_node->total_frequency);
    MarkovNode *next_markov_node = get_node_by_frequency(
                                cur_markov_node->frequency_list,
                                idx, cur_markov_node->total_frequency);
    return next_markov_node;
}



void generate_random_sequence(MarkovChain *markov_chain,
                              MarkovNode *first_node, int max_length)
{
    if ( !first_node)
    {
        return;
    }
    static int count = 0;
    count++;
    printf("Sequence %d: ", count);
    int idx = 0;
    MarkovNode *cur_markov_node = first_node;
    while (idx < max_length)
    {
        markov_chain->p_func(cur_markov_node->data);
        idx++;
        if (cur_markov_node->finish || idx == max_length)
        {
            break;
        }
        cur_markov_node = get_next_random_node(cur_markov_node);
        if (!cur_markov_node)
        {
            break;
        }
        printf(" ");
    }
    printf("\n");
}


/**
 * init the markov chain for the main func.
 * @return a new allocated empty data base aka markov_chain.
 */
MarkovChain *initiate_markov_chain(free_data free_f, copy_func copy_f,
            comp_func compare_f, print_func print_f, is_last is_finish_word)
{
    MarkovChain *markov_chain = malloc (sizeof(MarkovChain));
    LinkedList *list = malloc (sizeof(LinkedList));
    if  ( !markov_chain || !list )
    {
        printf("%s\n", ALLOCATION_ERROR_MESSAGE);
        free(markov_chain);
        if (list)
        {
            free(list);
        }
        return NULL;
    }
    list->first = NULL;
    list->last = NULL;
    list->size = 0;
    markov_chain->database = list;
    markov_chain->f_func = free_f;
    markov_chain->cy_func = copy_f;
    markov_chain->p_func = print_f;
    markov_chain->cp_func = compare_f;
    markov_chain->il_func = is_finish_word;
    return markov_chain;
}