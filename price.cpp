////
//// Created by Cephas Svosve on 20/12/2021.
////
//#include "price.h"
//#include <iostream>
//
////define addition of two prices
//price price::operator+ (const price& p){
//    try{
//        this->currency = this->currency;
//        this->value = this->value + p.value;
//
//        if(this->value >= 0){return *this;}
//        else{throw "negative pricing";}
//    }catch(...){
//        std::cout<<"negative pricing"<<std::endl;}
//}
//
//price price::operator+= (const price& p){
//    try{
//        this->currency = this->currency;
//        this->value += p.value;
//
//        if(this->value >= 0){return *this;}
//        else{throw "negative pricing";}
//    }catch(...){
//        std::cout<<"negative pricing"<<std::endl;}
//}
//
//
//
////overload subtraction of two prices
//price price::operator- (const price& p){
//    try{
//        this->currency = this->currency;
//        this->value = this->value - p.value;
//
//        if(this->value >= 0){return *this;}
//        else{throw "negative pricing";}
//    }catch(...){
//        std::cout<<"negative pricing"<<std::endl;}
//}
//
//
//price price::operator-= (const price& p){
//    try{
//
//        this->currency = this->currency;
//        this->value  -= p.value;
//
//        if(this->value >= 0){return *this;}
//        else{throw "negative pricing";}
//    }catch(...){
//        std::cout<<"negative pricing"<<std::endl;}
//}
//
//
//
//price price::operator*= (const price& p){
//    try{
//        this->currency = this->currency + this->currency ;
//        this->value *= p.value;
//
//        if(this->value >= 0){return *this;}
//        else{throw "negative pricing";}
//    }catch(...){std::cout<<"negative pricing"<<std::endl;}
//}
//
//
////define division of two prices
//price price::operator/ (const price& p){
//
//    try{
//        this->currency = "";
//        this->value = this->value / p.value;
//
//        if(this->value >= 0){return *this;}
//        else{throw "negative pricing";}
//    }catch(...){std::cout<<"negative pricing"<<std::endl;}
//}
//
//
//price price::operator/= (const price& p){
//
//    try{
//        this->currency = "";
//        this->value /= p.value;
//
//        if(this->value >= 0){return *this;}
//        else{throw "negative pricing";}
//    }catch(...){std::cout<<"negative pricing"<<std::endl;}
//}
//
////define addition of a price with a double
//price price::operator+ (const double& p){
//    throw "price added with non-price variable";
//}
//
//price price::operator+= (const double& p){
//    throw "price added with non-price variable";
//}
//
////define subtraction of a price from a double
//price price::operator- (const double& p){
//    throw "price subtracted with non-price variable";
//}
//
////define multiplication of a price with a double
//price price::operator* (double &p){
//    try{
//
//        this->currency = this->currency;
//        this->value = p* this->value ;
//
//        if(this->value >= 0){return *this;}
//        else{throw "negative pricing";}
//    }catch(...){std::cout<<"negative pricing"<<std::endl;}
//}
//
////define division of a price with a double
//price price::operator/ (const double& p){
//    try{
//        this->currency = this->currency;
//        this->value = this->value / p;
//
//        if(this->value >= 0){return *this;}
//        else{throw "negative pricing";}
//    }catch(...){std::cout<<"negative pricing"<<std::endl;}
//}
//
//
