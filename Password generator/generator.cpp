#include<iostream>
#include<random>
#include<chrono>
#include<string>
#include<cmath>
#include"generator.h"
#include"Xoshirocpp.h"
#include<vector>
#include<tuple>



std::random_device rd;

void pass_maker(int a, int b, std::string& str) {
    std::chrono::high_resolution_clock clock;
    std::uint64_t seed = { static_cast<unsigned int>(clock.now().time_since_epoch().count()) ^ rd() };
    std::uniform_int_distribution<int> number(a, b);
    XoshiroCpp::Xoshiro256PlusPlus num_generator(seed);
    str = str + static_cast<char>(number(num_generator));

}


double entropy_calculator(int w, int x, int y, int z,int lenstr) {

    int r = 0;
    r = (w == 1) ? r + 10 : r;
    r = (x == 1) ? r + 26 : r;
    r = (y == 1) ? r + 26 : r;
    r = (z == 1) ? r + 14 : r;
    double entropy = lenstr * std::log2(r);
    return entropy;

}



std::tuple<std::string,double> passwordGenerator(int symbol,int number,int lenchar) { 

    std::string pasw;
    int sc = 0;
    int uc = 0;
    int dc = 0;
    int syc = 0;
    std::vector <int> allowedArray = { 2,3 };
    if (symbol==1) {
        allowedArray.push_back(4);
    }
    if(number == 1) {
        allowedArray.push_back(1);
    }

    for (int i = 0;i < lenchar;i++) {

        XoshiroCpp::Xoshiro256PlusPlus character_decider(rd());
        
        std::uniform_int_distribution<size_t>decider(0, allowedArray.size()-1);
        if (allowedArray[decider(character_decider)] == 1) {
            pass_maker(48, 57, pasw);
            dc = 1;
        }
        if (allowedArray[decider(character_decider)] == 2) {
            pass_maker(65, 90, pasw);
            uc = 1;
        }
        if (allowedArray[decider(character_decider)] == 3) {
            pass_maker(97, 122, pasw);
            sc = 1;
        }
        if (allowedArray[decider(character_decider)] == 4) {
            pass_maker(33, 47, pasw);
            syc = 1;
        }

    }
    double entropy = entropy_calculator(dc, uc, sc, syc,lenchar);
    std::cout << entropy << std::endl;
    std::cout << pasw << std::endl;


    return { pasw,entropy };
}