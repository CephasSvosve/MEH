//
// Created by Cephas Svosve on 22/12/2021.
//

#include "funds.h"
#include <string>
#include <fstream>

funds::funds(int bias) {

//noise traders variations

    this->noise_seed = bias;
    std::cout<<"**noise_seed** "<< noise_seed << std::endl;
    dX = generateWhiteNoise1(this->noise_seed,3,25200);

//value trader variations
    this->bias = 0.00001*(bias-1);

//trend-followers varitions

    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_int_distribution<> dist(21, 504);
    int random_number1 = dist(e2);
    int random_number2 = dist(e2);

    this->window_size_MA1 = (random_number1<random_number2)? random_number1:random_number2;
    this->window_size_MA2 = (random_number1>random_number2)? random_number1:random_number2;
    
    


}


map<int, order>
funds::invest(){
    int a = int(this->clock->current_time());
    map<int, order> result;
    switch (this->fund_philosophy) {

        case 4:
            result= this->ag_value_demand();
            break;
        case 5:
            result= this->ag_noise_demand();
            break;
        case 6:
            if(a>window_size){
                result= this->ag_momentum_demand();}
            break;
        case 7:
            if (a>value_window){
                result= this->ag_growth_demand();}
            break;
        case 8:
            if (a>63){
            result= this->ag_index_demand();}
            break;
        default:
            break;

    }
    return result;

}


map<int, order>
funds::current_price_order(
        int order_id
        , int asset_id
        , double quote
        , double allocation_
        ,double port_alloc
        , double wealth_
        , double available_cash
        , double current_position
        ){
    order market_order;
    order limit_order;
    map<int, order> result_;
    //proportion of orders executed by limit entries
    double limit_order_proportion =0.5;

    double T = ((port_alloc*wealth_*allocation_)/quote); //target position
    double C = current_position;//current position
    double E = T-C;//Excess demand
    int order_id_ = 0;

    //There are two types of transactions:
    // (1) those that require money to buy more shares or sell short more shares
    // (2) those that require closing off part of an already existing position to reduce exposure


    //signature
//            //Closing part of a position (will not require money)
//            if(T<C && T>0){}else
//            if(T>C && T<0){}else
//            //expanding a position (requires money)
//            if(T<C && C<0){}else
//            if(T>C && C>0){}else
//            //closing off previous position then expand into a new position (hybrid)
//            if(T>0 && C<0){}else
//            if(T<0 && C>0){}else{}


    //Closing part of a position (will not require money)
    if(T<C && T>=0){
        if (!this->pending_market_order.find(asset_id)->second) {
            order_id_ = concat(1, order_id);
            market_order.set_id(this->get_identifier());
            market_order.set_order_type(order::market);
            market_order.set_ordered_asset(asset_id);
            market_order.set_order_size((1 - limit_order_proportion) * E, quote);
            market_order.set_status(order::active);
            result_.emplace(order_id_, market_order);
            this->pending_market_order.find(asset_id)->second = true;
        }

        order_id_ = concat(2, order_id);
        limit_order.set_id(this->get_identifier());
        limit_order.set_order_type(order::limit);
        limit_order.set_ordered_asset(asset_id);
        limit_order.set_order_size((limit_order_proportion)*E, quote);
        limit_order.set_status(order::active);
        result_.emplace(order_id_, limit_order);
    }else
    if(T>C && T<=0){

        if (!this->pending_market_order.find(asset_id)->second) {
            order_id_ = concat(1, order_id);
        market_order.set_id(this->get_identifier());
        market_order.set_order_type(order::market);
        market_order.set_ordered_asset(asset_id);
        market_order.set_order_size((1-limit_order_proportion)*E, quote);
        market_order.set_status(order::active);
        result_.emplace(order_id_, market_order);
        this->pending_market_order.find(asset_id)->second = true;
        }

        order_id_ = concat(2, order_id);
        limit_order.set_id(this->get_identifier());
        limit_order.set_order_type(order::limit);
        limit_order.set_ordered_asset(asset_id);
        limit_order.set_order_size((limit_order_proportion)*E, quote);
        limit_order.set_status(order::active);
        result_.emplace(order_id_, limit_order);
    }else
        //expanding a position (requires money)
    if(T<C && C<=0){

        double available_funds = min(abs(available_cash*allocation_/quote), abs(E));
        available_funds = (E/abs(E))* available_funds;//we retain the sign of excess demand

        if (!this->pending_market_order.find(asset_id)->second) {
            order_id_ = concat(1, order_id);
        market_order.set_id(this->get_identifier());
        market_order.set_order_type(order::market);
        market_order.set_ordered_asset(asset_id);
        market_order.set_order_size((1-limit_order_proportion)*available_funds, quote);
        market_order.set_status(order::active);
        result_.emplace(order_id_, market_order);
        this->pending_market_order.find(asset_id)->second = true;
        }

        order_id_ = concat(2, order_id);
        limit_order.set_id(this->get_identifier());
        limit_order.set_order_type(order::limit);
        limit_order.set_ordered_asset(asset_id);
        limit_order.set_order_size((limit_order_proportion)*available_funds, quote);
        limit_order.set_status(order::active);
        result_.emplace(order_id_, limit_order);

    }else
    if(T>C && C>=0){
        double available_funds = min(abs(available_cash*allocation_/quote), abs(E));
        available_funds = (E/abs(E))* available_funds;//we retain the sign of excess demand

        if (!this->pending_market_order.find(asset_id)->second) {
            order_id_ = concat(1, order_id);
        market_order.set_id(this->get_identifier());
        market_order.set_order_type(order::market);
        market_order.set_ordered_asset(asset_id);
        market_order.set_order_size((1-limit_order_proportion)*available_funds, quote);
        market_order.set_status(order::active);
        result_.emplace(order_id_, market_order);
        this->pending_market_order.find(asset_id)->second = true;
        }

        order_id_ = concat(2, order_id);
        limit_order.set_id(this->get_identifier());
        limit_order.set_order_type(order::limit);
        limit_order.set_ordered_asset(asset_id);
        limit_order.set_order_size((limit_order_proportion)*available_funds, quote);
        limit_order.set_status(order::active);
        result_.emplace(order_id_, limit_order);

    }else
        //closing off previous position then expand into a new position  (hybrid)
    if(T>0 && C<0){
        //then expand to the new position
        double available_funds = min(abs(available_cash*allocation_/quote), abs(T));
        available_funds = ((T/abs(T))* available_funds)-C;//we retain the sign of excess demand

        if (!this->pending_market_order.find(asset_id)->second) {
        //first close off current position
            order_id_ = concat(1, order_id);
        market_order.set_id(this->get_identifier());
        market_order.set_order_type(order::market);
        market_order.set_ordered_asset(asset_id);
        market_order.set_order_size((1-limit_order_proportion)*available_funds, quote);
        market_order.set_status(order::active);
        result_.emplace(order_id_, market_order);
        this->pending_market_order.find(asset_id)->second = true;
        }

        order_id_ = concat(2, order_id);
        limit_order.set_id(this->get_identifier());
        limit_order.set_order_type(order::limit);
        limit_order.set_ordered_asset(asset_id);
        limit_order.set_order_size((limit_order_proportion)*available_funds, quote);
        limit_order.set_status(order::active);
        result_.emplace(order_id_, limit_order);

    }else
    if(T<0 && C>0){

        double available_funds = min(abs(available_cash*allocation_/quote), abs(T));
        available_funds = ((T/abs(T))* available_funds)-C;//we retain the sign of excess demand


        //first close off current position
        if (!this->pending_market_order.find(asset_id)->second) {
            order_id_ = concat(1, order_id);
        market_order.set_id(this->get_identifier());
        market_order.set_order_type(order::market);
        market_order.set_ordered_asset(asset_id);
        market_order.set_order_size((1-limit_order_proportion)*available_funds, quote);
        market_order.set_status(order::active);
        result_.emplace(order_id_, market_order);
        this->pending_market_order.find(asset_id)->second = true;
        }

        order_id_ = concat(2, order_id);
        limit_order.set_id(this->get_identifier());
        limit_order.set_order_type(order::limit);
        limit_order.set_ordered_asset(asset_id);
        limit_order.set_order_size((limit_order_proportion)*available_funds, quote);
        limit_order.set_status(order::active);
        result_.emplace(order_id_, limit_order);

    }

return result_;
}

