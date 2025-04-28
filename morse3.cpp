#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <cmath>
#include <cstdint>
#include <exception>
#include <sstream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct WavHeader {
    char riffId[4] = {'R', 'I', 'F', 'F'};
    uint32_t riffSize;
    char waveId[4] = {'W', 'A', 'V', 'E'};

    char fmtId[4] = {'f', 'm', 't', ' '};
    uint32_t fmtSize = 16;
    uint16_t audioFormat = 1;
    uint16_t numChannels = 1;
    uint32_t sampleRate = 44100;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample = 16;

    char dataId[4] = {'d', 'a', 't', 'a'};
    uint32_t dataSize;
};

class MorseException : public std::exception {
    std::string msg;
public:
    explicit MorseException(std::string message) : msg(std::move(message)) {}
    const char* what() const noexcept override { return msg.c_str(); }
};

class MorseBase {
public:
    virtual ~MorseBase() noexcept = default;
    virtual std::string encode(const std::string& text) = 0;
    virtual std::string decode(const std::string& morse) = 0;
};

class MorseConverter : public MorseBase {
private:
    std::map<char, std::string> charToMorse;
    std::map<std::string, char> morseToChar;

    void initMaps() {
        const std::vector<std::pair<char, std::string>> mappings = {
            {'A', ".-"}, {'B', "-..."}, {'C', "-.-."}, {'D', "-.."}, {'E', "."},
            {'F', "..-."}, {'G', "--."}, {'H', "...."}, {'I', ".."}, {'J', ".---"},
            {'K', "-.-"}, {'L', ".-.."}, {'M', "--"}, {'N', "-."}, {'O', "---"},
            {'P', ".--."}, {'Q', "--.-"}, {'R', ".-."}, {'S', "..."}, {'T', "-"},
            {'U', "..-"}, {'V', "...-"}, {'W', ".--"}, {'X', "-..-"}, {'Y', "-.--"},
            {'Z', "--.."}, {'0', "-----"}, {'1', ".----"}, {'2', "..---"}, {'3', "...--"},
            {'4', "....-"}, {'5', "....."}, {'6', "-...."}, {'7', "--..."}, {'8', "---.."},
            {'9', "----."}, {'.', ".-.-.-"}, {',', "--..--"}, {'?', "..--.."}, {' ', "   "}
        };

        for (const auto& [c, s] : mappings) {
            charToMorse[c] = s;
            morseToChar[s] = c;
        }
    }

public:
    MorseConverter() { initMaps(); }
    ~MorseConverter() noexcept override = default;

    std::string encode(const std::string& text) override {
        std::string morse;
        bool prev_was_space = false;
        for (char c : text) {
            c = std::toupper(c);
            if (c == ' ') {
                morse += "   ";
                prev_was_space = true;
            } else {
                if (!morse.empty() && !prev_was_space) morse += ' ';
                if (charToMorse.count(c)) {
                    morse += charToMorse.at(c);
                } else {
                    throw MorseException("Character '" + std::string(1, c) + "' cannot be encoded in Morse.");
                }
                prev_was_space = false;
            }
        }
        return morse;
    }

    std::string decode(const std::string& morse) override {
        std::string text;
        size_t pos = 0, prev = 0;
        std::vector<std::string> words;
        while ((pos = morse.find("   ", prev)) != std::string::npos) {
            words.push_back(morse.substr(prev, pos - prev));
            prev = pos + 3;
        }
        words.push_back(morse.substr(prev));

        for (size_t i = 0; i < words.size(); ++i) {
            std::istringstream iss(words[i]);
            std::string token;
            while (iss >> token) {
                if (morseToChar.count(token))
                    text.push_back(morseToChar.at(token));
            }
            if (i != words.size() - 1)
                text.push_back(' ');
        }
        return text;
    }
};

template<typename SampleType = int8_t>
class WavProcessor {
    static constexpr SampleType MAX_AMP = std::numeric_limits<SampleType>::max();
    static constexpr double DOT_DURATION = 0.1;
    static constexpr double DASH_DURATION = 0.3;
    static constexpr double SYMBOL_SPACE = 0.1;
    static constexpr double WORD_SPACE = 0.7;

public:
    static std::vector<SampleType> generateSamples(const std::string& morse) {
        std::vector<SampleType> samples;
        const double freq = 800.0;
        const int sr = 44100;

        size_t i = 0;
        while (i < morse.size()) {
            char c = morse[i];
            if (c == '.' || c == '-') {
                if (c == '.') {
                    addSine(samples, DOT_DURATION, freq, sr);
                } else {
                    addSine(samples, DASH_DURATION, freq, sr);
                }
                addSilence(samples, SYMBOL_SPACE, sr);
                i++;
            } else if (c == ' ') {
                size_t spaceCount = 0;
                while (i < morse.size() && morse[i] == ' ') {
                    spaceCount++;
                    i++;
                }
                if (spaceCount == 1) {
                    addSilence(samples, SYMBOL_SPACE * 3, sr);
                } else if (spaceCount >= 3) {
                    addSilence(samples, WORD_SPACE, sr);
                }
            } else {
                i++;
            }
        }

        return samples;
    }

    static void saveWav(const std::string& filename, const std::vector<SampleType>& samples) {
        std::ofstream file(filename, std::ios::binary);
        if (!file) throw MorseException("Cannot open " + filename);

        WavHeader header;
        header.dataSize = static_cast<uint32_t>(samples.size() * sizeof(SampleType));
        header.riffSize = header.dataSize + sizeof(WavHeader) - 8;
        header.bitsPerSample = sizeof(SampleType) * 8;
        header.byteRate = header.sampleRate * header.numChannels * sizeof(SampleType);
        header.blockAlign = header.numChannels * sizeof(SampleType);

        file.write(reinterpret_cast<const char*>(&header), sizeof(header));
        file.write(reinterpret_cast<const char*>(samples.data()),
                   static_cast<std::streamsize>(samples.size() * sizeof(SampleType)));
    }

