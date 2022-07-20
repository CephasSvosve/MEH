//
// Created by Cephas Svosve on 20/12/2021.
//
#include "identity.h"
#include <iostream>

int create_identity( identity_type a){
    int i;
    try {
        switch(a){

            case 0:

                i = companies.size();
                companies.emplace(i, company_);
                break;
            case 1:
                i = assets.size();
                companies.emplace(i, asset);
                break;
            case 2:
                i = traders.size();
                companies.emplace(i, trader);
                break;
            case 3:
                i = fund_traders.size();
                companies.emplace(i, fund_trader);
                break;
            case 4:
                i = pricers.size();
                companies.emplace(i, pricer);
                break;
            case 5:
                i = trade_strategies.size();
                companies.emplace(i, trader);
                break;
            default:
                break;

        }
        return i;
    }catch (...) {
        std::cout<<"failed to create identity"<<std::endl;
    }

}
