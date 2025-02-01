
#include <cmath>
#include <vector>
#include <iostream>
#include <chrono>
#include <unordered_map>
#include <format>
#include <GLFW/glfw3.h>
#define TOLERANCE 1e-5

enum WaveformType
{
    SINE,
    SQUARE,
    TRIANGLE,
    SAWTOOTH
};
//----------------------------------------------------------------------------------------------------------------------------------------
enum Keys
{
    A = 0,  // C
    W = 1,  // C#
    S = 2,  // D
    E = 3,  // D#
    D = 4,  // E
    F = 5,  // F
    T = 6,  // F#
    G = 7,  // G
    Y = 8,  // G#
    H = 9,  // A
    U = 10, // A#
    J = 11  // B
};

//----------------------------------------------------------------------------------------------------------------------------------------
/*const double A4_FREQUENCY = 440.0;
const int MIDI_A4 = 49;
const int OCTAVE_COUNT = 8;
const int NOTE_COUNT = 12;

double computeFrequency(int noteIndex, int octave)
{
    int midiNumber = MIDI_A4 + (octave - 4) * 12 * noteIndex;
    return A4_FREQUENCY * std::pow(2.0, (midiNumber - MIDI_A4) / 12.0); // 2^((midi_i - midi_c)/12)
}

typedef std::array<std::array<double, NOTE_COUNT>, OCTAVE_COUNT> FrequencyTable;

const FrequencyTable generateFrequencyTable()
{
    FrequencyTable table = {};
    for (int octave = 0; octave < OCTAVE_COUNT; octave++)
    {
        for (int note = 0; note < NOTE_COUNT; note++)
        {
            table[octave][note] = computeFrequency(note, octave + 1);
        }
    }
    return table;
}
*/
const int OCTAVE_COUNT = 8;
const int NOTE_COUNT = 12;

// Define the C-style frequency table
typedef double FrequencyTable[OCTAVE_COUNT][NOTE_COUNT];

constexpr FrequencyTable freqTable = {
    {55.000000, 58.270470, 61.735413, 65.406391, 69.295658, 73.416192, 77.781746, 82.406889, 87.307058, 92.498606, 97.998859, 103.826174},                            // Octave 1
    {110.000000, 116.540940, 123.470825, 130.812783, 138.591315, 146.832384, 155.563492, 164.813778, 174.614116, 184.997211, 195.997718, 207.652349},                 // Octave 2
    {220.000000, 233.081881, 246.941651, 261.625565, 277.182631, 293.664768, 311.126984, 329.627557, 349.228231, 369.994423, 391.995436, 415.304698},                 // Octave 3
    {440.000000, 466.163762, 493.883301, 523.251131, 554.365262, 587.329536, 622.253967, 659.255114, 698.456463, 739.988845, 783.990872, 830.609395},                 // Octave 4
    {880.000000, 932.327523, 987.766603, 1046.502261, 1108.730524, 1174.659072, 1244.507935, 1318.510228, 1396.912926, 1479.977691, 1567.981744, 1661.218790},        // Octave 5
    {1760.000000, 1864.655046, 1975.533205, 2093.004522, 2217.461048, 2349.318143, 2489.015870, 2637.020455, 2793.825851, 2959.955382, 3135.963488, 3322.437581},     // Octave 6
    {3520.000000, 3729.310092, 3951.066410, 4186.009045, 4434.922096, 4698.636287, 4978.031740, 5274.040911, 5587.651703, 5919.910763, 6271.926976, 6644.875161},     // Octave 7
    {7040.000000, 7458.620184, 7902.132820, 8372.018090, 8869.844191, 9397.272573, 9956.063479, 10548.081821, 11175.303406, 11839.821526, 12543.853951, 13289.750322} // Octave 8
};

//----------------------------------------------------------------------------------------------------------------------------------------

const int _sampleRate = 44100;
const double _deltaTime = 1.0 / _sampleRate;
const double _recordingBufferSizeMultiplier = 10;                                                   // seconds
const int _bufferSize = static_cast<int>(std::floor(_sampleRate * _recordingBufferSizeMultiplier)); // total number of possible samples

