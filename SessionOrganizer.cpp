/* 
 * File:   SessionOrganizer.cpp
 * Author: Kapil Thakkar
 * 
 */

#include "SessionOrganizer.h"
#include "Util.h"

SessionOrganizer::SessionOrganizer ( )
{
    parallelTracks = 0;
    papersInSession = 0;
    sessionsInTrack = 0;
    processingTimeInMinutes = 0;
    tradeoffCoefficient = 1.0;
}

SessionOrganizer::SessionOrganizer ( string filename )
{
    readInInputFile ( filename );
    conference = new Conference ( parallelTracks, sessionsInTrack, papersInSession );
    bestconference = new Conference ( parallelTracks, sessionsInTrack, papersInSession );
}

void SessionOrganizer::organizePapers ( )
{
    int paperCounter = 0;
    for ( int i = 0; i < conference->getParallelTracks ( ); i++ )
    {
        for ( int j = 0; j < conference->getSessionsInTrack ( ); j++ )
        {
            for ( int k = 0; k < conference->getPapersInSession ( ); k++ )
            {
                conference->setPaper ( i, j, k, paperCounter );
                paperCounter++;
            }
        }
    }
}

void SessionOrganizer::readInInputFile ( string filename )
{
    vector<string> lines;
    string line;
    ifstream myfile ( filename.c_str () );
    if ( myfile.is_open ( ) )
    {
        while ( getline ( myfile, line ) )
        {
            //cout<<"Line read:"<<line<<endl;
            lines.push_back ( line );
        }
        myfile.close ( );
    }
    else
    {
        cout << "Unable to open input file";
        exit ( 0 );
    }

    if ( 6 > lines.size ( ) )
    {
        cout << "Not enough information given, check format of input file";
        exit ( 0 );
    }

    processingTimeInMinutes = atof ( lines[0].c_str () );
    papersInSession = atoi ( lines[1].c_str () );
    sessionsInTrack = atoi ( lines[2].c_str () );
    parallelTracks = atoi ( lines[3].c_str () );
    tradeoffCoefficient = atof ( lines[4].c_str () );

    int n = lines.size ( ) - 5;
    double ** tempDistanceMatrix = new double*[n];
    for ( int i = 0; i < n; ++i )
    {
        tempDistanceMatrix[i] = new double[n];
    }


    for ( int i = 0; i < n; i++ )
    {
        string tempLine = lines[ i + 5 ];
        string elements[n];
        splitString ( tempLine, " ", elements, n );

        for ( int j = 0; j < n; j++ )
        {
            tempDistanceMatrix[i][j] = atof ( elements[j].c_str () );
        }
    }
    distanceMatrix = tempDistanceMatrix;

    int numberOfPapers = n;
    int slots = parallelTracks * papersInSession*sessionsInTrack;
    if ( slots != numberOfPapers )
    {
        cout << "More papers than slots available! slots:" << slots << " num papers:" << numberOfPapers << endl;
        exit ( 0 );
    }
}

double** SessionOrganizer::getDistanceMatrix ( )
{
    return distanceMatrix;
}

double SessionOrganizer::getProcessingTimeInMinutes ( )
{
    return processingTimeInMinutes;
}

void SessionOrganizer::printSessionOrganiser ( char * filename)
{
    conference->printConference ( filename);
}

double SessionOrganizer::printGoodness ( )
{
    return conference->getGoodness();
}

void SessionOrganizer::printBestSessionOrganiser ( char * filename)
{
    bestconference->printConference ( filename);
}

double SessionOrganizer::printBestGoodness ( )
{
    return bestconference->getGoodness();
}

void SessionOrganizer::changeBest ()
{
    double gn = conference->getGoodness();
    bestconference->setNewGoodness(gn);

    for ( int i = 0; i < conference->getParallelTracks ( ); i++ )
    {
        Track tmpTrack = conference->getTrack ( i );
        for ( int j = 0; j < conference->getSessionsInTrack ( ); j++ )
        {
            Session tmpSession = tmpTrack.getSession ( j );
            for ( int k = 0; k < conference->getPapersInSession ( ); k++ )
            {
                int index = tmpSession.getPaper ( k );
                bestconference->setPaper ( i, j, k, index );
            }
        }
    }
}

