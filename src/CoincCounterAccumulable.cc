#include "CoincCounterAccumulable.hh"
#include <fstream> //std::ofstream
#include <iomanip>

void CoincCounterAccumulable::Merge(const G4VAccumulable& another)
{
	G4cout << "CoincCounterAccumulable::Merge" << G4endl;
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
	fCoincCounter[coincEnergies]++;
}

void CoincCounterAccumulable::Print()
{
	if(!fCoincCounter.size()) return;
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

void CoincCounterAccumulable::Print2file(G4String filename)
{
	if(!fCoincCounter.size()) return;

	std::ofstream fout(filename.c_str(), std::ofstream::app);
	if(!fout.is_open()) {
		G4cout << "************************************************\n";
		G4cout << " WARNING!! could not open file: " << filename << "  \n";
		G4cout << " output list of coincidences not written        \n";
		G4cout << "************************************************\n";
		return;
	}

	fout << "************************************************\n";
	fout << std::setw(8)<<"E1 (keV)"<<std::setw(10)<<"E2 (keV)"<<std::setw(10)<<"counts\n";
	for (auto it = fCoincCounter.begin();it != fCoincCounter.end(); ++it) {
		fout << std::left<< std::setw(10) << it->first.first << std::setw(11) << it->first.second << it->second << "\n";
	}
	fout << "************************************************\n\n";
	fout.close();
}