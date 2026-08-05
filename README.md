# CIRRUS
<u>**C**</u>omputational <u>**I**</u>nfrastructure for <u>**R**</u>econfigurable <u>**R**</u>egimes and <u>**U**</u>nified <u>**S**</u>olvers. 

A package working with energy transport in fluid and heat transfer regimes. The main driver for the solver and problem implementation is in c++ with auxiliary scripts for plotting in python.

Solver aims to deal with fluid flows and heat transfer in 1D, 2D, and 3D space using multiple solver types.

Geometry discretization deals only in uniform, cartesian grid layouts. Further updates may seek to expand these capabilites.

Heat transfer implementations will use Dirichlect, Neumann, Robin, convective, and symmetry boundary conditions. 

To compile and run, you can use the scripts/build.txt (validation tests) and scripts/test.txt (field test validatation) files. Create a .bat of the same name and remove the leading " and trailing " on the file (one set encapsulates the whole file).


# SETUP
The setup for this coding package was intended for Linux based operating systems. However, the compiler is written to be OS agnostic between Windows and Linux and should run on both.

CIRRUS relies on .json files for case inputs. These are parsed with nlohmann.json and is a required dependancy. This can be added to your machine using the the following:

Windows/MSYS2: 
    bash:
        pacman -S mingw-w64-ucrt-x86_64-nlohmann-json

Ubuntu:
  bash:
    sudo apt install nlohmann-json3-dev

Futhermore, for plotting purposes, the package requires a python interpreter or a .vtk viewer such paraview. However, if not provided, no automatic plotting will occur. The produced .csv and .vtk files are stored in the projects output/ folder and can be used for viewing externally to the package using tools such as excel, MATLAB, python and paraview. 

# DISCLAIMER

I do not claim the ideas from this or the code as explicitly that of the author's. This project was approached as a learning experience as the author doesn't have much experience in such large-scale coding packages. Much of the package was completed with the help of AI (ChatGPT) though each step was tested and vetted to make sure that the structure was readable. AI assisted in the creation of code and documentation, though the content was edited significantly to match the author's preference. 
