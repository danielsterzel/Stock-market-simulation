import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os

# Ustawienia
OUTPUT_DIR = "../output"
EXP_FOLDER = "EXP2"  # Analizujemy eksperyment z krachem


def generate_summary_chart():
    # 1. Szukamy plików CSV w folderze EXP2
    exp_path = os.path.join(OUTPUT_DIR, EXP_FOLDER)
    if not os.path.exists(exp_path):
        print("Brak folderu EXP2!")
        return

    data = []

    # Przeglądamy pliki
    for f in os.listdir(exp_path):
        if f.endswith(".csv"):
            path = os.path.join(exp_path, f)
            try:
                df = pd.read_csv(path)
                # Wyciągamy średnie z całego przebiegu
                name = f.replace(".csv", "")

                # Skracamy nazwy do wykresu
                short_name = name.split("_")[1]  # np. "Resilient" z "2A_Resilient_..."

                stats = {
                    "Scenario": short_name,
                    "Avg Spread": df['Spread'].mean(),
                    "Avg Slippage": df['Slippage'].mean(),
                    "Volatility": df['Volatility'].mean()
                }
                data.append(stats)
            except:
                pass

    if not data: return

    df_res = pd.DataFrame(data)
    # Sortujemy żeby było po kolei (A, B, C)
    df_res.sort_values("Scenario", inplace=True, ascending=False)

    # 2. RYSOWANIE
    plt.figure(figsize=(14, 6))

    # Wykres Spreadu
    plt.subplot(1, 2, 1)
    sns.barplot(data=df_res, x='Scenario', y='Avg Spread', palette='Reds_d')
    plt.title("Średni Spread podczas Krachu (Im mniej tym taniej)")
    plt.ylabel("Wartość Spreadu")

    # Wykres Zmienności (Volatility) lub Slippage
    plt.subplot(1, 2, 2)
    sns.barplot(data=df_res, x='Scenario', y='Avg Slippage', palette='Oranges_d')
    plt.title("Średni Slippage (Ryzyko realizacji)")
    plt.ylabel("Wartość Slippage")

    plt.tight_layout()
    save_path = os.path.join(OUTPUT_DIR, "SUMMARY_EXP2_CRASH.png")
    plt.savefig(save_path, dpi=150)
    print(f"Zapisano wykres podsumowujący: {save_path}")
    plt.show()


if __name__ == "__main__":
    generate_summary_chart()