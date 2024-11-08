# Branch Predictor Simulator

## Description
This project implements a configurable branch predictor simulator that supports different types of branch prediction schemes. The simulator reads branch traces and evaluates the prediction accuracy of different branch predictor configurations.

## Features
- Multiple branch predictor configurations:
  - Bimodal Predictor
  - Gshare Predictor
  - Hybrid Predictor (combination of Bimodal and Gshare)
- Configurable parameters for each predictor type
- Detailed statistics and performance metrics
- Trace-driven simulation

## Implementation Details

### Bimodal Predictor
- Uses a table of 2-bit saturating counters
- Indexed by PC bits
- Counter values:
  - 0-1: Predict not taken
  - 2-3: Predict taken
- All counters initialized to 2 (weakly taken)

### Gshare Predictor
- Combines PC bits with global branch history
- Uses XOR for index generation
- Global history register initialized to 0
- Same 2-bit counter scheme as bimodal

### Hybrid Predictor
- Uses chooser table to select between bimodal and gshare
- Chooser table uses 2-bit counters initialized to 1
- Updates based on relative performance of predictors

## Build Instructions
```bash
# Clone the repository
git clone https://github.com/dev-the-desai/Branch-Predictor-Simulator
cd Branch-Predictor-Simulator

# Build the project
make
```

## Usage

### Command Line Arguments

1. **Bimodal Predictor:**
```bash
./sim bimodal <M2> <tracefile>
```
- M2: Number of PC bits used to index the bimodal table

2. **Gshare Predictor:**
```bash
./sim gshare <M1> <N> <tracefile>
```
- M1: Number of PC bits used to index the gshare table
- N: Number of global branch history register bits

3. **Hybrid Predictor:**
```bash
./sim hybrid <K> <M1> <N> <M2> <tracefile>
```
- K: Number of PC bits used to index the chooser table
- M1: Number of PC bits used to index the gshare table
- N: Number of global branch history register bits
- M2: Number of PC bits used to index the bimodal table

### Input Format
The simulator accepts trace files in the following format:
```
<hex branch PC> t|n
<hex branch PC> t|n
...
```
Where:
- `<hex branch PC>`: Branch instruction address in hexadecimal
- `t`: Branch was taken
- `n`: Branch was not taken

### Output Format
The simulator provides:
1. Configuration summary
2. Performance statistics:
   - Total number of predictions
   - Number of mispredictions
   - Misprediction rate
3. Final state of the branch predictor
