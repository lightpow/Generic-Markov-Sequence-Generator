#include <string.h>
#include "markov_chain.h"

#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

#define EMPTY (-1)
#define BOARD_SIZE 100
#define MAX_GENERATION_LENGTH 60
#define DECIMAL 10
#define DICE_MAX 6
#define NUM_OF_TRANSITIONS 20
#define NUM_OF_MODS 1
#define NUM_OF_ARGS 3

#define NUM_ARGS_ERROR "Usage: invalid number of arguments"


static const int valid_args_num[NUM_OF_MODS] = {
    NUM_OF_ARGS
};



/**
 * represents the transitions by ladders and snakes in the game
 * each tuple (x,y) represents a ladder from x to if x<y or a snake otherwise
 */
static const int transitions[][2] = {
    {13, 4},
    {85, 17},
    {95, 67},
    {97, 58},
    {66, 89},
    {87, 31},
    {57, 83},
    {91, 25},
    {28, 50},
    {35, 11},
    {8, 30},
    {41, 62},
    {81, 43},
    {69, 32},
    {20, 39},
    {33, 70},
    {79, 99},
    {23, 76},
    {15, 47},
    {61, 14}
};


/**
 * struct represents a Cell in the game board
 */
typedef struct Cell {
    int number; // Cell number 1-100
    int ladder_to; // cell which ladder leads to, if there is one
    int snake_to; // cell which snake leads to, if there is one
    //both ladder_to and snake_to should be -1 if the Cell doesn't have them
} Cell;


/**
 * allocates memory for cells on the board and initalizes them
 * @param cells Array of pointer to Cell, represents game board
 * @return EXIT_SUCCESS if successful, else EXIT_FAILURE
 */
static int create_board(Cell *cells[BOARD_SIZE])
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        cells[i] = malloc(sizeof(Cell));
        if (cells[i] == NULL)
        {
            for (int j = 0; j < i; j++)
            {
                free(cells[j]);
            }
            printf(ALLOCATION_ERROR_MESSAGE);
            return EXIT_FAILURE;
        }
        *(cells[i]) = (Cell){i + 1, EMPTY, EMPTY};
    }

    for (int i = 0; i < NUM_OF_TRANSITIONS; i++)
    {
        int from = transitions[i][0];
        int to = transitions[i][1];
        if (from < to)
        {
            cells[from - 1]->ladder_to = to;
        } else
        {
            cells[from - 1]->snake_to = to;
        }
    }
    return EXIT_SUCCESS;
}


/**
 * manually allocate a new Node and inserts to the data base aka markov_chain.
 * @param markov_chain /
 * @param cells
 * @return EXIT_SUCCESS if successful, EXIT_FAILURE otherwise.
 */
static int add_cells_to_database(MarkovChain *markov_chain, Cell *cells[BOARD_SIZE])
{
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        Node *tmp = add_to_database(markov_chain, cells[i]);
        if (tmp == NULL)
        {
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}


/**
 * sets the frequencies list of all cells based on the data of the target cell if valid, adds the next sex cells otherwise.
 * @param markov_chain
 * @param cells
 * @return EXIT_SUCCESS if successful, EXIT_FAILURE otherwise.
 */
static int set_nodes_frequencies(MarkovChain *markov_chain, Cell *cells[BOARD_SIZE])
{
    MarkovNode *from_node = NULL, *to_node = NULL;
    size_t index_to;

    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        // This function should be called after allocating the cells and inserting them to the data base, therefore wont return a null ptr.
        from_node = get_node_from_database(markov_chain, cells[i])->data;
        if (cells[i]->snake_to != EMPTY || cells[i]->ladder_to != EMPTY)
        {
            index_to = MAX(cells[i]->snake_to, cells[i]->ladder_to) - 1;
            to_node = get_node_from_database(markov_chain,
                                             cells[index_to])->data;
            int res = add_node_to_frequency_list(from_node, to_node);
            if (res == EXIT_FAILURE)
            {
                return EXIT_FAILURE;
            }
        }
        else
        { // adds the next sex cells to the cur cell frequency if the cell has no ladder or snake target cell.
            for (int j = 1; j <= DICE_MAX; j++)
            {
                index_to = ( (Cell *) (from_node->data) )->number + j - 1;
                if (index_to >= BOARD_SIZE)
                {
                    break;
                }
                to_node = get_node_from_database(markov_chain,
                                                 cells[index_to])->data;
                int res = add_node_to_frequency_list(from_node, to_node);
                if (res == EXIT_FAILURE)
                {
                    return EXIT_FAILURE;
                }
            }
        }
    }
    return EXIT_SUCCESS;
}


