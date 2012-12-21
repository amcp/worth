#include "TieredTaxer.h"

Money TieredTaxer::computeTax( Money income) const {
	Money total(income.currency(), 0);

	if(income.currency() != this->currency) {
		cerr << "Income " << income << " was not denominated in " << this->currency << endl;
		return total;
	}

	Money workingRemainder = income;

	for (int i = tiers.size() - 1; i >= 0; i--) {
		if(workingRemainder < tiers[i]) {
			//go to the next one if the current tier is greater than the working remainder
			continue;
		}

		const Money tier = tiers.at(i);
		Money delta = workingRemainder - tier;
		Rate rate = rates[i];
		total = total + delta * rate;
		workingRemainder = tiers[i];
	}

	return total;
}
