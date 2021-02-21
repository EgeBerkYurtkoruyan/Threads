//  CS_204_HW8
//
//  Created by Ege Berk Yurtkoruyan on 9.05.2020.
#include <iostream>
#include <thread>
#include <random>
#include <mutex>
#include <time.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include "HW8DynIntQueue.h"

using namespace std;

static HW8DynIntQueue QUEUE;
static int CUSTOMER_COUNT;
static int counter;
mutex coutMutex, muti;

int random_range(const int & min, const int & max){
     static mt19937 generator(time(0));
     uniform_int_distribution<int> distribution(min, max);
     return distribution(generator);
}

void new_customer(const int & min, const int & max){
    
    int id = 0;
    while(CUSTOMER_COUNT){

        QUEUE.enqueue(++id);
        coutMutex.lock();
        
        time_t tt = chrono::system_clock::to_time_t (chrono::system_clock::now());
        struct tm *ptm = localtime(&tt);
        cout << "New customer with ID " << id << " has arrived (queue size is " << QUEUE.getCurrentSize() << "): " << put_time(ptm,"%X") << endl;
        
        coutMutex.unlock();
        
        CUSTOMER_COUNT--;
        int wait = random_range(min, max);
        this_thread::sleep_for(chrono::seconds(wait));
    }
}


void cashier1(const int & min, const int & max){
    
    while(counter){
        
        muti.lock();
        
        if(!QUEUE.isEmpty()){
            int id;
            QUEUE.dequeue(id);
            muti.unlock();
            counter--;
            coutMutex.lock();
            time_t tt = chrono::system_clock::to_time_t (chrono::system_clock::now());
            struct tm *ptm = localtime(&tt);
            cout << "Cashier 1 started transaction with customer " << id << " (queue size is " << QUEUE.getCurrentSize() << "): " << put_time(ptm,"%X") << endl;
            coutMutex.unlock();
            
            int wait = random_range(min, max);
            this_thread::sleep_for(chrono::seconds(wait));
            
            coutMutex.lock();
            ptm = localtime(&tt);
            cout << "Cashier 1 finished transaction with customer " << id << " " << put_time(ptm,"%X") << endl;
            coutMutex.unlock();
        }else if(CUSTOMER_COUNT == 0){
            muti.unlock();
            break;
        }else{
            muti.unlock();
        }
    }
}

void cashier2(const int & min, const int & max ,const int & open){
    
    while(counter){
        
        muti.lock();
        
        if(QUEUE.getCurrentSize() >= open){
            int id;
            
            QUEUE.dequeue(id);
            
            muti.unlock();
            counter--;
            coutMutex.lock();
            
            time_t tt = chrono::system_clock::to_time_t (chrono::system_clock::now());
            struct tm *ptm = localtime(&tt);
            cout << "Cashier 2 started transaction with customer " << id << " (queue size is " << QUEUE.getCurrentSize() << "): " << put_time(ptm,"%X") << endl;
            coutMutex.unlock();
            
            int wait = random_range(min, max);
            this_thread::sleep_for(chrono::seconds(wait));
            
            coutMutex.lock();
            ptm = localtime(&tt);
            cout << "Cashier 2 finished transaction with customer " << id << " " << put_time(ptm,"%X") << endl;
            coutMutex.unlock();
        }else if(CUSTOMER_COUNT == 0 && QUEUE.isEmpty()){
            muti.unlock();
            break;
        }else{
            muti.unlock();
        }
    }
}




int main() {
    
    int secondCash , customerMax , customerMin , cashMax , cashMin;
    
    cout << "Please enter the total number of customers: ";
    cin >> CUSTOMER_COUNT;
    counter = CUSTOMER_COUNT;
    cout << "Please enter the number of customers waiting in the queue to open the second cashier: ";
    cin >> secondCash;
    cout << "Please enter the inter-arrival time range between two customers: " << endl
        << "Min: ";
    cin >> customerMin;
    cout << "Max: ";
    cin >> customerMax;
    cout << "Please enter the checkout time range of cashiers: " << endl
        << "Min: ";
    cin >> cashMin;
    cout << "Max: ";
    cin >> cashMax;
    
    time_t tt = chrono::system_clock::to_time_t (chrono::system_clock::now());
    struct tm *ptm = localtime(&tt);
    cout << "Simulation starts " << put_time(ptm,"%X") << endl;
    
    thread customerThread(&new_customer , customerMin , customerMax);
    thread cashier1Thread(&cashier1 , cashMin , cashMax);
    thread cashier2Thread(&cashier2 , cashMin , cashMax , secondCash);
    
    if(customerThread.joinable())
        customerThread.join();
    if(cashier1Thread.joinable())
        cashier1Thread.join();
    if(cashier2Thread.joinable())
        cashier2Thread.join();
    
    ptm = localtime(&tt);
    cout << "End of the simulation ends: " << put_time(ptm,"%X") << endl;
    return 0;
}