    static std::string loadWav(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) throw MorseException("Cannot open " + filename);

        WavHeader header;
        file.read(reinterpret_cast<char*>(&header), sizeof(header));

        if (header.bitsPerSample != sizeof(SampleType) * 8) {
            throw MorseException("Unsupported sample type in WAV file.");
        }

        std::vector<SampleType> samples(header.dataSize / sizeof(SampleType));
        file.read(reinterpret_cast<char*>(samples.data()),
                  static_cast<std::streamsize>(header.dataSize));

        return decodeSamples(samples, header.sampleRate);
    }

private:
    static void addSine(std::vector<SampleType>& samples, double duration, double freq, int sr) {
        const int n = static_cast<int>(duration * sr);
        const double step = 2 * M_PI * freq / sr;

        for (int i = 0; i < n; ++i) {
            samples.push_back(static_cast<SampleType>(MAX_AMP * std::sin(step * i)));
        }
    }

    static void addSilence(std::vector<SampleType>& samples, double duration, int sr) {
        const int n = static_cast<int>(duration * sr);
        samples.insert(samples.end(), n, static_cast<SampleType>(0));
    }

    static std::string decodeSamples(const std::vector<SampleType>& samples, uint32_t sr) {
        std::string morse;
        const auto threshold = MAX_AMP / 100;
        bool in_tone = false;
        int tone_start = 0;
        int silence_start = -1;
        int debounce_counter = 0;
        const int debounce_threshold = static_cast<int>(sr * 0.001);

        for (size_t i = 0; i < samples.size(); ++i) {
            if (std::abs(samples[i]) > threshold) {
                if (!in_tone) {
                    if (++debounce_counter >= debounce_threshold) {
                        in_tone = true;
                        debounce_counter = 0;

                        if (silence_start != -1) {
                            double silence_duration = (i - silence_start) / static_cast<double>(sr);
                            if (silence_duration >= 0.79) {
                                morse += "   ";
                            } else if (silence_duration >= 0.39) {
                                morse += " ";
                            }
                            silence_start = -1;
                        }
                        tone_start = i;
                    }
                } else {
                    debounce_counter = 0;
                }
            } else {
                if (in_tone) {
                    if (++debounce_counter >= debounce_threshold) {
                        in_tone = false;
                        debounce_counter = 0;
                        double tone_duration = (i - tone_start) / static_cast<double>(sr);
                        morse += (tone_duration < (DOT_DURATION + DASH_DURATION) / 2) ? '.' : '-';
                        silence_start = i;
                    }
                } else {
                    debounce_counter = 0;
                }
            }
        }
        std::cout << "Decoded Morse: " << morse << std::endl;
        return morse;
    }
};

class FileHandler {
public:
    static std::string read(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) throw MorseException("Cannot read " + filename);
        return {std::istreambuf_iterator<char>(file), {}};
    }

    static void write(const std::string& filename, const std::string& content) {
        std::ofstream file(filename);
        if (!file) throw MorseException("Cannot write " + filename);
        file << content;
    }
};

class MorseEncoder : public MorseBase {
    MorseConverter converter;
public:
    std::string encode(const std::string& text) override { return converter.encode(text); }
    std::string decode(const std::string&) override { throw MorseException("Encoder cannot decode"); }

    void encodeFile(const std::string& input, const std::string& output) {
        const auto text = FileHandler::read(input);
        const auto morse = encode(text);
        const auto samples = WavProcessor<>::generateSamples(morse); // par défaut int16_t
        WavProcessor<>::saveWav(output, samples);
    }
};

class MorseDecoder : public MorseBase {
    MorseConverter converter;
public:
    std::string encode(const std::string&) override { throw MorseException("Decoder cannot encode"); }
    std::string decode(const std::string& morse) override { return converter.decode(morse); }

    void decodeFile(const std::string& input, const std::string& output) {
        const auto morse = WavProcessor<>::loadWav(input);
        const auto text = decode(morse);
        FileHandler::write(output, text);
    }
};

int main(int argc, char* argv[]) {
    try {
        if (argc == 4) {
            const std::string mode(argv[1]);
            const std::string input(argv[2]);
            const std::string output(argv[3]);

            if (mode == "--encode") {
                MorseEncoder().encodeFile(input, output);
                std::cout << "Encoded successfully to " << output << std::endl;
            }
            else if (mode == "--decode") {
                MorseDecoder().decodeFile(input, output);
                std::cout << "Decoded successfully to " << output << std::endl;
            }
            else {
                throw MorseException("Invalid mode. Use --encode or --decode");
            }
            return 0;
        }

        std::cout << "Running self-test...\n";
        const std::string test_message = "I HAVE 2 CUPS OF WATER.";
        const std::string test_file = "test.txt";
        const std::string test_wav = "test.wav";
        const std::string test_out = "output.txt";

        FileHandler::write(test_file, test_message);

        MorseEncoder encoder;
        std::string test_morse = encoder.encode(test_message);
        std::cout << "Generated Morse:\n" << test_morse << "\n\n";

        MorseEncoder().encodeFile(test_file, test_wav);
        MorseDecoder().decodeFile(test_wav, test_out);

        const std::string decoded = FileHandler::read(test_out);
        std::cout << "Original message: " << test_message << "\nDecoded message: " << decoded << "\n"
                  << (test_message == decoded ? "SUCCESS" : "FAILURE") << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}