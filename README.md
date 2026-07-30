# CIRRUS
<u>**C**</u>omputational <u>**I**</u>nfrastructure for <u>**R**</u>econfigurable <u>**R**</u>egimes and <u>**U**</u>nified <u>**S**</u>olvers. 

A package working with energy transport in fluid and heat transfer regimes. The main driver for the solver and problem implementation is in c++ with auxiliary scripts for plotting in python.

Solver aims to deal with fluid flows and heat transfer in 1D, 2D, and 3D space using multiple solver types.

Geometry discretization deals only in uniform, cartesian grid layouts. Further updates may seek to expand these capabilites.

Heat transfer implementations will use Dirichlect, Neumann, Robin, convective, and symmetry boundary conditions. 

To compile and run, you can use the comp_and_run.txt file. Create a .bat of the same name and remove the leading " and trailing " on the file (one set encapsulates the whole file).