map<int, order>
funds::reb_wealth(
        int order_id
        , int asset_id
        , double quote
        , double allocation_
        ,double port_alloc
        , double wealth_
        , double current_position
){
    double T = ((port_alloc*wealth_*allocation_)/quote); //target position
    double C = current_position;

    double excess = T-C;


    order market_order;
    order limit_order;
    map<int, order> result_;
    double limit_order_proportion =0.5;
    int order_id_ = 0;

    if(T<C && T>0){


        order_id_ = concat(-5, order_id);
        limit_order.set_id(this->get_identifier());
        limit_order.set_order_type(order::limit);
        limit_order.set_ordered_asset(asset_id);
        limit_order.set_order_size(excess*limit_order_proportion, quote);
        limit_order.set_status(order::active);
        result_.emplace(order_id_, limit_order);

        order_id_ = concat(-6, order_id);
        market_order.set_id(this->get_identifier());
        market_order.set_order_type(order::market);
        market_order.set_ordered_asset(asset_id);
        market_order.set_order_size(excess*(1-limit_order_proportion), quote);
        market_order.set_status(order::active);
        result_.emplace(order_id_, market_order);
    }else
        if(T<C && T<=0 && C>0){
            order_id_ = concat(-5, order_id);
            limit_order.set_id(this->get_identifier());
            limit_order.set_order_type(order::limit);
            limit_order.set_ordered_asset(asset_id);
            limit_order.set_order_size(-C*limit_order_proportion, quote);
            limit_order.set_status(order::active);
            result_.emplace(order_id_, limit_order);

            order_id_ = concat(-6, order_id);
            market_order.set_id(this->get_identifier());
            market_order.set_order_type(order::market);
            market_order.set_ordered_asset(asset_id);
            market_order.set_order_size(-C*(1-limit_order_proportion), quote);
            market_order.set_status(order::active);
            result_.emplace(order_id_, market_order);
        }else
            if(T>C && T < 0){
                order_id_ = concat(-5, order_id);
                limit_order.set_id(this->get_identifier());
                limit_order.set_order_type(order::limit);
                limit_order.set_ordered_asset(asset_id);
                limit_order.set_order_size(excess*limit_order_proportion, quote);
                limit_order.set_status(order::active);
                result_.emplace(order_id_, limit_order);

                order_id_ = concat(-6, order_id);
                market_order.set_id(this->get_identifier());
                market_order.set_order_type(order::market);
                market_order.set_ordered_asset(asset_id);
                market_order.set_order_size(excess*(1-limit_order_proportion), quote);
                market_order.set_status(order::active);
                result_.emplace(order_id_, market_order);
            }else
                if(T>C && T>=0 && C<0){
                    order_id_ = concat(-5, order_id);
                    limit_order.set_id(this->get_identifier());
                    limit_order.set_order_type(order::limit);
                    limit_order.set_ordered_asset(asset_id);
                    limit_order.set_order_size(-C*limit_order_proportion, quote);
                    limit_order.set_status(order::active);
                    result_.emplace(order_id_, limit_order);

                    order_id_ = concat(-6, order_id);
                    market_order.set_id(this->get_identifier());
                    market_order.set_order_type(order::market);
                    market_order.set_ordered_asset(asset_id);
                    market_order.set_order_size(-C*(1-limit_order_proportion), quote);
                    market_order.set_status(order::active);
                    result_.emplace(order_id_, market_order);
                }

    return result_;
}

