cmake_minimum_required(VERSION 3.28)
project(ex3b C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

# add snakes_and_ladders.c or tweets_generator.c based on which you want to run.
add_executable(ex3b
        linked_list.c
        linked_list.h
        markov_chain.c
        markov_chain.h)
