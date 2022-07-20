//
// Created by Cephas Svosve on 10/6/2022.
//

#include "Market.h"
#include "price_setter.h"
#include "market_watch.h"
#include <fstream>
#include <stdlib.h>


int main() {

    int noise_share;
    std::cout << " Share of passive investment(in units of 0.5%): ";
    std::cin >> noise_share;


    double market_wealth = 1.68e10;
    double shares_outstanding = 0.5 * market_wealth / 9;
    double market_cash = 0.5 * market_wealth;
    double noise_wealth = noise_share * 0.01;


    market_watch watch(market_watch::one_second);


//Define the price setter
    price_setter market_;
    market_.set_clock(&watch);



//create assets and register stocks to the market
    market_.register_assets(3);
    market_.generate_fundamentals();


//generate initial quotes
    market_.set_initial_quotes();

//set price setter's wealth
    market_.initial_wealth(0 * market_cash, 1 * shares_outstanding);

//initial wealth given to investment strategies
//experiment fund is the fund whose wealth we are varying in the experiments
    double experiment_fund_wealth = 0.;
    double other_funds_wealth = 1. - experiment_fund_wealth;


    double experiment_funds_cash = 0.5 * market_cash * experiment_fund_wealth;
    double experiment_funds_shares = 0.5 * shares_outstanding * experiment_fund_wealth;


    double funds_cash = 1 * market_cash * other_funds_wealth;
    double funds_shares = 0* shares_outstanding * other_funds_wealth;

    //define species populations

    int value_population = 3;
    int trend_population = 1;
    int noise_population = 1;
    int index_population = 1;
    int strategies = 4;



//initialize traders
    funds
            aggressive_value1
//            ,aggressive_value2
//            ,aggressive_value3
            ,aggressive_noisy
//            ,aggressive_noisy2(7)
//            ,aggressive_noisy3(8)
//            , aggressive_momentum
            , aggressive_index
;

    vector<funds *> participants = {
            &aggressive_value1
//            ,&aggressive_value2
//            ,&aggressive_value3
            ,&aggressive_noisy
//            ,&aggressive_noisy2
//            ,&aggressive_noisy3
//            , &aggressive_momentum
            , &aggressive_index

    };




//inform traders about assets on the market
    for (auto &p : participants) {
        p->set_clock(&watch);
        p->get_inform(market_.tradeable_assets());
        std::cout<<"identifiers "<<p->get_identifier()<<std::endl;
    }


    //define different value investors



{

    aggressive_value1.trade_strategy(funds::aggressive_fundamental_value, 0.00);
    aggressive_value1.set_reb_period(63);
    aggressive_value1.initial_wealth(funds_cash *0.2* (1. / (1))  ,
                                     funds_shares * 0*(1. / (1)));

//    aggressive_value2.trade_strategy(funds::aggressive_fundamental_value,0.03);
//    aggressive_value2.set_reb_period(63);
//    aggressive_value2.initial_wealth(funds_cash * 1* (1. / ( (strategies))),
//                                     funds_shares *1*  (1. / ( (strategies))));
////
//    aggressive_value3.trade_strategy(funds::aggressive_fundamental_value, 0.04);
//    aggressive_value3.set_reb_period(63);
//    aggressive_value3.initial_wealth(funds_cash * 1* (1. / ( (strategies))),
//                                     funds_shares * 1* (1. / ( (strategies))));

}
//    aggressive_growth.trade_strategy(funds::aggressive_growth);
//    aggressive_growth.set_reb_period(63);
//    aggressive_growth.initial_wealth((1./(investor_population))*0.5*market_cash*(1-passive_investment)
//            ,(1./(investor_population))*0.5*shares_outstanding*(1-passive_investment));
    {
        double wealth_share = (1./3);
        aggressive_noisy.trade_strategy(funds::aggressive_noise);
        aggressive_noisy.set_reb_period(1);
        aggressive_noisy.initial_wealth(funds_cash * 0.7* (1. / (1)),
                                        funds_shares * 0. * (1. / (1)));

//        aggressive_noisy2.trade_strategy(funds::aggressive_noise);
//        aggressive_noisy2.set_reb_period(1);
//        aggressive_noisy2.initial_wealth(funds_cash * wealth_share* (1. / (1)),
//                                        funds_shares * 0. * (1. / (1)));

//        aggressive_noisy3.trade_strategy(funds::aggressive_noise);
//        aggressive_noisy3.set_reb_period(1);
//        aggressive_noisy3.initial_wealth(funds_cash * wealth_share * (1. / (1)),
//                                        funds_shares * 0. * (1. / (1)));

    }

////
//////
//    aggressive_momentum.trade_strategy(funds::aggressive_momentum_investment);
//    aggressive_momentum.set_reb_period(21);
//    aggressive_momentum.initial_wealth(funds_cash *1/3* (1. / (1))  ,
//                                       funds_shares * 0.0*(1. / (1)));
//////
    aggressive_index.trade_strategy(funds::aggressive_index);
    aggressive_index.set_reb_period(63);
    aggressive_index.initial_wealth(funds_cash *0.1* (1. / (1))  ,
                                    funds_shares * 0.0*(1. / (1)));
//////


//register traders on the market
    market_.register_traders(participants);


//market simulation run point
    std::ofstream myfile;
    std::string file_name = "Experiment_equal_wealth" + std::to_string(noise_share);//"Experiment_(long_run_included)_" + std::to_string(passive_share);
    file_name += ".csv";
    myfile.open (file_name);

    watch.start();
    int day_count = 0;
    double total_spending = 0;
    while(day_count < 1000) {

        //adjust traders' balances for interest and dividends


        if (day_count < int((&watch)->current_time())) {
            for (auto &p : participants) {
                p->balance_cf(int((&watch)->current_time()));
            }
        }

//spending
//what is the next dividend


//how much wealth does each investment strategy have
//spend
//        for(auto &[x,y]: market_.trading_institutions){
//            std::cout<<"unaltered_weath "<<(y->wealth)/10e09<<std::endl;
//        }


        for(auto &[x,y]: market_.trading_institutions){
            (y)->wealth -= (y->wealth/market_wealth)*total_spending;
        }
        (&market_)->wealth -= (market_.wealth/market_wealth)*total_spending;
        total_spending = 0;


//        for(auto &[x,y]: market_.trading_institutions){
//            std::cout<<"new_weath "<<(y->wealth)/10e09<<std::endl;
//        }

//receive orders from traders

        market_.receive_orders();

//clear market

//        for (int i = 0; i < 50400; ++i) {
//
//
//            for (auto &[k, v] : market_.tradeable_assets()) {
////                if (result.find(k)!= result.end()){
////                    result.find(k)->second.push_back(v.get_midprice());
////                }else{
////                    result.emplace(k,(v.get_midprice()));
////                }
//
//
//                myfile << k << "," << (v.get_value(i)) << ","<<(v.get_dividend(i))<<",";
//            }
//
//            myfile << std::endl;
//
//        }
//        goto exit;
        if (day_count < int((&watch)->current_time())) {
            for (auto &[k, v] : market_.tradeable_assets()) {
//                if (result.find(k)!= result.end()){
//                    result.find(k)->second.push_back(v.get_midprice());
//                }else{
//                    result.emplace(k,(v.get_midprice()));
//                }


                myfile << k << "," << (v.get_midprice()) << ",";
                myfile << v.get_value((&watch)->current_time()) << ",";




}
            for(auto &[x,y]:market_.trading_institutions){

                myfile << y->fund_philosophy << "," << y->wealth << ",";
                for (auto &[a, b] : market_.tradeable_assets()) {
                    double investment = abs(y->stocks_at_hand.find(a)->second)*(b.get_midprice());
                    myfile << a <<","<<" stocks "","<<
                    abs(y->stocks_at_hand.find(a)->second)<<","<<" cash "<<","<<y->cash_at_hand<<","<<investment<<",";
                }
            }
            myfile << market_.get_identifier() <<","<<market_.wealth<<",";
            std::cout << std::endl;
            myfile << std::endl;

            //spendig
            for (auto &[a, b] : market_.tradeable_assets()) {
                total_spending += b.get_dividend(day_count+1)*shares_outstanding;
            }
            day_count++;
        }
        market_.clear(watch);
        std::cout << "market's watch time " << (&watch)->current_time() << std::endl;
    }

//exit:;
    myfile.close();
    return 0;
}
