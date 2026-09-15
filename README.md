# Generic Markov Sequence Generator

A generic Markov chain implementation in C that can generate sequences based on transition probabilities learned from input data.

## Overview

This project implements a reusable Markov chain data structure using C, dynamic memory allocation, and linked lists.

The implementation is designed to work with different data types through generic function pointers for:

- Data comparison
- Data copying
- Data printing
- Data freeing

The Markov chain can be used to generate sequences by randomly following transitions between stored items according to their learned frequencies.

## Features

- Generic Markov chain implementation in C
- Dynamic memory management
- Linked-list-based data structures
- Transition probability calculation
- Random sequence generation
- File-based input processing
- Configurable random seed
- Support for different item types

## Example Applications

The repository includes two example applications:

### Tweets Generator

Generates tweet-like sequences using words extracted from an input text file and a Markov chain built from word transitions.

### Snakes and Ladders

Uses a Markov chain to model movement between positions on a Snakes and Ladders board and generate random game sequences.

## Project Structure

- `markov_chain.c` / `markov_chain.h` — Core Markov chain implementation
- `linked_list.c` / `linked_list.h` — Linked list implementation
- `tweets_generator.c` — Text sequence generation example
- `snakes_and_ladders.c` — Snakes and Ladders example
- `CMakeLists.txt` — Build configuration
