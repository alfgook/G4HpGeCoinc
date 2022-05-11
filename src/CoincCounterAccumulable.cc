#include "CoincCounterAccumulable.hh"

void CoincCounterAccumulable::Merge(const G4VAccumulable& another)
{
	const CoincCounterAccumulable& theOther =
		static_cast<const CoincCounterAccumulable&>(another);

	//std::map<G4String,G4int>::const_iterator it;
	for (auto it = theOther.fCoincCounter.begin();it != theOther.fCoincCounter.end(); ++it) {
		std::pair<G4int,G4int> coincEnergies = it->first;
		G4int otherCount = it->second;
		if ( fCoincCounter.find(coincEnergies) == fCoincCounter.end()) {
			fCoincCounter[coincEnergies] = otherCount;
		}
		else {
			fCoincCounter[coincEnergies] += otherCount;
		}
	}
}

void CoincCounterAccumulable::Reset()
{
	fCoincCounter.clear();
}

void CoincCounterAccumulable::CountCoinc(std::pair<G4int,G4int> coincEnergies)
{
	//G4cout << "CoincCounterAccumulable::CountCoinc" << coincEnergies.first << " , " << coincEnergies.second << G4endl;
	fCoincCounter[coincEnergies]++;
}

void CoincCounterAccumulable::Print()
{
	G4cout << "************************************************\n";
	G4cout << "*  List of coincident decay gamma- and X-rays  *\n";
	G4cout << "************************************************\n";
	//G4cout << "E1 (keV)\tE2 (keV)\tcounts\n";
	G4cout << std::setw(8)<<"E1 (keV)"<<std::setw(10)<<"E2 (keV)"<<std::setw(10)<<"counts\n";
	for (auto it = fCoincCounter.begin();it != fCoincCounter.end(); ++it) {
		//G4cout << it->first.first << "\t" << it->first.second << "\t" << it->second << "\n";
		G4cout << std::left<< std::setw(10) << it->first.first << std::setw(11) << it->first.second << it->second << "\n";
	}
	G4cout << "************************************************\n";
	G4cout << G4endl;

}