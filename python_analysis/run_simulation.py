import subprocess
import pandas as pd
import matplotlib.pyplot as plt
import os

# --- 1. USTALANIE ŚCIEŻEK (WINDOWS) ---

# Pobieramy folder, w którym jest ten skrypt (czyli .../python_analysis)
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

# Wychodzimy piętro wyżej do głównego folderu projektu
PROJECT_ROOT = os.path.abspath(os.path.join(SCRIPT_DIR, ".."))

# Ścieżka do pliku EXE (zgodnie z Twoim opisem)
# Używamy os.path.join, żeby Windows wstawił poprawne ukośniki (\)
EXE_PATH = os.path.join(PROJECT_ROOT, "cpp_sim", "cmake-build-debug", "sm_sim.exe")

# Folder, w którym leży exe (potrzebny jako punkt startowy dla C++)
EXE_DIR = os.path.dirname(EXE_PATH)

# Folder wyjściowy na wyniki
OUTPUT_DIR = os.path.join(PROJECT_ROOT, "output")
INPUT_FILE_ARG = "input_prices.csv"

# --- 2. PARAMETRY SYMULACJI ---
STEPS = 6000
AGENTS_COUNT = 500

SCENARIOS = [
    ("1_Stable_Market", 0.80, 0.10, 0.10),
    ("2_Flash_Crash_Risk", 0.50, 0.30, 0.20),
    ("3_Momentum_Bubble", 0.20, 0.20, 0.60),
    ("4_Liquidity_Crisis", 0.05, 0.45, 0.50)
]


def run_simulation(name, cons, agg, mom):
    # Sprawdź czy plik exe na pewno istnieje tam gdzie mówisz
    if not os.path.exists(EXE_PATH):
        print(f"BŁĄD: Nie znaleziono pliku: {EXE_PATH}")
        return False

    # Nazwa pliku wyjściowego dla C++ (z ukośnikiem na początku)
    csv_arg = f"/{name}.csv"

    # Komenda do uruchomienia
    cmd = [
        EXE_PATH,  # Pełna ścieżka do sm_sim.exe
        INPUT_FILE_ARG,
        str(STEPS),
        str(AGENTS_COUNT),
        str(cons),
        str(agg),
        str(mom),
        csv_arg
    ]

    print(f"Running C++ Scenario: {name} ...")

    try:
        # cwd=EXE_DIR jest KLUCZOWE. C++ musi wystartować z folderu cmake-build-debug,
        # żeby relatywne ścieżki "../../data" działały poprawnie.
        result = subprocess.run(
            cmd,
            cwd=EXE_DIR,
            capture_output=True,
            text=True
        )

        if result.returncode != 0:
            print(f"C++ Error:\n{result.stderr}")
            print(f"C++ Stdout:\n{result.stdout}")
            return False
        return True
    except Exception as e:
        print(f"Python Execution Error: {e}")
        return False


def analyze_results(name):
    csv_path = os.path.join(OUTPUT_DIR, f"{name}.csv")

    if not os.path.exists(csv_path):
        print(f"   [WARN] Brak pliku wyników: {csv_path}")
        return

    try:
        df = pd.read_csv(csv_path)
    except Exception as e:
        print(f"   [WARN] Błąd odczytu CSV: {e}")
        return

    df['MidPrice'] = (df['BestBid'] + df['BestAsk']) / 2.0

    report = f"""
    SCENARIUSZ: {name}
    ---------------------------
    Slippage (avg): {df['Slippage'].mean():.4f}
    Spread (avg):   {df['Spread'].mean():.4f}
    Spread (max):   {df['Spread'].max():.4f}
    ---------------------------
    """
    print(report)

    with open(os.path.join(OUTPUT_DIR, "summary_report.txt"), "a") as f:
        f.write(report + "\n")

    # Wykres
    plt.style.use('ggplot')
    fig, axes = plt.subplots(3, 1, figsize=(10, 12), sharex=True)

    axes[0].plot(df['Fundamental'], 'k--', label='Fundament')
    axes[0].plot(df['MidPrice'], 'b-', alpha=0.6, label='Cena Rynkowa')
    axes[0].set_title(f"Scenariusz: {name}")
    axes[0].legend()

    axes[1].plot(df['Spread'], color='gray', alpha=0.3)
    axes[1].plot(df['Spread'].rolling(50).mean(), 'r-', label='Śr. Spread')
    axes[1].set_title("Płynność")

    axes[2].plot(df['Slippage'].rolling(50).mean(), color='orange', label='Śr. Slippage')
    axes[2].set_title("Ryzyko")

    plt.tight_layout()
    plt.savefig(os.path.join(OUTPUT_DIR, f"{name}_chart.png"))
    plt.close()


def main():
    # Diagnostyka ścieżek
    print("--- DIAGNOSTYKA ŚCIEŻEK ---")
    print(f"Script Dir:   {SCRIPT_DIR}")
    print(f"Project Root: {PROJECT_ROOT}")
    print(f"EXE Path:     {EXE_PATH}")
    print(f"Output Dir:   {OUTPUT_DIR}")

    if os.path.exists(EXE_PATH):
        print("STATUS: PLIK EXE ZNALEZIONY! :)")
    else:
        print("STATUS: PLIK EXE NIE ISTNIEJE! :( Sprawdź ścieżkę powyżej.")
        return
    print("---------------------------\n")

    if not os.path.exists(OUTPUT_DIR):
        os.makedirs(OUTPUT_DIR)

    # Wyczyść stary raport
    with open(os.path.join(OUTPUT_DIR, "summary_report.txt"), "w") as f:
        f.write("RAPORT ZBIORCZY\n=================\n")

    for name, cons, agg, mom in SCENARIOS:
        if run_simulation(name, cons, agg, mom):
            analyze_results(name)
        print("-" * 30)

    print(f"\nGotowe! Wyniki w: {os.path.abspath(OUTPUT_DIR)}")


if __name__ == "__main__":
    main()