map<int, order>
funds::stocks_order(
        int order_id
        , int asset_id
        , double quote
        , double predicted_quote
        , double allocation_
        ,double port_alloc
        , double wealth_
        , double current_position
){
    //signature
//    if(T<C && T>0){}else
//    if(T>C && T<0){}else
//    if(T<C && C<0){}else
//    if(T>C && C>0){}else
//    {}


    order market_order;
    order limit_order;
    map<int, order> result_;
    //proportion of orders executed by limit entries


//    double T = ((port_alloc*wealth_*allocation_)/quote); //target position
    double limit_order_proportion =0.5;
    double C = current_position;//current position
    int order_id_ = 0;
    //(1) limit sell under-priced stocks at hand
    //(2) limit buy over-priced stocks at hand

    if(C<0 && allocation_<0){
        order_id_ = concat(-1, order_id);
        limit_order.set_id(this->get_identifier());
        limit_order.set_order_type(order::limit);
        limit_order.set_ordered_asset(asset_id);
        limit_order.set_order_size(-C, predicted_quote);
        limit_order.set_status(order::active);
        result_.emplace(order_id_, limit_order);
    }else
        if(C>0 && allocation_>0){
            order_id_ = concat(-1, order_id);
            limit_order.set_id(this->get_identifier());
            limit_order.set_order_type(order::limit);
            limit_order.set_ordered_asset(asset_id);
            limit_order.set_order_size(-C, predicted_quote);
            limit_order.set_status(order::active);
            result_.emplace(order_id_, limit_order);
        }else
            if(C>0 && allocation_<=0){
                order_id_ = concat(-1, order_id);
                limit_order.set_id(this->get_identifier());
                limit_order.set_order_type(order::limit);
                limit_order.set_ordered_asset(asset_id);
                limit_order.set_order_size(-C*limit_order_proportion, quote);
                limit_order.set_status(order::active);
                result_.emplace(order_id_, limit_order);

                order_id_ = concat(-2, order_id);
                market_order.set_id(this->get_identifier());
                market_order.set_order_type(order::market);
                market_order.set_ordered_asset(asset_id);
                market_order.set_order_size(-C*(1-limit_order_proportion), quote);
                market_order.set_status(order::active);
                result_.emplace(order_id_, market_order);
            }else
                if(C<0 && allocation_>=0){
                    order_id_ = concat(-1, order_id);
                    limit_order.set_id(this->get_identifier());
                    limit_order.set_order_type(order::limit);
                    limit_order.set_ordered_asset(asset_id);
                    limit_order.set_order_size(-C*limit_order_proportion, quote);
                    limit_order.set_status(order::active);
                    result_.emplace(order_id_, limit_order);

                    order_id_ = concat(-2, order_id);
                    market_order.set_id(this->get_identifier());
                    market_order.set_order_type(order::market);
                    market_order.set_ordered_asset(asset_id);
                    market_order.set_order_size(-C*(1-limit_order_proportion), quote);
                    market_order.set_status(order::active);
                    result_.emplace(order_id_, market_order);
                }

//            if(0<=T && T<C){
//                order_id = concat(-1, order_id);
//                market_order.set_id(this->get_identifier());
//                market_order.set_order_type(order::limit);
//                market_order.set_ordered_asset(asset_id);
//                market_order.set_order_size(-T, predicted_quote);
//                market_order.set_status(order::active);
//                result_.emplace(order_id, market_order);
//            }else
//            if(C<T && T<=0){
//                order_id = concat(-1, order_id);
//                limit_order.set_id(this->get_identifier());
//                limit_order.set_order_type(order::limit);
//                limit_order.set_ordered_asset(asset_id);
//                limit_order.set_order_size(-T, predicted_quote);
//                limit_order.set_status(order::active);
//                result_.emplace(order_id, limit_order);
//            }else
//            if(T<C && C<=0){
//                order_id = concat(-1, order_id);
//                limit_order.set_id(this->get_identifier());
//                limit_order.set_order_type(order::limit);
//                limit_order.set_ordered_asset(asset_id);
//                limit_order.set_order_size(-C, predicted_quote);
//                limit_order.set_status(order::active);
//                result_.emplace(order_id, limit_order);
//            }else
//            if(0<=C && C<T){
//                order_id = concat(-1, order_id);
//                limit_order.set_id(this->get_identifier());
//                limit_order.set_order_type(order::limit);
//                limit_order.set_ordered_asset(asset_id);
//                limit_order.set_order_size(-C, predicted_quote);
//                limit_order.set_status(order::active);
//                result_.emplace(order_id, limit_order);
//            }
    return result_;
}

map<int, order>
funds::free_cash_distribution(int order_id
        , int asset_id
        , double quote
        , double allocation_
        , double available_cash
){
    order market_order;
    order limit_order;
    int order_id_=0;
    map<int, order> result_;
    //proportion of orders executed by limit entries
    double limit_order_proportion =0.5;


    double T = ((available_cash*allocation_)/quote); //target position

    if(T!=0) {

        order_id_ = concat(-3, order_id);
        market_order.set_id(this->get_identifier());
        market_order.set_order_type(order::market);
        market_order.set_ordered_asset(asset_id);
        market_order.set_order_size((1 - limit_order_proportion) * T, quote);
        market_order.set_status(order::active);
        result_.emplace(order_id_, market_order);


        order_id_ = concat(-4, order_id);
        limit_order.set_id(this->get_identifier());
        limit_order.set_order_type(order::limit);
        limit_order.set_ordered_asset(asset_id);
        limit_order.set_order_size((limit_order_proportion) * T, quote);
        limit_order.set_status(order::active);
        result_.emplace(order_id_, limit_order);
    }
    return result_;
}
void funds::trade_strategy(trading_strategy tradingStrategy){
    this->fund_philosophy = tradingStrategy;

}


