//
//
//
//
//#include "Market.h"
//#include <fstream>
//#include <omp.h>
//#include <mpi.h>
//
//
//int main() {
//
//    //printout output
//    std::ofstream output_file;
//    std::map<int,map<int,vector<double>>> result;
//    int num_of_threads = 4;
//
//
//
//
//    for(auto &[simulation,output]:result){
//        std::string file_name = "Experiment_" + std::to_string(simulation);
//        file_name += ".csv";
//
//        std::cout<<simulation<<": ";
//        int size = 0;
//        for(auto &[stock,prices]: output) {
//            if (!prices.empty()) {
//                size = prices.size();
//                break;
//            }
//        }
//
//        for (auto &[stock, prices]: output) {
//            output_file.open(file_name);
//            for(int elem = 0; elem < size; elem ++) {
//                output_file << prices.at(elem) << ",";
//            }
//            output_file << std::endl;
//        }
//        output_file.close();
//        }
//        std::cout<<std::endl;
//
//    return 0;
//};
