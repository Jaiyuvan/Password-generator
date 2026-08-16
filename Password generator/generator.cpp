#include<iostream>
#include<random>
#include<chrono>
#include<string>
#include<cmath>
#include"generator.h"
#include"Xoshirocpp.h"
#include<vector>
#include<tuple>
#include<fstream>


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

int wordIndex() {
    std::chrono::high_resolution_clock clock;
    std::uint64_t seed = { static_cast<unsigned int>(clock.now().time_since_epoch().count()) ^ rd() };
    XoshiroCpp::Xoshiro128Plus xnum(seed);
    std::uniform_int_distribution<int> index(0,99);
    
    return index(xnum);
}

std::string passByPhrase(int len) {
    std::chrono::high_resolution_clock TargetClock;
    std::uint32_t seed = { static_cast<unsigned int>(TargetClock.now().time_since_epoch().count()) };
    XoshiroCpp::Xoroshiro128Plus tnum(seed);
    std::uniform_int_distribution<int> range(1, 7700);

    std::size_t target = range(tnum);
    std::string passPhrase;
    std::vector<std::string> words;
    std::ifstream file;
    file.open("wordlist-eff-large.txt");
    if (!file.is_open()) {
        std::cout << "error while loading the file";

    }
    std::string line;
    for (std::size_t i=1;i <= target;i++) {
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    for (std::size_t j = 0;j < 100;j++) {
        std::getline(file, line);
        words.push_back(line);
    }

    for (int k = 0;k < len;k++) {
        int index = wordIndex();
        passPhrase =passPhrase+ words[index]+"-";
    }
    return passPhrase;
    

}