double SessionOrganizer::scoreOrganization ( )
{
    // Sum of pairwise similarities per session.
    double score1 = 0.0;
    for ( int i = 0; i < conference->getParallelTracks ( ); i++ )
    {
        Track tmpTrack = conference->getTrack ( i );
        for ( int j = 0; j < tmpTrack.getNumberOfSessions ( ); j++ )
        {
            Session tmpSession = tmpTrack.getSession ( j );
            for ( int k = 0; k < tmpSession.getNumberOfPapers ( ); k++ )
            {
                int index1 = tmpSession.getPaper ( k );
                for ( int l = k + 1; l < tmpSession.getNumberOfPapers ( ); l++ )
                {
                    int index2 = tmpSession.getPaper ( l );
                    score1 += 1 - distanceMatrix[index1][index2];
                }
            }
        }
    }

    // Sum of distances for competing papers.
    double score2 = 0.0;
    for ( int i = 0; i < conference->getParallelTracks ( ); i++ )
    {
        Track tmpTrack1 = conference->getTrack ( i );
        for ( int j = 0; j < tmpTrack1.getNumberOfSessions ( ); j++ )
        {
            Session tmpSession1 = tmpTrack1.getSession ( j );
            for ( int k = 0; k < tmpSession1.getNumberOfPapers ( ); k++ )
            {
                int index1 = tmpSession1.getPaper ( k );

                // Get competing papers.
                // for ( int l = i + 1; l < conference->getParallelTracks ( ); l++ )
                // {
                //     Track tmpTrack2 = conference->getTrack ( l );
                //     Session tmpSession2 = tmpTrack2.getSession ( j );
                //     for ( int m = 0; m < tmpSession2.getNumberOfPapers ( ); m++ )
                //     {
                //         int index2 = tmpSession2.getPaper ( m );
                //         score2 += distanceMatrix[index1][index2];
                //     }
                // }

                for ( int l = j + 1; l < tmpTrack1.getNumberOfSessions ( ); l++ )
                {
                    Session tmpSession2 = tmpTrack1.getSession ( l );
                    for ( int m = 0; m < tmpSession2.getNumberOfPapers ( ); m++ )
                    {
                        int index2 = tmpSession2.getPaper ( m );
                        score2 += distanceMatrix[index1][index2];
                    }
                }

            }
        }
    }
    double score = score1 + tradeoffCoefficient*score2;
    conference->setNewGoodness(score);
    return score;
}


double SessionOrganizer::newState ( )
{
    int maxa=0;
    int maxb=0;
    int maxTracka=0;
    int maxSessiona=0;
    int maxTrackb=0;
    int maxSessionb=0;
    int maxPaperb=0;
    double maxGoodness=0.0;
    int flag=0;

    for ( int i = 0; i < conference->getParallelTracks ( ); i++ )
    {
        Track tmpTrack = conference->getTrack ( i );
        for ( int j = 0; j < tmpTrack.getNumberOfSessions ( ); j++ )
        {
            Session tmpSession = tmpTrack.getSession ( j );
            int a = tmpSession.getPaper(0);
            
            for ( int t = 0; t < conference->getParallelTracks ( ); t++ )
            {
                Track tmpTrack2 = conference->getTrack ( t );
                for ( int s = 0; s < tmpTrack2.getNumberOfSessions ( ); s++ )
                {
                    
                    if(i==t && j==s){
                        continue;
                    }

                    Session tmpSession2 = tmpTrack2.getSession ( s );

                    for ( int k = 0; k < tmpSession2.getNumberOfPapers ( ); k++ )
                    {
                        int b = tmpSession2.getPaper ( k );
                        //calculate new cost after interchanging a and b
                        double newCost = calculateNewCost(i,j,0,a,t,s,k,b);
                        if(newCost>maxGoodness){
                            flag=1;
                            maxa=a;
                            maxb=b;
                            maxGoodness=newCost;
                            maxTracka=i;
                            maxSessiona=j;
                            maxTrackb=t;
                            maxSessionb=s;
                            maxPaperb=k;
                        }

                    }

                }
            
            }

        }

    }

    //cout<<maxGoodness<<endl;
    if(flag==1){
        conference->setPaper ( maxTracka, maxSessiona, 0, maxb );
        conference->setPaper ( maxTrackb, maxSessionb, maxPaperb, maxa );    
        conference->setGoodness (maxGoodness);
    }
    
    return maxGoodness;

}


