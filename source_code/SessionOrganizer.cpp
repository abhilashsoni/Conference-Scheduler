/* 
 * File:   SessionOrganizer.cpp
 * Author: Kapil Thakkar
 * 
 */

#include "SessionOrganizer.h"
#include "Util.h"
extern int t;

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
}

double SessionOrganizer::getProcesingTime()
{
    return processingTimeInMinutes;
}

void SessionOrganizer::organizePapers ( )
{
    int paperCounter = 0;
    for ( int i = 0; i < conference->getSessionsInTrack ( ); i++ ) //no of time slots
    {
        for ( int j = 0; j < conference->getParallelTracks ( ); j++ )   //no of parallel sessions
        {
            for ( int k = 0; k < conference->getPapersInSession ( ); k++ )
            {
                conference->setPaper ( j, i, k, paperCounter );
                paperCounter++;
            }
        }
    }
    //cout<<conference->getSessionsInTrack ( )<<"\n"<<conference->getParallelTracks ( )<<"\n"<<conference->getPapersInSession ( )<<"\n";

    int size=2;
    int session_array[size];
    int track_array[size];
    int paper_array[size];
    int total_session = conference->getSessionsInTrack ( );
    int total_tracks = conference->getParallelTracks ( );
    int total_papers = conference->getPapersInSession ( ); 
    //cout<<total_session<<total_tracks<<total_papers<<"\n";

    while(time(0)-t<getProcesingTime()*60-3)
    {
        for ( int i = 0; i < conference->getSessionsInTrack ( ); i++ ) //no of time slots
        {
            for(int i = 0; i<size; i++)
            {
                session_array[i] = rand()%total_session;
                track_array[i] = rand()%total_tracks;
                paper_array[i] = rand()%total_papers;
            }
        }
        double current_score = scoreOrganization();
        if(track_array[0]!=track_array[1] || session_array[0]!=session_array[1])
        {
            Track tmpTrack1 = conference->getTrack ( track_array[0] );
            Track tmpTrack2 = conference->getTrack ( track_array[1] );
            Session tmpSession1 = tmpTrack1.getSession ( session_array[0] ); 
            Session tmpSession2 = tmpTrack2.getSession ( session_array[1] );
            int paperIndex1 = tmpSession1.getPaper(paper_array[0]);
            int paperIndex2 = tmpSession2.getPaper(paper_array[1]);

            /*
            conference->setPaper(track_array[0] ,session_array[0], paper_array[0], paperIndex2);
            conference->setPaper(track_array[1] ,session_array[1], paper_array[1], paperIndex1);

            double new_score = scoreOrganization();

            if(new_score<current_score)
            {

                conference->setPaper(track_array[0] ,session_array[0], paper_array[0], paperIndex1);
                conference->setPaper(track_array[1] ,session_array[1], paper_array[1], paperIndex2);
            }
            else cout<<current_score<<"\t"<<new_score<<"\n";*/

            

            double new_score = current_score;
            for(int p = 0; p<tmpSession1.getNumberOfPapers() ;p++)
            {
                int tempPaperIndex = tmpSession1.getPaper(p);
                if(tempPaperIndex!=paperIndex1) {new_score = new_score+distanceMatrix[tempPaperIndex][paperIndex1]-1.0;
                new_score = new_score+1.0-double(distanceMatrix[tempPaperIndex][paperIndex2]);}
            }
            for(int p = 0; p<tmpSession2.getNumberOfPapers();p++)
            {
                int tempPaperIndex = tmpSession2.getPaper(p);
                if(tempPaperIndex!=paperIndex2) {new_score = new_score+distanceMatrix[tempPaperIndex][paperIndex2]-1.0;
                new_score = new_score+1.0-distanceMatrix[tempPaperIndex][paperIndex1];}
            }            
            for(int p=0; p<conference->getParallelTracks() ;p++)
            {
                if(p!=track_array[0])
                {
                    Track tempTrack=conference->getTrack(p);
                    Session tempSession = tempTrack.getSession(session_array[0]);
                    for(int l=0; l<tempSession.getNumberOfPapers(); l++)
                    {
                        int tempPaperIndex = tempSession.getPaper(l);
                        new_score = new_score+tradeoffCoefficient*distanceMatrix[tempPaperIndex][paperIndex2];
                        new_score = new_score-tradeoffCoefficient*distanceMatrix[tempPaperIndex][paperIndex1];
                    }
                }
            }
            for(int p=0; p<conference->getParallelTracks() ;p++)
            {
                if(p!=track_array[1])
                {
                    Track tempTrack=conference->getTrack(p);
                    Session tempSession = tempTrack.getSession(session_array[1]);
                    for(int l=0; l<tempSession.getNumberOfPapers(); l++)
                    {
                        int tempPaperIndex = tempSession.getPaper(l);
                        new_score = new_score+tradeoffCoefficient*distanceMatrix[tempPaperIndex][paperIndex1];
                        new_score = new_score-tradeoffCoefficient*distanceMatrix[tempPaperIndex][paperIndex2];
                    }
                }
            }
            if(new_score>current_score)
            {
                //cout<<current_score<<"\t"<<new_score<<"\n";
                conference->setPaper(track_array[0] ,session_array[0], paper_array[0], paperIndex2);
                conference->setPaper(track_array[1] ,session_array[1], paper_array[1], paperIndex1);
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
    parallelTracks = atoi ( lines[2].c_str () );
    sessionsInTrack = atoi ( lines[3].c_str () );
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

void SessionOrganizer::printSessionOrganiser ( char * filename)
{
    conference->printConference ( filename);
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
                for ( int l = i + 1; l < conference->getParallelTracks ( ); l++ )
                {
                    Track tmpTrack2 = conference->getTrack ( l );
                    Session tmpSession2 = tmpTrack2.getSession ( j );
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
    return score;
}
