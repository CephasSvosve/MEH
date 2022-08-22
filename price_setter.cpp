




//
// Created by Cephas Svosve on 5/1/2022.
//

#include "price_setter.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <random>
#include <omp.h>


template <typename T>
int sgn(T val) {
    return (0 < val) - (val < 0);
}


//method for accessing assets registered on the market
map<int,company>&
price_setter::tradeable_assets(){
    return this->assets;
}


//this lets us create stocks that will be traded on the market
void price_setter::register_assets(int number_of_assets){
    for(int i = 0; i < number_of_assets; i++){
        if(this->assets.size() < number_of_assets){
        company a;
        a.set_shares_outstanding(1.68e10*0.5/9);
        this->assets.emplace(a.get_identifier(),a);
        }
    }
}


//this generates fundamental processes and associates them with each asset
void price_setter::generate_fundamentals(){
    double day_cost_of_equity = pow((1.1),(1./252))-1;

//        auto earnings_processes  = generate(Earnings);
            auto dividends_processes = generate(Dividends);
//                auto free_cash_flow      = generate(Free_Cash_Flows);
                    int count = 0;

                //associate fundamental processes with assets

        for(auto &[i,j] : this->assets){
//            j.set_earnings_process(earnings_processes.at(count));
//                j.set_free_cash_flow_process(free_cash_flow.at(count));
                    j.set_dividends_process(dividends_processes.at(count));

                        //preset intrinsic value of the asset
                        //(1)....compute present value of each dividend
                        int size = j.get_dividend_process().size();


                        vector<double> iv_aggregate={};


                        iv_aggregate.reserve(size);

                        //growth rate g
                        double g =0;
                        for(int d=1; d < size; d++ ){

                         g += ((j.get_dividend_process().at(d))- (j.get_dividend_process().at(d-1)))/(j.get_dividend_process().at(d-1));

                        }


                         g = g/size;

std::cout<<"g value: "<<g<<" cost of cap: "<<day_cost_of_equity<<std::endl;
                            for(int d = 0; d < size; d++){
                                double iv = 0;
                                iv = j.get_dividend(d) /(day_cost_of_equity-g);
                                iv_aggregate.push_back(iv);

                            }








                        j.set_intrinsic_value(iv_aggregate);


        count ++;
    }



}


//this notifies the price setter of the available traders
void price_setter::register_traders(vector<funds*> i) {
    for (auto &p : i) {
        this->trading_institutions.emplace(p->get_identifier(), p);
    }
}


//this updates traders' balances before sending new quotes
void price_setter::update_balances(int t)
{
    for(auto &[k,v]: this->trading_institutions){
        v->balance_cf(t);
    }
}


void price_setter::set_initial_quotes(){
    for(auto &[k,v] : this->assets){
        double init_price = 25.;
//        std::cout<<" initial price for asset "<< k <<" is: USD";
//        std::cin >> init_price;
//      v.set_price(0,init_price,init_price);
        v.set_price(0,25.,25.);

//        this->bid.emplace(k,vector<order>());
//        this->ask.emplace(k,vector<order>());
//        this->market_orders.emplace(k,vector<order>());

        this->best_bid.emplace(k,init_price);
        this->best_ask.emplace(k,init_price);
        this->mid_price.emplace(k,init_price);
    }
    this->get_inform(this->assets);
}

void price_setter::receive_orders(int trader){
    this->bid.clear();
    this->ask.clear();
    this->market_orders.clear();
//#pragma omp parallel for
        for (auto &[k, v] : this->trading_institutions) {
            int abh = k;
            if ((v->wealth) > 0) {
                auto order_ = v->invest();
                for (auto &[i, j] : order_) {

                    if (abs(j.get_order_size()) > 0 && (j.get_status()!=0)) {
                        if (j.get_order_type()) {//this checks whether the order is a limit(1) or market(0) order
                            if (j.get_order_size() > 0) {//this checks whether order is a bid(+) or ask(-)
                                queue_bid(j.get_ordered_asset(), j);
                            } else {
                                queue_ask(j.get_ordered_asset(), j);
                            }
                        } else
                            if(j.get_id() == trader){//if order is a market order we stack the market order vector
                            queue_market_orders(j.get_ordered_asset(), j);
                        }
                    }
                }
            } else {
                for (auto &[i, j]: v->stocks_at_hand) {
                    v->stocks_at_hand.find(i)->second = 0;
                }
                v->bond_at_hand = 0;
                v->cash_at_hand = 0;
            }
        }

}


//void price_setter::receiving_orders(market_watch &market_clock){
//while(market_clock.current_time() < market_clock.get_terminal_time()) {
//            unique_lock<std::mutex> ul(g_mutex);
//            this->receive_orders();
//            g_ready = true;
//            ul.unlock();
//            g_cv.notify_one();
//            ul.lock();
//            g_cv.wait(ul,[this](){return (!g_ready);});
//    }
//}


