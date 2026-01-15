import matplotlib

matplotlib.use('Agg')

import subprocess
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os
import sys
import concurrent.futures
import time

# --- 1. KONFIGURACJA ŚCIEŻEK ---
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.abspath(os.path.join(SCRIPT_DIR, ".."))
EXE_PATH = os.path.join(PROJECT_ROOT, "cpp_sim", "cmake-build-debug", "sm_sim.exe")
EXE_DIR = os.path.dirname(EXE_PATH)
OUTPUT_DIR = os.path.join(PROJECT_ROOT, "output")
DATA_DIR = os.path.join(PROJECT_ROOT, "data")

STEPS = 2000
AGENTS_COUNT = 500
MAX_WORKERS = os.cpu_count()

# --- 2. SCENARIUSZE ---
SCENARIOS = [
    # EXP 1: WPŁYW SPEKULANTÓW
    # ("EXP1", "1A_Conservative_Dominated", 0.90, 0.05, 0.05, 0.0, "input_stable.csv"),
    # ("EXP1", "1B_Balanced_Market", 0.50, 0.40, 0.10, 0.0, "input_stable.csv"),
    # ("EXP1", "1C_Aggressive_Dominated", 0.10, 0.80, 0.10, 0.0, "input_stable.csv"),

    # EXP 2: TEST ODPORNOŚCI NA KRACH
    # ("EXP2", "2A_Resilient_StrongMaker", 0.70, 0.15, 0.15, 0.4, "input_crisis.csv"),
    # ("EXP2", "2B_Average_Fragility", 0.40, 0.40, 0.20, 0.4, "input_crisis.csv"),
    # ("EXP2", "2C_Critical_Fragility", 0.10, 0.70, 0.20, 0.4, "input_crisis.csv"),

    # EXP 3: EFEKT MOMENTUM
    # ("EXP3", "3A_No_Momentum", 0.60, 0.40, 0.00, 0.0, "input_bubble.csv"),
    # ("EXP3", "3B_Moderate_Momentum", 0.40, 0.30, 0.30, 0.0, "input_bubble.csv"),
    # ("EXP3", "3C_High_Momentum_Frenzy", 0.20, 0.10, 0.70, 0.0, "input_bubble.csv"),

    # EXP 4: GRANICA PŁYNNOŚCI
    # ("EXP4", "4A_Low_Liquidity", 0.10, 0.80, 0.10, 0.0, "input_growth.csv"),
    # ("EXP4", "4B_Liquidity_Death_Zone", 0.02, 0.88, 0.10, 0.0, "input_growth.csv"),

    # EXP 5: WALIDACJA
    ("EXP5", "5A_Standard_Tracking", 0.50, 0.30, 0.20, 0.0, "input_sine.csv"),
    ("EXP5", "5B_Precision_Tracking", 0.80, 0.10, 0.10, 0.0, "input_sine.csv"),
]


# --- 3. FUNKCJE ---

def setup_folders():
    if not os.path.exists(EXE_PATH):
        print(f"[CRITICAL] Brak pliku EXE: {EXE_PATH}")
        sys.exit(1)
    if not os.path.exists(DATA_DIR):
        print(f"[CRITICAL] Brak folderu data. Uruchom get_data.py!")
        sys.exit(1)

    for exp in set(s[0] for s in SCENARIOS):
        path = os.path.join(OUTPUT_DIR, exp)
        if not os.path.exists(path):
            os.makedirs(path)


# Funkcja uruchamiająca C++
def run_cpp_only(args):
    folder, name, cons, agg, mom, crash, inp_file = args
    input_abs_path = os.path.join(DATA_DIR, inp_file)
    csv_rel_arg = f"/{folder}/{name}.csv"

    cmd = [
        EXE_PATH, input_abs_path, str(STEPS), str(AGENTS_COUNT),
        str(cons), str(agg), str(mom), csv_rel_arg, str(crash)
    ]
    try:
        res = subprocess.run(cmd, cwd=EXE_DIR, capture_output=True, text=True)
        if res.returncode != 0:
            return f"ERR C++ ({name}): {res.stderr}"
        return None
    except Exception as e:
        return f"EXC ({name}): {e}"


