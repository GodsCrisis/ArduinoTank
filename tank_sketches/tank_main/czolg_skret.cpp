#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <math.h>


using namespace std;

float mapValue(int wartosc, int pierwsza_in, int druga_in, int pierwsza_out, int druga_out) {
    return (wartosc - pierwsza_in) * (float)(druga_out - pierwsza_out) / (druga_in - pierwsza_in) + pierwsza_out;
}

int main(){
    int slide = 0;
    int rot = 0;
    cin>>slide;
    cin>>rot;
    float mapped_slider = mapValue(slide, 0, 1023, -1, 1);
    float mapped_rot = mapValue(rot, 0, 1023, -1, 1);
    cout<<"mapped slider "<<mapped_slider<<endl;
    cout<<"mapped rot "<<mapped_rot<<endl;
    float kl = 255*(mapped_slider*(1-fabsf(mapped_rot))+mapped_slider*mapped_rot);
    float kr = 255*(mapped_slider*(1-fabsf(mapped_rot))-mapped_slider*mapped_rot);
    cout<<"lewy skret "<<kl<<endl;
    cout<<"prawy skret "<<kr<<endl;
    return 0;
}