//----------------------------------------------------------------------------------------------------------------------------------------

void processInput(GLFWwindow *window, Keys &selectedKey, bool &isPressed)
{
    isPressed = false; // Reset before checking for new key press

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        selectedKey = A;
        isPressed = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        selectedKey = W;
        isPressed = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        selectedKey = S;
        isPressed = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) // Fixed D#
    {
        selectedKey = E;
        isPressed = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        selectedKey = D;
        isPressed = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        selectedKey = F;
        isPressed = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        selectedKey = T;
        isPressed = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        selectedKey = G;
        isPressed = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) // Fixed G#
    {
        selectedKey = Y;
        isPressed = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
    {
        selectedKey = H;
        isPressed = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
    {
        selectedKey = U;
        isPressed = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
    {
        selectedKey = J;
        isPressed = true;
    }
}

struct TimerResult
{
    double _allTimes = 0;
    double _sampleCount = 0;
}; // how much time is elapsed during recording

inline std::unordered_map<const char *, TimerResult> _timerResult;

struct Timer
{
    std::chrono::time_point<std::chrono::high_resolution_clock> _startTime;
    const char *_name;

    Timer(const char *name)
    {
        _name = name;
        _startTime = std::chrono::high_resolution_clock::now();
    }
    ~Timer()
    {
        std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - _startTime;
        double time_spent = duration.count() * 1000.0;
        printf("time spent during the process: %f", time_spent);
    }
};

//----------------------------------------------------------------------------------------------------------------------------------------

struct AudioBuffers
{
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
    void readBufferIndex(int i)
    {
        std::cout << _audioBuffer[i] << ", ";
    }
};

//----------------------------------------------------------------------------------------------------------------------------------------

class Oscillator
{
private:
    double _frequency;      // Frequency in Hz. This is modified when we play different keys
    double _maxAmplitude;   // Amplitude (0 to 1)
    double _phase;          // Current phase
                            // Sample rate in Hz
    WaveformType _waveform; // Type of waveform
    double _waveAmplitude;

public:
    Oscillator(double &freq, double amp, double sampleRate, WaveformType type)
        : _frequency(freq), _maxAmplitude(amp), _phase(0.0), _waveform(type) {}

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
        if ((_phase - 1.0) >= TOLERANCE)
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

        double currentFrequency = 0.0;
    Oscillator osc1(currentFrequency, 0.5, 44100, TRIANGLE);
    AudioBuffers *buffer = new AudioBuffers;
    Keys selectedKey;
    int octave = 2;
    bool keyPressed = false;

    // implement this. assign buttons to each note
    int i = 0;
    while (!glfwWindowShouldClose(_window) && i < _bufferSize)
    {
        glfwPollEvents();
        /*switch (notes)
        {
        case A:
        }*/
        processInput(_window, selectedKey, keyPressed);
        if (!keyPressed)
        {
            currentFrequency = 0.0;
            osc1.setFrequency(currentFrequency);
        }
        else
        {
            currentFrequency = freqTable[octave][selectedKey];
            osc1.setFrequency(currentFrequency);
        }

        buffer->_audioBuffer[i] = osc1.generateSample();
        printf("frequency: %f \n", osc1.getFrequency());

        // first handle input. then generate input sample. write it to buffer and print

        // buffer->readBufferIndex(i);

        i++;
    }
    // buffer->readBuffer();

    /*FILE *frequencies = fopen("frequencies.txt", "w");
    // assert(frequencies);
    if (!frequencies)
    {
        printf("failed to open file");
        return -1;
    }

    for (int i = 0; i < _bufferSize; ++i)
    {
        fprintf(frequencies, "%lf\n", buffer->_audioBuffer[i]); // Write each sample in text format
    }
    fclose(frequencies);*/

    delete buffer;
    glfwDestroyWindow(_window);
    glfwTerminate();
    return 0;
}