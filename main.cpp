
#include <cmath>
#include <vector>
#include <iostream>
#include <GLFW/glfw3.h>
#define TOLERANCE 1e-5

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
const int _sampleRate = 44100;
const double _deltaTime = 1.0 / _sampleRate;
const double _recordingBufferSizeMultiplier = 1200;                                                 // seconds
const int _bufferSize = static_cast<int>(std::floor(_sampleRate * _recordingBufferSizeMultiplier)); // total number of possible samples
double _recordingTime;                                                                              // how much time is elapsed during recording

class AudioBuffers
{

public:
    std::vector<double> _audioBuffer;

    AudioBuffers() : _audioBuffer(_bufferSize) {};

    std::vector<double> getAudioBuffer()
    {
        return _audioBuffer;
    }
    void writeBuffer(double sample)
    {
        for (int i = 0; i < _audioBuffer.size(); ++i)
        {
            _audioBuffer[i] = sample;
        }
    }
    void readBuffer()
    {
        for (int i = 0; i < _audioBuffer.size(); ++i)
        {
            std::cout << _audioBuffer[i] << ", ";
        }
    }
};
class Oscillator
{
private:
    double _frequency; // Frequency in Hz
    double _initialFrequency;
    double _maxAmplitude;   // Amplitude (0 to 1)
    double _phase;          // Current phase
                            // Sample rate in Hz
    WaveformType _waveform; // Type of waveform
    double _waveAmplitude;

public:
    Oscillator(double freq, double amp, double sampleRate, WaveformType type)
        : _frequency(freq), _maxAmplitude(amp), _phase(0.0), _waveform(type) { _initialFrequency = _frequency; }

    double generateSample()
    {
        double waveAmplitude;
        // Generate the waveform based on the current type
        switch (_waveform)
        {
        case SINE:
            _waveAmplitude = _maxAmplitude * sin(2.0 * M_PI * _phase);
            break;

        case SQUARE:
            _waveAmplitude = _maxAmplitude * (_phase < 0.5 ? 1.0 : -1.0);
            break;

        case TRIANGLE:
            _waveAmplitude = _maxAmplitude * (2.0 * fabs(2.0 * (_phase - floor(_phase + 0.5))) - 1.0);
            break;

        case SAWTOOTH:
            _waveAmplitude = _maxAmplitude * (2.0 * (_phase - floor(_phase + 0.5)));
            break;
        }

        // Update phase
        _phase += _frequency / _sampleRate;
        if ((_phase - 1.0) < TOLERANCE)
            _phase -= 1.0;
        waveAmplitude = _waveAmplitude;
        return waveAmplitude;
    }
    double getSampleRate()
    {
        return _sampleRate;
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
    double getWaveAmplitude()
    {
        return _waveAmplitude;
    }
    void setToInitialFrequency()
    {
        _initialFrequency = _frequency;
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

    Oscillator osc1(440.0, 0.5, 44100, TRIANGLE);
    AudioBuffers *buffer = new AudioBuffers;

    int i = 0;
    while (!glfwWindowShouldClose(_window) && i < _bufferSize)
    {
        glfwPollEvents();

        if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(_window, GLFW_TRUE);
        }
        if (glfwGetKey(_window, GLFW_KEY_C) == GLFW_PRESS)
        {
            osc1.setFrequency(460.0);
        }
        // first handle input. then generate input sample. write it to buffer and print

        buffer->_audioBuffer[i] = osc1.generateSample();
        osc1.setToInitialFrequency();

        i++;
    }
    buffer->readBuffer();

    delete buffer;
    /*glfwDestroyWindow(_window);
    glfwTerminate();*/
}