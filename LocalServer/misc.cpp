
#include "misc.h"

int generateRandomNumber(int lower, int upper) {
    std::random_device rd; 
    std::mt19937 gen(rd());  
    std::uniform_int_distribution<> dis(lower, upper); 

    return dis(gen); 
}