void price_setter::queue_market_orders(int id, order order_){
    int k = id;
            if(this->market_orders.find(k) != this->market_orders.end()){
                this->market_orders.find(k)->second.push(order_);
            }else{
            queue<order> a;
        a.push(order_);
    this->market_orders.emplace(k,a);
    }
}


void price_setter::queue_bid(int id, order order_){
    auto k = id;
if(this->bid.find(k) != this->bid.end()){
    map<int,order> dummy_map = {};
        dummy_map.insert({order_.get_id(),order_});
            dummy_map.merge(this->bid.find(k)->second);//cancel orders if a new limit order has been passed by the same investor
                this->bid.find(k)->second.clear();
                    this->bid.find(k)->second.merge(dummy_map);
            }else{
        map<int,order> dummy_map = {};
    dummy_map.insert({order_.get_id(),order_});
this->bid.emplace(k,dummy_map);
    }
}


void price_setter::queue_ask(int id, order order_){
    auto k = id;
    if(this->ask.find(k) != this->ask.end()){
        map<int,order> dummy_map = {};
        dummy_map.insert({order_.get_id(),order_});
        dummy_map.merge(this->ask.find(k)->second);//cancel orders if a new limit order has been passed by the same investor
      this->ask.find(k)->second.clear();
        this->ask.find(k)->second.merge(dummy_map);
    }else{
        map<int,order> dummy_map = {};
        dummy_map.insert({order_.get_id(),order_});
        this->ask.emplace(k,dummy_map);
    }
}


//this method sorts compares orders by order prices in ascending order
bool ascending_order(order x, order y){
    return ((x.get_proposed_price()) < (y.get_proposed_price()));
}


bool descending_order(order x, order y){
    return ((x.get_proposed_price()) > (y.get_proposed_price()));
}


//this method sorts orders by order sizes in descending order
bool sort_by_size(order x, order y){
    return ((x.get_order_size()) > (y.get_order_size()));
}


//this method sorts orders by order id in decreasing order in descending order
bool sort_by_id(order x, order y){
    return ((x.get_id()) > (y.get_id()));
}


