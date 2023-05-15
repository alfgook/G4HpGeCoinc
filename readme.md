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

where the option -m mymacro.mac specifies a macro file (here mymacro.mac) to set up the simulation.

There are a number of detector setups that can be chosen and modified through geant4 UI commands. See the file detectorSetup.mac for more information. In GUI mode, the application will look for the file detectorSetup.mac in the build directory to build the detector (this file is not copied into the build directory by default). If the file detectorSetup.mac is not found a default dual detector setup is built. When running in batch mode the file detectorSetup.mac is ignored, the detector setup commands should then be placed in the macro file following the -m option.

There are further UI commands to control the radioactive source, see the example macro La140.mac.

It is also possible to use the cosmic ray shower event generator (CRY) as source, to perform cosmic background estimates.

To run with CRY, the library needs to be build on your computer. Follow the instructions in the manual https://nuclear.llnl.gov/simulation/doc_cry_v1.7/cry.pdf. Make sure that this results in the file libCRY.a in the subfolder lib, where you built the cry libary. Once the CRY library has been built you need to rebuild the G4HPGeCoinc application. To help the compiler find the CRY library, do 

	cmake -DCRY_PATH:PATH='/.../cry_v1.7' ..

then

	make

where you replace '/.../cry_v1.7' with the path to the directory where you just built CRY.

To run with CRY as primary event generator

	./G4HPGecoinc -t 1 -cry 1

The cry event generator will setup by reading from the file default.cry.setup, which is copied into the build directory by cmake. To control CRY this file may be edited. There are also UI commands to modify the CRY setup, see https://nuclear.llnl.gov/simulation/doc_cry_v1.7/cry.pdf for details.

======================================================================================================================================

The code provides a few choices for detector geometries that have been implemented. By defaul the detector is setup via a macro named "detectorSetup.mac". In this macro you may see how to setup four different detector types, and change their geometrical parameters. For batch mode the detector setup commands may also be provided in the same macro that setups the source.

The source can be one of three choices

1. Radioactive ions: This is the default. The choice of the ion is made with commands

/PrimaryGeneratorAction/ionZ 55
/PrimaryGeneratorAction/ionA 134

This would correspond to Cs-134. Envoking /run/beamOn NNN will generate NNN number of decays of the chosen ion. You may also set the excitation energy of the ion, for example

/PrimaryGeneratorAction/ionZ 47
/PrimaryGeneratorAction/ionA 108
/PrimaryGeneratorAction/ionEx 109.466

would correspond to meta stable Silver-108.

The code will follow the decay chain of each ion, by default. This behaviour can be changed by envoking /PrimaryGeneratorAction/followdecaychain 0. In this case only the primary decay will be executed, generally this is not what one would want to do. You can also modify the time you want to follow the decay chain by envoking (for example)

/process/had/rdm/thresholdForVeryLongDecayTime 72 h # ignore decays at rest after 72 h

2. General particle source:

By envoking /PrimaryGeneratorAction/useGPS 1 the source can be changed to use the standard geant4 general particle source.

3. Cosmic rays:

Cosmic rays can be simulated based on the cosmic-ray-shower libraary. In order to use this feature you need to start the code with the flag "-cry 1", i.e.

./G4HPGeCoinc -cry 1

The source of cosmic rays is then controlled by editing the input file to cry "default.cry.setup", for details see the cry manual https://nuclear.llnl.gov/simulation/doc_cry_v1.7/cry.pdf.


An example macro is provided, showing how to use the code:

1. Cs134.mac:

This macro sets up the simulation of a Cs-134 source. It uses a segmented well type of detector.