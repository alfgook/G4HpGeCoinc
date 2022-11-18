This is a framework for simulation of coincidence spectrometry of (segmented) HPGe detectors, based on geant4.

to build this project, make a build directory inside the main project directory (G4HPGecoinc), for example

mkdir build

run cmake in the build directory

	cd build
	cmake ..

for this to work you need to have geant4 installed and properly setup on your system, see https://geant4-userdoc.web.cern.ch/UsersGuides/InstallationGuide/html/

To run the application in GUI mode, from the build directory

	./G4HPGecoinc -t 1

where the option -t 1 specifies the number of threads to be used for the simulation.

To run the application in batch mode (for large simualtions)

	./G4HPGecoinc -t 8 -m mymacro.mac

where the option -m XXX specifies a macro file to set up the simulation.

There are a number of detector setups that can be chosen and modified through geant4 UI commands. See the file detectorSetup.mac for more information. In GUI mode, the application will look for the file detectorSetup.mac in the build directory to build the detector (this file is not copied into the build directory by default). If the file detectorSetup.mac is not found a default dual detector setup is built. When running in batch mode the file detectorSetup.mac is ignored, the detector setup commands should then be placed in the macro file following the -m option.

There are further UI commands to control the radioactive source, see the example macro La140.mac.

It is also possible to use the cosmic ray shower event generator (CRY) as source, to perform cosmic background estimates.

To run with CRY, the library needs to be build on your computer. Follow the instructions in the manual https://nuclear.llnl.gov/simulation/doc_cry_v1.7/cry.pdf. Make sure that this results in the file libCRY.a in the subfolder lib, where you built the cry libary. Once the CRY library has been built you need to rebuild the G4HPGeCoinc application. To help the compiler find the CRY library, do 

	cmake -DCRY_PATH:PATH='/home/alf/projects/GammaCoinc/cry_v1.7' ..

where you replace '/home/alf/projects/GammaCoinc/cry_v1.7' with the path to the directory where you just built CRY.

To run with CRY as primary event generator

	./G4HPGecoinc -t 1 -cry 1

The cry event generator will setup by reading from the file default.cry.setup, which is copied into the build directory by cmake. To control cry this file may be edited. There are also UI commands to modify the CRY setup, see https://nuclear.llnl.gov/simulation/doc_cry_v1.7/cry.pdf for details.