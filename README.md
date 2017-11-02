# webclient

This is a HTTP WEB CLIENT Traffic Generator written in C++.

The idea behind this framework is simple.
The user provides this framework with the job details that needs to be performed such as 
1. Total number of jobs to be executed.
2. Job specific parameters.
3. Tasks to be performed at each stage.

Imagine a car assembly line Factory where a skeleton of the car is provided as the input to the stage1.
At each stage, the car is assembled with more body parts and moved on to the next stage.
At the final stage of the assembly line, the car is delivered to the user.
      
Similarly, we can split the current task into a bunch of sub tasks and assign each sub task with a state.
Once, the sub task at each state is completed, the job is moved over to the next state.
      
Note, that if a job is held up in a state for a longer period of time, then you will notice a huge queue buildup for that state. 

The framework automatically takes care of this problem by spawning/shrinking additional threads based upon the queue lengths of each state.
     
Singleton design pattern, pthreads, mutex and conditional variables, maps and vectors are used in this application.
     
This C++ framework can also be used to build HTTP PROXY server as well. The only change you need to is change the tasks performed at each stage.