int funds::concat(int a, int b)
{

    // Convert both the integers to string
    string s1 = to_string(a);
    string s2 = to_string(b);

    // Concatenate both strings
    string s = s1 + s2;

    // Convert the concatenated string
    // to integer
    int c = stoi(s);

    // return the formed integer
    return c;
}

double min(double a, double b){

    double result = 0;
    bool a_is_minimum = a<b;
    if(a_is_minimum){
        result = a;
    }else{
        result = b;
    }

    return result;

}

double max(double a, double b){

    double result = 0;
    bool a_is_maximum = a>b;
    if(a_is_maximum){
        result = a;
    }else{
        result = b;
    }

    return result;

}


MatrixXd funds::generateWhiteNoise1(int seed,int rows, int columns){
    MatrixXd randoms(rows, columns);
    VectorXd a;


    //algorithm for generating random numbers that are seeded on changing time
    time_t now = seed;
    boost::random::mt19937 gen{static_cast<uint32_t>(now)};
    boost::normal_distribution<> nd(0.0, 1.0);
    boost::variate_generator<boost::mt19937 &,
    boost::normal_distribution<> > var_nor(gen, nd);



    //we generate the matrix of random numbers
    for (int rw = 0; rw < rows; rw++) {
        //we make sure the naturally occurring auto-correlation is sufficiently small by using a do-while loop
        //here we load each row with appropriate random numbers
        a = VectorXd(columns);
        for (int i = 0; i < columns; ++i) {
            a(i) = var_nor();
        }

        randoms.row(rw) = a;
    }
    return randoms;
}


std::map<int,order>
funds::ag_value_demand() {

    //Params definition
    map<int, order> result_;
    auto quotes = this->stocks_on_market;
    // wealth allocated to stocks
    double portfolio_alloc = 0.6;
    //trading signal for the ith stock
    double phi = 0;
    //sum of the trading signals
    double sum_of_signals = 0;
    //time variable t
    int t = int(clock->current_time());



//compute a sum of exponents of signals, see McFadden choice function...........................(1)
        for (auto &[k, v] : quotes) {
            const auto &[time, bid, ask] = v.get_price();
            //cost of capital
            double kc = 1.1;
            //daily equivalent cost of capital
            double day_kc = pow((kc),(1./252))-1;
            //daily dividend growth rate
            double g = day_kc-((v.get_dividend(1)/v.get_value(1)));
            //modified cost of capital
            double kc_modified = kc+this->bias;
            //daily equivalent modified cost of capital
            double day_kc_modified = pow((kc_modified),(1./252))-1;//base cost of capital
            //intrinsic value
            double value = v.get_value(t) * (day_kc - g)/(day_kc_modified - g);


            //current stock price
            double quoted_price_ = v.get_midprice();



            //the decision to buy a stock is inferred from the ask price
            //while the decision to sell a stock is inferred from the ask price
            if (ask <= (value)) {
                quoted_price_ = ask;
            } else if (bid >= (value)) {
                quoted_price_ = bid;
            }


            phi = -(log((quoted_price_) / (value)));//stock signal
            sum_of_signals = sum_of_signals + abs(phi);
        }




//standardize each stock's signal relative to sum described in (1)
        for (auto &[k, v] : quotes){
            //the following line searches whether a trader already holds any inventory of the stock
            // and if so, it stores the number of stocks at hand in the parameter j
            auto it = stocks_at_hand.find(k);
            double current_position = 0;
            if (stocks_at_hand.end() != it) { current_position = it->second; }
            const auto &[time, bid, ask] = v.get_price();
            double kc = 1.1;
            double day_kc = pow((kc),(1./252))-1;//base cost of capital
            double g = day_kc-((v.get_dividend(1)/v.get_value(1)));
            double kc_modified = kc+this->bias;
            double day_kc_modified = pow((kc_modified),(1./252))-1;//base cost of capital


            double value = v.get_value(t);//*(day_kc - g)/(day_kc_modified - g);
            double quoted_price_ = v.get_midprice();

            //the decision to buy a stock is inferred from the ask price
            //while the decision to sell a stock is inferred from the bid price
            if (ask <= (value)) {
                quoted_price_ = ask;
            } else if (bid >= (value)) {
                quoted_price_ = bid;
            }

            phi = -(log(quoted_price_ / value));
            double allocation = phi / sum_of_signals;


            int order_num;
            order_num = concat(t, k);

            double free_cash = 0;
            double short_positions = 0;
            double long_positions = 0;


            for (auto &[i, x]:this->stocks_at_hand) {
                if (x < 0) {
                    short_positions += -1 * (x) * this->stocks_on_market.find(i)->second.get_midprice();
                } else {
                    long_positions += (x) * this->stocks_on_market.find(i)->second.get_midprice();
                }
            }

            free_cash = max(0, (this->cash_at_hand - short_positions));



//rebalancing date
            if (t == reb_period * floor(t / reb_period)) {
                //first rebalnce the 40:60% asset allocation ratio between bonds and the portfolio of stocks
                if (bond_at_hand < ((1 - portfolio_alloc) * wealth)) {
                    double bond_purchase = min(((1 - portfolio_alloc) * wealth) - bond_at_hand, free_cash);
                    bond_at_hand += bond_purchase;
                    this->cash_at_hand -= bond_purchase;
                    free_cash = max(0, (this->cash_at_hand - short_positions));
                } else {
                    double excess_bond = bond_at_hand - ((1 - portfolio_alloc) * wealth);
                    bond_at_hand -= excess_bond;
                    this->cash_at_hand += excess_bond;
                    free_cash = max(0, (this->cash_at_hand - short_positions));
                }

                //now rebalance the stocks portfolio to reflect relative signal strength

                auto rebalancing_orders = reb_wealth(
                        order_num
                        , k
                        , quoted_price_
                        , allocation
                        , portfolio_alloc
                        , this->wealth
                        , current_position
                );

                auto free_cash_distribution1_ = free_cash_distribution(
                        order_num
                        ,k
                        ,quoted_price_
                        ,allocation
                        ,free_cash
                );


                auto predicted_price_orders = stocks_order(
                        order_num
                        , k
                        , quoted_price_
                        , value
                        , allocation
                        , portfolio_alloc
                        , this->wealth
                        , current_position
                );

                result_.merge(rebalancing_orders);
                result_.merge(free_cash_distribution1_);
                result_.merge(predicted_price_orders);

            }

                //when time is not a rebalancing date, we only allocate free cash
                // received from dividends, sales or rebalancing

            else {

//                auto cash_distribution = free_cash_distribution(order_num, k, quoted_price_, allocation, free_cash);

            auto free_cash_distribution1_ = free_cash_distribution(
                        order_num
                        ,k
                        ,quoted_price_
                        ,allocation
                        ,free_cash
                     );


                auto predicted_price_orders = stocks_order(
                        order_num, k, quoted_price_, value, allocation, portfolio_alloc, this->wealth, current_position
                );

                result_.merge(free_cash_distribution1_);
                result_.merge(predicted_price_orders);

            }
        }
        return result_;
}




