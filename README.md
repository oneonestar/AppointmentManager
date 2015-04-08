Appointment Manager (AMR)
=========================
An appointment management software that have the calendar and scheduling function.

Build
-----
To compile the program.
```
make
```
The executable program is located in bin/.

To clean up the object files.
```
make clean
```

To clean up the object files and the executable file.
```
make remove
```

To join the source files into one AMR.c file
```
make onefile
```

Documentation
-------------
Project doucmentationi:  
doc/AMRReport.pdf  
  
API documentation:  
doc/API.pdf  

File structure
--------------
-bin/ ..... executable file  
-src/ ..... source and header files  
-obj/ ..... object files during make  
-doc/ ..... documentations  
-test/in .. testing input  
-test/out . testing output  

How to test
--------------
Use input redirection  
./bin/amr alice bob charlie < test/in/fcfs.in  
./bin/amr alice bob charlie < test/in/prio.in  
./bin/amr alice bob charlie < test/in/opti.in  
