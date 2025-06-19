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

### Mp 4
Mp 4 My Resource in your Wallet
Build a thread-safe resource wallet and a multi-threaded server that allows multiple clients to interact with the wallet concurrently over a network.

### Mp 5
Mp 5 Getaway.html
Implement a multi-threaded HTTP server from scratch that:
Parses HTTP requests
Reads from sockets
Serves static .html and .png files
Understands and responds to browser requests using standard HTTP format

### Mp 6
Mp 6 Midnight Rain
Starting from this MP, the class changed its focus to python and networking services. This MP asked me to create a weather forecasting system based on the class schedule and the national weather service API. First I requested the course microservice to handle me the courses information to strip off any irrelevant characters, then I check the forecast to calculate the next meeting time, and return a json file with temperature, short forcast, course name, next meeting time and the forecast hour.

### Mp 7 
Mp 7 Dancing with Our C Code
This MP was the continuation of MP 2, but it just connected the MP 2 server to Mp 7 and to extract a hidden gif out of the png file.

### Mp 8
Mp 8 Come Back... Be Cached (Mandelbrot's Version)
This was one of the most interesting MP I've ever done in this course. In this MP we get the first impression on AWS storage like boto3. We created some small apis to manipulate the mandelbrot image, and how to load it onto our microservice for display. It was interactive so it was easy to test what we did. 

### Mp 9
Mp 9 Bigger Than the Whole Maze
This Mp was a project for the entire class to cooperate. I was asked to create a 7*7 matrix that represents a maze. Then, every student would join a large server to combine their work into an infinite maze. In this particular MP, I utilized code from one previous course, which happened to be creating mazes as well. I transcripted that code from C++ to python and it worked exactly what I wanted. 

### Final Project
This Final Project was also a collaborative project. Everyone would try to upload a list of images, and they would vote for each other. The one with the highest number of votes would start to conquer other areas(and devour other images). Eventually, one final image would stand out. But unfortunately, the display of this project was failed for some technical issues.


      