std::map<int,order>
funds::ag_noise_demand(){
    //Params definition
    map<int,order> result_;
    //get stock prices
    auto quotes = this->stocks_on_market;
    // wealth allocated to stock potfolio
    double portfolio_alloc = 1;
    //sum of signals will be used to standardize signals
    double sum_of_signals = 0;
    //fro storing trading signal for stock i
    double phi = 0;
    //get time from the market to synchronize processes
    int t = int(clock->current_time());




//Ornstein Uhlenbeck params
    //noise with a half life of 6 years, to match empirical evidence
    double mean_reversion_rate = (1 - pow(0.5, 1./(6*252.0)));//1./(6*252.0)
    //Ornstein Uhlenbeck noise generation
    double  X_t ;

    //compute the noise process parameters
    for(auto &[k, v] : quotes) {

                double var = 0.;
                double mean =0.;
                double sigma =0.;

                //store computed noise for next period's process
                if (noise1.find(k) == noise1.end()) {
                    noise1.emplace(k, 0);
                }


                //setting the noise process
                sigma = 0.02;
                var = (0.5 * pow(sigma, 2)) * (1 / mean_reversion_rate) * (1 - exp(-2 * mean_reversion_rate * 1));
                mean = noise1.find(k)->second * (exp(-mean_reversion_rate * 1)) + 0 * (1 - exp(-mean_reversion_rate * 1));
                X_t = mean + sqrt(var) * dX((k - 1), t);


                //update noise term when its a new day
                if (day_count.find(k) == day_count.end()) {
                    day_count.emplace(k, 0);
                } else if (day_count.find(k)->second < t) {
                    noise1.find(k)->second = X_t;
                    day_count.find(k)->second++;
                }
    }


    //compute the signal for each stock
    for(auto &[k, v] : quotes){
        //get bid and ask prices
        const auto &[time, bid, ask] = v.get_price();
        //get noise term
        X_t = noise1.find(k)->second;
        //biased intrinsic value
        double value = 2.*v.get_value(t)/(1.+exp(-X_t));


       double quoted_price_ = v.get_midprice();
                //the decision to buy a stock is inferred from the ask price
                //while the decision to sell a stock is inferred from the ask price
                if(ask <= (value)){
                    quoted_price_ = ask;
                }else
                if(bid >= (value )){
                    quoted_price_ = bid;
                }

        //stock signal
        phi = -(log((quoted_price_)/(value )));
        //sum of signals to be used for signal standardization
        sum_of_signals = sum_of_signals + abs(phi);
    }




//compute a standardized stock's signal relative to sum described in (1)
    for(auto &[k, v] : quotes){
        //the following line searches whether a trader already holds any inventory of the stock
        // and if so, it stores the number of stocks at hand in the parameter j
        auto it = stocks_at_hand.find(k);
        double current_position = 0;
        if (stocks_at_hand.end() != it){current_position = it->second;}
        const auto &[time, bid, ask] = v.get_price();
        //get noise
        X_t =  noise1.find(k)->second;
        //calculate biased intrinsic value
        double value = 2.*v.get_value(t)/(1.+exp(-X_t));
        //get current price
        double quoted_price_ =  v.get_midprice();



        //the decision to buy a stock is inferred from the ask price
        //while the decision to sell a stock is inferred from the bid price
        if(ask<=(value )){
            quoted_price_ = ask;
        }else
        if(bid>=(value)){
            quoted_price_ = bid;
            }

        //calculate stock's trade signal
        phi = -(log((quoted_price_)/(value)));
        double allocation = phi/sum_of_signals;
        int order_num;
        order_num = concat(t, k);

        double free_cash = 0.;
        double short_positions = 0.;
        double long_positions = 0.;

        //calculate cash required to cover short positions
        //mark this cash as uninvestable to represent margin adjustment at the clearing house
        for (auto &[i, x]: this->stocks_at_hand) {
            if (x < 0) {
                short_positions += -1 * (x) * this->stocks_on_market.find(i)->second.get_midprice();
            } else {
                long_positions += (x) * this->stocks_on_market.find(i)->second.get_midprice();
            }
        }

        free_cash = max(0, (this->cash_at_hand - short_positions));

        //rebalancing date transactions
        if ( t == reb_period*floor(t/reb_period)) {
            //rebalance the stocks portfolio to put more wealth in stocks with higher signal strength
            auto rebalancing_orders = reb_wealth(
                    order_num
                    , k
                    , quoted_price_
                    , allocation
                    , portfolio_alloc
                    , this->wealth
                    , current_position
            );

            auto free_cash_distribution1_ = free_cash_distribution(
                    order_num
                    ,k
                    ,quoted_price_
                    ,allocation
                    ,free_cash
            );


            auto predicted_price_orders = stocks_order(
                    order_num
                    , k
                    , quoted_price_
                    , value
                    , allocation
                    , portfolio_alloc
                    , this->wealth
                    , current_position
            );

            result_.merge(rebalancing_orders);
            result_.merge(free_cash_distribution1_);
            result_.merge(predicted_price_orders);

        }

            //when time is not a rebalancing date, we only allocate free cash
            // received from dividends, sales or rebalancing

        else {
            auto free_cash_distribution1_ = free_cash_distribution(
                    order_num
                    ,k
                    ,quoted_price_
                    ,allocation
                    ,free_cash
            );


            auto predicted_price_orders = stocks_order(
                    order_num,
                    k,
                    quoted_price_,
                    value,
                    allocation,
                    portfolio_alloc,
                    this->wealth,
                    current_position
            );


            result_.merge(free_cash_distribution1_);
            result_.merge(predicted_price_orders);

        }
    }
    return result_;
}



