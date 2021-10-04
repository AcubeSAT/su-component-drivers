# Component Drivers Software
This repository contains the driver software needed to operate the peripheral devices of the Science Unit (Camera, Valves, Pumps, etc)

[A full list of the Science Unit's devices can be found here](https://helit.org/dist-ocdt-wv/)

## Creating a new driver
This repository will contain every driver. Each diferent driver will be located in different branches.

Some guidelines:
- If you are working on a new driver, remember to create a new branch for it.
- While developing your driver remember to create unit tests. You can't merge into the main branch without them especially in this repo
- Follow the coding guidelines you learnt during the software trainings.

# Basic On-Board Software Project Template

A template for any simple software project to start from.
It includes libraries as:
- Catch2 for unit testing
- ETL to replace STL in modern C++
Since the libraries are submodules you need to run
  ```
  git submodule update --init
  ```

It also includes a sample class with header and source files, as well as one test case.

[NOTE]: EACH INDIVIDUAL DEVELOPER SHOULD CHANGE THE GUARDS FROM "BASIC-PROJECT" ETC, TO MATCH THEIR SPECIFIC WORKING TASK. BE SURE TO UPDATE THE EXECUTABLE NAME IN CMAKESLIST.TXT AS WELL  
