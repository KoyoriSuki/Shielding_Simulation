This program is used for gamma background evaluation under different height of lead/copper shielding

User defined UI commands:

/usrDefinedGeo/setCrystalType VALUE
	0: CMO 
	1: 2x2x2cm LMO 
	2: 1x1x1cm LMO
/usrDefinedGeo/setShieldingFlag VALUE
	0: without shielding
	1: shielding
/usrDefinedGeo/setShieldingHeight VALUE
	unit: cm
note: use /run/reinitializeGeometry after /usrDefinedGeo/ to correctly set the geometry

/mydet/setEnergyDistributionFlag VALUE
	not in use
/mydet/setUseCodeOrMacFile VALUE
	0: code in PrimaryGeneratorAction::GeneratePrimaries will be executed
	1: code in PrimaryGeneratorAction::GeneratePrimaries will be ignored

Provided macro files in macFiles:
	init_vis.mac: for visualization. If run ./DBDecay without any parameters
	gammaBkg.mac: not in use
	shieldingX.mac: shielding with X cm in height
	na22.mac: counting rate simulation with Na-22 ion
	job.cmd: for condor submission
	mergeResults.cc: merge the result root files

To get the result:
	mkdir build -> cd build -> cmake .. -> make

	if run with job.cmd, the results will be:
		shieldingHeightResultX.root, X is the thread number
		copy the mergeResults.cc to ./build/resultFiles, then run "root mergeResults.cc", 
		it will give you the merged results

	or directly run "./DBDecay (xxx.mac) (X)"

To read the result:
	Examples in readResults.cc