# Morse Code Audio Encoder/Decoder

A comprehensive C++ application that converts text files to Morse code audio (WAV format) and decodes Morse code audio back to text files. This project implements International Morse Code standards with configurable audio sample types and robust error handling.

## Features

- **Text to Morse Audio Encoding**: Convert text files to WAV audio files containing Morse code signals
- **Audio to Text Decoding**: Decode Morse code WAV files back to readable text
- **Multiple Sample Types**: Support for 8-bit and 16-bit PCM audio formats via template configuration
- **International Morse Code Standard**: Implements standard timing and character mapping
- **Robust Error Handling**: Custom exception handling with descriptive error messages
- **Self-Testing Capability**: Built-in test mode for verification of encoding/decoding accuracy
- **File I/O Management**: Comprehensive file handling for text and audio files

## Table of Contents

- [Technical Specifications](#technical-specifications)
- [Requirements](#requirements)
- [Installation & Compilation](#installation--compilation)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Audio Specifications](#audio-specifications)
- [Morse Code Implementation](#morse-code-implementation)
- [Examples](#examples)
- [Limitations](#limitations)
- [License](#license)

## Technical Specifications

### Audio Parameters
- **Sample Rate**: 44.1 kHz (CD quality)
- **Frequency**: 800 Hz sine wave
- **Bit Depth**: Configurable (8-bit/16-bit PCM)
- **Format**: Uncompressed WAV files
- **Channels**: Mono

### Morse Code Timing (International Standard)
- **Dot Duration**: 0.1 seconds
- **Dash Duration**: 0.3 seconds (3x dot duration)
- **Symbol Space**: 0.1 seconds (between dots/dashes)
- **Character Space**: 0.3 seconds (between letters)
- **Word Space**: 0.7 seconds (between words)

## Requirements

- **C++ Compiler**: GCC 4.8+ or Clang 3.4+ with C++11 support
- **Operating System**: Cross-platform (Linux, macOS, Windows)
- **Dependencies**: Standard C++ library only (no external dependencies)

## Installation & Compilation

### Quick Start

## Usage

### Command Line Interface

```bash
# Encode text file to Morse audio
./morse3 --encode input.txt output.wav

# Decode Morse audio to text file
./morse3 --decode input.wav output.txt

# Run self-test (no arguments)
./morse3
```

### Self-Test Mode
When run without arguments, the program performs a comprehensive self-test:
1. Encodes a test message to Morse audio
2. Decodes the audio back to text
3. Verifies the round-trip accuracy
4. Reports SUCCESS or FAILURE

## Project Structure

```
project_morse_final/
├── morse3.cpp          # Main source file
├── morse3              # Compiled executable
├── README.md           # Project documentation
├── test.txt           # Sample input text file
├── test1.txt          # Additional test file
├── test.wav           # Sample output audio file
├── test1.wav          # 16-bit audio sample
├── test2.wav          # 8-bit audio sample
├── output.txt         # Sample decoded output
└── .vscode/           # VS Code configuration
    └── tasks.json     # Build tasks configuration
```

## Audio Specifications

### WAV File Format
- **Header**: Standard WAV header with proper metadata
- **Encoding**: Linear Pulse Code Modulation (LPCM)
- **Endianness**: Little-endian (standard WAV format)
- **Compression**: None (uncompressed PCM)

### Sample Type Configuration
The application uses C++ templates to support different sample types:

```cpp
// Default: 16-bit samples (int16_t)
template<typename SampleType = int16_t>
class WavProcessor { ... }

// For 8-bit samples, modify and recompile:
template<typename SampleType = int8_t>
class WavProcessor { ... }
```

**Note**: To decode audio files encoded with specific sample types, the `SampleType` template parameter must match the encoding format before compilation.

## Morse Code Implementation

### Character Support
The application supports the complete International Morse Code character set:
- **Letters**: A-Z (case insensitive)
- **Numbers**: 0-9
- **Punctuation**: Period, comma, question mark, apostrophe, exclamation, slash, parentheses, ampersand, colon, semicolon, equals, plus, minus, underscore, quotation marks, dollar sign, at symbol

### Character Mapping Examples
```
A: .-     B: -...   C: -.-.   D: -..    E: .
F: ..-.   G: --.    H: ....   I: ..     J: .---
K: -.-    L: .-..   M: --     N: -.     O: ---
P: .--.   Q: --.-   R: .-.    S: ...    T: -
U: ..-    V: ...-   W: .--    X: -..-   Y: -.--
Z: --..

0: -----  1: .----  2: ..---  3: ...--  4: ....-
5: .....  6: -....  7: --...  8: ---..  9: ----.
```

### Special Handling
- **Case Insensitive**: Lowercase and uppercase letters are treated identically
- **Multiple Spaces**: Consecutive spaces in input text are normalized to single spaces
- **Unknown Characters**: Characters not in the Morse code table will generate an error

## Examples

### Basic Usage Example
```bash
# Create a text file
echo "HELLO WORLD" > message.txt

# Encode to Morse audio
./morse3 --encode message.txt message.wav

# Decode back to text
./morse3 --decode message.wav decoded.txt

# Verify the result
cat decoded.txt
# Output: HELLO WORLD
```

### Self-Test Output Example
```
Running self-test...
Generated Morse:
.... . .-.. .-.. ---   .-- --- .-. .-.. -..

Original message: I HAVE 2 CUPS OF WATER.
Decoded message: I HAVE 2 CUPS OF WATER.
SUCCESS
```

## Limitations

1. **Sample Type Configuration**: Requires recompilation to change between 8-bit and 16-bit audio formats
2. **Character Set**: Limited to International Morse Code character set (no Unicode support)
3. **Audio Format**: Only supports uncompressed WAV files (no MP3, OGG, etc.)
4. **Mono Audio**: Single channel audio only
5. **Fixed Parameters**: Audio frequency (800Hz) and timing parameters are hard-coded


## Architecture

The project follows object-oriented design principles with clear separation of concerns:

- **MorseConverter**: Handles text ↔ Morse code conversion
- **WavProcessor**: Manages audio generation and parsing (templated for different sample types)
- **FileHandler**: Manages file I/O operations
- **MorseEncoder/MorseDecoder**: High-level interfaces implementing the Strategy pattern
- **Custom Exception Handling**: MorseException for comprehensive error reporting

## Performance Considerations

- **Memory Efficiency**: Audio samples are processed in memory before writing to disk
- **Template Optimization**: Compile-time template specialization for different sample types
- **File I/O**: Buffered file operations for improved performance
- **Audio Processing**: Efficient sine wave generation using mathematical algorithms