std::map<int,order>
funds::ag_momentum_demand() {
//Params definition
    map<int,order> result_;
    //get quotes
    auto quotes = this->stocks_on_market;
    //allocate x% wealth to stocks
    double portfolio_alloc = 0.6;
    //sum of signals will standardize the signals
    double sum_of_signals = 0;
    //trading signal for stock i
    double phi = 0;
    //get market time to synchronize processes
    int t = int(clock->current_time());


//calculate trade signals
    for (auto &[k, v] : quotes) {
        //make sure trend follower begins trading when there is enough data to compute trend
        if (t > window_size) {
            //get stock prices
            double quoted_price_ = v.get_midprice();

            //get a vector of historical prices for the shorter term moving average
            vector<double> hist_prices_MA1 = v.get_price_range(this->window_size_MA1 - 1);
            //for the longer term moving average
            vector<double> hist_prices_MA2 = v.get_price_range(this->window_size_MA2 - 1);


            //calculate the average price over the historical prices
            double MA1 = std::accumulate(hist_prices_MA1.begin(), hist_prices_MA1.end(), 0.);
            double MA2 = std::accumulate(hist_prices_MA2.begin(), hist_prices_MA2.end(), 0.);

            double trend1 = (MA1 + (quoted_price_)) / this->window_size_MA1;
            double trend2 = (MA2 + (quoted_price_)) / this->window_size_MA2;

            //stock signal
            if (MA2> 0.) { phi =  (log(trend1 / trend2)); }
            else { phi = 0.; }

            //sum of signals to be used for standardizing signals
            sum_of_signals = sum_of_signals + abs(phi);
        }
    }



//compute a standardized stock signal relative to sum described above
    for (auto &[k, v] : quotes) {
        //get information about current positions at hand
        double current_position = 0;
        auto i = stocks_at_hand.find(k);
        if (stocks_at_hand.end() != i) { current_position = i->second; }

        //get current price
        double quoted_price_ = v.get_midprice();


        //get a vector of historical prices for the shorter term moving average
        vector<double> hist_prices_MA1 = v.get_price_range(this->window_size_MA1 - 1);
        //for the longer term moving average
        vector<double> hist_prices_MA2 = v.get_price_range(this->window_size_MA2 - 1);

        //calculate the average price over the historical prices
        double MA1 = std::accumulate(hist_prices_MA1.begin(), hist_prices_MA1.end(), 0.);
        double MA2 = std::accumulate(hist_prices_MA2.begin(), hist_prices_MA2.end(), 0.);

        double trend1 = (MA1 + (quoted_price_)) / this->window_size_MA1;
        double trend2 = (MA2 + (quoted_price_)) / this->window_size_MA2;

        //trade signal
        if (MA2 > 0.) { phi = (log(trend1 / trend2)); }
        else { phi = 0.; }//stock signal


        //z-score wealth allocation
        double allocation = (phi/sum_of_signals);// = phi / sum_of_signals;
        const auto &[time, bid,ask] = v.get_price();


      
        //only change signal if its a rebalancing date
        if ( t == reb_period*floor(t/reb_period)) {
            bool is_empty = this->signal.find(k)== this->signal.end();
            if(is_empty){
                this->signal.emplace(k,allocation);
            }else{
                this->signal.find(k)->second = allocation;
            }

        }


        //investment in the subject stock
        double excess_demand_market;
        order market_order;
        order limit_order;
        auto order_num = concat(t,k);

        double free_cash = 0;
        double short_positions = 0;
        double long_positions = 0;
        double investable_wealth = 0;

        //put aside enough cash to cover margin for short positions
        for(auto &[i,x]:this->stocks_at_hand){
            if(x<0){
                short_positions+= -1 * (x) * this->stocks_on_market.find(i)->second.get_midprice();
            } else{
                long_positions += (x) * this->stocks_on_market.find(i)->second.get_midprice();
            }
        }

        free_cash =max(0, (this->cash_at_hand - short_positions));




        //rebalancing transactions
        if ( t == reb_period*floor(t/reb_period)) {
            //first rebalance the 40:60% asset allocation ratio between bonds and the portfolio of stocks
            if(bond_at_hand < ((1-portfolio_alloc)*wealth)){
                double bond_purchase = min(((1-portfolio_alloc)*wealth)-bond_at_hand, free_cash);
                bond_at_hand += bond_purchase;
                this->cash_at_hand -= bond_purchase;
                free_cash -= bond_purchase;
            }else {
                double excess_bond = bond_at_hand - ((1-portfolio_alloc)*wealth);
                bond_at_hand -= excess_bond;
                this->cash_at_hand += excess_bond;
                free_cash = max(0, (this->cash_at_hand - short_positions));
            }


            //rebalance the stocks portfolio to realign portfolio with trade signals
            //sell excess stocks
            auto rebalancing_orders = reb_wealth(
                    order_num
                    , k
                    , quoted_price_
                    , allocation
                    , portfolio_alloc
                    , this->wealth
                    , current_position
            );

            //distribute free cash
            auto free_cash_distribution1_ = free_cash_distribution(
                    order_num
                    ,k
                    ,quoted_price_
                    ,allocation
                    ,free_cash
            );
        }

        else {
            //distribute free cash
            auto free_cash_distribution1_ = free_cash_distribution(
                    order_num
                    ,k
                    ,quoted_price_
                    ,allocation
                    ,free_cash
            );


            result_.merge(free_cash_distribution1_);

        }
    }
    return result_;
}


