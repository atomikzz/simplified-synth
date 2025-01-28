
#include <cmath>
#include <vector>
#include <iostream>
#include <GLFW/glfw3.h>
#

enum WaveformType
{
    SINE,
    SQUARE,
    TRIANGLE,
    SAWTOOTH
};
enum Notes
{
    A,
    As,
    B,
    C,
    Cs,
    D,
    Ds,
    E,
    F,
    Fs,
    G,
    Gs
};
class Oscillator
{
private:
    double _frequency;      // Frequency in Hz
    double _amplitude;      // Amplitude (0 to 1)
    double _phase;          // Current phase
    double _sampleRate;     // Sample rate in Hz
    WaveformType _waveform; // Type of waveform

public:
    Oscillator(double freq, double amp, double sampleRate, WaveformType type)
        : _frequency(freq), _amplitude(amp), _phase(0.0), _sampleRate(sampleRate), _waveform(type) {}

    double generateSample()
    {
        double sample = 0.0;

        // Generate the waveform based on the current type
        switch (_waveform)
        {
        case SINE:
            sample = _amplitude * sin(2.0 * M_PI * _phase);
            break;

        case SQUARE:
            sample = _amplitude * (_phase < 0.5 ? 1.0 : -1.0);
            break;

        case TRIANGLE:
            sample = _amplitude * (2.0 * fabs(2.0 * (_phase - floor(_phase + 0.5))) - 1.0);
            break;

        case SAWTOOTH:
            sample = _amplitude * (2.0 * (_phase - floor(_phase + 0.5)));
            break;
        }

        // Update phase
        _phase += _frequency / _sampleRate;
        if (_phase >= 1.0)
            _phase -= 1.0;

        return sample;
    }

    // Change waveform type
    void setWaveform(WaveformType type)
    {
        _waveform = type;
    }

    // Change frequency
    void setFrequency(double freq)
    {
        _frequency = freq;
    }
    double getFrequency()
    {
        return _frequency;
    }
    void writeBuffer(AudioBuffers &buffer)
    {
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            buffer[i] = (*this).generateSample();
            std::cout << "sample frequency: " << (*this).getFrequency() << "\n";
        }
    }
};

class AudioBuffers
{

private:
    std::vector<double> _audioBuffer;
    int _numSamples;

public:
    AudioBuffers(int numSamples) : _numSamples(numSamples) { _audioBuffer.resize(numSamples); }

    void printWaveform()
    {
        for (double sample : _audioBuffer)
        {
            std::cout << sample << "\n";
        }
    }
    int getNumSamples()
    {
        return _numSamples;
    }
};

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    GLFWwindow *_window = glfwCreateWindow(640, 480, "synth", nullptr, nullptr);
    glfwFocusWindow(_window);

    Oscillator osc1(440.0, 0.5, 25000, TRIANGLE);
    AudioBuffers buffer(10);
    osc1.writeBuffer(buffer);

    while (!glfwWindowShouldClose(_window))
    {
        glfwPollEvents();

        osc1.writeBuffer(buffer);
        buffer.printWaveform();
        if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(_window, GLFW_TRUE);
        }
        if (glfwGetKey(_window, GLFW_KEY_C) == GLFW_PRESS)
        {
            osc1.setFrequency(460.0);
        }
    }
    glfwDestroyWindow(_window);
    glfwTerminate();
}