#include "../classes/Authenticate.hpp"

Authenticate::Authenticate() : online(false){
    std::cout << "Authetication Constructor" << std::endl;
}

Authenticate::~Authenticate(){
    std::cout << "Authetication Destructor" << std::endl;
}