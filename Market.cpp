//
// Created by Cephas Svosve on 10/6/2022.
//

#include "Market.h"
#include "price_setter.h"
#include "market_watch.h"
#include <fstream>
#include <thread>


int main() {


    double market_wealth = 1.68e10;
    double shares_outstanding = 0.5 * market_wealth / 75;
    double market_cash = 0.5 * market_wealth;



    market_watch watch(market_watch::one_second);
    watch.set_terminal_time(1000);

//Define the price setter
    price_setter market_;
    market_.set_clock(&watch);



//create assets and register stocks to the market
    market_.register_assets(3);
    market_.generate_fundamentals();


//generate initial quotes
    market_.set_initial_quotes();

//set price setter's wealth
    market_.initial_wealth(0. * market_cash, 1 * shares_outstanding);

//initial wealth given to investment strategies
//experiment fund is the fund whose wealth we are varying in the experiments
    double experiment_fund_wealth = 0.;
    double other_funds_wealth = 1. - experiment_fund_wealth;


    double experiment_funds_cash = 0.5 * market_cash * experiment_fund_wealth;
    double experiment_funds_shares = 0.5 * shares_outstanding * experiment_fund_wealth;

//define species populations
    int num_traders = 1000;
    double funds_cash = 1 * market_cash * other_funds_wealth / num_traders;
    double funds_shares = 0. * shares_outstanding * other_funds_wealth / num_traders;





//initialize traders
    vector<funds> value = {};
    vector<funds> trend = {};
    vector<funds> noise = {};
    vector<funds> index = {};


//allocate traders to each strategy
    for (int i = 0; i < num_traders; i++) {
        funds w(i);
        funds x(i);
        funds y(i);
        funds z(i);

        value.push_back(w);
        trend.push_back(x);
        noise.push_back(y);
        index.push_back(z);
    }


    {

//    funds
//            aggressive_value1
////            ,aggressive_value2
////            ,aggressive_value3
//            ,aggressive_noisy
////            ,aggressive_noisy2(7)
////            ,aggressive_noisy3(8)
////            , aggressive_momentum
//            , aggressive_index
//;

//    vector<funds *> participants = {
//            &aggressive_value1
////            ,&aggressive_value2
////            ,&aggressive_value3
//            ,&aggressive_noisy
////            ,&aggressive_noisy2
////            ,&aggressive_noisy3
////            , &aggressive_momentum
//            , &aggressive_index
//
//    };
}
//store funds by addresses
    vector<funds *> value_participants = {};
    value_participants.reserve(value.size());
    for (auto &x : value) {
        value_participants.push_back(&x);
    }

    vector<funds *> trend_participants = {};
    trend_participants.reserve(trend.size());
    for (auto &x : trend) {
        trend_participants.push_back(&x);
    }

    vector<funds *> noise_participants = {};
    noise_participants.reserve(noise.size());
    for (auto &x : noise) {
        noise_participants.push_back(&x);
    }

    vector<funds *> index_participants = {};
    index_participants.reserve(index.size());
    for (auto &x : index) {
        index_participants.push_back(&x);
    }



//investment strategy wealths
    double value_share = 1;
    double noise_share = 0;
    double trend_share = 0;
    double index_share = 0;


//inform traders about assets on the market
    for (auto &p : value_participants) {
        p->set_clock(&watch);
        p->get_inform(market_.tradeable_assets());

        p->trade_strategy(funds::aggressive_fundamental_value);
        p->set_reb_period(1);
        p->initial_wealth(funds_cash * value_share, funds_shares * value_share);


    }

    for (auto &p : trend_participants) {
        p->set_clock(&watch);
        p->get_inform(market_.tradeable_assets());

        p->trade_strategy(funds::aggressive_momentum_investment);
        p->set_reb_period(21);
        p->initial_wealth(0 * trend_share, funds_shares * trend_share);
    }

    for (auto &p : noise_participants) {
        p->set_clock(&watch);
        p->get_inform(market_.tradeable_assets());

        p->trade_strategy(funds::aggressive_noise);
        p->set_reb_period(1);
        p->initial_wealth(0 * noise_share, funds_shares * noise_share);

    }

    for (auto &p : index_participants) {
        p->set_clock(&watch);
        p->get_inform(market_.tradeable_assets());

        p->trade_strategy(funds::aggressive_index);
        p->set_reb_period(63);
        p->initial_wealth(0 * index_share, 0);

    }


    //define different value investors




    {
//        double wealth_share = (1./3);
//        aggressive_noisy.trade_strategy(funds::aggressive_noise);
//        aggressive_noisy.set_reb_period(1);
//        aggressive_noisy.initial_wealth(funds_cash * 0.7* (1. / (1)),
//                                        funds_shares * 0. * (1. / (1)));

//        aggressive_noisy2.trade_strategy(funds::aggressive_noise);
//        aggressive_noisy2.set_reb_period(1);
//        aggressive_noisy2.initial_wealth(funds_cash * wealth_share* (1. / (1)),
//                                        funds_shares * 0. * (1. / (1)));

//        aggressive_noisy3.trade_strategy(funds::aggressive_noise);
//        aggressive_noisy3.set_reb_period(1);
//        aggressive_noisy3.initial_wealth(funds_cash * wealth_share * (1. / (1)),
//                                        funds_shares * 0. * (1. / (1)));

    }
    {
////
//////
//    aggressive_momentum.trade_strategy(funds::aggressive_momentum_investment);
//    aggressive_momentum.set_reb_period(21);
//    aggressive_momentum.initial_wealth(funds_cash *1/3* (1. / (1))  ,
//                                       funds_shares * 0.0*(1. / (1)));
//////
//    aggressive_index.trade_strategy(funds::aggressive_index);
//    aggressive_index.set_reb_period(63);
//    aggressive_index.initial_wealth(funds_cash *0.1* (1. / (1))  ,
//                                    funds_shares * 0.0*(1. / (1)));
//////
}

//register traders on the market
    market_.register_traders(value_participants);
    market_.register_traders(trend_participants);
    market_.register_traders(noise_participants);
    market_.register_traders(index_participants);
//
//    {
////market simulation run point
////    std::ofstream myfile;
////    std::string file_name = "Experiment_equal_wealth" + std::to_string(name);//"Experiment_(long_run_included)_" + std::to_string(passive_share);
////    file_name += ".csv";
////    myfile.open (file_name);
//}

    watch.start();

    market_.receive_orders();
    thread receiving_orders(&price_setter::receiving_orders, std::ref(market_), std::ref(watch));
    thread clearing(&price_setter::clear, std::ref(market_), std::ref(watch));
    {
        //    while((&watch)->current_time()<1001) {

        //adjust traders' balances for interest and dividends

        ;
//int((&watch)->current_time())
//        if (day_count < int(timer)) {
//            for (auto &p : value_participants) {
//                p->balance_cf(int((&watch)->current_time()));
//            }
//            for (auto &p : trend_participants) {
//                p->balance_cf(int((&watch)->current_time()));
//            }
//            for (auto &p : noise_participants) {
//                p->balance_cf(int((&watch)->current_time()));
//            }
//            for (auto &p : index_participants) {
//                p->balance_cf(int((&watch)->current_time()));
//            }
//        }

//spending
//what is the next dividend


//how much wealth does each investment strategy have
//spend
//        for(auto &[x,y]: market_.trading_institutions){
//            std::cout<<"unaltered_weath "<<(y->wealth)/10e09<<std::endl;
//        }


//        for(auto &[x,y]: market_.trading_institutions){
//            (y)->wealth -= (y->wealth/market_wealth)*total_spending;
//        }
//        (&market_)->wealth -= (market_.wealth/market_wealth)*total_spending;
//        total_spending = 0;


//        for(auto &[x,y]: market_.trading_institutions){
//            std::cout<<"new_weath "<<(y->wealth)/10e09<<std::endl;
//        }

//receive orders from traders

//        market_.receive_orders();

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
//        if (day_count < int(timer)) {
//            for (auto &[k, v] : market_.tradeable_assets()) {
//
//                myfile << k << "," << (v.get_midprice()) << ",";
//                myfile << v.get_value(int(timer)) << ",";




//}
//            for(auto &[x,y]:market_.trading_institutions){
//
//                myfile << y->fund_philosophy << "," << y->wealth << ",";
//                for (auto &[a, b] : market_.tradeable_assets()) {
//                    double investment = abs(y->stocks_at_hand.find(a)->second)*(b.get_midprice());
//                    myfile << a <<","<<" stocks "","<<
//                    abs(y->stocks_at_hand.find(a)->second)<<","<<" cash "<<","<<y->cash_at_hand<<","<<investment<<",";
//                }
//            }
//            myfile << market_.get_identifier() <<","<<market_.wealth<<",";
//            std::cout << std::endl;
//            myfile << std::endl;

        //spendig
//            for (auto &[a, b] : market_.tradeable_assets()) {
//                total_spending += b.get_dividend(day_count+1)*shares_outstanding;
//            }

//            day_count++;
//        }

//        market_.clear(watch);
//        timer += 0.2;

//        std::cout << "market's watch time " << (&watch)->current_time() << std::endl;
//    }
    }


    receiving_orders.join();
    clearing.join();
    {
//exit:;
//    myfile.close();
}

return 0;
}
