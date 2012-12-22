#include "Account.h"

unsigned int Account::idSequence = 0;

void Account::incrementIdSequence() {
  idSequence++;
}
