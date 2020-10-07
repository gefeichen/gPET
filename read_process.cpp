//
// Created by cgf on 9/27/20.
//
#include <fstream>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <algorithm>
#include <cstring>

using namespace std;

typedef struct Event
{
    int parn,pann,modn,cryn,siten,eventid;
    double t;
    float E, x,y,z;
    int RP, CP, RC, CC;
} Event;

typedef struct Coincidence
{
    int eventid1;
    double timestamp1, base1, resector1, module1;
    int submodule1, crystal1, layer1;
    int CP1, RP1, RC1;

    int eventid2;
    double timestamp2, base2, resector2, module2;
    int submodule2, crystal2, layer2;
    int CP2, RP2, RC2;
} Coincidence;

bool compevent(Event a, Event b){return (a.t<b.t);}
const double time_windows = 4.1; //ns

int main(int argc, char** argv)
{
    if(argc<2) {
        std::cout<<" Usage: input_dat_prefix output_coincidences_filename"<<std::endl ;
        return 1;
    }
    const string inputfilename = argv[1];
    cout << "Input file name is " << inputfilename << endl;
    long start, stop, num;
    FILE *fidin;
    if ((fidin=fopen(inputfilename.c_str(), "rb"))==NULL){
        cout<<"cannot open file:"<<inputfilename.c_str()<<endl;
        return 1;
    }
    start = ftell(fidin);
    fseek(fidin, 0, SEEK_END);
    stop = ftell(fidin);
    num = (stop-start)/sizeof(Event);
    Event *events = (Event*) malloc(sizeof(Event)*num);
    rewind(fidin);
    fread(events, sizeof(Event), num, fidin);
    fclose(fidin);
    cout<<"successfully read "<<num<<" singles from "<< inputfilename.c_str() <<endl;
    std::vector<Event> under_event;
    for (int i=0; i<num; i++){
        events[i].modn = events[i].modn/2;
        under_event.push_back(events[i]);
    }
    sort(under_event.begin(), under_event.end(), compevent);

    string outputfilename = argv[2];
    outputfilename = outputfilename + ".dat";
    cout << "coincidence file name is = " << outputfilename << endl ;
    ofstream write(outputfilename, ofstream::binary | ofstream::app);
    double time = 0.0;
    long count1 = 0;
    long count2 = 0;
    long count = 0;
    std::vector<struct Coincidence> coincidences;
    Coincidence coincidence;



    for(; count1 < num-1 ; count1++) {
        time = under_event[count1].t + time_windows;
        for (count2 = count1 + 1; count2 < num; count2++) {
            if (under_event[count2].t > time)break;
        }
        count2 = count2 - 1;
        if (count1 == count2)continue;
        for (int i = count1; i <= count2; i++) {
            coincidence.eventid1 = under_event[i].eventid;
            coincidence.timestamp1 = under_event[i].t;
            coincidence.base1 = 0;
            coincidence.resector1 = under_event[i].pann;
            coincidence.module1 = under_event[i].modn;
            coincidence.submodule1 = 0;
            coincidence.crystal1 = under_event[i].cryn;
            coincidence.layer1 = under_event[i].siten;
            coincidence.RC1 = under_event[i].RC;
            coincidence.RP1 = under_event[i].RP;
            coincidence.CP1 = under_event[i].CP;

            for (int j = i + 1; j <= count2; j++) {
                if (abs(under_event[i].pann - under_event[j].pann)>2)
                {
                    coincidence.eventid2 = under_event[j].eventid;
                    coincidence.timestamp2 = under_event[j].t;
                    coincidence.base2 = 0;
                    coincidence.resector2 = under_event[j].pann;
                    coincidence.module2 = under_event[j].modn;
                    coincidence.submodule2 = 0;
                    coincidence.crystal2 = under_event[j].cryn;
                    coincidence.layer2 = under_event[j].siten;
                    coincidence.RC2 = under_event[j].RC;
                    coincidence.RP2 = under_event[j].RP;
                    coincidence.CP2 = under_event[j].CP;
                    coincidences.push_back(coincidence);
                    //cout<<coincidence.resector1<<" "<<coincidence.resector2<<endl;
                    count++;
                }
            }
        }
    }
    cout<<"coincidences number is "<<count<<endl;
    printf("\n");
    //Coincidence *tCoincidence = new Coincidence[count];
    //memcpy(tCoincidence, &coincidences[0], sizeof(Coincidence)*count);
    for(int i=0; i<count; i++){
//        cout<<coincidences[i].eventid1<<' '<<coincidences[i].timestamp1<<' '<<coincidences[i].base1<<' '<<coincidences[i].resector1<<' '<<coincidences[i].module1<<' '
//            <<coincidences[i].submodule1<<' '<<coincidences[i].crystal1<<' '<<coincidences[i].layer1<<' '<<coincidences[i].CP1<<' '<<coincidences[i].RP1<<' '
//            <<coincidences[i].RC1<<' '<<coincidences[i].eventid2<<' '<<coincidences[i].timestamp2<<' '<<coincidences[i].base2<<' '<<coincidences[i].resector2<<' '
//            <<coincidences[i].module2<<' '<<coincidences[i].submodule2<<' '<<coincidences[i].crystal2<<' '<<coincidences[i].layer2<<' '<<coincidences[i].CP2<<' '
//            <<coincidences[i].RP2<<' '<<coincidences[i].RC2<<endl;
        write<<coincidences[i].eventid1<<' '<<coincidences[i].timestamp1<<' '<<coincidences[i].base1<<' '<<coincidences[i].resector1<<' '<<coincidences[i].module1<<' '
                <<coincidences[i].submodule1<<' '<<coincidences[i].crystal1<<' '<<coincidences[i].layer1<<' '<<coincidences[i].CP1<<' '<<coincidences[i].RP1<<' '
                <<coincidences[i].RC1<<' '<<coincidences[i].eventid2<<' '<<coincidences[i].timestamp2<<' '<<coincidences[i].base2<<' '<<coincidences[i].resector2<<' '
                <<coincidences[i].module2<<' '<<coincidences[i].submodule2<<' '<<coincidences[i].crystal2<<' '<<coincidences[i].layer2<<' '<<coincidences[i].CP2<<' '
                <<coincidences[i].RP2<<' '<<coincidences[i].RC2<<endl;
    }
    write.close();


    return 0;
}