std::map<int, order> funds::ag_index_demand(){
                //Params definition
                map<int,order> result_;
                //get market quotes
                auto quotes = this->stocks_on_market;
                // wealth allocated to stock potfolio
                double portfolio_alloc = 1.;
                //sum of signals used to standardize trade signals
                double sum_of_signals = 0.;
                //set maximum number of stocks included in the index
                int stocks_in_index = 2;
                //trading signal for stock i based on market capitalization
                double phi = 0.;
                //get time from market clock to synchronise trader with the rest of the market
                int t = int(clock->current_time());
                //target weights
                map<int,double> target_allocation = {};



    //push all quotes in a single vector to rank them
    vector<double> top_quotes = {};
    for(auto &[k, v] : quotes){
        top_quotes.push_back(v.get_midprice());
    }



    // arrange quotes in descending order
    sort(top_quotes.begin(), top_quotes.end(), greater<double>());
    //determine the threshold/boundary quote
    double marker_quote = top_quotes.at(stocks_in_index-1);
    
    
    
//compute standardized/z-scored trade signals
    for(auto &[k, v] : quotes){
        //only quotes greater or equal to the marker quote are included in the index
        if(v.get_midprice()>=marker_quote) {
            double quoted_price_ = v.get_midprice();
            //calculate market capitalization as the product of outstanding shares and market price
            double shares_outstanding = v.get_shares_outstanding();
            //stock signal
            phi = shares_outstanding * quoted_price_;
            sum_of_signals = sum_of_signals + abs(phi);
        }
    }

//z-scores
    for(auto &[k, v] : quotes){
        //get quotes
        const auto &[time, bid, ask] = v.get_price();
        //market capitalization
        auto shares_outstanding = v.get_shares_outstanding();
        double quoted_price_ = v.get_midprice();


        phi = shares_outstanding * quoted_price_;
        //final stndardized/z-scored signal
        double allocation = (phi/sum_of_signals);// = phi / sum_of_signals;
        //verify index allocation is only using index constituencies
        if(v.get_midprice()<marker_quote) {
            allocation = 0.;
        }

        //place the target weights in a vector
        if(target_allocation.find(k)!=target_allocation.end()){
            target_allocation.find(k)->second = allocation; 
        }else
        {
            target_allocation.emplace(k,allocation);
        }
    }


//execute trades
    for(auto &[k, v] : quotes) {

        //current holdings
        auto i = stocks_at_hand.find(k);
        double current_position = 0;
        if (stocks_at_hand.end() != i) { current_position = i->second; }
        double quoted_price_ = v.get_midprice();


        //signal used for trading is the one determined at the previous most recent rebalancing date
        if ( t == reb_period*floor(t/reb_period)) {
            bool is_empty = this->signal.find(k) == this->signal.end();
            if(is_empty){
                this->signal.emplace(k,target_allocation.find(k)->second);
            }else{
                this->signal.find(k)->second = target_allocation.find(k)->second;
            }

        }


//free cash NB:the index investor does not short-sell any stocks
        auto order_num = concat(t,k);
        double free_cash = 0.;
        free_cash = max(0, (this->cash_at_hand));


//rebalancing transactions
        if ( t == reb_period*floor(t/reb_period)) {
            //rebalance the stocks portfolio to realign portfolio with trade signals
            //sell excess stocks
            auto rebalancing_orders = reb_wealth(
                    order_num
                    , k
                    , quoted_price_
                    , target_allocation.find(k)->second
                    , portfolio_alloc
                    , this->wealth
                    , current_position
            );

            //distribute excess cash
            auto free_cash_distribution1_ = free_cash_distribution(
                    order_num
                    ,k
                    ,quoted_price_
                    ,target_allocation.find(k)->second
                    ,free_cash
            );
        }

        else {
            //distribute excess cash
            auto free_cash_distribution1_ = free_cash_distribution(
                    order_num
                    ,k
                    ,quoted_price_
                    ,target_allocation.find(k)->second
                    ,free_cash
            );
            result_.merge(free_cash_distribution1_);

        }
    }
    return result_;
}

