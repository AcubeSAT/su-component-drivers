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