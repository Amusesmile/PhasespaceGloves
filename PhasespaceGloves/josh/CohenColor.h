//
//  CohenColor.h
//  PhasespaceGloves
//
//  Created by Joshua Dickinson on 1/27/13.
//  Copyright (c) 2013 Joshua Dickinson. All rights reserved.
//

#ifndef PhasespaceGloves_CohenColor_h
#define PhasespaceGloves_CohenColor_h

/*
 Color relation algorithm (from what I remember):
 1. Hue (least important part of algorithm).  Random starting point.
 2. Assign changes spatially (fibbionacci) then divide the whole range of lightness into three parts and pick a random number in each and then use the same number for lightness and saturation based on a 3x3 matrix.
 3. There are only 9 values for lightness and saturation.
 
 Then 70% HM 20% ML 10% HH.  <-- these are the input parameters.
 */

/*
 Cohen harold = new Cohen(2, 5, 2, 0, 20);//number of darks, number of mediums, number of lights, beginning of rainbow, end of rainbow (0 to 100)
 */

class CohenColor
{
public:
    CohenColor(int lows = 2, int middles = 3, int highs = 3, float rainbowStart = 0.0, float rainbowEnd = 0.2)
    {
        setSV();
        set(lows, middles, highs, rainbowStart, rainbowEnd);
    }
    
    void setSV()
    {
        SV_vector.clear();
        for(int i = 0;i<3;i++)
        {
            for(int j = 0;j<3;j++)
            {
                SV_vector.push_back(rnd::uniform(0.333333*i, 0.33333*(i+1)));
            }
        }
    }
    
    void set(int lows, int middles, int highs, float rainbowStart, float rainbowEnd)
    {
        HSV_vector.clear();
        
        for (int i = 0;i<lows;i++)
        {
            float tempSaturationValue = SV_vector.at(rand()%3);
            float tempHue = rnd::uniform(rainbowStart, rainbowEnd);
            HSV_vector.push_back(HSV(tempHue, tempSaturationValue, tempSaturationValue));
        }
        for (int i = 0;i<middles;i++)
        {
            float tempSaturationValue = SV_vector.at(3+(rand()%3));
            float tempHue = rnd::uniform(rainbowStart, rainbowEnd);
            HSV_vector.push_back(HSV(tempHue, tempSaturationValue, tempSaturationValue));
        }
        for (int i = 0;i<highs;i++)
        {
            float tempSaturationValue = SV_vector.at(6+(rand()%3));
            float tempHue = rnd::uniform(rainbowStart, rainbowEnd);
            HSV_vector.push_back(HSV(tempHue, tempSaturationValue, tempSaturationValue));
        }
    }
    
    HSV operator()(int whichHSV)
    {
        return HSV_vector.at(whichHSV%HSV_vector.size());
    }
    
    HSV operator()()
    {
        return operator()(rand());
    }
    
private:
    std::vector<HSV> HSV_vector;
    std::vector<float> SV_vector;
};

#endif
