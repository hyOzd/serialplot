#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include "source.h"
#include "sink.h"

class TestSink : public Sink
{
public:
    int totalFed;
    int _numChannels;
    bool _hasX;

    TestSink()
        {
            totalFed = 0;
            _numChannels = 0;
            _hasX = false;
        };

    void feedIn(const SamplePack& data)
        {
            REQUIRE(data.numChannels() == numChannels());

            totalFed += data.numSamples();

            Sink::feedIn(data);
        };

    void setNumChannels(unsigned nc, bool x)
        {
            _numChannels = nc;
            _hasX = x;

            Sink::setNumChannels(nc, x);
        };

    virtual unsigned numChannels() const
        {
            return _numChannels;
        };

    virtual bool hasX() const
        {
            return _hasX;
        };
};

class TestSource : public Source
{
public:
    int _numChannels;
    bool _hasX;

    TestSource(unsigned nc, bool x)
        {
            _numChannels = nc;
            _hasX = x;
        };

    virtual unsigned numChannels() const
        {
            return _numChannels;
        };

    virtual bool hasX() const
        {
            return _hasX;
        };

    void _feed(const SamplePack& data) const
        {
            feedOut(data);
        };

    void _setNumChannels(unsigned nc, bool x)
        {
            _numChannels = nc;
            _hasX = x;

            updateNumChannels();
        };
};


#endif // TEST_HELPERS_H