double SessionOrganizer::calculateNewCost (int tracka, int sessiona, int papera, int ida, int trackb, int sessionb, int paperb, int idb)
{ 
    double scoreAdd1 = 0.0;
    double scoreSub1 = 0.0;
    double scoreAdd2 = 0.0;
    double scoreSub2 = 0.0;

    Track tmpTrack1 = conference->getTrack ( tracka );
    Session tmpSession1 = tmpTrack1.getSession ( sessiona );
    int index0 = ida;
    int index1 = idb;
    for ( int l = 0; l < tmpSession1.getNumberOfPapers ( ); l++ )
    {
        if(l==papera){
            continue;
        }

        int index2 = tmpSession1.getPaper ( l );
        scoreSub1 -= 1 - distanceMatrix[index0][index2];
        scoreAdd1 += 1 - distanceMatrix[index1][index2];
    }

    Track tmpTrack2 = conference->getTrack ( trackb );
    Session tmpSession2 = tmpTrack2.getSession ( sessionb );
    index0 = idb;
    index1 = ida;
    for ( int l = 0; l < tmpSession2.getNumberOfPapers ( ); l++ )
    {
        if(l==paperb){
            continue;
        }

        int index2 = tmpSession2.getPaper ( l );
        scoreSub1 -= 1 - distanceMatrix[index0][index2];
        scoreAdd1 += 1 - distanceMatrix[index1][index2];
    }

    tmpTrack1 = conference->getTrack ( tracka );
    index0 = ida;
    index1 = idb;
    for ( int j = 0; j < tmpTrack1.getNumberOfSessions ( ); j++ )
    {
        if(j==sessiona){
            continue;
        }

        Session tmpSession = tmpTrack1.getSession ( j );
        for ( int k = 0; k < tmpSession.getNumberOfPapers ( ); k++ )
        {
            int index2 = tmpSession.getPaper ( k );
            scoreSub2 -= distanceMatrix[index0][index2];
            scoreAdd2 += distanceMatrix[index1][index2];
        }
    }

    tmpTrack2 = conference->getTrack ( trackb );
    index0 = idb;
    index1 = ida;
    for ( int j = 0; j < tmpTrack2.getNumberOfSessions ( ); j++ )
    {
        if(j==sessionb){
            continue;
        }

        Session tmpSession = tmpTrack2.getSession ( j );
        for ( int k = 0; k < tmpSession.getNumberOfPapers ( ); k++ )
        {
            int index2 = tmpSession.getPaper ( k );
            scoreSub2 -= distanceMatrix[index0][index2];
            scoreAdd2 += distanceMatrix[index1][index2];
        }
    }


    double score = scoreAdd1 + scoreSub1 + tradeoffCoefficient*(scoreAdd2 + scoreSub2);
    return score;

}





void SessionOrganizer::newRandomState ( )
{
    int n = (conference->getSessionsInTrack())*(conference->getParallelTracks())*(conference->getPapersInSession());
    int arr[n];
    for(int i=0;i<n;i++){
        arr[i]=i;
    }
    std::random_shuffle(&arr[0],&arr[n]);

    int p = 0;
    for ( int i = 0; i < conference->getParallelTracks ( ); i++ )
    {
        for ( int j = 0; j < conference->getSessionsInTrack ( ); j++ )
        {
            for ( int k = 0; k < conference->getPapersInSession ( ); k++ )
            {
                conference->setPaper ( i, j, k, arr[p] );
                p++;
            }
        }
    }
    //conference -> printAnswer();
    
}

void SessionOrganizer::printIt()
{
    conference->printAnswer();
}


// void SessionOrganizer::newRandomState ()
// {  
//     int n;    
//     int papers[n];
//     srand(time(0));

//     n = (conference->getSessionsInTrack())*(conference->getParallelTracks())*(conference->getPapersInSession());
     
//     for (int i=0; i<n; i++) {
//         papers[i] = i;
//     }
    
//     for (int i=0; i<n; i++) {
//         int r = rand() % n;
//         int temp = papers[i]; papers[i] = papers[r]; papers[r] = temp;
//     }
    
//     int paperCounter = 0;
//     for ( int i = 0; i < conference->getSessionsInTrack ( ); i++ )
//     {
//         for ( int j = 0; j < conference->getParallelTracks ( ); j++ )
//         {
//             for ( int k = 0; k < conference->getPapersInSession ( ); k++ )
//             {
//                 conference->setPaper ( j, i, k, papers[paperCounter]);
//                 paperCounter++;
//             }
//         }
//     }
// }

