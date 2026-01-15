# 📈 Stock Market Simulation (Agent-Based Model)

![C++](https://img.shields.io/badge/C++-23-blue.svg) ![Python](https://img.shields.io/badge/Python-3.x-yellow.svg) ![License](https://img.shields.io/badge/License-MIT-green.svg)

## 📖 Overview
This project is a high-performance **hybrid Stock Market Simulator** that models the microstructure of financial markets using the **Mike-Farmer model**.

It combines the raw performance of **C++** (for the Matching Engine and Limit Order Book) with the analytical power of **Python** (for data processing, automation, and visualization).

The goal is to analyze how different types of market participants (Market Makers, HFT Speculators, Momentum Traders) influence liquidity, spread, and price stability during normal conditions and extreme events (e.g., Flash Crashes).

## 🚀 Key Features
*   **Hybrid Architecture:** C++ backend for millisecond-level simulation + Python for orchestration.
*   **Real-World Data:** Ingests historical data (e.g., AAPL, GME, SPY) via Yahoo Finance.
*   **Agent Types:**
    *   🛡️ **Conservative:** Market Makers providing liquidity.
    *   ⚔️ **Aggressive:** HFT/Speculators consuming liquidity.
    *   🚀 **Momentum:** Trend-following agents (simulating FOMO/Bubbles).
*   **Multithreading:** Runs multiple simulation scenarios in parallel using Python's `ThreadPoolExecutor`.
*   **Stress Testing:** Capabilities to inject artificial "Flash Crashes" to test market resilience.

## 📂 Project Structure
```text
.
├── cpp_sim/                # C++ Source Code (Simulation Core)
│   ├── cmake-build-debug/  # Contains the pre-compiled executable (sm_sim.exe)
│   ├── main.cpp
│   └── ...
├── data/                   # Input CSV files (Historical Market Data)
├── output/                 # Simulation results (CSVs and Charts)
├── python_analysis/        # Python scripts for automation and plotting
├── requirements.txt        # Python dependencies
└── README.md
```

## 🛠️ Prerequisites
*   **Python 3.10+**
*   **C++ Compiler** (Optional, only if you want to rebuild the engine. A pre-compiled `.exe` is included for Windows).

### Python Dependencies
Install the required libraries using pip:
```bash
pip install -r requirements.txt
```
*(Required libraries: `pandas`, `matplotlib`, `seaborn`, `numpy`, `yfinance`)*

## ⚡ How to Run

### 1. Prepare Data
First, download real market data (Apple, Tesla, GameStop, etc.) and generate synthetic validation data.
```bash
cd python_analysis
python get_data.py
```
*This will populate the `../data/` directory with necessary CSV files.*

### 2. Run Simulations
Run the main study script. This utilizes **multithreading** to run 5 different experiments (Volatility, Flash Crash, Liquidity Cliff, etc.) simultaneously.
```bash
python run_final_study.py
```

### 3. View Results
Check the `../output/` directory. You will find subfolders (e.g., `EXP1`, `EXP2`) containing:
*   Generated charts (`.png`) showing Price Tracking, Spread, and Slippage.
*   Raw simulation data (`.csv`).

---

## 🔬 Experiment Scenarios
The simulation covers the following research questions:
1.  **EXP1:** Impact of High-Frequency Trading (HFT) on Market Quality.
2.  **EXP2:** Market Resilience test during a Flash Crash (Liquidity Crisis).
3.  **EXP3:** Momentum Agents & Speculative Bubbles (GameStop scenario).
4.  **EXP4:** The Liquidity Cliff – What happens when Market Makers vanish?
5.  **EXP5:** Technical Validation (Sine Wave Tracking).
6.  
---

## ⚠️ Disclaimer
**EDUCATIONAL PURPOSE ONLY.**
This software is a theoretical simulation designed for academic research. It is **NOT** financial advice. The models used are simplifications of complex real-world dynamics. Do not use this tool or its results for real-world trading decisions. The authors are not responsible for any financial losses.

---

## 👥 Authors
*   **Daniel Sterzel**
*   **Szymon Kowalski**
*   **Michał Król**

*AGH University of Krakow / 2026*
