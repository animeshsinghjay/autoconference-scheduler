/* 
 * File:   Conference.cpp
 * Author: Kapil Thakkar
 * 
 */

#include "Conference.h"

Conference::Conference ( )
{
    this->parallelTracks = 0;
    this->sessionsInTrack = 0;
    this->papersInSession = 0;
    this->goodness = 0.0;
}

Conference::Conference ( int parallelTracks, int sessionsInTrack, int papersInSession )
{
    this->parallelTracks = parallelTracks;
    this->sessionsInTrack = sessionsInTrack;
    this->papersInSession = papersInSession;
    this->goodness = 0.0;
    initTracks ( parallelTracks, sessionsInTrack, papersInSession );
}

void Conference::initTracks ( int parallelTracks, int sessionsInTrack, int papersInSession )
{
    tracks = ( Track * ) malloc ( sizeof (Track ) * parallelTracks );
    for ( int i = 0; i < parallelTracks; i++ )
    {
        Track tempTrack ( sessionsInTrack );
        for ( int j = 0; j < sessionsInTrack; j++ )
        {
            Session tempSession ( papersInSession );
            tempTrack.setSession ( j, tempSession );
        }
        tracks[i] = tempTrack;
    }
}

int Conference::getParallelTracks ( )
{
    return parallelTracks;
}

int Conference::getSessionsInTrack ( )
{
    return sessionsInTrack;
}

int Conference::getPapersInSession ( )
{
    return papersInSession;
}

double Conference::getGoodness ( )
{
    return goodness;
}

Track Conference::getTrack ( int index )
{
    if ( index < parallelTracks )
    {
        return tracks[index];
    }
    else
    {
        cout << "Index out of bound - Conference::getTrack" << endl;
        exit ( 0 );
    }
}

void Conference::setPaper ( int trackIndex, int sessionIndex, int paperIndex, int paperId )
{
    if ( this->parallelTracks > trackIndex )
    {
        Track curTrack = tracks[trackIndex];
        curTrack.setPaper ( sessionIndex, paperIndex, paperId );
    }
    else
    {
        cout << "Index out of bound - Conference::setPaper" << endl;
        exit ( 0 );
    }
}

void Conference::setNewGoodness ( double g )
{
    this->goodness = g;
}

void Conference::setGoodness ( double g )
{
    this->goodness += g;
}

void Conference::printConference (char * filename )
{
    ofstream ofile(filename);

    for ( int i = 0; i < parallelTracks; i++ )
    {
        for ( int j = 0; j < sessionsInTrack; j++ )
        {
            for ( int k = 0; k < papersInSession; k++ )
            {
                ofile<< tracks[i].getSession ( j ).getPaper ( k ) << " ";
            }
            if ( j != sessionsInTrack - 1 )
            {
                ofile<<"| ";
            }
        }
        ofile<<"\n";
    }
    ofile.close();
    //cout<<"Organization written to ";
    //printf("%s :)\n",filename);

}


void Conference::printAnswer ()
{
    //ofstream ofile(filename);

    for ( int i = 0; i < parallelTracks; i++ )
    {
        for ( int j = 0; j < sessionsInTrack; j++ )
        {
            for ( int k = 0; k < papersInSession; k++ )
            {
                cout<< tracks[i].getSession ( j ).getPaper ( k ) << " ";
            }
            if ( j != sessionsInTrack - 1 )
            {
                cout<<"| ";
            }
        }
        cout<<"\n";
    }
    cout<<endl;
    //ofile.close();
    //cout<<"Organization written to ";
    //printf("%s :)\n",filename);

}