tuple<order,order>
match_market_orders(order &market_order, order &bid_order,order &ask_order){
tuple<order,order> result;

//solve for ask market orders
    if (market_order.get_order_size() < 0.){



                double bid_price = (floor(bid_order.get_proposed_price()*100))/100;


                    //do the standard market order to bid order
                    bool bid_is_active = bid_order.get_status();
                    bool mktask_is_active = market_order.get_status();

                    if (bid_is_active && mktask_is_active) {
                        //trade

                        double mkt_cash = market_order.get_order_size() * market_order.get_proposed_price();
                        double BID = bid_order.get_order_size();
                        double ASK = mkt_cash / bid_price;
                        std::cout<<market_order.get_id()<< " "<<bid_order.get_id()<<" price "<<bid_price <<" bid "<<BID<< " ask "<<ASK<<std::endl;
                        auto uncleared = BID + ASK;
                        std::cout<<"passed 1 "<<std::endl;
//solve for the 3 situations: uncleared <, > or = 0;
                        if (uncleared < 0.) {
                            //calculate executed bid
                            order exec_bid;
                            auto exec_bid_price = bid_price;
                            auto bid_trader_id = bid_order.get_id();

                            exec_bid.set_id(bid_trader_id);
                            exec_bid.set_status(order::active);
                            exec_bid.set_order_type(order::limit);
                            exec_bid.set_ordered_asset(bid_order.get_ordered_asset());
                            exec_bid.set_order_size(BID, exec_bid_price);

                            get<0>(result) = exec_bid;


                            //calculate executed ask
                            order exec_ask;
                            auto exec_ask_price = bid_price;
                            auto ask_trader_id = market_order.get_id();

                            exec_ask.set_id(ask_trader_id);
                            exec_ask.set_status(order::active);
                            exec_ask.set_order_type(order::market);
                            exec_ask.set_ordered_asset(market_order.get_ordered_asset());
                            exec_ask.set_order_size(-BID, exec_ask_price);

                            get<1>(result) = exec_ask;



                            //update the ask orders remaining in the order vectors after execution of trade
                            (&market_order)->set_order_size((uncleared * bid_price) / market_order.get_proposed_price(),
                                                        market_order.get_proposed_price());
                            //remove the fully filled bid order
                            (&bid_order)->set_status(order::filled);

                        } else if (uncleared > 0.) {
                            //calculate executed bid
                            order exec_bid;
                            auto exec_bid_price = bid_price;
                            auto bid_trader_id = bid_order.get_id();

                            exec_bid.set_id(bid_trader_id);
                            exec_bid.set_status(order::active);
                            exec_bid.set_order_type(order::limit);
                            exec_bid.set_ordered_asset(bid_order.get_ordered_asset());
                            exec_bid.set_order_size((BID - uncleared), exec_bid_price);

                            get<0>(result) = exec_bid;



                            //calculate executed ask
                            order exec_ask;
                            auto exec_ask_price = bid_price;
                            auto ask_trader_id = market_order.get_id();

                            exec_ask.set_id(ask_trader_id);
                            exec_ask.set_status(order::active);
                            exec_ask.set_order_type(order::market);
                            exec_ask.set_ordered_asset(market_order.get_ordered_asset());
                            exec_ask.set_order_size(-(BID - uncleared), exec_ask_price);

                            get<1>(result) = exec_ask;


                            (&bid_order)->set_order_size(uncleared, bid_price);
                            (&market_order)->set_status(order::filled);

                        } else if (uncleared == 0.) {
                            //calculate executed bid
                            order exec_bid;
                            auto exec_bid_price = bid_price;
                            auto bid_trader_id = bid_order.get_id();

                            exec_bid.set_id(bid_trader_id);
                            exec_bid.set_status(order::active);
                            exec_bid.set_order_type(order::limit);
                            exec_bid.set_ordered_asset(bid_order.get_ordered_asset());
                            exec_bid.set_order_size(BID, exec_bid_price);

                            get<0>(result) = exec_bid;


                            //calculate executed ask
                            order exec_ask;
                            auto exec_ask_price = bid_price;
                            auto ask_trader_id = market_order.get_id();

                            exec_ask.set_id(ask_trader_id);
                            exec_ask.set_status(order::active);
                            exec_ask.set_order_type(order::market);
                            exec_ask.set_ordered_asset(market_order.get_ordered_asset());
                            exec_ask.set_order_size(-BID, exec_ask_price);

                            get<1>(result) = exec_ask;


                            (&bid_order)->set_status(order::filled);
                            (&market_order)->set_status(order::filled);
                        }

                    }

        std::cout<<"passed 2 "<<std::endl;

            }else
                //solve for bid market orders
                if (market_order.get_order_size() > 0.) {



                double ask_price = (floor(ask_order.get_proposed_price()*100))/100;


                    bool ask_is_active = ask_order.get_status();
                    bool mktbid_is_active = market_order.get_status();


                    if (ask_is_active && mktbid_is_active) {
                        //trade
                        double mkt_cash = market_order.get_order_size() * market_order.get_proposed_price();
                        double BID = mkt_cash / (ask_price);
                        double ASK = ask_order.get_order_size();
                        auto uncleared = BID + ASK;



//solve for the 3 situations: uncleared <, > or = 0;
                        if (uncleared < 0.) {
                            //calculate executed bid
                            order exec_bid;
                            auto exec_bid_price = ask_price;
                            auto bid_trader_id = market_order.get_id();

                            exec_bid.set_id(bid_trader_id);
                            exec_bid.set_status(order::active);
                            exec_bid.set_order_type(order::market);
                            exec_bid.set_ordered_asset(market_order.get_ordered_asset());
                            exec_bid.set_order_size(BID, exec_bid_price);

                            get<0>(result) = exec_bid;

                            //calculate executed ask
                            order exec_ask;
                            auto exec_ask_price = ask_price;
                            auto ask_trader_id = ask_order.get_id();

                            exec_ask.set_id(ask_trader_id);
                            exec_ask.set_status(order::active);
                            exec_ask.set_order_type(order::limit);
                            exec_ask.set_ordered_asset(ask_order.get_ordered_asset());
                            exec_ask.set_order_size(-BID, exec_ask_price);

                            get<1>(result) = exec_ask;



                            //update the ask orders remaining in the order vectors after execution of trade
                            (&ask_order)->set_order_size(uncleared, ask_price);
                            //remove the fully filled bid order
                            (&market_order)->set_status(order::filled);

                        } else if (uncleared > 0.) {
                            //calculate executed bid
                            order exec_bid;
                            auto exec_bid_price = ask_price;
                            auto bid_trader_id = market_order.get_id();

                            exec_bid.set_id(bid_trader_id);
                            exec_bid.set_status(order::active);
                            exec_bid.set_order_type(order::market);
                            exec_bid.set_ordered_asset(market_order.get_ordered_asset());
                            exec_bid.set_order_size((BID - uncleared), exec_bid_price);

                            get<0>(result) = exec_bid;



                            //calculate executed ask
                            order exec_ask;
                            auto exec_ask_price = ask_price;
                            auto ask_trader_id = ask_order.get_id();

                            exec_ask.set_id(ask_trader_id);
                            exec_ask.set_status(order::active);
                            exec_ask.set_order_type(order::limit);
                            exec_ask.set_ordered_asset(ask_order.get_ordered_asset());
                            exec_ask.set_order_size(-(BID - uncleared), exec_ask_price);

                            get<1>(result) = exec_ask;

                            (&market_order)->set_order_size(
                                    (uncleared * ask_price) / (&market_order)->get_proposed_price(),
                                    (&market_order)->get_proposed_price());
                            (&ask_order)->set_status(order::filled);


                        } else if (uncleared == 0.) {
                            //calculate executed bid
                            order exec_bid;
                            auto exec_bid_price = ask_price;
                            auto bid_trader_id = (&market_order)->get_id();

                            exec_bid.set_id(bid_trader_id);
                            exec_bid.set_status(order::active);
                            exec_bid.set_order_type(order::market);
                            exec_bid.set_ordered_asset((&market_order)->get_ordered_asset());
                            exec_bid.set_order_size(BID, exec_bid_price);

                            get<0>(result) = exec_bid;




                            //calculate executed ask
                            order exec_ask;
                            auto exec_ask_price = ask_price;
                            auto ask_trader_id = ask_order.get_id();

                            exec_ask.set_id(ask_trader_id);
                            exec_ask.set_status(order::active);
                            exec_ask.set_order_type(order::limit);
                            exec_ask.set_ordered_asset(ask_order.get_ordered_asset());
                            exec_ask.set_order_size(-BID, exec_ask_price);

                            get<1>(result) = exec_ask;


                            market_order.set_status(order::filled);
                            ask_order.set_status(order::filled);
                        }
                    }
                    std::cout<<"passed 3 "<<std::endl;
        }

return result;
}


