# CS 340 Project

This is the summary of machine problems in Fall 2023 CS340. This course focused on computer systems and backends, with assignments completed in both C and Python.

## Table of Contents
- [Introduction](#introduction)
- [Machine Problems](#machine-problems)
  - [Mp 0](#mp-0)
  - [Mp 1](#mp-1)
  - [Mp 2](#mp-2)
  - [Mp 3](#mp-3)
  - [Mp 4](#mp-4)
  - [Mp 5](#mp-5)
  - [Mp 6](#mp-6)
  - [Mp 7](#mp-7)
  - [Mp 8](#mp-8)
  - [Mp 9](#mp-9)
  - [Final Project](#final-project)

## Introduction

Brief introduction about the course and its objectives.

## Machine Problems

### Mp 0

Mp 0 was just an introduction to C and nothing else. This MP asked us to familiarize with VSCode, and taught us how to debug using break points and code stepping.

### Mp 1 

Mp 1 Best Believe I'm Still 💎💎💎
This MP asked us to work with UTF-8 emojis. I learned that the emojis were encoded in 4 hexdecimal chunks. The MP was a great start in teaching us how to use malloc and free, as well as other basic C functions. I realized that C was extremely fundamental that it was coded according to memory locations. What was particularly noteworthy was that if I had to declare a variable, I had to first assign memory for that variable, then use strdup to give values to that variable, unlike simple "=" in C++ or python.

### Mp 2
Mp 2 I know Places
This MP continued on working with basic C functions, but this time we worked with PNG files. I implemented open, read and write functions for PNG.

### Mp 3
Mp 3 Alloc Too Well
This MP gave me the deepest impression. It asked us to implement the basic malloc and free and derivative functions from scratch. And it was not just simple memory application, it required block splitting(splitting a larger block into a smaller one if there was more space for further use), memory coalescing(coalesce two consecutive freed blocks into one larger), free list(use linked list to store the freed blocks).
      
