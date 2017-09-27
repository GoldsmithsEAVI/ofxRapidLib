//
//  fastDTW.cpp
//  RapidLib
//
//  Created by mzed on 07/09/2017.
//  Copyright © 2017 Goldsmiths. All rights reserved.
//

#include "fastDTW.h"
#include "dtw.h"

template<typename T>
fastDTW<T>::fastDTW() {};

template<typename T>
fastDTW<T>::~fastDTW() {};

template<typename T>
warpInfo fastDTW<T>::fullFastDTW(const std::vector<std::vector<T>> &seriesX, const std::vector<std::vector<T > > &seriesY, int searchRadius){
    
#ifndef EMSCRIPTEN
    if (seriesY.size() > seriesX.size()) {
        return fullFastDTW(seriesY, seriesX, searchRadius); //TODO: I'm not sure why I need this. Also, not sure why it fails with Emscripten.
    }
#endif
    
    dtw<T> dtw;
    searchRadius = (searchRadius < 0) ? 0 : searchRadius;
    int minSeries = searchRadius + 2;
    if (seriesX.size() <= minSeries || seriesY.size() <= minSeries) {
        return dtw.dynamicTimeWarp(seriesX, seriesY);
    }
    
    T resolution = 2.0;//TODO: Just hardcode this?
    std::vector<std::vector<T>> shrunkenX = downsample(seriesX, resolution);
    std::vector<std::vector<T>> shrunkenY = downsample(seriesY, resolution);
    
    //some nice recursion here
    searchWindow<T> window(seriesX, seriesY, shrunkenX, shrunkenY, getWarpPath(shrunkenX, shrunkenY, searchRadius), searchRadius);
    
    return dtw.constrainedDTW(seriesX, seriesY, window);
};

template<typename T>
T fastDTW<T>::getCost(const std::vector<std::vector<T>> &seriesX, const std::vector<std::vector<T > > &seriesY, int searchRadius){
    warpInfo info = fullFastDTW(seriesX, seriesY, searchRadius);
    return info.cost;
};

template<typename T>
warpPath fastDTW<T>::getWarpPath(const std::vector<std::vector<T>> &seriesX, const std::vector<std::vector<T > > &seriesY, int searchRadius){
    warpInfo info = fullFastDTW(seriesX, seriesY, searchRadius);
    return info.path;
};

template<typename T>
std::vector<std::vector<T> > fastDTW<T>::downsample(const std::vector<std::vector<T>> &series, T resolution) {
    std::vector<std::vector<T> > shrunkenSeries;
    for (int i = 0; i < series.size(); ++i) {
        if (i % 2 == 0) {
            shrunkenSeries.push_back(series[i]);
        } else {
            int shrunkIndex = int(i/2);
            for (int j = 0; j < series[i].size(); ++j) {
                shrunkenSeries[shrunkIndex][j] = (shrunkenSeries[shrunkIndex][j] + series[i][j]) / 2;
            }
        }
    }
    //TODO: implement downsampling by resolution
    return shrunkenSeries;
}

//explicit instantiation
template class fastDTW<double>;
template class fastDTW<float>;