tuple<order,order>
match_limit_orders(order &bid_order,order &ask_order){

    tuple<order,order>  result;

    double bid_price = (floor(bid_order.get_proposed_price()*100))/100;//assuming the ask order arrived first
    double ask_price = (floor(ask_order.get_proposed_price()*100))/100;

    bool bid_is_active = bid_order.get_status();
    bool ask_is_active = ask_order.get_status();

    //non-stale orders
    if (bid_is_active && ask_is_active) {
        //trade
        std::cout<<bid_order.get_id()<<" bid "<<bid_order.get_order_size()<<" at "<< bid_order.get_proposed_price() <<std::endl;
        std::cout<<ask_order.get_id()<<" ask "<<ask_order.get_order_size()<<" at "<< ask_order.get_proposed_price()<<std::endl;
        double BID = bid_order.get_order_size();
        double ASK = ask_order.get_order_size();
        auto uncleared = BID + ASK;

        //solve for the 3 situations: uncleared <, > or = 0;
        if (uncleared < 0.) {
            //calculate executed bid
            order exec_bid;
            auto exec_bid_price = bid_price;
            auto bid_trader_id = bid_order.get_id();

            exec_bid.set_id(bid_trader_id);
            exec_bid.set_status(order::active);
            exec_bid.set_order_type(order::limit);
            exec_bid.set_ordered_asset(bid_order.get_ordered_asset());
            exec_bid.set_order_size(BID, exec_bid_price);

            get<0>(result) = exec_bid;


            //calculate executed ask
            order exec_ask;
            auto exec_ask_price = ask_price;
            auto ask_trader_id = ask_order.get_id();

            exec_ask.set_id(ask_trader_id);
            exec_ask.set_status(order::active);
            exec_ask.set_order_type(order::limit);
            exec_ask.set_ordered_asset(ask_order.get_ordered_asset());
            exec_ask.set_order_size(-BID, exec_ask_price);

            get<1>(result) = exec_ask;


            //update the ask orders remaining in the order vectors after execution of trade
            (&ask_order)->set_order_size(uncleared, ask_price);
            //remove the fully filled bid order
            (&bid_order)->set_status(order::filled);
            std::cout<<"passed 5 "<<std::endl;
        } else if (uncleared > 0.) {
            //calculate executed bid
            order exec_bid;
            auto exec_bid_price = bid_price;
            auto bid_trader_id = bid_order.get_id();

            exec_bid.set_id(bid_trader_id);
            exec_bid.set_status(order::active);
            exec_bid.set_order_type(order::limit);
            exec_bid.set_ordered_asset(bid_order.get_ordered_asset());
            exec_bid.set_order_size((BID - uncleared), exec_bid_price);

            get<0>(result) = exec_bid;




            //calculate executed ask
            order exec_ask;
            auto exec_ask_price = ask_price;
            auto ask_trader_id = ask_order.get_id();

            exec_ask.set_id(ask_trader_id);
            exec_ask.set_status(order::active);
            exec_ask.set_order_type(order::limit);
            exec_ask.set_ordered_asset(ask_order.get_ordered_asset());
            exec_ask.set_order_size(-(BID - uncleared), exec_ask_price);

            get<1>(result) = exec_ask;


            (&bid_order)->set_order_size(uncleared, bid_price);
            (&ask_order)->set_status(order::filled);
            std::cout<<"passed 6 "<<std::endl;
        } else {
            //calculate executed bid
            order exec_bid;
            auto exec_bid_price = bid_price;
            auto bid_trader_id = bid_order.get_id();

            exec_bid.set_id(bid_trader_id);
            exec_bid.set_status(order::active);
            exec_bid.set_order_type(order::limit);
            exec_bid.set_ordered_asset(bid_order.get_ordered_asset());
            exec_bid.set_order_size(BID, exec_bid_price);

            get<0>(result) = exec_bid;


            //calculate executed ask
            order exec_ask;
            auto exec_ask_price = ask_price;
            auto ask_trader_id = ask_order.get_id();

            exec_ask.set_id(ask_trader_id);
            exec_ask.set_status(order::active);
            exec_ask.set_order_type(order::limit);
            exec_ask.set_ordered_asset(ask_order.get_ordered_asset());
            exec_ask.set_order_size(-BID, exec_ask_price);

            get<1>(result) = exec_ask;

            //update cash earned from spread
            (&bid_order)->set_status(order::filled);
            (&ask_order)->set_status(order::filled);

        }
//        ask_is_active = ask_order.get_status();
//        bid_is_active =  bid_order.get_status();
        std::cout<<"passed 7 "<<std::endl;
    }

    return result;
}


