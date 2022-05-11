//
//  TrackingAction.h
//  
//
//  Created by Alf Göök on 2022-05-11.
//
// Accumulating counts for coincident gamma-rays
//  entries are stored in a std::map, the energies are rounded to integers
//  provide them in keV (or eV if higher precision is needed)

#ifndef CoincCounterAccumulable_h
#define CoincCounterAccumulable_h 1

#include "G4VAccumulable.hh"
#include "globals.hh"
#include <map>

class CoincCounterAccumulable : public G4VAccumulable
{

public:
	CoincCounterAccumulable(const G4String& name)
		: G4VAccumulable(name) {}
	virtual ~CoincCounterAccumulable() {}

	void CountCoinc(std::pair<G4int,G4int> coincEnergies);
	virtual void Merge(const G4VAccumulable& other);
	virtual void Reset();
	void Print();

private:
	std::map<std::pair<G4int,G4int>,G4int> fCoincCounter;

};

#endif