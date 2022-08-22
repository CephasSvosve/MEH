//
// Created by Cephas Svosve on 21/12/2021.
//

#ifndef MEH_FUNDS_H
#define MEH_FUNDS_H
#include "value_investor.h"
#include "momentum_investor.h"
#include "noisy_investor.h"
#include <tuple>
#include <boost/random.hpp>
#include <map>
#include <Eigen/Dense>
#include "market_watch.h"
#include "stochastic_math.h"
#include <random>
#include "order.h"
#include <fstream>





using namespace std;

class funds
        :public fund
{
private:
    int window_size_MA1 = 0;
    int window_size_MA2 = 0;

    int window_size = 500;
    int value_window = 500;
    map<int,int> rebalancing_count;
    map<int,int> day_count;
    MatrixXd dX;


public:
    funds(int bias = 0);





    enum trading_strategy{
        aggressive_fundamental_value=4
        , aggressive_noise=5
        , aggressive_momentum_investment=6
        , aggressive_growth = 7
        , aggressive_index =8



    };




    trading_strategy fund_philosophy = {};
    double bias;
    int noise_seed;
    void trade_strategy(trading_strategy);


    map<int, order>
    invest();

    map<int, order>
    current_price_order(   int order_id
                , int asset_id
                , double quote
                , double allocation_
                ,double port_alloc
                , double wealth_
                , double available_cash
                , double current_position
                );

    map<int, order>
    stocks_order(   int order_id
            , int asset_id
            , double quote
            , double predicted_quote
            , double allocation_
            ,double port_alloc
            , double wealth_
            , double current_position
    );

    map<int, order>
    reb_wealth(
            int order_id
            , int asset_id
            , double quote
            , double allocation_
            ,double port_alloc
            , double wealth_
            , double current_position
    );

    map<int, order>
    free_cash_distribution(   int order_id
            , int asset_id
            , double quote
            , double allocation_
            , double available_cash

    );


    static int concat(int a, int b);

    static MatrixXd
    generateWhiteNoise1(int seed, int rows, int columns);


    std::map<int,order>
            ag_value_demand(),



            ag_growth_demand(),


            ag_momentum_demand(),



            ag_noise_demand(),



            ag_index_demand();








};
#endif //MEH_FUNDS_H





////
//// Created by Cephas Svosve on 21/12/2021.
////
//
//#ifndef UNTITLED30_FUNDS_H
//#define UNTITLED30_FUNDS_H
//#include "value_investor.h"
//#include "momentum_investor.h"
//#include "noisy_investor.h"
//#include <tuple>
//#include <boost/random.hpp>
//#include <map>
//#include <Eigen/Dense>
//#include "market_watch.h"
//#include "stochastic_math.h"
//#include <random>
//#include "order.h"
//
//
//
//
//
//using namespace std;
//
//class funds
//        :public fund
//     {
//private:
//    int window_size = 200;
//
//public:
//funds();
//
//enum trading_strategy{aggressive_fundamental_value=4
//                            , aggressive_noise=5
//                                , aggressive_momentum_investment=6
//                                    , aggressive_growth = 7
//                                        , aggressive_index =8
//                                            , fundamental_value =9
//                                                , noise = 10
//                                                    , momentum_investment = 11
//                                                        , growth = 12
//                                                            ,index_ = 13
//
//
//                                       };
//
//
//
//
//trading_strategy fund_philosophy;
//
//void trade_strategy(trading_strategy tradingStrategy);
//
//
//map<int, order>
//invest();
//
//
//static int concat(int a, int b);
//
//static MatrixXd
//generateWhiteNoise1(int seed, int rows, int columns);
//
//
//static double
//lateralcorrcoef(VectorXd a);
//
//
//std::map<int,order>
//ag_value_demand(),
//
//
//
//ag_growth_demand(),
//
//
//ag_momentum_demand(),
//
//
//
//ag_noise_demand(),
//
//
//
//ag_index_demand(),
//
//
//
//value_demand(),
//
//
//
//growth_demand(),
//
//
//momentum_demand(),
//
//
//
//noise_demand(),
//
//
//
//index_demand();
//
//
//
//
//};
//#endif //MEH_FUNDS_H