std::map<int,order>
funds::ag_growth_demand(){
    //Params definition
    map<int,order> result_;
    auto quotes = this->stocks_on_market;
    //double Beta = 14;//dummy threshold TODO: link this variable with actual value threshold
    double portfolio_alloc = 0.6;// wealth allocated to stock potfolio
    double sum_of_signals = 0;;
    double phi = 0; //trading signal for stock i
    double

    limit_order_proportion =0.;
    auto t = clock->current_time();





//compute a sum of exponents of signals, see McFadden choice function...........................(1)
    for(auto &[k, v] : quotes){
        const auto &[time, bid,ask] = v.get_price();
        auto quoted_price_ = (bid + ask)/2.;


        vector<double> value = v.get_intrinsic_value();

        double MA1 = std::accumulate( value.begin() + (t-(value_window-252)),  value.begin() + (t), 0.)/(value_window-252);
        double MA2 = std::accumulate(  value.begin() + (t-(value_window)),  value.begin() + (t), 0.)
                     /(value_window);


        double V = MA1;
        double V_o = MA2;
        phi = (V/V_o)+log(V/V_o) - 1;//stock signal
        sum_of_signals = sum_of_signals + abs(phi);
    }



//compute the ratio of each stock's signal relative to sum described in (1)
    for(auto &[k, v] : quotes) {
        //the following line searches whether a trader already holds inventory of the stock in subject
        // and stores the value in j
        auto i = stocks_at_hand.find(k);
        double current_position = 0;
        if (stocks_at_hand.end() != i) { current_position = i->second; }
        const auto &[time, bid, ask] = v.get_price();
        auto quoted_price_ = (bid + ask) / 2.;


        vector<double> value = v.get_intrinsic_value();

        double MA1 = std::accumulate( value.begin() + (t-value_window),  value.begin() + (t), 0.)/value_window;
        double MA2 = std::accumulate(  value.begin() + (t-(value_window+252)),  value.begin() + (t), 0.)
                     /(value_window+252);


        double V = MA1;
        double V_o = MA2;
        phi = (V/V_o)+log(V/V_o) - 1; //stock signal
        double allocation = (phi/sum_of_signals);// = phi / sum_of_signals; //stock wealth allocation

        //        if(allocation > 0){
//            quoted_price_ = ask;
//        }else{
//            quoted_price_ = bid;
//      }



        if ( t == reb_period*floor(t/reb_period)) {
            auto is_empty = this->signal.find(k)== this->signal.end();
            if(is_empty){
                this->signal.emplace(k,allocation);
            }else{
                allocation = allocation;
            }

        }
//investment in the subject stock
        double excess_demand_market;
        double excess_demand_limit;
        order market_order;
        order limit_order;


        int order_num;
        order_num = concat(t,k);
        double free_cash = 0;
        double short_positions = 0;
        double long_positions = 0;
        double investable_wealth = 0;

        for(auto &[i,x]:this->stocks_at_hand){
            if(x<0){
                short_positions+= -1 * (x) * this->stocks_on_market.find(i)->second.get_midprice();
            } else{
                long_positions += (x) * this->stocks_on_market.find(i)->second.get_midprice();
            }
        }

        free_cash =max(0, (this->cash_at_hand - short_positions));


//rebalancing date


        if ( t == reb_period*floor(t/reb_period)) {


            if(bond_at_hand < ((1-portfolio_alloc)*wealth)){
                double bond_purchase = min(((1-portfolio_alloc)*wealth)-bond_at_hand, free_cash);
                bond_at_hand += bond_purchase;
                this->cash_at_hand -= bond_purchase;
                free_cash -= bond_purchase;
            }else {
                double excess_bond = bond_at_hand - ((1-portfolio_alloc)*wealth);
                bond_at_hand -= excess_bond;
                this->cash_at_hand += excess_bond;
                free_cash = max(0, (this->cash_at_hand - short_positions));
            }


            auto T = free_cash * this->signal.find(k)->second*portfolio_alloc  / (quoted_price_);
            auto C = current_position;

            if(T!=0){
                order_num = concat(1, order_num);
                market_order.set_id(this->get_identifier());
                market_order.set_order_type(order::market);
                market_order.set_ordered_asset(k);
                market_order.set_order_size((1-limit_order_proportion)*T, quoted_price_);
                market_order.set_status(order::active);
                result_.emplace(order_num, market_order);

                order_num = concat(2, order_num);
                market_order.set_id(this->get_identifier());
                market_order.set_order_type(order::limit);
                market_order.set_ordered_asset(k);
                market_order.set_order_size((limit_order_proportion)*T, quoted_price_);
                market_order.set_status(order::active);
                result_.emplace(order_num, market_order);
            }


            if(abs(C+allocation)<=abs(C)) {
                order_num = concat(3, order_num);
                limit_order.set_id(this->get_identifier());
                limit_order.set_order_type(order::market);
                limit_order.set_ordered_asset(k);
                limit_order.set_order_size((1-limit_order_proportion)*-C, quoted_price_);
                limit_order.set_status(order::active);
                result_.emplace(order_num, limit_order);

                order_num = concat(4, order_num);
                limit_order.set_id(this->get_identifier());
                limit_order.set_order_type(order::limit);
                limit_order.set_ordered_asset(k);
                limit_order.set_order_size((limit_order_proportion)*-C, quoted_price_);
                limit_order.set_status(order::active);
                result_.emplace(order_num, limit_order);


            }

        }

            //when time is not a rebalancing date, adjust positions using excess cash (e.g. from dividends)
        else {

            auto T = free_cash * this->signal.find(k)->second  *portfolio_alloc  / (quoted_price_);
            auto C = current_position;

            if(T!=0){
                order_num = concat(1, order_num);
                market_order.set_id(this->get_identifier());
                market_order.set_order_type(order::market);
                market_order.set_ordered_asset(k);
                market_order.set_order_size((1-limit_order_proportion)*T, quoted_price_);
                market_order.set_status(order::active);
                result_.emplace(order_num, market_order);

                order_num = concat(2, order_num);
                market_order.set_id(this->get_identifier());
                market_order.set_order_type(order::limit);
                market_order.set_ordered_asset(k);
                market_order.set_order_size((limit_order_proportion)*T, quoted_price_);
                market_order.set_status(order::active);
                result_.emplace(order_num, market_order);
            }


        }
    }
    return result_;
}





