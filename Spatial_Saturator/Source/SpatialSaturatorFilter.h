/*
  ==============================================================================

    This file contains the basic framework code for the filter class

  ==============================================================================
*/
#ifndef __SpatialSaturatorFilter__SpatialSaturatorFilter__
#define __SpatialSaturatorFilter__SpatialSaturatorFilter__

#pragma once

#include <JuceHeader.h>
#include <math.h>

//==============================================================================
/**
*/
class Filter
{
public:

    void setCutOffFrequency(float cutOffFrequency);
    void setSampleRate(float sample_rate);
    //==============================================================================
    // Parameters
    float m_cutOffFrequency;
    float m_sample_rate;
    // Filter states
    double m_m1{}, m_m2{}, m_s1{}, m_s2{}, m_shp1{}, m_shp2{};
    // Q Parameter
    double Q = 1 / sqrt(2);

private:

};

class MidShelfFilter : public Filter
{
public:
    void processFilter(juce::AudioBuffer<float>&, juce::MidiBuffer&, int numberSamples, std::atomic<float>* cutOffFrequency, std::atomic<float>* gain);


private:

};

class SideShelfFilter : public Filter
{
public:
    void processFilter(juce::AudioBuffer<float>&, juce::MidiBuffer&, int numberSamples, std::atomic<float>* cutOffFrequency, std::atomic<float>* gain);


private:

};

class SideHpFilter : public Filter
{
public:
    void processFilter(juce::AudioBuffer<float>&, juce::MidiBuffer&, int numberSamples, std::atomic<float>* cutOffFrequency);


private:

};

#endif 