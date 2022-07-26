//
// Created by Cephas Svosve on 5/1/2022.
//

#ifndef MEH_PRICE_SETTER_H
#define MEH_PRICE_SETTER_H
#include <map>
#include <math.h>
#include "funds.h"
#include "price.h"
#include "market_watch.h"
#include "order.h"
#include "company.h"
#include "wiener.h"
#include <algorithm>
#include <queue>


using namespace std;


class price_setter
        :public funds {

private:
    map<int,company> assets;





    map<int,queue<order>> market_orders = {};//queue so that orders are executed using FIFO
    map<int,double> best_bid;
    map<int,double> best_ask;
    map<int,double> mid_price;

    map<int,double> inventory_t_1;
    map<int,vector<double>> study;

    //thread synchronizers
    mutex g_mutex;
    condition_variable g_cv;
    bool g_ready = false;


public:

    map<int,map<int,order>> ask = {};
    map<int,map<int,order>> bid = {};
    map<int, funds*> trading_institutions;
    int day_count = 0;
    map<int,company>
    &tradeable_assets();

    double order_size = 10000.;
    int balance_count = 0;

    void register_assets(int number_of_assets);
    void generate_fundamentals();
    void register_traders(vector<funds*> participants);
    void update_balances(int t);
    void set_initial_quotes();
    void receive_orders(int trader);
    void receiving_orders(market_watch &market_clock);
    void queue_market_orders(int id, order order_);
    void queue_bid(int id, order order_);
    void queue_ask(int id, order order_);
    void clear(market_watch &watch);


    tuple<order,order>
    update_mm_orders(const map<double, vector<order>, greater<>> &bid_orders_
            ,const map<double, vector<order>> &ask_orders_
            ,double &init_inventory
            ,double &curr_inventory
            ,double init_price
            ,int identifier
            ,int asset_id
            ,double quote) const;
};


#endif //UNTITLED32_PRICE_SETTER_H

////
//// Created by Cephas Svosve on 5/1/2022.
////
//
//#ifndef UNTITLED32_PRICE_SETTER_H
//#define UNTITLED32_PRICE_SETTER_H
//#include <map>
//#include <math.h>
//#include "funds.h"
//#include "price.h"
//#include "market_watch.h"
//#include "order.h"
//#include "company.h"
//#include "wiener.h"
//#include <algorithm>
//
//using namespace std;
//
//
//class price_setter
//        :public funds {
//
//private:
//    map<int,company> assets;
//
//
//
//
//    map<int,vector<order>> ask;
//    map<int,vector<order>> market_orders;
//    map<int,double> best_bid;
//    map<int,double> best_ask;
//    map<int,double> mid_price;
//    map<int,vector<order>> bid;
//
//
//public:
//    map<int, funds*> trading_institutions;
//    map<int,company>
//            &tradeable_assets();
//
//    void register_assets(int number_of_assets);
//    void generate_fundamentals(double interest_rate);
//    void register_traders(vector<funds*> participants);
//    void update_balances(int t);
//    void set_initial_quotes();
//    void receive_orders();
//    void stack_market_orders(int id, order order_);
//    void stack_bid(int id, order order_);
//    void stack_ask(int id, order order_);
//    void clear();
//    void update_price(int k, double reg);
//
//
//    bool not_similar(vector<order> &x);
//    bool isValid(int asset_id);
//
//};
//
//
//#endif //UNTITLED32_PRICE_SETTER_H