map<int, vector<order>>
record_executed_orders(tuple<order,order> &settlements){

    map<int, vector<order>> executed_orders_;

    order order1 = get<0>(settlements);
    order order2 = get<1>(settlements);
    std::cout<<"settled 0: "<<order1.get_id()<<" settled 1: "<<order2.get_id()<<std::endl;
    if (executed_orders_.find(order1.get_id()) != executed_orders_.end()){
        executed_orders_.find(order1.get_id())->second.push_back(order1);
    }else{
        executed_orders_.emplace(order1.get_id(), vector<order>{order1});
    }


    if (executed_orders_.find(order2.get_id()) != executed_orders_.end()){
        executed_orders_.find(order2.get_id())->second.push_back(order2);
    }else{
        executed_orders_.emplace(order2.get_id(), vector<order>{order2});
    }

    return executed_orders_;
}


order *get_best_bid(map<double, vector<order>, greater<>> &bids){
    order *result = {};
    for(auto &[b,vec]: bids){
        for(auto &x:vec){
            if (x.get_status() == 1){
                result = &x;
                goto exit;
            }
        }
    }
    exit:;
    return result;
}


order *get_best_ask(map<double, vector<order>> &asks){
    order *result = {};
    for(auto &[a,vec]: asks){
        for(auto &x:vec){
            if (x.get_status() == 1){
                result = (&x);
                goto exit;
            }
        }
    }
    exit:;
    return result;
}


void cancel_mm_orders(int order_id
                    , map<double, vector<order>, greater<>> &bids
                                ,map<double, vector<order>> &asks){


for(auto &[price, vec]: bids){
    for(auto &x : vec){
        if(x.get_id() == order_id){
            x.set_status(order::filled);
        }
    }
}

for(auto &[price, vec]: asks){
    for(auto &x : vec){
        if(x.get_id() == order_id){
            x.set_status(order::filled);
        }
    }
}

}


void add_mm_orders(tuple<order,order> new_orders
                  ,map<double, vector<order>, greater<>> &bids
                     ,map<double, vector<order>> &asks){

    double bid_price = get<0>(new_orders).get_proposed_price();
    bid_price = floor(bid_price*100)/100;

    double ask_price = get<1>(new_orders).get_proposed_price();
    ask_price = floor(ask_price*100)/100;


    //add bid
    if (bids.find(bid_price) != bids.end()){
        bids.find(bid_price)->second.push_back(get<0>(new_orders));
        }
    else{
            bids.emplace(bid_price, vector{get<0>(new_orders)});
            }

    //add ask
    if (asks.find(ask_price) != asks.end()){
        asks.find(ask_price)->second.push_back(get<1>(new_orders));
    }else{
        asks.emplace(ask_price, vector{get<1>(new_orders)});
    }


}


tuple<order,order>
price_setter::update_mm_orders(const map<double, vector<order>, greater<>> &bid_orders_
                    ,const map<double, vector<order>> &ask_orders_
                        ,double &init_inventory
                            ,double &curr_inventory
                                    ,double init_price
                                        ,int identifier
                                            ,int asset_id
                                                ,double quote) const{

    tuple<order,order>  new_order;

//    double best_ask_;
//    double best_bid_;

    //search for best bid
//    for(auto &[price, vec]: bid_orders_){
//        int x = 0;
//        for(auto &o:vec){
//            if (!o.filled){
//                x++;
//            }
//        }
//        if(x>0){
//            best_bid_ = price;
//            break;
//        }
//    }

    //search for best ask
//    for(auto &[price, vec]: ask_orders_){
//        int y = 0;
//        for(auto &o:vec){
//            if (!o.filled){
//                y++;
//            }
//        }
//        if(y>0){
//            best_ask_ = price;
//            break;
//        }
//    }



    double target_bid;
    double target_ask;

    double new_bid;
    double new_ask;

    order sell_limit;
    order buy_limit;

//compute target bid and target ask
    double d_shares   =  curr_inventory - init_inventory;

    target_ask =  init_price - ((d_shares/100)*0.01);
    std::cout<<"curr_cash "<< this->cash_at_hand<<"curr_shares " <<this->stocks_at_hand.find(asset_id)->second<<" dshares "<<d_shares<<" price_movement "<<((d_shares/1000)*0.01)<<std::endl;

    target_bid = 0.99*target_ask ;//market maker's bid price is always atleast 5 basis points below the ask price


//setting a new_ask price give a target ask


        new_ask = target_ask;



//setting a new bid_price given a target bid

        new_bid = target_bid;

//round order prices  downwards to two dp
        new_ask = (floor(new_ask*100))/100;
        new_bid = (floor(new_bid*100))/100;

//create order
    //double cash_ = cash_at_hand/3;
    double sell_size = min(abs(this->stocks_at_hand.find(asset_id)->second), 100*quote);

    double buy_size;
    if(this->cash_at_hand>quote){
        buy_size = min(this->cash_at_hand/(3*quote), 100*quote );
    }else{
        buy_size = 0;
    }


    //buy order
    buy_limit.set_id(identifier);
    buy_limit.set_order_type(order::limit);
    buy_limit.set_ordered_asset(asset_id);
    buy_limit.set_order_size(buy_size, new_bid);
    buy_limit.set_status(order::active);
    get<0>(new_order) = buy_limit;

    //sell order
    sell_limit.set_id(identifier);
    sell_limit.set_order_type(order::limit);
    sell_limit.set_ordered_asset(asset_id);
    sell_limit.set_order_size(-sell_size, new_ask);
    sell_limit.set_status(order::active);
    get<1>(new_order) = sell_limit;


    return new_order;
}


