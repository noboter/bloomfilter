#include "BloomFilter.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <time.h>
#include <chrono>
#include <math.h>

std::string randString(unsigned int len){
   std::string str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
   std::string ret;
   int pos;
   while(ret.size() != len) {
    pos = ((rand() % (str.size() - 1)));
    ret += str.substr(pos,1);
   }
   return ret;
}

int main(int argc, char *argv[]){

    srand(time(0));

    //n...number of entries -> 80Million
    int n = 80000000;
    //p...probability of false positives 0.000001
    double p = 0.000001;

    double m = ceil((n * log(p)) / log(1.0 / (pow(2.0, log(2.0)))));
    double k = round(log(2.0) * m / n);
    std::cout.precision(17);
    std::cout << "m: " << m << ", k:" << k << ", MB:" << (m/8.0/1024.0/1024.0) << std::endl;

    BloomFilter b((int)m,(int)k);

    int lasti = 0;
    double totalquery=0.0;
    double totalinsert=0.0;
    for(int i=0; i<n; i++){
        //std::string d = randString(20);

        std::stringstream ss;
        ss << std::setw(10) << std::setfill('0') << i;
        std::string d = ss.str();

        std::vector<uint8_t> mv(d.begin(), d.end());
        uint8_t* data = &mv[0];

        std::chrono::high_resolution_clock::time_point startquery = std::chrono::high_resolution_clock::now();
        bool t = b.possiblyContains(data,d.length());
        std::chrono::high_resolution_clock::time_point finishquery = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> elapsedquery = finishquery - startquery;
        totalquery+=elapsedquery.count();
        if(t){
            std::cout << "Average query time: " << (totalquery/(i+1)) << " s\n";
            std::cout<< i << "," << d << ", " << (i-lasti) <<std::endl;
            lasti=i;
        }

        if(i%10000000==0){
            std::cout << i << " Average query time: " << (totalquery/(i+1)) << " s ";
            std::cout << " Average insert time: " << (totalinsert/(i+1)) << " s\n";
        }

        std::chrono::high_resolution_clock::time_point startinsert = std::chrono::high_resolution_clock::now();
        b.add(data,d.length());
        std::chrono::high_resolution_clock::time_point finishinsert = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsedinsert = finishinsert - startinsert;
        totalinsert+=elapsedinsert.count();
    }

    std::cout << "Average query time: " << (totalquery/n) << " s\n";
    std::cout << "Average insert time: " << (totalinsert/n) << " s\n";

    //save bloom filter:
//  b.binary_write(fout);

    std::cout << "finished" << std::endl;

    return 0;
}