# Funkcja rysująca
def generate_plot_only(args):
    folder, name, cons, agg, mom, crash, inp_file = args
    csv_abs_path = os.path.join(OUTPUT_DIR, folder, f"{name}.csv")

    if not os.path.exists(csv_abs_path):
        return f"SKIP ({name}): Brak pliku CSV"

    try:
        df = pd.read_csv(csv_abs_path)
        if df.empty: return f"SKIP ({name}): Pusty CSV"
    except:
        return f"ERR ({name}): Uszkodzony CSV"

    df['MidPrice'] = (df['BestBid'] + df['BestAsk']) / 2.0
    df = df[df['MidPrice'] > 0]

    span = 50
    df['Price_EMA'] = df['MidPrice'].ewm(span=span, adjust=False).mean()
    df['Spread_EMA'] = df['Spread'].ewm(span=span, adjust=False).mean()
    df['Slippage_EMA'] = df['Slippage'].ewm(span=span, adjust=False).mean()

    plt.figure(figsize=(10, 13))

    info_text = (
        f"SCENARIO: {name}\n"
        f"Agents: Cons {cons * 100:.0f}% | Agg {agg * 100:.0f}% | Mom {mom * 100:.0f}%\n"
        f"Crash Severity: {crash}\n"
        f"--------------------------------------------------\n"
        f"Avg Spread: {df['Spread'].mean():.4f} | Volatility: {df['Volatility'].mean():.4f}"
    )
    plt.figtext(0.5, 0.96, info_text, fontsize=10, ha='center', va='top',
                bbox=dict(boxstyle='round,pad=0.5', facecolor='#f0f0f0', edgecolor='gray'))
    plt.subplots_adjust(top=0.88)

    # 1. Price
    ax1 = plt.subplot(3, 1, 1)
    relevant_data = df['Price_EMA'].tolist()
    if 'Fundamental' in df.columns:
        plt.plot(df['Fundamental'], 'k--', linewidth=1.5, alpha=0.6, label='Input Data')
        relevant_data.extend(df['Fundamental'].tolist())

    plt.plot(df['MidPrice'], 'blue', linewidth=0.3, alpha=0.1)
    plt.plot(df['Price_EMA'], 'cyan', linewidth=2, label='Simulation')

    y_min, y_max = min(relevant_data), max(relevant_data)
    margin = (y_max - y_min) * 0.15
    if margin < 1.0: margin = 1.0
    plt.ylim(y_min - margin, y_max + margin)
    plt.title("Price Tracking")
    plt.legend()
    plt.grid(True, alpha=0.3)

    # 2. Spread
    ax2 = plt.subplot(3, 1, 2, sharex=ax1)
    plt.plot(df['Spread'], 'red', alpha=0.1, linewidth=0.5)
    plt.plot(df['Spread_EMA'], 'darkred', linewidth=2)

    lim_y = max(df['Spread_EMA'].max(), df['Spread'].quantile(0.95)) * 1.2
    if lim_y < 0.5: lim_y = 0.5
    plt.ylim(0, lim_y)
    plt.title("Spread")
    plt.grid(True, alpha=0.3)

    # 3. Slippage
    ax3 = plt.subplot(3, 1, 3, sharex=ax1)
    plt.plot(df['Slippage'], 'orange', alpha=0.1, linewidth=0.5)
    plt.plot(df['Slippage_EMA'], 'darkorange', linewidth=2)

    lim_s = max(df['Slippage_EMA'].max(), df['Slippage'].quantile(0.95)) * 1.2
    if lim_s < 0.5: lim_s = 0.5
    plt.ylim(0, lim_s)
    plt.title("Slippage")
    plt.grid(True, alpha=0.3)

    plt.tight_layout(rect=[0, 0, 1, 0.88])
    png_path = csv_abs_path.replace(".csv", ".png")
    plt.savefig(png_path, dpi=100)
    plt.close()

    return f"PLOT OK: {name}"


# --- 4. MAIN ---

def main():
    setup_folders()
    start = time.time()

    # ETAP 1: SYMULACJE C++
    print(f"--- ETAP 1: URUCHAMIANIE SYMULACJI C++ ({len(SCENARIOS)} zadań) ---")
    with concurrent.futures.ThreadPoolExecutor(max_workers=MAX_WORKERS) as executor:
        futures = {executor.submit(run_cpp_only, s): s for s in SCENARIOS}

        for i, future in enumerate(concurrent.futures.as_completed(futures)):
            res = future.result()
            scen_name = futures[future][1]
            if res:
                print(f"[C++ FAIL] {res}")
            else:
                print(f"[{i + 1}/{len(SCENARIOS)}] C++ OK: {scen_name}")

    # ETAP 2: GENEROWANIE WYKRESÓW
    print(f"\n--- ETAP 2: GENEROWANIE WYKRESÓW (Sekwencyjnie) ---")
    for i, s in enumerate(SCENARIOS):
        res = generate_plot_only(s)
        print(f"[{i + 1}/{len(SCENARIOS)}] {res}")

    print(f"\n--- ZAKOŃCZONO w {time.time() - start:.2f}s ---")
    print(f"Wyniki znajdują się w folderze: {OUTPUT_DIR}")


if __name__ == "__main__":
    main()