void
price_setter::clear(market_watch &market_clock) {



//useful parameters

double total_spending = 0;
double market_wealth = 1.68e10;
double shares_outstanding = 0.5 * market_wealth / 75;
double market_cash = 0.5 * market_wealth;


//        unique_lock<std::mutex> ul(g_mutex);
//        g_cv.wait(ul, [this](){return g_ready;});
//Params definition
        map<int, order> ask_result_ = {};
        map<int, order> bid_result_ = {};


//map<asset, map<price,order>> market_maker_orders
        map<int, map<int, order>> mm_orders = {};


        auto quotes = this->assets;


        int t = int(clock->current_time());
        std::cout << std::endl;
        std::cout << "time " << t << std::endl;

        if (balance_count < t) {
            this->balance_cf(t);
            balance_count++;
        }

        double short_div = 0.;
        for (auto &[j, a]:this->trading_institutions) {
            for (auto &[i, b]: stocks_on_market) {
                if ((a->stocks_at_hand.find(i) != a->stocks_at_hand.end())
                    && a->stocks_at_hand.find(i)->second < 0.) {
                    short_div += a->stocks_at_hand.find(i)->second * -b.get_dividend(t);
                }
            }
        }

        this->cash_at_hand += short_div;


        for (auto &[i, j] : this->stocks_at_hand) {

            int short_stocks = 0;
            for (auto &[j, k]:this->trading_institutions) {
                if ((k->stocks_at_hand.find(i) != k->stocks_at_hand.end())
                    && k->stocks_at_hand.find(i)->second < 0) {

                    short_stocks += k->stocks_at_hand.find(i)->second;

                }
//                double short_stock_increase = short_stocks - this->short_stock.find(i)->second;
//                this->stocks_at_hand.find(i)->second += short_stock_increase;
//                this->short_stock.find(i)->second = short_stocks;

            
            }

            if (inventory_t_1.find(i) != inventory_t_1.end()) {
//            inventory_t_1.find(i)->second = j + short_stocks;
//            study.find(i)->second.push_back(j);
//        } else {
                inventory_t_1.emplace(i, j);
//            study.emplace(i, j);
            }
        }



//receive orders

//furthest range a market maker is allowed to charge away from the NBBO price

        map<int, vector<order>> executed_orders_;
        for (auto &[k, stock]:assets) {
            int assettid = k;
            double prevailing_bid;//*(1-regulation_range_);
            double prevailing_ask;//*(1+regulation_range_);


//
            int short_stocks = 0;
            for (auto &[j, x]:this->trading_institutions) {
                if ((x->stocks_at_hand.find(k) != x->stocks_at_hand.end())
                    && x->stocks_at_hand.find(k)->second < 0) {

                    short_stocks += x->stocks_at_hand.find(k)->second;
                }
            }



//create price maps
//buy-limits
            map<double, vector<order>, greater<>> bid_orders = {};
            bid_orders.clear();
            if (!this->bid.empty()) {
                if (this->bid.find(k) != this->bid.end()){
                    {
                        for (auto &[i,x] : this->bid.find(k)->second) {
                                double _bid_price = x.get_proposed_price();
                                _bid_price = (floor(_bid_price * 100)) / 100;

                                if (bid_orders.find(_bid_price) != bid_orders.end()) {
                                    bid_orders.find(_bid_price)->second.push_back(x);
                                } else {
                                    bid_orders.emplace(_bid_price, vector({x}));
                                }
                            }
                    }
                }
            }



//sell-limits
            map<double, vector<order>> ask_orders = {};
            ask_orders.clear();
            if (!this->ask.empty()) {
                if (this->ask.find(k) != this->ask.end()) {
                    for (auto &[i,x] : this->ask.find(k)->second) {
                        double _ask_price = x.get_proposed_price();
                        _ask_price = (floor(_ask_price * 100)) / 100;
                        if (ask_orders.find(_ask_price) != ask_orders.end()) {
                            ask_orders.find(_ask_price)->second.push_back(x);
                        } else {
                            ask_orders.emplace(_ask_price, vector({x}));
                        }
                    }
                }
            }




//market orders
//        map<double, queue<order>> market_bids_asks = {};
//        if (!this->market_orders.empty()) {
//            if (this->market_orders.find(k) != this->market_orders.end()) {
//                while(!this->market_orders.find(k)->second.empty()) {
//                    double market_price = this->market_orders.find(k)->second.front().get_proposed_price();
//
//
//                    market_price = (floor(market_price * 100)) / 100;
//                    if (market_bids_asks.find(market_price) != market_bids_asks.end()) {
//                        market_bids_asks.find(market_price)->second.push(this->market_orders.find(k)->second.front());
//                    } else {
//                        queue<order> a;
//                        a.push(this->market_orders.find(k)->second.front());
//                        market_bids_asks.emplace(market_price, a);
//                    }
//
//                    this->market_orders.find(k)->second.pop();
//                }
//            }
//        }


////sum of orders
//        double sum_ = 0.;
//        if (!this->market_orders.empty()) {
//            if (this->market_orders.find(k) != this->market_orders.end()) {
//                for (auto &x : this->market_orders.find(k)->second) {
//                   if (x.get_status() ==1){
//                       sum_ += abs(x.get_order_size());
//                   }
//                }
//            }
//        }
//
////probability of first execution
//    map<double, int> probability_ = {};
//        if (!this->market_orders.empty()) {
//            if (this->market_orders.find(k) != this->market_orders.end()) {
//                for (auto &x : this->market_orders.find(k)->second){
//                    if(x.get_status() ==1){
//                        probability_.emplace(abs(x.get_order_size())/sum_, x.get_id());
//                }
//            }
//        }
//    }
//
////cumulative probabilities
//    map<double, int> cum_probability_ = {};
//        double cum_sum = 0.;
//            for (auto &[p, id] : probability_){
//                    cum_sum += p;
//                        cum_probability_.emplace(cum_sum, id);
//            }
//            for(auto &[a,b]: cum_probability_){
//                std::cout<<b<<" : "<<a<<" ";
//            }
//
////pick executed transaction by cum_probability
//        std::random_device rd;
//        std::mt19937 e2(rd());
//        std::uniform_real_distribution<> dist(0.0, 1.0);
//        double random_number = dist(e2);
//
//        int first_comer = 0;
//            for (auto &[cum_prob, id] : cum_probability_){
//                if (cum_prob >= random_number){
//                    first_comer = id;
//                        break;
//                    }
//            }
//        std::cout<<endl;
//        std::cout<<"rand: "<<random_number<<" first_comer: "<<first_comer <<endl;
//
//


//match market orders

//            cancel_mm_orders(
//                    this->get_identifier(), bid_orders, ask_orders);


            tuple<order, order> new_orders1 =
                    update_mm_orders(bid_orders, ask_orders, this->initial_invetory.find(k)->second,
                                     (this->stocks_at_hand.find(k)->second), 25.,
                                     this->get_identifier(), k, stock.get_midprice());

            add_mm_orders(new_orders1, bid_orders, ask_orders);
//                    std::cout << "mmsk0 " << get<1>(new_orders1).get_proposed_price() << std::endl;
            //match market orders with limit orders

            while (this->market_orders.find(k) != market_orders.end()
                    && !this->market_orders.find(k)->second.empty()) {
                tuple<order, order>
                        matched_orders =
                        match_market_orders(this->market_orders.find(k)->second.front(),
                                            *get_best_bid(bid_orders), *get_best_ask(ask_orders));

                if (this->market_orders.find(k)->second.front().get_status() == order::filled) {
                    std::cout << "morder_before_pop " << this->market_orders.find(k)->second.front().get_status()
                              << " size " << this->market_orders.find(k)->second.front().get_order_size() << std::endl;
                    this->market_orders.find(k)->second.pop();
//                    std::cout << "morder_after_pop " << this->market_orders.find(k)->second.front().get_status()
//                              << " size " << this->market_orders.find(k)->second.front().get_order_size() << std::endl;
                }

                executed_orders_ = {};
                executed_orders_ = record_executed_orders(matched_orders);

                cancel_mm_orders(
                        this->get_identifier(), bid_orders, ask_orders);

                tuple<order, order> new_orders2 =
                        update_mm_orders(bid_orders, ask_orders, this->initial_invetory.find(k)->second,
                                         (this->stocks_at_hand.find(k)->second), 25.,
                                         this->get_identifier(), k, stock.get_midprice());

                add_mm_orders(new_orders2, bid_orders, ask_orders);
            }

            //update balances after each match
            if (executed_orders_.find(this->get_identifier()) != executed_orders_.end()) {
                this->balance_bd(executed_orders_.find(
                        this->get_identifier()
                )->second);
            }

            for (auto &[i, f]:executed_orders_) {
                if (i != this->get_identifier()) {
                    if (this->trading_institutions.find(i) != this->trading_institutions.end()) {
                        this->trading_institutions.find(i)->second->balance_bd(f);
                    }
                }
            }


////////////////////////////
//int active_mmbid_orders;
//int active_mmask_orders;
////update market maker's orders
//int active_market_orders =0;
//        for (auto &[price, orders]:market_bids_asks) {
//            for (auto &x:orders) {
//                if(x.get_status() == 1){
//                    active_market_orders++;
//                }
//            }
//        }
//
//
//
//do {
////    std::cout<<"passed by 0"<<std::endl;
//    active_mmbid_orders =0;
//    active_mmask_orders =0;

//update market maker's orders
            cancel_mm_orders(
                    this->get_identifier(), bid_orders, ask_orders);

            tuple<order, order> new_orders =
                    update_mm_orders(bid_orders, ask_orders, this->initial_invetory.find(k)->second,
                                     (this->stocks_at_hand.find(k)->second), 25.,
                                     this->get_identifier(), k, stock.get_midprice());

            add_mm_orders(new_orders, bid_orders, ask_orders);



//match limit orders after all market orders have been executed




//            std::cout << "bestbid_ " << get_best_bid(bid_orders)->get_proposed_price() << " from "
//                      << get_best_bid(bid_orders)->get_id() << std::endl;
//            std::cout << "bestask_ " << get_best_ask(ask_orders)->get_proposed_price() << " from "
//                      << get_best_ask(ask_orders)->get_id() << std::endl;
            while (get_best_bid(bid_orders)->get_proposed_price() >= get_best_ask(ask_orders)->get_proposed_price()
            && get_best_bid(bid_orders)->get_order_size()>0&& get_best_ask(ask_orders)->get_order_size()>0) {
//        std::cout<<"passed by 2"<<std::endl;
                tuple<order, order>
                        matched_limits2 =
                        match_limit_orders(*get_best_bid(bid_orders), *get_best_ask(ask_orders));


                executed_orders_ = {};
                executed_orders_ = record_executed_orders(matched_limits2);

                //update balances after each match
                if (executed_orders_.find(this->get_identifier()) != executed_orders_.end()) {
                    this->balance_bd(executed_orders_.find(
                            this->get_identifier()
                    )->second);
                }

                for (auto &[i, f]:executed_orders_) {
                    if (i != this->get_identifier()) {
                        if(this->trading_institutions.find(i)!= this->trading_institutions.end()){
                        this->trading_institutions.find(i)->second->balance_bd(f);
                        }
                    }
                }



            }

            {
                cancel_mm_orders(
                        this->get_identifier(), bid_orders, ask_orders);

                tuple<order, order> new_orders2 =
                        update_mm_orders(bid_orders, ask_orders, this->initial_invetory.find(k)->second,
                                         (this->stocks_at_hand.find(k)->second), 25.,
                                         this->get_identifier(), k, stock.get_midprice());

                add_mm_orders(new_orders2, bid_orders, ask_orders);

        }


            prevailing_ask = floor((*get_best_ask(ask_orders)).get_proposed_price()*100)/100;
            prevailing_bid = floor((*get_best_bid(bid_orders)).get_proposed_price()*100)/100;

//            std::cout << get_best_ask(ask_orders)->get_id() << " " << get_best_ask(ask_orders)->get_order_size() << " "
//                      << prevailing_ask;
//            std::cout << " " << prevailing_bid << get_best_bid(bid_orders)->get_order_size() << " "
//                      << get_best_bid(bid_orders)->get_order_size() << " " << get_best_bid(bid_orders)->get_id()
//                      << std::endl;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//inform traders about new prices
            for (auto &[i, v]: this->trading_institutions){
                v->stocks_on_market.find(k)->second.set_price(
                        t, prevailing_bid, prevailing_ask);
            }

            this->assets.find(k)->second.set_price(
                    t, prevailing_bid, prevailing_ask);


            this->best_bid.find(k)->second = prevailing_bid;
            this->best_ask.find(k)->second = prevailing_ask;

//            std::cout << "ask " << prevailing_ask << " " << "bid " << prevailing_bid << "midprice "
//                      << (prevailing_ask + prevailing_bid) / 2 << std::endl;
        }

        //spending
        for(auto &[x,y]: this->trading_institutions){
        (y)->wealth -= (y->wealth/market_wealth)*total_spending;
        }
        this->wealth -= (this->wealth/market_wealth)*total_spending;
        total_spending = 0;


//        for(auto &[x,y]: this->trading_institutions){
//            std::cout<<"new_weath "<<(y->wealth)/10e09<<std::endl;
//        }


        if (day_count < int((&market_clock)->current_time())) {

            //pay interest on cash and dividends on stocks
            for (auto &[i, v]: this->trading_institutions) {
                v->balance_cf(int((&market_clock)->current_time()));
            }

            std::cout << std::endl;

            for (auto &[a, b] : tradeable_assets()) {
                total_spending += b.get_dividend(day_count+1)*0.5 * market_wealth / 75;
            }
            day_count++;
        }else{
            total_spending = 0;
        }

        std::cout << " market's watch time " << (&market_clock)->current_time() << std::endl;
        //start receiving updated orders at this point
//        g_ready = false;
//        ul.unlock();
//        g_cv.notify_one();
//        ul.lock();



}
