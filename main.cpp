/* 
 * File:   main.cpp
 * Author: Kapil Thakkar
 *
 */

#include <cstdlib>

#include "SessionOrganizer.h"
#include <time.h>

using namespace std;

/*
 * 
 */
int main ( int argc, char** argv )
{
    // Parse the input.
    if ( argc < 3 )
    {   
        cout<<"Missing arguments\n";
        cout<<"Correct format : \n";
        cout << "./main <input_filename> <output_filename>";
        exit ( 0 );
    }
    string inputfilename ( argv[1] );
    
    // Initialize the conference organizer.
    SessionOrganizer *organizer  = new SessionOrganizer( inputfilename );

    // Organize the papers into tracks based on similarity.
    
    organizer->organizePapers ( );
    //organizer->newRandomState();


    organizer->printSessionOrganiser ( argv[2]);

    // Score the organization against the gold standard.
    double score = organizer->scoreOrganization ( );

    double processingTimeInMinutes=60*(organizer->getProcessingTimeInMinutes());
    float duration;
    clock_t start_time = clock();

    int zeros=0;
    int flag=0;
    int uu=0;

    do{
        double incr = organizer->newState();
        //organizer->printIt();
        

        if(incr==0.0){
            zeros++;
        }

        if(zeros>30){

            //organizer->printIt();
            if(flag==0){
                organizer->changeBest();
                flag=1;
            }
            double tempgood = organizer->printGoodness();
            double bestgood = organizer->printBestGoodness();
            if(tempgood>bestgood){
                organizer->changeBest();
            }
            organizer->newRandomState();
            double score = organizer->scoreOrganization ( );
            zeros=0;
        }


        duration=((float)(clock()-start_time))/ CLOCKS_PER_SEC;
        uu++;

    }while(duration<=(processingTimeInMinutes-1.5));
    //}while(uu<1000);

    double gg = organizer->printGoodness();
    double bg = organizer->printBestGoodness();
    //double tt = organizer->scoreOrganization();

     if(gg>bg){
       organizer->printSessionOrganiser ( argv[2]);
     }else{
        organizer->printBestSessionOrganiser ( argv[2]);
     }

    //cout<< "score:"<<score<<endl;
    //cout<< "new score:"<<bg<<"    "<<gg<<"    "<<tt<<endl;

    return 0;
}

