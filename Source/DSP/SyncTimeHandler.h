#pragma once
#include <JuceHeader.h>

class SyncTimeHandler
{
public:

    SyncTimeHandler() {}
    ~SyncTimeHandler() {}

    void setSyncActive(bool inSyncActive)
    {
        syncActive = inSyncActive;
    }

    void setTripletsActive(bool inTriplets)
    {
        triplets = inTriplets;
    }

    int SyncTimeHandler::getSyncActive() 
    { 
        return syncActive; 
    };

    float getSyncTimeInterval(int choice, juce::Optional<double>& bpmValue)
    {
        float syncTimeInterval;

        // TODO: Convertir a enum
        switch
			(choice)
		{
		case 0: /** 1 */
			timeInterval = 0.0f;
			break;
		case 1: /** 1/2 */
			timeInterval = 0.25f;
			break;
		case 2: /** 1/4 */
			timeInterval = 0.5f;
			break;
		case 3: /** 1/8 */
			timeInterval = 0.75f;
			break;
		default: 
			timeInterval = 1.0f;
			break;
		}

        /** Convert BPM to time in milliseconds */
        if (*bpmValue != currentBpm)
        {
            currentBpm = *bpmValue;
            beatInMilliseconds = (60.0 / *bpmValue) * 1000.0;
        }

        if (timeInterval == 0.0f) /** 1 */
            syncTimeInterval = static_cast<float> (beatInMilliseconds) * 4.0f;

        else if (timeInterval == 0.25f) /** 1/2 */
            syncTimeInterval = static_cast<float> (beatInMilliseconds) * 2.0f;

        else if (timeInterval == 0.5f) /** 1/4 */
            syncTimeInterval = static_cast<float> (beatInMilliseconds);

        else if (timeInterval == 0.75f) /** 1/8 */
            syncTimeInterval = static_cast<float> (beatInMilliseconds) / 2.0f;

        else /** 1/16 */
            syncTimeInterval = static_cast<float> (beatInMilliseconds) / 4.0f;

        /** Triplets */
        if (triplets)
        { 
            syncTimeInterval *= (2.0f / 3.0f);
        }
        /** Dotted */
        
       // else if (syncMode == 2)
         //  syncTimeInterval *= (1.5f);

        return syncTimeInterval;
    }

private:
    double currentBpm { 120.0 };
    double beatInMilliseconds { 500.0 };
	float timeInterval { 0.0f }; // Default to 1 (1 beat)

    bool triplets { false };
    bool syncActive { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SyncTimeHandler)
};