/**
 * fills database
 * @param markov_chain
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int fill_database_snakes(MarkovChain *markov_chain)
{
    Cell *cells[BOARD_SIZE];
    if (create_board(cells) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    if (add_cells_to_database(markov_chain, cells) == EXIT_FAILURE)
    {
        for (size_t i = 0; i < BOARD_SIZE; i++)
        {
            free(cells[i]);
        }
        return EXIT_FAILURE;
    }

    if(set_nodes_frequencies(markov_chain, cells) == EXIT_FAILURE)
    {
        for (size_t i = 0; i < BOARD_SIZE; i++)
        {
            free(cells[i]);
        }
        return EXIT_FAILURE;
    }

    // free temp arr
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        free(cells[i]);
    }
    return EXIT_SUCCESS;
}


/**
 * prints the cell in the walk.
 * @param to_print
 */
static void print_cell(const void* to_print)
{
    const Cell* cell = to_print;
    if ( !cell)
    {
        return;
    }
    printf( "[%d]", cell->number);
    if (cell->snake_to != EMPTY)
    {
        printf( " -snake to->");
        return;
    }
    if (cell->ladder_to != EMPTY)
    {
        printf( " -ladder to->");
        return;
    }
    if ( cell->number != BOARD_SIZE)
    {
        printf( " ->");
    }
}


/**
 * copies the cell content to a new cell.
 * @param to_copy
 * @return the new allocated cell.
 */
static void* copy_cell( const void* to_copy )
{
    const Cell* cell = to_copy;
    if (!cell)
    {
        return NULL;
    }
    Cell* new_cell = (Cell*) malloc(sizeof(Cell));
    if (new_cell == NULL)
    {
        return NULL;
    }
    new_cell->number = cell->number;
    new_cell->ladder_to = cell->ladder_to;
    new_cell->snake_to = cell->snake_to;
    return new_cell;
}

/**
 * compares the cells content.
 * @param cell1
 * @param cell2
 * @return the compare value.
 */
static int cmp_cell( const void* cell1, const void* cell2 )
{
    return ((const Cell *)cell1)->number - ((const Cell *)cell2)->number;
}


/**
 * checks if the given cell is the last cell.
 * @param to_check
 * @return true if so, false otherwise.
 */
static bool last_cell( const void* to_check )
{
    const Cell* cell = to_check;
    if ( cell->number == BOARD_SIZE )
    {
        return true;
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
 * @param argc num of arguments
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int main(int argc, char *argv[])
{
    if ( ! check_args_generic(argc, valid_args_num, NUM_OF_MODS))
    {
        printf("%s\n", NUM_ARGS_ERROR);
        return EXIT_FAILURE;
    }

    MarkovChain *markov_chain = initiate_markov_chain(free, copy_cell,
                        cmp_cell, print_cell, last_cell);
    if (!markov_chain)
    {
        return EXIT_FAILURE;
    }

    unsigned int seed = strtoul(argv[1], NULL, DECIMAL);
    int num_of_sequences = (int) strtoul(argv[2], NULL, DECIMAL);

    if ( fill_database_snakes(markov_chain) == EXIT_FAILURE )
    {
        free_database(&markov_chain);
        return EXIT_FAILURE;
    }

    srand(seed);
    sequence_gen(num_of_sequences, markov_chain->database->first->data,
                markov_chain);

    free_database(&markov_chain);
    return EXIT_SUCCESS;
}
