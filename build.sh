if [ -d "build" ]; then rm -Rf build; fi
if [ -d "bin" ]; then rm -Rf bin; fi
if [ -d "simulation_results" ]; then rm -Rf simulation_results; fi
mkdir -p simulation_results
mkdir -p build
cd build || exit
rm -rf *
cmake ..
make
cd ..
echo Compilation done. Executable in the bin folder
